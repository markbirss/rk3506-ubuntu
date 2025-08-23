/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"

#if defined(HAL_CRU_MODULE_ENABLED)
static struct CRU_BANK_INFO cruBanks[] = {
    CRU_BANK_CFG_FLAGS(CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(PHPTOPCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(SECURECRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(SBUSCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(PMU1SCRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(PMU1CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(DDR0CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(DDR1CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(DDR2CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(DDR3CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(BIGCORE0CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(BIGCORE1CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(DSUCRU_BASE, 0x300, 0x800, 0xa00),
};

const struct HAL_CRU_DEV g_cruDev = {
    .banks = cruBanks,
    .banksNum = HAL_ARRAY_SIZE(cruBanks),
};
#endif

#ifdef HAL_I2C_MODULE_ENABLED
const struct HAL_I2C_DEV g_i2c0Dev =
{
    .pReg = I2C0,
    .irqNum = I2C0_IRQn,
    .clkID = CLK_I2C0,
    .clkGateID = CLK_I2C0_GATE,
    .pclkGateID = PCLK_I2C0_GATE,
};

const struct HAL_I2C_DEV g_i2c1Dev =
{
    .pReg = I2C1,
    .irqNum = I2C1_IRQn,
    .clkID = CLK_I2C1,
    .clkGateID = CLK_I2C1_GATE,
    .pclkGateID = PCLK_I2C1_GATE,
};

const struct HAL_I2C_DEV g_i2c2Dev =
{
    .pReg = I2C2,
    .irqNum = I2C2_IRQn,
    .clkID = CLK_I2C2,
    .clkGateID = CLK_I2C2_GATE,
    .pclkGateID = PCLK_I2C2_GATE,
};

const struct HAL_I2C_DEV g_i2c3Dev =
{
    .pReg = I2C3,
    .irqNum = I2C3_IRQn,
    .clkID = CLK_I2C3,
    .clkGateID = CLK_I2C3_GATE,
    .pclkGateID = PCLK_I2C3_GATE,
};

const struct HAL_I2C_DEV g_i2c4Dev =
{
    .pReg = I2C4,
    .irqNum = I2C4_IRQn,
    .clkID = CLK_I2C4,
    .clkGateID = CLK_I2C4_GATE,
    .pclkGateID = PCLK_I2C4_GATE,
};

const struct HAL_I2C_DEV g_i2c5Dev =
{
    .pReg = I2C5,
    .irqNum = I2C5_IRQn,
    .clkID = CLK_I2C5,
    .clkGateID = CLK_I2C5_GATE,
    .pclkGateID = PCLK_I2C5_GATE,
};

const struct HAL_I2C_DEV g_i2c6Dev =
{
    .pReg = I2C6,
    .irqNum = I2C6_IRQn,
    .clkID = CLK_I2C6,
    .clkGateID = CLK_I2C6_GATE,
    .pclkGateID = PCLK_I2C6_GATE,
};

const struct HAL_I2C_DEV g_i2c7Dev =
{
    .pReg = I2C7,
    .irqNum = I2C7_IRQn,
    .clkID = CLK_I2C7,
    .clkGateID = CLK_I2C7_GATE,
    .pclkGateID = PCLK_I2C7_GATE,
};

const struct HAL_I2C_DEV g_i2c8Dev =
{
    .pReg = I2C8,
    .irqNum = I2C8_IRQn,
    .clkID = CLK_I2C8,
    .clkGateID = CLK_I2C8_GATE,
    .pclkGateID = PCLK_I2C8_GATE,
};
#endif /* HAL_I2C_MODULE_ENABLED */

#ifdef HAL_I2STDM_MODULE_ENABLED
struct HAL_I2STDM_DEV g_i2sTdm0Dev =
{
    .pReg = I2STDM0,
    .mclkTx = CLK_I2S0_8CH_TX,
    .mclkTxGate = CLK_I2S0_8CH_TX_GATE,
    .mclkRx = CLK_I2S0_8CH_RX,
    .mclkRxGate = CLK_I2S0_8CH_RX_GATE,
    .muxTxSel = MCLK_I2S0_8CH_TX_SEL,
    .muxRxSel = MCLK_I2S0_8CH_RX_SEL,
    .hclk = HCLK_I2S0_8CH_GATE,
    .rsts[0] = SRST_MRESETN_I2S0_8CH_TX,
    .rsts[1] = SRST_MRESETN_I2S0_8CH_RX,
    .bclkFs = 64,
    .pd = PD_AUDIO,
    .txDmaData =
    {
        .addr = (uint32_t)&(I2STDM0->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S0_8CH_TX,
        .dmac = DMA0,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(I2STDM0->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S0_8CH_RX,
        .dmac = DMA0,
    },
};

struct HAL_I2STDM_DEV g_i2sTdm1Dev =
{
    .pReg = I2STDM1,
    .mclkTx = CLK_I2S1_8CH_TX,
    .mclkTxGate = CLK_I2S1_8CH_TX_GATE,
    .mclkRx = CLK_I2S1_8CH_RX,
    .mclkRxGate = CLK_I2S1_8CH_RX_GATE,
    .muxTxSel = MCLK_I2S1_8CH_TX_SEL,
    .muxRxSel = MCLK_I2S1_8CH_RX_SEL,
    .hclk = HCLK_I2S1_8CH_GATE,
    .rsts[0] = SRST_MRESETN_I2S1_8CH_TX,
    .rsts[1] = SRST_MRESETN_I2S1_8CH_RX,
    .bclkFs = 64,
    .txDmaData =
    {
        .addr = (uint32_t)&(I2STDM1->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S1_8CH_TX,
        .dmac = DMA0,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(I2STDM1->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S1_8CH_RX,
        .dmac = DMA0,
    },
};
#endif

#ifdef HAL_I2S_MODULE_ENABLED
struct HAL_I2S_DEV g_i2s2Dev =
{
    .pReg = I2S2,
    .mclk = CLK_I2S2_2CH,
    .mclkGate = CLK_I2S2_2CH_GATE,
    .hclk = HCLK_I2S2_2CH_GATE,
    .bclkFs = 64,
    .pd = PD_AUDIO,
    .txDmaData =
    {
        .addr = (uint32_t)&(I2S2->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S2_2CH_TX,
        .dmac = DMA1,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(I2S2->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S2_2CH_RX,
        .dmac = DMA1,
    },
};

struct HAL_I2S_DEV g_i2s3Dev =
{
    .pReg = I2S3,
    .mclk = CLK_I2S3_2CH,
    .mclkGate = CLK_I2S3_2CH_GATE,
    .hclk = HCLK_I2S3_2CH_GATE,
    .bclkFs = 64,
    .pd = PD_AUDIO,
    .txDmaData =
    {
        .addr = (uint32_t)&(I2S3->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S3_2CH_TX,
        .dmac = DMA1,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(I2S3->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S3_2CH_RX,
        .dmac = DMA1,
    },
};
#endif

#ifdef HAL_PCIE_MODULE_ENABLED
const struct HAL_PHY_SNPS_PCIE3_DEV g_phy_pcie3Dev =
{
    .phyMode = PHY_MODE_PCIE_AGGREGATION,
};

struct HAL_PCIE_DEV g_pcieDev =
{
    .apbBase = PCIE3_4L_APB_BASE,
    .dbiBase = PCIE3_4L_DBI_BASE,
    .cfgBase = 0xF0000000,
    .lanes = 4,
    .gen = 3,
    .firstBusNo = 0,
    .legacyIrqNum = PCIE30x4_LEGACY_IRQn,
    .phy = (void *)&g_phy_pcie3Dev,
};
#endif

#ifdef HAL_PDM_MODULE_ENABLED
struct HAL_PDM_DEV g_pdm0Dev =
{
    .pReg = PDM0,
    .mclk = MCLK_PDM0,
    .mclkGate = MCLK_PDM0_GATE,
    .mclkRate = PDM_CLK_RATE,
    .hclk = HCLK_PDM0_GATE,
    .reset = SRST_RESETN_PDM0,
    .rxDmaData =
    {
        .addr = (uint32_t)&(PDM0->RXFIFO_DATA_REG),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_PDM0,
        .dmac = DMA0,
    },
};
#endif

#ifdef HAL_PL330_MODULE_ENABLED
struct HAL_PL330_DEV g_pl330Dev0 =
{
    .pReg = DMA0,
    .peripReqType = BURST,
    .irq[0] = DMAC0_IRQn,
    .irq[1] = DMAC0_ABORT_IRQn,
    .clkGate = ACLK_DMAC0_GATE,
    .pd = 0,
};

struct HAL_PL330_DEV g_pl330Dev1 =
{
    .pReg = DMA1,
    .peripReqType = BURST,
    .irq[0] = DMAC1_IRQn,
    .irq[1] = DMAC1_ABORT_IRQn,
    .clkGate = ACLK_DMAC1_GATE,
    .pd = 0,
};
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
    },
    .rxDma = {
        .channel = DMA_REQ_SPI0_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI0_BASE + 0x800,
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
    },
    .rxDma = {
        .channel = DMA_REQ_SPI1_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI1_BASE + 0x800,
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
    },
    .rxDma = {
        .channel = DMA_REQ_SPI2_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI2_BASE + 0x800,
    },
};

const struct HAL_SPI_DEV g_spi3Dev = {
    .base = SPI3_BASE,
    .clkId = CLK_SPI3,
    .clkGateID = CLK_SPI3_GATE,
    .pclkGateID = PCLK_SPI3_GATE,
    .irqNum = SPI3_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA_REQ_SPI3_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI3_BASE + 0x400,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI3_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI3_BASE + 0x800,
    },
};

const struct HAL_SPI_DEV g_spi4Dev = {
    .base = SPI4_BASE,
    .clkId = CLK_SPI4,
    .clkGateID = CLK_SPI4_GATE,
    .pclkGateID = PCLK_SPI4_GATE,
    .irqNum = SPI4_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA_REQ_SPI4_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI4_BASE + 0x400,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI4_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI4_BASE + 0x800,
    },
};
#endif /* HAL_SPI_MODULE_ENABLED */

#ifdef HAL_UART_MODULE_ENABLED
const struct HAL_UART_DEV g_uart0Dev =
{
    .pReg = UART0,
    .sclkID = CLK_UART0,
    .sclkGateID = CLK_UART0_GATE,
    .pclkGateID = PCLK_UART0_GATE,
    .irqNum = UART0_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART0,
};

const struct HAL_UART_DEV g_uart1Dev =
{
    .pReg = UART1,
    .sclkID = CLK_UART1,
    .sclkGateID = CLK_UART1_GATE,
    .pclkGateID = PCLK_UART1_GATE,
    .irqNum = UART1_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART1,
};

const struct HAL_UART_DEV g_uart2Dev =
{
    .pReg = UART2,
    .sclkID = CLK_UART2,
    .sclkGateID = CLK_UART2_GATE,
    .pclkGateID = PCLK_UART2_GATE,
    .irqNum = UART2_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART2,
};

const struct HAL_UART_DEV g_uart3Dev =
{
    .pReg = UART3,
    .sclkID = CLK_UART3,
    .sclkGateID = CLK_UART3_GATE,
    .pclkGateID = PCLK_UART3_GATE,
    .irqNum = UART3_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART3,
};

const struct HAL_UART_DEV g_uart4Dev =
{
    .pReg = UART4,
    .sclkID = CLK_UART4,
    .sclkGateID = CLK_UART4_GATE,
    .pclkGateID = PCLK_UART4_GATE,
    .irqNum = UART4_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART4,
};

const struct HAL_UART_DEV g_uart5Dev =
{
    .pReg = UART5,
    .sclkID = CLK_UART5,
    .sclkGateID = CLK_UART5_GATE,
    .pclkGateID = PCLK_UART5_GATE,
    .irqNum = UART5_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART5,
};

const struct HAL_UART_DEV g_uart6Dev =
{
    .pReg = UART6,
    .sclkID = CLK_UART6,
    .sclkGateID = CLK_UART6_GATE,
    .pclkGateID = PCLK_UART6_GATE,
    .irqNum = UART6_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART6,
};

const struct HAL_UART_DEV g_uart7Dev =
{
    .pReg = UART7,
    .sclkID = CLK_UART7,
    .sclkGateID = CLK_UART7_GATE,
    .pclkGateID = PCLK_UART7_GATE,
    .irqNum = UART7_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART7,
};

const struct HAL_UART_DEV g_uart8Dev =
{
    .pReg = UART8,
    .sclkID = CLK_UART8,
    .sclkGateID = CLK_UART8_GATE,
    .pclkGateID = PCLK_UART8_GATE,
    .irqNum = UART8_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART8,
};

const struct HAL_UART_DEV g_uart9Dev =
{
    .pReg = UART9,
    .sclkID = CLK_UART9,
    .sclkGateID = CLK_UART9_GATE,
    .pclkGateID = PCLK_UART9_GATE,
    .irqNum = UART9_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART9,
};
#endif

#ifdef HAL_VAD_MODULE_ENABLED
/* VAD_CONTROL[3:1]: voice source mapping */
const struct AUDIO_SRC_ADDR_MAP g_audioSrcAddrMaps[] =
{
    { 0, PDM0_BASE + 0x400 },
    { /* sentinel */ }
};

struct HAL_VAD_DEV g_vadDev =
{
    .pReg = VAD,
    .hclk = HCLK_VAD_GATE,
    .pd = PD_AUDIO,
    .irq = VAD_IRQn,
};
#endif

#ifdef HAL_PWM_MODULE_ENABLED
const struct HAL_PWM_DEV g_pwm0Dev =
{
    .pReg = PWM0,
    .clkID = CLK_PMU1PWM,
    .clkGateID = CLK_PMU1PWM_GATE,
    .pclkGateID = PCLK_PMU1PWM_GATE,
    .irqNum[0] = PWM0_IRQn,
};

const struct HAL_PWM_DEV g_pwm1Dev =
{
    .pReg = PWM1,
    .clkID = CLK_PWM1,
    .clkGateID = CLK_PWM1_GATE,
    .pclkGateID = PCLK_PWM1_GATE,
    .irqNum[0] = PWM1_IRQn,
};

const struct HAL_PWM_DEV g_pwm2Dev =
{
    .pReg = PWM2,
    .clkID = CLK_PWM2,
    .clkGateID = CLK_PWM2_GATE,
    .pclkGateID = PCLK_PWM2_GATE,
    .irqNum[0] = PWM2_IRQn,
};

const struct HAL_PWM_DEV g_pwm3Dev =
{
    .pReg = PWM3,
    .clkID = CLK_PWM3,
    .clkGateID = CLK_PWM3_GATE,
    .pclkGateID = PCLK_PWM3_GATE,
    .irqNum[0] = PWM3_IRQn,
};
#endif

void BSP_Init(void)
{
}

void BSP_SetLoaderFlag(void)
{
}
