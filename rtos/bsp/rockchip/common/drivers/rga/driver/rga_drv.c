/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * @file    rga_drv.c
 * @author  cerf yu
 * @version V0.1
 * @date    23-Jul-2024
 * @brief   rga driver
 *
 ******************************************************************************
 */

#include <rtdevice.h>
#include <rtthread.h>
#include <rthw.h>

#include <soc.h>

#include <stdio.h>

#include "drv_clock.h"

#include "rga_drv.h"
#include "rga_job.h"
#include "rga_hw_config.h"
#include "rga_debugger.h"
#include "rga_common.h"

#define RGA_DRIVER_NAME "rga"

struct rga_match_data
{
    enum RGA_DEVICE_TYPE device_type;

    const struct rga_backend_ops *ops;

    const char *irq_name;
    IRQn_Type irq_num;
};

extern const struct rga_backend_ops rga2_ops;

struct rga_drvdata *g_rga_drv_data;

static const struct rga_match_data rga2_match_data =
{
    .device_type = RGA_DEVICE_RGA2,
    .ops = &rga2_ops,
    .irq_name = "rga_irq",
    .irq_num = RGA_IRQn,
};

static int rga_session_deinit(struct rga_session *session)
{
    kfree(session);

    return 0;
}

static struct rga_session *rga_session_init(void)
{
    rt_thread_t current_thread;
    struct rga_session *session;

    session = kzalloc(sizeof(*session), GFP_KERNEL);
    if (!session)
    {
        pr_err("rga_session alloc failed\n");
        return ERR_PTR(-ENOMEM);
    }

    //TODO: register to session_manager

    current_thread = rt_thread_self();
    // session->tgid = current_thread->tid;
    session->pname = current_thread->name;

    return session;
}

static rt_err_t rga_ioctl(rt_device_t dev, int cmd, void *arg)
{
    int ret = 0;
    int i;
    struct rga_version_t driver_version;
    struct rga_hw_versions_t hw_versions;
    struct rga_drvdata *data = g_rga_drv_data;
    struct rga_session *session;

    if (!data)
    {
        pr_err("rga_drvdata is null, rga is not init\n");
        return -ENODEV;
    }

    if (arg == NULL)
    {
        pr_err("%s arg is null!\n", __func__);
        return -EFAULT;
    }

    if (DEBUGGER_EN(NONUSE))
        return 0;

    mutex_lock(&data->lock);

    session = rga_session_init();
    if (IS_ERR(session))
        return PTR_ERR(session);

    switch (cmd)
    {
    case RGA_BLIT_SYNC:
        ret = rga_job_commit(arg, session);

        break;

    case RGA_IOC_GET_HW_VERSION:
        /* RGA hardware version */
        hw_versions.size = data->num_of_scheduler > RGA_HW_SIZE ?
                           RGA_HW_SIZE : data->num_of_scheduler;

        for (i = 0; i < hw_versions.size; i++)
        {
            rt_memcpy(&hw_versions.version[i], &data->scheduler[i]->version,
                      sizeof(data->scheduler[i]->version));
        }

        if (copy_to_user((void *)arg, &hw_versions, sizeof(hw_versions)))
            ret = -EFAULT;
        else
            ret = true;

        break;

    case RGA_IOC_GET_DRVIER_VERSION:
        /* Driver version */
        driver_version.major = DRIVER_MAJOR_VERISON;
        driver_version.minor = DRIVER_MINOR_VERSION;
        driver_version.revision = DRIVER_REVISION_VERSION;
        rt_strncpy((char *)driver_version.str, DRIVER_VERSION, sizeof(driver_version.str));

        if (copy_to_user((void *)arg, &driver_version, sizeof(driver_version)))
            ret = -EFAULT;
        else
            ret = true;

        break;

    default:
        pr_err("unsupport cmd %#x\n", cmd);
        ret = -EINVAL;
        break;
    }

    rga_session_deinit(session);

    mutex_unlock(&data->lock);

    return ret;
}

static rt_err_t rga_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t rga_release(rt_device_t dev)
{
    return RT_EOK;
}

