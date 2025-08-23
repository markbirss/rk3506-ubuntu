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
#include "hal_base.h"

#ifdef RT_USING_SMP

#define THREAD_STACK_SIZE   1024
#define THREAD_PRIORITY     21
#define THREAD_TIMES        100

static void thread_entry(void* parameter)
{
    uint64_t start, end, delta, overhead;
    rt_uint32_t i, sche_delay, skip;
    rt_uint32_t min, max, total;
    char *name = parameter;

    min = 10000000;
    max = total = 0;
    start = HAL_GetSysTimerCount();
    end = HAL_GetSysTimerCount();
    overhead = end - start;
    skip = 0;
    for (i = 0; i < THREAD_TIMES; i++)
    {
        start = HAL_GetSysTimerCount();
        rt_thread_mdelay(10);  // 线程1延迟10个tick
        end = HAL_GetSysTimerCount();
        
        delta = end - start;
        delta -= overhead;
        if (delta < (PLL_INPUT_OSC_RATE * 10) / RT_TICK_PER_SECOND)
        {
            //rt_kprintf("delta: %lld, overhead: %lld\n", delta, overhead);
            skip++;
            continue;
        }
        sche_delay = ((delta - (PLL_INPUT_OSC_RATE * 10) / RT_TICK_PER_SECOND) * 1000000) / PLL_INPUT_OSC_RATE;
        min = sche_delay > min ? min : sche_delay;
        max = sche_delay > max ? sche_delay : max;
        total += sche_delay;
        //rt_kprintf("%s(cpu=%d): sche_delay %dus\n", name, rt_hw_cpu_id(), sche_delay);
        HAL_CPUDelayUs(10000);
    }

    rt_kprintf("%s(cpu=%d): min=%dus, max=%dus, avg=%dus\n", name, rt_hw_cpu_id(), min, max, total / (THREAD_TIMES - skip));
}

void timer_irq(int vector, void *param)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_CPUDelayUs(10000);
    HAL_TIMER_ClrInt(TIMER0);

    /* leave interrupt */
    rt_interrupt_leave();
}

void timer_irq_setup(void)
{
    rt_hw_interrupt_install(TIMER0_IRQn, timer_irq, RT_NULL, "timer");
    rt_hw_interrupt_umask(TIMER0_IRQn);
    HAL_TIMER_Init(TIMER0, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(TIMER0, PLL_INPUT_OSC_RATE / 10);
    HAL_TIMER_Start_IT(TIMER0);
}

void timer_irq_teardown(void)
{
    HAL_TIMER_Stop_IT(TIMER0);
}

static char *thread_name[RT_CPUS_NR] = {"thread0", "thread1", "thread2", "thread3"};

int test_sche_delay(int argc, char **argv)
{
    rt_err_t result;
    rt_thread_t thread;
    rt_uint32_t i, bind = 0;

    if (argc == 2)
        bind = 1;

    timer_irq_setup();

    for (i = 0; i < RT_CPUS_NR; i++)
    {
        thread = rt_thread_create(thread_name[i], thread_entry, thread_name[i], THREAD_STACK_SIZE, THREAD_PRIORITY, 10);
        if (thread != RT_NULL)
        {
            if (bind)
            {
                result = rt_thread_control(thread, RT_THREAD_CTRL_BIND_CPU, (void *)i);
                if (result != RT_EOK)
                {
                    rt_kprintf("bind cpu%d fail\n", i);
                    return -1;
                }
            }
            result = rt_thread_startup(thread);
            if (result != RT_EOK)
            {
                rt_kprintf("startup thread%d fail\n", i);
                return -1;
            }
        }
    }

    return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(test_sche_delay, sche delay test);
#endif

#else

#define THREAD_STACK_SIZE   1024
#define THREAD_PRIORITY     21
#define THREAD_TIMES        100

static void thread_entry(void* parameter)
{
    uint64_t start, end, delta, overhead;
    rt_uint32_t i, sche_delay, skip;
    rt_uint32_t min, max, total;
    char *name = parameter;

    min = 10000000;
    max = total = 0;
    start = HAL_GetSysTimerCount();
    end = HAL_GetSysTimerCount();
    overhead = end - start;
    skip = 0;
    for (i = 0; i < THREAD_TIMES; i++)
    {
        start = HAL_GetSysTimerCount();
        rt_thread_mdelay(10);  // 线程1延迟10个tick
        end = HAL_GetSysTimerCount();

        delta = end - start;
        delta -= overhead;
        if (delta < (PLL_INPUT_OSC_RATE * 10) / RT_TICK_PER_SECOND)
        {
            skip++;
            continue;
        }
        sche_delay = ((delta - (PLL_INPUT_OSC_RATE * 10) / RT_TICK_PER_SECOND) * 1000000) / PLL_INPUT_OSC_RATE;
        min = sche_delay > min ? min : sche_delay;
        max = sche_delay > max ? sche_delay : max;
        total += sche_delay;
        HAL_CPUDelayUs(10000);
    }

    rt_kprintf("min=%dus, max=%dus, avg=%dus\n", min, max, total / (THREAD_TIMES - skip));
}

void timer_irq(int vector, void *param)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_CPUDelayUs(10000);
    HAL_TIMER_ClrInt(TIMER0);

    /* leave interrupt */
    rt_interrupt_leave();
}

void timer_irq_setup(void)
{
    rt_hw_interrupt_install(TIMER0_IRQn, timer_irq, RT_NULL, "timer");
    rt_hw_interrupt_umask(TIMER0_IRQn);
    HAL_TIMER_Init(TIMER0, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(TIMER0, PLL_INPUT_OSC_RATE / 10);
    HAL_TIMER_Start_IT(TIMER0);
}

void timer_irq_teardown(void)
{
    HAL_TIMER_Stop_IT(TIMER0);
}

int test_sche_delay(int argc, char **argv)
{
    rt_err_t result;
    rt_thread_t thread;
    rt_uint32_t i;

    if (argc == 2)
        timer_irq_setup();

    for (i = 0; i < 1; i++)
    {
        thread = rt_thread_create("thread", thread_entry, "thread", THREAD_STACK_SIZE, THREAD_PRIORITY, 10);
        if (thread != RT_NULL)
        {
            result = rt_thread_startup(thread);
            if (result != RT_EOK)
            {
                rt_kprintf("startup thread%d fail\n", i);
                return -1;
            }
        }
    }

    return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(test_sche_delay, sche delay test);
#endif

#endif
