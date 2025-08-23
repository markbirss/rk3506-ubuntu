/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_SAI_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup SAI
 *  @{
 */

/** @defgroup SAI_How_To_Use How To Use
 *  @{

 The SAI driver can be used as follows:

 @} */

/** @defgroup SAI_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
#define SAI_VERSION1 0x20200520
#define SAI_VERSION2 0x23083576
#define SAI_VERSION3 0x23112118

#define CLK_SHIFT_RATE_HZ_MAX 5

#define SAI_DMA_BURST_SIZE     (8)/* size * width: 8*4 = 32 bytes */
#define SAI_DMA_BURST_SIZE_MAX (16)

/* XCR Transmit / Receive Control Register */
#define SAI_XCR_CTL_MASK       HAL_BIT(23)
#define SAI_XCR_CTL_CHAINED    HAL_BIT(23)
#define SAI_XCR_CTL_STANDALONE 0
#define SAI_XCR_DSE_MASK       HAL_BIT(22)
#define SAI_XCR_DSE_FALLING    HAL_BIT(22)
#define SAI_XCR_DSE_RISING     0
#define SAI_XCR_LANE_MASK      HAL_GENMASK(21, 20)
#define SAI_XCR_LANE(x)        ((x - 1) << 20)
#define SAI_XCR_LANE_V(v)      ((((v) & SAI_XCR_LANE_MASK) >> 20) + 1)
#define SAI_XCR_SJM_MASK       HAL_BIT(19)
#define SAI_XCR_SJM_L          HAL_BIT(19)
#define SAI_XCR_SJM_R          0
#define SAI_XCR_FBM_MASK       HAL_BIT(18)
#define SAI_XCR_FBM_LSB        HAL_BIT(18)
#define SAI_XCR_FBM_MSB        0
#define SAI_XCR_SNB_MASK       HAL_GENMASK(17, 11)
#define SAI_XCR_SNB(x)         ((x - 1) << 11)
#define SAI_XCR_VDJ_MASK       HAL_BIT(10)
#define SAI_XCR_VDJ_L          HAL_BIT(10)
#define SAI_XCR_VDJ_R          0
#define SAI_XCR_SBW_MASK       HAL_GENMASK(9, 5)
#define SAI_XCR_SBW(x)         ((x - 1) << 5)
#define SAI_XCR_SBW_V(v)       ((((v) & SAI_XCR_SBW_MASK) >> 5) + 1)
#define SAI_XCR_VDW_MASK       HAL_GENMASK(4, 0)
#define SAI_XCR_VDW(x)         ((x - 1) << 0)

/* FSCR Frame Sync Control Register */
#define SAI_FSCR_EDGE_DUAL   HAL_BIT(SAI_FSCR_EDGE_SHIFT)
#define SAI_FSCR_EDGE_RISING 0
#define SAI_FSCR_FPW(x)      ((x - 1) << SAI_FSCR_FPW_SHIFT)
#define SAI_FSCR_FW(x)       ((x - 1) << SAI_FSCR_FW_SHIFT)
#define SAI_FSCR_FW_V(v)     ((((v) & SAI_FSCR_FW_MASK) >> SAI_FSCR_FW_SHIFT) + 1)

/* MONO_CR Mono Control Register */
#define SAI_MONOCR_RMONO_SLOT_SEL(x) ((x - 1) << SAI_MONOCR_RMONO_SLOT_SHIFT)
#define SAI_MONOCR_RMONO_EN          HAL_BIT(SAI_MONOCR_RMONO_SHIFT)
#define SAI_MONOCR_RMONO_DIS         0
#define SAI_MONOCR_TMONO_EN          HAL_BIT(SAI_MONOCR_TMONO_SHIFT)
#define SAI_MONOCR_TMONO_DIS         0

/* XFER Transfer Start Register */
#ifdef  SAI_XFER_TX_AUTO_SHIFT
#define SAI_XFER_TX_AUTO_EN  HAL_BIT(SAI_XFER_TX_AUTO_SHIFT)
#define SAI_XFER_TX_AUTO_DIS 0
#else
#define SAI_XFER_RX_IDLE HAL_BIT(SAI_XFER_RX_IDLE_SHIFT)
#define SAI_XFER_TX_IDLE HAL_BIT(SAI_XFER_TX_IDLE_SHIFT)
#define SAI_XFER_FS_IDLE HAL_BIT(SAI_XFER_FS_IDLE_SHIFT)
#endif
#define SAI_XFER_RDC_EN  HAL_BIT(SAI_XFER_RDC_SHIFT)
#define SAI_XFER_RDC_DIS 0
#define SAI_XFER_TDC_EN  HAL_BIT(SAI_XFER_TDC_SHIFT)
#define SAI_XFER_TDC_DIS 0
#define SAI_XFER_RXS_EN  HAL_BIT(SAI_XFER_RXS_SHIFT)
#define SAI_XFER_RXS_DIS 0
#define SAI_XFER_TXS_EN  HAL_BIT(SAI_XFER_TXS_SHIFT)
#define SAI_XFER_TXS_DIS 0
#define SAI_XFER_FSS_EN  HAL_BIT(SAI_XFER_FSS_SHIFT)
#define SAI_XFER_FSS_DIS 0
#define SAI_XFER_CLK_EN  HAL_BIT(SAI_XFER_CLK_EN_SHIFT)
#define SAI_XFER_CLK_DIS 0

/* CLR Clear Logic Register */
#ifdef  SAI_CLR_FCR_SHIFT
#define SAI_CLR_FCR_EN  HAL_BIT(SAI_CLR_FCR_SHIFT)
#define SAI_CLR_FCR_DIS 0
#endif
#define SAI_CLR_FSC HAL_BIT(SAI_CLR_FSC_SHIFT)
#define SAI_CLR_RXC HAL_BIT(SAI_CLR_RXC_SHIFT)
#define SAI_CLR_TXC HAL_BIT(SAI_CLR_TXC_SHIFT)

/* CKR Clock Generation Register */
#define SAI_CKR_MDIV(x)         ((x - 1) << SAI_CKR_MDIV_SHIFT)
#define SAI_CKR_MSS_SLAVE       HAL_BIT(SAI_CKR_MSS_SHIFT)
#define SAI_CKR_MSS_MASTER      0
#define SAI_CKR_MSS_IS_SLAVE(v) ((v) & SAI_CKR_MSS_SLAVE)
#define SAI_CKR_CKP_INVERTED    HAL_BIT(SAI_CKR_CKP_SHIFT)
#define SAI_CKR_CKP_NORMAL      0
#define SAI_CKR_FSP_INVERTED    HAL_BIT(SAI_CKR_FSP_SHIFT)
#define SAI_CKR_FSP_NORMAL      0

/* DMACR DMA Control Register */
#define SAI_DMACR_RDE(x)   ((x) << SAI_DMACR_RDE_SHIFT)
#define SAI_DMACR_RDL(x)   ((x - 1) << SAI_DMACR_RDL_SHIFT)
#define SAI_DMACR_RDL_V(v) ((((v) & SAI_DMACR_RDL_MASK) >> SAI_DMACR_RDL_SHIFT) + 1)
#define SAI_DMACR_TDE(x)   ((x) << SAI_DMACR_TDE_SHIFT)
#define SAI_DMACR_TDL(x)   ((x - 1) << SAI_DMACR_TDL_SHIFT)
#define SAI_DMACR_TDL_V(v) ((((v) & SAI_DMACR_TDL_MASK) >> SAI_DMACR_TDL_SHIFT) + 1)

/* INTCR Interrupt Ctrl Register */
#ifdef  SAI_INTCR_FSLOSTC_SHIFT
#define SAI_INTCR_FSLOSTC   HAL_BIT(SAI_INTCR_FSLOSTC_SHIFT)
#define SAI_INTCR_FSLOST(x) ((x) << SAI_INTCR_FSLOST_SHIFT)
#define SAI_INTCR_FSERRC    HAL_BIT(SAI_INTCR_FSERRC_SHIFT)
#define SAI_INTCR_FSERR(x)  ((x) << SAI_INTCR_FSERR_SHIFT)
#endif
#define SAI_INTCR_RXOIC    HAL_BIT(SAI_INTCR_RXOIC_SHIFT)
#define SAI_INTCR_RXOIE(x) ((x) << SAI_INTCR_RXOIE_SHIFT)
#define SAI_INTCR_TXUIC    HAL_BIT(SAI_INTCR_TXUIC_SHIFT)
#define SAI_INTCR_TXUIE(x) ((x) << SAI_INTCR_TXUIE_SHIFT)

/* INTSR Interrupt Status Register */
#ifdef  SAI_INTCR_FSLOSTC_SHIFT
#define SAI_INTSR_FSLOSTI_ACT HAL_BIT(SAI_INTSR_FSLOSTI_SHIFT)
#define SAI_INTSR_FSLOSTI_INA 0
#define SAI_INTSR_FSERRI_ACT  HAL_BIT(SAI_INTSR_FSERRI_SHIFT)
#define SAI_INTSR_FSERRI_INA  0
#endif
#define SAI_INTSR_RXOI_ACT HAL_BIT(SAI_INTSR_RXOI_SHIFT)
#define SAI_INTSR_RXOI_INA 0
#define SAI_INTSR_TXUI_ACT HAL_BIT(SAI_INTSR_TXUI_SHIFT)
#define SAI_INTSR_TXUI_INA 0

/* PATH_SEL Path Select Register */
#define SAI_PATH_SEL_SYNC_IN_SEL_SYNC_IN_PORT HAL_BIT(SAI_PATH_SEL_SYNC_IN_SEL_SHIFT)
#define SAI_PATH_SEL_SYNC_IN_SEL_PAD          0

/* FSXN Frame Sync xN Register */
#ifdef  SAI_FSXN_OFFSET
#define SAI_FSXN_FSXN1_SHIFT(x) ((x) << SAI_FSXN_FSXN1_SHIFT_SHIFT)
#define SAI_FSXN_FSXN0_SHIFT(x) ((x) << SAI_FSXN_FSXN0_SHIFT_SHIFT)
#define SAI_FSXN_FSXN1_FW(x)    ((x - 1) << SAI_FSXN_FSXN1_FW_SHIFT)
#define SAI_FSXN_FSXN1_EN       HAL_BIT(SAI_FSXN_FSXN1_EN_SHIFT)
#define SAI_FSXN_FSXN1_DIS      0
#define SAI_FSXN_FSXN0_FW(x)    ((x - 1) << SAI_FSXN_FSXN0_FW_SHIFT)
#define SAI_FSXN_FSXN0_EN       HAL_BIT(SAI_FSXN_FSXN0_EN_SHIFT)
#define SAI_FSXN_FSXN0_DIS      0
#endif

/* FS_TIMEOUT Frame Sync Lost Detect Register */
#ifdef  SAI_FS_TIMEOUT_OFFSET
#define SAI_FS_TIMEOUT_EN     HAL_BIT(SAI_FS_TIMEOUT_TIMEOUT_EN_SHIFT)
#define SAI_FS_TIMEOUT_DIS    0
#define SAI_FS_TIMEOUT_VAL(x) ((x) << SAI_FS_TIMEOUT_TIMEOUT_VALUE_SHIFT)
#endif

/* XSHIFT: Transfer / Receive Frame Sync Shift Register */
#define SAI_XSHIFT_SHIFT_RIGHT_MASK HAL_GENMASK(23, 0)
#define SAI_XSHIFT_SHIFT_RIGHT(x)   ((x) << 0)
#define SAI_XSHIFT_SHIFT_LEFT_MASK  HAL_GENMASK(25, 24)
#define SAI_XSHIFT_SHIFT_LEFT(x)    ((x) << 24)

/* XFIFOLR: Transfer / Receive FIFO Level Register */
#define SAI_FIFOLR_XFL3_SHIFT 18
#define SAI_FIFOLR_XFL3_MASK  HAL_GENMASK(23, 18)
#define SAI_FIFOLR_XFL2_SHIFT 12
#define SAI_FIFOLR_XFL2_MASK  HAL_GENMASK(17, 12)
#define SAI_FIFOLR_XFL1_SHIFT 6
#define SAI_FIFOLR_XFL1_MASK  HAL_GENMASK(11, 6)
#define SAI_FIFOLR_XFL0_SHIFT 0
#define SAI_FIFOLR_XFL0_MASK  HAL_GENMASK(5, 0)

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

static HAL_Status SAI_createFmt(struct HAL_SAI_DEV *sai, eAUDIO_fmtType fmt)
{
    uint32_t xcrMask = 0, xcrVal = 0, xsftMask = 0, xsftVal = 0;
    uint32_t fscrMask = 0, fscrVal = 0;
    HAL_Status ret = HAL_OK;

    switch (fmt) {
    case AUDIO_FMT_TDM_RIGHT_J:
        sai->isTdm = true;
    /* fallthrough */
    case AUDIO_FMT_RIGHT_J:
        xcrMask = SAI_XCR_VDJ_MASK | SAI_XCR_DSE_MASK;
        xcrVal = SAI_XCR_VDJ_R | SAI_XCR_DSE_RISING;
        xsftMask = SAI_XSHIFT_SHIFT_RIGHT_MASK;
        xsftVal = SAI_XSHIFT_SHIFT_RIGHT(0);
        fscrMask = SAI_FSCR_EDGE_MASK;
        fscrVal = sai->isTdm ? SAI_FSCR_EDGE_RISING : SAI_FSCR_EDGE_DUAL;
        sai->fpw = FPW_HALF_FRAME_WIDTH;
        break;
    case AUDIO_FMT_TDM_LEFT_J:
        sai->isTdm = true;
    /* fallthrough */
    case AUDIO_FMT_LEFT_J:
        xcrMask = SAI_XCR_VDJ_MASK | SAI_XCR_DSE_MASK;
        xcrVal = SAI_XCR_VDJ_L | SAI_XCR_DSE_RISING;
        xsftMask = SAI_XSHIFT_SHIFT_RIGHT_MASK;
        xsftVal = SAI_XSHIFT_SHIFT_RIGHT(0);
        fscrMask = SAI_FSCR_EDGE_MASK;
        fscrVal = sai->isTdm ? SAI_FSCR_EDGE_RISING : SAI_FSCR_EDGE_DUAL;
        sai->fpw = FPW_HALF_FRAME_WIDTH;
        break;
    case AUDIO_FMT_TDM_I2S:
        MODIFY_REG(sai->pReg->CKR, SAI_CKR_FSP_MASK, SAI_CKR_FSP_INVERTED);
        sai->isTdm = true;
    /* fallthrough */
    case AUDIO_FMT_I2S:
        xcrMask = SAI_XCR_VDJ_MASK | SAI_XCR_DSE_MASK;
        xcrVal = SAI_XCR_VDJ_L | SAI_XCR_DSE_FALLING;
        xsftMask = SAI_XSHIFT_SHIFT_RIGHT_MASK;
        xsftVal = SAI_XSHIFT_SHIFT_RIGHT(2);
        fscrMask = SAI_FSCR_EDGE_MASK;
        fscrVal = sai->isTdm ? SAI_FSCR_EDGE_RISING : SAI_FSCR_EDGE_DUAL;
        sai->fpw = FPW_HALF_FRAME_WIDTH;
        break;
    case AUDIO_FMT_TDM_I2S_INV:
        sai->isTdm = true;
    /* fallthrough */
    case AUDIO_FMT_I2S_INV:
        xcrMask = SAI_XCR_VDJ_MASK | SAI_XCR_DSE_MASK;
        xcrVal = SAI_XCR_VDJ_L | SAI_XCR_DSE_RISING;
        xsftMask = SAI_XSHIFT_SHIFT_RIGHT_MASK;
        xsftVal = SAI_XSHIFT_SHIFT_RIGHT(2);
        fscrMask = SAI_FSCR_EDGE_MASK;
        fscrVal = sai->isTdm ? SAI_FSCR_EDGE_RISING : SAI_FSCR_EDGE_DUAL;
        sai->fpw = FPW_HALF_FRAME_WIDTH;
        break;
    case AUDIO_FMT_TDM_DSP_A:
        sai->isTdm = true;
    /* fallthrough */
    case AUDIO_FMT_DSP_A:
        xcrMask = SAI_XCR_VDJ_MASK | SAI_XCR_DSE_MASK;
        xcrVal = SAI_XCR_VDJ_L | SAI_XCR_DSE_RISING;
        xsftMask = SAI_XSHIFT_SHIFT_RIGHT_MASK;
        xsftVal = SAI_XSHIFT_SHIFT_RIGHT(2);
        fscrMask = SAI_FSCR_EDGE_MASK;
        fscrVal = SAI_FSCR_EDGE_RISING;
        sai->fpw = FPW_ONE_BCLK_WIDTH;
        break;
    case AUDIO_FMT_TDM_DSP_B:
        sai->isTdm = true;
    /* fallthrough */
    case AUDIO_FMT_DSP_B:
        xcrMask = SAI_XCR_VDJ_MASK | SAI_XCR_DSE_MASK;
        xcrVal = SAI_XCR_VDJ_L | SAI_XCR_DSE_RISING;
        xsftMask = SAI_XSHIFT_SHIFT_RIGHT_MASK;
        xsftVal = SAI_XSHIFT_SHIFT_RIGHT(0);
        fscrMask = SAI_FSCR_EDGE_MASK;
        fscrVal = SAI_FSCR_EDGE_RISING;
        sai->fpw = FPW_ONE_BCLK_WIDTH;
        break;
    default:
        HAL_DBG_ERR("%s: Unsupported fmt %u\n", __func__, fmt);
        ret = HAL_INVAL;
        break;
    }

    MODIFY_REG(sai->pReg->TXCR, xcrMask, xcrVal);
    MODIFY_REG(sai->pReg->RXCR, xcrMask, xcrVal);
    MODIFY_REG(sai->pReg->TX_TIMING_SHIFT, xsftMask, xsftVal);
    MODIFY_REG(sai->pReg->RX_TIMING_SHIFT, xsftMask, xsftVal);
    MODIFY_REG(sai->pReg->FSCR, fscrMask, fscrVal);

    return ret;
}

static uint8_t SAI_lanesAuto(struct HAL_SAI_DEV *sai, uint16_t channels)
{
    uint8_t lanes = 1;

    if (!sai->isTdm) {
        lanes = HAL_DIV_ROUND_UP(channels, 2);
    }

    return lanes;
}

static HAL_Status SAI_SetBclkDivAuto(struct HAL_SAI_DEV *sai, uint32_t slotWidth,
                                     uint32_t chPerLane, uint32_t sampleRate)
{
#if defined(HAL_CRU_MODULE_ENABLED)
    HAL_ASSERT(sai != NULL);

    uint32_t mclkRate, mclkReqRate, bclkRate, divBclk;

    if (SAI_CKR_MSS_IS_SLAVE(READ_REG(sai->pReg->CKR))) {
        return HAL_OK;
    }

    bclkRate = sai->fwRatio * slotWidth * chPerLane * sampleRate;
    if (sai->isClkAuto) {
        HAL_CRU_ClkSetFreq(sai->mclk, bclkRate);
    }

    mclkRate = HAL_CRU_ClkGetFreq(sai->mclk);
    if (mclkRate < bclkRate) {
        HAL_DBG_ERR("sai-%p: %s: Mismatch mclk: %" PRIu32 " Hz, at least %" PRIu32 " Hz\n",
                    sai->pReg, __func__, mclkRate, bclkRate);

        return HAL_INVAL;
    }

    divBclk = HAL_DivRoundClosest(mclkRate, bclkRate);
    mclkReqRate = bclkRate * divBclk;

    if (mclkRate < mclkReqRate - CLK_SHIFT_RATE_HZ_MAX ||
        mclkRate > mclkReqRate + CLK_SHIFT_RATE_HZ_MAX) {
        HAL_DBG_ERR("sai-%p: %s: Mismatch mclk: %" PRIu32 ", expected %" PRIu32 " (+/- %dHz)\n",
                    sai->pReg, __func__, mclkRate, mclkReqRate, CLK_SHIFT_RATE_HZ_MAX);

        return HAL_INVAL;
    }

    HAL_SAI_SetBclkDiv(sai->pReg, divBclk);
#endif

    return HAL_OK;
}

static void SAI_FCR(struct SAI_REG *pReg)
{
#ifdef SAI_CLR_FCR_SHIFT
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    /*
     * DELAY between FCR assert and deassert require 2 SCLK cycle.
     * it is safe to use a 3 SCLK cycle delay (max 3.9us, 1/256k sclk @8k).
     *
     * the 3 SCLK is 3 * 3.9 = 12 us, So, we use udelay(15).
     */
    WRITE_REG(pReg->CLR, SAI_CLR_FCR_EN | SAI_CLR_FSC | SAI_CLR_RXC | SAI_CLR_TXC);
    HAL_DelayUs(15);
    WRITE_REG(pReg->CLR, SAI_CLR_FCR_DIS);
    HAL_DelayUs(15);
#endif
}

static void SAI_ForceClear(struct SAI_REG *pReg, uint32_t clr)
{
#ifdef  SAI_CLR_FCR_SHIFT
    uint32_t dmacr, xfer;

    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    /*
     * The max clr done time is 1 frame@SR, so, the max time
     * is 125us (1/8kHz), using udelay(150us) is enough.
     */
    HAL_DelayUs(150);
    if (READ_REG(pReg->CLR) & clr) {
        dmacr = READ_REG(pReg->DMACR);
        xfer = READ_REG(pReg->XFER);
        HAL_SAI_DisableFIFOXrunDetect(pReg, AUDIO_STREAM_PLAYBACK);
        HAL_SAI_DisableFIFOXrunDetect(pReg, AUDIO_STREAM_CAPTURE);
        MODIFY_REG(pReg->DMACR, SAI_DMACR_RDE_MASK | SAI_DMACR_TDE_MASK,
                   SAI_DMACR_RDE(0) | SAI_DMACR_TDE(0));
        MODIFY_REG(pReg->XFER, SAI_XFER_RXS_MASK | SAI_XFER_TXS_MASK,
                   SAI_XFER_RXS_DIS | SAI_XFER_TXS_DIS);
        /*
         * There is a concurrency case in FCR routine:
         *
         * when the prepare CLR done and the FCR req at the same SCLK cycle,
         * the controller miss the CLR req of FCR which will break the
         * water level logic (always zero), leading the next XFER FIFO xrun.
         *
         * So, need one more FCR to gurantee the force clear success.
         *
         * a, the first one may failed because of concurrency.
         * b, the second one success.
         *
         * DELAY between FCR assert and deassert require 2 SCLK cycle.
         * it is safe to use a 3 SCLK cycle delay (max 3.9us, 1/256k sclk @8k).
         *
         * the 3 SCLK is 3 * 3.9 = 12 us, So, we use udelay(15).
         */
        MODIFY_REG(pReg->CLR,
                   SAI_CLR_FCR_MASK | SAI_CLR_RXC_MASK | SAI_CLR_TXC_MASK,
                   SAI_CLR_FCR_EN | SAI_CLR_RXC | SAI_CLR_TXC);
        HAL_DelayUs(15);
        MODIFY_REG(pReg->CLR, SAI_CLR_FCR_MASK, SAI_CLR_FCR_DIS);
        HAL_DelayUs(15);
        /* The second FCR */
        MODIFY_REG(pReg->CLR,
                   SAI_CLR_FCR_MASK | SAI_CLR_RXC_MASK | SAI_CLR_TXC_MASK,
                   SAI_CLR_FCR_EN | SAI_CLR_RXC | SAI_CLR_TXC);
        HAL_DelayUs(15);
        MODIFY_REG(pReg->CLR, SAI_CLR_FCR_MASK, SAI_CLR_FCR_DIS);
        HAL_DelayUs(15);

        WRITE_REG(pReg->DMACR, dmacr);
        WRITE_REG(pReg->XFER, xfer);
    }
#endif
}

/* only used in irq handler */
static void SAI_ClearFsErr(struct SAI_REG *pReg)
{
#ifdef SAI_INTCR_FSERR_SHIFT
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->INTCR, SAI_INTCR_FSERRC_MASK, SAI_INTCR_FSERRC);
    MODIFY_REG(pReg->INTCR, SAI_INTCR_FSERR_MASK, SAI_INTCR_FSERR(0));
    HAL_SAI_DisableFIFOXrunDetect(pReg, AUDIO_STREAM_PLAYBACK);
    HAL_SAI_DisableFIFOXrunDetect(pReg, AUDIO_STREAM_CAPTURE);
    MODIFY_REG(pReg->DMACR, SAI_DMACR_RDE_MASK | SAI_DMACR_TDE_MASK,
               SAI_DMACR_RDE(0) | SAI_DMACR_TDE(0));
    MODIFY_REG(pReg->XFER, SAI_XFER_RXS_MASK | SAI_XFER_TXS_MASK,
               SAI_XFER_RXS_DIS | SAI_XFER_TXS_DIS);
    SAI_FCR(pReg);
#endif
}

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup SAI_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 ...to do or delete this row

 *  @{
 */

/** @} */

/** @defgroup SAI_Exported_Functions_Group2 State and Errors Functions

 This section provides functions allowing to get the status of the module:

 *  @{
 */

/** @} */

/** @defgroup SAI_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

/** @} */

/** @defgroup SAI_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 ...to do or delete this row

 *  @{
 */

/**
 * @brief  Init sai controller.
 * @param  sai: the handle of sai.
 * @param  config: init config for sai init.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DevInit(struct HAL_SAI_DEV *sai, struct AUDIO_INIT_CONFIG *config)
{
    uint32_t mask = 0, val = 0;
    HAL_Status ret = HAL_OK;

    HAL_ASSERT(sai != NULL);
    HAL_ASSERT(config != NULL);

    sai->fwRatio = sai->fwRatio ? sai->fwRatio : 1;

    mask = SAI_CKR_MSS_MASK;
    val = config->master ? SAI_CKR_MSS_MASTER : SAI_CKR_MSS_SLAVE;
    MODIFY_REG(sai->pReg->CKR, mask, val);

    mask = SAI_CKR_CKP_MASK;
    val = config->clkInvert ? SAI_CKR_CKP_INVERTED : SAI_CKR_CKP_NORMAL;
    MODIFY_REG(sai->pReg->CKR, mask, val);

    MODIFY_REG(sai->pReg->DMACR, SAI_DMACR_TDL_MASK, SAI_DMACR_TDL(SAI_DMA_BURST_SIZE));
    MODIFY_REG(sai->pReg->DMACR, SAI_DMACR_RDL_MASK, SAI_DMACR_RDL(SAI_DMA_BURST_SIZE));

    ret = SAI_createFmt(sai, config->format);

    HAL_SAI_SetFsxn0FrameWidth(sai->pReg, 64, 1);
    HAL_SAI_SetFsxn1FrameWidth(sai->pReg, 64, 1);

    return ret;
}

/**
 * @brief  DeInit sai controller.
 * @param  sai: the handle of sai.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DevDeInit(struct HAL_SAI_DEV *sai)
{
    HAL_ASSERT(sai != NULL);

    MODIFY_REG(sai->pReg->XFER, SAI_XFER_CLK_EN_MASK | SAI_XFER_FSS_MASK,
               SAI_XFER_CLK_DIS | SAI_XFER_FSS_DIS);

    return HAL_OK;
}

/** @} */

/** @defgroup SAI_Exported_Functions_Group5 Other Functions
 *  @{
 */

/** @defgroup SAI_Low_Level_Functions Low Level Functions
 *  @{
 */

/**
 * @brief  Enable sai dma.
 * @param  pReg: the handle of SAI_REG.
 * @param  stream: AUDIO_STREAM_PLAYBACK or AUDIO_STREAM_CAPTURE.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_EnableDMA(struct SAI_REG *pReg, int stream)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    if (stream == AUDIO_STREAM_PLAYBACK) {
        MODIFY_REG(pReg->DMACR, SAI_DMACR_TDE_MASK, SAI_DMACR_TDE(1));
    } else {
        MODIFY_REG(pReg->DMACR, SAI_DMACR_RDE_MASK, SAI_DMACR_RDE(1));
    }

    return HAL_OK;
}

/**
 * @brief  Disable sai dma.
 * @param  pReg: the handle of SAI_REG.
 * @param  stream: AUDIO_STREAM_PLAYBACK or AUDIO_STREAM_CAPTURE.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DisableDMA(struct SAI_REG *pReg, int stream)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    if (stream == AUDIO_STREAM_PLAYBACK) {
        MODIFY_REG(pReg->DMACR, SAI_DMACR_TDE_MASK, SAI_DMACR_TDE(0));
    } else {
        MODIFY_REG(pReg->DMACR, SAI_DMACR_RDE_MASK, SAI_DMACR_RDE(0));
    }

    return HAL_OK;
}

/**
 * @brief  Enable sai tx.
 * @param  pReg: the handle of SAI_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_EnableTX(struct SAI_REG *pReg)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    HAL_SAI_EnableFsLostDetect(pReg);

    MODIFY_REG(pReg->DMACR, SAI_DMACR_TDE_MASK, SAI_DMACR_TDE(1));
    MODIFY_REG(pReg->XFER, SAI_XFER_TXS_MASK, SAI_XFER_TXS_EN);

    HAL_SAI_EnableFIFOXrunDetect(pReg, AUDIO_STREAM_PLAYBACK);

    return HAL_OK;
}

/**
 * @brief  Enable sai rx.
 * @param  pReg: the handle of SAI_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_EnableRX(struct SAI_REG *pReg)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    HAL_SAI_EnableFsLostDetect(pReg);

    MODIFY_REG(pReg->DMACR, SAI_DMACR_RDE_MASK, SAI_DMACR_RDE(1));
    MODIFY_REG(pReg->XFER, SAI_XFER_RXS_MASK, SAI_XFER_RXS_EN);

    HAL_SAI_EnableFIFOXrunDetect(pReg, AUDIO_STREAM_CAPTURE);

    return HAL_OK;
}

/**
 * @brief  Disable sai tx.
 * @param  pReg: the handle of SAI_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DisableTX(struct SAI_REG *pReg)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    HAL_SAI_DisableFIFOXrunDetect(pReg, AUDIO_STREAM_PLAYBACK);
    HAL_SAI_DisableFsLostDetect(pReg);

    MODIFY_REG(pReg->DMACR, SAI_DMACR_TDE_MASK, SAI_DMACR_TDE(0));
    MODIFY_REG(pReg->XFER, SAI_XFER_TXS_MASK, SAI_XFER_TXS_DIS);
    HAL_DelayUs(150);
    MODIFY_REG(pReg->CLR, SAI_CLR_TXC_MASK, SAI_CLR_TXC);
    SAI_ForceClear(pReg, SAI_CLR_TXC);

    return HAL_OK;
}

/**
 * @brief  Disable sai rx.
 * @param  pReg: the handle of SAI_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DisableRX(struct SAI_REG *pReg)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    HAL_SAI_DisableFIFOXrunDetect(pReg, AUDIO_STREAM_CAPTURE);
    HAL_SAI_DisableFsLostDetect(pReg);

    MODIFY_REG(pReg->DMACR, SAI_DMACR_RDE_MASK, SAI_DMACR_RDE(0));
    MODIFY_REG(pReg->XFER, SAI_XFER_RXS_MASK, SAI_XFER_RXS_DIS);
    HAL_DelayUs(150);
    MODIFY_REG(pReg->CLR, SAI_CLR_RXC_MASK, SAI_CLR_RXC);
    SAI_ForceClear(pReg, SAI_CLR_RXC);

    return HAL_OK;
}

/**
 * @brief  Set sai bclk div.
 * @param  pReg: the handle of SAI_REG.
 * @param  div: the ratio of mclk / bclk.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_SetBclkDiv(struct SAI_REG *pReg, int div)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->CKR, SAI_CKR_MDIV_MASK, SAI_CKR_MDIV(div));

    return HAL_OK;
}

/**
 * @brief  Set sai fsxn0 clk.
 * @param  pReg: the handle of SAI_REG.
 * @param  frameWidth: the frame width of fsxn clk.
 * @param  shiftFrame: shift to the left by shiftFrame releative to frame sync edge.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_SetFsxn0FrameWidth(struct SAI_REG *pReg, int frameWidth, int shiftFrame)
{
#ifdef SAI_FSXN_OFFSET
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->FSXN, SAI_FSXN_FSXN0_FW_MASK, SAI_FSXN_FSXN0_FW(frameWidth));
    MODIFY_REG(pReg->FSXN, SAI_FSXN_FSXN0_SHIFT_MASK, SAI_FSXN_FSXN0_SHIFT(shiftFrame));
#endif

    return HAL_OK;
}

/**
 * @brief  Set sai fsxn1 clk.
 * @param  pReg: the handle of SAI_REG.
 * @param  frameWidth: the frame width of fsxn clk.
 * @param  shiftFrame: shift to the left by shiftFrame releative to frame sync edge.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_SetFsxn1FrameWidth(struct SAI_REG *pReg, int frameWidth, int shiftFrame)
{
#ifdef SAI_FSXN_OFFSET
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->FSXN, SAI_FSXN_FSXN1_FW_MASK, SAI_FSXN_FSXN1_FW(frameWidth));
    MODIFY_REG(pReg->FSXN, SAI_FSXN_FSXN1_SHIFT_MASK, SAI_FSXN_FSXN1_SHIFT(shiftFrame));
#endif

    return HAL_OK;
}

/**
 * @brief  Enable FSXN0 clk
 * @param  pReg: the handle of SAI_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_EnableFsxn0(struct SAI_REG *pReg)
{
#ifdef SAI_FSXN_OFFSET
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->FSXN, SAI_FSXN_FSXN0_EN_MASK, SAI_FSXN_FSXN0_EN);
#endif

    return HAL_OK;
}

/**
 * @brief  Enable FSXN1 clk
 * @param  pReg: the handle of SAI_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_EnableFsxn1(struct SAI_REG *pReg)
{
#ifdef SAI_FSXN_OFFSET
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->FSXN, SAI_FSXN_FSXN1_EN_MASK, SAI_FSXN_FSXN1_EN);
#endif

    return HAL_OK;
}

/**
 * @brief  Disable FSXN0 clk
 * @param  pReg: the handle of SAI_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DisableFsxn0(struct SAI_REG *pReg)
{
#ifdef SAI_FSXN_OFFSET
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->FSXN, SAI_FSXN_FSXN0_EN_MASK, SAI_FSXN_FSXN0_DIS);
#endif

    return HAL_OK;
}

/**
 * @brief  Disable FSXN1 clk
 * @param  pReg: the handle of SAI_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DisableFsxn1(struct SAI_REG *pReg)
{
#ifdef SAI_FSXN_OFFSET
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->FSXN, SAI_FSXN_FSXN1_EN_MASK, SAI_FSXN_FSXN1_DIS);
#endif

    return HAL_OK;
}

/**
 * @brief  Set Fsync Lost Detect Count(nFS).
 * @param  pReg: the handle of SAI_REG.
 * @param  frameCount: the count of Fsync clk (at least 2).
 * @return HAL_Status
 */
HAL_Status HAL_SAI_SetFsLostDetectCount(struct SAI_REG *pReg, uint32_t frameCount)
{
#ifdef SAI_FS_TIMEOUT_OFFSET
    uint32_t fw;

    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    if (frameCount < 2) {
        frameCount = 2;
    }
    /*
     * The count is drived by SCLK from CRU, not external SCLK.
     * So, suggest to set SCLK freq equal to external SCLK
     * in SLAVE mode for better count caculation.
     */
    fw = SAI_FSCR_FW_V(READ_REG(pReg->FSCR));
    MODIFY_REG(pReg->FS_TIMEOUT, SAI_FS_TIMEOUT_TIMEOUT_VALUE_MASK,
               SAI_FS_TIMEOUT_VAL(frameCount * fw));
#endif

    return HAL_OK;
}

/**
 * @brief  Enable Fsync Lost Detect
 * @param  pReg: the handle of SAI_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_EnableFsLostDetect(struct SAI_REG *pReg)
{
#ifdef SAI_FS_TIMEOUT_OFFSET
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    /* work for slave only */
    if (!SAI_CKR_MSS_IS_SLAVE(READ_REG(pReg->CKR))) {
        return HAL_OK;
    }

    MODIFY_REG(pReg->INTCR, SAI_INTCR_FSERRC_MASK, SAI_INTCR_FSERRC);
    MODIFY_REG(pReg->INTCR, SAI_INTCR_FSERR_MASK, SAI_INTCR_FSERR(1));
    MODIFY_REG(pReg->INTCR, SAI_INTCR_FSLOSTC_MASK, SAI_INTCR_FSLOSTC);
    MODIFY_REG(pReg->INTCR, SAI_INTCR_FSLOST_MASK, SAI_INTCR_FSLOST(1));
    MODIFY_REG(pReg->FS_TIMEOUT, SAI_FS_TIMEOUT_TIMEOUT_EN_MASK, SAI_FS_TIMEOUT_EN);
#endif

    return HAL_OK;
}

