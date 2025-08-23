/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-01-11     Jason Zhu    first implementation
 * 2022-07-08     Steven Liu   Add rt_hw_iomux_config
*/

#include <rthw.h>
#include <rtthread.h>

#include "board.h"
#include "iomux.h"
#include "hal_base.h"
#include "hal_bsp.h"
#include "timer.h"

#ifdef RT_USING_UART
#include "drv_uart.h"
#endif

#ifdef RT_USING_I2C
#include "drv_i2c.h"
#endif

#if defined(RT_USING_UART0)
RT_WEAK const struct uart_board g_uart0_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart0",
};
#endif /* RT_USING_UART0 */

#if defined(RT_USING_UART2)
RT_WEAK const struct uart_board g_uart2_board =
{
    .baud_rate = UART_BR_1500000,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart2",
};
#endif /* RT_USING_UART2 */

#ifdef RT_USING_I2C
const struct rockchip_i2c_config rockchip_i2c_config_table[] =
{
    {
        .id = I2C4,
        .speed = I2C_400K,
    },
    { /* sentinel */ }
};
#endif

void rt_hw_board_init(void)
{
    rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
    rt_hw_interrupt_init();
    sysTick_config(SCR1_CORE_FREQUECY / RT_TICK_PER_SECOND);
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

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

}
