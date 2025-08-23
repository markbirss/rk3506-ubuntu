/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    board_wifibt.h
  * @author  aaron sun
  * @version V0.1
  * @date    25-JUN-2019
  * @brief   wice for rtt wlan struct interface
  *
  ******************************************************************************
  */
#ifndef __BOARD_WIFIBT_H__
#define __BOARD_WIFIBT_H__

#include "rtthread.h"
#include "rtdevice.h"
#include "hal_base.h"
#include "hal_def.h"

#define WIFI_POWER_ON_GPIO_BANK     GPIO_BANK0
#define WIFI_POWER_ON_GPIO_PIN      GPIO_PIN_A3
#define WIFI_POWER_ON_GPIO_GRP      GPIO0

#define WIFI_WAKEUP_GPIO_BANK       GPIO_BANK0
#define WIFI_WAKEUP_GPIO_PIN        GPIO_PIN_A1
#define WIFI_WAKEUP_GPIO_GRP        GPIO0
#define WIFI_WAKEUP_GPIO_PIN_NUM    1

#define BT_GPIO_PORT                (GPIO_BANK0)
#define BT_POWER_GPIO_PIN           GPIO_PIN_B1
#define BT_HOST_RX_PIN              GPIO_PIN_C7
#define BT_HOST_TX_PIN              GPIO_PIN_D0
#define BT_HOST_CTS_PIN             GPIO_PIN_D1
#define BT_HOST_RTS_PIN             GPIO_PIN_D2
#define BT_32K_CLK_OUT              GPIO_PIN_A7
#define BT_GPIO_PORT_BASE           GPIO0
#define BT_PIN_CONFIG_MUX_FUNC_NUM  PIN_CONFIG_MUX_FUNC1

int rk_wifi_host_reg_on(void);
void rk_wifi_host_wake_register(void (*call_back)(void *));
void rk_wifi_host_wake_enable();
void rk_wifi_host_wake_disable();

uint32_t rk_bt_rst(void);
uint32_t rk_bt_power_down(void);
void rk_bt_init_gpio(void);

#endif
