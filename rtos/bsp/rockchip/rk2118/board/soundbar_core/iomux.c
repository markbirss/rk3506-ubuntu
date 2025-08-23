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
 * @brief  Config iomux for USRT2
 */
void uart2_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK2,
                        GPIO_PIN_A1,   // UART2_TX
                        RMIO_UART2_TX_RM2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK2,
                        GPIO_PIN_A0,   // UART2_RX
                        RMIO_UART2_RX_RM2);
}

/**
 * @brief  Config iomux for SPI1
 */
void spi1_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A2,   // SPI1_MOSI
                        RMIO_SPI1_MOSI);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A3,   // SPI1_CLK
                        RMIO_SPI1_CLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A4,   // SPI1_CSN0
                        RMIO_SPI1_CSN0);
}

/**
 * @brief  Config iomux for I2C1
 */
void i2c1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_A3 |  // I2C1_SCL
                         GPIO_PIN_A4,   // I2C1_SDA
                         PIN_CONFIG_MUX_FUNC1);
}

/**
 * @brief  Config iomux for I2C2
 */
void i2c2_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B4,   // I2C2_SCL
                        RMIO_I2C2_SCL_RM1);
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B5,   // I2C2_SDA
                        RMIO_I2C2_SDA_RM1);
}

/**
 * @brief  Config iomux for I2C3
 */
void i2c3_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_D2 |  // I2C3_SCL
                         GPIO_PIN_D3,   // I2C3_SDA
                         PIN_CONFIG_MUX_FUNC1);
}

/**
 * @brief  Config iomux for I2C4
 */
void i2c4_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_D4 |  // I2C4_SCL
                         GPIO_PIN_D5,   // I2C4_SDA
                         PIN_CONFIG_MUX_FUNC1);
}

/**
 * @brief  Config iomux for SPDIF
 */
void spdif_rx1_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D0,   // SPDIF_RX1
                        RMIO_SPDIF_RX1);
}

/**
 * @brief  Config iomux for PWM1
 */
void pwm1_ch3_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B0,   // PWM1_CH3
                        RMIO_PWM1_CH3);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_B0,
                         PIN_CONFIG_PUL_UP);
}

/**
 * @brief  Config iomux for SAI0
 */
void sai0_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A6,   // SAI0_I2S_LRCK
                        RMIO_SAI0_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B0,   // SAI0_MCLK
                        RMIO_SAI0_MCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B1,   // SAI0_I2S_SCLK
                        RMIO_SAI0_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B3,   // SAI0_I2S_SDO0
                        RMIO_SAI0_SDO0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B4,   // SAI0_I2S_SDO1
                        RMIO_SAI0_SDO1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B5,   // SAI0_I2S_SDO2
                        RMIO_SAI0_SDO2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B7,   // SAI0_I2S_SDO3
                        RMIO_SAI0_SDO3);
}

/**
 * @brief  Config iomux for SAI1
 */
void sai1_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A7,   // SAI1_I2S_SDO0
                        RMIO_SAI1_SDO0);
}

/**
 * @brief  Config iomux for SAI4
 */
void sai4_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C0,   // SAI4_I2S_SCLK
                        RMIO_SAI4_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C1,   // SAI4_I2S_SDI0
                        RMIO_SAI4_SDI0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C2,   // SAI4_I2S_SDO1
                        RMIO_SAI4_SDI1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C3,   // SAI4_I2S_SDO2
                        RMIO_SAI4_SDI2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C4,   // SAI4_I2S_SDO3
                        RMIO_SAI4_SDI3);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C5,   // SAI4_I2S_LRCK
                        RMIO_SAI4_LRCK);
}

#ifdef RT_USING_IT6632X
void it6632x_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(IT6632X_RST_GPIO_BANK,
                         IT6632X_RST_GPIO,   // RST
                         IT6632X_RST_PIN_FUNC_GPIO);

    HAL_PINCTRL_SetIOMUX(IT6632X_INT_GPIO_BANK,
                         IT6632X_INT_GPIO,   // INT
                         IT6632X_INT_PIN_FUNC_GPIO);

    HAL_PINCTRL_SetIOMUX(IT6632X_RXMUTE_GPIO_BANK,
                         IT6632X_RXMUTE_GPIO,   // RXMUTE
                         IT6632X_RXMUTE_PIN_FUNC_GPIO);
}
#endif

/**
 * @brief  Config iomux for rk2118 soundbar core board
 */
void rt_hw_iomux_config(void)
{
    sai_mclkout_config_all();
    dsp_jtag_iomux_config();
    fspi0_iomux_config();
    uart2_iomux_config();
    uart0_iomux_config();
    mcu_jtag_m0_iomux_config();
    spi1_iomux_config();
    i2c1_iomux_config();
    i2c2_iomux_config();
    i2c3_iomux_config();
    i2c4_iomux_config();
    pwm1_ch3_iomux_config();
    spdif_rx1_iomux_config();
    sai0_iomux_config();
    sai1_iomux_config();
    sai4_iomux_config();
#ifdef RT_USING_IT6632X
    it6632x_iomux_config();
#endif
}

/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
