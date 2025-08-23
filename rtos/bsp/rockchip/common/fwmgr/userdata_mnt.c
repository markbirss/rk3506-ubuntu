/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_USER_DATA_PART

#ifdef RT_USING_DFS
#include <dfs_file.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <drivers/mtd_nor.h>

#include "drv_flash_partition.h"
#include "dma.h"

#include "fw_analysis.h"

#ifdef RT_USING_FSCK_FAT
extern int checkfilesys(const char *);
#endif

#ifdef RT_USING_SNOR
#define USERDATA_PART_NAME  "mtd_userdata"
#else
#define USERDATA_PART_NAME  "userdata"
#endif
#define USERDATA_MOUNT_POINT    "/data"

#ifdef RT_USING_SPINAND
#define USERDATA_ERASE
#endif

#ifdef RT_USING_DFS

#ifdef USERDATA_ERASE
#ifdef RT_USING_SNOR
static struct rt_mtd_nor_device *snor_dev;
#elif defined(RT_USING_SPINAND)
#include "mini_ftl.h"
static struct rt_mtd_nand_device *snand_dev;
#endif

#if defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
extern int32_t get_rk_partition_emmc(struct rt_flash_partition **part);
#endif

static int get_userdata_part_info(struct rt_flash_partition *info)
{
    struct rt_flash_partition *part_info = RT_NULL;
    int i;
    int part_num;
    int ret = RT_EOK;

#ifdef RT_USING_SNOR
    snor_dev = (struct rt_mtd_nor_device *)rt_device_find("snor");
    if (snor_dev == RT_NULL)
    {
        rt_kprintf("%s: rt_device_find snor failed\n", __func__);
        ret = RT_ERROR;
        goto nm_out;
    }
#elif defined(RT_USING_SPINAND)
    snand_dev = (struct rt_mtd_nand_device *)rt_device_find("spinand0");
    if (snand_dev == RT_NULL)
    {
        rt_kprintf("%s: rt_device_find spinand0 failed\n", __func__);
        ret = RT_ERROR;
        goto nm_out;
    }
#endif

#if defined(RKMCU_RK2118) && defined(RT_USING_SDIO1)
    part_num = get_rk_partition_emmc(&part_info);
#elif defined(RT_USING_SPINAND) || defined(RT_USING_SNOR)
    part_num = get_rk_partition(&part_info);
#endif
    if (part_num == 0 || part_info == RT_NULL)
    {
        rt_kprintf("%s: get_gpt_partition failed\n", __func__);
        ret = RT_ERROR;
        goto nm_out;
    }

    for (i = 0; i < part_num; i++)
    {
        if (!strcmp(part_info[i].name, USERDATA_PART_NAME))
        {
#ifdef RT_USING_SNOR
            //part_info[i].size = snor_dev->block_end * snor_dev->block_size - part_info[i].offset;
            part_info[i].size -= 64 * 512; // reserve for gpt
#endif
            memcpy(info, &part_info[i], sizeof(struct rt_flash_partition));
            rt_kprintf("userdata_mnt: part offset 0x%x size 0x%x\n", part_info[i].offset, part_info[i].size);
            break;
        }
    }

nm_out:
    return ret;
}

static int erase_userdata(struct rt_flash_partition *info, rt_device_t dev)
{
#ifdef RT_USING_SPINAND
    return rt_device_control(dev, RT_DEVICE_CTRL_MTD_FORMAT, RT_NULL);
#else
    int len;
    int offset = info->offset;
    int size = info->size;
#ifdef RT_USING_SNOR
    int block_size = 64 * 1024;
#elif defined(RT_USING_SPINAND)
    int block_size = snand_dev->page_size * snand_dev->pages_per_block;
#endif

#ifdef RT_USING_SPINAND
    size = size / block_size;
    size = size * block_size;
#endif

    while (size)
    {
#ifdef RT_USING_SNOR
        len = size > block_size ? block_size : size;
        if (rt_mtd_nor_erase_block(snor_dev, offset, len) != RT_EOK)
        {
            rt_kprintf("%s: flash erase offset 0x%x fail\n", __func__, offset);
            return RT_ERROR;
        }
#elif defined(RT_USING_SPINAND)
        len = block_size;
        if (mini_ftl_erase(snand_dev, offset, len) != RT_EOK)
        {
            rt_kprintf("%s: flash erase offset 0x%x fail\n", __func__, offset);
            return RT_ERROR;
        }
#endif
        if (size >= len)
            size -= len;
        else
            size = 0;

        //rt_kprintf("userdata: erase 0x%x/0x%x\n", offset, len);
        offset += len;
    }
    return RT_EOK;
#endif
}
#endif

static rt_bool_t check_mount_point_exist(void)
{
    struct stat st;
    int result = stat(USERDATA_MOUNT_POINT, &st);

    if (result == 0 && S_ISDIR(st.st_mode))
    {
        return RT_TRUE;
    }
    else
    {
        return RT_FALSE;
    }
}

