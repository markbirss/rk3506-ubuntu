/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    iomux.c
  * @version V0.1
  * @brief   iomux for rk2108 evb
  *
  * Change Logs:
  * Date           Author          Notes
  * 2019-05-25     Cliff.Chen      first implementation
  *
  ******************************************************************************
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
 * @brief  Config iomux for I2S1
 */
void i2s1_output_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A6 |  // I2S1_OUT_SCLK_MUX0
                         GPIO_PIN_A7 |  // I2S1_OUT_LRCK_MUX0
                         GPIO_PIN_B0,   // I2S1_OUT_SDO0_MUX0
                         PIN_CONFIG_MUX_FUNC4);

    /* this board use mclk_m1 but not mclk_m0 to be the mclk of codec */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D7,   // I2S1_MCLK_MUX1
                         PIN_CONFIG_MUX_FUNC3);

    WRITE_REG_MASK_WE(GRF->SOC_CON4,
                      GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_I2S1_MASK,  // used I2S1 as audio input and output
                      (0x1 << GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_I2S1_SHIFT));
}

/**
 * @brief  Config iomux for UART1
 */
void uart1_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A4 |  // UART1_TX_M1
                         GPIO_PIN_A5,   // UART1_RX_M1
                         PIN_CONFIG_MUX_FUNC2);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_UART1_IOMUX_SEL_MASK,
                      (1 << GRF_SOC_CON5_GRF_CON_UART1_IOMUX_SEL_SHIFT));

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A4 |  // UART1_TX_M1
                         GPIO_PIN_A5,   // UART1_RX_M1
                         PIN_CONFIG_PUL_UP);
}

/**
 * @brief  Config iomux for PDM
 */
void pdm_input_iomux_config(void)
{
    /* for pdm input */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A0 |  // PDM_IN_CLK0
                         GPIO_PIN_A2,   // PDM_IN_SDI0
                         PIN_CONFIG_MUX_FUNC1);

    WRITE_REG_MASK_WE(GRF->SOC_CON4,
                      GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_PDM_MASK,  // PDM used as audio input
                      (0x1 << GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_PDM_SHIFT));
}

/**
 * @brief  Config iomux for SDIO
 */
void sdio_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C0 |  // SDIO_CLK
                         GPIO_PIN_C1 |  // SDIO_CMD
                         GPIO_PIN_C2,   // SDIO_D0
                         PIN_CONFIG_MUX_FUNC4);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_C1 |  // SDIO_CMD
                         GPIO_PIN_C2,   // SDIO_D0
                         PIN_CONFIG_PUL_UP |
                         PIN_CONFIG_DRV_LEVEL1);
}

/**
 * @brief  config iomux for uart0
 */
void uart0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C7 |  // UART0_RX
                         GPIO_PIN_D0 |  // UART0_TX
                         GPIO_PIN_D1 |  // UART0_CTSN
                         GPIO_PIN_D2,   // UART0_RTSN
                         PIN_CONFIG_MUX_FUNC1);
}

#ifdef RT_USING_PMIC_RK816
/**
 * @brief  Config iomux for PMIC
 */
void pmic_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D6,   // PMIC_INT
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_D6,   // PMIC_INT
                         PIN_CONFIG_PUL_UP);
}
#endif

/**
 * @brief  Config iomux for AUDIO
 */
void audio_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A4,   // AUDIO_LOUT_M1
                         PIN_CONFIG_MUX_FUNC5);
}

/**
 * @brief  Config iomux for WIFI/BT
 */
void wifi_spi_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A0 |
                         GPIO_PIN_A1 |
                         GPIO_PIN_A2 |
                         GPIO_PIN_A3,
                         PIN_CONFIG_MUX_FUNC3);

    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_SPI_MST2_IOMUX_SEL_MASK,
                      0 << GRF_SOC_CON5_GRF_CON_SPI_MST2_IOMUX_SEL_SHIFT);
}

/**
 * @brief  Config iomux for rk2108 evb board
 */
void rt_hw_iomux_config(void)
{
    uart1_m1_iomux_config();

    pdm_input_iomux_config();

    sfc0_iomux_config();

#ifdef DSP_JTAG_ENABLE
    dsp_jtag_iomux_config();
#else
    sdio_iomux_config();
#endif

    key_ctrl_iomux_config();

#ifdef M4_JTAG_ENABLE
    m4_jtag_iomux_config();
#else
    uart0_iomux_config();
#endif

    i2c0_m1_iomux_config();

#ifdef RT_USING_I2STDM1
    i2s1_output_iomux_config();
#endif

#ifdef RT_USING_PMIC_RK816
    pmic_iomux_config();
#endif

    spi2_m0_iomux_config();

    audio_iomux_config();

    wifi_spi_iomux_config();
}
/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
