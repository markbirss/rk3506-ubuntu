/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-06-01     tyustli     the first version
 */

#ifndef __ST1232_H_
#define __ST1232_H_
#include <rtthread.h>
#include "touch.h"

/* GTxx touch HW pin define, these pins must defined in board config */
#ifndef TOUCH_IRQ_PIN
#define TOUCH_IRQ_PIN   0
#endif

#ifndef TOUCH_RST_PIN
#define TOUCH_RST_PIN   0
#endif

#ifndef TOUCH_I2C_DEV
#define TOUCH_I2C_DEV   0
#endif

#define ST1232_DEVICE_ADDRESS   (0x70)

#define REG_STATUS              0x01    /* Device Status | Error Code */

#define STATUS_NORMAL           0x00
#define STATUS_INIT             0x01
#define STATUS_ERROR            0x02
#define STATUS_AUTO_TUNING      0x03
#define STATUS_IDLE             0x04
#define STATUS_POWER_DOWN       0x05

#define ERROR_NONE              0x00
#define ERROR_INVALID_ADDRESS   0x10
#define ERROR_INVALID_VALUE     0x20
#define ERROR_INVALID_PLATFORM  0x30

#define REG_XY_RESOLUTION       0x04
#define REG_XY_COORDINATES      0x12
#define ST_TS_MAX_FINGERS       5

#endif
