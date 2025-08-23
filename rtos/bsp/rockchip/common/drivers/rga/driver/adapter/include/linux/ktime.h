/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Cerf Yu <cerf.yu@rock-chips.com>
 */

#ifndef __RTT_ADAPTER_LINUX_KTIME_H__
#define __RTT_ADAPTER_LINUX_KTIME_H__

#include <time.h>
#include <sys/time.h>

#define ktime_t rt_tick_t

#define ktime_get rt_tick_get
#define ktime_sub(A, B) ((A)-(B))
#define ktime_us_delta(A, B)  (((A)-(B)) * (1000000 / RT_TICK_PER_SECOND))

#endif /* #ifndef __RTT_ADAPTER_LINUX_KTIME_H__ */
