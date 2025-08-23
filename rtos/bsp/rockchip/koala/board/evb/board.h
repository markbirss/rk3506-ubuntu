/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-01     Frank Wang   board.h
 *
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <soc.h>

extern unsigned char __heap_start;
extern unsigned char __heap_end;

#define HEAP_BEGIN    (void*)&__heap_start
#define HEAP_END      (void*)&__heap_end

void rt_hw_board_init(void);

#endif