/**
 * @brief  Disable Fsync Lost Detect
 * @param  pReg: the handle of SAI_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DisableFsLostDetect(struct SAI_REG *pReg)
{
#ifdef SAI_FS_TIMEOUT_OFFSET
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->INTCR, SAI_INTCR_FSERRC_MASK, SAI_INTCR_FSERRC);
    MODIFY_REG(pReg->INTCR, SAI_INTCR_FSERR_MASK, SAI_INTCR_FSERR(0));
    MODIFY_REG(pReg->INTCR, SAI_INTCR_FSLOSTC_MASK, SAI_INTCR_FSLOSTC);
    MODIFY_REG(pReg->INTCR, SAI_INTCR_FSLOST_MASK, SAI_INTCR_FSLOST(0));
    MODIFY_REG(pReg->FS_TIMEOUT, SAI_FS_TIMEOUT_TIMEOUT_EN_MASK, SAI_FS_TIMEOUT_DIS);
#endif

    return HAL_OK;
}

/**
 * @brief  Enable FIFO XRUN Detect
 * @param  pReg: the handle of SAI_REG.
 * @param  stream: AUDIO_STREAM_PLAYBACK or AUDIO_STREAM_CAPTURE.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_EnableFIFOXrunDetect(struct SAI_REG *pReg, int stream)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    if (stream == AUDIO_STREAM_PLAYBACK) {
        MODIFY_REG(pReg->INTCR, SAI_INTCR_TXUIC_MASK, SAI_INTCR_TXUIC);
        MODIFY_REG(pReg->INTCR, SAI_INTCR_TXUIE_MASK, SAI_INTCR_TXUIE(1));
    } else {
        MODIFY_REG(pReg->INTCR, SAI_INTCR_RXOIC_MASK, SAI_INTCR_RXOIC);
        MODIFY_REG(pReg->INTCR, SAI_INTCR_RXOIE_MASK, SAI_INTCR_RXOIE(1));
    }

    return HAL_OK;
}

/**
 * @brief  Disable FIFO XRUN Detect
 * @param  pReg: the handle of SAI_REG.
 * @param  stream: AUDIO_STREAM_PLAYBACK or AUDIO_STREAM_CAPTURE.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DisableFIFOXrunDetect(struct SAI_REG *pReg, int stream)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    if (stream == AUDIO_STREAM_PLAYBACK) {
        MODIFY_REG(pReg->INTCR, SAI_INTCR_TXUIC_MASK, SAI_INTCR_TXUIC);
        MODIFY_REG(pReg->INTCR, SAI_INTCR_TXUIE_MASK, SAI_INTCR_TXUIE(0));
    } else {
        MODIFY_REG(pReg->INTCR, SAI_INTCR_RXOIC_MASK, SAI_INTCR_RXOIC);
        MODIFY_REG(pReg->INTCR, SAI_INTCR_RXOIE_MASK, SAI_INTCR_RXOIE(0));
    }

    return HAL_OK;
}

/**
 * @brief  Set tx slot mask
 * @param  pReg: the handle of SAI_REG.
 * @param  mask: 32-bits mask for 32 slot, each bit 1:masked, 0:active.
 * @return HAL_Status
 * Note    Should set before EnableTX.
 */
