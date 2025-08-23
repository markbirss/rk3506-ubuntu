/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-02-05     Cliff Chen   first implementation
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
 * @brief  Config iomux for DSP_JTAG
 */
RT_WEAK  void dsp_jtag_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK2,
                         GPIO_PIN_A4 |  // JTAG_DSP_TDO
                         GPIO_PIN_A5 |  // JTAG_DSP_TCK
                         GPIO_PIN_A6 |  // JTAG_DSP_TMS
                         GPIO_PIN_A7 |  // JTAG_DSP_TDI
                         GPIO_PIN_B0,   // JTAG_DSP_TRSTN
                         PIN_CONFIG_MUX_FUNC2);
}

/**
 * @brief  Config iomux for FSPI0
 */
RT_WEAK  void fspi0_iomux_config(void)
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

/**
 * @brief  Config iomux for UART2
 */
RT_WEAK  void uart2_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B3,   // UART2_TX_AUDIO_DEBUG
                        RMIO_UART2_TX_RM1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B4,   // UART2_RX_AUDIO_DEBUG
                        RMIO_UART2_RX_RM1);
}

/**
 * @brief  Config iomux for lcdc
 */
RT_WEAK  void lcdc_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_A0 |  // VO_LCDC_CSN
                         GPIO_PIN_A1 |  // VO_LCDC_RS
                         GPIO_PIN_A2 |  // VO_LCDC_WRN
                         GPIO_PIN_A3 |  // VO_LCDC_D0
                         GPIO_PIN_A4 |  // VO_LCDC_D1
                         GPIO_PIN_A5 |  // VO_LCDC_D2
                         GPIO_PIN_A6 |  // VO_LCDC_D3
                         GPIO_PIN_A7 |  // VO_LCDC_D4
                         GPIO_PIN_B0 |  // VO_LCDC_D5
                         GPIO_PIN_B1 |  // VO_LCDC_D6
                         GPIO_PIN_B2 |  // VO_LCDC_D7
                         GPIO_PIN_B3,   // VO_LCDC_RDN_M0
                         PIN_CONFIG_MUX_FUNC4);
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A0 |  // VO_LCDC_CSN
                         GPIO_PIN_A1 |  // VO_LCDC_RS
                         GPIO_PIN_A2 |  // VO_LCDC_WRN
                         GPIO_PIN_A3 |  // VO_LCDC_D0
                         GPIO_PIN_A4 |  // VO_LCDC_D1
                         GPIO_PIN_A5 |  // VO_LCDC_D2
                         GPIO_PIN_A6 |  // VO_LCDC_D3
                         GPIO_PIN_A7 |  // VO_LCDC_D4
                         GPIO_PIN_B0 |  // VO_LCDC_D5
                         GPIO_PIN_B1 |  // VO_LCDC_D6
                         GPIO_PIN_B2 |  // VO_LCDC_D7
                         GPIO_PIN_B3,   // VO_LCDC_RDN_M0
                         PIN_CONFIG_DRV_LEVEL1);
}

/**
 * @brief  Config iomux for MCU_JTAG
 */
RT_WEAK  void mcu_jtag_m0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_C6 |  // JTAG_MCU_SWO_M0
                         GPIO_PIN_D2 |  // JTAG_MCU_TMS_M0
                         GPIO_PIN_D3,   // JTAG_MCU_TCK_M0
                         PIN_CONFIG_MUX_FUNC4);

    /* m0 for cpu0, m1 for cpu1, default target for cpu0 */
    GRF->SOC_CON0 = (0x3 << (GRF_SOC_CON0_GRF_JTAG_SEL_SHIFT + 16))
                    | (0x1 << (GRF_SOC_CON0_JTAG_MCU_SWO_M0_SEL_SHIFT + 16))
                    | (0x2 << GRF_SOC_CON0_GRF_JTAG_SEL_SHIFT)
                    | (0x0 << GRF_SOC_CON0_JTAG_MCU_SWO_M0_SEL_SHIFT);
}

/**
 * @brief  Config iomux for MCU_JTAG
 */
RT_WEAK  void mcu_jtag_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B0 |  // JTAG_MCU_TMS_M1
                         GPIO_PIN_B1 |  // JTAG_MCU_TCK_M1
                         GPIO_PIN_B2,   // JTAG_MCU_SWO_M1
                         PIN_CONFIG_MUX_FUNC2);

    /* m1 for cpu0, m0 for cpu1, default target for cpu0 */
    GRF->SOC_CON0 = (0x3 << (GRF_SOC_CON0_GRF_JTAG_SEL_SHIFT + 16))
                    | (0x1 << (GRF_SOC_CON0_JTAG_MCU_SWO_M0_SEL_SHIFT + 16))
                    | (0x1 << GRF_SOC_CON0_GRF_JTAG_SEL_SHIFT)
                    | (0x0 << GRF_SOC_CON0_JTAG_MCU_SWO_M1_SEL_SHIFT);
}

