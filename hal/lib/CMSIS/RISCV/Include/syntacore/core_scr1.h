/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#ifndef __CORE_SCR1_H
#define __CORE_SCR1_H

/* ###########################  Core Function Access  ########################### */

/**
  \brief   Enable IRQ Interrupts
  \details Enables IRQ interrupts by clearing the I-bit in the mstatus.
           Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __enable_irq(void)
{
    __ASM volatile("csrrsi a0, mstatus, 8");
}

/**
  \brief   Disable IRQ Interrupts
  \details Disables IRQ interrupts by setting the I-bit in the mstatus.
  Can only be executed in Privileged modes.
 */
__STATIC_FORCEINLINE void __disable_irq(void)
{
    __ASM volatile("csrrci a0, mstatus, 8");
}

#endif /* end of __CORE_SCR1_H */
