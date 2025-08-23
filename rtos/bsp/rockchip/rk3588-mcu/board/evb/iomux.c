/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    iomux.c
  * @version V0.1
  * @brief   iomux for rk3588_mcu evb
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
    /* uart5_m0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_D4 | GPIO_PIN_D5,
                         PIN_CONFIG_MUX_FUNC10);
#endif
}

void i2c_iomux_config(void)
{
#ifdef RT_USING_I2C6
    /* i2c6_m0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C7 | GPIO_PIN_D0,
                         PIN_CONFIG_MUX_FUNC9);
#endif
}

void pwm_iomux_config(void)
{
#ifdef RT_USING_PWM2
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_D1,
                         PIN_CONFIG_MUX_FUNC11);
#endif
}

void gpio_iomux_config(void)
{
#ifdef RT_USING_AUTO_TEST
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_D2 | GPIO_PIN_D3,
                         PIN_CONFIG_MUX_FUNC0);
#endif

#ifdef HAL_GPIO_VIRTUAL_MODEL_FEATURE_ENABLED
#ifdef RT_USING_AUTO_TEST
    HAL_GPIO_SetVirtualModel(GPIO4, 0xffffffff, GPIO_VIRTUAL_MODEL_OS_A);
    HAL_GPIO_SetVirtualModel(GPIO4,
                             GPIO_PIN_D2 | GPIO_PIN_D3,
                             GPIO_VIRTUAL_MODEL_OS_B);
    HAL_GPIO_EnableVirtualModel(GPIO4);
#endif
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
    i2c_iomux_config();
    pwm_iomux_config();
    gpio_iomux_config();
}

/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
