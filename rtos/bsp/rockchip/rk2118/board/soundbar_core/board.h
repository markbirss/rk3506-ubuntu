/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-02-07     Cliff Chen   first implementation
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "board_base.h"

#ifdef RT_USING_IT6632X

#define IT6632X_USE_RESET_PIN
#define IT6632X_RST_GPIO_BANK     GPIO_BANK3
#define IT6632X_RST_GPIO          GPIO_PIN_A1
#define IT6632X_RST_PIN           BANK_PIN(GPIO_BANK3, 1)
#define IT6632X_RST_PIN_FUNC_GPIO PIN_CONFIG_MUX_FUNC0
#define IT6632X_RST_ON            PIN_HIGH
#define IT6632X_RST_OFF           PIN_LOW

#define IT6632X_INT_GPIO_BANK     GPIO_BANK3
#define IT6632X_INT_GPIO          GPIO_PIN_A0
#define IT6632X_INT_PIN           BANK_PIN(GPIO_BANK3, 0)
#define IT6632X_INT_PIN_FUNC_GPIO PIN_CONFIG_MUX_FUNC0
#define IT6632X_RXMUTE_UNMUTE        PIN_LOW

#define IT6632X_USE_RXMUTE_PIN
#define IT6632X_RXMUTE_GPIO_BANK     GPIO_BANK3
#define IT6632X_RXMUTE_GPIO          GPIO_PIN_A3
#define IT6632X_RXMUTE_PIN           BANK_PIN(GPIO_BANK3, 3)
#define IT6632X_RXMUTE_PIN_FUNC_GPIO PIN_CONFIG_MUX_FUNC0

#define IT6632X_DEVICE_NAME       "it6632x"

#define IT6632X_I2C_ADDR          (0x58)
#define IT6632X_I2C_DEV           "i2c3"

#endif

#endif
