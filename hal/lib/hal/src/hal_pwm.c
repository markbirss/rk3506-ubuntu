/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_PWM_MODULE_ENABLED

#include <strings.h>

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup PWM
 *  @{
 */

/** @defgroup PWM_How_To_Use How To Use
 *  @{

 The PWM HAL driver can be used as follows:

 - Declare a PWM_HANDLE handle structure, for example:
   ```
   struct PWM_HANDLE instance;
   ```
 - Invoke HAL_PWM_Init() API to initialize base address and clock frequency:
     - Base register address;
     - Check features support;
     - Input clock frequency.

 - Continous/oneshot mode
     - (optionally)Invoke HAL_PWM_SetWaveTable() to initialize wave table in wave generator mode;
     - (optionally)Invoke HAL_PWM_SetWave() to setup wave generator configurations;
     - (optionally)Invoke HAL_PWM_GlobalLock() to make configuration of channels unchangeable;
     - Invoke HAL_PWM_SetConfig() to configurate the output configurations;
     - (optionally)Invoke HAL_PWM_SetOutputOffset() to configurate the output offset;
     - Invoke HAL_PWM_Enable()/HAL_PWM_Disable() to start/stop PWM in continous/oneshot mode;
     - (optionally)Invoke HAL_PWM_GlobalUnlock() API to make sure channels update configuration simultaneously.

 - Capture mode
     - (optionally)Invoke HAL_PWM_EnableCaptureInt() API to enable interrupt of high/low effective cycles capture;
     - Invoke HAL_PWM_Enable() to start PWM in capture mode;
     - Invoke HAL_PWM_GetCaptureHighNs() to read effective high level duration in capture mode;
     - Invoke HAL_PWM_GetCaptureLowNs() to read effective low level duration in capture mode.
     - Invoke HAL_PWM_Disable() to stop PWM in capture mode;
     - (optionally)Invoke HAL_PWM_DisableCaptureInt() API to disable interrupt of high/low effective cycles capture.

 - Counter mode
     - Invoke HAL_PWM_EnableCounter() API to enable wave counter mode.
     - Invoke HAL_PWM_GetCounterRes() API to get counter result.
     - (optionally)Invoke HAL_PWM_ClearCounterRes() API to clear counter result.
     - Invoke HAL_PWM_DisableCounter() API to disable wave counter mode.

 - Frequency meter mode
     - Invoke HAL_PWM_EnableFreqMeter() API to enable freqency meter mode.
     - Invoke HAL_PWM_GetFreqMeterRes() API to get freqency meter result.
     - Invoke HAL_PWM_DisableFreqMeter() API to disable freqency meter mode.

 - Frequency meter mode
     - Invoke HAL_PWM_EnableFreqMeter() API to enable freqency meter mode.
     - Invoke HAL_PWM_GetFreqMeterRes() API to get freqency meter result.
     - Invoke HAL_PWM_DisableFreqMeter() API to disable freqency meter mode.

 - Invoke HAL_PWM_DeInit() if necessary.

 @} */

/** @defgroup PWM_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
/*
 * regs for pwm v4
 */
#define BIT(x)                 (1 << (x))
#define HIWORD_UPDATE(v, l, h) (((v) << (l)) | (HAL_GENMASK(h, l) << 16))

/* PWM_ENABLE */
#define PWM_ENABLE_V4         (0x3 << 0)
#define PWM_CLK_EN(v)         HIWORD_UPDATE(v, 0, 0)
#define PWM_EN(v)             HIWORD_UPDATE(v, 1, 1)
#define PWM_CTRL_UPDATE_EN(v) HIWORD_UPDATE(v, 2, 2)
#define PWM_GLOBAL_JOIN_EN(v) HIWORD_UPDATE(v, 4, 4)
/* PWM_CLK_CTRL */
#define CLK_PRESCALE(v)   HIWORD_UPDATE(v, 0, 2)
#define CLK_SCALE(v)      HIWORD_UPDATE(v, 4, 12)
#define CLK_SRC_SEL(v)    HIWORD_UPDATE(v, 13, 14)
#define CLK_GLOBAL_SEL(v) HIWORD_UPDATE(v, 15, 15)
/* PWM_CTRL */
#define PWM_MODE(v)            HIWORD_UPDATE(v, 0, 1)
#define ONESHOT_MODE           0
#define CONTINUOUS_MODE        1
#define CAPTURE_MODE           2
#define PWM_POLARITY(v)        HIWORD_UPDATE(v, 2, 3)
#define DUTY_NEGATIVE          (0 << 0)
#define DUTY_POSITIVE          (1 << 0)
#define INACTIVE_NEGATIVE      (0 << 1)
#define INACTIVE_POSITIVE      (1 << 1)
#define PWM_ALIGNED_MODE(v)    HIWORD_UPDATE(v, 4, 4)
#define PWM_ALIGNED_INVALID(v) HIWORD_UPDATE(v, 5, 5)
#define PWM_IN_SEL(v)          HIWORD_UPDATE(v, 6, 8)
/* INTSTS */
#define CAP_LPR_INT      BIT(PWM_INTSTS_CAP_LPC_INTSTS_SHIFT)
#define CAP_HPR_INT      BIT(PWM_INTSTS_CAP_HPC_INTSTS_SHIFT)
#define ONESHOT_END_INT  BIT(PWM_INTSTS_ONESHOT_END_INTSTS_SHIFT)
#define RELOAD_INT       BIT(PWM_INTSTS_RELOAD_INTSTS_SHIFT)
#define FREQ_INT         BIT(PWM_INTSTS_FREQ_INTSTS_SHIFT)
#define PWR_INT          BIT(PWM_INTSTS_PWR_INTSTS_SHIFT)
#define IR_TRANS_END_INT BIT(PWM_INTSTS_IR_TRANS_END_INTSTS_SHIFT)
#define WAVE_MAX_INT     BIT(PWM_INTSTS_WAVE_MAX_INTSTS_SHIFT)
#define WAVE_MIDDLE_INT  BIT(PWM_INTSTS_WAVE_MIDDLE_INTSTS_SHIFT)
/* INT_EN */
#define CAP_LPR_INT_EN(v)      HIWORD_UPDATE(v, 0, 0)
#define CAP_HPR_INT_EN(v)      HIWORD_UPDATE(v, 1, 1)
#define ONESHOT_END_INT_EN(v)  HIWORD_UPDATE(v, 2, 2)
#define RELOAD_INT_EN(v)       HIWORD_UPDATE(v, 3, 3)
#define FREQ_INT_EN(v)         HIWORD_UPDATE(v, 4, 4)
#define PWR_INT_EN(v)          HIWORD_UPDATE(v, 5, 5)
#define IR_TRANS_END_INT_EN(v) HIWORD_UPDATE(v, 6, 6)
#define WAVE_MAX_INT_EN(v)     HIWORD_UPDATE(v, 7, 7)
#define WAVE_MIDDLE_INT_EN(v)  HIWORD_UPDATE(v, 8, 8)
/* WAVE_CTRL */
#define WAVE_DUTY_EN(v)        HIWORD_UPDATE(v, 0, 0)
#define WAVE_PERIOD_EN(v)      HIWORD_UPDATE(v, 1, 1)
#define WAVE_WIDTH_MODE(v)     HIWORD_UPDATE(v, 2, 2)
#define WAVE_UPDATE_MODE(v)    HIWORD_UPDATE(v, 3, 3)
#define WAVE_MEM_CLK_SEL(v)    HIWORD_UPDATE(v, 4, 5)
#define WAVE_DUTY_AMPLIFY(v)   HIWORD_UPDATE(v, 6, 10)
#define WAVE_PERIOD_AMPLIFY(v) HIWORD_UPDATE(v, 11, 15)
/* GLOBAL CTRL */
#define GLOBAL_PWM_EN(v)        HIWORD_UPDATE(v, 0, 0)
#define GLOBAL_PWM_UPDATE_EN(v) HIWORD_UPDATE(v, 1, 1)
/* PWRMATCH_CTRL */
#define PWRKEY_ENABLE(v)       HIWORD_UPDATE(v, 0, 0)
#define PWRKEY_POLARITY(v)     HIWORD_UPDATE(v, 1, 1)
#define PWRKEY_CAPTURE_CTRL(v) HIWORD_UPDATE(v, 2, 2)
#define PWRKEY_INT_CTRL(v)     HIWORD_UPDATE(v, 3, 3)
/* FREQ_CTRL */
#define FREQ_EN(v)              HIWORD_UPDATE(v, 0, 0)
#define FREQ_CLK_SEL(v)         HIWORD_UPDATE(v, 1, 2)
#define FREQ_CHANNEL_SEL(v)     HIWORD_UPDATE(v, 3, 5)
#define FREQ_CLK_SWITCH_MODE(v) HIWORD_UPDATE(v, 6, 6)
#define FREQ_TIMIER_CLK_SEL(v)  HIWORD_UPDATE(v, 7, 7)
/* COUNTER_CTRL */
#define COUNTER_EN(v)          HIWORD_UPDATE(v, 0, 0)
#define COUNTER_CLK_SEL(v)     HIWORD_UPDATE(v, 1, 2)
#define COUNTER_CHANNEL_SEL(v) HIWORD_UPDATE(v, 3, 5)
#define COUNTER_CLR(v)         HIWORD_UPDATE(v, 6, 6)
#else
/*
 * regs for pwm v1-v3
 */
#define PWM_CNT_REG(pPWM, ch)    (pPWM->pReg->CHANNELS[ch].CNT)
#define PWM_PERIOD_REG(pPWM, ch) (pPWM->pReg->CHANNELS[ch].PERIOD_HPR)
#define PWM_DUTY_REG(pPWM, ch)   (pPWM->pReg->CHANNELS[ch].DUTY_LPR)
#define PWM_CTRL_REG(pPWM, ch)   (pPWM->pReg->CHANNELS[ch].CTRL)

#define PWM_INT_EN(ch)     (1 << (ch))
#define PWM_PWR_INT_EN(ch) (1 << ((ch) + 4 ))

#define PWM_DISABLE (0 << PWM_PWM0_CTRL_PWM_EN_SHIFT)
#define PWM_ENABLE  (1 << PWM_PWM0_CTRL_PWM_EN_SHIFT)

#define PWM_MODE_SHIFT (1)
#define PWM_MODE_MASK  (0x3U << PWM_MODE_SHIFT)

#define PWM_DUTY_POSTIVE  (1 << PWM_PWM0_CTRL_DUTY_POL_SHIFT)
#define PWM_DUTY_NEGATIVE (0 << PWM_PWM0_CTRL_DUTY_POL_SHIFT)
#define PWM_DUTY_MASK     (1 << 3)

#define PWM_INACTIVE_POSTIVE  (1 << PWM_PWM0_CTRL_INACTIVE_POL_SHIFT)
#define PWM_INACTIVE_NEGATIVE (0 << PWM_PWM0_CTRL_INACTIVE_POL_SHIFT)
#define PWM_INACTIVE_MASK     (1 << 4)

#define PWM_OUTPUT_LEFT       (0 << PWM_PWM0_CTRL_OUTPUT_MODE_SHIFT)
#define PWM_OUTPUT_CENTER     (1 << PWM_PWM0_CTRL_OUTPUT_MODE_SHIFT)
#define PWM_ALIGNED_MODE_MASK (1 << PWM_PWM0_CTRL_OUTPUT_MODE_SHIFT)

#define PWM_UNLOCK (0 << PWM_PWM0_CTRL_CONLOCK_SHIFT)
#define PWM_LOCK   (1 << PWM_PWM0_CTRL_CONLOCK_SHIFT)

#define PWM_LP_DISABLE (0 << PWM_PWM0_CTRL_FORCE_CLK_EN_SHIFT)
#define PWM_LP_ENABLE  (1 << PWM_PWM0_CTRL_FORCE_CLK_EN_SHIFT)

#define PWM_SEL_SRC_CLK   (0 << PWM_PWM0_CTRL_CLK_SEL_SHIFT)
#define PWM_SEL_SCALE_CLK (1 << PWM_PWM0_CTRL_CLK_SEL_SHIFT)

#ifdef PWM_PWM0_CTRL_ALIGNED_VLD_N_SHIFT
#define PWM_ALIGNED_VALID      (0 << PWM_PWM0_CTRL_ALIGNED_VLD_N_SHIFT)
#define PWM_ALIGNED_INVALID    (1 << PWM_PWM0_CTRL_ALIGNED_VLD_N_SHIFT)
#define PWM_ALIGNED_VALID_MASK (1 << PWM_PWM0_CTRL_ALIGNED_VLD_N_SHIFT)
#endif

#define PWM_CTRL_SCALE_SHIFT (PWM_PWM0_CTRL_SCALE_SHIFT)
#define PWM_CTRL_SCALE_MASK  (PWM_PWM0_CTRL_SCALE_MASK)

#define PWM_PWRMATCH_MAX_SHIFT (PWM_PWRMATCH_LPRE_CNT_MIN_SHIFT)

#ifdef PWM_FILTER_CTRL_PWM0_GLOBAL_LOCK_SHIFT
#define PWM_GLOBAL_LOCK_SHIFT (PWM_FILTER_CTRL_PWM0_GLOBAL_LOCK_SHIFT)
#define PWM_GLOBAL_LOCK_MASK  (PWM_FILTER_CTRL_PWM0_GLOBAL_LOCK_MASK | \
                               PWM_FILTER_CTRL_PWM1_GLOBAL_LOCK_MASK | \
                               PWM_FILTER_CTRL_PWM2_GLOBAL_LOCK_MASK | \
                               PWM_FILTER_CTRL_PWM3_GLOBAL_LOCK_MASK)
