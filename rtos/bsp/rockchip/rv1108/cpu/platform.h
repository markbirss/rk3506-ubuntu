/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-03-22     quanzhao     first version
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

/* for 'rt_inline' */
#include <rtdef.h>
/* SOC-relative definitions */
#include <soc.h>

/* the maximum number of gic */
#define ARM_GIC_MAX_NR    1

/* the maximum number of interrupts */
#define ARM_GIC_NR_IRQS    NUM_INTERRUPTS

/* the maximum entries of the exception table */
#define MAX_HANDLERS    NUM_INTERRUPTS

/* the basic constants and interfaces needed by gic */
rt_inline rt_uint32_t platform_get_gic_dist_base(void)
{
    return GIC_BASE + 0x1000;
}

rt_inline rt_uint32_t platform_get_gic_cpu_base(void)
{
    return GIC_BASE + 0x2000;;
}

#define GIC_IRQ_START   0
#define GIC_ACK_INTID_MASK  0x000003ff

/* the definition needed by gic.c */
#define __REG32(x)  (*((volatile unsigned int *)(x)))

#endif  /* __PLATFORM_H__ */
