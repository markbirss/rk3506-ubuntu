/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_SPDIFTX_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup SPDIFTX
 *  @{
 */

#ifndef _HAL_SPDIFTX_H_
#define _HAL_SPDIFTX_H_

#include "hal_audio.h"
#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup SPDIFTX_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/

/** spdiftx dev struct */
struct HAL_SPDIFTX_DEV {
    struct SPDIFTX_REG *pReg;
    struct AUDIO_DMA_DATA txDmaData;
    eCLOCK_Name mclk;
    uint32_t mclkGate;
    IRQn_Type irqNum;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup SPDIFTX_Public_Function_Declare Public Function Declare
 *  @{
 */

/** @defgroup SPDIFTX_Low_Level_API Low Level API
 *  @{
 */
HAL_Status HAL_SPDIFTX_Init(struct SPDIFTX_REG *pReg);
HAL_Status HAL_SPDIFTX_DeInit(struct SPDIFTX_REG *pReg);
HAL_Status HAL_SPDIFTX_Config(struct SPDIFTX_REG *pReg, struct AUDIO_PARAMS *params);
HAL_Status HAL_SPDIFTX_Enable(struct SPDIFTX_REG *pReg);
HAL_Status HAL_SPDIFTX_Disable(struct SPDIFTX_REG *pReg);

/** @} */

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_SPDIFTX_MODULE_ENABLED */
