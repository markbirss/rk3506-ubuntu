/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * @file    rknpu_drv.c
 * @author  randall zhuo
 * @version V0.1
 * @date    23-Jan-2024
 * @brief   rknpu driver
 *
 ******************************************************************************
 */

#include <rtdevice.h>
#include <rthw.h>
#include <rtthread.h>
#include "hal_base.h"

#include <soc.h>

#include "rknpu_drv.h"
#include "rknpu_job.h"

static void rt_rknpu_irq_handler(int vector, void *param);

static const struct rknpu_irqs_data rknpu_irqs[] =
{
    {"npu_irq", rt_rknpu_irq_handler}
};

static const struct rknpu_config rk2118_rknpu_config =
{
    .bw_priority_addr = 0x0,
    .bw_priority_length = 0x0,
    .dma_mask = 0, // TODO
    .pc_data_amount_scale = 1,
    .pc_task_number_bits = 16,
    .pc_task_number_mask = 0xffff,
    .pc_task_status_offset = 0x48,
    .pc_dma_ctrl = 1,
    .bw_enable = 1,
    .irqs = rknpu_irqs,
    // .resets = rknpu_resets,
    .num_irqs = ARRAY_SIZE(rknpu_irqs),
    // .num_resets = ARRAY_SIZE(rknpu_resets),
    .nbuf_phyaddr = 0,
    .nbuf_size = 0,
    .max_submit_number = (1 << 16) - 1,
    .core_mask = 0x1,
};

static struct rknpu_device g_rknpu_dev;

static void rt_rknpu_irq_handler(int vector, void *param)
{
    rt_interrupt_enter();
    rknpu_core0_irq_handler(vector, &g_rknpu_dev);
    rt_interrupt_leave();
}

/**
 * @brief  Open rknpu device.
 * @param  dev: rt_device_t for rknpu.
 * @oflag  oflag: open flag.
 * return rt_err_t.
 */
static rt_err_t rknpu_open(rt_device_t dev, rt_uint16_t oflag)
{
    // rt_kprintf("Enter rknpu_open\n");
    return RT_EOK;
}

/**
 * @brief  Close rknpu device.
 * @param  dev: rt_device_t for rknpu.
 * return rt_err_t.
 */
static rt_err_t rknpu_close(rt_device_t dev)
{
    // rt_kprintf("Enter rknpu_close\n");
    return RT_EOK;
}

static int rknpu_get_drv_version(uint32_t *version)
{
    *version = RKNPU_GET_DRV_VERSION_CODE(DRIVER_MAJOR, DRIVER_MINOR, DRIVER_PATCHLEVEL);
    return 0;
}

static int rknpu_soft_reset(struct rknpu_device *rknpu_dev)
{
    return RT_EOK;
}

static int rknpu_action(struct rknpu_device *rknpu_dev,
                        struct rknpu_action *args)
{
    int ret = -EINVAL;

    // rt_kprintf("Enter rknpu_action, cmd=%d\n", args->flags);

    switch (args->flags)
    {
    case RKNPU_GET_HW_VERSION:
        ret = rknpu_get_hw_version(rknpu_dev, (uint32_t *)&args->value);
        break;
    case RKNPU_GET_DRV_VERSION:
        ret = rknpu_get_drv_version((uint32_t *)&args->value);
        break;
    case RKNPU_GET_FREQ:
        args->value = 0;
        ret = 0;
        break;
    case RKNPU_SET_FREQ:
        break;
    case RKNPU_GET_VOLT:
        args->value = 0;
        ret = 0;
        break;
    case RKNPU_SET_VOLT:
        break;
    case RKNPU_ACT_RESET:
        ret = rknpu_soft_reset(rknpu_dev);
        break;
    case RKNPU_GET_BW_PRIORITY:
        ret = rknpu_get_bw_priority(rknpu_dev, (uint32_t *)&args->value, NULL, NULL);
        break;
    case RKNPU_SET_BW_PRIORITY:
        ret = rknpu_set_bw_priority(rknpu_dev, args->value, 0, 0);
        break;
    case RKNPU_GET_BW_EXPECT:
        ret = rknpu_get_bw_priority(rknpu_dev, NULL, (uint32_t *)&args->value, NULL);
        break;
    case RKNPU_SET_BW_EXPECT:
        ret = rknpu_set_bw_priority(rknpu_dev, 0, args->value, 0);
        break;
    case RKNPU_GET_BW_TW:
        ret = rknpu_get_bw_priority(rknpu_dev, NULL, NULL, (uint32_t *)&args->value);
        break;
    case RKNPU_SET_BW_TW:
        ret = rknpu_set_bw_priority(rknpu_dev, 0, 0, args->value);
        break;
    case RKNPU_ACT_CLR_TOTAL_RW_AMOUNT:
        ret = rknpu_clear_rw_amount(rknpu_dev);
        break;
    case RKNPU_GET_DT_WR_AMOUNT:
        ret = rknpu_get_rw_amount(rknpu_dev, (uint32_t *)&args->value, NULL, NULL);
        break;
    case RKNPU_GET_DT_RD_AMOUNT:
        ret = rknpu_get_rw_amount(rknpu_dev, NULL, (uint32_t *)&args->value, NULL);
        break;
    case RKNPU_GET_WT_RD_AMOUNT:
        ret = rknpu_get_rw_amount(rknpu_dev, NULL, NULL, (uint32_t *)&args->value);
        break;
    case RKNPU_GET_TOTAL_RW_AMOUNT:
        ret = rknpu_get_total_rw_amount(rknpu_dev, (uint32_t *)&args->value);
        break;
    case RKNPU_GET_IOMMU_EN:
        args->value = rknpu_dev->iommu_en;
        ret = 0;
        break;
    case RKNPU_SET_PROC_NICE:
        ret = 0;
        break;
    case RKNPU_GET_TOTAL_SRAM_SIZE:
        args->value = 0;
        ret = 0;
        break;
    case RKNPU_GET_FREE_SRAM_SIZE:
        args->value = 0;
        ret = 0;
        break;
    case RKNPU_POWER_ON:
        ret = 0;
        break;
    case RKNPU_POWER_OFF:
        ret = 0;
        break;
    default:
        ret = -EINVAL;
        break;
    }

