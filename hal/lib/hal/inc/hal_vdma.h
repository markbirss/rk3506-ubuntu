/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_VDMA_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup VDMA
 *  @{
 */

#ifndef _HAL_VDMA_H_
#define _HAL_VDMA_H_

#include "hal_audio.h"
#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup VDMA_Exported_Definition_Group1 Basic Definition
 *  @{
 */

typedef void (*VDMA_Callback)(void *cparam);

/***************************** Structure Definition **************************/

/** VDMA dev struct */
struct HAL_VDMA_DEV {
    struct VDMA_REG *pReg;
    uint32_t vsAddr;
    VDMA_Callback callback;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup VDMA_Public_Function_Declare Public Function Declare
 *  @{
 */
HAL_Status HAL_VDMA_Supsend(struct HAL_VDMA_DEV *vdma);
HAL_Status HAL_VDMA_Resume(struct HAL_VDMA_DEV *vdma);
HAL_Status HAL_VDMA_Init(struct HAL_VDMA_DEV *vdma, struct AUDIO_INIT_CONFIG *config);
HAL_Status HAL_VDMA_DeInit(struct HAL_VDMA_DEV *vdma);
HAL_Status HAL_VDMA_Enable(struct HAL_VDMA_DEV *vdma);
HAL_Status HAL_VDMA_Disable(struct HAL_VDMA_DEV *vdma);
HAL_Status HAL_VDMA_Config(struct HAL_VDMA_DEV *vdma, struct AUDIO_PARAMS *params);
HAL_Status HAL_VDMA_PrepDmaCyclic(struct HAL_VDMA_DEV *vdma, uint32_t dmaAddr,
                                  uint32_t len, uint32_t periodLen,
                                  eDMA_TRANSFER_DIRECTION direction,
                                  VDMA_Callback callback, void *cparam);
/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_VDMA_MODULE_ENABLED */
