/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"

#ifdef HAL_CANFD_MODULE_ENABLED
const struct HAL_CANFD_DEV g_can0Dev =
{
    .pReg = CAN0,
    .sclkID = CLK_CAN0,
    .sclkGateID = CLK_CAN0_GATE,
    .pclkGateID = HCLK_CAN0_GATE,
    .irqNum = CAN0_IRQn,
};

const struct HAL_CANFD_DEV g_can1Dev =
{
    .pReg = CAN1,
    .sclkID = CLK_CAN1,
    .sclkGateID = CLK_CAN1_GATE,
    .pclkGateID = HCLK_CAN1_GATE,
    .irqNum = CAN1_IRQn,
};
#endif

#ifdef HAL_CRU_MODULE_ENABLED
static struct CRU_BANK_INFO cruBanks[] = {
    CRU_BANK_CFG_FLAGS(CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(PHPTOPCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(SECURECRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(PMU1CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(DDR0CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(DDR1CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(BIGCORECRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(LITCORECRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(CCICRU_BASE, 0x300, 0x800, 0xa00),
};

const struct HAL_CRU_DEV g_cruDev = {
    .banks = cruBanks,
    .banksNum = HAL_ARRAY_SIZE(cruBanks),
};
#endif

#ifdef HAL_FSPI_MODULE_ENABLED
struct HAL_FSPI_HOST g_fspi0Dev =
{
    .instance = FSPI0,
    .sclkGate = SCLK_FSPI_X2_GATE,
    .hclkGate = HCLK_FSPI_GATE,
    .xipClkGate = 0,
    .sclkID = SCLK_FSPI_X2,
    .irqNum = FSPI0_IRQn,
    .xipMemCode = 0,
    .xipMemData = 0,
    .maxDllCells = 0xFF,
    .xmmcDev[0] =
    {
        .type = DEV_NOR,
    },
};

struct HAL_FSPI_HOST g_fspi1Dev =
{
    .instance = FSPI1,
    .sclkGate = SCLK_FSPI1_X2_GATE,
    .hclkGate = HCLK_FSPI1_GATE,
    .xipClkGate = 0,
    .sclkID = SCLK_FSPI1_X2,
    .irqNum = FSPI1_IRQn,
    .xipMemCode = 0,
    .xipMemData = 0,
    .maxDllCells = 0xFF,
    .xmmcDev[0] =
    {
        .type = DEV_NOR,
    },
};
#endif

#ifdef HAL_PCIE_MODULE_ENABLED
struct HAL_PCIE_DEV g_pcie0Dev =
{
    .apbBase = PCIE0_APB_BASE,
    .dbiBase = PCIE0_DBI_BASE,
    .cfgBase = 0x20000000,
    .lanes = 1,
    .gen = 2,
    .firstBusNo = 0x0,
#if defined(HAL_AP_CORE)
    .legacyIrqNum = PCIE0_LEGACY_IRQn,
#else
    .legacyIrqNum = 0,
#endif
};

struct HAL_PCIE_DEV g_pcie1Dev =
{
    .apbBase = PCIE1_APB_BASE,
    .dbiBase = PCIE1_DBI_BASE,
    .cfgBase = 0x21000000,
    .lanes = 1,
    .gen = 2,
    .firstBusNo = 0x20,
#if defined(HAL_AP_CORE)
    .legacyIrqNum = PCIE1_LEGACY_IRQn,
#else
    .legacyIrqNum = 0,
#endif
};
#endif

#ifdef HAL_PDM_MODULE_ENABLED
struct HAL_PDM_DEV g_pdm0Dev =
{
    .pReg = PDM0,
    .mclk = MCLK_PDM0,
    .clkOut = CLK_PDM0_OUT,
    .clkOutRate = 2400000,
    .gain = 0xaf,
};
#endif

#ifdef HAL_PWM_MODULE_ENABLED
const struct HAL_PWM_DEV g_pwm0Dev =
{
    .pReg = PWM0,
    .clkID = CLK_PMU1PWM,
    .clkGateID = CLK_PMU1PWM_GATE,
    .pclkGateID = PCLK_PMU1PWM_GATE,
#if defined(HAL_AP_CORE) || defined(HAL_BUS_MCU_CORE)
    .irqNum[0] = PWM0_CH0_IRQn,
    .irqNum[1] = PWM0_CH1_IRQn,
#else
    .irqNum[0] = PWM_CH0_IRQn,
    .irqNum[1] = PWM_CH1_IRQn,
#endif
};

#if defined(HAL_AP_CORE) || defined(HAL_BUS_MCU_CORE)
const struct HAL_PWM_DEV g_pwm1Dev =
{
    .pReg = PWM1,
    .clkID = CLK_PWM1,
    .clkGateID = CLK_PWM1_GATE,
    .pclkGateID = PCLK_PWM1_GATE,
    .irqNum[0] = PWM1_CH0_IRQn,
    .irqNum[1] = PWM1_CH1_IRQn,
    .irqNum[2] = PWM1_CH2_IRQn,
    .irqNum[3] = PWM1_CH3_IRQn,
    .irqNum[4] = PWM1_CH4_IRQn,
    .irqNum[5] = PWM1_CH5_IRQn,
};

const struct HAL_PWM_DEV g_pwm2Dev =
{
    .pReg = PWM2,
    .clkID = CLK_PWM2,
    .clkGateID = CLK_PWM2_GATE,
    .pclkGateID = PCLK_PWM2_GATE,
    .irqNum[0] = PWM2_CH0_IRQn,
    .irqNum[1] = PWM2_CH1_IRQn,
    .irqNum[2] = PWM2_CH2_IRQn,
    .irqNum[3] = PWM2_CH3_IRQn,
    .irqNum[4] = PWM2_CH4_IRQn,
    .irqNum[5] = PWM2_CH5_IRQn,
    .irqNum[6] = PWM2_CH6_IRQn,
    .irqNum[7] = PWM2_CH7_IRQn,
};
#endif
#endif

#ifdef HAL_SPI_MODULE_ENABLED
const struct HAL_SPI_DEV g_spi0Dev = {
    .base = SPI0_BASE,
    .clkId = CLK_SPI0,
    .clkGateID = CLK_SPI0_GATE,
    .pclkGateID = PCLK_SPI0_GATE,
    .maxFreq = 200000000,
    .irqNum = SPI0_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA_REQ_SPI0_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI0_BASE + 0x400,
        .dmac = DMA0,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI0_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI0_BASE + 0x800,
        .dmac = DMA0,
    },
};

const struct HAL_SPI_DEV g_spi1Dev = {
    .base = SPI1_BASE,
    .clkId = CLK_SPI1,
    .clkGateID = CLK_SPI1_GATE,
    .pclkGateID = PCLK_SPI1_GATE,
    .maxFreq = 200000000,
    .irqNum = SPI1_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA_REQ_SPI1_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI1_BASE + 0x400,
        .dmac = DMA0,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI1_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI1_BASE + 0x800,
        .dmac = DMA0,
    },
};

const struct HAL_SPI_DEV g_spi2Dev = {
    .base = SPI2_BASE,
    .clkId = CLK_SPI2,
    .clkGateID = CLK_SPI2_GATE,
    .pclkGateID = PCLK_SPI2_GATE,
    .maxFreq = 200000000,
    .irqNum = SPI2_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA_REQ_SPI2_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI2_BASE + 0x400,
        .dmac = DMA1,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI2_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI2_BASE + 0x800,
        .dmac = DMA1,
    },
};

const struct HAL_SPI_DEV g_spi3Dev = {
    .base = SPI3_BASE,
    .clkId = CLK_SPI3,
    .clkGateID = CLK_SPI3_GATE,
    .pclkGateID = PCLK_SPI3_GATE,
    .maxFreq = 200000000,
    .irqNum = SPI3_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA_REQ_SPI3_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI3_BASE + 0x400,
        .dmac = DMA1,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI3_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI3_BASE + 0x800,
        .dmac = DMA1,
    },
};

const struct HAL_SPI_DEV g_spi4Dev = {
    .base = SPI4_BASE,
    .clkId = CLK_SPI4,
    .clkGateID = CLK_SPI4_GATE,
    .pclkGateID = PCLK_SPI4_GATE,
    .maxFreq = 200000000,
    .irqNum = SPI4_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA_REQ_SPI4_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI4_BASE + 0x400,
        .dmac = DMA2,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI4_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI4_BASE + 0x800,
        .dmac = DMA2,
    },
};
#endif

#ifdef HAL_UART_MODULE_ENABLED
const struct HAL_UART_DEV g_uart0Dev =
{
    .pReg = UART0,
    .irqNum = UART0_IRQn,
    .sclkID = SCLK_UART0,
    .sclkGateID = SCLK_UART0_GATE,
    .pclkGateID = PCLK_UART0_GATE,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart1Dev =
{
    .pReg = UART1,
    .irqNum = UART1_IRQn,
    .sclkID = SCLK_UART1,
    .sclkGateID = SCLK_UART1_GATE,
    .pclkGateID = PCLK_UART1_GATE,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart2Dev =
{
    .pReg = UART2,
    .irqNum = UART2_IRQn,
    .sclkID = SCLK_UART2,
    .sclkGateID = SCLK_UART2_GATE,
    .pclkGateID = PCLK_UART2_GATE,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart3Dev =
{
    .pReg = UART3,
    .irqNum = UART3_IRQn,
    .sclkID = SCLK_UART3,
    .sclkGateID = SCLK_UART3_GATE,
    .pclkGateID = PCLK_UART3_GATE,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart4Dev =
{
    .pReg = UART4,
    .irqNum = UART4_IRQn,
    .sclkID = SCLK_UART4,
    .sclkGateID = SCLK_UART4_GATE,
    .pclkGateID = PCLK_UART4_GATE,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart5Dev =
{
    .pReg = UART5,
    .irqNum = UART5_IRQn,
    .sclkID = SCLK_UART5,
    .sclkGateID = SCLK_UART5_GATE,
    .pclkGateID = PCLK_UART5_GATE,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart6Dev =
{
    .pReg = UART6,
    .irqNum = UART6_IRQn,
    .sclkID = SCLK_UART6,
    .sclkGateID = SCLK_UART6_GATE,
    .pclkGateID = PCLK_UART6_GATE,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart7Dev =
{
    .pReg = UART7,
    .irqNum = UART7_IRQn,
    .sclkID = SCLK_UART7,
    .sclkGateID = SCLK_UART7_GATE,
    .pclkGateID = PCLK_UART7_GATE,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart8Dev =
{
    .pReg = UART8,
    .irqNum = UART8_IRQn,
    .sclkID = SCLK_UART8,
    .sclkGateID = SCLK_UART8_GATE,
    .pclkGateID = PCLK_UART8_GATE,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart9Dev =
{
    .pReg = UART9,
    .irqNum = UART9_IRQn,
    .sclkID = SCLK_UART9,
    .sclkGateID = SCLK_UART9_GATE,
    .pclkGateID = PCLK_UART9_GATE,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart10Dev =
{
    .pReg = UART10,
    .irqNum = UART10_IRQn,
    .sclkID = SCLK_UART10,
    .sclkGateID = SCLK_UART10_GATE,
    .pclkGateID = PCLK_UART10_GATE,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart11Dev =
{
    .pReg = UART11,
    .irqNum = UART11_IRQn,
    .sclkID = SCLK_UART11,
    .sclkGateID = SCLK_UART11_GATE,
    .pclkGateID = PCLK_UART11_GATE,
    .isAutoFlow = false,
};
#endif

#ifdef HAL_VDMA_MODULE_ENABLED
struct HAL_VDMA_DEV g_vdma0Dev =
{
    .pReg = VDMA,
    .vsAddr = (uint32_t)&(PDM0->INCR_RXDR),
};
#endif

void BSP_Init(void)
{
}

void BSP_SetLoaderFlag(void)
{
}
