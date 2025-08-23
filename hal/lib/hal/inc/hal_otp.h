/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */
#include "hal_conf.h"

#ifdef HAL_OTP_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup OTP
 *  @{
 */

#ifndef _HAL_OTP_H_
#define _HAL_OTP_H_

#include "hal_def.h"

/***************************** MACRO Definition ******************************/

/***************************** Structure Definition **************************/

/***************************** Function Declare ******************************/
/** @defgroup OTP_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_OTP_Read(struct OTPC_REG *reg, uint32_t addr, uint32_t *value);

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_OTP_MODULE_ENABLED */
