/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    iomux.c
  * @version V0.1
  * @brief   iomux for rk1808 evb
  *
  * Change Logs:
  * Date           Author          Notes
  * 2019-05-25     Jianqun xu      first implementation
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
static void set_sdio_iomux(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_A6 |
                         GPIO_PIN_A7 |
                         GPIO_PIN_B0 |
                         GPIO_PIN_B1 |
                         GPIO_PIN_B2 |
                         GPIO_PIN_B3,
                         PIN_CONFIG_MUX_FUNC1);
}

#ifdef RT_USING_SPI_DISPLAY
static void set_spi0_iomux(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B4 |
                         GPIO_PIN_B6 |
                         GPIO_PIN_B7,
                         PIN_CONFIG_MUX_FUNC1);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B5,
                         PIN_CONFIG_MUX_FUNC0);
}
#endif

/**
 * @brief  Config iomux for board of rk1808 evb
 */
void rt_hw_iomux_config(void)
{
    set_sdio_iomux();

#ifdef RT_USING_SPI_DISPLAY
    set_spi0_iomux();
#endif
}

/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
