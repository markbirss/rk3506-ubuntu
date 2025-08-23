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
#include "hal_base.h"
#include "hal_bsp.h"
#include "iomux.h"
#ifdef RT_USING_I2C
#include "drv_i2c.h"
#endif

#ifdef RT_USING_SDIO
#include "drv_sdio.h"
#include <drivers/mmcsd_core.h>
#endif

extern const struct clk_init clk_inits[];

#ifdef RT_USING_SDIO
RT_WEAK struct rk_mmc_platform_data rk_mmc_table[] =
{
#ifdef RT_USING_SDIO0
    {
        .flags = MMCSD_BUSWIDTH_4 | MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ | MMCSD_SUP_HIGHSPEED,
        .irq = SDIO_IRQn,
        .base = MMC0_BASE,
        .clk_id = CLK_SDMMC,
        .clk_gate = CLK_SDMMC_GATE,
        .hclk_gate = HCLK_SDMMC_GATE,
        .freq_min = 100000,
        .freq_max = 50000000,
        .control_id = 0,
        .is_pwr_gpio = true, /* using gpio to control power */
        .pwr_gpio = GPIO0,
        .pwr_gpio_bank = GPIO_BANK0,
        .pwr_gpio_pin = GPIO_PIN_C5,
    },
#endif
    { /* sentinel */ },
};
#endif

#ifdef RT_USING_I2C
const struct rockchip_i2c_config rockchip_i2c_config_table[] =
{
    {
        .id = I2C0,
        .speed = I2C_100K,
    },
    { /* sentinel */ }
};
#endif

#if defined(RT_USING_UART0)
const struct uart_board g_uart0_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart0",
};
#endif /* RT_USING_UART0 */

#if defined(RT_USING_UART2)
const struct uart_board g_uart2_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart2",
};
#endif /* RT_USING_UART2 */

/**
 * This function will initial Pisces board.
 */
void rt_hw_board_init()
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

    usb_phy_init();

    clk_init(clk_inits, true);

    /* Update system core clock after clk_init */
    SystemCoreClockUpdate();

#if defined(RT_USING_CONSOLE) && defined(RK2118_CPU_CORE0)
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
