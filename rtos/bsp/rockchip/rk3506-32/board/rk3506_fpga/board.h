/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-09     Cliff Chen   first implementation
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "board_base.h"
#include "hal_base.h"

#define RT_HW_LCD_POWER_EN_PIN          BANK_PIN(GPIO_BANK2, 6)
#define RT_HW_LCD_POWER_EN_BANK         GPIO_BANK2
#define RT_HW_LCD_POWER_EN_GPIO         GPIO_PIN_A6
#define RT_HW_LCD_POWER_EN_FLAG         PIN_LOW

#define RT_HW_LCD_RESET_PIN             BANK_PIN(GPIO_BANK1, 1)
#define RT_HW_LCD_RESET_BANK            GPIO_BANK1
#define RT_HW_LCD_RESET_GPIO            GPIO_PIN_A1
#define RT_HW_LCD_RESET_FLAG            PIN_LOW
#endif
