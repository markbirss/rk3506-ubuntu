/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_ASRC_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup ASRC
 *  @{
 */

/** @defgroup ASRC_How_To_Use How To Use
 *  @{

 The ASRC driver can be used as follows:

 The ASRC is asynchronous sampling rate converter. It is used to connect audio
 devices with different clock sources and frequencies.
 - Invoke HAL_ASRC_Init() to init ASRC.
 - Invoke HAL_ASRC_Config() to setup ASRC audio parameters.
 - Invoke HAL_ASRC_SelectSeriesMode() to set ASRC as master or slave mode.
 - Invoke ASRC hard or soft mute function to mute asrc lane 0-3.
 - Invoke HAL_ASRC_Start() to start ASRC.
 - Invoke HAL_ASRC_Stop() to stop ASRC.
 - Invoke HAL_ASRC_DeInit() to clse ASRC device.

 @} */

/** @defgroup ASRC_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#ifdef HAL_ASRC_DEBUG
#define HAL_ASRC_DBG(...) HAL_DBG(__VA_ARGS__)
#else
#define HAL_ASRC_DBG(...) do { if (0) { HAL_DBG(__VA_ARGS__); } } while (0)
#endif

/* CON */
#define ASRC_EN           (0x1 << ASRC_CON_EN_SHIFT)
#define ASRC_DIS          (0x0 << ASRC_CON_EN_SHIFT)
#define ASRC_OUT_STOP     (1 << ASRC_CON_OUT_STOP_SHIFT)
#define ASRC_OUT_START    (0 << ASRC_CON_OUT_STOP_SHIFT)
#define ASRC_IN_STOP      (1 << ASRC_CON_IN_STOP_SHIFT)
#define ASRC_IN_START     (0 << ASRC_CON_IN_STOP_SHIFT)
#define ASRC_REAL_TIME    (0x0 << ASRC_CON_MODE_SHIFT)
#define ASRC_MEMORY_FETCH (0x1 << ASRC_CON_MODE_SHIFT)
#define ASRC_M2M          (0x0 << ASRC_CON_REAL_TIME_MODE_SHIFT)
#define ASRC_S2M          (0x1 << ASRC_CON_REAL_TIME_MODE_SHIFT)
#define ASRC_M2D          (0x2 << ASRC_CON_REAL_TIME_MODE_SHIFT)
#define ASRC_S2D          (0x3 << ASRC_CON_REAL_TIME_MODE_SHIFT)
#define ASRC_CHAN_NUM(x)  (((x - 2) / 2) << ASRC_CON_CHAN_NUM_SHIFT)
#define ASRC_SERIES(x)    ((x) << ASRC_CON_SERIES_EN_SHIFT)

/* CLKDIV_CON */
#define ASRC_CLKDIV_CON_SRC_LRCK_DIV(x) ((x - 1) << ASRC_CLKDIV_CON_SRC_LRCK_DIV_CON_SHIFT)
#define ASRC_CLKDIV_CON_DST_LRCK_DIV(x) ((x - 1) << ASRC_CLKDIV_CON_DST_LRCK_DIV_CON_SHIFT)

/* INT_CON */
#define ASRC_CONV_DONE_EN (0x1 << ASRC_INT_CON_CONV_DONE_EN_SHIFT)

/* DATA_FMT */
#define ASRC_OWL_16BIT        (0x2 << ASRC_DATA_FMT_OWL_SHIFT)
#define ASRC_OWL_20BIT        (0x1 << ASRC_DATA_FMT_OWL_SHIFT)
#define ASRC_OWL_24BIT        (0x0 << ASRC_DATA_FMT_OWL_SHIFT)
#define ASRC_IWL_16BIT        (0x2 << ASRC_DATA_FMT_IWL_SHIFT)
#define ASRC_IWL_20BIT        (0x1 << ASRC_DATA_FMT_IWL_SHIFT)
#define ASRC_IWL_24BIT        (0x0 << ASRC_DATA_FMT_IWL_SHIFT)
#define ASRC_DATA_FMT_IFMT_16 (0x1 << ASRC_DATA_FMT_IFMT_SHIFT)
#define ASRC_DATA_FMT_IFMT_32 (0x0 << ASRC_DATA_FMT_IFMT_SHIFT)
#define ASRC_DATA_FMT_OFMT_16 (0x1 << ASRC_DATA_FMT_OFMT_SHIFT)
#define ASRC_DATA_FMT_OFMT_32 (0x0 << ASRC_DATA_FMT_OFMT_SHIFT)
#define ASRC_DATA_FMT_ISJM(x) ((x) << ASRC_DATA_FMT_ISJM_SHIFT)
#define ASRC_DATA_FMT_OSJM(x) ((x) << ASRC_DATA_FMT_OSJM_SHIFT)

/* DMA_THRESH */
#define ASRC_DMA_TX_THRESH(x) (x << ASRC_DMA_THRESH_DMA_TX_THRESH_SHIFT)
#define ASRC_DMA_RX_THRESH(x) (x << ASRC_DMA_THRESH_DMA_RX_THRESH_SHIFT)
#define ASRC_IN_THRESH(x)     (x << ASRC_DMA_THRESH_ASRC_IN_THRESH_SHIFT)
#define ASRC_OUT_THRESH(x)    (x << ASRC_DMA_THRESH_ASRC_OUT_THRESH_SHIFT)
#define ASRC_NEG_THRESH(x)    (x << ASRC_DMA_THRESH_ASRC_NEG_THRESH_SHIFT)
#define ASRC_POS_THRESH(x)    (x << ASRC_DMA_THRESH_ASRC_POS_THRESH_SHIFT)

/* TRACK_PERIOD */
#define ASRC_RATIO_TRACK_DIV(x)    (x << ASRC_TRACK_PERIOD_RATIO_TRACK_DIV_SHIFT)
#define ASRC_RATIO_TRACK_PERIOD(x) (x << ASRC_TRACK_PERIOD_RATIO_TRACK_PERIOD_SHIFT)

#define ASRC_SAMPLE_FREQ    200000000
#define ASRC_REF_LRCK_COUNT 0x48000

/********************* Private Structure Definition **************************/
/********************* Private Variable Definition ***************************/
#if defined(HAL_ASRC_DEBUG) && defined(RKMCU_RK2118)
static const char * const s_grp0LrckName[] = {
    "ASRC0", "ASRC1", "ASRC2", "ASRC3", "SPDIFRX0", "PDM0",
    "SAI0", "SAI1", "SAI2", "SAI3", "SAI4",
};

static const char * const s_grp1LrckName[] = {
    "ASRC4", "ASRC5", "ASRC6", "ASRC7", "SPDIFTX0", "SPDIFRX1",
    "SAI4", "SAI5", "SAI6", "SAI7", "SAI0",
};

#endif
/********************* Private Function Definition ***************************/

static bool HAL_ASRC_IsLrckFromAsrcMclk(eASRC_lrckSel lrckMux)
{
    return (lrckMux == LRCK_SEL_MCLK_ASRC0 ||
            lrckMux == LRCK_SEL_MCLK_ASRC1 ||
            lrckMux == LRCK_SEL_MCLK_ASRC2 ||
            lrckMux == LRCK_SEL_MCLK_ASRC3 ||
            lrckMux == LRCK_SEL_MCLK_ASRC4 ||
            lrckMux == LRCK_SEL_MCLK_ASRC5 ||
            lrckMux == LRCK_SEL_MCLK_ASRC6 ||
            lrckMux == LRCK_SEL_MCLK_ASRC7);
}

static HAL_Status HAL_ASRC_RefineLrckDiv(struct HAL_ASRC_DEV *asrc, struct ASRC_PARAMS *params,
                                         uint32_t *lrckDiv)
{
#ifdef HAL_CRU_MODULE_ENABLED
    uint32_t mclkRate, mclkReqRate;

    HAL_ASSERT(asrc != NULL);
    HAL_ASSERT(params != NULL);
    HAL_ASSERT(lrckDiv != NULL);
    HAL_ASSERT(params->sampleRate != 0);

    if (!HAL_ASRC_IsLrckFromAsrcMclk(params->lrckMux)) {
        return HAL_OK;
    }

    if (params->sampleRate % 8000 == 0 || params->sampleRate % 12000 == 0) {
        mclkReqRate = 49152000;
    } else if (params->sampleRate % 11025 == 0) {
        mclkReqRate = 45158400;
    } else {
        HAL_DBG_ERR("asrc-%p: %s: Invalid samplerate: %d\n",
                    asrc->pReg, __func__, params->sampleRate);

        return HAL_INVAL;
    }

    HAL_CRU_ClkSetFreq(asrc->mclk, mclkReqRate);
    mclkRate = HAL_CRU_ClkGetFreq(asrc->mclk);
    if (mclkRate != mclkReqRate) {
        HAL_DBG_ERR("asrc-%p: %s: Mismatch mclk: %" PRIu32 " Hz, expected %" PRIu32 " Hz\n",
                    asrc->pReg, __func__, mclkRate, mclkReqRate);

        return HAL_INVAL;
    }

    if (mclkRate % params->sampleRate) {
        HAL_DBG_ERR("asrc-%p: %s: Mismatch mclk: %" PRIu32 " Hz for samplerate: %d\n",
                    asrc->pReg, __func__, mclkRate, params->sampleRate);

        return HAL_INVAL;
    }

    *lrckDiv = mclkRate / params->sampleRate;
#endif

    return HAL_OK;
}

static HAL_Status HAL_ASRC_ConfigLrck(struct HAL_ASRC_DEV *asrc, struct ASRC_PARAMS *rxParams,
                                      struct ASRC_PARAMS *txParams)
{
#ifdef HAL_CRU_MODULE_ENABLED
    HAL_Status ret;
    uint32_t lrckDiv;

    HAL_ASSERT(asrc != NULL);
    HAL_ASSERT(rxParams != NULL);
    HAL_ASSERT(txParams != NULL);

    lrckDiv = txParams->lrckDiv ? txParams->lrckDiv : 1;
    ret = HAL_ASRC_RefineLrckDiv(asrc, txParams, &lrckDiv);
    if (ret) {
        return ret;
    }

    MODIFY_REG(asrc->pReg->CLKDIV_CON,
               ASRC_CLKDIV_CON_DST_LRCK_DIV_EN_MASK |
               ASRC_CLKDIV_CON_DST_LRCK_DIV_CON_MASK,
               ASRC_CLKDIV_CON_DST_LRCK_DIV_EN_MASK |
               ASRC_CLKDIV_CON_DST_LRCK_DIV(lrckDiv));

    HAL_CRU_ClkSetMux(CLK_GET_MUX(asrc->lrckTX), txParams->lrckMux);

    lrckDiv = rxParams->lrckDiv ? rxParams->lrckDiv : 1;
    ret = HAL_ASRC_RefineLrckDiv(asrc, rxParams, &lrckDiv);
    if (ret) {
        return ret;
    }

    MODIFY_REG(asrc->pReg->CLKDIV_CON,
               ASRC_CLKDIV_CON_SRC_LRCK_DIV_EN_MASK |
               ASRC_CLKDIV_CON_SRC_LRCK_DIV_CON_MASK,
               ASRC_CLKDIV_CON_SRC_LRCK_DIV_EN_MASK |
               ASRC_CLKDIV_CON_SRC_LRCK_DIV(lrckDiv));

    HAL_CRU_ClkSetMux(CLK_GET_MUX(asrc->lrckRX), rxParams->lrckMux);
#endif

    return HAL_OK;
}

static HAL_Status HAL_ASRC_ConfigPeriod(struct HAL_ASRC_DEV *asrc, struct ASRC_PARAMS *rxParams,
                                        struct ASRC_PARAMS *txParams)
{
    int div = 0, period = 0;

    HAL_ASSERT(asrc != NULL);

    MODIFY_REG(asrc->pReg->CON, ASRC_CON_RATIO_TRACK_MODE_MASK, ASRC_CON_RATIO_TRACK_MODE_MASK);

    div = 0x3;
    period = 1024;

    MODIFY_REG(asrc->pReg->TRACK_PERIOD,
               ASRC_TRACK_PERIOD_RATIO_TRACK_DIV_MASK | ASRC_TRACK_PERIOD_RATIO_TRACK_PERIOD_MASK,
               ASRC_RATIO_TRACK_DIV(div) | ASRC_RATIO_TRACK_PERIOD(period));

    return HAL_OK;
}

static int HAL_ASRC_CalculateRatio(struct HAL_ASRC_DEV *asrc,
                                   int numerator, int denominator)
{
    int i, integerPart, remainder, ratio, digit;
    unsigned int temp = 0;

    HAL_ASSERT(asrc != NULL);
    HAL_ASSERT(denominator != 0);

    integerPart = numerator / denominator;
    remainder = numerator % denominator;
    ratio = integerPart << 22;

    for (i = 0; i < 8; i++) {
        remainder <<= 4;
        digit = remainder / denominator;
        temp |= (digit << (28 - i * 4));
        remainder %= denominator;
    }

    ratio += (temp >> 10);

    return ratio;
}

static void HAL_ASRC_ConfigRatio(struct HAL_ASRC_DEV *asrc, eAUDIO_sampleRate rxRate,
                                 eAUDIO_sampleRate txRate)
{
    int ratio;

    HAL_ASSERT(asrc != NULL);

    ratio = HAL_ASRC_CalculateRatio(asrc, rxRate, txRate);

    WRITE_REG(asrc->pReg->SAMPLE_RATE, rxRate);
    WRITE_REG(asrc->pReg->RESAMPLE_RATE, txRate);
    WRITE_REG(asrc->pReg->MANUAL_RATIO, ratio);
}

static void HAL_ASRC_DumpConfig(struct HAL_ASRC_DEV *asrc, struct ASRC_PARAMS *rxParams,
                                struct ASRC_PARAMS *txParams)
{
#if defined(HAL_ASRC_DEBUG) && defined(RKMCU_RK2118)
    HAL_ASSERT(asrc != NULL);
    HAL_ASSERT(rxParams != NULL);
    HAL_ASSERT(txParams != NULL);

    const char * *lrckName = asrc->pReg <= ASRC3 ? s_grp0LrckName : s_grp1LrckName;

    HAL_DBG("asrc-%p: ch: %u, mode: %d, glink: %d\n",
            asrc->pReg, asrc->channels, asrc->mode, asrc->groupLink);
    HAL_DBG("asrc-%p: rx: rate: %u, bits: %u, lrck: %s, div: %" PRIu32 "\n",
            asrc->pReg, rxParams->sampleRate, rxParams->sampleBits,
            lrckName[rxParams->lrckMux], rxParams->lrckDiv);
    HAL_DBG("asrc-%p: tx: rate: %u, bits: %u, lrck: %s, div: %" PRIu32 "\n",
            asrc->pReg, txParams->sampleRate, txParams->sampleBits,
            lrckName[txParams->lrckMux], txParams->lrckDiv);
#endif
}

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup ASRC_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 ...to do or delete this row

 *  @{
 */

/** @} */

/** @defgroup ASRC_Exported_Functions_Group2 State and Errors Functions

 This section provides functions allowing to get the status of the module:

 *  @{
 */

/** @} */

/** @defgroup ASRC_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

/** @} */

/** @defgroup ASRC_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 ...to do or delete this row

 *  @{
 */

/**
 * @brief  Get ASRC Owner, such as core dsp0
 * @param  asrc: the handle of asrc.
 * @return owner id, 0 is invalid, which means it is idle.
 */
uint32_t HAL_ASRC_GetOwner(struct HAL_ASRC_DEV *asrc)
{
#ifdef HAL_SPINLOCK_MODULE_ENABLED
    if (asrc->spinLockId) {
        return HAL_SPINLOCK_GetOwner(asrc->spinLockId);
    }
#endif

    return 0;
}

/**
 * @brief  Try to lock the asrc.
 * @param  asrc: the handle of asrc.
 * @return HAL_Check
 */
HAL_Check HAL_ASRC_TryLock(struct HAL_ASRC_DEV *asrc)
{
#ifdef HAL_SPINLOCK_MODULE_ENABLED
    if (asrc->spinLockId) {
        if (HAL_ASRC_GetOwner(asrc)) {
            return HAL_FALSE;
        }

        return HAL_SPINLOCK_TryLock(asrc->spinLockId);
    }
#endif

    return HAL_TRUE;
}

/**
 * @brief  Unlock the asrc.
 * @param  asrc: the handle of asrc.
 */
void HAL_ASRC_UnLock(struct HAL_ASRC_DEV *asrc)
{
#ifdef HAL_SPINLOCK_MODULE_ENABLED
    if (asrc->spinLockId) {
        HAL_SPINLOCK_Unlock(asrc->spinLockId);
    }
#endif
}

/**
 * @brief  Init ASRC controller.
 * @param  asrc: the handle of asrc.
 * @param  config: init config for asrc init.
 * @return HAL_Status
 */
HAL_Status HAL_ASRC_Init(struct HAL_ASRC_DEV *asrc, struct AUDIO_INIT_CONFIG *config)
{
    uint32_t val = 0, mask = 0;

    HAL_ASSERT(asrc != NULL);
    HAL_ASSERT(config != NULL);

#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkEnable(asrc->mclk);
#endif

    mask = ASRC_CON_MODE_MASK | ASRC_CON_REAL_TIME_MODE_MASK;

    if (config->asrcMode == ASRC_FILE_MODE) {
        val |= ASRC_MEMORY_FETCH;
        MODIFY_REG(asrc->pReg->FETCH_LEN,
                   ASRC_FETCH_LEN_FETCH_LENGTH_MASK, asrc->fetchLength);
    } else {
        switch (config->asrcMode) {
        case ASRC_REAL_TIME_M2M_MODE:
            val = ASRC_M2M | ASRC_REAL_TIME;
            break;
        case ASRC_REAL_TIME_S2M_MODE:
            val = ASRC_S2M | ASRC_REAL_TIME;
            break;
        case ASRC_REAL_TIME_M2D_MODE:
            val = ASRC_M2D | ASRC_REAL_TIME;
            break;
        case ASRC_REAL_TIME_S2D_MODE:
            val = ASRC_S2D | ASRC_REAL_TIME;
            break;
        default:

            return HAL_INVAL;
        }
    }

    MODIFY_REG(asrc->pReg->CON, mask, val);

    mask = ASRC_DMA_THRESH_DMA_TX_THRESH_MASK |
           ASRC_DMA_THRESH_DMA_RX_THRESH_MASK |
           ASRC_DMA_THRESH_ASRC_IN_THRESH_MASK |
           ASRC_DMA_THRESH_ASRC_OUT_THRESH_MASK |
           ASRC_DMA_THRESH_ASRC_NEG_THRESH_MASK |
           ASRC_DMA_THRESH_ASRC_POS_THRESH_MASK;

    val = ASRC_DMA_TX_THRESH(0x1) |
          ASRC_DMA_RX_THRESH(0x1) |
          ASRC_IN_THRESH(0x1) |
          ASRC_OUT_THRESH(0x17) |
          ASRC_NEG_THRESH(0x10) |
          ASRC_POS_THRESH(0x2f);

    MODIFY_REG(asrc->pReg->DMA_THRESH, mask, val);
    asrc->mode = config->asrcMode;

    return HAL_OK;
}

/**
 * @brief  DeInit ASRC controller.
 * @param  asrc: the handle of asrc.
 * @return HAL_Status
 */
HAL_Status HAL_ASRC_DeInit(struct HAL_ASRC_DEV *asrc)
{
    HAL_ASSERT(asrc != NULL);

#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkDisable(asrc->mclk);
#endif
    asrc->channels = 0;
    asrc->groupLink = false;

    return HAL_OK;
}

/** @} */

/** @defgroup ASRC_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Start ASRC controller.
 * @param  asrc: the handle of asrc.
 * @return HAL_Status
 */
HAL_Status HAL_ASRC_Start(struct HAL_ASRC_DEV *asrc)
{
    HAL_ASSERT(asrc != NULL);
    MODIFY_REG(asrc->pReg->CON,
               ASRC_CON_IN_STOP_MASK | ASRC_CON_OUT_STOP_MASK,
               ASRC_IN_START | ASRC_OUT_START);
    MODIFY_REG(asrc->pReg->CON, ASRC_CON_EN_MASK, ASRC_EN);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_CONV_ERROR_EN_MASK,
               ASRC_INT_CON_CONV_ERROR_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_SRC_LRCK_UNLOCK_EN_MASK,
               ASRC_INT_CON_SRC_LRCK_UNLOCK_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_DST_LRCK_UNLOCK_EN_MASK,
               ASRC_INT_CON_DST_LRCK_UNLOCK_EN_MASK);
#ifdef ASRC_DEBUG
    /* Enable all interrupts for debug. */
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_OUT_START_EN_MASK, ASRC_INT_CON_OUT_START_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_CONV_DONE_EN_MASK, ASRC_CONV_DONE_EN);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_RATIO_INIT_DONE_EN_MASK, ASRC_INT_CON_RATIO_INIT_DONE_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_RATIO_CHANGE_DONE_EN_MASK, ASRC_INT_CON_RATIO_CHANGE_DONE_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_RATIO_UPDATE_DONE_EN_MASK, ASRC_INT_CON_RATIO_UPDATE_DONE_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_FIFO_IN_FULL_EN_MASK, ASRC_INT_CON_FIFO_IN_FULL_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_FIFO_IN_EMPTY_EN_MASK, ASRC_INT_CON_FIFO_IN_EMPTY_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_FIFO_OUT_FULL_EN_MASK, ASRC_INT_CON_FIFO_OUT_FULL_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_FIFO_OUT_EMPTY_EN_MASK, ASRC_INT_CON_FIFO_OUT_EMPTY_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_RATIO_UNLOCK_EN_MASK, ASRC_INT_CON_RATIO_UNLOCK_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_SRC_LRCK_UNLOCK_EN_MASK, ASRC_INT_CON_SRC_LRCK_UNLOCK_EN_MASK);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_DST_LRCK_UNLOCK_EN_MASK, ASRC_INT_CON_DST_LRCK_UNLOCK_EN_MASK);
#endif

    return HAL_OK;
}

