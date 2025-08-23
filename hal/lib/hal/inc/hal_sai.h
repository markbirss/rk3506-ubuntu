/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_SAI_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup SAI
 *  @{
 */

#ifndef _HAL_SAI_H_
#define _HAL_SAI_H_

#include "hal_audio.h"
#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup SAI_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/

typedef void (*RK_SAI_CALLBACK)(void *pCBParam, uint32_t event);

/**
 * enum eSAI_status - SAI status.
 */
typedef enum {
    SAI_TX_FIFO_EMPTY    = HAL_BIT(0),
    SAI_TX_FIFO_UNDERRUN = HAL_BIT(1),
    SAI_RX_FIFO_FULL     = HAL_BIT(16),
    SAI_RX_FIFO_OVERRUN  = HAL_BIT(17),
    SAI_FS_CLK_ERR       = HAL_BIT(18),
    SAI_FS_CLK_LOST      = HAL_BIT(19),
} eSAI_status;

/**
 * enum FPW_mode - Frame Pluse Width.
 */
typedef enum {
    FPW_ONE_BCLK_WIDTH,
    FPW_ONE_SLOT_WIDTH,
    FPW_HALF_FRAME_WIDTH,
} eFPW_mode;

/** sai dev struct */
struct HAL_SAI_DEV {
    struct SAI_REG *pReg;
    struct AUDIO_DMA_DATA rxDmaData;
    struct AUDIO_DMA_DATA txDmaData;
    eCLOCK_Name mclk;
    eFPW_mode fpw;
    ePD_Id pd;
    IRQn_Type irqNum;
    uint32_t mclkGate;
    uint16_t bclkFs;
    uint8_t txLanes;
    uint8_t rxLanes;
    uint8_t maxLanes;
    uint8_t fwRatio;
    bool isClkAuto;
    bool isTdm;

    RK_SAI_CALLBACK pCallback; /* TX */
    void *pCBParam;
    RK_SAI_CALLBACK pCallback2; /* RX */
    void *pCBParam2;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup SAI_Public_Function_Declare Public Function Declare
 *  @{
 */

/** @defgroup SAI_Dev_Level_API Dev Level API
 *  @{
 */
HAL_Status HAL_SAI_DevInit(struct HAL_SAI_DEV *sai, struct AUDIO_INIT_CONFIG *config);
HAL_Status HAL_SAI_DevDeInit(struct HAL_SAI_DEV *sai);
HAL_Status HAL_SAI_DevEnable(struct HAL_SAI_DEV *sai, eAUDIO_streamType stream);
HAL_Status HAL_SAI_DevDisable(struct HAL_SAI_DEV *sai, eAUDIO_streamType stream);
HAL_Status HAL_SAI_DevConfig(struct HAL_SAI_DEV *sai, eAUDIO_streamType stream,
                             struct AUDIO_PARAMS *params);
HAL_Status HAL_SAI_DevRegisterCallback(struct HAL_SAI_DEV *sai, RK_SAI_CALLBACK pCB, void *pCBParam);
HAL_Status HAL_SAI_DevRegisterCallback2(struct HAL_SAI_DEV *sai, RK_SAI_CALLBACK pCB2, void *pCBParam2);
HAL_Status HAL_SAI_DevIrqHandler(struct HAL_SAI_DEV *sai);
HAL_Status HAL_SAI_DevSetFsLostDetectCount(struct HAL_SAI_DEV *sai, uint32_t frameCount,
                                           uint32_t slotWidth, uint32_t chPerLane,
                                           uint32_t sampleRate);

/** @} */

/** @defgroup SAI_Low_Level_API Low Level API
 *  @{
 */
HAL_Status HAL_SAI_EnableTX(struct SAI_REG *pReg);
HAL_Status HAL_SAI_EnableRX(struct SAI_REG *pReg);
HAL_Status HAL_SAI_DisableTX(struct SAI_REG *pReg);
HAL_Status HAL_SAI_DisableRX(struct SAI_REG *pReg);
HAL_Status HAL_SAI_SetBclkDiv(struct SAI_REG *pReg, int div);
HAL_Status HAL_SAI_SetFsxn0FrameWidth(struct SAI_REG *pReg, int frameWidth, int shiftFrame);
HAL_Status HAL_SAI_SetFsxn1FrameWidth(struct SAI_REG *pReg, int frameWidth, int shiftFrame);
HAL_Status HAL_SAI_EnableFsxn0(struct SAI_REG *pReg);
HAL_Status HAL_SAI_EnableFsxn1(struct SAI_REG *pReg);
HAL_Status HAL_SAI_DisableFsxn0(struct SAI_REG *pReg);
HAL_Status HAL_SAI_DisableFsxn1(struct SAI_REG *pReg);
HAL_Status HAL_SAI_SetFsLostDetectCount(struct SAI_REG *pReg, uint32_t frameCount);
HAL_Status HAL_SAI_EnableFsLostDetect(struct SAI_REG *pReg);
HAL_Status HAL_SAI_DisableFsLostDetect(struct SAI_REG *pReg);
HAL_Status HAL_SAI_EnableFIFOXrunDetect(struct SAI_REG *pReg, int stream);
HAL_Status HAL_SAI_DisableFIFOXrunDetect(struct SAI_REG *pReg, int stream);
HAL_Status HAL_SAI_SetChainedMode(struct SAI_REG *pReg);
HAL_Status HAL_SAI_EnableDMA(struct SAI_REG *pReg, int stream);
HAL_Status HAL_SAI_DisableDMA(struct SAI_REG *pReg, int stream);
HAL_Status HAL_SAI_SetTxSlotMask(struct SAI_REG *pReg, uint32_t mask);
HAL_Status HAL_SAI_SetRxSlotMask(struct SAI_REG *pReg, uint32_t mask);
HAL_Status HAL_SAI_SetTxTimingShift(struct SAI_REG *pReg, int shift);
HAL_Status HAL_SAI_SetRxTimingShift(struct SAI_REG *pReg, int shift);
uint32_t HAL_SAI_GetTxSlotMask(struct SAI_REG *pReg);
uint32_t HAL_SAI_GetRxSlotMask(struct SAI_REG *pReg);
uint32_t HAL_SAI_ClearIrq(struct SAI_REG *pReg);

HAL_Status HAL_SAI_DeInterLeavedFrame(void *in, void *out, int frames, int channels, int width, int lanes);
int HAL_SAI_GetInterLeavedChOffset(int ch, int channels, int lanes);
int HAL_SAI_GetDeInterLeavedChOffset(int ch, int channels, int lanes);
HAL_Status HAL_SAI_GetInterLeavedChTable(int *table, int channels, int lanes);
HAL_Status HAL_SAI_GetDeInterLeavedChTable(int *table, int channels, int lanes);

/** @} */

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_SAI_MODULE_ENABLED */
