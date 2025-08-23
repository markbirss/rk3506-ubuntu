/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"

#ifdef HAL_PL330_MODULE_ENABLED
struct HAL_PL330_DEV g_pl330Dev =
{
    .pReg = DMA,
    .peripReqType = BURST,
    .irq[0] = DMAC_IRQn,
    .irq[1] = DMAC_ABORT_IRQn,
    .pd = 0,
};
#endif /* HAL_PL330_MODULE_ENABLED */

#ifdef HAL_CRU_MODULE_ENABLED
static struct CRU_BANK_INFO cruBanks[] = {
    CRU_BANK_CFG_FLAGS(TOPCRU_BASE, 0x100, 0x300, 0x400),
    CRU_BANK_CFG_FLAGS(PMU0CRU_BASE, 0x100, 0x180, 0x200),
    CRU_BANK_CFG_FLAGS(PMU1CRU_BASE, 0x100, 0x180, 0x200),
    CRU_BANK_CFG_FLAGS(DDRCRU_BASE, 0x100, 0x180, 0x200),
    CRU_BANK_CFG_FLAGS(SUBDDRCRU_BASE, 0x100, 0x180, 0x200),
    CRU_BANK_CFG_FLAGS(PERICRU_BASE, 0x100, 0x300, 0x400),
};

const struct HAL_CRU_DEV g_cruDev = {
    .banks = cruBanks,
    .banksNum = HAL_ARRAY_SIZE(cruBanks),
};
#endif /* HAL_CRU_MODULE_ENABLED */

#ifdef HAL_I2C_MODULE_ENABLED
const struct HAL_I2C_DEV g_i2c0Dev =
{
    .pReg = I2C0,
    .irqNum = I2C0_IRQn,
    .clkID = CLK_PMU0_I2C0,
    .clkGateID = PCLK_PMU0_I2C0_GATE,
    .pclkGateID = CLK_PMU0_I2C0_GATE,
};

#if defined(HAL_BUS_MCU_CORE) || defined(HAL_AP_CORE)
const struct HAL_I2C_DEV g_i2c1Dev =
{
    .pReg = I2C1,
    .irqNum = I2C1_IRQn,
    .clkID = CLK_I2C,
    .clkGateID = CLK_I2C1_GATE,
    .pclkGateID = PCLK_I2C1_GATE,
};

const struct HAL_I2C_DEV g_i2c2Dev =
{
    .pReg = I2C2,
    .irqNum = I2C2_IRQn,
    .clkID = CLK_I2C,
    .clkGateID = CLK_I2C2_GATE,
    .pclkGateID = PCLK_I2C2_GATE,
};

const struct HAL_I2C_DEV g_i2c3Dev =
{
    .pReg = I2C3,
    .irqNum = I2C3_IRQn,
    .clkID = CLK_I2C,
    .clkGateID = CLK_I2C3_GATE,
    .pclkGateID = PCLK_I2C3_GATE,
};

const struct HAL_I2C_DEV g_i2c4Dev =
{
    .pReg = I2C4,
    .irqNum = I2C4_IRQn,
    .clkID = CLK_I2C,
    .clkGateID = CLK_I2C4_GATE,
    .pclkGateID = PCLK_I2C4_GATE,
};

const struct HAL_I2C_DEV g_i2c5Dev =
{
    .pReg = I2C5,
    .irqNum = I2C5_IRQn,
    .clkID = CLK_I2C,
    .clkGateID = CLK_I2C5_GATE,
    .pclkGateID = PCLK_I2C5_GATE,
};
#endif /* HAL_BUS_MCU_CORE || HAL_AP_CORE */
#endif /* HAL_I2C_MODULE_ENABLED */

#ifdef HAL_SAI_MODULE_ENABLED
struct HAL_SAI_DEV g_sai0Dev =
{
    .pReg = SAI0,
    .mclk = CLK_SAI0,
    .mclkGate = MCLK_SAI0_GATE,
    .bclkFs = 64,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI0->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_SAI0_RX,
        .dmac = DMA,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI0->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_SAI0_TX,
        .dmac = DMA,
    },
};

