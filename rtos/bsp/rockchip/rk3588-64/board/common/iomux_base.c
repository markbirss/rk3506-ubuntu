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

/**
 * @brief  Config iomux m0 for UART2
 */
RT_WEAK void uart2_m0_iomux_config(void)
{
    /* UART2 M0 RX-0D0 TX-0D1 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B5 |
                         GPIO_PIN_B6,
                         PIN_CONFIG_MUX_FUNC10);
}

/**
 * @brief  Config iomux m0 for UART5
 */
RT_WEAK void uart5_m0_iomux_config(void)
{
    /* UART5 M0 RX-0D0 TX-0D1 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_D4 |
                         GPIO_PIN_D5,
                         PIN_CONFIG_MUX_FUNC10);
}


/**
 * @brief  Config io domian for board of rk3588 evb1
 */
RT_WEAK void rt_hw_iodomain_config(void)
{
}

/**
 * @brief  Config iomux for RK3588
 */
RT_WEAK void rt_hw_iomux_config(void)
{
    rt_hw_iodomain_config();

#ifdef RT_USING_UART
#ifdef RT_USING_UART2
    uart2_m0_iomux_config();
#endif
#ifdef RT_USING_UART5
    uart5_m0_iomux_config();
#endif
#endif
}
