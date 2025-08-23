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

#ifdef RT_USING_I2C0
/**
 * @brief  Config iomux for I2C0
 */
void i2c0_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A7,   // I2C0_SCL
                        RMIO_I2C0_SCL);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A6,   // I2C0_SDA
                        RMIO_I2C0_SDA);
}
#endif

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
 * @brief  Config iomux for SPDIF
 */
void spdif_rx1_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D4,   // SPDIF_RX1
                        RMIO_SPDIF_RX1);
}

/**
 * @brief  Config iomux for uart1
 */
void uart1_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_C2,   // UART1_TX
                        RMIO_UART1_TX_RM1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_C3,   // UART1_RX
                        RMIO_UART1_RX_RM1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_C4,   // UART1_RTSN
                        RMIO_UART1_RTSN_RM1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_C5,   // UART1_CTSN
                        RMIO_UART1_CTSN_RM1);
}

/**
 * @brief  Config iomux for sdmmc
 */
void sdmmc_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_C7 |  // SDMMC_D0
                         GPIO_PIN_D0 |  // SDMMC_CLK
                         GPIO_PIN_D1 |  // SDMMC_CMD
                         GPIO_PIN_C6 |  // SDMMC_D1
                         GPIO_PIN_D2 |  // SDMMC_D3
                         GPIO_PIN_D3,   // SDMMC_D2
                         PIN_CONFIG_MUX_FUNC1);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_D1 |  // SDMMC_CMD
                         GPIO_PIN_C7 |  // SDMMC_D0
                         GPIO_PIN_C6 |  // SDMMC_D1
                         GPIO_PIN_D2 |  // SDMMC_D3
                         GPIO_PIN_D3,   // SDMMC_D2
                         PIN_CONFIG_PUL_UP |
                         PIN_CONFIG_DRV_LEVEL1);

    //*((unsigned long *)0x50180134) = 0x40004;
}

/**
 * @brief  Config iomux for SAI0
 */
void sai0_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A0,   // SAI0_I2S_MCLK
                        RMIO_SAI0_MCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A1,   // SAI0_I2S_SCLK
                        RMIO_SAI0_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A2,   // SAI0_I2S_LRCK
                        RMIO_SAI0_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A5,   // SAI0_I2S_SDI0
                        RMIO_SAI0_SDI0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A7,   // SAI0_I2S_SDI1
                        RMIO_SAI0_SDI1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B1,   // SAI0_I2S_SDI2
                        RMIO_SAI0_SDI2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B4,   // SAI0_I2S_SDI3
                        RMIO_SAI0_SDI3);
}

/**
 * @brief  Config iomux for SAI4
 */
void sai4_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C4,   // SAI4_I2S_MCLK
                        RMIO_SAI4_MCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C5,   // SAI4_I2S_SCLK
                        RMIO_SAI4_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C6,   // SAI4_I2S_LRCK
                        RMIO_SAI4_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C7,   // SAI4_I2S_SDI0
                        RMIO_SAI4_SDI0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D0,   // SAI4_I2S_SDO0
                        RMIO_SAI4_SDO0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D1,   // SAI4_I2S_SDO1
                        RMIO_SAI4_SDO1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D2,   // SAI4_I2S_SDO2
                        RMIO_SAI4_SDO2);
}

/**
 * @brief  Config iomux for SAI7
 */
void sai7_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C0,   // SAI7_PCM_SCLK
                        RMIO_SAI7_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C1,   // SAI7_PCM_LRCK
                        RMIO_SAI7_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C2,   // SAI7_PCM_OUT
                        RMIO_SAI7_SDO0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C3,   // SAI7_PCM_IN
                        RMIO_SAI7_SDI0);
}

/**
 * @brief  Config iomux for audio_connector
 */
void audio_connector_iomux_config(void)
{

}

#ifdef RT_USING_GMAC0
/**
 * @brief  Config iomux for RMII
 */
