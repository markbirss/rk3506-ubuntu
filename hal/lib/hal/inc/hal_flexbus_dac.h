/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#if defined(HAL_FLEXBUS_MODULE_ENABLED) && defined(HAL_FLEXBUS_DAC_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FLEXBUS_DAC
 *  @{
 */

#ifndef __HAL_FLEXBUS_DAC_H
#define __HAL_FLEXBUS_DAC_H

#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup FLEXBUS_DAC_Exported_Definition_Group1 Basic Definition
 *  @{
 */

#define HAL_FLEXBUS_MIN_TX_NUM_OF_DFS_MIN 1 /**< Min tx num_of_dfs of FLEXBUS */
#define HAL_FLEXBUS_MAX_TX_RATE           200000000 /**< Max rx rate of FLEXBUS tx */

/***************************** Structure Definition **************************/

typedef enum {
    FLEXBUS_DAC_DONE = 0,
    FLEXBUS_DAC_ERR,
} eFLEXBUS_DAC_result;

/** @brief  FLEXBUS DAC Configuration Structure definition */
struct FLEXBUS_DAC_HANDLE {
    struct FLEXBUS_HANDLE *pFB;
    bool cPol;
    bool cPha;
    bool freeSclk;
    bool cutOff;
    bool continueMode;
    uint32_t dfs;
    uint32_t rate;
    eFLEXBUS_DAC_result result;
    uint32_t srcBufLen;
    void *pSrcBuf0;
    void *pSrcBuf1;
};

/** @} */

/***************************** Function Declare ******************************/
/** @defgroup FLEXBUS_DAC_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_FLEXBUS_DAC_WriteBlockEnable(struct FLEXBUS_DAC_HANDLE *pFBDAC, void *pSrcBuf,
                                            uint32_t srcLen, uint32_t numOfDfs);
HAL_Status HAL_FLEXBUS_DAC_WriteContinueEnable(struct FLEXBUS_DAC_HANDLE *pFBDAC, void *pSrcBuf0,
                                               void *pSrcBuf1, uint32_t srcLen);
HAL_Status HAL_FLEXBUS_DAC_WriteContinueStop(struct FLEXBUS_DAC_HANDLE *pFBDAC);
HAL_Status HAL_FLEXBUS_DAC_QueryStatus(struct FLEXBUS_DAC_HANDLE *pFBDAC);
HAL_Status HAL_FLEXBUS_DAC_Isr(struct FLEXBUS_DAC_HANDLE *pFBDAC, uint32_t isr);
void HAL_FLEXBUS_DAC_WriteDisable(struct FLEXBUS_DAC_HANDLE *pFBDAC);
HAL_Status HAL_FLEXBUS_DAC_Init(struct FLEXBUS_DAC_HANDLE *pFBDAC, struct FLEXBUS_HANDLE *pFB);
HAL_Status HAL_FLEXBUS_DAC_DeInit(struct FLEXBUS_DAC_HANDLE *pFBDAC);

/** @} */

#endif

/** @} */

/** @} */

#endif  /* HAL_FLEXBUS_MODULE_ENABLED && HAL_FLEXBUS_DAC_MODULE_ENABLED */
