/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Cerf Yu <cerf.yu@rock-chips.com>
 */

#ifndef __RTT_ADAPTER_LINUX_ERR_H__
#define __RTT_ADAPTER_LINUX_ERR_H__

#include <errno.h>

/*
 * Kernel pointers have redundant information, so we can use a
 * scheme where we can return either an error code or a normal
 * pointer with the same return value.
 *
 * This should be a per-architecture thing, to allow different
 * error and pointer decisions.
 */
#define MAX_ERRNO   4095

#ifndef __ASSEMBLY__

#define IS_ERR_VALUE(x) unlikely((x) >= (unsigned long)-MAX_ERRNO)

static inline void *ERR_PTR(long error)
{
    return (void *) error;
}

static inline long PTR_ERR(__force const void *ptr)
{
    return (long) ptr;
}

static inline bool IS_ERR(__force const void *ptr)
{
    return IS_ERR_VALUE((unsigned long)ptr);
}

static inline bool IS_ERR_OR_NULL(__force const void *ptr)
{
    return !ptr || IS_ERR_VALUE((unsigned long)ptr);
}

/**
 * ERR_CAST - Explicitly cast an error-valued pointer to another pointer type
 * @ptr: The pointer to cast.
 *
 * Explicitly cast an error-valued pointer to another pointer type in such a
 * way as to make it clear that's what's going on.
 */
static inline void *ERR_CAST(__force const void *ptr)
{
    /* cast away the const */
    return (void *) ptr;
}

static inline int PTR_ERR_OR_ZERO(__force const void *ptr)
{
    if (IS_ERR(ptr))
        return PTR_ERR(ptr);
    else
        return 0;
}

/* Deprecated */
#define PTR_RET(p) PTR_ERR_OR_ZERO(p)

#endif

#endif /* #ifndef __RTT_ADAPTER_LINUX_ERR_H__ */
