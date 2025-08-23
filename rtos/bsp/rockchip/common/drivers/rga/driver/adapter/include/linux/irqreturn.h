/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Cerf Yu <cerf.yu@rock-chips.com>
 */

#ifndef __RTT_ADAPTER_LINUX_IRQRETURN_H__
#define __RTT_ADAPTER_LINUX_IRQRETURN_H__

/**
 * enum irqreturn
 * @IRQ_NONE        interrupt was not from this device
 * @IRQ_HANDLED     interrupt was handled by this device
 * @IRQ_WAKE_THREAD handler requests to wake the handler thread
 */
enum irqreturn
{
    IRQ_NONE        = (0 << 0),
    IRQ_HANDLED     = (1 << 0),
    IRQ_WAKE_THREAD     = (1 << 1),
};

typedef enum irqreturn irqreturn_t;
#define IRQ_RETVAL(x)   ((x) ? IRQ_HANDLED : IRQ_NONE)

#endif /* #ifndef __RTT_ADAPTER_LINUX_IRQRETURN_H__ */
