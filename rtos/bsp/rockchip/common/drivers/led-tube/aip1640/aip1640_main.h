/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __AIP1640_MAIN_H__
#define __AIP1640_MAIN_H__

#ifdef RT_USING_AIP1640
#include <rtdevice.h>
#include <rtthread.h>

#define RT_AIP1640_DEVICE          "aip1640"

struct aip1640_info
{
    char aip1640_buf[16];
    uint32_t mode_buf;
    uint32_t mode_val;
    uint32_t dim_val;
};

#endif
#endif
