/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-12     Steven Liu   the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "hal_base.h"

#include <rtthread.h>

#define THREAD_PRIORITY         20
#define THREAD_STACK_SIZE       2048
#define THREAD_TIMESLICE        5

static rt_thread_t tid[RT_CPUS_NR] = {0};

static void thread_entry(void *parameter)
{
    rt_uint32_t cpu_id = rt_hw_cpu_id();

    while (1)
    {
        rt_kprintf("CPU %ld: Hello RT-Thread SMP!\n", cpu_id);
        rt_thread_mdelay(1000);
    }
}

int smp_test(int argc, char **argv)
{
    int i;

    for (i = 0; i < RT_CPUS_NR; i++)
    {
        tid[i] = rt_thread_create("cpu_thread",
                                  thread_entry, NULL,
                                  THREAD_STACK_SIZE,
                                  THREAD_PRIORITY,
                                  THREAD_TIMESLICE);

        if (tid[i] != RT_NULL)
        {
            rt_thread_control(tid[i], RT_THREAD_CTRL_BIND_CPU, i);
            rt_thread_startup(tid[i]);
        }
        else
        {
            rt_kprintf("Failed to create thread for CPU %d\n", i);
        }
    }

    return 0;
}

MSH_CMD_EXPORT(smp_test, smp test);

int main(int argc, char **argv)
{
    rt_uint32_t cpu_id;

    cpu_id = HAL_CPU_TOPOLOGY_GetCurrentCpuId();
    rt_kprintf("Hello RK3568 RT-Thread! CPU_ID(%d)\n", cpu_id);

    return RT_EOK;
}
