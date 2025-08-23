/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#ifndef __SOC_H
#define __SOC_H
#ifdef __cplusplus
  extern "C" {
#endif

#include "hal_conf.h"

/* IO definitions (access restrictions to peripheral registers) */
#ifdef __cplusplus
  #define   __I     volatile             /*!< brief Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< brief Defines 'read only' permissions */
#endif
#define     __O     volatile             /*!< brief Defines 'write only' permissions */
#define     __IO    volatile             /*!< brief Defines 'read / write' permissions */

/* ================================================================================ */
/* ================                    DMA REQ                      =============== */
/* ================================================================================ */
typedef enum {
    DMA_REQ_UART0_RX = 0,
    DMA_REQ_UART0_TX = 1,
    DMA_REQ_UART1_RX = 2,
    DMA_REQ_UART1_TX = 3,
    DMA_REQ_UART2_RX = 4,
    DMA_REQ_UART2_TX = 5,
    DMA_REQ_SAI_RX   = 10,
    DMA_REQ_SAI_TX   = 11,
    DMA_REQ_I2C4_RX  = 12,
    DMA_REQ_I2C4_TX  = 13,
    DMA_REQ_SPI0_RX  = 14,
    DMA_REQ_SPI0_TX  = 15,
} DMA_REQ_Type;

/* ================================================================================ */
/* ================                       IRQ                      ================ */
/* ================================================================================ */
#ifdef HAL_MCU_CORE
typedef enum
{
/******  Platform Exceptions Numbers ***************************************************/
  SARADC_IRQn            = 17,      /*!< SARADC Interrupt            */
  TIMER0_IRQn            = 32,      /*!< TIMER0 Interrupt            */
  TIMER1_IRQn            = 33,      /*!< TIMER1 Interrupt            */
  TIMER2_IRQn            = 34,      /*!< TIMER2 Interrupt            */
  TIMER3_IRQn            = 35,      /*!< TIMER3 Interrupt            */
  TIMER4_IRQn            = 36,      /*!< TIMER4 Interrupt            */
  TIMER5_IRQn            = 37,      /*!< TIMER5 Interrupt            */
  SPI0_IRQn              = 41,      /*!< SPI0 Interrupt              */
  FSPI0_IRQn             = 52,      /*!< FSPI0 Interrupt             */
  VICAP_IRQn             = 57,      /*!< VICAP Interrupt          */
  MIPI_ISP_IRQn          = 58,      /*!< MIPI ISP Interrupt          */
  MI_ISP_IRQn            = 59,      /*!< MI ISP Interrupt            */
  ISP_IRQn               = 60,      /*!< ISP Interrupt               */
  I2C0_IRQn              = 63,      /*!< I2C0 Interrupt              */
  I2C1_IRQn              = 64,      /*!< I2C1 Interrupt              */
  I2C2_IRQn              = 65,      /*!< I2C2 Interrupt              */
  I2C3_IRQn              = 66,      /*!< I2C3 Interrupt              */
  I2C4_IRQn              = 67,      /*!< I2C4 Interrupt              */
  UART0_IRQn             = 69,      /*!< UART0 Interrupt             */
  UART1_IRQn             = 70,      /*!< UART1 Interrupt             */
  UART2_IRQn             = 71,      /*!< UART2 Interrupt             */
  GPIO0_IRQn             = 75,      /*!< GPIO0_0 Interrupt           */
  GPIO3_IRQn             = 76,      /*!< GPIO0_1 Interrupt           */
  GPIO6_IRQn             = 77,      /*!< GPIO0_2 Interrupt           */
  GPIO9_IRQn             = 78,      /*!< GPIO0_3 Interrupt           */
  GPIO1_IRQn             = 79,      /*!< GPIO1_0 Interrupt           */
  GPIO4_IRQn             = 80,      /*!< GPIO1_1 Interrupt           */
  GPIO7_IRQn             = 81,      /*!< GPIO1_2 Interrupt           */
  GPIO10_IRQn            = 82,      /*!< GPIO1_3 Interrupt           */
  GPIO2_IRQn             = 83,      /*!< GPIO2_0 Interrupt           */
  GPIO5_IRQn             = 84,      /*!< GPIO2_1 Interrupt           */
  GPIO8_IRQn             = 85,      /*!< GPIO2_2 Interrupt           */
  GPIO11_IRQn            = 86,      /*!< GPIO2_3 Interrupt           */
  PWM0_CH0_IRQn          = 87,      /*!< PWM0_CH0 Interrupt          */
  PWM0_CH1_IRQn          = 88,      /*!< PWM0_CH1 Interrupt          */
  PWM0_CH2_IRQn          = 89,      /*!< PWM0_CH2 Interrupt          */
  PWM0_CH3_IRQn          = 90,      /*!< PWM0_CH3 Interrupt          */
  PWM1_CH0_IRQn          = 91,      /*!< PWM1_CH0 Interrupt          */
  PWM1_CH1_IRQn          = 92,      /*!< PWM1_CH1 Interrupt          */
  PWM1_CH2_IRQn          = 93,      /*!< PWM1_CH2 Interrupt          */
  PWM1_CH3_IRQn          = 94,      /*!< PWM1_CH3 Interrupt          */
  PWM2_CH0_IRQn          = 95,      /*!< PWM2_CH0 Interrupt          */
  PWM2_CH1_IRQn          = 96,      /*!< PWM2_CH1 Interrupt          */
  PWM2_CH2_IRQn          = 97,      /*!< PWM2_CH2 Interrupt          */
  PWM2_CH3_IRQn          = 98,      /*!< PWM2_CH3 Interrupt          */
  HPMCU_MBOX0_AP_IRQn    = 106,     /*!< HPMCU Mailbox0 AP Interrupt */
  HPMCU_MBOX1_AP_IRQn    = 107,     /*!< HPMCU Mailbox1 AP Interrupt */
  HPMCU_MBOX2_AP_IRQn    = 108,     /*!< HPMCU Mailbox2 AP Interrupt */
  HPMCU_MBOX3_AP_IRQn    = 109,     /*!< HPMCU Mailbox3 AP Interrupt */
  HPMCU_MBOX0_BB_IRQn    = 110,     /*!< HPMCU Mailbox0 BB Interrupt */
  HPMCU_MBOX1_BB_IRQn    = 111,     /*!< HPMCU Mailbox1 BB Interrupt */
  HPMCU_MBOX2_BB_IRQn    = 112,     /*!< HPMCU Mailbox2 BB Interrupt */
  HPMCU_MBOX3_BB_IRQn    = 113,     /*!< HPMCU Mailbox3 BB Interrupt */
  LPMCU_MBOX0_AP_IRQn    = 114,     /*!< LPMCU Mailbox0 AP Interrupt */
  LPMCU_MBOX1_AP_IRQn    = 115,     /*!< LPMCU Mailbox1 AP Interrupt */
  LPMCU_MBOX2_AP_IRQn    = 116,     /*!< LPMCU Mailbox2 AP Interrupt */
  LPMCU_MBOX3_AP_IRQn    = 117,     /*!< LPMCU Mailbox3 AP Interrupt */
  LPMCU_MBOX0_BB_IRQn    = 118,     /*!< LPMCU Mailbox0 BB Interrupt */
  LPMCU_MBOX1_BB_IRQn    = 119,     /*!< LPMCU Mailbox1 BB Interrupt */
  LPMCU_MBOX2_BB_IRQn    = 120,     /*!< LPMCU Mailbox2 BB Interrupt */
  LPMCU_MBOX3_BB_IRQn    = 121,     /*!< LPMCU Mailbox3 BB Interrupt */
  FSPI1_IRQn             = 124,     /*!< FSPI1 Interrupt             */
  NUM_INTERRUPTS         = 128,
} IRQn_Type;
#endif
/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */
#ifdef HAL_MCU_CORE
#define __RISC_V
#endif

#ifndef __ASSEMBLY__
#include "cmsis_compiler.h"               /* CMSIS compiler specific defines */
#include "system_rv1103b.h"
#endif /* __ASSEMBLY__ */
#include "rv1103b.h"
#include "rv1103b_cru.h"

/*****************************************CACHE*****************************************/
#ifdef HAL_AP_CORE
/* CACHE LINE SIZE */
#define CACHE_LINE_SHIFT (6U)
#define CACHE_LINE_SIZE  (0x1U << CACHE_LINE_SHIFT)
#else
/* CACHE LINE SIZE */
#define CACHE_LINE_SHIFT                (5U)
#define CACHE_LINE_SIZE                 (0x1U << CACHE_LINE_SHIFT)
#define CACHE_LINE_ADDR_MASK            (0xFFFFFFFFU << CACHE_LINE_SHIFT)
#define CACHE_M_CLEAN                   0x0U
#define CACHE_M_INVALID                 0x2U
#define CACHE_M_CLEAN_INVALID           0x4U
#define CACHE_M_INVALID_ALL             0x6U
#define CACHE_REVISION                  (0x00000100U)
#endif

/****************************************************************************************/
/*                                                                                      */
/*                               Register Bitmap Section                                */
/*                                                                                      */
/****************************************************************************************/
/******************************************CRU*******************************************/
#define PLL_INPUT_OSC_RATE   (24 * 1000 * 1000)
#define PCLK_CSIHOST0 LSCLK_VI_ROOT
#define PCLK_CSIHOST1 LSCLK_VI_ROOT
#define PCLK_CSIPHY   LSCLK_VI_ROOT
#define CRU_CLK_USE_CON_BANK
#define CLK64(mux, div) ((((mux) & 0xffffffffULL) << 32) | ((div) & 0xffffffffULL))

#ifndef __ASSEMBLY__
typedef enum CLOCK_Name {
    CLK_INVALID = 0U,
    PLL_GPLL,
    TCLK_WDT_NS,
    TCLK_WDT_S,
    CLK_I2C0,
    CLK_I2C1,
    CLK_I2C2,
    CLK_I2C3,
    CLK_I2C4,

    /* PERI */
    PCLK_PERI_ROOT = CLK64(0U, PCLK_PERI_ROOT_DIV),
    CLK_TSADC      = CLK64(0U, CLK_TSADC_DIV),
    CLK_TSADC_TSEN = CLK64(0U, CLK_TSADC_TSEN_DIV),
    CLK_SARADC     = CLK64(0U, CLK_SARADC_DIV),

    /* TOP */
    CLK_UART0_SRC  = CLK64(0U, CLK_UART0_SRC_DIV),
    CLK_UART0_FRAC = CLK64(0U, CLK_UART0_FRAC_DIV),
    CLK_UART0      = CLK64(SCLK_UART0_SRC_SEL, 0U),
    CLK_UART1_SRC  = CLK64(0U, CLK_UART1_SRC_DIV),
    CLK_UART1_FRAC = CLK64(0U, CLK_UART1_FRAC_DIV),
    CLK_UART1      = CLK64(SCLK_UART1_SRC_SEL, 0U),
    CLK_UART2_SRC  = CLK64(0U, CLK_UART2_SRC_DIV),
    CLK_UART2_FRAC = CLK64(0U, CLK_UART2_FRAC_DIV),
    CLK_UART2      = CLK64(SCLK_UART2_SRC_SEL, 0U),
    CLK_SAI_SRC    = CLK64(0U, CLK_SAI_SRC_DIV),
    CLK_SAI_FRAC   = CLK64(0U, CLK_SAI_FRAC_DIV),
    MCLK_SAI       = CLK64(MCLK_SAI_SRC_SEL, 0U),

    CCLK_EMMC   = CLK64(CCLK_EMMC_SRC_SEL, CCLK_EMMC_SRC_DIV),
    CCLK_SDMMC0 = CLK64(CCLK_SDMMC0_SRC_SEL, CCLK_SDMMC0_SRC_DIV),
    CCLK_SDMMC1 = CLK64(CCLK_SDMMC1_SRC_SEL, CCLK_SDMMC1_SRC_DIV),

    SCLK_SFC_2X       = CLK64(SCLK_SFC_2X_SRC_SEL, SCLK_SFC_2X_SRC_DIV),
    LSCLK_PERI_SRC    = CLK64(LSCLK_PERI_SRC_SEL, 0U),
    ACLK_PERI_SRC     = CLK64(ACLK_PERI_SRC_SEL, 0U),
    HCLK_HPMCU_SRC    = CLK64(HCLK_HPMCU_SRC_SEL, 0U),
    CLK_I2C_PMU       = CLK64(CLK_I2C_PMU_SRC_SEL, 0U),
    CLK_I2C_PERI      = CLK64(CLK_I2C_PERI_SRC_SEL, 0U),
    CLK_SPI0          = CLK64(CLK_SPI0_SRC_SEL, 0U),
    CLK_PWM0          = CLK64(CLK_PWM0_SRC_SEL, 0U),
    CLK_PWM1          = CLK64(CLK_PWM1_SRC_SEL, 0U),
    CLK_PWM2          = CLK64(CLK_PWM2_SRC_SEL, 0U),
    DCLK_DECOM        = CLK64(DCLK_DECOM_SRC_SEL, 0U),
    CLK_CORE_CRYPTO   = CLK64(CLK_CORE_CRYPTO_SRC_SEL, 0U),
    CLK_PKA_CRYPTO    = CLK64(CLK_PKA_CRYPTO_SRC_SEL, 0U),
    CLK_CORE_RGA      = CLK64(CLK_CORE_RGA_SRC_SEL, 0U),
    CLK_FREQ_PWM0     = CLK64(CLK_FREQ_PWM0_SRC_SEL, 0U),
    CLK_COUNTER_PWM0  = CLK64(CLK_COUNTER_PWM0_SRC_SEL, 0U),
    MCLK_SAI_OUT2IO   = CLK64(MCLK_SAI_OUT2IO_SEL, MCLK_SAI_OUT2IO_DIV),
    CLK_REF_MIPI0_SRC = CLK64(0U, CLK_REF_MIPI0_SRC_DIV),
    CLK_REF_MIPI1_SRC = CLK64(0U, CLK_REF_MIPI1_SRC_DIV),
    CLK_MIPI0_OUT2IO  = CLK64(CLK_MIPI0_OUT2IO_SEL, 0U),
    CLK_MIPI1_OUT2IO  = CLK64(CLK_MIPI1_OUT2IO_SEL, 0U),
    DCLK_VICAP_SRC    = CLK64(DCLK_VICAP_SRC_SEL, 0U),

    CLK_ISP_GPLL_SRC = CLK64(0U, CLK_ISP_GPLL_SRC_DIV),
    CLK_CORE_ISP     = CLK64(CLK_CORE_ISP_SEL, 0U),
    LSCLK_VI_ROOT    = CLK64(LSCLK_VI_ROOT_SEL, 0U),

    SCLK_SFC_2X_PMU1 = CLK64(SCLK_SFC_2X_PMU1_SEL, 0U),
} eCLOCK_Name;
#endif /* __ASSEMBLY__ */

/****************************************************************************************/
/*                                                                                      */
/*                                Module Address Section                                */
/*                                                                                      */
/****************************************************************************************/
/* Memory Base */
#define XIP_MAP0_BASE1      0x22000000U /* FSPI1 map address0 */
#define XIP_MAP0_BASE0      0x28000000U /* FSPI0 map address0 */
/****************************************************************************************/
/*                                                                                      */
/*                               Register Bitmap Section                                */
/*                                                                                      */
/****************************************************************************************/
/******************************************FSPI******************************************/
#define FSPI_CHIP_CNT                            (2)
/******************************************MBOX*******************************************/
#define LPMCU_MBOX0    MBOX0
#define LPMCU_MBOX1    MBOX1
#define LPMCU_MBOX2    MBOX2
#define LPMCU_MBOX3    MBOX3
#define HPMCU_MBOX0    MBOX4
#define HPMCU_MBOX1    MBOX5
#define HPMCU_MBOX2    MBOX6
#define HPMCU_MBOX3    MBOX7
#define MBOX_CNT       8         /* Total Mailbox in SoC */

#define PCLK_SARADC_CONTROL_GATE PCLK_SARADC_GATE

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __SOC_H */