/**
 * @brief  Switch jtag to another cpu
 */
RT_WEAK  void mcu_jtag_switch(void)
{
    uint32_t value;

    /* change to another cpu */
    value = GRF->SOC_CON0 ^ ((0x3 << GRF_SOC_CON0_GRF_JTAG_SEL_SHIFT) | (GRF_SOC_CON0_JTAG_MCU_SWO_M0_SEL_SHIFT));
    value |= (0x3 << (GRF_SOC_CON0_GRF_JTAG_SEL_SHIFT + 16)) | (0x1 << (GRF_SOC_CON0_JTAG_MCU_SWO_M0_SEL_SHIFT + 16));
    GRF->SOC_CON0 = value;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(mcu_jtag_switch, jtag switch);
#endif

/**
 * @brief  Config iomux for SPI2
 */
RT_WEAK  void spi2_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A0,   // SPI2_CLK
                        RMIO_SPI2_CLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A1,   // SPI2_CSN0
                        RMIO_SPI2_CSN0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A2,   // SPI2_MOSI
                        RMIO_SPI2_MOSI);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A3,   // SPI2_MISO
                        RMIO_SPI2_MISO);

    HAL_PINCTRL_SetIOMUX(GPIO_BANK3,
                         GPIO_PIN_A4,   // SPI2_CRQ
                         PIN_CONFIG_MUX_FUNC0);

    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A0,   // SPI2_CLK
                         PIN_CONFIG_DRV_LEVEL2);
    HAL_PINCTRL_SetParam(GPIO_BANK3,
                         GPIO_PIN_A1 |  // SPI2_CSN0
                         GPIO_PIN_A2 |  // SPI2_MOSI
                         GPIO_PIN_A3,   // SPI2_MISO
                         PIN_CONFIG_DRV_LEVEL3);
}

/**
 * @brief  Config iomux for I2C2
 */
RT_WEAK  void i2c2_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A5,   // I2C2_SCL
                        RMIO_I2C2_SCL_RM1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_A6,   // I2C2_SDA
                        RMIO_I2C2_SDA_RM1);
}

/**
 * @brief  Config iomux for CAN
 */
RT_WEAK  void can_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B2,   // CAN_TX
                        RMIO_CAN_TX);

    HAL_PINCTRL_SetRMIO(GPIO_BANK3,
                        GPIO_PIN_B3,   // CAN_RX
                        RMIO_CAN_RX);
}

/**
 * @brief  Config iomux for UART0
 */
RT_WEAK  void uart0_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0,
                         GPIO_PIN_B1 |  // UART0_TX
                         GPIO_PIN_B2,   // UART0_RX
                         PIN_CONFIG_MUX_FUNC1);
}

/**
 * @brief  Config all SAIX_MCLK as OUTPUT
 */
RT_WEAK  void sai_mclkout_config_all(void)
{
    WRITE_REG_MASK_WE(GRF->SOC_CON1, HAL_GENMASK(7, 0), HAL_GENMASK(7, 0));
}

/**
 * @brief  Config SAIX_MCLK as OUTPUT
 * @param  saiId: SAIx_MCLK index [0~7].
 */
RT_WEAK  void sai_mclkout_config(int saiId)
{
    WRITE_REG_MASK_WE(GRF->SOC_CON1, HAL_BIT(saiId), HAL_BIT(saiId));
}

/**
 * @brief  Config SAIX_MCLK as INPUT
 * @param  saiId: SAIx_MCLK index [0~7].
 */
RT_WEAK  void sai_mclkin_config(int saiId)
{
    WRITE_REG_MASK_WE(GRF->SOC_CON1, HAL_BIT(saiId), 0);
}

/**
 * @brief  Config iomux for SAI7
 */
RT_WEAK  void sai7_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D2,   // SAI7_TDM_SCLK
                        RMIO_SAI7_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D3,   // SAI7_TDM_LRCK
                        RMIO_SAI7_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D4,   // SAI7_TDM_SDO0
                        RMIO_SAI7_SDO0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D5,   // SAI7_TDM_SDO1
                        RMIO_SAI7_SDO1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D6,   // SAI7_TDM_SDI0
                        RMIO_SAI7_SDI0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D7,   // SAI7_TDM_SDI1
                        RMIO_SAI7_SDI1);
}

/**
 * @brief  Config iomux for SAI4
 */
RT_WEAK  void sai4_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C3,   // SAI4_TDM_LRCK
                        RMIO_SAI4_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C4,   // SAI4_TDM_SCLK
                        RMIO_SAI4_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C5,   // SAI4_TDM_SDI0
                        RMIO_SAI4_SDI0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C6,   // SAI4_TDM_SDO0
                        RMIO_SAI4_SDO0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C7,   // SAI4_TDM_SDO1
                        RMIO_SAI4_SDO1);
}

