/**
  * Copyright (c) 2020 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    int_mux.c
  * @author  Jason Zhu
  * @version V0.1
  * @date    22-June-2021
  * @brief
  *
  ******************************************************************************
  */

#include <rtthread.h>
#include "int_mux.h"

void rt_hw_interrupt_active(int vector)
{
    unsigned int index, set;
    unsigned int temp;

    index = vector / 8;
    set = vector % 8;
    temp = *(unsigned int *)(INT_MUX + index * 4);
    *(unsigned int *)(INT_MUX + index * 4) = (1 << set) | temp;
}

rt_uint32_t rt_hw_interrupt_get_active(void)
{
    rt_uint32_t cause = -1;
    int i, n;

    /* The Interrupt Multiplexer will select 4 interrupts from
     * 256 interrupts using round robin algorithm. You can configure
     * corresponding mask bit for each interrupt. It’s a 2-step
     * operation to obtain the interrupt in service. First you
     * should read the INT_FLAG_LEVEL2 to obtain the correct
     * INT_FLAG_GROUP, then read the corresponding INT_FLAG_GROUP
     * to obtain the interrupt in service.
     */
    n = *(unsigned int *)(INT_MUX + INT_MUX_FLAG_LEVEL2);
    for (i = 0; i < 32; i++)
    {
        if (n & 0x1)
            break;
        n = n >> 1;
    }

    if (i < INT_GROUP)
    {
        switch (*(unsigned int *)(INT_MUX + INT_MUX_GROUP + i * 4))
        {
        case 0x01:
            cause = i * 8 ;
            break;
        case 0x02:
            cause = i * 8 + 1;
            break;
        case 0x04:
            cause = i * 8 + 2;
            break;
        case 0x08:
            cause = i * 8 + 3;
            break;
        case 0x10:
            cause = i * 8 + 4;
            break;
        case 0x20:
            cause = i * 8 + 5;
            break;
        case 0x40:
            cause = i * 8 + 6;
            break;
        case 0x80:
            cause = i * 8 + 7;
            break;
        default:
            cause = -1;
        }
    }

    return cause;
}
