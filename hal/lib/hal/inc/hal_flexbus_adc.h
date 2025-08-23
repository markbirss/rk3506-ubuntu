/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#if defined(HAL_FLEXBUS_MODULE_ENABLED) && defined(HAL_FLEXBUS_ADC_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FLEXBUS_ADC
 *  @{
 */

#ifndef __HAL_FLEXBUS_ADC_H
#define __HAL_FLEXBUS_ADC_H

#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup FLEXBUS_ADC_Exported_Definition_Group1 Basic Definition
 *  @{
 */

#define HAL_FLEXBUS_MIN_RX_NUM_OF_DFS_MIN 8 /**< Min rx num_of_dfs of FLEXBUS */
#define HAL_FLEXBUS_MAX_RX_RATE           200000000 /**< Max rx rate of FLEXBUS rx */

/***************************** Structure Definition **************************/

typedef enum {
    FLEXBUS_ADC_DONE = 0,
    FLEXBUS_ADC_ERR,
} eFLEXBUS_ADC_result;

/** @brief  FLEXBUS ADC HW information definition */
struct HAL_FLEXBUS_ADC_DEV {
    eCLOCK_Name refclkID;
    uint32_t refclkGateID;
};

/** @brief  FLEXBUS ADC Configuration Structure definition */
struct FLEXBUS_ADC_HANDLE {
    struct FLEXBUS_HANDLE *pFB;
    bool slaveMode;
    bool cPol;
    bool cPha;
    bool freeSclk;
    bool cutOff;
    bool continueMode;
    uint32_t dfs;
    uint32_t rate;
    eFLEXBUS_ADC_result result;
    uint32_t dstBufLen;
    void *pDstBuf0;
    void *pDstBuf1;
};

/** @} */

/***************************** Function Declare ******************************/
/** @defgroup FLEXBUS_ADC_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_FLEXBUS_ADC_ReadBlockEnable(struct FLEXBUS_ADC_HANDLE *pFBADC, void *pDstBuf,
                                           uint32_t dstLen, uint32_t numOfDfs);
HAL_Status HAL_FLEXBUS_ADC_ReadContinueEnable(struct FLEXBUS_ADC_HANDLE *pFBADC, void *pDstBuf0,
                                              void *pDstBuf1, uint32_t dstLen);
HAL_Status HAL_FLEXBUS_ADC_ReadContinueStop(struct FLEXBUS_ADC_HANDLE *pFBADC);
HAL_Status HAL_FLEXBUS_ADC_QueryStatus(struct FLEXBUS_ADC_HANDLE *pFBADC);
HAL_Status HAL_FLEXBUS_ADC_Isr(struct FLEXBUS_ADC_HANDLE *pFBADC, uint32_t isr);
void HAL_FLEXBUS_ADC_ReadDisable(struct FLEXBUS_ADC_HANDLE *pFBADC);
HAL_Status HAL_FLEXBUS_ADC_Init(struct FLEXBUS_ADC_HANDLE *pFBADC, struct FLEXBUS_HANDLE *pFB);
HAL_Status HAL_FLEXBUS_ADC_DeInit(struct FLEXBUS_ADC_HANDLE *pFBADC);

/** @} */

#endif

/** @} */

/** @} */

#endif  /* HAL_FLEXBUS_MODULE_ENABLED && HAL_FLEXBUS_ADC_MODULE_ENABLED */
