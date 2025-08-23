/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_bsp.h"

#ifdef HAL_ASRC_MODULE_ENABLED
struct HAL_ASRC_DEV g_asrc0Dev =
{
    .pReg = ASRC0,
    .mclk = MCLK_ASRC0,
    .mclkGate = MCLK_ASRC0_GATE,
    .lrckTX = LRCK_ASRC0_DST,
    .lrckRX = LRCK_ASRC0_SRC,
    .maxChannels = 8,
    .spinLockId = ASRC0_LOCK_ID,
    .irqNum = ASRC0_IRQn,
    .hrst = SRST_HRESETN_ASRC0,
    .txDmaData =
    {
        .addr = (uint32_t)&(ASRC0->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA1_REQ_AUDIO0_ASRC0_TX,
        .dmac = DMA1,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(ASRC0->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA1_REQ_AUDIO0_ASRC0_RX,
        .dmac = DMA1,
    },
};

struct HAL_ASRC_DEV g_asrc1Dev =
{
    .pReg = ASRC1,
    .mclk = MCLK_ASRC1,
    .mclkGate = MCLK_ASRC1_GATE,
    .lrckTX = LRCK_ASRC1_DST,
    .lrckRX = LRCK_ASRC1_SRC,
    .maxChannels = 4,
    .spinLockId = ASRC1_LOCK_ID,
    .irqNum = ASRC1_IRQn,
    .hrst = SRST_HRESETN_ASRC1,
    .txDmaData =
    {
        .addr = (uint32_t)&(ASRC1->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA1_REQ_AUDIO0_ASRC1_TX,
        .dmac = DMA1,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(ASRC1->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA1_REQ_AUDIO0_ASRC1_RX,
        .dmac = DMA1,
    },
};

struct HAL_ASRC_DEV g_asrc2Dev =
{
    .pReg = ASRC2,
    .mclk = MCLK_ASRC2,
    .mclkGate = MCLK_ASRC2_GATE,
    .lrckTX = LRCK_ASRC2_DST,
    .lrckRX = LRCK_ASRC2_SRC,
    .maxChannels = 4,
    .spinLockId = ASRC2_LOCK_ID,
    .irqNum = ASRC2_IRQn,
    .hrst = SRST_HRESETN_ASRC2,
    .txDmaData =
    {
        .addr = (uint32_t)&(ASRC2->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA1_REQ_AUDIO0_ASRC2_TX,
        .dmac = DMA1,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(ASRC2->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA1_REQ_AUDIO0_ASRC2_RX,
        .dmac = DMA1,
    },
};

struct HAL_ASRC_DEV g_asrc3Dev =
{
    .pReg = ASRC3,
    .mclk = MCLK_ASRC3,
    .mclkGate = MCLK_ASRC3_GATE,
    .lrckTX = LRCK_ASRC3_DST,
    .lrckRX = LRCK_ASRC3_SRC,
    .maxChannels = 4,
    .spinLockId = ASRC3_LOCK_ID,
    .irqNum = ASRC3_IRQn,
    .hrst = SRST_HRESETN_ASRC3,
    .txDmaData =
    {
        .addr = (uint32_t)&(ASRC3->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA1_REQ_AUDIO0_ASRC3_TX,
        .dmac = DMA1,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(ASRC3->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA1_REQ_AUDIO0_ASRC3_RX,
        .dmac = DMA1,
    },
};

struct HAL_ASRC_DEV g_asrc4Dev =
{
    .pReg = ASRC4,
    .mclk = MCLK_ASRC4,
    .mclkGate = MCLK_ASRC4_GATE,
    .lrckTX = LRCK_ASRC4_DST,
    .lrckRX = LRCK_ASRC4_SRC,
    .maxChannels = 8,
    .spinLockId = ASRC4_LOCK_ID,
    .irqNum = ASRC4_IRQn,
    .hrst = SRST_HRESETN_ASRC4,
    .txDmaData =
    {
        .addr = (uint32_t)&(ASRC4->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA3_REQ_AUDIO1_ASRC4_TX,
        .dmac = DMA3,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(ASRC4->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA3_REQ_AUDIO1_ASRC4_RX,
        .dmac = DMA3,
    },
};

struct HAL_ASRC_DEV g_asrc5Dev =
{
    .pReg = ASRC5,
    .mclk = MCLK_ASRC5,
    .mclkGate = MCLK_ASRC5_GATE,
    .lrckTX = LRCK_ASRC5_DST,
    .lrckRX = LRCK_ASRC5_SRC,
    .maxChannels = 4,
    .spinLockId = ASRC5_LOCK_ID,
    .irqNum = ASRC5_IRQn,
    .hrst = SRST_HRESETN_ASRC5,
    .txDmaData =
    {
        .addr = (uint32_t)&(ASRC5->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA3_REQ_AUDIO1_ASRC5_TX,
        .dmac = DMA3,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(ASRC5->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA3_REQ_AUDIO1_ASRC5_RX,
        .dmac = DMA3,
    },
};

struct HAL_ASRC_DEV g_asrc6Dev =
{
    .pReg = ASRC6,
    .mclk = MCLK_ASRC6,
    .mclkGate = MCLK_ASRC6_GATE,
    .lrckTX = LRCK_ASRC6_DST,
    .lrckRX = LRCK_ASRC6_SRC,
    .maxChannels = 4,
    .spinLockId = ASRC6_LOCK_ID,
    .irqNum = ASRC6_IRQn,
    .hrst = SRST_HRESETN_ASRC6,
    .txDmaData =
    {
        .addr = (uint32_t)&(ASRC6->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA3_REQ_AUDIO1_ASRC6_TX,
        .dmac = DMA3,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(ASRC6->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA3_REQ_AUDIO1_ASRC6_RX,
        .dmac = DMA3,
    },
};

struct HAL_ASRC_DEV g_asrc7Dev =
{
    .pReg = ASRC7,
    .mclk = MCLK_ASRC7,
    .mclkGate = MCLK_ASRC7_GATE,
    .lrckTX = LRCK_ASRC7_DST,
    .lrckRX = LRCK_ASRC7_SRC,
    .maxChannels = 4,
    .spinLockId = ASRC7_LOCK_ID,
    .irqNum = ASRC7_IRQn,
    .hrst = SRST_HRESETN_ASRC7,
    .txDmaData =
    {
        .addr = (uint32_t)&(ASRC7->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA3_REQ_AUDIO1_ASRC7_TX,
        .dmac = DMA3,
    },
    .rxDmaData =
    {
        .addr = (uint32_t)&(ASRC7->FIFO_IN_FIXED_DR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 4,
        .dmaReqCh = DMA3_REQ_AUDIO1_ASRC7_RX,
        .dmac = DMA3,
    },
};
#endif

#ifdef HAL_CANFD_MODULE_ENABLED
const struct HAL_CANFD_DEV g_can0Dev =
{
    .pReg = CAN,
    .sclkID = CLK_CAN,
    .sclkGateID = CLK_CAN_GATE,
    .pclkGateID = PCLK_CAN_GATE,
    .irqNum = CAN_IRQn,
};
#endif

#ifdef HAL_CRU_MODULE_ENABLED
static struct CRU_BANK_INFO cruBanks[] = {
    CRU_BANK_CFG_FLAGS(CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(PMU_CRU_BASE, 0x300, 0x800, 0xa00),
    CRU_BANK_CFG_FLAGS(SCRU_BASE, 0x0, 0x40, 0x80),
};

HAL_SECTION_SRAM_DATA
const struct HAL_CRU_DEV g_cruDev = {
    .banks = cruBanks,
    .banksNum = HAL_ARRAY_SIZE(cruBanks),
};
#endif

#ifdef HAL_FACC_FIR_MODULE_ENABLED
struct HAL_FACC_DEV g_firDev = {
    .pReg = FIR_ACC,
    .irqNum = FACC_FIR_IRQn,
};
#endif

#ifdef HAL_FACC_IIR_MODULE_ENABLED
struct HAL_FACC_DEV g_iirDev = {
    .pReg = IIR_ACC,
    .irqNum = FACC_IIR_IRQn,
};
#endif

#ifdef HAL_FSPI_MODULE_ENABLED
struct HAL_FSPI_HOST g_fspi0Dev =
{
    .instance = FSPI0,
    .sclkGate = 0,
    .hclkGate = 0,
    .xipClkGate = 0,
    .sclkID = SCLK_SFC,
    .irqNum = FSPI_IRQn,
    .xipMemCode = XIP_MAP0_BASE0,
    .xipMemData = XIP_MAP0_BASE0,
    .maxDllCells = 0x17F,
    .xmmcDev[0] =
    {
        .type = DEV_NOR,
    },
};
#endif

#ifdef HAL_GMAC_MODULE_ENABLED
const struct HAL_GMAC_DEV g_gmac0Dev =
{
    .pReg = GMAC,
    .clkID125M = CLK_MAC_ROOT,
    .clkID50M = CLK_MAC_ROOT,
    .clkGateID125M = CLK_MAC_ROOT_GATE,
    .clkGateID50M = CLK_MAC_ROOT_GATE,
    .pclkID = ACLK_HSPERI,
    .pclkGateID = PCLK_MAC_GATE,
#ifdef HAL_GMAC_PTP_FEATURE_ENABLED
    .ptpClkID = CLK_MAC_PTP_ROOT,
    .ptpClkGateID = CLK_MAC_PTP_ROOT_GATE,
#endif
    .irqNum = MAC_SBD_IRQn,
};
#endif

#ifdef HAL_HCD_MODULE_ENABLED
const struct HAL_USB_DEV g_usbotgh0Dev =
{
    .pReg = USB_OTG,
    .hclkGateID = HCLK_USBOTG_GATE,
    .utmiclkGateID = PCLK_USBPHY_GATE,
    .irqNum = OTG_IRQn,
    .BvalidIrqNum = OTG_BVALID_IRQn,
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
#endif /* HAL_I2C_MODULE_ENABLED */

#ifdef HAL_PCD_MODULE_ENABLED
const struct HAL_USB_DEV g_usbdDev =
{
    .pReg = USB_OTG,
    .hclkGateID = HCLK_USBOTG_GATE,
    .utmiclkGateID = PCLK_USBPHY_GATE,
    .irqNum = OTG_IRQn,
    .BvalidIrqNum = OTG_BVALID_IRQn,
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

#ifdef HAL_PDM_MODULE_ENABLED
struct HAL_PDM_DEV g_pdm0Dev =
{
    .pReg = PDM,
    .mclk = MCLK_PDM,
    .clkOut = CLKOUT_PDM,
    .clkOutRate = 2048000,
    .gain = 0xaf,
    .rxDmaData =
    {
        .addr = (uint32_t)&(PDM->RXFIFO_DATA_REG),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_AUDIO0_PDM_RX,
        .dmac = DMA1,
    },
};
#endif

#ifdef HAL_PL330_MODULE_ENABLED
struct HAL_PL330_DEV g_pl330Dev0 =
{
    .pReg = DMA0,
    .peripReqType = BURST,
    .irq[0] = DMAC0_BUS_CH0_IRQn,
    .irq[1] = DMAC0_BUS_CH1_IRQn,
    .irq[2] = DMAC0_BUS_CH2_IRQn,
    .irq[3] = DMAC0_BUS_CH3_IRQn,
    .irq[4] = DMAC0_BUS_CH4_IRQn,
    .irq[5] = DMAC0_BUS_CH5_IRQn,
    .irq[6] = DMAC0_BUS_CH6_IRQn,
    .irq[7] = DMAC0_BUS_CH7_IRQn,
    .irq[8] = DMAC0_BUS_ABORT_IRQn,
    .pd = 0,
};

struct HAL_PL330_DEV g_pl330Dev1 =
{
    .pReg = DMA1,
    .peripReqType = BURST,
    .irq[0] = DMAC1_AUDIO0_CH0_IRQn,
    .irq[1] = DMAC1_AUDIO0_CH1_IRQn,
    .irq[2] = DMAC1_AUDIO0_CH2_IRQn,
    .irq[3] = DMAC1_AUDIO0_CH3_IRQn,
    .irq[4] = DMAC1_AUDIO0_CH4_IRQn,
    .irq[5] = DMAC1_AUDIO0_CH5_IRQn,
    .irq[6] = DMAC1_AUDIO0_CH6_IRQn,
    .irq[7] = DMAC1_AUDIO0_CH7_IRQn,
    .irq[8] = DMAC1_AUDIO0_ABORT_IRQn,
    .pd = 0,
};

struct HAL_PL330_DEV g_pl330Dev2 =
{
    .pReg = DMA2,
    .peripReqType = BURST,
    .irq[0] = DMAC2_AUDIO0_CH0_IRQn,
    .irq[1] = DMAC2_AUDIO0_CH1_IRQn,
    .irq[2] = DMAC2_AUDIO0_CH2_IRQn,
    .irq[3] = DMAC2_AUDIO0_CH3_IRQn,
    .irq[4] = DMAC2_AUDIO0_CH4_IRQn,
    .irq[5] = DMAC2_AUDIO0_CH5_IRQn,
    .irq[6] = DMAC2_AUDIO0_CH6_IRQn,
    .irq[7] = DMAC2_AUDIO0_CH7_IRQn,
    .irq[8] = DMAC2_AUDIO0_ABORT_IRQn,
    .pd = 0,
};

struct HAL_PL330_DEV g_pl330Dev3 =
{
    .pReg = DMA3,
    .peripReqType = BURST,
    .irq[0] = DMAC3_AUDIO1_CH0_IRQn,
    .irq[1] = DMAC3_AUDIO1_CH1_IRQn,
    .irq[2] = DMAC3_AUDIO1_CH2_IRQn,
    .irq[3] = DMAC3_AUDIO1_CH3_IRQn,
    .irq[4] = DMAC3_AUDIO1_CH4_IRQn,
    .irq[5] = DMAC3_AUDIO1_CH5_IRQn,
    .irq[6] = DMAC3_AUDIO1_CH6_IRQn,
    .irq[7] = DMAC3_AUDIO1_CH7_IRQn,
    .irq[8] = DMAC3_AUDIO1_ABORT_IRQn,
    .pd = 0,
};

struct HAL_PL330_DEV g_pl330Dev4 =
{
    .pReg = DMA4,
    .peripReqType = BURST,
    .irq[0] = DMAC4_AUDIO1_CH0_IRQn,
    .irq[1] = DMAC4_AUDIO1_CH1_IRQn,
    .irq[2] = DMAC4_AUDIO1_CH2_IRQn,
    .irq[3] = DMAC4_AUDIO1_CH3_IRQn,
    .irq[4] = DMAC4_AUDIO1_CH4_IRQn,
    .irq[5] = DMAC4_AUDIO1_CH5_IRQn,
    .irq[6] = DMAC4_AUDIO1_CH6_IRQn,
    .irq[7] = DMAC4_AUDIO1_CH7_IRQn,
    .irq[8] = DMAC4_AUDIO1_ABORT_IRQn,
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
};
#endif

#ifdef HAL_SAI_MODULE_ENABLED
struct HAL_SAI_DEV g_sai0Dev =
{
    .pReg = SAI0,
    .mclk = SCLK_SAI0,
    .mclkGate = SCLK_SAI0_GATE,
    .maxLanes = 4,
    .bclkFs = 64,
    .irqNum = SAI0_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI0->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_AUDIO0_SAI0_RX,
        .dmac = DMA1,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI0->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_AUDIO0_SAI0_TX,
        .dmac = DMA1,
    },
};

struct HAL_SAI_DEV g_sai1Dev =
{
    .pReg = SAI1,
    .mclk = SCLK_SAI1,
    .mclkGate = SCLK_SAI1_GATE,
    .maxLanes = 2,
    .bclkFs = 64,
    .irqNum = SAI1_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI1->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_AUDIO0_SAI1_RX,
        .dmac = DMA1,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI1->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_AUDIO0_SAI1_TX,
        .dmac = DMA1,
    },
};

struct HAL_SAI_DEV g_sai2Dev =
{
    .pReg = SAI2,
    .mclk = SCLK_SAI2,
    .mclkGate = SCLK_SAI2_GATE,
    .maxLanes = 2,
    .bclkFs = 64,
    .irqNum = SAI2_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI2->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA2_REQ_AUDIO0_SAI2_RX,
        .dmac = DMA2,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI2->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA2_REQ_AUDIO0_SAI2_TX,
        .dmac = DMA2,
    },
};

struct HAL_SAI_DEV g_sai3Dev =
{
    .pReg = SAI3,
    .mclk = SCLK_SAI3,
    .mclkGate = SCLK_SAI3_GATE,
    .maxLanes = 2,
    .bclkFs = 64,
    .irqNum = SAI3_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI3->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA2_REQ_AUDIO0_SAI3_RX,
        .dmac = DMA2,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI3->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA2_REQ_AUDIO0_SAI3_TX,
        .dmac = DMA2,
    },
};

struct HAL_SAI_DEV g_sai4Dev =
{
    .pReg = SAI4,
    .mclk = SCLK_SAI4,
    .mclkGate = SCLK_SAI4_GATE,
    .maxLanes = 4,
    .bclkFs = 64,
    .irqNum = SAI4_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI4->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA3_REQ_AUDIO1_SAI4_RX,
        .dmac = DMA3,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI4->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA3_REQ_AUDIO1_SAI4_TX,
        .dmac = DMA3,
    },
};

struct HAL_SAI_DEV g_sai5Dev =
{
    .pReg = SAI5,
    .mclk = SCLK_SAI5,
    .mclkGate = SCLK_SAI5_GATE,
    .maxLanes = 2,
    .bclkFs = 64,
    .irqNum = SAI5_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI5->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA3_REQ_AUDIO1_SAI5_RX,
        .dmac = DMA3,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI5->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA3_REQ_AUDIO1_SAI5_TX,
        .dmac = DMA3,
    },
};

struct HAL_SAI_DEV g_sai6Dev =
{
    .pReg = SAI6,
    .mclk = SCLK_SAI6,
    .mclkGate = SCLK_SAI6_GATE,
    .maxLanes = 2,
    .bclkFs = 64,
    .irqNum = SAI6_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI6->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA4_REQ_AUDIO1_SAI6_RX,
        .dmac = DMA4,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI6->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA4_REQ_AUDIO1_SAI6_TX,
        .dmac = DMA4,
    },
};

struct HAL_SAI_DEV g_sai7Dev =
{
    .pReg = SAI7,
    .mclk = SCLK_SAI7,
    .mclkGate = SCLK_SAI7_GATE,
    .maxLanes = 2,
    .bclkFs = 64,
    .irqNum = SAI7_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SAI7->RXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA4_REQ_AUDIO1_SAI7_RX,
        .dmac = DMA4,
    },
    .txDmaData =
    {
        .addr = (uint32_t)&(SAI7->TXDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA4_REQ_AUDIO1_SAI7_TX,
        .dmac = DMA4,
    },
};
#endif

#ifdef HAL_SPDIFRX_MODULE_ENABLED
struct HAL_SPDIFRX_DEV g_spdifrx0Dev =
{
    .pReg = SPDIFRX0,
    .mclk = MCLK_SPDIFRX0,
    .mclkGate = MCLK_SPDIFRX0_GATE,
    .rst = SRST_RESETN_SPDIFRX0,
    .irqNum = SPDIF_RX0_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SPDIFRX0->SMPDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA1_REQ_AUDIO0_SPDIF_RX,
        .dmac = DMA1,
    },
};

struct HAL_SPDIFRX_DEV g_spdifrx1Dev =
{
    .pReg = SPDIFRX1,
    .mclk = MCLK_SPDIFRX1,
    .mclkGate = MCLK_SPDIFRX1_GATE,
    .rst = SRST_RESETN_SPDIFRX1,
    .irqNum = SPDIF_RX1_IRQn,
    .rxDmaData =
    {
        .addr = (uint32_t)&(SPDIFRX1->SMPDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA3_REQ_AUDIO1_SPDIF_RX,
        .dmac = DMA3,
    },
};
#endif

#ifdef HAL_SPDIFTX_MODULE_ENABLED
struct HAL_SPDIFTX_DEV g_spdiftx0Dev =
{
    .pReg = SPDIFTX0,
    .mclk = MCLK_SPDIFTX,
    .mclkGate = MCLK_SPDIFTX_GATE,
    .irqNum = SPDIF_TX_IRQn,
    .txDmaData =
    {
        .addr = (uint32_t)&(SPDIFTX0->SMPDR),
        .addrWidth = DMA_SLAVE_BUSWIDTH_4_BYTES,
        .maxBurst = 8,
        .dmaReqCh = DMA4_REQ_AUDIO1_SPDIF_TX,
        .dmac = DMA4,
    },
};
#endif

#ifdef HAL_SPI_MODULE_ENABLED
const struct HAL_SPI_DEV g_spi1Dev = {
    .base = SPI1_BASE,
    .clkId = CLK_SPI1,
    .clkGateID = CLK_SPI1_GATE,
    .pclkGateID = PCLK_SPI1_GATE,
    .maxFreq = 200000000,
    .irqNum = SPI1_IRQn,
    .isSlave = false,
    .txDma = {
        .channel = DMA0_REQ_BUS_SPI1_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI1_BASE + 0x400,
        .dmac = DMA0,
    },
    .rxDma = {
        .channel = DMA0_REQ_BUS_SPI1_RX,
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
        .channel = DMA0_REQ_BUS_SPI2_TX,
        .direction = DMA_MEM_TO_DEV,
        .addr = SPI2_BASE + 0x400,
        .dmac = DMA0,
    },
    .rxDma = {
        .channel = DMA0_REQ_BUS_SPI2_RX,
        .direction = DMA_DEV_TO_MEM,
        .addr = SPI2_BASE + 0x800,
        .dmac = DMA0,
    },
};
#endif

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
    .sclkGateID = 0,
    .pclkGateID = 0,
    .irqNum = UART0_IRQn,
    .isAutoFlow = false,
    .runtimeID = PM_RUNTIME_ID_UART0,
};

const struct HAL_UART_DEV g_uart1Dev =
{
    .pReg = UART1,
    .sclkID = CLK_UART1,
    .sclkGateID = 0,
    .pclkGateID = 0,
    .irqNum = UART1_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART1,
};

const struct HAL_UART_DEV g_uart2Dev =
{
    .pReg = UART2,
    .sclkID = CLK_UART2,
    .sclkGateID = 0,
    .pclkGateID = 0,
    .irqNum = UART2_IRQn,
    .isAutoFlow = false,
    .runtimeID = PM_RUNTIME_ID_UART2,
};

const struct HAL_UART_DEV g_uart3Dev =
{
    .pReg = UART3,
    .sclkID = CLK_UART3,
    .sclkGateID = 0,
    .pclkGateID = 0,
    .irqNum = UART3_IRQn,
    .isAutoFlow = true,
    .runtimeID = PM_RUNTIME_ID_UART3,
};
#endif

__WEAK void BSP_MPU_Init(void)
{
}

void BSP_DeInit(void)
{
}

void BSP_Init(void)
{
    BSP_MPU_Init();
}

void BSP_SetLoaderFlag(void)
{
    GRF_PMU->OS_REG0 = SYS_UPGRADE_FLAG;
}
