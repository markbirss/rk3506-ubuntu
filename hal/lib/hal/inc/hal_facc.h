/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#if defined(HAL_FACC_FIR_MODULE_ENABLED) || defined(HAL_FACC_IIR_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FACC
 *  @{
 */

#ifndef _HAL_FACC_H_
#define _HAL_FACC_H_

#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup FACC_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/**
 * enum eFACC_mode - facc work mode.
 */
typedef enum {
    FACC_MODE_LECACY = 0,
    FACC_MODE_ACM
} eFACC_mode;

/**
 * enum eFACC_irqType - facc irq type.
 */
typedef enum {
    FACC_IRQ_NONE        = 0,
    FACC_IRQ_CHAN_DONE   = 1 << 1,
    FACC_IRQ_ALL_DONE    = 1 << 2,
    FACC_IRQ_WDT_TIMEOUT = 1 << 3,
    FACC_IRQ_MAC_ERR     = 1 << 4,
    FACC_IRQ_AXI_ERR     = 1 << 5
} eFACC_irqType;

/***************************** Structure Definition **************************/

/**
 * struct HAL_FACC_CONFIG - facc config struct.
 */
struct HAL_FACC_CONFIG {
    eFACC_mode mode;
    uint32_t wdtCnt;
    uint32_t enMacErr;
};

/**
 * struct HAL_FACC_DEV - facc device struct.
 */
struct HAL_FACC_DEV {
    void *pReg;               /**< facc register base */
    IRQn_Type irqNum;         /**< irq number */
    struct HAL_FACC_OPS *ops; /**< facc ops */
    eFACC_irqType irqType;    /**< facc irq type */
    void *priv;               /**< facc dev privData */
};

/**
 * struct HAL_FACC_OPS - facc ops struct.
 */
struct HAL_FACC_OPS {
    HAL_Status (*start)(struct HAL_FACC_DEV *dev, uint32_t chanptr);
    HAL_Status (*stop)(struct HAL_FACC_DEV *dev);
    HAL_Status (*reset)(struct HAL_FACC_DEV *dev);
    HAL_Status (*pause)(struct HAL_FACC_DEV *dev);
    HAL_Status (*release)(struct HAL_FACC_DEV *dev);
    HAL_Status (*irq_handler)(struct HAL_FACC_DEV *dev);
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup FACC_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_FACC_Init(void);
HAL_Status HAL_FACC_DeInit(void);
HAL_Status HAL_FACC_Reset(void);
HAL_Status HAL_FACC_FIR_Config(struct HAL_FACC_DEV *fir, struct HAL_FACC_CONFIG *config);
HAL_Status HAL_FACC_IIR_Config(struct HAL_FACC_DEV *iir, struct HAL_FACC_CONFIG *config);

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_FACC_FIR_MODULE_ENABLED || HAL_FACC_IIR_MODULE_ENABLED */
