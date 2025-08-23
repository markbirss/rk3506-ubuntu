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

#if defined(RT_USING_SMP)
struct mem_desc platform_mem_desc[] =
{
#ifdef RT_USING_UNCACHE_HEAP
    {FIRMWARE_BASE, FIRMWARE_BASE + FIRMWARE_SIZE, FIRMWARE_BASE, SHARED_MEM},
    {RT_UNCACHE_HEAP_BASE, RT_UNCACHE_HEAP_BASE + RT_UNCACHE_HEAP_SIZE, RT_UNCACHE_HEAP_BASE, UNCACHED_MEM},
#else
    {FIRMWARE_BASE, FIRMWARE_BASE + DRAM_SIZE, FIRMWARE_BASE, SHARED_MEM},
#endif
    {0xFF000000, 0xFFFF0000, 0xFF000000, DEVICE_MEM} /* DEVICE */
};
#else
struct mem_desc platform_mem_desc[] =
{
#ifdef RT_USING_UNCACHE_HEAP
    {FIRMWARE_BASE, FIRMWARE_BASE + FIRMWARE_SIZE, FIRMWARE_BASE, NORMAL_MEM},
    {RT_UNCACHE_HEAP_BASE, RT_UNCACHE_HEAP_BASE + RT_UNCACHE_HEAP_SIZE, RT_UNCACHE_HEAP_BASE, UNCACHED_MEM},
#else
    {FIRMWARE_BASE, FIRMWARE_BASE + DRAM_SIZE, FIRMWARE_BASE, NORMAL_MEM},
#endif
    {SHMEM_BASE, SHMEM_BASE + SHMEM_SIZE, SHMEM_BASE, SHARED_MEM},
    {0xFF000000, 0xFFFF0000, 0xFF000000, DEVICE_MEM} /* DEVICE */
};
#endif

const rt_uint32_t platform_mem_desc_size = sizeof(platform_mem_desc) / sizeof(platform_mem_desc[0]);

RT_WEAK void tick_isr(int vector, void *param)
{
    /* enter interrupt */
    rt_interrupt_enter();

#ifdef RT_USING_SMP
    if (rt_hw_cpu_id() == 0)
        HAL_IncTick();
#else
    HAL_IncTick();
#endif

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

static struct GIC_AMP_IRQ_INIT_CFG irqsConfig[] =
{
    /* Config the irqs here. */
    // todo...

    GIC_AMP_IRQ_CFG_ROUTE(0, 0, CPU_GET_AFFINITY(1, 0)),   /* sentinel */
};

static struct GIC_IRQ_AMP_CTRL irqConfig =
{
    .cpuAff = CPU_GET_AFFINITY(1, 0),
    .defPrio = 0xd0,
    .defRouteAff = CPU_GET_AFFINITY(1, 0),
    .irqsCfg = &irqsConfig[0],
};

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

#ifdef RT_USING_AUDIO
RT_WEAK const struct audio_card_desc rk_board_audio_cards[] =
{
#ifdef RT_USING_AUDIO_CARD_I2S0
    {
        .name = "i2s0",
        .dai =  I2STDM0,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_FALSE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_ACODEC
    {
        .name = "acodec",
        .dai =  I2STDM2,
        .codec = ACODEC,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
        .rxMap = 0x0321,
    },
#endif

    { /* sentinel */ }
};

#endif

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
    HAL_GIC_Init(&irqConfig);
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

    /* initialize uart */
#ifdef RT_USING_UART
    rt_hw_usart_init();
#endif

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

#ifdef RT_USING_PIN
    rt_hw_iomux_config();
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
#define PSCI_CPU_ON_AARCH32		0x84000003U

void arm_psci_cpu_on(rt_ubase_t funcid, rt_ubase_t cpuid, rt_ubase_t entry)
{
    __asm volatile("smc #0" : : :);
}

void rt_hw_secondary_cpu_up(void)
{
    int i;
    extern void secondary_cpu_start(void);

    /* TODO: Fix cpu1 und exception */
    for (i = RT_CPUS_NR - 1; i > 0; --i)
    {
        arm_psci_cpu_on(PSCI_CPU_ON_AARCH32, i, (rt_ubase_t)secondary_cpu_start);
    }
}

void secondary_cpu_c_start(void)
{
    uint32_t id;

    id = rt_hw_cpu_id();
    rt_kprintf("cpu %d startup.\n",id);
    rt_hw_vector_init();
    rt_hw_spin_lock(&_cpus_lock);
    arm_gic_cpu_init(0, GIC_CPU_INTERFACE_BASE);
    generic_timer_config();
    rt_system_scheduler_start();
}

void rt_hw_secondary_cpu_idle_exec(void)
{
    __WFE();
}
#endif
