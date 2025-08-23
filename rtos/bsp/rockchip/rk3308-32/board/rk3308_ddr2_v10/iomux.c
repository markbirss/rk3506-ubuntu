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
 * @brief  Config io domian for board of rk3308_ddr2p116sd4_v10
 */

void rt_hw_iodomain_config(void)
{
    /* VCC IO 2 voltage select 1v8 */
    GRF->SOC_CON0 = (1 << GRF_SOC_CON0_IO_VSEL2_SHIFT) |
                    (GRF_SOC_CON0_IO_VSEL2_MASK << 16);
}

void spkmute_iomux_config(void)
{
    /* PA_MUTE is GPIO0_A5 */
    HAL_PINCTRL_SetIOMUX(PA_MUTE_GPIO_BANK,
                         PA_MUTE_PIN,
                         PA_MUTE_PIN_FUNC_GPIO);

    HAL_GPIO_SetPinDirection(PA_MUTE_GPIO, PA_MUTE_PIN, GPIO_OUT);
}

/**
 * @brief  Config iomux for RK3308
 */
void rt_hw_iomux_config(void)
{
    rt_hw_iodomain_config();

#ifdef RT_USING_UART0
    uart0_m0_iomux_config();
#endif

#ifdef RT_USING_UART1
    uart1_m0_iomux_config();
#endif

#ifdef RT_USING_UART2
    uart2_m1_iomux_config();
#endif

#ifdef RT_USING_UART3
    uart3_m1_iomux_config();
#endif

#ifdef RT_USING_UART4
    uart4_m0_iomux_config();
#endif

#ifdef RT_USING_I2C0
    i2c0_m0_iomux_config();
#endif

#ifdef RT_USING_I2C1
    i2c1_m0_iomux_config();
#endif

#ifdef RT_USING_I2C2
    i2c2_m0_iomux_config();
#endif

#ifdef RT_USING_SPI0
    spi0_m0_iomux_config();
#endif

#ifdef RT_USING_SPI1
    spi1_m0_iomux_config();
#endif

#ifdef RT_USING_SPI2
    spi2_m0_iomux_config();
#endif

#ifdef RT_USING_SDIO0
    emmc_iomux_config();
#endif

#ifdef RT_USING_GMAC
#ifdef RT_USING_GMAC0
    gmac0_m1_iomux_config();
#endif
#endif

#ifdef RT_USING_AUDIO_CARD_I2S0
    i2s0_8ch_m0_iomux_config();
#endif

    spkmute_iomux_config();

#ifdef RT_USING_BACKLIGHT
    pwm0_ch1_iomux_config();
#endif

#ifdef RT_USING_VOP
    lcdc_ctrl_iomux_config();
    lcdc_rgb888_m1_iomux_config();
#endif
}
