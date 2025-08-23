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

#ifdef RT_USING_BENCHMARK
extern void benchmark(void);

void config_freq(void)
{
    rt_kprintf("enter set freq 1008000000\n");
    rt_kprintf("origin freq=%d\n", HAL_CRU_ClkGetFreq(ARMCLK_L));
    HAL_CRU_ClkSetFreq(ARMCLK_L, 1008000000);
    HAL_SystemCoreClockUpdate(1008000000, HAL_SYSTICK_CLKSRC_EXT);
    rt_kprintf("leave set freq 1008000000\n");
}
#endif

int main(int argc, char **argv)
{
#ifdef RT_USING_SMP
    rt_kprintf("Hello RK3576 RT-Thread!\n");
#else
    uint32_t cpu_id = HAL_CPU_TOPOLOGY_GetCurrentCpuId();

    rt_kprintf("Hello RK3576 RT-Thread! CPU_ID=%d\n", cpu_id);
#endif
    return RT_EOK;
}
