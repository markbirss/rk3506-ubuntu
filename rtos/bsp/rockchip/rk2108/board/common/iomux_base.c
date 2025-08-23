/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-25     Cliff Chen   first implementation
 */

/** @addtogroup RKBSP_Board_Driver
 *  @{
 */

/** @addtogroup IOMUX
 *  @{
 */

/** @defgroup How_To_Use How To Use
 *  @{
 @verbatim

 ==============================================================================
                    #### How to use ####
 ==============================================================================
 This file provide IOMUX for board, it will be invoked when board initialization.

 @endverbatim
 @} */
#include "rtdef.h"
#include "iomux.h"
#include "hal_base.h"

/********************* Private MACRO Definition ******************************/
/** @defgroup IOMUX_Private_Macro Private Macro
 *  @{
 */

/** @} */  // IOMUX_Private_Macro

/********************* Private Structure Definition **************************/
/** @defgroup IOMUX_Private_Structure Private Structure
 *  @{
 */

/** @} */  // IOMUX_Private_Structure

/********************* Private Variable Definition ***************************/
/** @defgroup IOMUX_Private_Variable Private Variable
 *  @{
 */

/** @} */  // IOMUX_Private_Variable

/********************* Private Function Definition ***************************/
/** @defgroup IOMUX_Private_Function Private Function
 *  @{
 */

/** @} */  // IOMUX_Private_Function

/********************* Public Function Definition ****************************/

/** @defgroup IOMUX_Public_Functions Public Functions
 *  @{
 */

/**
 * @brief  Config iomux for PDM
 */
RT_WEAK  void pdm_input_iomux_config(void)
{
    // for pdm input
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A0 |  // PDM_IN_CLK0
                         GPIO_PIN_A1 |  // PDM_IN_CLK1
                         GPIO_PIN_A2 |  // PDM_IN_SDI0
                         GPIO_PIN_A3,   // PDM_IN_SDI1
                         PIN_CONFIG_MUX_FUNC1);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A0 |  // PDM_IN_CLK0
                         GPIO_PIN_A1 |  // PDM_IN_CLK1
                         GPIO_PIN_A2 |  // PDM_IN_SDI0
                         GPIO_PIN_A3,   // PDM_IN_SDI1
                         PIN_CONFIG_PUL_NORMAL);

    WRITE_REG_MASK_WE(GRF->SOC_CON2,
                      GRF_SOC_CON2_GRF_CON_AUDIO_BYPASS_CTRL_SEL_MASK,  // Audio bypass controlled by GRF
                      (0x1 << GRF_SOC_CON2_GRF_CON_AUDIO_BYPASS_CTRL_SEL_SHIFT));

    WRITE_REG_MASK_WE(GRF->SOC_CON4,
                      GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_PDM_MASK,  // PDM used as audio input
                      (0x1 << GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_PDM_SHIFT));
}

/**
 * @brief  Config iomux for I2S1
 */
RT_WEAK  void i2s1_input_iomux_config(void)
{
    // for i2s input
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A0 |  // I2S_IN_SCLK
                         GPIO_PIN_A1 |  // I2S_IN_LRCK
                         GPIO_PIN_A2 |  // I2S_IN_SDI0
                         GPIO_PIN_A3 |  // I2S_IN_SDI1
                         GPIO_PIN_A4 |  // I2S_IN_MCLK
                         GPIO_PIN_A5,   // I2S_IN_SDI2
                         PIN_CONFIG_MUX_FUNC4);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A0 |  // I2S_IN_SCLK
                         GPIO_PIN_A1 |  // I2S_IN_LRCK
                         GPIO_PIN_A2 |  // I2S_IN_SDI0
                         GPIO_PIN_A3 |  // I2S_IN_SDI1
                         GPIO_PIN_A4 |  // I2S_IN_MCLK
                         GPIO_PIN_A5,   // I2S_IN_SDI2
                         PIN_CONFIG_PUL_NORMAL);

    WRITE_REG_MASK_WE(GRF->SOC_CON4,
                      GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_I2S1_MASK |  // I2S1 used as audio input
                      GRF_SOC_CON4_GRF_CON_I2S1_SDI2_FROM_IO_MASK,    // I2S1 sdi2 come from IO
                      (0x1 << GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_I2S1_SHIFT) |
                      (0x1 << GRF_SOC_CON4_GRF_CON_I2S1_SDI2_FROM_IO_SHIFT));
}

