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
    DMA_REQ_SAI0_TX = 0,
    DMA_REQ_SAI2_TX = 0,
    DMA_REQ_SAI4_TX = 0,
    DMA_REQ_SAI0_RX = 1,
    DMA_REQ_SAI2_RX = 1,
    DMA_REQ_SAI4_RX = 1,
    DMA_REQ_SAI1_TX = 2,
    DMA_REQ_SAI3_TX = 2,
    DMA_REQ_SAI1_RX = 3,
    DMA_REQ_SAI3_RX = 3,
    DMA_REQ_SAI5_RX = 3,
    DMA_REQ_PDM0 = 4,
    DMA_REQ_PDM1 = 4,
    DMA_REQ_SAI6_TX = 4,
    DMA_REQ_SPDIF_TX0 = 5,
    DMA_REQ_SPDIF_TX1 = 5,
    DMA_REQ_SAI6_RX = 5,
    DMA_REQ_UART0_TX = 6,
    DMA_REQ_SPDIF_TX4 = 6,
    DMA_REQ_UART7_TX = 6,
    DMA_REQ_UART0_RX = 7,
    DMA_REQ_SAI8_TX = 7,
    DMA_REQ_UART7_RX = 7,
    DMA_REQ_UART1_TX = 8,
    DMA_REQ_SPDIF_RX0 = 8,
    DMA_REQ_UART8_TX = 8,
    DMA_REQ_UART1_RX = 9,
    DMA_REQ_UART4_TX = 9,
    DMA_REQ_UART8_RX = 9,
    DMA_REQ_UART2_TX = 10,
    DMA_REQ_UART4_RX = 10,
    DMA_REQ_UART9_TX = 10,
    DMA_REQ_UART2_RX = 11,
    DMA_REQ_UART5_TX = 11,
    DMA_REQ_UART9_RX = 11,
    DMA_REQ_UART3_TX = 12,
    DMA_REQ_UART5_RX = 12,
    DMA_REQ_SPI4_TX = 12,
    DMA_REQ_UART3_RX = 13,
    DMA_REQ_UART6_TX = 13,
    DMA_REQ_SPI4_RX = 13,
    DMA_REQ_SPI0_TX = 14,
    DMA_REQ_UART6_RX = 14,
    DMA_REQ_SPI0_RX = 15,
    DMA_REQ_SPI2_TX = 15,
    DMA_REQ_SPI1_TX = 16,
    DMA_REQ_SPI2_RX = 16,
    DMA_REQ_SPDIF_RX1 = 16,
    DMA_REQ_SPI1_RX = 17,
    DMA_REQ_SPI3_TX = 17,
    DMA_REQ_ASRC_4CH0_RX = 17,
    DMA_REQ_SPI3_RX = 18,
    DMA_REQ_ASRC_4CH0_TX = 18,
    DMA_REQ_SAI7_TX = 19,
    DMA_REQ_CAN0_RX = 20,
    DMA_REQ_CAN1_RX = 21,
    DMA_REQ_UART10_TX = 21,
    DMA_REQ_I3C0_RX = 22,
    DMA_REQ_I3C1_RX = 22,
    DMA_REQ_UART10_RX = 22,
    DMA_REQ_I3C0_TX = 23,
    DMA_REQ_I3C1_TX = 23,
    DMA_REQ_UART11_TX = 23,
    DMA_REQ_I3C0_IBI = 24,
    DMA_REQ_I3C1_IBI = 24,
    DMA_REQ_UART11_RX = 24,
    DMA_REQ_SPDIF_TX5 = 25,
    DMA_REQ_ASRC_4CH1_RX = 25,
    DMA_REQ_SAI9_TX = 26,
    DMA_REQ_ASRC_4CH1_TX = 26,
    DMA_REQ_ASRC_2CH0_RX = 27,
    DMA_REQ_ASRC_2CH1_RX = 27,
    DMA_REQ_SPDIF_RX2 = 27,
    DMA_REQ_ASRC_2CH0_TX = 28,
    DMA_REQ_ASRC_2CH1_TX = 28,
    DMA_REQ_SPDIF_TX2 = 28,
    DMA_REQ_SPDIF_TX3 = 29,
    DMA_REQ_DSMC_DMA0 = 30,
    DMA_REQ_DSMC_DMA1 = 31,
} DMA_REQ_Type;
#endif /* __ASSEMBLY__ */

/* ================================================================================ */
/* ================                       IRQ                      ================ */
/* ================================================================================ */
#ifdef HAL_MCU_CORE

#if defined(HAL_BUS_MCU_CORE)

#define INTMUX_NUM_INT_PER_CON    416
#define INTMUX_NUM_OUT_PER_CON    13
#define INTMUX_NUM_INT_PER_OUT    32
#define INTMUX_NUM_GROUP_PER_OUT  1
#define INTMUX_NUM_GROUP_PER_CON  13
#define INTMUX_NUM_INT_PER_GROUP  32
/* INTMUX IRQ start from GIC irq num 64 */
#define INTMUX_IRQ_START_NUM      64
#define INTMUX_IRQ_OUT0
#define INTMUX_IRQ_OUT1
#define INTMUX_IRQ_OUT2
#define INTMUX_IRQ_OUT3
#define INTMUX_IRQ_OUT4
#define INTMUX_IRQ_OUT5
#define INTMUX_IRQ_OUT6
#define INTMUX_IRQ_OUT7
#define INTMUX_IRQ_OUT8
#define INTMUX_IRQ_OUT9
#define INTMUX_IRQ_OUT10
#define INTMUX_IRQ_OUT11
#define INTMUX_IRQ_OUT12
#define INTMUX_OUT_IRQ_START_NUM  16

#define NUM_EXT_INTERRUPTS        416