/**
 * @brief  Config iomux for SAI5
 */
RT_WEAK  void sai5_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C0,   // SAI5_I2S_SCLK
                        RMIO_SAI5_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C1,   // SAI5_I2S_LRCK
                        RMIO_SAI5_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_C2,   // SAI5_I2S_SDI0
                        RMIO_SAI5_SDI0);
}

/**
 * @brief  Config iomux for SAI6
 * Conflict with sai7
 */
RT_WEAK  void sai6_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D0,   // SAI6_I2S_SCLK
                        RMIO_SAI6_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D1,   // SAI6_I2S_LRCK
                        RMIO_SAI6_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_D2,   // SAI6_I2S_SDI0
                        RMIO_SAI6_SDI0);
}

/**
 * @brief  Config iomux for SAI0
 */
RT_WEAK  void sai0_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A0,   // SAI0_TDM_SCLK
                        RMIO_SAI0_SCLK);
#if 0 // for i2s multi-lanes
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A1,   // SAI0_TDM_LRCK
                        RMIO_SAI0_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A2,   // SAI0_TDM_LRCKxN_0
                        RMIO_SAI0_LRCKXN_0);
#else // for tdm8 one lanes
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A2,   // SAI0_TDM_LRCK
                        RMIO_SAI0_LRCK);
#endif
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A3,   // SAI0_TDM_SDO0
                        RMIO_SAI0_SDO0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A4,   // SAI0_TDM_SDO1
                        RMIO_SAI0_SDO1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A5,   // SAI0_TDM_SDI0
                        RMIO_SAI0_SDI0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A6,   // SAI0_TDM_SDI1
                        RMIO_SAI0_SDI1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A7,   // SAI0_TDM_SDI2
                        RMIO_SAI0_SDI2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B0,   // SAI0_TDM_LRCKxN_1
                        RMIO_SAI0_LRCKXN_1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B1,   // SAI0_TDM_SDI3
                        RMIO_SAI0_SDI3);
}

/**
 * @brief  Config iomux for SPI0
 */
RT_WEAK  void spi0_m1_iomux_config(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1,
                         GPIO_PIN_A0 |  // SPI0_CLK
                         GPIO_PIN_A1 |  // SPI0_CSN
                         GPIO_PIN_A2 |  // SPI0_MOSI
                         GPIO_PIN_A3,   // SPI0_MISO
                         PIN_CONFIG_MUX_FUNC3);
}

/**
 * @brief  Config iomux for SAI1
 */
RT_WEAK  void sai1_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B2,   // SAI1_TDM_SCLK
                        RMIO_SAI1_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B3,   // SAI1_TDM_LRCK
                        RMIO_SAI1_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B4,   // SAI1_TDM_SDI0
                        RMIO_SAI1_SDI0);
}

/**
 * @brief  Config iomux for SAI2
 */
RT_WEAK  void sai2_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B5,   // SAI2_I2S_SCLK
                        RMIO_SAI2_SCLK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B6,   // SAI2_I2S_LRCK
                        RMIO_SAI2_LRCK);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B7,   // SAI2_I2S_SDI0
                        RMIO_SAI2_SDI0);
}

/**
 * @brief  Config iomux for pdm
 */
RT_WEAK  void pdm_iomux_config(void)
{
    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B0,   // PDM_CLK1
                        RMIO_PDM_CLK1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_A7,   // PDM_CLK0
                        RMIO_PDM_CLK0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B1,   // PDM_SDI0
                        RMIO_PDM_SDI0);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B3,   // PDM_SDI1
                        RMIO_PDM_SDI1);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B4,   // PDM_SDI2
                        RMIO_PDM_SDI2);

    HAL_PINCTRL_SetRMIO(GPIO_BANK4,
                        GPIO_PIN_B5,   // PDM_SDI3
                        RMIO_PDM_SDI3);
}

/**
 * @brief  Config iomux for rk2118 evb board
 */
RT_WEAK  void rt_hw_iomux_config(void)
{
    sai_mclkout_config_all();
    dsp_jtag_iomux_config();
    fspi0_iomux_config();
    uart2_iomux_config();
    mcu_jtag_m0_iomux_config();
    spi2_iomux_config();
    can_iomux_config();
    uart0_iomux_config();
    sai7_iomux_config();
    sai4_iomux_config();
    sai5_iomux_config();
    sai0_iomux_config();
    spi0_m1_iomux_config();
    sai1_iomux_config();
    sai2_iomux_config();
    pdm_iomux_config();
}
/** @} */  // IOMUX_Public_Functions

/** @} */  // IOMUX

/** @} */  // RKBSP_Board_Driver