/**
 * @brief  Disable ASRC controller.
 * @param  asrc: the handle of asrc.
 * @return HAL_Status
 */
HAL_Status HAL_ASRC_Stop(struct HAL_ASRC_DEV *asrc)
{
    HAL_ASSERT(asrc != NULL);

    MODIFY_REG(asrc->pReg->CON,
               ASRC_CON_IN_STOP_MASK | ASRC_CON_OUT_STOP_MASK,
               ASRC_IN_STOP | ASRC_OUT_STOP);
    MODIFY_REG(asrc->pReg->CON, ASRC_CON_EN_MASK, ASRC_DIS);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_CONV_ERROR_EN_MASK, 0);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_SRC_LRCK_UNLOCK_EN_MASK, 0);
    MODIFY_REG(asrc->pReg->INT_CON, ASRC_INT_CON_DST_LRCK_UNLOCK_EN_MASK, 0);

    return HAL_OK;
}

/**
 * @brief  Config ASRC controller.
 * @param  asrc: the handle of asrc.
 * @param  rxParams: rx asrc params.
 * @param  txParams: tx asrc params.
 * @return HAL_Status
 */
HAL_Status HAL_ASRC_Config(struct HAL_ASRC_DEV *asrc, struct ASRC_PARAMS *rxParams,
                           struct ASRC_PARAMS *txParams)
{
    uint32_t val = 0;

