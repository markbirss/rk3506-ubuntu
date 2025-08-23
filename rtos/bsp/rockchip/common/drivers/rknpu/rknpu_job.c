/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * @file    rknpu_job.c
 * @author  randall zhuo
 * @version V0.1
 * @date    23-Jan-2024
 * @brief   rknpu job handle
 *
 ******************************************************************************
 */

#include "rknpu_ioctl.h"
#include "rknpu_drv.h"
#include "rknpu_job.h"

static inline void writel(uint32_t value, uint32_t address)
{
    *((volatile uint32_t *)address) = value;
}

static inline uint32_t readl(uint32_t address)
{
    return *((volatile uint32_t *)address);
}

#define _REG_READ(base, offset) readl(base + (offset))
#define _REG_WRITE(base, value, offset) writel(value, base + (offset))

#define REG_READ(offset) _REG_READ(rknpu_core_base, offset)
#define REG_WRITE(value, offset) _REG_WRITE(rknpu_core_base, value, offset)

static int rknpu_wait_core_index(int core_mask)
{
    int index = 0;

    switch (core_mask)
    {
    case RKNPU_CORE0_MASK:
    case RKNPU_CORE0_MASK | RKNPU_CORE1_MASK:
    case RKNPU_CORE0_MASK | RKNPU_CORE1_MASK | RKNPU_CORE2_MASK:
        index = 0;
        break;
    case RKNPU_CORE1_MASK:
        index = 1;
        break;
    case RKNPU_CORE2_MASK:
        index = 2;
        break;
    default:
        break;
    }

    return index;
}

static int rknpu_core_mask(int core_index)
{
    int core_mask = RKNPU_CORE_AUTO_MASK;

    switch (core_index)
    {
    case 0:
        core_mask = RKNPU_CORE0_MASK;
        break;
    case 1:
        core_mask = RKNPU_CORE1_MASK;
        break;
    case 2:
        core_mask = RKNPU_CORE2_MASK;
        break;
    default:
        break;
    }

    return core_mask;
}

static int rknpu_get_task_number(struct rknpu_job *job, int core_index)
{
    int task_num = job->args->task_number;

    return task_num;
}

static void rknpu_job_free(struct rknpu_job *job)
{
    rt_free(job);
}

static int rknpu_job_cleanup(struct rknpu_job *job)
{
    rknpu_job_free(job);

    return 0;
}

static struct rknpu_job *rknpu_job_alloc(struct rknpu_device *rknpu_dev,
        struct rknpu_submit *args)
{
    struct rknpu_job *job = NULL;

    job = rt_malloc(sizeof(*job));
    if (!job)
        return NULL;

    job->timestamp = ktime_get();
    job->rknpu_dev = rknpu_dev;
    job->use_core_num = (args->core_mask & RKNPU_CORE0_MASK) +
                        ((args->core_mask & RKNPU_CORE1_MASK) >> 1) +
                        ((args->core_mask & RKNPU_CORE2_MASK) >> 2);
    atomic_init(&job->run_count, job->use_core_num);
    atomic_init(&job->interrupt_count, job->use_core_num);
    for (int i = 0; i < RKNPU_MAX_CORES; i++)
    {
        atomic_init(&job->submit_count[i], 0);
    }

    job->args = args;
    job->args_owner = false;
    return job;
}

