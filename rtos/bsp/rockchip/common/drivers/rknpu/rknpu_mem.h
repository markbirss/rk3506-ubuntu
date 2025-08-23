/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Randall zhuo <randall.zhuo@rock-chips.com>
 */

#ifndef __RTT_RKNPU_MEM_H_
#define __RTT_RKNPU_MEM_H_

#include <rtthread.h>

void *rt_malloc_rknpu(rt_size_t size);
void rt_free_rknpu(void *ptr);

#endif /* __RTT_RKNPU_MEM_H_ */