#ifndef __ASSEMBLY__
typedef enum {
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn       = -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn            = -13,     /*  3 HardFault Interrupt */

  SVCall_IRQn               =  -5,     /* 11 SV Call Interrupt */

  PendSV_IRQn               =  -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn              =  -1,     /* 15 System Tick Interrupt */

/******  Platform Exceptions Numbers ***************************************************/
  GPIO0_EXP3_IRQn           =  0,      /*!< GPIO0 EXP3 Interrupt          */
  GPIO1_EXP3_IRQn           =  1,      /*!< GPIO1 EXP3 Interrupt          */
  GPIO2_EXP3_IRQn           =  2,      /*!< GPIO2 EXP3 Interrupt          */
  GPIO3_EXP3_IRQn           =  3,      /*!< GPIO3 EXP3 Interrupt          */
  GPIO4_EXP3_IRQn           =  4,      /*!< GPIO4 EXP3 Interrupt          */
  SARADC_IRQn               =  5,      /*!< SARADC Interrupt              */
  TIMER9_IRQn               =  6,      /*!< TIMER9 Interrupt              */
  TIMER10_IRQn              =  7,      /*!< TIMER10 Interrupt             */
  SPI4_IRQn                 =  8,      /*!< SPI4 Interrupt                */
  MBOX_BB0_IRQn             =  9,      /*!< MBOX_BB0 Interrupt            */
  MMC0_IRQn                 =  10,     /*!< MMC0 Interrupt                */
  DMAC2_IRQn                =  11,     /*!< DMAC2 Interrupt               */
  I2C3_IRQn                 =  12,     /*!< I2C3 Interrupt                */
  UART11_IRQn               =  13,     /*!< UART11 Interrupt              */
  PWM_CH14_IRQn             =  14,     /*!< PWM CH14 Interrupt            */
  FLEXBUS_IRQn              =  15,     /*!< FLEXBUS Interrupt             */
  INTMUX_OUT0_IRQn          =  16,     /*!< INTMUX OUT0 Interrupt         */
  INTMUX_OUT1_IRQn          =  17,     /*!< INTMUX OUT1 Interrupt         */
  INTMUX_OUT2_IRQn          =  18,     /*!< INTMUX OUT2 Interrupt         */
  INTMUX_OUT3_IRQn          =  19,     /*!< INTMUX OUT3 Interrupt         */
  INTMUX_OUT4_IRQn          =  20,     /*!< INTMUX OUT4 Interrupt         */
  INTMUX_OUT5_IRQn          =  21,     /*!< INTMUX OUT5 Interrupt         */
  INTMUX_OUT6_IRQn          =  22,     /*!< INTMUX OUT6 Interrupt         */
  INTMUX_OUT7_IRQn          =  23,     /*!< INTMUX OUT7 Interrupt         */
  INTMUX_OUT8_IRQn          =  24,     /*!< INTMUX OUT8 Interrupt         */
  INTMUX_OUT9_IRQn          =  25,     /*!< INTMUX OUT9 Interrupt         */
  INTMUX_OUT10_IRQn         =  26,     /*!< INTMUX OUT10 Interrupt        */
  INTMUX_OUT11_IRQn         =  27,     /*!< INTMUX OUT11 Interrupt        */
  INTMUX_OUT12_IRQn         =  28,     /*!< INTMUX OUT12 Interrupt        */
  GPIO0_EXP2_IRQn           =  29,     /*!< GPIO0 EXP2 Interrupt          */
  GPIO4_EXP1_IRQn           =  30,     /*!< GPIO4 EXP1 Interrupt          */
  GPIO4_EXP2_IRQn           =  31,     /*!< GPIO4 EXP2 Interrupt          */
  NUM_INTERRUPTS            =  32,     /*!< Number of internal IRQ        */
  WDT0_IRQn                 =  72 + NUM_INTERRUPTS,     /*!< WDT0 Interrupt                 */
  TIMER0_IRQn               =  77 + NUM_INTERRUPTS,     /*!< TIMER0 Interrupt               */
  TIMER1_IRQn               =  78 + NUM_INTERRUPTS,     /*!< TIMER1 Interrupt               */
  TIMER2_IRQn               =  79 + NUM_INTERRUPTS,     /*!< TIMER2 Interrupt               */
  TIMER3_IRQn               =  80 + NUM_INTERRUPTS,     /*!< TIMER3 Interrupt               */
  TIMER4_IRQn               =  81 + NUM_INTERRUPTS,     /*!< TIMER4 Interrupt               */
  TIMER5_IRQn               =  82 + NUM_INTERRUPTS,     /*!< TIMER5 Interrupt               */
  TIMER6_IRQn               =  83 + NUM_INTERRUPTS,     /*!< TIMER6 Interrupt               */
  TIMER7_IRQn               =  84 + NUM_INTERRUPTS,     /*!< TIMER7 Interrupt               */
  TIMER8_IRQn               =  85 + NUM_INTERRUPTS,     /*!< TIMER8 Interrupt               */
  TIMER11_IRQn              =  88 + NUM_INTERRUPTS,     /*!< TIMER11 Interrupt              */
  UART0_IRQn                =  108 + NUM_INTERRUPTS,    /*!< UART0 Interrupt                */
  UART1_IRQn                =  109 + NUM_INTERRUPTS,    /*!< UART1 Interrupt                */
  UART2_IRQn                =  110 + NUM_INTERRUPTS,    /*!< UART2 Interrupt                */
  UART3_IRQn                =  111 + NUM_INTERRUPTS,    /*!< UART3 Interrupt                */
  UART4_IRQn                =  112 + NUM_INTERRUPTS,    /*!< UART4 Interrupt                */
  UART5_IRQn                =  113 + NUM_INTERRUPTS,    /*!< UART5 Interrupt                */
  UART6_IRQn                =  114 + NUM_INTERRUPTS,    /*!< UART6 Interrupt                */
  UART7_IRQn                =  115 + NUM_INTERRUPTS,    /*!< UART7 Interrupt                */
  UART8_IRQn                =  116 + NUM_INTERRUPTS,    /*!< UART8 Interrupt                */
  UART9_IRQn                =  117 + NUM_INTERRUPTS,    /*!< UART9 Interrupt                */
  UART10_IRQn               =  118 + NUM_INTERRUPTS,    /*!< UART10 Interrupt               */
  PWM0_CH0_IRQn             =  132 + NUM_INTERRUPTS,    /*!< PWM0_CH0 Interrupt             */
  PWM0_CH1_IRQn             =  133 + NUM_INTERRUPTS,    /*!< PWM0_CH1 Interrupt             */
  PWM1_CH0_IRQn             =  134 + NUM_INTERRUPTS,    /*!< PWM1_CH0 Interrupt             */
  PWM1_CH1_IRQn             =  135 + NUM_INTERRUPTS,    /*!< PWM1_CH1 Interrupt             */
  PWM1_CH2_IRQn             =  136 + NUM_INTERRUPTS,    /*!< PWM1_CH2 Interrupt             */
  PWM1_CH3_IRQn             =  137 + NUM_INTERRUPTS,    /*!< PWM1_CH3 Interrupt             */
  PWM1_CH4_IRQn             =  138 + NUM_INTERRUPTS,    /*!< PWM1_CH4 Interrupt             */
  PWM1_CH5_IRQn             =  139 + NUM_INTERRUPTS,    /*!< PWM1_CH5 Interrupt             */
  PWM2_CH0_IRQn             =  140 + NUM_INTERRUPTS,    /*!< PWM2_CH0 Interrupt             */
  PWM2_CH1_IRQn             =  141 + NUM_INTERRUPTS,    /*!< PWM2_CH1 Interrupt             */
  PWM2_CH2_IRQn             =  142 + NUM_INTERRUPTS,    /*!< PWM2_CH2 Interrupt             */
  PWM2_CH3_IRQn             =  143 + NUM_INTERRUPTS,    /*!< PWM2_CH3 Interrupt             */
  PWM2_CH4_IRQn             =  144 + NUM_INTERRUPTS,    /*!< PWM2_CH4 Interrupt             */
  PWM2_CH5_IRQn             =  145 + NUM_INTERRUPTS,    /*!< PWM2_CH5 Interrupt             */
  PWM2_CH6_IRQn             =  146 + NUM_INTERRUPTS,    /*!< PWM2_CH6 Interrupt             */
  PWM2_CH7_IRQn             =  147 + NUM_INTERRUPTS,    /*!< PWM2_CH7 Interrupt             */
  SPI0_IRQn                 =  148 + NUM_INTERRUPTS,    /*!< SPI0 Interrupt                 */
  SPI1_IRQn                 =  149 + NUM_INTERRUPTS,    /*!< SPI1 Interrupt                 */
  SPI2_IRQn                 =  150 + NUM_INTERRUPTS,    /*!< SPI2 Interrupt                 */
  SPI3_IRQn                 =  151 + NUM_INTERRUPTS,    /*!< SPI3 Interrupt                 */
  TSADC_IRQn                =  155 + NUM_INTERRUPTS,    /*!< TSADC Interrupt                */
  MBOX_BB1_IRQn             =  172 + NUM_INTERRUPTS,    /*!< MBOX1 Interrupt                */
  MBOX_BB2_IRQn             =  173 + NUM_INTERRUPTS,    /*!< MBOX2 Interrupt                */
  MBOX_BB3_IRQn             =  174 + NUM_INTERRUPTS,    /*!< MBOX3 Interrupt                */
  MBOX_BB4_IRQn             =  175 + NUM_INTERRUPTS,    /*!< MBOX4 Interrupt                */
  MBOX_BB5_IRQn             =  176 + NUM_INTERRUPTS,    /*!< MBOX5 Interrupt                */
  MBOX_BB6_IRQn             =  177 + NUM_INTERRUPTS,    /*!< MBOX6 Interrupt                */
  MBOX_BB7_IRQn             =  178 + NUM_INTERRUPTS,    /*!< MBOX7 Interrupt                */
  MBOX_BB8_IRQn             =  179 + NUM_INTERRUPTS,    /*!< MBOX8 Interrupt                */
  MBOX_BB9_IRQn             =  180 + NUM_INTERRUPTS,    /*!< MBOX9 Interrupt                */
  MBOX_BB10_IRQn            =  181 + NUM_INTERRUPTS,    /*!< MBOX10 Interrupt               */
  MBOX_BB11_IRQn            =  182 + NUM_INTERRUPTS,    /*!< MBOX11 Interrupt               */
  MBOX_BB12_IRQn            =  183 + NUM_INTERRUPTS,    /*!< MBOX12 Interrupt               */
  MBOX_BB13_IRQn            =  184 + NUM_INTERRUPTS,    /*!< MBOX13 Interrup                */
  GPIO0_IRQn                =  185 + NUM_INTERRUPTS,    /*!< GPIO0 Interrupt                */
  GPIO0_EXP1_IRQn           =  186 + NUM_INTERRUPTS,    /*!< GPIO0 EXP1 Interrupt           */
  GPIO1_IRQn                =  189 + NUM_INTERRUPTS,    /*!< GPIO1 Interrupt                */
  GPIO1_EXP1_IRQn           =  190 + NUM_INTERRUPTS,    /*!< GPIO1 EXP1 Interrupt           */
  GPIO1_EXP2_IRQn           =  191 + NUM_INTERRUPTS,    /*!< GPIO1 EXP2 Interrupt           */
  GPIO2_IRQn                =  193 + NUM_INTERRUPTS,    /*!< GPIO2 Interrupt                */
  GPIO2_EXP1_IRQn           =  194 + NUM_INTERRUPTS,    /*!< GPIO2 EXP1 Interrupt           */
  GPIO2_EXP2_IRQn           =  195 + NUM_INTERRUPTS,    /*!< GPIO2 EXP2 Interrupt           */
  GPIO3_IRQn                =  197 + NUM_INTERRUPTS,    /*!< GPIO3 Interrupt                */
  GPIO3_EXP1_IRQn           =  198 + NUM_INTERRUPTS,    /*!< GPIO3 EXP1 Interrupt           */
  GPIO3_EXP2_IRQn           =  199 + NUM_INTERRUPTS,    /*!< GPIO3 EXP2 Interrupt           */
  GPIO4_IRQn                =  201 + NUM_INTERRUPTS,    /*!< GPIO4 Interrupt                */
  FSPI0_IRQn                =  286 + NUM_INTERRUPTS,    /*!< FSPI0 Interrupt                */
  FSPI1_IRQn                =  287 + NUM_INTERRUPTS,    /*!< FSPI1 Interrupt                */
  TOTAL_INTERRUPTS          =  (INTMUX_IRQ_START_NUM + NUM_INTERRUPTS + NUM_EXT_INTERRUPTS),
} IRQn_Type;
#endif /* __ASSEMBLY__ */