static int rknpu_job_wait(struct rknpu_job *job)
{
    struct rknpu_device *rknpu_dev = job->rknpu_dev;
    struct rknpu_submit *args = job->args;
    struct rknpu_task *last_task = NULL;
    struct rknpu_subcore_data *subcore_data = NULL;
    struct rknpu_job *entry, *q;
    uint32_t rknpu_core_base = 0;
    int core_index = rknpu_wait_core_index(job->args->core_mask);
#ifdef RT_USING_SMP
    unsigned long flags;
#endif
    int wait_count = 0;
    bool continue_wait = false;
    int ret = -EINVAL;
    int i = 0;

    subcore_data = &rknpu_dev->subcore_datas[core_index];

    do
    {
        ret = rt_event_recv(&subcore_data->job_done_wq,
                            RKNPU_JOB_DONE || rknpu_dev->soft_reseting,
                            RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                            args->timeout * RT_TICK_PER_SECOND / 1000, 0);

        if (++wait_count >= 3)
            break;

        if (ret != RT_EOK)
        {
            int64_t elapse_time_us = 0;
#ifdef RT_USING_SMP
            flags = rt_spin_lock_irqsave(&rknpu_dev->irq_lock);
#endif
            elapse_time_us = ktime_us_delta(ktime_get(), job->hw_commit_time);
            continue_wait = job->hw_commit_time == 0 ? true : (elapse_time_us < args->timeout * 1000);
#ifdef RT_USING_SMP
            rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif
            LOG_ERROR("job: %p, wait_count: %d, continue wait: %d, commit elapse time: %ldus, wait time: %ldus, timeout: %dus\n",
                      job, wait_count, continue_wait,
                      (job->hw_commit_time == 0 ? 0 : elapse_time_us),
                      ktime_us_delta(ktime_get(), job->timestamp),
                      args->timeout * 1000);

            rt_thread_mdelay(100);
        }
    }
    while (ret != RT_EOK && continue_wait);

    last_task = job->last_task;
    if (!last_task)
    {
#ifdef RT_USING_SMP
        flags = rt_spin_lock_irqsave(&rknpu_dev->irq_lock);
#endif
        for (i = 0; i < job->use_core_num; i++)
        {
            subcore_data = &rknpu_dev->subcore_datas[i];
            rt_list_for_each_entry_safe(entry, q, &subcore_data->todo_list, head[i])
            {
                if (entry == job)
                {
                    rt_list_remove(&job->head[i]);
                    break;
                }
            }
        }
#ifdef RT_USING_SMP
        rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif

        LOG_ERROR("job commit failed\n");
        return ret < 0 ? ret : -EINVAL;
    }

    last_task->int_status = job->int_status[core_index];

    if (ret != RT_EOK)
    {
        args->task_counter = 0;
        rknpu_core_base = rknpu_dev->base[core_index];
        if (args->flags & RKNPU_JOB_PC)
        {
            uint32_t task_status = REG_READ(rknpu_dev->config->pc_task_status_offset);
            if (((task_status & rknpu_dev->config->pc_task_number_mask) == 0) && ((task_status & 0xfff00000) != 0))
            {
                args->task_counter = ((task_status & (rknpu_dev->config->pc_task_number_mask << 16)) >> 16) - 1;
            }
            else
            {
                args->task_counter = task_status & rknpu_dev->config->pc_task_number_mask;
            }
        }

        LOG_ERROR("failed to wait job, task counter: %d, flags: 0x%x, ret = %d, elapsed time: %ldus\n",
                  args->task_counter, args->flags, ret,
                  ktime_us_delta(ktime_get(), job->timestamp));

        return ret < 0 ? ret : -ETIMEDOUT;
    }

    if (!(job->flags & RKNPU_JOB_DONE))
        return -EINVAL;

    args->task_counter = args->task_number;
    args->hw_elapse_time = job->hw_elapse_time;

    return 0;
}

