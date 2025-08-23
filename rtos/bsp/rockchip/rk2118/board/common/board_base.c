/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-02-07     Cliff Chen   first implementation
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"
#include "drv_clock.h"
#include "drv_uart.h"
#include "drv_cache.h"
#include "drv_heap.h"
#include "drv_opp.h"
#include "drv_otp.h"
#include "drv_thermal.h"
#include "hal_base.h"
#include "hal_bsp.h"
#include "iomux.h"
#ifdef RT_USING_HIFI4
#include "core/rk_core.h"
#endif

#ifdef RT_USING_USB_DEVICE
#include "drv_usbd.h"
#endif

extern const struct clk_init clk_inits[];

#ifdef RT_USING_SDIO
void rt_board_mmc_init(void)
{
    volatile int *ptr1 = (volatile int *)MMC0_BASE + 0x134; /* sdmmc sample phase reg */

    /* Init SDMMC sample phase to 0 degree */
    *ptr1 = 0x0 | 0x6 << 16;
}
#endif /* RT_USING_SDIO */

#if defined(RT_USING_TSADC)
RT_WEAK const struct tsadc_init g_tsadc_init =
{
    .chn_id = {0},
    .chn_num = 1,
    .polarity = TSHUT_LOW_ACTIVE,
    .mode = TSHUT_MODE_CRU,
    .otp_offset = {32},
    .otp_mask = 0x3ff,
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

#if defined(RT_USING_OTP)
RT_WEAK const struct otp_data g_otp_data =
{
    .reg = OTPC,
    .size = 0x78,
    .nbytes = 0x2,
    .ns_offset = 0x1c0,
};
#endif /* RT_USING_OTP */

#if defined(RT_USING_OPP)
/* RK2118M || RK2118BM */
#if defined(RT_USING_CLK_CONFIG0) || defined(RT_USING_CLK_CONFIG1)
static void set_vdd_dsp0_to_1v(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_A5, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_A5, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A5, GPIO_HIGH);
}

static void set_vdd_dsp0_to_0v9(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_A5, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_A5, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A5, GPIO_LOW);
}

static void set_vdd_core_to_0v95(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_A6, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_A6, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A6, GPIO_HIGH);
}

static void set_vdd_core_to_0v9(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK0, GPIO_PIN_A6, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO0, GPIO_PIN_A6, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO0, GPIO_PIN_A6, GPIO_LOW);
}

static rt_err_t set_vdd_dsp0_volt(struct rk_opp_table *opp_table, uint32_t volt)
{
    if (volt == 1000000)
        set_vdd_dsp0_to_1v();
    else if (volt == 900000)
        set_vdd_dsp0_to_0v9();

    return RT_EOK;
}

static rt_err_t set_vdd_core_volt(struct rk_opp_table *opp_table, uint32_t volt)
{
    if (volt == 950000)
        set_vdd_core_to_0v95();
    else if (volt == 900000)
        set_vdd_core_to_0v9();

    return RT_EOK;
}

static struct rk_opp dsp0_opps[] =
{
    {.rate = 700000000, .volt = 1000000, .len = 36},
};

static const struct rk_pvtm_config dsp0_pvtm_cfg =
{
    .base = GRF_BASE,
    .offset = 0x1b8,
    .freq = 700000000,
    .volt = 1000000,
    .len_step = 1,
    .tsadc_ch = 0,
    .ref_temp = 40,
    .temp_prop = {300, 300},
};

static const struct rk_pvtm_table dsp0_pvtm_tbl[] =
{
    {.value = 0,    .sel = 0},
    {.value = 740,  .sel = 1},
    {.value = 755,  .sel = 2},
    {.value = 770,  .sel = 3},
    {.value = 785,  .sel = 4},
    {.value = 800,  .sel = 5},
    {.value = 810,  .sel = 6},
    {.value = 825,  .sel = 7},
    {.value = 835,  .sel = 8},
    {.value = 845,  .sel = 9},
    {.value = 855,  .sel = 10},
    {.value = 865,  .sel = 11},
    {.value = UINT_MAX},
};

static struct rk_opp dsp12_opps[] =
{
    {.rate = 491520000, .volt = 900000},
};