#endif

#ifdef PWM_PWM0_CAPTURE_CNT_EN_OFFSET
#define PWM_CAPTURE_EN_SHIFT (PWM_PWM0_CAPTURE_CNT_EN_POS_CAPTURE_EN_SHIFT)
#define PWM_CAPTURE_EN_MASK  (PWM_PWM0_CAPTURE_CNT_EN_POS_CAPTURE_EN_MASK | \
                              PWM_PWM0_CAPTURE_CNT_EN_NEG_CAPTURE_EN_MASK)
#endif
#endif

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/**
  * @brief  Check the base parameters of PWM funcs.
  * @param  pPWM: pointer to a PWM_HANDLE structure that contains
  *               the information for PWM module.
  * @param  channel: PWM channel.
  */
__STATIC_FORCEINLINE void Hal_PWM_ParaCheck(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    HAL_ASSERT(pPWM != NULL);
#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
    HAL_ASSERT(channel < pPWM->channelNum);
#else
    HAL_ASSERT(channel < HAL_PWM_NUM_CHANNELS);
#endif
}

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup PWM_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
/**
  * @brief  Handle PWM interrupt for capture/oneshot mode.
  * @param  pPWM: pointer to a PWM_HANDLE structure that contains
  *               the information for PWM module.
  * @param  channel: PWM channel.
  * @retval HAL status
  */
HAL_Status HAL_PWM_ChannelIRQHandler(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    struct PWM_REG *reg;
    uint32_t status;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    status = READ_REG(reg->INTSTS);
    if (status & CAP_LPR_INT) {
        WRITE_REG(reg->INTSTS, CAP_LPR_INT);
        pPWM->pChHandle[channel].result.negCycles = READ_REG(reg->LPC);
    }

    if (status & CAP_HPR_INT) {
        WRITE_REG(reg->INTSTS, CAP_HPR_INT);
        pPWM->pChHandle[channel].result.posCycles = READ_REG(reg->HPC);
    }

    return HAL_OK;
}

