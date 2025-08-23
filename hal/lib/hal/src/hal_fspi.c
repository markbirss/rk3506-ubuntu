/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_FSPI_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FSPI
 *  @{
 */

/** @defgroup FSPI_How_To_Use How To Use
 *  @{

 The FSPI driver can be used as follows:

 For Nor flash This host driver need to be used in conjunction with device flash driver like
 hal_snor.c:

 - DMA mode: Register handler by calling HAL_FSPI_IRQHelper().
 - DMA mode: Unmask TRANSM interrupt by calling HAL_FSPI_UnmaskDMAInterrupt().
 - Initialize FSPI controller by calling HAL_FSPI_Init();
 - Send FSPI request:
    - SNOR support api: HAL_FSPI_SpiXfer() which can analyze the flash
        read and write protocol of the SNOR framework;
 - DMA mode: Handling interrupt return in DMA mode.
 - Configure XIP mode if needed by calling:
    - SNOR support api: HAL_FSPI_SpiXipConfig() which can analyze the flash
        read and write protocol of the SNOR framework;

 For psram:
 - Initialize FSPI controller by calling HAL_FSPI_Init();
 - Configure XIP mode if needed by calling HAL_FSPI_XmmcRequest() which support for FSPI config FSPI directly.

 Note.
 - If Nor flash and psram place in one FSPI host, Nor flash for cs0 and psram for cs1.
 - If psram is initial by preloader and work all the timer, set g_fspidev->xmmcDev[cs].type = DEV_PSRAM in hal_bsp.c.

 The APS6404L driver is low layer qpi psram code, it's completely different from hal_qpipsram
 driver can be used as follows:

 Select the corresponding initialization interface according to the requirements:

 - Initial APS6404L QPIPsram qpi mode by calling HAL_FSPI_APS6404lQpiHighSpeedInit(), support dll tuning;
 - Initial APS6404L QPIPsram qpi mode by calling HAL_FSPI_APS6404lQpiLowSpeedInit();
 - Initial APS6404L QPIPsram spi mode by calling HAL_FSPI_APS6404lSpiInit();

 @} */

/** @defgroup FSPI_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
// #define FSPI_DEBUG
#ifdef FSPI_DEBUG
#define FSPI_DBG(...) HAL_DBG(__VA_ARGS__)
#else
#define FSPI_DBG(...)
#endif

// #define XIP_DEBUG
#ifdef XIP_DEBUG
static void XIP_DBG(uint32_t value, int digits)
{
    uint32_t tmp;

    while (digits-- > 0) {
        tmp = value >> (4 * digits);
        tmp &= 0xf;

        if (tmp < 10) {
            WRITE_REG(UART0->THR, (uint8_t)(tmp + '0'));
        } else {
            WRITE_REG(UART0->THR, (uint8_t)(tmp - 10 + 'A'));
        }
    }
}

void XIP_DBGCOMBO(char tag, uint32_t value1, uint32_t value2, uint32_t value3, uint32_t value4)
{
    WRITE_REG(UART0->THR, tag);
    WRITE_REG(UART0->THR, '_');
    XIP_DBG(value1, 8);
    WRITE_REG(UART0->THR, '_');
    XIP_DBG(value2, 8);
    WRITE_REG(UART0->THR, '_');
    XIP_DBG(value3, 8);
    WRITE_REG(UART0->THR, '_');
    XIP_DBG(value4, 8);
    WRITE_REG(UART0->THR, '\r');
    WRITE_REG(UART0->THR, '\n');
    HAL_DelayUs(1000);
}

HAL_Status XIP_DBGHEX(char *s, void *buf, uint32_t width, uint32_t len)
{
    uint32_t i;
    uint32_t *p32 = (uint32_t *)buf;

    for (i = 0; i < len; i += 4) {
        XIP_DBGCOMBO('_', p32[i + 0], p32[i + 1], p32[i + 2], p32[i + 3]);
    }

    return HAL_OK;
}

#endif

/* FSPI_CTRL */
#define FSPI_CTRL_SHIFTPHASE_NEGEDGE 1

/* FSPI_RCVR */
#define FSPI_RCVR_RCVR_RESET (1 << FSPI_RCVR_RCVR_SHIFT) /* Recover The FSPI State Machine */

/* FSPI_ISR */
#define FSPI_ISR_DMAS_ACTIVE   (1 << FSPI_ISR_DMAS_SHIFT) /* DMA Finish Interrupt Active */
#define FSPI_ISR_NSPIS_ACTIVE  (1 << FSPI_ISR_NSPIS_SHIFT) /* SPI Error Interrupt Active */
#define FSPI_ISR_AHBS_ACTIVE   (1 << FSPI_ISR_AHBS_SHIFT) /* AHB Error Interrupt Active */
#define FSPI_ISR_TRANSS_ACTIVE (1 << FSPI_ISR_TRANSS_SHIFT) /* Transfer finish Interrupt Active */
#define FSPI_ISR_TXES_ACTIVE   (1 << FSPI_ISR_TXES_SHIFT) /* Transmit FIFO Empty Interrupt Active */
#define FSPI_ISR_TXOS_ACTIVE   (1 << FSPI_ISR_TXOS_SHIFT) /* Transmit FIFO Overflow Interrupt Active */
#define FSPI_ISR_RXUS_ACTIVE   (1 << FSPI_ISR_RXUS_SHIFT) /* Receive FIFO Underflow Interrupt Active */
#define FSPI_ISR_RXFS_ACTIVE   (1 << FSPI_ISR_RXFS_SHIFT) /* Receive FIFO Full Interrupt Active */

/* FSPI_FSR */
#define FSPI_FSR_RXFS_EMPTY (1 << FSPI_FSR_RXFS_SHIFT) /* Receive FIFO Full */
#define FSPI_FSR_RXES_EMPTY (1 << FSPI_FSR_RXES_SHIFT) /* Receive FIFO Empty */
#define FSPI_FSR_TXFS_FULL  (1 << FSPI_FSR_TXFS_SHIFT) /* Transmit FIFO Full */
#define FSPI_FSR_TXES_EMPTY (1 << FSPI_FSR_TXES_SHIFT) /* Transmit FIFO Empty */

/* FSPI_SR */
#define FSPI_SR_SR_BUSY (1 << FSPI_SR_SR_SHIFT) /* When busy, do not set the control register. */

/* FSPI_DMATR */
#define FSPI_DMATR_DMATR_START (1 << FSPI_DMATR_DMATR_SHIFT) /* Write 1 to start the dma transfer. */

/* FSPI_RISR */
#define FSPI_RISR_TRANSS_ACTIVE (1 << FSPI_RISR_TRANSS_SHIFT)

/* FSPI attributes */
#define FSPI_VER_VER_1  1
#define FSPI_VER_VER_3  3
#define FSPI_VER_VER_4  4
#define FSPI_VER_VER_5  5
#define FSPI_VER_VER_6  6
#define FSPI_VER_VER_10 10

#define FSPI_NOR_FLASH_PAGE_SIZE 0x100

#define FSPI_MAX_IOSIZE_VER3 (1024U * 8)
#define FSPI_MAX_IOSIZE_VER4 (0xFFFFFFFFU)

#define FSPI_VER_VER (FSPI_VER & FSPI_VER_VER_MASK)

#define TUNING_PATTERN_SIZE 0x8

static const uint32_t tuning_blk_pattern_4bit[TUNING_PATTERN_SIZE] =
{
    0xf0f0f0f0, 0x0f0f0f0f,
    0x92375c42, 0x98f92204,
    0xf0f0f0f0, 0x0f0f0f0f,
    0x8e6c4b50, 0xe0bc3418,
};

/********************* Private Structure Definition **************************/

typedef union {
    uint32_t d32;
    struct {
        unsigned txempty : 1; /* tx fifo empty */
        unsigned txfull : 1; /* tx fifo full */
        unsigned rxempty : 1; /* rx fifo empty */
        unsigned rxfull : 1; /* tx fifo empty interrupt mask */
        unsigned reserved7_4 : 4;
        unsigned txlevel : 7; /* tx fifo: 0: full; 1: left 1 entry; ... */
        unsigned reserved15 : 1;
        unsigned rxlevel : 5; /* rx fifo: 0: full; 1: left 1 entry; ... */
        unsigned reserved31_21 : 11;
    } b;
} FSPIFSR_DATA;

/** FSPI_XMMC bit union */
typedef union {
    uint32_t d32;
    struct {
        unsigned reserverd1 : 5;
        unsigned devHwEn : 1; /* device Hwrite Enable */
        unsigned prefetch : 1; /* prefetch enable */
        unsigned uincrPrefetchEn : 1; /* undefine INCR Burst Prefetch Enable */
        unsigned uincrLen : 4; /* undefine INCR length */
        unsigned devWrapEn : 1; /* device Wrap Enable */
        unsigned devIncrEn : 1; /* device INCR2/4/8/16 Enable */
        unsigned devUdfincrEn : 1; /* device Undefine INCR Enable */
        unsigned reserved2 : 17;
    } b;
} FSPIXMMCCTRL_DATA;
/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/
static void FSPI_Reset(struct HAL_FSPI_HOST *host)
{
    int32_t timeout = 10000;

    host->instance->RCVR = FSPI_RCVR_RCVR_RESET;
    while ((host->instance->RCVR == FSPI_RCVR_RCVR_RESET) && (timeout > 0)) {
        HAL_CPUDelayUs(1);
        timeout--;
    }
    host->instance->ICLR = 0xFFFFFFFF;
}

/**
 * @brief  Clear internal data transmission finish interrupt.
 * @param  host: FSPI host.
 * @return HAL_Status.
 */
static HAL_Status FSPI_ClearIsr(struct HAL_FSPI_HOST *host)
{
    host->instance->ICLR = 0xFFFFFFFF;

    return HAL_OK;
}

/*
 * VER 3: FSPI0 1/2 map space avail , FSPI1 1/4 map space avail:
 *   FSPI0: all 32MB map space in CS0
 *   FSPI1-n: all 16MB map space in CS0
 * VER 4-n: It's configurable, 1/2/4 CS, TBD
 * others: FSPI0: CS divide in average, each CS up to 64MB
 */
static void FSPI_XmmcDevRegionInit(struct HAL_FSPI_HOST *host)
{
#if (FSPI_VER_VER == FSPI_VER_VER_3)
    if (host->instance == FSPI0) {
        host->instance->DEVRGN = 25;
        host->instance->DEVSIZE0 = 25;
        host->instance->DEVSIZE1 = 25;
    } else {
#ifdef FSPI1
        host->instance->DEVRGN = 23;
        host->instance->DEVSIZE0 = 23;
        host->instance->DEVSIZE1 = 23;
#endif
    }
#else
    host->instance->DEVRGN = 27;
    host->instance->DEVSIZE0 = 26;
    host->instance->DEVSIZE1 = 26;
#endif /* FSPI_VER_VER == FSPI_VER_VER_3 */
}

HAL_UNUSED static void FSPI_TimeOutInit(struct HAL_FSPI_HOST *host)
{
    WRITE_REG(host->instance->SCLK_INATM_CNT, 0x20);
    SET_BIT(host->instance->TME0, FSPI_TME0_SCLK_INATM_EN_MASK);
    SET_BIT(host->instance->TME1, FSPI_TME0_SCLK_INATM_EN_MASK);
}

static void FSPI_ContModeInit(struct HAL_FSPI_HOST *host)
{
    /* Setup when FSPI->AX = 0xa5, Cancel when FSPI->AX = 0x0 */
    WRITE_REG(host->instance->EXT_AX, 0xa5 << FSPI_EXT_AX_AX_SETUP_PAT_SHIFT |
              0x5a << FSPI_EXT_AX_AX_CANCEL_PAT_SHIFT);
    /* Skip Continuous mode in default */
    switch (host->cs) {
    case 0:
        WRITE_REG(host->instance->AX0, 0 << FSPI_AX0_AX_SHIFT);
        break;
    case 1:
#ifdef RKMCU_RK2108
        WRITE_REG(host->instance->AX0, 0 << FSPI_AX0_AX_SHIFT);
#else
        WRITE_REG(host->instance->AX1, 0 << FSPI_AX1_AX_SHIFT);
#endif
        break;
    default:
        break;
    }
}

static void FSPI_ContModeDeInit(struct HAL_FSPI_HOST *host)
{
    /* FSPI avoid setup pattern match with FSPI->AX */
    WRITE_REG(host->instance->EXT_AX, 0x77 << FSPI_EXT_AX_AX_SETUP_PAT_SHIFT |
              0x88 << FSPI_EXT_AX_AX_CANCEL_PAT_SHIFT);
}

static HAL_Status FSPI_PollingEnable(struct HAL_FSPI_HOST *host, struct HAL_SPI_MEM_OP *op)
{
    HAL_Status ret = HAL_OK;

#if defined(FSPI_HRDYMASK_XIP_HUP_STATE_MASK) && defined(FSPI_POLLDLY_CTRL_OFFSET)
    int32_t timeout = 0;

    if (READ_REG(host->instance->MODE) & FSPI_MODE_XMMC_MODE_EN_MASK) {
        WRITE_REG(host->instance->HRDYMASK, FSPI_HRDYMASK_XIP_HUP_REQ_MASK);
        while (!(READ_REG(host->instance->HRDYMASK) & FSPI_HRDYMASK_XIP_HUP_STATE_MASK)) {
            HAL_CPUDelayUs(1);
            if (timeout++ > 100000) { /*wait 100ms*/
                ret = HAL_TIMEOUT;
                break;
            }
        }
    }
    WRITE_REG(host->instance->POLLDLY_CTRL, 0x80000100);
    WRITE_REG(host->instance->POLL_CTRL, FSPI_POLL_CTRL_ST_POLL_EN_MASK |                   \
              (op->cmd.opcode & 0xFF) << FSPI_POLL_CTRL_ST_POLL_CMD_PARA_SHIFT |            \
              FSPI_POLL_CTRL_POLL_DLY_EN_MASK |                                             \
              ((uint8_t *)op->data.buf.in)[0] << FSPI_POLL_CTRL_ST_POLL_EXPECT_DATA_SHIFT | \
              ((uint8_t *)op->data.buf.in)[1] << FSPI_POLL_CTRL_ST_POLL_BIT_COMP_EN_SHIFT);
    CLEAR_BIT(host->instance->IMR, FSPI_IMR_STPOLLM_MASK);
    WRITE_REG(host->instance->ICLR, FSPI_RISR_STPOLLS_MASK);
#endif

    return ret;
}

static HAL_Status FSPI_PollingDisable(struct HAL_FSPI_HOST *host)
{
#ifdef FSPI_POLL_CTRL_OFFSET
    WRITE_REG(host->instance->POLL_CTRL, 0);
    WRITE_REG(host->instance->ICLR, FSPI_RISR_STPOLLS_MASK);
    SET_BIT(host->instance->IMR, FSPI_IMR_STPOLLM_MASK);
#endif

    return HAL_OK;
}

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup FSPI_Exported_Functions_Group2 State and Errors Functions
 *  @{
 */

/**
 * @brief  Get the status of Polling Progress.
 * @param  host: FSPI host.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_IsPollFinished(struct HAL_FSPI_HOST *host)
{
    HAL_Status ret = HAL_OK;

#ifdef FSPI_RISR_STPOLLS_SHIFT
    if (READ_REG(host->instance->RISR) & FSPI_RISR_STPOLLS_MASK) {
        // HAL_DBG_HEX("FSPI-POLL-2:", FSPI0, 4, 0x41); /* Debug-POLL */
        FSPI_PollingDisable(host);
        ret = HAL_OK;
    } else {
        ret = HAL_BUSY;
    }
#endif

    return ret;
}

