/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Randall zhuo <randall.zhuo@rock-chips.com>
 */

#ifndef _RTT_RTOS_ADAPTER_H
#define _RTT_RTOS_ADAPTER_H

#include <rtdbg.h>

#define __iomem

#define ktime_get rt_tick_get
#define ktime_sub(A, B) ((A)-(B))
#define ktime_us_delta(A, B)  (((A)-(B)) * (1000000/RT_TICK_PER_SECOND))

typedef int irqreturn_t;

#define IRQ_HANDLED 1

#ifndef bool
#define bool unsigned int
#define true (1)
#define false (0)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(ar)              (sizeof(ar)/sizeof(ar[0]))
#endif

#define EINVAL RT_EINVAL
#define ENOMEM RT_ENOMEM
#define ETIMEDOUT RT_ETIMEOUT

#endif // end _RTT_RTOS_ADAPTER_H