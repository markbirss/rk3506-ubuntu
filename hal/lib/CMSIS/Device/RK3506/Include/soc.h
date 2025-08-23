/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#ifndef __SOC_H
#define __SOC_H
#ifdef __cplusplus
  extern "C" {
#endif

#include "hal_conf.h"

/* IO definitions (access restrictions to peripheral registers) */
#ifdef __cplusplus
  #define   __I     volatile             /*!< brief Defines 'read only' permissions    */
#else
  #define   __I     volatile const       /*!< brief Defines 'read only' permissions    */
#endif
#define     __O     volatile             /*!< brief Defines 'write only' permissions   */
#define     __IO    volatile             /*!< brief Defines 'read / write' permissions */

/* ================================================================================ */
/* ================                    DMA REQ                      =============== */
/* ================================================================================ */
#ifndef __ASSEMBLY__
typedef enum {
    /* DMAC0 */
    DMA0_REQ_SPI0_TX = 0,
    DMA0_REQ_CAN0_RX_0 = 0,
    DMA0_REQ_ASRC0_RX = 0,
    DMA0_REQ_SPI0_RX = 1,
    DMA0_REQ_CAN1_RX_1 = 1,
    DMA0_REQ_ASRC0_TX = 1,
    DMA0_REQ_SPI1_TX = 2,
    DMA0_REQ_DSMC_TX_2 = 2,
    DMA0_REQ_ASRC1_RX = 2,
    DMA0_REQ_SPI1_RX = 3,
    DMA0_REQ_DSMC_RX_3 = 3,
    DMA0_REQ_ASRC1_TX = 3,
    DMA0_REQ_UART0_TX = 4,
    DMA0_REQ_CAN0_RX_4 = 4,
    DMA0_REQ_UART0_RX = 5,
    DMA0_REQ_CAN1_RX_5 = 5,
    DMA0_REQ_PDM_RX = 5,
    DMA0_REQ_UART1_TX = 6,
    DMA0_REQ_SPDIF_TX = 6,
    DMA0_REQ_UART1_RX = 7,
    DMA0_REQ_SPDIF_RX = 7,
    DMA0_REQ_UART2_TX = 8,
    DMA0_REQ_DSMC_TX_8 = 8,
    DMA0_REQ_SAI0_RX = 8,
    DMA0_REQ_UART2_RX = 9,
    DMA0_REQ_SAI0_TX = 9,
    DMA0_REQ_UART3_TX = 10,
    DMA0_REQ_DSMC_RX_10 = 10,
    DMA0_REQ_SAI1_RX = 10,
    DMA0_REQ_UART3_RX = 11,
    DMA0_REQ_SAI1_TX = 11,

    /* DMAC1 */
    DMA1_REQ_SAI0_RX = 0,
    DMA1_REQ_SAI0_TX = 1,
    DMA1_REQ_SAI1_RX = 2,
    DMA1_REQ_SAI1_TX = 3,
    DMA1_REQ_SAI2_RX = 4,
    DMA1_REQ_SAI2_TX = 5,
    DMA1_REQ_SAI3_TX = 6,
    DMA1_REQ_SAI3_RX = 7,
    DMA1_REQ_SAI4_RX = 8,
    DMA1_REQ_PDM_RX = 9,
    DMA1_REQ_SPDIF_TX = 10,
    DMA1_REQ_CAN0_RX = 10,
    DMA1_REQ_SPDIF_RX = 11,
    DMA1_REQ_CAN1_RX = 11,
    DMA1_REQ_UART4_TX = 12,
    DMA1_REQ_UART4_RX = 13,
    DMA1_REQ_UART5_TX = 14,
    DMA1_REQ_UART5_RX = 15,
    DMA1_REQ_ASRC0_RX = 16,
    DMA1_REQ_ASRC0_TX = 17,
    DMA1_REQ_ASRC1_RX = 18,
    DMA1_REQ_ASRC1_TX = 19,
} DMA_REQ_Type;
#endif /* __ASSEMBLY__ */

/* ================================================================================ */
/* ================                       IRQ                      ================ */
/* ================================================================================ */
#if defined(HAL_BUS_MCU_CORE)

#define INTMUX_NUM_INT_PER_CON    128
#define INTMUX_NUM_OUT_PER_CON    4
#define INTMUX_NUM_INT_PER_OUT    32
#define INTMUX_NUM_GROUP_PER_OUT  1
#define INTMUX_NUM_GROUP_PER_CON  4
#define INTMUX_NUM_INT_PER_GROUP  32
/* INTMUX IRQ start from GIC SPI(Shared Peripheral Interrupt) */
#define INTMUX_IRQ_START_NUM      0
#define INTMUX_IRQ_OUT0
#define INTMUX_IRQ_OUT1
#define INTMUX_IRQ_OUT2
#define INTMUX_IRQ_OUT3
#define INTMUX_OUT_IRQ_START_NUM  28

#define NUM_EXT_INTERRUPTS        128

#ifndef __ASSEMBLY__
typedef enum {
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn       = -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn            = -13,     /*  3 HardFault Interrupt */

  SVCall_IRQn               =  -5,     /* 11 SV Call Interrupt */

  PendSV_IRQn               =  -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn              =  -1,     /* 15 System Tick Interrupt */

/******  Platform Exceptions Numbers ***************************************************/
  GPIO0_1_IRQn              =  0,      /*!< GPIO0_1 Interrupt         */
  GPIO0_2_IRQn              =  1,      /*!< GPIO0_2 Interrupt         */
  GPIO0_3_IRQn              =  2,      /*!< GPIO0_3 Interrupt         */
  GPIO1_1_IRQn              =  3,      /*!< GPIO1_1 Interrupt in pd_core which response faster than GPIO1_SHADOW_1*/
  GPIO1_SHADOW_1_IRQn       =  3,      /*!< GPIO1_SHADOW_1 Interrupt in pd_pmu which response slower than GPIO1_1 */
  GPIO1_2_IRQn              =  4,      /*!< GPIO1_2 Interrupt in pd_core which response faster than GPIO1_SHADOW_2*/
  GPIO1_SHADOW_2_IRQn       =  4,      /*!< GPIO1_SHADOW_2 Interrupt in pd_pmu which response slower than GPIO1_2 */
  GPIO1_3_IRQn              =  5,      /*!< GPIO1_3 Interrupt in pd_core which response faster than GPIO1_SHADOW_3*/
  GPIO1_SHADOW_3_IRQn       =  5,      /*!< GPIO1_SHADOW_3 Interrupt in pd_pmu which response slower than GPIO1_3 */
  GPIO2_3_IRQn              =  6,      /*!< GPIO2_3 Interrupt         */
  GPIO3_3_IRQn              =  7,      /*!< GPIO3_3 Interrupt         */
  TOUCH_KEY_POS_IRQn        =  8,      /*!< TOUCH_KEY_POS Interrupt   */
  TOUCH_KEY_NEG_IRQn        =  9,      /*!< TOUCH_KEY_NEG Interrupt   */
  PWM1_CH6_IRQn             =  10,     /*!< PWM1_CH6 Interrupt        */
  PWM1_CH7_IRQn             =  11,     /*!< PWM1_CH7 Interrupt        */
  UART3_IRQn                =  12,     /*!< UART3 Interrupt           */
  UART4_IRQn                =  13,     /*!< UART4 Interrupt           */
  I2C2_IRQn                 =  14,     /*!< I2C2 Interrupt            */
  SPI1_IRQn                 =  15,     /*!< SPI1 Interrupt            */
  SARADC_IRQn               =  16,     /*!< SARADC Interrupt          */
  SDMMC_IRQn                =  17,     /*!< SDMMC Interrupt           */
  TIMER4_IRQn               =  18,     /*!< TIMER0_4 Interrupt        */
  TIMER5_IRQn               =  19,     /*!< TIMER0_5 Interrupt        */
  TIMER10_IRQn              =  20,     /*!< TIMER1_4 Interrupt        */
  TIMER11_IRQn              =  21,     /*!< TIMER1_5 and DSMC use the same Interrupt ID */
  DSMC_IRQn                 =  21,     /*!< DSMC and TIMER1_5 use the same Interrupt ID */
  MAILBOX_8MUX1_IRQn        =  22,     /*!< MAILBOX_8MUX1 Interrupt   */
  DMAC0_IRQn                =  23,     /*!< DMAC0 Interrupt           */
  DMAC0_ABORT_IRQn          =  24,     /*!< DMAC0_ABORT Interrupt     */
  DMAC1_IRQn                =  25,     /*!< DMAC1 Interrupt           */
  DMAC1_ABORT_IRQn          =  26,     /*!< DMAC1_ABORT Interrupt     */
  FLEXBUS_IRQn              =  27,     /*!< FLEXBUS Interrupt         */
  INTMUX_OUT0_IRQn          =  28,     /*!< INTMUX0 Interrupt         */
  INTMUX_OUT1_IRQn          =  29,     /*!< INTMUX1 Interrupt         */
  INTMUX_OUT2_IRQn          =  30,     /*!< INTMUX2 Interrupt         */
  INTMUX_OUT3_IRQn          =  31,     /*!< INTMUX3 Interrupt         */
  NUM_INTERRUPTS            =  32,     /*!< Number of internal IRQ    */
  GPIO0_0_IRQn              =  0 + NUM_INTERRUPTS,
  GPIO1_0_IRQn              =  1 + NUM_INTERRUPTS,
  GPIO2_0_IRQn              =  2 + NUM_INTERRUPTS,
  GPIO2_1_IRQn              =  3 + NUM_INTERRUPTS,
  GPIO2_2_IRQn              =  4 + NUM_INTERRUPTS,
  GPIO3_0_IRQn              =  5 + NUM_INTERRUPTS,
  GPIO3_1_IRQn              =  6 + NUM_INTERRUPTS,
  GPIO3_2_IRQn              =  7 + NUM_INTERRUPTS,
  GPIO4_0_IRQn              =  8 + NUM_INTERRUPTS,
  GPIO4_1_IRQn              =  9 + NUM_INTERRUPTS,
  GPIO4_2_IRQn              =  10 + NUM_INTERRUPTS,
  GPIO4_3_IRQn              =  11 + NUM_INTERRUPTS,
  PWM0_CH0_IRQn             =  12 + NUM_INTERRUPTS,
  PWM0_CH1_IRQn             =  13 + NUM_INTERRUPTS,
  PWM0_CH2_IRQn             =  14 + NUM_INTERRUPTS,
  PWM0_CH3_IRQn             =  15 + NUM_INTERRUPTS,
  PWM1_CH0_IRQn             =  16 + NUM_INTERRUPTS,
  PWM1_CH1_IRQn             =  17 + NUM_INTERRUPTS,
  PWM1_CH2_IRQn             =  18 + NUM_INTERRUPTS,
  PWM1_CH3_IRQn             =  19 + NUM_INTERRUPTS,
  PWM1_CH4_IRQn             =  20 + NUM_INTERRUPTS,
  PWM1_CH5_IRQn             =  21 + NUM_INTERRUPTS,
  UART0_IRQn                =  22 + NUM_INTERRUPTS,
  UART1_IRQn                =  23 + NUM_INTERRUPTS,
  UART2_IRQn                =  24 + NUM_INTERRUPTS,
  UART5_IRQn                =  25 + NUM_INTERRUPTS,
  I2C0_IRQn                 =  26 + NUM_INTERRUPTS,
  I2C1_IRQn                 =  27 + NUM_INTERRUPTS,
  SPI0_IRQn                 =  28 + NUM_INTERRUPTS,
  CAN0_IRQn                 =  29 + NUM_INTERRUPTS,
  CAN1_IRQn                 =  30 + NUM_INTERRUPTS,
  SPDIF_TX_IRQn             =  31 + NUM_INTERRUPTS,
  SPDIF_RX_IRQn             =  32 + NUM_INTERRUPTS,
  PDM_IRQn                  =  33 + NUM_INTERRUPTS,
  SAI0_IRQn                 =  34 + NUM_INTERRUPTS,
  SAI1_IRQn                 =  35 + NUM_INTERRUPTS,
  SAI2_IRQn                 =  36 + NUM_INTERRUPTS,
  SAI3_IRQn                 =  37 + NUM_INTERRUPTS,
  SAI4_IRQn                 =  38 + NUM_INTERRUPTS,
  ASRC0_IRQn                =  39 + NUM_INTERRUPTS,
  ASRC1_IRQn                =  40 + NUM_INTERRUPTS,
  TSADC_IRQn                =  41 + NUM_INTERRUPTS,
  VOP_IRQn                  =  42 + NUM_INTERRUPTS,
  MIPI_DSIHOST_IRQn         =  43 + NUM_INTERRUPTS,
  RGA_IRQn                  =  44 + NUM_INTERRUPTS,
  OTPC_NS_IRQn              =  45 + NUM_INTERRUPTS,
  OTPC_S_IRQn               =  46 + NUM_INTERRUPTS,
  KEY_READER_IRQn           =  47 + NUM_INTERRUPTS,
  OTPC_MASK_IRQn            =  48 + NUM_INTERRUPTS,
  MAC0_SBD_IRQn             =  49 + NUM_INTERRUPTS,
  MAC0_SBD_TX_IRQn          =  50 + NUM_INTERRUPTS,
  MAC0_SBD_RX_IRQn          =  51 + NUM_INTERRUPTS,
  MAC0_PMT_IRQn             =  52 + NUM_INTERRUPTS,
  MAC1_SBD_IRQn             =  53 + NUM_INTERRUPTS,
  MAC1_SBD_TX_IRQn          =  54 + NUM_INTERRUPTS,
  MAC1_SBD_RX_IRQn          =  55 + NUM_INTERRUPTS,
  MAC1_PMT_IRQn             =  56 + NUM_INTERRUPTS,
  OTG0_IRQn                 =  57 + NUM_INTERRUPTS,
  OTG0_BVALID_IRQn          =  58 + NUM_INTERRUPTS,
  OTG0_ID_IRQn              =  59 + NUM_INTERRUPTS,
  OTG0_LINESTATE_IRQn       =  60 + NUM_INTERRUPTS,
  OTG0_DISCONNECT_IRQn      =  61 + NUM_INTERRUPTS,
  OTG1_IRQn                 =  62 + NUM_INTERRUPTS,
  OTG1_BVALID_IRQn          =  63 + NUM_INTERRUPTS,
  OTG1_ID_IRQn              =  64 + NUM_INTERRUPTS,
  OTG1_LINESTATE_IRQn       =  65 + NUM_INTERRUPTS,
  OTG1_DISCONNECT_IRQn      =  66 + NUM_INTERRUPTS,
  SPI2APB_IRQn              =  67 + NUM_INTERRUPTS,
  FSPI_IRQn                 =  68 + NUM_INTERRUPTS,
  DDRC_AWPOISON0_IRQn       =  69 + NUM_INTERRUPTS,
  DDRC_AWPOISON1_IRQn       =  70 + NUM_INTERRUPTS,
  DDRC_ARPOISON0_IRQn       =  71 + NUM_INTERRUPTS,
  DDRC_ARPOISON1_IRQn       =  72 + NUM_INTERRUPTS,
  DDRC_DFI_ALERT_ERR_IRQn   =  73 + NUM_INTERRUPTS,
  DDR_MONITOR_IRQn          =  74 + NUM_INTERRUPTS,
  DDRPHY_IRQn               =  75 + NUM_INTERRUPTS,
  TIMER0_IRQn               =  76 + NUM_INTERRUPTS,
  TIMER1_IRQn               =  77 + NUM_INTERRUPTS,
  TIMER2_IRQn               =  78 + NUM_INTERRUPTS,
  TIMER3_IRQn               =  79 + NUM_INTERRUPTS,
  TIMER6_IRQn               =  80 + NUM_INTERRUPTS,
  TIMER7_IRQn               =  81 + NUM_INTERRUPTS,
  TIMER8_IRQn               =  82 + NUM_INTERRUPTS,
  TIMER9_IRQn               =  83 + NUM_INTERRUPTS,
  HPTIMER_IRQn              =  84 + NUM_INTERRUPTS,
  WDT0_IRQn                 =  85 + NUM_INTERRUPTS,
  WDT1_IRQn                 =  86 + NUM_INTERRUPTS,
  Reserved_IRQn             =  87 + NUM_INTERRUPTS,
  CRYPTO_IRQn               =  88 + NUM_INTERRUPTS,
  CRYPTO_KLAD_IRQn          =  89 + NUM_INTERRUPTS,
  CRYPTO_SC_IRQn            =  90 + NUM_INTERRUPTS,
  NS_TRNG_IRQn              =  91 + NUM_INTERRUPTS,
  S_TRNG_IRQn               =  92 + NUM_INTERRUPTS,
  DSMC_SLAVE_IRQn           =  93 + NUM_INTERRUPTS,
  A7_PMUIRQ_0_IRQn          =  94 + NUM_INTERRUPTS,
  A7_PMUIRQ_1_IRQn          =  95 + NUM_INTERRUPTS,
  A7_PMUIRQ_2_IRQn          =  96 + NUM_INTERRUPTS,
  A7_PMUIRQ_3_IRQn          =  97 + NUM_INTERRUPTS,
  A7_AXIERRIRQ_IRQn         =  98 + NUM_INTERRUPTS,
  TIMER1_5_IRQn             =  99 + NUM_INTERRUPTS,      /*!< TIMER1_5 and DSMC use the same Interrupt ID */
  PMU_IRQn                  =  100 + NUM_INTERRUPTS,
  NPOR_POWERGOOD_IRQn       =  101 + NUM_INTERRUPTS,
  GPIO1_SHADOW_0_IRQn       =  102 + NUM_INTERRUPTS,
  OTG0_VBUSVALID_IRQn       =  106 + NUM_INTERRUPTS,
  OTG1_VBUSVALID_IRQn       =  107 + NUM_INTERRUPTS,
  VOP_LB_INTR_IRQn          =  108 + NUM_INTERRUPTS,
  DMA2DDR_IRQn              =  109 + NUM_INTERRUPTS,
  MAILBOX_AP_0_IRQn         =  110 + NUM_INTERRUPTS,
  MAILBOX_AP_1_IRQn         =  111 + NUM_INTERRUPTS,
  MAILBOX_AP_2_IRQn         =  112 + NUM_INTERRUPTS,
  MAILBOX_AP_3_IRQn         =  113 + NUM_INTERRUPTS,
  MAILBOX_BB_0_IRQn         =  114 + NUM_INTERRUPTS,
  MAILBOX_BB_1_IRQn         =  115 + NUM_INTERRUPTS,
  MAILBOX_BB_2_IRQn         =  116 + NUM_INTERRUPTS,
  MAILBOX_BB_3_IRQn         =  117 + NUM_INTERRUPTS,
  MAC0_MCGR_REQ_IRQn        =  118 + NUM_INTERRUPTS,
  MAC1_MCGR_REQ_IRQn        =  119 + NUM_INTERRUPTS,
  DMAC0_MASK_IRQ0_IRQn      =  120 + NUM_INTERRUPTS,
  DMAC0_MASK_IRQ1_IRQn      =  121 + NUM_INTERRUPTS,
  DMAC0_MASK_IRQ2_IRQn      =  122 + NUM_INTERRUPTS,
  DMAC1_MASK_IRQ0_IRQn      =  123 + NUM_INTERRUPTS,
  DMAC1_MASK_IRQ1_IRQn      =  124 + NUM_INTERRUPTS,
  DMAC1_MASK_IRQ2_IRQn      =  125 + NUM_INTERRUPTS,
  TOTAL_INTERRUPTS          =  (INTMUX_IRQ_START_NUM + NUM_INTERRUPTS + NUM_EXT_INTERRUPTS),
} IRQn_Type;
#endif /* __ASSEMBLY__ */
#else
#ifndef __ASSEMBLY__
typedef enum
{
/* When IPI_SGIs are used in AMP mode, you need to pay attention to whether it conflicts
 * with SMP mode. Especially in the case of Linux OS as The Master Core.
 * IPI_SGI 0~7 for non-secure and IPI_SGI 8~15 for secure.
 */
  IPI_SGI0                  = 0,
  IPI_SGI1                  = 1,
  IPI_SGI2                  = 2,
  IPI_SGI3                  = 3,
  IPI_SGI4                  = 4,
  IPI_SGI5                  = 5,
  IPI_SGI6                  = 6,
  IPI_SGI7                  = 7,
  IPI_SGI8                  = 8,
  IPI_SGI9                  = 9,
  IPI_SGI10                 = 10,
  IPI_SGI11                 = 11,
  IPI_SGI12                 = 12,
  IPI_SGI13                 = 13,
  IPI_SGI14                 = 14,
  IPI_SGI15                 = 15,

  CNTHP_IRQn                = 26,
  CNTV_IRQn                 = 27,
  CNTPS_IRQn                = 29,
  CNTPNS_IRQn               = 30,

  GPIO0_0_IRQn              =  32,
  GPIO0_1_IRQn              =  33,
  GPIO0_2_IRQn              =  34,
  GPIO0_3_IRQn              =  35,
  GPIO1_0_IRQn              =  36,
  GPIO1_1_IRQn              =  37,
  GPIO1_2_IRQn              =  38,
  GPIO1_3_IRQn              =  39,
  GPIO2_0_IRQn              =  40,
  GPIO2_1_IRQn              =  41,
  GPIO2_2_IRQn              =  42,
  GPIO2_3_IRQn              =  43,
  GPIO3_0_IRQn              =  44,
  GPIO3_1_IRQn              =  45,
  GPIO3_2_IRQn              =  46,
  GPIO3_3_IRQn              =  47,
  GPIO4_0_IRQn              =  48,
  GPIO4_1_IRQn              =  49,
  GPIO4_2_IRQn              =  50,
  GPIO4_3_IRQn              =  51,
  TOUCH_KEY_POS_IRQn        =  52,
  TOUCH_KEY_NEG_IRQn        =  53,
  PWM0_CH0_IRQn             =  54,
  PWM0_CH1_IRQn             =  55,
  PWM0_CH2_IRQn             =  56,
  PWM0_CH3_IRQn             =  57,
  PWM1_CH0_IRQn             =  58,
  PWM1_CH1_IRQn             =  59,
  PWM1_CH2_IRQn             =  60,
  PWM1_CH3_IRQn             =  61,
  PWM1_CH4_IRQn             =  62,
  PWM1_CH5_IRQn             =  63,
  PWM1_CH6_IRQn             =  64,
  PWM1_CH7_IRQn             =  65,
  UART0_IRQn                =  66,
  UART1_IRQn                =  67,
  UART2_IRQn                =  68,
  UART3_IRQn                =  69,
  UART4_IRQn                =  70,
  UART5_IRQn                =  71,
  I2C0_IRQn                 =  72,
  I2C1_IRQn                 =  73,
  I2C2_IRQn                 =  74,
  SPI0_IRQn                 =  75,
  SPI1_IRQn                 =  76,
  CAN0_IRQn                 =  77,
  CAN1_IRQn                 =  78,
  SPDIF_TX_IRQn             =  79,
  SPDIF_RX_IRQn             =  80,
  PDM_IRQn                  =  81,
  SAI0_IRQn                 =  82,
  SAI1_IRQn                 =  83,
  SAI2_IRQn                 =  84,
  SAI3_IRQn                 =  85,
  SAI4_IRQn                 =  86,
  ASRC0_IRQn                =  87,
  ASRC1_IRQn                =  88,
  SARADC_IRQn               =  89,
  TSADC_IRQn                =  90,
  VOP_IRQn                  =  91,
  MIPI_DSIHOST_IRQn         =  92,
  RGA_IRQn                  =  93,
  OTPC_NS_IRQn              =  94,
  OTPC_S_IRQn               =  95,
  KEY_READER_IRQn           =  96,
  OTPC_MASK_IRQn            =  97,
  MAC0_SBD_IRQn             =  98,
  MAC0_SBD_TX_IRQn          =  99,
  MAC0_SBD_RX_IRQn          =  100,
  MAC0_PMT_IRQn             =  101,
  MAC1_SBD_IRQn             =  102,
  MAC1_SBD_TX_IRQn          =  103,
  MAC1_SBD_RX_IRQn          =  104,
  MAC1_PMT_IRQn             =  105,
  OTG0_IRQn                 =  106,
  OTG0_BVALID_IRQn          =  107,
  OTG0_ID_IRQn              =  108,
  OTG0_LINESTATE_IRQn       =  109,
  OTG0_DISCONNECT_IRQn      =  110,
  OTG1_IRQn                 =  111,
  OTG1_BVALID_IRQn          =  112,
  OTG1_ID_IRQn              =  113,
  OTG1_LINESTATE_IRQn       =  114,
  OTG1_DISCONNECT_IRQn      =  115,
  SPI2APB_IRQn              =  116,
  FSPI_IRQn                 =  117,
  SDMMC_IRQn                =  118,
  DDRC_AWPOISON0_IRQn       =  119,
  DDRC_AWPOISON1_IRQn       =  120,
  DDRC_ARPOISON0_IRQn       =  121,
  DDRC_ARPOISON1_IRQn       =  122,
  DDRC_DFI_ALERT_ERR_IRQn   =  123,
  DDR_MONITOR_IRQn          =  124,
  DDRPHY_IRQn               =  125,
  TIMER0_IRQn               =  126,
  TIMER1_IRQn               =  127,
  TIMER2_IRQn               =  128,
  TIMER3_IRQn               =  129,
  TIMER4_IRQn               =  130,
  TIMER5_IRQn               =  131,
  TIMER6_IRQn               =  132,
  TIMER7_IRQn               =  133,
  TIMER8_IRQn               =  134,
  TIMER9_IRQn               =  135,
  TIMER10_IRQn              =  136,
  TIMER11_IRQn              =  137,
  HPTIMER_IRQn              =  138,
  WDT0_IRQn                 =  139,
  WDT1_IRQn                 =  140,
  CRYPTO_IRQn               =  143,
  CRYPTO_KLAD_IRQn          =  144,
  CRYPTO_SC_IRQn            =  145,
  NS_TRNG_IRQn              =  146,
  S_TRNG_IRQn               =  147,
  DMAC0_IRQn                =  148,
  DMAC0_ABORT_IRQn          =  149,
  DMAC1_IRQn                =  150,
  DMAC1_ABORT_IRQn          =  151,
  DSMC_SLAVE_IRQn           =  152,
  A7_PMUIRQ_0_IRQn          =  153,
  A7_PMUIRQ_1_IRQn          =  154,
  A7_PMUIRQ_2_IRQn          =  155,
  A7_PMUIRQ_3_IRQn          =  156,
  A7_AXIERRIRQ_IRQn         =  157,
  DSMC_IRQn                 =  158,
  FLEXBUS_IRQn              =  159,
  PMU_IRQn                  =  160,
  NPOR_POWERGOOD_IRQn       =  161,
  GPIO1_SHADOW_0_IRQn       =  162,
  GPIO1_SHADOW_1_IRQn       =  163,
  GPIO1_SHADOW_2_IRQn       =  164,
  GPIO1_SHADOW_3_IRQn       =  165,
  OTG0_VBUSVALID_IRQn       =  166,
  OTG1_VBUSVALID_IRQn       =  167,
  VOP_LB_INTR_IRQn          =  168,
  DMA2DDR_IRQn              =  169,
  MAILBOX_AP_0_IRQn         =  170,
  MAILBOX_AP_1_IRQn         =  171,
  MAILBOX_AP_2_IRQn         =  172,
  MAILBOX_AP_3_IRQn         =  173,
  MAILBOX_BB_0_IRQn         =  174,
  MAILBOX_BB_1_IRQn         =  175,
  MAILBOX_BB_2_IRQn         =  176,
  MAILBOX_BB_3_IRQn         =  177,
  MAC0_MCGR_REQ_IRQn        =  178,
  MAC1_MCGR_REQ_IRQn        =  179,
  DMAC0_MASK_IRQ0_IRQn      =  180,
  DMAC0_MASK_IRQ1_IRQn      =  181,
  DMAC0_MASK_IRQ2_IRQn      =  182,
  DMAC1_MASK_IRQ0_IRQn      =  183,
  DMAC1_MASK_IRQ1_IRQn      =  184,
  DMAC1_MASK_IRQ2_IRQn      =  185,
  NUM_INTERRUPTS            =  192,      /*!< Number of internal IRQ        */
} IRQn_Type;
#endif /* __ASSEMBLY__ */
#endif

#if defined(HAL_BUS_MCU_CORE)

#define RSVD_MCU_IRQn(_N)               (RSVD0_MCU_IRQn + (_N))
#define HAS_CUSTOME_INTC

#endif

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

#define PLL_INPUT_32K_RATE       (32 * 1000)
#define PLL_INPUT_400K_RATE      (400 * 1000)
#define PLL_INPUT_OSC_RATE       (24 * 1000 * 1000)

/* --------  Configuration of Core Peripherals  ----------------------------------- */
#if defined(HAL_AP_CORE) && defined(HAL_MCU_CORE)
#error "HAL_AP_CORE and HAL_MCU_CORE only one of them can be enabled"
#endif

#if !defined(HAL_AP_CORE) && !defined(HAL_MCU_CORE)
#error "Please define HAL_AP_CORE or HAL_MCU_CORE on hal_conf.h"
#endif

/* GIC Base */
#define GIC_DISTRIBUTOR_BASE       (0xFF581000)
#define GIC_CPU_INTERFACE_BASE     (0xFF582000)

#ifdef HAL_AP_CORE
#define __CA_REV                  0x0005U    /* Core revision r0p5                            */
#define __CORTEX_A                7U         /* Cortex-A7 Core                                */
#define __FPU_PRESENT             1U         /* FPU present                                   */
#define __TIM_PRESENT             1U         /* TIM present                                   */
#define __L2C_PRESENT             0U         /* L2C present                                   */
#define HAL_GIC_V2                1U         /* GIC version 2 */
#else
#define __CM0_REV                 0x0000U   /* Core revision r0p0 */
#define __MPU_PRESENT             0U        /* no MPU present */
#define __VTOR_PRESENT            0U        /* no VTOR present */
#define __NVIC_PRIO_BITS          2U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */

#define NVIC_PERIPH_IRQ_OFFSET    16U
#define MAX_INTERRUPT_VECTOR      64U
#endif

#ifndef __ASSEMBLY__
#include "cmsis_compiler.h"               /* CMSIS compiler specific defines */
#ifdef HAL_AP_CORE
#include "core_ca.h"
#else
#include "core_cm0.h"
#endif
#include "system_rk3506.h"
#endif /* __ASSEMBLY__ */
#include "rk3506.h"
#include "rk3506_cru.h"
#include "rk3506_usb.h"

/********************************** CPU Topology ****************************************/
#define MPIDR_MT_MASK                      ((1U) << 24)
#define MPIDR_AFFLVL_MASK                  (0xFFU)
#define MPIDR_AFF0_SHIFT                   (0U)
#define MPIDR_AFF1_SHIFT                   (8U)
#define MPIDR_AFF2_SHIFT                   (16U)
#define MPIDR_AFFINITY_MASK                (0xFFFFFFU)
#define PLATFORM_CLUSTER0_CORE_COUNT       (3)
#define PLATFORM_CLUSTER1_CORE_COUNT       (0)
#define PLATFORM_CORE_COUNT                (PLATFORM_CLUSTER0_CORE_COUNT + PLATFORM_CLUSTER1_CORE_COUNT)
#define CPU_GET_AFFINITY(cpuId, clusterId) (((cpuId) << MPIDR_AFF0_SHIFT) | ((0xF) << MPIDR_AFF1_SHIFT))

/*****************************************CACHE*****************************************/
#ifdef HAL_AP_CORE
/* CACHE LINE SIZE */
#define CACHE_LINE_SHIFT (6U)
#define CACHE_LINE_SIZE  (0x1U << CACHE_LINE_SHIFT)
#endif

/****************************************************************************************/
/*                                                                                      */
/*                                Module Address Section                                */
/*                                                                                      */
/****************************************************************************************/
/* Memory Base */
#define DSMC_MEM_BASE                  0xC0000000U              /* DSMC_MEM base address            */
#define USB_OTG0_BASE                  0xFF740000U              /* USB OTG0 base address            */
#define USB_OTG1_BASE                  0xFF780000U              /* USB OTG1 base address            */
#define USB_INNO_PHY_BASE              0xFF2B0000U              /* USB PHY base address             */
#define USB_PHY_CON_BASE               (GRF->SOC_CON24)         /* USB PHY0 control base address    */
#define USB_PHY1_CON_BASE              (GRF->SOC_CON28)         /* USB PHY1 control base address    */
#define USB_PHY_BCD_DET_CON            (GRF->SOC_CON27)         /* USB PHY BCD control base address */
#define USB_PHY_STATUS_BASE            (GRF->USBPHY_STATUS)     /* USB PHY status base address      */
#define USB_PHY_BCD_DET_BASE           USB_PHY_STATUS_BASE      /* USB PHY BCD status base address  */
#define USB_PHY_BVALID_IRQ_CON_BASE    (GRF->USBOTG0_SIG_DETECT_CON)
#define USB_PHY_BVALID_IRQ_CLR_BASE    (GRF->USBOTG0_SIG_DETECT_CLR)
#define USB_PHY_BVALID_IRQ_STATUS_BASE (GRF->USBOTG0_SIG_DETECT_STATUS)
/****************************************************************************************/
/*                                                                                      */
/*                               Module Variable Section                                */
/*                                                                                      */
/****************************************************************************************/
/* Module Variable Define */
#define USB_OTG0                       ((struct USB_GLOBAL_REG *) USB_OTG0_BASE)
#define USB_OTG1                       ((struct USB_GLOBAL_REG *) USB_OTG1_BASE)
#define IS_PCD_INSTANCE(instance)      (((instance) == USB_OTG0) || ((instance) == USB_OTG1))
#define IS_HCD_INSTANCE(instance)      (((instance) == USB_OTG0) || ((instance) == USB_OTG1))

/****************************************************************************************/
/*                                                                                      */
/*                               Register Bitmap Section                                */
/*                                                                                      */
/****************************************************************************************/
/******************************************USB*******************************************/
#define USB_PHY_SUSPEND_MASK                      \
    (GRF_SOC_CON24_USBOTG0_SW_EN_MASK |           \
     GRF_SOC_CON24_USBOTG0_UTMI_SUSPEND_N_MASK |  \
     GRF_SOC_CON24_USBOTG0_UTMI_OPMODE_MASK |     \
     GRF_SOC_CON24_USBOTG0_UTMI_XCVRSELECT_MASK | \
     GRF_SOC_CON24_USBOTG0_UTMI_TERMSELECT_MASK | \
     GRF_SOC_CON24_USBOTG0_UTMI_DPPULLDOWN_MASK | \
     GRF_SOC_CON24_USBOTG0_UTMI_DMPULLDOWN_MASK)
#define USB_PHY1_SUSPEND_MASK                     \
    (GRF_SOC_CON28_USBOTG1_SW_EN_MASK |           \
     GRF_SOC_CON28_USBOTG1_UTMI_SUSPEND_N_MASK |  \
     GRF_SOC_CON28_USBOTG1_UTMI_OPMODE_MASK |     \
     GRF_SOC_CON28_USBOTG1_UTMI_XCVRSELECT_MASK | \
     GRF_SOC_CON28_USBOTG1_UTMI_TERMSELECT_MASK | \
     GRF_SOC_CON28_USBOTG1_UTMI_DPPULLDOWN_MASK | \
     GRF_SOC_CON28_USBOTG1_UTMI_DMPULLDOWN_MASK)
#define USB_PHY_RESUME_MASK                  GRF_SOC_CON24_USBOTG0_SW_EN_MASK
#define USB_PHY_CON_SHIFT                    GRF_SOC_CON24_USBOTG0_SW_EN_SHIFT
#define USB_PHY_SUSPEND_VAL                  (0x1D1U)
#define USB_PHY_RESUME_VAL                   0
#define USB_PHY1_RESUME_MASK                 GRF_SOC_CON28_USBOTG1_SW_EN_MASK
#define USB_PHY1_CON_SHIFT                   GRF_SOC_CON28_USBOTG1_SW_EN_SHIFT
#define USB_PHY1_SUSPEND_VAL                 (0x1D1U)
#define USB_PHY1_RESUME_VAL                  0
#define USB_OTG_UTMI_BVALID_MASK             GRF_USBPHY_STATUS_USBOTG0_UTMI_BVALID_MASK
#define USB_OTG_UTMI_BVALID_SHIFT            GRF_USBPHY_STATUS_USBOTG0_UTMI_BVALID_SHIFT
#define USB_PHY_BVALID_RISE_IRQ_CON_MASK     GRF_USBOTG0_SIG_DETECT_CON_OTG0_BVALID_RISE_IRQ_EN_MASK
#define USB_PHY_BVALID_RISE_IRQ_CON_EN       USB_PHY_BVALID_RISE_IRQ_CON_MASK
#define USB_PHY_BVALID_RISE_IRQ_CLR_MASK     GRF_USBOTG0_SIG_DETECT_CLR_OTG0_BVALID_RISE_IRQ_CLR_MASK
#define USB_PHY_BVALID_RISE_IRQ_CLR_EN       USB_PHY_BVALID_RISE_IRQ_CLR_MASK
#define USB_PHY_BVALID_RISE_IRQ_STATUS_MASK  GRF_USBOTG0_SIG_DETECT_STATUS_OTG0_BVALID_RISE_IRQ_MASK
#define USB_PHY_BVALID_RISE_IRQ_STATUS_SHIFT GRF_USBOTG0_SIG_DETECT_STATUS_OTG0_BVALID_RISE_IRQ_SHIFT
#define USB_PHY_BVALID_FALL_IRQ_CON_MASK     GRF_USBOTG0_SIG_DETECT_CON_OTG0_BVALID_FALL_IRQ_EN_MASK
#define USB_PHY_BVALID_FALL_IRQ_CON_EN       USB_PHY_BVALID_FALL_IRQ_CON_MASK
#define USB_PHY_BVALID_FALL_IRQ_CLR_MASK     GRF_USBOTG0_SIG_DETECT_CLR_OTG0_BVALID_FALL_IRQ_CLR_MASK
#define USB_PHY_BVALID_FALL_IRQ_CLR_EN       USB_PHY_BVALID_FALL_IRQ_CLR_MASK
#define USB_PHY_BVALID_FALL_IRQ_STATUS_MASK  GRF_USBOTG0_SIG_DETECT_STATUS_OTG0_BVALID_FALL_IRQ_MASK
#define USB_PHY_BVALID_FALL_IRQ_STATUS_SHIFT GRF_USBOTG0_SIG_DETECT_STATUS_OTG0_BVALID_FALL_IRQ_SHIFT
#define USB_PHY_IDP_SINK_MASK                GRF_SOC_CON27_USBPHY_IDP_SINK_EN_MASK
#define USB_PHY_IDP_SINK_EN                  USB_PHY_IDP_SINK_MASK
#define USB_PHY_IDM_SINK_MASK                GRF_SOC_CON27_USBPHY_IDM_SINK_EN_MASK
#define USB_PHY_IDM_SINK_EN                  USB_PHY_IDM_SINK_MASK
#define USB_PHY_IDP_SRC_MASK                 GRF_SOC_CON27_USBPHY_IDP_SRC_EN_MASK
#define USB_PHY_IDP_SRC_EN                   USB_PHY_IDP_SRC_MASK
#define USB_PHY_RDM_PDWN_MASK                GRF_SOC_CON27_USBPHY_RDM_PDWN_EN_MASK
#define USB_PHY_RDM_PDWN_EN                  USB_PHY_RDM_PDWN_MASK
#define USB_PHY_VDP_SRC_MASK                 GRF_SOC_CON27_USBPHY_VDP_SRC_EN_MASK
#define USB_PHY_VDP_SRC_EN                   USB_PHY_VDP_SRC_MASK
#define USB_PHY_VDM_SRC_MASK                 GRF_SOC_CON27_USBPHY_VDM_SRC_EN_MASK
#define USB_PHY_VDM_SRC_EN                   USB_PHY_VDM_SRC_MASK
#define USB_PHY_CHG_MODE_MASK                (0x1FFU)
#define USB_PHY_CHG_MODE_VAL                 (0x057U)
#define USB_PHY_CP_DET_BIT                   (HAL_BIT(19))
#define USB_PHY_DCP_DET_BIT                  (HAL_BIT(18))
#define USB_PHY_DP_DET_BIT                   (HAL_BIT(20))

/****************************************************************************************/
/*                                                                                      */
/*                           Platform Differences Section                               */
/*                                                                                      */
/****************************************************************************************/

/******************************************CRU*******************************************/
#define CRU_CLK_USE_CON_BANK
#define CLK64(mux, div) ((((mux) & 0xffffffffULL) << 32) | ((div) & 0xffffffffULL))

#ifndef __ASSEMBLY__
typedef enum CLOCK_Name {
    /* TODO */
    CLK_INVALID = 0U,
    PLL_GPLL,
    PLL_V0PLL,
    PLL_V1PLL,
    ARMCLK,

    CLK_GPLL_DIV      = CLK64(0U, CLK_GPLL_DIV_DIV),
    CLK_GPLL_DIV_100M = CLK64(0U, CLK_GPLL_DIV_100M_DIV),
    CLK_V0PLL_DIV     = CLK64(0U, CLK_V0PLL_DIV_DIV),
    CLK_V1PLL_DIV     = CLK64(0U, CLK_V1PLL_DIV_DIV),

    CLK_INT_VOICE0   = CLK64(0U, CLK_INT_VOICE_MATRIX0_DIV),
    CLK_INT_VOICE1   = CLK64(0U, CLK_INT_VOICE_MATRIX1_DIV),
    CLK_INT_VOICE2   = CLK64(0U, CLK_INT_VOICE_MATRIX2_DIV),

    CLK_FRAC_UART0   = CLK64(CLK_FRAC_UART_MATRIX0_MUX_SEL, CLK_FRAC_UART_MATRIX0_DIV),
    CLK_FRAC_UART1   = CLK64(CLK_FRAC_UART_MATRIX1_MUX_SEL, CLK_FRAC_UART_MATRIX1_DIV),
    CLK_FRAC_VOICE0  = CLK64(CLK_FRAC_VOICE_MATRIX0_MUX_SEL, CLK_FRAC_VOICE_MATRIX0_DIV),
    CLK_FRAC_VOICE1  = CLK64(CLK_FRAC_VOICE_MATRIX1_MUX_SEL, CLK_FRAC_VOICE_MATRIX1_DIV),
    CLK_FRAC_COMMON0 = CLK64(CLK_FRAC_COMMON_MATRIX0_MUX_SEL, CLK_FRAC_COMMON_MATRIX0_DIV),
    CLK_FRAC_COMMON1 = CLK64(CLK_FRAC_COMMON_MATRIX1_MUX_SEL, CLK_FRAC_COMMON_MATRIX1_DIV),
    CLK_FRAC_COMMON2 = CLK64(CLK_FRAC_COMMON_MATRIX2_MUX_SEL, CLK_FRAC_COMMON_MATRIX2_DIV),

    ACLK_BUS_ROOT    = CLK64(ACLK_BUS_ROOT_SEL, ACLK_BUS_ROOT_DIV),
    HCLK_BUS_ROOT    = CLK64(HCLK_BUS_ROOT_SEL, HCLK_BUS_ROOT_DIV),
    PCLK_BUS_ROOT    = CLK64(PCLK_BUS_ROOT_SEL, PCLK_BUS_ROOT_DIV),
    ACLK_HSPERI_ROOT = CLK64(ACLK_HSPERI_ROOT_SEL, ACLK_HSPERI_ROOT_DIV),
    HCLK_LSPERI_ROOT = CLK64(HCLK_LSPERI_ROOT_SEL, HCLK_LSPERI_ROOT_DIV),

    CLK_I2C0         = CLK64(CLK_I2C0_SEL, CLK_I2C0_DIV),
    CLK_I2C1         = CLK64(CLK_I2C1_SEL, CLK_I2C1_DIV),
    CLK_I2C2         = CLK64(CLK_I2C2_SEL, CLK_I2C2_DIV),

    CLK_SPI0         = CLK64(CLK_SPI0_SEL, CLK_SPI0_DIV),
    CLK_SPI1         = CLK64(CLK_SPI1_SEL, CLK_SPI1_DIV),
    SCLK_FSPI        = CLK64(SCLK_FSPI_SEL, SCLK_FSPI_DIV),

    CLK_UART0        = CLK64(SCLK_UART0_SEL, SCLK_UART0_DIV),
    CLK_UART1        = CLK64(SCLK_UART1_SEL, SCLK_UART1_DIV),
    CLK_UART2        = CLK64(SCLK_UART2_SEL, SCLK_UART2_DIV),
    CLK_UART3        = CLK64(SCLK_UART3_SEL, SCLK_UART3_DIV),
    CLK_UART4        = CLK64(SCLK_UART4_SEL, SCLK_UART4_DIV),
    CLK_UART5        = CLK64(SCLK_UART5_SEL, SCLK_UART5_DIV),

    CLK_PWM0         = CLK64(0U, CLK_PWM0_DIV),
    CLK_PWM1         = CLK64(CLK_PWM1_SEL, CLK_PWM1_DIV),

    CCLK_SRC_SDMMC   = CLK64(CCLK_SRC_SDMMC_SEL, CCLK_SRC_SDMMC_DIV),

    CLK_TSADC        = CLK64(0U, CLK_TSADC_DIV),
    CLK_TSADC_TSEN   = CLK64(0U, CLK_TSADC_TSEN_DIV),
    CLK_SARADC       = CLK64(CLK_SARADC_SEL, CLK_SARADC_DIV),

    CLK_CAN0         = CLK64(CLK_CAN0_SEL, CLK_CAN0_DIV),
    CLK_CAN1         = CLK64(CLK_CAN1_SEL, CLK_CAN1_DIV),

    CLK_DSMC         = CLK64(CLK_DSMC_SEL, CLK_DSMC_DIV),
    CLK_ROOT_DSMC    = CLK64(ACLK_CORE_PERI_ROOT_SEL, ACLK_CORE_PERI_ROOT_DIV),

    CLK_FLEXBUS_TX   = CLK64(CLK_FLEXBUS_TX_SEL, CLK_FLEXBUS_TX_DIV),
    CLK_FLEXBUS_RX   = CLK64(CLK_FLEXBUS_RX_SEL, CLK_FLEXBUS_RX_DIV),

    DCLK_VOP         = CLK64(DCLK_VOP_SEL, DCLK_VOP_DIV),

    CLK_MAC_PTP      = CLK64(CLK_MAC_PTP_ROOT_SEL, CLK_MAC_PTP_ROOT_DIV),
    CLK_MAC_OUT      = CLK64(0U, CLK_MAC_OUT_DIV),
    CLK_MAC          = CLK64(0U, CLK_MAC_ROOT_DIV),

    CLK_REF_OUT0     = CLK64(CLK_REF_OUT0_SEL, CLK_REF_OUT0_DIV),
    CLK_REF_OUT1     = CLK64(CLK_REF_OUT1_SEL, CLK_REF_OUT1_DIV),

    MCLK_SPDIFRX     = CLK64(MCLK_SPDIFRX_SEL, MCLK_SPDIFRX_DIV),
    MCLK_SPDIFTX     = CLK64(MCLK_SPDIFTX_SEL, MCLK_SPDIFTX_DIV),

    MCLK_SAI0        = CLK64(MCLK_SAI0_SEL, MCLK_SAI0_DIV),
    MCLK_SAI1        = CLK64(MCLK_SAI1_SEL, MCLK_SAI1_DIV),
    MCLK_SAI2        = CLK64(MCLK_SAI2_SEL, MCLK_SAI2_DIV),
    MCLK_SAI3        = CLK64(MCLK_SAI3_SRC_SEL, MCLK_SAI3_SRC_DIV),
    MCLK_SAI4        = CLK64(MCLK_SAI4_SRC_SEL, MCLK_SAI4_SRC_DIV),
    CLK_ASRC0        = CLK64(CLK_ASRC0_SEL, CLK_ASRC0_DIV),
    MCLK_ASRC0       = CLK64(MCLK_ASRC0_SEL, 0U),
    CLK_ASRC1        = CLK64(CLK_ASRC1_SEL, CLK_ASRC1_DIV),
    MCLK_ASRC1       = CLK64(MCLK_ASRC1_SEL, 0U),
    MCLK_ASRC2       = CLK64(MCLK_ASRC2_SEL, 0U),
    MCLK_ASRC3       = CLK64(MCLK_ASRC3_SEL, 0U),
    MCLK_PDM         = CLK64(MCLK_PDM_SEL, MCLK_PDM_DIV),
    CLKOUT_PDM       = CLK64(CLKOUT_PDM_SEL, CLKOUT_PDM_DIV),
    LRCK_ASRC0_SRC   = CLK64(LRCK_ASRC0_SRC_SEL, 0U),
    LRCK_ASRC0_DST   = CLK64(LRCK_ASRC0_DST_SEL, 0U),
    LRCK_ASRC1_SRC   = CLK64(LRCK_ASRC1_SRC_SEL, 0U),
    LRCK_ASRC1_DST   = CLK64(LRCK_ASRC1_DST_SEL, 0U),
} eCLOCK_Name;

/******************************************RMIO******************************************/
/** @addtogroup RK_HAL_Driver
 *  @{
 */
/** @addtogroup PINCTRL
 *  @{
 */
/** @addtogroup PINCTRL_Exported_Definition_Group1
 *  @{
 */
/** RMIO Function Definition */
typedef enum {
    RMIO_NO_FUNCTION = 0,
    RMIO_UART1_TX          = 0x1,
    RMIO_UART1_RX          = 0x2,
    RMIO_UART2_TX          = 0x3,
    RMIO_UART2_RX          = 0x4,
    RMIO_UART3_TX          = 0x5,
    RMIO_UART3_RX          = 0x6,
    RMIO_UART3_CTS_REN     = 0x7,
    RMIO_UART3_RTSN_DE     = 0x8,
    RMIO_UART4_TX          = 0x9,
    RMIO_UART4_RX          = 0xA,
    RMIO_UART4_CTS_REN     = 0xB,
    RMIO_UART4_RTSN_DE     = 0xC,
    RMIO_MIPITE            = 0xD,
    RMIO_CLK32_IOUT        = 0xE,
    RMIO_I2C0_SCL          = 0xF,
    RMIO_I2C0_SDA          = 0x10,
    RMIO_I2C1_SCL          = 0x11,
    RMIO_I2C1_SDA          = 0x12,
    RMIO_I2C2_SCL          = 0x13,
    RMIO_I2C2_SDA          = 0x14,
    RMIO_PDMCLK0           = 0x15,
    RMIO_PDM_SDI0          = 0x16,
    RMIO_PDM_SDI1          = 0x17,
    RMIO_PDM_SDI2          = 0x18,
    RMIO_PDM_SDI3          = 0x19,
    RMIO_CAN1_TX           = 0x1A,
    RMIO_CAN1_RX           = 0x1B,
    RMIO_CAN0_TX           = 0x1C,
    RMIO_CAN0_RX           = 0x1D,
    RMIO_PWM0_CH0          = 0x1E,
    RMIO_PWM0_CH1          = 0x1F,
    RMIO_PWM0_CH2          = 0x20,
    RMIO_PWM0_CH3          = 0x21,
    RMIO_PWM1_CH0          = 0x22,
    RMIO_PWM1_CH1          = 0x23,
    RMIO_PWM1_CH2          = 0x24,
    RMIO_PWM1_CH3          = 0x25,
    RMIO_PWM1_CH4          = 0x26,
    RMIO_PWM1_CH5          = 0x27,
    RMIO_PWM1_CH6          = 0x28,
    RMIO_PWM1_CH7          = 0x29,
    RMIO_TOUCH_KEY_DRIVE   = 0x2A,
    RMIO_TOUCH_KEY0        = 0x2B,
    RMIO_TOUCH_KEY1        = 0x2C,
    RMIO_TOUCH_KEY2        = 0x2D,
    RMIO_TOUCH_KEY3        = 0x2E,
    RMIO_TOUCH_KEY4        = 0x2F,
    RMIO_TOUCH_KEY5        = 0x30,
    RMIO_TOUCH_KEY6        = 0x31,
    RMIO_TOUCH_KEY7        = 0x32,
    RMIO_SAI0_MCLK         = 0x33,
    RMIO_SAI0_SCLK         = 0x34,
    RMIO_SAI0_LRCK         = 0x35,
    RMIO_SAI0_SDI0         = 0x36,
    RMIO_SAI0_SDI1         = 0x37,
    RMIO_SAI0_SDI2         = 0x38,
    RMIO_SAI0_SDI3         = 0x39,
    RMIO_SAI0_SDO          = 0x3A,
    RMIO_SAI1_MCLK         = 0x3B,
    RMIO_SAI1_SCLK         = 0x3C,
    RMIO_SAI1_LRCK         = 0x3D,
    RMIO_SAI1_SDI          = 0x3E,
    RMIO_SAI1_SDO0         = 0x3F,
    RMIO_SAI1_SDO1         = 0x40,
    RMIO_SAI1_SDO2         = 0x41,
    RMIO_SAI1_SDO3         = 0x42,
    RMIO_SPI0_CLK          = 0x43,
    RMIO_SPI0_MOSI         = 0x44,
    RMIO_SPI0_MISO         = 0x45,
    RMIO_SPI0_CSN0         = 0x46,
    RMIO_SPI0_CSN1         = 0x47,
    RMIO_SPI1_CLK          = 0x48,
    RMIO_SPI1_MOSI         = 0x49,
    RMIO_SPI1_MISO         = 0x4A,
    RMIO_SPI1_CSN0         = 0x4B,
    RMIO_SPI1_CSN1         = 0x4C,
    RMIO_WDT_TSADC_SHUT    = 0x4D,
    RMIO_PMU_SLEEP         = 0x4E,
    RMIO_CORE_POWER_OFF    = 0x4F,
    RMIO_SPDIF_TX          = 0x50,
    RMIO_SPDIF_RX          = 0x51,
    RMIO_BIP_CNTR_A0       = 0x52,
    RMIO_BIP_CNTR_A1       = 0x53,
    RMIO_BIP_CNTR_A2       = 0x54,
    RMIO_BIP_CNTR_A3       = 0x55,
    RMIO_BIP_CNTR_A4       = 0x56,
    RMIO_BIP_CNTR_A5       = 0x57,
    RMIO_BIP_CNTR_B0       = 0x58,
    RMIO_BIP_CNTR_B1       = 0x59,
    RMIO_BIP_CNTR_B2       = 0x5A,
    RMIO_BIP_CNTR_B3       = 0x5B,
    RMIO_BIP_CNTR_B4       = 0x5C,
    RMIO_BIP_CNTR_B5       = 0x5D,
    RMIO_PDMCLK1           = 0x5E,
    RMIO_RMII0_PTP         = 0x5F,
    RMIO_RMII0_MCGPST_TRIG = 0x60,
    RMIO_RMII1_PTP         = 0x61,
    RMIO_RMII1_MCGPST_TRI  = 0x62,
} eRMIO_Name;
/** @} */
/** @} */
/** @} */
#endif /* __ASSEMBLY__ */

/****************************************MBOX********************************************/
#define MBOX_CNT             4

/****************************************GPIO********************************************/
#define GPIO0_IRQn              GPIO0_0_IRQn
#define GPIO1_IRQn              GPIO1_0_IRQn
#define GPIO2_IRQn              GPIO2_0_IRQn
#define GPIO3_IRQn              GPIO3_0_IRQn
#define GPIO4_IRQn              GPIO4_0_IRQn

#define IS_GPIO_EXP_INSTANCE(instance)     \
        IS_GPIO0_EXP_INSTANCE(instance) || \
        IS_GPIO1_EXP_INSTANCE(instance) || \
        IS_GPIO2_EXP_INSTANCE(instance) || \
        IS_GPIO3_EXP_INSTANCE(instance) || \
        IS_GPIO4_EXP_INSTANCE(instance)

/****************************************PMU*********************************************/
#ifndef __ASSEMBLY__
typedef enum PD_Id {
    PD_INVALID = 0U,
} ePD_Id;
#endif
/****************************************FSPI********************************************/
#define FSPI_CHIP_CNT                            (1)

/****************************************WDT*********************************************/
#define GLB_RST_SND_WDT GLB_RST_SND_WDT0
#define GLB_RST_FST_WDT GLB_RST_FST_WDT0
#define WDT WDT0
#define PCLK_WDT PCLK_BUS_ROOT
#define WDT_CR_WDT_EN_MASK WDT_CR_EN_MASK
/****************************************SPI2APB*****************************************/
#define SPISLV0_IRQn                              SPI2APB_IRQn

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __SOC_H */
