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
                         GPIO_PIN_A4 |  // FSPI_D3
                         GPIO_PIN_A5 |  // FSPI_CLK
                         GPIO_PIN_A6 |  // FSPI_D0
                         GPIO_PIN_A7 |  // FSPI_D2
                         GPIO_PIN_B0 |  // FSPI_D1
                         GPIO_PIN_B1,   // FSPI_CSN
                         PIN_CONFIG_MUX_FUNC2);
}

#ifdef RT_USING_I2C0
/**
 * @brief  Config iomux for I2C0
 */
void i2c0_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A1,   // I2C0_SCL
                        RMIO_I2C0_SCL);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A2,   // I2C0_SDA
                        RMIO_I2C0_SDA);
}
#endif

#ifdef RT_USING_TOUCHKEY
/**
 * @brief  Config iomux for touchkey
 */
void touchkey_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A0,
                        RMIO_TOUCH_KEY_DRIVE_RM1);
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A0,
                         PIN_CONFIG_PUL_NORMAL | PIN_CONFIG_DRV_LEVEL2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A1,
                        RMIO_TOUCH_KEY_IN0_RM1);
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A1,
                         PIN_CONFIG_PUL_NORMAL | PIN_CONFIG_DRV_LEVEL2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A2,
                        RMIO_TOUCH_KEY_IN1_RM1);
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A2,
                         PIN_CONFIG_PUL_NORMAL | PIN_CONFIG_DRV_LEVEL2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A3,
                        RMIO_TOUCH_KEY_IN2_RM1);
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A3,
                         PIN_CONFIG_PUL_NORMAL | PIN_CONFIG_DRV_LEVEL2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A4,
                        RMIO_TOUCH_KEY_IN3_RM1);
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A4,
                         PIN_CONFIG_PUL_NORMAL | PIN_CONFIG_DRV_LEVEL2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A5,
                        RMIO_TOUCH_KEY_IN4_RM1);
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A5,
                         PIN_CONFIG_PUL_NORMAL | PIN_CONFIG_DRV_LEVEL2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A6,
                        RMIO_TOUCH_KEY_IN5_RM1);
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A6,
                         PIN_CONFIG_PUL_NORMAL | PIN_CONFIG_DRV_LEVEL2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A7,
                        RMIO_TOUCH_KEY_IN6_RM1);
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A7,
                         PIN_CONFIG_PUL_NORMAL | PIN_CONFIG_DRV_LEVEL2);
}
#endif

#ifdef RT_USING_PWM0
/**
 * @brief  Config iomux for pwm0
 */
void pwm0_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_A5,   // PWM0_2
                        RMIO_PWM0_CH2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK0,
                        GPIO_PIN_B0,   // PWM0_3
                        RMIO_PWM0_CH1);
}
#endif

void rt_hw_iomux_config(void)
{
    sai_mclkout_config_all();
    uart0_iomux_config();
    fspi0_iomux_config();
    dsp_jtag_iomux_config();
    // conflict with uart0
    //mcu_jtag_m1_iomux_config();
#ifdef RT_USING_I2C0
    i2c0_iomux_config();
#endif
#ifdef RT_USING_TOUCHKEY
    touchkey_iomux_config();
#endif
#ifdef RT_USING_PWM0
    pwm0_iomux_config();
#endif
}

/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
