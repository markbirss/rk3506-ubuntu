/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"

#ifdef HAL_ACDCDIG_DSM_MODULE_ENABLED
struct HAL_ACDCDIG_DSM_DEV g_acdcDigDSMDev = {
    .pReg = ACDCDIG_DSM,
    .mclk = MCLK_SAI3,
    .reset = SRST_MRESETN_DSM,
};
#endif

#ifdef HAL_ACODEC_MODULE_ENABLED
struct HAL_ACODEC_DEV g_acodecDev =
{
    .pReg = ACODEC,
    .mclk = MCLK_SAI4,
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
#endif

#ifdef HAL_CRU_MODULE_ENABLED
static struct CRU_BANK_INFO cruBanks[] = {
    CRU_BANK_CFG_FLAGS(CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(SCRU_BASE, 0x10, 0x40, 0x80),
    CRU_BANK_CFG_FLAGS(CRU_PMU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(SCRU_PMU_BASE, 0x10, 0x40, 0x80),
};

const struct HAL_CRU_DEV g_cruDev = {
    .banks = cruBanks,
    .banksNum = HAL_ARRAY_SIZE(cruBanks),
};
#endif  /* End of HAL_CRU_MODULE_ENABLED */

#ifdef HAL_DSMC_HOST_MODULE_ENABLED
struct HAL_DSMC_HOST g_dsmcHostDev =
{
    .pReg = DSMC,
    .clkGate = CLK_DSMC_GATE,
    .aclkGate = ACLK_DSMC_GATE,
    .pClkGate = PCLK_DSMC_GATE,
    .sclkID = CLK_DSMC,
    .sclkRootID = CLK_ROOT_DSMC,
    .maxFreq = 100000000,
    .arst = SRST_ARESETN_DSMC,
    .prst = SRST_PRESETN_DSMC,
    .irqNum = DSMC_IRQn,
    .regionMem = {
        .regionBase = 0xc0000000,
        .regionSize = 0x2000000,
    },
    .dmaReq[0] = {
        .brstSize = DMA_SLAVE_BUSWIDTH_8_BYTES,
        .brstLen = 16,
        .dmaReqCh = DMA0_REQ_DSMC_TX_2,
        .dmac = DMA0,
    },
    .dmaReq[1] = {
        .brstSize = DMA_SLAVE_BUSWIDTH_8_BYTES,
        .brstLen = 16,
        .dmaReqCh = DMA0_REQ_DSMC_RX_3,
        .dmac = DMA0,
    },
    .dsmcHostDev = {
        .type = DEV_DSMC_LB,
        .clkMode = 0,
        .ioWidth = MCR_IOWIDTH_X8,
        .wrapSize = DSMC_BURST_WRAPSIZE_32CLK,
        .dmaReqMuxOffset = DSMC_DMA_MUX,
        .ChipSelCfg[0] = {
            .deviceType = DEV_DSMC_LB,
            .intEn = 2,
            .rcshi = 1,
            .wcshi = 0,
            .rcss = 0,
            .wcss = 0,
            .rcsh = 0,
            .wcsh = 0,
            .rdLatency = 2,
            .wrLatency = 2,
            .dqsDLL = {
                .rxDLLDQS[0] = 0x20,
                .rxDLLDQS[1] = 0x20,
            },
            .slvRgn[0] = {
                .attribute = RGNX_ATTR_MERGE_FIFO,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = true,
            },
            .slvRgn[1] = {
                .attribute = RGNX_ATTR_NO_MERGE_FIFO,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
            .slvRgn[2] = {
                .attribute = RGNX_ATTR_DPRA,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
            .slvRgn[3] = {
                .attribute = RGNX_ATTR_REG,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = true,
            },
        },
        .ChipSelCfg[1] = {
            .deviceType = DEV_UNKNOWN,
            .intEn = 1,
            .rcshi = 1,
            .wcshi = 0,
            .rcss = 0,
            .wcss = 0,
            .rcsh = 0,
            .wcsh = 0,
            .rdLatency = 2,
            .wrLatency = 2,
            .dqsDLL = {
                .rxDLLDQS[0] = 0x20,
                .rxDLLDQS[1] = 0x20,
            },
            .slvRgn[0] = {
                .attribute = RGNX_ATTR_MERGE_FIFO,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
            .slvRgn[1] = {
                .attribute = RGNX_ATTR_NO_MERGE_FIFO,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
            .slvRgn[2] = {
                .attribute = RGNX_ATTR_DPRA,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
            .slvRgn[3] = {
                .attribute = RGNX_ATTR_REG,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
        },
        .ChipSelCfg[2] = {
            .deviceType = DEV_UNKNOWN,
            .intEn = 0,
            .rcshi = 1,
            .wcshi = 0,
            .rcss = 0,
            .wcss = 0,
            .rcsh = 0,
            .wcsh = 0,
            .rdLatency = 2,
            .wrLatency = 2,
            .dqsDLL = {
                .rxDLLDQS[0] = 0x20,
                .rxDLLDQS[1] = 0x20,
            },
            .slvRgn[0] = {
                .attribute = RGNX_ATTR_MERGE_FIFO,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
            .slvRgn[1] = {
                .attribute = RGNX_ATTR_NO_MERGE_FIFO,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
            .slvRgn[2] = {
                .attribute = RGNX_ATTR_DPRA,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
            .slvRgn[3] = {
                .attribute = RGNX_ATTR_REG,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
        },
        .ChipSelCfg[3] = {
            .deviceType = DEV_UNKNOWN,
            .intEn = 3,
            .rcshi = 1,
            .wcshi = 0,
            .rcss = 0,
            .wcss = 0,
            .rcsh = 0,
            .wcsh = 0,
            .rdLatency = 2,
            .wrLatency = 2,
            .dqsDLL = {
                .rxDLLDQS[0] = 0x20,
                .rxDLLDQS[1] = 0x20,
            },
            .slvRgn[0] = {
                .attribute = RGNX_ATTR_MERGE_FIFO,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
            .slvRgn[1] = {
                .attribute = RGNX_ATTR_NO_MERGE_FIFO,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
            .slvRgn[2] = {
                .attribute = RGNX_ATTR_DPRA,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
            .slvRgn[3] = {
                .attribute = RGNX_ATTR_REG,
                .caAddrWidth = 0,
                .dummyClkNum = 1,
                .cs0BeCtrled = 0,
                .cs0Ctrl = 0,
                .status = false,
            },
        },
    },
};
#endif  /* End of HAL_DSMC_HOST_MODULE_ENABLED */

#ifdef HAL_DSMC_SLAVE_MODULE_ENABLED
struct HAL_DSMC_SLAVE g_dsmcSlaveDev =
{
    .pReg = LBC_SLAVE_CSR,
    .aclkGate = ACLK_DSMC_SLV_GATE,
    .hClkGate = HCLK_DSMC_SLV_GATE,
    .rst = SRST_RESETN_DSMC_SLV,
    .arst = SRST_ARESETN_DSMC_SLV,
    .hrst = SRST_HRESETN_DSMC_SLV,
    .irqNum = DSMC_SLAVE_IRQn,
    .lbcMem = {
        .lbcMemBase = 0x6000000,
        .lbcMemSize = 0x2000000,
    },
};
#endif

#ifdef HAL_FLEXBUS_MODULE_ENABLED
const struct HAL_FLEXBUS_DEV g_flexbusDev =
{
    .pReg = FLEXBUS,
    .txclkID = CLK_FLEXBUS_TX,
    .rxclkID = CLK_FLEXBUS_RX,
    .txclkGateID = CLK_FLEXBUS_TX_GATE,
    .rxclkGateID = CLK_FLEXBUS_RX_GATE,
    .aclkGateID = ACLK_FLEXBUS_GATE,
    .hclkGateID = HCLK_FLEXBUS_GATE,
    .irqNum = FLEXBUS_IRQn,
#if defined(HAL_FLEXBUS_DAC_MODULE_ENABLED)
    .opMode0 = FLEXBUS0_OPMODE_DAC,
#elif defined(HAL_FLEXBUS_SPI_MODULE_ENABLED)
    .opMode0 = FLEXBUS0_OPMODE_SPI,
#endif
#if defined(HAL_FLEXBUS_ADC_MODULE_ENABLED)
    .opMode1 = FLEXBUS1_OPMODE_ADC,
#endif
};

#ifdef HAL_FLEXBUS_ADC_MODULE_ENABLED
const struct HAL_FLEXBUS_ADC_DEV g_flexbusADCDev =
{
    .refclkID = CLK_REF_OUT1,
    .refclkGateID = CLK_REF_OUT1_GATE,
};
#endif  /* End of HAL_FLEXBUS_ADC_MODULE_ENABLED */
#endif  /* End of HAL_FLEXBUS_MODULE_ENABLED */

#ifdef HAL_FSPI_MODULE_ENABLED
struct HAL_FSPI_HOST g_fspi0Dev =
{
    .instance = FSPI,
    .sclkGate = SCLK_FSPI_GATE,
    .hclkGate = HCLK_FSPI_GATE,
    .xipClkGate = 0,
    .sclkID = SCLK_FSPI,
    .irqNum = FSPI_IRQn,
    .xipMemCode = 0,
    .xipMemData = 0,
    .maxDllCells = 0x17F,
    .xmmcDev[0] =
    {
        .type = 0,
    },
};
#endif  /* End of HAL_FSPI_MODULE_ENABLED */

#ifdef HAL_GMAC_MODULE_ENABLED
const struct HAL_GMAC_DEV g_gmac0Dev =
{
    .pReg = GMAC0,
    .clkID125M = CLK_MAC,
    .clkID50M = CLK_MAC,
    .clkGateID125M = CLK_MAC0_GATE,
    .clkGateID50M = CLK_MAC0_GATE,
    .pclkID = HCLK_LSPERI_ROOT,
    .pclkGateID = PCLK_MAC0_GATE,
#ifdef HAL_GMAC_PTP_FEATURE_ENABLED
    .ptpClkID = CLK_MAC_PTP,
    .ptpClkGateID = CLK_MAC_PTP_GATE,
#endif
    .irqNum = MAC0_SBD_IRQn,
};

const struct HAL_GMAC_DEV g_gmac1Dev =
{
    .pReg = GMAC1,
    .clkID125M = CLK_MAC,
    .clkID50M = CLK_MAC,
    .clkGateID125M = CLK_MAC1_GATE,
    .clkGateID50M = CLK_MAC1_GATE,
    .pclkID = HCLK_LSPERI_ROOT,
    .pclkGateID = PCLK_MAC1_GATE,
#ifdef HAL_GMAC_PTP_FEATURE_ENABLED
    .ptpClkID = CLK_MAC_PTP,
    .ptpClkGateID = CLK_MAC_PTP_ROOT_GATE,
#endif
    .irqNum = MAC1_SBD_IRQn,
};
#endif

#ifdef HAL_HCD_MODULE_ENABLED
const struct HAL_USB_DEV g_usbotgh0Dev =
{
    .pReg = USB_OTG0,
    .hclkGateID = HCLK_USBOTG0_GATE,
    .utmiclkGateID = CLK_REF_SRC_USBPHY_GATE,
    .irqNum = OTG0_IRQn,
    .cfg =
    {
        .phyif = USB_PHY_UTMI_WIDTH_16,
        .speed = USB_OTG_SPEED_HIGH,
        .hcNum = 8,
        .dmaEnable = true,
        .sofEnable = false,
        .lpmEnable = false,
        .suspendEnable = false,
    },
};

const struct HAL_USB_DEV g_usbotgh1Dev =
{
    .pReg = USB_OTG1,
    .hclkGateID = HCLK_USBOTG1_GATE,
    .utmiclkGateID = CLK_REF_SRC_USBPHY_GATE,
    .irqNum = OTG1_IRQn,
    .cfg =
    {
        .phyif = USB_PHY_UTMI_WIDTH_16,
        .speed = USB_OTG_SPEED_HIGH,
        .hcNum = 8,
        .dmaEnable = true,
        .sofEnable = false,
        .lpmEnable = false,
        .suspendEnable = false,
    },
};
#endif

#ifdef HAL_I2C_MODULE_ENABLED
const struct HAL_I2C_DEV g_i2c0Dev =
{
    .pReg = I2C0,
    .irqNum = I2C0_IRQn,
    .clkID = CLK_I2C0,
    .clkGateID = PCLK_I2C0_GATE,
    .pclkGateID = CLK_I2C0_GATE,
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
#endif  /* End of HAL_I2C_MODULE_ENABLED */

#ifdef HAL_PCD_MODULE_ENABLED
const struct HAL_USB_DEV g_usbdDev =
{
    .pReg = USB_OTG0,
    .hclkGateID = HCLK_USBOTG0_GATE,
    .utmiclkGateID = CLK_REF_SRC_USBPHY_GATE,
    .irqNum = OTG0_IRQn,
    .cfg =
    {
        .epNum = 10,
        .ep0Mps = USB_OTG_MAX_EP0_SIZE,
        .phyif = USB_PHY_UTMI_WIDTH_16,
        .speed = USB_OTG_SPEED_HIGH,
        .dmaEnable = true,
        .sofEnable = false,
        .lpmEnable = false,
        .vbusSensingEnable = false,
        .suspendEnable = false,
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

#ifdef HAL_PWM_MODULE_ENABLED
const struct HAL_PWM_DEV g_pwm0Dev =
{
    .pReg = PWM0,
    .clkID = CLK_PWM0,
    .clkGateID = CLK_PWM0_GATE,
    .pclkGateID = PCLK_PWM0_GATE,
    .irqNum[0] = PWM0_CH0_IRQn,
    .irqNum[1] = PWM0_CH1_IRQn,
    .irqNum[2] = PWM0_CH2_IRQn,
    .irqNum[3] = PWM0_CH3_IRQn,
};

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
    .irqNum[6] = PWM1_CH6_IRQn,
    .irqNum[7] = PWM1_CH7_IRQn,
};
#endif  /* End of HAL_PWM_MODULE_ENABLED */

#ifdef HAL_SAI_MODULE_ENABLED
struct HAL_SAI_DEV g_sai0Dev =
{
    .pReg = SAI0,
    .mclk = MCLK_SAI0,
    .mclkGate = MCLK_SAI0_GATE,
    .maxLanes = 4,
    .bclkFs = 64,
    .irqNum = SAI0_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI0->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_SAI0_RX,
        .dmac = DMA1,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI0->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_SAI0_TX,
        .dmac = DMA1,
    },
};

struct HAL_SAI_DEV g_sai1Dev =
{
    .pReg = SAI1,
    .mclk = MCLK_SAI1,
    .mclkGate = MCLK_SAI1_GATE,
    .maxLanes = 4,
    .bclkFs = 64,
    .irqNum = SAI1_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI1->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_SAI1_RX,
        .dmac = DMA1,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI1->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_SAI1_TX,
        .dmac = DMA1,
    },
};

struct HAL_SAI_DEV g_sai2Dev =
{
    .pReg = SAI2,
    .mclk = MCLK_SAI2,
    .mclkGate = MCLK_SAI2_GATE,
    .maxLanes = 1,
    .bclkFs = 64,
    .irqNum = SAI2_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI2->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_SAI2_RX,
        .dmac = DMA1,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI2->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_SAI2_TX,
        .dmac = DMA1,
    },
};

struct HAL_SAI_DEV g_sai3Dev =
{
    .pReg = SAI3,
    .mclk = MCLK_SAI3,
    .mclkGate = MCLK_SAI3_GATE,
    .maxLanes = 1,
    .bclkFs = 64,
    .irqNum = SAI3_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI3->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_SAI3_RX,
        .dmac = DMA1,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI3->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_SAI3_TX,
        .dmac = DMA1,
    },
};

struct HAL_SAI_DEV g_sai4Dev =
{
    .pReg = SAI4,
    .mclk = MCLK_SAI4,
    .mclkGate = MCLK_SAI4_GATE,
    .maxLanes = 1,
    .bclkFs = 64,
    .irqNum = SAI4_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI4->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_SAI4_RX,
        .dmac = DMA1,
    },
};

#endif

#ifdef HAL_SPDIFRX_MODULE_ENABLED
struct HAL_SPDIFRX_DEV g_spdifrx0Dev =
{
    .pReg = SPDIFRX0,
    .mclk = MCLK_SPDIFRX,
    .rst = SRST_RESETN_SPDIFRX,
    .irqNum = SPDIF_RX_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SPDIFRX0->SMPDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_SPDIF_RX,
        .dmac = DMA1,
    },
};
#endif

#ifdef HAL_SPDIFTX_MODULE_ENABLED
struct HAL_SPDIFTX_DEV g_spdiftx0Dev =
{
    .pReg = SPDIFTX0,
    .mclk = MCLK_SPDIFTX,
    .irqNum = SPDIF_TX_IRQn,
    .txDmaData =
    {
        .addr = (uint32_t)&(SPDIFTX0->SMPDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_SPDIF_TX,
        .dmac = DMA1,
    },
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
        .channel = DMA0_REQ_SPI0_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI0_BASE + 0x400,
        .dmac = DMA0,
    },
    .rxDma = {
        .channel = DMA0_REQ_SPI0_RX,
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
    .irqNum = SPI1_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA0_REQ_SPI1_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI1_BASE + 0x400,
        .dmac = DMA0,
    },
    .rxDma = {
        .channel = DMA0_REQ_SPI1_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI1_BASE + 0x800,
        .dmac = DMA0,
    },
};
#endif  /* End of HAL_SPI_MODULE_ENABLED */

#ifdef HAL_TRNG_MODULE_ENABLED
const struct HAL_TRNG_DEV g_trngnsDev =
{
    .pReg = TRNG0,
};

const struct HAL_TRNG_DEV g_trngsDev =
{
    .pReg = TRNG1,
};
#endif

#ifdef HAL_UART_MODULE_ENABLED
const struct HAL_UART_DEV g_uart0Dev =
{
    .pReg = UART0,
    .sclkID = CLK_UART0,
    .sclkGateID = SCLK_UART0_SRC_GATE,
    .pclkGateID = PCLK_UART0_GATE,
    .irqNum = UART0_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart1Dev =
{
    .pReg = UART1,
    .sclkID = CLK_UART1,
    .sclkGateID = SCLK_UART1_SRC_GATE,
    .pclkGateID = PCLK_UART1_GATE,
    .irqNum = UART1_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart2Dev =
{
    .pReg = UART2,
    .sclkID = CLK_UART2,
    .sclkGateID = SCLK_UART2_SRC_GATE,
    .pclkGateID = PCLK_UART2_GATE,
    .irqNum = UART2_IRQn,
    .isAutoFlow = false,
};

const struct HAL_UART_DEV g_uart3Dev =
{
    .pReg = UART3,
    .sclkID = CLK_UART3,
    .sclkGateID = SCLK_UART3_SRC_GATE,
    .pclkGateID = PCLK_UART3_GATE,
    .irqNum = UART3_IRQn,
    .isAutoFlow = true,
};

const struct HAL_UART_DEV g_uart4Dev =
{
    .pReg = UART4,
    .sclkID = CLK_UART4,
    .sclkGateID = SCLK_UART4_SRC_GATE,
    .pclkGateID = PCLK_UART4_GATE,
    .irqNum = UART4_IRQn,
    .isAutoFlow = true,
};

const struct HAL_UART_DEV g_uart5Dev =
{
    .pReg = UART5,
    .sclkID = CLK_UART5,
    .sclkGateID = SCLK_UART5_GATE,
    .pclkGateID = PCLK_UART5_GATE,
    .irqNum = UART5_IRQn,
    .isAutoFlow = true,
};
#endif  /* End of HAL_UART_MODULE_ENABLED */

void BSP_Init(void)
{
}

void BSP_SetLoaderFlag(void)
{
    GRF_PMU->OS_REG0 = LDR_UPGRADE_FLAG;
}
