/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-09     Cliff Chen   first implementation
 */

#ifndef __BOARD_BASE_H__
#define __BOARD_BASE_H__

#include "rtconfig.h"
#include <stdint.h>
#include "hal_base.h"

#define MAX_HANDLERS        NUM_INTERRUPTS
#define __REG32(x) (*((volatile unsigned int*)((rt_uint32_t)x)))

#ifndef PLL_INPUT_OSC_RATE
#define PLL_INPUT_OSC_RATE  24000000
#endif

extern uint32_t __bss_start[];
extern uint32_t __bss_end[];

extern uint32_t __heap_begin[];
extern uint32_t __heap_end[];

#ifdef RT_USING_UNCACHE_HEAP
#define RT_HW_HEAP_BEGIN        (void*)&__heap_begin
#define RT_HW_HEAP_END          ((void*)&__heap_end - RT_UNCACHE_HEAP_SIZE)
#define FIRMWARE_SIZE           (DRAM_SIZE - RT_UNCACHE_HEAP_SIZE)
#define RT_UNCACHE_HEAP_BASE    (FIRMWARE_BASE + FIRMWARE_SIZE)    // same as: RT_HW_HEAP_END
#else
#define RT_HW_HEAP_BEGIN        (void*)&__heap_begin
#define RT_HW_HEAP_END          (void*)&__heap_end
#endif

#define GIC_IRQ_START               0
#define ARM_GIC_NR_IRQS             NUM_INTERRUPTS
#define ARM_GIC_MAX_NR              NUM_INTERRUPTS
#define GIC_ACK_INTID_MASK          0x000003ff

rt_inline rt_uint32_t platform_get_gic_dist_base(void)
{
    return GIC_DISTRIBUTOR_BASE;
}

rt_inline rt_uint32_t platform_get_gic_cpu_base(void)
{
    return GIC_CPU_INTERFACE_BASE;
}

void rt_hw_board_init(void);

#endif
