/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-9-1       Tony.xie     first implementation
 *
 */

#include "drv_pm.h"

#define TICK_DELAY_TIME_MS (100)
#define TICK_DELAY_MAX_TIME_MS (1000 * 4)

#ifdef RT_USING_PM_RUNTIME
#ifdef RT_USING_PM_TICK_DELAY
#define RK_TICK_TIMER_COUNT ((PLL_INPUT_OSC_RATE / RT_TICK_PER_SECOND) - 1)
#define RK_PM_SUB_ABS(a,b) (a >= b)? (a - b):(b - a)

static uint64_t pm_tick_timer_count;
static uint32_t pm_rt_sys_tick;

void pm_ref_timer_count_reinit(void)
{
    pm_ref_timer_set_count(0);
}
#endif

void rt_pm_runtime_enter(void)
{
    ePM_RUNTIME_idleMode idle_mode;
    rt_tick_t idle_tick, next_timer_tick;
#ifdef RT_USING_PM_TICK_DELAY
    rt_tick_t pre_tick, timer_tick, sys_tick, delta_tick;
    rt_uint64_t tick_timer_count, cur_count, ref_count;
#if TICK_DEALY_DBG
    rt_uint64_t pre_count;
#endif
#endif

    next_timer_tick = rt_timer_next_timeout_tick();

    if (next_timer_tick == RT_TICK_MAX)
        idle_tick = PM_MS_TO_SYS_TICK(TICK_DELAY_MAX_TIME_MS);
    else
        idle_tick = next_timer_tick - rt_tick_get();

#ifdef RT_USING_PM_TICK_DELAY
    if (idle_tick > PM_MS_TO_SYS_TICK(TICK_DELAY_TIME_MS))
    {
        idle_tick -= 2;

        pre_tick = rt_tick_get();
        ref_count = pm_ref_timer_get_count();

        if (!ref_count)
        {
            pm_tick_timer_count = HAL_TIMER_GetCount(TICK_TIMER);
            tick_timer_count = HAL_TIMER_GetCount(TICK_TIMER);

            ref_count = HAL_GetSysTimerCount();
            pm_ref_timer_set_count(ref_count);

            PM_TICKDLY_INFO("***tick timer check %u,%u,%u\n",
                            (rt_uint32_t)pm_tick_timer_count,
                            (rt_uint32_t)tick_timer_count,
                            RK_TICK_TIMER_COUNT);
            /*  checking the tick timer is  a inc or dec timer */
            if (tick_timer_count > pm_tick_timer_count)
                pm_tick_timer_count = RK_TICK_TIMER_COUNT -
                                      pm_tick_timer_count;

            pm_rt_sys_tick = pre_tick;
            ref_count = ref_count + pm_tick_timer_count;

            PM_TICKDLY_INFO("***base %ums, %u-%u=%u, %u\n",
                            PM_TIMER_TO_MS(ref_count),
                            rk_pm_timer_to_tick(ref_count),
                            pm_rt_sys_tick,
                            RK_PM_SUB_ABS(rk_pm_timer_to_tick(ref_count),
                                          pm_rt_sys_tick),
                            (rt_uint32_t)pm_tick_timer_count);
        }

        HAL_TIMER_SetCount(PM_TIMER, rk_pm_tick_to_timer(idle_tick));
        HAL_TIMER_Start_IT(PM_TIMER);

        HAL_TIMER_Stop_IT(TICK_TIMER);
        HAL_TIMER_ClrInt(TICK_TIMER);
        HAL_NVIC_ClearPendingIRQ(TICK_IRQn);

#if TICK_DEALY_DBG
        pre_count = HAL_GetSysTimerCount();
        PM_TICKDLY_INFO("*now %ums, %u:%u-%u=%u\n",
                        PM_TIMER_TO_MS(pre_count),
                        RK_PM_SUB_ABS(rk_pm_timer_to_tick(ref_count),
                                      pm_rt_sys_tick),
                        rk_pm_timer_to_tick(pre_count),
                        pre_tick,
                        RK_PM_SUB_ABS(rk_pm_timer_to_tick(pre_count),
                                      pre_tick));

        PM_TICKDLY_INFO(" time out %ums:%u\n",
                        (rt_uint32_t)PM_SYS_TICK_TO_MS(idle_tick), idle_tick);
#endif
        idle_mode = PM_RUNTIME_IDLE_DEEP;
    }
    else if (idle_tick > PM_MS_TO_SYS_TICK(2))
#else
    if (idle_tick > PM_MS_TO_SYS_TICK(2))
#endif
    {
        idle_mode = PM_RUNTIME_IDLE_DEEP;
    }
#ifdef RT_USING_PM_IDLE_NORMAL
    else if (idle_tick > PM_MS_TO_SYS_TICK(0))
    {
        idle_mode = PM_RUNTIME_IDLE_NORMAL;
    }
#endif
    else
    {
        idle_mode = PM_RUNTIME_IDLE_ONLY;
    }

    HAL_PM_RuntimeEnter(idle_mode);

#ifdef RT_USING_PM_TICK_DELAY
    if (idle_tick > PM_MS_TO_SYS_TICK(TICK_DELAY_TIME_MS))
    {
        HAL_TIMER_Start_IT(TICK_TIMER);

        HAL_TIMER_Stop_IT(PM_TIMER);
        HAL_TIMER_ClrInt(PM_TIMER);

        /* Get the reference count difference */
        cur_count = HAL_GetSysTimerCount();
        timer_tick = rk_pm_timer_to_tick(cur_count - ref_count);

        /* Get the system tick difference */
        sys_tick =  pre_tick - pm_rt_sys_tick;

        if (timer_tick >= sys_tick)
        {
            delta_tick = timer_tick - sys_tick;
        }
        else
        {
            rt_kprintf("%s error! %u\n", __func__, sys_tick - timer_tick);
            return;
        }

        /* adjust OS tick */
        rt_tick_set(rt_tick_get() + delta_tick);
        /* check system timer */
        rt_timer_check();

        PM_TICKDLY_INFO(" end %u,%u:\n",
                        PM_TIMER_TO_MS(cur_count - pre_count), rt_tick_get());
    }
#endif
}
#endif
