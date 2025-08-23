/**
  * Copyright (c) 2019 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    board.h
  * @author  Jason Zhu
  * @version V0.1
  * @date    1-Aug-2019
  * @brief
  *
  ******************************************************************************
  */

#ifndef __BOARD__
#define __BOARD__

#include "drv_uart.h"
#include "int_mux.h"
#include "riscv_csr_encoding.h"
#include "timer.h"

#define MAX_HANDLERS    32
#define INTERRUPT_SEPERATE 8

extern  void *_end;
extern  void *_heap_end;
#define HEAP_BEGIN  &_end
#define HEAP_END    &_heap_end

#endif
