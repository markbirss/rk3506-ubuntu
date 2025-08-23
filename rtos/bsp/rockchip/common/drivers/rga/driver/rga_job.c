/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * @file    rga_job.c
 * @author  cerf yu
 * @version V0.1
 * @date    23-Jul-2024
 * @brief   rga driver
 *
 ******************************************************************************
 */

#include "rga_job.h"
#include "rga_common.h"
#include "rga_debugger.h"

static void rga_job_free(struct rga_job *job)
{
    if (job->cmd_buf)
        kfree(job->cmd_buf);

    kfree(job);
}

static struct rga_job *rga_job_alloc(struct rga_req *rga_command_base)
{
    struct rga_job *job = NULL;

    job = kzalloc(sizeof(*job), GFP_KERNEL);
    if (!job)
        return NULL;

    // INIT_LIST_HEAD(&job->head);
    // kref_init(&job->refcount);

    job->timestamp = ktime_get();
    // job->pid = (rt_thread_self())->tid;

    job->rga_command_base = *rga_command_base;

    if (rga_command_base->priority > 0)
    {
        if (rga_command_base->priority > RGA_SCHED_PRIORITY_MAX)
            job->priority = RGA_SCHED_PRIORITY_MAX;
        else
            job->priority = rga_command_base->priority;
    }

    if (DEBUGGER_EN(INTERNAL_MODE))
    {
        job->flags |= RGA_JOB_DEBUG_FAKE_BUFFER;

        /* skip subsequent flag judgments. */
        return job;
    }

    // if (job->rga_command_base.handle_flag & 1) {
    //  job->flags |= RGA_JOB_USE_HANDLE;

    //  rga_job_judgment_support_core(job);
    // }

    rt_completion_init(&job->completion);

    return job;
}

static void rga_job_dump_info(struct rga_job *job)
{
    pr_info("job: reqeust_id = %d, priority = %d, core = %d\n",
            job->request_id, job->priority, job->core);
}

static int rga_job_put_buffer(struct rga_job *job)
{
    struct rga_img_info_t *dst = NULL;

    dst = &job->rga_command_base.dst;

    rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, (void *)dst->yrgb_addr,
                         rga_image_size_cal(dst->vir_w, dst->vir_h, dst->format, NULL, NULL, NULL));

    return 0;

}

static void rga_set_channel_buffer(struct rga_img_info_t *img, uint64_t phy_addr)
{
    img->yrgb_addr = phy_addr;
    img->uv_addr = img->yrgb_addr + img->vir_w * img->vir_h;
    img->v_addr = img->uv_addr + img->vir_w * img->vir_h;

    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, (void *)phy_addr,
                         rga_image_size_cal(img->vir_w, img->vir_h, img->format, NULL, NULL, NULL));
}

static int rga_job_set_buffer(struct rga_job *job)
{
    struct rga_img_info_t *src0 = NULL;
    struct rga_img_info_t *src1 = NULL;
    struct rga_img_info_t *dst = NULL;
    struct rga_img_info_t *els = NULL;

    if (job->rga_command_base.render_mode != COLOR_FILL_MODE)
        src0 = &job->rga_command_base.src;

    if (job->rga_command_base.render_mode != UPDATE_PALETTE_TABLE_MODE)
        src1 = job->rga_command_base.bsfilter_flag ?
               &job->rga_command_base.pat : NULL;
    else
        els = &job->rga_command_base.pat;

    dst = &job->rga_command_base.dst;

    if (src0 && src0->uv_addr)
        rga_set_channel_buffer(src0, src0->uv_addr);
    if (src1 && src1->uv_addr)
        rga_set_channel_buffer(src1, src1->uv_addr);
    if (els && els->uv_addr)
        rga_set_channel_buffer(els, els->uv_addr);
    if (dst && dst->uv_addr)
        rga_set_channel_buffer(dst, dst->uv_addr);

    return 0;

}

static int rga_job_run(struct rga_job *job, struct rga_scheduler *scheduler)
{
    int ret = 0;

    ret = scheduler->ops->set_reg(job, scheduler);
    if (ret < 0)
    {
        pr_err("set reg failed");
        return ret;
    }

    set_bit(RGA_JOB_STATE_RUNNING, &job->state);

    /* for debug */
    if (DEBUGGER_EN(MSG))
        rga_job_dump_info(job);

    return ret;
}

static int rga_job_timeout_query_state(struct rga_job *job, int orig_ret)
{
    struct rga_scheduler *scheduler = job->scheduler;

    if (scheduler->ops->read_status)
    {
        scheduler->ops->read_status(job, scheduler);
        pr_err("request[%d] core[%d]: INTR[0x%x], HW_STATUS[0x%x], CMD_STATUS[0x%x], WORK_CYCLE[0x%x(%d)]\n",
               job->request_id, scheduler->core,
               job->intr_status, job->hw_status, job->cmd_status,
               job->work_cycle, job->work_cycle);
    }

    if (test_bit(RGA_JOB_STATE_DONE, &job->state) &&
            test_bit(RGA_JOB_STATE_FINISH, &job->state))
    {
        return orig_ret;
    }
    else if (!test_bit(RGA_JOB_STATE_DONE, &job->state) &&
             test_bit(RGA_JOB_STATE_FINISH, &job->state))
    {
        pr_err("request[%d] job hardware has finished, but the software has timeout!\n",
               job->request_id);
        return -EBUSY;
    }
    else if (!test_bit(RGA_JOB_STATE_DONE, &job->state) &&
             !test_bit(RGA_JOB_STATE_FINISH, &job->state))
    {
        pr_err("request[%d] job hardware has timeout.\n", job->request_id);
        return -EBUSY;
    }

    return orig_ret;
}

