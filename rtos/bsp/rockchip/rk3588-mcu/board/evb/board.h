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

#ifndef __BOARD_H__
#define __BOARD_H__

extern  void *_end;
extern  void *_heap_end;
#define HEAP_BEGIN  &_end
#define HEAP_END    &_heap_end

#endif