/**
 * @brief  Config iomux for I2S1
 */
RT_WEAK  void i2s1_output_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A4 |  // I2S1_MCLK_MUX0
                         GPIO_PIN_A6 |  // I2S1_OUT_SCLK_MUX0
                         GPIO_PIN_A7 |  // I2S1_OUT_LRCK_MUX0
                         GPIO_PIN_B0,   // I2S1_OUT_SDO0_MUX0
                         PIN_CONFIG_MUX_FUNC4);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A4 |  // I2S1_MCLK_MUX0
                         GPIO_PIN_A6 |  // I2S1_OUT_SCLK_MUX0
                         GPIO_PIN_A7 |  // I2S1_OUT_LRCK_MUX0
                         GPIO_PIN_B0,   // I2S1_OUT_SDO0_MUX0
                         PIN_CONFIG_PUL_NORMAL);

    WRITE_REG_MASK_WE(GRF->SOC_CON4,
                      GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_I2S1_MASK,  // used I2S1 as audio input
                      (0x1 << GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_I2S1_SHIFT));
}

/**
 * @brief  Config iomux for VICAP
 */
RT_WEAK  void vicap_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A0 |  // VICAP_D0
                         GPIO_PIN_A1 |  // VICAP_D1
                         GPIO_PIN_A2 |  // VICAP_D2
                         GPIO_PIN_A3 |  // VICAP_D3
                         GPIO_PIN_A4 |  // VICAP_D4
                         GPIO_PIN_A5 |  // VICAP_D5
                         GPIO_PIN_A6 |  // VICAP_D6
                         GPIO_PIN_A7 |  // VICAP_D7
                         GPIO_PIN_B0 |  // VICAP_D8
                         GPIO_PIN_B1,   // VICAP_D9
                         PIN_CONFIG_MUX_FUNC5);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D5 |  // VICAP_HREF
                         GPIO_PIN_D6 |  // VICAP_VSYNC
                         GPIO_PIN_D7,   // VICAP_PCLK
                         PIN_CONFIG_MUX_FUNC2);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A4,// VICAP_CLKOUT
                         PIN_CONFIG_MUX_FUNC3);
}

/**
 * @brief  Config iomux for LCDC
 */
RT_WEAK  void lcdc_iomux_config(void)
{
#ifdef RT_USING_VOP_MCU
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A0 |  // LCDC_D0
                         GPIO_PIN_A1 |  // LCDC_D1
                         GPIO_PIN_A2 |  // LCDC_D2
                         GPIO_PIN_A3 |  // LCDC_D3
                         GPIO_PIN_A6 |  // LCDC_D6
                         GPIO_PIN_A7 |  // LCDC_D7
                         GPIO_PIN_B0 |  // LCDC_WR
                         GPIO_PIN_B1,   // LCDC_RD
                         PIN_CONFIG_MUX_FUNC3);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A4 |  // LCDC_D4
                         GPIO_PIN_A5,   // LCDC_D5
                         PIN_CONFIG_MUX_FUNC6);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A4 |  // LCDC_CMD
                         GPIO_PIN_A5,   // LCDC_CS
                         PIN_CONFIG_MUX_FUNC2);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D2,
                         PIN_CONFIG_MUX_FUNC4);// PWM2

#elif defined RT_USING_PANEL_ST7703_DS
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B4,   // LCDC_RST
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_B4, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_B4, GPIO_HIGH);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B2,   // LCDC_en
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_B2, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_B2, GPIO_HIGH);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D2,
                         PIN_CONFIG_MUX_FUNC4);//PWM2
#endif
}

/**
 * @brief  Config iomux for SFC0
 */
RT_WEAK  void sfc0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B2 |  // SFC_D3
                         GPIO_PIN_B3 |  // SFC_D2
                         GPIO_PIN_B4 |  // SFC_CS
                         GPIO_PIN_B5,   // SFC_CLK
                         PIN_CONFIG_MUX_FUNC3);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B6 |  // SFC_D0
                         GPIO_PIN_B7,   // SFC_D1
                         PIN_CONFIG_MUX_FUNC2);
}

/**
 * @brief  Config iomux for SDIO
 */