void rmii_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A0 |  // ETH_RMII_CRSDV
                         GPIO_PIN_A1 |  // ETH_RMII_MDIO
                         GPIO_PIN_A2 |  // ETH_RMII_MDC
                         GPIO_PIN_A3 |  // ETH_RMII_TXEN
                         GPIO_PIN_A4 |  // ETH_RMII_TXD1
                         GPIO_PIN_A5 |  // ETH_RMII_TXD0
                         GPIO_PIN_A6 |  // ETH_RMII_CLK
                         GPIO_PIN_A7 |  // ETH_RMII_RXD1
                         GPIO_PIN_B0,   // ETH_RMII_RXD0
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

/**
 * @brief  Config iomux for emmc(multiplexing with fspi)
 */
void emmc_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A0 |  // EMMC_D5
                         GPIO_PIN_A1 |  // EMMC_D3
                         GPIO_PIN_A2 |  // EMMC_D4
                         GPIO_PIN_A3 |  // EMMC_D0
                         GPIO_PIN_A4 |  // EMMC_D1
                         GPIO_PIN_A6 |  // EMMC_D2
                         GPIO_PIN_A7 |  // EMMC_D7
                         GPIO_PIN_B0 |  // EMMC_D6
                         GPIO_PIN_B1 |  // EMMC_CMD
                         GPIO_PIN_B3,   // EMMC_CLK
                         PIN_CONFIG_MUX_FUNC1);

    HAL_PINCTRL_SetParam(GPIO_BANK1,
                         GPIO_PIN_A0 |  // EMMC_D5
                         GPIO_PIN_A1 |  // EMMC_D3
                         GPIO_PIN_A2 |  // EMMC_D4
                         GPIO_PIN_A3 |  // EMMC_D0
                         GPIO_PIN_A4 |  // EMMC_D1
                         GPIO_PIN_A6 |  // EMMC_D2
                         GPIO_PIN_A7 |  // EMMC_D7
                         GPIO_PIN_B0 |  // EMMC_D6
                         GPIO_PIN_B1 |  // EMMC_CMD
                         GPIO_PIN_B3,   // EMMC_CLK
                         PIN_CONFIG_PUL_UP |
                         PIN_CONFIG_DRV_LEVEL1);
}

/**
 * @brief  Config iomux for pwm0
 */
void pwm0_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A2,   // PWM0_1 IR
                        RMIO_PWM0_CH1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A3,   // PWM0_3
                        RMIO_PWM0_CH3);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A5,   // PWM0_2
                        RMIO_PWM0_CH2);
}

void uart2_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B3,   // UART2_TX_AUDIO_DEBUG
                        RMIO_UART2_TX_RM1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B4,   // UART2_RX_AUDIO_DEBUG
                        RMIO_UART2_RX_RM1);

    HAL_PINCTRL_SetParam(GPIO_BANK3, GPIO_PIN_B4, PIN_CONFIG_PUL_UP);
}

/**
 * @brief  Config iomux for usb host
 */
void usb_host_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_A6,
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A6, // USB_HOST_PWREN
                         PIN_CONFIG_PUL_UP);

    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_A6, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO3, GPIO_PIN_A6, GPIO_HIGH);
}

void rt_hw_iomux_config(void)
{
#ifdef RK2118_CPU_CORE1
    return;
#endif
    sai_mclkout_config_all();
    uart1_iomux_config();
#ifdef RT_USING_I2C0
    i2c0_iomux_config();
#endif
    // conflict with SDMMC_D1/2/3
    mcu_jtag_m0_iomux_config();
    uart0_iomux_config();
    sai4_iomux_config();
    sai7_iomux_config();
    dsp_jtag_iomux_config();
#ifdef RT_USING_SNOR
    fspi0_iomux_config();
#endif
#ifdef RT_USING_SDIO1
    emmc_iomux_config();
#endif
    pwm0_iomux_config();
#ifdef RT_USING_VOP
    lcdc_iomux_config();
#else
    usb_host_iomux_config();
#endif
    uart2_iomux_config();
#ifdef RT_USING_SDIO0
    // conflict with JTAG_MCU_SWO/TMSTCK
    sdmmc_iomux_config();
#endif
#ifdef RT_USING_IT6632X
    i2c1_iomux_config();
    sai0_iomux_config();
    spdif_rx1_iomux_config();
#endif
#ifdef RT_USING_GMAC0
    // conflict with dsp_jtag
    rmii_iomux_config();
#endif
}

/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
