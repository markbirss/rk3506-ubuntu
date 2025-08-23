/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-02-07     Cliff Chen   first implementation
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
 * @brief  Config iomux for UART0
 */
void uart0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B1 |  // UART0_TX
                         GPIO_PIN_B2,   // UART0_RX
                         PIN_CONFIG_MUX_FUNC1);
}

/**
 * @brief  Config iomux for UART2
 */
void uart2_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B4,   // UART2_TX_AUDIO_DEBUG
                        RMIO_UART2_TX_RM1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B5,   // UART2_RX_AUDIO_DEBUG
                        RMIO_UART2_RX_RM1);
}


/**
 * @brief  Config all SAIX_MCLK as OUTPUT
 */
void sai_mclkout_config_all(void)
{
    WRITE_REG_MASK_WE(GRF->SOC_CON1, HAL_GENMASK(7, 0), HAL_GENMASK(7, 0));
}

void rt_hw_iomux_config(void)
{
    i2c2_iomux_config();
    uart0_iomux_config();
    uart2_iomux_config();
    spi0_m1_iomux_config();
    sai_mclkout_config_all();
}

/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Drive
