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

#if defined(RT_USING_UART0)
RT_WEAK void uart0_m0_iomux_config(void)
{
    /* UART0 M0 RX-2A0 TX-2A1 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A0 |
                         GPIO_PIN_A1,
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

#if defined(RT_USING_UART1)
RT_WEAK void uart1_m0_iomux_config(void)
{
    /* UART1 M0 RX-1D0 TX-1D1 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_D0 |
                         GPIO_PIN_D1,
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

#if defined(RT_USING_UART2)
RT_WEAK  void uart2_m1_iomux_config(void)
{
    /* UART2 M1 RX-4D2 TX-4D3 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_D2 |
                         GPIO_PIN_D3,
                         PIN_CONFIG_MUX_FUNC2);
}
#endif

#if defined(RT_USING_UART3)
RT_WEAK void uart3_m1_iomux_config(void)
{
    /* UART3 M1 RX-0C1 TX-0C2 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C1 |
                         GPIO_PIN_C2,
                         PIN_CONFIG_MUX_FUNC3);

    /* set UART master 1 IOMUX selection to M1 */
    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_UART3_MULTI_IOFUNC_SRC_SEL_MASK,
                      (1 << GRF_SOC_CON5_GRF_UART3_MULTI_IOFUNC_SRC_SEL_SHIFT));
}
#endif

#if defined(RT_USING_UART4)
RT_WEAK  void uart4_m0_iomux_config(void)
{
    /* UART4 M0 RX-4B0 TX-4B1 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_B0 |
                         GPIO_PIN_B1,
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

#ifdef RT_USING_I2C0
RT_WEAK  void i2c0_m0_iomux_config(void)
{
    /* I2C0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_D0 |
                         GPIO_PIN_D1,
                         PIN_CONFIG_MUX_FUNC2);
}
#endif

#ifdef RT_USING_I2C1
RT_WEAK  void i2c1_m0_iomux_config(void)
{
    /* I2C1 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B3 |
                         GPIO_PIN_B4,
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

#ifdef RT_USING_I2C2
RT_WEAK  void i2c2_m0_iomux_config(void)
{
    /* I2C2 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A2 |
                         GPIO_PIN_A3,
                         PIN_CONFIG_MUX_FUNC3);

    /* set SOC_CON13 sel plus: GPIO2_A2 IOMUX FUN3 (I2C2_SDA) */
    WRITE_REG_MASK_WE(GRF->SOC_CON13,
                      GRF_SOC_CON13_GPIO2A2_SEL_SRC_CTRL_MASK | GRF_SOC_CON13_GPIO2A2_SEL_PLUS_MASK,
                      GRF_SOC_CON13_GPIO2A2_SEL_SRC_CTRL_MASK | (3 << GRF_SOC_CON13_GPIO2A2_SEL_PLUS_SHIFT));

    /* set SOC_CON13 sel plus: GPIO2_A3 IOMUX FUN3 (I2C2_SCL) */
    WRITE_REG_MASK_WE(GRF->SOC_CON13,
                      GRF_SOC_CON13_GPIO2A3_SEL_SRC_CTRL_MASK | GRF_SOC_CON13_GPIO2A3_SEL_PLUS_MASK,
                      GRF_SOC_CON13_GPIO2A3_SEL_SRC_CTRL_MASK | (3 << GRF_SOC_CON13_GPIO2A3_SEL_PLUS_SHIFT));
}
#endif

#ifdef RT_USING_SPI0
RT_WEAK  void spi0_m0_iomux_config(void)
{
    /* I2C2 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A0 |
                         GPIO_PIN_A1 |
                         GPIO_PIN_A2 |
                         GPIO_PIN_A3,
                         PIN_CONFIG_MUX_FUNC2);
}
#endif

#ifdef RT_USING_SPI1
RT_WEAK void spi1_m0_iomux_config(void)
{
    /* SPI1 M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_B2 |  // SPI1_MISO
                         GPIO_PIN_B3 |  // SPI1_CLK
                         GPIO_PIN_B4 |  // SPI1_MOSI
                         GPIO_PIN_B5,   // SPI1_CSN0
                         PIN_CONFIG_MUX_FUNC3);

    /* set SPI master 1 IOMUX selection to M0 */
    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_SPI1_MULTI_IOFUNC_SRC_SEL_MASK,
                      (0 << GRF_SOC_CON5_GRF_SPI1_MULTI_IOFUNC_SRC_SEL_SHIFT));

    /* set SOC_CON15 sel plus */
    WRITE_REG_MASK_WE(GRF->SOC_CON15,
                      GRF_SOC_CON15_GPIO3B2_SEL_PLUS_MASK | GRF_SOC_CON15_GPIO3B2_SEL_PLUS_MASK |
                      GRF_SOC_CON15_GPIO3B3_SEL_PLUS_MASK | GRF_SOC_CON15_GPIO3B3_SEL_SRC_CTRL_MASK,
                      (3 << GRF_SOC_CON15_GPIO3B2_SEL_PLUS_SHIFT) + (1 << GRF_SOC_CON15_GPIO3B2_SEL_SRC_CTRL_SHIFT) |
                      (3 << GRF_SOC_CON15_GPIO3B3_SEL_PLUS_SHIFT) + (1 << GRF_SOC_CON15_GPIO3B3_SEL_SRC_CTRL_SHIFT));
}

