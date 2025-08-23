/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_remotectl.c
  * @author  Zhenke Fan
  * @version V0.1
  * @date    25-02-2024
  * @brief   remotectl driver
  *
  ******************************************************************************
  */
#ifndef __REMOTECTL_H__
#define __REMOTECTL_H__

#ifdef RT_USING_PWM_REMOTECTL
#include <rtdevice.h>
#include <rtthread.h>

#define RT_PWM_TIME_PRE_MIN_HIGH                4000
#define RT_PWM_TIME_PRE_MAX_HIGH                5000

#define RT_PWM_TIME_PRE_MIN_LOW                 8000
#define RT_PWM_TIME_PRE_MAX_LOW                 10000

#define RT_PWM_TIME_BIT0_MIN                    1400
#define RT_PWM_TIME_BIT0_MAX                    1900

#define RT_PWM_TIME_BIT1_MIN                    400
#define RT_PWM_TIME_BIT1_MAX                    700

#define RT_PWM_TIME_DATA_OVER_MIN               39000
#define RT_PWM_TIME_DATA_OVER_MAX               42000

#define RK_PWM_TIME_RPT_MIN                     1800
#define RK_PWM_TIME_RPT_MAX                     3000

#define RT_PWM_TIME_SEQ1_MIN                    95000
#define RT_PWM_TIME_SEQ1_MAX                    98000

#define RT_REMOTECTL_INIT                       (1)
#define RT_REMOTECTL_GET_INFO                   (2)

#define RT_PWM_CLK                              0x20C020C0
#define RT_PWM_FILTER_VAL                       0xFFFF03F1
#define RT_PWRMATCH_CTRL                        0x00050005
#define RT_PWRMATCH_CTRL_DISABLE                0x00050000
#define RT_PWRMATCH_INT_ENABLE                  0x00020002
#define RT_PWRMATCH_INT_DISABLE                 0x00020000

struct remotectl_pwm_info
{
    char *name;
    uint32_t channel;
    uint32_t pwrmatch_value;
    uint32_t rtl_usercode;
};

struct rt_remotectl_keycode
{
    rt_uint32_t keycode;
    rt_uint32_t press;
};

struct rt_remotectl_drvdata
{
    rt_uint32_t state;
    rt_uint32_t count;
    rt_uint32_t code;
    rt_uint32_t usercode;
    struct rt_remotectl_keycode key;
};

typedef enum _RMC_STATE
{
    RMC_IDLE,
    RMC_PRELOAD = 0,
    RMC_USERCODE,
    RMC_GETDATA,
    RMC_SEQUENCE,
} eRMC_STATE;

void remotectl_get_pwm_period(const char *name, uint32_t ch, uint32_t freq, uint32_t low, uint32_t high);

#endif
#endif
