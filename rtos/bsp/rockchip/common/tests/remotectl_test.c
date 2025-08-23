/**
  * Copyright (c) 2024 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    remotectl_test.c
  * @version V0.1
  * @brief   remote control driver
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-03-21     Zhenke Fan     first implementation
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_PWM_REMOTECTL
#ifdef RT_USING_PWM_REMOTECTL_TEST
#include "drv_pwm_remotectl.h"

#define RT_REMOTE_DEVICE          "remotecontrol"

static struct rt_device *remote_dev;
struct rt_remotectl_keycode key;
static rt_timer_t ir_timer;
int ir_timeout = 3 * 1000;

static void remote_device_init(void)
{
    remote_dev = rt_device_find(RT_REMOTE_DEVICE);
    if (remote_dev == NULL)
    {
        rt_kprintf("Failed to find device %s\n", RT_REMOTE_DEVICE);
        return;
    }
    else
    {
        rt_kprintf("find device %s\n", RT_REMOTE_DEVICE);
    }
    rt_device_control(remote_dev, RT_REMOTECTL_INIT, NULL);

}

void enter_long_key_mode()
{
    rt_kprintf("%s\n", __func__);
}

void remote_control(void *args)
{
    int ret;
    int count = 0;

    remote_device_init();
    while (1)
    {
        ret = rt_device_control(remote_dev, RT_REMOTECTL_GET_INFO, (void *)&key);
        if (ret == RT_EOK)
        {
            if (key.press == 1)
                count++;
            if (count == 1 && key.press == 1)
                rt_kprintf("remotectl get ir keycode 0x%0x down\n", key.keycode);
            if (key.keycode == 0xcb && key.press == 1)                    //play key long press
            {
                if (count == 1)
                {
                    ir_timer = rt_timer_create("ir_timer", enter_long_key_mode, RT_NULL, ir_timeout, RT_TIMER_FLAG_ONE_SHOT);
                    rt_timer_start(ir_timer);
                }
            }
            else if (key.press == 0)
            {
                if (ir_timer && key.keycode == 0xcb)
                {
                    rt_timer_delete(ir_timer);
                }
                rt_kprintf("remotectl get ir keycode 0x%0x up\n", key.keycode);
                count = 0;
            }
        }
        rt_thread_mdelay(30);
    }

}

int remote_control_test(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("remotectl", remote_control, NULL, 512, 20, 10);
    if (tid)
        rt_thread_startup(tid);

    return RT_EOK;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(remote_control_test, remote control test);
#endif

#endif
#endif
