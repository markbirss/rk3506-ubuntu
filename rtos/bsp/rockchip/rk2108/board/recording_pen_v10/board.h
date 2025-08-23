/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-10     Cliff      add board.h to this bsp
 *
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "board_base.h"

#ifdef RT_USING_BACKLIGHT
#undef LCD_BACKLIGHT_PWM
#define LCD_BACKLIGHT_REGULATOR PWR_ID_VCC_3V0_BL
#define LCD_BACKLIGHT_MIN_VOLT  2500000
#define LCD_BACKLIGHT_MAX_VOLT  3300000
#endif

#undef AUDIOPWM_PA_PIN
#define AUDIOPWM_PA_PIN           GPIO_PIN_C5

#ifdef USB_VBUS_PIN
#undef USB_VBUS_PIN
#define RT_USING_USB_PMIC_VBUS
#endif

#ifdef AUDIOPWM_PA_PIN
#undef AUDIOPWM_PA_PIN
#define AUDIOPWM_PA_PIN GPIO_PIN_C5
#endif

#endif