static int rknpu_job_subcore_commit_pc(struct rknpu_job *job,
                                       int core_index)
{
    struct rknpu_device *rknpu_dev = job->rknpu_dev;
    struct rknpu_submit *args = job->args;
    void *task_obj =
        (void *)(uintptr_t)args->task_obj_addr;
    struct rknpu_task *task_base = NULL;
    struct rknpu_task *first_task = NULL;
    struct rknpu_task *last_task = NULL;
    uint32_t rknpu_core_base = rknpu_dev->base[core_index];
    int task_start = args->task_start;
    int task_end;
    int task_number = args->task_number;
    int task_pp_en = args->flags & RKNPU_JOB_PINGPONG ? 1 : 0;
    int pc_data_amount_scale = rknpu_dev->config->pc_data_amount_scale;
    int pc_task_number_bits = rknpu_dev->config->pc_task_number_bits;
    int submit_index = atomic_load(&job->submit_count[core_index]);
    int max_submit_number = rknpu_dev->config->max_submit_number;
#ifdef RT_USING_SMP
    unsigned long flags;
#endif

    if (!task_obj)
    {
        job->ret = -EINVAL;
        return job->ret;
    }

    task_start = task_start + submit_index * max_submit_number;
    task_number = task_number - submit_index * max_submit_number;
    task_number = task_number > max_submit_number ? max_submit_number : task_number;
    task_end = task_start + task_number - 1;

    task_base = task_obj;
    first_task = &task_base[task_start];
    last_task = &task_base[task_end];

    if (rknpu_dev->config->pc_dma_ctrl)
    {
#ifdef RT_USING_SMP
        flags = rt_spin_lock_irqsave(&rknpu_dev->irq_lock);
#endif
        REG_WRITE(first_task->regcmd_addr, RKNPU_OFFSET_PC_DATA_ADDR);
#ifdef RT_USING_SMP
        rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif
    }
    else
    {
        REG_WRITE(first_task->regcmd_addr, RKNPU_OFFSET_PC_DATA_ADDR);
    }

    REG_WRITE((first_task->regcfg_amount + RKNPU_PC_DATA_EXTRA_AMOUNT + pc_data_amount_scale - 1) / pc_data_amount_scale -  1,
              RKNPU_OFFSET_PC_DATA_AMOUNT);

    REG_WRITE(last_task->int_mask, RKNPU_OFFSET_INT_MASK);

    REG_WRITE(first_task->int_mask, RKNPU_OFFSET_INT_CLEAR);

    REG_WRITE(((0x6 | task_pp_en) << pc_task_number_bits) | task_number, RKNPU_OFFSET_PC_TASK_CONTROL);

    REG_WRITE(args->task_base_addr, RKNPU_OFFSET_PC_DMA_BASE_ADDR);

    job->first_task = first_task;
    job->last_task = last_task;
    job->int_mask[core_index] = last_task->int_mask;

    REG_WRITE(0x1, RKNPU_OFFSET_PC_OP_EN);
    REG_WRITE(0x0, RKNPU_OFFSET_PC_OP_EN);

    return 0;
}

static int rknpu_job_subcore_commit(struct rknpu_job *job,
                                    int core_index)
{
    struct rknpu_device *rknpu_dev = job->rknpu_dev;
    struct rknpu_submit *args = job->args;
    uint32_t rknpu_core_base = rknpu_dev->base[core_index];
#ifdef RT_USING_SMP
    unsigned long flags;
#endif

    // switch to slave mode
    if (rknpu_dev->config->pc_dma_ctrl)
    {
#ifdef RT_USING_SMP
        flags = rt_spin_lock_irqsave(&rknpu_dev->irq_lock);
#endif
        REG_WRITE(0x1, RKNPU_OFFSET_PC_DATA_ADDR);
#ifdef RT_USING_SMP
        rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif
    }
    else
    {
        REG_WRITE(0x1, RKNPU_OFFSET_PC_DATA_ADDR);
    }

    if (!(args->flags & RKNPU_JOB_PC))
    {
        job->ret = -EINVAL;
        return job->ret;
    }

    return rknpu_job_subcore_commit_pc(job, core_index);
}

static void rknpu_job_commit(struct rknpu_job *job)
{
    rknpu_job_subcore_commit(job, 0);
}

static void rknpu_job_next(struct rknpu_device *rknpu_dev, int core_index)
{
    struct rknpu_job *job = NULL;
    struct rknpu_subcore_data *subcore_data = NULL;
#ifdef RT_USING_SMP
    unsigned long flags;
#endif

    if (rknpu_dev->soft_reseting)
        return;

    subcore_data = &rknpu_dev->subcore_datas[core_index];

#ifdef RT_USING_SMP
    flags = rt_spin_lock_irqsave(&rknpu_dev->irq_lock);
#endif

    if (subcore_data->job || rt_list_isempty(&subcore_data->todo_list))
    {
#ifdef RT_USING_SMP
        rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif
        return;
    }

    job = rt_list_first_entry(&subcore_data->todo_list, struct rknpu_job, head[core_index]);

    rt_list_remove(&job->head[core_index]);
    rt_list_init(&job->head[core_index]);
    subcore_data->job = job;
    job->hw_commit_time = ktime_get();
    job->hw_recoder_time = job->hw_commit_time;
#ifdef RT_USING_SMP
    rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif

    atomic_fetch_sub(&job->run_count, 1);
    if (atomic_load(&job->run_count) == 0)
    {
        rknpu_job_commit(job);
    }
}

