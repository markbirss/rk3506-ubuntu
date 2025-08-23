/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author          Notes
 * 2019-09-15     Frank Wang      first implementation
 *
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"
#include "drv_cache.h"
#include "hal_base.h"
#include "hal_bsp.h"

#ifdef RT_USING_CRU
#include "drv_clock.h"
#endif
#ifdef RT_USING_CODEC
#include "drv_codecs.h"
#endif
#ifdef RT_USING_PIN
#include "iomux.h"
#endif
#ifdef RT_USING_PM
#include "drv_pm.h"
#endif
#ifdef RT_USING_UART
#include "drv_uart.h"
#endif
#ifdef HAL_PWR_MODULE_ENABLED
#include "drv_regulator.h"
#endif
#ifdef RT_USING_AUDIO
#include "rk_audio.h"
#endif

#ifdef RT_USING_SDIO
#include "drv_sdio.h"
#include <drivers/mmcsd_core.h>
#endif
#ifdef RT_USING_USB_DEVICE
#include "drv_usbd.h"
#endif

#ifdef RT_USING_MODULE
#define DATA_EXEC_FLAG   0U
#else
#define DATA_EXEC_FLAG   1U
#endif

#ifdef RT_USING_SYSTICK
#define TICK_IRQn  SysTick_IRQn
#else
#define TICK_TIMER TIMER5
#define TICK_IRQn  TIMER5_IRQn
#endif

#ifdef RT_USING_CRU
static const struct clk_init clk_inits[] =
{
    INIT_CLK("HCLK_MCU_BUS", HCLK_MCU_BUS, 24000000),
    INIT_CLK("PCLK_MCU_BUS", PCLK_MCU_BUS, 24000000),
    INIT_CLK("ACLK_PERI_BUS", ACLK_PERI_BUS, 24000000),
    INIT_CLK("HCLK_PERI_BUS", HCLK_PERI_BUS, 24000000),
    INIT_CLK("HCLK_TOP_BUS", HCLK_TOP_BUS, 24000000),
    INIT_CLK("PCLK_TOP_BUS", PCLK_TOP_BUS, 24000000),
    INIT_CLK("CLK_CRYPTO", CLK_CRYPTO, 16000000),
    INIT_CLK("CLK_OTG_USBPHY", CLK_OTG_USBPHY, 16000000),
    INIT_CLK("SCLK_SFC_SRC", CLK_XIP_SFC0, 5000000),
    INIT_CLK("SCLK_SFC1_SRC", CLK_XIP_SFC1, 5000000),
    INIT_CLK("PLL_GPLL", PLL_GPLL, 384000000),
    INIT_CLK("PLL_VPLL", PLL_VPLL, 491520000),
    INIT_CLK("SCLK_SFC_SRC", CLK_XIP_SFC0, 50000000),
    INIT_CLK("CLK_XIP_HYPERX8", CLK_XIP_HYPERX8, 300000000),
    INIT_CLK("CLK_HIFI3", CLK_HIFI3, 164000000),
    INIT_CLK("HCLK_MCU_BUS", HCLK_MCU_BUS, 200000000),
    INIT_CLK("PCLK_MCU_BUS", PCLK_MCU_BUS, 100000000),
    INIT_CLK("SCLK_M4F0", SCLK_M4F0, 200000000),
    INIT_CLK("ACLK_PERI_BUS", ACLK_PERI_BUS, 200000000),
    INIT_CLK("HCLK_PERI_BUS", HCLK_PERI_BUS, 100000000),
    INIT_CLK("HCLK_TOP_BUS", HCLK_TOP_BUS, 100000000),
    INIT_CLK("PCLK_TOP_BUS", PCLK_TOP_BUS, 100000000),
    INIT_CLK("CLK_CRYPTO", CLK_CRYPTO, 100000000),
    INIT_CLK("CLK_OTG_USBPHY", CLK_OTG_USBPHY, 24000000),
    { /* sentinel */ }
};

static const struct clk_unused clks_unused[] =
{
    { /* sentinel */ }
};
#endif

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

