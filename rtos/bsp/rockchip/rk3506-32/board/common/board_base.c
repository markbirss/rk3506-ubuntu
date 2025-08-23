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
#include <rtdevice.h>

#include "board.h"
#include "cp15.h"
#include "mmu.h"
#include "gic.h"
#include "interrupt.h"
#include "hal_base.h"
#include "hal_bsp.h"
#include "drv_heap.h"
#include "drv_thermal.h"


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
    INIT_CLK("PLL_GPLL", PLL_GPLL, 0),
    INIT_CLK("PLL_V0PLL", PLL_V0PLL, 0),
    INIT_CLK("PLL_V1PLL", PLL_V1PLL, 0),
    INIT_CLK("CLK_GPLL_DIV", CLK_GPLL_DIV, 0),
    INIT_CLK("CLK_GPLL_DIV_100M", CLK_GPLL_DIV_100M, 0),
    INIT_CLK("CLK_V0PLL_DIV", CLK_V0PLL_DIV, 0),
    INIT_CLK("CLK_V1PLL_DIV", CLK_V1PLL_DIV, 0),
    INIT_CLK("ACLK_BUS_ROOT", ACLK_BUS_ROOT, 0),
    INIT_CLK("HCLK_BUS_ROOT", HCLK_BUS_ROOT, 0),
    INIT_CLK("PCLK_BUS_ROOT", PCLK_BUS_ROOT, 0),
    INIT_CLK("ACLK_HSPERI_ROOT", ACLK_HSPERI_ROOT, 0),
    INIT_CLK("HCLK_LSPERI_ROOT", HCLK_LSPERI_ROOT, 0),

    INIT_CLK("CLK_INT_VOICE0", CLK_INT_VOICE0, 0),
    INIT_CLK("CLK_INT_VOICE1", CLK_INT_VOICE1, 0),
    INIT_CLK("CLK_INT_VOICE2", CLK_INT_VOICE2, 0),
    INIT_CLK("CLK_FRAC_COMMON0", CLK_FRAC_COMMON0, 0),
    INIT_CLK("CLK_FRAC_COMMON1", CLK_FRAC_COMMON1, 0),
    INIT_CLK("CLK_FRAC_COMMON2", CLK_FRAC_COMMON2, 0),
    INIT_CLK("CLK_FRAC_UART0", CLK_FRAC_UART0, 96000000),
    INIT_CLK("CLK_FRAC_UART1", CLK_FRAC_UART1, 128000000),
    INIT_CLK("CLK_FRAC_VOICE0", CLK_FRAC_VOICE0, 0),
    INIT_CLK("CLK_FRAC_VOICE1", CLK_FRAC_VOICE1, 0),
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

#ifdef RT_USING_SDIO
#include "drv_sdio.h"
#endif

#ifdef RT_USING_USB_DEVICE
#include "drv_usbd.h"
#endif

#ifdef RT_USING_SYSTICK
#define TICK_IRQn CNTPNS_IRQn
static uint32_t g_tick_load;
#endif

#if defined(RT_USING_TSADC)
RT_WEAK const struct tsadc_init g_tsadc_init =
{
    .chn_id = {0},
    .chn_num = 1,
    .polarity = TSHUT_LOW_ACTIVE,
    .mode = TSHUT_MODE_CRU,
};
#endif /* RT_USING_TSADC */

#if defined(RT_USING_UART0)
RT_WEAK const struct uart_board g_uart0_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart0",
};
#endif /* RT_USING_UART0 */
#if defined(RT_USING_UART4)
RT_WEAK const struct uart_board g_uart4_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart4",
};
#endif /* RT_USING_UART4 */

#ifdef RT_USING_USB_DEVICE
RT_WEAK struct ep_id g_usb_ep_pool[] =
{
    { 0x0,  USB_EP_ATTR_CONTROL,    USB_DIR_INOUT,  64,   ID_ASSIGNED   },
    { 0x1,  USB_EP_ATTR_BULK,       USB_DIR_IN,     1024, ID_UNASSIGNED },
    { 0x2,  USB_EP_ATTR_BULK,       USB_DIR_OUT,    512,  ID_UNASSIGNED },
    { 0x3,  USB_EP_ATTR_ISOC,       USB_DIR_IN,     1024, ID_UNASSIGNED },
    { 0x4,  USB_EP_ATTR_ISOC,       USB_DIR_OUT,    1024,  ID_UNASSIGNED },
    { 0x5,  USB_EP_ATTR_INT,        USB_DIR_IN,     64,   ID_UNASSIGNED },
    { 0x6,  USB_EP_ATTR_INT,        USB_DIR_OUT,    64,   ID_UNASSIGNED },
    { 0xFF, USB_EP_ATTR_TYPE_MASK,  USB_DIR_MASK,   0,    ID_ASSIGNED   },
};
#endif

