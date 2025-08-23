/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: BSD-3-Clause
  ******************************************************************************
  * @file    drv_flash_partition.c
  * @version V1.0
  * @brief   flash partition device driver
  *
  * Change Logs:
  * Date           Author          Notes
  * 2019-02-20     Dingqiang Lin   the first version
  *
  ******************************************************************************
  */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <drivers/mtd_nor.h>
#ifdef RT_USING_DFS
#include <dfs_fs.h>
#endif

#include "drv_flash_partition.h"

#if defined(RT_USING_SNOR) || defined(RT_USING_SPINAND)
#include "hal_base.h"

//define PART_DEBUG
#ifdef PART_DEBUG
#define PART_DBG(...)     rt_kprintf(__VA_ARGS__)
#else
#define PART_DBG(...)
#endif

static struct rt_flash_partition *flash_parts;
static int32_t part_num = 0;

#ifdef RT_USING_SNOR
static rt_err_t part_blk_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t part_blk_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t part_blk_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t part_blk_control(rt_device_t dev, int cmd, void *args)
{
    struct rt_flash_partition *blk_part = DEV_2_PART(dev);
    struct rt_mtd_nor_device *mtd_nor = (struct rt_mtd_nor_device *)dev->user_data;

    PART_DBG("%s %ld\n", __func__, blk_part->size);
    PART_DBG("%s %ld\n", __func__, mtd_nor->block_size);

    RT_ASSERT(dev != RT_NULL);
    switch (cmd)
    {
    case RT_DEVICE_CTRL_BLK_GETGEOME:
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL)
            return -RT_ERROR;
        geometry->bytes_per_sector  = mtd_nor->block_size;
        geometry->sector_count      = blk_part->size / mtd_nor->block_size;
        geometry->block_size        = mtd_nor->block_size;
        break;
    }
    default:
        break;
    }

    return RT_EOK;
}

static rt_size_t part_blk_read(rt_device_t dev, rt_off_t sec, void *buffer, rt_size_t nsec)
{
    struct rt_flash_partition *blk_part = DEV_2_PART(dev);
    struct rt_mtd_nor_device *mtd_nor = (struct rt_mtd_nor_device *)dev->user_data;
    rt_size_t   read_count = 0;
    rt_uint8_t *ptr = (rt_uint8_t *)buffer;
    rt_size_t ret;

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(nsec != 0);

    PART_DBG("%s sec = %08x,nsec = %08x %lx %lx\n", __func__, sec, nsec, blk_part->offset, blk_part->size);
    if (!(blk_part->mask_flags & PART_FLAG_RDONLY))
    {
        rt_kprintf("ERROR: partition %s is unreadable, mask_flags = %04x\n", blk_part->name, blk_part->mask_flags);
        return 0;
    }

    while (read_count < nsec)
    {
        if (((sec + 1) * mtd_nor->block_size) > (blk_part->offset + blk_part->size))
        {
            rt_kprintf("ERROR: read overrun!\n");
            return read_count;
        }

        /* It'a BLOCK device */
        ret = rt_mtd_nor_read(mtd_nor, (sec * mtd_nor->block_size + blk_part->offset), ptr, mtd_nor->block_size);
        if (ret != mtd_nor->block_size)
            return read_count;
        sec++;
        ptr += mtd_nor->block_size;
        read_count++;
    }

    return nsec;
}

