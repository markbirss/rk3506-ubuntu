/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    iomux.c
  * @version V0.1
  * @brief   iomux for rk2108d_rk3399 evb
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
 * @brief  Config iomux for rk2108d_rk3399 evb board
 */

/**
 * @brief  Config iomux for touch panel
 */
static void touch_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D3 |  // TP_RESETN (Touch Panel Reset)
                         GPIO_PIN_D4,   // TP_INTN (Touch Panel Interrupt)
                         PIN_CONFIG_MUX_FUNC1);
}

/**
 * @brief  Config iomux for LCDC
 */
void lcdc_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A0 |  // LCD_IN_RESETN (LCD Reset from AP)
                         GPIO_PIN_A1 |  // LCD_IN_TE (LCD TE from AP)
                         GPIO_PIN_A2 |  // LCD_OUT_RESETN (Reset to LCD)
                         GPIO_PIN_A3 |  // LCD_OUT_TE (TE to LCD)
                         GPIO_PIN_A4,   // LDO_OUT_PWR_EN (Power Enable to External LDO)
                         PIN_CONFIG_MUX_FUNC1);

#ifdef RT_HW_LCD_GPIO_TRIGGER_PIN
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C0,   // gpio1_c0 connect to panel TE pin and used to trigger new frame
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_C0, GPIO_IN);
#else
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A1,   // LCD_IN_TE (LCD TE from AP)
                         PIN_CONFIG_MUX_FUNC1);
#endif

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A2,   // LCDC_RST
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_A2, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_A2, GPIO_HIGH);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C0,   // LCDC_en
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_C0, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_C0, GPIO_HIGH);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A4,
                         PIN_CONFIG_MUX_FUNC0);//PWM3

    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_A4, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_A4, GPIO_HIGH);
}

void rt_hw_iomux_config(void)
{
    uart1_m0_iomux_config();

    sfc0_iomux_config();

    sfc1_iomux_config();

#ifdef M4_JTAG_ENABLE
    m4_jtag_iomux_config();
#endif

    lcdc_iomux_config();

    i2c0_m0_iomux_config();
    touch_iomux_config();

#ifdef RT_USING_SPINAND_SPI_HOST
    spi1_m0_iomux_config();
#endif
}
/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
