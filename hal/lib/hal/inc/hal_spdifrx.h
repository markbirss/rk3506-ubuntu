/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_SPDIFRX_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup SPDIFRX
 *  @{
 */

#ifndef _HAL_SPDIFRX_H_
#define _HAL_SPDIFRX_H_

#include "hal_audio.h"
#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup SPDIFRX_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/

/**
 * enum eSPDIFRX_status - SPDIFRX status.
 */
typedef enum {
    SPDIFRX_PARITY_BIT_ERROR               = HAL_BIT(0),
    SPDIFRX_CS_CHANGED                     = HAL_BIT(1),
    SPDIFRX_VALIDITY_FLAG                  = HAL_BIT(2),
    SPDIFRX_BURST_PREAMBLE_SYNC_WORD_FOUND = HAL_BIT(3),
    SPDIFRX_FIFO_FULL                      = HAL_BIT(4),
    SPDIFRX_FIFO_OVERRUN                   = HAL_BIT(5),
    SPDIFRX_BMC_DECODE_ERROR               = HAL_BIT(6),
    SPDIFRX_CDR_LOST_LOCK                  = HAL_BIT(7),
    SPDIFRX_BLOCK_RECEIVED                 = HAL_BIT(8),
    SPDIFRX_CDR_LOCKED                     = HAL_BIT(9),
    SPDIFRX_USER_DATA_CHANGED              = HAL_BIT(10),
    SPDIFRX_DATA_WIDTH_CHANGED             = HAL_BIT(11),
    SPDIFRX_AUDIO_TYPE_CHANGED             = HAL_BIT(12),
} eSPDIFRX_status;

/**
 * enum eSPDIFRX_dataFmt - the Data Store Format.
 */
typedef enum {
    SPDIFRX_IEC60958,
    SPDIFRX_DATA24BIT,
    SPDIFRX_DATA16BIT,
} eSPDIFRX_dataFmt;

/**
 * enum eSPDIFRX_audioType - the audio type of input stream.
 */
typedef enum {
    SPDIFRX_AUDIO_TYPE_PCM,
    SPDIFRX_AUDIO_TYPE_COMPRESSED,
} eSPDIFRX_audioType;

/** spdifrx dev struct */
struct HAL_SPDIFRX_DEV {
    struct SPDIFRX_REG *pReg;
    struct AUDIO_DMA_DATA rxDmaData;
    eCLOCK_Name mclk;
    uint32_t mclkGate;
    uint32_t rst;
    IRQn_Type irqNum;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup SPDIFRX_Public_Function_Declare Public Function Declare
 *  @{
 */

/** @defgroup SPDIFRX_Dev_Level_API Dev Level API
 *  @{
 */
HAL_Status HAL_SPDIFRX_DevInit(struct HAL_SPDIFRX_DEV *spdifrx);
HAL_Status HAL_SPDIFRX_DevDeInit(struct HAL_SPDIFRX_DEV *spdifrx);
HAL_Status HAL_SPDIFRX_DevEnable(struct HAL_SPDIFRX_DEV *spdifrx);
HAL_Status HAL_SPDIFRX_DevDisable(struct HAL_SPDIFRX_DEV *spdifrx);

/** @} */

/** @defgroup SPDIFRX_Low_Level_API Low Level API
 *  @{
 */
HAL_Status HAL_SPDIFRX_Init(struct SPDIFRX_REG *pReg);
HAL_Status HAL_SPDIFRX_DeInit(struct SPDIFRX_REG *pReg);
HAL_Status HAL_SPDIFRX_Enable(struct SPDIFRX_REG *pReg);
HAL_Status HAL_SPDIFRX_Disable(struct SPDIFRX_REG *pReg);
HAL_Status HAL_SPDIFRX_SetFmt(struct SPDIFRX_REG *pReg, eSPDIFRX_dataFmt fmt);
eSPDIFRX_audioType HAL_SPDIFRX_GetAudioType(struct SPDIFRX_REG *pReg);
uint16_t HAL_SPDIFRX_GetPC(struct SPDIFRX_REG *pReg);
uint16_t HAL_SPDIFRX_GetPD(struct SPDIFRX_REG *pReg);
uint32_t HAL_SPDIFRX_IrqHandler(struct SPDIFRX_REG *pReg);
bool HAL_SPDIFRX_IsLocked(struct SPDIFRX_REG *pReg);

/** @} */

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_SPDIFRX_MODULE_ENABLED */
