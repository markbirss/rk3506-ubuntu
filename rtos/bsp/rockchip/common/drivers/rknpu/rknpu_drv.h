/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Randall zhuo <randall.zhuo@rock-chips.com>
 */

#ifndef __RTT_RKNPU_DRV_H_
#define __RTT_RKNPU_DRV_H_

#include <rtdevice.h>
#include <rthw.h>

#include "rtos_adapter.h"
#include "rknpu_job.h"
#include "hal_rknpu.h"

#define DRIVER_NAME "rknpu"
#define DRIVER_DESC "RKNPU driver"
#define DRIVER_DATE "20240123"
#define DRIVER_MAJOR 0
#define DRIVER_MINOR 8
#define DRIVER_PATCHLEVEL 0

#define RKNPU_CORE_BASE_ADDR 0x50C80000
#define RKNPU_RESET_ADDR 0x50C90008

#define LOG_INFO LOG_I
#define LOG_WARN LOG_W
#define LOG_DEBUG LOG_D
#define LOG_ERROR LOG_E

struct rknpu_irqs_data
{
    const char *name;
    void (*irq_hdl)(int vector, void *ctx);
};

struct rknpu_config
{
    rt_uint32_t bw_priority_addr;
    rt_uint32_t bw_priority_length;
    rt_uint32_t dma_mask;
    rt_uint32_t pc_data_amount_scale;
    rt_uint32_t pc_task_number_bits;
    rt_uint32_t pc_task_number_mask;
    rt_uint32_t pc_task_status_offset;
    rt_uint32_t pc_dma_ctrl;
    rt_uint32_t bw_enable;
    const struct rknpu_irqs_data *irqs;
    int num_irqs;
    int num_resets;
    rt_uint32_t nbuf_phyaddr;
    rt_uint32_t nbuf_size;
    rt_uint32_t max_submit_number;
    rt_uint32_t core_mask;
};

struct rknpu_subcore_data
{
    rt_list_t todo_list;
    struct rt_event job_done_wq;
    struct rknpu_job *job;
    int64_t task_num;
};

struct rknpu_device
{
    struct rt_device dev;
    const struct rknpu_config *config;
    struct HAL_RKNPU_DEV hal_rknpu;
    struct rt_mutex lock;
#ifdef RT_USING_SMP
    struct rt_spinlock irq_lock;
#endif
    int iommu_en;
    uint32_t base[RKNPU_MAX_CORES];
    uint32_t reset_addr;
    uint32_t bw_priority_base;

    struct rknpu_subcore_data subcore_datas[RKNPU_MAX_CORES];
    uint32_t soft_reseting;
};

#endif /* __RTT_RKNPU_DRV_H_ */
