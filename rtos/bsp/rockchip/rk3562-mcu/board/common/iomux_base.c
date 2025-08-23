/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-09     Cliff chen   first implementation
 */

#include "rtdef.h"
#include "iomux.h"
#include "hal_base.h"
#include "hal_bsp.h"

#ifdef RT_USING_I2C0
RT_WEAK void i2c0_m0_iomux_config(void)
{
    /* I2C0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B1 |
                         GPIO_PIN_B2,
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

#ifdef RT_USING_SPI0
RT_WEAK  void spi0_m1_iomux_config(void)
{
    /* SPI0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_B4 |
                         GPIO_PIN_B5 |
                         GPIO_PIN_B7,
                         PIN_CONFIG_MUX_FUNC4);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_C0,
                         PIN_CONFIG_MUX_FUNC4);
}
#endif

/**
 * @brief  Config iomux for RK3562
 */
RT_WEAK void rt_hw_iomux_config(void)
{
}
