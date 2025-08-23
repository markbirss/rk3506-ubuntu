/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    pwm_bl.c
  * @version V0.1
  * @brief   wfd pwm backlight driver
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-08-15     Damon Ding      first implementation
  *
  ******************************************************************************
  */

#ifndef __PWM_BL_H__
#define __PWM_BL_H__

#include <rtthread.h>
#include <rtdevice.h>

struct pwm_bl_config
{
    int channel;
    int period;
    int duty;
    int polarity;
    char dev_name[8];
};

int pwm_bl_enable(struct pwm_bl_config *bl_config);
int pwm_bl_disable(struct pwm_bl_config *bl_config);

#endif
