/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-17     Cliff Chen   first implementation
 */

#include <rtthread.h>
#include <rthw.h>
#include <interrupt.h>
#include "hal_base.h"

#ifdef RT_USING_SMP

#define TEST_COUNT        1000          /* 测试次数 */
#define TIMER_COUNT       (PLL_INPUT_OSC_RATE / 100)

static rt_uint64_t cur_lat[RT_CPUS_NR], min_lat[RT_CPUS_NR], max_lat[RT_CPUS_NR], sum_lat[RT_CPUS_NR];
static rt_uint32_t interrupt_count[RT_CPUS_NR];

#define __to_us(x) ((x * 1000000) / PLL_INPUT_OSC_RATE)

struct timer_info
{
    struct TIMER_REG *timer;
    IRQn_Type irq;
};

struct timer_info t_info[RT_CPUS_NR] =
{
    {TIMER0, TIMER0_IRQn},
    {TIMER1, TIMER1_IRQn},
    {TIMER2, TIMER2_IRQn},
    {TIMER3, TIMER3_IRQn}
};

static void timer_irq_handler(int irq, void *param)
{
    rt_uint32_t cpu_id;

    rt_interrupt_enter();
    cpu_id = rt_hw_cpu_id();
    cur_lat[cpu_id] = HAL_TIMER_GetCount(t_info[cpu_id].timer);
    RT_ASSERT(irq == t_info[cpu_id].irq);
    if (cur_lat[cpu_id] > (TIMER_COUNT / 2))
    {
        //rt_kprintf("timer%d may be is descending timer\n", cpu_id);
        cur_lat[cpu_id] = TIMER_COUNT - cur_lat[cpu_id];
    }
    min_lat[cpu_id] = min_lat[cpu_id] > cur_lat[cpu_id] ? cur_lat[cpu_id] : min_lat[cpu_id];
    max_lat[cpu_id] = max_lat[cpu_id] > cur_lat[cpu_id] ? max_lat[cpu_id] : cur_lat[cpu_id];
    sum_lat[cpu_id] += cur_lat[cpu_id];

    if (++interrupt_count[cpu_id] == TEST_COUNT)
    {
        HAL_TIMER_Stop_IT(t_info[cpu_id].timer);
        HAL_TIMER_DeInit(t_info[cpu_id].timer);
        rt_kprintf("cpu%d: min=%lldus, max=%lldus, avg=%lldus\n", cpu_id,
                    __to_us(min_lat[cpu_id]), __to_us(max_lat[cpu_id]), __to_us(sum_lat[cpu_id] / TEST_COUNT));
    }
    HAL_TIMER_ClrInt(t_info[cpu_id].timer);

    rt_interrupt_leave();
}