#elif defined(HAL_PMU_MCU_CORE)

#define INTMUX_NUM_INT_PER_CON    416
#define INTMUX_NUM_OUT_PER_CON    13
#define INTMUX_NUM_INT_PER_OUT    32
#define INTMUX_NUM_GROUP_PER_OUT  1
#define INTMUX_NUM_GROUP_PER_CON  13
#define INTMUX_NUM_INT_PER_GROUP  32
/* INTMUX IRQ start from GIC irq num 64 */
#define INTMUX_IRQ_START_NUM      64
#define INTMUX_IRQ_OUT0
#define INTMUX_IRQ_OUT1
#define INTMUX_IRQ_OUT2
#define INTMUX_IRQ_OUT3
#define INTMUX_IRQ_OUT4
#define INTMUX_IRQ_OUT5
#define INTMUX_IRQ_OUT6
#define INTMUX_IRQ_OUT7
#define INTMUX_IRQ_OUT8
#define INTMUX_IRQ_OUT9
#define INTMUX_IRQ_OUT10
#define INTMUX_IRQ_OUT11
#define INTMUX_IRQ_OUT12
#define INTMUX_OUT_IRQ_START_NUM  14

#define NUM_EXT_INTERRUPTS        416

#ifndef __ASSEMBLY__
typedef enum {
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn       = -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn            = -13,     /*  3 HardFault Interrupt */

  SVCall_IRQn               =  -5,     /* 11 SV Call Interrupt */

  PendSV_IRQn               =  -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn              =  -1,     /* 15 System Tick Interrupt */

/******  Platform Exceptions Numbers ***************************************************/
  GPIO0_IRQn                =  0,      /*!< GPIO0 Interrupt               */
  I2C0_IRQn                 =  1,      /*!< I2C0 Interrupt                */
  VAD_IRQn                  =  2,      /*!< VAD Interrupt                 */
  PWM_CH0_IRQn              =  3,      /*!< PWM CH0 Interrupt             */
  PWM_CH1_IRQn              =  4,      /*!< PWM CH1 Interrupt             */
  PMU_WDT_IRQn              =  5,      /*!< PMU WDT Interrupt             */
  PMU_TIMER1_IRQn           =  6,      /*!< PMU TIMER1 Interrupt          */
  PMU_TIMER0_IRQn           =  7,      /*!< PMU TIMER0 Interrupt          */
  SDMMC_DETECT_IRQn         =  8,      /*!< SDMMC DETECT Interrupt        */
  UART1_IRQn                =  9,      /*!< UART1 Interrupt               */
  PMU_CRC_CHK_RST_IRQn      =  10,     /*!< PMU CRC CHK RST Interrupt     */
  OSC_CHK_RST_IRQn          =  11,     /*!< OSC CHK RST Interrupt         */
  PMU_MCU_CACHE_IRQn        =  12,     /*!< PMU MCU CACHE Interrupt       */
  PMIC_IRQn                 =  13,     /*!< PMIC Interrupt                */
  INTMUX_OUT0_IRQn          =  14,     /*!< INTMUX OUT0 Interrupt         */
  INTMUX_OUT1_IRQn          =  15,     /*!< INTMUX OUT1 Interrupt         */
  INTMUX_OUT2_IRQn          =  16,     /*!< INTMUX OUT2 Interrupt         */
  INTMUX_OUT3_IRQn          =  17,     /*!< INTMUX OUT3 Interrupt         */
  INTMUX_OUT4_IRQn          =  18,     /*!< INTMUX OUT4 Interrupt         */
  INTMUX_OUT5_IRQn          =  19,     /*!< INTMUX OUT5 Interrupt         */
  INTMUX_OUT6_IRQn          =  20,     /*!< INTMUX OUT6 Interrupt         */
  INTMUX_OUT7_IRQn          =  21,     /*!< INTMUX OUT7 Interrupt         */
  INTMUX_OUT8_IRQn          =  22,     /*!< INTMUX OUT8 Interrupt         */
  INTMUX_OUT9_IRQn          =  23,     /*!< INTMUX OUT9 Interrupt         */
  INTMUX_OUT10_IRQn         =  24,     /*!< INTMUX OUT10 Interrupt        */
  INTMUX_OUT11_IRQn         =  25,     /*!< INTMUX OUT11 Interrupt        */
  INTMUX_OUT12_IRQn         =  26,     /*!< INTMUX OUT12 Interrupt        */
  PDM0_IRQn                 =  27,     /*!< PDM0 Interrupt                */
  MBOX_BB6_IRQn             =  28,     /*!< MBOX BB6 Interrupt            */
  PMU_PVTM_IRQn             =  29,     /*!< PMU PVTM Interrupt            */
  NUM_INTERRUPTS            =  32,     /*!< Number of internal IRQ        */
  TIMER0_IRQn               =  77 + NUM_INTERRUPTS,     /*!< TIMER0 Interrupt               */
  TIMER1_IRQn               =  78 + NUM_INTERRUPTS,     /*!< TIMER1 Interrupt               */
  TIMER2_IRQn               =  79 + NUM_INTERRUPTS,     /*!< TIMER2 Interrupt               */
  TIMER3_IRQn               =  80 + NUM_INTERRUPTS,     /*!< TIMER3 Interrupt               */
  TIMER4_IRQn               =  81 + NUM_INTERRUPTS,     /*!< TIMER4 Interrupt               */
  TIMER5_IRQn               =  82 + NUM_INTERRUPTS,     /*!< TIMER5 Interrupt               */
  TIMER6_IRQn               =  83 + NUM_INTERRUPTS,     /*!< TIMER6 Interrupt               */
  TIMER7_IRQn               =  84 + NUM_INTERRUPTS,     /*!< TIMER7 Interrupt               */
  TIMER8_IRQn               =  85 + NUM_INTERRUPTS,     /*!< TIMER8 Interrupt               */
  TIMER9_IRQn               =  86 + NUM_INTERRUPTS,     /*!< TIMER9 Interrupt               */
  TIMER10_IRQn              =  87 + NUM_INTERRUPTS,     /*!< TIMER10 Interrupt              */
  TIMER11_IRQn              =  88 + NUM_INTERRUPTS,     /*!< TIMER11 Interrupt              */
  UART0_IRQn                =  108 + NUM_INTERRUPTS,    /*!< UART0 Interrupt                */
  UART2_IRQn                =  110 + NUM_INTERRUPTS,    /*!< UART2 Interrupt                */
  UART3_IRQn                =  111 + NUM_INTERRUPTS,    /*!< UART3 Interrupt                */
  UART4_IRQn                =  112 + NUM_INTERRUPTS,    /*!< UART4 Interrupt                */
  UART5_IRQn                =  113 + NUM_INTERRUPTS,    /*!< UART5 Interrupt                */
  UART6_IRQn                =  114 + NUM_INTERRUPTS,    /*!< UART6 Interrupt                */
  UART7_IRQn                =  115 + NUM_INTERRUPTS,    /*!< UART7 Interrupt                */
  UART8_IRQn                =  116 + NUM_INTERRUPTS,    /*!< UART8 Interrupt                */
  UART9_IRQn                =  117 + NUM_INTERRUPTS,    /*!< UART9 Interrupt                */
  UART10_IRQn               =  118 + NUM_INTERRUPTS,    /*!< UART10 Interrupt               */
  UART11_IRQn               =  119 + NUM_INTERRUPTS,    /*!< UART11 Interrupt               */
  SPI0_IRQn                 =  148 + NUM_INTERRUPTS,    /*!< SPI0 Interrupt                 */
  SPI1_IRQn                 =  149 + NUM_INTERRUPTS,    /*!< SPI1 Interrupt                 */
  SPI2_IRQn                 =  150 + NUM_INTERRUPTS,    /*!< SPI2 Interrupt                 */
  SPI3_IRQn                 =  151 + NUM_INTERRUPTS,    /*!< SPI3 Interrupt                 */
  SPI4_IRQn                 =  152 + NUM_INTERRUPTS,    /*!< SPI4 Interrupt                 */
  MBOX_BB0_IRQn             =  171 + NUM_INTERRUPTS,    /*!< MBOX0 Interrupt                */
  MBOX_BB1_IRQn             =  172 + NUM_INTERRUPTS,    /*!< MBOX1 Interrupt                */
  MBOX_BB2_IRQn             =  173 + NUM_INTERRUPTS,    /*!< MBOX2 Interrupt                */
  MBOX_BB3_IRQn             =  174 + NUM_INTERRUPTS,    /*!< MBOX3 Interrupt                */
  MBOX_BB4_IRQn             =  175 + NUM_INTERRUPTS,    /*!< MBOX4 Interrupt                */
  MBOX_BB5_IRQn             =  176 + NUM_INTERRUPTS,    /*!< MBOX5 Interrupt                */
  MBOX_BB7_IRQn             =  178 + NUM_INTERRUPTS,    /*!< MBOX7 Interrupt                */
  MBOX_BB8_IRQn             =  179 + NUM_INTERRUPTS,    /*!< MBOX8 Interrupt                */
  MBOX_BB9_IRQn             =  180 + NUM_INTERRUPTS,    /*!< MBOX9 Interrupt                */
  MBOX_BB10_IRQn            =  181 + NUM_INTERRUPTS,    /*!< MBOX10 Interrupt               */
  MBOX_BB11_IRQn            =  182 + NUM_INTERRUPTS,    /*!< MBOX11 Interrupt               */
  MBOX_BB12_IRQn            =  183 + NUM_INTERRUPTS,    /*!< MBOX12 Interrupt               */
  MBOX_BB13_IRQn            =  184 + NUM_INTERRUPTS,    /*!< MBOX13 Interrup                */
  GPIO0_EXP1_IRQn           =  186 + NUM_INTERRUPTS,    /*!< GPIO0 EXP1 Interrupt           */
  GPIO0_EXP2_IRQn           =  187 + NUM_INTERRUPTS,    /*!< GPIO0 EXP2 Interrupt           */
  GPIO0_EXP3_IRQn           =  188 + NUM_INTERRUPTS,    /*!< GPIO0 EXP3 Interrupt           */
  GPIO1_IRQn                =  189 + NUM_INTERRUPTS,    /*!< GPIO1 Interrupt                */
  GPIO1_EXP1_IRQn           =  190 + NUM_INTERRUPTS,    /*!< GPIO1 EXP1 Interrupt           */
  GPIO1_EXP2_IRQn           =  191 + NUM_INTERRUPTS,    /*!< GPIO1 EXP2 Interrupt           */
  GPIO1_EXP3_IRQn           =  192 + NUM_INTERRUPTS,    /*!< GPIO1 EXP3 Interrupt           */
  GPIO2_IRQn                =  193 + NUM_INTERRUPTS,    /*!< GPIO2 Interrupt                */
  GPIO2_EXP1_IRQn           =  194 + NUM_INTERRUPTS,    /*!< GPIO2 EXP1 Interrupt           */
  GPIO2_EXP2_IRQn           =  195 + NUM_INTERRUPTS,    /*!< GPIO2 EXP2 Interrupt           */
  GPIO2_EXP3_IRQn           =  196 + NUM_INTERRUPTS,    /*!< GPIO2 EXP3 Interrupt           */
  GPIO3_IRQn                =  197 + NUM_INTERRUPTS,    /*!< GPIO3 Interrupt                */
  GPIO3_EXP1_IRQn           =  198 + NUM_INTERRUPTS,    /*!< GPIO3 EXP1 Interrupt           */
  GPIO3_EXP2_IRQn           =  199 + NUM_INTERRUPTS,    /*!< GPIO3 EXP2 Interrupt           */
  GPIO3_EXP3_IRQn           =  200 + NUM_INTERRUPTS,    /*!< GPIO3 EXP3 Interrupt           */
  GPIO4_IRQn                =  201 + NUM_INTERRUPTS,    /*!< GPIO4 Interrupt                */
  GPIO4_EXP1_IRQn           =  202 + NUM_INTERRUPTS,    /*!< GPIO4 EXP1 Interrupt           */
  GPIO4_EXP2_IRQn           =  203 + NUM_INTERRUPTS,    /*!< GPIO4 EXP2 Interrupt           */
  GPIO4_EXP3_IRQn           =  204 + NUM_INTERRUPTS,    /*!< GPIO4 EXP3 Interrupt           */
  FSPI0_IRQn                =  286 + NUM_INTERRUPTS,    /*!< FSPI0 Interrupt                */
  FSPI1_IRQn                =  287 + NUM_INTERRUPTS,    /*!< FSPI1 Interrupt                */
  TOTAL_INTERRUPTS          =  (INTMUX_IRQ_START_NUM + NUM_INTERRUPTS + NUM_EXT_INTERRUPTS),
} IRQn_Type;
#endif /* __ASSEMBLY__ */

