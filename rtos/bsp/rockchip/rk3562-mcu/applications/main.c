/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-30     Tanek        first version
 */

#include <stdio.h>
#include <string.h>
#include <rtthread.h>
#include <rthw.h>
#include <hal_base.h>

static int wdt_int_count = 0;
static int wdt_time_out = 8;
static int timer_int_count = 0;
static float latency_sum = 0.0;
struct TIMER_REG *timer = NULL;
static bool desc_timer = true;
static int fixed_spend = 0;
static float latency_max = 0.0;

extern void write_reg(uint32_t addr, uint32_t val);

void wdt_isr(int vector, void *param)
{
    if (++wdt_int_count <= 3)
    {
        rt_kprintf("eoi: %d\n", WDT->EOI);
        /*wdt_time_out++;
        WDT->TORR = wdt_time_out;
        WDT->CRR = 0x76;*/
    }
    else
    {
        rt_kprintf("int %d stat: %d, ccvr: %d\n", wdt_int_count, WDT->STAT, WDT->CCVR);
    }
}

void wdt_test(void)
{
    /* register wdt handler */
    rt_hw_interrupt_install(WDT0_IRQn, wdt_isr, NULL, "wdt");
    rt_hw_interrupt_umask(WDT0_IRQn);

    /* WDT_TORR[0:3]=1, set timeout_period */
    WDT->TORR = wdt_time_out;
    WDT->CRR = 0x76;

    /* WDT_CR[4:2]=0x2, rst_pluse_length; WDT_CR[1]=0x1, resp_mode; WDT_CR[0]=0x1, en */
    WDT->CR = 0x7;

    rt_kprintf("1s: %d\n", WDT->CCVR);
    rt_thread_delay(10);
    rt_kprintf("2s: %d\n", WDT->CCVR);
}

void tick_test(void)
{
    rt_kprintf("begin 10s test\n");
    rt_thread_delay(10 * RT_TICK_PER_SECOND);
    rt_kprintf("finish 10s test\n");
}

void timer_isr(int vector, void *param)
{
    uint32_t count;
    float latency;
    char szBuf[64];

    count = (uint32_t)HAL_TIMER_GetCount(timer);
    if (desc_timer)
        count = 24000000 - count;
    if (count > fixed_spend)
        count -= fixed_spend;
    latency = count * 1000000000.0 / 24000000;
    sprintf(szBuf, "latency=%fns(count=%ld)\n", latency, count);
    rt_kprintf("%s", szBuf);
    timer_int_count++;
    latency_sum += latency;
    latency_max = latency_max > latency ? latency_max : latency;
    if (timer_int_count == 100)
    {
        sprintf(szBuf, "latency avg=%f,max=%f\n", latency_sum / timer_int_count, latency_max);
        rt_kprintf("%s", szBuf);
        timer_int_count = 0;
        latency_sum = 0.0;
        latency_max = 0.0;
        HAL_TIMER_ClrInt(timer);
        HAL_TIMER_Stop_IT(timer);
    }

    HAL_TIMER_ClrInt(timer);
}

void timer_test(void)
{
    uint64_t start, end;
    uint32_t count;

    start = HAL_GetSysTimerCount();
    rt_thread_delay(RT_TICK_PER_SECOND);
    end = HAL_GetSysTimerCount();
    count = (uint32_t)(end - start);
    rt_kprintf("systimer 1s count: %ld(%lld, %lld)\n", count, start, end);
    RT_ASSERT(PLL_INPUT_OSC_RATE - count < 1000000);

    rt_kprintf("\n\ntest internal irq\n");
    timer = TIMER1;
    desc_timer = false;
    HAL_TIMER_Init(timer, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(timer, 2000000000);
    HAL_TIMER_Start(timer);
    start = HAL_TIMER_GetCount(timer);
    rt_thread_delay(RT_TICK_PER_SECOND);
    end = HAL_TIMER_GetCount(timer);
    count = (uint32_t)(end - start);
    fixed_spend = start;
    rt_kprintf("internal timer 1s count: %ld(%lld, %lld), fixed_spend=%d\n",
               count, start, end, fixed_spend);
    HAL_TIMER_Stop(timer);

    rt_hw_interrupt_install(TIMER1_IRQn, timer_isr, NULL, "timer");
    rt_hw_interrupt_umask(TIMER1_IRQn);
    HAL_TIMER_Init(timer, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(timer, 24000000);
    HAL_TIMER_Start_IT(timer);
}

void high_isr(int vector, void *param)
{
    rt_tick_t start, end, count;

    start = rt_tick_get();
    HAL_CPUDelayUs(1000000);
    end = rt_tick_get();
    count = end - start;
    RT_ASSERT(count == 0);
    rt_kprintf("high isr pass\n");
}

void low_isr(int vector, void *param)
{
    rt_tick_t start, end, count;

    start = rt_tick_get();
    HAL_CPUDelayUs(1000000);
    end = rt_tick_get();
    count = end - start;
    RT_ASSERT(count > 0);
    rt_kprintf("low isr pass\n");
}

void irq_prio_test(void)
{
    // set tick irq to middle priority
    HAL_NVIC_SetPriority(SysTick_IRQn, NVIC_PERIPH_PRIO_DEFAULT, NVIC_PERIPH_SUB_PRIO_DEFAULT);

    // irq has lower priority, so the tick update success
    HAL_NVIC_SetPriority(TIMER1_IRQn, NVIC_PERIPH_PRIO_LOWEST, NVIC_PERIPH_SUB_PRIO_DEFAULT);
    rt_hw_interrupt_install(TIMER1_IRQn, low_isr, NULL, "low");
    rt_hw_interrupt_umask(TIMER1_IRQn);
    HAL_NVIC_SetPendingIRQ(TIMER1_IRQn);

    rt_thread_delay(2 * RT_TICK_PER_SECOND);

    // irq has high priority, so the tick can not update
    HAL_NVIC_SetPriority(TIMER1_IRQn, NVIC_PERIPH_PRIO_HIGHEST, NVIC_PERIPH_SUB_PRIO_DEFAULT);
    rt_hw_interrupt_install(TIMER1_IRQn, high_isr, NULL, "high");
    rt_hw_interrupt_umask(TIMER1_IRQn);
    HAL_NVIC_SetPendingIRQ(TIMER1_IRQn);

    // change tick irq to lowest priority
    HAL_NVIC_SetPriority(SysTick_IRQn, NVIC_PERIPH_PRIO_LOWEST, NVIC_PERIPH_SUB_PRIO_DEFAULT);
}

void mbox_isr(int vector, void *param)
{
    rt_kprintf("cmd=%x, data=%x\n", MBOX0->A2B[0].CMD, MBOX0->A2B[0].DATA);
    MBOX0->A2B_STATUS = 0x1;
}

void mbox_test(int argc, char **argv)
{
    rt_hw_interrupt_install(MBOX_BB_IRQn, mbox_isr, NULL, "mbox");
    rt_hw_interrupt_umask(MBOX_BB_IRQn);
}

extern void benchmark(void);

//SECTION(".tcm.bss") static uint8_t bench_thread_stack[2048];
extern void test_dcache();
int main(void)
{
    //struct rt_thread tcb;
    //wdt_test();
    //tick_test();
    //irq_test();

    //rt_thread_init(&tcb, "bench", benchmark, NULL, bench_thread_stack, 2048, 10, 1000000);
    //rt_thread_startup(&tcb);
    //timer_test();
    //mbox_test(0, NULL);
    return 0;
}