    HAL_ASSERT(asrc != NULL);
    HAL_ASSERT(rxParams != NULL);
    HAL_ASSERT(txParams != NULL);

    HAL_ASRC_DumpConfig(asrc, rxParams, txParams);

    HAL_ASRC_ConfigLrck(asrc, rxParams, txParams);
    HAL_ASRC_ConfigPeriod(asrc, rxParams, txParams);
    HAL_ASRC_ConfigRatio(asrc, rxParams->sampleRate, txParams->sampleRate);

    MODIFY_REG(asrc->pReg->CON, ASRC_CON_CHAN_NUM_MASK, ASRC_CHAN_NUM(asrc->channels));

    switch (rxParams->sampleBits) {
    case 32:
        val = ASRC_DATA_FMT_OSJM(8) | ASRC_DATA_FMT_ISJM(8) |
              ASRC_OWL_24BIT | ASRC_IWL_24BIT |
              ASRC_DATA_FMT_OFMT_32 | ASRC_DATA_FMT_IFMT_32;
        break;
    case 24:
        val = ASRC_DATA_FMT_OSJM(0) | ASRC_DATA_FMT_ISJM(0) |
              ASRC_OWL_24BIT | ASRC_IWL_24BIT |
              ASRC_DATA_FMT_OFMT_32 | ASRC_DATA_FMT_IFMT_32;
        break;
    case 16:
        val = ASRC_DATA_FMT_OSJM(0) | ASRC_DATA_FMT_ISJM(0) |
              ASRC_IWL_16BIT | ASRC_OWL_16BIT |
              ASRC_DATA_FMT_IFMT_16 | ASRC_DATA_FMT_OFMT_16;
        break;
    default:

        HAL_DBG_ERR("asrc-%p: %s: Invalid samplebits: %d\n",
                    asrc->pReg, __func__, rxParams->sampleBits);

        return HAL_INVAL;
    }