#if defined(RT_USING_UART2)
const struct uart_board g_uart2_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart2",
};
#endif /* RT_USING_UART2 */

#ifdef RT_USING_USB_DEVICE
struct ep_id g_usb_ep_pool[] =
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

#ifdef PRINT_CLK_SUMMARY_INFO
/**
 *
 */
void print_clk_summary_info(void)
{
}
#endif

static void tick_isr(int vector, void *param)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_IncTick();
    rt_tick_increase();
#ifdef TICK_TIMER
    HAL_TIMER_ClrInt(TICK_TIMER);
#endif

    /* leave interrupt */
    rt_interrupt_leave();
}

static void mpu_init(void)
{
    static const ARM_MPU_Region_t table[] =
    {
        {
            .RBAR = ARM_MPU_RBAR(0U, 0x00080000),
            .RASR = ARM_MPU_RASR(0U, ARM_MPU_AP_FULL, 0U, 0U, 1U, 0U, 0U, ARM_MPU_REGION_SIZE_256KB)
        },
        {
            .RBAR = ARM_MPU_RBAR(1U, 0x10000000U),
            .RASR = ARM_MPU_RASR(0U, ARM_MPU_AP_FULL, 0U, 0U, 1U, 0U, 0U, ARM_MPU_REGION_SIZE_8MB)
        },
        {
            .RBAR = ARM_MPU_RBAR(2U, 0x20000000U),
            .RASR = ARM_MPU_RASR(DATA_EXEC_FLAG, ARM_MPU_AP_FULL, 0U, 0U, 1U, 0U, 0U, ARM_MPU_REGION_SIZE_256KB)
        },
        {
            .RBAR = ARM_MPU_RBAR(3U, 0x40000000U),
            .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_256MB)
        },
        {
            .RBAR = ARM_MPU_RBAR(4U, 0x60000000U),
            .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_256MB)
        },
    };

    ARM_MPU_Load(&(table[0]), 5U);

#ifdef RT_USING_UNCACHE_HEAP
    ARM_MPU_Region_t uncache_region;

    uncache_region.RBAR = ARM_MPU_RBAR(5U, RK_UNCACHE_HEAP_START);
    uncache_region.RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, RT_UNCACHE_HEAP_ORDER);
    ARM_MPU_SetRegionEx(5, uncache_region.RBAR, uncache_region.RASR);
#endif

    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);
}

void rt_hw_board_init()
{
    mpu_init();

    /* HAL_Init */
    HAL_Init();

    /* tick init */
    HAL_SetTickFreq(1000 / RT_TICK_PER_SECOND);
    rt_hw_interrupt_install(TICK_IRQn, tick_isr, RT_NULL, "tick");
    rt_hw_interrupt_umask(TICK_IRQn);
    HAL_NVIC_SetPriority(TICK_IRQn, NVIC_PERIPH_PRIO_LOWEST, NVIC_PERIPH_SUB_PRIO_LOWEST);
#ifdef RT_USING_SYSTICK
    HAL_SYSTICK_CLKSourceConfig(HAL_SYSTICK_CLKSRC_EXT);
    HAL_SYSTICK_Config((PLL_INPUT_OSC_RATE / RT_TICK_PER_SECOND) - 1);
    HAL_SYSTICK_Enable();
#else
    HAL_TIMER_Init(TICK_TIMER, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(TICK_TIMER, (PLL_INPUT_OSC_RATE / RT_TICK_PER_SECOND) - 1);
    HAL_TIMER_Start_IT(TICK_TIMER);
#endif

    rt_hw_cpu_cache_init();

#ifdef RT_USING_PIN
    rt_hw_iomux_config();
#endif


#ifdef RT_USING_CRU
    clk_init(clk_inits, true);
    /* disable some clks when init, and enabled by device when needed */
    clk_disable_unused(clks_unused);
#endif

    /* Initial usart deriver, and set console device */
#ifdef RT_USING_UART
    rt_hw_usart_init();
#endif

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    /* Print clk summary info */
#ifdef PRINT_CLK_SUMMARY_INFO
    print_clk_summary_info();
#endif

    /* hal bsp init */
    BSP_Init();

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}