static rt_size_t part_blk_write(rt_device_t dev, rt_off_t sec, const void *buffer, rt_size_t nsec)
{
    struct rt_flash_partition *blk_part = DEV_2_PART(dev);
    struct rt_mtd_nor_device *mtd_nor = (struct rt_mtd_nor_device *)dev->user_data;
    rt_size_t   write_count = 0;
    rt_uint8_t *ptr = (rt_uint8_t *)buffer;
    rt_size_t ret;

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(nsec != 0);

    PART_DBG("%s sec = %08x,nsec = %08x %lx %lx\n", __func__, sec, nsec, blk_part->offset, blk_part->size);

    if (!(blk_part->mask_flags & PART_FLAG_WRONLY))
    {
        rt_kprintf("ERROR: partition %s is unwriteable, mask_flags = %04x\n", blk_part->name, blk_part->mask_flags);
        return 0;
    }

    while (write_count < nsec)
    {
        if (((sec + 1) * mtd_nor->block_size) > (blk_part->offset + blk_part->size))
        {
            rt_kprintf("ERROR: write overrun!\n");
            return write_count;
        }
        /* It'a BLOCK device */
        ret = rt_mtd_nor_erase_block(mtd_nor, (sec * mtd_nor->block_size + blk_part->offset), mtd_nor->block_size);
        if (ret)
            return write_count;
        ret = rt_mtd_nor_write(mtd_nor, (sec * mtd_nor->block_size + blk_part->offset), ptr, mtd_nor->block_size);
        if (ret != mtd_nor->block_size)
            return write_count;
        sec++;
        ptr += mtd_nor->block_size;
        write_count++;
    }

    return write_count;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops part_blk_ops =
{
    part_blk_init,
    part_blk_open,
    part_blk_close,
    part_blk_read,
    part_blk_write,
    part_blk_control,
};
#endif

/* Register a partition as block partition */
static rt_err_t blk_init_partition(struct rt_mtd_nor_device *dev, struct rt_flash_partition *blk_part)
{
    if (dev == RT_NULL)
        return -RT_EIO;

    if (blk_part == RT_NULL)
        return -RT_EINVAL;

    PART_DBG("blk part name: %s\n", blk_part->name);
    /* blk dev setting */
    blk_part->blk.type      = RT_Device_Class_Block;
#ifdef RT_USING_DEVICE_OPS
    blk_part->blk.ops       = &part_blk_ops;
#else
    blk_part->blk.init      = part_blk_init;
    blk_part->blk.open      = part_blk_open;
    blk_part->blk.read      = part_blk_read;
    blk_part->blk.write     = part_blk_write;
    blk_part->blk.close     = part_blk_close;
    blk_part->blk.control   = part_blk_control;
#endif
    blk_part->blk.user_data = dev;  /* snor blk dev for operation */
    /* register device */
    return rt_device_register(&blk_part->blk, blk_part->name, blk_part->mask_flags | RT_DEVICE_FLAG_STANDALONE);
}

static rt_base_t part_mtd_nor_read_id(struct rt_mtd_nor_device *dev)
{
    struct rt_flash_partition *mtd_part = DEV_2_PART(dev);
    struct rt_mtd_nor_device *mtd_nor = (struct rt_mtd_nor_device *)mtd_part->user_data;

    RT_ASSERT(dev != RT_NULL);

    return rt_mtd_nor_read_id(mtd_nor);
}

static rt_size_t part_mtd_nor_read(struct rt_mtd_nor_device *dev, rt_off_t offset, rt_uint8_t *buffer, rt_uint32_t length)
{
    struct rt_flash_partition *mtd_part = DEV_2_PART(dev);
    struct rt_mtd_nor_device *mtd_nor = (struct rt_mtd_nor_device *)mtd_part->user_data;

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(length != 0);

    PART_DBG("%s offset = %08x,size = %08x\n", __func__, offset, length);

    if (!(mtd_part->mask_flags & PART_FLAG_RDONLY))
    {
        rt_kprintf("ERROR: this partion %s is unreadable\n", mtd_part->name);
        /* read only partition, ignore this data */
        return length;
    }

    if ((offset + length) > mtd_part->size)
    {
        rt_kprintf("ERROR: %s Pos %x len %x is exceeding partition size\n", __func__, offset, length);
        return 0;
    }

    return rt_mtd_nor_read(mtd_nor, (mtd_part->offset + offset), buffer, length);
}

static rt_size_t part_mtd_nor_write(struct rt_mtd_nor_device *dev, rt_off_t offset, const rt_uint8_t *buffer, rt_uint32_t length)
{
    struct rt_flash_partition *mtd_part = DEV_2_PART(dev);
    struct rt_mtd_nor_device *mtd_nor = (struct rt_mtd_nor_device *)mtd_part->user_data;

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(length != 0);

    PART_DBG("%s offset = %08x,size = %08x\n", __func__, offset, length);

    if (!(mtd_part->mask_flags & PART_FLAG_WRONLY))
    {
        rt_kprintf("ERROR: this partion %s is unwritable\n", mtd_part->name);
        /* write only partition, ignore this data */
        return length;
    }

    if ((offset + length) > mtd_part->size)
    {
        rt_kprintf("ERROR: %s Pos %x len %x is exceeding partition size\n", __func__, offset, length);
        return 0;
    }

    return rt_mtd_nor_write(mtd_nor, (mtd_part->offset + offset), buffer, length);
}

static rt_err_t part_mtd_nor_erase_block(struct rt_mtd_nor_device *dev, rt_off_t offset, rt_uint32_t length)
{
    struct rt_flash_partition *mtd_part = DEV_2_PART(dev);
    struct rt_mtd_nor_device *mtd_nor = (struct rt_mtd_nor_device *)mtd_part->user_data;

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(length != 0);

    PART_DBG("%s offset = %08x,size = %08x\n", __func__, offset, length);

    if ((offset + length) > mtd_part->size)
    {
        rt_kprintf("ERROR: %s Pos %x len %x is exceeding partition size\n", __func__, offset, length);
        return 0;
    }

    return rt_mtd_nor_erase_block(mtd_nor, (mtd_part->offset + offset), length);
}

const static struct rt_mtd_nor_driver_ops mtd_part_mtd_ops =
{
    part_mtd_nor_read_id,
    part_mtd_nor_read,
    part_mtd_nor_write,
    part_mtd_nor_erase_block,
};

/* Register a partition as mtd nor partition */
static rt_err_t mtd_nor_init_partition(struct rt_mtd_nor_device *dev, struct rt_flash_partition *mtd_part)
{
    if (dev == RT_NULL)
        return -RT_EIO;

    RT_ASSERT(mtd_part);

    PART_DBG("mtd part name: %s\n", mtd_part->name);
    /* mtd dev setting */
    mtd_part->mtd_nor.block_size    = dev->block_size;
    mtd_part->mtd_nor.block_start   = 0;
    mtd_part->mtd_nor.block_end     = mtd_part->size / dev->block_size;
    mtd_part->mtd_nor.ops           = &mtd_part_mtd_ops;
    mtd_part->user_data             = dev;   /* snor mtd dev for operation */
    return rt_mtd_nor_register_device(mtd_part->name, &mtd_part->mtd_nor);
}
#endif /* RT_USING_SNOR */

/* Parse RK_PARTITION for rk flash partition and register it */
uint32_t rk_partition_init(struct rk_partition_info *part_temp, uint32_t flash_size)
{
#ifdef RT_USING_DFS
    struct dfs_partition part;
    int ret;
#endif
    int32_t i;

    if (flash_parts)
    {
        rt_kprintf("flash_parts(%p) is available already\n", flash_parts);
        return RT_EOK;
    }

    /* Parse RK_PARTITION for rk partition */
    if (part_temp->hdr.ui_fw_tag == RK_PARTITION_TAG)
    {
        part_num = part_temp->hdr.ui_part_entry_count;
        flash_parts = rt_malloc(sizeof(struct rt_flash_partition) * part_num);
        rt_memset(flash_parts, 0, sizeof(struct rt_flash_partition) * part_num);

        RT_ASSERT(flash_parts);
        for (i = 0; i < part_num; i++)
        {
            rt_strncpy(flash_parts[i].name, (const char *)part_temp->part[i].sz_name,
                       RK_PARTITION_NAME_SIZE);
            PART_DBG("rk_partition flags=%08x type= %08x off=%08x size=%08x %s\n",
                     part_temp->part[i].ui_part_property,
                     part_temp->part[i].em_part_type,
                     part_temp->part[i].ui_pt_off,
                     part_temp->part[i].ui_pt_sz,
                     &flash_parts[i].name[0]);
            flash_parts[i].offset = (uint32_t)part_temp->part[i].ui_pt_off << 9;
            if (part_temp->part[i].ui_pt_sz == 0xFFFFFFFF || (part_temp->part[i].ui_part_property & RK_PARTITION_NO_PARTITION_SIZE))
            {
                flash_parts[i].size = flash_size - flash_parts[i].offset;
                PART_DBG(">>>>>>>>>>>[%d] size change=%08x\n", i, flash_parts[i].size);
            }
            else
                flash_parts[i].size = (uint32_t)part_temp->part[i].ui_pt_sz << 9;
            flash_parts[i].type = (uint32_t)part_temp->part[i].em_part_type;
            flash_parts[i].mask_flags = (part_temp->part[i].ui_part_property &
                                         RK_PARTITION_PROPERTY_MASK) >>
                                        RK_PARTITION_PROPERTY_SHIFT;
            if (part_temp->part[i].ui_part_property & RK_PARTITION_REGISTER_TYPE_MTD)
                flash_parts[i].mask_flags |= PART_FLAG_MTD;
            else
                flash_parts[i].mask_flags |= PART_FLAG_BLK;
        }
#ifdef RT_USING_DFS
    }
    else if (dfs_filesystem_get_partition(&part, (uint8_t *)(part_temp), RK_PARTITION_SEC, 0) == 0)
    {
        for (i = 0; i < RK_PARTITION_MAX_PARTITION; i++)
        {
            ret = dfs_filesystem_get_partition(&part, (uint8_t *)(part_temp), RK_PARTITION_SEC, i);
            if (ret != RT_EOK)
            {
                break;
            }
        }
        part_num = i;
        flash_parts = rt_malloc(sizeof(struct rt_flash_partition) * part_num);
        rt_memset(flash_parts, 0, sizeof(struct rt_flash_partition) * part_num);
        RT_ASSERT(flash_parts);
        for (i = 0; i < part_num; i++)
        {
            dfs_filesystem_get_partition(&part, (uint8_t *)(part_temp), 8, i);
            rt_strncpy(flash_parts[i].name, part.name, RK_PARTITION_NAME_SIZE);
            flash_parts[i].offset = part.offset << 9;
            if (part.size == 0xFFFFFFFF)
            {
                flash_parts[i].size = flash_size - (part.offset << 9);
            }
            else if ((part.offset + part.size + 64) >= (flash_size >> 9))     /* reserved 64KB for gpt backup */
            {
                flash_parts[i].size = (flash_size - 0x10000) - (part.offset << 9);
                rt_kprintf("part: name=%s off=%08x size=%08x type=%08x flags=%08x\n",
                           flash_parts[i].name,
                           flash_parts[i].offset,
                           flash_parts[i].size,
                           flash_parts[i].type,
                           flash_parts[i].mask_flags);
            }
            else
            {
                flash_parts[i].size = part.size << 9;
            }
            flash_parts[i].type = PART_USER1;
            if (!rt_strncmp(flash_parts[i].name, "mtd_", 4))
            {
                flash_parts[i].mask_flags = PART_FLAG_MTD | PART_FLAG_RDWR;
            }
            else
            {
                flash_parts[i].mask_flags = PART_FLAG_BLK | PART_FLAG_RDWR;
            }
            PART_DBG("part: name=%s off=%08x size=%08x type=%08x flags=%08x\n",
                     flash_parts[i].name,
                     flash_parts[i].offset,
                     flash_parts[i].size,
                     flash_parts[i].type,
                     flash_parts[i].mask_flags);
        }
#endif
    }
    else
    {
#if defined(RT_ROOT_PART_OFFSET) && defined(RT_ROOT_PART_SIZE)
        flash_parts = rt_malloc(sizeof(struct rt_flash_partition));
        part_num = 1;
        rt_strncpy(flash_parts->name, "root", 5);
        flash_parts->offset = RT_ROOT_PART_OFFSET * 512;
        flash_parts->size = RT_ROOT_PART_SIZE * 512;
        flash_parts->mask_flags = PART_FLAG_BLK | PART_FLAG_RDWR;
        flash_parts->type = 0x8;
        rt_kprintf("%s register root in config\n", __func__);
#endif
    }

    return RT_EOK;
}

#ifdef RT_USING_SNOR
/* Parse RK_PARTITION for rk flash partition and register it */
uint32_t mtd_nor_rk_partition_init(struct rt_mtd_nor_device *mtd)
{
    uint32_t ret;
    struct rk_partition_info *part_temp;
    int32_t i;

    if (flash_parts)
    {
        rt_kprintf("flash_parts(%p) is available already\n", flash_parts);
        return RT_EOK;
    }

    /* Parse RK_PARTITION for rk flash partition */
    part_temp = rt_malloc(RK_PARTITION_SIZE);
    RT_ASSERT(part_temp);
    if (rt_mtd_nor_read(mtd, 0, (rt_uint8_t *)part_temp, RK_PARTITION_SIZE) == RK_PARTITION_SIZE)
    {
        rk_partition_init(part_temp, mtd->block_end * mtd->block_size);
    }
    rt_free(part_temp);

    /* Register partitions */
    for (i = 0; i < part_num; i++)
    {
        PART_DBG("rt_flash_partition flags=%08x type= %08x off=%08x size=%08x %s\n",
                 flash_parts[i].mask_flags,
                 flash_parts[i].type,
                 flash_parts[i].offset,
                 flash_parts[i].size,
                 flash_parts[i].name);
        if (flash_parts[i].mask_flags & PART_FLAG_RDWR)
        {
            if (flash_parts[i].mask_flags & PART_FLAG_MTD)
                ret = mtd_nor_init_partition(mtd, &flash_parts[i]);
            else
                ret = blk_init_partition(mtd, &flash_parts[i]);
            if (ret)
                return ret;
        }
    }

    return RT_EOK;
}
#endif /* RT_USING_SNOR */

#ifdef RT_USING_SPINAND
/* Parse RK_PARTITION for rk flash partition */
uint32_t mtd_nand_rk_partition_init(struct rt_mtd_nand_device *mtd)
{
    struct rk_partition_info *part_temp;
    uint32_t part_size = RK_PARTITION_SIZE;
    uint32_t i, page_count;
    rt_err_t ret = 0;

    if (flash_parts)
    {
        rt_kprintf("flash_parts(%p) is available already\n", flash_parts);
        return RT_EOK;
    }

    /* Parse RK_PARTITION for rk flash partition */
    part_temp = rt_malloc(part_size);
    RT_ASSERT(part_temp);
    page_count = (RK_PARTITION_SIZE + mtd->page_size - 1) / mtd->page_size;
    for (i = 0; i < page_count; i++)
    {
        ret = rt_mtd_nand_read(mtd, i, (rt_uint8_t *)part_temp + mtd->page_size * i, mtd->page_size, RT_NULL, 0);
        if (ret)
        {
            rt_kprintf("%s read page failed, ret=%d\n", __func__, ret);
            break;
        }
    }
    if (ret == RT_EOK)
    {
        rk_partition_init(part_temp, mtd->block_total * mtd->pages_per_block * mtd->page_size);
    }
    rt_free(part_temp);

    return RT_EOK;
}
#endif

/* Get RK_PARTITION */
int32_t get_rk_partition(struct rt_flash_partition **part)
{
    *part = flash_parts;

    return part_num;
}

#ifdef RT_USING_SNOR
static void update_flash_parts_name(int em_part_type, char *part_name)
{
#ifdef RT_USING_XIP
    struct rt_flash_partition *part;
    int i;

    for (i = 0; i < part_num; i++)
    {
        part = &flash_parts[i];

        if (part->type == em_part_type)
        {
            rt_strncpy((char *)part->name, (const char *)part_name, RK_PARTITION_NAME_SIZE);
        }
    }

#endif
}
#endif

int change_part_name(int em_part_type, char *new_name)
{
    int ret = -RT_ERROR;
#ifdef RT_USING_SNOR
    int size = 0;
    rt_uint8_t *partition = RT_NULL;
    STRUCT_PART_ITEM *entry = RT_NULL;
    STRUCT_PART_INFO *part_info = RT_NULL;;
    struct rt_mtd_nor_device *snor_device = RT_NULL;

    snor_device = (struct rt_mtd_nor_device *)rt_device_find("snor");
    if (snor_device ==  RT_NULL)
    {
        rt_kprintf("Did not find device: snor....\n");
        return -RT_ERROR;
    }

    size = RK_PARTITION_SIZE * 2;
    partition = (rt_uint8_t *)rt_malloc_align(size, 64);
    if (size == rt_mtd_nor_read(snor_device, 0, partition, size))
    {
        part_info = (STRUCT_PART_INFO *)partition;
        if (part_info->hdr.ui_fw_tag == RK_PARTITION_TAG)
        {
            int i;
            for (i = 0; i < part_info->hdr.ui_part_entry_count; i++)
            {
                if (part_info->part[i].em_part_type == em_part_type)
                {
                    entry = &part_info->part[i];
                    if (entry)
                    {
                        if (!rt_strcmp((const char *)entry->sz_name, (const char *)new_name))
                        {
                            /* have same part name do nothing */
                            PART_DBG("have same part name do nothing\n");
                            rt_free_align(partition);
                            return RT_EOK;
                        }

                        PART_DBG("old part name : %s\n", entry->sz_name);
                        rt_memset(entry->sz_name, 0, sizeof(entry->sz_name));
                        rt_strncpy((char *)entry->sz_name, (const char *)new_name, RK_PARTITION_NAME_SIZE);
                        PART_DBG("new part name : %s\n", entry->sz_name);
                        update_flash_parts_name(part_info->part[i].em_part_type, new_name);

                        /* write back to flash*/
                        rt_mtd_nor_erase_block(snor_device, 0, snor_device->block_size);
                        if (size != rt_mtd_nor_write(snor_device, 0, (const rt_uint8_t *)partition, size))
                        {
                            PART_DBG("mtd nor write partition data Error!");
                            ret = -RT_ERROR;
                        }

                        ret = RT_EOK;
                    }
                }
            }

        }
    }

    if (partition)
        rt_free_align(partition);
#endif /* RT_USING_SNOR */

    return ret;
}

ssize_t read_rk_partition(char *part_name, void *buf, size_t count, off_t offset)
{
#ifdef RT_USING_XIP
    struct rt_flash_partition *part;
    int i;
    unsigned long addr;
    size_t len = -1;

    for (i = 0; i < part_num; i++)
    {
        part = &flash_parts[i];

        if (strncmp(part_name, part->name, RK_PARTITION_NAME_SIZE) == 0)
        {
            if (offset < part->size)
            {
                addr = XIP_MAP0_BASE0 + part->offset + offset;
                len = (offset + count) > part->size ? (part->size - offset) : count;
                //rt_kprintf("found addr: 0x%x, read len=%d\n", addr, len);
                memcpy(buf, (const void *)addr, len);
            }
        }
    }

    return len;
#else
    return -1;
#endif
}

void *get_addr_by_part_name(char *part_name)
{
#ifdef RT_USING_XIP
    struct rt_flash_partition *part;
    int i;
    void  *addr = NULL;

    for (i = 0; i < part_num; i++)
    {
        part = &flash_parts[i];

        if (strncmp(part_name, part->name, RK_PARTITION_NAME_SIZE) == 0)
        {
            addr = (void *)(XIP_MAP0_BASE0 + part->offset);
        }
    }

    return addr;
#else
    return NULL;
#endif
}

#endif
