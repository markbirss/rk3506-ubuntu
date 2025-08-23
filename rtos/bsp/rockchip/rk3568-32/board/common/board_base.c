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
#include "cp15.h"
#include "mmu.h"
#include "gic.h"
#include "interrupt.h"
#include "hal_base.h"
#include "hal_bsp.h"
#include "drv_heap.h"

#ifdef RT_USING_AUDIO
#include "rk_audio.h"
#endif

#ifdef RT_USING_SMP
#define FIRM_MEM     SHARED_MEM
#else
#define FIRM_MEM     NORMAL_MEM
#endif

struct mem_desc platform_mem_desc[] =
{
#ifdef RT_USING_UNCACHE_HEAP
    {FIRMWARE_BASE, FIRMWARE_BASE + FIRMWARE_SIZE - 1, FIRMWARE_BASE, FIRM_MEM},
    {RT_UNCACHE_HEAP_BASE, RT_UNCACHE_HEAP_BASE + RT_UNCACHE_HEAP_SIZE - 1, RT_UNCACHE_HEAP_BASE, UNCACHED_MEM},
#else
    {FIRMWARE_BASE, FIRMWARE_BASE + DRAM_SIZE - 1, FIRMWARE_BASE, FIRM_MEM},
#endif
    {SHMEM_BASE, SHMEM_BASE + SHMEM_SIZE - 1, SHMEM_BASE, SHARED_MEM},
#ifdef LINUX_RPMSG_BASE
    {LINUX_RPMSG_BASE, LINUX_RPMSG_BASE + LINUX_RPMSG_SIZE - 1, LINUX_RPMSG_BASE, UNCACHED_MEM},
#endif
    {0xF0000000, 0xFE8C0000 - 1, 0xF0000000, DEVICE_MEM}
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

#ifdef RT_USING_SDIO
#include "drv_sdio.h"
#endif

extern void SysTick_Handler(void);
RT_WEAK void tick_isr(int vector, void *param)
{
    HAL_IncTick();
    rt_tick_increase();
#ifdef RT_USING_SYSTICK
    HAL_ARCHTIMER_SetCNTPTVAL(g_tick_load);
#else
    HAL_TIMER_ClrInt(TICK_TIMER);
#endif
    rt_hw_interrupt_clear_pending(TICK_IRQn);
}

void idle_wfi(void)
{
    asm volatile("wfi");
}

#ifdef HAL_GIC_MODULE_ENABLED
static struct GIC_AMP_IRQ_INIT_CFG irqsConfig[] =
{
    /* TODO: Config the irqs here. */

#ifdef HAL_GIC_WAIT_LINUX_INIT_ENABLED
    /* TODO: By default, TIMER1 is used for CPU1 */
    GIC_AMP_IRQ_CFG_ROUTE(TICK_IRQn, 0xd0, CPU_GET_AFFINITY(1, 0)),
#endif