HAL_Status HAL_SAI_SetTxSlotMask(struct SAI_REG *pReg, uint32_t mask)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->TX_SLOT_MASK[0], 0xffffffff, mask);

    return HAL_OK;
}

/**
 * @brief  Get tx slot mask
 * @param  pReg: the handle of SAI_REG.
 * @return mask
 */
uint32_t HAL_SAI_GetTxSlotMask(struct SAI_REG *pReg)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    return READ_REG(pReg->TX_SLOT_MASK[0]);
}

/**
 * @brief  Set rx slot mask
 * @param  pReg: the handle of SAI_REG.
 * @param  mask: 32-bits mask for 32 slot, each bit 1:masked, 0:active.
 * @return HAL_Status
 * Note    Should set before EnableRX.
 */
HAL_Status HAL_SAI_SetRxSlotMask(struct SAI_REG *pReg, uint32_t mask)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->RX_SLOT_MASK[0], 0xffffffff, mask);

    return HAL_OK;
}

/**
 * @brief  Get rx slot mask
 * @param  pReg: the handle of SAI_REG.
 * @return mask
 */
uint32_t HAL_SAI_GetRxSlotMask(struct SAI_REG *pReg)
{
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    return READ_REG(pReg->RX_SLOT_MASK[0]);
}

/**
 * @brief  Set sai chained mode, such as sai0 + sai1
 * @param  pReg: the handle of SAI_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_SetChainedMode(struct SAI_REG *pReg)
{
#ifdef SAI_TXCR_CTL_SHIFT
    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    MODIFY_REG(pReg->PATH_SEL, SAI_PATH_SEL_SYNC_IN_SEL_MASK,
               SAI_PATH_SEL_SYNC_IN_SEL_SYNC_IN_PORT);
    MODIFY_REG(pReg->TXCR, SAI_XCR_CTL_MASK, SAI_XCR_CTL_CHAINED);
    MODIFY_REG(pReg->RXCR, SAI_XCR_CTL_MASK, SAI_XCR_CTL_CHAINED);
#endif

    return HAL_OK;
}

/**
 * @brief  sai clear irq
 * @param  pReg: the handle of SAI_REG.
 * @return raw irq status
 */