static void rknpu_job_done(struct rknpu_job *job, int ret, int core_index)
{
    struct rknpu_device *rknpu_dev = job->rknpu_dev;
    struct rknpu_subcore_data *subcore_data = NULL;
    rt_tick_t now;
#ifdef RT_USING_SMP
    unsigned long flags;
#endif
    int max_submit_number = rknpu_dev->config->max_submit_number;

    atomic_fetch_add(&job->submit_count[core_index], 1);
    if (atomic_load(&job->submit_count[core_index]) < (rknpu_get_task_number(job, core_index) + max_submit_number - 1) / max_submit_number)
    {
        rknpu_job_subcore_commit(job, core_index);
        return;
    }

    subcore_data = &rknpu_dev->subcore_datas[core_index];

#ifdef RT_USING_SMP
    flags = rt_spin_lock_irqsave(&rknpu_dev->irq_lock);
#endif
    subcore_data->job = NULL;
    subcore_data->task_num -= rknpu_get_task_number(job, core_index);
    now = ktime_get();
    job->hw_elapse_time = ktime_sub(now, job->hw_commit_time);
#ifdef RT_USING_SMP
    rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif

    atomic_fetch_sub(&job->interrupt_count, 1);
    if (atomic_load(&job->interrupt_count) == 0)
    {
        job->flags |= RKNPU_JOB_DONE;
        job->ret = ret;
        rt_event_send(&subcore_data->job_done_wq, RKNPU_JOB_DONE);
    }

    rknpu_job_next(rknpu_dev, core_index);
}

static int rknpu_schedule_core_index(struct rknpu_device *rknpu_dev)
{
    return 0;
}

static void rknpu_job_schedule(struct rknpu_job *job)
{
    struct rknpu_device *rknpu_dev = job->rknpu_dev;
    struct rknpu_subcore_data *subcore_data = NULL;
    int i = 0, core_index = 0;
#ifdef RT_USING_SMP
    unsigned long flags;
#endif

    if (job->args->core_mask == RKNPU_CORE_AUTO_MASK)
    {
        core_index = rknpu_schedule_core_index(rknpu_dev);
        job->args->core_mask = rknpu_core_mask(core_index);
        job->use_core_num = 1;
        atomic_store(&job->run_count, job->use_core_num);
        atomic_store(&job->interrupt_count, job->use_core_num);
    }

#ifdef RT_USING_SMP
    flags = rt_spin_lock_irqsave(&rknpu_dev->irq_lock);
#endif
    for (i = 0; i < rknpu_dev->config->num_irqs; i++)
    {
        if (job->args->core_mask & rknpu_core_mask(i))
        {
            subcore_data = &rknpu_dev->subcore_datas[i];
            rt_list_insert_after(&subcore_data->todo_list, &job->head[i]);
            subcore_data->task_num += rknpu_get_task_number(job, i);
        }
    }
#ifdef RT_USING_SMP
    rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif

    for (i = 0; i < rknpu_dev->config->num_irqs; i++)
    {
        if (job->args->core_mask & rknpu_core_mask(i))
            rknpu_job_next(rknpu_dev, i);
    }
}

// TODO
int rknpu_soft_reset(struct rknpu_device *rknpu_dev)
{
    return RT_EOK;
}

