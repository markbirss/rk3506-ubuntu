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
 * @brief  Config iomux for I2S
 */
RT_UNUSED void i2s1_input_iomux_config(void)
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
                      (0x0 << GRF_SOC_CON4_GRF_CON_I2S1_SDI2_FROM_IO_SHIFT));
}

/**
 * @brief  Config iomux for I2S1
 */
RT_UNUSED void i2s1_output_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A4 |  // I2S1_MCLK_MUX0
                         GPIO_PIN_A6 |  // I2S1_OUT_SCLK_MUX0
                         GPIO_PIN_A7 |  // I2S1_OUT_LRCK_MUX0
                         GPIO_PIN_B0,   // I2S1_OUT_SDO0_MUX0
                         PIN_CONFIG_MUX_FUNC4);
}

/**
 * @brief  Config iomux for rk2108 evb board
 */
void rt_hw_iomux_config(void)
{
    pdm_input_iomux_config();

    sfc0_iomux_config();

#ifdef DSP_JTAG_ENABLE
    dsp_jtag_iomux_config();
#else
    sdio_iomux_config();
#endif

    uart2_iomux_config();

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
}
/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
