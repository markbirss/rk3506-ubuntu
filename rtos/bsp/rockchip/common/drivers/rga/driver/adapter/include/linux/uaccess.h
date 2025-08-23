/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Cerf Yu <cerf.yu@rock-chips.com>
 */

#ifndef __RTT_ADAPTER_LINUX_UACCESS_H__
#define __RTT_ADAPTER_LINUX_UACCESS_H__

#include <rtdevice.h>

static inline unsigned long copy_from_user(void *to, const void __user *from, unsigned long n)
{
    if ((to == NULL) || (from == NULL) || (n == 0))
    {
        return n;
    }

    // void *kaddr = drv_shmpool_get_kernel_logical(current->pid, from, n);
    /* yqw add */
    const void *kaddr = from;

    if (kaddr)
    {
        rt_memcpy(to, kaddr, n);
        return 0;
    }

    return n;
}

static inline unsigned long
copy_to_user(void __user *to, const void *from, unsigned long n)
{
    if ((to == NULL) || (from == NULL) || (n == 0))
    {
        return n;
    }

    // void *kaddr = drv_shmpool_get_kernel_logical(current->pid, to, n);
    /* yqw add */
    void *kaddr = to;

    if (kaddr)
    {
        rt_memcpy(kaddr, from, n);
        return 0;
    }

    return n;
}

#endif /* #ifndef __RTT_ADAPTER_LINUX_UACCESS_H__ */
