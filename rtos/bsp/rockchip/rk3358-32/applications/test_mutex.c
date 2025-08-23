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
#include <rtdevice.h>
#include <board.h>

#define TEST_COUNT 1000

static rt_mutex_t test_mutex;
static rt_uint64_t total_wakeup_latency;
static rt_uint64_t min_wakeup_latency;
static rt_uint64_t max_wakeup_latency;
static volatile rt_uint64_t start_counter;
static rt_uint32_t test_finish;

static inline rt_uint64_t read_cpu_counter(void)
{
    rt_uint32_t low, high;

    asm volatile("mrrc p15, 0, %0, %1, c14" : "=r" (low), "=r" (high));

    return ((rt_uint64_t)high << 32) | low;
}

static void thread_a_entry(void *parameter)
{
    while(1)
    {
        rt_mutex_take(test_mutex, RT_WAITING_FOREVER);

        /* 释放互斥锁并开始计时 */
        start_counter = read_cpu_counter();
        rt_mutex_release(test_mutex);

        if (test_finish)
            break;

        rt_thread_delay(1);
    }
}

static void thread_b_entry(void *parameter)
{
    rt_uint32_t i = 0;
    rt_uint64_t end_counter, wakeup_latency;

    while(1)
    {
        rt_mutex_take(test_mutex, RT_WAITING_FOREVER);
        if (start_counter == -1) {
            rt_mutex_release(test_mutex);
            //rt_thread_delay(1);
            continue;
        }

        end_counter = read_cpu_counter();
        wakeup_latency = (end_counter - start_counter) * 1000 / (SystemCoreClock / 1000000);
        total_wakeup_latency += wakeup_latency;

        if (wakeup_latency < min_wakeup_latency)
        {
            min_wakeup_latency = wakeup_latency;
        }
        if (wakeup_latency > max_wakeup_latency)
        {
            max_wakeup_latency = wakeup_latency;
        }
        if (++i == TEST_COUNT) {
            rt_mutex_release(test_mutex);
            break;
        }

        start_counter = -1;
        rt_mutex_release(test_mutex);
    }

    rt_kprintf("Average wakeup latency: %llu ns, Min wakeup latency: %llu ns, Max wakeup latency: %llu ns\n",
               total_wakeup_latency / TEST_COUNT, min_wakeup_latency, max_wakeup_latency);
    test_finish = 1;
}

void mutex_wakeup_latency_test(int argc, char **argv)
{
    rt_thread_t thread_a, thread_b;
    rt_uint32_t cpu;

    test_finish = 0;
    min_wakeup_latency = -1;
    max_wakeup_latency = 0;
    total_wakeup_latency = 0;

    /* 创建互斥锁 */
    test_mutex = rt_mutex_create("test_mutex", RT_IPC_FLAG_FIFO);
    if (test_mutex == RT_NULL)
    {
        rt_kprintf("Failed to create mutex\n");
        return;
    }

    /* 创建线程A */
    thread_a = rt_thread_create("thread_a",
                                thread_a_entry,
                                RT_NULL,
                                1024,
                                RT_THREAD_PRIORITY_MAX / 2 - 1,
                                10);
    if (thread_a != RT_NULL)
    {
        rt_thread_startup(thread_a);
    }
    else
    {
        rt_kprintf("Failed to create thread A\n");
    }

    /* 创建线程B */
    thread_b = rt_thread_create("thread_b",
                                thread_b_entry,
                                RT_NULL,
                                1024,
                                RT_THREAD_PRIORITY_MAX / 2,
                                10);
    if (thread_b != RT_NULL)
    {
#ifdef RT_USING_SMP
        if (argc == 2) {
            cpu = 3;
            rt_thread_control(thread_b, RT_THREAD_CTRL_BIND_CPU, (void *)cpu);
        }
#endif
        rt_thread_startup(thread_b);
    }
    else
    {
        rt_kprintf("Failed to create thread B\n");
    }
}

MSH_CMD_EXPORT(mutex_wakeup_latency_test, test mutex wakeup latency on RT-Thread);