#elif defined(HAL_NPU_MCU_CORE)
#define __RISC_V

#ifndef __ASSEMBLY__
typedef enum {
    NPU_EXT0_IRQn            =  0,      /*!< NPU EXT0 Interrupt          */
    NPU_EXT1_IRQn            =  1,      /*!< NPU EXT1 Interrupt          */
    UART1_IRQn               =  2,      /*!< UART1 Interrupt             */
    MBOX_BB12_IRQn           =  3,      /*!< MBOX_BB12 Interrupt         */
    MBOX_BB13_IRQn           =  4,      /*!< MBOX_BB13 Interrupt         */
    TIMER0_IRQn              =  5,      /*!< TIMER0 Interrupt            */
    TIMER1_IRQn              =  6,      /*!< TIMER1 Interrupt            */
    WDT_IRQn                 =  7,      /*!< WDT Interrupt               */
    TIMER_INTER_IRQn         =  8,      /*!< TIMER_INTER Interrupt       */
    SOFT_IRQn                =  9,      /*!< SOFT Interrupt              */
} IRQn_Type;
#endif /* __ASSEMBLY__ */
#else
#error missing IRQn_Type define for interrupt
#endif

#define HAS_CUSTOME_INTC

#else /* HAL_AP_CORE */

#ifndef __ASSEMBLY__
typedef enum {
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

/******  Platform Exceptions Numbers ***************************************************/
  WDT0_IRQn                 =  72,     /*!< WDT0 Interrupt                 */
  TIMER0_IRQn               =  77,     /*!< TIMER0 Interrupt               */
  TIMER1_IRQn               =  78,     /*!< TIMER1 Interrupt               */
  TIMER2_IRQn               =  79,     /*!< TIMER2 Interrupt               */
  TIMER3_IRQn               =  80,     /*!< TIMER3 Interrupt               */
  TIMER4_IRQn               =  81,     /*!< TIMER4 Interrupt               */
  TIMER5_IRQn               =  82,     /*!< TIMER5 Interrupt               */
  TIMER6_IRQn               =  83,     /*!< TIMER6 Interrupt               */
  TIMER7_IRQn               =  84,     /*!< TIMER7 Interrupt               */
  TIMER8_IRQn               =  85,     /*!< TIMER8 Interrupt               */
  TIMER11_IRQn              =  88,     /*!< TIMER11 Interrupt              */
  UART0_IRQn                =  108,    /*!< UART0 Interrupt                */
  UART1_IRQn                =  109,    /*!< UART1 Interrupt                */
  UART2_IRQn                =  110,    /*!< UART2 Interrupt                */
  UART3_IRQn                =  111,    /*!< UART3 Interrupt                */
  UART4_IRQn                =  112,    /*!< UART4 Interrupt                */
  UART5_IRQn                =  113,    /*!< UART5 Interrupt                */
  UART6_IRQn                =  114,    /*!< UART6 Interrupt                */
  UART7_IRQn                =  115,    /*!< UART7 Interrupt                */
  UART8_IRQn                =  116,    /*!< UART8 Interrupt                */
  UART9_IRQn                =  117,    /*!< UART9 Interrupt                */
  UART10_IRQn               =  118,    /*!< UART10 Interrupt               */
  UART11_IRQn               =  119,    /*!< UART10 Interrupt               */
  PWM0_CH0_IRQn             =  132,    /*!< PWM0_CH0 Interrupt             */
  PWM0_CH1_IRQn             =  133,    /*!< PWM0_CH1 Interrupt             */
  PWM1_CH0_IRQn             =  134,    /*!< PWM1_CH0 Interrupt             */
  PWM1_CH1_IRQn             =  135,    /*!< PWM1_CH1 Interrupt             */
  PWM1_CH2_IRQn             =  136,    /*!< PWM1_CH2 Interrupt             */
  PWM1_CH3_IRQn             =  137,    /*!< PWM1_CH3 Interrupt             */
  PWM1_CH4_IRQn             =  138,    /*!< PWM1_CH4 Interrupt             */
  PWM1_CH5_IRQn             =  139,    /*!< PWM1_CH5 Interrupt             */
  PWM2_CH0_IRQn             =  140,    /*!< PWM2_CH0 Interrupt             */
  PWM2_CH1_IRQn             =  141,    /*!< PWM2_CH1 Interrupt             */
  PWM2_CH2_IRQn             =  142,    /*!< PWM2_CH2 Interrupt             */
  PWM2_CH3_IRQn             =  143,    /*!< PWM2_CH3 Interrupt             */
  PWM2_CH4_IRQn             =  144,    /*!< PWM2_CH4 Interrupt             */
  PWM2_CH5_IRQn             =  145,    /*!< PWM2_CH5 Interrupt             */
  PWM2_CH6_IRQn             =  146,    /*!< PWM2_CH6 Interrupt             */
  PWM2_CH7_IRQn             =  147,    /*!< PWM2_CH7 Interrupt             */
  SPI0_IRQn                 =  148,    /*!< SPI0 Interrupt                 */
  SPI1_IRQn                 =  149,    /*!< SPI1 Interrupt                 */
  SPI2_IRQn                 =  150,    /*!< SPI2 Interrupt                 */
  SPI3_IRQn                 =  151,    /*!< SPI3 Interrupt                 */
  TSADC_IRQn                =  155,    /*!< TSADC Interrupt                */
  MBOX_AP0_IRQn             =  157,    /*!< MBOX_AP0 Interrupt             */
  MBOX_AP1_IRQn             =  158,    /*!< MBOX_AP1 Interrupt             */
  MBOX_AP2_IRQn             =  159,    /*!< MBOX_AP2 Interrupt             */
  MBOX_AP3_IRQn             =  160,    /*!< MBOX_AP3 Interrupt             */
  MBOX_AP4_IRQn             =  161,    /*!< MBOX_AP4 Interrupt             */
  MBOX_AP5_IRQn             =  162,    /*!< MBOX_AP5 Interrupt             */
  MBOX_AP6_IRQn             =  163,    /*!< MBOX_AP6 Interrupt             */
  MBOX_AP7_IRQn             =  164,    /*!< MBOX_AP7 Interrupt             */
  MBOX_AP8_IRQn             =  165,    /*!< MBOX_AP8 Interrupt             */
  MBOX_AP9_IRQn             =  166,    /*!< MBOX_AP9 Interrupt             */
  MBOX_AP10_IRQn            =  167,    /*!< MBOX_AP10 Interrupt            */
  MBOX_AP11_IRQn            =  168,    /*!< MBOX_AP11 Interrupt            */
  MBOX_AP12_IRQn            =  169,    /*!< MBOX_AP12 Interrupt            */
  MBOX_AP13_IRQn            =  170,    /*!< MBOX_AP13 Interrupt            */
  MBOX_BB0_IRQn             =  171,    /*!< MBOX_BB0 Interrupt             */
  MBOX_BB1_IRQn             =  172,    /*!< MBOX_BB1 Interrupt             */
  MBOX_BB2_IRQn             =  173,    /*!< MBOX_BB2 Interrupt             */
  MBOX_BB3_IRQn             =  174,    /*!< MBOX_BB3 Interrupt             */
  MBOX_BB4_IRQn             =  175,    /*!< MBOX_BB4 Interrupt             */
  MBOX_BB5_IRQn             =  176,    /*!< MBOX_BB5 Interrupt             */
  MBOX_BB6_IRQn             =  177,    /*!< MBOX_BB6 Interrupt             */
  MBOX_BB7_IRQn             =  178,    /*!< MBOX_BB7 Interrupt             */
  MBOX_BB8_IRQn             =  179,    /*!< MBOX_BB8 Interrupt             */
  MBOX_BB9_IRQn             =  180,    /*!< MBOX_BB9 Interrupt             */
  MBOX_BB10_IRQn            =  181,    /*!< MBOX_BB10 Interrupt            */
  MBOX_BB11_IRQn            =  182,    /*!< MBOX_BB11 Interrupt            */
  MBOX_BB12_IRQn            =  183,    /*!< MBOX_BB12 Interrupt            */
  MBOX_BB13_IRQn            =  184,    /*!< MBOX_BB13 Interrupt            */
  GPIO0_IRQn                =  185,    /*!< GPIO0 Interrupt                */
  GPIO0_EXP1_IRQn           =  186,    /*!< GPIO0 EXP1 Interrupt           */
  GPIO0_EXP2_IRQn           =  187,    /*!< GPIO0 EXP2 Interrupt           */
  GPIO0_EXP3_IRQn           =  188,    /*!< GPIO0 EXP3 Interrupt           */
  GPIO1_IRQn                =  189,    /*!< GPIO1 Interrupt                */
  GPIO1_EXP1_IRQn           =  190,    /*!< GPIO1 EXP1 Interrupt           */
  GPIO1_EXP2_IRQn           =  191,    /*!< GPIO1 EXP2 Interrupt           */
  GPIO1_EXP3_IRQn           =  192,    /*!< GPIO1 EXP3 Interrupt           */
  GPIO2_IRQn                =  193,    /*!< GPIO2 Interrupt                */
  GPIO2_EXP1_IRQn           =  194,    /*!< GPIO2 EXP1 Interrupt           */
  GPIO2_EXP2_IRQn           =  195,    /*!< GPIO2 EXP2 Interrupt           */
  GPIO2_EXP3_IRQn           =  196,    /*!< GPIO2 EXP3 Interrupt           */
  GPIO3_IRQn                =  197,    /*!< GPIO3 Interrupt                */
  GPIO3_EXP1_IRQn           =  198,    /*!< GPIO3 EXP1 Interrupt           */
  GPIO3_EXP2_IRQn           =  199,    /*!< GPIO3 EXP2 Interrupt           */
  GPIO3_EXP3_IRQn           =  200,    /*!< GPIO3 EXP3 Interrupt           */
  GPIO4_IRQn                =  201,    /*!< GPIO4 Interrupt                */
  GPIO4_EXP1_IRQn           =  202,    /*!< GPIO4 EXP1 Interrupt           */
  GPIO4_EXP2_IRQn           =  203,    /*!< GPIO4 EXP2 Interrupt           */
  GPIO4_EXP3_IRQn           =  204,    /*!< GPIO4 EXP3 Interrupt           */
  FSPI0_IRQn                =  286,    /*!< FSPI0 Interrupt                */
  FSPI1_IRQn                =  287,    /*!< FSPI1 Interrupt                */
  NUM_INTERRUPTS            =  512,
} IRQn_Type;
#endif /* __ASSEMBLY__ */