    MODIFY_REG(asrc->pReg->DATA_FMT,
               ASRC_DATA_FMT_OSJM_MASK | ASRC_DATA_FMT_ISJM_MASK |
               ASRC_DATA_FMT_IWL_MASK | ASRC_DATA_FMT_OWL_MASK |
               ASRC_DATA_FMT_IFMT_MASK | ASRC_DATA_FMT_OFMT_MASK,
               val);

    /*
     * Calculate lrck margin, the margin need less than
     * (ASRC_SAMPLE_FREQ / sampleRate) / 4
     */
    MODIFY_REG(asrc->pReg->LRCK_MARGIN, ASRC_LRCK_MARGIN_SRC_LRCK_MARGIN_MASK,
               ((ASRC_SAMPLE_FREQ / rxParams->sampleRate) / 4) << ASRC_LRCK_MARGIN_SRC_LRCK_MARGIN_SHIFT);
    MODIFY_REG(asrc->pReg->LRCK_MARGIN, ASRC_LRCK_MARGIN_DST_LRCK_MARGIN_MASK,
               ((ASRC_SAMPLE_FREQ / txParams->sampleRate) / 4) << ASRC_LRCK_MARGIN_DST_LRCK_MARGIN_SHIFT);

    return HAL_OK;
}

/**
 * @brief  ASRC series master or slave mode.
 * @param  asrc: the handle of asrc.
 * @param  mode: master or slave mode.
 * @return HAL_Status
 */
