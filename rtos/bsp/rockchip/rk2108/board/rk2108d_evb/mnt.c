/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
 */

/** @addtogroup RKBSP_Board_Driver
 *  @{
 */

/** @addtogroup MNT
 *  @{
 */

/** @defgroup How_To_Use How To Use
 *  @{
 @verbatim

 ==============================================================================
                    #### How to use ####
 ==============================================================================
 This file provide mount table for filesystem. The patition defined in mount table will be mounted automatically.

 @endverbatim
 @} */

#include <rtthread.h>
#include <rtdevice.h>

#ifdef RT_USING_DFS_MNTTABLE
#include <dfs_fs.h>

#define PARTITION_GPT           "gpt"
#define PARTITION_LOADER        "loader"
#define PARTITION_FIRMWARE      "firmware"
#define PARTITION_ROOT          "root"

/********************* Private Variable Definition ***************************/
/** @defgroup MNT_Private_Variable Private Variable
 *  @{
 */

/**
 * @brief  Config mount table of filesystem
 * @attention The mount_table must be terminated with NULL, and the partition's name
 * must be the same as above.
 */

const struct dfs_mount_tbl mount_table[] =
{
    {PARTITION_ROOT, "/", "elm", 0, 0},
#if defined(RT_USING_LITTLEFS) && defined(RT_USING_SPINAND)
    {"spinand0", "/nand", "lfs", 0, 0},
#elif defined(RT_USING_SDIO0) && defined(RT_SDCARD_MOUNT_POINT)
    {"sd0", RT_SDCARD_MOUNT_POINT, "elm", 0, 0},
#endif
    {0}
};

#endif

/** @} */  // MNT_Private_Variable

/** @} */  // MNT

/** @} */  // RKBSP_Board_Driver
