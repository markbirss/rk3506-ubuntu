/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-02-07     Cliff Chen   first implementation
 */

#ifndef __BOARD_BASE_H__
#define __BOARD_BASE_H__
#include <rtconfig.h>

void systick_isr(int vector, void *param);
void mpu_init(void);
void swo_console_hook(const char *str, int flush);
void spinlock_init(void);
void rt_hw_board_init(void);
void usb_phy_init(void);
void rt_memory_heap_init(void);
#ifdef RT_USING_CMBACKTRACE
int rt_cm_backtrace_init(void);
#endif
void rt_board_mmc_init(void);

#endif
