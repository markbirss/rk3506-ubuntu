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
 * @brief  Config io domian for board of rk3588 evb1
 */

void rt_hw_iodomain_config(void)
{
}

void i2s0_8ch_m0_iomux_config(void)
{
    /* I2S0 8CH */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C2 |  // I2S0_MCLK
                         GPIO_PIN_C3 |  // I2S0_SCLKTX
                         GPIO_PIN_C4 |  // I2S0_SCLKRX
                         GPIO_PIN_C5 |  // I2S0_LRCK_TX
                         GPIO_PIN_C6 |  // I2S0_LRCK_RX
                         GPIO_PIN_C7 |  // I2S0_SDO0
                         GPIO_PIN_D0 |  // I2S0_SDO1
                         GPIO_PIN_D1 |  // I2S0_SDO2
                         GPIO_PIN_D2,   // I2S0_SDO3
                         PIN_CONFIG_MUX_FUNC1);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_D3 |  // I2S0_SDI1
                         GPIO_PIN_D4,   // I2S0_SDI0
                         PIN_CONFIG_MUX_FUNC2);
}

void i2s1_8ch_m0_iomux_config(void)
{
    /* I2S1 8CH */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_A0 |  // I2S1_MCLK_M0
                         GPIO_PIN_A1 |  // I2S1_SCLKTX_M0
                         GPIO_PIN_A2 |  // I2S1_LRCKTX_M0
                         GPIO_PIN_A3 |  // I2S1_SCLKRX_M0
                         GPIO_PIN_A4 |  // I2S1_LRCKRX_M0
                         GPIO_PIN_A5 |  // I2S1_SDI0_M0
                         GPIO_PIN_A6 |  // I2S1_SDI1_M0
                         GPIO_PIN_A7 |  // I2S1_SDI2_M0
                         GPIO_PIN_B0 |  // I2S1_SDI3_M0
                         GPIO_PIN_B1 |  // I2S1_SDO0_M0
                         GPIO_PIN_B2 |  // I2S1_SDO1_M0
                         GPIO_PIN_B3 |  // I2S1_SDO2_M0
                         GPIO_PIN_B4,   // I2S1_SDO3_M0
                         PIN_CONFIG_MUX_FUNC3);
}

/**
 * @brief  Config iomux for RK3588
 */
void rt_hw_iomux_config(void)
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
#ifdef RT_USING_I2STDM
#ifdef RT_USING_I2STDM0
    i2s0_8ch_m0_iomux_config();
#endif
#ifdef RT_USING_I2STDM1
    i2s1_8ch_m0_iomux_config();
#endif
#endif
}
