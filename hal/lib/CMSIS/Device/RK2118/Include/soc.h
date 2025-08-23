/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2022 Rockchip Electronics Co., Ltd.
 */

#ifndef SOC_H
#define SOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_conf.h"

/* ================================================================================ */
/* ================                    DMA REQ                      =============== */
/* ================================================================================ */
typedef enum {
    DMA0_REQ_BUS_SPI1_TX                  = 0,
    DMA0_REQ_BUS_SPI1_RX                  = 1,
    DMA0_REQ_BUS_SPI2_TX                  = 2,
    DMA0_REQ_BUS_SPI2_RX                  = 3,
    DMA0_REQ_BUS_UART0_TX                 = 4,
    DMA0_REQ_BUS_UART0_RX                 = 5,
    DMA0_REQ_BUS_UART1_TX                 = 6,
    DMA0_REQ_BUS_UART1_RX                 = 7,
    DMA0_REQ_BUS_UART2_TX                 = 8,
    DMA0_REQ_BUS_UART2_RX                 = 9,
    DMA0_REQ_BUS_UART3_TX                 = 10,
    DMA0_REQ_BUS_UART3_RX                 = 11,
    DMA0_REQ_BUS_CAN_RX                   = 12,
    DMA1_REQ_AUDIO0_SAI0_TX               = 0,
    DMA1_REQ_AUDIO0_SAI0_RX               = 1,
    DMA1_REQ_AUDIO0_SAI1_TX               = 2,
    DMA1_REQ_AUDIO0_SAI1_RX               = 3,
    DMA1_REQ_AUDIO0_SAI2_TX               = 4,
    DMA1_REQ_AUDIO0_SAI2_RX               = 5,
    DMA1_REQ_AUDIO0_SAI3_TX               = 6,
    DMA1_REQ_AUDIO0_SAI3_RX               = 7,
    DMA1_REQ_AUDIO0_ASRC0_TX              = 8,
    DMA1_REQ_AUDIO0_ASRC0_RX              = 9,
    DMA1_REQ_AUDIO0_ASRC1_TX              = 10,
    DMA1_REQ_AUDIO0_ASRC1_RX              = 11,
    DMA1_REQ_AUDIO0_ASRC2_TX              = 12,
    DMA1_REQ_AUDIO0_ASRC2_RX              = 13,
    DMA1_REQ_AUDIO0_ASRC3_TX              = 14,
    DMA1_REQ_AUDIO0_ASRC3_RX              = 15,
    DMA1_REQ_AUDIO0_PDM_RX                = 16,
    DMA1_REQ_AUDIO0_SPDIF_RX              = 17,
    DMA2_REQ_AUDIO0_SAI0_TX               = 0,
    DMA2_REQ_AUDIO0_SAI0_RX               = 1,
    DMA2_REQ_AUDIO0_SAI1_TX               = 2,
    DMA2_REQ_AUDIO0_SAI1_RX               = 3,
    DMA2_REQ_AUDIO0_SAI2_TX               = 4,
    DMA2_REQ_AUDIO0_SAI2_RX               = 5,
    DMA2_REQ_AUDIO0_SAI3_TX               = 6,
    DMA2_REQ_AUDIO0_SAI3_RX               = 7,
    DMA2_REQ_AUDIO0_ASRC0_TX              = 8,
    DMA2_REQ_AUDIO0_ASRC0_RX              = 9,
    DMA2_REQ_AUDIO0_ASRC1_TX              = 10,
    DMA2_REQ_AUDIO0_ASRC1_RX              = 11,
    DMA2_REQ_AUDIO0_ASRC2_TX              = 12,
    DMA2_REQ_AUDIO0_ASRC2_RX              = 13,
    DMA2_REQ_AUDIO0_ASRC3_TX              = 14,
    DMA2_REQ_AUDIO0_ASRC3_RX              = 15,
    DMA2_REQ_AUDIO0_PDM_RX                = 16,
    DMA2_REQ_AUDIO0_SPDIF_RX              = 17,
    DMA3_REQ_AUDIO1_SAI4_TX               = 0,
    DMA3_REQ_AUDIO1_SAI4_RX               = 1,
    DMA3_REQ_AUDIO1_SAI5_TX               = 2,
    DMA3_REQ_AUDIO1_SAI5_RX               = 3,
    DMA3_REQ_AUDIO1_SAI6_TX               = 4,
    DMA3_REQ_AUDIO1_SAI6_RX               = 5,
    DMA3_REQ_AUDIO1_SAI7_TX               = 6,
    DMA3_REQ_AUDIO1_SAI7_RX               = 7,
    DMA3_REQ_AUDIO1_ASRC4_TX              = 8,
    DMA3_REQ_AUDIO1_ASRC4_RX              = 9,
    DMA3_REQ_AUDIO1_ASRC5_TX              = 10,
    DMA3_REQ_AUDIO1_ASRC5_RX              = 11,
    DMA3_REQ_AUDIO1_ASRC6_TX              = 12,
    DMA3_REQ_AUDIO1_ASRC6_RX              = 13,
    DMA3_REQ_AUDIO1_ASRC7_TX              = 14,
    DMA3_REQ_AUDIO1_ASRC7_RX              = 15,
    DMA3_REQ_AUDIO1_SPDIF_TX              = 16,
    DMA3_REQ_AUDIO1_SPDIF_RX              = 17,
    DMA4_REQ_AUDIO1_SAI4_TX               = 0,
    DMA4_REQ_AUDIO1_SAI4_RX               = 1,
    DMA4_REQ_AUDIO1_SAI5_TX               = 2,
    DMA4_REQ_AUDIO1_SAI5_RX               = 3,
    DMA4_REQ_AUDIO1_SAI6_TX               = 4,
    DMA4_REQ_AUDIO1_SAI6_RX               = 5,
    DMA4_REQ_AUDIO1_SAI7_TX               = 6,
    DMA4_REQ_AUDIO1_SAI7_RX               = 7,
    DMA4_REQ_AUDIO1_ASRC4_TX              = 8,
    DMA4_REQ_AUDIO1_ASRC4_RX              = 9,
    DMA4_REQ_AUDIO1_ASRC5_TX              = 10,
    DMA4_REQ_AUDIO1_ASRC5_RX              = 11,
    DMA4_REQ_AUDIO1_ASRC6_TX              = 12,
    DMA4_REQ_AUDIO1_ASRC6_RX              = 13,
    DMA4_REQ_AUDIO1_ASRC7_TX              = 14,
    DMA4_REQ_AUDIO1_ASRC7_RX              = 15,
    DMA4_REQ_AUDIO1_SPDIF_TX              = 16,
    DMA4_REQ_AUDIO1_SPDIF_RX              = 17,
} DMA_REQ_Type;

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn           = -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn                = -13,     /*  3 HardFault Interrupt */
  MemoryManagement_IRQn         = -12,     /*  4 Memory Management Interrupt */
  BusFault_IRQn                 = -11,     /*  5 Bus Fault Interrupt */
  UsageFault_IRQn               = -10,     /*  6 Usage Fault Interrupt */
  SecureFault_IRQn              =  -9,     /*  7 Secure Fault Interrupt */
  SVCall_IRQn                   =  -5,     /* 11 SV Call Interrupt */
  DebugMonitor_IRQn             =  -4,     /* 12 Debug Monitor Interrupt */
  PendSV_IRQn                   =  -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn                  =  -1,     /* 15 System Tick Interrupt */

