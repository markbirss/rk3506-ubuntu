/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_PWM_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
  * @{
  */

/** @addtogroup PWM
  * @{
  */

#ifndef __HAL_PWM_H
#define __HAL_PWM_H

#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup PWM_Exported_Definition_Group1 Basic Definition
 *  @{
 */

#define PWM_MAIN_VERSION_SHIFT 24
#define PWM_MAIN_VERSION_MASK  0xff
#define PWM_MAIN_VERSION(v)    ((v >> PWM_MAIN_VERSION_SHIFT) & PWM_MAIN_VERSION_MASK)

#ifndef PWM_VERSION_ID
#define PWM_VERSION_ID (0x01000000)
#endif

#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
#define PWM_PWRMATCH_MAX_COUNT (16)
#define PWM_CHANNEL_MAX        (8)
#define PWM_CHANNEL_OFFSET     (0x1000)
#define PWM_WAVE_TABEL_MAX     (0x300)
#else
#define HAL_PWM_NUM_CHANNELS   (HAL_ARRAY_SIZE(((struct PWM_REG *)0)->CHANNELS))
#define PWM_PWRMATCH_MAX_COUNT (10)
#define PWM_CHANNEL_MAX        (1)
#endif

/***************************** Structure Definition **************************/

/**
  * @brief  PWM operation mode definition
  */
typedef enum {
    HAL_PWM_ONE_SHOT = 0, /**< one-shot mode generates configured periods */
    HAL_PWM_CONTINUOUS,   /**< continuous mode generates periods in series */
    HAL_PWM_CAPTURE,      /**< capture mode supports to measure input waveform */
} ePWM_Mode;

/**
  * @brief  PWM capture interrupt mode definition
  */
typedef enum {
    HAL_PWM_CAP_LPR_INT = 1, /**< enable LPR interrupt in capture mode */
    HAL_PWM_CAP_HPR_INT,     /**< enable HPR interrupt in capture mode */
} ePWM_captureIntMode;

/**
  * @brief  PWM aligned mode definition
  */
typedef enum {
    HAL_PWM_LEFT_ALIGNED = 1, /**< set waveform to left-aligned mode */
    HAL_PWM_CENTER_ALIGNED,   /**< set waveform to center-aligned mode */
    HAL_PWM_UNALIGNED,        /**< set waveform to unaligned mode */
} ePWM_alignedMode;

/**
  * @brief PWM wave table element width
  */
typedef enum {
    HAL_PWM_WAVE_TABLE_8BITS_WIDTH,  /**< each element in table is 8bits */
    HAL_PWM_WAVE_TABLE_16BITS_WIDTH, /**< each element in table is 16bits */
} ePWM_waveTableWidthMode;

/**
 * @brief PWM wave generator update mode
 */
typedef enum {
    /**
      * The wave table address will wrap back to minimum address when increase to
      * maximum and then increase again.
      */
    HAL_PWM_WAVE_INCREASING,
    /**
      * The wave table address will change to decreasing when increasing to the maximum
      * address. it will return to increasing when decrease to the minimum value.
      */
    HAL_PWM_WAVE_INCREASING_THEN_DECREASING,
} ePWM_waveUpdateMode;

/**
  * @brief  PWM HW information definition
  */
struct HAL_PWM_DEV {
    struct PWM_REG *pReg;
    eCLOCK_Name clkID;
    uint32_t clkGateID;
    uint32_t pclkGateID;
    IRQn_Type irqNum[PWM_CHANNEL_MAX];
};

/**
  * @brief  PWM handle Structure definition
  */
struct HAL_PWM_CONFIG {
    uint8_t channel;
    uint32_t periodNS;
    uint32_t dutyNS;
    bool polarity;
    ePWM_alignedMode alignedMode;
};

/**
  * @brief  PWM capture data
  */
struct PWM_CAPTURE {
    uint32_t period;
    uint32_t posCycles;
    uint32_t negCycles;
    bool pol;
    bool active;
};

/**
  * @brief  PWM match data
  */
struct PWM_MATCH {
    uint32_t match[PWM_PWRMATCH_MAX_COUNT];
    uint8_t matchCount;
    uint16_t lpreMin;
    uint16_t lpreMax;
    uint16_t hpreMin;
    uint16_t hpreMax;
    uint16_t ldMin;
    uint16_t ldMax;
    uint16_t hdZeroMin;
    uint16_t hdZeroMax;
    uint16_t hdOneMin;
    uint16_t hdOneMax;
};

#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
/**
  * @brief  PWM channel handle structure definition
  */
struct PWM_CHANNEL_HANDLE {
    struct PWM_REG *pReg;
    struct PWM_CAPTURE result;
    ePWM_Mode mode;
};

/**
  * @brief  PWM handle structure definition
  */
struct PWM_HANDLE {
    struct PWM_CHANNEL_HANDLE pChHandle[PWM_CHANNEL_MAX];
    uint32_t freq;
    uint32_t channelNum;
    uint32_t scaler;
    uint8_t globalGrantMask;
    uint8_t globalMask;
    bool freqMeterSupport;
    bool counterSupport;
    bool waveSupport;
};
#else
/**
  * @brief  PWM Handle Structure definition
  */
struct PWM_HANDLE {
    struct PWM_REG *pReg;
    uint32_t freq;
    ePWM_Mode mode[HAL_PWM_NUM_CHANNELS];
    struct PWM_CAPTURE result[HAL_PWM_NUM_CHANNELS];
};
#endif

/**
  * @brief  PWM wave table config Structure definition
  * @param  offset: the offset of wave table to set
  * @param  len: the length of wave table to set
  * @param  data: the data of wave table to set
  */
struct PWM_WAVE_TABLE {
    uint16_t offset;
    uint16_t len;
    uint64_t *data;
};

/**
  * @brief  PWM wave generator config Structure definition
  * @param  dutyTable: the wave table config of duty
  * @param  periodTable: the wave table config of period
  * @param  enable: enable or disable wave generator
  * @param  dutyEnable: to update duty by duty table or not
  * @param  periodEnable: to update period by period table or not
  * @param  rpt: the number of repeated effective periods
  * @param  widthMode: the width mode of wave table
  * @param  updateMode: the update mode of wave generator
  * @param  dutyMax: the maximum address of duty table
  * @param  dutyMin: the minimum address of duty table
  * @param  periodMax: the maximum address of period table
  * @param  periodMin: the minimum address of period table
  * @param  offset: the initial offset address of duty and period
  * @param  middle: the middle address of duty and period
  * @param  maxHold: the time to stop at maximum address
  * @param  minHold: the time to stop at minimum address
  * @param  middleHold: the time to stop at middle address
  * @param  clkRate: the dclk rate in wave generator mode
  */
struct PWM_WAVE_CONFIG {
    struct PWM_WAVE_TABLE *dutyTable;
    struct PWM_WAVE_TABLE *periodTable;
    bool enable;
    bool dutyEnable;
    bool periodEnable;
    uint16_t rpt;
    uint32_t widthMode;
    uint32_t updateMode;
    uint32_t dutyMax;
    uint32_t dutyMin;
    uint32_t periodMax;
    uint32_t periodMin;
    uint32_t offset;
    uint32_t middle;
    uint32_t maxHold;
    uint32_t minHold;
    uint32_t middleHold;
    uint64_t clkRate;
};

/**
  * @}
  */

/***************************** Function Declare ******************************/
/** @defgroup PWM_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_PWM_IRQHandler(struct PWM_HANDLE *pPWM);
HAL_Status HAL_PWM_ChannelIRQHandler(struct PWM_HANDLE *pPWM, uint8_t channel);
HAL_Status HAL_PWM_SetConfig(struct PWM_HANDLE *pPWM, uint8_t channel,
                             const struct HAL_PWM_CONFIG *config);
HAL_Status HAL_PWM_SetOneshot(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t count);
HAL_Status HAL_PWM_SetCapturedFreq(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t freq);
HAL_Status HAL_PWM_SetMatch(struct PWM_HANDLE *pPWM, uint8_t channel, const struct PWM_MATCH *data);
uint64_t HAL_PWM_GetCaptureHighNs(struct PWM_HANDLE *pPWM, uint8_t channel);
uint64_t HAL_PWM_GetCaptureLowNs(struct PWM_HANDLE *pPWM, uint8_t channel);
#if defined(PWM_PWM0_OFFSET_OFFSET) || defined(PWM_OFFSET_OFFSET)
HAL_Status HAL_PWM_SetOutputOffset(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t offsetNS);
#else
inline HAL_Status HAL_PWM_SetOutputOffset(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t offsetNS)
{
    return HAL_OK;
}
#endif
#if defined(PWM_FILTER_CTRL_PWM0_GLOBAL_LOCK_SHIFT) || defined(PWM_GLOBAL_CTRL_OFFSET)
HAL_Status HAL_PWM_GlobalLock(struct PWM_HANDLE *pPWM, uint8_t channelMask);
HAL_Status HAL_PWM_GlobalUnlock(struct PWM_HANDLE *pPWM, uint8_t channelMask);
#else
inline HAL_Status HAL_PWM_GlobalLock(struct PWM_HANDLE *pPWM, uint8_t channelMask)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_GlobalUnlock(struct PWM_HANDLE *pPWM, uint8_t channelMask)
{
    return HAL_OK;
}
#endif
#if defined(PWM_PWM0_CAPTURE_CNT_EN_OFFSET) || defined(PWM_COUNTER_CTRL_OFFSET)
HAL_Status HAL_PWM_EnableCounter(struct PWM_HANDLE *pPWM, uint8_t channel);
HAL_Status HAL_PWM_DisableCounter(struct PWM_HANDLE *pPWM, uint8_t channel);
HAL_Status HAL_PWM_ClearCounterRes(struct PWM_HANDLE *pPWM, uint8_t channel);
HAL_Status HAL_PWM_GetCounterRes(struct PWM_HANDLE *pPWM, uint8_t channel, uint64_t *cntRes);
#else
inline HAL_Status HAL_PWM_EnableCounter(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_DisableCounter(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_ClearCounterRes(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_GetCounterRes(struct PWM_HANDLE *pPWM, uint8_t channel, uint64_t *cntRes)
{
    return HAL_OK;
}
#endif
ePWM_Mode HAL_PWM_GetMode(struct PWM_HANDLE *pPWM, uint8_t channel);
HAL_Status HAL_PWM_Enable(struct PWM_HANDLE *pPWM, uint8_t channel, ePWM_Mode mode);
HAL_Status HAL_PWM_Disable(struct PWM_HANDLE *pPWM, uint8_t channel);
HAL_Status HAL_PWM_Init(struct PWM_HANDLE *pPWM, struct PWM_REG *pReg, uint32_t freq);
HAL_Status HAL_PWM_DeInit(struct PWM_HANDLE *pPWM);
#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
HAL_Status HAL_PWM_EnableCaptureInt(struct PWM_HANDLE *pPWM, uint8_t channel, ePWM_captureIntMode mode);
HAL_Status HAL_PWM_DisableCaptureInt(struct PWM_HANDLE *pPWM, uint8_t channel);
HAL_Status HAL_PWM_GlobalUpdate(struct PWM_HANDLE *pPWM);
HAL_Status HAL_PWM_GlobalEnable(struct PWM_HANDLE *pPWM);
HAL_Status HAL_PWM_GlobalDisable(struct PWM_HANDLE *pPWM);
HAL_Status HAL_PWM_EnableFreqMeter(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t delayMs);
HAL_Status HAL_PWM_DisableFreqMeter(struct PWM_HANDLE *pPWM, uint8_t channel);
HAL_Status HAL_PWM_GetFreqMeterRes(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t delayMs, uint32_t *freqHz);
#ifdef PWM_WAVE_MEM_ARBITER_WAVE_MEM_GRANT_SHIFT
HAL_Status HAL_PWM_SetWaveTable(struct PWM_HANDLE *pPWM, uint8_t channel, struct PWM_WAVE_TABLE *table,
                                ePWM_waveTableWidthMode widthMode, uint64_t clkRate);
HAL_Status HAL_PWM_SetWave(struct PWM_HANDLE *pPWM, uint8_t channel, struct PWM_WAVE_CONFIG *config);
#else
inline HAL_Status HAL_PWM_SetWaveTable(struct PWM_HANDLE *pPWM, uint8_t channel, struct PWM_WAVE_TABLE *table,
                                       ePWM_waveTableWidthMode widthMode, uint64_t clkRate)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_SetWave(struct PWM_HANDLE *pPWM, uint8_t channel, struct PWM_WAVE_CONFIG *config)
{
    return HAL_OK;
}
#endif
#else
inline HAL_Status HAL_PWM_EnableCaptureInt(struct PWM_HANDLE *pPWM, uint8_t channel, ePWM_captureIntMode mode)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_DisableCaptureInt(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_GlobalUpdate(struct PWM_HANDLE *pPWM)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_GlobalEnable(struct PWM_HANDLE *pPWM)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_GlobalDisable(struct PWM_HANDLE *pPWM)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_EnableFreqMeter(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t delayMs)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_DisableFreqMeter(struct PWM_HANDLE *pPWM, uint8_t channel)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_GetFreqMeterRes(struct PWM_HANDLE *pPWM, uint8_t channel, uint32_t delayMs, uint32_t *freqHz)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_SetWaveTable(struct PWM_HANDLE *pPWM, uint8_t channel, struct PWM_WAVE_TABLE *table,
                                       ePWM_waveTableWidthMode widthMode, uint64_t clkRate)
{
    return HAL_OK;
}
inline HAL_Status HAL_PWM_SetWave(struct PWM_HANDLE *pPWM, uint8_t channel, struct PWM_WAVE_CONFIG *config)
{
    return HAL_OK;
}
#endif

/** @} */

#endif

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_PWM_MODULE_ENABLED */