#if defined(RT_USING_SMP)
struct mem_desc platform_mem_desc[] =
{
#ifdef RT_USING_UNCACHE_HEAP
    {FIRMWARE_BASE, FIRMWARE_BASE + FIRMWARE_SIZE - 1, FIRMWARE_BASE, SHARED_MEM},
    {RT_UNCACHE_HEAP_BASE, RT_UNCACHE_HEAP_BASE + RT_UNCACHE_HEAP_SIZE - 1, RT_UNCACHE_HEAP_BASE, UNCACHED_MEM},
#else
    {FIRMWARE_BASE, FIRMWARE_BASE + DRAM_SIZE - 1, FIRMWARE_BASE, SHARED_MEM},
#endif
#ifdef RT_USING_DSMC_HOST
    {DSMC_MEM_BASE, 0xFF000000 - 1, DSMC_MEM_BASE, UNCACHED_MEM}, /* DSMC HOST memory space */
#endif
#ifdef RT_USING_DSMC_SLAVE
    {DSMC_SLAVE_MEM_BASE, DSMC_SLAVE_MEM_BASE + DSMC_SLAVE_MEM_SIZE - 1, DSMC_SLAVE_MEM_BASE, SHARED_MEM}, /* reserve for DSMC slave */
#endif
    {0xFF000000, 0xFFF00000 - 1, 0xFF000000, DEVICE_MEM} /* DEVICE */
};
#else
struct mem_desc platform_mem_desc[] =
{
#ifdef RT_USING_UNCACHE_HEAP
    {FIRMWARE_BASE, FIRMWARE_BASE + FIRMWARE_SIZE - 1, FIRMWARE_BASE, NORMAL_MEM},
    {RT_UNCACHE_HEAP_BASE, RT_UNCACHE_HEAP_BASE + RT_UNCACHE_HEAP_SIZE - 1, RT_UNCACHE_HEAP_BASE, UNCACHED_MEM},
#else
    {FIRMWARE_BASE, FIRMWARE_BASE + DRAM_SIZE - 1, FIRMWARE_BASE, NORMAL_MEM},
#endif
#ifdef LINUX_RPMSG_BASE
    {LINUX_RPMSG_BASE, LINUX_RPMSG_BASE + LINUX_RPMSG_SIZE - 1, LINUX_RPMSG_BASE, UNCACHED_MEM},
#endif
#ifdef RT_USING_DSMC_HOST
    {DSMC_MEM_BASE, 0xFF000000 - 1, DSMC_MEM_BASE, UNCACHED_MEM}, /* DSMC HOST memory space */
#endif
#ifdef RT_USING_DSMC_SLAVE
    {DSMC_SLAVE_MEM_BASE, DSMC_SLAVE_MEM_BASE + DSMC_SLAVE_MEM_SIZE - 1, DSMC_SLAVE_MEM_BASE, NORMAL_MEM}, /* reserve for DSMC slave */
#endif
    {0xFF000000, 0xFFF00000 - 1, 0xFF000000, DEVICE_MEM} /* DEVICE */
};
#endif

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

#ifdef RT_USING_SDIO
RT_WEAK struct rk_mmc_platform_data rk_mmc_table[] =
{
#ifdef RT_USING_SDIO0
    {
        .flags = MMCSD_BUSWIDTH_4 | MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ | MMCSD_SUP_HIGHSPEED,
        .irq = SDMMC_IRQn,
        .base = MMC_BASE,
        .clk_id = CCLK_SRC_SDMMC,
        .freq_min = 100000,
        .freq_max = 50000000,
        .control_id = 0,
    },
#endif
    { /* sentinel */ },
};
#endif

#ifdef HAL_GIC_MODULE_ENABLED
static struct GIC_AMP_IRQ_INIT_CFG irqsConfig[] =
{
    /* TODO: Config the irqs here. */

    /* TODO: By default, UART4 is used for cpu2 */
#ifdef RT_USING_UART4
    GIC_AMP_IRQ_CFG_ROUTE(UART4_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
#endif

#ifdef RT_USING_PIN
    GIC_AMP_IRQ_CFG_ROUTE(GPIO0_3_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(GPIO1_3_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(GPIO2_3_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(GPIO3_3_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(GPIO4_3_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
#endif

#ifdef HAL_GIC_WAIT_LINUX_INIT_ENABLED
    /* route tick to cpu3 when GIC init by linux */
    GIC_AMP_IRQ_CFG_ROUTE(TICK_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
#endif

#ifdef RT_USING_RPMSG_LITE
    GIC_AMP_IRQ_CFG_ROUTE(MAILBOX_BB_2_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
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

/**
 *  Initialize the Hardware related stuffs. Called from rtthread_startup()
 *  after interrupt disabled.
 */
void rt_hw_board_init(void)
{
#if defined(HAL_DBG_USING_LIBC_PRINTF) || defined(HAL_DBG_USING_HAL_PRINTF)
    struct HAL_UART_CONFIG hal_uart_config =
    {
        .baudRate = UART_BR_1500000,
        .dataBit = UART_DATA_8B,
        .stopBit = UART_ONE_STOPBIT,
        .parity = UART_PARITY_DISABLE,
    };
#endif

    /* HAL_Init */
    HAL_Init();

    /* hal bsp init */
    BSP_Init();

#ifdef RT_USING_PIN
    rt_hw_iomux_config();
#endif

#if defined(HAL_DBG_USING_LIBC_PRINTF) || defined(HAL_DBG_USING_HAL_PRINTF)
    HAL_UART_Init(&g_uart4Dev, &hal_uart_config);
#endif

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
#define PSCI_CPU_ON_AARCH32     0x84000003U

__attribute__((noinline)) void arm_psci_cpu_on(rt_ubase_t funcid, rt_ubase_t cpuid, rt_ubase_t entry)
{
    __asm volatile("smc #0" : : : "r0", "r1", "r2");
}

void rt_hw_secondary_cpu_up(void)
{
    int i;
    extern void secondary_cpu_start(void);

    for (i = 1; i < RT_CPUS_NR; ++i)
    {
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
    cpu_id &= 0xf;
    return cpu_id;
};

void secondary_cpu_c_start(void)
{
    rt_hw_vector_init();
    arm_gic_cpu_init(0, GIC_CPU_INTERFACE_BASE);
    generic_timer_config();
    rt_hw_spin_lock(&_cpus_lock);
    rt_system_scheduler_start();
}

void rt_hw_secondary_cpu_idle_exec(void)
{
    __WFE();
}
#endif