struct rk_opp_table g_opp_table[] =
{
    {
        .name = "dsp0_opp_table",
        .opps = dsp0_opps,
        .num_opps = HAL_ARRAY_SIZE(dsp0_opps),
        .pvtm_cfg = &dsp0_pvtm_cfg,
        .pvtm_tbl = dsp0_pvtm_tbl,
        .clk_id = CLK_DSP0,
        .pvtm_sel = UINT_MAX,
        .set_reg_volt = set_vdd_dsp0_volt,
        .init_freq = 700000000,
    },
    {
        .name = "dsp12_opp_table",
        .opps = dsp12_opps,
        .num_opps = HAL_ARRAY_SIZE(dsp12_opps),
        .clk_id = CLK_DSP1,
        .set_reg_volt = set_vdd_core_volt,
        .init_freq = 491520000,
        .low_temp = 10,
        .low_temp_min_volt = 950000,
    },
    { /* sentinel */ },
};
/* RK2118G || RK2118B */
#elif defined(RT_USING_CLK_CONFIG2)
static struct rk_opp dsp0_opps[] =
{
    {.rate = 800000000, .volt = 1100000, .volt_min = 1000000, .len = 34},
};

static const struct rk_pvtm_config dsp0_pvtm_cfg =
{
    .base = GRF_BASE,
    .offset = 0x1b8,
    .freq = 800000000,
    .volt = 1100000,
    .volt_step = 12500,
    .len_step = 1,
    .tsadc_ch = 0,
    .ref_temp = 40,
    .temp_prop = {300, 300},
};

static const struct rk_pvtm_table dsp0_pvtm_tbl[] =
{
    {.value = 0,    .sel = 0},
    {.value = 815,  .sel = 1},
    {.value = 830,  .sel = 2},
    {.value = 840,  .sel = 3},
    {.value = 853,  .sel = 4},
    {.value = 865,  .sel = 5},
    {.value = 877,  .sel = 6},
    {.value = 890,  .sel = 7},
    {.value = 900,  .sel = 8},
    {.value = 915,  .sel = 9},
    {.value = 927,  .sel = 10},
    {.value = 940,  .sel = 11},
    {.value = 957,  .sel = 12},
    {.value = 965,  .sel = 13},
    {.value = 980,  .sel = 14},
    {.value = 995,  .sel = 15},
    {.value = 1010, .sel = 16},
    {.value = UINT_MAX},
};

static const struct rk_opp_otp dsp0_opp_otp =
{
    .pvt_len_offset = 42,
};

static struct rk_opp dsp12_opps[] =
{
    {.rate = 589824000, .volt = 950000},
};

struct rk_opp_table g_opp_table[] =
{
    {
        .name = "dsp0_opp_table",
        .opps = dsp0_opps,
        .num_opps = HAL_ARRAY_SIZE(dsp0_opps),
        .pvtm_cfg = &dsp0_pvtm_cfg,
        .pvtm_tbl = dsp0_pvtm_tbl,
        .otp = &dsp0_opp_otp,
        .clk_id = CLK_DSP0,
        .pvtm_sel = UINT_MAX,
        .typical_sel = 2,
        .pwr_id = PWR_ID_DSP_CORE,
        .init_freq = 800000000,
    },
    {
        .name = "dsp12_opp_table",
        .opps = dsp12_opps,
        .num_opps = HAL_ARRAY_SIZE(dsp12_opps),
        .clk_id = CLK_DSP1,
        .pwr_id = PWR_ID_CORE,
        .init_freq = 589824000,
        .low_temp = 10,
        .low_temp_min_volt = 975000,
    },
    { /* sentinel */ },
};
#endif

#if defined(RT_USING_TSADC)
void thermal_monitor()
{
    int temp = tsadc_get_temp(0);

    opp_adjust_by_temp(temp);
};
#endif
#endif

RT_WEAK void systick_isr(int vector, void *param)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_SYSTICK_IRQHandler();
    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

