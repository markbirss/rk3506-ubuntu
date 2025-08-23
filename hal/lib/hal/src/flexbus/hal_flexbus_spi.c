/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_FLEXBUS_MODULE_ENABLED) && defined(HAL_FLEXBUS_SPI_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FLEXBUS_SPI
 *  @{
 */

/** @defgroup FLEXBUS_SPI_How_To_Use How To Use
 *  @{

 The FLEXBUS SPI driver can be used as follows:

 - Invoke HAL_FLEXBUS_SPI_Init() for FLEXBUS_SPI handler initial.

 - Invoke HAL_FLEXBUS_SPI_Config() for FLEXBUS_SPI configuration.

 - Invoke HAL_FLEXBUS_SPI_IrqHandler() helper to IRQ isr.

 - Invoke HAL_FLEXBUS_SPI_SendStart() to begin one output transmission.

 - Invoke HAL_FLEXBUS_SPI_RecvStart() to begin one input transmission.

 - Invoke HAL_FLEXBUS_SPI_IsXferDone() to check poll mode transmission state.

 - Invoke HAL_FLEXBUS_SPI_Stop() to stop one transmission.

 - Invoke HAL_FLEXBUS_SPI_SetCS() to set cs.

 - Invoke HAL_FLEXBUS_SPI_DeInit() for FLEXBUS_SPI handler de-initial.

 @} */

/** @defgroup FLEXBUS_SPI_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define FLEXBUS_SPI_ERR_ISR (FLEXBUS_ISR_DMA_TIMEOUT_IS_MASK | FLEXBUS_ISR_DMA_ERR_IS_MASK | \
                 FLEXBUS_ISR_TX_UDF_IS_MASK | FLEXBUS_ISR_TX_OVF_IS_MASK)

#define FLEXBUS_RX_ENR (HAL_BIT(16 + 1) | HAL_BIT(1))
#define FLEXBUS_TX_ENR (HAL_BIT(16) | HAL_BIT(0))

#define HAL_FLEXBUS_MAX_DLL_CELLS 0xff

/********************* Private Function Definition ***************************/
static void FLEXBUS_SPI_SetDelayLines(struct HAL_FLEXBUS_SPI_HANDLE *flexbus, uint32_t cells)
{
    if (cells) {
        WRITE_REG(flexbus->fb->pReg->DLL_EN, 1);
        WRITE_REG(flexbus->fb->pReg->DLL_NUM, cells);
    } else {
        WRITE_REG(flexbus->fb->pReg->DLL_EN, 0);
    }
}

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup FLEXBUS_SPI_Exported_Functions_Group2 State and Errors Functions
 *  @{
 */

/**
 * @brief  Confirm the transmission status of the controller.
 * @param  flexbus: FLEXBUS SPI handler.
 * @return HAL_Status, HAL_OK for finish state, HAL_ERROR for error state, HAL_Busy for busy state.
 */
HAL_Status HAL_FLEXBUS_SPI_IsXferDone(struct HAL_FLEXBUS_SPI_HANDLE *flexbus)
{
    uint32_t val, status;

    HAL_ASSERT(flexbus != NULL);

    val = READ_REG(flexbus->fb->pReg->RISR);
    status = READ_REG(flexbus->fb->pReg->STATUS);

    if (val & (FLEXBUS_RISR_RX_DONE_RIS_MASK | FLEXBUS_RISR_TX_DONE_RIS_MASK)) {
        return HAL_OK;
    } else if (val & FLEXBUS_SPI_ERR_ISR) {
        return HAL_ERROR;
    } else if (status & (FLEXBUS_STATUS_TX_BUSY_MASK | FLEXBUS_STATUS_RX_BUSY_MASK)) {
        return HAL_BUSY;
    } else {
        return HAL_OK;
    }
}

/** @} */

/** @defgroup FLEXBUS_SPI_Exported_Functions_Group3 IO Functions
 *  @{
 */

/**
 * @brief  Start FLEXBUS SPI mode send transmission.
 * @param  flexbus: FLEXBUS SPI handler.
 * @param  addr: dma dst physical address.
 * @param  size: transmission length in bytes.
 * @return HAL_Status.
 */
HAL_Status HAL_FLEXBUS_SPI_SendStart(struct HAL_FLEXBUS_SPI_HANDLE *flexbus, uint32_t addr, uint32_t size)
{
    uint32_t ctrl, len;

    HAL_ASSERT(flexbus != NULL);

    len = size * 8 / flexbus->cfg.width;

    ctrl = FLEXBUS_TX_ONLY;
    WRITE_REG(flexbus->fb->pReg->COM_CTL, ctrl);
    if (flexbus->useIrq) {
        WRITE_REG(flexbus->fb->pReg->IMR, FLEXBUS_IMR_TX_DONE_IM_MASK);
    }

    WRITE_REG(flexbus->fb->pReg->TX_NUM, len);
    WRITE_REG(flexbus->fb->pReg->DMA_SRC_ADDR0, addr >> 2);
    WRITE_REG(flexbus->fb->pReg->ENR, FLEXBUS_TX_ENR);

    return HAL_OK;
}

/**
 * @brief  Start FLEXBUS SPI mode receiver transmission.
 * @param  flexbus: FLEXBUS SPI handler.
 * @param  addr: dma dst physical address.
 * @param  size: transmission length in bytes.
 * @return HAL_Status.
 */
HAL_Status HAL_FLEXBUS_SPI_RecvStart(struct HAL_FLEXBUS_SPI_HANDLE *flexbus, uint32_t addr, uint32_t size)
{
    uint32_t ctrl, len;

    HAL_ASSERT(flexbus != NULL);

    len = size * 8 / flexbus->cfg.width;

    ctrl = FLEXBUS_RX_ONLY | FLEXBUS_COM_CTL_SCLK_SHARE_MASK | FLEXBUS_COM_CTL_TX_USE_RX_MASK;
    WRITE_REG(flexbus->fb->pReg->COM_CTL, ctrl);
    if (flexbus->useIrq) {
        WRITE_REG(flexbus->fb->pReg->IMR, FLEXBUS_IMR_RX_DONE_IM_MASK);
    }

    WRITE_REG(flexbus->fb->pReg->TX_NUM, 0);
    WRITE_REG(flexbus->fb->pReg->RX_NUM, len);
    WRITE_REG(flexbus->fb->pReg->DMA_DST_ADDR0, addr >> 2);
    WRITE_REG(flexbus->fb->pReg->ENR, FLEXBUS_RX_ENR);

    return HAL_OK;
}

/**
 * @brief  Stop FLEXBUS SPI mode transmission.
 * @param  flexbus: FLEXBUS SPI handler.
 * @return HAL_Status
 */
HAL_Status HAL_FLEXBUS_SPI_Stop(struct HAL_FLEXBUS_SPI_HANDLE *flexbus)
{
    HAL_ASSERT(flexbus != NULL);

    WRITE_REG(flexbus->fb->pReg->ENR, 0xFFFF0000);
    if (flexbus->useIrq) {
        WRITE_REG(flexbus->fb->pReg->ICR, 0xFFFFFFFF);
        WRITE_REG(flexbus->fb->pReg->IMR, 0);
    }

    return HAL_OK;
}

/**
 * @brief  FLEXBUS SPI mode chips select.
 * @param  flexbus: FLEXBUS SPI handler.
 * @param  enable: 1 for cs assert, 0 for cs de-assert.
 * @return HAL_Status
 */
HAL_Status HAL_FLEXBUS_SPI_SetCS(struct HAL_FLEXBUS_SPI_HANDLE *flexbus, bool enable)
{
    HAL_ASSERT(flexbus != NULL);

    if (enable) {
        WRITE_REG(flexbus->fb->pReg->CSN_CFG, 0x30003);
    } else {
        WRITE_REG(flexbus->fb->pReg->CSN_CFG, 0x30000);
    }

    return HAL_OK;
}

/** @} */

/** @defgroup FLEXBUS_SPI_Exported_Functions_Group4 Init and DeInit Functions
 *  @{
 */

/**
 * @brief  Initialize FLEXBUS SPI mode.
 * @param  flexbus: FLEXBUS SPI handler.
 * @param  pFBDev: FLEXBUS handler.
 * @param  useIrq: useIrq or poll mode.
 * @return HAL_Status
 */
HAL_Status HAL_FLEXBUS_SPI_Init(struct HAL_FLEXBUS_SPI_HANDLE *flexbus, const struct HAL_FLEXBUS_DEV *pFBDev, bool useIrq)
{
    HAL_Status ret = HAL_OK;

    HAL_ASSERT(flexbus != NULL);
    HAL_ASSERT(pFBDev != NULL);

    if (pFBDev->opMode0 != FLEXBUS0_OPMODE_SPI || pFBDev->opMode1 != FLEXBUS1_OPMODE_NULL) {
        return HAL_INVAL;
    }

    flexbus->useIrq = useIrq;

    flexbus->version = READ_REG(flexbus->fb->pReg->REVISION) >> 16;
    flexbus->maxSize = HAL_FLEXBUS_SPI_MAX_IOSIZE;

    WRITE_REG(flexbus->fb->pReg->TXWAT_START, 0x10);
    WRITE_REG(flexbus->fb->pReg->DMA_SRC_LEN0, flexbus->maxSize);
    WRITE_REG(flexbus->fb->pReg->DMA_DST_LEN0, flexbus->maxSize);

    /* Using internal clk as sample clock */
    WRITE_REG(flexbus->fb->pReg->SLAVE_MODE, 0);

    return ret;
}

/**
 * @brief  Deinitialize FLEXBUS.
 * @param  flexbus: FLEXBUS SPI handler.
 * @return HAL_Status
 */
HAL_Status HAL_FLEXBUS_SPI_DeInit(struct HAL_FLEXBUS_SPI_HANDLE *flexbus)
{
    HAL_ASSERT(flexbus != NULL);

    return HAL_OK;
}

/** @} */

/** @defgroup FLEXBUS_SPI_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  FLEXBUS SPI mode controller configuration.
 * @param  flexbus: FLEXBUS SPI handler.
 * @param  cfg: Configuration.
 * @return HAL_Status
 */
HAL_Status HAL_FLEXBUS_SPI_Config(struct HAL_FLEXBUS_SPI_HANDLE *flexbus, struct HAL_FLEXBUS_SPI_CONFIG *cfg)
{
    uint32_t ctrl, dfs_cfg;

    HAL_ASSERT(flexbus != NULL && cfg != NULL);

    if (cfg->dll > HAL_FLEXBUS_MAX_DLL_CELLS) {
        return HAL_INVAL;
    }

    switch (cfg->width) {
    case 1:
        dfs_cfg = flexbus->fb->pDfsReg->dfs1Bit;
        break;
    case 2:
        dfs_cfg = flexbus->fb->pDfsReg->dfs2Bit;
        break;
    case 4:
        dfs_cfg = flexbus->fb->pDfsReg->dfs4Bit;
        break;
    case 8:
        dfs_cfg = flexbus->fb->pDfsReg->dfs8Bit;
        break;
    case 16:
        dfs_cfg = flexbus->fb->pDfsReg->dfs16Bit;
        break;
    default:
        return HAL_INVAL;
    }

    /* Din[1] PAD input valid when bits_per_word=1, */
    if (cfg->width == 1) {
        WRITE_REG(flexbus->fb->pReg->REMAP, 0x10);
    } else {
        WRITE_REG(flexbus->fb->pReg->REMAP, 0x0);
    }

    HAL_FLEXBUS_GrfConfig(flexbus->fb, false, cfg->mode & HAL_BIT(1), cfg->mode & HAL_BIT(0));

    ctrl = FLEXBUS_TX_CTL_DAT_UNIT_MASK | dfs_cfg;
    ctrl |= (cfg->mode & 0x3U) << FLEXBUS_TX_CTL_CPHA_SHIFT;
    if (!cfg->lsb) {
        ctrl |= FLEXBUS_TX_CTL_SHIFT_MASK;
    }
    WRITE_REG(flexbus->fb->pReg->TX_CTL, ctrl);

    ctrl = FLEXBUS_RX_CTL_DAT_UNIT_MASK | dfs_cfg | FLEXBUS_RX_CTL_RXD_DY_MASK | FLEXBUS_RX_CTL_AUTO_PAD_MASK;
    ctrl |= (cfg->mode & 0x3U) << FLEXBUS_RX_CTL_CPHA_SHIFT;
    if (!cfg->lsb) {
        ctrl |= FLEXBUS_RX_CTL_SHIFT_MASK;
    }
    WRITE_REG(flexbus->fb->pReg->RX_CTL, ctrl);

    FLEXBUS_SPI_SetDelayLines(flexbus, cfg->dll);

    memcpy(&flexbus->cfg, cfg, sizeof(struct HAL_FLEXBUS_SPI_CONFIG));

    return 0;
}

/**
 * @brief  FLEXBUS SPI mode irq handler helper.
 * @param  flexbus: FLEXBUS SPI handler.
 * @return HAL_Status
 */
HAL_Status HAL_FLEXBUS_SPI_IrqHandler(struct HAL_FLEXBUS_SPI_HANDLE *flexbus)
{
    HAL_Status ret = HAL_OK;
    uint32_t isr;

    HAL_ASSERT(flexbus != NULL);

    isr = READ_REG(flexbus->fb->pReg->ISR);
    HAL_DBG("isr=%lx\n", isr);
    WRITE_REG(flexbus->fb->pReg->ICR, isr);
    if (isr & FLEXBUS_SPI_ERR_ISR) {
        if (isr & FLEXBUS_ISR_DMA_TIMEOUT_IS_MASK) {
            HAL_DBG_ERR("dma timeout!\n");
            WRITE_REG(flexbus->fb->pReg->ICR, FLEXBUS_ISR_DMA_TIMEOUT_IS_MASK);
        }
        if (isr & FLEXBUS_ISR_DMA_ERR_IS_MASK) {
            HAL_DBG_ERR("dma err!\n");
            WRITE_REG(flexbus->fb->pReg->ICR, FLEXBUS_ISR_DMA_ERR_IS_MASK);
        }
        if (isr & FLEXBUS_ISR_TX_UDF_IS_MASK) {
            HAL_DBG_ERR("tx underflow!\n");
            WRITE_REG(flexbus->fb->pReg->ICR, FLEXBUS_ISR_TX_UDF_IS_MASK);
        }
        if (isr & FLEXBUS_ISR_TX_OVF_IS_MASK) {
            HAL_DBG_ERR("tx overflow!\n");
            WRITE_REG(flexbus->fb->pReg->ICR, FLEXBUS_ISR_TX_OVF_IS_MASK);
        }

        ret = HAL_ERROR;
    }

    return ret;
}
/** @} */

/** @} */

/** @} */

#endif  /* HAL_FLEXBUS_MODULE_ENABLED && HAL_FLEXBUS_SPI_MODULE_ENABLED */
