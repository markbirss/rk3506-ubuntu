/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_ACDCDIG_DSM_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup ACDCDIG_DSM
 *  @{
 */

#ifndef _HAL_ACDCDIG_DSM_H_
#define _HAL_ACDCDIG_DSM_H_

#include "hal_audio.h"
#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup ACDCDIG_DSM_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/
/** acdcDigDsm dev struct */
struct HAL_ACDCDIG_DSM_DEV {
    struct ACDCDIG_DSM_REG *pReg;
    eCLOCK_Name mclk;
    uint16_t bclkFs;
    uint16_t enabled;
    uint32_t reset;
    eAUDIO_fmtType format;
    ePD_Id pd;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup ACDCDIG_DSM_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_ACDCDIG_DSM_Supsend(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm);
HAL_Status HAL_ACDCDIG_DSM_Resume(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm);
HAL_Status HAL_ACDCDIG_DSM_Init(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm, struct AUDIO_INIT_CONFIG *config);
HAL_Status HAL_ACDCDIG_DSM_DeInit(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm);
HAL_Status HAL_ACDCDIG_DSM_Enable(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm, eAUDIO_streamType stream);
HAL_Status HAL_ACDCDIG_DSM_Disable(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm, eAUDIO_streamType stream);
HAL_Status HAL_ACDCDIG_DSM_Config(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm, eAUDIO_streamType stream, struct AUDIO_PARAMS *params);
HAL_Status HAL_ACDCDIG_DSM_SetGain(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm,
                                   eAUDIO_streamType stream, struct AUDIO_DB_CONFIG *dBConfig);
HAL_Status HAL_ACDCDIG_DSM_GetGain(struct HAL_ACDCDIG_DSM_DEV *acdcDigDsm,
                                   eAUDIO_streamType stream, struct AUDIO_DB_CONFIG *dBConfig);

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_ACDCDIG_DSM_MODULE_ENABLED */
