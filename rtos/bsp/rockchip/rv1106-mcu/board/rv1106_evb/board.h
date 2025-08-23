/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-08     Steven Liu   first implementation
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "drv_uart.h"
#include "int_mux.h"
#include "riscv_csr_encoding.h"
#include "timer.h"
#include "board_base.h"
#include "hal_base.h"

#define MAX_HANDLERS       NUM_INTERRUPTS
#define INTERRUPT_SEPERATE 8

extern  void *_end;
extern  void *_heap_end;
#define HEAP_BEGIN  &_end
#define HEAP_END    &_heap_end

#endif
