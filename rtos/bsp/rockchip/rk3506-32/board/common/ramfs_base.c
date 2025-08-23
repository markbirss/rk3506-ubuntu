/**
  * Copyright (c) 2023 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  */

#include <rtthread.h>
#include <rtdevice.h>

#ifdef RT_USING_DFS_RAMFS
#include <dfs_fs.h>
#include "dfs_ramfs.h"

#ifdef RT_DFS_RAMFS_SIZE
#define RAMFS_SIZE          RT_DFS_RAMFS_SIZE
#else
#define RAMFS_SIZE          32*1204
#endif

int ramfs_init(void)
{
    struct dfs_ramfs *ramfs;
    int ret = -RT_ERROR;

    rt_uint8_t *addr = rt_malloc(RAMFS_SIZE);
    if (addr == NULL)
    {
        rt_kprintf("ERROR: %s, %d\n", __func__, __LINE__);
        return ret;
    }
    ramfs = (struct dfs_ramfs *)dfs_ramfs_create((rt_uint8_t *)addr, RAMFS_SIZE);

    if (opendir("ramfs"))
    {
        ret = dfs_mount(NULL, "/ramfs", "ram", 0, ramfs);
    }
    else
    {
        if (RT_EOK == mkdir("/ramfs", 0x777))
        {
            ret = dfs_mount(NULL, "/ramfs", "ram", 0, ramfs);
        }
    }

    if (ret)
    {
        ret = dfs_mount(NULL, "/", "ram", 0, ramfs);
    }

    if (ret)
    {
        rt_kprintf("mount ramfs failed\n");
    }

    return ret;
}

INIT_APP_EXPORT(ramfs_init);

#endif
