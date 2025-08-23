/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_OTP_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup OTP
 *  @{
 */

/** @defgroup OTP_How_To_Use How To Use
 *  @{

 The OTP driver can be used as follows:

 - invoke otp functions to read data form otp.

 @} */

/** @defgroup OTP_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup OTP_Exported_Functions_Group3 IO Functions
 *  @{
 */

/**
 * @brief  read data from otp.
 * @param  reg: otp base address.
 * @param  addr: data addr in otp.
 * @param  value: output data from otp.
 * @return HAL_Status.
 */
HAL_Status HAL_OTP_Read(struct OTPC_REG *reg, uint32_t addr, uint32_t *value)
{
    HAL_Status ret = HAL_OK;
    uint32_t start, timeoutMs = 10;
    uint32_t qp;

    WRITE_REG_MASK_WE(reg->USER_CTRL, OTPC_USER_CTRL_USER_DCTRL_MASK,
                      0x1 << OTPC_USER_CTRL_USER_DCTRL_SHIFT);
    HAL_CPUDelayUs(5);
    WRITE_REG_MASK_WE(reg->USER_ADDR, OTPC_USER_ADDR_OTPC_USER_ADDR_MASK,
                      addr << OTPC_USER_ADDR_OTPC_USER_ADDR_SHIFT);
    WRITE_REG_MASK_WE(reg->USER_ENABLE, OTPC_USER_ENABLE_OTPC_USER_ENABLE_MASK,
                      0x1 << OTPC_USER_ENABLE_OTPC_USER_ENABLE_SHIFT);

    start = HAL_GetTick();
    while (!(READ_REG(reg->INT_STATUS) &
             (0x1 << OTPC_INT_STATUS_USER_DONE_INT_STATUS_SHIFT))) {
        if ((HAL_GetTick() - start) > timeoutMs) {
            HAL_DBG_ERR("timeout during read setup\n");
            ret = HAL_TIMEOUT;
            goto out;
        }
    }
    WRITE_REG(reg->INT_STATUS, 0x1 << OTPC_INT_STATUS_USER_DONE_INT_STATUS_SHIFT);

    qp = READ_REG(reg->USER_QP);
    if (((qp & 0xc0) == 0xc0) || (qp & 0x20)) {
        HAL_DBG_ERR("ecc check error during read setup\n");
        ret = HAL_ERROR;
        goto out;
    }

    *value = READ_REG(reg->USER_Q);

out:
    WRITE_REG_MASK_WE(reg->USER_CTRL, OTPC_USER_CTRL_USER_DCTRL_MASK, 0);

    return ret;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_OTP_MODULE_ENABLED */
