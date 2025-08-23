/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Cerf Yu <cerf.yu@rock-chips.com>
 */

#ifndef __RTT_ADAPTER_LINUX_SLAB_H__
#define __RTT_ADAPTER_LINUX_SLAB_H__

#include <stdlib.h>
#include <string.h>

#include <rtdevice.h>

#include "adapter/include/linux/types.h"

#define ZERO_SIZE_PTR ((void *)16)
#define ZERO_OR_NULL_PTR(x) ((unsigned long)(x) <= (unsigned long)ZERO_SIZE_PTR)

static inline void *kmalloc(size_t size, gfp_t flags)
{
    void *mem = rt_malloc(size);
    if (flags & __GFP_ZERO)
    {
        rt_memset(mem, 0, size);
    }
    return mem;
}

static inline void *kzalloc(size_t size, gfp_t flags)
{
    return kmalloc(size, flags | __GFP_ZERO);
}

static inline void *kcalloc(size_t nsize, size_t size, gfp_t flags)
{
    return rt_calloc(nsize, size);
}

static inline void kfree(void *addr)
{
    rt_free(addr);
}

#endif  /* #ifndef __RTT_ADAPTER_LINUX_SLAB_H__ */
