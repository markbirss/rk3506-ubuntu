/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-12     Steven Liu   first implementation
 */

#include "rtdef.h"
#include "iomux.h"
#include "hal_base.h"
#include "drivers/pin.h"

/**
 * @brief  Config iomux m0 for UART0
 */
RT_WEAK void uart0_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D4,
                         PIN_CONFIG_MUX_FUNC9);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D5,
                         PIN_CONFIG_MUX_FUNC9);
}

/**
 * @brief  Config iomux m2 for UART5
 */
RT_WEAK void uart5_m2_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A4,
                         PIN_CONFIG_MUX_FUNC9);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A5,
                         PIN_CONFIG_MUX_FUNC9);
}

/**
 * @brief  Config iomux for RK3576
 */
RT_WEAK void rt_hw_iomux_config(void)
{

#ifdef RT_USING_UART
#ifdef RT_USING_UART5
    uart5_m2_iomux_config();
#endif
#endif
}
