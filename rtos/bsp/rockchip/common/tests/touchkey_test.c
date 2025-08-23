/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    touchkey_test.c
  * @author  Simon Xue
  * @version V0.1
  * @date    28-Feb-2024
  * @brief   touchkey test
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_COMMON_TEST_TOUCHKEY
#include "hal_base.h"
#include "drv_touchkey.h"

static rt_err_t touchkey_getchar_rx_ind(rt_device_t dev, rt_size_t size)
{
    uint8_t ch;
    rt_size_t i;
    rt_err_t ret;

    for (i = 0; i < size; i++)
    {
        /* read a char */
        if (rt_device_read(dev, 0, &ch, 1))
        {
            rt_kprintf("dev : %s read 0x%x\n", dev->parent.name, ch);
        }
        else
        {
            ret = rt_get_errno();
            rt_kprintf("dev : %s read error %d\n", dev->parent.name, ret);
        }
    }

    return RT_EOK;
}

static int touchkey_test(int argc, char **argv)
{
    static rt_device_t touchkey_dev = RT_NULL;
    rt_err_t result;
    uint8_t ch;

    if (argc > 1)
    {
        if (!strcmp(argv[1], "probe"))
        {
            touchkey_dev = rt_device_find(argv[2]);
            RT_ASSERT(touchkey_dev != RT_NULL);

            result = rt_device_init(touchkey_dev);
            if (result != RT_EOK)
            {
                rt_kprintf("To initialize device:%s failed. The error code is %d\n",
                           touchkey_dev->parent.name, result);
                return result;
            }

            result = rt_device_open(touchkey_dev, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
            if (result != RT_EOK)
            {
                rt_kprintf("To open device:%s failed. The error code is %d\n",
                           touchkey_dev->parent.name, result);
                return result;
            }
        }
        else
        {
            if (touchkey_dev == RT_NULL)
            {
                rt_kprintf("Please using 'touchkey_test probe <touchkey_name>' first\n");
                return -RT_ERROR;
            }

            if (!strcmp(argv[1], "read"))
            {
                if (argc == 2)
                {
                    result = rt_device_read(touchkey_dev, 0, &ch, 1);
                    if (result != 1)
                    {
                        rt_kprintf("read device :%s failed, error: %d, result = %d\n",
                                   touchkey_dev->parent.name, rt_get_errno(), result);
                        return -RT_ERROR;
                    }
                    rt_kprintf("device: %s read value is 0x%x\n",
                               touchkey_dev->parent.name, ch);
                }
                else
                {
                    rt_kprintf("touchkey     - read touchkey value\n");
                }
            }
            else if (!strcmp(argv[1], "set_ind"))
            {
                if (!strcmp(argv[2], "null"))
                {
                    rt_device_set_rx_indicate(touchkey_dev, NULL);
                }
                else
                {
                    rt_device_set_rx_indicate(touchkey_dev, touchkey_getchar_rx_ind);
                }
            }
            else if (!strcmp(argv[1], "close"))
            {
                rt_device_set_rx_indicate(touchkey_dev, NULL);
                rt_device_close(touchkey_dev);
                touchkey_dev = NULL;
            }
            else
            {
                rt_kprintf("Unknown command. Please enter 'touchkey_test' for help\n");
            }
        }
    }
    else
    {
        rt_kprintf("Usage: \n");
        rt_kprintf("touchkey_test probe <touchkey_name>   - probe touchkey by name\n");
        rt_kprintf("touchkey_test read     - read touchkey value\n");
        rt_kprintf("touchkey_test close     - close test, need probe to restart test\n");
        rt_kprintf("touchkey_test set_ind callback     - set touchkey indicate callback, set null to cancel\n");
        return -RT_ERROR;
    }
    return RT_EOK;
}

static rt_timer_t timer;
static struct rt_event event;

void timer_func(void *parameter)
{
    rt_err_t result;
    char read_buffer = 0;
    rt_device_t touchkey_dev = (rt_device_t)parameter;

    result = rt_device_read(touchkey_dev, 0, &read_buffer, 1);
    if (result == RT_EOK)
    {
        if (read_buffer != 0)
        {
            rt_kprintf("touchkey read value is 0x%x\n", read_buffer);
            rt_timer_stop(timer);
            rt_event_send(&event, 0x1);
        }
    }
}

int _at_touchkey_test(void)
{
    static rt_device_t touchkey_dev = RT_NULL;
    rt_err_t result;
    rt_uint32_t status;

    touchkey_dev = rt_device_find("touchkey");
    RT_ASSERT(touchkey_dev != RT_NULL);

    result = rt_device_init(touchkey_dev);
    if (result != RT_EOK)
        return -RT_ERROR;

    result = rt_device_open(touchkey_dev, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    if (result != RT_EOK)
        return -RT_ERROR;

    if (!timer)
    {
        timer = rt_timer_create("touchkey_read", timer_func, touchkey_dev,
                                10, RT_TIMER_FLAG_PERIODIC);
        if (!timer)
            return RT_ERROR;

        rt_timer_start(timer);
    }

    rt_kprintf("start touchkey auto test : \n");

    /* TODO : simulate a touchkey event */

    /* wait touchkey 2 seconds to trigger interrupt */
    if (rt_event_recv(&event, 0xffffffff, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                      rt_tick_from_millisecond(2000), &status) != RT_EOK)
    {
        rt_kprintf("wait touchkey timeout");
        rt_timer_stop(timer);
        return -RT_ETIMEOUT;
    }

    if (status != 0x1)
        return -RT_ERROR;

    return RT_EOK;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(touchkey_test, touchkey drive test. e.g: touchkey_test);
FINSH_FUNCTION_EXPORT(_at_touchkey_test, touchkey test for auto test);
#endif

#endif
