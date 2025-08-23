/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    iomux.c
  * @version V0.1
  * @brief   iomux for Koala evb
  *
  * Change Logs:
  * Date           Author          Notes
  * 2019-08-08     Andy Yan      first implementation
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

static void set_i2c2_iomux(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A2 |
                         GPIO_PIN_A3,
                         PIN_CONFIG_MUX_FUNC1);
}

#ifdef RT_USING_I2STDM0
/**
 * @brief  Config iomux for i2stdm0
 */
static void set_i2stdm0_iomux(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C0 |
                         GPIO_PIN_C1 |
                         GPIO_PIN_C2 |
                         GPIO_PIN_C3 |
                         GPIO_PIN_C4 |
                         GPIO_PIN_C5 |
                         GPIO_PIN_C6 |
                         GPIO_PIN_C7,
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

#ifdef RT_USING_PDM0
/**
 * @brief  Config iomux for pdm0
 */
static void set_pdm0_iomux(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C1 |
                         GPIO_PIN_C3 |
                         GPIO_PIN_C5 |
                         GPIO_PIN_C7,
                         PIN_CONFIG_MUX_FUNC2);
}
#endif

static void set_sfc0_iomux(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A0 |
                         GPIO_PIN_A1 |
                         GPIO_PIN_A2 |
                         GPIO_PIN_A3 |
                         GPIO_PIN_A4 |
                         GPIO_PIN_A5,
                         PIN_CONFIG_MUX_FUNC1);

    WRITE_REG_MASK_WE(GRF->GPIO1A_E, GRF_GPIO1A_E_GPIO1A5_E_MASK,
                      2 << GRF_GPIO1A_E_GPIO1A5_E_SHIFT);
}

/**
 * @brief  Config iomux for koala evb
 */
void rt_hw_iomux_config(void)
{
    set_i2c2_iomux();
#ifdef RT_USING_I2STDM0
    set_i2stdm0_iomux();
#endif
#ifdef RT_USING_PDM0
    set_pdm0_iomux();
#endif
    set_sfc0_iomux();
}
/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
