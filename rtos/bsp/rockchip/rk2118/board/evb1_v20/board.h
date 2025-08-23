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
#define ES8388_HPCTL_GPIO_BANK GPIO_BANK0
#define ES8388_HPCTL_GPIO_GPIO GPIO_PIN_A1
#define ES8388_HPCTL_GPIO_GRP  GPIO0
#define ES8388_HPCTL_GPIO_EN   GPIO_HIGH
#endif

#ifdef RT_USING_IT6632X

#define IT6632X_INT_GPIO_BANK     GPIO_BANK4
#define IT6632X_INT_GPIO          GPIO_PIN_D2
#define IT6632X_INT_PIN           BANK_PIN(GPIO_BANK4, 26)
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

#define EP91A7P_INT_GPIO_BANK     GPIO_BANK4
#define EP91A7P_INT_GPIO          GPIO_PIN_D2
#define EP91A7P_INT_PIN           BANK_PIN(GPIO_BANK4, 26)
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

#ifdef RT_USING_AIP1640
#define AIP1640_CLK_GPIO_BANKC       GPIO3
#define AIP1640_CLK_GPIO             GPIO_PIN_B2
#define AIP1640_DATA_GPIO_BANKC      GPIO3
#define AIP1640_DATA_GPIO            GPIO_PIN_A0
#define LED_PWR_GPIO_BANK            GPIO3
#define LED_PWR_GPIO                 GPIO_PIN_A2
#endif

#ifdef RT_USING_USB_HOST
#define USB_PWR_GPIO_GROP            GPIO3
#define USB_PWR_GPIO                 GPIO_PIN_B6
#endif

#define BT_CTRL_GPIO_GROP            GPIO3
#define BT_CTRL_GPIO                 GPIO_PIN_C5

#ifdef RT_USING_GMAC0
#define PHY_CLK_GPIO_GROP            GPIO_BANK2
#define PHY_CLK_GPIO                 GPIO_PIN_A6
#define PHY_RST_GPIO_GROP            GPIO0
#define PHY_RST_GPIO                 GPIO_PIN_A0
#endif

#define SAI4_LRCK_GPIO_BANK          GPIO_BANK4
#define SAI4_LRCK_GPIO_GROP          GPIO4
#define SAI4_LRCK_GPIO               GPIO_PIN_C1

#define SAI4_SCLK_GPIO_BANK          GPIO_BANK4
#define SAI4_SCLK_GPIO_GROP          GPIO4
#define SAI4_SCLK_GPIO               GPIO_PIN_C0

#define SAI4_SDI0_GPIO_BANK          GPIO_BANK4
#define SAI4_SDI0_GPIO_GROP          GPIO4
#define SAI4_SDI0_GPIO               GPIO_PIN_C6

#define SAI4_SDI1_GPIO_BANK          GPIO_BANK4
#define SAI4_SDI1_GPIO_GROP          GPIO4
#define SAI4_SDI1_GPIO               GPIO_PIN_C7

#define SAI4_SDI2_GPIO_BANK          GPIO_BANK4
#define SAI4_SDI2_GPIO_GROP          GPIO4
#define SAI4_SDI2_GPIO               GPIO_PIN_D0

#define SAI4_SDI3_GPIO_BANK          GPIO_BANK4
#define SAI4_SDI3_GPIO_GROP          GPIO4
#define SAI4_SDI3_GPIO               GPIO_PIN_D1

#endif
