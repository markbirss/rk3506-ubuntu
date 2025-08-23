/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_ACODEC_MODULE_ENABLED) && defined(SOC_RK3506)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup ACODEC
 *  @{
 */

/** @defgroup ACODEC_How_To_Use How To Use
 *  @{

 The ACODEC driver can be used as follows:

 - Invoke HAL_ACODEC_Init() to initialize acodec.
 - Invoke HAL_ACODEC_Config() to config samplerate, width.
 - Invoke HAL_ACODEC_Enable() to enable acodec.
 - Invoke HAL_ACODEC_Disable() to disable acodec.
 - Invoke HAL_ACODEC_DeInit() to deinitialize acodec.
 - More details refer to APIs' descriptions as below.

 @} */

/** @defgroup ACODEC_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
#define MCLK_REFERENCE_8000    32768000
#define MCLK_REFERENCE_11025   45158400
#define MCLK_REFERENCE_12000   49152000
#define MCLK_I2S_REFERENCE_DIV 4
#define I2S_MCLK_FS            64

#define RK3506_ADC_IP_EN              (1 << ACODEC_ADC_LDO_ADC_IP_EN_SHIFT)
#define RK3506_ADC_IP_DIS             (0 << ACODEC_ADC_LDO_ADC_IP_EN_SHIFT)
#define RK3506_LDO_VSEL_1_5V          (0 << ACODEC_ADC_LDO_LDO_VSEL_SHIFT)
#define RK3506_LDO_VSEL_1_55V         (1 << ACODEC_ADC_LDO_LDO_VSEL_SHIFT)
#define RK3506_LDO_VSEL_1_6V          (2 << ACODEC_ADC_LDO_LDO_VSEL_SHIFT)
#define RK3506_LDO_VSEL_1_65V         (3 << ACODEC_ADC_LDO_LDO_VSEL_SHIFT)
#define RK3506_PGA_PWD_EN             (0 << ACODEC_MIC_PGA0_MIC_PGA_PWD_SHIFT)
#define RK3506_PGA_PWD_DIS            (1 << ACODEC_MIC_PGA0_MIC_PGA_PWD_SHIFT)
#define RK3506_PGA_INPUT_DEC_N1_34DB  (0 << ACODEC_MIC_PGA0_MIC_PGA_INPUT_DEC_SHIFT)
#define RK3506_PGA_INPUT_DEC_N4_34DB  (1 << ACODEC_MIC_PGA0_MIC_PGA_INPUT_DEC_SHIFT)
#define RK3506_PGA_INPUT_DEC_N7_34DB  (2 << ACODEC_MIC_PGA0_MIC_PGA_INPUT_DEC_SHIFT)
#define RK3506_PGA_INPUT_DEC_N10_34DB (3 << ACODEC_MIC_PGA0_MIC_PGA_INPUT_DEC_SHIFT)
#define RK3506_PGA_GAIN_3DB           (0x10 << ACODEC_MIC_PGA0_MIC_PGA_GAIN_SHIFT)
#define RK3506_PGA_CHOP_SEL_200K      (0 << ACODEC_MIC_PGA1_MIC_PGA_CHOP_SEL_SHIFT)
#define RK3506_PGA_CHOP_SEL_400K      (1 << ACODEC_MIC_PGA1_MIC_PGA_CHOP_SEL_SHIFT)
#define RK3506_PGA_CHOP_SEL_800K      (2 << ACODEC_MIC_PGA1_MIC_PGA_CHOP_SEL_SHIFT)
#define RK3506_PGA_BUF_IB_SEL_100_PCT (0 << ACODEC_MIC_PGA2_PGA_BUF_IB_SEL_SHIFT)
#define RK3506_PGA_BUF_IB_SEL_67_PCT  (1 << ACODEC_MIC_PGA2_PGA_BUF_IB_SEL_SHIFT)
#define RK3506_PGA_BUF_IB_SEL_133_PCT (2 << ACODEC_MIC_PGA2_PGA_BUF_IB_SEL_SHIFT)
#define RK3506_PGA_BUF_IB_SEL_167_PCT (3 << ACODEC_MIC_PGA2_PGA_BUF_IB_SEL_SHIFT)
#define RK3506_PGA_BUF_CHOP_SEL_200K  (1 << ACODEC_MIC_PGA2_PGA_BUF_CHOP_SEL_SHIFT)
#define RK3506_PGA_BUF_CHOP_SEL_400K  (2 << ACODEC_MIC_PGA2_PGA_BUF_CHOP_SEL_SHIFT)
#define RK3506_PGA_BUF_CHOP_SEL_800K  (3 << ACODEC_MIC_PGA2_PGA_BUF_CHOP_SEL_SHIFT)
#define RK3506_ADC_PWD_EN             (0 << ACODEC_ADC0_ADC_PWD_SHIFT)
#define RK3506_ADC_PWD_DIS            (1 << ACODEC_ADC0_ADC_PWD_SHIFT)
#define RK3506_ADC_METH_DMA           (1 << ACODEC_ADC0_ADC_METH_CTRL_SHIFT)
#define RK3506_ADC_METH_ONE           (2 << ACODEC_ADC0_ADC_METH_CTRL_SHIFT)
#define RK3506_ADC_METH_DUAL          (3 << ACODEC_ADC0_ADC_METH_CTRL_SHIFT)
#define RK3506_ADC_EN                 (1 << ACODEC_DIG_CLKEN_ADC_EN_SHIFT)
#define RK3506_ADC_DIS                (0 << ACODEC_DIG_CLKEN_ADC_EN_SHIFT)
#define RK3506_ADC_CKE_EN             (1 << ACODEC_DIG_CLKEN_ADC_CKE_SHIFT)
#define RK3506_ADC_CKE_DIS            (0 << ACODEC_DIG_CLKEN_ADC_CKE_SHIFT)
#define RK3506_I2STX_EN               (1 << ACODEC_DIG_CLKEN_I2STX_EN_SHIFT)
#define RK3506_I2STX_DIS              (0 << ACODEC_DIG_CLKEN_I2STX_EN_SHIFT)
#define RK3506_I2STX_CKE_EN           (1 << ACODEC_DIG_CLKEN_I2STX_CKE_SHIFT)
#define RK3506_I2STX_CKE_DIS          (0 << ACODEC_DIG_CLKEN_I2STX_CKE_SHIFT)
#define RK3506_TXC_EN                 (1 << ACODEC_I2S_TXCMD_TXC_SHIFT)
#define RK3506_TXC_DIS                (0 << ACODEC_I2S_TXCMD_TXC_SHIFT)
#define RK3506_TXS_START              (1 << ACODEC_I2S_TXCMD_TXS_SHIFT)
#define RK3506_TXS_STOP               (0 << ACODEC_I2S_TXCMD_TXS_SHIFT)
#define RK3506_HK_VAG_BUF_EN          (1 << ACODEC_ADC_HK0_HK_VAG_BUF_EN_SHIFT)
#define RK3506_HK_VAG_BUF_DIS         (0 << ACODEC_ADC_HK0_HK_VAG_BUF_EN_SHIFT)
#define RK3506_HK_ADC_BUF_EN          (1 << ACODEC_ADC_HK0_HK_ADC_BUF_EN_SHIFT)
#define RK3506_HK_ADC_BUF_DIS         (0 << ACODEC_ADC_HK0_HK_ADC_BUF_EN_SHIFT)
#define RK3506_HK_VREF_1P2V_SEL_P10M  (1 << ACODEC_ADC_HK1_HK_VREF_1P2V_SEL_SHIFT)
#define RK3506_HK_VREF_1P2V_SEL_N10M  (2 << ACODEC_ADC_HK1_HK_VREF_1P2V_SEL_SHIFT)
#define RK3506_HK_VREF_1P2V_SEL_N20M  (3 << ACODEC_ADC_HK1_HK_VREF_1P2V_SEL_SHIFT)
#define RK3506_ADC_CHOP_EN            (0 << ACODEC_ADC2_ADC_CHOP_EN_SHIFT)
#define RK3506_ADC_CHOP_DIS           (1 << ACODEC_ADC2_ADC_CHOP_EN_SHIFT)
#define RK3506_ADC_CAPTRIM_80_PCT     (0 << ACODEC_ADC2_ADC_CAPTRIM_SHIFT)
#define RK3506_ADC_CAPTRIM_90_PCT     (1 << ACODEC_ADC2_ADC_CAPTRIM_SHIFT)
#define RK3506_ADC_CAPTRIM_100_PCT    (2 << ACODEC_ADC2_ADC_CAPTRIM_SHIFT)
#define RK3506_ADC_CAPTRIM_110_PCT    (3 << ACODEC_ADC2_ADC_CAPTRIM_SHIFT)
#define RK3506_ADC_CAPTRIM_120_PCT    (4 << ACODEC_ADC2_ADC_CAPTRIM_SHIFT)
#define RK3506_ADC_CAPTRIM_130_PCT    (5 << ACODEC_ADC2_ADC_CAPTRIM_SHIFT)
#define RK3506_ADC_CAPTRIM_140_PCT    (6 << ACODEC_ADC2_ADC_CAPTRIM_SHIFT)
#define RK3506_ADC_CAPTRIM_150_PCT    (7 << ACODEC_ADC2_ADC_CAPTRIM_SHIFT)
#define RK3506_ADC_BYPS_EN            (1 << ACODEC_ADC_AGC0_ADC_BYPS_SHIFT)
#define RK3506_ADC_BYPS_DIS           (0 << ACODEC_ADC_AGC0_ADC_BYPS_SHIFT)
#define RK3506_ADC_NG_MODE_EN         (1 << ACODEC_ADC_AGC0_ADC_NG_MODE_EN_SHIFT)
#define RK3506_ADC_NG_MODE_DIS        (0 << ACODEC_ADC_AGC0_ADC_NG_MODE_EN_SHIFT)
#define RK3506_I2S_MASTER             (0x1 << ACODEC_I2S_CKM_I2S_MST_SHIFT)
#define RK3506_I2S_SLAVE              (0x0 << ACODEC_I2S_CKM_I2S_MST_SHIFT)
#define RK3506_SCK_DIV(x)             ((x - 1) << ACODEC_I2S_CKM_SCK_DIV_SHIFT)
#define RK3506_CICCOMP_EN32_MSK       (0x1 << ACODEC_ADC_FILTER_CICCOMP_ENA32_SHIFT)
#define RK3506_CICCOMP_EN32_EN        (0x1 << ACODEC_ADC_FILTER_CICCOMP_ENA32_SHIFT)
#define RK3506_CICCOMP_EN32_DIS       (0x0 << ACODEC_ADC_FILTER_CICCOMP_ENA32_SHIFT)
#define RK3506_CICCOMP_EN64_MSK       (0x1 << ACODEC_ADC_FILTER_CICCOMP_ENA64_SHIFT)
#define RK3506_CICCOMP_EN64_EN        (0x1 << ACODEC_ADC_FILTER_CICCOMP_ENA64_SHIFT)
#define RK3506_CICCOMP_EN64_DIS       (0x0 << ACODEC_ADC_FILTER_CICCOMP_ENA64_SHIFT)
#define RK3506_CICCOMP_CF_MSK         (0x3 << ACODEC_ADC_FILTER_CICCOMP_CF_SHIFT)
#define RK3506_CICCOMP_CF_37_5_PCT    (0x0 << ACODEC_ADC_FILTER_CICCOMP_CF_SHIFT)
#define RK3506_CICCOMP_CF_75_PCT      (0x1 << ACODEC_ADC_FILTER_CICCOMP_CF_SHIFT)
#define RK3506_CICCOMP_CF_100_PCT     (0x2 << ACODEC_ADC_FILTER_CICCOMP_CF_SHIFT)
#define RK3506_HPF_MSK                (0x1 << ACODEC_ADC_FILTER_HPFL_SHIFT)
#define RK3506_HPF_EN                 (0x1 << ACODEC_ADC_FILTER_HPFL_SHIFT)
#define RK3506_HPF_DIS                (0x0 << ACODEC_ADC_FILTER_HPFL_SHIFT)
#define RK3506_HPF_CF_MSK             (0x3 << ACODEC_ADC_FILTER_HPF_CF_SHIFT)
#define RK3506_HPF_CF_3_79HZ          (0x0 << ACODEC_ADC_FILTER_HPF_CF_SHIFT)
#define RK3506_HPF_CF_60HZ            (0x1 << ACODEC_ADC_FILTER_HPF_CF_SHIFT)
#define RK3506_HPF_CF_243HZ           (0x2 << ACODEC_ADC_FILTER_HPF_CF_SHIFT)
#define RK3506_HPF_CF_493HZ           (0x3 << ACODEC_ADC_FILTER_HPF_CF_SHIFT)
#define RK3506_ACODEC_FILTER_MSK      (0xff)
#define RK3506_ACODEC_FILTER_MODE1    (RK3506_HPF_CF_60HZ)
#define RK3506_ACODEC_FILTER_MODE2    (RK3506_HPF_CF_60HZ | RK3506_CICCOMP_CF_100_PCT | RK3506_CICCOMP_EN32_EN)
#define RK3506_ACODEC_FILTER_MODE3    (RK3506_HPF_CF_60HZ | RK3506_CICCOMP_CF_100_PCT | RK3506_CICCOMP_EN64_EN)
#define RK3506_VDW_TX(x)              ((x - 1) << ACODEC_I2S_TXCR0_VDW_TX_SHIFT)
#define RK3506_ADCSRT(x)              (x)

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/
static unsigned int SamplerateToBit(unsigned int samplerate)
{
    switch (samplerate) {
    case 8000:
    case 11025:
    case 12000:

        return 0;
    case 16000:
    case 22050:
    case 24000:

        return 1;
    case 32000:
    case 44100:
    case 48000:

        return 2;
    case 64000:
    case 88200:
    case 96000:

        return 3;
    case 128000:
    case 176400:
    case 192000:

        return 4;
    default:

        return 2;
    }
}

static void ACODEC_PowerOn(struct HAL_ACODEC_DEV *acodec)
{
    struct ACODEC_REG *reg = acodec->pReg;

    MODIFY_REG(reg->ADC_LDO,
               ACODEC_ADC_LDO_ADC_IP_EN_MASK,
               RK3506_ADC_IP_EN);
    MODIFY_REG(reg->MIC_PGA[0],
               ACODEC_MIC_PGA0_MIC_PGA_PWD_MASK,
               RK3506_PGA_PWD_EN);
    MODIFY_REG(reg->ADC[0],
               ACODEC_ADC0_ADC_PWD_MASK | ACODEC_ADC0_ADC_METH_CTRL_MASK,
               RK3506_ADC_PWD_EN | RK3506_ADC_METH_DMA);
}

static void ACODEC_PowerOff(struct HAL_ACODEC_DEV *acodec)
{
    struct ACODEC_REG *reg = acodec->pReg;

    MODIFY_REG(reg->MIC_PGA[0],
               ACODEC_MIC_PGA0_MIC_PGA_PWD_MASK,
               RK3506_PGA_PWD_DIS);
    MODIFY_REG(reg->ADC[0],
               ACODEC_ADC0_ADC_PWD_MASK,
               RK3506_ADC_PWD_DIS);
    MODIFY_REG(reg->ADC_LDO,
               ACODEC_ADC_LDO_ADC_IP_EN_MASK,
               RK3506_ADC_IP_DIS);
}

static int ACODEC_ADC_Enable(struct HAL_ACODEC_DEV *acodec)
{
    struct ACODEC_REG *reg = acodec->pReg;

    MODIFY_REG(reg->DIG_CLKEN,
               ACODEC_DIG_CLKEN_ADC_EN_MASK | ACODEC_DIG_CLKEN_ADC_CKE_MASK,
               RK3506_ADC_EN | RK3506_ADC_CKE_EN);

    return HAL_OK;
}

static int ACODEC_ADC_Disable(struct HAL_ACODEC_DEV *acodec)
{
    struct ACODEC_REG *reg = acodec->pReg;

    MODIFY_REG(reg->DIG_CLKEN,
               ACODEC_DIG_CLKEN_ADC_EN_MASK | ACODEC_DIG_CLKEN_ADC_CKE_MASK,
               RK3506_ADC_DIS | RK3506_ADC_CKE_DIS);

    return HAL_OK;
}

static void ACODEC_TX_Start(struct HAL_ACODEC_DEV *acodec)
{
    struct ACODEC_REG *reg = acodec->pReg;

    MODIFY_REG(reg->I2S_TXCMD,
               ACODEC_I2S_TXCMD_TXC_MASK | ACODEC_I2S_TXCMD_TXS_MASK,
               RK3506_TXC_DIS | RK3506_TXS_START);
    MODIFY_REG(reg->DIG_CLKEN,
               ACODEC_DIG_CLKEN_I2STX_CKE_MASK | ACODEC_DIG_CLKEN_I2STX_EN_MASK,
               RK3506_I2STX_CKE_EN | RK3506_I2STX_EN);
}

static void ACODEC_TX_Stop(struct HAL_ACODEC_DEV *acodec)
{
    struct ACODEC_REG *reg = acodec->pReg;

    MODIFY_REG(reg->I2S_TXCMD,
               ACODEC_I2S_TXCMD_TXC_MASK | ACODEC_I2S_TXCMD_TXS_MASK,
               RK3506_TXC_EN | RK3506_TXS_STOP);
    MODIFY_REG(reg->DIG_CLKEN,
               ACODEC_DIG_CLKEN_I2STX_CKE_MASK | ACODEC_DIG_CLKEN_I2STX_EN_MASK,
               RK3506_I2STX_CKE_DIS | RK3506_I2STX_DIS);
}

static void ACODEC_Reset(struct HAL_ACODEC_DEV *acodec)
{
    struct ACODEC_REG *reg = acodec->pReg;

    /* Auto clear reset */
    MODIFY_REG(reg->DIG_CLKEN,
               ACODEC_DIG_CLKEN_SRST_MASK,
               ACODEC_DIG_CLKEN_SRST_MASK);
    MODIFY_REG(reg->ADC_LDO,
               ACODEC_ADC_LDO_LDO_VSEL_MASK,
               RK3506_LDO_VSEL_1_65V);
    MODIFY_REG(reg->MIC_PGA[1],
               ACODEC_MIC_PGA1_MIC_PGA_CHOP_SEL_MASK,
               RK3506_PGA_CHOP_SEL_200K);
    MODIFY_REG(reg->ADC_HK[0],
               ACODEC_ADC_HK0_HK_VAG_BUF_EN_MASK | ACODEC_ADC_HK0_HK_ADC_BUF_EN_MASK,
               RK3506_HK_VAG_BUF_EN | RK3506_HK_ADC_BUF_EN);
    MODIFY_REG(reg->ADC[2],
               ACODEC_ADC2_ADC_CHOP_EN_MASK | ACODEC_ADC2_ADC_CAPTRIM_MASK,
               RK3506_ADC_CHOP_DIS | RK3506_ADC_CAPTRIM_100_PCT);
    MODIFY_REG(reg->ADC_AGC[0],
               ACODEC_ADC_AGC0_ADC_BYPS_MASK | ACODEC_ADC_AGC0_ADC_NG_MODE_EN_MASK,
               RK3506_ADC_BYPS_EN | RK3506_ADC_NG_MODE_EN);
    MODIFY_REG(reg->ADC_HK[1],
               ACODEC_ADC_HK1_HK_VREF_1P2V_SEL_MASK,
               RK3506_HK_VREF_1P2V_SEL_N10M);
    MODIFY_REG(reg->MIC_PGA[2],
               ACODEC_MIC_PGA2_PGA_BUF_IB_SEL_MASK | ACODEC_MIC_PGA2_PGA_BUF_CHOP_SEL_MASK,
               RK3506_PGA_BUF_IB_SEL_167_PCT | RK3506_PGA_BUF_CHOP_SEL_400K);
    MODIFY_REG(reg->MIC_PGA[0],
               ACODEC_MIC_PGA0_MIC_PGA_INPUT_DEC_MASK | ACODEC_MIC_PGA0_MIC_PGA_GAIN_MASK,
               RK3506_PGA_INPUT_DEC_N4_34DB | RK3506_PGA_GAIN_3DB);
}