uint32_t HAL_SAI_ClearIrq(struct SAI_REG *pReg)
{
    uint32_t irqStatus;

    HAL_ASSERT(IS_SAI_INSTANCE(pReg));

    irqStatus = READ_REG(pReg->INTSR);

#ifdef SAI_INTCR_FSLOSTC_SHIFT
    if (irqStatus & SAI_INTSR_FSLOSTI_ACT) {
        MODIFY_REG(pReg->INTCR, SAI_INTCR_FSLOSTC_MASK, SAI_INTCR_FSLOSTC);
    }
    if (irqStatus & SAI_INTSR_FSERRI_ACT) {
        MODIFY_REG(pReg->INTCR, SAI_INTCR_FSERRC_MASK, SAI_INTCR_FSERRC);
    }
#endif
    if (irqStatus & SAI_INTSR_TXUI_ACT) {
        MODIFY_REG(pReg->INTCR, SAI_INTCR_TXUIC_MASK, SAI_INTCR_TXUIC);
    }
    if (irqStatus & SAI_INTSR_RXOI_ACT) {
        MODIFY_REG(pReg->INTCR, SAI_INTCR_RXOIC_MASK, SAI_INTCR_RXOIC);
    }

    return irqStatus;
}

/**
 * @brief  set tx timing shift
 * @param  pReg: the handle of SAI_REG.
 * @param  shift: postive: right shift, negtive: left shift (half cycle step)
 * @return HAL_Status
 * Note    Should set before EnableTX.
 */