#define RSVD_IRQn(_N)               (RSVD0_IRQn + (_N))

#endif

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

#define PLL_INPUT_32K_RATE       (32 * 1000)
#define PLL_INPUT_OSC_RATE       (24 * 1000 * 1000)

#define MBOX_CNT                  14         /* Total Mailbox in SoC */

/* --------  Configuration of Core Peripherals  ----------------------------------- */
#if defined(HAL_AP_CORE) && defined(HAL_MCU_CORE)
#error "HAL_AP_CORE and HAL_MCU_CORE only one of them can be enabled"
#endif

#if !defined(HAL_AP_CORE) && !defined(HAL_MCU_CORE)
#error "Please define HAL_AP_CORE or HAL_MCU_CORE on hal_conf.h"
#endif

#ifdef HAL_MCU_CORE
#define __CM0_REV                 0x0000U   /* Core revision r0p0 */
#define __MPU_PRESENT             0U        /* no MPU present */
#define __VTOR_PRESENT            0U        /* no VTOR present */
#define __NVIC_PRIO_BITS          2U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */

#define NVIC_PERIPH_IRQ_OFFSET    16U
#define MAX_INTERRUPT_VECTOR      64U

#else /* HAL_AP_CORE */

#define __CORTEX_A                53U       /* Cortex-A53 Core */
#define __FPU_PRESENT             1U        /* FPU present */
#define __TIM_PRESENT             1U        /* Generic Timer */