/**
 * @brief  Configurate PWM mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  config: Configuration for PWM.
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetConfig(struct PWM_HANDLE *pPWM, uint8_t channel,
                             const struct HAL_PWM_CONFIG *config)
{
    struct PWM_REG *reg;
    uint32_t period, duty;
    uint64_t freqKhz;
    uint64_t tmp;
    uint32_t scaler;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    HAL_ASSERT(config != NULL);
    HAL_DBG("channel=%d, period_ns=%" PRId32 ", duty_ns=%" PRId32 "\n",
            channel, config->periodNS, config->dutyNS);

    scaler = pPWM->scaler ? pPWM->scaler * 2 : 1;
    freqKhz = pPWM->freq / 1000;
    tmp = (uint64_t)scaler * 1000000;

    period = HAL_DivU64((uint64_t)freqKhz * config->periodNS, tmp);
    duty = HAL_DivU64((uint64_t)freqKhz * config->dutyNS, tmp);

    WRITE_REG(reg->PERIOD, period);
    WRITE_REG(reg->DUTY, duty);

    if (config->polarity) {
        WRITE_REG(reg->CTRL, PWM_POLARITY(DUTY_NEGATIVE | INACTIVE_POSITIVE));
    } else {
        WRITE_REG(reg->CTRL, PWM_POLARITY(DUTY_POSITIVE | INACTIVE_NEGATIVE));
    }

    switch (config->alignedMode) {
    case HAL_PWM_LEFT_ALIGNED:
        WRITE_REG(reg->CTRL, PWM_ALIGNED_INVALID(true) | PWM_ALIGNED_MODE(HAL_PWM_LEFT_ALIGNED));
        break;
    case HAL_PWM_CENTER_ALIGNED:
        WRITE_REG(reg->CTRL, PWM_ALIGNED_INVALID(true) | PWM_ALIGNED_MODE(HAL_PWM_CENTER_ALIGNED));
        break;
    case HAL_PWM_UNALIGNED:
    default:
        WRITE_REG(reg->CTRL, PWM_ALIGNED_INVALID(false));
        break;
    }

    WRITE_REG(reg->ENABLE, PWM_CTRL_UPDATE_EN(true));

    HAL_DBG("channel=%d, period=%" PRIu32 ", duty=%" PRIu32 ", polarity=%d\n",
            channel, period, duty, config->polarity);

    return HAL_OK;
}

/**
 * @brief  Configurate PWM oneshot count.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  count: (count + 1)repeated effective periods of output waveform
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetOneshot(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t count)
{
    struct PWM_REG *reg;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    HAL_DBG("Oneshot count=%" PRId32 "\n", count);

    WRITE_REG(reg->RPT, count);

    return HAL_OK;
}

/**
 * @brief  Configurate PWM captured frequency.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  freq: PWM use the frequency to capture data
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetCapturedFreq(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t freq)
{
    struct PWM_REG *reg;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    HAL_ASSERT(freq != 0);
    HAL_DBG("Captured freq=%" PRId32 "\n", freq);

    WRITE_REG(reg->CLK_CTRL, CLK_SCALE(pPWM->freq / (2 * freq)));

    return HAL_OK;
}

/**
 * @brief  Configurate PWM matched setting.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  data: matching configuration.
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetMatch(struct PWM_HANDLE *pPWM, uint8_t channel, const struct PWM_MATCH *data)
{
    struct PWM_REG *reg;
    uint32_t val;
    uint8_t i;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    HAL_ASSERT(data != NULL);
    HAL_ASSERT(data->matchCount <= PWM_PWRMATCH_MAX_COUNT);

    val = BIT(channel) << PWM_PWRMATCH_ARBITER_PWRKEY_READ_LOCK_SHIFT |
          BIT(channel) << PWM_PWRMATCH_ARBITER_PWRKEY_GRANT_SHIFT;
    WRITE_REG(reg->PWRMATCH_ARBITER, val);

    /* preloader low */
    WRITE_REG(reg->PWRMATCH_LPRE, data->lpreMin | (data->lpreMax << PWM_PWRMATCH_LPRE_CNT_MAX_SHIFT));
    /* preloader high */
    WRITE_REG(reg->PWRMATCH_HPRE, data->hpreMin | (data->hpreMax << PWM_PWRMATCH_HPRE_CNT_MAX_SHIFT));
    /* logic 0/1 low */
    WRITE_REG(reg->PWRMATCH_LD, data->ldMin | (data->ldMax << PWM_PWRMATCH_LD_CNT_MAX_SHIFT));
    /* logic 0 high */
    WRITE_REG(reg->PWRMATCH_HD_ZERO, data->hdZeroMin | (data->hdZeroMax << PWM_PWRMATCH_HD_ZERO_CNT_MAX_SHIFT));
    /* logic 1 high */
    WRITE_REG(reg->PWRMATCH_HD_ONE, data->hdOneMin | (data->hdOneMax << PWM_PWRMATCH_HD_ONE_CNT_MAX_SHIFT));

    for (i = 0; i < data->matchCount; i++) {
        WRITE_REG(reg->PWRMATCH_VALUE[i], data->match[i]);
    }

    /* Enable pwr irq */
    WRITE_REG(reg->INT_EN, PWR_INT_EN(true));
    /* Enable pwr */
    WRITE_REG(reg->PWRMATCH_CTRL, PWRKEY_ENABLE(true));

    return HAL_OK;
}

/**
 * @brief  Configurate PWM channel output offset.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  offsetNS: PWM channel output offset configuration.
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetOutputOffset(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t offsetNS)
{
    struct PWM_REG *reg;
    uint32_t period, duty;
    uint32_t outOffset, outOffsetMax;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    period = READ_REG(reg->PERIOD);
    duty = READ_REG(reg->DUTY);

    outOffset = HAL_DivU64((uint64_t)pPWM->freq * offsetNS, 1000000000);
    outOffsetMax = period - duty;
    if (outOffset < 0 || outOffset > outOffsetMax) {
        return HAL_INVAL;
    }

    HAL_DBG("channel=%d, offsetNS=%" PRId32 "\n", channel, offsetNS);

    WRITE_REG(reg->OFFSET, outOffset);

    return HAL_OK;
}

/**
 * @brief  Enable PWM global lock.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channelMask: PWM channel mask, such as 0x5 indicates
 *                      channel0 and channel2.
 * @retval HAL status
 */
HAL_Status HAL_PWM_GlobalLock(struct PWM_HANDLE *pPWM, uint8_t channelMask)
{
    struct PWM_REG *reg;
    uint32_t val, channelId;

    HAL_ASSERT(pPWM != NULL);
    HAL_ASSERT(channelMask <= (PWM_GLOBAL_ARBITER_GLOBAL_GRANT_MASK >> PWM_GLOBAL_ARBITER_GLOBAL_GRANT_SHIFT));
    HAL_DBG("globalMask=0x%04x, global lock\n", channelMask);

    if (!channelMask && pPWM->globalMask) {
        HAL_PWM_GlobalUnlock(pPWM, channelMask);

        return HAL_OK;
    }
    pPWM->globalMask = channelMask;

    while (ffs(channelMask)) {
        channelId = ffs(channelMask) - 1;
        reg = pPWM->pChHandle[channelId].pReg;

        WRITE_REG(reg->ENABLE, PWM_GLOBAL_JOIN_EN(true));
        WRITE_REG(reg->CLK_CTRL, CLK_GLOBAL_SEL(true));

        if (!pPWM->globalGrantMask) {
            val = BIT(channelId) << PWM_GLOBAL_ARBITER_GLOBAL_READ_LOCK_SHIFT |
                  BIT(channelId) << PWM_GLOBAL_ARBITER_GLOBAL_GRANT_SHIFT;
            WRITE_REG(reg->GLOBAL_ARBITER, val);

            pPWM->globalGrantMask = BIT(channelId);
        }

        channelMask &= ~BIT(channelId);
    }

    return HAL_OK;
}

/**
 * @brief  Disable PWM global lock.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channelMask: PWM channel mask, such as 0x5 indicates
 *                      channel0 and channel2.
 * @retval HAL status
 */
HAL_Status HAL_PWM_GlobalUnlock(struct PWM_HANDLE *pPWM, uint8_t channelMask)
{
    struct PWM_REG *reg;
    uint32_t channelId, grantId;

    HAL_ASSERT(pPWM != NULL);
    HAL_DBG("globalMask=0x%04x, globalGrantMask=0x%04x, global unlock\n", pPWM->globalMask, pPWM->globalGrantMask);

    if (!pPWM->globalMask || !pPWM->globalGrantMask) {
        return HAL_INVAL;
    }
    grantId = ffs(pPWM->globalGrantMask) - 1;

    while (ffs(pPWM->globalMask)) {
        channelId = ffs(pPWM->globalMask) - 1;
        reg = pPWM->pChHandle[channelId].pReg;

        if (channelId == grantId) {
            WRITE_REG(reg->GLOBAL_ARBITER, 0);

            pPWM->globalGrantMask = 0;
        }

        WRITE_REG(reg->ENABLE, PWM_GLOBAL_JOIN_EN(false));
        WRITE_REG(reg->CLK_CTRL, CLK_GLOBAL_SEL(false));

        pPWM->globalMask &= ~BIT(channelId);
    }

    return HAL_OK;
}

