/**
  * Copyright (c) 2023 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_saradc.c
  * @author  Simon Xue
  * @version V0.1
  * @date    21-Nov-2023
  * @brief   saradc driver
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rthw.h>

#if defined(RT_USING_MULTI_SARADC)
#include "hal_base.h"
#include <rtdef.h>
#include <rtthread.h>
#include <rtdbg.h>
#include "drv_clock.h"

struct rk_saradc
{
    struct SARADC_REG *reg;
    rt_uint32_t mode;
    const char *name;
    struct rt_event event;
    struct rt_adc_device dev;
    struct clk_gate *pclk_saradc;
    struct clk_gate *clk_saradc;
    int pclk_id;
    int clk_id;
    int irq;
};

#define DEFINE_ROCKCHIP_SARADC(ID)                          \
                                                            \
static struct rk_saradc saradc##ID =                        \
{                                                           \
    .reg = SARADC##ID,                                      \
    .mode = SARADC_INT_MOD,                                 \
    .name = "rk_adc"#ID,                                    \
    .pclk_id = PCLK_SARADC_CONTROL_GATE##ID,                \
    .clk_id = CLK_SARADC_GATE##ID,                          \
    .irq = SARADC##ID##_IRQn,                               \
};

#ifdef RT_USING_SARADC0
DEFINE_ROCKCHIP_SARADC(0)
#endif

#ifdef RT_USING_SARADC1
DEFINE_ROCKCHIP_SARADC(1)
#endif

static struct rk_saradc *rk_saradc_table[] =
{
#ifdef RT_USING_SARADC0
    &saradc0,
#endif
#ifdef RT_USING_SARADC1
    &saradc1,
#endif
    RT_NULL
};

static rt_err_t rk_wait_saradc_completed(struct rk_saradc *saradc)
{
    rt_uint32_t status;

    if (rt_event_recv(&saradc->event, 0xffffffff, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                      rt_tick_from_millisecond(1), &status) != RT_EOK)
    {
        LOG_E("wait completed timeout");
        return -RT_ETIMEOUT;
    }

    if (status != 0x1)
        return -RT_ERROR;

    return RT_EOK;
}
static rt_err_t rk_get_saradc_value(struct rt_adc_device *device, rt_uint32_t channel, rt_uint32_t *value)
{
    int ret;

    struct rk_saradc *saradc = device->parent.user_data;

    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(value != RT_NULL);

    clk_enable(saradc->pclk_saradc);
    clk_enable(saradc->clk_saradc);

    HAL_SARADC_Start(saradc->reg, saradc->mode, channel);

    ret = rk_wait_saradc_completed(saradc);

    if (ret != RT_EOK)
    {
        LOG_D("%s :  failed to read adc data\n", saradc->name);
        return -RT_ERROR;
    }

    *value = (rt_uint32_t)HAL_SARADC_GetRaw(saradc->reg, channel);

    HAL_SARADC_Stop(saradc->reg);

    clk_disable(saradc->clk_saradc);
    clk_disable(saradc->pclk_saradc);

    return RT_EOK;
}

static void rk_saradc_irq_handler(int vector, void *param)
{
    struct rk_saradc *saradc = (struct rk_saradc *)param;

    rt_interrupt_enter();

    HAL_SARADC_IrqHandler(saradc->reg);

    rt_event_send(&saradc->event, 0x1);

    rt_interrupt_leave();
}

static const struct rt_adc_ops rk_saradc_ops =
{
    .convert = rk_get_saradc_value,
};

static int rk_saradc_init(void)
{
    int result = RT_EOK;
    struct rk_saradc **saradc;

    for (saradc = rk_saradc_table; *saradc != RT_NULL; saradc++)
    {
#ifdef PCLK_SARADC_CONTROL_GATE_MULTI
        (*saradc)->pclk_saradc = get_clk_gate_from_id((*saradc)->pclk_id);
#endif
        (*saradc)->clk_saradc = get_clk_gate_from_id((*saradc)->clk_id);

        rt_event_init(&(*saradc)->event, "saradc", RT_IPC_FLAG_FIFO);
        rt_hw_interrupt_install((*saradc)->irq, rk_saradc_irq_handler, *saradc, (*saradc)->name);
        rt_hw_interrupt_umask((*saradc)->irq);

        if (rt_hw_adc_register(&(*saradc)->dev, (*saradc)->name, &rk_saradc_ops, *saradc) == RT_EOK)
        {
            LOG_D("%s init success", (*saradc)->name);
        }
        else
        {
            LOG_E("%s register failed", (*saradc)->name);
            result = -RT_ERROR;
        }
    }

    return result;
}

INIT_DEVICE_EXPORT(rk_saradc_init);

#endif