#define CACHE_LINE_SHIFT          (6U)
#define CACHE_LINE_SIZE           (0x1U << CACHE_LINE_SHIFT)

#define HAL_GIC_V2                1U        /* GIC version 2 */

#endif

#ifndef __ASSEMBLY__
#include "cmsis_compiler.h"                 /* CMSIS compiler specific defines */
#ifdef __CORTEX_A
#include "core_ca.h"
#elif defined(HAL_MCU_CORE) && !defined(__RISC_V)
#include "core_cm0.h"
#endif
#include "system_rk3576.h"
#endif /* __ASSEMBLY__ */
#include "rk3576.h"

/****************************************************************************************/
/*                                                                                      */
/*                                Module Address Section                                */
/*                                                                                      */
/****************************************************************************************/
/* Memory Base */
#define PCIE0_DBI_BASE                  (0x22000000U + MCU_OFFSET) /* PCIE0 DBI base address */
#define PCIE1_DBI_BASE                  (0x22400000U + MCU_OFFSET) /* PCIE1 DBI base address */
#define PCIE0_APB_BASE                  (0x2A200000U + MCU_OFFSET) /* PCIE0 APB base address */
#define PCIE1_APB_BASE                  (0x2A210000U + MCU_OFFSET) /* PCIE1 APB base address */
#define GIC_DISTRIBUTOR_BASE            (0x2A701000) /* GICD base address */
#define GIC_CPU_INTERFACE_BASE          (0x2A702000) /* GICC base address */

/****************************************************************************************/
/*                                                                                      */
/*                               Register Bitmap Section                                */
/*                                                                                      */
/****************************************************************************************/

#ifdef HAL_AP_CORE
/********************************** CPU Topology ****************************************/
#define MPIDR_MT_MASK                      ((1U) << 24)
#define MPIDR_AFFLVL_MASK                  (0xFFU)
#define MPIDR_AFF0_SHIFT                   (0U)
#define MPIDR_AFF1_SHIFT                   (8U)
#define MPIDR_AFF2_SHIFT                   (16U)
#define MPIDR_AFFINITY_MASK                (0xFFFFFFU)
#define PLATFORM_CLUSTER0_CORE_COUNT       (4)
#define PLATFORM_CLUSTER1_CORE_COUNT       (4)
#define PLATFORM_CORE_COUNT                (PLATFORM_CLUSTER0_CORE_COUNT + PLATFORM_CLUSTER1_CORE_COUNT)
#define CPU_GET_AFFINITY(cpuId, clusterId) (((cpuId) << MPIDR_AFF0_SHIFT) | ((clusterId) << MPIDR_AFF1_SHIFT))

#endif /* HAL_AP_CORE */

#if defined(HAL_MCU_CORE) && defined(__CORTEX_M)
/*****************************************CACHE*****************************************/
/* CACHE LINE SIZE */
#define CACHE_LINE_SHIFT                (5U)
#define CACHE_LINE_SIZE                 (0x1U << CACHE_LINE_SHIFT)
#define CACHE_LINE_ADDR_MASK            (0xFFFFFFFFU << CACHE_LINE_SHIFT)
#define CACHE_M_CLEAN                   0x0U
#define CACHE_M_INVALID                 0x2U
#define CACHE_M_CLEAN_INVALID           0x4U
#define CACHE_M_INVALID_ALL             0x6U
#define CACHE_REVISION                  (0x00000100U)