struct HAL_SAI_DEV g_sai1Dev =
{
    .pReg = SAI1,
    .mclk = CLK_SAI1,
    .mclkGate = MCLK_SAI1_GATE,
    .bclkFs = 64,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI1->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_SAI1_RX,
        .dmac = DMA,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI1->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_SAI1_TX,
        .dmac = DMA,
    },
};

struct HAL_SAI_DEV g_sai2Dev =
{
    .pReg = SAI2,
    .mclk = CLK_SAI2,
    .mclkGate = MCLK_SAI2_GATE,
    .bclkFs = 64,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI2->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_SAI2_RX,
        .dmac = DMA,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI2->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_SAI2_TX,
        .dmac = DMA,
    },
};
#endif

#ifdef HAL_SPI_MODULE_ENABLED
const struct HAL_SPI_DEV g_spi0Dev = {
    .base = SPI0_BASE,
    .clkId = CLK_PMU1_SPI0,
    .clkGateID = CLK_PMU1_SPI0_GATE,
    .pclkGateID = PCLK_PMU1_SPI0_GATE,
    .maxFreq = 200000000,
    .irqNum = SPI0_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA_REQ_SPI0_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI0_BASE + 0x400,
        .dmac = DMA,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI0_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI0_BASE + 0x800,
        .dmac = DMA,
    },
};

#if defined(HAL_BUS_MCU_CORE) || defined(HAL_AP_CORE)
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
        .dmac = DMA,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI1_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI1_BASE + 0x800,
        .dmac = DMA,
    },
};

const struct HAL_SPI_DEV g_spi2Dev = {
    .base = SPI2_BASE,
    .clkId = CLK_SPI2,
    .clkGateID = CLK_SPI2_GATE,
    .pclkGateID = PCLK_SPI2_GATE,
    .irqNum = SPI2_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA_REQ_SPI2_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI2_BASE + 0x400,
        .dmac = DMA,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI2_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI2_BASE + 0x800,
        .dmac = DMA,
    },
};
#endif /* HAL_BUS_MCU_CORE || HAL_AP_CORE */
#endif /* HAL_SPI_MODULE_ENABLED */

#if defined(HAL_BUS_MCU_CORE) || defined(HAL_AP_CORE)

#ifdef HAL_FSPI_MODULE_ENABLED
struct HAL_FSPI_HOST g_fspi0Dev =
{
    .instance = FSPI,
    .sclkGate = SCLK_SFC_GATE,
    .hclkGate = HCLK_SFC_GATE,
    .xipClkGate = 0,
    .sclkID = SCLK_SFC,
    .irqNum = FSPI0_IRQn,
    .xipMemCode = 0,
    .xipMemData = 0,
    .maxDllCells = 0x1FF,
    .xmmcDev[0] =
    {
        .type = 0,
    },
};
#endif

