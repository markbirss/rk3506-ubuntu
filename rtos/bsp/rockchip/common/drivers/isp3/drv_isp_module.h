/**
  * Copyright (c) 2022 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_isp_module.h
  * @version V0.0.1
  * @date    6-July-2022
  * @brief
  *
  ******************************************************************************
  */

#ifndef __DRV_ISP_MODULE_H__
#define __DRV_ISP_MODULE_H__

#include <rthw.h>
#include "common.h"

#define ISP_MODULE_RAWAE0    BIT(0)
#define ISP_MODULE_RAWAE1    BIT(1)
#define ISP_MODULE_RAWAWB    BIT(2)
#define ISP_MODULE_BLS       BIT(3)
#define ISP_MODULE_AWBGAIN   BIT(4)

#define ISP_RAWAELITE_MEAN_NUM 25
#define ISP_RAWAEBIG_MEAN_NUM 225

struct rk_isp_dev;

/***************************params***************************/

struct isp_bls_cfg
{
    int16_t r;
    int16_t gr;
    int16_t gb;
    int16_t b;
};

struct isp_awbgain_cfg
{
    uint16_t gb;
    uint16_t gr;
    uint16_t b;
    uint16_t r;
};

struct rkisp_params
{
    struct isp_bls_cfg bls;
    struct isp_awbgain_cfg awbgain;
};

struct rkisp_params_buffer
{
    uint32_t module_cfg_update;
    uint32_t module_en_update;
    uint32_t module_ens;

    uint32_t frame_id;
    struct rkisp_params params;
};

/***************************stats****************************/

struct isp_rawae_meas_data
{
    uint16_t channelg_xy;
    uint16_t channelb_xy;
    uint16_t channelr_xy;
};

struct isp_rawaelite_stat
{
    struct isp_rawae_meas_data data[ISP_RAWAELITE_MEAN_NUM];
};

struct isp_rawaebig_stat
{
    struct isp_rawae_meas_data data[ISP_RAWAEBIG_MEAN_NUM];
};

struct isp_rawawb_stat
{
    uint32_t rgain_nor;
    uint32_t bgain_nor;
    uint32_t wp_num_nor;
    uint32_t wp_num2;

    uint32_t rgain_big;
    uint32_t bgain_big;
    uint32_t wp_num_big;
};

struct rkisp_stats
{
    struct isp_rawaelite_stat rawae0;
    struct isp_rawaebig_stat rawae1;
    struct isp_rawawb_stat rawawb;
};

struct rkisp_stats_buffer
{
    uint32_t meas_type;
    uint32_t frame_id;
    struct rkisp_stats stats;
};

/*************************************************************/

rt_err_t rk_isp_module_init(struct rk_isp_dev *dev);
rt_err_t rk_isp_set_params(struct rk_isp_dev *dev, struct rkisp_params_buffer *buf);
rt_err_t rk_isp_get_stat(struct rk_isp_dev *dev, struct rkisp_stats_buffer *buf);
rt_err_t rk_isp_isr(struct rk_isp_dev *dev, uint32_t mis);

#endif