static void rknpu_job_abort(struct rknpu_job *job)
{
    struct rknpu_device *rknpu_dev = job->rknpu_dev;
    struct rknpu_subcore_data *subcore_data = NULL;
#ifdef RT_USING_SMP
    unsigned long flags;
#endif
    int i = 0;

    rt_thread_mdelay(100);

#ifdef RT_USING_SMP
    flags = rt_spin_lock_irqsave(&rknpu_dev->irq_lock);
#endif
    for (i = 0; i < rknpu_dev->config->num_irqs; i++)
    {
        if (job->args->core_mask & rknpu_core_mask(i))
        {
            subcore_data = &rknpu_dev->subcore_datas[i];
            if (job == subcore_data->job && !job->irq_entry[i])
            {
                subcore_data->job = NULL;
                subcore_data->task_num -= rknpu_get_task_number(job, i);
            }
        }
    }
#ifdef RT_USING_SMP
    rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif

    if (job->ret == -ETIMEDOUT)
    {
        LOG_ERROR("job timeout, flags: 0x%x:\n", job->flags);
        for (i = 0; i < rknpu_dev->config->num_irqs; i++)
        {
            if (job->args->core_mask & rknpu_core_mask(i))
            {
                uint32_t rknpu_core_base = rknpu_dev->base[i];
                LOG_ERROR("\tcore %d irq status: 0x%x, raw status: 0x%x, require mask: 0x%x, task counter: 0x%x, elapsed time: %ldus\n",
                          i, REG_READ(RKNPU_OFFSET_INT_STATUS), REG_READ(RKNPU_OFFSET_INT_RAW_STATUS),
                          job->int_mask[i], REG_READ(rknpu_dev->config->pc_task_status_offset) & rknpu_dev->config->pc_task_number_mask,
                          ktime_us_delta(ktime_get(), job->timestamp));
            }
        }
        rknpu_soft_reset(rknpu_dev);
    }
    else
    {
        LOG_ERROR("job abort, flags: 0x%x, ret: %d, elapsed time: %ldus\n",
                  job->flags, job->ret, ktime_us_delta(ktime_get(), job->timestamp));
    }

    rknpu_job_cleanup(job);
}

static uint32_t rknpu_fuzz_status(uint32_t status)
{
    uint32_t fuzz_status = 0;

    if ((status & 0x3) != 0)
        fuzz_status |= 0x3;

    if ((status & 0xc) != 0)
        fuzz_status |= 0xc;

    if ((status & 0x30) != 0)
        fuzz_status |= 0x30;

    if ((status & 0xc0) != 0)
        fuzz_status |= 0xc0;

    if ((status & 0x300) != 0)
        fuzz_status |= 0x300;

    if ((status & 0xc00) != 0)
        fuzz_status |= 0xc00;

    return fuzz_status;
}

static irqreturn_t rknpu_irq_handler(int irq, void *data, int core_index)
{
    struct rknpu_device *rknpu_dev = data;
    uint32_t rknpu_core_base = rknpu_dev->base[core_index];
    struct rknpu_subcore_data *subcore_data = NULL;
    struct rknpu_job *job = NULL;
    uint32_t status = 0;
#ifdef RT_USING_SMP
    unsigned long flags;
#endif

    subcore_data = &rknpu_dev->subcore_datas[core_index];

#ifdef RT_USING_SMP
    flags = rt_spin_lock_irqsave(&rknpu_dev->irq_lock);
#endif

    job = subcore_data->job;
    if (!job)
    {
#ifdef RT_USING_SMP
        rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif
        REG_WRITE(RKNPU_INT_CLEAR, RKNPU_OFFSET_INT_CLEAR);
        rknpu_job_next(rknpu_dev, core_index);
        return IRQ_HANDLED;
    }
    job->irq_entry[core_index] = true;

#ifdef RT_USING_SMP
    rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif

    status = REG_READ(RKNPU_OFFSET_INT_STATUS);

    job->int_status[core_index] = status;

    if (rknpu_fuzz_status(status) != job->int_mask[core_index])
    {
        LOG_ERROR("invalid irq status: 0x%x, raw status: 0x%x, require mask: 0x%x, task counter: 0x%x\n",
                  status, REG_READ(RKNPU_OFFSET_INT_RAW_STATUS), job->int_mask[core_index],
                  (REG_READ(rknpu_dev->config->pc_task_status_offset) & rknpu_dev->config->pc_task_number_mask));

        REG_WRITE(RKNPU_INT_CLEAR, RKNPU_OFFSET_INT_CLEAR);
        return IRQ_HANDLED;
    }

    REG_WRITE(RKNPU_INT_CLEAR, RKNPU_OFFSET_INT_CLEAR);

    rknpu_job_done(job, 0, core_index);

    return IRQ_HANDLED;
}