/* -------------------  Processor Interrupt Numbers  ------------------------------ */
  GPIO0_IRQn                    =   0,
  GPIO5_IRQn                    =   1,
  GPIO10_IRQn                   =   2,
  GPIO15_IRQn                   =   3,
  GPIO1_IRQn                    =   4,
  GPIO6_IRQn                    =   5,
  GPIO11_IRQn                   =   6,
  GPIO16_IRQn                   =   7,
  GPIO2_IRQn                    =   8,
  GPIO7_IRQn                    =   9,
  GPIO12_IRQn                   =   10,
  GPIO17_IRQn                   =   11,
  GPIO3_IRQn                    =   12,
  GPIO8_IRQn                    =   13,
  GPIO13_IRQn                   =   14,
  GPIO18_IRQn                   =   15,
  GPIO4_IRQn                    =   16,
  GPIO9_IRQn                    =   17,
  GPIO14_IRQn                   =   18,
  GPIO19_IRQn                   =   19,
  TOUCH_KEY_POS_IRQn            =   20,
  TOUCH_KEY_NEG_IRQn            =   21,
  PWM0_CH0_IRQn                 =   22,
  PWM0_CH1_IRQn                 =   23,
  PWM0_CH2_IRQn                 =   24,
  PWM0_CH3_IRQn                 =   25,
  PWM1_CH0_IRQn                 =   26,
  PWM1_CH1_IRQn                 =   27,
  PWM1_CH2_IRQn                 =   28,
  PWM1_CH3_IRQn                 =   29,
  UART0_IRQn                    =   30,
  UART1_IRQn                    =   31,
  UART2_IRQn                    =   32,
  UART3_IRQn                    =   33,
  I2C0_IRQn                     =   34,
  I2C1_IRQn                     =   35,
  I2C2_IRQn                     =   36,
  I2C3_IRQn                     =   37,
  I2C4_IRQn                     =   38,
  I2C5_IRQn                     =   39,
  SPI1_IRQn                     =   40,
  SPI2_IRQn                     =   41,
  CAN_IRQn                      =   42,
  SAI0_IRQn                     =   43,
  SAI1_IRQn                     =   44,
  SAI2_IRQn                     =   45,
  SAI3_IRQn                     =   46,
  ASRC0_IRQn                    =   47,
  ASRC1_IRQn                    =   48,
  ASRC2_IRQn                    =   49,
  ASRC3_IRQn                    =   50,
  PDM_IRQn                      =   51,
  SPDIF_RX0_IRQn                =   52,
  SAI4_IRQn                     =   53,
  SAI5_IRQn                     =   54,
  SAI6_IRQn                     =   55,
  SAI7_IRQn                     =   56,
  ASRC4_IRQn                    =   57,
  ASRC5_IRQn                    =   58,
  ASRC6_IRQn                    =   59,
  ASRC7_IRQn                    =   60,
  SPDIF_TX_IRQn                 =   61,
  SPDIF_RX1_IRQn                =   62,
  SARADC_IRQn                   =   63,
  TSADC_IRQn                    =   64,
  VOP_IRQn                      =   65,
  VOP_LB_INTR_IRQn              =   66,
  FACC_FIR_IRQn                 =   67,
  FACC_IIR_IRQn                 =   68,
  FLEXBUS_IRQn                  =   69,
  OTPC_NS_IRQn                  =   70,
  OTPC_S_IRQn                   =   71,
  KEY_READER_IRQn               =   72,
  OTPC_MASK_IRQn                =   73,
  MAC_SBD_IRQn                  =   74,
  MAC_SBD_TX_IRQn               =   75,
  MAC_SBD_RX_IRQn               =   76,
  MAC_PMT_IRQn                  =   77,
  MAC_MCGR_IRQn                 =   78,
  OTG_IRQn                      =   79,
  OTG_BVALID_IRQn               =   80,
  OTG_ID_IRQn                   =   81,
  OTG_LINESTATE_IRQn            =   82,
  OTG_DISCONNECT_IRQn           =   83,
  OTG_VBUSVALID_IRQn            =   84,
  SPI2APB_IRQn                  =   85,
  FSPI_IRQn                     =   86,
  SDMMC_IRQn                    =   87,
  SDIO_IRQn                     =   88,
  DDRC_AWPOISON0_IRQn           =   89,
  DDRC_AWPOISON1_IRQn           =   90,
  DDRC_ARPOISON0_IRQn           =   91,
  DDRC_ARPOISON1_IRQn           =   92,
  DDRC_DFI_ALERT_ERR_IRQn       =   93,
  DDR_MONITOR_IRQn              =   94,
  DDRPHY_IRQn                   =   95,
  TIMER0_IRQn                   =   96,
  TIMER1_IRQn                   =   97,
  TIMER2_IRQn                   =   98,
  TIMER3_IRQn                   =   99,
  TIMER4_IRQn                   =   100,
  TIMER5_IRQn                   =   101,
  TIMER6_IRQn                   =   102,
  TIMER7_IRQn                   =   103,
  TIMER8_IRQn                   =   104,
  TIMER9_IRQn                   =   105,
  TIMER10_IRQn                  =   106,
  TIMER11_IRQn                  =   107,
  TIMER12_IRQn                  =   108,
  TIMER13_IRQn                  =   109,
  TIMER14_IRQn                  =   110,
  TIMER15_IRQn                  =   111,
  TIMER16_IRQn                  =   112,
  TIMER17_IRQn                  =   113,
  TIMER18_IRQn                  =   114,
  TIMER19_IRQn                  =   115,
  HPTIMER_IRQn                  =   116,
  WDT0_IRQn                     =   117,
  WDT0_RSTN_IRQn                =   118,
  WDT1_IRQn                     =   119,
  WDT1_RSTN_IRQn                =   120,
  WDT2_IRQn                     =   121,
  WDT2_RSTN_IRQn                =   122,
  WDT3_IRQn                     =   123,
  WDT3_RSTN_IRQn                =   124,
  WDT4_IRQn                     =   125,
  WDT4_RSTN_IRQn                =   126,
  DSP0_ERR_IRQn                 =   127,
  DSP1_ERR_IRQn                 =   128,
  DSP2_ERR_IRQn                 =   129,
  CRYPTO_IRQn                   =   130,
  CRYPTO_KLAD_IRQn              =   131,
  CRYPTO_SC_IRQn                =   132,
  NS_TRNG_IRQn                  =   133,
  S_TRNG_IRQn                   =   134,
  DMAC0_BUS_CH0_IRQn            =   135,
  DMAC0_BUS_CH1_IRQn            =   136,
  DMAC0_BUS_CH2_IRQn            =   137,
  DMAC0_BUS_CH3_IRQn            =   138,
  DMAC0_BUS_CH4_IRQn            =   139,
  DMAC0_BUS_CH5_IRQn            =   140,
  DMAC0_BUS_CH6_IRQn            =   141,
  DMAC0_BUS_CH7_IRQn            =   142,
  DMAC0_BUS_ABORT_IRQn          =   143,
  DMAC1_AUDIO0_CH0_IRQn         =   144,
  DMAC1_AUDIO0_CH1_IRQn         =   145,
  DMAC1_AUDIO0_CH2_IRQn         =   146,
  DMAC1_AUDIO0_CH3_IRQn         =   147,
  DMAC1_AUDIO0_CH4_IRQn         =   148,
  DMAC1_AUDIO0_CH5_IRQn         =   149,
  DMAC1_AUDIO0_CH6_IRQn         =   150,
  DMAC1_AUDIO0_CH7_IRQn         =   151,
  DMAC1_AUDIO0_ABORT_IRQn       =   152,
  DMAC2_AUDIO0_CH0_IRQn         =   153,
  DMAC2_AUDIO0_CH1_IRQn         =   154,
  DMAC2_AUDIO0_CH2_IRQn         =   155,
  DMAC2_AUDIO0_CH3_IRQn         =   156,
  DMAC2_AUDIO0_CH4_IRQn         =   157,
  DMAC2_AUDIO0_CH5_IRQn         =   158,
  DMAC2_AUDIO0_CH6_IRQn         =   159,
  DMAC2_AUDIO0_CH7_IRQn         =   160,
  DMAC2_AUDIO0_ABORT_IRQn       =   161,
  DMAC3_AUDIO1_CH0_IRQn         =   162,
  DMAC3_AUDIO1_CH1_IRQn         =   163,
  DMAC3_AUDIO1_CH2_IRQn         =   164,
  DMAC3_AUDIO1_CH3_IRQn         =   165,
  DMAC3_AUDIO1_CH4_IRQn         =   166,
  DMAC3_AUDIO1_CH5_IRQn         =   167,
  DMAC3_AUDIO1_CH6_IRQn         =   168,
  DMAC3_AUDIO1_CH7_IRQn         =   169,
  DMAC3_AUDIO1_ABORT_IRQn       =   170,
  DMAC4_AUDIO1_CH0_IRQn         =   171,
  DMAC4_AUDIO1_CH1_IRQn         =   172,
  DMAC4_AUDIO1_CH2_IRQn         =   173,
  DMAC4_AUDIO1_CH3_IRQn         =   174,
  DMAC4_AUDIO1_CH4_IRQn         =   175,
  DMAC4_AUDIO1_CH5_IRQn         =   176,
  DMAC4_AUDIO1_CH6_IRQn         =   177,
  DMAC4_AUDIO1_CH7_IRQn         =   178,
  DMAC4_AUDIO1_ABORT_IRQn       =   179,
  NPU_IRQn                      =   180,
  PMU_IRQn                      =   181,
  NPOR_POWERGOOD_IRQn           =   182,
  DMA2DDR_IRQn                  =   183,
  MBOX0_AP_IRQn                 =   184,
  MBOX1_AP_IRQn                 =   185,
  MBOX2_AP_IRQn                 =   186,
  MBOX3_AP_IRQn                 =   187,
  MBOX4_AP_IRQn                 =   188,
  MBOX5_AP_IRQn                 =   189,
  MBOX6_AP_IRQn                 =   190,
  MBOX7_AP_IRQn                 =   191,
  MBOX8_AP_IRQn                 =   192,
  MBOX9_AP_IRQn                 =   193,
  MBOX10_AP_IRQn                =   194,
  MBOX11_AP_IRQn                =   195,
  MBOX12_AP_IRQn                =   196,
  MBOX13_AP_IRQn                =   197,
  MBOX14_AP_IRQn                =   198,
  MBOX15_AP_IRQn                =   199,
  MBOX0_BB_IRQn                 =   200,
  MBOX1_BB_IRQn                 =   201,
  MBOX2_BB_IRQn                 =   202,
  MBOX3_BB_IRQn                 =   203,
  MBOX4_BB_IRQn                 =   204,
  MBOX5_BB_IRQn                 =   205,
  MBOX6_BB_IRQn                 =   206,
  MBOX7_BB_IRQn                 =   207,
  MBOX8_BB_IRQn                 =   208,
  MBOX9_BB_IRQn                 =   209,
  MBOX10_BB_IRQn                =   210,
  MBOX11_BB_IRQn                =   211,
  MBOX12_BB_IRQn                =   212,
  MBOX13_BB_IRQn                =   213,
  MBOX14_BB_IRQn                =   214,
  MBOX15_BB_IRQn                =   215,

  NUM_INTERRUPTS
  /* Interrupts 214 .. 239 are reserved */
} IRQn_Type;

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


