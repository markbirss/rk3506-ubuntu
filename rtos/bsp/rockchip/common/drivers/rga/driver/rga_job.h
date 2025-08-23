/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * @file    rga_job.h
 * @author  cerf yu
 * @version V0.1
 * @date    23-Jul-2024
 * @brief   rga driver
 *
 ******************************************************************************
 */

#ifndef __LINUX_RKRGA_JOB_H_
#define __LINUX_RKRGA_JOB_H_

#include "rga_drv.h"

#define RGA_CMD_REG_SIZE 256 /* 32 * 8 bit */

enum job_flags
{
    RGA_JOB_DONE            = 1 << 0,
    RGA_JOB_ASYNC           = 1 << 1,
    RGA_JOB_SYNC            = 1 << 2,
    RGA_JOB_USE_HANDLE      = 1 << 3,
    RGA_JOB_UNSUPPORT_RGA_MMU   = 1 << 4,
    RGA_JOB_DEBUG_FAKE_BUFFER   = 1 << 5,
};

int rga_job_commit(struct rga_req *rga_command_base, struct rga_session *session);
int rga_job_wait(struct rga_job *job);
struct rga_job *rga_job_done(struct rga_scheduler *scheduler);

int rga_job_assign(struct rga_job *job);

#endif /* #ifndef __LINUX_RKRGA_JOB_H_ */
