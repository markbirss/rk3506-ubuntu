/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-01     Frank Wang   the first version
 */

#include <rthw.h>
#include <rtthread.h>
#include <mmu.h>

#include "board.h"
#include "drv_flash_partition.h"
#include "drv_uart.h"
#include "hal_base.h"
#include "hal_bsp.h"

#ifdef RT_USING_UART
#include "drv_uart.h"
#endif

#define RK_PLL_MODE_SLOW   0
#define RK_PLL_MODE_NORMAL 1
#define POST_DIV2  1
#define POST_DIV1 2
#define REFDIV 1
#define FBDIV 0x54
#define DSMPD 1

struct mem_desc platform_mem_desc[] =
{
    {0x62000000, 0x80000000 - 1, 0x62000000, NORMAL_MEM},
    {0x10080000, 0x1039c000 - 1, 0x10080000, DEVICE_MEM},
    {0x20000000, 0x28010000 - 1, 0x20000000, DEVICE_MEM},
    {0x30000000, 0x35010000 - 1, 0x30000000, DEVICE_MEM},
};

#if defined(RT_USING_UART0)
const struct uart_board g_uart0_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart0",
};
#endif /* RT_USING_UART0 */

#if defined(RT_USING_UART2)
const struct uart_board g_uart2_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart2",
};
#endif /* RT_USING_UART2 */

const rt_uint32_t platform_mem_desc_size = sizeof(platform_mem_desc) / sizeof(platform_mem_desc[0]);

static void delay_us(uint32_t us)
{
    volatile uint32_t len;
    for (; us > 0; us --)
        for (len = 0; len < 20; len++);
}

/* force set to 1.08G as default */
static void rt_hw_set_cpu_pll(void)
{
    int delay = 100;

    /* Force PLL into slow mode to ensure output stable clock */
    CRU->APLL_CON[3] = VAL_MASK_WE(0x1 << 8, RK_PLL_MODE_SLOW << 8);

    /* Pll Power down */
    CRU->APLL_CON[3] = VAL_MASK_WE(0x1, 1 << 0);

    /* Pll Config */
    CRU->APLL_CON[1] = VAL_MASK_WE(0x7 << 12,  POST_DIV2 << 12);
    CRU->APLL_CON[0] = VAL_MASK_WE(0xfff, FBDIV << 0);
    CRU->APLL_CON[1] = VAL_MASK_WE(0x7 << 8, POST_DIV1 << 8);
    CRU->APLL_CON[1] = VAL_MASK_WE(0x3f, REFDIV << 0);
    CRU->APLL_CON[3] = VAL_MASK_WE(0x1 << 3, DSMPD << 3);

    /* Pll Power up */
    CRU->APLL_CON[3] = VAL_MASK_WE(0x1, 0 << 0);

    /* Waiting for pll lock */
    while (delay > 0)
    {
        if (CRU->APLL_CON[2] & (1 << 31))
            break;

        delay_us(10);
        delay--;
    }

    if (delay == 0)
    {
        rt_kprintf("cpu pll set timeout!\n");
        return;
    }

    /* Force PLL into normal mode */
    CRU->APLL_CON[3] = VAL_MASK_WE(0x1 << 8, RK_PLL_MODE_NORMAL << 8);

    rt_kprintf("APLL: 0x%08x:0x%08x:0x%08x:0x%08x\n", CRU->APLL_CON[0],
               CRU->APLL_CON[1], CRU->APLL_CON[2], CRU->APLL_CON[3]);
}

static void set_uart_jtag_iomux(bool uart_en)
{
    if (uart_en)
        WRITE_REG((*(uint32_t *)(0x1030002c)), 0x003c0014);
    else /* GRF_GPIO2D_IOMUX[5:2] = {1010} */
        WRITE_REG((*(uint32_t *)(0x1030002c)), 0x003c0028);
}

/**
 * This function will initial RV1108 board.
 */
void rt_hw_board_init()
{
    set_uart_jtag_iomux(true);

    /* HAL_Init */
    HAL_Init();

    /* Initial usart deriver, and set console device */
#ifdef RT_USING_UART
    rt_hw_usart_init();
#endif

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    /* hal bsp init */
    BSP_Init();

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

    /* set arm pll */
    rt_hw_set_cpu_pll();
}