#ifndef HAL_CACHE_DECODED_ADDR_BASE
#error "Please define HAL_CACHE_DECODED_ADDR_BASE on hal_conf.h"
#endif

#endif /* defined(HAL_MCU_CORE) && defined(__CORTEX_M) */

/****************************************WDT*********************************************/
#define WDT_CR_WDT_EN_MASK WDT_CR_EN_MASK
#define GLB_RST_SND_WDT GLB_RST_SND_WDT0
#define GLB_RST_FST_WDT GLB_RST_FST_WDT0
/****************************************FSPI********************************************/
#define FSPI_CHIP_CNT                            (2)
/****************************************************************************************/
/*                                                                                      */
/*                           Platform Differences Section                               */
/*                                                                                      */
/****************************************************************************************/
#if defined(HAL_AP_CORE)

#undef DCACHE
#undef ICACHE

#endif

/******************************************CRU*******************************************/
#define CRU_CLK_USE_CON_BANK
#define CLK64(mux, div) ((((mux) & 0xffffffffULL) << 32) | ((div) & 0xffffffffULL))

#define CLK_PDM0              CLK_PDM0_SRC_TOP
#define MCLK_PDM0             MCLK_PDM0_SRC_TOP
#define CLK_PDM0_OUT          CLK_PDM0
#define CLK_PDM1_OUT          CLK_PDM1

