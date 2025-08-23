/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-10     Cliff      add board.h to this bsp
 *
 */

#ifndef __BOARD_H__
#define __BOARD_H__

// Internal SRAM memory size[Kbytes] <8-64>
// Default: 64
#define RK_SRAM_END             0x20100000
#ifndef RT_USING_UNCACHE_HEAP
#define RK_SYS_HEAP_END         RK_SRAM_END
#else
#define RK_UNCACHE_HEAP_SIZE    (0x2U << RT_UNCACHE_HEAP_ORDER)
#define RK_SYS_HEAP_END         (RK_SRAM_END - RK_UNCACHE_HEAP_SIZE)
#define RK_UNCACHE_HEAP_START   RK_SYS_HEAP_END
#endif

void rt_hw_board_init(void);

#endif
