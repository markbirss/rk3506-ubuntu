/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_flexbus.c
  * @author  Wesley Yao
  * @version V1.0
  * @date    19-July-2024
  * @brief   flexbus driver
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rthw.h>

#if defined(RT_USING_FLEXBUS)
#include <rtdbg.h>
#include <rtdef.h>
#include <rtthread.h>
#include "drv_clock.h"
#include "drv_flexbus.h"
#include "drv_pm.h"
#include "hal_base.h"
#include "hal_bsp.h"

struct flexbus_dev flexbus =
{
    .hal_dev = &g_flexbusDev,
    .fb0_data = RT_NULL,
    .fb1_data = RT_NULL,
    .fb0_isr = RT_NULL,
    .fb1_isr = RT_NULL,
};

static void flexbus_isr(int vector, void *param)
{
    rt_uint32_t isr;

    rt_interrupt_enter();

    isr = READ_REG(flexbus.hal_dev->pReg->ISR);

    if (flexbus.hal_dev->opMode0 != FLEXBUS0_OPMODE_NULL && flexbus.fb0_isr != RT_NULL)
    {
        flexbus.fb0_isr(&flexbus, isr);
    }
    if (flexbus.hal_dev->opMode1 != FLEXBUS1_OPMODE_NULL && flexbus.fb1_isr != RT_NULL)
    {
        flexbus.fb1_isr(&flexbus, isr);
    }

    rt_interrupt_leave();
}

int rockchip_rt_hw_flexbus_init(void)
{
    flexbus.parent.type = RT_Device_Class_Miscellaneous;
    flexbus.parent.user_data = &flexbus;

    rt_hw_interrupt_install(flexbus.hal_dev->irqNum, flexbus_isr, RT_NULL, RT_NULL);
    rt_hw_interrupt_umask(flexbus.hal_dev->irqNum);

    HAL_FLEXBUS_Init(&flexbus.instance, flexbus.hal_dev->pReg,
                     flexbus.hal_dev->opMode0, flexbus.hal_dev->opMode1);

    /* register flexbus device to RT-Thread */
    if (rt_device_register(&flexbus.parent, "flexbus", RT_DEVICE_OFLAG_OPEN) == RT_EOK)
    {
        LOG_D("%s success.\n", __func__);
    }
    else
    {
        LOG_E("%s failed! register failed!\n", __func__);
        return -RT_ERROR;
    }

    return RT_EOK;
}

INIT_PREV_EXPORT(rockchip_rt_hw_flexbus_init);

#endif