static void ACODEC_SetDAIFormat(struct HAL_ACODEC_DEV *acodec, struct AUDIO_INIT_CONFIG *config)
{
    struct ACODEC_REG *reg = acodec->pReg;
    int val = 0;

    val = config->master ? RK3506_I2S_MASTER : RK3506_I2S_SLAVE;
    MODIFY_REG(reg->I2S_CKM,
               ACODEC_I2S_CKM_I2S_MST_MASK | ACODEC_I2S_CKM_SCK_EN_MASK,
               val | ACODEC_I2S_CKM_SCK_EN_MASK);
}

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup ACODEC_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 ...to do or delete this row

 *  @{
 */

/**
 * @brief  acodec suspend.
 * @param  acodec: the handle of acodec.
 * @return HAL_Status
 */
HAL_Status HAL_ACODEC_Supsend(struct HAL_ACODEC_DEV *acodec)
{
    return HAL_OK;
}

/**
 * @brief  acodec resume.
 * @param  acodec: the handle of acodec.
 * @return HAL_Status
 */
HAL_Status HAL_ACODEC_Resume(struct HAL_ACODEC_DEV *acodec)
{
    return HAL_OK;
}

/** @} */

/** @defgroup ACODEC_Exported_Functions_Group2 State and Errors Functions

 This section provides functions allowing to get the status of the module:

 *  @{
 */

/** @} */

/** @defgroup ACODEC_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

/** @} */

/** @defgroup ACODEC_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 ...to do or delete this row

 *  @{
 */

/**
 * @brief  Init acodec.
 * @param  acodec: the handle of acodec.
 * @param  config: init config for acodec init.
 * @return HAL_Status
 */
HAL_Status HAL_ACODEC_Init(struct HAL_ACODEC_DEV *acodec, struct AUDIO_INIT_CONFIG *config)
{
    ACODEC_Reset(acodec);
    ACODEC_SetDAIFormat(acodec, config);

    return HAL_OK;
}

/**
 * @brief  DeInit acodec.
 * @param  acodec: the handle of acodec.
 * @return HAL_Status
 */
HAL_Status HAL_ACODEC_DeInit(struct HAL_ACODEC_DEV *acodec)
{
    return HAL_OK;
}

/** @} */

/** @defgroup ACODEC_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Enable acodec.
 * @param  acodec: the handle of acodec.
 * @param  stream: AUDIO_STREAM_PLAYBACK or AUDIO_STREAM_CAPTURE.
 * @return HAL_Status
 */
HAL_Status HAL_ACODEC_Enable(struct HAL_ACODEC_DEV *acodec,
                             eAUDIO_streamType stream)
{
    if (stream == AUDIO_STREAM_CAPTURE) {
        ACODEC_ADC_Enable(acodec);
        ACODEC_TX_Start(acodec);
        ACODEC_PowerOn(acodec);

        return HAL_OK;
    } else {
        return HAL_INVAL;
    }
}

/**
 * @brief  Disable acodec.
 * @param  acodec: the handle of acodec.
 * @param  stream: AUDIO_STREAM_PLAYBACK or AUDIO_STREAM_CAPTURE.
 * @return HAL_Status
 */
HAL_Status HAL_ACODEC_Disable(struct HAL_ACODEC_DEV *acodec,
                              eAUDIO_streamType stream)
{
    if (stream == AUDIO_STREAM_CAPTURE) {
        ACODEC_TX_Stop(acodec);
        ACODEC_ADC_Disable(acodec);
        ACODEC_PowerOff(acodec);

        return HAL_OK;
    } else {
        return HAL_INVAL;
    }
}

/**
 * @brief  Config acodec.
 * @param  acodec: the handle of acodec.
 * @param  stream: AUDIO_STREAM_PLAYBACK or AUDIO_STREAM_CAPTURE.
 * @param  params: audio params.
 * @return HAL_Status
 */
HAL_Status HAL_ACODEC_Config(struct HAL_ACODEC_DEV *acodec,
                             eAUDIO_streamType stream,
                             struct AUDIO_PARAMS *params)
{
    struct ACODEC_REG *reg = acodec->pReg;
    unsigned int width, rate;

#ifdef HAL_CRU_MODULE_ENABLED
    int ratio;

    HAL_CRU_ClkEnable(acodec->mclk);
    if ((params->sampleRate % 1200) == 0) {
        HAL_CRU_ClkSetFreq(acodec->mclk, MCLK_REFERENCE_12000);
        ratio = MCLK_REFERENCE_12000 / MCLK_I2S_REFERENCE_DIV /
                (I2S_MCLK_FS * params->sampleRate);
        MODIFY_REG(reg->I2S_CKM,
                   ACODEC_I2S_CKM_SCK_DIV_MASK,
                   RK3506_SCK_DIV(ratio));
    } else if ((params->sampleRate % 11025) == 0) {
        HAL_CRU_ClkSetFreq(acodec->mclk, MCLK_REFERENCE_11025);
        ratio = MCLK_REFERENCE_11025 / MCLK_I2S_REFERENCE_DIV /
                (I2S_MCLK_FS * params->sampleRate);
        MODIFY_REG(reg->I2S_CKM,
                   ACODEC_I2S_CKM_SCK_DIV_MASK,
                   RK3506_SCK_DIV(ratio));
    } else if ((params->sampleRate % 8000) == 0) {
        HAL_CRU_ClkSetFreq(acodec->mclk, MCLK_REFERENCE_8000);
        ratio = MCLK_REFERENCE_8000 / MCLK_I2S_REFERENCE_DIV /
                (I2S_MCLK_FS * params->sampleRate);
        MODIFY_REG(reg->I2S_CKM,
                   ACODEC_I2S_CKM_SCK_DIV_MASK,
                   RK3506_SCK_DIV(ratio));
    }
#endif
    switch (params->sampleRate) {
    case 8000:
    case 11025:
    case 12000:
    case 64000:
    case 88200:
    case 96000:
    case 128000:
    case 176400:
    case 192000:
        MODIFY_REG(reg->ADC_FILTER,
                   RK3506_ACODEC_FILTER_MSK,
                   RK3506_ACODEC_FILTER_MODE1);
        break;
    case 16000:
    case 24000:
    case 22050:
        MODIFY_REG(reg->ADC_FILTER,
                   RK3506_ACODEC_FILTER_MSK,
                   RK3506_ACODEC_FILTER_MODE3);
        break;
    case 32000:
    case 44100:
    case 48000:
        MODIFY_REG(reg->ADC_FILTER,
                   RK3506_ACODEC_FILTER_MSK,
                   RK3506_ACODEC_FILTER_MODE2);
        break;
    default:
        MODIFY_REG(reg->ADC_FILTER,
                   RK3506_ACODEC_FILTER_MSK,
                   RK3506_ACODEC_FILTER_MODE2);
    }

    rate = SamplerateToBit(params->sampleRate);
    width = params->sampleBits;
    MODIFY_REG(reg->I2S_TXCR[0],
               ACODEC_I2S_TXCR0_VDW_TX_MASK,
               RK3506_VDW_TX(width));
    MODIFY_REG(reg->DIG_CLKEN,
               ACODEC_DIG_CLKEN_ADCSRT_MASK,
               RK3506_ADCSRT(rate));

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif
