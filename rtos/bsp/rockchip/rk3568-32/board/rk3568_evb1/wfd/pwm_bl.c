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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <fcntl.h>

#include "pwm_bl.h"

int pwm_bl_enable(struct pwm_bl_config *bl_config)
{
    static struct rt_device_pwm *pwm_device = RT_NULL;
    int ret;

    pwm_device = (struct rt_device_pwm *)rt_device_find(bl_config->dev_name);
    if (pwm_device == RT_NULL)
    {
        rt_kprintf("failed to find <%s> for backlight\n", bl_config->dev_name);
        return -EINVAL;
    }

    ret = rt_pwm_set(pwm_device, bl_config->channel, bl_config->period, bl_config->duty);
    if (ret)
    {
        rt_kprintf("failed to config <%s> for backlight\n", bl_config->dev_name);
        return -EINVAL;
    }

    ret = rt_pwm_enable(pwm_device, bl_config->channel);
    if (ret)
    {
        rt_kprintf("failed to enable <%s> for backlight\n", bl_config->dev_name);
        return -EINVAL;
    }

    return 0;
}

int pwm_bl_disable(struct pwm_bl_config *bl_config)
{
    static struct rt_device_pwm *pwm_device = RT_NULL;
    int ret;

    pwm_device = (struct rt_device_pwm *)rt_device_find(bl_config->dev_name);
    if (pwm_device == RT_NULL)
    {
        rt_kprintf("failed to find <%s> for backlight\n", bl_config->dev_name);
        return -EINVAL;
    }

    ret = rt_pwm_disable(pwm_device, bl_config->channel);
    if (ret)
    {
        rt_kprintf("failed to disable <%s> for backlight\n", bl_config->dev_name);
        return -EINVAL;
    }

    return 0;
}
