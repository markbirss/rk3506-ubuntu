/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_thermal.h
  * @version V0.1
  * @brief   tsadc interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-03-11     Ye.Zhang      first implementation
  *
  ******************************************************************************
  */

#ifndef _DRV_THERMAL_H_
#define _DRV_THERMAL_H_

#include <hal_base.h>
#include "hal_tsadc.h"

#if defined(RT_USING_TSADC)

#define SOC_MAX_SENSORS 7

struct tsadc_init
{
    int chn_id[SOC_MAX_SENSORS];
    int chn_num;
    eTSADC_tshutPolarity polarity;
    eTSADC_tshutMode mode;
    uint32_t otp_offset[SOC_MAX_SENSORS];
    uint32_t otp_mask;
};

void tsadc_dev_init(const struct tsadc_init *p_tsadc_init);
int tsadc_get_temp(int chn);
#endif

#endif // _DRV_THERMAL_H_