    GIC_AMP_IRQ_CFG_ROUTE(UART2_IRQn, 0xd0, CPU_GET_AFFINITY(0, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(UART4_IRQn, 0xd0, CPU_GET_AFFINITY(1, 0)),

#ifdef HAL_GIC_WAIT_LINUX_INIT_ENABLED
    GIC_AMP_IRQ_CFG_ROUTE(AMP_CPUOFF_REQ_IRQ(3), 0xd0, CPU_GET_AFFINITY(3, 0)),
#ifdef HAL_GIC_PREEMPT_FEATURE_ENABLED
    GIC_AMP_IRQ_CFG_ROUTE(GIC_TOUCH_REQ_IRQ(3), 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif
#else
    GIC_AMP_IRQ_CFG_ROUTE(AMP_CPUOFF_REQ_IRQ(0), 0xd0, CPU_GET_AFFINITY(0, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(AMP_CPUOFF_REQ_IRQ(1), 0xd0, CPU_GET_AFFINITY(1, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(AMP_CPUOFF_REQ_IRQ(2), 0xd0, CPU_GET_AFFINITY(2, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(AMP_CPUOFF_REQ_IRQ(3), 0xd0, CPU_GET_AFFINITY(3, 0)),

#ifdef HAL_GIC_PREEMPT_FEATURE_ENABLED
    GIC_AMP_IRQ_CFG_ROUTE(GIC_TOUCH_REQ_IRQ(0), 0xd0, CPU_GET_AFFINITY(0, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(GIC_TOUCH_REQ_IRQ(1), 0xd0, CPU_GET_AFFINITY(1, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(GIC_TOUCH_REQ_IRQ(2), 0xd0, CPU_GET_AFFINITY(2, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(GIC_TOUCH_REQ_IRQ(3), 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif
#endif

#if defined(RT_USING_COMMON_TEST_RPMSG_LITE)
    GIC_AMP_IRQ_CFG_ROUTE(MBOX0_CH0_A2B_IRQn, 0xd0, CPU_GET_AFFINITY(0, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(MBOX0_CH2_A2B_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(MBOX0_CH3_A2B_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif

#if defined(RT_USING_COMMON_TEST_LINUX_RPMSG_LITE)
    GIC_AMP_IRQ_CFG_ROUTE(MBOX0_CH3_A2B_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
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

#ifdef RT_USING_SDIO
RT_WEAK struct rk_mmc_platform_data rk_mmc_table[] =
{
#ifdef RT_USING_SDIO0
    {
        .flags = MMCSD_BUSWIDTH_4 | MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ | MMCSD_SUP_HIGHSPEED,
        .irq = SDMMC0_IRQn,
        .base = MMC_BASE,
        .clk_id = CLK_SDMMC0,
        .freq_min = 100000,
        .freq_max = 50000000,
        .control_id = 0,
    },
#endif
    { /* sentinel */ },
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
    rt_hw_interrupt_clear_pending(TICK_IRQn);
    rt_hw_interrupt_install(TICK_IRQn, tick_isr, RT_NULL, "tick");
    rt_hw_interrupt_umask(TICK_IRQn);
    HAL_ARCHTIMER_SetCNTPCTL(1U);
}
#endif

#ifdef RT_USING_AUDIO
RT_WEAK const struct audio_card_desc rk_board_audio_cards[] =
{
#ifdef RT_USING_AUDIO_CARD_I2S1
    {
        .name = "i2s1",
        .dai =  I2STDM1,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_I2S2
    {
        .name = "i2s2",
        .dai =  I2STDM2,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_I2S3
    {
        .name = "i2s3",
        .dai =  I2STDM3,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
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
#ifdef HAL_GIC_MODULE_ENABLED
    HAL_GIC_Init(&irqConfig);
#endif
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
    rt_hw_interrupt_umask(RT_SCHEDULE_IPI);
#endif
}

#if defined(RT_USING_SMP)
#define PSCI_CPU_ON_AARCH32     0x84000003U

rt_uint64_t get_main_cpu_affval(void)
{
    return 0;
}

rt_uint32_t arm_gic_cpumask_to_affval(rt_uint32_t *cpu_mask, rt_uint32_t *cluster_id, rt_uint32_t *target_list)
{
    int i;

    if (*cpu_mask == 0)
    {
        return 0;
    }

    /* there is only one cluster in RK3568 */
    *cluster_id = 0;

    i = __rt_ffs(*cpu_mask);
    *target_list = i - 1;
    *cpu_mask &= ~(1 << (i - 1));

    return 1;
}

__attribute__((noinline)) void arm_psci_cpu_on(rt_ubase_t funcid, rt_ubase_t cpuid, rt_ubase_t entry)
{
    __asm volatile("smc #0" : : :);
}

void rt_hw_secondary_cpu_up(void)
{
    int i;
    extern void secondary_cpu_start(void);

    for (i = 1; i < RT_CPUS_NR; ++i)
    {
        /* TODO: if there is no delay, some cpu will fail to up */
        HAL_DelayMs(100);
        arm_psci_cpu_on(PSCI_CPU_ON_AARCH32, i, (rt_ubase_t)secondary_cpu_start);
    }
}

int rt_hw_cpu_id(void)
{
    int cpu_id;

    __asm__ volatile(
        "mrc p15, 0, %0, c0, c0, 5"
        :"=r"(cpu_id)
    );

    cpu_id &= 0xf00;
    return cpu_id >> 8;
};

void secondary_cpu_c_start(void)
{
    rt_kprintf("cpu%d bringup\n", rt_hw_cpu_id());
    rt_hw_vector_init();
    arm_gic_secondary_cpu_init();
    generic_timer_config();
    rt_hw_spin_lock(&_cpus_lock);
    rt_hw_interrupt_umask(RT_SCHEDULE_IPI);
    rt_system_scheduler_start();
}

void rt_hw_secondary_cpu_idle_exec(void)
{
    __WFE();
}
#endif
