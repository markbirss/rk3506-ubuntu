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

#ifdef RT_USING_SYSTICK
#define TICK_IRQn  SysTick_IRQn
#else
#define TICK_TIMER TIMER0
#define TICK_IRQn  TIMER0_IRQn
#endif

#endif
