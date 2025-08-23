/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_ACDCDIG_DSM_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup ACDCDIG_DSM
 *  @{
 */

/** @defgroup ACDCDIG_DSM_How_To_Use How To Use
 *  @{

 The ACDCDIG_DSM driver can be used as follows:

 ACDCDIG_DSM is used to display audio.
 - Invoke HAL_ACDCDIG_DSM_Init to init ACDCDIG_DSM.
 - Invoke HAL_ACDCDIG_DSM_Config to setup ACDCDIG_DSM parameters.
 - Invoke HAL_ACDCDIG_DSM_SetGain to set ACDCDIG_DSM gain.
 - Invoke HAL_ACDCDIG_DSM_Enable to start ACDCDIG_DSM.
 - Invoke HAL_ACDCDIG_DSM_Disable to stopACDCDIG_DSM.
 - Invoke HAL_ACDCDIG_DSM_DeInit to deinit ACDCDIG_DSM.
 @} */

/** @defgroup ACDCDIG_DSM_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
/* DACSCLKRXINT_DIV */
#define ACDCDIG_DSM_INTSCLK_DIV_SCKRXDIV(x) ((x) - 1)

/* I2S_CKR0 */
#define ACDCDIG_DSM_I2S_CKR0_RSD_64  (0 << ACDCDIG_DSM_I2S_CKR0_RSD_SHIFT)
#define ACDCDIG_DSM_I2S_CKR0_RSD_128 (1 << ACDCDIG_DSM_I2S_CKR0_RSD_SHIFT)
#define ACDCDIG_DSM_I2S_CKR0_RSD_256 (2 << ACDCDIG_DSM_I2S_CKR0_RSD_SHIFT)

/* I2S_XFER */
#define ACDCDIG_DSM_I2S_XFER_RXS_START (1 << ACDCDIG_DSM_I2S_XFER_RXS_SHIFT)
#define ACDCDIG_DSM_I2S_XFER_RXS_STOP  (0 << ACDCDIG_DSM_I2S_XFER_RXS_SHIFT)

/* DIGEN */
#define ACDCDIG_DSM_DIGEN_DACGLB_EN      (1 << ACDCDIG_DSM_DIGEN_DACGLBEN_SHIFT)
#define ACDCDIG_DSM_DIGEN_DACGLB_DIS     (0 << ACDCDIG_DSM_DIGEN_DACGLBEN_SHIFT)
#define ACDCDIG_DSM_DIGEN_DACEN_L0R1_EN  (1 << ACDCDIG_DSM_DIGEN_DACEN_L0R1_SHIFT)
#define ACDCDIG_DSM_DIGEN_DACEN_L0R1_DIS (0 << ACDCDIG_DSM_DIGEN_DACEN_L0R1_SHIFT)

/* CTRL */
#define ACDCDIG_DSM_CTRL_DSM_EN           (1 << ACDCDIG_DSM_CTRL_DSM_EN_SHIFT)
#define ACDCDIG_DSM_CTRL_DSM_DIS          (0 << ACDCDIG_DSM_CTRL_DSM_EN_SHIFT)
#define ACDCDIG_DSM_CTRL_DSM_MODE_CKE_EN  (1 << ACDCDIG_DSM_CTRL_DSM_MODE_CKE_SHIFT)
#define ACDCDIG_DSM_CTRL_DSM_MODE_CKE_DIS (0 << ACDCDIG_DSM_CTRL_DSM_MODE_CKE_SHIFT)
#define ACDCDIG_DSM_CTRL_DSM_MODE_0       (1 << ACDCDIG_DSM_CTRL_DSM_MODE_SHIFT)

/* I2S_CLR */
#define ACDCDIG_DSM_I2S_CLR_RXC (1 << ACDCDIG_DSM_I2S_CLR_RXC_SHIFT)

/* VOLL */
#define ACDCDIG_DSM_VOLL(x) (x << ACDCDIG_DSM_VOLL0_DACLV0_SHIFT)

/* VOLR */
#define ACDCDIG_DSM_VOLR(x) (x << ACDCDIG_DSM_VOLR0_DACRV0_SHIFT)

/* CFG1 */
#define ACDCDIG_DSM_CFG1_DACSRT(x) (x << ACDCDIG_DSM_CFG1_DACSRT_SHIFT)

/* I2S_RXCR0 */
#define ACDCDIG_DSM_I2S_RXCR0_VDW(x) ((x - 1) << ACDCDIG_DSM_I2S_RXCR0_VDW_SHIFT)
/**
  * The range of gains.
  * To avoid using decimals, the scaled 1000 times dB is like Linux ALSA TLV.
  * min: -95.625dB, max: 0dB, step: 0.375dB.
  */
#define ACDCDIG_DSM_DB_MIN  (-95625)
#define ACDCDIG_DSM_DB_MAX  (0)
#define ACDCDIG_DSM_DB_STEP (375)

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/
static void HAL_ACDCDIG_DSM_GetClock(unsigned int samplerate,
                                     unsigned int *mclk,
                                     unsigned int *sclk)
{
    switch (samplerate) {
    case 12000:
    case 24000:
    case 48000:
    case 96000:
    case 192000:
        *mclk = 49152000;
        *sclk = 6144000;
        break;
    case 11025:
    case 22050:
    case 44100:
    case 88200:
    case 176400:
        *mclk = 45158400;
        *sclk = 5644800;
        break;
    case 8000:
    case 16000:
    case 32000:
    case 64000:
    case 128000:
        *mclk = 32768000;
        *sclk = 4096000;
        break;
    default:
        *mclk = 0;
        *sclk = 0;
        break;
    }
}

static void HAL_ACDCDIG_DSM_EnableDacClk(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm)
{
    struct ACDCDIG_DSM_REG *reg = acdcDigDsm->pReg;

    MODIFY_REG(reg->CLKCTRL,
               ACDCDIG_DSM_CLKCTRL_DAC_CKE_MASK |
               ACDCDIG_DSM_CLKCTRL_I2SRX_CKE_MASK |
               ACDCDIG_DSM_CLKCTRL_CKE_BCLKRX_MASK |
               ACDCDIG_DSM_CLKCTRL_DAC_SYNC_ENA_MASK |
               ACDCDIG_DSM_CLKCTRL_DAC_MOD_ATTENU_EN_MASK,
               ACDCDIG_DSM_CLKCTRL_DAC_CKE_MASK |
               ACDCDIG_DSM_CLKCTRL_I2SRX_CKE_MASK |
               ACDCDIG_DSM_CLKCTRL_CKE_BCLKRX_MASK |
               ACDCDIG_DSM_CLKCTRL_DAC_SYNC_ENA_MASK |
               ACDCDIG_DSM_CLKCTRL_DAC_MOD_ATTENU_EN_MASK);
}

static HAL_Status HAL_ACDCDIG_DSM_SetClock(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm,
                                           unsigned int samplerate)
{
    unsigned int mclk, sclk, bclk, div_bclk;
    struct ACDCDIG_DSM_REG *reg = acdcDigDsm->pReg;

    HAL_ACDCDIG_DSM_GetClock(samplerate, &mclk, &sclk);
    if (!mclk || !sclk) {
        return HAL_INVAL;
    }

    bclk = 64 * samplerate;
    div_bclk = HAL_DIV_ROUND_UP(mclk, bclk);
#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkEnable(acdcDigDsm->mclk);
    HAL_CRU_ClkSetFreq(acdcDigDsm->mclk, mclk);
#endif
    HAL_ACDCDIG_DSM_EnableDacClk(acdcDigDsm);
    MODIFY_REG(reg->INTSCLK_DIV,
               ACDCDIG_DSM_INTSCLK_DIV_SCKRXDIV_MASK,
               ACDCDIG_DSM_INTSCLK_DIV_SCKRXDIV(div_bclk));

    MODIFY_REG(reg->I2S_CKR0,
               ACDCDIG_DSM_I2S_CKR0_RSD_MASK,
               ACDCDIG_DSM_I2S_CKR0_RSD_64);

    return HAL_OK;
}
/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup ACDCDIG_DSM_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 ...to do or delete this row

 *  @{
 */

/**
 * @brief  acdcDigDsm suspend.
 * @param  acdcDigDsm: the handle of acdcDigDsm.
 * @return HAL_Status
 */
HAL_Status HAL_ACDCDIG_DSM_Supsend(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm)
{
    return HAL_OK;
}

/**
 * @brief  acdcDigDsm resume.
 * @param  acdcDigDsm: the handle of acdcDigDsm.
 * @return HAL_Status
 */
HAL_Status HAL_ACDCDIG_DSM_Resume(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm)
{
    return HAL_OK;
}

/** @} */

/** @defgroup ACDCDIG_DSM_Exported_Functions_Group2 State and Errors Functions

 This section provides functions allowing to get the status of the module:

 *  @{
 */

/** @} */

/** @defgroup ACDCDIG_DSM_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

/** @} */

/** @defgroup ACDCDIG_DSM_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 ...to do or delete this row

 *  @{
 */

/**
 * @brief  Init acdcDigDsm controller.
 * @param  acdcDigDsm: the handle of acdcDigDsm.
 * @param  config: init config for i2s init.
 * @return HAL_Status
 */
HAL_Status HAL_ACDCDIG_DSM_Init(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm, struct AUDIO_INIT_CONFIG *config)
{
#if defined(SOC_RK3562)
    WRITE_REG(PERI_GRF->AUDIO_CON,
              (1 << (PERI_GRF_AUDIO_CON_RKDSM_SAI_ACTIVE_SHIFT + 16)) |
              (1 << PERI_GRF_AUDIO_CON_RKDSM_SAI_ACTIVE_SHIFT));
#elif defined(SOC_RK3506)
    WRITE_REG(GRF->SOC_CON0,
              (1 << (GRF_SOC_CON0_GRF_CON_SAI3_SEL_SHIFT + 16)) |
              (1 << GRF_SOC_CON0_GRF_CON_SAI3_SEL_SHIFT));
#endif

    return HAL_OK;
}

/**
 * @brief  DeInit acdcDigDsm controller.
 * @param  acdcDigDsm: the handle of acdcDigDsm.
 * @return HAL_Status
 */
HAL_Status HAL_ACDCDIG_DSM_DeInit(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm)
{
#if defined(SOC_RK3562)
    WRITE_REG(PERI_GRF->AUDIO_CON,
              (1 << (PERI_GRF_AUDIO_CON_RKDSM_SAI_ACTIVE_SHIFT + 16)));
#elif defined(SOC_RK3506)
    WRITE_REG(GRF->SOC_CON0,
              (1 << (GRF_SOC_CON0_GRF_CON_SAI3_SEL_SHIFT + 16)));
#endif
#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkDisable(acdcDigDsm->mclk);
    HAL_CRU_ClkResetAssert(acdcDigDsm->reset);
    HAL_CRU_ClkResetDeassert(acdcDigDsm->reset);
#endif

    return HAL_OK;
}

/** @} */

/** @defgroup ACDCDIG_DSM_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Enable acdcDigDsm controller.
 * @param  acdcDigDsm: the handle of acdcDigDsm.
 * @param  stream: audio stream type.
 * @return HAL_Status
 */
HAL_Status HAL_ACDCDIG_DSM_Enable(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm,
                                  eAUDIO_streamType stream)
{
    struct ACDCDIG_DSM_REG *reg = acdcDigDsm->pReg;

    MODIFY_REG(reg->CTRL,
               ACDCDIG_DSM_CTRL_DSM_EN_MASK |
               ACDCDIG_DSM_CTRL_DSM_MODE_CKE_MASK |
               ACDCDIG_DSM_CTRL_DSM_MODE_MASK,
               ACDCDIG_DSM_CTRL_DSM_EN |
               ACDCDIG_DSM_CTRL_DSM_MODE_CKE_EN |
               ACDCDIG_DSM_CTRL_DSM_MODE_0);
    MODIFY_REG(reg->I2S_XFER,
               ACDCDIG_DSM_I2S_XFER_RXS_MASK,
               ACDCDIG_DSM_I2S_XFER_RXS_START);
    MODIFY_REG(reg->DIGEN,
               ACDCDIG_DSM_DIGEN_DACGLBEN_MASK |
               ACDCDIG_DSM_DIGEN_DACEN_L0R1_MASK,
               ACDCDIG_DSM_DIGEN_DACGLB_EN |
               ACDCDIG_DSM_DIGEN_DACEN_L0R1_EN);

    return HAL_OK;
}

/**
 * @brief  Disable acdcDigDsm controller.
 * @param  acdcDigDsm: the handle of acdcDigDsm.
 * @param  stream: audio stream type.
 * @return HAL_Status
 */
HAL_Status HAL_ACDCDIG_DSM_Disable(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm,
                                   eAUDIO_streamType stream)
{
    struct ACDCDIG_DSM_REG *reg = acdcDigDsm->pReg;

    MODIFY_REG(reg->CTRL,
               ACDCDIG_DSM_CTRL_DSM_EN_MASK |
               ACDCDIG_DSM_CTRL_DSM_MODE_CKE_MASK,
               ACDCDIG_DSM_CTRL_DSM_DIS |
               ACDCDIG_DSM_CTRL_DSM_MODE_CKE_DIS);
    MODIFY_REG(reg->I2S_XFER,
               ACDCDIG_DSM_I2S_XFER_RXS_MASK,
               ACDCDIG_DSM_I2S_XFER_RXS_STOP);
    MODIFY_REG(reg->I2S_CLR,
               ACDCDIG_DSM_I2S_CLR_RXC_MASK,
               ACDCDIG_DSM_I2S_CLR_RXC);
    MODIFY_REG(reg->DIGEN,
               ACDCDIG_DSM_DIGEN_DACGLBEN_MASK |
               ACDCDIG_DSM_DIGEN_DACEN_L0R1_MASK,
               ACDCDIG_DSM_DIGEN_DACGLB_DIS |
               ACDCDIG_DSM_DIGEN_DACEN_L0R1_DIS);

    return HAL_OK;
}

/**
 * @brief  Config acdcDigDsm controller.
 * @param  acdcDigDsm: the handle of acdcDigDsm.
 * @param  stream: audio stream type.
 * @param  params: audio params.
 * @return HAL_Status
 */
HAL_Status HAL_ACDCDIG_DSM_Config(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm,
                                  eAUDIO_streamType stream,
                                  struct AUDIO_PARAMS *params)
{
    struct ACDCDIG_DSM_REG *reg = acdcDigDsm->pReg;
    uint32_t val = 0;

    HAL_ACDCDIG_DSM_SetClock(acdcDigDsm, params->sampleRate);
    switch (params->sampleRate) {
    case AUDIO_SAMPLERATE_8000:
    case AUDIO_SAMPLERATE_11025:
    case AUDIO_SAMPLERATE_12000:
        val = 0;
        break;
    case AUDIO_SAMPLERATE_16000:
    case AUDIO_SAMPLERATE_22050:
    case AUDIO_SAMPLERATE_24000:
        val = 1;
        break;
    case AUDIO_SAMPLERATE_32000:
    case AUDIO_SAMPLERATE_44100:
    case AUDIO_SAMPLERATE_48000:
        val = 2;
        break;
    case AUDIO_SAMPLERATE_64000:
    case AUDIO_SAMPLERATE_88200:
    case AUDIO_SAMPLERATE_96000:
        val = 3;
        break;
    case AUDIO_SAMPLERATE_128000:
    case AUDIO_SAMPLERATE_176400:
    case AUDIO_SAMPLERATE_192000:
        val = 4;
        break;
    default:

        return HAL_INVAL;
    }

    MODIFY_REG(reg->CFG1,
               ACDCDIG_DSM_CFG1_DACSRT_MASK,
               ACDCDIG_DSM_CFG1_DACSRT(val));

    switch (params->sampleBits) {
    case AUDIO_SAMPLEBITS_16:
        val = 16;
        break;
    case AUDIO_SAMPLEBITS_24:
    case AUDIO_SAMPLEBITS_32:
        val = 24;
        break;
    default:

        return HAL_INVAL;
    }

    MODIFY_REG(reg->I2S_RXCR0,
               ACDCDIG_DSM_I2S_RXCR0_VDW_MASK,
               ACDCDIG_DSM_I2S_RXCR0_VDW(val));

    return HAL_OK;
}

/**
 * @brief  Set Gain for acdcDigDsm.
 * @param  acdcDigDsm: the handle of acdcDigDsm.
 * @param  stream: audio stream type.
 * @param  dBConfig: The gains config of codec, it includes ADC/DAC channel,
 *                   and the dB range is: -95625(-95.625dB) ~ 95625(95.625dB).
 *                   Similar to Linux ALSA TLV dB scale, they are scaled 1000
 *                   times to avoid representation of fractional parts.
 * @return HAL_Status
 */
HAL_Status HAL_ACDCDIG_DSM_SetGain(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm,
                                   eAUDIO_streamType stream,
                                   struct AUDIO_DB_CONFIG *dBConfig)
{
    struct ACDCDIG_DSM_REG *reg = acdcDigDsm->pReg;
    int dBStep;

    dBStep = dBConfig->dB;
    if (dBStep > ACDCDIG_DSM_DB_MAX) {
        dBStep = ACDCDIG_DSM_DB_MAX;
    } else if (dBStep < ACDCDIG_DSM_DB_MIN) {
        dBStep = ACDCDIG_DSM_DB_MAX;
    }

    if (dBStep > 0) {
        dBStep = dBConfig->dB / ACDCDIG_DSM_DB_STEP;
    } else {
        dBStep = -(dBConfig->dB / ACDCDIG_DSM_DB_STEP);
    }

    if (dBConfig->ch < 4) {
        MODIFY_REG(reg->VOLL[dBConfig->ch],
                   ACDCDIG_DSM_VOLL0_DACLV0_MASK,
                   ACDCDIG_DSM_VOLL(dBStep));
        MODIFY_REG(reg->VOLR[dBConfig->ch],
                   ACDCDIG_DSM_VOLL0_DACLV0_MASK,
                   ACDCDIG_DSM_VOLL(dBStep));
    } else {
        return HAL_INVAL;
    }

    return HAL_OK;
}

/**
 * @brief  Get Gain for acdcDigDsm.
 * @param  acdcDigDsm: the handle of acdcDigDsm.
 * @param  stream: audio stream type.
 * @param  dBConfig: The gains config of codec, it includes ADC/DAC channel,
 *                   and the dB range is: -95625(-95.625dB) ~ 95625(95.625dB).
 *                   Similar to Linux ALSA TLV dB scale, they are scaled 1000
 *                   times to avoid representation of fractional parts.
 * @return HAL_Status
 */
HAL_Status HAL_ACDCDIG_DSM_GetGain(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm,
                                   eAUDIO_streamType stream,
                                   struct AUDIO_DB_CONFIG *dBConfig)
{
    struct ACDCDIG_DSM_REG *reg = acdcDigDsm->pReg;
    int dBStep;

    if (dBConfig->ch < 4) {
        dBStep = READ_REG(reg->VOLL[dBConfig->ch]) |
                 ACDCDIG_DSM_VOLL0_DACLV0_MASK;
    } else {
        return HAL_INVAL;
    }

    dBConfig->dB = -(dBStep * ACDCDIG_DSM_DB_STEP);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_ACDCDIG_DSM_MODULE_ENABLED */
