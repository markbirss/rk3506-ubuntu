/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_FLEXBUS_MODULE_ENABLED) && defined(HAL_FLEXBUS_ADC_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FLEXBUS_ADC
 *  @{
 */

/** @defgroup FLEXBUS_ADC_How_To_Use How To Use
 *  @{

 The FLEXBUS ADC driver can be used as follows:

 - Ensure that the upper-layer FLEXBUS_HANDLE is initialized.

 - Declare a FLEXBUS_ADC_HANDLE handle structure, for example:
   ```
   FLEXBUS_ADC_HANDLE instance;
   ```

 - Invoke HAL_FLEXBUS_ADC_Init() API to configure.

 - Read a block of data in single mode: HAL_FLEXBUS_ADC_ReadBlockEnable(), HAL_FLEXBUS_ADC_Isr() and HAL_FLEXBUS_ADC_ReadDisable().

 - Read data in continue mode: HAL_FLEXBUS_ADC_ReadContinueEnable(), HAL_FLEXBUS_ADC_ReadContinueStop(), HAL_FLEXBUS_ADC_Isr() and HAL_FLEXBUS_ADC_ReadDisable().

 - Invoke HAL_FLEXBUS_ADC_DeInit() if necessary.

 @} */

/** @defgroup FLEXBUS_ADC_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define FLEXBUS_ADC_ERR_IMR (FLEXBUS_IMR_DMA_TIMEOUT_IM_MASK | FLEXBUS_IMR_DMA_ERR_IM_MASK | \
                             FLEXBUS_IMR_RX_UDF_IM_MASK | FLEXBUS_IMR_RX_OVF_IM_MASK)
#define FLEXBUS_ADC_IMR     (FLEXBUS_ADC_ERR_IMR | FLEXBUS_IMR_RX_DONE_IM_MASK)
#define FLEXBUS_ADC_ERR_ISR (FLEXBUS_ISR_DMA_TIMEOUT_IS_MASK | FLEXBUS_ISR_DMA_ERR_IS_MASK | \
                             FLEXBUS_ISR_RX_UDF_IS_MASK | FLEXBUS_ISR_RX_OVF_IS_MASK)
#define FLEXBUS_ADC_ISR     (FLEXBUS_ADC_ERR_ISR | FLEXBUS_ISR_RX_DONE_IS_MASK)
#define FLEXBUS_ADC_ERR_ICR (FLEXBUS_ICR_CTIMEOUT_MASK | FLEXBUS_ICR_CDMA_ERR_MASK | \
                             FLEXBUS_ICR_CRX_UDF_MASK | FLEXBUS_ICR_CRX_OVF_MASK)

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup FLEXBUS_ADC_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

/**
 * @brief  Enable read a block of data.
 * @param  pFBADC: pointer to a FLEXBUS_ADC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS ADC mode.
 * @param  pDstBuf: pointer to the destination buffer.
 * @param  dstLen: length of the destination buffer.
 * @param  numOfDfs: number of dfs to transmit.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_ADC_ReadBlockEnable(struct FLEXBUS_ADC_HANDLE *pFBADC, void *pDstBuf,
                                           uint32_t dstLen, uint32_t numOfDfs)
{
    HAL_ASSERT(pFBADC != NULL);

    if (numOfDfs > 0x08000000) {
        return HAL_INVAL;
    }
    if (pDstBuf == NULL) {
        HAL_DBG_ERR("%s buf is null!\n", __func__);

        return HAL_INVAL;
    }

    WRITE_REG(pFBADC->pFB->pReg->RX_NUM, numOfDfs);
    MODIFY_REG(pFBADC->pFB->pReg->RX_CTL,
               FLEXBUS_RX_CTL_AUTO_PAD_MASK | FLEXBUS_RX_CTL_CUTOFF_CFG_MASK |
               FLEXBUS_RX_CTL_CONTINUE_MODE_MASK,
               FLEXBUS_RX_CTL_AUTO_PAD_MASK);
    WRITE_REG(pFBADC->pFB->pReg->DMA_DST_ADDR0, (uint32_t)pDstBuf >> 2);
    WRITE_REG(pFBADC->pFB->pReg->DMA_DST_LEN0, dstLen);

    WRITE_REG_MASK_WE(pFBADC->pFB->pReg->ENR, FLEXBUS_ENR_RX_ENR_MASK, FLEXBUS_ENR_RX_ENR_MASK);
    pFBADC->continueMode = 0;

    return HAL_OK;
}

/**
 * @brief  Enable read data in continue mode.
 * @param  pFBADC: pointer to a FLEXBUS_ADC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS ADC mode.
 * @param  pDstBuf0: pointer to the destination buffer0.
 * @param  pDstBuf1: pointer to the destination buffer1.
 * @param  dstLen: length of the destination buffer.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_ADC_ReadContinueEnable(struct FLEXBUS_ADC_HANDLE *pFBADC, void *pDstBuf0,
                                              void *pDstBuf1, uint32_t dstLen)
{
    uint32_t val;

    HAL_ASSERT(pFBADC != NULL);

    if (pDstBuf0 == NULL || pDstBuf1 == NULL) {
        HAL_DBG_ERR("%s buf is null!\n", __func__);

        return HAL_INVAL;
    }

    WRITE_REG(pFBADC->pFB->pReg->RX_NUM, dstLen * 8 / pFBADC->dfs);
    val = FLEXBUS_RX_CTL_CONTINUE_MODE_MASK;
    if (pFBADC->cutOff) {
        val |= FLEXBUS_RX_CTL_CUTOFF_CFG_MASK;
    }
    MODIFY_REG(pFBADC->pFB->pReg->RX_CTL,
               FLEXBUS_RX_CTL_AUTO_PAD_MASK | FLEXBUS_RX_CTL_CUTOFF_CFG_MASK |
               FLEXBUS_RX_CTL_CONTINUE_MODE_MASK,
               val);
    WRITE_REG(pFBADC->pFB->pReg->DMA_DST_ADDR0, (uint32_t)pDstBuf0 >> 2);
    WRITE_REG(pFBADC->pFB->pReg->DMA_DST_ADDR1, (uint32_t)pDstBuf1 >> 2);
    WRITE_REG(pFBADC->pFB->pReg->DMA_DST_LEN0, dstLen);
    WRITE_REG(pFBADC->pFB->pReg->DMA_DST_LEN1, dstLen);

    WRITE_REG_MASK_WE(pFBADC->pFB->pReg->ENR, FLEXBUS_ENR_RX_ENR_MASK, FLEXBUS_ENR_RX_ENR_MASK);
    pFBADC->continueMode = 1;

    return HAL_OK;
}

/**
 * @brief  Stop read data in continue mode.
 * @param  pFBADC: pointer to a FLEXBUS_ADC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS ADC mode.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_ADC_ReadContinueStop(struct FLEXBUS_ADC_HANDLE *pFBADC)
{
    HAL_ASSERT(pFBADC != NULL);

    pFBADC->continueMode = 0;
    SET_BIT(pFBADC->pFB->pReg->STOP, FLEXBUS_STOP_RX_STOP_MASK);

    return HAL_OK;
}

/**
 * @brief  Query the FLEXBUS rx status is idle or busy.
 * @param  pFBADC: pointer to a FLEXBUS_ADC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS ADC mode.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_ADC_QueryStatus(struct FLEXBUS_ADC_HANDLE *pFBADC)
{
    HAL_ASSERT(pFBADC != NULL);

    if (READ_REG(pFBADC->pFB->pReg->STATUS) & FLEXBUS_STATUS_RX_BUSY_MASK) {
        return HAL_BUSY;
    }

    return HAL_OK;
}

/**
 * @brief  Handle FLEXBUS ADC interrupt.
 * @param  pFBADC: pointer to a FLEXBUS_ADC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS ADC mode.
 * @param  isr: FLEXBUS interrupt
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_ADC_Isr(struct FLEXBUS_ADC_HANDLE *pFBADC, uint32_t isr)
{
    HAL_Status ret = HAL_OK;

    HAL_ASSERT(pFBADC != NULL);

    if (pFBADC->pFB->opMode1 != FLEXBUS1_OPMODE_ADC) {
        return HAL_INVAL;
    }

    if (isr & FLEXBUS_ISR_RX_DONE_IS_MASK) {
        pFBADC->result = FLEXBUS_ADC_DONE;
        SET_BIT(pFBADC->pFB->pReg->ICR, FLEXBUS_ICR_CRX_DONE_MASK);
    }

    if (isr & FLEXBUS_ADC_ERR_ISR) {
        if (isr & FLEXBUS_ISR_DMA_TIMEOUT_IS_MASK) {
            HAL_DBG_ERR("FLEXBUS_ADC dma timeout!\n");
        }
        if (isr & FLEXBUS_ISR_DMA_ERR_IS_MASK) {
            HAL_DBG_ERR("FLEXBUS_ADC dma err!\n");
        }
        if (isr & FLEXBUS_ISR_RX_UDF_IS_MASK) {
            HAL_DBG_ERR("FLEXBUS_ADC rx underflow!\n");
        }
        if (isr & FLEXBUS_ISR_RX_OVF_IS_MASK) {
            HAL_DBG_ERR("FLEXBUS_ADC rx overflow!\n");
        }

        if (pFBADC->continueMode && pFBADC->cutOff) {
            if (isr & (FLEXBUS_ISR_DMA_TIMEOUT_IS_MASK | FLEXBUS_ISR_DMA_ERR_IS_MASK)) {
                ret = HAL_ERROR;
            }
        } else {
            ret = HAL_ERROR;
        }

        pFBADC->result = FLEXBUS_ADC_ERR;
        SET_BIT(pFBADC->pFB->pReg->ICR, FLEXBUS_ADC_ERR_ICR);
    }

    return ret;
}

/**
 * @brief  Disable read.
 * @param  pFBADC: pointer to a FLEXBUS_ADC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS ADC mode.
 */
void HAL_FLEXBUS_ADC_ReadDisable(struct FLEXBUS_ADC_HANDLE *pFBADC)
{
    HAL_ASSERT(pFBADC != NULL);

    WRITE_REG_MASK_WE(pFBADC->pFB->pReg->ENR, FLEXBUS_ENR_RX_ENR_MASK, 0x0);
}

/** @} */

/** @defgroup FLEXBUS_ADC_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit module as follows:

 *  @{
 */

/**
 * @brief  Initialize FLEXBUS ADC mode.
 * @param  pFBADC: pointer to a FLEXBUS_ADC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS ADC mode.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_ADC_Init(struct FLEXBUS_ADC_HANDLE *pFBADC, struct FLEXBUS_HANDLE *pFB)
{
    uint32_t val;

    HAL_ASSERT(pFBADC != NULL);
    HAL_ASSERT(pFB != NULL);
    HAL_ASSERT(IS_FLEXBUS_INSTANCE(pFB->pReg));

    if (pFB->opMode1 != FLEXBUS1_OPMODE_ADC) {
        return HAL_INVAL;
    }

    pFBADC->pFB = pFB;

    if (!pFBADC->slaveMode && pFBADC->freeSclk) {
        WRITE_REG_MASK_WE(pFBADC->pFB->pReg->FREE_SCLK,
                          FLEXBUS_FREE_SCLK_RX_FREE_MODE_MASK, FLEXBUS_FREE_SCLK_RX_FREE_MODE_MASK);
    }
    if (pFBADC->cutOff && (pFBADC->slaveMode || pFBADC->freeSclk)) {
        HAL_DBG_WRN("FLEXBUS_ADC cannot support cutOff in slaveMode or freeSclk.\n");
        pFBADC->cutOff = 0;
    }

    val = 0x0;
    if (pFBADC->slaveMode) {
        val |= FLEXBUS_SLAVE_MODE_CLK1_IN_MASK;
    }
    WRITE_REG(pFBADC->pFB->pReg->SLAVE_MODE, val);

    switch (pFBADC->dfs) {
    case 4:
        val = pFBADC->pFB->pDfsReg->dfs4Bit;
        break;
    case 8:
        val = pFBADC->pFB->pDfsReg->dfs8Bit;
        break;
    case 16:
        val = pFBADC->pFB->pDfsReg->dfs16Bit;
        break;
    default:
        return HAL_INVAL;
    }
    if (!pFBADC->slaveMode) {
        val |= FLEXBUS_RX_CTL_RXD_DY_MASK;
    }
    if (pFBADC->cPol) {
        val |= FLEXBUS_RX_CTL_CPOL_MASK;
    }
    if (pFBADC->cPha) {
        val |= FLEXBUS_RX_CTL_CPHA_MASK;
    }
    WRITE_REG(pFBADC->pFB->pReg->RX_CTL, val);

    SET_BIT(pFBADC->pFB->pReg->IMR, FLEXBUS_ADC_IMR);

    HAL_FLEXBUS_GrfConfig(pFBADC->pFB, pFBADC->slaveMode, pFBADC->cPol, pFBADC->cPha);

    return HAL_OK;
}

/**
 * @brief  DeInitialize FLEXBUS ADC mode.
 * @param  pFBADC: pointer to a FLEXBUS_ADC_HANDLE structure that contains
 *                 the configuration information for FLEXBUS ADC mode.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_ADC_DeInit(struct FLEXBUS_ADC_HANDLE *pFBADC)
{
    HAL_ASSERT(pFBADC != NULL);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif  /* HAL_FLEXBUS_MODULE_ENABLED && HAL_FLEXBUS_ADC_MODULE_ENABLED */
