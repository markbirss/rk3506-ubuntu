/**
  * Copyright (c) 2018 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    interrupt.c
  * @version V0.1
  * @brief   interrupt interface for rt-thread
  *
  * Change Logs:
  * Date           Author          Notes
  * 2019-05-08     Cliff.Chen      first implementation
  *
  ******************************************************************************
  */

#include <rthw.h>
#include "hal_base.h"

#if defined(ARCH_ARM_CORTEX_M0) || defined(ARCH_ARM_CORTEX_M3) || defined(ARCH_ARM_CORTEX_M4) || defined(ARCH_ARM_CORTEX_M7) || defined(ARCH_ARM_CORTEX_M33)

#ifdef RT_USING_PROF_IRQ

#ifndef TOTAL_INTERRUPTS
#define TOTAL_INTERRUPTS NUM_INTERRUPTS
#endif

#define IRQ_AVG_COUNT  200

struct irq_summry
{
    uint64_t count;
    uint64_t start;
    uint64_t time_total;
    uint64_t time_avg;
    uint64_t time_max;
};

static struct irq_summry g_irq_prof[TOTAL_INTERRUPTS];

static void irq_enter_hook(void)
{
    uint32_t irq;

    irq = __get_IPSR() - 16;
    g_irq_prof[irq].count++;
    g_irq_prof[irq].start = HAL_TIMER_GetCount(SYS_TIMER);
}

static void irq_leave_hook(void)
{
    uint64_t time_cur, end;
    uint32_t irq;

    irq = __get_IPSR() - 16;
    end = HAL_TIMER_GetCount(SYS_TIMER);
    if (end < g_irq_prof[irq].start)
        time_cur = end + (uint64_t) -1 - g_irq_prof[irq].start;
    else
        time_cur = end - g_irq_prof[irq].start;
    g_irq_prof[irq].time_total += time_cur;
    g_irq_prof[irq].time_max =
        g_irq_prof[irq].time_max > time_cur ? g_irq_prof[irq].time_max : time_cur;
    if (g_irq_prof[irq].count > 0 &&
            g_irq_prof[irq].count % IRQ_AVG_COUNT == 0)
    {
        g_irq_prof[irq].time_avg = g_irq_prof[irq].time_total / IRQ_AVG_COUNT;
        g_irq_prof[irq].time_total = 0;
    }
}

static void dump_irq_summary(int argc, char **argv)
{
    uint32_t i;

    rt_kprintf("---IRQ SUMMARY(the unit of time is us)---\n");
    rt_kprintf("IRQ    COUNT       AVG         MAX\n");
    for (i = 0; i < NUM_INTERRUPTS; i++)
    {
        rt_kprintf("%03d    %08d    %08d    %08d\n", i, g_irq_prof[i].count,
                   (uint32_t)(g_irq_prof[i].time_avg * 1000000 / PLL_INPUT_OSC_RATE),
                   (uint32_t)(g_irq_prof[i].time_max * 1000000 / PLL_INPUT_OSC_RATE));
    }
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(dump_irq_summary, dump irq summary);
#endif

#endif    /* end of RT_USING_PROF_IRQ */

void rt_hw_interrupt_init(void)
{
    uint32_t i;

    for (i = 0; i < NUM_INTERRUPTS; i++)
    {
        HAL_NVIC_SetPriority(i, NVIC_PERIPH_PRIO_DEFAULT, NVIC_PERIPH_SUB_PRIO_DEFAULT);
    }

#if defined(HAL_INTMUX_MODULE_ENABLED)
    HAL_INTMUX_Init();
#endif

#ifdef RT_USING_PROF_IRQ
    rt_interrupt_enter_sethook(irq_enter_hook);
    rt_interrupt_leave_sethook(irq_leave_hook);
#endif
}

void rt_hw_interrupt_mask(int vector)
{
#ifdef HAL_INTMUX_MODULE_ENABLED
    if (vector >= NUM_INTERRUPTS) {
        HAL_INTMUX_DisableIRQ(vector);
        return;
    }
#endif
    HAL_NVIC_DisableIRQ(vector);
}

void rt_hw_interrupt_umask(int vector)
{
#ifdef HAL_INTMUX_MODULE_ENABLED
    if (vector >= NUM_INTERRUPTS) {
        HAL_INTMUX_EnableIRQ(vector);
        return;
    }
#endif
    HAL_NVIC_EnableIRQ(vector);
}

rt_isr_handler_t rt_hw_interrupt_install(int              vector,
        rt_isr_handler_t handler,
        void            *param,
        const char      *name)
{
#ifdef HAL_INTMUX_MODULE_ENABLED
    if (vector >= NUM_INTERRUPTS) {
        HAL_INTMUX_SetIRQHandler(vector, (HAL_INTMUX_HANDLER)handler, param);
        return handler;
    }
#endif
    HAL_NVIC_SetIRQHandler(vector, (NVIC_IRQHandler)handler);

    return handler;
}

#endif    /* end of defined(ARCH_ARM_CORTEX_M0) || defined(ARCH_ARM_CORTEX_M3) || defined(ARCH_ARM_CORTEX_M4) || defined(ARCH_ARM_CORTEX_M7) */
