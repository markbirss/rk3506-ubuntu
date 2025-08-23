/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * @file    rtos_adapter.h
 * @author  cerf yu
 * @version V0.1
 * @date    23-Jul-2024
 * @brief   rga driver
 *
 ******************************************************************************
 */

#ifndef __RTT_ADAPTER_H__
#define __RTT_ADAPTER_H__

#include <rtdevice.h>
#include <rtthread.h>
#include <rtdbg.h>

#define unlikely
#define udelay rt_hw_us_delay

#define pr_err rt_kprintf
#define pr_info rt_kprintf
#define pr_debug rt_kprintf

#ifndef bool
#define bool unsigned int
#define true (1)
#define false (0)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(ar)              (sizeof(ar)/sizeof(ar[0]))
#endif

#undef ALIGN
#define ALIGN(val, align) (((val) + ((align) - 1)) & ~((align) - 1))

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define u64_to_user_ptr(x) ({ \
    (void __user *)(uintptr_t)(x); \
})
#define ptr_to_u64(ptr) ((uint64_t)(uintptr_t)(ptr))
#define u64_to_ptr(var) ((void *)(uintptr_t)(var))

/* completion */
typedef struct rt_completion wait_queue_head_t;
#define init_waitqueue_head rt_completion_init
#define wake_up rt_completion_done
#define wait_event_timeout(completion,flag,t) rt_completion_wait((struct rt_completion *)&completion,(t) * RT_TICK_PER_SECOND / 1000)

#include "adapter/include/linux/types.h"
#include "adapter/include/linux/ktime.h"
#include "adapter/include/linux/irqreturn.h"
#include "adapter/include/linux/uaccess.h"
#include "adapter/include/linux/bitops.h"
#include "adapter/include/linux/slab.h"
#include "adapter/include/linux/lock.h"
#include "adapter/include/linux/err.h"
#include "adapter/include/linux/ioctl.h"

#define list_empty rt_list_isempty
#define list_first_entry rt_list_first_entry
#define list_for_each_entry rt_list_for_each_entry
#define list_add_tail rt_list_insert_after
#define list_add rt_list_insert_before
#define list_del rt_list_remove
#define list_del_init rt_list_remove

#endif /* #ifndef __RTT_ADAPTER_H__ */
