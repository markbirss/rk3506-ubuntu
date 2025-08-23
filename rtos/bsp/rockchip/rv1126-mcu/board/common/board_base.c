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

#include <rthw.h>
#include "board.h"
#include "hal_base.h"
#include "hal_bsp.h"
#include "timer.h"

#ifdef RT_USING_UART
#include "drv_uart.h"
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

int rt_hw_board_init(void)
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

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

    return 0;
}
