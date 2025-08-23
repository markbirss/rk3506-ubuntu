/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_SPI2AHB_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup SPI2AHB
 *  @{
 */

#ifndef _HAL_SPI2AHB_H_
#define _HAL_SPI2AHB_H_

#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup SPI2AHB_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/

/**
 *  SPI2AHB controller configuration
 */
union SPI2AHB_CTRL {
    uint32_t d32;
    struct {
        unsigned fbm : 1;   /**< 0b: lsb; 1b: msb */
        unsigned ddr :  1;  /**< 0b: str; 1b: ddr */
        unsigned cs : 1;    /**< 0b: cs low; 1b: cs high */
        unsigned spim :  1; /**< 0b: mode0; 1b: mode3 */
        unsigned datb : 2;  /**< 00b: 1bit; 01b: 2bits; 10b: 4bits */
        unsigned reserved31_6 : 26;
    } b;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup SPI2AHB_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_SPI2AHB_Init(struct SPI2AHB_REG *pReg, uint32_t ctrl);
HAL_Status HAL_SPI2AHB_DeInit(struct SPI2AHB_REG *pReg);
HAL_Status HAL_SPI2AHB_FrameConfig(struct SPI2AHB_REG *pReg, uint32_t addr, uint32_t length);
HAL_Status HAL_SPI2AHB_Enable(struct SPI2AHB_REG *pReg);
HAL_Status HAL_SPI2AHB_Disable(struct SPI2AHB_REG *pReg);

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_SPI2AHB_MODULE_ENABLED */
