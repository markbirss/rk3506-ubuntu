/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-09     Cliff Chen   first implementation
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"
#include "cp15.h"
#include "mmu.h"
#include "gic.h"
#include "interrupt.h"
#include "hal_base.h"
#include "hal_bsp.h"
#include "drv_heap.h"


#ifdef RT_USING_PIN
#include "iomux.h"
#endif

#ifdef RT_USING_UART
#include "drv_uart.h"
#endif

#ifdef RT_USING_CRU
#include "drv_clock.h"

RT_WEAK const struct clk_init clk_inits[] =
{
    INIT_CLK("PLL_APLL", PLL_APLL, 816 * MHZ),
    { /* sentinel */ },
};

RT_WEAK const struct clk_unused clks_unused[] =
{
    { /* sentinel */ },
};
#endif

#ifdef RT_USING_AUDIO
#include "rk_audio.h"
#endif

#ifdef RT_USING_SYSTICK
#define TICK_IRQn CNTPNS_IRQn
static uint32_t g_tick_load;
#endif

#if defined(RT_USING_UART0)
RT_WEAK const struct uart_board g_uart0_board =
{
    .baud_rate = UART_BR_115200,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart0",
};
#endif /* RT_USING_UART0 */
#if defined(RT_USING_UART2)
RT_WEAK const struct uart_board g_uart2_board =
{
    .baud_rate = UART_BR_115200,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart2",
};
#endif /* RT_USING_UART0 */

struct mem_desc platform_mem_desc[] =
{
    {FIRMWARE_BASE, FIRMWARE_BASE + DRAM_SIZE, FIRMWARE_BASE, NORMAL_MEM},
    {0xFF000000, 0xFFC00000, 0xFF000000, DEVICE_MEM} /* DEVICE */
};

const rt_uint32_t platform_mem_desc_size = sizeof(platform_mem_desc) / sizeof(platform_mem_desc[0]);

RT_WEAK void tick_isr(int vector, void *param)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_IncTick();

    rt_tick_increase();
#ifdef RT_USING_SYSTICK
    HAL_ARCHTIMER_SetCNTPTVAL(g_tick_load);
#else
    HAL_TIMER_ClrInt(TICK_TIMER);
#endif

    /* leave interrupt */
    rt_interrupt_leave();
}

void idle_wfi(void)
{
    asm volatile("wfi");
}

#ifdef RT_USING_SYSTICK

static void generic_timer_config(void)
{
    uint32_t freq;

    freq = HAL_ARCHTIMER_GetCNTFRQ();

    // Calculate load value
    g_tick_load = (freq / RT_TICK_PER_SECOND) - 1U;

    HAL_ARCHTIMER_SetCNTPCTL(0U);
    HAL_ARCHTIMER_SetCNTPTVAL(g_tick_load);
    rt_hw_interrupt_clear_pending(TICK_IRQn);
    rt_hw_interrupt_install(TICK_IRQn, tick_isr, RT_NULL, "tick");
    rt_hw_interrupt_umask(TICK_IRQn);
    HAL_ARCHTIMER_SetCNTPCTL(1U);
}
#endif

/**
 *  The work around for Cluster ID of rv1103 is not zero.
 */
int rt_hw_cpu_id(void)
{
    return 0;
}

/**
 *  Initialize the Hardware related stuffs. Called from rtthread_startup()
 *  after interrupt disabled.
 */
void rt_hw_board_init(void)
{
    /* HAL_Init */
    HAL_Init();

    /* hal bsp init */
    BSP_Init();

    /* initialize hardware interrupt */
    rt_hw_interrupt_init();

    /* tick init */
    HAL_SetTickFreq(1000 / RT_TICK_PER_SECOND);

#ifdef RT_USING_SYSTICK
    generic_timer_config();
#else
    rt_hw_interrupt_install(TICK_IRQn, tick_isr, RT_NULL, "tick");
    rt_hw_interrupt_umask(TICK_IRQn);
    HAL_TIMER_Init(TICK_TIMER, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(TICK_TIMER, (PLL_INPUT_OSC_RATE / RT_TICK_PER_SECOND) - 1);
    HAL_TIMER_Start_IT(TICK_TIMER);
#endif

#ifdef RT_USING_CRU
    clk_init(clk_inits, false);
    /* disable some clks when init, and enabled by device when needed */
    clk_disable_unused(clks_unused);
#endif

#ifdef RT_USING_PIN
    rt_hw_iomux_config();
#endif

    /* initialize uart */
#ifdef RT_USING_UART
    rt_hw_usart_init();
#endif

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

#ifdef RT_USING_HEAP
    /* initialize memory system */
    rt_system_heap_init(RT_HW_HEAP_BEGIN, RT_HW_HEAP_END);
#endif

    rt_kprintf("base_mem: BASE = 0x%08x, SIZE = 0x%08x\n", FIRMWARE_BASE, DRAM_SIZE);

    rt_thread_idle_sethook(idle_wfi);

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}

