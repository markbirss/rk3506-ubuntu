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

#define RT_HW_LCD_POWER_EN_PIN          BANK_PIN(GPIO_BANK0, 4)
#define RT_HW_LCD_POWER_EN_BANK         GPIO_BANK0
#define RT_HW_LCD_POWER_EN_GPIO         GPIO_PIN_A4
#define RT_HW_LCD_POWER_EN_FLAG         PIN_LOW

#define RT_HW_LCD_RESET_PIN             BANK_PIN(GPIO_BANK0, 20)
#define RT_HW_LCD_RESET_BANK            GPIO_BANK0
#define RT_HW_LCD_RESET_GPIO            GPIO_PIN_C4
#define RT_HW_LCD_RESET_FLAG            PIN_LOW

#define USB_HOST_VBUS_PIN               BANK_PIN(GPIO_BANK0, 21)

#define PA_MUTE_GPIO_BANK        GPIO_BANK0
#define PA_MUTE_GPIO             GPIO0
#define PA_MUTE_PIN              GPIO_PIN_A5
#define PA_MUTE_PIN_FUNC_GPIO    PIN_CONFIG_MUX_FUNC0
#define PA_MUTE_SWITCH_ON        GPIO_HIGH
#define PA_MUTE_SWITCH_OFF       GPIO_LOW

#endif
