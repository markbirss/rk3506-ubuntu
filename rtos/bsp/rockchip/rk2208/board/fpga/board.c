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
#include "drv_clock.h"
#include "drv_uart.h"
#include "drv_cache.h"
#include "drv_heap.h"
#include "hal_base.h"
#include "hal_bsp.h"

static const struct clk_init clk_inits[] =
{
    INIT_CLK("PLL_GPLL", PLL_GPLL, 1188000000),
    INIT_CLK("PLL_CPLL", PLL_CPLL, 1000000000),
    INIT_CLK("HCLK_M4", HCLK_M4, 400000000),
    INIT_CLK("ACLK_DSP", ACLK_DSP, 300000000),
    INIT_CLK("ACLK_LOGIC", ACLK_LOGIC, 300000000),
    INIT_CLK("HCLK_LOGIC", HCLK_LOGIC, 150000000),
    INIT_CLK("PCLK_LOGIC", PCLK_LOGIC, 150000000),
    { /* sentinel */ },
};

#if defined(RT_USING_UART0)
const struct uart_board g_uart0_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart0",
};
#endif /* RT_USING_UART0 */

#if defined(RT_USING_UART1)
const struct uart_board g_uart1_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart1",
};
#endif /* RT_USING_UART1 */

static void systick_isr(int vector, void *param)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_SYSTICK_IRQHandler();
    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

#ifdef RT_USING_UNCACHE_HEAP
extern const rt_uint32_t __uncache_heap_start__[];
extern const rt_uint32_t __uncache_heap_end__[];
#endif
static void mpu_init(void)
{
#ifdef RT_USING_UNCACHE_HEAP
    /* text section: non shared, rw, np, exec, cachable */
    ARM_MPU_SetRegion(0, ARM_MPU_RBAR(0x00400000, 0U, 0U, 1U, 0U), ARM_MPU_RLAR(((rt_uint32_t)__uncache_heap_start__ - 32), 0U));
    /* device section: shared, rw, np, xn */
    ARM_MPU_SetRegion(1, ARM_MPU_RBAR(0x40000000U, 1U, 0U, 1U, 1U), ARM_MPU_RLAR(0x45000000U, 1U));
    /* uncache heap: non shared, rw, np, exec, uncachable */
    ARM_MPU_SetRegion(2, ARM_MPU_RBAR((rt_uint32_t)__uncache_heap_start__, 0U, 0U, 1U, 1U), ARM_MPU_RLAR(0x00500000, 2U));

    /* cachable normal memory*/
    ARM_MPU_SetMemAttr(0, ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(0, 0, 1, 1), ARM_MPU_SH_INNER));
    /* device memory */
    ARM_MPU_SetMemAttr(1, ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRnE));
    /* uncachable normal memory */
    ARM_MPU_SetMemAttr(2, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));
#else
    static const ARM_MPU_Region_t table[] =
    {
        { .RBAR = ARM_MPU_RBAR(0x00400000U, 0U, 0U, 1U, 0U), .RLAR = ARM_MPU_RLAR(0x00500000U, 0U) },
        { .RBAR = ARM_MPU_RBAR(0x40000000U, 1U, 0U, 1U, 1U), .RLAR = ARM_MPU_RLAR(0x45000000U, 1U) },
        {},
    };

    ARM_MPU_Load(0U, &(table[0]), 2U);

    ARM_MPU_SetMemAttr(0, ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(0, 0, 1, 1), ARM_MPU_SH_INNER));
    ARM_MPU_SetMemAttr(1, ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRnE));
#endif
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
}

#ifdef __ARMCC_VERSION
extern const rt_uint32_t Image$$ARM_LIB_HEAP$$Limit[];
extern const rt_uint32_t Image$$ARM_LIB_STACK$$Base[];
#define HEAP_START       Image$$ARM_LIB_HEAP$$Limit
#define HEAP_END         Image$$ARM_LIB_STACK$$Base
#else
extern const rt_uint32_t __heap_begin__[];
extern const rt_uint32_t __heap_end__[];
#define HEAP_START       (__heap_begin__)
#define HEAP_END         (__heap_end__)
#endif

/**
 * This function will initial Pisces board.
 */
void rt_hw_board_init()
{
    /* mpu init */
    mpu_init();

    /* HAL_Init */
    HAL_Init();

    /* hal bsp init */
    BSP_Init();

    /* System tick init */
    rt_hw_interrupt_install(SysTick_IRQn, systick_isr, RT_NULL, "tick");
    HAL_SetTickFreq(1000 / RT_TICK_PER_SECOND);
    HAL_SYSTICK_Init();

    rt_system_heap_init((void *)HEAP_START, (void *)HEAP_END);
#ifdef RT_USING_UNCACHE_HEAP
    rt_uncache_heap_init((void *)__uncache_heap_start__, (void *)__uncache_heap_end__);
#endif

    /* Initial usart deriver, and set console device */
    rt_hw_usart_init();

    rt_hw_cpu_cache_init();

    //clk_init(clk_inits, true);

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}

#ifdef __ARMCC_VERSION
extern int $Super$$main(void);
void _start(void)
{
    $Super$$main();
}
#else
extern int entry(void);
void _start(void)
{
    entry();
}
#endif
