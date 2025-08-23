/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"

#ifdef HAL_I2STDM_MODULE_ENABLED
struct HAL_I2STDM_DEV g_i2sTdm0Dev =
{
    .pReg = I2STDM0,
    .mclkTx = CLK_I2S0_8CH_TX,
    .mclkTxGate = MCLK_I2S0_8CH_TX_GATE,
    .mclkRx = CLK_I2S0_8CH_RX,
    .mclkRxGate = MCLK_I2S0_8CH_RX_GATE,
    .hclk = HCLK_I2S0_8CH_GATE,
    .rsts[0] = SRST_M_I2S0_8CH_TX,
    .rsts[1] = SRST_M_I2S0_8CH_RX,
    .bclkFs = 64,
    .txDmaData =
    {
        .addr = (uint32_t)&(I2STDM0->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S0_8CH_TX,
        .dmac = DMA1,
    },
};

struct HAL_I2STDM_DEV g_i2sTdm1Dev =
{
    .pReg = I2STDM1,
    .mclkTx = CLK_I2S1_8CH_TX,
    .mclkTxGate = MCLK_I2S1_8CH_TX_GATE,
    .mclkRx = CLK_I2S1_8CH_RX,
    .mclkRxGate = MCLK_I2S1_8CH_RX_GATE,
    .hclk = HCLK_I2S1_8CH_GATE,
    .rsts[0] = SRST_M_I2S1_8CH_TX,
    .rsts[1] = SRST_M_I2S1_8CH_RX,
    .bclkFs = 64,
    .rxDmaData =
    {
        .addr = (uint32_t)&(I2STDM1->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S1_8CH_RX,
        .dmac = DMA1,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(I2STDM1->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S1_8CH_TX,
        .dmac = DMA1,
    },
};

struct HAL_I2STDM_DEV g_i2sTdm2Dev =
{
    .pReg = I2STDM2,
    .mclkTx = CLK_I2S2_2CH,
    .mclkTxGate = MCLK_I2S2_2CH_GATE,
    .mclkRx = CLK_I2S2_2CH,
    .mclkRxGate = MCLK_I2S2_2CH_GATE,
    .hclk = HCLK_I2S2_2CH_GATE,
    .rsts[0] = SRST_M_I2S2_2CH,
    .rsts[1] = SRST_M_I2S2_2CH,
    .bclkFs = 64,
    .rxDmaData =
    {
        .addr = (uint32_t)&(I2STDM2->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S2_2CH_RX,
        .dmac = DMA1,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(I2STDM2->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S2_2CH_TX,
        .dmac = DMA1,
    },
};

struct HAL_I2STDM_DEV g_i2sTdm3Dev =
{
    .pReg = I2STDM3,
    .mclkTx = CLK_I2S3_2CH_TX,
    .mclkTxGate = MCLK_I2S3_2CH_TX_GATE,
    .mclkRx = CLK_I2S3_2CH_RX,
    .mclkRxGate = MCLK_I2S3_2CH_RX_GATE,
    .hclk = HCLK_I2S3_2CH_GATE,
    .rsts[0] = SRST_M_I2S3_2CH_TX,
    .rsts[1] = SRST_M_I2S3_2CH_RX,
    .bclkFs = 64,
    .rxDmaData =
    {
        .addr = (uint32_t)&(I2STDM3->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S3_2CH_RX,
        .dmac = DMA1,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(I2STDM3->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA_REQ_I2S3_2CH_TX,
        .dmac = DMA1,
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
    .pd = 0,
};

struct HAL_PL330_DEV g_pl330Dev1 =
{
    .pReg = DMA1,
    .peripReqType = BURST,
    .irq[0] = DMAC1_IRQn,
    .irq[1] = DMAC1_ABORT_IRQn,
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
    .irqNum = SPI2_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA_REQ_SPI2_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI2_BASE + 0x400,
        .dmac = DMA0,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI2_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI2_BASE + 0x800,
        .dmac = DMA0,
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
        .dmac = DMA0,
    },
    .rxDma = {
        .channel = DMA_REQ_SPI3_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI3_BASE + 0x800,
        .dmac = DMA0,
    },
};
#endif

#ifdef HAL_UART_MODULE_ENABLED
const struct HAL_UART_DEV g_uart0Dev =
{
    .pReg = UART0,
    .sclkID = CLK_UART0,
    .irqNum = UART0_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart1Dev =
{
    .pReg = UART1,
    .sclkID = CLK_UART1,
    .sclkGateID = SCLK_UART1_GATE,
    .pclkGateID = PCLK_UART1_GATE,
    .irqNum = UART1_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart2Dev =
{
    .pReg = UART2,
    .sclkID = CLK_UART2,
    .sclkGateID = SCLK_UART2_GATE,
    .pclkGateID = PCLK_UART2_GATE,
    .irqNum = UART2_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart3Dev =
{
    .pReg = UART3,
    .sclkID = CLK_UART3,
    .sclkGateID = SCLK_UART3_GATE,
    .pclkGateID = PCLK_UART3_GATE,
    .irqNum = UART3_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart4Dev =
{
    .pReg = UART4,
    .sclkID = CLK_UART4,
    .sclkGateID = SCLK_UART4_GATE,
    .pclkGateID = PCLK_UART4_GATE,
    .irqNum = UART4_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart5Dev =
{
    .pReg = UART5,
    .sclkID = CLK_UART5,
    .sclkGateID = SCLK_UART5_GATE,
    .pclkGateID = PCLK_UART5_GATE,
    .irqNum = UART5_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart6Dev =
{
    .pReg = UART6,
    .sclkID = CLK_UART6,
    .sclkGateID = SCLK_UART6_GATE,
    .pclkGateID = PCLK_UART6_GATE,
    .irqNum = UART6_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart7Dev =
{
    .pReg = UART7,
    .sclkID = CLK_UART7,
    .sclkGateID = SCLK_UART7_GATE,
    .pclkGateID = PCLK_UART7_GATE,
    .irqNum = UART7_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart8Dev =
{
    .pReg = UART8,
    .sclkID = CLK_UART8,
    .sclkGateID = SCLK_UART8_GATE,
    .pclkGateID = PCLK_UART8_GATE,
    .irqNum = UART8_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart9Dev =
{
    .pReg = UART9,
    .sclkID = CLK_UART9,
    .sclkGateID = SCLK_UART9_GATE,
    .pclkGateID = PCLK_UART9_GATE,
    .irqNum = UART9_IRQn,
    .isAutoFlow = false,
};
#endif

#ifdef HAL_I2C_MODULE_ENABLED
const struct HAL_I2C_DEV g_i2c0Dev =
{
    .pReg = I2C0,
    .irqNum = I2C0_IRQn,
    .clkID = CLK_I2C,
    .clkGateID = CLK_I2C0_GATE,
    .pclkGateID = PCLK_I2C0_GATE,
    .runtimeID = PM_RUNTIME_ID_I2C0,
};

const struct HAL_I2C_DEV g_i2c1Dev =
{
    .pReg = I2C1,
    .irqNum = I2C1_IRQn,
    .clkID = CLK_I2C,
    .clkGateID = CLK_I2C1_GATE,
    .pclkGateID = PCLK_I2C1_GATE,
    .runtimeID = PM_RUNTIME_ID_I2C1,
};

const struct HAL_I2C_DEV g_i2c2Dev =
{
    .pReg = I2C2,
    .irqNum = I2C2_IRQn,
    .clkID = CLK_I2C,
    .clkGateID = CLK_I2C2_GATE,
    .pclkGateID = PCLK_I2C2_GATE,
    .runtimeID = PM_RUNTIME_ID_I2C2,
};

const struct HAL_I2C_DEV g_i2c3Dev =
{
    .pReg = I2C3,
    .irqNum = I2C3_IRQn,
    .clkID = CLK_I2C,
    .clkGateID = CLK_I2C3_GATE,
    .pclkGateID = PCLK_I2C3_GATE,
    .runtimeID = PM_RUNTIME_ID_I2C3,
};

const struct HAL_I2C_DEV g_i2c4Dev =
{
    .pReg = I2C4,
    .irqNum = I2C4_IRQn,
    .clkID = CLK_I2C,
    .clkGateID = CLK_I2C4_GATE,
    .pclkGateID = PCLK_I2C4_GATE,
    .runtimeID = PM_RUNTIME_ID_I2C4,
};

const struct HAL_I2C_DEV g_i2c5Dev =
{
    .pReg = I2C5,
    .irqNum = I2C5_IRQn,
    .clkID = CLK_I2C,
    .clkGateID = CLK_I2C5_GATE,
    .pclkGateID = PCLK_I2C5_GATE,
    .runtimeID = PM_RUNTIME_ID_I2C5,
};
#endif

#ifdef HAL_FSPI_MODULE_ENABLED
struct HAL_FSPI_HOST g_fspi0Dev =
{
    .instance = FSPI,
    .sclkGate = SCLK_SFC_GATE,
    .hclkGate = HCLK_SFC_GATE,
    .xipClkGate = 0,
    .sclkID = 0,
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

#ifdef HAL_CANFD_MODULE_ENABLED
const struct HAL_CANFD_DEV g_can0Dev =
{
    .pReg = CAN0,
    .sclkID = CLK_CAN0,
    .sclkGateID = CLK_CAN0_GATE,
    .pclkGateID = PCLK_CAN0_GATE,
    .irqNum = CAN0_IRQn,
};

const struct HAL_CANFD_DEV g_can1Dev =
{
    .pReg = CAN1,
    .sclkID = CLK_CAN1,
    .sclkGateID = CLK_CAN1_GATE,
    .pclkGateID = PCLK_CAN1_GATE,
    .irqNum = CAN1_IRQn,
};

const struct HAL_CANFD_DEV g_can2Dev =
{
    .pReg = CAN2,
    .sclkID = CLK_CAN2,
    .sclkGateID = CLK_CAN2_GATE,
    .pclkGateID = PCLK_CAN2_GATE,
    .irqNum = CAN2_IRQn,
};
#endif

#ifdef HAL_GMAC_MODULE_ENABLED
const struct HAL_GMAC_DEV g_gmac0Dev =
{
    .pReg = GMAC0,
    .clkID125M = CLK_MAC0_2TOP,
    .clkID50M = CLK_MAC0_2TOP,
    .clkGateID125M = CLK_MAC0_2TOP_GATE,
    .clkGateID50M = CLK_MAC0_2TOP_GATE,
    .pclkID = PCLK_PHP,
    .pclkGateID = PCLK_GMAC0_GATE,
#ifdef HAL_GMAC_PTP_FEATURE_ENABLED
    .ptpClkID = CLK_GMAC0_PTP_REF,
    .ptpClkGateID = CLK_GMAC0_PTP_REF_GATE,
#endif
    .irqNum = GMAC0_IRQn,
};

const struct HAL_GMAC_DEV g_gmac1Dev =
{
    .pReg = GMAC1,
    .clkID125M = CLK_MAC1_2TOP,
    .clkID50M = CLK_MAC1_2TOP,
    .clkGateID125M = CLK_MAC1_2TOP_GATE,
    .clkGateID50M = CLK_MAC1_2TOP_GATE,
    .pclkID = PCLK_USB,
    .pclkGateID = PCLK_GMAC1_GATE,
#ifdef HAL_GMAC_PTP_FEATURE_ENABLED
    .ptpClkID = CLK_GMAC1_PTP_REF,
    .ptpClkGateID = CLK_GMAC1_PTP_REF_GATE,
#endif
    .irqNum = GMAC1_IRQn,
};
#endif

#ifdef HAL_PCIE_MODULE_ENABLED
struct HAL_PCIE_DEV g_pcieDev =
{
    .apbBase = PCIE3X2_APB_BASE,
    .dbiBase = PCIE3X2_DBI_BASE,
    .cfgBase = 0xF0000000,
    .lanes = 2,
    .gen = 3,
    .firstBusNo = 0x20,
    .legacyIrqNum = PCIE30x2_LEGACY_IRQn,
};
#endif

#ifdef HAL_PWM_MODULE_ENABLED
const struct HAL_PWM_DEV g_pwm0Dev =
{
    .pReg = PWM0,
    .clkID = 0,
    .clkGateID = CLK_PWM0_GATE,
    .pclkGateID = PCLK_PWM0_GATE,
    .irqNum[0] = PWM_PMU_IRQn,
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

#if defined(HAL_EHCI_MODULE_ENABLED) || defined(HAL_OHCI_MODULE_ENABLED)
const struct HAL_USBH_DEV g_usbhDev =
{
    .ehciReg = USB2HOST0_EHCI,
    .ohciReg = USB2HOST0_OHCI,
    .ehciIrqNum = USB2HOST0_EHCI_IRQn,
    .ohciIrqNum = USB2HOST0_OHCI_IRQn,
    .usbhGateID = HCLK_USB2HOST0_GATE,
    .usbhArbGateID = HCLK_USB2HOST0_ARB_GATE,
    .utmiclkGateID = XIN_OSC0_USBPHY0_GATE,
};
#endif

void BSP_Init(void)
{
}