/**
 * @brief  PWM global update.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @retval HAL status
 */
HAL_Status HAL_PWM_GlobalUpdate(struct PWM_HANDLE *pPWM)
{
    struct PWM_REG *reg;
    uint32_t channelId;

    HAL_ASSERT(pPWM != NULL);
    HAL_DBG("globalMask=0x%04x, globalGrantMask=0x%04x, global update\n", pPWM->globalMask, pPWM->globalGrantMask);

    if (!pPWM->globalMask || !pPWM->globalGrantMask) {
        return HAL_INVAL;
    }
    channelId = ffs(pPWM->globalGrantMask) - 1;
    reg = pPWM->pChHandle[channelId].pReg;

    WRITE_REG(reg->GLOBAL_CTRL, GLOBAL_PWM_UPDATE_EN(true));

    return HAL_OK;
}

/**
 * @brief  PWM global enable.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @retval HAL status
 */
HAL_Status HAL_PWM_GlobalEnable(struct PWM_HANDLE *pPWM)
{
    struct PWM_REG *reg;
    uint32_t channelId;

    HAL_ASSERT(pPWM != NULL);
    HAL_DBG("globalMask=0x%04x, globalGrantMask=0x%04x, global update\n", pPWM->globalMask, pPWM->globalGrantMask);

    if (!pPWM->globalMask || !pPWM->globalGrantMask) {
        return HAL_INVAL;
    }
    channelId = ffs(pPWM->globalGrantMask) - 1;
    reg = pPWM->pChHandle[channelId].pReg;

    WRITE_REG(reg->ENABLE, PWM_CLK_EN(true));
    WRITE_REG(reg->GLOBAL_CTRL, GLOBAL_PWM_EN(true));

    return HAL_OK;
}

/**
 * @brief  PWM global disable.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @retval HAL status
 */
HAL_Status HAL_PWM_GlobalDisable(struct PWM_HANDLE *pPWM)
{
    struct PWM_REG *reg;
    uint32_t channelId;

    HAL_ASSERT(pPWM != NULL);
    HAL_DBG("globalMask=0x%04x, globalGrantMask=0x%04x, global update\n", pPWM->globalMask, pPWM->globalGrantMask);

    if (!pPWM->globalMask || !pPWM->globalGrantMask) {
        return HAL_INVAL;
    }
    channelId = ffs(pPWM->globalGrantMask) - 1;
    reg = pPWM->pChHandle[channelId].pReg;

    WRITE_REG(reg->ENABLE, PWM_CLK_EN(false));
    WRITE_REG(reg->GLOBAL_CTRL, GLOBAL_PWM_UPDATE_EN(false));

    return HAL_OK;
}

/**
 * @brief  Enable PWM capture interrupt.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  mode: PWM capture interrupt mode(HPR/LPR).
 * @retval HAL status
 */
HAL_Status HAL_PWM_EnableCaptureInt(struct PWM_HANDLE *pPWM, uint8_t channel,
                                    ePWM_captureIntMode mode)
{
    struct PWM_REG *reg;
    uint32_t val;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    HAL_DBG("channel=%d, capture interrupt mode=%d enable\n", channel, mode);

    val = CAP_LPR_INT_EN(!!(mode & HAL_PWM_CAP_LPR_INT)) |
          CAP_HPR_INT_EN(!!(mode & HAL_PWM_CAP_HPR_INT)) |
          PWM_IN_SEL(channel);
    WRITE_REG(reg->INT_EN, val);

    return HAL_OK;
}

/**
 * @brief  Disable PWM capture interrupt.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval HAL status
 */
HAL_Status HAL_PWM_DisableCaptureInt(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    struct PWM_REG *reg;
    uint32_t val;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    HAL_DBG("channel=%d, capture interrupt disable\n", channel);

    val = CAP_LPR_INT_EN(false) |
          CAP_HPR_INT_EN(false) |
          PWM_IN_SEL(0);
    WRITE_REG(reg->INT_EN, val);

    return HAL_OK;
}

/**
 * @brief  Get effective high level duration in capture mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval high level duration(in nanoseconds).
 */
uint64_t HAL_PWM_GetCaptureHighNs(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    struct PWM_REG *reg;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    return HAL_DivU64((uint64_t)(READ_REG(reg->HPC) * 1000000000ULL), pPWM->freq);
}

/**
 * @brief  Get effective low level duration in capture mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval low level duration(in nanoseconds).
 */
uint64_t HAL_PWM_GetCaptureLowNs(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    struct PWM_REG *reg;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    return HAL_DivU64((uint64_t)(READ_REG(reg->LPC) * 1000000000ULL), pPWM->freq);
}

/**
 * @brief  Get PWM mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval ePWM_Mode
 */
ePWM_Mode HAL_PWM_GetMode(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    struct PWM_REG *reg;
    uint32_t ctrl;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    HAL_DBG("channel=%d\n", channel);

    ctrl = READ_REG(reg->CTRL);

    return (ePWM_Mode)((ctrl >> PWM_CTRL_PWM_MODE_SHIFT) & PWM_CTRL_PWM_MODE_MASK);
}

/**
 * @brief  Enable PWM.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  mode: Current mode on for PWM.
 * @retval HAL status
 */
HAL_Status HAL_PWM_Enable(struct PWM_HANDLE *pPWM, uint8_t channel, ePWM_Mode mode)
{
    struct PWM_REG *reg;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    HAL_DBG("Enable channel=%d\n", channel);

    pPWM->pChHandle[channel].mode = mode;

    if (mode == HAL_PWM_ONE_SHOT) {
        WRITE_REG(reg->CTRL, PWM_MODE(ONESHOT_MODE) | PWM_ALIGNED_INVALID(true));
        WRITE_REG(reg->INT_EN, ONESHOT_END_INT_EN(true));
    } else if (mode == HAL_PWM_CONTINUOUS) {
        WRITE_REG(reg->CTRL, PWM_MODE(CONTINUOUS_MODE) | PWM_ALIGNED_INVALID(false));
    } else if (mode == HAL_PWM_CAPTURE) {
        WRITE_REG(reg->CTRL, PWM_MODE(CAPTURE_MODE) | PWM_ALIGNED_INVALID(false));
    }

    WRITE_REG(reg->ENABLE, PWM_EN(true) | PWM_CLK_EN(true));

    return HAL_OK;
}

/**
 * @brief  Disable PWM.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval HAL status
 */
HAL_Status HAL_PWM_Disable(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    struct PWM_REG *reg;

    Hal_PWM_ParaCheck(pPWM, channel);
    reg = pPWM->pChHandle[channel].pReg;

    HAL_DBG("Disable channel=%d\n", channel);

    WRITE_REG(reg->INT_EN, ONESHOT_END_INT_EN(false));
    WRITE_REG(reg->ENABLE, PWM_EN(false) | PWM_CLK_EN(false));

    return HAL_OK;
}

/**
 * @brief  Enable PWM wave counter mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval HAL status
 */
HAL_Status HAL_PWM_EnableCounter(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    struct PWM_REG *reg;
    uint32_t val, arbiter;

    Hal_PWM_ParaCheck(pPWM, channel);

    if (!pPWM->counterSupport) {
        HAL_DBG("channel=%d, unsupported counter mode\n", channel);

        return HAL_INVAL;
    }
    reg = pPWM->pChHandle[channel].pReg;

    HAL_DBG("channel=%d, wave counter enable\n", channel);

    arbiter = BIT(channel) << PWM_COUNTER_ARBITER_COUNTER_READ_LOCK_SHIFT |
              BIT(channel) << PWM_COUNTER_ARBITER_COUNTER_GRANT_SHIFT;
    WRITE_REG(reg->COUNTER_ARBITER, arbiter);

    val = READ_REG(reg->COUNTER_ARBITER);
    if (!(val & arbiter)) {
        HAL_DBG_ERR("failed to abtain counter arbitration for channel%d\n", channel);

        return HAL_INVAL;
    }

    WRITE_REG(reg->COUNTER_CTRL, COUNTER_EN(true) | COUNTER_CHANNEL_SEL(channel));

    return HAL_OK;
}