HAL_Status HAL_SAI_SetTxTimingShift(struct SAI_REG *pReg, int shift)
{
    uint32_t xsftMask = 0, xsftVal = 0;

    HAL_ASSERT(IS_SAI_INSTANCE(pReg));
    HAL_ASSERT(shift >= -2);

    if (shift >= 0) {
        xsftMask = SAI_XSHIFT_SHIFT_RIGHT_MASK;
        xsftVal = SAI_XSHIFT_SHIFT_RIGHT(shift);
    } else {
        xsftMask = SAI_XSHIFT_SHIFT_LEFT_MASK;
        xsftVal = SAI_XSHIFT_SHIFT_LEFT(-shift);
    }

    MODIFY_REG(pReg->TX_TIMING_SHIFT, xsftMask, xsftVal);

    return HAL_OK;
}

/**
 * @brief  set rx timing shift
 * @param  pReg: the handle of SAI_REG.
 * @param  shift: postive: right shift (half cycle step)
 * @return HAL_Status
 * Note    Should set before EnableRX.
 */
HAL_Status HAL_SAI_SetRxTimingShift(struct SAI_REG *pReg, int shift)
{
    uint32_t xsftMask = 0, xsftVal = 0;

    HAL_ASSERT(IS_SAI_INSTANCE(pReg));
    HAL_ASSERT(shift >= 0);

    xsftMask = SAI_XSHIFT_SHIFT_RIGHT_MASK;
    xsftVal = SAI_XSHIFT_SHIFT_RIGHT(shift);

    MODIFY_REG(pReg->RX_TIMING_SHIFT, xsftMask, xsftVal);

    return HAL_OK;
}

