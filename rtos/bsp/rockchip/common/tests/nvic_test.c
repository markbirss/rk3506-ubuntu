/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
 */
#include "rtthread.h"
#include "rthw.h"
#include "hal_base.h"

#if defined(RT_USING_COMMON_TEST_NVIC) && defined(HAL_NVIC_MODULE_ENABLED)

struct timer_info
{
    struct TIMER_REG *timer;
    IRQn_Type irq;
};

static struct timer_info t_info = {TIMER1, TIMER1_IRQn};
static uint32_t timer_int_count = 0;
static double latency_sum = 0.0, latency_max = 0.0;

void timer_isr(int vector, void *param)
{
    uint32_t count1, count2, count, fixed_spend;
    float latency;
    char szBuf[64];

    count1 = (uint32_t)HAL_TIMER_GetCount(t_info.timer);
    count2 = (uint32_t)HAL_TIMER_GetCount(t_info.timer);
    if (count1 > count2)          /* timer is decreased */
    {
        count = PLL_INPUT_OSC_RATE - count1;
        fixed_spend = count1 - count2;
    }
    else
    {
        count = count1;
        fixed_spend = count2 - count1;
    }
    if (count > fixed_spend)       /* subtract the overhead of reading register */
        count -= fixed_spend;

    latency = count * 1000000000.0 / PLL_INPUT_OSC_RATE;
    sprintf(szBuf, "latency=%fns(count=%ld)\n", latency, count);
    rt_kprintf("%s", szBuf);

    timer_int_count++;
    latency_sum += latency;
    latency_max = latency_max > latency ? latency_max : latency;
    if (timer_int_count == 100)
    {
        sprintf(szBuf, "timer latency avg=%f,max=%f\n", latency_sum / timer_int_count, latency_max);
        rt_kprintf("%s", szBuf);
        timer_int_count = 0;
        latency_sum = 0.0;
        latency_max = 0.0;
        HAL_TIMER_Stop_IT(t_info.timer);
    }

    HAL_TIMER_ClrInt(t_info.timer);
}

static void irq_latency_test(void)
{
    rt_hw_interrupt_install(t_info.irq, timer_isr, NULL, "timer");
    rt_hw_interrupt_umask(t_info.irq);
    HAL_TIMER_Init(t_info.timer, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(t_info.timer, PLL_INPUT_OSC_RATE);
    HAL_TIMER_Start_IT(t_info.timer);
}

static uint32_t irq_raise_tick = 0;

void soft_irq_isr(int vector, void *param)
{
    uint32_t irq_resp_tick, count;
    int32_t fixed_overhead;
    double latency;
    char szBuf[64];

    irq_resp_tick = SysTick->VAL;
    fixed_overhead = irq_resp_tick - SysTick->VAL;
    if (fixed_overhead < 0)
        fixed_overhead += SysTick->LOAD;

    count = irq_raise_tick - irq_resp_tick;
    if (count < 0)                 /* systick overflow */
        count += SysTick->LOAD;
    if (count > fixed_overhead)
        count -= fixed_overhead;

    latency = (count * 1000000000.0) / SystemCoreClock;
    sprintf(szBuf, "softirq latency=%fns(count=%ld)\n", latency, count);
    rt_kprintf("%s", szBuf);
    rt_kprintf("irq_raise_tick=%d, irq_resp_tick=%d, fixed_overhead=%d\n", irq_raise_tick, irq_resp_tick, fixed_overhead);
}

static void soft_irq_test(void)
{
    rt_hw_interrupt_install(TIMER1_IRQn, soft_irq_isr, NULL, "timer");
    rt_hw_interrupt_umask(TIMER1_IRQn);
    irq_raise_tick = SysTick->VAL;
    HAL_NVIC_SetPendingIRQ(TIMER1_IRQn);
}

static volatile bool irq_mask;
void irq_mask_isr(int vector, void *param)
{
    if (irq_mask)
    {
        rt_kprintf("irq mask failed\n");
    }
    else
    {
        rt_kprintf("irq mask test pass\n");
    }
}

static void irq_mask_test(void)
{
    rt_hw_interrupt_install(TIMER1_IRQn, irq_mask_isr, NULL, "timer");
    rt_hw_interrupt_mask(TIMER1_IRQn);
    irq_mask = true;
    HAL_NVIC_SetPendingIRQ(TIMER1_IRQn);
    HAL_DelayMs(10);
    irq_mask = false;
    rt_hw_interrupt_umask(TIMER1_IRQn);
}

static void test_nvic(int argc, char **argv)
{
    int irq_latency = 0, soft_irq = 0, irq_mask = 0;

    if (argc == 1)
        irq_latency = soft_irq = irq_mask = 1;

    if (argc == 2)
    {
        if (atoi(argv[1]) == 1)
            irq_latency = 1;
        else if (atoi(argv[1]) == 2)
            soft_irq = 1;
        else if (atoi(argv[1]) == 3)
            irq_mask = 1;
        else
            rt_kprintf("test_nvic [1,2,3]\n");
    }
    else
        rt_kprintf("test_nvic [1,2,3]\n");

    if (irq_latency)
        irq_latency_test();
    if (soft_irq)
        soft_irq_test();
    if (irq_mask)
        irq_mask_test();

    rt_kprintf("%s finish\n", __FUNCTION__);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(test_nvic, nvic test);
#endif

#endif