static void rt_rga_irq_handler(int vector, void *param)
{
    struct rga_scheduler *scheduler = (struct rga_scheduler *)param;
    struct rga_job *job;
    int irq_ret = 0;

    rt_interrupt_enter();

    if (scheduler->ops->irq)
        irq_ret = scheduler->ops->irq(scheduler);
    if (irq_ret != IRQ_WAKE_THREAD)
    {
        pr_err("irq error!\n");
        return;
    }

    job = rga_job_done(scheduler);
    if (job == NULL)
    {
        pr_err("isr thread invalid job!\n");
        return;
    }

    if (scheduler->ops->isr_thread)
        scheduler->ops->isr_thread(job, scheduler);

    rt_interrupt_leave();
}

#ifdef RT_USING_DEVICE_OPS
static const struct rt_device_ops rga_ops =
{
    .open = rga_open,
    .close = rga_release,
    .control = rga_ioctl,
};
#endif

static struct rt_device *rga_device_register(const char *name, void *data, rt_uint32_t flag)
{
    rt_uint32_t ret;
    struct rt_device *dev;

    dev = (struct rt_device *)rt_malloc(sizeof(*dev));
    if (dev == NULL)
    {
        pr_err("failed to allocate device.\n");
        return NULL;
    }
    rt_memset(dev, 0, sizeof(*dev));

    dev->type = RT_Device_Class_Miscellaneous;
    dev->rx_indicate = RT_NULL;
    dev->tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    dev->ops = &rga_ops;
#else
    dev->init = RT_NULL;
    dev->open = rga_open;
    dev->close = rga_release;
    dev->read = RT_NULL;
    dev->write = RT_NULL;
    dev->control = rga_ioctl;
#endif

    dev->user_data = data;

    ret = rt_device_register(dev, name, flag);
    if (ret < 0)
    {
        pr_err("failed to register %s device\n", name);
        rt_free(dev);

        return NULL;
    }

    return dev;
}

static int rga_device_unregister(struct rt_device *dev)
{
    rt_device_unregister(dev);

    rt_free(dev);

    return 0;
}

static int rga_scheduler_init(struct rga_scheduler *scheduler,
                              const struct rga_match_data *match_data,
                              struct rga_drvdata *drv_data)
{
    switch (match_data->device_type)
    {
    case RGA_DEVICE_RGA2:
        switch (drv_data->device_count[match_data->device_type])
        {
        case 0:
            scheduler->core = RGA2_SCHEDULER_CORE0;
            break;
        case 1:
            scheduler->core = RGA2_SCHEDULER_CORE1;
            break;
        default:
            pr_err("scheduler failed to match RGA2\n");
            return -EINVAL;
        }

        break;
    case RGA_DEVICE_RGA3:
        switch (drv_data->device_count[match_data->device_type])
        {
        case 0:
            scheduler->core = RGA3_SCHEDULER_CORE0;
            break;
        case 1:
            scheduler->core = RGA3_SCHEDULER_CORE1;
            break;
        default:
            pr_err("scheduler failed to match RGA2\n");
            return -EINVAL;
        }

        break;
    default:

        return -EINVAL;
    }

    scheduler->ops = match_data->ops;

    spin_lock_init(&scheduler->irq_lock);
    // INIT_LIST_HEAD(&scheduler->todo_list);
    init_waitqueue_head(&scheduler->job_done_wq);

    return 0;
}

