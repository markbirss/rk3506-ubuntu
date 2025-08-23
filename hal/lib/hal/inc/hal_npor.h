/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_NPOR_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup NPOR
 *  @{
 */

#ifndef _HAL_NPOR_H_
#define _HAL_NPOR_H_

#include "hal_def.h"

/***************************** MACRO Definition ******************************/

/***************************** Structure Definition **************************/

/***************************** Function Declare ******************************/
/** @defgroup NPOR_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Check HAL_NPOR_IsPowergood(void);

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_NPOR_MODULE_ENABLED */
