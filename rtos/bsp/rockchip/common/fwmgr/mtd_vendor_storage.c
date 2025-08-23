/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_VENDOR_STORAGE

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <drivers/mtd_nor.h>

#include "drv_flash_partition.h"
#include "dma.h"

#include "rk_vendor_storage.h"

#if defined(RT_USING_SPINAND) || defined(RT_USING_SNOR)

#ifdef RT_USING_MINI_FTL
#include "mini_ftl.h"
#endif

#define MTD_VENDOR_NOR_OFFSET   (40 * 512)
#define MTD_VENDOR_NOR_SIZE     (16 * 512)
#define MTD_VENDOR_TAG          VENDOR_HEAD_TAG

struct mtd_nand_info
{
    u32 blk_offset;
    u32 page_offset;
    u32 version;
    u32 ops_size;
};

struct mtd_info
{
    u32 size;
    u32 offset;
    u32 erasesize;
    u32 writesize;
};

struct erase_info
{
    u32 addr;
    u32 len;
    u32 fail_addr;
};

static struct flash_vendor_info *g_vendor;
static struct mtd_info *mtd;
static u32 mtd_erase_size;
static const char *vendor_mtd_name = "vnvm";
static struct mtd_nand_info nand_info;
#ifdef RT_USING_SNOR
static struct rt_mtd_nor_device *snor_dev;
#elif defined(RT_USING_SPINAND)
static struct rt_mtd_nand_device *snand_dev;
#else
#error "Must define RT_USING_SNOR or RT_USING_SPINAND"
#endif

static int mtd_erase(struct mtd_info *mtd, struct erase_info *ei)
{
#ifdef RT_USING_SNOR
    ei->addr += mtd->offset;
    //RK_VENDOR_INF("%s: addr 0x%x, erase size 0x%x\n", __func__, ei->addr, ei->len);
    if (rt_mtd_nor_erase_block(snor_dev, ei->addr, ei->len) != RT_EOK)
    {
        RK_VENDOR_INF("%s: erase failed\n", __func__);
        return -1;
    }
#elif defined(RT_USING_SPINAND)
    //u32 block;

    ei->addr += mtd->offset;
    if ((ei->addr % mtd_erase_size) != 0)
    {
        RK_VENDOR_INF("%s: addr 0x%x not erase size 0x%x align\n", __func__, ei->addr, mtd_erase_size);
        return -1;
    }

    if (ei->len != mtd_erase_size)
    {
        RK_VENDOR_INF("%s: len 0x%x != erase size 0x%x\n", __func__, ei->len, mtd_erase_size);
        return -1;
    }

    //block = ei->addr / mtd_erase_size;
    //RK_VENDOR_INF("%s: addr 0x%x, erase size 0x%x, block 0x%x\n", __func__, ei->addr, mtd_erase_size, block);
    //if (rt_mtd_nand_erase_block(snand_dev, block) != RT_EOK)
    if (mini_ftl_erase(snand_dev, ei->addr, ei->len) != RT_EOK)
    {
        RK_VENDOR_INF("%s: erase failed\n", __func__);
        return -1;
    }
#endif
    return 0;
}

static int mtd_block_isbad(struct mtd_info *mtd, u32 offset)
{
#ifdef RT_USING_SPINAND
    u32 block;

    offset += mtd->offset;
    block = offset / mtd->erasesize;

    //RK_VENDOR_INF("%s: offset 0x%x, erase size 0x%x, block idx 0x%x\n", __func__, offset, mtd->erasesize, block);
    if (rt_mtd_nand_check_block(snand_dev, block) != RT_EOK)
    {
        RK_VENDOR_INF("%s: bad block 0x%x\n", __func__, block);
        return -1;
    }
#endif
    return 0;
}

static int mtd_write(struct mtd_info *mtd, u32 offset, u32 size, u32 *writed, u8 *buf)
{
#ifdef RT_USING_SNOR
    offset += mtd->offset;
    //RK_VENDOR_INF("%s: offset 0x%x size 0x%x\n", __func__, offset, size);
    if (rt_mtd_nor_write(snor_dev, offset, buf, size) != size)
    {
        RK_VENDOR_INF("%s: rt_mtd_nor_write failed\n", __func__);
        return -1;
    }
#elif defined(RT_USING_SPINAND)
    u32 page;
    //u32 blk_idx = offset / mtd->erasesize;
    //u32 page_idx = (offset % mtd->erasesize) / mtd->writesize;
    int i;

    offset += mtd->offset;
    page = offset / snand_dev->page_size;
    if ((offset % snand_dev->page_size) != 0)
    {
        RK_VENDOR_INF("%s: offset 0x%x not page size 0x%x align\n", __func__, offset, snand_dev->page_size);
        return -1;
    }

    if ((size % snand_dev->page_size) != 0)
    {
        RK_VENDOR_INF("%s: size 0x%x not page size 0x%x align\n", __func__, size, snand_dev->page_size);
        return -1;
    }

    //RK_VENDOR_INF("%s: offset 0x%x [%d:%d] page idx %d size 0x%x page_size 0x%x\n", __func__, offset, blk_idx, page_idx, page, size, snand_dev->page_size);
    for (i = 0; i < size / snand_dev->page_size; i++)
    {
        if (rt_mtd_nand_write(snand_dev, page + i, buf + i * snand_dev->page_size, snand_dev->page_size, RT_NULL, 0) != RT_EOK)
        {
            RK_VENDOR_INF("%s: rt_mtd_nand_write failed\n", __func__);
            return -1;
        }
    }
#endif
    *writed = size;

    return 0;
}

static int mtd_read(struct mtd_info *mtd, u32 offset, u32 size, u32 *readed, u8 *buf)
{
#ifdef RT_USING_SNOR
    offset += mtd->offset;
    //RK_VENDOR_INF("%s: offset 0x%x size 0x%x\n", __func__, offset, size);
    if (rt_mtd_nor_read(snor_dev, offset, buf, size) != size)
    {
        RK_VENDOR_INF("%s: rt_mtd_nor_read failed\n", __func__);
        return -1;
    }
#elif defined(RT_USING_SPINAND)
    u32 page;
    //u32 blk_idx = offset / mtd->erasesize;
    //u32 page_idx = (offset % mtd->erasesize) / mtd->writesize;
    int i;

    offset += mtd->offset;
    page = offset / snand_dev->page_size;
    if ((offset % snand_dev->page_size) != 0)
    {
        RK_VENDOR_INF("%s: offset 0x%x not page size 0x%x align\n", __func__, offset, snand_dev->page_size);
        return -1;
    }

    if ((size % snand_dev->page_size) != 0)
    {
        RK_VENDOR_INF("%s: size 0x%x not page size 0x%x align\n", __func__, size, snand_dev->page_size);
        return -1;
    }

    //RK_VENDOR_INF("%s: offset 0x%x [%d:%d] page idx %d size 0x%x page_size 0x%x\n", __func__, offset, blk_idx, page_idx, page, size, snand_dev->page_size);
    for (i = 0; i < size / snand_dev->page_size; i++)
    {
        if (rt_mtd_nand_read(snand_dev, page + i, buf + i * snand_dev->page_size, snand_dev->page_size, RT_NULL, 0) != RT_EOK)
        {
            RK_VENDOR_INF("%s: rt_mtd_nand_read failed\n", __func__);
            return -1;
        }
    }
#endif
    *readed = size;

    return 0;
}

static struct mtd_info *get_mtd_device_nm(const char *name)
{
#ifdef RT_USING_SNOR
    int page_size;

    mtd = RT_NULL;

    snor_dev = (struct rt_mtd_nor_device *)rt_device_find("snor");
    if (snor_dev == RT_NULL)
    {
        rt_kprintf("%s: rt_device_find snor failed\n", __func__);
        return RT_NULL;
    }
    page_size = snor_dev->block_size;

    mtd = (struct mtd_info *)rt_malloc(sizeof(struct mtd_info));
    if (mtd == RT_NULL)
    {
        return RT_NULL;
    }

    mtd->size = MTD_VENDOR_NOR_SIZE;
    mtd->offset = MTD_VENDOR_NOR_OFFSET;
    mtd->erasesize = page_size;
    mtd->writesize = page_size;

    RK_VENDOR_INF("mtd: size 0x%x offset 0x%x erasesize 0x%x writesize 0x%x\n", mtd->size, mtd->offset, mtd->erasesize, mtd->writesize);
    return mtd;
#else
    struct rt_flash_partition *part_info = RT_NULL;
    int i;
    int part_num;

    mtd = RT_NULL;

    snand_dev = (struct rt_mtd_nand_device *)rt_device_find("spinand0");
    if (snand_dev == RT_NULL)
    {
        RK_VENDOR_INF("%s: rt_device_find spinand0 failed\n", __func__);
        goto nm_out;
    }

    part_num = get_rk_partition(&part_info);
    if (part_num == 0 || part_info == RT_NULL)
    {
        RK_VENDOR_INF("%s: get_rk_partition failed\n", __func__);
        goto nm_out;
    }

    for (i = 0; i < part_num; i++)
    {
        if (!strcmp(part_info[i].name, name))
        {
            mtd = (struct mtd_info *)rt_malloc(sizeof(struct mtd_info));
            if (mtd == RT_NULL)
            {
                goto nm_out;
            }

            mtd->size = part_info[i].size;
            mtd->offset = part_info[i].offset;
            mtd->erasesize = snand_dev->page_size * snand_dev->pages_per_block;
            mtd->writesize = snand_dev->page_size;

            RK_VENDOR_INF("mtd: size 0x%x offset 0x%x erasesize 0x%x writesize 0x%x\n", mtd->size, mtd->offset, mtd->erasesize, mtd->writesize);

            break;
        }
    }

nm_out:
    return mtd;
#endif
}

