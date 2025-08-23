/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_otp.h
  * @version V0.1
  * @brief   otp interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-04-30     Finley.Xiao     first implementation
  *
  ******************************************************************************
  */

#ifndef _DRV_OTP_H_
#define _DRV_OTP_H_

#include <hal_base.h>
#include "hal_otp.h"

#if defined(RT_USING_OTP)

struct otp_data
{
    struct OTPC_REG *reg;
    uint32_t size;
    uint32_t nbytes;
    uint32_t ns_offset;
};

rt_err_t otp_read(uint8_t offset, uint32_t count, uint8_t *val);

#endif

#endif // _DRV_OTP_H_
