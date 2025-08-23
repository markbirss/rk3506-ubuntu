/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    iomux.c
  * @version V0.1
  * @brief   iomux for rk2106 evb
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
 * @brief  Config iomux for I2C0
 */
void i2c0_iomux_config(void)
{
}

/**
 * @brief  Config iomux for I2C1
 */
void i2c1_iomux_config(void)
{
}

/**
 * @brief  Config iomux for UART
 */
void uart0a_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_B4 |  // UART0A_RX
                         GPIO_PIN_B5,   // UART0A_TX
                         PIN_CONFIG_MUX_FUNC1);
}

/**
 * @brief  Config iomux for UART
 */
void uart1a_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_C1 |  // UART1A_RX
                         GPIO_PIN_C0,   // UART1A_TX
                         PIN_CONFIG_MUX_FUNC1);
}

/**
 * @brief  Config iomux for JTAG
 */
void jtag_iomux_config(void)
{
}

/**
 * @brief  Config iomux for SPI0
 */
RT_WEAK RT_UNUSED void spi0_iomux_config(void)
{
}


/**
 * @brief  Config iomux for SPI1
 */
RT_WEAK RT_UNUSED void spi1_iomux_config(void)
{
}

/**
 * @brief  Config iomux for PWM0
 */
RT_WEAK RT_UNUSED void pwm0_iomux_config(void)
{
}

/**
 * @brief  Config iomux for rk2108 evb board
 */
RT_WEAK RT_UNUSED void rt_hw_iomux_config(void)
{
    uart1a_iomux_config();
}

/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