static struct rga_scheduler *rga_job_schedule(struct rga_job *job)
{
    struct rga_scheduler *scheduler = NULL;

    // for (i = 0; i < g_rga_drv_data->num_of_scheduler; i++) {
    //  scheduler = g_rga_drv_data->scheduler[i];
    //  rga_job_scheduler_timeout_clean(scheduler);
    // }

    if (g_rga_drv_data->num_of_scheduler > 1)
    {
        job->core = rga_job_assign(job);
        if (job->core <= 0)
        {
            pr_err("job assign failed");
            job->ret = -EINVAL;
            return NULL;
        }
    }
    else
    {
        job->core = g_rga_drv_data->scheduler[0]->core;
        job->scheduler = g_rga_drv_data->scheduler[0];
    }

    scheduler = job->scheduler;
    if (scheduler == NULL)
    {
        pr_err("failed to get scheduler, %s(%d)\n", __func__, __LINE__);
        job->ret = -EFAULT;
        return NULL;
    }

    return scheduler;
}

int rga_job_wait(struct rga_job *job)
{
    int ret;

    ret = rt_completion_wait(&job->completion, RGA_JOB_TIMEOUT_DELAY * RT_TICK_PER_SECOND / 1000);
    switch (ret)
    {
    case RT_EOK:
        if (test_bit(RGA_JOB_STATE_DONE, &job->state))
        {
            break;
        }
        else
        {
            ret = rga_job_timeout_query_state(job, -EBUSY);
            goto err_scheduler_abort;
        }
    case -RT_ETIMEOUT:
    default:
        ret = rga_job_timeout_query_state(job, -EBUSY);
        goto err_scheduler_abort;
    }

    return 0;

err_scheduler_abort:
    job->scheduler->ops->soft_reset(job->scheduler);

    return ret;
}

int rga_job_commit(struct rga_req *rga_command_base, struct rga_session *session)
{
    int ret = 0;
    struct rga_job *job = NULL;
    struct rga_scheduler *scheduler = NULL;

    job = rga_job_alloc(rga_command_base);
    if (!job)
    {
        pr_err("failed to alloc rga job!\n");
        return -ENOMEM;
    }

    job->session = session;

    scheduler = rga_job_schedule(job);
    if (scheduler == NULL)
    {
        pr_err("failed to get scheduler, %s(%d)\n", __func__, __LINE__);
        goto err_free_job;
    }

    job->cmd_buf = kzalloc(sizeof(*job->cmd_buf), GFP_KERNEL);
    if (job->cmd_buf == NULL)
    {
        pr_err("failed to alloc command buffer.\n");
        ret = -ENOMEM;
        goto err_free_job;
    }

    job->cmd_buf->vaddr = kzalloc(RGA_CMD_REG_SIZE, GFP_KERNEL);
    if (job->cmd_buf->vaddr == NULL)
    {
        pr_err("failed to alloc command buffer virt_addr.\n");
        ret = -ENOMEM;
        goto err_free_cmd_buf;
    }
    job->cmd_buf->dma_addr = (dma_addr_t)job->cmd_buf->vaddr;
    job->cmd_buf->size = RGA_CMD_REG_SIZE;

    rga_job_set_buffer(job);

    ret = scheduler->ops->init_reg(job);
    if (ret < 0)
    {
        pr_err("%s: init reg failed", __func__);
        job->ret = ret;
        goto err_free_cmd_buf_virt_addr;
    }

    scheduler->running_job = job;
    set_bit(RGA_JOB_STATE_PREPARE, &job->state);

    ret = rga_job_run(job, scheduler);
    /* If some error before hw run */
    if (ret < 0)
    {
        pr_err("some error on rga_job_run before hw start, %s(%d)\n", __func__, __LINE__);

        scheduler->running_job = NULL;
        goto err_free_cmd_buf_virt_addr;
    }

    ret = rga_job_wait(job);
    if (ret < 0)
    {
        pr_err("wait job timeout\n");
        goto err_free_cmd_buf_virt_addr;
    }

    rga_job_put_buffer(job);

err_free_cmd_buf_virt_addr:
    kfree(job->cmd_buf->vaddr);
    job->cmd_buf->vaddr = NULL;

err_free_cmd_buf:
    kfree(job->cmd_buf);
    job->cmd_buf = NULL;

err_free_job:
    ret = job->ret;
    rga_job_free(job);

    return ret;
}

struct rga_job *rga_job_done(struct rga_scheduler *scheduler)
{
    struct rga_job *job;
    ktime_t now = ktime_get();

    job = scheduler->running_job;
    if (job == NULL)
    {
        pr_err("core[0x%x] running job has been cleanup.\n", scheduler->core);

        return NULL;
    }

    scheduler->running_job = NULL;
    set_bit(RGA_JOB_STATE_DONE, &job->state);
    rt_completion_done(&job->completion);

    if (scheduler->ops->read_back_reg)
        scheduler->ops->read_back_reg(job, scheduler);

    if (DEBUGGER_EN(TIME))
        pr_info("request[%d], hardware[%s] cost time %lld us, work cycle %d\n",
                job->request_id,
                rga_get_core_name(scheduler->core),
                ktime_us_delta(now, job->hw_running_time),
                job->work_cycle);

    return job;
}