HAL_Status HAL_ASRC_SelectSeriesMode(struct HAL_ASRC_DEV *asrc, eASRC_seriesMode mode)
{
    HAL_ASSERT(asrc != NULL);

    MODIFY_REG(asrc->pReg->CON, ASRC_CON_SERIES_EN_MASK, ASRC_SERIES(mode));

    return HAL_OK;
}

/**
 * @brief  ASRC hard mute control.
 * @param  asrc: the handle of asrc.
 * @param  lane: asrc lane 0-3.
 * @return HAL_Status
 */
HAL_Status HAL_ASRC_HardMuteByLane(struct HAL_ASRC_DEV *asrc, uint8_t lane)
{
    HAL_ASSERT(asrc != NULL);
    HAL_ASSERT(lane <= 3);

    MODIFY_REG(asrc->pReg->CON,
               (1 << lane) << ASRC_CON_HARD_MUTE_EN_SHIFT,
               (1 << lane) << ASRC_CON_HARD_MUTE_EN_SHIFT);

    return HAL_OK;
}

/**
 * @brief  ASRC hard unmute control.
 * @param  asrc: the handle of asrc.
 * @param  lane: asrc lane 0-3.
 * @return HAL_Status
 */
HAL_Status HAL_ASRC_HardUnmuteByLane(struct HAL_ASRC_DEV *asrc, uint8_t lane)
{
    HAL_ASSERT(asrc != NULL);
    HAL_ASSERT(lane <= 3);

    MODIFY_REG(asrc->pReg->CON,
               (1 << lane) << ASRC_CON_HARD_MUTE_EN_SHIFT,
               (0 << lane) << ASRC_CON_HARD_MUTE_EN_SHIFT);

    return HAL_OK;
}