/**
 * @brief  Disable PWM wave counter mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval HAL status
 */
HAL_Status HAL_PWM_DisableCounter(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    struct PWM_REG *reg;

    Hal_PWM_ParaCheck(pPWM, channel);

    if (!pPWM->counterSupport) {
        HAL_DBG("channel=%d, unsupported counter mode\n", channel);

        return HAL_INVAL;
    }
    reg = pPWM->pChHandle[channel].pReg;

    HAL_DBG("channel=%d, wave counter disable\n", channel);

    WRITE_REG(reg->COUNTER_CTRL, COUNTER_EN(false) | COUNTER_CHANNEL_SEL(0));

    WRITE_REG(reg->COUNTER_ARBITER, 0);

    return HAL_OK;
}

/**
 * @brief  Get PWM wave counter result.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  cntRes: counter result.
 * @retval HAL status
 */
HAL_Status HAL_PWM_GetCounterRes(struct PWM_HANDLE *pPWM, uint8_t channel, uint64_t *cntRes)
{
    struct PWM_REG *reg;
    uint64_t low, high;

    Hal_PWM_ParaCheck(pPWM, channel);

    if (!pPWM->counterSupport) {
        HAL_DBG("channel=%d, unsupported counter mode\n", channel);

        return HAL_INVAL;
    }
    reg = pPWM->pChHandle[channel].pReg;

    HAL_ASSERT(cntRes != NULL);

    low = READ_REG(reg->COUNTER_LOW);
    high = READ_REG(reg->COUNTER_HIGH);

    *cntRes = (high << 32) | low;
    if (!*cntRes) {
        return HAL_INVAL;
    }

    return HAL_OK;
}

/**
 * @brief  Clear PWM wave counter result.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval HAL status
 */
HAL_Status HAL_PWM_ClearCounterRes(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    struct PWM_REG *reg;

    Hal_PWM_ParaCheck(pPWM, channel);

    if (!pPWM->counterSupport) {
        HAL_DBG("channel=%d, unsupported counter mode\n", channel);

        return HAL_INVAL;
    }
    reg = pPWM->pChHandle[channel].pReg;

    HAL_DBG("channel=%d, clear wave counter result\n", channel);

    WRITE_REG(reg->COUNTER_CTRL, COUNTER_CLR(true));

    return HAL_OK;
}

/**
 * @brief  Enable PWM frequency meter mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  delayMs: time to wait, in milliseconds, before getting frequency meter result.
 * @retval HAL status
 */
HAL_Status HAL_PWM_EnableFreqMeter(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t delayMs)
{
    struct PWM_REG *reg;
    uint64_t timerVal, div;
    uint32_t val, arbiter;

    Hal_PWM_ParaCheck(pPWM, channel);

    if (!pPWM->freqMeterSupport) {
        HAL_DBG("channel=%d, unsupported frequency meter mode\n", channel);

        return HAL_INVAL;
    }
    reg = pPWM->pChHandle[channel].pReg;

    HAL_ASSERT(delayMs != 0);
    HAL_DBG("channel=%d, frequency meter enable\n", channel);

    arbiter = BIT(channel) << PWM_FREQ_ARBITER_FREQ_READ_LOCK_SHIFT |
              BIT(channel) << PWM_FREQ_ARBITER_FREQ_GRANT_SHIFT;
    WRITE_REG(reg->FREQ_ARBITER, arbiter);

    val = READ_REG(reg->FREQ_ARBITER);
    if (!(val & arbiter)) {
        HAL_DBG_ERR("failed to abtain frequency meter arbitration for channel%d\n", channel);

        return HAL_INVAL;
    }

    div = (uint64_t)pPWM->freq * delayMs;
    timerVal = HAL_DivU64(div, 1000);
    WRITE_REG(reg->FREQ_TIMER_VALUE, timerVal);

    WRITE_REG(reg->INT_EN, FREQ_INT_EN(true));
    WRITE_REG(reg->FREQ_CTRL, FREQ_EN(true) | FREQ_CHANNEL_SEL(channel));

    return HAL_OK;
}

/**
 * @brief  Disable PWM frequency meter mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval HAL status
 */
HAL_Status HAL_PWM_DisableFreqMeter(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    struct PWM_REG *reg;

    Hal_PWM_ParaCheck(pPWM, channel);

    if (!pPWM->freqMeterSupport) {
        HAL_DBG("channel=%d, unsupported frequency meter mode\n", channel);

        return HAL_INVAL;
    }
    reg = pPWM->pChHandle[channel].pReg;

    HAL_DBG("channel=%d, frequency meter disable\n", channel);

    WRITE_REG(reg->FREQ_TIMER_VALUE, 0);
    WRITE_REG(reg->INT_EN, FREQ_INT_EN(false));
    WRITE_REG(reg->FREQ_CTRL, FREQ_EN(false) | FREQ_CHANNEL_SEL(0));

    WRITE_REG(reg->FREQ_ARBITER, 0);

    return HAL_OK;
}

/**
 * @brief  Set PWM frequency meter mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  delayMs: time to wait, in milliseconds, before getting frequency meter result.
 * @param  freqHz: parameter in Hz to fill with frequency meter result.
 * @retval HAL status
 */
HAL_Status HAL_PWM_GetFreqMeterRes(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t delayMs, uint32_t *freqHz)
{
    struct PWM_REG *reg;
    uint32_t status;
    uint32_t freqRes;
    uint32_t freqTimer;

    Hal_PWM_ParaCheck(pPWM, channel);

    if (!pPWM->freqMeterSupport) {
        HAL_DBG("channel=%d, unsupported frequency meter mode\n", channel);

        return HAL_INVAL;
    }
    reg = pPWM->pChHandle[channel].pReg;

    HAL_ASSERT(delayMs != 0);
    HAL_ASSERT(freqHz != NULL);

    HAL_DelayMs(delayMs);

    status = READ_REG(reg->INTSTS);
    if (!(status & FREQ_INT)) {
        return HAL_TIMEOUT;
    }
    WRITE_REG(reg->INTSTS, FREQ_INT);

    freqRes = READ_REG(reg->FREQ_RESULT_VALUE);
    freqTimer = READ_REG(reg->FREQ_TIMER_VALUE);
    *freqHz = HAL_DivU64((uint64_t)pPWM->freq * freqRes, freqTimer);
    if (!*freqHz) {
        return HAL_INVAL;
    }

    HAL_DBG("channel=%d, frequency meter get result: %" PRId32 "Hz\n", channel, *freqHz);

    return HAL_OK;
}