static int mtd_vendor_nand_write(void)
{
    size_t bytes_write;
    int err, count = 0;
    struct erase_info ei;

re_write:
    if (nand_info.page_offset >= mtd_erase_size)
    {
        nand_info.blk_offset += mtd_erase_size;
        if (nand_info.blk_offset >= mtd->size)
            nand_info.blk_offset = 0;
        if (mtd_block_isbad(mtd, nand_info.blk_offset))
            goto re_write;

        memset(&ei, 0, sizeof(struct erase_info));
        ei.addr = nand_info.blk_offset;
        ei.len  = mtd_erase_size;
        if (mtd_erase(mtd, &ei))
            goto re_write;

        nand_info.page_offset = 0;
    }

    err = mtd_write(mtd, nand_info.blk_offset + nand_info.page_offset,
                    nand_info.ops_size, &bytes_write, (u8 *)g_vendor);
    nand_info.page_offset += nand_info.ops_size;
    if (err)
        goto re_write;

    count++;
    /* write 2 copies for reliability */
    if (count < 2)
        goto re_write;

    return 0;
}

static int mtd_vendor_storage_init(void)
{
    int err, offset;
    size_t bytes_read;
    struct erase_info ei;

    mtd = get_mtd_device_nm(vendor_mtd_name);
    if (mtd == RT_NULL)
        return -EIO;

    nand_info.page_offset = 0;
    nand_info.blk_offset = 0;
    nand_info.version = 0;
    nand_info.ops_size = (sizeof(*g_vendor) + mtd->writesize - 1) / mtd->writesize;
    nand_info.ops_size *= mtd->writesize;

    //RK_VENDOR_INF("init: sizeof(*g_vendor) %d align nand_info.ops_size = %d\n", sizeof(*g_vendor), nand_info.ops_size);

#ifdef RT_USING_DMA
    g_vendor = rt_dma_malloc(nand_info.ops_size);
#else
    g_vendor = rt_malloc_align(nand_info.ops_size, 64);
#endif
    if (!g_vendor)
    {
        RK_VENDOR_INF("%s: malloc %d failed\n", __func__, nand_info.ops_size);
        return -ENOMEM;
    }

    mtd_erase_size = mtd->erasesize;
    if (mtd_erase_size > mtd->size)
    {
        RK_VENDOR_INF("%s: mtd_erase_size %d > part size %d error\n", __func__, mtd_erase_size, mtd->size);
        return -ENOMEM;
    }

    for (offset = 0; offset < mtd->size; offset += mtd_erase_size)
    {
        if (!mtd_block_isbad(mtd, offset))
        {
            err = mtd_read(mtd, offset, nand_info.ops_size,
                           &bytes_read, (u8 *)g_vendor);
            if (err != 0)
                continue;
            if (bytes_read == nand_info.ops_size &&
                    g_vendor->tag == MTD_VENDOR_TAG &&
                    g_vendor->version == g_vendor->version2)
            {
                if (g_vendor->version > nand_info.version)
                {
                    nand_info.version = g_vendor->version;
                    nand_info.blk_offset = offset;
                    //RK_VENDOR_INF("init: L%d blk_offset 0x%x version %d\n", __LINE__, offset, g_vendor->version);
                }
            }
        }
        else if (nand_info.blk_offset == offset)
            nand_info.blk_offset += mtd_erase_size;
    }

    if (nand_info.version)
    {
        for (offset = mtd_erase_size - nand_info.ops_size;
                offset >= 0;
                offset -= nand_info.ops_size)
        {
            err = mtd_read(mtd, nand_info.blk_offset + offset,
                           nand_info.ops_size,
                           &bytes_read,
                           (u8 *)g_vendor);

            /* the page is not programmed */
            if (!err && bytes_read == nand_info.ops_size &&
                    g_vendor->tag == 0xFFFFFFFF &&
                    g_vendor->version == 0xFFFFFFFF &&
                    g_vendor->version2 == 0xFFFFFFFF)
                continue;

            /* point to the next free page */
            if (nand_info.page_offset < offset)
                nand_info.page_offset = offset + nand_info.ops_size;

            /* ecc error or io error */
            if (err != 0)
                continue;

            if (bytes_read == nand_info.ops_size &&
                    g_vendor->tag == MTD_VENDOR_TAG &&
                    g_vendor->version == g_vendor->version2)
            {
                if (nand_info.version > g_vendor->version)
                    g_vendor->version = nand_info.version;
                else
                    nand_info.version = g_vendor->version;
                //RK_VENDOR_INF("init: L%d page_offset 0x%x version %d\n", __LINE__, offset, g_vendor->version);
                break;
            }
        }
    }
    else
    {
        memset((u8 *)g_vendor, 0, nand_info.ops_size);
        g_vendor->version = 1;
        g_vendor->tag = MTD_VENDOR_TAG;
        g_vendor->free_size = sizeof(g_vendor->data);
        g_vendor->version2 = g_vendor->version;
        for (offset = 0; offset < mtd->size; offset += mtd_erase_size)
        {
            if (!mtd_block_isbad(mtd, offset))
            {
                memset(&ei, 0, sizeof(struct erase_info));
                ei.addr = nand_info.blk_offset + offset;
                ei.len  = mtd_erase_size;
                mtd_erase(mtd, &ei);
            }
        }
        //RK_VENDOR_INF("init: L%d version %d\n", __LINE__, g_vendor->version);
        mtd_vendor_nand_write();
    }

    return 0;
}