/** @} */

/** @defgroup FSPI_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 - Operate in blocking mode (Normal) using HAL_FSPI_Request();

 *  @{
 */

/**
 * @brief  Configuration register with flash operation protocol.
 * @param  host: FSPI host.
 * @param  op: flash operation protocol.
 * @return HAL_Status.
 * @attention Set host->cs to select chip.
 */
HAL_Status HAL_FSPI_XferStart(struct HAL_FSPI_HOST *host, struct HAL_SPI_MEM_OP *op)
{
    struct FSPI_REG *pReg = host->instance;
    FSPICMD_DATA FSPICmd;
    FSPICTRL_DATA FSPICtrl;
    uint32_t ctrlTemp;

#ifdef FSPI_CMD_EXT_OFFSET
    uint32_t cmdExt = 0, dummyExt = 0;
#endif

    FSPICmd.d32 = 0;
    FSPICtrl.d32 = 0;
    ctrlTemp = READ_REG(pReg->CTRL0);

    /* set CMD */
#ifdef FSPI_CMD_EXT_OFFSET
    if (op->cmd.nbytes == 2) {
        cmdExt = op->cmd.opcode;
        FSPICmd.b.cmd = op->cmd.opcode & 0xFF;
        FSPICtrl.b.cmd_ctrl = 2;
    }
#endif
    FSPICmd.b.cmd = op->cmd.opcode & 0xFF;

    if (op->cmd.buswidth == 8) {
        FSPICtrl.b.cmdlines = FSPI_LINES_X8;
        FSPICtrl.b.addrlines = FSPI_LINES_X8;
        FSPICtrl.b.datalines = FSPI_LINES_X8;
    } else if (op->cmd.buswidth == 4) {
        FSPICtrl.b.cmdlines = FSPI_LINES_X4;
        FSPICtrl.b.datalines = FSPI_LINES_X4;
    }
    FSPICtrl.b.cmd_str = op->cmd.dtr ? 0 : 1;

    /* set ADDR */
    if (op->addr.nbytes) {
        if (op->addr.nbytes == 4) {
            FSPICmd.b.addrbits = FSPI_ADDR_32BITS;
        } else if (op->addr.nbytes == 3) {
            FSPICmd.b.addrbits = FSPI_ADDR_24BITS;
        } else {
            FSPICmd.b.addrbits = FSPI_ADDR_XBITS;
            pReg->ABIT0 = op->addr.nbytes * 8 - 1;
        }

        if (op->addr.buswidth == 8) {
            FSPICtrl.b.addrlines = FSPI_LINES_X8;
        } else if (op->addr.buswidth == 4) {
            FSPICtrl.b.addrlines = FSPI_LINES_X4;
        }
    }
    FSPICtrl.b.addr_str = op->addr.dtr ? 0 : 1;

    /* set DUMMY*/
    if (op->dummy.nbytes) {
        switch (op->dummy.buswidth) {
#ifdef FSPI_DUMM_CTRL_DUMM_EXT_SHIFT
        case 8:
            dummyExt = ((op->dummy.nbytes >> op->dummy.dtr) << FSPI_DUMM_CTRL_DUMM_EXT_SHIFT) | FSPI_DUMM_CTRL_DUMM_SEL_MASK;
            dummyExt |= FSPI_DUMM_CTRL_DUMM_SEL_MASK;
            break;
#endif
        case 4:
            FSPICmd.b.dummybits = op->dummy.nbytes * 2;
            break;
        case 2:
            FSPICmd.b.dummybits = op->dummy.nbytes * 4;
            break;
        default:
            FSPICmd.b.dummybits = op->dummy.nbytes * 8;
            break;
        }
    }

    /* set DATA */
#if (FSPI_VER_VER > FSPI_VER_VER_3)
    WRITE_REG(pReg->LEN_EXT, op->data.nbytes);
#else
    FSPICmd.b.datasize = op->data.nbytes;
#endif
    if (op->data.nbytes) {
        if (op->data.dir == HAL_SPI_MEM_DATA_OUT) {
            FSPICmd.b.rw = FSPI_WRITE;
        }
        if (op->data.buswidth == 8) {
            FSPICtrl.b.datalines = FSPI_LINES_X8;
        } else if (op->data.buswidth == 4) {
            FSPICtrl.b.datalines = FSPI_LINES_X4;
        } else if (op->data.buswidth == 2) {
            FSPICtrl.b.datalines = FSPI_LINES_X2;
        } else {
            FSPICtrl.b.datalines = FSPI_LINES_X1;
        }
    }
    FSPICtrl.b.dtr_mode = op->data.dtr ? 1 : 0;
    FSPICtrl.b.dat_mode = op->data.swap ? 1 : 0;

    /* spitial setting */
    FSPICtrl.b.sps = host->mode & HAL_SPI_CPHA;
#ifdef RKMCU_RK2108
    if (host->cs >= 1) {
        FSPICmd.b.cs = 2;
    }
#else
    FSPICmd.b.cs = host->cs;
#endif
    FSPICtrl.b.dqs_mode = host->mode & HAL_SPI_DQS ? 1 : 0;
    if (op->data.nbytes == 0 && op->addr.nbytes) {
        FSPICmd.b.rw = FSPI_WRITE;
    }

    if (!(pReg->FSR & FSPI_FSR_TXES_EMPTY) || !(pReg->FSR & FSPI_FSR_RXES_EMPTY) || (pReg->SR & FSPI_SR_SR_BUSY)) {
        FSPI_Reset(host);
    }

    if (op->data.poll) {
        FSPICtrl.d32 = ctrlTemp;
    }

    // FSPI_DBG("%s 1 %x %x %x\n", __func__, op->addr.nbytes, op->dummy.nbytes, op->data.nbytes);
    // FSPI_DBG("%s 2 %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32 " %x\n", __func__, FSPICtrl.d32, FSPICmd.d32, cmdExt, dummyExt, op->addr.val, host->cs);
    // XIP_DBGCOMBO('2', FSPICtrl.d32, FSPICmd.d32, cmdExt, dummyExt);

    /* config FSPI */
#ifdef FSPI_CMD_EXT_OFFSET
    WRITE_REG(pReg->CMD_EXT, cmdExt);
    WRITE_REG(pReg->DUMM_CTRL, dummyExt);
#endif

#ifdef HAL_NPOR_MODULE_ENABLED
    while (!HAL_NPOR_IsPowergood()) {
        FSPI_DBG("npor");
    }
#endif

    switch (host->cs) {
    case 0:
        pReg->CTRL0 = FSPICtrl.d32;
        break;
    case 1:
#ifdef RKMCU_RK2108
        pReg->CTRL0 = FSPICtrl.d32;
#else
        pReg->CTRL1 = FSPICtrl.d32;
#endif
        break;
    default:
        break;
    }
    pReg->CMD = FSPICmd.d32;
    if (op->addr.nbytes) {
        pReg->ADDR = op->addr.val;
    }

    return HAL_OK;
}

/**
 * @brief  IO transfer.
 * @param  host: FSPI host.
 * @param  len: data n bytes.
 * @param  data: transfer buffer.
 * @param  dir: transfer direction.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_XferData(struct HAL_FSPI_HOST *host, uint32_t len, void *data, uint32_t dir)
{
    HAL_Status ret = HAL_OK;
    __IO FSPIFSR_DATA fifostat;
    int32_t timeout = 0;
    uint32_t i, words;
    uint32_t *pData = (uint32_t *)data;
    struct FSPI_REG *pReg = host->instance;
    uint32_t temp = 0;

    HAL_ASSERT(data && len);

    if (len && len < 4 && dir == FSPI_WRITE) {
        if (len == 1) {
            temp = *((uint8_t *)data);
        } else if (len == 2) {
            temp = *((uint16_t *)data);
        } else {
            temp = ((uint8_t *)data)[0] | ((uint8_t *)data)[1] << 8 | ((uint8_t *)data)[2] << 16;
        }
        pData = &temp;
    } else if (len >= 4 && !HAL_IS_ALIGNED((uint32_t)data, 4)) {
        FSPI_DBG("%s data unaligned access\n", __func__);
#if defined(HAL_FSPI_WORD_ALIGNED_CHECK)

        return HAL_INVAL;
#endif
    }

    /* FSPI_DBG("%s %p len %" PRIx32 " word0 %" PRIx32 " dir %" PRIx32 "\n", __func__, pData, len, pData[0], dir); */
    if (dir == FSPI_WRITE) {
        words = (len + 3) >> 2;
        while (words) {
            fifostat.d32 = (pReg->FSR & FSPI_FSR_TXWLVL_MASK);
            if ((fifostat.b.txlevel) > 0) {
                uint32_t count = HAL_MIN(words, fifostat.b.txlevel);

                for (i = 0; i < count; i++) {
                    pReg->DATA = *pData++;
                    words--;
                }
                if (words == 0) {
                    break;
                }
                timeout = 0;
            } else {
                HAL_CPUDelayUs(1);
                if (timeout++ > 10000) {
                    ret = HAL_TIMEOUT;
                    break;
                }
            }
        }
    } else {
        uint32_t bytes = len & 0x3;

        words = len >> 2;
        while (words) {
            fifostat.d32 = pReg->FSR;
            if (fifostat.b.rxlevel > 0) {
                uint32_t count = HAL_MIN(words, fifostat.b.rxlevel);
                if (count == 15) { /* Reduce CPU cost */
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;

                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    *pData++ = pReg->DATA;
                    words -= 15;
                } else {
                    for (i = 0; i < count; i++) {
                        *pData++ = pReg->DATA;
                    }
                    words -= count;
                }

                if (0 == words) {
                    break;
                }
                timeout = 0;
            } else {
                HAL_CPUDelayUs(1);
                if (timeout++ > 10000) {
                    ret = HAL_TIMEOUT;
                    break;
                }
            }
        }

        timeout = 0;
        while (bytes) {
            fifostat.d32 = pReg->FSR;
            if (fifostat.b.rxlevel > 0) {
                uint8_t *pData1 = (uint8_t *)pData;
                words = pReg->DATA;
                for (i = 0; i < bytes; i++) {
                    pData1[i] = (uint8_t)((words >> (i * 8)) & 0xFF);
                }
                break;
            } else {
                HAL_CPUDelayUs(1);
                if (timeout++ > 10000) {
                    ret = HAL_TIMEOUT;
                    break;
                }
            }
        }
    }

    return ret;
}

/**
 * @brief  IO transfer by internal DMA.
 * @param  host: FSPI host.
 * @param  len: data n bytes.
 * @param  data: transfer buffer.
 * @param  dir: transfer direction.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_XferData_DMA(struct HAL_FSPI_HOST *host, uint32_t len, void *data, uint32_t dir)
{
    HAL_Status ret = HAL_OK;
    int32_t timeout = 0;
    struct FSPI_REG *pReg = host->instance;

    HAL_ASSERT(data && len);

    if (dir == HAL_SPI_MEM_DATA_OUT) {
        HAL_DCACHE_CleanInvalidateByRange((uint32_t)data, len);
    } else {
        HAL_DCACHE_InvalidateByRange((uint32_t)data, len);
    }
    pReg->ICLR = 0xFFFFFFFF;
    pReg->DMAADDR = (uint32_t)data;
    pReg->DMATR = FSPI_DMATR_DMATR_START;

    timeout = len * 10;
    while (timeout--) {
        if (READ_BIT(pReg->RISR, FSPI_ISR_DMAS_MASK)) {
            break;
        }
        HAL_DelayUs(1);
    }

    pReg->ICLR = 0xFFFFFFFF;
    if (timeout <= 0) {
        ret = HAL_TIMEOUT;
    }

    if (dir == HAL_SPI_MEM_DATA_IN) {
        HAL_DCACHE_InvalidateByRange((uint32_t)data, len);
    }

    return ret;
}

/**
 * @brief  Wait for FSPI host transfer finished.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_XferDone(struct HAL_FSPI_HOST *host)
{
    HAL_Status ret = HAL_OK;
    int32_t timeout = 0;
    struct FSPI_REG *pReg = host->instance;

    while (pReg->SR & FSPI_SR_SR_BUSY) {
        HAL_CPUDelayUs(1);
        if (timeout++ > 100000) { /*wait 100ms*/
            ret = HAL_TIMEOUT;
            FSPI_Reset(host);
            break;
        }
    }
    HAL_CPUDelayUs(1); //CS# High Time (read/write) >100ns

    return ret;
}

