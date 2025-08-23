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

#define USB_VBUS_PIN            BANK_PIN(GPIO_BANK2, 12)
#define RT_USING_USB_SWITCH

#define RT_USING_CODEC_ES8388
#ifdef RT_USING_CODEC_ES8388
#define ES8388_I2C_BUS "i2c0"
#define ES8388_I2C_ADDR 0x10
#define ES8388_HPCTL_GPIO_BANK GPIO_BANK3
#define ES8388_HPCTL_GPIO_GPIO GPIO_PIN_A7
#define ES8388_HPCTL_GPIO_GRP  GPIO3
#define ES8388_HPCTL_GPIO_EN   GPIO_HIGH
#endif

#ifdef RT_USING_IT6632X

#define IT6632X_INT_GPIO_BANK     GPIO_BANK3
#define IT6632X_INT_GPIO          GPIO_PIN_B6
#define IT6632X_INT_PIN           BANK_PIN(GPIO_BANK3, 14)
#define IT6632X_INT_PIN_FUNC_GPIO PIN_CONFIG_MUX_FUNC0

#define IT6632X_USE_RXMUTE_PIN
#define IT6632X_RXMUTE_GPIO_BANK     GPIO_BANK3
#define IT6632X_RXMUTE_GPIO          GPIO_PIN_B7
#define IT6632X_RXMUTE_PIN           BANK_PIN(GPIO_BANK3, 15)
#define IT6632X_RXMUTE_PIN_FUNC_GPIO PIN_CONFIG_MUX_FUNC0
#define IT6632X_RXMUTE_UNMUTE        PIN_LOW

#define IT6632X_DEVICE_NAME       "it6632x"

#define IT6632X_I2C_ADDR          (0x58)
#define IT6632X_I2C_DEV           "i2c2"

#endif

#ifdef RT_USING_EP91A7P

#define EP91A7P_INT_GPIO_BANK     GPIO_BANK3
#define EP91A7P_INT_GPIO          GPIO_PIN_B6
#define EP91A7P_INT_PIN           BANK_PIN(GPIO_BANK3, 14)
#define EP91A7P_INT_PIN_FUNC_GPIO PIN_CONFIG_MUX_FUNC0

#define EP91A7P_RXMUTE_GROP          GPIO3
#define EP91A7P_RXMUTE_GPIO_BANK     GPIO_BANK3
#define EP91A7P_RXMUTE_GPIO          GPIO_PIN_B7
#define EP91A7P_RXMUTE_PIN           BANK_PIN(GPIO_BANK3, 15)
#define EP91A7P_RXMUTE_PIN_FUNC_GPIO PIN_CONFIG_MUX_FUNC0
#define EP91A7P_RXMUTE_UNMUTE        PIN_LOW

#define EP91A7P_DEVICE_NAME        "ep91a7p"
#define EP91A7P_I2C_ADDR           (0x61)
#define EP91A7P_I2C_DEV            "i2c2"

#endif

#endif