/** @} */

/** @defgroup SAI_Dev_Level_Functions Dev Level Functions
 *  @{
 */

/**
 * @brief  Enable sai stream.
 * @param  sai: the handle of sai.
 * @param  stream: AUDIO_STREAM_PLAYBACK or AUDIO_STREAM_CAPTURE.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DevEnable(struct HAL_SAI_DEV *sai, eAUDIO_streamType stream)
{
    HAL_ASSERT(sai != NULL);

    if (stream == AUDIO_STREAM_PLAYBACK) {
        HAL_SAI_EnableTX(sai->pReg);
    } else {
        HAL_SAI_EnableRX(sai->pReg);
    }

    return HAL_OK;
}

/**
 * @brief  Disable sai stream.
 * @param  sai: the handle of sai.
 * @param  stream: AUDIO_STREAM_PLAYBACK or AUDIO_STREAM_CAPTURE.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DevDisable(struct HAL_SAI_DEV *sai, eAUDIO_streamType stream)
{
    HAL_ASSERT(sai != NULL);

    if (stream == AUDIO_STREAM_PLAYBACK) {
        HAL_SAI_DisableTX(sai->pReg);
    } else {
        HAL_SAI_DisableRX(sai->pReg);
    }

    return HAL_OK;
}

static HAL_Status HAL_SAI_SetWaterLevel(struct HAL_SAI_DEV *sai, eAUDIO_streamType stream,
                                        uint8_t lanes, struct AUDIO_PARAMS *params)
{
#ifdef HAL_AUDIO_LOW_LATENCY
    uint32_t frameWord, width;
    uint8_t dl = SAI_DMA_BURST_SIZE;

    HAL_ASSERT(sai != NULL);
    HAL_ASSERT(params != NULL);
    HAL_ASSERT(lanes != 0);

    /*
     * One frame latency, e.g. 20us@48kHz situation
     * note: dma burst len should use one frame at the same time.
     */
    width = HAL_AUDIO_GetPhysicalWidth(params->sampleBits);
    frameWord = width * params->channels / 32;
    if (!frameWord) {
        HAL_DBG_ERR("SAI-%p: %s: Invalid frameWord: %d\n",
                    sai->pReg, __func__, frameWord);

        return HAL_INVAL;
    }

    /*
     * 16bits: 1 word L/R interleaved per lanes
     * 32bits: 2 word L/R interleaved per lanes
     *
     * Normalization:
     * 16bits: 1 / 1 = 1
     * 32bits: 2 / 2 = 1
     */
    if (width == 32) {
        frameWord /= 2;
    }

    dl = HAL_DIV_ROUND_UP(frameWord, lanes);

    /*
     * Align the real WL for all lanes case
     *
     * Currently, 1 Lane case use all FIFOs, the real WL is expanded maxLanes times.
     * so, here to align latency for all cases.
     */
    if (sai->maxLanes && lanes == 1) {
        dl = HAL_DIV_ROUND_UP(dl, sai->maxLanes);
    }

    /* Inverse-normalization */
    if (width == 32) {
        dl *= 2;
    }

    /*
     * WA for 1 lanes rx, need one more to cover it, because REQ triggered by
     * FIFO0 WL, but use all FIFOs space. there is a misalignment.
     */
    if (lanes == 1 && stream == AUDIO_STREAM_CAPTURE && params->channels > 2) {
        dl++;
    }

    dl = dl > SAI_DMA_BURST_SIZE_MAX ? SAI_DMA_BURST_SIZE_MAX : dl;

    if (stream == AUDIO_STREAM_PLAYBACK) {
        MODIFY_REG(sai->pReg->DMACR, SAI_DMACR_TDL_MASK, SAI_DMACR_TDL(dl));
    } else {
        MODIFY_REG(sai->pReg->DMACR, SAI_DMACR_RDL_MASK, SAI_DMACR_RDL(dl));
    }

