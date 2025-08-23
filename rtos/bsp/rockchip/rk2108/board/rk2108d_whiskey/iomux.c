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
#include "board.h"

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
                         GPIO_PIN_A3 |  // TOUCH_INT
                         GPIO_PIN_B0,   // TOUCH_RST
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A3,
                         PIN_CONFIG_PUL_NORMAL |
                         PIN_CONFIG_DRV_LEVEL0);
}

/**
 * @brief  Config iomux for LCDC
 */
void lcdc_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A0 |  // LCD_IN_RESETN (LCD Reset from AP)
                         GPIO_PIN_A2 |  // LCD_OUT_RESETN (Reset to LCD)
                         GPIO_PIN_A3 |  // LCD_OUT_TE (TE to LCD)
                         GPIO_PIN_A4,   // LDO_OUT_PWR_EN (Power Enable to External LDO)
                         PIN_CONFIG_MUX_FUNC1);

#ifdef RT_HW_LCD_GPIO_TRIGGER_PIN
    /* use gpio interrupt to trigger new frame */
    HAL_PINCTRL_SetIOMUX(RT_HW_LCD_GPIO_TRIGGER_BANK,
                         RT_HW_LCD_GPIO_TRIGGER_GPIO,
                         PIN_CONFIG_MUX_FUNC0);
#else
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A1,  // LCD_IN_TE (LCD TE from AP)
                         PIN_CONFIG_MUX_FUNC1);
#endif

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_C1,   // LCDC_RST
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_C1, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_C1, GPIO_HIGH);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D5,   // LCDC_1V8_en
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_D5, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D5, GPIO_LOW);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D6,   // LCDC_3V3_en
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_D6, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D6, GPIO_HIGH);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A5,
                         PIN_CONFIG_MUX_FUNC0);//PWM3
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_A5, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_A5, GPIO_HIGH);
}

/**
 * @brief  Config iomux for SPI1_m0
 */
void spi1_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C2 |  // SPI_MST1_CS_M0
                         GPIO_PIN_C3 |  // SPI_MST1_CLK_M0
                         GPIO_PIN_C4 |  // SPI_MST1_MISO_M0
                         GPIO_PIN_C5,   // SPI_MST1_MOSI_M0
                         PIN_CONFIG_MUX_FUNC1);

    /* set SPI master 1 IOMUX selection to M0 */
    WRITE_REG_MASK_WE(GRF->SOC_CON5,
                      GRF_SOC_CON5_GRF_CON_SPI_MST1_IOMUX_SEL_MASK,
                      (0 << GRF_SOC_CON5_GRF_CON_SPI_MST1_IOMUX_SEL_SHIFT));

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A5,    // SPI_NAND_EN
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_A5, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A5, GPIO_LOW);
}

/**
 * @brief  Config iomux for AUDIO PWM M0
 */
void audio_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D1,   // AUDIO_LOUT_M0
                         PIN_CONFIG_MUX_FUNC3);
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D2,   // AUDIO_ROUT_M0
                         PIN_CONFIG_MUX_FUNC3);
}

void rt_hw_iomux_config(void)
{
    pdm_input_iomux_config();

    uart1_m2_iomux_config();

    sfc0_iomux_config();

    sfc1_iomux_config();

#ifdef M4_JTAG_ENABLE
    m4_jtag_iomux_config();
#endif

    lcdc_iomux_config();

    i2c0_m1_iomux_config();
    touch_iomux_config();

#ifdef RT_USING_AUDIOPWM
    audio_iomux_config();
#endif
#ifdef RT_USING_SPINAND_SPI_HOST
    spi1_m0_iomux_config();
#endif
}
/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