#ifndef __ASSEMBLY__
typedef enum CLOCK_Name {
    CLK_INVALID = 0U,
    PLL_BPLL,
    PLL_LPLL,
    PLL_VPLL,
    PLL_AUPLL,
    PLL_CPLL,
    PLL_GPLL,
    PLL_PPLL,
    TCLK_WDT0,

    /* top */
    ACLK_TOP            = CLK64(ACLK_TOP_BIU_SEL, ACLK_TOP_BIU_DIV),
    ACLK_TOP_MID        = CLK64(ACLK_TOP_MID_BIU_SEL, ACLK_TOP_MID_BIU_DIV),
    ACLK_SECURE_HIGH    = CLK64(ACLK_SECURE_HIGH_BIU_SEL, ACLK_SECURE_HIGH_BIU_DIV),
    HCLK_TOP            = CLK64(HCLK_TOP_BIU_SEL, 0U),
    HCLK_VO0VOP_CHANNEL = CLK64(HCLK_VO0VOP_CHANNEL_BIU_SEL, 0U),
    ACLK_VO0VOP_CHANNEL = CLK64(ACLK_VO0VOP_CHANNEL_BIU_SEL, ACLK_VO0VOP_CHANNEL_BIU_DIV),

    /* frac */
    CLK_AUDIO_FRAC_0  = CLK64(CLK_MATRIX_AUDIO_FRAC_0_SEL, CLK_MATRIX_AUDIO_FRAC_0_DIV),
    CLK_AUDIO_FRAC_1  = CLK64(CLK_MATRIX_AUDIO_FRAC_1_SEL, CLK_MATRIX_AUDIO_FRAC_1_DIV),
    CLK_AUDIO_FRAC_2  = CLK64(CLK_MATRIX_AUDIO_FRAC_2_SEL, CLK_MATRIX_AUDIO_FRAC_2_DIV),
    CLK_AUDIO_FRAC_3  = CLK64(CLK_MATRIX_AUDIO_FRAC_3_SEL, CLK_MATRIX_AUDIO_FRAC_3_DIV),
    CLK_UART_FRAC_0   = CLK64(CLK_MATRIX_UART_FRAC_0_SEL, CLK_MATRIX_UART_FRAC_0_DIV),
    CLK_UART_FRAC_1   = CLK64(CLK_MATRIX_UART_FRAC_1_SEL, CLK_MATRIX_UART_FRAC_1_DIV),
    CLK_UART_FRAC_2   = CLK64(CLK_MATRIX_UART_FRAC_2_SEL, CLK_MATRIX_UART_FRAC_2_DIV),
    CLK_AUDIO_INT_0   = CLK64(0U, CLK_MATRIX_AUDIO_INT_0_DIV),
    CLK_AUDIO_INT_1   = CLK64(0U, CLK_MATRIX_AUDIO_INT_1_DIV),
    CLK_AUDIO_INT_2   = CLK64(0U, CLK_MATRIX_AUDIO_INT_2_DIV),
    CLK_UART1_SRC_TOP = CLK64(CLK_UART1_SRC_TOP_SEL, CLK_UART1_SRC_TOP_DIV),
    CLK_PDM0_SRC_TOP  = CLK64(CLK_PDM0_SRC_TOP_SEL, CLK_PDM0_SRC_TOP_DIV),
    LCLK_ASRC_SRC_0   = CLK64(LCLK_ASRC_SRC_0_SEL, LCLK_ASRC_SRC_0_DIV),
    LCLK_ASRC_SRC_1   = CLK64(LCLK_ASRC_SRC_1_SEL, LCLK_ASRC_SRC_1_DIV),
    MCLK_PDM0_SRC_TOP = CLK64(MCLK_PDM0_SRC_TOP_SEL, MCLK_PDM0_SRC_TOP_DIV),
    MCLK_PDM1         = CLK64(MCLK_PDM1_SEL, MCLK_PDM1_DIV),
    CLK_PDM1          = CLK64(CLK_PDM1_SEL, CLK_PDM1_DIV),

    /* gmac */
    CLK_GMAC0_125M_SRC = CLK64(0U, CLK_MATRIX_GMAC0_125M_SRC_DIV),
    CLK_GMAC1_125M_SRC = CLK64(0U, CLK_MATRIX_GMAC1_125M_SRC_DIV),

    /* top */
    HCLK_BUS_ROOT        = CLK64(HCLK_BUS_ROOT_SEL, 0U),
    PCLK_BUS_ROOT        = CLK64(PCLK_BUS_ROOT_SEL, 0U),
    ACLK_BUS_ROOT        = CLK64(ACLK_BUS_ROOT_SEL, ACLK_BUS_ROOT_DIV),
    ACLK_CENTER_ROOT     = CLK64(ACLK_CENTER_ROOT_SEL, ACLK_CENTER_ROOT_DIV),
    ACLK_CENTER_LOW_ROOT = CLK64(ACLK_CENTER_LOW_ROOT_SEL, 0U),
    HCLK_CENTER_ROOT     = CLK64(HCLK_CENTER_ROOT_SEL, 0U),
    PCLK_CENTER_ROOT     = CLK64(PCLK_CENTER_ROOT_SEL, 0U),

    CLK_CAN0 = CLK64(CLK_CAN0_SEL, CLK_CAN0_DIV),
    CLK_CAN1 = CLK64(CLK_CAN1_SEL, CLK_CAN1_DIV),

    CLK_I2C1 = CLK64(CLK_I2C1_SEL, 0U),
    CLK_I2C2 = CLK64(CLK_I2C2_SEL, 0U),
    CLK_I2C3 = CLK64(CLK_I2C3_SEL, 0U),
    CLK_I2C4 = CLK64(CLK_I2C4_SEL, 0U),
    CLK_I2C5 = CLK64(CLK_I2C5_SEL, 0U),
    CLK_I2C6 = CLK64(CLK_I2C6_SEL, 0U),
    CLK_I2C7 = CLK64(CLK_I2C7_SEL, 0U),
    CLK_I2C8 = CLK64(CLK_I2C8_SEL, 0U),
    CLK_I2C9 = CLK64(CLK_I2C9_SEL, 0U),

    CLK_TSADC  = CLK64(0U, CLK_TSADC_DIV),
    CLK_SARADC = CLK64(CLK_SARADC_SEL, CLK_SARADC_DIV),

    SCLK_UART0  = CLK64(SCLK_UART0_SEL, SCLK_UART0_DIV),
    SCLK_UART1  = CLK64(SCLK_UART1_SEL, 0U),
    SCLK_UART2  = CLK64(SCLK_UART2_SEL, SCLK_UART2_DIV),
    SCLK_UART3  = CLK64(SCLK_UART3_SEL, SCLK_UART3_DIV),
    SCLK_UART4  = CLK64(SCLK_UART4_SEL, SCLK_UART4_DIV),
    SCLK_UART5  = CLK64(SCLK_UART5_SEL, SCLK_UART5_DIV),
    SCLK_UART6  = CLK64(SCLK_UART6_SEL, SCLK_UART6_DIV),
    SCLK_UART7  = CLK64(SCLK_UART7_SEL, SCLK_UART7_DIV),
    SCLK_UART8  = CLK64(SCLK_UART8_SEL, SCLK_UART8_DIV),
    SCLK_UART9  = CLK64(SCLK_UART9_SEL, SCLK_UART9_DIV),
    SCLK_UART10 = CLK64(SCLK_UART10_SEL, SCLK_UART10_DIV),
    SCLK_UART11 = CLK64(SCLK_UART11_SEL, SCLK_UART11_DIV),

    CLK_SPI0 = CLK64(CLK_SPI0_SEL, 0U),
    CLK_SPI1 = CLK64(CLK_SPI1_SEL, 0U),
    CLK_SPI2 = CLK64(CLK_SPI2_SEL, 0U),
    CLK_SPI3 = CLK64(CLK_SPI3_SEL, 0U),
    CLK_SPI4 = CLK64(CLK_SPI4_SEL, 0U),

    CLK_PWM1 = CLK64(CLK_PWM1_SEL, 0U),
    CLK_PWM2 = CLK64(CLK_PWM2_SEL, 0U),

    DCLK_VP0_SRC = CLK64(DCLK_VP0_SRC_SEL, DCLK_VP0_SRC_DIV),
    DCLK_VP1_SRC = CLK64(DCLK_VP1_SRC_SEL, DCLK_VP1_SRC_DIV),
    DCLK_VP2_SRC = CLK64(DCLK_VP2_SRC_SEL, DCLK_VP2_SRC_DIV),
    DCLK_VP0     = CLK64(DCLK_VP0_SEL, 0U),
    DCLK_VP1     = CLK64(DCLK_VP1_SEL, 0U),
    DCLK_VP2     = CLK64(DCLK_VP2_SEL, 0U),

    DCLK_DECOM    = CLK64(DCLK_DECOM_SEL, DCLK_DECOM_DIV),
    ACLK_NVM_ROOT = CLK64(ACLK_NVM_ROOT_SEL, ACLK_NVM_ROOT_DIV),
    SCLK_FSPI_X2  = CLK64(SCLK_FSPI_X2_SEL, SCLK_FSPI_X2_DIV),
    CCLK_SRC_EMMC = CLK64(CCLK_SRC_EMMC_SEL, CCLK_SRC_EMMC_DIV),
    BCLK_EMMC     = CLK64(BCLK_EMMC_SEL, 0U),
    ACLK_UFS_ROOT = CLK64(ACLK_UFS_ROOT_SEL, ACLK_UFS_ROOT_DIV),
    ACLK_USB_ROOT = CLK64(ACLK_USB_ROOT_SEL, ACLK_USB_ROOT_DIV),

    CCLK_SRC_SDIO   = CLK64(CCLK_SRC_SDIO_SEL, CCLK_SRC_SDIO_DIV),
    CCLK_SRC_SDMMC0 = CLK64(CCLK_SRC_SDMMC0_SEL, CCLK_SRC_SDMMC0_DIV),
    SCLK_FSPI1_X2   = CLK64(SCLK_FSPI1_X2_SEL, SCLK_FSPI1_X2_DIV),

    MCLK_SAI0_8CH_SRC = CLK64(MCLK_SAI0_8CH_SRC_SEL, MCLK_SAI0_8CH_SRC_DIV),
    MCLK_SAI1_8CH_SRC = CLK64(MCLK_SAI1_8CH_SRC_SEL, MCLK_SAI1_8CH_SRC_DIV),
    MCLK_SAI2_2CH_SRC = CLK64(MCLK_SAI2_2CH_SRC_SEL, MCLK_SAI2_2CH_SRC_DIV),
    MCLK_SAI3_2CH_SRC = CLK64(MCLK_SAI3_2CH_SRC_SEL, MCLK_SAI3_2CH_SRC_DIV),
    MCLK_SAI4_2CH_SRC = CLK64(MCLK_SAI4_2CH_SRC_SEL, MCLK_SAI4_2CH_SRC_DIV),
    MCLK_SAI5_8CH_SRC = CLK64(MCLK_SAI5_8CH_SRC_SEL, MCLK_SAI5_8CH_SRC_DIV),
    MCLK_SAI6_8CH_SRC = CLK64(MCLK_SAI6_8CH_SRC_SEL, MCLK_SAI6_8CH_SRC_DIV),
    MCLK_SAI7_8CH_SRC = CLK64(MCLK_SAI7_8CH_SRC_SEL, MCLK_SAI7_8CH_SRC_DIV),
    MCLK_SAI8_8CH_SRC = CLK64(MCLK_SAI8_8CH_SRC_SEL, MCLK_SAI8_8CH_SRC_DIV),
    MCLK_SAI9_8CH_SRC = CLK64(MCLK_SAI9_8CH_SRC_SEL, MCLK_SAI9_8CH_SRC_DIV),
    CLK_ASRC_2CH_0    = CLK64(CLK_ASRC_2CH_0_SEL, CLK_ASRC_2CH_0_DIV),
    CLK_ASRC_2CH_1    = CLK64(CLK_ASRC_2CH_1_SEL, CLK_ASRC_2CH_1_DIV),
    CLK_ASRC_4CH_0    = CLK64(CLK_ASRC_4CH_0_SEL, CLK_ASRC_4CH_0_DIV),
    CLK_ASRC_4CH_1    = CLK64(CLK_ASRC_4CH_1_SEL, CLK_ASRC_4CH_1_DIV),

    MCLK_SPDIF_TX0 = CLK64(MCLK_SPDIF_TX0_SEL, MCLK_SPDIF_TX0_DIV),
    MCLK_SPDIF_TX1 = CLK64(MCLK_SPDIF_TX1_SEL, MCLK_SPDIF_TX1_DIV),
    MCLK_SPDIF_TX2 = CLK64(MCLK_SPDIF_TX2_SEL, MCLK_SPDIF_TX2_DIV),
    MCLK_SPDIF_RX2 = CLK64(MCLK_SPDIFRX2_SEL, MCLK_SPDIFRX2_DIV),
    MCLK_SPDIF_TX3 = CLK64(MCLK_SPDIF_TX3_SEL, MCLK_SPDIF_TX3_DIV),
    MCLK_SPDIF_TX4 = CLK64(MCLK_SPDIF_TX4_SEL, MCLK_SPDIF_TX4_DIV),
    MCLK_SPDIF_TX5 = CLK64(MCLK_SPDIF_TX5_SEL, MCLK_SPDIF_TX5_DIV),

    /* pmu */
    CLK_I2C0    = CLK64(CLK_I2C0_SEL, 0U),
    CLK_PMU1PWM = CLK64(CLK_PMU1PWM_SEL, 0U),
} eCLOCK_Name;
#endif /* __ASSEMBLY__ */

#ifndef __ASSEMBLY__
typedef enum PD_Id {
    PD_INVALID = 0U,
} ePD_Id;
#endif
/****************************************FSPI********************************************/
#define FSPI_CHIP_CNT                            (2)
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __SOC_H */
