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
 * @brief  Config iomux for FSPI0
 */
void fspi0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A0 |  // FSPI_D5
                         GPIO_PIN_A1 |  // FSPI_D7
                         GPIO_PIN_A2 |  // FSPI_D6
                         GPIO_PIN_A3 |  // FSPI_D4
                         GPIO_PIN_A4 |  // FSPI_D3
                         GPIO_PIN_A5 |  // FSPI_CLK
                         GPIO_PIN_A6 |  // FSPI_D0
                         GPIO_PIN_A7 |  // FSPI_D2
                         GPIO_PIN_B0 |  // FSPI_D1
                         GPIO_PIN_B1 |  // FSPI_CSN
                         GPIO_PIN_B2 |  // FSPI_RSTN
                         GPIO_PIN_B3,   // FSPI_DQS
                         PIN_CONFIG_MUX_FUNC2);
}

/**
 * @brief  Config iomux for PWM0
 */
void pwm0_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A0,
                        RMIO_PWM0_CH0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A1,
                        RMIO_PWM0_CH1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A2,
                        RMIO_PWM0_CH2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A3,
                        RMIO_PWM0_CH3);
}

/**
 * @brief  Config iomux for PWM1
 */
void pwm1_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_C0,
                        RMIO_PWM1_CH0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_C1,
                        RMIO_PWM1_CH1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_C2,
                        RMIO_PWM1_CH2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_C3,
                        RMIO_PWM1_CH3);
}

#ifdef RT_USING_SDIO
/**
 * @brief  Config iomux for SDIO
 */
void sdmmc_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_D0 |  // SDMMC_CLK
                         GPIO_PIN_D1 |  // SDMMC_CMD
                         GPIO_PIN_C7 |  // SDMMC_D0
                         GPIO_PIN_C6 |  // SDMMC_D1
                         GPIO_PIN_D2 |  // SDMMC_D2
                         GPIO_PIN_D3,   // SDMMC_D3
                         PIN_CONFIG_MUX_FUNC1);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C4 |    // SDMMC_DET
                         GPIO_PIN_C5,     // SDMMC_PWREN
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_D1 |  // SDMMC_CMD
                         GPIO_PIN_C7 |  // SDMMC_D0
                         GPIO_PIN_C6 |  // SDMMC_D1
                         GPIO_PIN_D2 |  // SDMMC_D2
                         GPIO_PIN_D3,   // SDMMC_D3
                         PIN_CONFIG_PUL_UP |
                         PIN_CONFIG_DRV_LEVEL2);

    /*
     * PWERN is gpio function.
     */
    //HAL_GPIO_SetPinDirection(GPIO_BANK0, GPIO_PIN_C5, GPIO_OUT);
    //HAL_GPIO_SetPinLevel(GPIO_BANK0, GPIO_PIN_C5, GPIO_HIGH);


}
#endif

#ifdef RT_USING_I2C0
/**
 * @brief  Config iomux for I2C0
 */
void i2c0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK4,
                         GPIO_PIN_A1 |  // I2C0_SCL
                         GPIO_PIN_A2,   // I2C0_SDA
                         PIN_CONFIG_MUX_FUNC1);
}
#endif

void rt_hw_iomux_config(void)
{
    sai_mclkout_config_all();
    uart0_iomux_config();
    fspi0_iomux_config();
    dsp_jtag_iomux_config();
#ifdef RK2118_CPU_CORE0
    mcu_jtag_m0_iomux_config();
#endif
#ifdef RT_USING_SDIO
    sdmmc_iomux_config();
#endif
#ifdef RT_USING_I2C0
    i2c0_iomux_config();
#endif
    pdm_iomux_config();
#ifdef RT_USING_PWM0
    pwm0_iomux_config();
#endif
#ifdef RT_USING_PWM1
    pwm1_iomux_config();
#endif

}

/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
