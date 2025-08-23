/**
  * Copyright (c) 2019 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    board.c
  * @author  Jason Zhu
  * @version V0.1
  * @date    1-Aug-2019
  * @brief
  *
  ******************************************************************************
  */

#include <rtthread.h>
#include <rthw.h>
#include "board.h"
#include "hal_base.h"
#include "hal_bsp.h"
#include "iomux.h"

#ifdef RT_USING_CRU
#include "drv_clock.h"

#ifdef RT_USING_CODEC
#include "drv_codecs.h"
#endif

#ifdef RT_USING_AUDIO
#include "rk_audio.h"
#endif

#define SYSTICK_CLK (75 * MHZ)

RT_WEAK const struct clk_init clk_inits[] =
{
    INIT_CLK("CAL_CORE", CAL_CORE, 50 * MHZ),
    INIT_CLK("SYS_CORE", SYS_CORE, 50 * MHZ),
    INIT_CLK("PLL_APLL", PLL_APLL, 300 * MHZ),
    INIT_CLK("CAL_CORE", CAL_CORE, 150 * MHZ),
    INIT_CLK("CAL_STCLK_CORE", CAL_STCLK_CORE, 150 * MHZ),
    INIT_CLK("SYS_CORE", SYS_CORE, 75 * MHZ),
    INIT_CLK("SYS_STCLK_CORE", SYS_STCLK_CORE, SYSTICK_CLK),
    INIT_CLK("PCLK_LOGIC", PCLK_LOGIC, 75 * MHZ),
    { /* sentinel */ },
};

RT_WEAK const struct clk_unused clks_unused[] =
{
    { /* sentinel */ },
};
#endif

#ifdef RT_USING_AUDIO
const struct audio_card_desc rk_board_audio_cards[] =
{
#ifdef RT_USING_AUDIO_CARD_RK2106
    {
        .name = "acodec",
        .dai = I2S0,
        .codec = ACODEC,
        .capture = true,
        .playback = true,
        .codec_master = true,
        .format = AUDIO_FMT_I2S,
    },
#endif
    { /* sentinel */ }
};
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

#if defined(RT_USING_UART1)
RT_WEAK const struct uart_board g_uart1_board =
{
    .baud_rate = UART_BR_115200,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart1",
};
#endif /* RT_USING_UART0 */

#ifdef RT_USING_USB_DEVICE
#include "drv_usbd.h"

RT_WEAK struct ep_id g_usb_ep_pool[] =
{
    { 0x0,  USB_EP_ATTR_CONTROL,    USB_DIR_INOUT,  64,   ID_ASSIGNED   },
    { 0x1,  USB_EP_ATTR_ISOC,       USB_DIR_IN,     64,   ID_UNASSIGNED },
    { 0x2,  USB_EP_ATTR_ISOC,       USB_DIR_OUT,    64,   ID_UNASSIGNED },
    { 0x3,  USB_EP_ATTR_INT,        USB_DIR_IN,     64,   ID_UNASSIGNED },
    { 0xFF, USB_EP_ATTR_TYPE_MASK,  USB_DIR_MASK,   0,    ID_ASSIGNED   },
};
#endif

extern int __heap_start[];
extern int __heap_end[];
#define __SRAM0_START             (0x01000000)
#define __SRAM0_SIZE              (384 * 1024)
#define __PMUSRAM_START           (0x00000000)
#define __PMUSRAM_SIZE            (64 * 1024)
static struct rt_memheap _sram0_heap;
static struct rt_memheap _pmusram_heap;

extern void SysTick_Handler(void);
RT_WEAK void tick_isr(int vector, void *param)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_IncTick();
    rt_tick_increase();
#ifdef RT_USING_CORE_FREERTOS
    SysTick_Handler();
#endif
#ifdef TICK_TIMER
    HAL_TIMER_ClrInt(TICK_TIMER);
#endif

    /* leave interrupt */
    rt_interrupt_leave();
}

static void rt_idle_wfi(void)
{
    rt_ubase_t level;

    asm volatile("MRS     %0, PRIMASK\n"
                 "CPSID   I"
                 : "=r"(level) : : "memory", "cc");

    asm volatile("dsb");
    asm volatile("wfi");

    asm volatile("MSR     PRIMASK, %0"
                 : : "r"(level) : "memory", "cc");
}

static void rt_hw_interrupt_default_config()
{
    int irq;

    for (irq = 0; irq < NUM_INTERRUPTS; irq++)
    {
        rt_hw_interrupt_mask(irq);
    }
}

int rt_hw_board_init(void)
{
    /* HAL_Init */
    HAL_Init();

    /* hal bsp init */
    BSP_Init();

    /* interrupt init */
    rt_hw_interrupt_init();

    rt_hw_interrupt_default_config();

    /* tick init */
    HAL_SetTickFreq(1000 / RT_TICK_PER_SECOND);
    rt_hw_interrupt_install(TICK_IRQn, tick_isr, RT_NULL, "tick");
    rt_hw_interrupt_umask(TICK_IRQn);
    HAL_NVIC_SetPriority(TICK_IRQn, NVIC_PERIPH_PRIO_LOWEST, NVIC_PERIPH_SUB_PRIO_LOWEST);
#ifdef RT_USING_SYSTICK
    HAL_SYSTICK_CLKSourceConfig(HAL_SYSTICK_CLKSRC_CORE);
    HAL_SYSTICK_Config((SYSTICK_CLK / RT_TICK_PER_SECOND) - 1);
    HAL_SYSTICK_Enable();
#else
    HAL_TIMER_Init(TICK_TIMER, TIMER_FREE_RUNNING);
    HAL_TIMER_SetCount(TICK_TIMER, (PLL_INPUT_OSC_RATE / RT_TICK_PER_SECOND) - 1);
    HAL_TIMER_Start_IT(TICK_TIMER);
#endif

#ifdef RT_USING_PIN
    rt_hw_iomux_config();
#endif

#ifdef RT_USING_CRU
    clk_init(clk_inits, false);
    clk_disable_unused(clks_unused);
#endif

    rt_system_heap_init((void *)__heap_start, (void *)__heap_end);
    rt_memheap_init(&_sram0_heap, "sram0", (void *)__SRAM0_START, __SRAM0_SIZE);
    rt_memheap_init(&_pmusram_heap, "pmusram", (void *)__PMUSRAM_START, __PMUSRAM_SIZE);

#ifdef RT_USING_UART
    rt_hw_usart_init();
#endif
#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    rt_thread_idle_sethook(rt_idle_wfi);

    rt_components_board_init();

    return 0;
}
