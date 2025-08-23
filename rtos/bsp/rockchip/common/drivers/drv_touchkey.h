/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_touchkey.h
  * @author  Simon Xue
  * @version V0.1
  * @date    28-Feb-2024
  * @brief   touchkey driver
  *
  ******************************************************************************
  */
#ifndef __TOUCHCKEY_H__
#define __TOUCHCKEY_H__

#include <rtthread.h>

#define RT_TOUCHKEY_CTRL_GET_INFO                     (1)

struct rt_touchkey_info
{
    rt_uint32_t    count;                   /* Buffer count */
};

#endif