#endif

    return HAL_OK;
}

/**
 * @brief  Config sai controller.
 * @param  sai: the handle of sai.
 * @param  stream: AUDIO_STREAM_PLAYBACK or AUDIO_STREAM_CAPTURE.
 * @param  params: audio params.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DevConfig(struct HAL_SAI_DEV *sai, eAUDIO_streamType stream,
                             struct AUDIO_PARAMS *params)
{
    uint32_t val, slotWidth, chPerLane, slotPerLane, fscr;
    uint8_t lanes;
    HAL_Status ret = HAL_OK;

    HAL_ASSERT(sai != NULL);
    HAL_ASSERT(params != NULL);

    val = SAI_XCR_VDW(params->sampleBits);
    lanes = SAI_lanesAuto(sai, params->channels);

    if (stream == AUDIO_STREAM_PLAYBACK) {
        slotWidth = SAI_XCR_SBW_V(READ_REG(sai->pReg->TXCR));
        lanes = sai->txLanes ? sai->txLanes : lanes;
        chPerLane = params->channels / lanes;
        slotPerLane = params->slots / lanes;

        if (slotPerLane > chPerLane) {
            HAL_SAI_SetTxSlotMask(sai->pReg, HAL_GENMASK(slotPerLane - 1, chPerLane));
            chPerLane = slotPerLane;
        }

        val |= SAI_XCR_LANE(lanes);
        val |= SAI_XCR_SNB(chPerLane);

        MODIFY_REG(sai->pReg->TXCR, SAI_XCR_VDW_MASK | SAI_XCR_LANE_MASK |
                   SAI_XCR_SNB_MASK, val);
    } else {
        slotWidth = SAI_XCR_SBW_V(READ_REG(sai->pReg->RXCR));
        lanes = sai->rxLanes ? sai->rxLanes : lanes;
        chPerLane = params->channels / lanes;
        slotPerLane = params->slots / lanes;

        if (slotPerLane > chPerLane) {
            HAL_SAI_SetRxSlotMask(sai->pReg, HAL_GENMASK(slotPerLane - 1, chPerLane));
            chPerLane = slotPerLane;
        }

        val |= SAI_XCR_LANE(lanes);
        val |= SAI_XCR_SNB(chPerLane);

        MODIFY_REG(sai->pReg->RXCR, SAI_XCR_VDW_MASK | SAI_XCR_LANE_MASK |
                   SAI_XCR_SNB_MASK, val);
    }

    HAL_SAI_SetWaterLevel(sai, stream, lanes, params);

    fscr = SAI_FSCR_FW(sai->fwRatio * slotWidth * chPerLane);

    switch (sai->fpw) {
    case FPW_ONE_BCLK_WIDTH:
        fscr |= SAI_FSCR_FPW(1);
        break;
    case FPW_ONE_SLOT_WIDTH:
        fscr |= SAI_FSCR_FPW(slotWidth);
        break;
    case FPW_HALF_FRAME_WIDTH:
        fscr |= SAI_FSCR_FPW(sai->fwRatio * slotWidth * chPerLane / 2);
        break;
    default:
        HAL_DBG_ERR("%s: Invalid Frame Pulse Width %d\n", __func__, sai->fpw);

        return HAL_INVAL;
    }

    MODIFY_REG(sai->pReg->FSCR, SAI_FSCR_FW_MASK | SAI_FSCR_FPW_MASK, fscr);

    ret = SAI_SetBclkDivAuto(sai, slotWidth, chPerLane, params->sampleRate);
    if (ret) {
        return ret;
    }

    /*
     * Should wait for one BCLK ready after DIV and then ungate
     * output clk to achieve the clean clk.
     *
     * The best way is to use delay per samplerate, but, the max time
     * is quite a tiny value, so, let's make it simple to use the max
     * time.
     *
     * The max BCLK cycle time is: 15.6us @ 8K-8Bit (64K BCLK)
     */
    HAL_DelayUs(20);
    MODIFY_REG(sai->pReg->XFER, SAI_XFER_CLK_EN_MASK | SAI_XFER_FSS_MASK,
               SAI_XFER_CLK_EN | SAI_XFER_FSS_EN);

    HAL_SAI_DevSetFsLostDetectCount(sai, 2, slotWidth, chPerLane, params->sampleRate);

    return ret;
}

/**
 * @brief  Set sai irq callback.
 * @param  sai: the handle of sai.
 * @param  pCB: function callback.
 * @param  pCBParam: the param for function callback.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DevRegisterCallback(struct HAL_SAI_DEV *sai, RK_SAI_CALLBACK pCB, void *pCBParam)
{
    HAL_ASSERT(sai != NULL);

    sai->pCallback = pCB;
    sai->pCBParam = pCBParam;

    return HAL_OK;
}

/**
 * @brief  Set sai irq callback2.
 * @param  sai: the handle of sai.
 * @param  pCB2: second function callback.
 * @param  pCBParam2: the param for second function callback.
 * @return HAL_Status
 * Note    optional, if need to seperate TX/RX for easy use.
 */
HAL_Status HAL_SAI_DevRegisterCallback2(struct HAL_SAI_DEV *sai, RK_SAI_CALLBACK pCB2, void *pCBParam2)
{
    HAL_ASSERT(sai != NULL);

    sai->pCallback2 = pCB2;
    sai->pCBParam2 = pCBParam2;

    return HAL_OK;
}

/**
 * @brief  sai irq handler
 * @param  sai: the handle of sai.
 * @return raw irq status
 */