/* --------  Configuration of Core Peripherals  ----------------------------------- */
#if defined(HAL_MCU_CORE)
#define __STAR_REV                0x0100U   /* Core revision r1p0 */
#define __SAUREGION_PRESENT       1U        /* SAU regions present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    1U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT             1U        /* FPU present */
#define __DSP_PRESENT             1U        /* DSP extension present */
#define __ICACHE_PRESENT          1U        /* ICache present */
#define __DCACHE_PRESENT          1U        /* DCache present */
#define NVIC_PERIPH_IRQ_OFFSET    16U       /* Interrupt offset for NVIC peripherals*/

#define CACHE_LINE_SHIFT                (5U)
#define CACHE_LINE_SIZE                 (0x1U << CACHE_LINE_SHIFT)
#define CACHE_LINE_ADDR_MASK            (0xFFFFFFFFU << CACHE_LINE_SHIFT)

#include "core_starmc1.h"                   /* Processor and core peripherals */
#include "system_rk2118.h"                  /* System Header */
#include "rk2118.h"                         /* SOC peripherals */
#include "rk2118_usb.h"                     /* USB peripheral */
#elif defined(HAL_DSP_CORE)
#include "core_hifi4.h"                     /* Processor and core peripherals */
#include "system_rk2118.h"                  /* System Header */
#include "rk2118.h"                         /* SOC peripherals */
#else
#error "you must define HAL_MCU_CORE or HAL_DSP_CORE!!!"
#endif

/* switch */
#if defined(ARCH_ARM_CORTEX_SECURE)
/* Memory Base */
#undef FIR_ACC_TCB_BASE
#undef IIR_ACC_TCB_BASE
#undef TRNG1_BASE
#undef CRYPTO_BASE
#undef TRNG0_BASE
#undef CAN_BASE
#undef MMC0_BASE
#undef VOP_BASE
#undef FLEXBUS_BASE
#undef SPI2_BASE
#undef GPIO3_IOC_BASE
#undef RM1_IO_BASE
#undef DMA0_BASE
#undef GRF_BASE
#undef SPINLOCK0_BASE
#undef SPINLOCK1_BASE
#undef SPINLOCK2_BASE
#undef SPINLOCK3_BASE
#undef SPINLOCK4_BASE
#undef SPINLOCK5_BASE
#undef SPINLOCK6_BASE
#undef SPINLOCK7_BASE
#undef MBOX0_BASE
#undef MBOX1_BASE
#undef MBOX2_BASE
#undef MBOX3_BASE
#undef MBOX4_BASE
#undef MBOX5_BASE
#undef MBOX6_BASE
#undef MBOX7_BASE
#undef MBOX8_BASE
#undef MBOX9_BASE
#undef MBOX10_BASE
#undef MBOX11_BASE
#undef MBOX12_BASE
#undef MBOX13_BASE
#undef MBOX14_BASE
#undef MBOX15_BASE
#undef WDT0_BASE
#undef WDT1_BASE
#undef WDT2_BASE
#undef WDT3_BASE
#undef WDT4_BASE
#undef TIMER0_BASE
#undef TIMER1_BASE
#undef TIMER2_BASE
#undef TIMER3_BASE
#undef TIMER4_BASE
#undef TIMER5_BASE
#undef TIMER6_BASE
#undef TIMER7_BASE
#undef TIMER8_BASE
#undef TIMER9_BASE
#undef TIMER10_BASE
#undef TIMER11_BASE
#undef TIMER12_BASE
#undef TIMER13_BASE
#undef TIMER14_BASE
#undef TIMER15_BASE
#undef TIMER16_BASE
#undef TIMER17_BASE
#undef TIMER18_BASE
#undef TIMER19_BASE
#undef INTMUX0_BASE
#undef INTMUX1_BASE
#undef INTMUX2_BASE
#undef INTMUX3_BASE
#undef INTMUX4_BASE
#undef INTMUX5_BASE
#undef INTMUX6_BASE
#undef INTMUX7_BASE
#undef INTMUX8_BASE
#undef INTMUX9_BASE
#undef INTMUX10_BASE
#undef INTMUX11_BASE
#undef I2C0_BASE
#undef I2C1_BASE
#undef I2C2_BASE
#undef I2C3_BASE
#undef I2C4_BASE
#undef I2C5_BASE
#undef UART0_BASE
#undef UART1_BASE
#undef UART2_BASE
#undef UART3_BASE
#undef PWM1_BASE
#undef SPI1_BASE
#undef GPIO1_BASE
#undef GPIO2_BASE
#undef GPIO3_BASE
#undef GPIO4_BASE
#undef SAI0_BASE
#undef SAI1_BASE
#undef SAI2_BASE
#undef SAI3_BASE
#undef ASRC0_BASE
#undef ASRC1_BASE
#undef ASRC2_BASE
#undef ASRC3_BASE
#undef PDM_BASE
#undef SPDIFRX0_BASE
#undef DMA1_BASE
#undef DMA2_BASE
#undef TSADC_BASE
#undef GPIO4_AB_IOC_BASE
#undef RM3_IO_BASE
#undef SAI4_BASE
#undef SAI5_BASE
#undef SAI6_BASE
#undef SAI7_BASE
#undef ASRC4_BASE
#undef ASRC5_BASE
#undef ASRC6_BASE
#undef ASRC7_BASE
#undef SPDIFTX0_BASE
#undef SPDIFRX1_BASE
#undef DMA3_BASE
#undef DMA4_BASE
#undef GPIO4_CD_IOC_BASE
#undef RM4_IO_BASE
#undef MMC1_BASE
#undef FSPI0_BASE
#undef GMAC_BASE
#undef SPI2APB_BASE
#undef OTPC_BASE
#undef SARADC0_BASE
#undef GPIO1_IOC_BASE
#undef GPIO2_IOC_BASE
#undef RM2_IO_BASE
#undef CRU_BASE
#undef SCRU_BASE
#undef PMU_CRU_BASE
#undef PMU_BASE
#undef GRF_PMU_BASE
#undef GPIO0_BASE
#undef TIMER_HP0_BASE
#undef PWM0_BASE
#undef TOUCH_SENSOR_BASE
#undef GPIO0_IOC_BASE
#undef RM0_IO_BASE
#undef FIR_ACC_BASE
#undef IIR_ACC_BASE
#define FIR_ACC_TCB_BASE               0x00000000U /* FIR_ACC_TCB base address */
#define IIR_ACC_TCB_BASE               0x00000000U /* IIR_ACC_TCB base address */
#define TRNG1_BASE                     0x40120000U /* TRNG1 base address */
#define CRYPTO_BASE                    0x40000000U /* CRYPTO base address */
#define TRNG0_BASE                     0x40010000U /* TRNG0 base address */
#define CAN_BASE                       0x400A0000U /* CAN base address */
#define MMC0_BASE                      0x40180000U /* MMC0 base address */
#define VOP_BASE                       0x40190000U /* VOP base address */
#define FLEXBUS_BASE                   0x401A0000U /* FLEXBUS base address */
#define SPI2_BASE                      0x401C0000U /* SPI2 base address */
#define GPIO3_IOC_BASE                 0x401D0000U /* GPIO3_IOC base address */
#define RM1_IO_BASE                    0x401D0000U /* RM1_IO base address */
#define DMA0_BASE                      0x40200000U /* DMA0 base address */
#define GRF_BASE                       0x40210000U /* GRF base address */
#define SPINLOCK0_BASE                 0x40220000U /* SPINLOCK0 base address */
#define SPINLOCK1_BASE                 0x40221000U /* SPINLOCK1 base address */
#define SPINLOCK2_BASE                 0x40222000U /* SPINLOCK2 base address */
#define SPINLOCK3_BASE                 0x40223000U /* SPINLOCK3 base address */
#define SPINLOCK4_BASE                 0x40224000U /* SPINLOCK4 base address */
#define SPINLOCK5_BASE                 0x40225000U /* SPINLOCK5 base address */
#define SPINLOCK6_BASE                 0x40226000U /* SPINLOCK6 base address */
#define SPINLOCK7_BASE                 0x40227000U /* SPINLOCK7 base address */
#define MBOX0_BASE                     0x40240000U /* MBOX0 base address */
#define MBOX1_BASE                     0x40241000U /* MBOX1 base address */
#define MBOX2_BASE                     0x40242000U /* MBOX2 base address */
#define MBOX3_BASE                     0x40243000U /* MBOX3 base address */
#define MBOX4_BASE                     0x40244000U /* MBOX4 base address */
#define MBOX5_BASE                     0x40245000U /* MBOX5 base address */
#define MBOX6_BASE                     0x40246000U /* MBOX6 base address */
#define MBOX7_BASE                     0x40247000U /* MBOX7 base address */
#define MBOX8_BASE                     0x40248000U /* MBOX8 base address */
#define MBOX9_BASE                     0x40249000U /* MBOX9 base address */
#define MBOX10_BASE                    0x4024A000U /* MBOX10 base address */
#define MBOX11_BASE                    0x4024B000U /* MBOX11 base address */
#define MBOX12_BASE                    0x4024C000U /* MBOX12 base address */
#define MBOX13_BASE                    0x4024D000U /* MBOX13 base address */
#define MBOX14_BASE                    0x4024E000U /* MBOX14 base address */
#define MBOX15_BASE                    0x4024F000U /* MBOX15 base address */
#define WDT0_BASE                      0x40260000U /* WDT0 base address */
#define WDT1_BASE                      0x40270000U /* WDT1 base address */
#define WDT2_BASE                      0x40280000U /* WDT2 base address */
#define WDT3_BASE                      0x40290000U /* WDT3 base address */
#define WDT4_BASE                      0x402A0000U /* WDT4 base address */
#define TIMER0_BASE                    0x402C0000U /* TIMER0 base address */
#define TIMER1_BASE                    0x402C1000U /* TIMER1 base address */
#define TIMER2_BASE                    0x402C2000U /* TIMER2 base address */
#define TIMER3_BASE                    0x402C3000U /* TIMER3 base address */
#define TIMER4_BASE                    0x402D0000U /* TIMER4 base address */
#define TIMER5_BASE                    0x402D1000U /* TIMER5 base address */
#define TIMER6_BASE                    0x402D2000U /* TIMER6 base address */
#define TIMER7_BASE                    0x402D3000U /* TIMER7 base address */
#define TIMER8_BASE                    0x402E0000U /* TIMER8 base address */
#define TIMER9_BASE                    0x402E1000U /* TIMER9 base address */
#define TIMER10_BASE                   0x402E2000U /* TIMER10 base address */
#define TIMER11_BASE                   0x402E3000U /* TIMER11 base address */
#define TIMER12_BASE                   0x402F0000U /* TIMER12 base address */
#define TIMER13_BASE                   0x402F1000U /* TIMER13 base address */
#define TIMER14_BASE                   0x402F2000U /* TIMER14 base address */
#define TIMER15_BASE                   0x402F3000U /* TIMER15 base address */
#define TIMER16_BASE                   0x40300000U /* TIMER16 base address */
#define TIMER17_BASE                   0x40301000U /* TIMER17 base address */
#define TIMER18_BASE                   0x40302000U /* TIMER18 base address */
#define TIMER19_BASE                   0x40303000U /* TIMER19 base address */
#define INTMUX0_BASE                   0x40320000U /* INTMUX0 base address */
#define INTMUX1_BASE                   0x40321000U /* INTMUX1 base address */
#define INTMUX2_BASE                   0x40322000U /* INTMUX2 base address */
#define INTMUX3_BASE                   0x40323000U /* INTMUX3 base address */
#define INTMUX4_BASE                   0x40330000U /* INTMUX4 base address */
#define INTMUX5_BASE                   0x40331000U /* INTMUX5 base address */
#define INTMUX6_BASE                   0x40332000U /* INTMUX6 base address */
#define INTMUX7_BASE                   0x40333000U /* INTMUX7 base address */
#define INTMUX8_BASE                   0x40340000U /* INTMUX8 base address */
#define INTMUX9_BASE                   0x40341000U /* INTMUX9 base address */
#define INTMUX10_BASE                  0x40342000U /* INTMUX10 base address */
#define INTMUX11_BASE                  0x40343000U /* INTMUX11 base address */
#define I2C0_BASE                      0x40400000U /* I2C0 base address */
#define I2C1_BASE                      0x40410000U /* I2C1 base address */
#define I2C2_BASE                      0x40420000U /* I2C2 base address */
#define I2C3_BASE                      0x40430000U /* I2C3 base address */
#define I2C4_BASE                      0x40440000U /* I2C4 base address */
#define I2C5_BASE                      0x40450000U /* I2C5 base address */
#define UART0_BASE                     0x40460000U /* UART0 base address */
#define UART1_BASE                     0x40470000U /* UART1 base address */
#define UART2_BASE                     0x40480000U /* UART2 base address */
#define UART3_BASE                     0x40490000U /* UART3 base address */
#define PWM1_BASE                      0x404A0000U /* PWM1 base address */
#define SPI1_BASE                      0x404C0000U /* SPI1 base address */
#define GPIO1_BASE                     0x404E0000U /* GPIO1 base address */
#define GPIO2_BASE                     0x404F0000U /* GPIO2 base address */
#define GPIO3_BASE                     0x40500000U /* GPIO3 base address */
#define GPIO4_BASE                     0x40510000U /* GPIO4 base address */
#define SAI0_BASE                      0x40600000U /* SAI0 base address */
#define SAI1_BASE                      0x40610000U /* SAI1 base address */
#define SAI2_BASE                      0x40620000U /* SAI2 base address */
#define SAI3_BASE                      0x40630000U /* SAI3 base address */
#define ASRC0_BASE                     0x40680000U /* ASRC0 base address */
#define ASRC1_BASE                     0x40690000U /* ASRC1 base address */
#define ASRC2_BASE                     0x406A0000U /* ASRC2 base address */
#define ASRC3_BASE                     0x406B0000U /* ASRC3 base address */
#define PDM_BASE                       0x40700000U /* PDM base address */
#define SPDIFRX0_BASE                  0x40720000U /* SPDIFRX0 base address */
#define DMA1_BASE                      0x40780000U /* DMA1 base address */
#define DMA2_BASE                      0x40790000U /* DMA2 base address */
#define TSADC_BASE                     0x407C0000U /* TSADC base address */
#define GPIO4_AB_IOC_BASE              0x407D0000U /* GPIO4_AB_IOC base address */
#define RM3_IO_BASE                    0x407D0000U /* RM3_IO base address */
#define SAI4_BASE                      0x40800000U /* SAI4 base address */
#define SAI5_BASE                      0x40810000U /* SAI5 base address */
#define SAI6_BASE                      0x40820000U /* SAI6 base address */
#define SAI7_BASE                      0x40830000U /* SAI7 base address */
#define ASRC4_BASE                     0x40880000U /* ASRC4 base address */
#define ASRC5_BASE                     0x40890000U /* ASRC5 base address */
#define ASRC6_BASE                     0x408A0000U /* ASRC6 base address */
#define ASRC7_BASE                     0x408B0000U /* ASRC7 base address */
#define SPDIFTX0_BASE                  0x40900000U /* SPDIFTX0 base address */
#define SPDIFRX1_BASE                  0x40920000U /* SPDIFRX1 base address */
#define DMA3_BASE                      0x40980000U /* DMA3 base address */
#define DMA4_BASE                      0x40990000U /* DMA4 base address */
#define GPIO4_CD_IOC_BASE              0x409D0000U /* GPIO4_CD_IOC base address */
#define RM4_IO_BASE                    0x409D0000U /* RM4_IO base address */
#define MMC1_BASE                      0x40A00000U /* MMC1 base address */
#define FSPI0_BASE                     0x40A10000U /* FSPI0 base address */
#define GMAC_BASE                      0x40A40000U /* GMAC base address */
#define SPI2APB_BASE                   0x40A50000U /* SPI2APB base address */
#define OTPC_BASE                      0x40A60000U /* OTPC base address */
#define SARADC0_BASE                   0x40A70000U /* SARADC0 base address */
#define GPIO1_IOC_BASE                 0x40A80000U /* GPIO1_IOC base address */
#define GPIO2_IOC_BASE                 0x40A80000U /* GPIO2_IOC base address */
#define RM2_IO_BASE                    0x40A80000U /* RM2_IO base address */
#define CRU_BASE                       0x40B00000U /* CRU base address */
#define SCRU_BASE                      0x40B08000U /* SCRU base address */
#define PMU_CRU_BASE                   0x40B10000U /* PMU_CRU base address */
#define PMU_BASE                       0x40B20000U /* PMU base address */
#define GRF_PMU_BASE                   0x40B30000U /* GRF_PMU base address */
#define GPIO0_BASE                     0x40B50000U /* GPIO0 base address */
#define TIMER_HP0_BASE                 0x40B60000U /* TIMER_HP0 base address */
#define PWM0_BASE                      0x40B70000U /* PWM0 base address */
#define TOUCH_SENSOR_BASE              0x40B80000U /* TOUCH_SENSOR base address */
#define GPIO0_IOC_BASE                 0x40BC0000U /* GPIO0_IOC base address */
#define RM0_IO_BASE                    0x40BC0000U /* RM0_IO base address */
#define FIR_ACC_BASE                   0x40CC0000U /* FIR_ACC base address */
#define IIR_ACC_BASE                   0x40CC2000U /* IIR_ACC base address */
#endif

