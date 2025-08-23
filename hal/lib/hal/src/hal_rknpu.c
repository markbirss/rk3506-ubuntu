/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * @file    hal_rknpu.c
 * @author  randall zhuo
 * @version V0.1
 * @date    23-Jan-2024
 * @brief   rknpu hal handle
 *
 ******************************************************************************
 */

#include "hal_base.h"

#ifdef HAL_RKNPU_MODULE_ENABLED

#include "hal_rknpu.h"

/**
 * @brief  Init rknpu.
 * @param  rknpu: The handle of struct rknpu.
 * @return HAL_Status
 */
HAL_Status HAL_RKNPU_Init(struct HAL_RKNPU_DEV *rknpu)
{
    rknpu->aclk = ACLK_NPU;
    rknpu->hclk = HCLK_NPU;

#if defined(HAL_PD_MODULE_ENABLED)
    rknpu->pd = PD_NPU;
#endif

    return HAL_OK;
}

/**
 * @brief  DeInit rknpu.
 * @param  rknpu: The handle of struct rknpu.
 * @return HAL_Status
 */
HAL_Status HAL_RKNPU_DeInit(struct HAL_RKNPU_DEV *rknpu)
{
    return HAL_OK;
}

/**
 * @brief  PowerON rknpu.
 * @param  rknpu: The handle of struct rknpu.
 * @return HAL_Status
 */
HAL_Status HAL_RKNPU_PowerOn(struct HAL_RKNPU_DEV *rknpu)
{
    HAL_CRU_ClkEnable((uint32_t)rknpu->aclk);
    HAL_CRU_ClkEnable((uint32_t)rknpu->hclk);
#if defined(HAL_PD_MODULE_ENABLED)
    HAL_PD_On(PD_NPU);
#endif

    return HAL_OK;
}

/**
 * @brief  Poweroff rknpu.
 * @param  rknpu: The handle of struct rknpu.
 * @return HAL_Status
 */
HAL_Status HAL_RKNPU_PowerOff(struct HAL_RKNPU_DEV *rknpu)
{
#if defined(HAL_PD_MODULE_ENABLED)
    HAL_PD_Off(PD_NPU);
#endif
    HAL_CRU_ClkDisable((uint32_t)rknpu->hclk);
    HAL_CRU_ClkDisable((uint32_t)rknpu->aclk);

    return HAL_OK;
}
#endif
