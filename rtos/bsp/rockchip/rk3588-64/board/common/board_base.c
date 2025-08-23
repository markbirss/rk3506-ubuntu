/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-12     Steven Liu   first implementation
 * 2022-08-22     Steven Liu   update project structure and related code
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"
#include "mmu.h"
#include "interrupt.h"
#include "gicv3.h"
#include "hal_base.h"
#include "hal_bsp.h"
#include "drv_heap.h"

uint32_t SystemCoreClock = 1008000000U;

struct mem_desc platform_mem_desc[] =
{
#ifdef RT_USING_UNCACHE_HEAP
    {FIRMWARE_BASE, FIRMWARE_BASE + FIRMWARE_SIZE, FIRMWARE_BASE, NORMAL_MEM},
    {RT_UNCACHE_HEAP_BASE, RT_UNCACHE_HEAP_BASE + RT_UNCACHE_HEAP_SIZE, RT_UNCACHE_HEAP_BASE, NORMAL_NOCACHE_MEM},
#else
    {FIRMWARE_BASE, FIRMWARE_BASE + DRAM_SIZE, FIRMWARE_BASE, NORMAL_MEM},
#endif
    {SHMEM_BASE, SHMEM_BASE + SHMEM_SIZE, SHMEM_BASE, NORMAL_MEM},
#ifdef LINUX_RPMSG_BASE
    {LINUX_RPMSG_BASE, LINUX_RPMSG_BASE + LINUX_RPMSG_SIZE, LINUX_RPMSG_BASE, NORMAL_NOCACHE_MEM},
#endif
    {0xF0000000, 0xFF000000, 0xF0000000, DEVICE_MEM}
};

const rt_uint32_t platform_mem_desc_size = sizeof(platform_mem_desc) / sizeof(platform_mem_desc[0]);

#ifdef RT_USING_PIN
#include "iomux.h"
#endif

#ifdef RT_USING_UART
#include "drv_uart.h"
#endif

#ifdef RT_USING_SYSTICK
#define TICK_IRQn CNTPNS_IRQn
static uint32_t g_tick_load;
#endif

#if defined(RT_USING_UART2)
RT_WEAK const struct uart_board g_uart2_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart2",
};
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART4)
RT_WEAK const struct uart_board g_uart4_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart4",
};
#endif /* RT_USING_UART4 */

#if defined(RT_USING_UART5)
RT_WEAK const struct uart_board g_uart5_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart5",
};
#endif /* RT_USING_UART4 */

extern void SysTick_Handler(void);
RT_WEAK void tick_isr(int vector, void *param)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_IncTick();
    rt_tick_increase();
#ifdef RT_USING_SYSTICK
    HAL_GIC_ClearPending(TICK_IRQn);
    HAL_GIC_EndOfInterrupt(TICK_IRQn);
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

#ifdef HAL_GIC_MODULE_ENABLED
static struct GIC_AMP_IRQ_INIT_CFG irqsConfig[] =
{
    /* TODO: Config the irqs here. */

