/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    iomux.c
  * @version V0.1
  * @brief   iomux for rk3562_mcu evb
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
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B7 |
                         GPIO_PIN_C0,
                         PIN_CONFIG_MUX_FUNC3);
#endif
#ifdef RT_USING_UART7
    /* uart7_m1 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B3 |
                         GPIO_PIN_B4,
                         PIN_CONFIG_MUX_FUNC3);
#endif
}

/**
 * @brief  Config iomux for JTAG
 */
void jtag_iomux_config(void)
{
}

#ifdef RT_USING_PWM
#ifdef RT_USING_PWM0
/**
 * @brief  Config iomux for PWM Controller0
 */
void pwm0_m0_iomux_config(void)
{
    /* PWM0 M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C3,
                         PIN_CONFIG_MUX_FUNC2);
    /* PWM1 M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C4,
                         PIN_CONFIG_MUX_FUNC2);
    /* PWM2 M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C5,
                         PIN_CONFIG_MUX_FUNC2);
    /* PWM3 M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A7,
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

#ifdef RT_USING_PWM1
/**
 * @brief  Config iomux for PWM Controller1
 */
void pwm1_m0_iomux_config(void)
{
    /* PWM4 M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B7,
                         PIN_CONFIG_MUX_FUNC2);
    /* PWM5 M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C2,
                         PIN_CONFIG_MUX_FUNC2);
    /* PWM6 M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C1,
                         PIN_CONFIG_MUX_FUNC2);
    /* PWM7 M0 */
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C0,
                         PIN_CONFIG_MUX_FUNC2);
}
#endif
#endif


/**
 * @brief  Config iomux for rk2108 evb board
 */
void rt_hw_iomux_config(void)
{
#ifdef RT_USING_I2C0
    i2c0_m0_iomux_config();
#endif
#ifdef RT_USING_SPI0
    spi0_m1_iomux_config();
#endif
    uart_iomux_config();

#ifdef RT_USING_PWM
#ifdef RT_USING_PWM0
    pwm0_m0_iomux_config();
#endif
#ifdef RT_USING_PWM1
    pwm1_m0_iomux_config();
#endif
#endif
}

/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