#undef INTMUX0
#undef INTMUX1
#undef INTMUX2
#undef INTMUX3
#undef INTMUX4
#undef INTMUX5
#undef INTMUX6
#undef INTMUX7
#undef INTMUX8
#undef INTMUX9
#undef INTMUX10
#undef INTMUX11
#undef IS_INTMUX_INSTANCE

#if defined(HAL_DSP_CORE)
#if defined(RK2118_DSP_CORE0)
#define INTMUX0             ((struct INTMUX_REG *) INTMUX0_BASE)
#define INTMUX1             ((struct INTMUX_REG *) INTMUX1_BASE)
#define INTMUX2             ((struct INTMUX_REG *) INTMUX2_BASE)
#define INTMUX3             ((struct INTMUX_REG *) INTMUX3_BASE)
#elif defined(RK2118_DSP_CORE1)
#define INTMUX0             ((struct INTMUX_REG *) INTMUX4_BASE)
#define INTMUX1             ((struct INTMUX_REG *) INTMUX5_BASE)
#define INTMUX2             ((struct INTMUX_REG *) INTMUX6_BASE)
#define INTMUX3             ((struct INTMUX_REG *) INTMUX7_BASE)
#elif defined(RK2118_DSP_CORE2)
#define INTMUX0             ((struct INTMUX_REG *) INTMUX8_BASE)
#define INTMUX1             ((struct INTMUX_REG *) INTMUX9_BASE)
#define INTMUX2             ((struct INTMUX_REG *) INTMUX10_BASE)
#define INTMUX3             ((struct INTMUX_REG *) INTMUX11_BASE)
#endif

#define IS_INTMUX_INSTANCE(instance) (((instance) == INTMUX0) || \
                                      ((instance) == INTMUX1) || \
                                      ((instance) == INTMUX2) || \
                                      ((instance) == INTMUX3))