    /* TODO: By default, UART2 is used for master core CPU0, and UART5 is used for remote core CPU3 */
#ifdef RT_USING_UART2
    GIC_AMP_IRQ_CFG_ROUTE(UART2_IRQn, 0xd0, CPU_GET_AFFINITY(0, 0)),
#endif
#ifdef RT_USING_UART5
    GIC_AMP_IRQ_CFG_ROUTE(UART5_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif
    /* DMAC0 for I2S0/I2S1 and DMAC1 for I2S2/I2S3 */
#ifdef RT_USING_RT_USING_DMA0
    GIC_AMP_IRQ_CFG_ROUTE(DMAC0_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif
#ifdef RT_USING_RT_USING_DMA1
    GIC_AMP_IRQ_CFG_ROUTE(DMAC1_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif
#ifdef RT_USING_PIN
    GIC_AMP_IRQ_CFG_ROUTE(GPIO0_EXP_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(GPIO1_EXP_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(GPIO2_EXP_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(GPIO3_EXP_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(GPIO4_EXP_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif

#ifdef HAL_GIC_WAIT_LINUX_INIT_ENABLED
    /* route tick to cpu3 when GIC init by linux */
    GIC_AMP_IRQ_CFG_ROUTE(TICK_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif

    GIC_AMP_IRQ_CFG_ROUTE(0, 0, CPU_GET_AFFINITY(0, 0)),   /* sentinel */
};

static struct GIC_IRQ_AMP_CTRL irqConfig =
{
    .cpuAff = CPU_GET_AFFINITY(0, 0),
    .defPrio = 0xd0,
    .defRouteAff = CPU_GET_AFFINITY(0, 0),
    .irqsCfg = &irqsConfig[0],
};
#endif

#ifdef RT_USING_SYSTICK

static void generic_timer_config(void)
{
    uint32_t freq;

    freq = HAL_ARCHTIMER_GetCNTFRQ();

    // Calculate load value
    g_tick_load = (freq / RT_TICK_PER_SECOND) - 1U;

    HAL_ARCHTIMER_SetCNTPCTL(0U);
    HAL_ARCHTIMER_SetCNTPTVAL(g_tick_load);
    HAL_GIC_ClearPending(TICK_IRQn);
    rt_hw_interrupt_install(TICK_IRQn, tick_isr, RT_NULL, "tick");
    rt_hw_interrupt_umask(TICK_IRQn);
    HAL_ARCHTIMER_SetCNTPCTL(1U);
}
#endif

/**
 *  Initialize the Hardware related stuffs. Called from rtthread_startup()
 *  after interrupt disabled.
 */
void rt_hw_board_init(void)
{
    rt_hw_init_mmu_table(platform_mem_desc, platform_mem_desc_size);
    rt_hw_mmu_init();

    /* HAL_Init */
    HAL_Init();

    /* HAL bsp init */
    BSP_Init();

    /* HAL spinlock init */
    HAL_SPINLOCK_Init(HAL_CPU_TOPOLOGY_GetCurrentCpuId() + 1);

    /* initialize hardware interrupt */
#ifdef HAL_GIC_MODULE_ENABLED
    HAL_GIC_Init(&irqConfig);
#endif
    rt_hw_interrupt_init();

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

#ifdef RT_USING_HEAP
    /* initialize memory system */
    rt_system_heap_init(RT_HW_HEAP_BEGIN, RT_HW_HEAP_END);
#endif

#ifdef RT_USING_UNCACHE_HEAP
#if ((RT_UNCACHE_HEAP_BASE & 0x000fffff) || (RT_UNCACHE_HEAP_SIZE & 0x000fffff) || (DRAM_SIZE <= RT_UNCACHE_HEAP_SIZE))
#error "Uncache heap base and size should be at least 1M align, Uncache heap size must less than dram size!";
#endif
    rt_kprintf("base_mem: BASE = 0x%08x, SIZE = 0x%08x\n", FIRMWARE_BASE, FIRMWARE_SIZE);
    rt_kprintf("uncached_heap: BASE = 0x%08x, SIZE = 0x%08x\n", RT_UNCACHE_HEAP_BASE, RT_UNCACHE_HEAP_SIZE);
    rt_uncache_heap_init((void *)RT_UNCACHE_HEAP_BASE, (void *)(RT_UNCACHE_HEAP_BASE + RT_UNCACHE_HEAP_SIZE));
#else
    rt_kprintf("base_mem: BASE = 0x%08x, SIZE = 0x%08x\n", FIRMWARE_BASE, DRAM_SIZE);
#endif
    rt_kprintf("share_mem: BASE = 0x%08x, SIZE = 0x%08x\n", SHMEM_BASE, SHMEM_SIZE);

    rt_thread_idle_sethook(idle_wfi);

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#ifdef RT_USING_SMP
    /* install IPI handle */
    rt_hw_ipi_handler_install(RT_SCHEDULE_IPI, rt_scheduler_ipi_handler);
#endif

}

#if defined(RT_USING_SMP)
#include "psci.h"

/* The more common mpidr_el1 table, redefine it in BSP if it is in other cases */
rt_uint64_t rt_cpu_mpidr_early[] =
{
    [0] = 0x81000000,
    [1] = 0x81000100,
    [2] = 0x81000200,
    [3] = 0x81000300,
    [4] = 0x81000400,
    [5] = 0x81000500,
    [6] = 0x81000600,
    [7] = 0x81000700,
    [RT_CPUS_NR] = 0
};

void rt_hw_secondary_cpu_up(void)
{
    int i;
    extern void secondary_cpu_start(void);
    extern rt_uint64_t rt_cpu_mpidr_early[];

    /* TODO: Fix cpu1 und exception */
    for (i = 1; i < RT_CPUS_NR; ++i)
    {
        HAL_CPUDelayUs(10000);
        arm_psci_cpu_on(rt_cpu_mpidr_early[i], (rt_ubase_t)(secondary_cpu_start));
    }
}

void secondary_cpu_c_start(void)
{
    rt_hw_mmu_init();
    rt_hw_spin_lock(&_cpus_lock);

    arm_gic_cpu_init(0, platform_get_gic_cpu_base());
#ifdef BSP_USING_GICV3
    arm_gic_redist_init(0, platform_get_gic_redist_base());
#endif
    rt_hw_vector_init();
    generic_timer_config();
    arm_gic_umask(0, RT_SCHEDULE_IPI);

    rt_kprintf("\rcall cpu %d on success\n", rt_hw_cpu_id());

    rt_system_scheduler_start();
}

void rt_hw_secondary_cpu_idle_exec(void)
{
    __WFE();
}
#endif
