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

#include "drv_flash_partition.h"
#include "dma.h"

#include "rk_vendor_storage.h"
#include "fw_analysis.h"

#if defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)

#define EMMC_VENDOR_PART_START      (1024 * 7)
#define EMMC_VENDOR_PART_SIZE       VENDOR_PART_SIZE
#define EMMC_VENDOR_PART_NUM        2
#define EMMC_VENDOR_TAG         VENDOR_HEAD_TAG

static struct vendor_info *g_vendor;

static int rk_emmc_transfer(u8 *buffer, u32 addr, u32 size, int write)
{
    int block_size = 4 * 1024;

    //rt_kprintf("%s: addr 0x%x size 0x%x write %d\n", __func__, addr, size, write);

    while (size)
    {
        if (block_size > size)
            block_size = size;
        if (write)
        {
            if (fw_flash_write(addr << 9, buffer, block_size) != block_size)
                return -1;
        }
        else
        {
            if (fw_flash_read(addr << 9, buffer, block_size) != block_size)
                return -1;
        }
        addr += block_size >> 9;
        buffer += block_size;
        size -= block_size;
    }

    return 0;
}

static int emmc_vendor_ops(u8 *buffer, u32 addr, u32 n_sec, int write)
{
    return rk_emmc_transfer(buffer, addr, n_sec << 9, write);
}

static int emmc_vendor_storage_init(void)
{
    u32 i, max_ver, max_index;
    u8 *p_buf;

    max_ver = 0;
    max_index = 0;
    for (i = 0; i < EMMC_VENDOR_PART_NUM; i++)
    {
        /* read first 512 bytes */
        p_buf = (u8 *)g_vendor;
        if (rk_emmc_transfer(p_buf, EMMC_VENDOR_PART_START +
                             EMMC_VENDOR_PART_SIZE * i, 512, 0))
            goto error_exit;
        /* read last 512 bytes */
        p_buf += (EMMC_VENDOR_PART_SIZE - 1) * 512;
        if (rk_emmc_transfer(p_buf, EMMC_VENDOR_PART_START +
                             EMMC_VENDOR_PART_SIZE * (i + 1) - 1,
                             512, 0))
            goto error_exit;

        if (g_vendor->tag == EMMC_VENDOR_TAG &&
                g_vendor->version2 == g_vendor->version)
        {
            if (max_ver < g_vendor->version)
            {
                max_index = i;
                max_ver = g_vendor->version;
            }
        }
    }
    if (max_ver)
    {
        if (emmc_vendor_ops((u8 *)g_vendor, EMMC_VENDOR_PART_START +
                            EMMC_VENDOR_PART_SIZE * max_index,
                            EMMC_VENDOR_PART_SIZE, 0))
            goto error_exit;
        g_vendor->free_size = sizeof(g_vendor->data) - g_vendor->free_offset;
    }
    else
    {
        memset((void *)g_vendor, 0, sizeof(*g_vendor));
        g_vendor->version = 1;
        g_vendor->tag = EMMC_VENDOR_TAG;
        g_vendor->version2 = g_vendor->version;
        g_vendor->free_offset = 0;
        g_vendor->free_size = sizeof(g_vendor->data);
    }
    return 0;
error_exit:
    return -1;
}

static int emmc_vendor_read(u32 id, void *pbuf, u32 size)
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

static int emmc_vendor_write(u32 id, void *pbuf, u32 size)
{
    u32 i, j, next_index, align_size, alloc_size, item_num;
    u32 offset, next_size;
    u8 *p_data;
    struct vendor_item *item;
    struct vendor_item *next_item;

    if (!g_vendor)
        return -ENOMEM;

    p_data = g_vendor->data;
    item_num = g_vendor->item_num;
    align_size = VENDOR_ALIGN(size, 0x40); /* align to 64 bytes*/
    next_index = g_vendor->next_index;
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
            g_vendor->next_index++;
            if (g_vendor->next_index >= EMMC_VENDOR_PART_NUM)
                g_vendor->next_index = 0;
            emmc_vendor_ops((u8 *)g_vendor, EMMC_VENDOR_PART_START +
                            EMMC_VENDOR_PART_SIZE * next_index,
                            EMMC_VENDOR_PART_SIZE, 1);
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
        g_vendor->next_index++;
        if (g_vendor->next_index >= EMMC_VENDOR_PART_NUM)
            g_vendor->next_index = 0;
        emmc_vendor_ops((u8 *)g_vendor, EMMC_VENDOR_PART_START +
                        EMMC_VENDOR_PART_SIZE * next_index,
                        EMMC_VENDOR_PART_SIZE, 1);
        return 0;
    }
    return (-1);
}

static int emmc_vendor_storage_probe(void)
{
    int ret;

#ifdef RT_USING_DMA
    g_vendor = rt_dma_malloc(sizeof(*g_vendor));
#else
    g_vendor = rt_malloc_align(sizeof(*g_vendor), 64);
#endif
    if (!g_vendor)
    {
        RK_VENDOR_INF("%s: malloc %d failed\n", __func__, sizeof(*g_vendor));
        return -ENOMEM;
    }

    ret = emmc_vendor_storage_init();
    if (ret)
    {
        RK_VENDOR_INF("%s: emmc_vendor_storage_init failed\n", __func__);
        if (g_vendor != RT_NULL)
            rt_free_align(g_vendor);
        g_vendor = NULL;
        return ret;
    }

    rk_vendor_register(emmc_vendor_read, emmc_vendor_write);

    return 0;
}

INIT_APP_EXPORT(emmc_vendor_storage_probe);
#endif
#endif //RT_USING_VENDOR_STORAGE