/**
 * @brief  ASRC soft mute control.
 * @param  asrc: the handle of asrc.
 * @param  lane: asrc lane 0-3.
 * @return HAL_Status
 */
HAL_Status HAL_ASRC_SoftMuteByLane(struct HAL_ASRC_DEV *asrc, uint8_t lane)
{
    HAL_ASSERT(asrc != NULL);
    HAL_ASSERT(lane <= 3);

    MODIFY_REG(asrc->pReg->CON,
               (1 << lane) << ASRC_CON_SOFT_MUTE_EN_SHIFT,
               (1 << lane) << ASRC_CON_SOFT_MUTE_EN_SHIFT);

    return HAL_OK;
}

/**
 * @brief  ASRC soft unmute control.
 * @param  asrc: the handle of asrc.
 * @param  lane: asrc lane 0-3.
 * @return HAL_Status
 */
HAL_Status HAL_ASRC_SoftUnmuteByLane(struct HAL_ASRC_DEV *asrc, uint8_t lane)
{
    HAL_ASSERT(asrc != NULL);
    HAL_ASSERT(lane <= 3);

    MODIFY_REG(asrc->pReg->CON,
               (1 << lane) << ASRC_CON_SOFT_MUTE_EN_SHIFT,
               (0 << lane) << ASRC_CON_SOFT_MUTE_EN_SHIFT);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_ASRC_MODULE_ENABLED */
