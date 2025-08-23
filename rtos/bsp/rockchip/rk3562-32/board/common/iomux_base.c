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

#ifdef RT_USING_UART0
RT_WEAK  void uart0_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D0 |
                         GPIO_PIN_D1,
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

#ifdef RT_USING_UART7
RT_WEAK void uart7_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B3,
                         PIN_CONFIG_MUX_FUNC3);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B4,
                         PIN_CONFIG_MUX_FUNC3);
}
#endif

#ifdef RT_USING_GMAC
#ifdef RT_USING_GMAC0
/**
 * @brief  Config iomux for GMAC0
 */
RT_WEAK void gmac0_m0_iomux_config(void)
{
    /* GMAC0 iomux */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_A0 | /* RGMII_RSTn */
                         GPIO_PIN_A1,  /* RGMII_INT/PMEB_M0 */
                         PIN_CONFIG_MUX_FUNC0);

    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_A1, GPIO_IN);
    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_A0, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO3, GPIO_PIN_A0, GPIO_HIGH);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_D4 | /* RGMII_TXD2_M0 */
                         GPIO_PIN_D5 | /* RGMII_TXD3_M0 */
                         GPIO_PIN_D6 | /* RGMII_TXCLK_M0 */
                         GPIO_PIN_D7,  /* RGMII_RXD2_M0 */
                         PIN_CONFIG_MUX_FUNC2);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_A0 | /* RGMII_RXD3_M0 */
                         GPIO_PIN_A1 | /* RGMII_RXCLK_M0 */
                         GPIO_PIN_A2 | /* RGMII_TXD0_M0 */
                         GPIO_PIN_A3 | /* RGMII_TXD1_M0 */
                         GPIO_PIN_A4 | /* RGMII_TXEN_M0 */
                         GPIO_PIN_A5 | /* RGMII_RXD0_M0 */
                         GPIO_PIN_A6 | /* RGMII_RXD1_M0 */
                         GPIO_PIN_A7 | /* RGMII_RXDV_M0 */
                         GPIO_PIN_B1 | /* ETH_CLK_25M_OUT_M0 */
                         GPIO_PIN_B2 | /* RGMII_MDC_M0 */
                         GPIO_PIN_B3 | /* RGMII_MDIO_M0 */
                         GPIO_PIN_B7,  /* RGMII_CLK_M0 */
                         PIN_CONFIG_MUX_FUNC2);
}
#endif
#endif

#ifdef RT_USING_SNOR_FSPI_HOST
RT_WEAK  void fspi_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A0 |
                         GPIO_PIN_A1 |
                         GPIO_PIN_A2 |
                         GPIO_PIN_A3,
                         PIN_CONFIG_MUX_FUNC2);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B0 |
                         GPIO_PIN_B1,
                         PIN_CONFIG_MUX_FUNC2);
}
#endif

#ifdef RT_USING_SAI0
RT_WEAK  void sai0_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_A4 | // i2s0_lrck_m0
                         GPIO_PIN_A2 | // i2s0_mclk_m0
                         GPIO_PIN_A3 | // i2s0_sclk_m0
                         GPIO_PIN_B1 | // i2s0_sdi0_m0
                         GPIO_PIN_A5 | // i2s0_sdo0_m0
                         GPIO_PIN_A6 | // i2s0_sdo1_m0
                         GPIO_PIN_A7 | // i2s0_sdo2_m0
                         GPIO_PIN_B0,  // i2s0_sdo3_m0
                         PIN_CONFIG_MUX_FUNC1);

    WRITE_REG_MASK_WE(PERI_GRF->AUDIO_CON,
                      PERI_GRF_AUDIO_CON_SAI0_MCLK_OE_MASK,
                      (1 << PERI_GRF_AUDIO_CON_SAI0_MCLK_OE_SHIFT));
}
#endif

/**
 * @brief  Config iomux for RK3562
 */
RT_WEAK  void rt_hw_iomux_config(void)
{
#ifdef RT_USING_UART0
    uart0_m0_iomux_config();
#endif
#ifdef RT_USING_SNOR_FSPI_HOST
    fspi_iomux_config();
#endif
}