/**
 * @brief  SPI Nor flash data transmission interface to support open source specifications SNOR.
 * @param  host: FSPI host.
 * @param  op: flash operation protocol.
 * @return HAL_Status.
 * @attention Set host->cs to select chip.
 */
HAL_Status HAL_FSPI_SpiXfer(struct HAL_FSPI_HOST *host, struct HAL_SPI_MEM_OP *op)
{
    HAL_Status ret = HAL_OK;
    uint32_t dir = op->data.dir;
    void *pData = NULL;

    HAL_ASSERT(IS_FSPI_INSTANCE(host->instance));

    if (op->data.buf.in) {
        pData = (void *)op->data.buf.in;
    } else if (op->data.buf.out) {
        pData = (void *)op->data.buf.out;
    }

    ret = HAL_FSPI_XferStart(host, op);
    if (ret) {
        return ret;
    }

    if (pData) {
#if defined(HAL_FSPI_DMA_ENABLED)
        if ((op->data.nbytes >= FSPI_NOR_FLASH_PAGE_SIZE) &&
            HAL_IS_CACHELINE_ALIGNED(pData) &&
            HAL_IS_CACHELINE_ALIGNED(op->data.nbytes)) {
            ret = HAL_FSPI_XferData_DMA(host, op->data.nbytes, pData, dir);
        } else
#endif
        ret = HAL_FSPI_XferData(host, op->data.nbytes, pData, dir);
        if (ret) {
            FSPI_Reset(host);
            FSPI_DBG("%s xfer data failed ret %d\n", __func__, ret);

            return ret;
        }
    }

    return HAL_FSPI_XferDone(host);
}

/**
 * @brief  SPI Nor flash data transmission interface with polling enable.
 * @param  host: FSPI host.
 * @param  op: flash operation protocol.
 * @return HAL_Status.
 * @attention Set host->cs to select chip.
 */
HAL_Status HAL_FSPI_SpiXferHWPolling(struct HAL_FSPI_HOST *host, struct HAL_SPI_MEM_OP *op)
{
    HAL_Status ret = HAL_OK;

    HAL_ASSERT(IS_FSPI_INSTANCE(host->instance));
    if (!op->data.buf.in || op->data.nbytes < HAL_SPI_POLL_DATA_FORMAT_SIZE) {
        return HAL_INVAL;
    }
    ret = FSPI_PollingEnable(host, op);
    if (ret) {
        return ret;
    }
    // HAL_DBG_HEX("FSPI-POLL-1:", FSPI0, 4, 0x41); /* Debug-POLL */

    ret = HAL_FSPI_XferStart(host, op);
    if (ret) {
        FSPI_PollingDisable(host);
    }

    return ret;
}

/** @} */

/** @defgroup FSPI_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 *  @{
 */

