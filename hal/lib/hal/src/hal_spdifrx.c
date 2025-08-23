/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_SPDIFRX_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup SPDIFRX
 *  @{
 */

/** @defgroup SPDIFRX_How_To_Use How To Use
 *  @{

 The SPDIFRX driver can be used as follows:

 @} */

/** @defgroup SPDIFRX_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
#define SPDIFRX_DMA_BURST_SIZE (8) /* size * width: 8*4 = 32 bytes */

/* CFGR */
#define SPDIFRX_CFGR_DAT_JOIN_EN   HAL_BIT(SPDIFRX_CFGR_DAT_SHIFT)
#define SPDIFRX_CFGR_DAT_JOIN_DIS  0
#define SPDIFRX_CFGR_TWAD_IEC958   HAL_BIT(SPDIFRX_CFGR_TWAD_SHIFT)
#define SPDIFRX_CFGR_TWAD_DATAONLY 0
#define SPDIFRX_CFGR_EN            HAL_BIT(SPDIFRX_CFGR_EN_SHIFT)
#define SPDIFRX_CFGR_DIS           0

/* CLR */
#define SPDIFRX_CLR_RXSC HAL_BIT(SPDIFRX_CLR_RXSC_SHIFT)

/* CDR */
#define SPDIFRX_CDR_CS_IDLE        (0 << SPDIFRX_CDR_CS_SHIFT)
#define SPDIFRX_CDR_CS_DETECT      (1 << SPDIFRX_CDR_CS_SHIFT)
#define SPDIFRX_CDR_CS_MEASUREMENT (2 << SPDIFRX_CDR_CS_SHIFT)
#define SPDIFRX_CDR_CS_LOCKED      (3 << SPDIFRX_CDR_CS_SHIFT)
#define SPDIFRX_CDR_AVGSEL_AVG     HAL_BIT(SPDIFRX_CDR_AVGSEL_SHIFT)
#define SPDIFRX_CDR_AVGSEL_MIN     0
#define SPDIFRX_CDR_BYPASS_EN      HAL_BIT(SPDIFRX_CDR_BYPASS_SHIFT)
#define SPDIFRX_CDR_BYPASS_DIS     0

/* DMACR */
#define SPDIFRX_DMACR_RDE_EN  HAL_BIT(SPDIFRX_DMACR_RDE_SHIFT)
#define SPDIFRX_DMACR_RDE_DIS 0
#define SPDIFRX_DMACR_RDL(x)  (((x) - 1) << SPDIFRX_DMACR_RDL_SHIFT)

/* INTEN */
#define SPDIFRX_INTEN_TYPEIE_EN   HAL_BIT(SPDIFRX_INTEN_TYPEIE_SHIFT)
#define SPDIFRX_INTEN_TYPEIE_DIS  0
#define SPDIFRX_INTEN_VCGIE_EN    HAL_BIT(SPDIFRX_INTEN_VCGIE_SHIFT)
#define SPDIFRX_INTEN_VCGIE_DIS   0
#define SPDIFRX_INTEN_UBCIE_EN    HAL_BIT(SPDIFRX_INTEN_UBCIE_SHIFT)
#define SPDIFRX_INTEN_UBCIE_DIS   0
#define SPDIFRX_INTEN_SYNCIE_EN   HAL_BIT(SPDIFRX_INTEN_SYNCIE_SHIFT)
#define SPDIFRX_INTEN_SYNCIE_DIS  0
#define SPDIFRX_INTEN_BTEIE_EN    HAL_BIT(SPDIFRX_INTEN_BTEIE_SHIFT)
#define SPDIFRX_INTEN_BTEIE_DIS   0
#define SPDIFRX_INTEN_NSYNCIE_EN  HAL_BIT(SPDIFRX_INTEN_NSYNCIE_SHIFT)
#define SPDIFRX_INTEN_NSYNCIE_DIS 0
#define SPDIFRX_INTEN_BMDEIE_EN   HAL_BIT(SPDIFRX_INTEN_BMDEIE_SHIFT)
#define SPDIFRX_INTEN_BMDEIE_DIS  0
#define SPDIFRX_INTEN_RXOIE_EN    HAL_BIT(SPDIFRX_INTEN_RXOIE_SHIFT)
#define SPDIFRX_INTEN_RXOIE_DIS   0
#define SPDIFRX_INTEN_RXFIE_EN    HAL_BIT(SPDIFRX_INTEN_RXFIE_SHIFT)
#define SPDIFRX_INTEN_RXFIE_DIS   0
#define SPDIFRX_INTEN_NPSPIE_EN   HAL_BIT(SPDIFRX_INTEN_NPSPIE_SHIFT)
#define SPDIFRX_INTEN_NPSPIE_DIS  0
#define SPDIFRX_INTEN_NVLDIE_EN   HAL_BIT(SPDIFRX_INTEN_NVLDIE_SHIFT)
#define SPDIFRX_INTEN_NVLDIE_DIS  0
#define SPDIFRX_INTEN_CSCIE_EN    HAL_BIT(SPDIFRX_INTEN_CSCIE_SHIFT)
#define SPDIFRX_INTEN_CSCIE_DIS   0
#define SPDIFRX_INTEN_PEIE_EN     HAL_BIT(SPDIFRX_INTEN_PEIE_SHIFT)
#define SPDIFRX_INTEN_PEIE_DIS    0

/* INTSR */
#define SPDIFRX_INTSR_VCGISR_ACTIVE   HAL_BIT(SPDIFRX_INTSR_VCGISR_SHIFT)
#define SPDIFRX_INTSR_UBCISR_ACTIVE   HAL_BIT(SPDIFRX_INTSR_UBCISR_SHIFT)
#define SPDIFRX_INTSR_SYNCISR_ACTIVE  HAL_BIT(SPDIFRX_INTSR_SYNCISR_SHIFT)
#define SPDIFRX_INTSR_BTEISR_ACTIVE   HAL_BIT(SPDIFRX_INTSR_BTEISR_SHIFT)
#define SPDIFRX_INTSR_NSYNCISR_ACTIVE HAL_BIT(SPDIFRX_INTSR_NSYNCISR_SHIFT)
#define SPDIFRX_INTSR_BMDEISR_ACTIVE  HAL_BIT(SPDIFRX_INTSR_BMDEISR_SHIFT)
#define SPDIFRX_INTSR_RXOISR_ACTIVE   HAL_BIT(SPDIFRX_INTSR_RXOISR_SHIFT)
#define SPDIFRX_INTSR_RXFISR_ACTIVE   HAL_BIT(SPDIFRX_INTSR_RXFISR_SHIFT)
#define SPDIFRX_INTSR_NPSPISR_ACTIVE  HAL_BIT(SPDIFRX_INTSR_NPSPISR_SHIFT)
#define SPDIFRX_INTSR_NVLDISR_ACTIVE  HAL_BIT(SPDIFRX_INTSR_NVLDISR_SHIFT)
#define SPDIFRX_INTSR_CSCISR_ACTIVE   HAL_BIT(SPDIFRX_INTSR_CSCISR_SHIFT)
#define SPDIFRX_INTSR_PEISR_ACTIVE    HAL_BIT(SPDIFRX_INTSR_PEISR_SHIFT)

/* INTCLR */
#define SPDIFRX_INTCLR_VCGICLR   HAL_BIT(SPDIFRX_INTCLR_VCGICLR_SHIFT)
#define SPDIFRX_INTCLR_UBCICLR   HAL_BIT(SPDIFRX_INTCLR_UBCICLR_SHIFT)
#define SPDIFRX_INTCLR_SYNCICLR  HAL_BIT(SPDIFRX_INTCLR_SYNCICLR_SHIFT)
#define SPDIFRX_INTCLR_BTECLR    HAL_BIT(SPDIFRX_INTCLR_BTEICLR_SHIFT)
#define SPDIFRX_INTCLR_NSYNCICLR HAL_BIT(SPDIFRX_INTCLR_NSYNCICLR_SHIFT)
#define SPDIFRX_INTCLR_BMDEICLR  HAL_BIT(SPDIFRX_INTCLR_BMDEICLR_SHIFT)
#define SPDIFRX_INTCLR_RXOICLR   HAL_BIT(SPDIFRX_INTCLR_RXOICLR_SHIFT)
#define SPDIFRX_INTCLR_RXFICLR   HAL_BIT(SPDIFRX_INTCLR_RXFICLR_SHIFT)
#define SPDIFRX_INTCLR_NPSPICLR  HAL_BIT(SPDIFRX_INTCLR_NPSPICLR_SHIFT)
#define SPDIFRX_INTCLR_NVLDICLR  HAL_BIT(SPDIFRX_INTCLR_NVLDICLR_SHIFT)
#define SPDIFRX_INTCLR_CSCICLR   HAL_BIT(SPDIFRX_INTCLR_CSCICLR_SHIFT)
#define SPDIFRX_INTCLR_PEICLR    HAL_BIT(SPDIFRX_INTCLR_PEICLR_SHIFT)

/* STATUS */
#define SPDIFRX_STATUS_AUDIO_TYPE_V(v)       (((v) & SPDIFRX_STATUS_AUDIO_TYPE_MASK) >> SPDIFRX_STATUS_AUDIO_TYPE_SHIFT)
#define SPDIFRX_STATUS_AUDIO_TYPE_COMPRESSED HAL_BIT(SPDIFRX_STATUS_AUDIO_TYPE_SHIFT)
#define SPDIFRX_STATUS_AUDIO_TYPE_PCM        0

/* BURSTINFO */
#define SPDIFRX_BURSTINFO_PD_V(v)       (((v) & SPDIFRX_BURSTINFO_PD_MASK) >> SPDIFRX_BURSTINFO_PD_SHIFT)
#define SPDIFRX_BURSTINFO_PC_V(v)       (((v) & HAL_GENMASK(15, 0)) >> 0)
#define SPDIFRX_BURSTINFO_BSNUM_V(v)    (((v) & SPDIFRX_BURSTINFO_BSNUM_MASK) >> SPDIFRX_BURSTINFO_BSNUM_SHIFT)
#define SPDIFRX_BURSTINFO_DATAINFO_V(v) (((v) & SPDIFRX_BURSTINFO_DATAINFO_MASK) >> SPDIFRX_BURSTINFO_DATAINFO_SHIFT)
#define SPDIFRX_BURSTINFO_ERRFLAG       HAL_BIT(SPDIFRX_BURSTINFO_ERRFLAG_SHIFT)
#define SPDIFRX_BURSTINFO_VALID         (0 << 7)
#define SPDIFRX_BURSTINFO_DATATYPE_V(v) (((v) & SPDIFRX_BURSTINFO_DATATYPE_MASK) >> SPDIFRX_BURSTINFO_DATATYPE_SHIFT)

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup SPDIFRX_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 ...to do or delete this row

 *  @{
 */

/** @} */

/** @defgroup SPDIFRX_Exported_Functions_Group2 State and Errors Functions

 This section provides functions allowing to get the status of the module:

 *  @{
 */

/** @} */

/** @defgroup SPDIFRX_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

/** @} */

/** @defgroup SPDIFRX_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 ...to do or delete this row

 *  @{
 */

/**
 * @brief  Init spdifrx controller dev.
 * @param  spdifrx: the handle of spdifrx dev.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFRX_DevInit(struct HAL_SPDIFRX_DEV *spdifrx)
{
    HAL_ASSERT(spdifrx != NULL);

    return HAL_SPDIFRX_Init(spdifrx->pReg);
}

/**
 * @brief  DeInit spdifrx controller dev.
 * @param  spdifrx: the handle of spdifrx dev.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFRX_DevDeInit(struct HAL_SPDIFRX_DEV *spdifrx)
{
    HAL_ASSERT(spdifrx != NULL);

    return HAL_SPDIFRX_DeInit(spdifrx->pReg);
}

/** @} */

/** @defgroup SPDIFRX_Exported_Functions_Group5 Other Functions
 *  @{
 */

/** @defgroup SPDIFRX_Low_Level_Functions Low Level Functions
 *  @{
 */

/**
 * @brief  Check spdifrx cdr locked or not.
 * @param  pReg: the handle of SPDIFRX_REG.
 * @return true or false
 */
bool HAL_SPDIFRX_IsLocked(struct SPDIFRX_REG *pReg)
{
    uint32_t state;

    HAL_ASSERT(IS_SPDIFRX_INSTANCE(pReg));

    state = READ_REG(pReg->CDR) & SPDIFRX_CDR_CS_MASK;
    if (state == SPDIFRX_CDR_CS_LOCKED) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief  Set data store format.
 * @param  pReg: the handle of SPDIFRX_REG.
 * @param  fmt: eSPDIFRX_dataFmt.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFRX_SetFmt(struct SPDIFRX_REG *pReg, eSPDIFRX_dataFmt fmt)
{
    uint32_t val, msk;

    HAL_ASSERT(IS_SPDIFRX_INSTANCE(pReg));

    msk = SPDIFRX_CFGR_TWAD_MASK | SPDIFRX_CFGR_DAT_MASK;

    switch (fmt) {
    case SPDIFRX_IEC60958:
        val = SPDIFRX_CFGR_TWAD_IEC958 | SPDIFRX_CFGR_DAT_JOIN_DIS;
        break;
    case SPDIFRX_DATA24BIT:
        val = SPDIFRX_CFGR_TWAD_DATAONLY | SPDIFRX_CFGR_DAT_JOIN_DIS;
        break;
    case SPDIFRX_DATA16BIT:
        val = SPDIFRX_CFGR_TWAD_DATAONLY | SPDIFRX_CFGR_DAT_JOIN_EN;
        break;
    default:
        HAL_DBG_ERR("Invalid fmt: %u\n", fmt);

        return HAL_INVAL;
    }
    ;

    MODIFY_REG(pReg->CFGR, msk, val);

    return HAL_OK;
}

/**
 * @brief  Get the audio type of input stream.
 * @param  pReg: the handle of SPDIFRX_REG.
 * @return HAL_Status
 */
eSPDIFRX_audioType HAL_SPDIFRX_GetAudioType(struct SPDIFRX_REG *pReg)
{
    HAL_ASSERT(IS_SPDIFRX_INSTANCE(pReg));

    return SPDIFRX_STATUS_AUDIO_TYPE_V(READ_REG(pReg->STATUS));
}

/**
 * @brief  Get the burst-info(PC) which contain data type
 * @param  pReg: the handle of SPDIFRX_REG.
 * @return 16 bits val of PC
 */
uint16_t HAL_SPDIFRX_GetPC(struct SPDIFRX_REG *pReg)
{
    HAL_ASSERT(IS_SPDIFRX_INSTANCE(pReg));

    return SPDIFRX_BURSTINFO_PC_V(READ_REG(pReg->BURSTINFO));
}

/**
 * @brief  Get the length-code(PD) which indicates the number of bits or bytes
 *         according to data-type.
 * @param  pReg: the handle of SPDIFRX_REG.
 * @return 16 bits val of PD
 */
uint16_t HAL_SPDIFRX_GetPD(struct SPDIFRX_REG *pReg)
{
    HAL_ASSERT(IS_SPDIFRX_INSTANCE(pReg));

    return SPDIFRX_BURSTINFO_PD_V(READ_REG(pReg->BURSTINFO));
}

/**
 * @brief  Init spdifrx controller.
 * @param  pReg: the handle of SPDIFRX_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFRX_Init(struct SPDIFRX_REG *pReg)
{
    HAL_ASSERT(IS_SPDIFRX_INSTANCE(pReg));

    MODIFY_REG(pReg->INTEN,
               SPDIFRX_INTEN_TYPEIE_MASK | SPDIFRX_INTEN_VCGIE_MASK |
               SPDIFRX_INTEN_SYNCIE_MASK | SPDIFRX_INTEN_NSYNCIE_MASK |
               SPDIFRX_INTEN_BMDEIE_MASK | SPDIFRX_INTEN_RXOIE_MASK |
               SPDIFRX_INTEN_CSCIE_MASK | SPDIFRX_INTEN_PEIE_MASK,
               SPDIFRX_INTEN_TYPEIE_EN | SPDIFRX_INTEN_VCGIE_EN |
               SPDIFRX_INTEN_SYNCIE_EN | SPDIFRX_INTEN_NSYNCIE_EN |
               SPDIFRX_INTEN_BMDEIE_EN | SPDIFRX_INTEN_RXOIE_EN |
               SPDIFRX_INTEN_CSCIE_EN | SPDIFRX_INTEN_PEIE_EN);

    MODIFY_REG(pReg->CDR,
               SPDIFRX_CDR_AVGSEL_MASK | SPDIFRX_CDR_BYPASS_MASK,
               SPDIFRX_CDR_AVGSEL_MIN | SPDIFRX_CDR_BYPASS_DIS);

    MODIFY_REG(pReg->DMACR, SPDIFRX_DMACR_RDL_MASK, SPDIFRX_DMACR_RDL(SPDIFRX_DMA_BURST_SIZE));

    HAL_SPDIFRX_SetFmt(pReg, SPDIFRX_IEC60958);

    return HAL_OK;
}

/**
 * @brief  DeInit spdifrx controller.
 * @param  pReg: the handle of SPDIFRX_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFRX_DeInit(struct SPDIFRX_REG *pReg)
{
    HAL_ASSERT(IS_SPDIFRX_INSTANCE(pReg));

    MODIFY_REG(pReg->INTEN, 0xffffffff, 0x0);

    return HAL_OK;
}

/**
 * @brief  Enable spdifrx.
 * @param  pReg: the handle of SPDIFRX_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFRX_Enable(struct SPDIFRX_REG *pReg)
{
    HAL_ASSERT(IS_SPDIFRX_INSTANCE(pReg));

    if (!HAL_SPDIFRX_IsLocked(pReg)) {
        HAL_DBG_ERR("%s: CDR has not been locked\n", __func__);

        return HAL_ERROR;
    }

    MODIFY_REG(pReg->DMACR, SPDIFRX_DMACR_RDE_MASK, SPDIFRX_DMACR_RDE_EN);
    MODIFY_REG(pReg->CFGR, SPDIFRX_CFGR_EN_MASK, SPDIFRX_CFGR_EN);

    return HAL_OK;
}

/**
 * @brief  Disable spdifrx rx.
 * @param  pReg: the handle of SPDIFRX_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFRX_Disable(struct SPDIFRX_REG *pReg)
{
    HAL_ASSERT(IS_SPDIFRX_INSTANCE(pReg));

    MODIFY_REG(pReg->DMACR, SPDIFRX_DMACR_RDE_MASK, SPDIFRX_DMACR_RDE_DIS);
    MODIFY_REG(pReg->CFGR, SPDIFRX_CFGR_EN_MASK, SPDIFRX_CFGR_DIS);

    WRITE_REG(pReg->CLR, SPDIFRX_CLR_RXSC);

    return HAL_OK;
}

/**
 * @brief  spdifrx IrqHandler
 * @param  pReg: the handle of SPDIFRX_REG.
 * @return raw irq status
 */
uint32_t HAL_SPDIFRX_IrqHandler(struct SPDIFRX_REG *pReg)
{
    uint32_t irqStatus;

    HAL_ASSERT(IS_SPDIFRX_INSTANCE(pReg));

    irqStatus = READ_REG(pReg->INTSR);
    WRITE_REG(pReg->INTCLR, irqStatus);

    return irqStatus;
}

/** @} */

/** @defgroup SAI_Dev_Level_Functions Dev Level Functions
 *  @{
 */

/**
 * @brief  Enable spdifrx.
 * @param  spdifrx: the handle of HAL_SPDIFRX_DEV.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFRX_DevEnable(struct HAL_SPDIFRX_DEV *spdifrx)
{
#if defined(HAL_CRU_MODULE_ENABLED)
    HAL_ASSERT(spdifrx != NULL);

    if (!HAL_SPDIFRX_IsLocked(spdifrx->pReg)) {
        HAL_DBG_ERR("%s: Pre CDR has not been locked\n", __func__);

        return HAL_ERROR;
    }

    HAL_CRU_ClkResetAssert(spdifrx->rst);
    HAL_DelayUs(10);
    HAL_CRU_ClkResetDeassert(spdifrx->rst);
    HAL_DelayUs(2000);
#endif

    return HAL_SPDIFRX_Enable(spdifrx->pReg);
}

/**
 * @brief  Disable spdifrx.
 * @param  spdifrx: the handle of HAL_SPDIFRX_DEV.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFRX_DevDisable(struct HAL_SPDIFRX_DEV *spdifrx)
{
    HAL_ASSERT(spdifrx != NULL);

    return HAL_SPDIFRX_Disable(spdifrx->pReg);
}

/** @} */

/** @} */

/** @} */

/** @} */

#endif /* HAL_SPDIFRX_MODULE_ENABLED */
