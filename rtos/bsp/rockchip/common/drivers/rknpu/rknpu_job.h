/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Randall zhuo <randall.zhuo@rock-chips.com>
 */

#ifndef __RTT_RKNPU_JOB_H_
#define __RTT_RKNPU_JOB_H_

#include "rknpu_ioctl.h"
#include "rtos_adapter.h"

#include <stdatomic.h>

#define RKNPU_MAX_CORES 1

#define RKNPU_JOB_DONE (1 << 0)
#define RKNPU_JOB_ASYNC (1 << 1)
#define RKNPU_JOB_DETACHED (1 << 2)

#define RKNPU_CORE_AUTO_MASK 0x00
#define RKNPU_CORE0_MASK 0x01
#define RKNPU_CORE1_MASK 0x02
#define RKNPU_CORE2_MASK 0x04

struct rknpu_job
{
    struct rknpu_device *rknpu_dev;
    rt_list_t head[RKNPU_MAX_CORES];
    bool irq_entry[RKNPU_MAX_CORES];
    unsigned int flags;
    int ret;
    struct rknpu_submit *args;
    bool args_owner;
    struct rknpu_task *first_task;
    struct rknpu_task *last_task;
    uint32_t int_mask[RKNPU_MAX_CORES];
    uint32_t int_status[RKNPU_MAX_CORES];
    rt_tick_t timestamp;
    uint32_t use_core_num;
    atomic_int run_count;
    atomic_int interrupt_count;
    rt_tick_t hw_commit_time;
    rt_tick_t hw_recoder_time;
    rt_tick_t hw_elapse_time;
    atomic_int submit_count[RKNPU_MAX_CORES];
};

irqreturn_t rknpu_core0_irq_handler(int irq, void *data);

int rknpu_submit_ioctl(struct rknpu_device *rknpu_dev, unsigned long data);

int rknpu_get_hw_version(struct rknpu_device *rknpu_dev, uint32_t *version);

int rknpu_get_bw_priority(struct rknpu_device *rknpu_dev, uint32_t *priority,
                          uint32_t *expect, uint32_t *tw);

int rknpu_set_bw_priority(struct rknpu_device *rknpu_dev, uint32_t priority,
                          uint32_t expect, uint32_t tw);

int rknpu_clear_rw_amount(struct rknpu_device *rknpu_dev);

int rknpu_get_rw_amount(struct rknpu_device *rknpu_dev, uint32_t *dt_wr,
                        uint32_t *dt_rd, uint32_t *wd_rd);

int rknpu_get_total_rw_amount(struct rknpu_device *rknpu_dev, uint32_t *amount);

#endif /* __RTT_RKNPU_JOB_H_ */