RT_WEAK  void sdio_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C0 |  // SDIO_CLK
                         GPIO_PIN_C1 |  // SDIO_CMD
                         GPIO_PIN_C2 |  // SDIO_D0
                         GPIO_PIN_C3 |  // SDIO_D1
                         GPIO_PIN_C4,   // SDIO_D2
                         PIN_CONFIG_MUX_FUNC4);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C5,   // SDIO_D3
                         PIN_CONFIG_MUX_FUNC3);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_C1 |  // SDIO_CMD
                         GPIO_PIN_C2 |  // SDIO_D0
                         GPIO_PIN_C3 |  // SDIO_D1
                         GPIO_PIN_C4 |  // SDIO_D2
                         GPIO_PIN_C5,   // SDIO_D3
                         PIN_CONFIG_PUL_UP |
                         PIN_CONFIG_DRV_LEVEL1);
    HAL_PINCTRL_SetParam(GPIO_BANK1, GPIO_PIN_A4, PIN_CONFIG_PUL_UP);
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_A4, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_A4, GPIO_HIGH);
}

/**
 * @brief  Config iomux for DSP JTAG
 */
RT_WEAK  void dsp_jtag_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C0 |  // DSP_JTAG0_TCK
                         GPIO_PIN_C1 |  // DSP_JTAG0_TMS
                         GPIO_PIN_C2 |  // DSP_JTAG0_TDI
                         GPIO_PIN_C3 |  // DSP_JTAG0_TRSTN
                         GPIO_PIN_C4,   // DSP_JTAG0_TDO
                         PIN_CONFIG_MUX_FUNC3);
}

/**
 * @brief  Config iomux for UART2
 */
RT_WEAK  void uart2_iomux_config(void)
{
    // for uart2
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A0 |  // UART2_RX
                         GPIO_PIN_A1 |  // UART2_TX
                         GPIO_PIN_A2 |  // UART2_CTS
                         GPIO_PIN_A3,   // UART2_RTS
                         PIN_CONFIG_MUX_FUNC4);
}

RT_WEAK  void uart2_rts_iomux_config(int uart_en)
{
    if (uart_en)
    {
        HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                             GPIO_PIN_A3,
                             PIN_CONFIG_MUX_FUNC4);
    }
    else
    {
        HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                             GPIO_PIN_A3,
                             PIN_CONFIG_MUX_FUNC0);
        HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_A3, GPIO_OUT);
        HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_A3, GPIO_HIGH);
    }
}

/**
 * @brief  Config iomux for PCM_M0
 */
RT_WEAK  void pcm_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D3 |  // PCM_CLK_M0
                         GPIO_PIN_D4 |  // PCM_SYNC_M0
                         GPIO_PIN_D5 |  // PCM_IN_M0
                         GPIO_PIN_D6,   // PCM_OUT_M0
                         PIN_CONFIG_MUX_FUNC4);

    WRITE_REG_MASK_WE(GRF->SOC_CON2,
                      GRF_SOC_CON2_GRF_CON_I2S_LRCK_MUX_MASK |
                      GRF_SOC_CON2_GRF_CON_I2S_SCLK_MUX_MASK,
                      (0x1 << GRF_SOC_CON2_GRF_CON_I2S_LRCK_MUX_SHIFT) |
                      (0x1 << GRF_SOC_CON2_GRF_CON_I2S_SCLK_MUX_SHIFT));
    WRITE_REG_MASK_WE(GRF->SOC_CON4,
                      GRF_SOC_CON4_GRF_CON_BT_PCM_I2S0_MASK,
                      (0x1 << GRF_SOC_CON4_GRF_CON_BT_PCM_I2S0_SHIFT));
    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_PCM_IOMUX_SEL_MASK,
                      (0x0 << GRF_SOC_CON5_GRF_CON_PCM_IOMUX_SEL_SHIFT));
}

/**
 * @brief  Config iomux for PCM_M1
 */
RT_WEAK  void pcm_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B0 |  // PCM_SYNC_M1
                         GPIO_PIN_B1 |  // PCM_IN_M1
                         GPIO_PIN_B2,   // PCM_OUT_M1
                         PIN_CONFIG_MUX_FUNC2);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B4,   // PCM_CLK
                         PIN_CONFIG_MUX_FUNC1);

    WRITE_REG_MASK_WE(GRF->SOC_CON2,
                      GRF_SOC_CON2_GRF_CON_I2S_LRCK_MUX_MASK |
                      GRF_SOC_CON2_GRF_CON_I2S_SCLK_MUX_MASK,
                      (0x1 << GRF_SOC_CON2_GRF_CON_I2S_LRCK_MUX_SHIFT) |
                      (0x1 << GRF_SOC_CON2_GRF_CON_I2S_SCLK_MUX_SHIFT));
    WRITE_REG_MASK_WE(GRF->SOC_CON4,
                      GRF_SOC_CON4_GRF_CON_BT_PCM_I2S0_MASK,
                      (0x1 << GRF_SOC_CON4_GRF_CON_BT_PCM_I2S0_SHIFT));
    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_PCM_IOMUX_SEL_MASK,
                      (0x1 << GRF_SOC_CON5_GRF_CON_PCM_IOMUX_SEL_SHIFT));
}