static int mtd_vendor_read(u32 id, void *pbuf, u32 size)
{
    u32 i;

    if (!g_vendor)
        return -ENOMEM;

    for (i = 0; i < g_vendor->item_num; i++)
    {
        if (g_vendor->item[i].id == id)
        {
            if (size > g_vendor->item[i].size)
                size = g_vendor->item[i].size;
            memcpy(pbuf,
                   &g_vendor->data[g_vendor->item[i].offset],
                   size);
            return size;
        }
    }
    return (-1);
}

static int mtd_vendor_write(u32 id, void *pbuf, u32 size)
{
    u32 i, j, align_size, alloc_size, item_num;
    u32 offset, next_size;
    u8 *p_data;
    struct vendor_item *item;
    struct vendor_item *next_item;

    if (!g_vendor)
        return -ENOMEM;

    p_data = g_vendor->data;
    item_num = g_vendor->item_num;
    align_size = VENDOR_ALIGN(size, 0x40); /* align to 64 bytes*/
    for (i = 0; i < item_num; i++)
    {
        item = &g_vendor->item[i];
        if (item->id == id)
        {
            alloc_size = VENDOR_ALIGN(item->size, 0x40);
            if (size > alloc_size)
            {
                if (g_vendor->free_size < align_size)
                    return -1;
                offset = item->offset;
                for (j = i; j < item_num - 1; j++)
                {
                    item = &g_vendor->item[j];
                    next_item = &g_vendor->item[j + 1];
                    item->id = next_item->id;
                    item->size = next_item->size;
                    item->offset = offset;
                    next_size = VENDOR_ALIGN(next_item->size,
                                             0x40);
                    memcpy(&p_data[offset],
                           &p_data[next_item->offset],
                           next_size);
                    offset += next_size;
                }
                item = &g_vendor->item[j];
                item->id = id;
                item->offset = offset;
                item->size = size;
                memcpy(&p_data[item->offset], pbuf, size);
                g_vendor->free_offset = offset + align_size;
                g_vendor->free_size = sizeof(g_vendor->data) - g_vendor->free_offset;
            }
            else
            {
                memcpy(&p_data[item->offset],
                       pbuf,
                       size);
                g_vendor->item[i].size = size;
            }
            g_vendor->version++;
            g_vendor->version2 = g_vendor->version;
            //RK_VENDOR_INF("write: L%d id %d item_num %d version %d\n", __LINE__, id, g_vendor->item_num, g_vendor->version);
            mtd_vendor_nand_write();
            return 0;
        }
    }

    if (g_vendor->free_size >= align_size)
    {
        item = &g_vendor->item[g_vendor->item_num];
        item->id = id;
        item->offset = g_vendor->free_offset;
        item->size = size;
        g_vendor->free_offset += align_size;
        g_vendor->free_size -= align_size;
        memcpy(&g_vendor->data[item->offset], pbuf, size);
        g_vendor->item_num++;
        g_vendor->version++;
        g_vendor->version2 = g_vendor->version;
        //RK_VENDOR_INF("write: L%d id %d item_num %d version %d\n", __LINE__, id, g_vendor->item_num, g_vendor->version);
        mtd_vendor_nand_write();
        return 0;
    }
    return (-1);
}

static int mtd_vendor_storage_probe(void)
{
    int ret;

    ret = mtd_vendor_storage_init();
    if (ret)
    {
        RK_VENDOR_INF("%s: mtd_vendor_storage_init failed\n", __func__);
        if (g_vendor != RT_NULL)
            rt_free_align(g_vendor);
        g_vendor = NULL;
        return ret;
    }

    rk_vendor_register(mtd_vendor_read, mtd_vendor_write);

    return 0;
}

INIT_APP_EXPORT(mtd_vendor_storage_probe);

#endif
#endif //RT_USING_VENDOR_STORAGE
