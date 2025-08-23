/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_RKNPU_MODULE_ENABLED

#ifndef _HAL_RKNPU_H_
#define _HAL_RKNPU_H_

#include "hal_def.h"

struct HAL_RKNPU_DEV {
    eCLOCK_Name aclk;
    eCLOCK_Name hclk;
    ePD_Id pd;
    uint8_t irq;  /**< irq number */
};

HAL_Status HAL_RKNPU_Init(struct HAL_RKNPU_DEV *rknpu);
HAL_Status HAL_RKNPU_DeInit(struct HAL_RKNPU_DEV *rknpu);
HAL_Status HAL_RKNPU_PowerOn(struct HAL_RKNPU_DEV *rknpu);
HAL_Status HAL_RKNPU_PowerOff(struct HAL_RKNPU_DEV *rknpu);

#endif

#endif
