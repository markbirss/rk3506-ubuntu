/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    board_wlan.c
  * @author  aaron sun
  * @version V0.1
  * @date    25-JUN-2019
  * @brief   wice for rtt wlan struct interface
  *
  ******************************************************************************
  */
#include "rtthread.h"
#include "rtdevice.h"
#include "hal_base.h"

int wice_host_reg_on(void)
{
    rt_kprintf("\n init wlan....");
    /* register power on */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4, GPIO_PIN_C0, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO4, GPIO_PIN_C0, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO4, GPIO_PIN_C0, GPIO_LOW);
    HAL_GPIO_SetPinLevel(GPIO4, GPIO_PIN_C0, GPIO_HIGH);
    return 0;
}
INIT_PREV_EXPORT(wice_host_reg_on);

void wice_host_wake_register(void (*call_back)(void *))
{
    rt_err_t ret;
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4, GPIO_PIN_C1, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO4, GPIO_PIN_C1, GPIO_IN);
    ret = rt_pin_attach_irq(BANK_PIN(GPIO_BANK4, 17), PIN_IRQ_MODE_RISING, call_back, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_pin_irq_enable(BANK_PIN(GPIO_BANK4, 17), PIN_IRQ_ENABLE);
    RT_ASSERT(ret == RT_EOK);
}

void wice_host_wake_enable()
{
    rt_err_t ret;
    ret = rt_pin_irq_enable(BANK_PIN(GPIO_BANK4, 17), PIN_IRQ_ENABLE);
    RT_ASSERT(ret == RT_EOK);
}

void wice_host_wake_disable()
{
    rt_err_t ret;
    ret = rt_pin_irq_enable(BANK_PIN(GPIO_BANK4, 17), PIN_IRQ_DISABLE);
    RT_ASSERT(ret == RT_EOK);
}