/**
 * @brief  Init FSPI.
 * @param  host: FSPI host.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_Init(struct HAL_FSPI_HOST *host)
{
    int32_t timeout = 0;
    struct FSPI_REG *pReg;
    HAL_Status ret = HAL_OK;

    HAL_ASSERT(IS_FSPI_INSTANCE(host->instance));

    pReg = host->instance;
    pReg->MODE = 0;
    while (pReg->SR & FSPI_SR_SR_BUSY) {
        HAL_CPUDelayUs(1);
        if (timeout++ > 1000) {
            return HAL_TIMEOUT;
        }
    }
    FSPI_ContModeInit(host);
    pReg->CTRL0 = 0;
    FSPI_XmmcDevRegionInit(host);

#if (FSPI_VER_VER > FSPI_VER_VER_3)
    WRITE_REG(pReg->LEN_CTRL, FSPI_LEN_CTRL_TRB_SEL_MASK);
#endif

    return ret;
}

/**
 * @brief  DeInit FSPI.
 * @param  host: FSPI host.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_DeInit(struct HAL_FSPI_HOST *host)
{
    HAL_ASSERT(IS_FSPI_INSTANCE(host->instance));

    host->instance->MODE = 0;
    FSPI_ContModeDeInit(host);
    FSPI_Reset(host);

    return HAL_OK;
}

/**
 * @brief  Initial APS6404L QPIPsram qpi mode with delay line tuning and clock no divide.
 * @param  pReg: FSPI reg base.
 * @param  xipBase: FSPI XIP base.
 * @param  dll: delay line cells,
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_APS6404lQpiHighSpeedInit(struct FSPI_REG *pReg, uint32_t xipBase, uint32_t dll)
{
    int32_t i, right, left, cells;
    uint32_t *buf = (uint32_t *)xipBase;

    /* FSPI low speed configuration */
    WRITE_REG(pReg->MODE, 0);
    WRITE_REG(pReg->CTRL0, 2);
    WRITE_REG(pReg->EXT_CTRL, 0x4023);
    WRITE_REG(pReg->CMD, 0x35);
    WRITE_REG(pReg->CTRL0, 0x2a02);
    WRITE_REG(pReg->XMMC_CTRL, 0x72a0);
    WRITE_REG(pReg->XMMC_RCMD0, 0x46eb);
    WRITE_REG(pReg->XMMC_WCMD0, 0x4038);
    WRITE_REG(pReg->MODE, 1);

    for (i = 0; i < TUNING_PATTERN_SIZE; i++) {
        buf[i] = tuning_blk_pattern_4bit[i];
    }

    /* FSPI high speed configuration */
    WRITE_REG(pReg->MODE, 0);
    WRITE_REG(pReg->EXT_CTRL, 0x1004023);
    WRITE_REG(pReg->MODE, 1);
    left = -1;
    for (right = 0; right <= dll; right++) {
        WRITE_REG(pReg->MODE, 0);
        WRITE_REG(pReg->DLL_CTRL0, 1 << FSPI_DLL_CTRL0_SCLK_SMP_SEL_SHIFT | right);
        WRITE_REG(pReg->MODE, 1);
        for (i = 0; i < TUNING_PATTERN_SIZE; i++) {
            if (buf[i] != tuning_blk_pattern_4bit[i]) {
                break;
            }
        }
        if (left == -1 && i >= TUNING_PATTERN_SIZE) {
            left = right;
        } else if (left >= 0 && i < TUNING_PATTERN_SIZE) {
            break;
        }
    }

    if (left >= 0 && (right - left > 10)) {
        cells = (uint8_t)((right + left) / 2);
        WRITE_REG(pReg->MODE, 0);
        WRITE_REG(pReg->DLL_CTRL0, 1 << FSPI_DLL_CTRL0_SCLK_SMP_SEL_SHIFT | cells);
        WRITE_REG(pReg->MODE, 1);
    } else {
        WRITE_REG(pReg->MODE, 0);
        WRITE_REG(pReg->DLL_CTRL0, 0);
        WRITE_REG(pReg->MODE, 1);
    }

    return HAL_OK;
}

/**
 * @brief  Initial APS6404L QPIPsram qpi mode with no deline line.
 * @param  pReg: FSPI reg base.
 * @param  xipBase: FSPI XIP base.
 * @param  div2: whether setting clock divide 2.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_APS6404lQpiLowSpeedInit(struct FSPI_REG *pReg, uint32_t xipBase, bool div2)
{
    /* FSPI low speed configuration */
    WRITE_REG(pReg->MODE, 0);
    WRITE_REG(pReg->CTRL0, 2);
    if (div2) {
        WRITE_REG(pReg->EXT_CTRL, 0x4023);
    } else {
        WRITE_REG(pReg->EXT_CTRL, 0x1004023);
    }
    WRITE_REG(pReg->CMD, 0x35);
    WRITE_REG(pReg->CTRL0, 0x2a02);
    WRITE_REG(pReg->XMMC_CTRL, 0x72a0);
    WRITE_REG(pReg->XMMC_RCMD0, 0x46eb);
    WRITE_REG(pReg->XMMC_WCMD0, 0x4038);
    WRITE_REG(pReg->MODE, 1);

    return HAL_OK;
}

/**
 * @brief  Initial APS6404L QPIPsram spi mode with clock divide 2.
 * @param  pReg: FSPI reg base.
 * @param  xipBase: FSPI XIP base.
 * @param  div2: whether setting clock divide 2.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_APS6404lSpiInit(struct FSPI_REG *pReg, uint32_t xipBase, bool div2)
{
    /* FSPI low speed configuration */
    WRITE_REG(pReg->MODE, 0);
    WRITE_REG(pReg->CTRL0, 2);
    if (div2) {
        WRITE_REG(pReg->EXT_CTRL, 0x4023);
    } else {
        WRITE_REG(pReg->EXT_CTRL, 0x1004023);
    }
    WRITE_REG(pReg->XMMC_CTRL, 0x72a0);
    WRITE_REG(pReg->XMMC_RCMD0, 0x4003);
    WRITE_REG(pReg->XMMC_WCMD0, 0x4002);
    WRITE_REG(pReg->MODE, 1);

    return HAL_OK;
}

/** @} */

/** @defgroup FSPI_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Mask internal data transmission finish interrupt.
 * @param  host: FSPI host.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_MaskDMAInterrupt(struct HAL_FSPI_HOST *host)
{
    SET_BIT(host->instance->IMR, FSPI_IMR_DMAM_MASK);

    return HAL_OK;
}

/**
 * @brief  Unmask internal data transmission finish interrupt.
 * @param  host: FSPI host.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_UnmaskDMAInterrupt(struct HAL_FSPI_HOST *host)
{
    CLEAR_BIT(host->instance->IMR, FSPI_IMR_DMAM_MASK);

    return HAL_OK;
}

/**
 * @brief  FSPI interrupt handler.
 * @param  host: FSPI host.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_IRQHelper(struct HAL_FSPI_HOST *host)
{
    FSPI_ClearIsr(host);

    return HAL_OK;
}

/**
 * @brief  Set XMMC dev.
 * @param  host: FSPI host.
 * @param  op: flash operation protocol.
 * @return HAL_Status.
 * @attention Set host->cs to select chip.
 */
