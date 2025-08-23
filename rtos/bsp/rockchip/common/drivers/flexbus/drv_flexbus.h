/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_flexbus.h
  * @author  Wesley Yao
  * @version V1.0
  * @date    19-July-2024
  * @brief   flexbus driver
  *
  ******************************************************************************
  */

#ifndef __FLEXBUS_H__
#define __FLEXBUS_H__

#include <rtthread.h>

#define ROUND_UP(x, align) ((x + (align - 1)) & ~(align - 1))

struct flexbus_dev;

struct flexbus_dev
{
    struct rt_device parent;
    const struct HAL_FLEXBUS_DEV *hal_dev;
    struct FLEXBUS_HANDLE instance;
    void *fb0_data;
    void *fb1_data;
    void (*fb0_isr)(struct flexbus_dev *flexbus, rt_uint32_t isr);
    void (*fb1_isr)(struct flexbus_dev *flexbus, rt_uint32_t isr);
};

#endif