#ifdef PWM_WAVE_MEM_ARBITER_WAVE_MEM_GRANT_SHIFT
/**
 * @brief  Set PWM wave table in wave generator mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  table: pointer to PWM_WAVE_TABLE structure that contains
 *                the wave table information.
 * @param  widthMode: wave table element width.
 * @param  clkRate: the dclk rate in wave generator mode
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetWaveTable(struct PWM_HANDLE *pPWM, uint8_t channel, struct PWM_WAVE_TABLE *table,
                                ePWM_waveTableWidthMode widthMode, uint64_t clkRate)
{
    struct PWM_REG *reg;
    uint64_t tableVal;
    uint64_t freqKhz;
    uint64_t div;
    uint64_t tmp;
    uint32_t val, arbiter;
    uint32_t scaler;
    uint16_t tableMax;
    uint16_t i;

    Hal_PWM_ParaCheck(pPWM, channel);

    if (!pPWM->waveSupport) {
        HAL_DBG("channel=%d, unsupported wave generator mode\n", channel);

        return HAL_INVAL;
    }
    reg = pPWM->pChHandle[channel].pReg;

    HAL_ASSERT(table != NULL);
    HAL_ASSERT(clkRate != 0);
    HAL_DBG("channel=%d, wave table init\n", channel);

    if (widthMode == HAL_PWM_WAVE_TABLE_16BITS_WIDTH) {
        tableMax = PWM_WAVE_TABEL_MAX / 2;
    } else {
        tableMax = PWM_WAVE_TABEL_MAX;
    }

    pPWM->scaler = HAL_DivU64(pPWM->freq, clkRate * 2);
    if (pPWM->scaler > 256) {
        HAL_DBG_ERR("unsupported scale factor %" PRId32 "(max: 512) for for channel%d\n", pPWM->scaler * 2, channel);

        return HAL_INVAL;
    }
    scaler = pPWM->scaler ? pPWM->scaler * 2 : 1;

    if (!table->data || table->offset > tableMax || table->len > tableMax) {
        HAL_DBG_ERR("the wave table to set is out of range for channel%d\n", channel);

        return HAL_INVAL;
    }

    arbiter = BIT(channel) << PWM_WAVE_MEM_ARBITER_WAVE_MEM_READ_LOCK_SHIFT |
              BIT(channel) << PWM_WAVE_MEM_ARBITER_WAVE_MEM_GRANT_SHIFT;
    WRITE_REG(reg->WAVE_MEM_ARBITER, arbiter);

    val = READ_REG(reg->WAVE_MEM_ARBITER);
    if (!(val & arbiter)) {
        HAL_DBG_ERR("failed to abtain wave memory arbitration for channel%d\n", channel);

        return HAL_INVAL;
    }

    freqKhz = pPWM->freq / 1000;
    if (widthMode == HAL_PWM_WAVE_TABLE_16BITS_WIDTH) {
        for (i = 0; i < table->len; i++) {
            div = (uint64_t)freqKhz * table->data[i];
            tmp = (uint64_t)scaler * 1000000;
            tableVal = HAL_DivU64(div, tmp);
            *(volatile uint32_t *)(&reg->WAVE_MEM + (table->offset + i) * 2) = tableVal & 0xff;
            while (!(READ_REG(reg->WAVE_MEM_STATUS) & BIT(PWM_WAVE_MEM_STATUS_ACCESS_DONE_SHIFT))) {
                ;
            }

            *(volatile uint32_t *)(&reg->WAVE_MEM + ((table->offset + i) * 2 + 1)) = (tableVal >> 8) & 0xff;
            while (!(READ_REG(reg->WAVE_MEM_STATUS) & BIT(PWM_WAVE_MEM_STATUS_ACCESS_DONE_SHIFT))) {
                ;
            }
        }
    } else {
        for (i = 0; i < table->len; i++) {
            div = (uint64_t)freqKhz * table->data[i];
            tmp = (uint64_t)scaler * 1000000;
            tableVal = HAL_DivU64(div, tmp);
            *(volatile uint32_t *)(&reg->WAVE_MEM + (table->offset + i)) = tableVal;
            while (!(READ_REG(reg->WAVE_MEM_STATUS) & BIT(PWM_WAVE_MEM_STATUS_ACCESS_DONE_SHIFT))) {
                ;
            }
        }
    }

    WRITE_REG(reg->WAVE_MEM_ARBITER, 0);

    return HAL_OK;
}

/**
 * @brief  Set PWM wave config in wave generator mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  config: pointer to PWM_WAVE_CONFIG structure that contains
 *                the wave mode configurations.
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetWave(struct PWM_HANDLE *pPWM, uint8_t channel, struct PWM_WAVE_CONFIG *config)
{
    struct PWM_REG *reg;
    uint32_t ctrl = 0;
    uint32_t maxVal = 0;
    uint32_t minVal = 0;
    uint32_t offset = 0;
    uint32_t middle = 0;
    uint32_t rpt = 0;
    uint8_t factor = 0;

    Hal_PWM_ParaCheck(pPWM, channel);

    if (!pPWM->waveSupport) {
        HAL_DBG("channel=%d, unsupported wave generator mode\n", channel);

        return HAL_INVAL;
    }
    reg = pPWM->pChHandle[channel].pReg;

    HAL_ASSERT(config != NULL);
    HAL_DBG("channel=%d, wave generator mode init\n", channel);

    if (config->enable) {
        /*
         * If the width mode is 16-bits mode, two 8-bits table units
         * are combined into one 16-bits unit.
         */
        if (config->widthMode == HAL_PWM_WAVE_TABLE_16BITS_WIDTH) {
            factor = 2;
        } else {
            factor = 1;
        }

        ctrl = WAVE_DUTY_EN(config->dutyEnable) |
               WAVE_PERIOD_EN(config->periodEnable) |
               WAVE_WIDTH_MODE(config->widthMode) |
               WAVE_UPDATE_MODE(config->updateMode);
        maxVal = config->dutyMax * factor << PWM_WAVE_MAX_WAVE_DUTY_MAX_SHIFT |
                 config->periodMax * factor << PWM_WAVE_MAX_WAVE_PERIOD_MAX_SHIFT;
        minVal = config->dutyMin * factor << PWM_WAVE_MIN_WAVE_DUTY_MIN_SHIFT |
                 config->periodMin * factor << PWM_WAVE_MIN_WAVE_PERIOD_MIN_SHIFT;
        offset = config->offset * factor << PWM_WAVE_OFFSET_WAVE_OFFSET_SHIFT;
        middle = config->middle * factor << PWM_WAVE_MIDDLE_WAVE_MIDDLE_SHIFT;

        rpt = config->rpt << PWM_RPT_RPT_FIRST_DIMENSIONAL_SHIFT;
    } else {
        pPWM->scaler = 0;
        ctrl = WAVE_DUTY_EN(false) | WAVE_PERIOD_EN(false);
    }

    WRITE_REG(reg->CLK_CTRL, CLK_SCALE(pPWM->scaler));
    WRITE_REG(reg->WAVE_CTRL, ctrl);
    WRITE_REG(reg->WAVE_MAX, maxVal);
    WRITE_REG(reg->WAVE_MIN, minVal);
    WRITE_REG(reg->WAVE_OFFSET, offset);
    WRITE_REG(reg->WAVE_MIDDLE, middle);

    WRITE_REG(reg->RPT, rpt);
    WRITE_REG(reg->INT_EN, WAVE_MAX_INT_EN(config->enable) | WAVE_MIDDLE_INT_EN(config->enable));

    return HAL_OK;
}
#endif
#else
/**
  * @brief  Handle PWM interrupt for capture/oneshot mode.
  * @param  pPWM: pointer to a PWM_HANDLE structure that contains
  *               the information for PWM module.
  * @retval HAL status
  */
HAL_Status HAL_PWM_IRQHandler(struct PWM_HANDLE *pPWM)
{
    uint32_t status;
    uint32_t i;

    HAL_ASSERT(pPWM != NULL);
    status = READ_REG(pPWM->pReg->INTSTS);

    /* clean ipd */
    WRITE_REG(pPWM->pReg->INTSTS, status & 0xf);

    if (status & 0xf) {
        for (i = 0; i < HAL_PWM_NUM_CHANNELS; i++) {
            if ((status & (1 << i)) &&
                (pPWM->mode[i] == HAL_PWM_CAPTURE)) {
                pPWM->result[i].active = true;
                pPWM->result[i].pol = status & (1 << (i + 8));
                if (pPWM->result[i].pol) {
                    pPWM->result[i].period = READ_REG(PWM_PERIOD_REG(pPWM, i));
                } else {
                    pPWM->result[i].period = READ_REG(PWM_DUTY_REG(pPWM, i));
                }
            }
        }
    }

    return HAL_OK;
}