/**
 * @brief  Config iomux for touch
 */
RT_WEAK  void touch_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B0 |  // TOUCH_INT
                         GPIO_PIN_B1 |  // TOUCH_RST
                         GPIO_PIN_B2,   // PANNEL_POWER(LCDC & TOUCH)
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK1,
                         GPIO_PIN_B0,
                         PIN_CONFIG_PUL_NORMAL |
                         PIN_CONFIG_DRV_LEVEL0);

    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_B2, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_B2, GPIO_HIGH);
}

/**
 * @brief  Config iomux for KEY CTRL
 */
RT_WEAK  void key_ctrl_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B6 |  // KEY_IN_M0
                         GPIO_PIN_B7,   // KEY_OUT_M0
                         PIN_CONFIG_MUX_FUNC1);

    HAL_PINCTRL_SetParam(GPIO_BANK1,
                         GPIO_PIN_B6,
                         PIN_CONFIG_PUL_NORMAL);
>>>>>>> [BSP] Rockchip: Merge rockchip porting code
}

/**
 * @brief  Config iomux for UART0
 */
RT_WEAK  void uart0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C7 |  // UART0_RX
                         GPIO_PIN_D0,   // UART0_TX
                         PIN_CONFIG_MUX_FUNC1);
}

/**
 * @brief  Config iomux for UART1
 */
RT_WEAK  void uart1_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D1 |  // UART1_RX_M0
                         GPIO_PIN_D2,   // UART1_TX_M0
                         PIN_CONFIG_MUX_FUNC2);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_UART1_IOMUX_SEL_MASK,
                      (0 << GRF_SOC_CON5_GRF_CON_UART1_IOMUX_SEL_SHIFT));
}

RT_WEAK  void uart1_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A5 |  // UART1_RX_M1
                         GPIO_PIN_A4,   // UART1_TX_M1
                         PIN_CONFIG_MUX_FUNC2);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_UART1_IOMUX_SEL_MASK,
                      (1 << GRF_SOC_CON5_GRF_CON_UART1_IOMUX_SEL_SHIFT));
}

RT_WEAK  void uart1_m2_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B1 |  // UART1_RX_M2
                         GPIO_PIN_B0,   // UART1_TX_M2
                         PIN_CONFIG_MUX_FUNC3);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_UART1_IOMUX_SEL_MASK,
                      (2 << GRF_SOC_CON5_GRF_CON_UART1_IOMUX_SEL_SHIFT));
}

RT_WEAK  void uart1_m3_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A5,  // UART1_RX_M3
                         PIN_CONFIG_MUX_FUNC2);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B1,  // UART1_TX_M3
                         PIN_CONFIG_MUX_FUNC4);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_UART1_IOMUX_SEL_MASK,
                      (3 << GRF_SOC_CON5_GRF_CON_UART1_IOMUX_SEL_SHIFT));
}

/**
 * @brief  Config iomux for AUDIO PWM M0
 */
RT_WEAK  void audio_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D1,   // AUDIO_LOUT_M0
                         PIN_CONFIG_MUX_FUNC3);
}

/**
 * @brief  Config iomux for M4 JTAG
 */
RT_WEAK  void m4_jtag_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C7 |  // M4_JTAG_TCK
                         GPIO_PIN_D0,   // M4_JTAG_TMS
                         PIN_CONFIG_MUX_FUNC2);
}

/**
 * @brief  Config iomux for SFC1
 */
RT_WEAK  void sfc1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C4 |  // SFC1_CS
                         GPIO_PIN_C5 |  // SFC1_CLK
                         GPIO_PIN_C6 |  // SFC1_D0
                         GPIO_PIN_C7,   // SFC1_D1
                         PIN_CONFIG_MUX_FUNC2);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C2 |  // SFC1_D3
                         GPIO_PIN_C3,   // SFC1_D2
                         PIN_CONFIG_MUX_FUNC3);
}

/**
 * @brief  Config iomux for SPI1_m0
 */
RT_WEAK  void spi1_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C2 |  // SPI_MST1_CS_M0
                         GPIO_PIN_C3 |  // SPI_MST1_CLK_M0
                         GPIO_PIN_C4 |  // SPI_MST1_MISO_M0
                         GPIO_PIN_C5,   // SPI_MST1_MOSI_M0
                         PIN_CONFIG_MUX_FUNC1);

    /* set SPI master 1 IOMUX selection to M0 */
    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_SPI_MST1_IOMUX_SEL_MASK,
                      (0 << GRF_SOC_CON5_GRF_CON_SPI_MST1_IOMUX_SEL_SHIFT));

}

/**
 * @brief  Config iomux for SPI1_m1
 */
RT_WEAK  void spi1_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C0 |  // SPI_MST1_CS_M1
                         GPIO_PIN_C1 |  // SPI_MST1_CLK_M1
                         GPIO_PIN_C2 |  // SPI_MST1_MOSI_M1
                         GPIO_PIN_C3,   // SPI_MST1_MISO_M1
                         PIN_CONFIG_MUX_FUNC1);

    /* set SPI master 1 IOMUX selection to M1 */
    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_SPI_MST1_IOMUX_SEL_MASK,
                      (1 << GRF_SOC_CON5_GRF_CON_SPI_MST1_IOMUX_SEL_SHIFT));

}

/**
 * @brief  Config iomux for SPI2_m0
 */
RT_WEAK  void spi2_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A0 |  // SPI_MST2_CS0_M0
                         GPIO_PIN_A1 |  // SPI_MST2_CLK_M0
                         GPIO_PIN_A2 |  // SPI_MST2_MISO_M0
                         GPIO_PIN_A3 |  // SPI_MST2_MOSI_M0
                         GPIO_PIN_A5,   // SPI_MST2_CS1
                         PIN_CONFIG_MUX_FUNC3);

    /* set SPI master 2 IOMUX selection to M0 */
    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_SPI_MST2_IOMUX_SEL_MASK,
                      (0 << GRF_SOC_CON5_GRF_CON_SPI_MST2_IOMUX_SEL_SHIFT));

#ifdef RT_USING_SPI_SCREEN
    /*
     * set GPIO0_C4 to be GPIO function, it is used as the A0(DCX) pin of
     * the SPI screen
     */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C4,
                         PIN_CONFIG_MUX_FUNC0);
#endif
}

/**
 * @brief  Config iomux for SPI2_m1
 */
RT_WEAK  void spi2_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C4 |  // SPI_MST2_CS0_M1
                         GPIO_PIN_C5 |  // SPI_MST2_CLK_M1
                         GPIO_PIN_C6 |  // SPI_MST2_MOSI_M1
                         GPIO_PIN_C7,   // SPI_MST2_MISO_M1
                         PIN_CONFIG_MUX_FUNC1);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A5,   // SPI_MST2_CS1
                         PIN_CONFIG_MUX_FUNC3);

    /* set SPI master 2 IOMUX selection to M1 */
    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_SPI_MST2_IOMUX_SEL_MASK,
                      (1 << GRF_SOC_CON5_GRF_CON_SPI_MST2_IOMUX_SEL_SHIFT));
}

/**
 * @brief  Config iomux for I2C1
 */
RT_WEAK  void i2c0_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C0 |  // I2C_MST0_SCL_M0
                         GPIO_PIN_C1,   // I2C_MST0_SDA_M0
                         PIN_CONFIG_MUX_FUNC1);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_I2C_MST0_IOMUX_SEL_MASK,
                      (0 << GRF_SOC_CON5_GRF_CON_I2C_MST0_IOMUX_SEL_SHIFT));
}


RT_WEAK  void i2c0_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D3 |  // I2C_MST0_SCL_M1
                         GPIO_PIN_D4,   // I2C_MST0_SDA_M1
                         PIN_CONFIG_MUX_FUNC2);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_I2C_MST0_IOMUX_SEL_MASK,
                      (1 << GRF_SOC_CON5_GRF_CON_I2C_MST0_IOMUX_SEL_SHIFT));
}

RT_WEAK  void i2c0_m2_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A2 |  // I2C_MST0_SCL_M2
                         GPIO_PIN_A3,   // I2C_MST0_SDA_M2
                         PIN_CONFIG_MUX_FUNC5);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_I2C_MST0_IOMUX_SEL_MASK,
                      (2 << GRF_SOC_CON5_GRF_CON_I2C_MST0_IOMUX_SEL_SHIFT));
}

/**
 * @brief  Config iomux for I2C1
 */
RT_WEAK  void i2c1_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C2 |  // I2C_MST1_SCL_M0
                         GPIO_PIN_C3,   // I2C_MST1_SDA_M0
                         PIN_CONFIG_MUX_FUNC2);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_I2C_MST1_IOMUX_SEL_MASK,
                      (0 << GRF_SOC_CON5_GRF_CON_I2C_MST1_IOMUX_SEL_SHIFT));
}

RT_WEAK  void i2c1_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B2 |  // I2C_MST1_SCL_M1
                         GPIO_PIN_B3,   // I2C_MST1_SDA_M1
                         PIN_CONFIG_MUX_FUNC2);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_I2C_MST1_IOMUX_SEL_MASK,
                      (1 << GRF_SOC_CON5_GRF_CON_I2C_MST1_IOMUX_SEL_SHIFT));
}

RT_WEAK  void i2c1_m2_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C0 |  // I2C_MST1_SCL_M2
                         GPIO_PIN_C1,   // I2C_MST1_SDA_M2
                         PIN_CONFIG_MUX_FUNC2);

    /* The GPIO1_C1 default config is pull-down, clear it in i2c mode */
    HAL_PINCTRL_SetParam(GPIO_BANK1, GPIO_PIN_C0 | GPIO_PIN_C1,
                         PIN_CONFIG_PUL_NORMAL);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_I2C_MST1_IOMUX_SEL_MASK,
                      (2 << GRF_SOC_CON5_GRF_CON_I2C_MST1_IOMUX_SEL_SHIFT));
}

/**
 * @brief  Config iomux for I2C2
 */
RT_WEAK  void i2c2_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C4 |  // I2C_MST2_SCL_M0
                         GPIO_PIN_C5,   // I2C_MST2_SDA_M0
                         PIN_CONFIG_MUX_FUNC2);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_I2C_MST2_IOMUX_SEL_MASK,
                      (0 << GRF_SOC_CON5_GRF_CON_I2C_MST2_IOMUX_SEL_SHIFT));
}

RT_WEAK  void i2c2_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B4 |  // I2C_MST2_SCL_M1
                         GPIO_PIN_B5,   // I2C_MST2_SDA_M1
                         PIN_CONFIG_MUX_FUNC2);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_I2C_MST2_IOMUX_SEL_MASK,
                      (1 << GRF_SOC_CON5_GRF_CON_I2C_MST2_IOMUX_SEL_SHIFT));
}

RT_WEAK  void i2c2_m2_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C2 |  // I2C_MST2_SCL_M2
                         GPIO_PIN_C3,   // I2C_MST2_SDA_M2
                         PIN_CONFIG_MUX_FUNC2);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_I2C_MST2_IOMUX_SEL_MASK,
                      (2 << GRF_SOC_CON5_GRF_CON_I2C_MST2_IOMUX_SEL_SHIFT));
}

/**
 * @brief  Config iomux for rk2108 evb board
 */
RT_WEAK  void rt_hw_iomux_config(void)
{
    pdm_input_iomux_config();

    sfc0_iomux_config();

#ifdef DSP_JTAG_ENABLE
    dsp_jtag_iomux_config();
#else
    sdio_iomux_config();
#endif

    uart2_iomux_config();

    touch_iomux_config();

    key_ctrl_iomux_config();

#ifdef M4_JTAG_ENABLE
    m4_jtag_iomux_config();
#else
    uart0_iomux_config();
#endif

    i2c0_m1_iomux_config();
    i2c1_m2_iomux_config();

    lcdc_iomux_config();
#ifdef RT_USING_I2STDM1
    i2s1_output_iomux_config();
#endif

#ifdef RT_USING_VICAP
    vicap_iomux_config();
#endif

#ifdef RT_USING_QPIPSRAM_SPI_HOST
    spi1_m0_iomux_config();
#endif

#ifdef RT_USING_AUDIOPWM
    audio_iomux_config();
#endif
#if defined(RT_USING_PCM_M0)
    pcm_m0_iomux_config();
#elif defined(RT_USING_PCM_M1)
    pcm_m1_iomux_config();
#endif
#if defined(RT_USING_SPINAND_FSPI_HOST) || defined(CONFIG_RT_USING_QPIPSRAM)
    sfc1_iomux_config();
#endif
}
/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
