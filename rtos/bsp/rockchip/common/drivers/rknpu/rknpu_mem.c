/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * @file    rknpu_mem.c
 * @author  randall zhuo
 * @version V0.1
 * @date    23-Jan-2024
 * @brief   rknpu memory handle
 *
 ******************************************************************************
 */

#include <rtconfig.h>
#include <rtthread.h>
#include <drv_heap.h>
#include "rknpu_mem.h"

void *rt_malloc_rknpu(rt_size_t size)
{
    return rt_malloc(size);
}
RTM_EXPORT(rt_malloc_rknpu);

void rt_free_rknpu(void *ptr)
{
    rt_free(ptr);
}
RTM_EXPORT(rt_free_rknpu);