RT_WEAK void spi1_m1_iomux_config(void)
{
    /* SPI1 M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A4 |  // SPI1_MISO_M1
                         GPIO_PIN_A5 |  // SPI1_MOSI_M1
                         GPIO_PIN_A7,   // SPI1_CLK_M1
                         PIN_CONFIG_MUX_FUNC2);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_B1,   // SPI1_CSN0_M1
                         PIN_CONFIG_MUX_FUNC2);

    /* set SPI master 1 IOMUX selection to M1 */
    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_SPI1_MULTI_IOFUNC_SRC_SEL_MASK,
                      (1 << GRF_SOC_CON5_GRF_SPI1_MULTI_IOFUNC_SRC_SEL_SHIFT));
}
#endif

#ifdef RT_USING_SPI2
RT_WEAK void spi2_m0_iomux_config(void)
{
    /* SPI2 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_D0 |  // SPI2_CLK
                         GPIO_PIN_D1,  // SPI2_CS0N0
                         PIN_CONFIG_MUX_FUNC3);

    /* set SPI master 2 IOMUX selection to M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C6 |  //SPI2_MISO
                         GPIO_PIN_C7,   // SPI2_MOSI
                         PIN_CONFIG_MUX_FUNC3);
}
#endif

#ifdef RT_USING_AUDIO_CARD_I2S0
RT_WEAK void i2s0_8ch_m0_iomux_config(void)
{
    /* I2S0 8CH */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A4 |  // I2S0_MCLK(8CH)
                         GPIO_PIN_A5 |  // I2S0_SCLK_TX(8CH)
                         GPIO_PIN_A6 |  // I2S0_SCLK_RX(8CH)
                         GPIO_PIN_A7 |  // I2S0_LRCK_TX(8CH)
                         GPIO_PIN_B0 |  // I2S0_LRCK_RX(8CH)
                         GPIO_PIN_B1 |  // I2S0_SDO0(8CH)
                         GPIO_PIN_B5,   // I2S0_SDI0(8CH)
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

#ifdef RT_USING_AUDIO_CARD_I2S1
RT_WEAK void i2s1_8ch_m0_iomux_config(void)
{
    /* I2S1 8CH M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A2 |  // I2S1_MCLK_M0(8CH)
                         GPIO_PIN_A3 |  // I2S1_SCLK_TX_M0(8CH)
                         GPIO_PIN_A4 |  // I2S1_SCLK_RX_M0(8CH)
                         GPIO_PIN_A5 |  // I2S1_LRCK_TX_M0(8CH)
                         GPIO_PIN_A6 |  // I2S1_LRCK_RX_M0(8CH)
                         GPIO_PIN_A7 |  // I2S1_SDO0_M0(8CH)
                         GPIO_PIN_B3,   // I2S1_SDI0_M0(8CH)
                         PIN_CONFIG_MUX_FUNC2);
}
#endif

#ifdef RT_USING_SDIO0
/**
 * @brief  Config iomux for SDIO
 */
RT_WEAK void emmc_iomux_config(void)
{
    /* EMMC D0 ~ D7*/
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_A0 |
                         GPIO_PIN_A1 |
                         GPIO_PIN_A2 |
                         GPIO_PIN_A3 |
                         GPIO_PIN_A4 |
                         GPIO_PIN_A5 |
                         GPIO_PIN_A6 |
                         GPIO_PIN_A7,
                         PIN_CONFIG_MUX_FUNC2);
    /* EMMC CMD & CLK & PWR */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_B0 |  /* CMD */
                         GPIO_PIN_B1 |  /* CLK */
                         GPIO_PIN_B3,   /* PWR */
                         PIN_CONFIG_MUX_FUNC2);
}
#endif

#ifdef RT_USING_GMAC
#ifdef RT_USING_GMAC0
/**
 * @brief  Config iomux for GMAC0
 */
