/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_TRNG_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup TRNG
 *  @{
 */

#ifndef _HAL_TRNG_H_
#define _HAL_TRNG_H_

#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup TRNG_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/
/**
 * struct HAL_TRNG_DEV - trng dev struct.
 */
struct HAL_TRNG_DEV {
    struct TRNG_REG *pReg; /**< registers base address */
    eCLOCK_Name sclkID;
    uint32_t sclkGateID;
    uint32_t pclkGateID;

    IRQn_Type irqNum;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup TRNG_Public_Function_Declare Public Function Declare
 *  @{
 */

/**
 * @brief  Init trng and deassert reset.
 * @param  dev: the handle of trng.
 * @return HAL_Status
 */
HAL_Status HAL_TRNG_Init(const struct HAL_TRNG_DEV *dev);

/**
 * @brief  DeInit trng.
 * @param  dev: the handle of trng.
 * @return HAL_Status
 */
HAL_Status HAL_TRNG_DeInit(const struct HAL_TRNG_DEV *dev);

/**
 * @brief  get TRNG data
 * @param  dev: the handle of trng.
 * @param  data: trng buffer.
 * @param  len: trng buffer length.
 * @return HAL_Status
 */
HAL_Status HAL_TRNG_Get(const struct HAL_TRNG_DEV *dev, uint8_t *data, uint32_t len);

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_TRNG_MODULE_ENABLED */
