/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    iomux.c
  * @version V0.1
  * @brief   iomux for rk2108 evb
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
 * @brief  Config iomux for fspi1
 */
RT_UNUSED static void iomux_config_fspi1(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B0 |  // FSPI1_CLK
                         GPIO_PIN_B2 |  // FSPI1_CS0
                         GPIO_PIN_B4 |  // FSPI1_D0
                         GPIO_PIN_B6 |  // FSPI1_D2
                         GPIO_PIN_B7 |  // FSPI1_D1
                         GPIO_PIN_C0,   // FSPI1_D3
                         PIN_CONFIG_MUX_FUNC2);
    WRITE_REG_MASK_WE(GRF->SOC_CON0, GRF_SOC_CON0_GRF_CON_HYPERX8_SFC1_SEL_MASK,
                      GRF_SOC_CON0_GRF_CON_HYPERX8_SFC1_SEL_MASK);
}

/**
 * @brief  Config iomux for M4 Jtag
 */
void iomux_config_m4jtag(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A0 |  // M4F_JTAG_TCK
                         GPIO_PIN_A1,   // M4F_JTAG_TMS
                         PIN_CONFIG_MUX_FUNC6);
}

/**
 * @brief  Config iomux for UART
 */
void iomux_config_uart0_m0(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B4 |  // UART0_CTSN_M0
                         GPIO_PIN_B5 |  // UART0_RTSN_M0
                         GPIO_PIN_B6 |  // UART0_RX_M0
                         GPIO_PIN_B7,   // UART0_TX_M0
                         PIN_CONFIG_MUX_FUNC2);
}

void iomux_config_uart0_m1(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C4 |  // UART0_CTSN_M1
                         GPIO_PIN_C5 |  // UART0_RTSN_M1
                         GPIO_PIN_C6 |  // UART0_RX_M1
                         GPIO_PIN_C7,   // UART0_TX_M1
                         PIN_CONFIG_MUX_FUNC3);
}

void iomux_config_uart1_m0(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C0 |  // UART1_CTSN_M0
                         GPIO_PIN_C1 |  // UART1_RTSN_M0
                         GPIO_PIN_C2 |  // UART1_RX_M0
                         GPIO_PIN_C3,   // UART1_TX_M0
                         PIN_CONFIG_MUX_FUNC3);
}

void iomux_config_uart1_m1(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A4 |  // UART1_CTSN_M1
                         GPIO_PIN_A5 |  // UART1_RTSN_M1
                         GPIO_PIN_A6 |  // UART1_RX_M1
                         GPIO_PIN_A7,   // UART1_TX_M1
                         PIN_CONFIG_MUX_FUNC3);
}

void iomux_config_uart1_m2(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D0 |  // UART1_RX_M2
                         GPIO_PIN_D1,   // UART1_TX_M2
                         PIN_CONFIG_MUX_FUNC4);
}

void iomux_config_uart2_m0(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D2 |  // UART2_CTSN_M0
                         GPIO_PIN_D3 |  // UART2_RTSN_M0
                         GPIO_PIN_D4 |  // UART2_RX_M0
                         GPIO_PIN_D5,   // UART2_TX_M0
                         PIN_CONFIG_MUX_FUNC4);
}

void iomux_config_uart2_m1(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B0 |  // UART2_CTSN_M1
                         GPIO_PIN_B1 |  // UART2_RTSN_M1
                         GPIO_PIN_B2 |  // UART2_RX_M1
                         GPIO_PIN_B3,   // UART2_TX_M1
                         PIN_CONFIG_MUX_FUNC3);
}

RT_UNUSED static void iomux_config_hyperbus(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B0 | // HypweBus_CKP
                         GPIO_PIN_B1 | // HypweBus_CKN
                         GPIO_PIN_B2 | // HypweBus_CSn
                         GPIO_PIN_B3 | // HypweBus_RWDS
                         GPIO_PIN_B4 | // HypweBus_D0
                         GPIO_PIN_B5 | // HypweBus_D1
                         GPIO_PIN_B6 | // HypweBus_D2
                         GPIO_PIN_B7 | // HypweBus_D3
                         GPIO_PIN_C0 | // HypweBus_D4
                         GPIO_PIN_C1 | // HypweBus_D5
                         GPIO_PIN_C2 | // HypweBus_D6
                         GPIO_PIN_C3,  // HypweBus_D7
                         PIN_CONFIG_MUX_FUNC1);
}

/**
 * @brief  Config iomux for SPI M1
 */
RT_UNUSED static void iomux_config_spi(void)
{
#ifdef RT_USING_SPI0
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C0 | // SPI0_CS0
                         GPIO_PIN_C1 | // SPI0_CLK
                         GPIO_PIN_C2 | // SPI0_MOSI
                         GPIO_PIN_C3,  // SPI0_MIMO
                         PIN_CONFIG_MUX_FUNC4);
    WRITE_REG_MASK_WE(GRF->SOC_CON1,
                      GRF_SOC_CON1_GRF_SPI0_MULTI_IOFUNC_SRC_SEL_MASK,
                      1 << GRF_SOC_CON1_GRF_SPI0_MULTI_IOFUNC_SRC_SEL_SHIFT);
#endif
#ifdef RT_USING_SPI1
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B0 | // SPI1_CS0
                         GPIO_PIN_B1 | // SPI1_CLK
                         GPIO_PIN_B2 | // SPI1_MOSI
                         GPIO_PIN_B3,  // SPI1_MIMO
                         PIN_CONFIG_MUX_FUNC4);
    WRITE_REG_MASK_WE(GRF->SOC_CON1,
                      GRF_SOC_CON1_GRF_SPI1_MULTI_IOFUNC_SRC_SEL_MASK,
                      1 << GRF_SOC_CON1_GRF_SPI1_MULTI_IOFUNC_SRC_SEL_SHIFT);
#endif
    WRITE_REG_MASK_WE(GRF->SOC_CON15,
                      GRF_SOC_CON15_GRF_SARADC_IEN_MASK,
                      0);
}

/**
 * @brief  Config iomux for rk2206 evb board
 */
void rt_hw_iomux_config(void)
{

#ifdef M4_JTAG_ENABLE
    iomux_config_m4jtag();
#endif
    iomux_config_uart0_m0();

#ifdef RT_USING_HYPERPSRAM
    iomux_config_hyperbus();
#endif

#ifdef RT_USING_QPIPSRAM_FSPI_HOST
    iomux_config_fspi1();
#endif

#ifdef RT_USING_SPI
    iomux_config_spi();
#endif
}
/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