irqreturn_t rknpu_core0_irq_handler(int irq, void *data)
{
    return rknpu_irq_handler(irq, data, 0);
}

static int rknpu_submit(struct rknpu_device *rknpu_dev,
                        struct rknpu_submit *args)
{
    struct rknpu_job *job = NULL;
    int ret = -EINVAL;

    if (args->task_number == 0)
    {
        LOG_ERROR("invalid rknpu task number!\n");
        return -EINVAL;
    }

    if (args->core_mask > rknpu_dev->config->core_mask)
    {
        LOG_ERROR("invalid rknpu core mask: 0x%x", args->core_mask);
        return -EINVAL;
    }

    job = rknpu_job_alloc(rknpu_dev, args);
    if (!job)
    {
        LOG_ERROR("failed to allocate rknpu job!\n");
        return -ENOMEM;
    }

    rknpu_job_schedule(job);
    if (args->flags & RKNPU_JOB_PC)
        job->ret = rknpu_job_wait(job);

    args->task_counter = job->args->task_counter;
    ret = job->ret;
    if (!ret)
        rknpu_job_cleanup(job);
    else
        rknpu_job_abort(job);

    return ret;
}

int rknpu_submit_ioctl(struct rknpu_device *rknpu_dev, unsigned long data)
{
    struct rknpu_submit *args = (struct rknpu_submit *)data;
    int ret = -EINVAL;

    ret = rknpu_submit(rknpu_dev, args);
    return ret;
}

int rknpu_get_hw_version(struct rknpu_device *rknpu_dev, uint32_t *version)
{
    uint32_t rknpu_core_base = rknpu_dev->base[0];

    if (version == NULL)
        return -EINVAL;

    *version = REG_READ(RKNPU_OFFSET_VERSION) + (REG_READ(RKNPU_OFFSET_VERSION_NUM) & 0xffff);

    return 0;
}

int rknpu_get_bw_priority(struct rknpu_device *rknpu_dev, uint32_t *priority,
                          uint32_t *expect, uint32_t *tw)
{
    uint32_t base = rknpu_dev->bw_priority_base;

    if (!rknpu_dev->config->bw_enable)
    {
        LOG_WARN("Get bw_priority is not supported on this device!\n");
        return 0;
    }

    if (!base)
        return -EINVAL;

    rt_mutex_take(&rknpu_dev->lock, RT_WAITING_FOREVER);

    if (priority != NULL)
        *priority = _REG_READ(base, 0x0);

    if (expect != NULL)
        *expect = _REG_READ(base, 0x8);

    if (tw != NULL)
        *tw = _REG_READ(base, 0xc);

    rt_mutex_release(&rknpu_dev->lock);

    return 0;
}

int rknpu_set_bw_priority(struct rknpu_device *rknpu_dev, uint32_t priority,
                          uint32_t expect, uint32_t tw)
{
    uint32_t base = rknpu_dev->bw_priority_base;

    if (!rknpu_dev->config->bw_enable)
    {
        LOG_WARN("Set bw_priority is not supported on this device!\n");
        return 0;
    }

    if (!base)
        return -EINVAL;

    rt_mutex_take(&rknpu_dev->lock, RT_WAITING_FOREVER);

    if (priority != 0)
        _REG_WRITE(base, priority, 0x0);

    if (expect != 0)
        _REG_WRITE(base, expect, 0x8);

    if (tw != 0)
        _REG_WRITE(base, tw, 0xc);

    rt_mutex_release(&rknpu_dev->lock);

    return 0;
}

