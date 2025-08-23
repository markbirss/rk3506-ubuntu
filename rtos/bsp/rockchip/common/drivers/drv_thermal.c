/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_thermal.c
  * @version V0.1
  * @brief   thermal interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-03-11     Ye.Zhang        first implementation
  *
  ******************************************************************************
  */

#include <rthw.h>
#include <rtthread.h>

#if defined(RT_USING_TSADC)

#include "hal_base.h"
#include "drv_thermal.h"
#include "drv_otp.h"

#define MONITOR_PERIOD_TIME 500
static bool is_init = false;
static bool thermal_mode = true;
static int trim_temp[SOC_MAX_SENSORS];
extern const struct tsadc_init g_tsadc_init;

void tsadc_get_trim_temp(const struct tsadc_init *p_tsadc_init, int chn)
{
#if defined(RT_USING_OTP)
    int trim_base = 30;
    uint8_t trim_l;
    uint8_t trim_h;
    uint32_t trim_code;

    if (p_tsadc_init->otp_mask == 0)
        return;

    otp_read(p_tsadc_init->otp_offset[chn], 1, &trim_l);
    otp_read(p_tsadc_init->otp_offset[chn] + 1, 1, &trim_h);
    if (trim_l == 0 && trim_h == 0)
        return;

    trim_code = ((trim_h << 8) | trim_l) & p_tsadc_init->otp_mask;
    trim_temp[chn] = trim_base - HAL_TSADC_CodeToTemp(trim_code);
#endif
}

void tsadc_dev_init(const struct tsadc_init *p_tsadc_init)
{
    is_init = true;
    for (int i = 0; i < p_tsadc_init->chn_num; i++)
    {
        HAL_TSADC_Enable_AUTO(p_tsadc_init->chn_id[i], p_tsadc_init->polarity, p_tsadc_init->mode);
        tsadc_get_trim_temp(p_tsadc_init, i);
    }
    rt_kprintf("tsadc init\n");
}

int tsadc_get_temp(int chn)
{
    return HAL_TSADC_GetTemperature_AUTO(chn) + trim_temp[chn];
}

int tsadc_default_init(void)
{
    if (!is_init)
    {
        for (int i = 0; i < g_tsadc_init.chn_num; i++)
        {
            HAL_TSADC_Enable_AUTO(g_tsadc_init.chn_id[i], g_tsadc_init.polarity, g_tsadc_init.mode);
            tsadc_get_trim_temp(&g_tsadc_init, i);
        }
        rt_kprintf("tsadc default init\n");
    }

    return RT_EOK;
}

static rt_thread_t thermal_monitor_thread = RT_NULL;

RT_WEAK void thermal_monitor(void)
{
    return;
};

void thermal_monitor_func()
{
    while (1)
    {
        if (thermal_mode)
            thermal_monitor();
        rt_thread_mdelay(MONITOR_PERIOD_TIME);
    }
}

int thermal_monitor_thread_init()
{
    thermal_monitor_thread = rt_thread_create("thermal_monitor_thread_init", thermal_monitor_func, RT_NULL, 1024, 10, 20);
    RT_ASSERT(thermal_monitor_thread);
    rt_thread_startup(thermal_monitor_thread);

    return RT_EOK;
}

void get_temp(int argc, char **argv)
{
    int chn = 0;

    if (1 == argc)
    {
        for (int i = 0; i < g_tsadc_init.chn_num; i++)
        {
            rt_kprintf("channel %d: %d\n", i, tsadc_get_temp(g_tsadc_init.chn_id[i]));
        }
    }
    else
    {
        chn = atoi(argv[1]);
        rt_kprintf("channel %d: %d\n", chn, tsadc_get_temp(chn));
    }
}

void set_thermal_mode(int argc, char **argv)
{
    if (2 <= argc)
    {
        if (atoi(argv[1]) == 1)
        {
            thermal_mode = true;
            rt_kprintf("enable thermal_monitor\n");
        }
        else if (atoi(argv[1]) == 0)
        {
            thermal_mode = false;
            rt_kprintf("disable thermal_monitor\n");
        }
    }
}
#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(get_temp, get current temp.e.g: get_temp <channel>);
MSH_CMD_EXPORT(set_thermal_mode, set thermal mode. e.g: set_thermal_mode 0);
#endif

INIT_BOARD_EXPORT(tsadc_default_init);
INIT_APP_EXPORT(thermal_monitor_thread_init);
#endif