/**
 * @brief  Configurate PWM mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel(0~3).
 * @param  config: Configuration for PWM.
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetConfig(struct PWM_HANDLE *pPWM, uint8_t channel,
                             const struct HAL_PWM_CONFIG *config)
{
    uint32_t period, duty;
    uint32_t ctrl;

    Hal_PWM_ParaCheck(pPWM, channel);

    HAL_ASSERT(config != NULL);
    HAL_DBG("channel=%d, period_ns=%" PRId32 ", duty_ns=%" PRId32 "\n",
            channel, config->periodNS, config->dutyNS);

    period = HAL_DivU64((uint64_t)pPWM->freq * config->periodNS, 1000000000);
    duty = HAL_DivU64((uint64_t)pPWM->freq * config->dutyNS, 1000000000);

    ctrl = READ_REG(PWM_CTRL_REG(pPWM, channel));
    ctrl |= PWM_LOCK;
    WRITE_REG(PWM_CTRL_REG(pPWM, channel), ctrl);

    WRITE_REG(PWM_PERIOD_REG(pPWM, channel), period);
    WRITE_REG(PWM_DUTY_REG(pPWM, channel), duty);

    ctrl &= ~(PWM_DUTY_MASK | PWM_INACTIVE_MASK);
    if (config->polarity) {
        ctrl |= PWM_DUTY_NEGATIVE | PWM_INACTIVE_POSTIVE;
    } else {
        ctrl |= PWM_DUTY_POSTIVE | PWM_INACTIVE_NEGATIVE;
    }

    ctrl &= ~(PWM_ALIGNED_MODE_MASK);
    switch (config->alignedMode) {
    case HAL_PWM_LEFT_ALIGNED:
        ctrl |= PWM_OUTPUT_LEFT;
        break;
    case HAL_PWM_CENTER_ALIGNED:
        ctrl |= PWM_OUTPUT_CENTER;
        break;
    case HAL_PWM_UNALIGNED:
    default:
        break;
    }

#ifdef PWM_PWM0_CTRL_ALIGNED_VLD_N_SHIFT
    ctrl &= ~(PWM_ALIGNED_VALID_MASK);
    switch (config->alignedMode) {
    case HAL_PWM_LEFT_ALIGNED:
    case HAL_PWM_CENTER_ALIGNED:
        ctrl |= PWM_ALIGNED_VALID;
        break;
    case HAL_PWM_UNALIGNED:
        ctrl |= PWM_ALIGNED_INVALID;
        break;
    default:
        break;
    }
#endif

    ctrl &= ~PWM_LOCK;
    WRITE_REG(PWM_CTRL_REG(pPWM, channel), ctrl);

    HAL_DBG("channel=%d, period=%" PRIu32 ", duty=%" PRIu32 ", polarity=%d\n",
            channel, period, duty, config->polarity);

    return HAL_OK;
}

/**
 * @brief  Configurate PWM oneshot count.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel(0~3).
 * @param  count: (count + 1)repeated effective periods of output waveform
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetOneshot(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t count)
{
    uint32_t ctrl;

    Hal_PWM_ParaCheck(pPWM, channel);

    HAL_DBG("Oneshot count=%" PRId32 "\n", count);

    ctrl = READ_REG(PWM_CTRL_REG(pPWM, channel));
    ctrl &= ~PWM_PWM0_CTRL_RPT_MASK;
    ctrl |= (count << PWM_PWM0_CTRL_RPT_SHIFT) & PWM_PWM0_CTRL_RPT_MASK;
    WRITE_REG(PWM_CTRL_REG(pPWM, channel), ctrl);

    return HAL_OK;
}

/**
 * @brief  Configurate PWM captured frequency.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel(0~3).
 * @param  freq: PWM use the frequency to capture data
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetCapturedFreq(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t freq)
{
    uint32_t ctrl;

    Hal_PWM_ParaCheck(pPWM, channel);

    HAL_ASSERT(freq != 0);
    HAL_DBG("Captured freq=%" PRId32 "\n", freq);

    ctrl = READ_REG(PWM_CTRL_REG(pPWM, channel));
    ctrl &= ~PWM_CTRL_SCALE_MASK;
    ctrl |= PWM_LP_ENABLE | PWM_SEL_SCALE_CLK;
    ctrl |= ((pPWM->freq / (2 * freq)) << PWM_CTRL_SCALE_SHIFT) & PWM_CTRL_SCALE_MASK;
    WRITE_REG(PWM_CTRL_REG(pPWM, channel), ctrl);

    return HAL_OK;
}

/**
 * @brief  Configurate PWM matched setting.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel(0~3).
 * @param  data: matching configuration.
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetMatch(struct PWM_HANDLE *pPWM, uint8_t channel, const struct PWM_MATCH *data)
{
    uint8_t i;

    Hal_PWM_ParaCheck(pPWM, channel);

    HAL_ASSERT(data != NULL);
    HAL_ASSERT(data->matchCount <= PWM_PWRMATCH_MAX_COUNT);

    /* preloader low */
    WRITE_REG(pPWM->pReg->PWRMATCH_LPRE, data->lpreMin | (data->lpreMax << PWM_PWRMATCH_MAX_SHIFT));
    /* preloader high */
    WRITE_REG(pPWM->pReg->PWRMATCH_HPRE, data->hpreMin | (data->hpreMax << PWM_PWRMATCH_MAX_SHIFT));
    /* logic 0/1 low */
    WRITE_REG(pPWM->pReg->PWRMATCH_LD, data->ldMin | (data->ldMax << PWM_PWRMATCH_MAX_SHIFT));
    /* logic 0 high */
    WRITE_REG(pPWM->pReg->PWRMATCH_HD_ZERO, data->hdZeroMin | (data->hdZeroMax << PWM_PWRMATCH_MAX_SHIFT));
    /* logic 1 high */
    WRITE_REG(pPWM->pReg->PWRMATCH_HD_ONE, data->hdOneMin | (data->hdOneMax << PWM_PWRMATCH_MAX_SHIFT));

    for (i = 0; i < data->matchCount; i++) {
        WRITE_REG(pPWM->pReg->PWRMATCH_VALUE[i], data->match[i]);
    }

    /* Enable pwr irq */
    SET_BIT(pPWM->pReg->INT_EN, PWM_PWR_INT_EN(channel));
    /* Enable pwr */
    SET_BIT(pPWM->pReg->PWRMATCH_CTRL, channel);

    return HAL_OK;
}

#if defined(PWM_PWM0_OFFSET_OFFSET)
/**
 * @brief  Configurate PWM channel output offset.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel(0~3).
 * @param  offsetNS: PWM channel output offset configuration.
 * @retval HAL status
 */
HAL_Status HAL_PWM_SetOutputOffset(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t offsetNS)
{
    uint32_t period, duty;
    uint32_t offset, offsetMax;

    Hal_PWM_ParaCheck(pPWM, channel);

    period = READ_REG(PWM_PERIOD_REG(pPWM, channel));
    duty = READ_REG(PWM_DUTY_REG(pPWM, channel));

    offset = HAL_DivU64((uint64_t)pPWM->freq * offsetNS, 1000000000);
    offsetMax = period - duty;
    if (offset < 0 || offset > offsetMax) {
        return HAL_INVAL;
    }

    HAL_DBG("channel=%d, offsetNS=%" PRId32 "\n", channel, offsetNS);

    WRITE_REG(pPWM->pReg->OFFSET[channel], offset);

    return HAL_OK;
}
#endif

#if defined(PWM_FILTER_CTRL_PWM0_GLOBAL_LOCK_SHIFT)
/**
 * @brief  Enable PWM global lock.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channelMask: PWM channel mask, such as 0x5 indicates
 *                      channel0 and channel2.
 * @retval HAL status
 */
HAL_Status HAL_PWM_GlobalLock(struct PWM_HANDLE *pPWM, uint8_t channelMask)
{
    uint32_t filterCtrl;

    HAL_ASSERT(pPWM != NULL);
    HAL_ASSERT(channelMask <= (PWM_GLOBAL_LOCK_MASK >> PWM_GLOBAL_LOCK_SHIFT));
    HAL_DBG("channelMask=0x%04x, global lock\n", channelMask);

    filterCtrl = READ_REG(pPWM->pReg->FILTER_CTRL);
    filterCtrl |= (channelMask << PWM_GLOBAL_LOCK_SHIFT) & PWM_GLOBAL_LOCK_MASK;
    WRITE_REG(pPWM->pReg->FILTER_CTRL, filterCtrl);

    return HAL_OK;
}

/**
 * @brief  Disable PWM global lock.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channelMask: PWM channel mask, such as 0x5 indicates
 *                      channel0 and channel2.
 * @retval HAL status
 */
HAL_Status HAL_PWM_GlobalUnlock(struct PWM_HANDLE *pPWM, uint8_t channelMask)
{
    uint32_t filterCtrl;

    HAL_ASSERT(pPWM != NULL);
    HAL_ASSERT(channelMask <= (PWM_GLOBAL_LOCK_MASK >> PWM_GLOBAL_LOCK_SHIFT));
    HAL_DBG("channelMask=0x%04x, global unlock\n", channelMask);

    filterCtrl = READ_REG(pPWM->pReg->FILTER_CTRL);
    filterCtrl &= ~((channelMask << PWM_GLOBAL_LOCK_SHIFT) & PWM_GLOBAL_LOCK_MASK);
    WRITE_REG(pPWM->pReg->FILTER_CTRL, filterCtrl);

    return HAL_OK;
}
#endif

