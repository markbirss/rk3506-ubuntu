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
#define USB_VBUS_PIN            BANK_PIN(GPIO_BANK1, 17)
#endif

#ifdef RT_USING_TOUCH
#undef  TOUCH_IRQ_PIN
#define TOUCH_IRQ_PIN           BANK_PIN(GPIO_BANK0, 28)
#undef  TOUCH_RST_PIN
#define TOUCH_RST_PIN           BANK_PIN(GPIO_BANK0, 27)
#undef  TOUCH_I2C_DEV
#define TOUCH_I2C_DEV           "i2c0"
#undef  TOUCH_DEV_NAME
#define TOUCH_DEV_NAME          "s3706"
#endif

#define RT_HW_LCD_GPIO_TRIGGER_PIN    BANK_PIN(GPIO_BANK1, 16)

#endif
