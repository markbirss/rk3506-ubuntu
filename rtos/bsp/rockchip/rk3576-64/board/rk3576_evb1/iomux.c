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
 * @brief  Config iomux m0 for jtag
 */
RT_WEAK void jtag_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A2 |    // JTAG_TCK_M0
                         GPIO_PIN_A3,     // JTAG_TMS_M0
                         PIN_CONFIG_MUX_FUNC9);
}

/**
 * @brief  Config iomux for RK3576
 */
void rt_hw_iomux_config(void)
{

#ifdef RT_USING_UART
#ifdef RT_USING_UART5
    uart5_m2_iomux_config();
#endif
#endif
    jtag_m0_iomux_config();
}
