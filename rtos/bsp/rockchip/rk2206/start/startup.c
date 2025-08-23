/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-10     Cliff      first implementation
 *
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"
#include "drv_heap.h"

/*@{*/

extern int  rt_application_init(void);

#if defined(__CC_ARM) || defined(__CLANG_ARM)
extern int Image$$RW_RAM$$ZI$$Limit;
#define RK_SRAM_BEGIN    (&Image$$RW_RAM$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define RK_SRAM_BEGIN    (__segment_end("HEAP"))
#else
extern int __heap_start__;
#define RK_SRAM_BEGIN    (&__heap_start__)
#ifdef RT_USING_LARGE_HEAP
extern int __large_start__, __large_end__;
#define RK_LARGE_BEGIN   (&__large_start__)
#define RK_LARGE_END     (&__large_end__)
#endif
#endif

/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t *file, uint32_t line)
{
    rt_kprintf("\n\r Wrong parameter value detected on\r\n");
    rt_kprintf("       file  %s\r\n", file);
    rt_kprintf("       line  %d\r\n", line);

    while (1)
        ;
}

#ifdef RT_USING_CMBACKTRACE
extern int rt_cm_backtrace_init(void);
#endif

/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
#ifdef RT_USING_CMBACKTRACE
    /* init cmbacktrace */
    rt_cm_backtrace_init();
#endif

    /* init interrupt */
    rt_hw_interrupt_init();

    /* init board */
    rt_hw_board_init();

    /* show version */
    rt_show_version();

    /* init tick */
    rt_system_tick_init();

    /* init kernel object */
    rt_system_object_init();

    /* init timer system */
    rt_system_timer_init();

#ifdef RT_USING_HEAP
    rt_system_heap_init((void *)RK_SRAM_BEGIN, (void *)RK_SYS_HEAP_END);
#endif

#ifdef RT_USING_UNCACHE_HEAP
    rt_uncache_heap_init((void *)RK_UNCACHE_HEAP_START, (void *)RK_SRAM_END);
#endif

#ifdef RT_USING_LARGE_HEAP
    rt_large_heap_init((void *)RK_LARGE_BEGIN, (void *)RK_LARGE_END);
#endif

    /* init scheduler system */
    rt_system_scheduler_init();

    /* init application */
    rt_application_init();

    /* init timer thread */
    rt_system_timer_thread_init();

    /* init idle thread */
    rt_thread_idle_init();

    /* start scheduler */
    rt_system_scheduler_start();

    /* never reach here */
    return;
}

int Main(void)
{
    /* disable interrupt first */
    rt_hw_interrupt_disable();

    /* startup RT-Thread RTOS */
    rtthread_startup();

    return 0;
}