HAL_Status HAL_SAI_DevIrqHandler(struct HAL_SAI_DEV *sai)
{
    uint32_t event;

    HAL_ASSERT(sai != NULL);

    event = HAL_SAI_ClearIrq(sai->pReg);

    if (event & SAI_TX_FIFO_UNDERRUN) {
        HAL_SAI_DisableTX(sai->pReg);
        HAL_DBG_ERR("SAI-%p: TX FIFO UNDERRUN\n", sai->pReg);
    }

    if (event & SAI_RX_FIFO_OVERRUN) {
        HAL_SAI_DisableRX(sai->pReg);
        HAL_DBG_ERR("SAI-%p: RX FIFO OVERRUN\n", sai->pReg);
    }

    if (event & SAI_FS_CLK_ERR) {
        SAI_ClearFsErr(sai->pReg);
    }

    if (sai->pCallback) {
        sai->pCallback(sai->pCBParam, event);
    }

    if (sai->pCallback2) {
        sai->pCallback2(sai->pCBParam2, event);
    }

    return HAL_OK;
}

/**
 * @brief  Dev Set Fsync Lost Detect Count(nFS).
 * @param  sai: the handle of sai.
 * @param  frameCount: the count of Fsync clk (at least 2).
 * @param  slotWidth:  slotWidth.
 * @param  chPerLane:  chPerLane.
 * @param  sampleRate: sampleRate.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DevSetFsLostDetectCount(struct HAL_SAI_DEV *sai, uint32_t frameCount,
                                           uint32_t slotWidth, uint32_t chPerLane,
                                           uint32_t sampleRate)
{
#if defined(SAI_FS_TIMEOUT_OFFSET) && defined(HAL_CRU_MODULE_ENABLED)
    uint32_t mclkRate, bclkRate, div;

    HAL_ASSERT(sai != NULL);

    if (!SAI_CKR_MSS_IS_SLAVE(READ_REG(sai->pReg->CKR))) {
        return HAL_OK;
    }

    /*
     * The count is drived by SCLK from CRU, not external SCLK.
     * So, suggest to set SCLK freq equal to external SCLK
     * in SLAVE mode for better counter caculation.
     *
     * Unfortunally, the various req SCLK is not always available.
     * So, we set the SCLK a fixed rate which is multiple of the
     * req SCLK for easy counter caculation.
     */
    if (sampleRate % 8000 == 0 || sampleRate % 12000 == 0) {
        mclkRate = 49152000;
    } else if (sampleRate % 11025 == 0) {
        mclkRate = 45158400;
    } else {
        HAL_DBG_ERR("SAI-%p: %s: Invalid samplerate: %" PRIu32 "\n",
                    sai->pReg, __func__, sampleRate);

        return HAL_INVAL;
    }

    bclkRate = sai->fwRatio * slotWidth * chPerLane * sampleRate;
    div = HAL_DivRoundClosest(mclkRate, bclkRate);
    frameCount *= div;

    HAL_CRU_ClkSetFreq(sai->mclk, mclkRate);
    HAL_SAI_SetBclkDiv(sai->pReg, 1);

    HAL_SAI_SetFsLostDetectCount(sai->pReg, frameCount);
#endif

    return HAL_OK;
}

/**
 * @brief  DeInterLeavedFrame for sai data buf.
 * @param  in: input buf with data interleaved by lanes.
 * @param  out: out buf with order.
 * @param  frames: total frames.
 * @param  channels: total channels.
 * @param  lanes: sai lanes used.
 * @param  width: data bit width.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_DeInterLeavedFrame(void *in, void *out, int frames, int channels, int lanes, int width)
{
    uint32_t chPerChunk = 2, chPerLane = 0, chunkPerLane = 0;
    uint32_t chunkBytes, stridBytes, frameBytes;
    uint32_t ofs, ifs, lfs, i, j, f;

    HAL_ASSERT(channels % 2 == 0);
    HAL_ASSERT(width >= 16);
    HAL_ASSERT(lanes > 0);
    HAL_ASSERT(frames > 0);
    HAL_ASSERT(in != NULL);
    HAL_ASSERT(out != NULL);

    chPerLane = channels / lanes;

    HAL_ASSERT(chPerLane % 2 == 0);

    chunkPerLane = chPerLane / 2;

    frameBytes = AUDIO_FRAMES_TO_BYTES(1, channels, width);
    chunkBytes = AUDIO_FRAMES_TO_BYTES(1, chPerChunk, width);
    stridBytes = chunkBytes * lanes;

    for (f = 0; f < frames; f++) {
        for (i = 0; i < lanes; i++) {
            lfs = i * chunkBytes;
            for (j = 0; j < chunkPerLane; j++) {
                ofs = ((i * chunkPerLane + j) * chunkBytes) % frameBytes;
                ifs = (lfs + j * stridBytes) % frameBytes;
                /* frames offset */
                ofs += frameBytes * f;
                ifs += frameBytes * f;
                memcpy(out + ofs, in + ifs, chunkBytes);
            }
        }
    }

    return HAL_OK;
}

/**
 * @brief  Get the interleaved channel offset (1 frame).
 * @param  ch: origin ch offset in a frame.
 * @param  channels: total channels.
 * @param  lanes: sai lanes used.
 * @return interleaved ch offset
 */
int HAL_SAI_GetInterLeavedChOffset(int ch, int channels, int lanes)
{
    uint32_t chunkCh = 2, stridCh = 0, chPerLane = 0;
    int s, sm, c, cm;

    HAL_ASSERT(channels % 2 == 0);
    HAL_ASSERT(lanes > 0);
    HAL_ASSERT(ch >= 0 && ch < channels);

    stridCh = chunkCh * lanes;
    chPerLane = (channels + stridCh - 1) / stridCh;
    chPerLane *= chunkCh;

    HAL_ASSERT(chPerLane % 2 == 0);

    s = ch / stridCh;
    sm = ch % stridCh;

    c = sm / chunkCh;
    cm = sm % chunkCh;

    return (c * chPerLane + s * chunkCh + cm);
}

/**
 * @brief  Get the de-interleaved channel offset (1 frame).
 * @param  ch: origin ch offset in a frame.
 * @param  channels: total channels.
 * @param  lanes: sai lanes used.
 * @return deinterleaved ch offset
 */
int HAL_SAI_GetDeInterLeavedChOffset(int ch, int channels, int lanes)
{
    uint32_t chunkCh = 2, stridCh = 0, chPerLane = 0;
    int s, sm, c, cm;

    HAL_ASSERT(channels % 2 == 0);
    HAL_ASSERT(lanes > 0);
    HAL_ASSERT(ch >= 0 && ch < channels);

    stridCh = chunkCh * lanes;
    chPerLane = (channels + stridCh - 1) / stridCh;
    chPerLane *= chunkCh;

    HAL_ASSERT(chPerLane % 2 == 0);

    s = ch / chPerLane;
    sm = ch % chPerLane;

    c = sm / chunkCh;
    cm = sm % chunkCh;

    return (c * stridCh + s * chunkCh + cm);
}

/**
 * @brief  Get the interleaved channel offset table (1 frame).
 * @param  table: output interleaved channel offset table.
 * @param  channels: total channels.
 * @param  lanes: sai lanes used.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_GetInterLeavedChTable(int *table, int channels, int lanes)
{
    int i;

    for (i = 0; i < channels; i++) {
        table[i] = HAL_SAI_GetInterLeavedChOffset(i, channels, lanes);
    }

    return HAL_OK;
}

/**
 * @brief  Get the de-interleaved channel offset table (1 frame).
 * @param  table: output deinterleaved channel offset table.
 * @param  channels: total channels.
 * @param  lanes: sai lanes used.
 * @return HAL_Status
 */
HAL_Status HAL_SAI_GetDeInterLeavedChTable(int *table, int channels, int lanes)
{
    int i;

    for (i = 0; i < channels; i++) {
        table[i] = HAL_SAI_GetDeInterLeavedChOffset(i, channels, lanes);
    }

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

/** @} */

#endif /* HAL_SAI_MODULE_ENABLED */
