/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
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

/**
 * @brief  Config iomux for PDM
 */
void pdm_input_iomux_config(void)
{
    // for pdm input
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A0 |  // PDM_IN_CLK0
                         GPIO_PIN_A2,   // PDM_IN_SDI0
                         PIN_CONFIG_MUX_FUNC1);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A0 |  // PDM_IN_CLK0
                         GPIO_PIN_A2,   // PDM_IN_SDI0
                         PIN_CONFIG_PUL_NORMAL);

    WRITE_REG_MASK_WE(GRF->SOC_CON2,
                      GRF_SOC_CON2_GRF_CON_AUDIO_BYPASS_CTRL_SEL_MASK,  // Audio bypass controlled by GRF
                      (0x1 << GRF_SOC_CON2_GRF_CON_AUDIO_BYPASS_CTRL_SEL_SHIFT));

    WRITE_REG_MASK_WE(GRF->SOC_CON4,
                      GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_PDM_MASK,  // PDM used as audio input
                      (0x1 << GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_PDM_SHIFT));
}

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
                         GPIO_PIN_A3,  // TOUCH_INT
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A7,   // TOUCH_RST
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_A3,
                         PIN_CONFIG_PUL_NORMAL |
                         PIN_CONFIG_DRV_LEVEL0);
}

static void codec_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_B1 |  // AMP_RST
                         GPIO_PIN_B2,   // AMP_INT
                         PIN_CONFIG_MUX_FUNC0);

    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_B1, GPIO_OUT);
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_B2, GPIO_IN);
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

    /* use gpio interrupt to trigger new frame */
    HAL_PINCTRL_SetIOMUX(RT_HW_LCD_GPIO_TRIGGER_BANK,
                         RT_HW_LCD_GPIO_TRIGGER_GPIO,
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetIOMUX(RT_HW_LCD_POWER_EN_BANK,
                         RT_HW_LCD_POWER_EN_GPIO,   // LCDC_RST
                         PIN_CONFIG_MUX_FUNC0);
}

/**
 * @brief  Config iomux for SDIO
 */
void sdio_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C0 |  // SDIO_CLK
                         GPIO_PIN_C1 |  // SDIO_CMD
                         GPIO_PIN_C2 |  // SDIO_D0
                         GPIO_PIN_C3 |  // SDIO_D1
                         GPIO_PIN_C4,   // SDIO_D2
                         PIN_CONFIG_MUX_FUNC4);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C5,   // SDIO_D3
                         PIN_CONFIG_MUX_FUNC3);

    HAL_PINCTRL_SetParam(GPIO_BANK0,
                         GPIO_PIN_C1 |  // SDIO_CMD
                         GPIO_PIN_C2 |  // SDIO_D0
                         GPIO_PIN_C3 |  // SDIO_D1
                         GPIO_PIN_C4 |  // SDIO_D2
                         GPIO_PIN_C5,   // SDIO_D3
                         PIN_CONFIG_PUL_UP |
                         PIN_CONFIG_DRV_LEVEL1);
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

void rt_hw_qpipsram_cs1_gpio_init(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D2,
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_D2, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D2, GPIO_HIGH);
}

void rt_hw_qpipsram_cs1_gpio_take(void)
{
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D2, GPIO_LOW);
}

void rt_hw_qpipsram_cs1_gpio_release(void)
{
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D2, GPIO_HIGH);
}

void rt_hw_qpipsram_cs2_gpio_init(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_C6,
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_C6, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_C6, GPIO_HIGH);
}

void rt_hw_qpipsram_cs2_gpio_take(void)
{
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_C6, GPIO_LOW);
}

void rt_hw_qpipsram_cs2_gpio_release(void)
{
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_C6, GPIO_HIGH);
}

void rt_hw_qpipsram_cs3_gpio_init(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_D1,
                         PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_D1, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D1, GPIO_HIGH);
}

void rt_hw_qpipsram_cs3_gpio_take(void)
{
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D1, GPIO_LOW);
}

void rt_hw_qpipsram_cs3_gpio_release(void)
{
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_D1, GPIO_HIGH);
}

void usb_vbus_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_A1,
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK0, GPIO_PIN_A1, PIN_CONFIG_PUL_NORMAL);
}

void power_key_iomux_config(void)
{
#if defined(POWER_KEY_BANK) && defined(POWER_KEY_PIN)
    HAL_PINCTRL_SetIOMUX(POWER_KEY_BANK,
                         POWER_KEY_PIN,
                         PIN_CONFIG_MUX_FUNC0);
#endif
}

void cy20721_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(RT_HW_SENSOR_HUB_PWR_BANK,
                         RT_HW_SENSOR_HUB_PWR_GPIO,
                         PIN_CONFIG_MUX_FUNC0);
}

void rt_hw_iomux_config(void)
{
    codec_iomux_config();

    i2s1_output_iomux_config();

    i2c1_m2_iomux_config();

    pdm_input_iomux_config();

    uart0_iomux_config();

    sfc0_iomux_config();

    sfc1_iomux_config();

#ifdef RT_USING_SDIO0
    sdio_iomux_config();
#endif

#ifdef M4_JTAG_ENABLE
    m4_jtag_iomux_config();
#endif

    lcdc_iomux_config();

    i2c0_m1_iomux_config();
    touch_iomux_config();

#ifdef RT_USING_SPINAND_SPI_HOST
    spi1_m0_iomux_config();
#endif

#ifdef RT_USING_QPIPSRAM_FSPI_HOST_CS1_GPIO
    rt_hw_qpipsram_cs1_gpio_init();
#endif

#ifdef RT_USING_QPIPSRAM_FSPI_HOST_CS2_GPIO
    rt_hw_qpipsram_cs2_gpio_init();
#endif

#ifdef RT_USING_QPIPSRAM_FSPI_HOST_CS3_GPIO
    rt_hw_qpipsram_cs3_gpio_init();
#endif

#ifdef RT_USING_USB_DEVICE
    usb_vbus_iomux_config();
#endif

    power_key_iomux_config();

    uart2_iomux_config();
    cy20721_iomux_config();
}
/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