extern const rt_uint32_t __code_start__[];
extern const rt_uint32_t __code_end__[];
extern const rt_uint32_t __data_start__[];
extern const rt_uint32_t __data_end__[];
extern const rt_uint32_t __device_start__[];
extern const rt_uint32_t __device_end__[];
extern const rt_uint32_t __ext_mem_start__[];
extern const rt_uint32_t __ext_mem_end__[];
#ifdef RT_USING_UNCACHE_HEAP
extern const rt_uint32_t __uncache_heap_start__[];
extern const rt_uint32_t __uncache_heap_end__[];
#endif
extern const rt_uint32_t __spi2apb_buffer_start__[];
extern const rt_uint32_t __spi2apb_buffer_end__[];
RT_WEAK void mpu_init(void)
{
    /* text section: non shared, ro, np, exec, cachable */
    ARM_MPU_SetRegion(0, ARM_MPU_RBAR((rt_uint32_t)__code_start__, 0U, 0U, 1U, 0U), ARM_MPU_RLAR((rt_uint32_t)__code_end__, 0U));
    /* data section: non shared, rw, np, exec, cachable */
    ARM_MPU_SetRegion(1, ARM_MPU_RBAR((rt_uint32_t)__data_start__, 0U, 0U, 1U, 0U), ARM_MPU_RLAR((rt_uint32_t)__data_end__, 1U));
    /* device section: shared, rw, np, xn */
    ARM_MPU_SetRegion(2, ARM_MPU_RBAR((rt_uint32_t)__device_start__, 1U, 0U, 1U, 1U), ARM_MPU_RLAR((rt_uint32_t)__device_end__, 2U));

    /* cachable normal memory, ARM_MPU_ATTR_MEMORY_(NT, WB, RA, WA) */
    ARM_MPU_SetMemAttr(0, ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(0, 0, 1, 0), ARM_MPU_ATTR_MEMORY_(0, 0, 1, 0)));
    ARM_MPU_SetMemAttr(1, ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(1, 1, 1, 0), ARM_MPU_ATTR_MEMORY_(1, 1, 1, 0)));
    /* device memory */
    ARM_MPU_SetMemAttr(2, ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE, ARM_MPU_ATTR_DEVICE_nGnRnE));

#ifdef RT_USING_UNCACHE_HEAP
    /* uncache heap: non shared, rw, np, exec, uncachable */
    ARM_MPU_SetRegion(3, ARM_MPU_RBAR((rt_uint32_t)__uncache_heap_start__, 0U, 0U, 1U, 1U), ARM_MPU_RLAR((rt_uint32_t)__uncache_heap_end__, 3U));
    /* uncachable normal memory */
    ARM_MPU_SetMemAttr(3, ARM_MPU_ATTR(ARM_MPU_ATTR_NON_CACHEABLE, ARM_MPU_ATTR_NON_CACHEABLE));
#endif
#ifdef RK2118_CPU_CORE0
    /* ddr section: non shared, rw, np, exec, cachable */
    ARM_MPU_SetRegion(4, ARM_MPU_RBAR((rt_uint32_t)__ext_mem_start__, 0U, 0U, 1U, 0U), ARM_MPU_RLAR((rt_uint32_t)__ext_mem_end__, 4U));
    ARM_MPU_SetMemAttr(4, ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(1, 1, 1, 0), ARM_MPU_ATTR_MEMORY_(1, 1, 1, 0)));
    /* spi2adb buffer: non shared, rw, np, exec, cachable */
    ARM_MPU_SetRegion(5, ARM_MPU_RBAR((rt_uint32_t)__spi2apb_buffer_start__, 0U, 0U, 1U, 0U), ARM_MPU_RLAR((rt_uint32_t)__spi2apb_buffer_end__, 5U));
    ARM_MPU_SetMemAttr(5, ARM_MPU_ATTR(ARM_MPU_ATTR_MEMORY_(1, 1, 1, 0), ARM_MPU_ATTR_MEMORY_(1, 1, 1, 0)));
#endif

    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
}

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

extern const rt_uint32_t __sram_heap_begin__[];
extern const rt_uint32_t __sram_heap_end__[];
extern const rt_uint32_t __ddr_heap_begin__[];
extern const rt_uint32_t __ddr_heap_end__[];
extern const rt_uint32_t __uncache_heap_start__[];
extern const rt_uint32_t __uncache_heap_end__[];

#if defined(RK2118_CPU_CORE0) && defined(RT_USING_DDR)
static struct rt_memheap ddr_heap;
#endif