#endif

extern uint32_t g_oscRate;
#define PLL_INPUT_OSC_RATE        g_oscRate  /* freqency of OSC */
#define TIMER_CHAN_CNT            8          /* Core0 and core1 use 4 timer respectively */
#define MBOX_CNT                  16         /* Total Mailbox in SoC */
#define PL330_NR_IRQS             9          /* Total 9 IRQS */

/****************************************************************************************/
/*                                                                                      */
/*                                Module Address Section                                */
/*                                                                                      */
/****************************************************************************************/
/* Memory Base */
#if defined(ARCH_ARM_CORTEX_SECURE)
#define PMU_SRAM_BASE                            0x40B40000U /* PMU_SRAM base address */
#define USB_OTG_BASE                             0x40040000U /* USB OTG base address */
#define USB_INNO_PHY_BASE                        0x403C0000U /* USB INNO base address */
#else
#define PMU_SRAM_BASE                            0x50B40000U /* PMU_SRAM base address */
#define USB_OTG_BASE                             0x50040000U /* USB OTG base address */
#define USB_INNO_PHY_BASE                        0x503C0000U /* USB INNO base address */
#endif
#define USB_PHY_CON_BASE                         (GRF->SOC_CON24) /* USB PHY control base address */
#define USB_PHY_BCD_DET_CON                      (GRF->SOC_CON27) /* USB PHY BCD control base address*/
#define USB_PHY_STATUS_BASE                      (GRF->SOC_STATUS6) /* USB PHY status base address */
#define USB_PHY_BCD_DET_BASE                     USB_PHY_STATUS_BASE /* USB PHY BCD status base address */
#define USB_PHY_BVALID_IRQ_CON_BASE              (GRF->USBOTG_SIG_DETECT_CON)
#define USB_PHY_BVALID_IRQ_CLR_BASE              (GRF->USBOTG_SIG_DETECT_CLR)
#define USB_PHY_BVALID_IRQ_STATUS_BASE           (GRF->USBOTG_SIG_DETECT_STATUS)
/****************************************************************************************/
/*                                                                                      */
/*                               Module Variable Section                                */
/*                                                                                      */
/****************************************************************************************/
/* Module Variable Define */
#define USB_OTG                     ((struct USB_GLOBAL_REG *) USB_OTG_BASE)
#define IS_PCD_INSTANCE(instance)   ((instance) == USB_OTG)
#define IS_HCD_INSTANCE(instance)   ((instance) == USB_OTG)
/****************************************************************************************/
/*                                                                                      */
/*                               Register Bitmap Section                                */
/*                                                                                      */
/****************************************************************************************/
/******************************************USB*******************************************/
#define USB_PHY_SUSPEND_MASK \
    (GRF_SOC_CON24_USBOTG_SW_EN_MASK | \
     GRF_SOC_CON24_USBOTG_UTMI_SUSPEND_N_MASK | \
     GRF_SOC_CON24_USBOTG_UTMI_OPMODE_MASK | \
     GRF_SOC_CON24_USBOTG_UTMI_XCVRSELECT_MASK | \
     GRF_SOC_CON24_USBOTG_UTMI_TERMSELECT_MASK | \
     GRF_SOC_CON24_USBOTG_UTMI_DPPULLDOWN_MASK | \
     GRF_SOC_CON24_USBOTG_UTMI_DMPULLDOWN_MASK)
#define USB_PHY_RESUME_MASK                      GRF_SOC_CON24_USBOTG_SW_EN_MASK
#define USB_PHY_CON_SHIFT                        GRF_SOC_CON24_USBOTG_SW_EN_SHIFT
#define USB_PHY_SUSPEND_VAL                      0x1D1U
#define USB_PHY_RESUME_VAL                       0
#define USB_OTG_UTMI_BVALID_MASK                 GRF_SOC_STATUS6_USBOTG_UTMI_BVALID_MASK
#define USB_OTG_UTMI_BVALID_SHIFT                GRF_SOC_STATUS6_USBOTG_UTMI_BVALID_SHIFT
#define USB_PHY_BVALID_RISE_IRQ_CON_MASK         GRF_USBOTG_SIG_DETECT_CON_OTG_BVALID_RISE_IRQ_EN_MASK
#define USB_PHY_BVALID_RISE_IRQ_CON_EN           USB_PHY_BVALID_RISE_IRQ_CON_MASK
#define USB_PHY_BVALID_RISE_IRQ_CLR_MASK         GRF_USBOTG_SIG_DETECT_CLR_OTG_BVALID_RISE_IRQ_CLR_MASK
#define USB_PHY_BVALID_RISE_IRQ_CLR_EN           USB_PHY_BVALID_RISE_IRQ_CLR_MASK
#define USB_PHY_BVALID_RISE_IRQ_STATUS_MASK      GRF_USBOTG_SIG_DETECT_STATUS_OTG_BVALID_RISE_IRQ_MASK
#define USB_PHY_BVALID_RISE_IRQ_STATUS_SHIFT     GRF_USBOTG_SIG_DETECT_STATUS_OTG_BVALID_RISE_IRQ_SHIFT
#define USB_PHY_BVALID_FALL_IRQ_CON_MASK         GRF_USBOTG_SIG_DETECT_CON_OTG_BVALID_FALL_IRQ_EN_MASK
#define USB_PHY_BVALID_FALL_IRQ_CON_EN           USB_PHY_BVALID_FALL_IRQ_CON_MASK
#define USB_PHY_BVALID_FALL_IRQ_CLR_MASK         GRF_USBOTG_SIG_DETECT_CLR_OTG_BVALID_FALL_IRQ_CLR_MASK
#define USB_PHY_BVALID_FALL_IRQ_CLR_EN           USB_PHY_BVALID_FALL_IRQ_CLR_MASK
#define USB_PHY_BVALID_FALL_IRQ_STATUS_MASK      GRF_USBOTG_SIG_DETECT_STATUS_OTG_BVALID_FALL_IRQ_MASK
#define USB_PHY_BVALID_FALL_IRQ_STATUS_SHIFT     GRF_USBOTG_SIG_DETECT_STATUS_OTG_BVALID_FALL_IRQ_SHIFT
#define USB_PHY_IDP_SINK_MASK                    GRF_SOC_CON27_USBPHY_IDP_SINK_EN_MASK
#define USB_PHY_IDP_SINK_EN                      USB_PHY_IDP_SINK_MASK
#define USB_PHY_IDM_SINK_MASK                    GRF_SOC_CON27_USBPHY_IDM_SINK_EN_MASK
#define USB_PHY_IDM_SINK_EN                      USB_PHY_IDM_SINK_MASK
#define USB_PHY_IDP_SRC_MASK                     GRF_SOC_CON27_USBPHY_IDP_SRC_EN_MASK
#define USB_PHY_IDP_SRC_EN                       USB_PHY_IDP_SRC_MASK
#define USB_PHY_RDM_PDWN_MASK                    GRF_SOC_CON27_USBPHY_RDM_PDWN_EN_MASK
#define USB_PHY_RDM_PDWN_EN                      USB_PHY_RDM_PDWN_MASK
#define USB_PHY_VDP_SRC_MASK                     GRF_SOC_CON27_USBPHY_VDP_SRC_EN_MASK
#define USB_PHY_VDP_SRC_EN                       USB_PHY_VDP_SRC_MASK
#define USB_PHY_VDM_SRC_MASK                     GRF_SOC_CON27_USBPHY_VDM_SRC_EN_MASK
#define USB_PHY_VDM_SRC_EN                       USB_PHY_VDM_SRC_MASK
#define USB_PHY_CHG_MODE_MASK                    (0x1FFU)
#define USB_PHY_CHG_MODE_VAL                     (0x057U)
#define USB_PHY_CP_DET_BIT                       (HAL_BIT(19))
#define USB_PHY_DCP_DET_BIT                      (HAL_BIT(18))
#define USB_PHY_DP_DET_BIT                       (HAL_BIT(20))
/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

/****************************************************************************************/
/*                                                                                      */
/*                                Module Address Section                                */
/*                                                                                      */
/****************************************************************************************/
/* Memory Base */
#if defined(ARCH_ARM_CORTEX_SECURE)
#define XIP_MAP0_BASE0      0x01000000U /* FSPI0 map address0 */
#else
#define XIP_MAP0_BASE0      0x11000000U /* FSPI0 map address0 */
#endif
/****************************************************************************************/
/*                                                                                      */
/*                               Register Bitmap Section                                */
/*                                                                                      */
/****************************************************************************************/
/******************************************FSPI******************************************/
#define FSPI_CHIP_CNT                            (2)
/******************************************CRU*******************************************/
#define CRU_CLK_USE_CON_BANK
#define CLK64(mux, div) ((((mux) & 0xffffffffULL) << 32) | ((div) & 0xffffffffULL))