    return ret;
}

/**
 * @brief  Control rknpu device.
 * @param  dev: rt_device_t for rknpu.
 * @param  cmd: control command.
 * @param  args: args between driver and userspace.
 * return rt_err_t.
 */
static rt_err_t rknpu_control(rt_device_t dev, int cmd, void *args)
{
    struct rknpu_device *rknpu_dev = (struct rknpu_device *)dev->user_data;
    rt_err_t ret = RT_EOK;

    // rt_kprintf("Enter rknpu_control, cmd=%d\n", cmd);

    switch (cmd)
    {
    case IOCTL_RKNPU_ACTION:
        ret = rknpu_action(rknpu_dev, (struct rknpu_action *)args);
        break;
    case IOCTL_RKNPU_SUBMIT:
        ret = rknpu_submit_ioctl(rknpu_dev, (unsigned long)args);
        break;
    case IOCTL_RKNPU_MEM_CREATE:
        break;
    case RKNPU_MEM_MAP:
        break;
    case IOCTL_RKNPU_MEM_DESTROY:
        break;
    case IOCTL_RKNPU_MEM_SYNC:
        break;
    default:
        break;
    }

    return ret;
}

static int rknpu_init_reg(struct rknpu_device *rknpu_dev)
{
    const struct rknpu_config *config = rknpu_dev->config;

    rknpu_dev->base[0] = RKNPU_CORE_BASE_ADDR;
    rknpu_dev->reset_addr = RKNPU_RESET_ADDR;

    if (config->bw_priority_length > 0)
    {
        rknpu_dev->bw_priority_base = config->bw_priority_addr;
    }

    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
static const struct rt_device_ops rknpu_ops =
{
    .open = rknpu_open,
    .close = rknpu_close,
    .control = rknpu_control,
};
#endif

static int rt_hw_rknpu_init(void)
{
    int result = RT_EOK;
    struct rknpu_device *rknpu_dev = &g_rknpu_dev;

    rknpu_dev->config = &rk2118_rknpu_config;
    rknpu_dev->iommu_en = 0;

    rknpu_init_reg(rknpu_dev);

    if (rt_mutex_init(&(rknpu_dev->lock), "rknpu_lock", RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        RT_ASSERT(0);
    }

#ifdef RT_USING_SMP
    rt_spin_lock_init(&rknpu_dev->irq_lock);
#endif

    for (int i = 0; i < rknpu_dev->config->num_irqs; i++)
    {
        rt_list_init(&rknpu_dev->subcore_datas[i].todo_list);
        result = rt_event_init(&rknpu_dev->subcore_datas[i].job_done_wq, "rknpu_job_done", RT_IPC_FLAG_FIFO);

        if (result != RT_EOK)
        {
            LOG_ERROR("rknpu init rknpu_job_done event failed");
            RT_ASSERT(0);
        }

        rknpu_dev->subcore_datas[i].task_num = 0;
        rknpu_dev->subcore_datas[i].job = NULL;
    }

    rknpu_dev->soft_reseting = 0;

    rknpu_dev->dev.type = RT_Device_Class_Miscellaneous;
#ifdef RT_USING_DEVICE_OPS
    rknpu_dev->dev.ops = &rknpu_ops;
#else
    rknpu_dev->dev.init = RT_NULL;
    rknpu_dev->dev.open = rknpu_open;
    rknpu_dev->dev.close = rknpu_close;
    rknpu_dev->dev.read = RT_NULL;
    rknpu_dev->dev.write = RT_NULL;
    rknpu_dev->dev.control = rknpu_control;
#endif
    rknpu_dev->dev.user_data = rknpu_dev;

    result = rt_device_register(&rknpu_dev->dev, "rknpu", RT_DEVICE_FLAG_RDWR);

    HAL_RKNPU_Init(&rknpu_dev->hal_rknpu);
    HAL_RKNPU_PowerOn(&rknpu_dev->hal_rknpu);

    // install irq
    rt_hw_interrupt_install(NPU_IRQn, rknpu_dev->config->irqs->irq_hdl, rknpu_dev, rknpu_dev->config->irqs->name);
    rt_hw_interrupt_umask(NPU_IRQn);

    return result;
}

INIT_DEVICE_EXPORT(rt_hw_rknpu_init);