/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#if defined(HAL_FLEXBUS_MODULE_ENABLED) && defined(HAL_FLEXBUS_SPI_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FLEXBUS_SPI
 *  @{
 */

#ifndef __HAL_FLEXBUS_SPI_H
#define __HAL_FLEXBUS_SPI_H

#include "hal_def.h"

/** @defgroup FLEXBUS_SPI_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** MACRO Definition ******************************/
#define HAL_FLEXBUS_SPI_MAX_IOSIZE 0x4000

/***************************** Structure Definition **************************/

struct HAL_FLEXBUS_SPI_CONFIG {
    uint8_t mode; /**< spi mode bit0 cpha, bit1 cpol */
    uint8_t lsb;  /**< lsb or msb */
    uint8_t width;/**< data io lines */
    uint32_t dll; /**< delay line cells */
};

/** @brief  FLEXBUS SPI mode handler */
struct HAL_FLEXBUS_SPI_HANDLE {
    struct FLEXBUS_HANDLE *fb;          /**< FLEXBUS handler. */
    struct HAL_FLEXBUS_SPI_CONFIG cfg;  /**< FLEXBUS configuration. */
    uint32_t version;                   /**< FLEXBUS version */
    uint32_t rate;                      /**< FLEXBUS controller rate. */
    uint32_t maxSize;                   /**< FLEXBUS SPI mode max io size(software limitation). */
    bool useIrq;                        /**< FLEXBUS controller irq enable bits */
};

/** @} */

/***************************** Function Declare ******************************/
/** @defgroup FLEXBUS_SPI_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_FLEXBUS_SPI_IsXferDone(struct HAL_FLEXBUS_SPI_HANDLE *flexbus);
HAL_Status HAL_FLEXBUS_SPI_SendStart(struct HAL_FLEXBUS_SPI_HANDLE *flexbus, uint32_t addr, uint32_t size);
HAL_Status HAL_FLEXBUS_SPI_RecvStart(struct HAL_FLEXBUS_SPI_HANDLE *flexbus, uint32_t addr, uint32_t size);
HAL_Status HAL_FLEXBUS_SPI_Stop(struct HAL_FLEXBUS_SPI_HANDLE *flexbus);
HAL_Status HAL_FLEXBUS_SPI_SetCS(struct HAL_FLEXBUS_SPI_HANDLE *flexbus, bool enable);
HAL_Status HAL_FLEXBUS_SPI_Init(struct HAL_FLEXBUS_SPI_HANDLE *flexbus, const struct HAL_FLEXBUS_DEV *pFBDev, bool irq);
HAL_Status HAL_FLEXBUS_SPI_DeInit(struct HAL_FLEXBUS_SPI_HANDLE *flexbus);
HAL_Status HAL_FLEXBUS_SPI_Config(struct HAL_FLEXBUS_SPI_HANDLE *flexbus, struct HAL_FLEXBUS_SPI_CONFIG *cfg);
HAL_Status HAL_FLEXBUS_SPI_IrqHandler(struct HAL_FLEXBUS_SPI_HANDLE *flexbus);

/** @} */

#endif

/** @} */

/** @} */

#endif  /* HAL_FLEXBUS_MODULE_ENABLED && HAL_FLEXBUS_SPI_MODULE_ENABLED */
