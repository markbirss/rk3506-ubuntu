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
#include "board.h"

/**
 * @brief  Config io domian for board of RK3506_evb1
 */

void rt_hw_iodomain_config(void)
{
}

void sai1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B0 |  // mclk
                         GPIO_PIN_B1 |  // sclk
                         GPIO_PIN_B2 |  // lrck
                         GPIO_PIN_B3 |  // sdi0
                         GPIO_PIN_B4,   // sdo0
                         PIN_CONFIG_MUX_FUNC1);

    /* set sai1 mclk output enable */
    WRITE_REG_MASK_WE(GRF_PMU->SOC_CON1,
                      GRF_PMU_SOC_CON1_SAI1_MCLK_OEN_MASK,
                      (1 << GRF_PMU_SOC_CON1_SAI1_MCLK_OEN_SHIFT));
}

void spkmute_iomux_config(void)
{
    /* PA_MUTE is GPIO1_C6 */
    HAL_PINCTRL_SetIOMUX(PA_MUTE_GPIO_BANK,
                         PA_MUTE_PIN,
                         PA_MUTE_PIN_FUNC_GPIO);

    HAL_GPIO_SetPinDirection(PA_MUTE_GPIO, PA_MUTE_PIN, GPIO_OUT);
}

void touch_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A6 |  // TOUCH_INT
                         GPIO_PIN_A7 |  // TOUCH_RST
                         GPIO_PIN_A1,   // PANNEL_POWER(LCDC & TOUCH)
                         PIN_CONFIG_MUX_FUNC0);

    /* PANNEL & TOUCH POWER_EN ON */
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_A1, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A1, GPIO_HIGH);
}

void flexbus_spi_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1, GPIO_PIN_B2, PIN_CONFIG_MUX_FUNC5); /* CSN */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1, GPIO_PIN_C1, PIN_CONFIG_MUX_FUNC3); /* CLK */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_D3 |  // D0
                         GPIO_PIN_D2 |  // D1
                         GPIO_PIN_D1 |  // D2
                         GPIO_PIN_D0 |  // D3
                         GPIO_PIN_C7 |  // D4
                         GPIO_PIN_C6 |  // D5
                         GPIO_PIN_C5 |  // D6
                         GPIO_PIN_C4,   // D7
                         PIN_CONFIG_MUX_FUNC3);
}

/**
 * @brief  Config iomux for RK3506
 */
void rt_hw_iomux_config(void)
{
#if defined(RT_USING_UART0)
    uart0_iomux_config();
#endif

#if defined(RT_USING_UART4)
    uart4_iomux_config();
#endif

#if defined(RT_USING_I2C0)
    i2c0_iomux_config();
#endif

#if defined(RT_USING_I2C2)
    i2c2_iomux_config();
#endif

#ifdef RT_USING_DSMC_HOST
    dsmc_host_iomux_config();
#endif

#ifdef RT_USING_DSMC_SLAVE
    dsmc_slave_iomux_config();
#endif

#if defined(RT_USING_SDIO0)
    emmc_iomux_config();
#endif

#ifdef RT_USING_GMAC0
    rmii0_iomux_config();
#endif

#ifdef RT_USING_SAI1
    sai1_iomux_config();
#endif

#ifdef RT_USING_CODEC
    spkmute_iomux_config();
#endif

#if defined(RT_USING_BACKLIGHT)
    pwm0_iomux_config();
#endif

#if defined(RT_USING_VOP_RGB)
    lcdc_iomux_config();
#endif

#ifdef RT_USING_TOUCH
    touch_iomux_config();
#endif

#if defined(RT_USING_FLEXBUS)
#if defined(RT_USING_FLEXBUS_ADC)
    flexbus_adc_iomux_config();
#endif
#if defined(RT_USING_FLEXBUS_DAC)
    flexbus_dac_iomux_config();
#endif
#ifdef RT_USING_FLEXBUS_SPI
    flexbus_spi_iomux_config();
#endif
#endif
}
