/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-01-26     Cliff Chen   first implementation
 */

#ifndef __GIC_PORT_H
#define __GIC_PORT_H

#define GIC_IRQ_START               0
#define ARM_GIC_NR_IRQS             NUM_INTERRUPTS
#define ARM_GIC_MAX_NR              8
#define GIC_ACK_INTID_MASK          0x000003ff

rt_inline rt_uint64_t platform_get_gic_dist_base(void)
{
    return GIC_DISTRIBUTOR_BASE;
}

rt_inline rt_uint64_t platform_get_gic_cpu_base(void)
{
    return GIC_CPU_INTERFACE_BASE;
}

#endif