#ifndef __ASSEMBLY__
typedef enum CLOCK_Name {
    CLK_INVALID = 0U,
    PLL_GPLL,
    PLL_VPLL0,
    PLL_VPLL1,
    CLK_WDT0,
    CLK_WDT1,
    CLK_WDT2,
    CLK_WDT3,
    CLK_WDT4,

    PLL_GPLL_DIV      = CLK64(0U, CLK_GPLL_DIV_DIV),
    PLL_VPLL0_DIV     = CLK64(0U, CLK_V0PLL_DIV_DIV),
    PLL_VPLL1_DIV     = CLK64(0U, CLK_V1PLL_DIV_DIV),
    PLL_GPLL_DIV_100M = CLK64(0U, CLK_GPLL_DIV_100M_DIV),

    /* dsp */
    CLK_DSP0_SRC = CLK64(CLK_DSP0_SRC_DIV_SEL, CLK_DSP0_SRC_DIV_DIV),
    CLK_DSP0     = CLK64(CLK_DSP0_SRC_PVTMUX_SEL, 0U),
    CLK_DSP1     = CLK64(CLK_DSP1_ROOT_SEL, CLK_DSP1_ROOT_DIV),
    CLK_DSP2     = CLK64(CLK_DSP2_ROOT_SEL, CLK_DSP2_ROOT_DIV),

    /* npu */
    ACLK_NPU = CLK64(ACLK_NPU_ROOT_SEL, ACLK_NPU_ROOT_DIV),
    HCLK_NPU = CLK64(HCLK_NPU_ROOT_SEL, HCLK_NPU_ROOT_DIV),

    /* star */
    CLK_STARSE0 = CLK64(CLK_STARSE0_ROOT_SEL, CLK_STARSE0_ROOT_DIV),
    CLK_STARSE1 = CLK64(CLK_STARSE1_ROOT_SEL, CLK_STARSE1_ROOT_DIV),

    /* top */
    ACLK_HSPERI      = CLK64(ACLK_HSPERI_ROOT_SEL, ACLK_HSPERI_ROOT_DIV),
    ACLK_PERIB       = CLK64(ACLK_PERIB_ROOT_SEL, ACLK_PERIB_ROOT_DIV),
    HCLK_PERIB       = CLK64(HCLK_PERIB_ROOT_SEL, HCLK_PERIB_ROOT_DIV),
    ACLK_BUS         = CLK64(ACLK_BUS_ROOT_SEL, ACLK_BUS_ROOT_DIV),
    HCLK_BUS         = CLK64(HCLK_BUS_ROOT_SEL, HCLK_BUS_ROOT_DIV),
    PCLK_BUS         = CLK64(PCLK_BUS_ROOT_SEL, PCLK_BUS_ROOT_DIV),
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

    CLK_UART0 = CLK64(SCLK_UART0_SEL, SCLK_UART0_DIV),
    CLK_UART1 = CLK64(SCLK_UART1_SEL, SCLK_UART1_DIV),
    CLK_UART2 = CLK64(SCLK_UART2_SEL, SCLK_UART2_DIV),
    CLK_UART3 = CLK64(SCLK_UART3_SEL, SCLK_UART3_DIV),

    CLK_SPI1 = CLK64(CLK_SPI1_SEL, CLK_SPI1_DIV),
    CLK_SPI2 = CLK64(CLK_SPI2_SEL, CLK_SPI2_DIV),

    CLK_RKTIMER0_TIME0 = CLK64(CLK_RKTIMER0_TIMER0_SEL, 0U),
    CLK_RKTIMER0_TIME1 = CLK64(CLK_RKTIMER0_TIMER1_SEL, 0U),
    CLK_RKTIMER0_TIME2 = CLK64(CLK_RKTIMER0_TIMER2_SEL, 0U),
    CLK_RKTIMER0_TIME3 = CLK64(CLK_RKTIMER0_TIMER3_SEL, 0U),
    CLK_RKTIMER1_TIME0 = CLK64(CLK_RKTIMER1_TIMER0_SEL, 0U),
    CLK_RKTIMER1_TIME1 = CLK64(CLK_RKTIMER1_TIMER1_SEL, 0U),
    CLK_RKTIMER1_TIME2 = CLK64(CLK_RKTIMER1_TIMER2_SEL, 0U),
    CLK_RKTIMER1_TIME3 = CLK64(CLK_RKTIMER1_TIMER3_SEL, 0U),
    CLK_RKTIMER2_TIME0 = CLK64(CLK_RKTIMER2_TIMER0_SEL, 0U),
    CLK_RKTIMER2_TIME1 = CLK64(CLK_RKTIMER2_TIMER1_SEL, 0U),
    CLK_RKTIMER2_TIME2 = CLK64(CLK_RKTIMER2_TIMER2_SEL, 0U),
    CLK_RKTIMER2_TIME3 = CLK64(CLK_RKTIMER2_TIMER3_SEL, 0U),

    CLK_I2C0 = CLK64(CLK_I2C0_SEL, CLK_I2C0_DIV),
    CLK_I2C1 = CLK64(CLK_I2C1_SEL, CLK_I2C1_DIV),
    CLK_I2C2 = CLK64(CLK_I2C2_SEL, CLK_I2C2_DIV),
    CLK_I2C3 = CLK64(CLK_I2C3_SEL, CLK_I2C3_DIV),
    CLK_I2C4 = CLK64(CLK_I2C4_SEL, CLK_I2C4_DIV),
    CLK_I2C5 = CLK64(CLK_I2C5_SEL, CLK_I2C5_DIV),

    CLK_PWM1             = CLK64(CLK_PWM1_SEL, CLK_PWM1_DIV),
    CLK_FREQ_PWM1_SAI    = CLK64(CLK_FREQ_PWM1_SAI_SEL, 0U),
    CLK_FREQ_PWM1        = CLK64(CLK_FREQ_PWM1_SEL, 0U),
    CLK_COUNTER_PWM1_SAI = CLK64(CLK_COUNTER_PWM1_SAI_SEL, 0U),
    CLK_COUNTER_PWM1     = CLK64(CLK_COUNTER_PWM1_SEL, 0U),

    CLK_CAN = CLK64(CLK_CAN_SEL, CLK_CAN_DIV),

    DBCLK_GPIO1 = CLK64(DBCLK_GPIO1_SEL, 0U),
    DBCLK_GPIO2 = CLK64(DBCLK_GPIO2_SEL, 0U),
    DBCLK_GPIO3 = CLK64(DBCLK_GPIO3_SEL, 0U),
    DBCLK_GPIO4 = CLK64(DBCLK_GPIO4_SEL, 0U),

    CLK_TSADC      = CLK64(0U, CLK_TSADC_DIV),
    CLK_TSADC_TSEN = CLK64(0U, CLK_TSADC_TSEN_DIV),
    CLK_SARADC     = CLK64(CLK_SARADC_SEL, CLK_SARADC_DIV),

    /* audio */
    ACLK_AUDIO0 = CLK64(ACLK_AUDIO0_ROOT_SEL, ACLK_AUDIO0_ROOT_DIV),
    HCLK_AUDIO0 = CLK64(HCLK_AUDIO0_ROOT_SEL, HCLK_AUDIO0_ROOT_DIV),

    SCLK_SAI0      = CLK64(SCLK_SAI0_SEL, 0U),
    MCLK_OUT_SAI0  = CLK64(MCLK_OUT_SAI0_SEL, MCLK_OUT_SAI0_DIV),
    SCLK_SAI1      = CLK64(SCLK_SAI1_SEL, 0U),
    MCLK_OUT_SAI1  = CLK64(MCLK_OUT_SAI1_SEL, MCLK_OUT_SAI1_DIV),
    SCLK_SAI2      = CLK64(SCLK_SAI2_SEL, 0U),
    MCLK_OUT_SAI2  = CLK64(MCLK_OUT_SAI2_SEL, MCLK_OUT_SAI2_DIV),
    SCLK_SAI3      = CLK64(SCLK_SAI3_SEL, 0U),
    MCLK_OUT_SAI3  = CLK64(MCLK_OUT_SAI3_SEL, MCLK_OUT_SAI3_DIV),
    MCLK_PDM       = CLK64(MCLK_PDM_SEL, MCLK_PDM_DIV),
    CLKOUT_PDM     = CLK64(CLKOUT_PDM_SEL, CLKOUT_PDM_DIV),
    CLK_ASRC0      = CLK64(CLK_ASRC0_SEL, CLK_ASRC0_DIV),
    MCLK_ASRC0     = CLK64(MCLK_ASRC0_SEL, 0U),
    LRCK_ASRC0_SRC = CLK64(LRCK_ASRC0_SRC_SEL, 0U),
    LRCK_ASRC0_DST = CLK64(LRCK_ASRC0_DST_SEL, 0U),
    CLK_ASRC1      = CLK64(CLK_ASRC1_SEL, CLK_ASRC1_DIV),
    MCLK_ASRC1     = CLK64(MCLK_ASRC1_SEL, 0U),
    LRCK_ASRC1_SRC = CLK64(LRCK_ASRC1_SRC_SEL, 0U),
    LRCK_ASRC1_DST = CLK64(LRCK_ASRC1_DST_SEL, 0U),
    CLK_ASRC2      = CLK64(CLK_ASRC2_SEL, CLK_ASRC2_DIV),
    MCLK_ASRC2     = CLK64(MCLK_ASRC2_SEL, 0U),
    LRCK_ASRC2_SRC = CLK64(LRCK_ASRC2_SRC_SEL, 0U),
    LRCK_ASRC2_DST = CLK64(LRCK_ASRC2_DST_SEL, 0U),
    CLK_ASRC3      = CLK64(CLK_ASRC3_SEL, CLK_ASRC3_DIV),
    MCLK_ASRC3     = CLK64(MCLK_ASRC3_SEL, 0U),
    LRCK_ASRC3_SRC = CLK64(LRCK_ASRC3_SRC_SEL, 0U),
    LRCK_ASRC3_DST = CLK64(LRCK_ASRC3_DST_SEL, 0U),

    MCLK_SPDIFRX0 = CLK64(MCLK_SPDIFRX0_SEL, MCLK_SPDIFRX0_DIV),
    MCLK_SPDIFRX1 = CLK64(MCLK_SPDIFRX1_SEL, MCLK_SPDIFRX1_DIV),
    MCLK_SPDIFTX  = CLK64(MCLK_SPDIFTX_SEL, MCLK_SPDIFTX_DIV),

    ACLK_AUDIO1 = CLK64(ACLK_AUDIO1_ROOT_SEL, ACLK_AUDIO1_ROOT_DIV),
    HCLK_AUDIO1 = CLK64(HCLK_AUDIO1_ROOT_SEL, HCLK_AUDIO1_ROOT_DIV),

    SCLK_SAI4      = CLK64(SCLK_SAI4_SEL, 0U),
    MCLK_OUT_SAI4  = CLK64(MCLK_OUT_SAI4_SEL, MCLK_OUT_SAI4_DIV),
    SCLK_SAI5      = CLK64(SCLK_SAI5_SEL, 0U),
    MCLK_OUT_SAI5  = CLK64(MCLK_OUT_SAI5_SEL, MCLK_OUT_SAI5_DIV),
    SCLK_SAI6      = CLK64(SCLK_SAI6_SEL, 0U),
    MCLK_OUT_SAI6  = CLK64(MCLK_OUT_SAI6_SEL, MCLK_OUT_SAI6_DIV),
    SCLK_SAI7      = CLK64(SCLK_SAI7_SEL, 0U),
    MCLK_OUT_SAI7  = CLK64(MCLK_OUT_SAI7_SEL, MCLK_OUT_SAI7_DIV),
    CLK_ASRC4      = CLK64(CLK_ASRC4_SEL, CLK_ASRC4_DIV),
    MCLK_ASRC4     = CLK64(MCLK_ASRC4_SEL, 0U),
    LRCK_ASRC4_SRC = CLK64(LRCK_ASRC4_SRC_SEL, 0U),
    LRCK_ASRC4_DST = CLK64(LRCK_ASRC4_DST_SEL, 0U),
    CLK_ASRC5      = CLK64(CLK_ASRC5_SEL, CLK_ASRC5_DIV),
    MCLK_ASRC5     = CLK64(MCLK_ASRC5_SEL, 0U),
    LRCK_ASRC5_SRC = CLK64(LRCK_ASRC5_SRC_SEL, 0U),
    LRCK_ASRC5_DST = CLK64(LRCK_ASRC5_DST_SEL, 0U),
    CLK_ASRC6      = CLK64(CLK_ASRC6_SEL, CLK_ASRC6_DIV),
    MCLK_ASRC6     = CLK64(MCLK_ASRC6_SEL, 0U),
    LRCK_ASRC6_SRC = CLK64(LRCK_ASRC6_SRC_SEL, 0U),
    LRCK_ASRC6_DST = CLK64(LRCK_ASRC6_DST_SEL, 0U),
    CLK_ASRC7      = CLK64(CLK_ASRC7_SEL, CLK_ASRC7_DIV),
    MCLK_ASRC7     = CLK64(MCLK_ASRC7_SEL, 0U),
    LRCK_ASRC7_SRC = CLK64(LRCK_ASRC7_SRC_SEL, 0U),
    LRCK_ASRC7_DST = CLK64(LRCK_ASRC7_DST_SEL, 0U),

    /* mmc */
    CLK_SDMMC     = CLK64(CLK_SDMMC_SEL, CLK_SDMMC_DIV),
    CCLK_SRC_EMMC = CLK64(CCLK_SRC_EMMC_SEL, CCLK_SRC_EMMC_DIV),
    SCLK_SFC      = CLK64(SCLK_SFC_SEL, SCLK_SFC_DIV),

    /* vop */
    DCLK_VOP = CLK64(DCLK_VOP_SEL, DCLK_VOP_DIV),

    /* flexbus */
    CLK_FLEXBUS_TX = CLK64(CLK_FLEXBUS_TX_SEL, CLK_FLEXBUS_TX_DIV),
    CLK_FLEXBUS_RX = CLK64(CLK_FLEXBUS_RX_SEL, CLK_FLEXBUS_RX_DIV),

    /* mac */
    CLK_MAC_ROOT     = CLK64(0U, CLK_MAC_ROOT_DIV),
    CLK_MAC_PTP_ROOT = CLK64(CLK_MAC_PTP_ROOT_SEL, CLK_MAC_PTP_ROOT_DIV),

    /* facc */
    ACLK_FACC  = CLK64(ACLK_FACC_ROOT_SEL, ACLK_FACC_ROOT_DIV),
    HCLK_FACC  = CLK64(HCLK_FACC_ROOT_SEL, HCLK_FACC_ROOT_DIV),
    CLK_RKFACC = CLK64(CLK_RKFACC_SEL, CLK_RKFACC_DIV),

    /* pmu */
    PCLK_PMU     = CLK64(PCLK_PMU_ROOT_SEL, PCLK_PMU_ROOT_DIV),
    CLK_PWM0     = CLK64(CLK_PWM0_SEL, CLK_PWM0_DIV),
    CLK_MAC_OUT  = CLK64(0U, CLK_MAC_OUT_DIV),
    CLK_HPTIMER  = CLK64(CLK_HPTIMER_SEL, 0U),
    DBCLK_GPIO0  = CLK64(DBCLK_GPIO0_SEL, 0U),
    CLK_REF_OUT0 = CLK64(CLK_REF_OUT0_SEL, CLK_REF_OUT0_DIV),
    CLK_REF_OUT1 = CLK64(CLK_REF_OUT1_SEL, CLK_REF_OUT1_DIV),
    CLK_32K_FRAC = CLK64(CLK_DEEPSLOW_FRAC_SEL, CLK_DEEPSLOW_FRAC_DIV),
    CLK_32K      = CLK64(CLK_DEEPSLOW_SEL, 0U),
    CLK_DDRPHY   = CLK64(CLK_DDRPHY_SEL, CLK_DDRPHY_DIV),

    /* scru */
    CLK_RKTIMER3_TIME1 = CLK64(CLK_RKTIMER3_TIMER1_SEL, 0U),
    CLK_RKTIMER3_TIME2 = CLK64(CLK_RKTIMER3_TIMER2_SEL, 0U),
    CLK_RKTIMER3_TIME3 = CLK64(CLK_RKTIMER3_TIMER3_SEL, 0U),
    CLK_RKTIMER4_TIME1 = CLK64(CLK_RKTIMER4_TIMER1_SEL, 0U),
    CLK_RKTIMER4_TIME2 = CLK64(CLK_RKTIMER4_TIMER2_SEL, 0U),
    CLK_RKTIMER4_TIME3 = CLK64(CLK_RKTIMER4_TIMER3_SEL, 0U),

    CLK_CORE_CRYPTO  = CLK64(CLK_CORE_CRYPTO_SEL, CLK_CORE_CRYPTO_DIV),
    CLK_PKA_CRYPTO   = CLK64(CLK_PKA_CRYPTO_SEL, CLK_PKA_CRYPTO_DIV),
    SSTCLKEN_STARSE0 = CLK64(0U, SSTCLKEN_STARSE0_DIV),
    SSTCLKEN_STARSE1 = CLK64(0U, SSTCLKEN_STARSE1_DIV),
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
    /* RM0_IO GPIO0A0~0B2 */
    RMIO_I2C0_SCL = 1,
    RMIO_I2C0_SDA = 2,
    RMIO_I2C1_SCL = 3,
    RMIO_I2C1_SDA = 4,
    RMIO_PWM0_CH0 = 5,
    RMIO_PWM0_CH1 = 6,
    RMIO_PWM0_CH2 = 7,
    RMIO_PWM0_CH3 = 8,
    RMIO_TOUCH_KEY_DRIVE_RM0 = 9,
    RMIO_TOUCH_KEY_IN0_RM0 = 10,
    RMIO_TOUCH_KEY_IN1_RM0 = 11,
    RMIO_TOUCH_KEY_IN2_RM0 = 12,
    RMIO_TOUCH_KEY_IN3_RM0 = 13,
    RMIO_SPI1_CLK = 14,
    RMIO_SPI1_MOSI = 15,
    RMIO_SPI1_MISO = 16,
    RMIO_SPI1_CSN0 = 17,
    RMIO_SPI1_CSN1 = 18,
    RMIO_ETH_PPSCLK = 19,
    RMIO_ETH_PPSTRIG = 20,
    /* RM1_IO GPIO3A0~3D3 */
    RMIO_UART1_TX_RM1 = 1,
    RMIO_UART1_RX_RM1 = 2,
    RMIO_UART1_CTSN_RM1 = 3,
    RMIO_UART1_RTSN_RM1 = 4,
    RMIO_UART2_TX_RM1 = 5,
    RMIO_UART2_RX_RM1 = 6,
    RMIO_UART2_CTSN_RM1 = 7,
    RMIO_UART2_RTSN_RM1 = 8,
    RMIO_UART3_TX = 9,
    RMIO_UART3_RX = 10,
    RMIO_UART3_CTSN = 11,
    RMIO_UART3_RTSN = 12,
    RMIO_I2C2_SCL_RM1 = 13,
    RMIO_I2C2_SDA_RM1 = 14,
    RMIO_I2C3_SCL_RM1 = 15,
    RMIO_I2C3_SDA_RM1 = 16,
    RMIO_I2C4_SCL_RM1 = 17,
    RMIO_I2C4_SDA_RM1 = 18,
    RMIO_I2C5_SCL_RM1 = 19,
    RMIO_I2C5_SDA_RM1 = 20,
    RMIO_CAN_TX = 21,
    RMIO_CAN_RX = 22,
    RMIO_PWM1_CH0 = 23,
    RMIO_PWM1_CH1 = 24,
    RMIO_PWM1_CH2 = 25,
    RMIO_PWM1_CH3 = 26,
    RMIO_TOUCH_KEY_DRIVE_RM1 = 27,
    RMIO_TOUCH_KEY_IN0_RM1 = 28,
    RMIO_TOUCH_KEY_IN1_RM1 = 29,
    RMIO_TOUCH_KEY_IN2_RM1 = 30,
    RMIO_TOUCH_KEY_IN3_RM1 = 31,
    RMIO_TOUCH_KEY_IN4_RM1 = 32,
    RMIO_TOUCH_KEY_IN5_RM1 = 33,
    RMIO_TOUCH_KEY_IN6_RM1 = 34,
    RMIO_TOUCH_KEY_IN7_RM1 = 35,
    RMIO_SPI2_CLK = 36,
    RMIO_SPI2_MOSI = 37,
    RMIO_SPI2_MISO = 38,
    RMIO_SPI2_CSN0 = 39,
    RMIO_SPI2_CSN1 = 40,
    /* RM2_IO GPIO2A0~2B0 */
    RMIO_UART1_TX_RM2 = 1,
    RMIO_UART1_RX_RM2 = 2,
    RMIO_UART1_CTSN_RM2 = 3,
    RMIO_UART1_RTSN_RM2 = 4,
    RMIO_UART2_TX_RM2 = 5,
    RMIO_UART2_RX_RM2 = 6,
    RMIO_UART2_CTSN_RM2 = 7,
    RMIO_UART2_RTSN_RM2 = 8,
    RMIO_I2C2_SCL_RM2 = 9,
    RMIO_I2C2_SDA_RM2 = 10,
    RMIO_I2C3_SCL_RM2 = 11,
    RMIO_I2C3_SDA_RM2 = 12,
    RMIO_I2C4_SCL_RM2 = 13,
    RMIO_I2C4_SDA_RM2 = 14,
    RMIO_I2C5_SCL_RM2 = 15,
    RMIO_I2C5_SDA_RM2 = 16,
    /* RM3_IO GPIO4A0~4B7 */
    RMIO_SPDIF_RX0 = 1,
    RMIO_PDM_CLK0 = 2,
    RMIO_PDM_CLK1 = 3,
    RMIO_PDM_SDI0 = 4,
    RMIO_PDM_SDI1 = 5,
    RMIO_PDM_SDI2 = 6,
    RMIO_PDM_SDI3 = 7,
    RMIO_SAI0_MCLK = 8,
    RMIO_SAI1_MCLK = 9,
    RMIO_SAI2_MCLK = 10,
    RMIO_SAI3_MCLK = 11,
    RMIO_SAI3_SCLK = 12,
    RMIO_SAI3_LRCK = 13,
    RMIO_SAI3_SDI0 = 14,
    RMIO_SAI3_SDI1 = 15,
    RMIO_SAI3_SDO0 = 16,
    RMIO_SAI3_SDO1 = 17,
    RMIO_SAI2_SCLK = 18,
    RMIO_SAI2_LRCK = 19,
    RMIO_SAI2_SDI0 = 20,
    RMIO_SAI2_SDI1 = 21,
    RMIO_SAI2_SDO0 = 22,
    RMIO_SAI2_SDO1 = 23,
    RMIO_SAI1_SCLK = 24,
    RMIO_SAI1_LRCK = 25,
    RMIO_SAI1_SDI0 = 26,
    RMIO_SAI1_SDI1 = 27,
    RMIO_SAI1_SDO0 = 28,
    RMIO_SAI1_SDO1 = 29,
    RMIO_SAI0_SCLK = 30,
    RMIO_SAI0_LRCK = 31,
    RMIO_SAI0_SDI0 = 32,
    RMIO_SAI0_SDI1 = 33,
    RMIO_SAI0_SDI2 = 34,
    RMIO_SAI0_SDI3 = 35,
    RMIO_SAI0_SDO0 = 36,
    RMIO_SAI0_SDO1 = 37,
    RMIO_SAI0_SDO2 = 38,
    RMIO_SAI0_SDO3 = 39,
    RMIO_SAI0_LRCKXN_0 = 40,
    RMIO_SAI0_LRCKXN_1 = 41,
    /* RM4_IO GPIO4C0~4D7 */
    RMIO_SPDIF_TX = 1,
    RMIO_SPDIF_RX1 = 2,
    RMIO_SAI4_MCLK = 3,
    RMIO_SAI5_MCLK = 4,
    RMIO_SAI6_MCLK = 5,
    RMIO_SAI7_MCLK = 6,
    RMIO_SAI7_SCLK = 7,
    RMIO_SAI7_LRCK = 8,
    RMIO_SAI7_SDI0 = 9,
    RMIO_SAI7_SDI1 = 10,
    RMIO_SAI7_SDO0 = 11,
    RMIO_SAI7_SDO1 = 12,
    RMIO_SAI6_SCLK = 13,
    RMIO_SAI6_LRCK = 14,
    RMIO_SAI6_SDI0 = 15,
    RMIO_SAI6_SDI1 = 16,
    RMIO_SAI6_SDO0 = 17,
    RMIO_SAI6_SDO1 = 18,
    RMIO_SAI5_SCLK = 19,
    RMIO_SAI5_LRCK = 20,
    RMIO_SAI5_SDI0 = 21,
    RMIO_SAI5_SDI1 = 22,
    RMIO_SAI5_SDO0 = 23,
    RMIO_SAI5_SDO1 = 24,
    RMIO_SAI4_SCLK = 25,
    RMIO_SAI4_LRCK = 26,
    RMIO_SAI4_SDI0 = 27,
    RMIO_SAI4_SDI1 = 28,
    RMIO_SAI4_SDI2 = 29,
    RMIO_SAI4_SDI3 = 30,
    RMIO_SAI4_SDO0 = 31,
    RMIO_SAI4_SDO1 = 32,
    RMIO_SAI4_SDO2 = 33,
    RMIO_SAI4_SDO3 = 34,
    RMIO_SAI4_LRCKXN_0 = 35,
    RMIO_SAI4_LRCKXN_1 = 36,
} eRMIO_Name;
/** @} */
/** @} */
/** @} */