static rt_bool_t root_mnt(void)
{
#if defined(RT_USING_NEW_OTA_MODE_AB_RECOVERY) || defined(RT_USING_NEW_OTA_MODE_AB)
    rt_device_t dev_root;
    uint32_t slot = -1;
    char *name;
    struct statfs st;
    char root_name[2][8] =
    {
        "root_a",
        "root_b"
    };

    if(dfs_statfs("/", &st) == RT_EOK)
        return RT_TRUE;

    if (fw_slot_get_current_running(&slot) != RT_EOK)
        return RT_FALSE;

    name = root_name[slot];
    rt_kprintf("%s: use %s\n", __func__, name);

    dev_root = rt_device_find(name);
    if (dev_root == RT_NULL)
    {
        rt_kprintf("can't find %s device\n", name);
        return RT_FALSE;
    }

    if (dfs_mount(name, "/", "elm", 0, 0) != 0)
    {
        rt_kprintf("mount %s fs[elm] on %s failed.\n", name, "/");
        return RT_FALSE;
    }
    rt_kprintf("mount %s fs[elm] on %s success.\n", name, "/");
#endif
    return RT_TRUE;
}

static void userdata_mnt_thr(void *param)
{
#ifdef USERDATA_ERASE
    struct rt_flash_partition udata_info;
#endif
    rt_device_t dev_udata;
    int mount_count = 1;
    const char *path;
#ifdef RT_USING_SNOR
    char fs_name[8] = "lfs";
#else
    char fs_name[8] = "elm";
#endif

    if (root_mnt() != RT_TRUE)
    {
        return;
    }

    dev_udata = rt_device_find(USERDATA_PART_NAME);
    if (dev_udata == RT_NULL)
    {
        //rt_kprintf("can't find %s device\n", USERDATA_PART_NAME);
        return;
    }

    if (check_mount_point_exist() != RT_TRUE)
    {
        return;
    }

    // if already mounted, skip mount here
    path = dfs_filesystem_get_mounted_path(dev_udata);
    if (path != NULL)
    {
        return;
    }

    // if opened by other, skip mount here
    if (rt_device_open(dev_udata, RT_DEVICE_OFLAG_RDWR) != RT_EOK)
    {
        return;
    }
    rt_device_close(dev_udata);

#ifdef USERDATA_ERASE
    if (get_userdata_part_info(&udata_info) != RT_EOK)
    {
        rt_kprintf("get_userdata_part_info failed\n");
        return;
    }
#endif

#if defined(RT_USING_FSCK_FAT) && !defined(RT_USING_SNOR)
    // 1. fsck userdata
    rt_kprintf("userdata_mnt: fsck\n");
    checkfilesys(USERDATA_PART_NAME);
#endif
do_mount:
    // 2. try mount userdata
    rt_kprintf("userdata_mnt: try mount fs[%s]\n", fs_name);
    if (dfs_mount(USERDATA_PART_NAME, USERDATA_MOUNT_POINT, fs_name, 0, 0) != 0)
    {
        rt_kprintf("mount %s fs[%s] on %s failed.\n", USERDATA_PART_NAME, fs_name, USERDATA_MOUNT_POINT);
        if (mount_count-- <= 0)
            return;

#ifdef USERDATA_ERASE
        // 3. if mount fail, maybe the first time startup after upgrade, need erase userdata
        rt_kprintf("userdata_mnt: mount fail, erase userdata\n");
        if (erase_userdata(&udata_info, dev_udata) != RT_EOK)
        {
            rt_kprintf("erase_userdata failed\n");
            return;
        }
#endif
        // 3. mkfs on userdata
        rt_kprintf("userdata_mnt: mkfs on userdata\n");
        if (dfs_mkfs(fs_name, USERDATA_PART_NAME) != RT_EOK)
        {
            rt_kprintf("dfs_mkfs failed\n");
            return;
        }

        goto do_mount;
    }

    rt_kprintf("mount %s fs[%s] on %s success.\n", USERDATA_PART_NAME, fs_name, USERDATA_MOUNT_POINT);
}

static int userdata_mnt(void)
{
    rt_thread_t mnt_tid;

#ifdef RK2118_CPU_CORE1
    return 0;
#endif

#ifdef RT_USING_SNOR
#ifdef RT_BUILD_RECOVERY_FW
    return 0;
#endif
#endif

    mnt_tid = rt_thread_create("userdata_mnt",
                               userdata_mnt_thr, RT_NULL,
                               2048,
                               21,
                               20);
    if (mnt_tid != RT_NULL)
    {
        rt_thread_startup(mnt_tid);
    }

    return 0;
}

INIT_ENV_EXPORT(userdata_mnt);
#endif
#endif //RT_USING_USER_DATA_PART