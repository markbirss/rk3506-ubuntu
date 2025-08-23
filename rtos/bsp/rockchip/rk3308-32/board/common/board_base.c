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

#ifdef RT_USING_GMAC
#include "drv_gmac.h"
#endif

#ifdef RT_USING_CRU
#include "drv_clock.h"

RT_WEAK const struct clk_init clk_inits[] =
{
    INIT_CLK("PLL_APLL", PLL_APLL, 816 * MHZ),
    INIT_CLK("CLK_TSADC", CLK_TSADC, 50000),
    { /* sentinel */ },
};

RT_WEAK const struct clk_unused clks_unused[] =
{
    { /* sentinel */ },
};
#endif

#ifdef RT_USING_SDIO
#include "drv_sdio.h"
#endif

#ifdef RT_USING_AUDIO
#include "rk_audio.h"
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
    .chn_id = {0, 1},
    .chn_num = 2,
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

#if defined(RT_USING_UART1)
RT_WEAK const struct uart_board g_uart1_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart1",
};
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
RT_WEAK const struct uart_board g_uart2_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart2",
};
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
RT_WEAK const struct uart_board g_uart3_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart3",
};
#endif /* RT_USING_UART3 */

#if defined(RT_USING_UART4)
RT_WEAK const struct uart_board g_uart4_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart4",
};
#endif /* RT_USING_UART4 */

