/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_FACC_FIR_MODULE_ENABLED) || defined(HAL_FACC_IIR_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FACC
 *  @{
 */

/** @defgroup FACC_How_To_Use How To Use
 *  @{

 The FACC driver can be used as follows:
 - Initialize the FACC module by calling HAL_FACC_Init().
 - De-initialize the FACC module by calling HAL_FACC_DeInit().
 - Reset the FACC module by calling HAL_FACC_Reset().
 - Configure the FIR_ACC device by calling HAL_FACC_FIR_Config().
 - Configure the IIR_ACC device by calling HAL_FACC_IIR_Config().

 @} */

/** @defgroup FACC_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/* FIR_ACC FGCTL */
#define FIR_ACC_FGCTL_BURST_LEN     (0x7 << FIR_ACC_FGCTL_BLEN_SHIFT)
#define FIR_ACC_FGCTL_OUTSTD_RD_LEN (0x7 << FIR_ACC_FGCTL_OUTSTD_RD_SHIFT)
#define FIR_ACC_FGCTL_OUTSTD_WD_LEN (0x7 << FIR_ACC_FGCTL_OUTSTD_WR_SHIFT)

/* IIR_ACC FGCTL */
#define IIR_ACC_FGCTL_BURST_LEN     (0x7 << IIR_ACC_FGCTL_BLEN_SHIFT)
#define IIR_ACC_FGCTL_OUTSTD_RD_LEN (0x7 << IIR_ACC_FGCTL_OUTSTD_RD_SHIFT)
#define IIR_ACC_FGCTL_OUTSTD_WD_LEN (0x7 << IIR_ACC_FGCTL_OUTSTD_SHIFT)

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

#ifdef HAL_FACC_FIR_MODULE_ENABLED
/**
 * @brief  Start the FIR_ACC channel.
 * @param  fir: Pointer to a FIR_ACC_DEV structure that represents the device.
 * @param  chanptr: The address of the channel data.
 * @return HAL_Status
 */
static HAL_Status HAL_FIR_ACC_Start(struct HAL_FACC_DEV *fir, uint32_t chanptr)
{
    struct FIR_ACC_REG *pReg = (struct FIR_ACC_REG *)fir->pReg;

    HAL_ASSERT(chanptr);
    WRITE_REG(pReg->CHNPTR, chanptr);
    WRITE_REG_MASK_WE(pReg->RUNCTL, FIR_ACC_RUNCTL_EN_MASK, FIR_ACC_RUNCTL_EN_MASK);

    return HAL_OK;
}

/**
 * @brief  Stop the FIR_ACC channel.
 * @param  fir: Pointer to a FIR_ACC_DEV structure that represents the device.
 * @return HAL_Status
 */
static HAL_Status HAL_FIR_ACC_Stop(struct HAL_FACC_DEV *fir)
{
    struct FIR_ACC_REG *pReg = (struct FIR_ACC_REG *)fir->pReg;

    WRITE_REG_MASK_WE(pReg->RUNCTL, FIR_ACC_RUNCTL_EN_MASK, 0);
    WRITE_REG(pReg->CHNPTR, 0);

    return HAL_OK;
}

/**
 * @brief  Reset the FIR_ACC device.
 * @param  fir: Pointer to a FIR_ACC_DEV structure that represents the device.
 * @return HAL_Status
 */
static HAL_Status HAL_FIR_ACC_Reset(struct HAL_FACC_DEV *fir)
{
    struct FIR_ACC_REG *pReg = (struct FIR_ACC_REG *)fir->pReg;
    uint32_t rstCnt = 20, tmp;
    uint32_t rstMask = FIR_ACC_GSTAT_TRRSTDONE_MASK |
                       FIR_ACC_GSTAT_DRRSTDONE_MASK |
                       FIR_ACC_GSTAT_LRRSTDONE_MASK |
                       FIR_ACC_GSTAT_PRRSTDONE_MASK |
                       FIR_ACC_GSTAT_WRSTDONE_MASK;

    WRITE_REG_MASK_WE(pReg->RUNCTL, FIR_ACC_RUNCTL_SWRST_MASK, 0);
    do {
        rstCnt--;
        tmp = READ_REG(pReg->GSTAT);
        if ((tmp & rstMask) == rstMask) {
            break;
        }
        HAL_DelayMs(1);
    } while (rstCnt);
    WRITE_REG_MASK_WE(pReg->RUNCTL, FIR_ACC_RUNCTL_SWRST_MASK, FIR_ACC_RUNCTL_SWRST_MASK);

    if (rstCnt == 0) {
        HAL_DBG("FirAcc Reset timeout\n");
    }

    return HAL_OK;
}

/**
 * @brief  Pause the FIR_ACC channel.
 * @param  fir: Pointer to a FIR_ACC_DEV structure that represents the device.
 * @return HAL_Status
 */
static HAL_Status HAL_FIR_ACC_Pause(struct HAL_FACC_DEV *fir)
{
    struct FIR_ACC_REG *pReg = (struct FIR_ACC_REG *)fir->pReg;

    WRITE_REG_MASK_WE(pReg->RUNCTL, FIR_ACC_RUNCTL_HALT_MASK, FIR_ACC_RUNCTL_HALT_MASK);

    return HAL_OK;
}

/**
 * @brief  Release the FIR_ACC channel.
 * @param  fir: Pointer to a FIR_ACC_DEV structure that represents the device.
 * @return HAL_Status
 */
static HAL_Status HAL_FIR_ACC_Release(struct HAL_FACC_DEV *fir)
{
    struct FIR_ACC_REG *pReg = (struct FIR_ACC_REG *)fir->pReg;

    WRITE_REG_MASK_WE(pReg->RUNCTL, FIR_ACC_RUNCTL_HALT_MASK, 0);

    return HAL_OK;
}

/**
 * @brief  Interrupt handler for the FIR_ACC device.
 * @param  fir: Pointer to a FIR_ACC_DEV structure that represents the device.
 * @return HAL_Status
 */
static HAL_Status HAL_FIR_ACC_IrqHandler(struct HAL_FACC_DEV *fir)
{
    struct FIR_ACC_REG *pReg = (struct FIR_ACC_REG *)fir->pReg;
    uint32_t gstat, macStat, errMask;

    fir->irqType = FACC_IRQ_NONE;
    gstat = READ_REG(pReg->GSTAT);

    if (gstat & FIR_ACC_GSTAT_WDTTO_MASK) {
        // clear wdt interrupt
        WRITE_REG_MASK_WE(pReg->RUNCTL, FIR_ACC_RUNCTL_CLRWDT_MASK, FIR_ACC_RUNCTL_CLRWDT_MASK);
        fir->irqType |= FACC_IRQ_WDT_TIMEOUT;
    }

    if (gstat & FIR_ACC_GSTAT_WRITEDONE_MASK) {
        fir->irqType |= FACC_IRQ_CHAN_DONE;
    }

    if (gstat & FIR_ACC_GSTAT_ALLCHDONE_MASK) {
        fir->irqType |= FACC_IRQ_ALL_DONE;
    }

    if (gstat & FIR_ACC_GSTAT_MACERR_MASK) {
        errMask = FIR_ACC_MACSTAT_ARZ_MASK |
                  FIR_ACC_MACSTAT_ARI_MASK |
                  FIR_ACC_MACSTAT_AYNAN_MASK |
                  FIR_ACC_MACSTAT_AYINF_MASK |
                  FIR_ACC_MACSTAT_MINTOV_SHIFT |
                  FIR_ACC_MACSTAT_MRI_MASK |
                  FIR_ACC_MACSTAT_MINV_MASK |
                  FIR_ACC_MACSTAT_MYINF_MASK;
        macStat = READ_REG(pReg->MACSTAT);
        if ((macStat & errMask) != 0) {
            fir->irqType |= FACC_IRQ_MAC_ERR;
            HAL_DBG_WRN("Fir Acc Mac Error: 0x%" PRIx32 "\n", macStat);
        }
    }

    if (gstat & FIR_ACC_GSTAT_AXIERR_MASK) {
        HAL_DBG_ERR("Fir Acc Axi Error, gstat: 0x%" PRIx32 "\n", gstat);
        fir->irqType |= FACC_IRQ_AXI_ERR;
    }

    // clear interrupt
    WRITE_REG_MASK_WE(pReg->RUNCTL, FIR_ACC_RUNCTL_CLRIRQ_MASK, FIR_ACC_RUNCTL_CLRIRQ_MASK);

    return HAL_OK;
}

static const struct HAL_FACC_OPS firOps = {
    .start = HAL_FIR_ACC_Start,
    .stop = HAL_FIR_ACC_Stop,
    .reset = HAL_FIR_ACC_Reset,
    .pause = HAL_FIR_ACC_Pause,
    .release = HAL_FIR_ACC_Release,
    .irq_handler = HAL_FIR_ACC_IrqHandler,
};
#endif

#ifdef HAL_FACC_IIR_MODULE_ENABLED
/**
 *
 *
 * @brief  Start the IIR_ACC channel.
 * @param  iir: Pointer to a IIR_ACC_DEV structure that represents the device.
 * @param  chanptr: The address of the channel data.
 * @return HAL_Status
 */
static HAL_Status HAL_IIR_ACC_Start(struct HAL_FACC_DEV *iir, uint32_t chanptr)
{
    struct IIR_ACC_REG *pReg = (struct IIR_ACC_REG *)iir->pReg;

    HAL_ASSERT(chanptr);
    WRITE_REG(pReg->CHNPTR, chanptr);
    WRITE_REG_MASK_WE(pReg->RUNCTL, IIR_ACC_RUNCTL_EN_MASK, IIR_ACC_RUNCTL_EN_MASK);

    return HAL_OK;
}

/**
 * @brief  Stop the IIR_ACC channel.
 * @param  iir: Pointer to a IIR_ACC_DEV structure that represents the device.
 * @return HAL_Status
 */
static HAL_Status HAL_IIR_ACC_Stop(struct HAL_FACC_DEV *iir)
{
    struct IIR_ACC_REG *pReg = (struct IIR_ACC_REG *)iir->pReg;

    WRITE_REG_MASK_WE(pReg->RUNCTL, IIR_ACC_RUNCTL_EN_MASK, 0);
    WRITE_REG(pReg->CHNPTR, 0);

    return HAL_OK;
}

/**
 * @brief  Reset the IIR_ACC device.
 * @param  iir: Pointer to a IIR_ACC_DEV structure that represents the device.
 * @return HAL_Status
 */
static HAL_Status HAL_IIR_ACC_Reset(struct HAL_FACC_DEV *iir)
{
    struct IIR_ACC_REG *pReg = (struct IIR_ACC_REG *)iir->pReg;
    uint32_t rstCnt = 20;
    uint32_t rstMask = IIR_ACC_GSTAT_PRRSTDONE_MASK |
                       IIR_ACC_GSTAT_DRRSTDONE_MASK |
                       IIR_ACC_GSTAT_TRRSTDONE_MASK |
                       IIR_ACC_GSTAT_KWRSTDONE_MASK |
                       IIR_ACC_GSTAT_DWRSTDONE_MASK;

    WRITE_REG_MASK_WE(pReg->RUNCTL, IIR_ACC_RUNCTL_SWRST_MASK, 0);
    do {
        rstCnt--;
        if ((READ_REG(pReg->GSTAT) & rstMask) == rstMask) {
            break;
        }
        HAL_DelayMs(1);
    } while (rstCnt);
    WRITE_REG_MASK_WE(pReg->RUNCTL, IIR_ACC_RUNCTL_SWRST_MASK, IIR_ACC_RUNCTL_SWRST_MASK);

    if (rstCnt == 0) {
        HAL_DBG("FirAcc Reset timeout\n");
    }

    return HAL_OK;
}

/**
 * @brief  Pause the IIR_ACC channel.
 * @param  iir: Pointer to a IIR_ACC_DEV structure that represents the device.
 * @return HAL_Status
 */
static HAL_Status HAL_IIR_ACC_Pause(struct HAL_FACC_DEV *iir)
{
    struct IIR_ACC_REG *pReg = (struct IIR_ACC_REG *)iir->pReg;

    WRITE_REG_MASK_WE(pReg->RUNCTL, IIR_ACC_RUNCTL_HALT_MASK, IIR_ACC_RUNCTL_HALT_MASK);

    return HAL_OK;
}

/**
 * @brief  Release the IIR_ACC channel.
 * @param  iir: Pointer to a IIR_ACC_DEV structure that represents the device.
 * @return HAL_Status
 */
static HAL_Status HAL_IIR_ACC_Release(struct HAL_FACC_DEV *iir)
{
    struct IIR_ACC_REG *pReg = (struct IIR_ACC_REG *)iir->pReg;

    WRITE_REG_MASK_WE(pReg->RUNCTL, IIR_ACC_RUNCTL_HALT_MASK, 0);

    return HAL_OK;
}

/**
 * @brief  IrqHandler for IIR_ACC channel.
 * @param  iir: Pointer to a IIR_ACC_DEV structure that represents the device.
 * @return HAL_Status
 */
static HAL_Status HAL_IIR_ACC_IrqHandler(struct HAL_FACC_DEV *iir)
{
    struct IIR_ACC_REG *pReg = (struct IIR_ACC_REG *)iir->pReg;
    uint32_t gstat, macStat, errMask;

    gstat = READ_REG(pReg->GSTAT);
    iir->irqType = FACC_IRQ_NONE;
    // clear wdt interrupt
    if (gstat & IIR_ACC_GSTAT_WDTTO_MASK) {
        WRITE_REG_MASK_WE(pReg->RUNCTL, IIR_ACC_RUNCTL_CLRWDT_MASK, IIR_ACC_RUNCTL_CLRWDT_MASK);
        iir->irqType |= FACC_IRQ_WDT_TIMEOUT;
    }

    if (gstat & IIR_ACC_GSTAT_SVDKDONE_MASK) {
        iir->irqType |= FACC_IRQ_CHAN_DONE;
    }

    if (gstat & IIR_ACC_GSTAT_DKALLDONE_MASK) {
        iir->irqType |= FACC_IRQ_ALL_DONE;
    }

    if (gstat & IIR_ACC_GSTAT_MACERR_MASK) {
        errMask = IIR_ACC_MACSTAT_INF_MASK |
                  IIR_ACC_MACSTAT_INV_MASK |
                  IIR_ACC_MACSTAT_HUGE_MASK;
        macStat = READ_REG(pReg->MACSTAT);
        if ((macStat & errMask) != 0) {
            iir->irqType |= FACC_IRQ_MAC_ERR;
            HAL_DBG_WRN("Iir Acc Mac Error: 0x%" PRIx32 "\n", macStat);
        }
    }

    if (gstat & IIR_ACC_GSTAT_AXIERR_MASK) {
        HAL_DBG_ERR("Iir Acc Axi Error, gstat: 0x%" PRIx32 "\n", gstat);
        iir->irqType |= FACC_IRQ_AXI_ERR;
    }

    // clear interrupt
    WRITE_REG_MASK_WE(pReg->RUNCTL, IIR_ACC_RUNCTL_CLRIRQ_MASK, IIR_ACC_RUNCTL_CLRIRQ_MASK);

    return HAL_OK;
}

static const struct HAL_FACC_OPS iirOps = {
    .start = HAL_IIR_ACC_Start,
    .stop = HAL_IIR_ACC_Stop,
    .reset = HAL_IIR_ACC_Reset,
    .pause = HAL_IIR_ACC_Pause,
    .release = HAL_IIR_ACC_Release,
    .irq_handler = HAL_IIR_ACC_IrqHandler,
};

#endif

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup ASRC_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 ...to do or delete this row

 *  @{
 */

#ifdef HAL_FACC_FIR_MODULE_ENABLED
/**
 * @brief  Initialize the FIR_ACC device.
 * @param  fir: Pointer to a FIR_ACC_DEV structure that represents the device.
 * @param  config: The base of the configuration.
 * @return HAL_Status
 */
HAL_Status HAL_FACC_FIR_Config(struct HAL_FACC_DEV *fir, struct HAL_FACC_CONFIG *config)
{
    uint32_t irq = 0;
    struct FIR_ACC_REG *pReg = FIR_ACC;

    fir->ops = (struct HAL_FACC_OPS *)&firOps;
    fir->irqType = FACC_IRQ_NONE;

    HAL_FIR_ACC_Reset(fir);
    HAL_FIR_ACC_Stop(fir);

    MODIFY_REG(pReg->FGCTL, FIR_ACC_FGCTL_ACM_MASK, config->mode << FIR_ACC_FGCTL_ACM_SHIFT);
    MODIFY_REG(pReg->FGCTL, FIR_ACC_FGCTL_CCINTR_MASK, FIR_ACC_FGCTL_CCINTR_MASK);
    MODIFY_REG(pReg->FGCTL, FIR_ACC_FGCTL_OUTSTD_WR_MASK, FIR_ACC_FGCTL_OUTSTD_WD_LEN);
    MODIFY_REG(pReg->FGCTL, FIR_ACC_FGCTL_OUTSTD_RD_MASK, FIR_ACC_FGCTL_OUTSTD_RD_LEN);
    MODIFY_REG(pReg->FGCTL, FIR_ACC_FGCTL_BLEN_MASK, FIR_ACC_FGCTL_BURST_LEN);

    if (config->wdtCnt) {
        irq |= FIR_ACC_RUNCTL_WDTEN_MASK;
        WRITE_REG(pReg->WDTCNT, config->wdtCnt);
    }

    if (config->enMacErr) {
        irq |= FIR_ACC_RUNCTL_MACIRQEN_MASK;
    }

    irq |= FIR_ACC_RUNCTL_IRQEN_MASK | FIR_ACC_RUNCTL_DONEIRQEN_MASK | FIR_ACC_RUNCTL_AXIERRORIRQEN_MASK;
    WRITE_REG_MASK_WE(pReg->RUNCTL, irq, irq);

    return HAL_OK;
}
#endif /* HAL_FACC_FIR_MODULE_ENABLED */

#ifdef HAL_FACC_IIR_MODULE_ENABLED
/**
 * @brief  Initialize the IIR_ACC device.
 * @param  iir: Pointer to a IIR_ACC_DEV structure that represents the device.
 * @param  config: The base of the configuration.
 * @return HAL_Status
 */
HAL_Status HAL_FACC_IIR_Config(struct HAL_FACC_DEV *iir, struct HAL_FACC_CONFIG *config)
{
    uint32_t irq = 0;
    struct IIR_ACC_REG *pReg = (struct IIR_ACC_REG *)iir->pReg;

    iir->ops = (struct HAL_FACC_OPS *)&iirOps;
    iir->irqType = FACC_IRQ_NONE;

    HAL_IIR_ACC_Reset(iir);
    HAL_IIR_ACC_Stop(iir);

    MODIFY_REG(pReg->FGCTL, IIR_ACC_FGCTL_ACM_MASK, config->mode << IIR_ACC_FGCTL_ACM_SHIFT);
    MODIFY_REG(pReg->FGCTL, IIR_ACC_FGCTL_CCINTR_MASK, IIR_ACC_FGCTL_CCINTR_MASK);
    MODIFY_REG(pReg->FGCTL, IIR_ACC_FGCTL_OUTSTD_MASK, IIR_ACC_FGCTL_OUTSTD_WD_LEN);
    MODIFY_REG(pReg->FGCTL, IIR_ACC_FGCTL_OUTSTD_RD_MASK, IIR_ACC_FGCTL_OUTSTD_RD_LEN);
    MODIFY_REG(pReg->FGCTL, IIR_ACC_FGCTL_BLEN_MASK, IIR_ACC_FGCTL_BURST_LEN);

    if (config->wdtCnt) {
        irq |= IIR_ACC_RUNCTL_WDTEN_MASK;
        WRITE_REG(pReg->WDTCNT, config->wdtCnt);
    }

    if (config->enMacErr) {
        irq |= IIR_ACC_RUNCTL_MACIRQEN_MASK;
    }
    irq |= IIR_ACC_RUNCTL_IRQEN_MASK | IIR_ACC_RUNCTL_DKDONEIRQEN_MASK | IIR_ACC_RUNCTL_AXIERRORIRQEN_MASK;
    WRITE_REG_MASK_WE(pReg->RUNCTL, irq, irq);

    return HAL_OK;
}
#endif /* HAL_FACC_IIR_MODULE_ENABLED */

/**
 * @brief Reset FACC module.
 * @return HAL_Status
 */
HAL_Status HAL_FACC_Reset(void)
{
    return HAL_OK;
}

/**
 * @brief Initialize FACC module.
 * @return HAL_Status
*/
HAL_Status HAL_FACC_Init(void)
{
    HAL_FACC_Reset();

    return HAL_OK;
}

/**
 * @brief De-initialize FACC module.
 * @return HAL_Status
 */
HAL_Status HAL_FACC_DeInit(void)
{
    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif  /* HAL_FACC_FIR_MODULE_ENABLED || HAL_FACC_IIR_MODULE_ENABLED */