int rknpu_clear_rw_amount(struct rknpu_device *rknpu_dev)
{
    uint32_t rknpu_core_base = rknpu_dev->base[0];
#ifdef RT_USING_SMP
    unsigned long flags;
#endif

    if (!rknpu_dev->config->bw_enable)
    {
        LOG_WARN("Clear rw_amount is not supported on this device!\n");
        return 0;
    }

    if (rknpu_dev->config->pc_dma_ctrl)
    {
        uint32_t pc_data_addr = 0;

#ifdef RT_USING_SMP
        flags = rt_spin_lock_irqsave(&rknpu_dev->irq_lock);
#endif
        pc_data_addr = REG_READ(RKNPU_OFFSET_PC_DATA_ADDR);

        REG_WRITE(0x1, RKNPU_OFFSET_PC_DATA_ADDR);
        REG_WRITE(0x80000101, RKNPU_OFFSET_CLR_ALL_RW_AMOUNT);
        REG_WRITE(0x00000101, RKNPU_OFFSET_CLR_ALL_RW_AMOUNT);
        // REG_WRITE(0x80000101, RKNPU_OFFSET_OTHER_CLR_ALL_RW_AMOUNT);
        // REG_WRITE(0x00000101, RKNPU_OFFSET_OTHER_CLR_ALL_RW_AMOUNT);
        REG_WRITE(pc_data_addr, RKNPU_OFFSET_PC_DATA_ADDR);
#ifdef RT_USING_SMP
        rt_spin_unlock_irqrestore(&rknpu_dev->irq_lock, flags);
#endif
    }
    else
    {
        rt_mutex_take(&rknpu_dev->lock, RT_WAITING_FOREVER);
        REG_WRITE(0x80000101, RKNPU_OFFSET_CLR_ALL_RW_AMOUNT);
        REG_WRITE(0x00000101, RKNPU_OFFSET_CLR_ALL_RW_AMOUNT);
        // REG_WRITE(0x80000101, RKNPU_OFFSET_OTHER_CLR_ALL_RW_AMOUNT);
        // REG_WRITE(0x00000101, RKNPU_OFFSET_OTHER_CLR_ALL_RW_AMOUNT);
        rt_mutex_release(&rknpu_dev->lock);
    }

    return 0;
}

int rknpu_get_rw_amount(struct rknpu_device *rknpu_dev, uint32_t *dt_wr,
                        uint32_t *dt_rd, uint32_t *wd_rd)
{
    uint32_t rknpu_core_base = rknpu_dev->base[0];
    int amount_scale = rknpu_dev->config->pc_data_amount_scale;

    if (!rknpu_dev->config->bw_enable)
    {
        LOG_WARN("Get rw_amount is not supported on this device!\n");
        return 0;
    }

    rt_mutex_take(&rknpu_dev->lock, RT_WAITING_FOREVER);

    if (dt_wr != NULL)
        *dt_wr = REG_READ(RKNPU_OFFSET_DT_WR_AMOUNT) * amount_scale;

    if (dt_rd != NULL)
        *dt_rd = REG_READ(RKNPU_OFFSET_DT_RD_AMOUNT) * amount_scale;

    if (wd_rd != NULL)
        *wd_rd = REG_READ(RKNPU_OFFSET_WT_RD_AMOUNT) * amount_scale;

    rt_mutex_release(&rknpu_dev->lock);

    return 0;
}

int rknpu_get_total_rw_amount(struct rknpu_device *rknpu_dev, uint32_t *amount)
{
    uint32_t dt_wr = 0;
    uint32_t dt_rd = 0;
    uint32_t wd_rd = 0;
    int ret = -EINVAL;

    if (!rknpu_dev->config->bw_enable)
    {
        LOG_WARN(
            "Get total_rw_amount is not supported on this device!\n");
        return 0;
    }

    ret = rknpu_get_rw_amount(rknpu_dev, &dt_wr, &dt_rd, &wd_rd);

    if (amount != NULL)
        *amount = dt_wr + dt_rd + wd_rd;

    return ret;
}
