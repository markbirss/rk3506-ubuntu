/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtthread.h>
#include <rtdevice.h>

#ifdef RT_USING_AIP1640
#include "hal_base.h"
#include "../drv_led_tube.h"
#include "aip1640_display.h"
#include "aip1640_main.h"

struct rt_device *rt_aip1640;
struct aip1640_info *g_aip1640_info;

static rt_err_t aip1640_control(rt_device_t dev, int cmd, void *args)
{

    switch (cmd)
    {
    case RT_LED_SET_DISPLAY_INFO:
        rt_hw_aip1640_display_info(args);
        break;
    case RT_LED_SET_DISPLAY_DIMMER:
        rt_hw_aip1640_display_dimmer(args);
        break;
    case RT_LED_SET_PLAY_STATUS:
        rt_hw_aip1640_play_status(args);
        break;
    default:
        break;
    }

    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops aip1640_ops =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    aip1640_control,
};
#endif

int rt_hw_aip1640_init(void)
{
    g_aip1640_info = rt_malloc(sizeof(struct aip1640_info));
    if (!g_aip1640_info)
    {
        return -RT_ERROR;
    }
    memset(g_aip1640_info, 0, sizeof(struct aip1640_info));

    rt_aip1640 = rt_malloc(sizeof(struct rt_device));
    if (!rt_aip1640)
    {
        rt_free(g_aip1640_info);
        return -RT_ERROR;
    }

    /* init rt_device structure */
    rt_aip1640->type = RT_Device_Class_Graphic;
#ifdef RT_USING_DEVICE_OPS
    rt_aip1640->ops = &aip1640_ops;
#else
    rt_aip1640->init = NULL;
    rt_aip1640->open = NULL;
    rt_aip1640->close = NULL;
    rt_aip1640->read = NULL;
    rt_aip1640->write = NULL;
    rt_aip1640->control = aip1640_control;
#endif

    /* register rt_aip1640 device to RT-Thread */
    rt_device_register(rt_aip1640, RT_AIP1640_DEVICE, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    g_aip1640_info->dim_val = 0x8f;
    rt_hw_aip1640_display_info("HELLO");
    return 0;
}

INIT_DEVICE_EXPORT(rt_hw_aip1640_init);
#endif