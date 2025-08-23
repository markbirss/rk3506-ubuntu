/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if (defined(RKMCU_RK2118) || defined(SOC_RK3506)) && defined(HAL_TRNG_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup TRNG
 *  @{
 */

/** @defgroup TRNG_How_To_Use How To Use
 *  @{

 The TRNG driver can be used as follows:

 - HAL_TRNG_Init(): init trng module
 - HAL_TRNG_Get(): get rng data
 @} */

/** @defgroup TRNG_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define RK_TRNG_DATA_SIZE (HAL_ARRAY_SIZE(((struct TRNG_REG *)0)->DRNG_DATA))

#define TRNG_WRITE_MASK_ALL (0xffffu)

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/
/**
 * @brief  get TRNG data once
 * @param  dev: the handle of trng.
 * @param  data: trng buffer.
 * @param  len: trng buffer length.
 * @return HAL_Status
 */
static HAL_Status HAL_TRNG_GetOnce(const struct HAL_TRNG_DEV *dev, uint8_t *data, uint32_t len)
{
    uint32_t i, ctrl = 0;
    uint32_t buf[RK_TRNG_DATA_SIZE];
    HAL_Status ret = HAL_OK;
    struct TRNG_REG *pReg;

    HAL_ASSERT(dev);
    HAL_ASSERT(data);

    pReg = dev->pReg;

    if (len > RK_TRNG_DATA_SIZE) {
        return HAL_ERROR;
    }

    memset(buf, 0, sizeof(buf));

    ctrl = TRNG_CTRL_SW_DRNG_REQ_MASK;

    WRITE_REG_MASK_WE(pReg->CTRL, TRNG_WRITE_MASK_ALL, ctrl);

    while ((READ_REG(pReg->CTRL) & TRNG_CTRL_SW_DRNG_REQ_MASK)) {
        ;
    }

    while (!(READ_REG(pReg->STATE) & TRNG_STATE_SW_DRNG_ACK_MASK)) {
        ;
    }

    WRITE_REG(pReg->STATE, TRNG_STATE_SW_DRNG_ACK_MASK);

    for (i = 0; i < HAL_ARRAY_SIZE(pReg->DRNG_DATA); i++) {
        buf[i] = READ_REG(pReg->DRNG_DATA[i]);
    }

    memcpy(data, buf, len);

    /* close TRNG */
    WRITE_REG_MASK_WE(pReg->CTRL, TRNG_WRITE_MASK_ALL, 0);

    return ret;
}

/** @} */
/********************* Public Function Definition ***************************/

/** @defgroup TRNG_Exported_Functions_Group1 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 *  @{
 */

/**
 * @brief  Init trng and deassert reset.
 * @param  dev: the handle of trng.
 * @return HAL_Status
 */
HAL_Status HAL_TRNG_Init(const struct HAL_TRNG_DEV *dev)
{
    uint32_t value = 0;
    struct TRNG_REG *pReg;

    HAL_ASSERT(dev);

    pReg = dev->pReg;

    WRITE_REG_MASK_WE(pReg->CTRL, TRNG_WRITE_MASK_ALL, 0);

    /* clear interrupt status register */
    value = READ_REG(pReg->STATE);

    WRITE_REG(pReg->STATE, value);

    return HAL_OK;
}

/**
 * @brief  DeInit trng.
 * @param  dev: the handle of trng.
 * @return HAL_Status
 */
HAL_Status HAL_TRNG_DeInit(const struct HAL_TRNG_DEV *dev)
{
    HAL_ASSERT(dev);

    return HAL_OK;
}

/**
 * @brief  get TRNG data
 * @param  dev: the handle of trng.
 * @param  data: trng buffer.
 * @param  len: trng buffer length.
 * @return HAL_Status
 */
HAL_Status HAL_TRNG_Get(const struct HAL_TRNG_DEV *dev, uint8_t *data, uint32_t len)
{
    uint32_t read_len = 0;
    HAL_Status ret = HAL_OK;

    HAL_ASSERT(dev);
    HAL_ASSERT(data);

    while (len) {
        read_len = len > RK_TRNG_DATA_SIZE ? RK_TRNG_DATA_SIZE : len;
        ret = HAL_TRNG_GetOnce(dev, data, read_len);
        if (ret != HAL_OK) {
            return ret;
        }

        data += read_len;
        len -= read_len;
    }

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* (MCU_RK2118 || SOC_RK3506) && HAL_TRNG_MODULE_ENABLED */
