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

#include "spinlock_id.h"

#ifdef RT_USING_RPMSG_LITE
#include "rpmsg_base.h"
#endif

#define MAX_HANDLERS        NUM_INTERRUPTS
#define __REG32(x) (*((volatile unsigned int*)((rt_uint32_t)x)))

#ifndef PLL_INPUT_OSC_RATE
#define PLL_INPUT_OSC_RATE  24000000
#endif

extern uint32_t __bss_start[];
extern uint32_t __bss_end[];

extern uint32_t __heap_begin[];
extern uint32_t __heap_end[];

extern uint32_t __share_log0_start__[];
extern uint32_t __share_log0_end__[];

extern uint32_t __share_log1_start__[];
extern uint32_t __share_log1_end__[];

extern uint32_t __share_log2_start__[];
extern uint32_t __share_log2_end__[];

extern uint32_t __share_log3_start__[];
extern uint32_t __share_log3_end__[];

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

#ifdef RT_USING_BACKLIGHT
#define LCD_BACKLIGHT_PWM           "pwm0"
#define LCD_BACKLIGHT_PWM_CHANNEL   1
#define LCD_BACKLIGHT_PWM_INVERT    1
#endif

rt_inline rt_uint32_t platform_get_gic_dist_base(void)
{
    return GIC_DISTRIBUTOR_BASE;
}

rt_inline rt_uint32_t platform_get_gic_cpu_base(void)
{
    return GIC_CPU_INTERFACE_BASE;
}

void rt_hw_board_init(void);

#ifndef RT_USING_SMP

#ifdef PRIMARY_CPU
void *rt_malloc_shmem(rt_size_t size);
void rt_free_shmem(void *ptr);
#endif

#ifdef RT_USING_LOGBUFFER
struct ringbuffer_t *get_log_ringbuffer(void);
#endif

#endif //RT_USING_SMP

#endif