typedef enum PD_Id {
    PD_INVALID = 0U,
} ePD_Id;
#endif /* __ASSEMBLY__ */
/****************************************SARADC******************************************/
#define SARADC SARADC0

/****************************************SPI2APB*****************************************/
#define SPISLV0_IRQn SPI2APB_IRQn

/****************************************WDT*********************************************/
#define WDT_CR_WDT_EN_MASK WDT_CR_EN_MASK

#if defined(RK2118_CPU_CORE0)
#define WDT WDT0
#define PCLK_WDT CLK_WDT0
#define GLB_RST_SND_WDT GLB_RST_SND_WDT0
#define GLB_RST_FST_WDT GLB_RST_FST_WDT0
#elif defined(RK2118_CPU_CORE1)
#define WDT WDT1
#undef WDT0_IRQn
#define WDT0_IRQn WDT1_IRQn
#define PCLK_WDT CLK_WDT1
#undef PCLK_WDT0_GATE
#define PCLK_WDT0_GATE PCLK_WDT1_GATE
#undef TCLK_WDT0_GATE
#define TCLK_WDT0_GATE TCLK_WDT1_GATE
#define GLB_RST_SND_WDT GLB_RST_SND_WDT1
#define GLB_RST_FST_WDT GLB_RST_FST_WDT1
#elif defined(RK2118_DSP_CORE0)
#define WDT WDT2
#undef WDT0_IRQn
#define WDT0_IRQn WDT2_IRQn
#define PCLK_WDT CLK_WDT2
#undef PCLK_WDT0_GATE
#define PCLK_WDT0_GATE PCLK_WDT2_GATE
#undef TCLK_WDT0_GATE
#define TCLK_WDT0_GATE TCLK_WDT2_GATE
#define GLB_RST_SND_WDT GLB_RST_SND_WDT2
#define GLB_RST_FST_WDT GLB_RST_FST_WDT2
#elif defined(RK2118_DSP_CORE1)
#define WDT WDT3
#undef WDT0_IRQn
#define WDT0_IRQn WDT3_IRQn
#define PCLK_WDT CLK_WDT3
#undef PCLK_WDT0_GATE
#define PCLK_WDT0_GATE PCLK_WDT3_GATE
#undef TCLK_WDT0_GATE
#define TCLK_WDT0_GATE TCLK_WDT3_GATE
#define GLB_RST_SND_WDT GLB_RST_SND_WDT3
#define GLB_RST_FST_WDT GLB_RST_FST_WDT3
#elif defined(RK2118_DSP_CORE2)
#define WDT WDT4
#undef WDT0_IRQn
#define WDT0_IRQn WDT4_IRQn
#define PCLK_WDT CLK_WDT4
#undef PCLK_WDT0_GATE
#define PCLK_WDT0_GATE PCLK_WDT4_GATE
#undef TCLK_WDT0_GATE
#define TCLK_WDT0_GATE TCLK_WDT4_GATE
#define GLB_RST_SND_WDT GLB_RST_SND_WDT4
#define GLB_RST_FST_WDT GLB_RST_FST_WDT4
#endif

#ifdef __cplusplus
}
#endif

#endif  /* SOC_H */
