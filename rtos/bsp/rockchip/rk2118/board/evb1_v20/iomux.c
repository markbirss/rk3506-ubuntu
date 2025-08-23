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

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A6 |
                         GPIO_PIN_A7,   //HI-Z
                         PIN_CONFIG_PUL_NORMAL);
}
#endif

#ifdef RT_USING_I2C2
/**
 * @brief  Config iomux for I2C2
 */
void i2c2_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_C1,       // I2C2_SCL for it6632x
                        RMIO_I2C2_SCL_RM1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_C0,       // I2C2_SDA for it6632x
                        RMIO_I2C2_SDA_RM1);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_C0 |
                         GPIO_PIN_C1,   // HI-Z
                         PIN_CONFIG_PUL_NORMAL);
}
#endif

/**
 * @brief  Config iomux for SPDIF
 */
void spdif_tx1_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D4,   // SPDIF_TX1
                        RMIO_SPDIF_TX);
}

/**
 * @brief  Config iomux for arc SPDIF
 */
void spdif_rx1_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D5,
                        RMIO_NO_FUNCTION);   //switch to GPIO4_D3

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D3,   // SPDIF_RX1
                        RMIO_SPDIF_RX1);
}

/**
 * @brief  Config iomux for opt SPDIF
 */
void opt_rx1_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D3,        //switch to GPIO4_D5
                        RMIO_NO_FUNCTION);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D5,   // SPDIF_RX1
                        RMIO_SPDIF_RX1);
}

#ifdef RT_USING_PMIC
/**
 * @brief  Config iomux for opt PDM
 */
void pdm_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B0,        //PDM_CLK
                        RMIO_PDM_CLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A7,        // PDM_SDI0
                        RMIO_PDM_SDI0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A6,       // PDM_SDI1
                        RMIO_PDM_SDI1);
}
#endif

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

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_C2 |
                         GPIO_PIN_C3,   //PULL-UP
                         PIN_CONFIG_PUL_UP);
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
}

/**
 * @brief  Config iomux for SAI0
 */
void sai0_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A0,   // SAI0_I2S_SCLK
                        RMIO_SAI0_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A1,   // SAI0_I2S_LRCK
                        RMIO_SAI0_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A2,   // SAI0_I2S_SDI0
                        RMIO_SAI0_SDI0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A3,   // SAI0_I2S_SDI1
                        RMIO_SAI0_SDI1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A4,   // SAI0_I2S_SDI2
                        RMIO_SAI0_SDI2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A5,   // SAI0_I2S_SDI3
                        RMIO_SAI0_SDI3);
}

/**
 * @brief  Config iomux for SAI1 FOR ES8388
 */
void sai1_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B1,   // SAI1_I2S_MCLK
                        RMIO_SAI1_MCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B3,     // SAI1_PCM_SCLK
                        RMIO_SAI1_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B4,     // SAI1_PCM_LRCK
                        RMIO_SAI1_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B5,     // SAI1_PCM_IN
                        RMIO_SAI1_SDI0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B7,     // SAI1_PCM_OUT
                        RMIO_SAI1_SDO0);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B2,
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK1,
                         GPIO_PIN_B2,    //HP_DET_L
                         PIN_CONFIG_PUL_DOWN);

    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_B2, GPIO_IN);
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
                        GPIO_PIN_C1,   // SAI4_I2S_LRCK
                        RMIO_SAI4_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C2,   // SAI4_I2S_SDO0
                        RMIO_SAI4_SDO0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C3,   // SAI4_I2S_SDO1
                        RMIO_SAI4_SDO1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C4,   // SAI4_I2S_SDO2
                        RMIO_SAI4_SDO2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C5,   // SAI4_I2S_SDO3
                        RMIO_SAI4_SDO3);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C6,   // SAI4_I2S_SDI0
                        RMIO_SAI4_SDI0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C7,   // SAI4_I2S_SDI1
                        RMIO_SAI4_SDI1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D0,   // SAI4_I2S_SDI2
                        RMIO_SAI4_SDI2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D1,   // SAI4_I2S_SDI3
                        RMIO_SAI4_SDI3);
}

/**
 * @brief  Config iomux for audio
 */
void audio_iomux_config(void)          //BT LE
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_C5,        //LE_CTRL_H
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_C5,   //HI-Z
                         PIN_CONFIG_PUL_NORMAL);

    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_C5, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO3, GPIO_PIN_C5, GPIO_LOW);
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

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A0,        //PHY_RST
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A0,       //HI-Z
                         PIN_CONFIG_PUL_NORMAL);

    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_A0, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A0, GPIO_HIGH);
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
                        GPIO_PIN_A2,   // PWM0_3 IR
                        RMIO_PWM0_CH3);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A2,       //PULL-UP
                         PIN_CONFIG_PUL_UP);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A3,   // PWM0_0
                        RMIO_PWM0_CH0);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A3,       //PULL-NORMAL
                         PIN_CONFIG_PUL_NORMAL);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A5,   // PWM0_1
                        RMIO_PWM0_CH1);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A5,       //PULL-NORMAL
                         PIN_CONFIG_PUL_NORMAL);
}

void uart2_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B4,   // UART2_TX_AUDIO_DEBUG
                        RMIO_UART2_TX_RM1);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_B4,       //PULL-UP
                         PIN_CONFIG_PUL_UP);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B5,   // UART2_RX_AUDIO_DEBUG
                        RMIO_UART2_RX_RM1);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_B5,       //PULL-UP
                         PIN_CONFIG_PUL_UP);

    HAL_PINCTRL_SetParam(GPIO_BANK3, GPIO_PIN_B5, PIN_CONFIG_PUL_UP);
}

/**
 * @brief  Config iomux for usb host
 */
void usb_host_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_B6,
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_B6, // HI-Z
                         PIN_CONFIG_PUL_NORMAL);

    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_B6, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO3, GPIO_PIN_B6, GPIO_HIGH);
}

#ifdef RT_USING_AIP1640
void led_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_A0,   // data
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A0,   //HI-Z
                         PIN_CONFIG_PUL_NORMAL);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_B2,   // CLK
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_B2,   //HI-Z
                         PIN_CONFIG_PUL_NORMAL);



    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_A2,   // pwer
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A2,   //HI-Z
                         PIN_CONFIG_PUL_NORMAL);

    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_A0, GPIO_OUT);
    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_B2, GPIO_OUT);

    HAL_GPIO_SetPinDirection(GPIO3, GPIO_PIN_A2, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO3, GPIO_PIN_A2, GPIO_HIGH);
}
#endif

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
#ifdef RT_USING_I2C2
    i2c2_iomux_config();
#endif
    // conflict with SDMMC_D1/2/3
    mcu_jtag_m0_iomux_config();
    uart0_iomux_config();
    sai4_iomux_config();
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
#ifdef RT_USING_HDMI_SWITCH_DRIVERS
    spdif_rx1_iomux_config();
    sai0_iomux_config();
#endif
    spdif_tx1_iomux_config();
    opt_rx1_iomux_config();
#ifdef RT_USING_GMAC0
    // conflict with dsp_jtag
    rmii_iomux_config();
#endif
    sai1_iomux_config();
    audio_iomux_config();
#ifdef RT_USING_AIP1640
    led_iomux_config();
#endif

#ifdef RT_USING_PMIC
    pdm_iomux_config();
#endif
}

/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