HAL_Status HAL_FSPI_XmmcSetting(struct HAL_FSPI_HOST *host, struct HAL_SPI_MEM_OP *op)
{
    FSPICMD_DATA FSPICmd;
    FSPICTRL_DATA FSPICtrl;
    FSPIXMMCCTRL_DATA xmmcCtrl;
    uint32_t cmdExt = 0, dummyExt = 0;

    HAL_ASSERT(IS_FSPI_INSTANCE(host->instance));

    FSPICmd.d32 = 0;
    FSPICtrl.d32 = 0;

    /* set CMD */
    if (op->cmd.nbytes == 2) {
        cmdExt = op->cmd.opcode;
        FSPICtrl.b.cmd_ctrl = 2;
    }
    FSPICmd.b.cmd = op->cmd.opcode & 0xFF;

    if (op->cmd.buswidth == 8) {
        FSPICtrl.b.cmdlines = FSPI_LINES_X8;
        FSPICtrl.b.addrlines = FSPI_LINES_X8;
        FSPICtrl.b.datalines = FSPI_LINES_X8;
    } else if (op->cmd.buswidth == 4) {
        FSPICtrl.b.cmdlines = FSPI_LINES_X4;
        FSPICtrl.b.datalines = FSPI_LINES_X4;
    }
    FSPICtrl.b.cmd_str = op->cmd.dtr ? 0 : 1;
    if (op->addr.buswidth == 4 && (host->xmmcDev[host->cs].type == DEV_NOR || host->xmmcDev[host->cs].type == DEV_SPINAND)) {
        FSPICmd.b.readmode = 1;
    }

    /* set ADDR */
    if (op->addr.nbytes) {
        if (op->addr.nbytes == 4) {
            FSPICmd.b.addrbits = FSPI_ADDR_32BITS;
        } else if (op->addr.nbytes == 3) {
            FSPICmd.b.addrbits = FSPI_ADDR_24BITS;
        } else {
            FSPICmd.b.addrbits = FSPI_ADDR_XBITS;
        }

        if (op->addr.buswidth == 8) {
            FSPICtrl.b.addrlines = FSPI_LINES_X8;
        } else if (op->addr.buswidth == 4) {
            FSPICtrl.b.addrlines = FSPI_LINES_X4;
        }
    }
    FSPICtrl.b.addr_str = op->addr.dtr ? 0 : 1;

    /* set DUMMY*/
    FSPICtrl.b.scic = op->dummy.a2dIdle;
    if (op->dummy.nbytes) {
        switch (op->dummy.buswidth) {
#ifdef FSPI_DUMM_CTRL_DUMM_EXT_SHIFT
        case 8:
            dummyExt = ((op->dummy.nbytes >> op->dummy.dtr) << FSPI_DUMM_CTRL_DUMM_EXT_SHIFT) | FSPI_DUMM_CTRL_DUMM_SEL_MASK;
            dummyExt |= FSPI_DUMM_CTRL_DUMM_SEL_MASK;
            break;
#endif
        case 4:
            FSPICmd.b.dummybits = op->dummy.nbytes * 2;
            break;
        case 2:
            FSPICmd.b.dummybits = op->dummy.nbytes * 4;
            break;
        default:
            FSPICmd.b.dummybits = op->dummy.nbytes * 8;
            break;
        }
    }

    if (FSPICmd.b.readmode) {
        FSPICmd.b.dummybits -= op->data.dtr ? 1 : 2; /* M7-0 ocuppy Dummy 2 cycles  */
    }
    /* set DATA */
    if (op->data.dir == HAL_SPI_MEM_DATA_OUT) {
        FSPICmd.b.rw = FSPI_WRITE;
    }
    if (op->data.buswidth == 8) {
        FSPICtrl.b.datalines = FSPI_LINES_X8;
    } else if (op->data.buswidth == 4) {
        FSPICtrl.b.datalines = FSPI_LINES_X4;
    } else if (op->data.buswidth == 2) {
        FSPICtrl.b.datalines = FSPI_LINES_X2;
    } else {
        FSPICtrl.b.datalines = FSPI_LINES_X1;
    }
    FSPICtrl.b.dtr_mode = op->data.dtr ? 1 : 0;
    FSPICtrl.b.dat_mode = op->data.swap ? 1 : 0;

    /* set XMMC */
    if (host->xmmcDev[0].type == DEV_PSRAM || host->xmmcDev[1].type == DEV_PSRAM) {
        xmmcCtrl.b.devHwEn = 1;
        xmmcCtrl.b.prefetch = 0;
    } else {
        xmmcCtrl.b.devHwEn = 0;
#if !defined(HAL_DCACHE_MODULE_ENABLED) && (FSPI_VER_VER == 3)
        xmmcCtrl.b.prefetch = 0;
#else
        xmmcCtrl.b.prefetch = 1;
        FSPI_TimeOutInit(host);
#endif
    }

    /* spetial setting */
    FSPICtrl.b.sps = host->mode & HAL_SPI_CPHA;
    FSPICtrl.b.dqs_mode = host->mode & HAL_SPI_DQS ? 1 : 0;

    // HAL_DBG("%s 1 %x %x %x\n", __func__, op->addr.nbytes, op->dummy.nbytes, op->data.nbytes);
    // HAL_DBG("%s 2 %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32 " %x\n", __func__, FSPICtrl.d32, FSPICmd.d32, cmdExt, dummyExt, op->addr.val, host->cs);
    host->xmmcDev[host->cs].ctrl = FSPICtrl.d32;
    host->xmmcCtrl = xmmcCtrl.d32;
    host->xmmcDummyCtrl = dummyExt;
    if (op->data.dir == HAL_SPI_MEM_DATA_IN) {
        host->xmmcDev[host->cs].readCmd = FSPICmd.d32 | (cmdExt << 16);
    } else {
        host->xmmcDev[host->cs].writeCmd = FSPICmd.d32;
    }

    return HAL_OK;
}

/**
 * @brief  Configure FSPI XIP mode.
 * @param  host: FSPI host.
 * @param  on: 1 enable, 0 disable.
 * @return HAL_Status.
 * If psram is initialized by preloader and work all the timer, set g_fspidev->xmmcDev[cs].type = DEV_PSRAM in hal_bsp.c.
 */