#ifdef RT_USING_GMAC
const struct rockchip_eth_config rockchip_eth_config_table[] =
{
#ifdef RT_USING_GMAC0
    {
        .id = GMAC0,
        .mode = RMII_MODE,
        .phy_addr = 0,

        .external_clk = true,

        .reset_gpio_bank = GPIO4,
        .reset_gpio_num = GPIO_PIN_C0,
        .reset_delay_ms = {1, 20, 100},
    },
#endif
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
    {0xFF000000, 0xFFFF0000 - 1, 0xFF000000, DEVICE_MEM} /* DEVICE */
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
    {SHMEM_BASE, SHMEM_BASE + SHMEM_SIZE - 1, SHMEM_BASE, SHARED_MEM},
#ifdef LINUX_RPMSG_BASE
    {LINUX_RPMSG_BASE, LINUX_RPMSG_BASE + LINUX_RPMSG_SIZE - 1, LINUX_RPMSG_BASE, UNCACHED_MEM},
#endif
    {0xFF000000, 0xFFF00000 - 1, 0xFF000000, DEVICE_MEM}, /* DEVICE */
    {0xFFF00000, 0xFFFFFFFF,     0xFFF00000, NORMAL_MEM} /* SRAM */
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

#ifndef RT_USING_SMP
static struct GIC_AMP_IRQ_INIT_CFG irqsConfig[] =
{
    /* Config the irqs here. */
    // todo...

#ifdef AMP_LINUX_ENABLE
    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_03_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),

#if defined(RT_USING_UART0)
    GIC_AMP_IRQ_CFG_ROUTE(UART0_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
#elif defined(RT_USING_UART1)
    GIC_AMP_IRQ_CFG_ROUTE(UART1_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),
#endif

#else // #ifdef AMP_LINUX_ENABLE
    GIC_AMP_IRQ_CFG_ROUTE(AMP0_IRQn, 0xd0, CPU_GET_AFFINITY(0, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(AMP1_IRQn, 0xd0, CPU_GET_AFFINITY(1, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(AMP2_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(AMP3_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),

    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_01_IRQn, 0xd0, CPU_GET_AFFINITY(1, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_02_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_03_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),

    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_10_IRQn, 0xd0, CPU_GET_AFFINITY(0, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_12_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_13_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),

    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_20_IRQn, 0xd0, CPU_GET_AFFINITY(0, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_21_IRQn, 0xd0, CPU_GET_AFFINITY(1, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_23_IRQn, 0xd0, CPU_GET_AFFINITY(3, 0)),

    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_30_IRQn, 0xd0, CPU_GET_AFFINITY(0, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_31_IRQn, 0xd0, CPU_GET_AFFINITY(1, 0)),
    GIC_AMP_IRQ_CFG_ROUTE(RPMSG_32_IRQn, 0xd0, CPU_GET_AFFINITY(2, 0)),
#endif

    GIC_AMP_IRQ_CFG_ROUTE(0, 0, CPU_GET_AFFINITY(DEFAULT_IRQ_CPU, 0)),   /* sentinel */
};

static struct GIC_IRQ_AMP_CTRL irqConfig =
{
    .cpuAff = CPU_GET_AFFINITY(DEFAULT_IRQ_CPU, 0),
    .defPrio = 0xd0,
    .defRouteAff = CPU_GET_AFFINITY(DEFAULT_IRQ_CPU, 0),
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

#ifdef RT_USING_SDIO
RT_WEAK struct rk_mmc_platform_data rk_mmc_table[] =
{
#ifdef RT_USING_SDIO0
    {
        .flags = MMCSD_BUSWIDTH_8 | MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ | MMCSD_SUP_HIGHSPEED,
        .irq = EMMC_IRQn,
        .base = EMMC_BASE,
        .clk_id = CLK_EMMC,
        .freq_min = 100000,
        .freq_max = 50000000,
        .control_id = 0,
    },
#endif
    { /* sentinel */ },
};

#ifdef RT_USING_SDIO0
static void rt_board_mmc_init(void)
{
    volatile int *ptr1 = CRU_BASE + CRU_EMMC_CON1_OFFSET; /* CRU eMMC sample phase reg */

    /* Init eMMC sample phase to 90 degree */
    *ptr1 = 0x02 | 0x02 << 16;
}
#endif

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

#ifdef RT_USING_USB_DEVICE
RT_WEAK struct ep_id g_usb_ep_pool[] =
{
    { 0x0,  USB_EP_ATTR_CONTROL,    USB_DIR_INOUT,  64,   ID_ASSIGNED   },
    { 0x1,  USB_EP_ATTR_BULK,       USB_DIR_IN,     1024, ID_UNASSIGNED },
    { 0x2,  USB_EP_ATTR_BULK,       USB_DIR_OUT,    512,  ID_UNASSIGNED },
    { 0x3,  USB_EP_ATTR_ISOC,       USB_DIR_IN,     1024, ID_UNASSIGNED },
    { 0x4,  USB_EP_ATTR_ISOC,       USB_DIR_OUT,    512,  ID_UNASSIGNED },
    { 0x5,  USB_EP_ATTR_INT,        USB_DIR_IN,     64,   ID_UNASSIGNED },
    { 0x6,  USB_EP_ATTR_INT,        USB_DIR_OUT,    64,   ID_UNASSIGNED },
    { 0xFF, USB_EP_ATTR_TYPE_MASK,  USB_DIR_MASK,   0,    ID_ASSIGNED   },
};
#endif

#ifndef RT_USING_SMP

#ifdef PRIMARY_CPU
extern int __share_heap_begin, __share_heap_end;
#define SHMEM_HEAP_BEGIN   (&__share_heap_begin)
#define SHMEM_HEAP_SIZE    ((rt_uint32_t)(&__share_heap_end) - (rt_uint32_t)(&__share_heap_begin))

static struct rt_memheap _shmem_heap;

rt_err_t rt_shmem_heap_init(void)
{
    /* initialize a share memory heap in the system */
    return rt_memheap_init(&_shmem_heap,
                           "shmemheap",
                           SHMEM_HEAP_BEGIN,
                           SHMEM_HEAP_SIZE);
}

void *rt_malloc_shmem(rt_size_t size)
{
    return rt_memheap_alloc(&_shmem_heap, size);
}

void rt_free_shmem(void *ptr)
{
    rt_memheap_free(ptr);
}
#endif


#ifdef RT_USING_LOGBUFFER

#if defined(CPU0)
#define LOG_MEM_BASE ((uint32_t)&__share_log0_start__)
#define LOG_MEM_END  ((uint32_t)&__share_log0_end__)
#elif defined(PRIMARY_CPU)
#define LOG_MEM_BASE ((uint32_t)&__share_log1_start__)
#define LOG_MEM_END  ((uint32_t)&__share_log1_end__)
#elif defined(CPU2)
#define LOG_MEM_BASE ((uint32_t)&__share_log2_start__)
#define LOG_MEM_END  ((uint32_t)&__share_log2_end__)
#elif defined(CPU3)
#define LOG_MEM_BASE ((uint32_t)&__share_log3_start__)
#define LOG_MEM_END  ((uint32_t)&__share_log3_end__)
#else
#error "error: Undefined CPU id!"
#endif

static struct rt_ringbuffer log_buffer, *plog_buf = NULL;
static void log_buffer_init(void)
{
    rt_ringbuffer_init(&log_buffer, (uint8_t *)LOG_MEM_BASE, (int16_t)(LOG_MEM_END - LOG_MEM_BASE));
    plog_buf = &log_buffer;
}

struct ringbuffer_t *get_log_ringbuffer(void)
{
    RT_ASSERT(plog_buf != NULL);
    return plog_buf;
}
#endif

#ifdef RT_USING_SHARE_CONSOLE_OUT
int rt_amp_printf(const char *fmt, ...)
{
    va_list args;
    rt_size_t length, outlen;
    static char rt_log_buf[32 + RT_CONSOLEBUF_SIZE];
    char *p_log_str = &rt_log_buf[32];

    va_start(args, fmt);
    /* the return value of vsnprintf is the number of bytes that would be
     * written to buffer had if the size of the buffer been sufficiently
     * large excluding the terminating null byte. If the output string
     * would be larger than the rt_log_buf, we have to adjust the output
     * length. */
    length = rt_vsnprintf(p_log_str, RT_CONSOLEBUF_SIZE - 1, fmt, args);
    va_end(args);

    if (length > RT_CONSOLEBUF_SIZE - 1)
        length = RT_CONSOLEBUF_SIZE - 1;

    p_log_str[RT_CONSOLEBUF_SIZE - 1] = 0;

    /* Add time stamp */
    outlen = 0;
    if ((p_log_str[strlen(p_log_str) - 1] == '\n') &&
            (strlen(p_log_str) > 1))    // fix shell command input
    {
        uint64_t cnt64;
        uint32_t cpu_id, sec, ms, us;
        cpu_id = HAL_CPU_TOPOLOGY_GetCurrentCpuId();
        cnt64 = HAL_GetSysTimerCount();
        us = (uint32_t)((cnt64 / (PLL_INPUT_OSC_RATE / 1000000)) % 1000);
        ms = (uint32_t)((cnt64 / (PLL_INPUT_OSC_RATE / 1000)) % 1000);
        sec = (uint32_t)(cnt64 / PLL_INPUT_OSC_RATE);
        outlen = snprintf(rt_log_buf, 32 - 1, "[(%d)%d.%03d.%03d] ", cpu_id, sec, ms, us);
    }
    outlen += snprintf(rt_log_buf + outlen, RT_CONSOLEBUF_SIZE - 1, "%s", p_log_str);

    /* Save log to buffer */
#ifdef RT_USING_LOGBUFFER
    rt_ringbuffer_put_force(get_log_ringbuffer(), rt_log_buf, outlen);
#endif

    rt_kprintf("%s", rt_log_buf);

    return RT_EOK;
}
#endif

#endif // #ifndef RT_USING_SMP

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
#ifndef RT_USING_SMP
    HAL_GIC_Init(&irqConfig);
#endif
    rt_hw_interrupt_init();

    /* SPINLOCK Init */
#ifdef HAL_SPINLOCK_MODULE_ENABLED
    uint32_t ownerID;
    ownerID = HAL_CPU_TOPOLOGY_GetCurrentCpuId() + 1;
    HAL_SPINLOCK_Init(ownerID);
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

#ifdef RT_USING_CRU
    clk_init(clk_inits, false);
    /* disable some clks when init, and enabled by device when needed */
    clk_disable_unused(clks_unused);
#endif

#ifdef RT_USING_PIN
    rt_hw_iomux_config();
#endif

#ifndef RT_USING_SMP
#ifdef RT_USING_LOGBUFFER
    log_buffer_init();
#endif
#endif

    /* initialize uart */
#ifdef RT_USING_UART
    rt_hw_usart_init();
#endif

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

#ifdef RT_USING_SDIO0
    rt_board_mmc_init();
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

#ifndef RT_USING_SMP
#if defined PRIMARY_CPU
    rt_shmem_heap_init();
    rt_kprintf("share_heap: BASE = 0x%08x, SIZE = 0x%08x\n", SHMEM_HEAP_BEGIN, SHMEM_HEAP_SIZE);
#endif
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
    __asm volatile("smc #0" : : :);
}

void rt_hw_secondary_cpu_up(void)
{
    int i;
    extern void secondary_cpu_start(void);

    /* TODO: Fix cpu1 und exception */
    for (i = 1; i < RT_CPUS_NR; ++i)
    {
        HAL_CPUDelayUs(10000);
        arm_psci_cpu_on(PSCI_CPU_ON_AARCH32, i, (rt_ubase_t)secondary_cpu_start);
    }
}

void secondary_cpu_c_start(void)
{
    uint32_t id;

    id = rt_hw_cpu_id();
    rt_kprintf("cpu %d startup.\n", id);
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
