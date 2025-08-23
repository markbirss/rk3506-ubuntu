/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_PDM_MODULE_ENABLED) && ((PDM_VERSION == 0x23112118U) || (PDM_VERSION == 0x23023576U))

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup PDM
 *  @{
 */

/** @defgroup PDM_How_To_Use How To Use
 *  @{

 The PDM driver can be used as follows:

 @} */

/** @defgroup PDM_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
/* SYSCONFIG */
#define PDM_SYSCONFIG_RX_START      (0x1U << PDM_SYSCONFIG_RX_START_SHIFT)
#define PDM_SYSCONFIG_RX_STOP       (0x0U << PDM_SYSCONFIG_RX_START_SHIFT)
#define PDM_SYSCONFIG_NUM_START_EN  (0x1U << PDM_SYSCONFIG_NUM_START_SHIFT)
#define PDM_SYSCONFIG_NUM_START_DIS (0x0U << PDM_SYSCONFIG_NUM_START_SHIFT)
#define PDM_SYSCONFIG_RX_CLR_WR     (0x1U << PDM_SYSCONFIG_RX_CLR_SHIFT)

/* CTRL */
#define PDM_CTRL_SJM_RJ    (0x0U << PDM_CTRL_SJM_SEL_SHIFT)
#define PDM_CTRL_SJM_LJ    (0x1U << PDM_CTRL_SJM_SEL_SHIFT)
#define PDM_CTRL_VDW_MSK   (0x1fU << PDM_CTRL_DATA_VLD_WIDTH_SHIFT)
#define PDM_CTRL_VDW(x)    ((x - 1) << PDM_CTRL_DATA_VLD_WIDTH_SHIFT)
#define PDM_CTRL_PATH_MASK (0xFU << PDM_CTRL_PATH0_EN_SHIFT)
#define PDM_CTRL_PATH3_EN  (0x1U << PDM_CTRL_PATH3_EN_SHIFT)
#define PDM_CTRL_PATH2_EN  (0x1U << PDM_CTRL_PATH2_EN_SHIFT)
#define PDM_CTRL_PATH1_EN  (0x1U << PDM_CTRL_PATH1_EN_SHIFT)
#define PDM_CTRL_PATH0_EN  (0x1U << PDM_CTRL_PATH0_EN_SHIFT)

/* FILTER_CTRL */
#define PDM_HPF_FREQ_0_234 (0x0U << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_0_468 (0x1U << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_0_937 (0x2U << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_1_875 (0x3U << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_3_75  (0x4U << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_7_5   (0x5U << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_15    (0x6U << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_30    (0x7U << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_60    (0x8U << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_122   (0x9U << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_251   (0xaU << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_528   (0xbU << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_1183  (0xcU << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_3152  (0xdU << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_23999 (0xeU << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_FREQ_0     (0xfU << PDM_FILTER_CTRL_HPF_CTRL_SHIFT)
#define PDM_HPF_LE         (0x1U << PDM_FILTER_CTRL_HPF_CTRLL_SHIFT)
#define PDM_HPF_RE         (0x1U << PDM_FILTER_CTRL_HPF_CTRLR_SHIFT)

/* FIFO_CTRL */
#define PDM_FIFO_CTRL_RDE_EN     (0x1U << PDM_FIFO_CTRL_RDE_SHIFT)
#define PDM_FIFO_CTRL_RDE_DIS    (0x0U << PDM_FIFO_CTRL_RDE_SHIFT)
#define PDM_FIFO_CTRL_DMA_RDL(x) ((x - 1) << PDM_FIFO_CTRL_RDL_SHIFT)

/********************* Private Structure Definition **************************/
/********************* Private Variable Definition ***************************/
/********************* Private Function Definition ***************************/
static HAL_Status PDM_ChangeSampleRate(struct HAL_PDM_DEV *pdm,
                                       struct AUDIO_PARAMS *params)
{
    struct PDM_REG *reg = pdm->pReg;
    uint32_t ratio, scale = 0;
    HAL_Status ret = HAL_OK;

    HAL_ASSERT(IS_PDM_INSTANCE(reg));
    ratio = pdm->clkOutRate / params->sampleRate / 2;
    switch (ratio) {
    case 8: scale = 27; break;
    case 12: scale = 33; break;
    case 16: scale = 36; break;
    case 24: scale = 42; break;
    case 25: scale = 42; break;
    case 32: scale = 45; break;
    case 48: scale = 51; break;
    case 50: scale = 51; break;
    case 64: scale = 54; break;
    case 75: scale = 57; break;
    case 96: scale = 60; break;
    case 100: scale = 60; break;
    case 125: scale = 63; break;
    case 150: scale = 66; break;
    default:
        ret = HAL_INVAL;
        break;
    }

    if (ret == HAL_OK) {
        MODIFY_REG(reg->FILTER_CTRL,
                   PDM_FILTER_CTRL_CIC_SCALE_MASK | PDM_FILTER_CTRL_CIC_VALUE_MASK,
                   (scale << PDM_FILTER_CTRL_CIC_SCALE_SHIFT) |
                   (ratio - 1) << PDM_FILTER_CTRL_CIC_VALUE_SHIFT);
    }

    return ret;
}

static HAL_Status PDM_ChangeClkFreq(struct HAL_PDM_DEV *pdm,
                                    struct AUDIO_PARAMS *params)
{
    HAL_Status ret = HAL_OK;

#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkEnable(pdm->clkOut);
    HAL_CRU_ClkSetFreq(pdm->clkOut, pdm->clkOutRate);
#endif

    ret = PDM_ChangeSampleRate(pdm, params);
    HAL_ASSERT(ret == HAL_OK);

    return ret;
}

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup PDM_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 ...to do or delete this row

 *  @{
 */

/**
 * @brief  pdm suspend.
 * @param  pdm: the handle of pdm.
 * @return HAL_Status
 */
HAL_Status HAL_PDM_Supsend(struct HAL_PDM_DEV *pdm)
{
    return HAL_OK;
}

/**
 * @brief  pdm resume.
 * @param  pdm: the handle of pdm.
 * @return HAL_Status
 */
HAL_Status HAL_PDM_Resume(struct HAL_PDM_DEV *pdm)
{
    return HAL_OK;
}

/** @} */

/** @defgroup PDM_Exported_Functions_Group2 State and Errors Functions

 This section provides functions allowing to get the status of the module:

 *  @{
 */

/** @} */

/** @defgroup PDM_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

/** @} */

/** @defgroup PDM_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 ...to do or delete this row

 *  @{
 */

/**
 * @brief  Init pdm controller.
 * @param  pdm: the handle of pdm.
 * @param  config: init config for i2s init.
 * @return HAL_Status
 */
HAL_Status HAL_PDM_Init(struct HAL_PDM_DEV *pdm, struct AUDIO_INIT_CONFIG *config)
{
    pdm->mode = config->pdmMode;

    return HAL_OK;
}

/**
 * @brief  DeInit pdm controller.
 * @param  pdm: the handle of pdm.
 * @return HAL_Status
 */
HAL_Status HAL_PDM_DeInit(struct HAL_PDM_DEV *pdm)
{
    HAL_ASSERT(IS_PDM_INSTANCE(pdm->pReg));

    return HAL_OK;
}

/** @} */

/** @defgroup PDM_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Enable pdm controller.
 * @param  pdm: the handle of pdm.
 * @return HAL_Status
 */
HAL_Status HAL_PDM_Enable(struct HAL_PDM_DEV *pdm)
{
    struct PDM_REG *reg = pdm->pReg;

    HAL_ASSERT(IS_PDM_INSTANCE(reg));

    MODIFY_REG(reg->FIFO_CTRL,
               PDM_FIFO_CTRL_RDE_MASK, PDM_FIFO_CTRL_RDE_EN);
    MODIFY_REG(reg->SYSCONFIG,
               PDM_SYSCONFIG_RX_START_MASK, PDM_SYSCONFIG_RX_START);
    MODIFY_REG(reg->SYSCONFIG,
               PDM_SYSCONFIG_NUM_START_MASK, PDM_SYSCONFIG_NUM_START_EN);

    return HAL_OK;
}

/**
 * @brief  Disable pdm controller.
 * @param  pdm: the handle of pdm.
 * @return HAL_Status
 */
HAL_Status HAL_PDM_Disable(struct HAL_PDM_DEV *pdm)
{
    struct PDM_REG *reg = pdm->pReg;

    HAL_ASSERT(IS_PDM_INSTANCE(reg));

    MODIFY_REG(reg->FIFO_CTRL,
               PDM_FIFO_CTRL_RDE_MASK, PDM_FIFO_CTRL_RDE_DIS);
    MODIFY_REG(reg->SYSCONFIG,
               PDM_SYSCONFIG_RX_START_MASK | PDM_SYSCONFIG_RX_CLR_MASK |
               PDM_SYSCONFIG_NUM_START_MASK,
               PDM_SYSCONFIG_RX_STOP | PDM_SYSCONFIG_RX_CLR_WR |
               PDM_SYSCONFIG_NUM_START_DIS);
#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkDisable(pdm->clkOut);
#endif

    return HAL_OK;
}

/**
 * @brief  Config pdm controller.
 * @param  pdm: the handle of pdm.
 * @param  params: audio params.
 * @return HAL_Status
 */
HAL_Status HAL_PDM_Config(struct HAL_PDM_DEV *pdm, struct AUDIO_PARAMS *params)
{
    struct PDM_REG *reg = pdm->pReg;
    uint32_t val = 0;
    HAL_Status ret = HAL_OK;

    ret = PDM_ChangeClkFreq(pdm, params);
    HAL_ASSERT(ret == HAL_OK);

    MODIFY_REG(reg->CTRL,
               PDM_CTRL_SJM_SEL_MASK, PDM_CTRL_SJM_LJ);
    MODIFY_REG(reg->FILTER_CTRL,
               PDM_FILTER_CTRL_HPF_CTRLR_MASK | PDM_FILTER_CTRL_HPF_CTRLL_MASK |
               PDM_FILTER_CTRL_HPF_CTRL_MASK,
               PDM_HPF_RE | PDM_HPF_LE | PDM_HPF_FREQ_60);
    val = PDM_CTRL_VDW(params->sampleBits);
    switch (params->channels) {
    case 8:
        val |= PDM_CTRL_PATH3_EN;
    /* fallthrough */
    case 6:
        val |= PDM_CTRL_PATH2_EN;
    /* fallthrough */
    case 4:
        val |= PDM_CTRL_PATH1_EN;
    /* fallthrough */
    case 2:
        val |= PDM_CTRL_PATH0_EN;
        break;
    default:

        return HAL_INVAL;
    }

    MODIFY_REG(reg->CTRL, PDM_CTRL_PATH_MASK | PDM_CTRL_VDW_MSK, val);
    /* all channels share the single FIFO */
    MODIFY_REG(reg->FIFO_CTRL, PDM_FIFO_CTRL_RDL_MASK,
               PDM_FIFO_CTRL_DMA_RDL(8 * params->channels));

    return ret;
}

/**
 * @brief  pdm mute control.
 * @param  pdm: the handle of pdm.
 * @param  channel: set the pdm which channel to mute,value: 0-7.
 * @return HAL_Status
 */
HAL_Status HAL_PDM_MuteByChannel(struct HAL_PDM_DEV *pdm, uint8_t channel)
{
#if (PDM_VERSION != 0x23023576U)
    struct PDM_REG *reg = pdm->pReg;

    if (channel > 7) {
        return HAL_INVAL;
    }

    MODIFY_REG(reg->SYSCONFIG,
               (1 << channel) << PDM_SYSCONFIG_MUTE_SHIFT,
               (1 << channel) << PDM_SYSCONFIG_MUTE_SHIFT);
#endif

    return HAL_OK;
}

/**
 * @brief  pdm unmute control.
 * @param  pdm: the handle of pdm.
 * @param  channel: set the pdm which channel to mute,value: 0-7.
 * @return HAL_Status
 */
HAL_Status HAL_PDM_UnmuteByChannel(struct HAL_PDM_DEV *pdm, uint8_t channel)
{
#if (PDM_VERSION != 0x23023576U)
    struct PDM_REG *reg = pdm->pReg;

    if (channel > 7) {
        return HAL_INVAL;
    }

    MODIFY_REG(reg->SYSCONFIG,
               (1 << channel) << PDM_SYSCONFIG_MUTE_SHIFT,
               (0 << channel) << PDM_SYSCONFIG_MUTE_SHIFT);
#endif

    return HAL_OK;
}

/**
 * @brief  pdm set gain control.
 * @param  pdm: the handle of pdm.
 * @param  gain: set pdm gain.
 * @return HAL_Status
 */
HAL_Status HAL_PDM_SetGain(struct HAL_PDM_DEV *pdm, uint32_t gain)
{
    struct PDM_REG *reg = pdm->pReg;

    if (gain > 0xff) {
        return HAL_INVAL;
    }

    pdm->gain = gain;
#if (PDM_VERSION != 0x23023576U)
    MODIFY_REG(reg->GAIN_CTRL, PDM_GAIN_CTRL_GAIN_CTRL_MASK,
               gain << PDM_GAIN_CTRL_GAIN_CTRL_SHIFT);
#else
    MODIFY_REG(reg->FILTER_CTRL, PDM_FILTER_CTRL_GAIN_CTRL_MASK,
               gain << PDM_FILTER_CTRL_GAIN_CTRL_SHIFT);
#endif

    return HAL_OK;
}

/**
 * @brief  pdm get gain control.
 * @param  pdm: the handle of pdm.
 * @return gain
 */
uint32_t HAL_PDM_GetGain(struct HAL_PDM_DEV *pdm)
{
    struct PDM_REG *reg = pdm->pReg;

#if (PDM_VERSION != 0x23023576U)
    pdm->gain = READ_REG(reg->GAIN_CTRL) | PDM_GAIN_CTRL_GAIN_CTRL_MASK;
#else
    pdm->gain = (READ_REG(reg->FILTER_CTRL) | PDM_FILTER_CTRL_GAIN_CTRL_MASK) >>
                PDM_FILTER_CTRL_GAIN_CTRL_SHIFT;
#endif

    return pdm->gain;
}

/** @} */

/** @} */

/** @} */

#endif /* defined(HAL_PDM_MODULE_ENABLED) && (PDM_VERSION == 0x23112118U) */
