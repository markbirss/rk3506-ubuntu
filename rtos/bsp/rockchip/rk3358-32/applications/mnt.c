/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-17     Cliff Chen   first implementation
 */

#include <rtthread.h>

#ifdef BSP_USING_SDIO0
#include <dfs_fs.h>

int mnt_init(void)
{
    rt_thread_delay(RT_TICK_PER_SECOND);
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("file system initialization done!\n");
    }

    return 0;
}
INIT_ENV_EXPORT(mnt_init);
#endif