static int rga_scheduler_probe(struct rga_drvdata *drv_data, const struct rga_match_data *match_data)
{
    rt_uint32_t ret;
    struct rga_scheduler *scheduler;

    scheduler = (struct rga_scheduler *)rt_malloc(sizeof(*scheduler));
    if (scheduler == NULL)
    {
        pr_err("failed to alloc scheduler.\n");
        return -ENOMEM;
    }
    rt_memset(scheduler, 0, sizeof(*scheduler));

    ret = rga_scheduler_init(scheduler, match_data, drv_data);
    if (ret < 0)
    {
        pr_err("failed to init scheduler\n");
        goto free_scheduler;
    }

    /* get hw resouce */
    scheduler->rga_base = (void __iomem *)RGA2_BASE;

    /* get IRQ */
    rt_hw_interrupt_install(match_data->irq_num, rt_rga_irq_handler, scheduler, match_data->irq_name);
    rt_hw_interrupt_umask(match_data->irq_num);

    /* get clk */
    // scheduler->aclk = get_clk_gate_from_id(ACLK_RGA_GATE);
    // scheduler->hclk = get_clk_gate_from_id(HCLK_RGA_GATE);
    // scheduler->core_clk = get_clk_gate_from_id(CLK_CORE_RGA_GATE);

    //TODO: PM

    scheduler->ops->get_version(scheduler);
    if (scheduler->core == RGA3_SCHEDULER_CORE0 ||
            scheduler->core == RGA3_SCHEDULER_CORE1)
    {
        // scheduler->data = &rga3_data;
    }
    else if (scheduler->core == RGA2_SCHEDULER_CORE0 ||
             scheduler->core == RGA2_SCHEDULER_CORE1)
    {
        if (!strcmp((const char *) scheduler->version.str, "3.3.87975"))
        {
            scheduler->data = &rga2e_1106_data;
        }
        else if (!strcmp((const char *) scheduler->version.str, "3.6.92812") ||
                 !strcmp((const char *) scheduler->version.str, "3.7.93215"))
        {
            scheduler->data = &rga2e_iommu_data;
        }
        else if (!strcmp((const char *) scheduler->version.str, "3.a.07135"))
        {
            scheduler->data = &rga2e_3506_data;
        }
        else if (!strcmp((const char *) scheduler->version.str, "3.e.19357"))
        {
            scheduler->data = &rga2p_iommu_data;
            rga_hw_set_issue_mask(scheduler, RGA_HW_ISSUE_DIS_AUTO_RST);
        }
        else if (!strcmp((const char *) scheduler->version.str, "3.f.23690"))
        {
            scheduler->data = &rga2p_lite_1103b_data;
        }
        else
        {
            scheduler->data = &rga2e_data;
        }
    }

    drv_data->scheduler[drv_data->num_of_scheduler] = scheduler;
    drv_data->num_of_scheduler++;
    drv_data->device_count[match_data->device_type]++;

    pr_info("%s probe successfully, irq = %d, hw_version:%s\n",
            rga_get_core_name(scheduler->core),
            match_data->irq_num, scheduler->version.str);

    return 0;

free_scheduler:
    rt_free(scheduler);

    return ret;
}

static int rga_scheduler_remove(struct rga_drvdata *drv_data, struct rga_scheduler *scheduler)
{
    rt_free(scheduler);

    return 0;
}

int rt_hw_rga_init(void)
{
    int i, ret;
    struct rt_device *dev;
    struct rga_drvdata *data;

    data = (struct rga_drvdata *)rt_malloc(sizeof(*data));
    if (data == NULL)
    {
        pr_err("failed to allocate driver data.\n");
        return -ENOMEM;
    }
    rt_memset(data, 0x0, sizeof(*data));

    mutex_init(&data->lock);

    /* probe RGA hardware */
    ret = rga_scheduler_probe(data, &rga2_match_data);
    if (ret < 0)
    {
        pr_err("rga2_driver register failed (%d).\n", ret);
        goto err_free_drvdata;
    }

    /* register RGA device */
    dev = rga_device_register(RGA_DRIVER_NAME, data, RT_DEVICE_FLAG_RDWR);
    if (dev == NULL)
    {
        pr_err("failed to register %s device\n", RGA_DRIVER_NAME);
        goto err_remove_scheduler;
    }

    //TODO: init reqeust/session... manager

    g_rga_drv_data = data;

    pr_info("%s module initialized. v%s\n", DRIVER_NAME, DRIVER_VERSION);

    return 0;

err_remove_scheduler:
    for (i = 0; i < data->num_of_scheduler; i++)
    {
        rga_scheduler_remove(data, data->scheduler[i]);
        data->scheduler[i] = NULL;
    }

    data->num_of_scheduler = 0;

err_free_drvdata:
    rt_free(data);

    return ret;
}

void rt_hw_rga_exit(void)
{
    int i;
    struct rga_drvdata *data = g_rga_drv_data;

    if (data == NULL)
        return;

    rga_device_unregister(data->dev);

    for (i = 0; i < data->num_of_scheduler; i++)
    {
        rga_scheduler_remove(data, data->scheduler[i]);
        data->scheduler[i] = NULL;
    }

    data->num_of_scheduler = 0;

    rt_free(g_rga_drv_data);
    g_rga_drv_data = NULL;

    pr_info("Module exited. v%s\n", DRIVER_VERSION);
}

INIT_DEVICE_EXPORT(rt_hw_rga_init);