#ifdef HAL_UART_MODULE_ENABLED
const struct HAL_UART_DEV g_uart0Dev =
{
    .pReg = UART0,
    .sclkID = CLK_PMU1_UART0,
    .sclkGateID = CLK_PMU1_UART0_GATE,
    .pclkGateID = PCLK_PMU1_UART0_GATE,
    .irqNum = UART0_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart1Dev =
{
    .pReg = UART1,
    .sclkID = CLK_UART1,
    .sclkGateID = CLK_UART1_GATE,
    .pclkGateID = PCLK_UART1_GATE,
    .irqNum = UART1_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart2Dev =
{
    .pReg = UART2,
    .sclkID = CLK_UART2,
    .sclkGateID = CLK_UART2_GATE,
    .pclkGateID = PCLK_UART2_GATE,
    .irqNum = UART2_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart3Dev =
{
    .pReg = UART3,
    .sclkID = CLK_UART3,
    .sclkGateID = CLK_UART3_GATE,
    .pclkGateID = PCLK_UART3_GATE,
    .irqNum = UART3_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart4Dev =
{
    .pReg = UART4,
    .sclkID = CLK_UART4,
    .sclkGateID = CLK_UART4_GATE,
    .pclkGateID = PCLK_UART4_GATE,
    .irqNum = UART4_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart5Dev =
{
    .pReg = UART5,
    .sclkID = CLK_UART5,
    .sclkGateID = CLK_UART5_GATE,
    .pclkGateID = PCLK_UART5_GATE,
    .irqNum = UART5_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart6Dev =
{
    .pReg = UART6,
    .sclkID = CLK_UART6,
    .sclkGateID = CLK_UART6_GATE,
    .pclkGateID = PCLK_UART6_GATE,
    .irqNum = UART6_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart7Dev =
{
    .pReg = UART7,
    .sclkID = CLK_UART7,
    .sclkGateID = CLK_UART7_GATE,
    .pclkGateID = PCLK_UART7_GATE,
    .irqNum = UART7_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart8Dev =
{
    .pReg = UART8,
    .sclkID = CLK_UART8,
    .sclkGateID = CLK_UART8_GATE,
    .pclkGateID = PCLK_UART8_GATE,
    .irqNum = UART8_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart9Dev =
{
    .pReg = UART9,
    .sclkID = CLK_UART9,
    .sclkGateID = CLK_UART9_GATE,
    .pclkGateID = PCLK_UART9_GATE,
    .irqNum = UART9_IRQn,
    .isAutoFlow = false,
};
#endif

#endif /* HAL_BUS_MCU_CORE || HAL_AP_CORE */

#ifdef HAL_PMU_MCU_CORE

#ifdef HAL_UART_MODULE_ENABLED
const struct HAL_UART_DEV g_uart0Dev =
{
    .pReg = UART0,
    .sclkID = CLK_PMU1_UART0,
    .sclkGateID = CLK_PMU1_UART0_GATE,
    .pclkGateID = PCLK_PMU1_UART0_GATE,
    .irqNum = UART0_IRQn,
    .isAutoFlow = false,
};
#endif

#endif /* HAL_PMU_MCU_CORE */

#ifdef HAL_PWM_MODULE_ENABLED
const struct HAL_PWM_DEV g_pwm0Dev =
{
    .pReg = PWM0,
    .clkID = CLK_PMU1_PWM0,
    .clkGateID = CLK_PMU1_PWM0_GATE,
    .pclkGateID = PCLK_PMU1_PWM0_GATE,
    .irqNum[0] = PWM0_IRQn,
};

const struct HAL_PWM_DEV g_pwm1Dev =
{
    .pReg = PWM1,
    .clkID = CLK_PWM1_PERI,
    .clkGateID = CLK_PWM1_PERI_GATE,
    .pclkGateID = PCLK_PWM1_PERI_GATE,
    .irqNum[0] = PWM1_IRQn,
};

const struct HAL_PWM_DEV g_pwm2Dev =
{
    .pReg = PWM2,
    .clkID = CLK_PWM2_PERI,
    .clkGateID = CLK_PWM2_PERI_GATE,
    .pclkGateID = PCLK_PWM2_PERI_GATE,
    .irqNum[0] = PWM2_IRQn,
};

const struct HAL_PWM_DEV g_pwm3Dev =
{
    .pReg = PWM3,
    .clkID = CLK_PWM3_PERI,
    .clkGateID = CLK_PWM3_PERI_GATE,
    .pclkGateID = PCLK_PWM3_PERI_GATE,
    .irqNum[0] = PWM3_IRQn,
};
#endif

#ifdef HAL_GMAC_MODULE_ENABLED
const struct HAL_GMAC_DEV g_gmac0Dev =
{
    .pReg = GMAC0,
    .clkID125M = CLK_GMAC_125M_CRU_I,
    .clkID50M = CLK_GMAC_50M_CRU_I,
    .clkGateID125M = CLK_GMAC_125M_CRU_GATE,
    .clkGateID50M = CLK_GMAC_50M_CRU_GATE,
    .pclkID = PCLK_BUS,
    .pclkGateID = PCLK_GMAC_GATE,
    .irqNum = GMAC0_IRQn,
};
#endif

void BSP_Init(void)
{
}

void BSP_SetLoaderFlag(void)
{
}