void rt_memory_heap_init(void)
{
#ifdef RK2118_CPU_CORE0
    /* system heap in sram for cpu0 */
    rt_system_heap_init((void *)__sram_heap_begin__, (void *)__sram_heap_end__);
#ifdef RT_USING_DDR
    rt_memheap_init(&ddr_heap, "ddr", (void *)__ddr_heap_begin__, (rt_base_t)__ddr_heap_end__ - (rt_base_t)__ddr_heap_begin__);
#endif
#ifdef RT_USING_UNCACHE_HEAP
    rt_uncache_heap_init((void *)__uncache_heap_start__, (void *)__uncache_heap_end__);
#endif
#else
    /* system heap in ddr for cpu1 */
    rt_system_heap_init((void *)__ddr_heap_begin__, (void *)__ddr_heap_end__);
#endif
}

#ifdef RT_USING_SWO_PRINTF
void swo_console_hook(const char *str, int flush)
{
    char ch;

    while (str && *str != '\0')
    {
        ch = *str;
        ITM_SendChar(ch);
        str++;
    }
}
#endif

void spinlock_init(void)
{
#ifdef RT_USING_HIFI4
    HAL_SPINLOCK_Init(rk_core_id());
#endif
}

RT_WEAK void usb_phy_init(void)
{
#ifndef RT_USING_USB
    /* Set USB2 PHY enter suspend mode */
    WRITE_REG_MASK_WE(USB_PHY_CON_BASE, USB_PHY_SUSPEND_MASK,
                      USB_PHY_SUSPEND_VAL << USB_PHY_CON_SHIFT);

    /* Turn off differential receiver by default to save power */
    WRITE_REG(*(uint32_t *)(USB_INNO_PHY_BASE + 0x0100U), 0x00);
#else
    /* Set Host Disconnect Detection to 675mV */
    MODIFY_REG(*(uint32_t *)(USB_INNO_PHY_BASE + 0x60U), 0x3 << 0, 0x0 << 0);
    MODIFY_REG(*(uint32_t *)(USB_INNO_PHY_BASE + 0x68U), 0x1 << 0, 0x0 << 0);
    MODIFY_REG(*(uint32_t *)(USB_INNO_PHY_BASE + 0x64U), 0x1 << 7, 0x1 << 7);

    /* Increase the disconnect detection threshold */
    WRITE_REG(*(uint32_t *)(USB_INNO_PHY_BASE + 0xbcU), 0x10);
    WRITE_REG(*(uint32_t *)(USB_INNO_PHY_BASE + 0x74U), 0x50);
    WRITE_REG(*(uint32_t *)(USB_INNO_PHY_BASE + 0x184U), 0xf1);
    WRITE_REG(*(uint32_t *)(USB_INNO_PHY_BASE + 0x194U), 0xf1);
#ifdef RT_USING_USB_SWITCH
    /* Set Pre-emphasize Strength to 2 */
    WRITE_REG(*(uint32_t *)(USB_INNO_PHY_BASE + 0x0040U), 0x51);

    /* Set Eye-height to 437.5mV */
    WRITE_REG(*(uint32_t *)(USB_INNO_PHY_BASE + 0x0124U), 0x10);
#endif
#endif
}

/**
 * This function will initial Pisces board.
 */
RT_WEAK void rt_hw_board_init()
{
    /* mpu init */
    mpu_init();

    /* HAL_Init */
    HAL_Init();

    /* spinlock init */
    spinlock_init();

    /* hal bsp init */
    BSP_Init();

    /* Initialize cmbacktrace early to ensure that rt_assert_hook can take effect as soon as possible. */
#ifdef RT_USING_CMBACKTRACE
    rt_cm_backtrace_init();
#endif

    /* System tick init */
    rt_hw_interrupt_install(SysTick_IRQn, systick_isr, RT_NULL, "tick");
    HAL_SetTickFreq(1000 / RT_TICK_PER_SECOND);
    HAL_SYSTICK_Init();

#ifdef RT_USING_PIN
    rt_hw_iomux_config();
#endif

    rt_memory_heap_init();

    /* Initial usart deriver, and set console device */
    rt_hw_usart_init();

#ifdef RT_USING_SDIO
    rt_board_mmc_init();
#endif

    usb_phy_init();

    clk_init(clk_inits, true);

    /* Update system core clock after clk_init */
    SystemCoreClockUpdate();

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

#ifdef RT_USING_SWO_PRINTF
    rt_console_set_output_hook(swo_console_hook);
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
