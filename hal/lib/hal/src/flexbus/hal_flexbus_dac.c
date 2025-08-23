/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_FLEXBUS_MODULE_ENABLED) && defined(HAL_FLEXBUS_DAC_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FLEXBUS_DAC
 *  @{
 */

/** @defgroup FLEXBUS_DAC_How_To_Use How To Use
 *  @{

 The FLEXBUS DAC driver can be used as follows:

 - Ensure that the upper-layer FLEXBUS_HANDLE is initialized.

 - Declare a FLEXBUS_DAC_HANDLE handle structure, for example:
   ```
   FLEXBUS_DAC_HANDLE instance;
   ```

 - Invoke HAL_FLEXBUS_DAC_Init() API to configure.

 - Write a block of data in single mode: HAL_FLEXBUS_DAC_WriteBlockEnable(), HAL_FLEXBUS_DAC_Isr() and HAL_FLEXBUS_DAC_WriteDisable().

 - Write data in continue mode: HAL_FLEXBUS_DAC_WriteContinueEnable(), HAL_FLEXBUS_DAC_WriteContinueStop(), HAL_FLEXBUS_DAC_Isr() and HAL_FLEXBUS_DAC_WriteDisable().

 - Invoke HAL_FLEXBUS_DAC_DeInit() if necessary.

 @} */

/** @defgroup FLEXBUS_DAC_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define FLEXBUS_DAC_ERR_IMR (FLEXBUS_IMR_DMA_TIMEOUT_IM_MASK | FLEXBUS_IMR_DMA_ERR_IM_MASK | \
                             FLEXBUS_IMR_TX_UDF_IM_MASK | FLEXBUS_IMR_TX_OVF_IM_MASK)
#define FLEXBUS_DAC_IMR     (FLEXBUS_DAC_ERR_IMR | FLEXBUS_IMR_TX_DONE_IM_MASK)
#define FLEXBUS_DAC_ERR_ISR (FLEXBUS_ISR_DMA_TIMEOUT_IS_MASK | FLEXBUS_ISR_DMA_ERR_IS_MASK | \
                             FLEXBUS_ISR_TX_UDF_IS_MASK | FLEXBUS_ISR_TX_OVF_IS_MASK)
#define FLEXBUS_DAC_ISR     (FLEXBUS_DAC_ERR_ISR | FLEXBUS_ISR_TX_DONE_IS_MASK)
#define FLEXBUS_DAC_ERR_ICR (FLEXBUS_ICR_CTIMEOUT_MASK | FLEXBUS_ICR_CDMA_ERR_MASK | \
                             FLEXBUS_ICR_CTX_UDF_MASK | FLEXBUS_ICR_CTX_OVF_MASK)

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup FLEXBUS_DAC_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

/**
 * @brief  Enable write a block of data.
 * @param  pFBDAC: pointer to a FLEXBUS_DAC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS DAC mode.
 * @param  pSrcBuf: pointer to the source buffer.
 * @param  srcLen: length of the source buffer.
 * @param  numOfDfs: number of dfs to transmit.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_DAC_WriteBlockEnable(struct FLEXBUS_DAC_HANDLE *pFBDAC, void *pSrcBuf,
                                            uint32_t srcLen, uint32_t numOfDfs)
{
    uint32_t val;

    HAL_ASSERT(pFBDAC != NULL);

    if (numOfDfs > 0x08000000) {
        return HAL_INVAL;
    }
    if (pSrcBuf == NULL) {
        HAL_DBG_ERR("%s buf is null!\n", __func__);

        return HAL_INVAL;
    }

    WRITE_REG(pFBDAC->pFB->pReg->TX_NUM, numOfDfs);
    val = numOfDfs * pFBDAC->dfs / 32 / 2;
    if (val < 16) {
        val = 16;
    } else if (val > pFBDAC->pFB->txwatStartMax) {
        val = pFBDAC->pFB->txwatStartMax;
    }
    WRITE_REG(pFBDAC->pFB->pReg->TXWAT_START, val);

    CLEAR_BIT(pFBDAC->pFB->pReg->TX_CTL,
              FLEXBUS_TX_CTL_CUTOFF_CFG_MASK | FLEXBUS_TX_CTL_CONTINUE_MODE_MASK);
    WRITE_REG(pFBDAC->pFB->pReg->DMA_SRC_ADDR0, (uint32_t)pSrcBuf >> 2);
    WRITE_REG(pFBDAC->pFB->pReg->DMA_SRC_LEN0, srcLen);

    WRITE_REG_MASK_WE(pFBDAC->pFB->pReg->ENR, FLEXBUS_ENR_TX_ENR_MASK, FLEXBUS_ENR_TX_ENR_MASK);
    pFBDAC->continueMode = 0;

    return HAL_OK;
}

/**
 * @brief  Enable write data in continue mode.
 * @param  pFBDAC: pointer to a FLEXBUS_DAC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS DAC mode.
 * @param  pSrcBuf0: pointer to the source buffer0.
 * @param  pSrcBuf1: pointer to the source buffer1.
 * @param  srcLen: length of the source buffer.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_DAC_WriteContinueEnable(struct FLEXBUS_DAC_HANDLE *pFBDAC, void *pSrcBuf0,
                                               void *pSrcBuf1, uint32_t srcLen)
{
    uint32_t val;

    HAL_ASSERT(pFBDAC != NULL);

    if (pSrcBuf0 == NULL || pSrcBuf1 == NULL) {
        HAL_DBG_ERR("%s buf is null!\n", __func__);

        return HAL_INVAL;
    }

    WRITE_REG(pFBDAC->pFB->pReg->TX_NUM, srcLen * 8 / pFBDAC->dfs);
    WRITE_REG(pFBDAC->pFB->pReg->TXWAT_START, pFBDAC->pFB->txwatStartMax);

    val = FLEXBUS_TX_CTL_CONTINUE_MODE_MASK;
    if (pFBDAC->cutOff) {
        val |= FLEXBUS_TX_CTL_CUTOFF_CFG_MASK;
    }
    MODIFY_REG(pFBDAC->pFB->pReg->TX_CTL,
               FLEXBUS_TX_CTL_CUTOFF_CFG_MASK | FLEXBUS_TX_CTL_CONTINUE_MODE_MASK,
               val);
    WRITE_REG(pFBDAC->pFB->pReg->DMA_SRC_ADDR0, (uint32_t)pSrcBuf0 >> 2);
    WRITE_REG(pFBDAC->pFB->pReg->DMA_SRC_ADDR1, (uint32_t)pSrcBuf1 >> 2);
    WRITE_REG(pFBDAC->pFB->pReg->DMA_SRC_LEN0, srcLen);
    WRITE_REG(pFBDAC->pFB->pReg->DMA_SRC_LEN1, srcLen);

    WRITE_REG_MASK_WE(pFBDAC->pFB->pReg->ENR, FLEXBUS_ENR_TX_ENR_MASK, FLEXBUS_ENR_TX_ENR_MASK);
    pFBDAC->continueMode = 1;

    return HAL_OK;
}

/**
 * @brief  Stop write data in continue mode.
 * @param  pFBDAC: pointer to a FLEXBUS_DAC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS DAC mode.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_DAC_WriteContinueStop(struct FLEXBUS_DAC_HANDLE *pFBDAC)
{
    HAL_ASSERT(pFBDAC != NULL);

    pFBDAC->continueMode = 0;
    SET_BIT(pFBDAC->pFB->pReg->STOP, FLEXBUS_STOP_TX_STOP_MASK);

    return HAL_OK;
}

/**
 * @brief  Query the FLEXBUS tx status is idle or busy.
 * @param  pFBDAC: pointer to a FLEXBUS_DAC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS DAC mode.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_DAC_QueryStatus(struct FLEXBUS_DAC_HANDLE *pFBDAC)
{
    HAL_ASSERT(pFBDAC != NULL);

    if (READ_REG(pFBDAC->pFB->pReg->STATUS) & FLEXBUS_STATUS_TX_BUSY_MASK) {
        return HAL_BUSY;
    }

    return HAL_OK;
}

/**
 * @brief  Handle FLEXBUS DAC interrupt.
 * @param  pFBDAC: pointer to a FLEXBUS_DAC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS DAC mode.
 * @param  isr: FLEXBUS interrupt
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_DAC_Isr(struct FLEXBUS_DAC_HANDLE *pFBDAC, uint32_t isr)
{
    HAL_Status ret = HAL_OK;

    HAL_ASSERT(pFBDAC != NULL);

    if (pFBDAC->pFB->opMode0 != FLEXBUS0_OPMODE_DAC) {
        return HAL_INVAL;
    }

    if (isr & FLEXBUS_ISR_TX_DONE_IS_MASK) {
        pFBDAC->result = FLEXBUS_DAC_DONE;
        SET_BIT(pFBDAC->pFB->pReg->ICR, FLEXBUS_ICR_CTX_DONE_MASK);
    }

    if (isr & FLEXBUS_DAC_ERR_ISR) {
        if (isr & FLEXBUS_ISR_DMA_TIMEOUT_IS_MASK) {
            HAL_DBG_ERR("FLEXBUS_DAC dma timeout!\n");
        }
        if (isr & FLEXBUS_ISR_DMA_ERR_IS_MASK) {
            HAL_DBG_ERR("FLEXBUS_DAC dma err!\n");
        }
        if (isr & FLEXBUS_ISR_TX_UDF_IS_MASK) {
            HAL_DBG_ERR("FLEXBUS_DAC tx underflow!\n");
        }
        if (isr & FLEXBUS_ISR_TX_OVF_IS_MASK) {
            HAL_DBG_ERR("FLEXBUS_DAC tx overflow!\n");
        }

        if (pFBDAC->continueMode && pFBDAC->cutOff) {
            if (isr & (FLEXBUS_ISR_DMA_TIMEOUT_IS_MASK | FLEXBUS_ISR_DMA_ERR_IS_MASK)) {
                ret = HAL_ERROR;
            }
        } else {
            ret = HAL_ERROR;
        }

        pFBDAC->result = FLEXBUS_DAC_ERR;
        SET_BIT(pFBDAC->pFB->pReg->ICR, FLEXBUS_DAC_ERR_ICR);
    }

    return ret;
}

/**
 * @brief  Disable Write.
 * @param  pFBDAC: pointer to a FLEXBUS_DAC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS DAC mode.
 */
void HAL_FLEXBUS_DAC_WriteDisable(struct FLEXBUS_DAC_HANDLE *pFBDAC)
{
    HAL_ASSERT(pFBDAC != NULL);

    WRITE_REG_MASK_WE(pFBDAC->pFB->pReg->ENR, FLEXBUS_ENR_TX_ENR_MASK, 0x0);
}

/** @} */

/** @defgroup FLEXBUS_DAC_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit module as follows:

 *  @{
 */

/**
 * @brief  Initialize FLEXBUS DAC mode.
 * @param  pFBDAC: pointer to a FLEXBUS_DAC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS DAC mode.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_DAC_Init(struct FLEXBUS_DAC_HANDLE *pFBDAC, struct FLEXBUS_HANDLE *pFB)
{
    uint32_t val;

    HAL_ASSERT(pFBDAC != NULL);
    HAL_ASSERT(pFB != NULL);
    HAL_ASSERT(IS_FLEXBUS_INSTANCE(pFB->pReg));

    if (pFB->opMode0 != FLEXBUS0_OPMODE_DAC) {
        return HAL_INVAL;
    }

    pFBDAC->pFB = pFB;

    if (pFBDAC->freeSclk) {
        WRITE_REG_MASK_WE(pFBDAC->pFB->pReg->FREE_SCLK,
                          FLEXBUS_FREE_SCLK_TX_FREE_MODE_MASK, FLEXBUS_FREE_SCLK_TX_FREE_MODE_MASK);
    }

    switch (pFBDAC->dfs) {
    case 4:
        val = pFBDAC->pFB->pDfsReg->dfs4Bit;
        break;
    case 8:
        val = pFBDAC->pFB->pDfsReg->dfs8Bit;
        break;
    case 16:
        val = pFBDAC->pFB->pDfsReg->dfs16Bit;
        break;
    default:
        return HAL_INVAL;
    }
    if (pFBDAC->cPol) {
        val |= FLEXBUS_TX_CTL_CPOL_MASK;
    }
    if (pFBDAC->cPha) {
        val |= FLEXBUS_TX_CTL_CPHA_MASK;
    }
    WRITE_REG(pFBDAC->pFB->pReg->TX_CTL, val);

    SET_BIT(pFBDAC->pFB->pReg->IMR, FLEXBUS_DAC_IMR);

    return HAL_OK;
}

/**
 * @brief  DeInitialize FLEXBUS DAC mode.
 * @param  pFBDAC: pointer to a FLEXBUS_DAC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS DAC mode.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_DAC_DeInit(struct FLEXBUS_DAC_HANDLE *pFBDAC)
{
    HAL_ASSERT(pFBDAC != NULL);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif  /* HAL_FLEXBUS_MODULE_ENABLED && HAL_FLEXBUS_DAC_MODULE_ENABLED */
