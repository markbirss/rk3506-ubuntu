/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    iomux.c
  * @version V0.1
  * @brief   iomux for rk2108 avb
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
 * @brief  Config iomux for PCM
 */
RT_UNUSED void pcm_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D3 |  // PCM_CLK_MUX0
                         GPIO_PIN_D4 |  // PCM_SYNC_MUX0
                         GPIO_PIN_D5 |  // PCM_IN_MUX0
                         GPIO_PIN_D6,   // PCM_OUT_MUX0
                         PIN_CONFIG_MUX_FUNC4);

}

/**
 * @brief  Config iomux for rk2108 avb board
 */
void rt_hw_iomux_config(void)
{
    i2s1_input_iomux_config();

    sfc0_iomux_config();

    sdio_iomux_config();

    uart2_iomux_config();

    key_ctrl_iomux_config();

    uart0_iomux_config();
#ifdef RT_USING_I2STDM1
    i2s1_output_iomux_config();
#endif
    i2c1_m1_iomux_config();
}
/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
