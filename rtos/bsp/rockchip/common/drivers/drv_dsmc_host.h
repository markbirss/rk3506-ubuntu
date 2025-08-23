/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_dsmc_host.h
  * @author  Zhihuan He
  * @version V0.1
  * @date    12-Sep-2024
  * @brief   dsmc host driver
  *
  ******************************************************************************
  */

#include "iomux_base.h"

#ifdef RT_USING_DSMC_HOST

#ifndef _DRV_DSMC_HOST_H_
#define _DRV_DSMC_HOST_H_

struct rockchip_rt_dsmc_host
{
    struct rt_device parent;

    struct HAL_DSMC_HOST *host;
    struct dsmc_host_ops *ops;

    /* DMA */
    uint32_t ops_cs;
    uint32_t ops_dma_tx_req;
    uint32_t ops_dma_rx_req;
    struct rt_device *dma;
    struct rt_dma_transfer xfer;
    bool no_dma;
};

struct dsmc_host_ops
{
    rt_err_t (*read)(struct rockchip_rt_dsmc_host *dsmc_host,
                     uint32_t cs, uint32_t region,
                     unsigned long addr, uint32_t *data);
    rt_err_t (*write)(struct rockchip_rt_dsmc_host *dsmc_host,
                      uint32_t cs, uint32_t region,
                      unsigned long addr, uint32_t val);
    rt_err_t (*copy_from)(struct rockchip_rt_dsmc_host *dsmc_host,
                          uint32_t cs, uint32_t region, uint32_t from,
                          uint32_t dst_phys, size_t size);
    rt_err_t (*copy_from_state)(struct rockchip_rt_dsmc_host *dsmc_host);
    rt_err_t (*copy_to)(struct rockchip_rt_dsmc_host *dsmc_host,
                        uint32_t cs, uint32_t region, uint32_t src_phys,
                        uint32_t to, size_t size);
    rt_err_t (*copy_to_state)(struct rockchip_rt_dsmc_host *dsmc_host);
};

#endif

#endif /* _DRV_DSMC_HOST_H_ */