RT_WEAK void gmac0_m1_iomux_config(void)
{
    /* GMAC0 iomux */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_A0 | /* gmac0_rxer_m1 */
                         GPIO_PIN_A1 | /* gmac0_rxdv_m1 */
                         GPIO_PIN_A2 | /* gmac0_rxd0_m1 */
                         GPIO_PIN_A3 | /* gmac0_rxd1_m1 */
                         GPIO_PIN_A4 | /* gmac0_txd0_m1 */
                         GPIO_PIN_A5 | /* gmac0_txd1_m1 */
                         GPIO_PIN_B4 | /* gmac0_clk_m1 */
                         GPIO_PIN_B5 | /* gmac0_mdc_m1 */
                         GPIO_PIN_B6 | /* gmac0_mdio_m1 */
                         GPIO_PIN_B7,  /* gmac0_txen_m1 */
                         PIN_CONFIG_MUX_FUNC2);

    /* set GMAC IOMUX selection to M1 */
    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_MAC_MULTI_IOFUNC_SRC_SEL_MASK,
                      (1 << GRF_SOC_CON5_GRF_MAC_MULTI_IOFUNC_SRC_SEL_SHIFT));
}
#endif
#endif

#ifdef RT_USING_BACKLIGHT
RT_WEAK void pwm0_ch1_iomux_config(void)
{
    /* I2S0 8CH */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B6,
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

#ifdef RT_USING_VOP
RT_WEAK void lcdc_ctrl_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A0 |  // dclk
                         GPIO_PIN_A1 |  // hysc
                         GPIO_PIN_A2 |  // vsync
                         GPIO_PIN_A3 |  // den
                         GPIO_PIN_A4 |  // d0
                         GPIO_PIN_A5 |  // d1
                         GPIO_PIN_A6 |  // d2
                         GPIO_PIN_A7 |  // d3
                         GPIO_PIN_B0 |  // d4
                         GPIO_PIN_B1 |  // d5
                         GPIO_PIN_B2 |  // d6
                         GPIO_PIN_B3 |  // d7
                         GPIO_PIN_B4 |  // d8
                         GPIO_PIN_B5 |  // d9
                         GPIO_PIN_B6 |  // d10
                         GPIO_PIN_B7 |  // d11
                         GPIO_PIN_C0 |  // d12
                         GPIO_PIN_C1 |  // d13
                         GPIO_PIN_C2 |  // d14
                         GPIO_PIN_C3 |  // d15
                         GPIO_PIN_C4 |  // d16
                         GPIO_PIN_C5,   // d17
                         PIN_CONFIG_MUX_FUNC1);

    HAL_PINCTRL_SetParam(GPIO_BANK1,
                         GPIO_PIN_A0 |  // dclk
                         GPIO_PIN_A1 |  // hysc
                         GPIO_PIN_A2 |  // vsync
                         GPIO_PIN_A3 |  // den
                         GPIO_PIN_A4 |  // d0
                         GPIO_PIN_A5 |  // d1
                         GPIO_PIN_A6 |  // d2
                         GPIO_PIN_A7 |  // d3
                         GPIO_PIN_B0 |  // d4
                         GPIO_PIN_B1 |  // d5
                         GPIO_PIN_B2 |  // d6
                         GPIO_PIN_B3 |  // d7
                         GPIO_PIN_B4 |  // d8
                         GPIO_PIN_B5 |  // d9
                         GPIO_PIN_B6 |  // d10
                         GPIO_PIN_B7 |  // d11
                         GPIO_PIN_C0 |  // d12
                         GPIO_PIN_C1 |  // d13
                         GPIO_PIN_C2 |  // d14
                         GPIO_PIN_C3 |  // d15
                         GPIO_PIN_C4 |  // d16
                         GPIO_PIN_C5,
                         PIN_CONFIG_DRV_LEVEL3);
};

RT_WEAK void lcdc_rgb888_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C6 |  // d18
                         GPIO_PIN_C7,   // d19
                         PIN_CONFIG_MUX_FUNC6);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_B1 |  // d20
                         GPIO_PIN_B2 |  // d21
                         GPIO_PIN_B7 |  // d22
                         GPIO_PIN_C0,   // d23
                         PIN_CONFIG_MUX_FUNC3);
}

RT_WEAK void lcdc_rgb888_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_A6 |  // d18
                         GPIO_PIN_A7 |  // d19
                         GPIO_PIN_B0 |  // d20
                         GPIO_PIN_B1,   // d21
                         PIN_CONFIG_MUX_FUNC3);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_B2 |  // d22
                         GPIO_PIN_B3,   // d23
                         PIN_CONFIG_MUX_FUNC8);
}
#endif

/**
 * @brief  Config iomux for RK3308
 */
RT_WEAK  void rt_hw_iomux_config(void)
{
#ifdef RT_USING_UART2
    uart2_m1_iomux_config();
#endif
}