void interrupt_latency_thread_entry(void *param)
{
    rt_uint32_t cpu_id = rt_hw_cpu_id();

    min_lat[cpu_id] = (rt_uint64_t)-1;
    max_lat[cpu_id] = 0;
    sum_lat[cpu_id] = 0;
    interrupt_count[cpu_id] = 0;
    rt_hw_interrupt_install(t_info[cpu_id].irq, timer_irq_handler, RT_NULL, "Test Timer IRQ");
    rt_hw_interrupt_umask(t_info[cpu_id].irq);
    rt_hw_interrupt_set_target_cpus(t_info[cpu_id].irq, 1 << cpu_id);
    HAL_TIMER_Init(t_info[cpu_id].timer, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(t_info[cpu_id].timer, TIMER_COUNT);
    HAL_TIMER_Start_IT(t_info[cpu_id].timer);
    rt_kprintf("cpu%d irq test\n", cpu_id);
}

void multi_core_interrupt_latency_test(void)
{
    rt_uint32_t i;
    char thread_name[RT_CPUS_NR][16] = {0};

    for (i = 0; i < RT_CPUS_NR; i++)
    {
        if (i == 1)
            continue;
        rt_snprintf(thread_name[i], RT_NAME_MAX, "irq_test_%d", i);
        rt_thread_t tid = rt_thread_create(thread_name[i],
                                           interrupt_latency_thread_entry,
                                           (void *)i,
                                           1024,
                                           RT_THREAD_PRIORITY_MAX / 2,
                                           10);
        if (tid != RT_NULL)
        {
            rt_thread_control (tid, RT_THREAD_CTRL_BIND_CPU, (void *)i);
            rt_thread_startup(tid);
        }
        else
        {
            rt_kprintf("Failed to create thread for core %d\n", i);
        }
    }
}

MSH_CMD_EXPORT(multi_core_interrupt_latency_test, test multi-core interrupt concurrency latency on Rockchip platform);

#else

#define RT_CPUS_NR        1
#define TEST_COUNT        1000          /* 测试次数 */
#define TIMER_COUNT       (PLL_INPUT_OSC_RATE / 100)

static rt_uint64_t cur_lat[RT_CPUS_NR], min_lat[RT_CPUS_NR], max_lat[RT_CPUS_NR], sum_lat[RT_CPUS_NR];
static rt_uint32_t interrupt_count[RT_CPUS_NR];

#define __to_us(x) ((x * 1000000) / PLL_INPUT_OSC_RATE)

struct timer_info
{
    struct TIMER_REG *timer;
    IRQn_Type irq;
};

struct timer_info t_info[RT_CPUS_NR] =
{
    {TIMER0, TIMER0_IRQn}
};

static void timer_irq_handler(int irq, void *param)
{
    rt_uint32_t cpu_id = 0;

    rt_interrupt_enter();
    cur_lat[cpu_id] = HAL_TIMER_GetCount(t_info[cpu_id].timer);
    RT_ASSERT(irq == t_info[cpu_id].irq);
    if (cur_lat[cpu_id] > (TIMER_COUNT / 2))
    {
        cur_lat[cpu_id] = TIMER_COUNT - cur_lat[cpu_id];
    }
    min_lat[cpu_id] = min_lat[cpu_id] > cur_lat[cpu_id] ? cur_lat[cpu_id] : min_lat[cpu_id];
    max_lat[cpu_id] = max_lat[cpu_id] > cur_lat[cpu_id] ? max_lat[cpu_id] : cur_lat[cpu_id];
    sum_lat[cpu_id] += cur_lat[cpu_id];

    if (++interrupt_count[cpu_id] == TEST_COUNT)
    {
        HAL_TIMER_Stop_IT(t_info[cpu_id].timer);
        HAL_TIMER_DeInit(t_info[cpu_id].timer);
        rt_kprintf("cpu%d: min=%lldus, max=%lldus, avg=%lldus\n", cpu_id,
                    __to_us(min_lat[cpu_id]), __to_us(max_lat[cpu_id]), __to_us(sum_lat[cpu_id] / TEST_COUNT));
    }
    HAL_TIMER_ClrInt(t_info[cpu_id].timer);

    rt_interrupt_leave();
}

void interrupt_latency_thread_entry(void *param)
{
    rt_uint32_t cpu_id = 0;

    min_lat[cpu_id] = (rt_uint64_t)-1;
    max_lat[cpu_id] = 0;
    sum_lat[cpu_id] = 0;
    interrupt_count[cpu_id] = 0;
    rt_hw_interrupt_install(t_info[cpu_id].irq, timer_irq_handler, RT_NULL, "Test Timer IRQ");
    rt_hw_interrupt_umask(t_info[cpu_id].irq);
    rt_hw_interrupt_set_target_cpus(t_info[cpu_id].irq, 1 << cpu_id);
    HAL_TIMER_Init(t_info[cpu_id].timer, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(t_info[cpu_id].timer, TIMER_COUNT);
    HAL_TIMER_Start_IT(t_info[cpu_id].timer);
    rt_kprintf("cpu%d irq test\n", cpu_id);
}

void multi_core_interrupt_latency_test(void)
{
    rt_uint32_t i;
    char thread_name[RT_CPUS_NR][16] = {0};

    for (i = 0; i < RT_CPUS_NR; i++)
    {
        if (i == 1)
            continue;
        rt_snprintf(thread_name[i], RT_NAME_MAX, "irq_test_%d", i);
        rt_thread_t tid = rt_thread_create(thread_name[i],
                                           interrupt_latency_thread_entry,
                                           (void *)i,
                                           1024,
                                           RT_THREAD_PRIORITY_MAX / 2,
                                           10);
        if (tid != RT_NULL)
        {
            rt_thread_control (tid, RT_THREAD_CTRL_BIND_CPU, (void *)i);
            rt_thread_startup(tid);
        }
        else
        {
            rt_kprintf("Failed to create thread for core %d\n", i);
        }
    }
}

MSH_CMD_EXPORT(multi_core_interrupt_latency_test, test multi-core interrupt concurrency latency on Rockchip platform);

#endif