/**
 * @brief  Get effective high level duration in capture mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval high level duration(in nanoseconds).
 */
uint64_t HAL_PWM_GetCaptureHighNs(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    Hal_PWM_ParaCheck(pPWM, channel);

    return HAL_DivU64((uint64_t)(READ_REG(PWM_PERIOD_REG(pPWM, channel)) * 1000000000ULL), pPWM->freq);
}

/**
 * @brief  Get effective low level duration in capture mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval low level duration(in nanoseconds).
 */
uint64_t HAL_PWM_GetCaptureLowNs(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    Hal_PWM_ParaCheck(pPWM, channel);

    return HAL_DivU64((uint64_t)(READ_REG(PWM_DUTY_REG(pPWM, channel)) * 1000000000ULL), pPWM->freq);
}

/**
 * @brief  Get PWM mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel(0~3).
 * @retval ePWM_Mode
 */
ePWM_Mode HAL_PWM_GetMode(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    uint32_t ctrl;

    Hal_PWM_ParaCheck(pPWM, channel);

    HAL_DBG("channel=%d\n", channel);

    ctrl = READ_REG(PWM_CTRL_REG(pPWM, channel));

    return (ePWM_Mode)((ctrl >> PWM_MODE_SHIFT) & PWM_MODE_MASK);
}

/**
 * @brief  Enable PWM.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel(0~3).
 * @param  mode: Current mode on for PWM.
 * @retval HAL status
 */
HAL_Status HAL_PWM_Enable(struct PWM_HANDLE *pPWM, uint8_t channel, ePWM_Mode mode)
{
    uint32_t enableConf, intEnable;

    Hal_PWM_ParaCheck(pPWM, channel);

    HAL_DBG("Enable channel=%d\n", channel);

    pPWM->mode[channel] = mode;
    if (pPWM->mode[channel] != HAL_PWM_CONTINUOUS) {
        intEnable = READ_REG(pPWM->pReg->INT_EN);
        /* Enable irq */
        intEnable |= PWM_INT_EN(channel);
        WRITE_REG(pPWM->pReg->INT_EN, intEnable);
    }

    enableConf = READ_REG(PWM_CTRL_REG(pPWM, channel));
    /* clean mode */
    enableConf &= ~PWM_MODE_MASK;
    enableConf |= (mode << PWM_MODE_SHIFT) | PWM_OUTPUT_LEFT | PWM_LP_DISABLE | PWM_ENABLE;
    WRITE_REG(PWM_CTRL_REG(pPWM, channel), enableConf);

    return HAL_OK;
}

/**
 * @brief  Disable PWM.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel(0~3).
 * @retval HAL status
 */
HAL_Status HAL_PWM_Disable(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    uint32_t ctrl, intEnable;

    Hal_PWM_ParaCheck(pPWM, channel);

    HAL_DBG("Disable channel=%d\n", channel);

    if (pPWM->mode[channel] != HAL_PWM_CONTINUOUS) {
        intEnable = READ_REG(pPWM->pReg->INT_EN);
        /* Disable irq */
        intEnable &= ~PWM_INT_EN(channel);
        WRITE_REG(pPWM->pReg->INT_EN, intEnable);
    }

    ctrl = READ_REG(PWM_CTRL_REG(pPWM, channel));
    ctrl &= ~PWM_ENABLE;
    WRITE_REG(PWM_CTRL_REG(pPWM, channel), ctrl);

    return HAL_OK;
}

#if defined(PWM_PWM0_CAPTURE_CNT_EN_OFFSET)
/**
 * @brief  Enable PWM wave counter mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel(0~3).
 * @retval HAL status
 */
HAL_Status HAL_PWM_EnableCounter(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    uint32_t modeCtrl = 0;

    Hal_PWM_ParaCheck(pPWM, channel);

    HAL_DBG("channel=%d, wave counter enable\n", channel);

    modeCtrl = READ_REG(pPWM->pReg->CAPTURE_CNT_EN[channel]);
    /* Enable Negedge and posedge both by default */
    modeCtrl |= (0x3 << PWM_CAPTURE_EN_SHIFT) & PWM_CAPTURE_EN_MASK;
    WRITE_REG(pPWM->pReg->CAPTURE_CNT_EN[channel], modeCtrl);

    return HAL_OK;
}

/**
 * @brief  Disable PWM wave counter mode.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel(0~3).
 * @retval HAL status
 */
HAL_Status HAL_PWM_DisableCounter(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    uint32_t modeCtrl = 0;

    Hal_PWM_ParaCheck(pPWM, channel);

    HAL_DBG("channel=%d, wave counter enable\n", channel);

    modeCtrl = READ_REG(pPWM->pReg->CAPTURE_CNT_EN[channel]);
    modeCtrl &= ~((0x3 << PWM_CAPTURE_EN_SHIFT) & PWM_CAPTURE_EN_MASK);
    WRITE_REG(pPWM->pReg->CAPTURE_CNT_EN[channel], modeCtrl);

    return HAL_OK;
}

/**
 * @brief  Get PWM wave counter result.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @param  cntRes: counter result.
 * @retval HAL status
 */
HAL_Status HAL_PWM_GetCounterRes(struct PWM_HANDLE *pPWM, uint8_t channel, uint64_t *cntRes)
{
    uint32_t low, high;

    Hal_PWM_ParaCheck(pPWM, channel);

    HAL_ASSERT(cntRes != NULL);

    low = READ_REG(pPWM->pReg->CAPTURE_NEG_CNT[channel]);
    high = READ_REG(pPWM->pReg->CAPTURE_POS_CNT[channel]);

    /*
     * the effective counter result is the smaller one of
     * the posedge number and negedge number
     */
    *cntRes = high > low ? low : high;
    if (!*cntRes) {
        return HAL_INVAL;
    }

    return HAL_OK;
}

/**
 * @brief  Clear PWM wave counter result.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  channel: PWM channel.
 * @retval HAL status
 */
HAL_Status HAL_PWM_ClearCounterRes(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    HAL_DBG("channel=%d, unsupported to clear wave counter result\n", channel);

    return HAL_INVAL;
}
#endif
#endif

/** @} */

/** @defgroup PWM_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 *  @{
 */

/**
 * @brief  Initialize the PWM according to the specified parameters.
 * @param  pPWM: pointer to a PWM_HANDLE structure that contains
 *               the information for PWM module.
 * @param  pReg: PWM controller register base address.
 * @param  freq: PWM bus input clock frequency.
 * @return HAL_Status
 */
HAL_Status HAL_PWM_Init(struct PWM_HANDLE *pPWM, struct PWM_REG *pReg, uint32_t freq)
{
#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
    uint32_t reg;
    uint32_t version;
    uint8_t i;

    HAL_ASSERT(pPWM != NULL);

    pPWM->freq = freq;

    version = READ_REG(pReg->VERSION_ID);
    pPWM->channelNum = (version & PWM_VERSION_ID_CHANNEL_NUM_SUPPORT_MASK) >> PWM_VERSION_ID_CHANNEL_NUM_SUPPORT_SHIFT;
    pPWM->freqMeterSupport = !!(version & PWM_VERSION_ID_FREQ_METER_SUPPORT_MASK);
    pPWM->counterSupport = !!(version & PWM_VERSION_ID_COUNTER_SUPPORT_MASK);
    pPWM->waveSupport = !!(version & PWM_VERSION_ID_WAVE_SUPPORT_MASK);

    for (i = 0; i < pPWM->channelNum; i++) {
        reg = (uint32_t)pReg + i * PWM_CHANNEL_OFFSET;
        pPWM->pChHandle[i].pReg = (struct PWM_REG *)(reg);
    }
#else
    HAL_ASSERT(pPWM != NULL);

    pPWM->pReg = pReg;
    HAL_ASSERT(IS_PWM_INSTANCE(pPWM->pReg));

    pPWM->freq = freq;
#endif

    return HAL_OK;
}

/**
  * @brief  De Initialize the PWM peripheral.
  * @param  pPWM: pointer to a PWM_HANDLE structure that contains
  *               the information for PWM module.
  * @return HAL status
  */
HAL_Status HAL_PWM_DeInit(struct PWM_HANDLE *pPWM)
{
    /* ...to do */
    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_PWM_MODULE_ENABLED */
