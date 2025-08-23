/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    iomux.c
  * @version V0.1
  * @brief   iomux for rk3576 mcu evb
  *
  * Change Logs:
  * Date           Author          Notes
  * 2020-10-15     Cliff.Chen      first implementation
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
#include "drivers/pin.h"
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
 * @brief  Config iomux for UART
 */
void uart_iomux_config(void)
{
#ifdef RT_USING_UART5
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A4,
                         PIN_CONFIG_MUX_FUNC9);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A5,
                         PIN_CONFIG_MUX_FUNC9);
#endif
#ifdef RT_USING_UART7
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A0,
                         PIN_CONFIG_MUX_FUNC10);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A1,
                         PIN_CONFIG_MUX_FUNC10);
#endif
#ifdef RT_USING_UART8
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A6,
                         PIN_CONFIG_MUX_FUNC9);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A7,
                         PIN_CONFIG_MUX_FUNC9);
#endif
}

/**
 * @brief  Config iomux for JTAG
 */
void jtag_iomux_config(void)
{
}

/**
 * @brief  Config iomux for rk2108 evb board
 */
RT_WEAK void rt_hw_iomux_config(void)
{
    uart_iomux_config();
}

/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
