/**
  * Copyright (c) 2024 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_remotectl.c
  * @version V0.1
  * @brief   remote control driver
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-02-25     Zhenke Fan     first implementation
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>
#include "hal_base.h"

#ifdef RT_USING_PWM_REMOTECTL
#include <string.h>
#include "drv_pwm_remotectl.h"
#include "hal_bsp.h"
#include "drv_clock.h"
#include "hal_base.h"

#define RK_REMOTE_DEBUG 0
#if RK_REMOTE_DEBUG
#define REMOTE_DBG(...)     rt_kprintf(__VA_ARGS__)
#else
#define REMOTE_DBG(...)
#endif

static rt_sem_t remotectl_sem = RT_NULL;
static struct rt_device_pwm *pwm_capture_dev;
static struct rt_remotectl_drvdata *remote_data;
struct rt_device *rt_remote;
static rt_timer_t remote_timer;
struct PWM_REG *reg;
rt_uint32_t timeout = RT_TICK_PER_SECOND * 130 / 1000;                  //130ms

RT_WEAK const struct remotectl_pwm_info rtl_pwm_info_table[] = {0};

static void pwm_remotectl_wakeup_init()
{
    WRITE_REG(reg->CLK_CTRL, RT_PWM_CLK);              //24MHz div to 1MHz
    WRITE_REG(reg->FILTER_CTRL, RT_PWM_FILTER_VAL);
    WRITE_REG(reg->PWRMATCH_ARBITER, (1 << rtl_pwm_info_table[0].channel));
    WRITE_REG(reg->PWRMATCH_VALUE[0], rtl_pwm_info_table[0].pwrmatch_value);
}

static void remotectl_init()
{
    pwm_capture_dev = (struct rt_device_pwm *)rt_device_find(rtl_pwm_info_table[0].name);

    if (pwm_capture_dev == RT_NULL)
    {
        rt_kprintf("%s failed! can't find %s device!\n", __func__, rtl_pwm_info_table[0].name);
        return;
    }

    rt_pwm_int_enable(pwm_capture_dev, rtl_pwm_info_table[0].channel);
    rt_pwm_set_capture(pwm_capture_dev, rtl_pwm_info_table[0].channel);
    pwm_remotectl_wakeup_init();

}

static void pwm_remotectl_timer(void *parameter)
{
    remote_data->key.press = 0;
    rt_sem_release(remotectl_sem);
    remote_data->count = 0;
    remote_data->state = RMC_PRELOAD;
}

static void remotectl_parse_pwm_data(uint32_t data)
{

    switch (remote_data->state)
    {
    case RMC_PRELOAD:
        if (data > RT_PWM_TIME_PRE_MIN_HIGH && data < RT_PWM_TIME_PRE_MAX_HIGH)
        {
            remote_data->count = 0;
            remote_data->state = RMC_USERCODE;
            remote_data->code = 0;
        }
        else
        {
            remote_data->count = 0;
            remote_data->state = RMC_PRELOAD;
        }
        break;
    case RMC_USERCODE:
        if (remote_data->count <= 8)
        {
            if (data > RT_PWM_TIME_BIT0_MIN && data < RT_PWM_TIME_BIT0_MAX)
            {
                remote_data->code += (1 << (remote_data->count - 1));
            }
            if (remote_data->count == 8)
            {
                remote_data->usercode = remote_data->code;
                remote_data->code = 0;
            }
        }
        else if (remote_data->count <= 16)
        {
            if (data > RT_PWM_TIME_BIT0_MIN && data < RT_PWM_TIME_BIT0_MAX)
            {
                remote_data->code += (1 << (remote_data->count - 9));
            }
        }

        if (remote_data->count == 16)
        {
            remote_data->usercode = (remote_data->usercode << 8) | remote_data->code;
            if (remote_data->usercode == rtl_pwm_info_table[0].rtl_usercode)
            {
                remote_data->count = 0;
                remote_data->state = RMC_GETDATA;
                remote_data->code = 0;
            }
            else
            {
                rt_kprintf("remotectl err usercode 0x%04x, expect 0x%04x\n", remote_data->usercode, rtl_pwm_info_table[0].rtl_usercode);
                remote_data->count = 0;
                remote_data->state = RMC_PRELOAD;
            }
        }
        break;
    case RMC_GETDATA:
        if (remote_data->count <= 8)
        {
            if (data > RT_PWM_TIME_BIT0_MIN && data < RT_PWM_TIME_BIT0_MAX)
            {
                remote_data->code += (1 << (remote_data->count - 1));
            }
            if (remote_data->count == 8)
            {
                remote_data->key.keycode = remote_data->code;
            }
        }
        else if (remote_data->count <= 16)
        {
            if (data > RT_PWM_TIME_BIT0_MIN && data < RT_PWM_TIME_BIT0_MAX)
            {
                remote_data->code += (1 << (remote_data->count - 9));
            }
        }

        if (remote_data->count == 16)
        {
            if (remote_data->code == 0xff)
            {
                REMOTE_DBG("remote ctl get ir keycode 0x%0x down\n", remote_data->key.keycode);
                remote_data->key.press = 1;
                rt_sem_release(remotectl_sem);
                remote_data->count = 0;
                remote_data->state = RMC_SEQUENCE;
                if (remote_timer)
                    rt_timer_start(remote_timer);
            }
            else
            {
                remote_data->count = 0;
                remote_data->state = RMC_PRELOAD;
            }
        }
        break;
    case RMC_SEQUENCE:
        if (remote_timer)
        {
            rt_timer_stop(remote_timer);
            rt_timer_start(remote_timer);
        }
        if (remote_data->count == 1)
        {
            if (!(data > RT_PWM_TIME_DATA_OVER_MIN &&
                    data < RT_PWM_TIME_DATA_OVER_MAX))
            {
                if (remote_timer)
                {
                    rt_timer_stop(remote_timer);
                }
                remote_data->key.press = 0;
                rt_sem_release(remotectl_sem);
                remote_data->count = 0;
                remote_data->state = RMC_PRELOAD;
            }
        }
        else if ((remote_data->count) % 2 == 0)
        {
            if (!(data > RK_PWM_TIME_RPT_MIN && data < RK_PWM_TIME_RPT_MAX))
            {
                if (remote_timer)
                {
                    rt_timer_stop(remote_timer);
                }
                remote_data->key.press = 0;
                rt_sem_release(remotectl_sem);
                remote_data->count = 0;
                remote_data->state = RMC_PRELOAD;
            }
        }
        else
        {
            if (!(data > RT_PWM_TIME_SEQ1_MIN && data < RT_PWM_TIME_SEQ1_MAX))
            {
                if (remote_timer)
                {
                    rt_timer_stop(remote_timer);
                }
                remote_data->key.press = 0;
                rt_sem_release(remotectl_sem);
                remote_data->count = 0;
                remote_data->state = RMC_PRELOAD;
            }
            else
            {
                REMOTE_DBG("remote ctl get ir repeat keycode %0x \n", remote_data->key.keycode);
                if (remote_data->key.press)
                    rt_sem_release(remotectl_sem);
            }
        }
        break;
    default:
        rt_kprintf("Unsupport remote_control state: 0x%lx\n", remote_data->state);
        break;
    }

}

void remotectl_get_pwm_period(const char *name, uint32_t ch, uint32_t freq, uint32_t low, uint32_t high)
{
    if (rt_strcmp(name, rtl_pwm_info_table[0].name) == 0 && (ch == rtl_pwm_info_table[0].channel))
    {
        remotectl_parse_pwm_data(high);
        remote_data->count++;
    }
}

static rt_err_t remotectl_control(rt_device_t dev, int cmd, void *args)
{
    switch (cmd)
    {
    case RT_REMOTECTL_INIT:
        remotectl_init();
        break;
    case RT_REMOTECTL_GET_INFO:
        if (rt_sem_take(remotectl_sem, RT_WAITING_FOREVER) == RT_EOK)
        {
            if (!remote_data->key.press)
                REMOTE_DBG("remote ctl get ir keycode 0x%0x up\n", remote_data->key.keycode);
            *(struct rt_remotectl_keycode *)args = remote_data->key;
            return RT_EOK;
        }
        break;
    default:
        rt_kprintf("Unsupport remote_control cmd: 0x%lx\n", cmd);
        break;
    }

    return 0;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops remotectl_ops =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    remotectl_control,
};
#endif

#if defined(RT_USING_PM)
static int rt_remotectl_pm_suspend(const struct rt_device *device, rt_uint8_t mode)
{
    REMOTE_DBG("%s enter\n", __FUNCTION__);
    WRITE_REG(reg->PWRMATCH_CTRL, RT_PWRMATCH_CTRL);
    return RT_EOK;
}

static void rt_remotectl_pm_resume(const struct rt_device *device, rt_uint8_t mode)
{
    uint32_t keycode;

    REMOTE_DBG("%s enter\n", __FUNCTION__);
    keycode = READ_REG(reg->PWRCAPTURE_VALUE);
    REMOTE_DBG("%s irq keycode 0x%x\n", __FUNCTION__, keycode);

    if (keycode == rtl_pwm_info_table[0].pwrmatch_value)
    {
        rt_pm_request(0);
        remote_data->key.press = 0;
        remote_data->key.keycode = (rtl_pwm_info_table[0].pwrmatch_value >> 16) & 0xff;
        rt_sem_release(remotectl_sem);
        WRITE_REG(reg->PWRMATCH_CTRL, RT_PWRMATCH_CTRL_DISABLE);
    }
    else
    {
        rt_pm_release(0);
    }
}

static struct rt_device_pm_ops rk_remotectl_pm_ops =
{
    .suspend = rt_remotectl_pm_suspend,
    .resume = rt_remotectl_pm_resume,
};
#endif

int rt_remotectl_init(void)
{
    uint32_t addr_base;

    remote_timer = rt_timer_create("remote_timer", pwm_remotectl_timer, RT_NULL, timeout, RT_TIMER_FLAG_ONE_SHOT);
    if (!remote_timer)
    {
        return RT_ERROR;
    }

    remotectl_sem = rt_sem_create("dsem", 0, RT_IPC_FLAG_PRIO);
    if (!remotectl_sem)
    {
        rt_timer_delete(remote_timer);
        return RT_ERROR;
    }

    remote_data = rt_malloc(sizeof(struct rt_remotectl_drvdata));
    if (!remote_data)
    {
        rt_timer_delete(remote_timer);
        rt_sem_delete(remotectl_sem);
        return -RT_ENOMEM;
    }
    memset(remote_data, 0x00, sizeof(struct rt_remotectl_drvdata));

    rt_remote = rt_malloc(sizeof(struct rt_device));
    if (!rt_remote)
    {
        rt_timer_delete(remote_timer);
        rt_sem_delete(remotectl_sem);
        rt_free(remote_data);
        return -RT_ENOMEM;
    }

    if (rt_strcmp("pwm0", rtl_pwm_info_table[0].name) == 0)
        addr_base = PWM0_BASE;
    else
        addr_base = PWM1_BASE;
    addr_base += (rtl_pwm_info_table[0].channel << 12);

    reg = ((struct PWM_REG *)addr_base);

    /* init rt_device structure */
    rt_remote->type = RT_Device_Class_Graphic;
#ifdef RT_USING_DEVICE_OPS
    rt_remote->ops = &remotectl_ops;
#else
    rt_remote->init = NULL;
    rt_remote->open = NULL;
    rt_remote->close = NULL;
    rt_remote->read = NULL;
    rt_remote->write = NULL;
    rt_remote->control = remotectl_control;
#endif

#if defined(RT_USING_PM)
    rt_pm_device_register(rt_remote, &rk_remotectl_pm_ops);
#endif

    /* register rt_remote device to RT-Thread */
    rt_device_register(rt_remote, "remotecontrol", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);

    return 0;
}

INIT_DEVICE_EXPORT(rt_remotectl_init);
#endif
