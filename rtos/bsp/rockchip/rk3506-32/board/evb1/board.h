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

#define RT_HW_LCD_POWER_EN_PIN          BANK_PIN(GPIO_BANK0, 1)
#define RT_HW_LCD_POWER_EN_BANK         GPIO_BANK0
#define RT_HW_LCD_POWER_EN_GPIO         GPIO_PIN_A1
#ifdef  RT_USING_DW_MIPI_DSI
#define RT_HW_LCD_POWER_EN_FLAG         PIN_HIGH
#else
#define RT_HW_LCD_POWER_EN_FLAG         PIN_LOW
#endif

#define RT_HW_LCD_RESET_PIN             BANK_PIN(GPIO_BANK0, 7)
#define RT_HW_LCD_RESET_BANK            GPIO_BANK0
#define RT_HW_LCD_RESET_GPIO            GPIO_PIN_A7
#define RT_HW_LCD_RESET_FLAG            PIN_LOW

#define PA_MUTE_GPIO_BANK               GPIO_BANK1
#define PA_MUTE_GPIO                    GPIO1
#define PA_MUTE_PIN                     GPIO_PIN_C6
#define PA_MUTE_PIN_FUNC_GPIO           PIN_CONFIG_MUX_FUNC0
#define PA_MUTE_SWITCH_ON               GPIO_HIGH
#define PA_MUTE_SWITCH_OFF              GPIO_LOW

#define USB_OTG_HOST0_VBUS_PIN          BANK_PIN(GPIO_BANK1, 20)
#define USB_OTG_HOST1_VBUS_PIN          BANK_PIN(GPIO_BANK1, 24)

#define TOUCH_IRQ_PIN                   BANK_PIN(GPIO_BANK0, 6)
#define TOUCH_RST_PIN                   BANK_PIN(GPIO_BANK0, 7)
#define TOUCH_I2C_DEV                   "i2c2"
#define TOUCH_DEV_NAME                  "gtxx"
#endif
