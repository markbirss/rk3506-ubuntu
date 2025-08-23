/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023-2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_DSMC_SLAVE_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup DSMC_SLAVE
 *  @{
 */

#ifndef _HAL_DSMC_SLAVE_H_
#define _HAL_DSMC_SLAVE_H_

#include "hal_base.h"

/***************************** MACRO Definition ******************************/
/** @defgroup DSMC_SLAVE_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/
struct LBC_SLAVE_MEM {
    uintptr_t lbcMemBase;
    size_t lbcMemSize;
};

struct HAL_DSMC_SLAVE {
    struct LBC_SLAVE_CSR_REG *pReg;     /**< Specifies DSMC_SLAVE register base address */
    uint32_t aclkGate;                  /**< Specifies DSMC_SLAVE aclk Gate */
    uint32_t hClkGate;                  /**< Specifies DSMC_SLAVE hclk Gate */
    uint32_t rst;                       /**< Specifies DSMC_SLAVE reset */
    uint32_t arst;                      /**< Specifies DSMC_SLAVE areset */
    uint32_t hrst;                      /**< Specifies DSMC_SLAVE hreset */
    IRQn_Type irqNum;                   /**< Specifies DSMC_SLAVE irq number */
    struct LBC_SLAVE_MEM lbcMem;        /**< Specifies DSMC_SLAVE memory region */
};
/** @} */
/***************************** Function Declare ******************************/
/** @defgroup DSMC_SLAVE_Public_Function_Declare Public Function Declare
 *  @{
 */
HAL_Status HAL_DSMC_SLAVE_Init(struct HAL_DSMC_SLAVE *slave);
HAL_Status HAL_DSMC_SLAVE_DeInit(struct HAL_DSMC_SLAVE *slave);
HAL_Status HAL_DSMC_SLAVE_IrqHander(struct HAL_DSMC_SLAVE *slave);

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_DSMC_SLAVE_MODULE_ENABLED */