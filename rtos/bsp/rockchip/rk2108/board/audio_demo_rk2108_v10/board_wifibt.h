/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    board_wlan.h
  * @author  aaron sun
  * @version V0.1
  * @date    25-JUN-2019
  * @brief   wice for rtt wlan struct interface
  *
  ******************************************************************************
  */
#ifndef __BOARD_WIFIBT_H__
#define __BOARD_WIFIBT_H__

#include "board_wifibt_base.h"

#define WIFI_POWER_ON_GPIO_BANK GPIO_BANK0
#define WIFI_POWER_ON_GPIO_PIN  GPIO_PIN_D6
#define WIFI_POWER_ON_GPIO_GRP  GPIO0

#define WIFI_WAKEUP_GPIO_BANK GPIO_BANK0
#define WIFI_WAKEUP_GPIO_PIN  GPIO_PIN_D7
#define WIFI_WAKEUP_GPIO_GRP  GPIO0
#define WIFI_WAKEUP_GPIO_PIN_NUM  31

#define BT_GPIO_PORT                (GPIO_BANK1)
#define BT_POWER_GPIO_PIN           GPIO_PIN_A5
#define BT_HOST_RX_PIN              GPIO_PIN_A0
#define BT_HOST_TX_PIN              GPIO_PIN_A1
#define BT_HOST_CTS_PIN             GPIO_PIN_A2
#define BT_HOST_RTS_PIN             GPIO_PIN_A3
#define BT_32K_CLK_OUT              GPIO_PIN_A7
#define BT_GPIO_PORT_BASE           GPIO1
#define BT_PIN_CONFIG_MUX_FUNC_NUM  PIN_CONFIG_MUX_FUNC4

#endif
