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
#ifndef __BOARD_WIFIBT_BASE_H__
#define __BOARD_WIFIBT_BASE_H__

#include "rtthread.h"
#include "rtdevice.h"
#include "hal_base.h"
#include "hal_def.h"

int rk_wifi_host_reg_on(void);
void rk_wifi_host_wake_register(void (*call_back)(void *));
void rk_wifi_host_wake_enable();
void rk_wifi_host_wake_disable();

uint32_t rk_bt_rst(void);
uint32_t rk_bt_power_down(void);
void rk_bt_init_gpio(void);

#endif