HAL_Status HAL_FSPI_XmmcRequest(struct HAL_FSPI_HOST *host, uint8_t on)
{
    struct FSPI_REG *pReg = host->instance;
    int timeout = 0;

    if (on) {
        if (pReg->MODE & 0x1) {
            return HAL_INVAL;
        }

        /* FSPI xmmcCtrl config */
        MODIFY_REG(pReg->XMMC_CTRL, FSPI_XMMC_CTRL_DEV_HWEN_MASK |
                   FSPI_XMMC_CTRL_PFT_EN_MASK,
                   host->xmmcCtrl);
        FSPI_ContModeInit(host);

        /* FSPI device config */
        /* FSPI_DBG("%s %p enable 3 %" PRIx32 " %" PRIx32 " %" PRIx32 " %" PRIx32 "\n", __func__,
                    host->instance,
                    host->xmmcDev[0].ctrl, host->cs,
                    host->xmmcDev[0].readCmd, host->xmmcDev[0].writeCmd); */
        WRITE_REG(pReg->CTRL0, host->xmmcDev[0].ctrl);
        WRITE_REG(pReg->XMMC_RCMD0, host->xmmcDev[0].readCmd);
        WRITE_REG(pReg->XMMC_WCMD0, host->xmmcDev[0].writeCmd);

        WRITE_REG(pReg->CTRL1, host->xmmcDev[1].ctrl);
        WRITE_REG(pReg->XMMC_RCMD1, host->xmmcDev[1].readCmd);
        WRITE_REG(pReg->XMMC_WCMD1, host->xmmcDev[1].writeCmd);

#ifdef FSPI_CMD_EXT_OFFSET
        WRITE_REG(pReg->DUMM_CTRL, host->xmmcDummyCtrl);
#endif

        // HAL_DBG_HEX("FSPI:", FSPI0, 4, 0x41);
        // XIP_DBGHEX("FSPI:", FSPI0, 4, 0x40);
        WRITE_REG(pReg->MODE, 1);
    } else {
        /* FSPI_DBG("%s diable\n", __func__); */
        WRITE_REG(pReg->MODE, 0);
        while (READ_REG(pReg->SR) & FSPI_SR_SR_BUSY) {
            HAL_CPUDelayUs(1);
            if (timeout++ > 1000) {
                return HAL_TIMEOUT;
            }
        }
#ifdef RKMCU_RK2118
        WRITE_REG(CRU->SOFTRST_CON[38], 0x05000500);
        HAL_CPUDelayUs(1);
        WRITE_REG(CRU->SOFTRST_CON[38], 0x05000000);
        HAL_CPUDelayUs(1);
        HAL_FSPI_Init(host);
#ifdef FSPI_SLF_DQS_CTRL_OFFSET
        if (!(host->xmmcDev[0].ctrl & FSPI_CTRL0_DQS_MODE_MASK) && (host->xmmcDev[0].ctrl & FSPI_CTRL0_DTR_MODE_MASK)) {
            WRITE_REG(host->instance->SLF_DQS_CTRL, FSPI_SLF_DQS_CTRL_SLF_BLD_DQS_EN0_MASK | FSPI_SLF_DQS_CTRL_SLF_BLD_DQS_EN1_MASK);
        }
        HAL_FSPI_SetDelayLines(host, host->cell);
#endif
#endif
    }

    return HAL_OK;
}

/**
 * @brief  Set FSPI delay line.
 * @param  host: FSPI host.
 * @param  cells: delay line cells.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_SetDelayLines(struct HAL_FSPI_HOST *host, uint16_t cells)
{
    HAL_ASSERT(IS_FSPI_INSTANCE(host->instance));

    if (cells > HAL_FSPI_GetMaxDllCells(host)) {
        return HAL_INVAL;
    }

    if (host->cs == 0) {
        WRITE_REG(host->instance->DLL_CTRL0, 1 << FSPI_DLL_CTRL0_SCLK_SMP_SEL_SHIFT | cells);
    } else {
#ifdef RKMCU_RK2108
        WRITE_REG(host->instance->DLL_CTRL0, 1 << FSPI_DLL_CTRL0_SCLK_SMP_SEL_SHIFT | cells);
#else
        WRITE_REG(host->instance->DLL_CTRL1, 1 << FSPI_DLL_CTRL0_SCLK_SMP_SEL_SHIFT | cells);
#endif
    }

    return HAL_OK;
}

/**
 * @brief  Disable FSPI delay line function.
 * @param  host: FSPI host.
 * @return HAL_Status.
 */
HAL_Status HAL_FSPI_DLLDisable(struct HAL_FSPI_HOST *host)
{
    HAL_ASSERT(IS_FSPI_INSTANCE(host->instance));
    if (host->cs == 0) {
        CLEAR_REG(host->instance->DLL_CTRL0);
    } else {
#ifdef RKMCU_RK2108
        CLEAR_REG(host->instance->DLL_CTRL0);
#else
        CLEAR_REG(host->instance->DLL_CTRL1);
#endif
    }

    return HAL_OK;
}

/**
 * @brief  Get FSPI XMMC status.
 * @param  host: FSPI host.
 * @return uint32_t: XMMC status.
 */
uint32_t HAL_FSPI_GetXMMCStatus(struct HAL_FSPI_HOST *host)
{
    HAL_ASSERT(IS_FSPI_INSTANCE(host->instance));

    return (uint32_t)host->instance->XMMCSR;
}

/**
 * @brief  Get FSPI supported max io transmition size.
 * @param  host: FSPI host.
 * @return uint32_t: size in bytes.
 */
uint32_t HAL_FSPI_GetMaxIoSize(struct HAL_FSPI_HOST *host)
{
    HAL_ASSERT(IS_FSPI_INSTANCE(host->instance));

#if (FSPI_VER_VER > FSPI_VER_VER_3)

    return FSPI_MAX_IOSIZE_VER4;
#else

    return FSPI_MAX_IOSIZE_VER3;
#endif
}

/**
 * @brief  Get FSPI supported max delay line cess.
 * @param  host: FSPI host.
 * @return uint32_t: the numbers of cells.
 */
uint32_t HAL_FSPI_GetMaxDllCells(struct HAL_FSPI_HOST *host)
{
    HAL_ASSERT(IS_FSPI_INSTANCE(host->instance));

    return host->maxDllCells;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_FSPI_MODULE_ENABLED */
