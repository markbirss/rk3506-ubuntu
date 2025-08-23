/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "board_base.h"

#ifdef USB_VBUS_PIN
#undef USB_VBUS_PIN
#define USB_VBUS_PIN            BANK_PIN(GPIO_BANK1, 24)
#endif

#ifdef RT_USING_TOUCH
#undef  TOUCH_IRQ_PIN
#define TOUCH_IRQ_PIN           BANK_PIN(GPIO_BANK0, 3)
#undef  TOUCH_RST_PIN
#define TOUCH_RST_PIN           BANK_PIN(GPIO_BANK0, 8)
#undef  TOUCH_I2C_DEV
#define TOUCH_I2C_DEV           "i2c0"
#undef  TOUCH_DEV_NAME
#define TOUCH_DEV_NAME          "cstxx"
#endif

#ifdef RT_USING_AUDIOPWM
#undef AUDIOPWM_PA_GPIO_BANK
#define AUDIOPWM_PA_GPIO_BANK     GPIO_BANK1
#undef AUDIOPWM_PA_GPIO
#define AUDIOPWM_PA_GPIO          GPIO1
#undef AUDIOPWM_PA_PIN
#define AUDIOPWM_PA_PIN           GPIO_PIN_A4
#endif

#define RT_HW_LCD_GPIO_TRIGGER_PIN    BANK_PIN(GPIO_BANK1, 1)
#define RT_HW_LCD_GPIO_TRIGGER_BANK   GPIO_BANK1
#define RT_HW_LCD_GPIO_TRIGGER_GPIO   GPIO_PIN_A1


#endif
