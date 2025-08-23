/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_DSMC_SLAVE_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup DSMC_SLAVE
 *  @{
 */

/** @defgroup DSMC_SLAVE_How_To_Use How To Use
 *  @{

 The DSMC_SLAVE driver can be used as follows:

 - Invoke HAL_DSMC_SLAVE_Init() to initialize DSMC_SLAVE.
 - Invoke HAL_DSMC_SLAVE_DeInit() to deinitialize DSMC_SLAVE.

 @} */

/** @defgroup DSMC_SLAVE_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define S2H_INT_FOR_DMA_NUM (15)
#define AXI_ADDR_4GB_RANGE  (1ULL << 32)

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/
static HAL_Status DSMC_SLAVE_DmaTrigger(struct LBC_SLAVE_CSR_REG *pReg)
{
    int timeOut = 1000;

    /* wait interrupt register empty */
    while (timeOut-- > 0) {
        if (!(READ_REG(pReg->LBC_S2H_INT_STA) &
              (0x1 << S2H_INT_FOR_DMA_NUM))) {
            break;
        }
        if (timeOut == 0) {
            HAL_DBG_ERR("Timeout waiting for s2h interrupt empty!\n");

            return HAL_TIMEOUT;
        }
        HAL_CPUDelayUs(1);
    }

    /* trigger a slave to host interrupt which will start dma hardware mode copy */
    WRITE_REG(pReg->APP_CON15, 0x1);

    return HAL_OK;
}

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup DSMC_SLAVE_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 *  @{
 */

/**
 * @brief  DSMC_SLAVE Init.
 * @param  slave: pointer to a DSMC_SLAVE structure.
 * @return HAL_Status
 */
HAL_Status HAL_DSMC_SLAVE_Init(struct HAL_DSMC_SLAVE *slave)
{
    struct LBC_SLAVE_CSR_REG *pReg;

    HAL_ASSERT(IS_LBC_SLAVE_CSR_INSTANCE(slave->pReg));
    pReg = slave->pReg;

#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkEnable(slave->aclkGate);
    HAL_CRU_ClkEnable(slave->hClkGate);
#endif

    if (slave->lbcMem.lbcMemBase < AXI_ADDR_4GB_RANGE) {
        WRITE_REG(pReg->AXI_WR_ADDR_BASE, slave->lbcMem.lbcMemBase);
        WRITE_REG(pReg->AXI_RD_ADDR_BASE, slave->lbcMem.lbcMemBase);
    } else {
        HAL_DBG_ERR("DSMC SLAVE: Invalid address for slave memory!\n");

        return HAL_ERROR;
    }

    /* clear all h2s interrupt */
    WRITE_REG(pReg->APP_H2S_INT_STA, LBC_SLAVE_CSR_APP_H2S_INT_STA_APP_H2S_INT_STA_MASK);

    /* enable all h2s interrupt */
    WRITE_REG(pReg->APP_H2S_INT_STA_EN, 0xffffffff);
    WRITE_REG(pReg->APP_H2S_INT_STA_SIG_EN, 0xffffffff);

    return HAL_OK;
}

/**
 * @brief  DSMC_SLAVE DeInit.
 * @param  slave: pointer to a DSMC_SLAVE structure.
 * @return HAL_Status
 */
HAL_Status HAL_DSMC_SLAVE_DeInit(struct HAL_DSMC_SLAVE *slave)
{
    HAL_ASSERT(IS_LBC_SLAVE_CSR_INSTANCE(slave->pReg));

#ifdef HAL_CRU_MODULE_ENABLED
    HAL_CRU_ClkDisable(slave->aclkGate);
    HAL_CRU_ClkDisable(slave->hClkGate);
#endif

    return HAL_OK;
}

/** @} */

/** @defgroup DSMC_SLAVE_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  DSMC_SLAVE irq hander.
 * @param  slave: pointer to a DSMC_SLAVE structure.
 * @return HAL_Status
 */
HAL_Status HAL_DSMC_SLAVE_IrqHander(struct HAL_DSMC_SLAVE *slave)
{
    struct LBC_SLAVE_CSR_REG *pReg;

    HAL_ASSERT(IS_LBC_SLAVE_CSR_INSTANCE(slave->pReg));
    pReg = slave->pReg;

    if (READ_REG(pReg->LBC_CON15)) {
        DSMC_SLAVE_DmaTrigger(pReg);
    }

    /* clear all h2s interrupt */
    WRITE_REG(pReg->APP_H2S_INT_STA, LBC_SLAVE_CSR_APP_H2S_INT_STA_APP_H2S_INT_STA_MASK);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_DSMC_SLAVE_MODULE_ENABLED */
