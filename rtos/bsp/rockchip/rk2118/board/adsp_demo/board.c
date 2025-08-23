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

#if defined(RT_USING_CLK_CONFIG0)
const struct clk_init clk_inits[] =
{
    INIT_CLK("PLL_GPLL", PLL_GPLL, 800000000),
    INIT_CLK("PLL_VPLL0", PLL_VPLL0, 983040000),
    INIT_CLK("PLL_VPLL1", PLL_VPLL1, 903168000),
    INIT_CLK("PLL_GPLL_DIV", PLL_GPLL_DIV, 200000000),
    INIT_CLK("PLL_VPLL0_DIV", PLL_VPLL0_DIV, 122880000),
    INIT_CLK("PLL_VPLL1_DIV", PLL_VPLL1_DIV, 112896000),
    INIT_CLK("CLK_DSP0_SRC", CLK_DSP0_SRC, 400000000),
    INIT_CLK("CLK_DSP0", CLK_DSP0, 700000000),
    INIT_CLK("CLK_DSP1", CLK_DSP1, 491520000),
    INIT_CLK("CLK_DSP2", CLK_DSP2, 491520000),
    INIT_CLK("ACLK_NPU", ACLK_NPU, 400000000),
    INIT_CLK("HCLK_NPU", HCLK_NPU, 150000000),
    INIT_CLK("CLK_STARSE0", CLK_STARSE0, 400000000),
    INIT_CLK("CLK_STARSE1", CLK_STARSE1, 400000000),
    INIT_CLK("ACLK_BUS", ACLK_BUS, 300000000),
    INIT_CLK("HCLK_BUS", HCLK_BUS, 150000000),
    INIT_CLK("PCLK_BUS", PCLK_BUS, 150000000),
    INIT_CLK("ACLK_HSPERI", ACLK_HSPERI, 150000000),
    INIT_CLK("ACLK_PERIB", ACLK_PERIB, 150000000),
    INIT_CLK("HCLK_PERIB", HCLK_PERIB, 150000000),
    INIT_CLK("CLK_INT_VOICE0", CLK_INT_VOICE0, 49152000),
    INIT_CLK("CLK_INT_VOICE1", CLK_INT_VOICE1, 45158400),
    INIT_CLK("CLK_INT_VOICE2", CLK_INT_VOICE2, 98304000),
    INIT_CLK("CLK_FRAC_UART0", CLK_FRAC_UART0, 64000000),
    INIT_CLK("CLK_FRAC_UART1", CLK_FRAC_UART1, 48000000),
    INIT_CLK("CLK_FRAC_VOICE0", CLK_FRAC_VOICE0, 24576000),
    INIT_CLK("CLK_FRAC_VOICE1", CLK_FRAC_VOICE1, 22579200),
    INIT_CLK("CLK_FRAC_COMMON0", CLK_FRAC_COMMON0, 12288000),
    INIT_CLK("CLK_FRAC_COMMON1", CLK_FRAC_COMMON1, 11289600),
    INIT_CLK("CLK_FRAC_COMMON2", CLK_FRAC_COMMON2, 8192000),
    INIT_CLK("PCLK_PMU", PCLK_PMU, 100000000),
    INIT_CLK("CLK_32K_FRAC", CLK_32K_FRAC, 32768),
    INIT_CLK("CLK_MAC_OUT", CLK_MAC_OUT, 50000000),
    INIT_CLK("CLK_CORE_CRYPTO", CLK_CORE_CRYPTO, 300000000),
    INIT_CLK("CLK_PKA_CRYPTO", CLK_PKA_CRYPTO, 300000000),
    /* Audio */
    INIT_CLK("MCLK_PDM", MCLK_PDM, 100000000),
    INIT_CLK("CLKOUT_PDM", CLKOUT_PDM, 3072000),
    INIT_CLK("MCLK_SPDIFTX", MCLK_SPDIFTX, 6144000),
    INIT_CLK("MCLK_OUT_SAI0", MCLK_OUT_SAI0, 24576000),
    INIT_CLK("MCLK_OUT_SAI1", MCLK_OUT_SAI1, 24576000),
    INIT_CLK("MCLK_OUT_SAI2", MCLK_OUT_SAI2, 24576000),
    INIT_CLK("MCLK_OUT_SAI3", MCLK_OUT_SAI3, 24576000),
    INIT_CLK("MCLK_OUT_SAI4", MCLK_OUT_SAI4, 24576000),
    INIT_CLK("MCLK_OUT_SAI5", MCLK_OUT_SAI5, 24576000),
    INIT_CLK("MCLK_OUT_SAI6", MCLK_OUT_SAI6, 24576000),
    INIT_CLK("MCLK_OUT_SAI7", MCLK_OUT_SAI7, 24576000),
    INIT_CLK("CLK_TSADC", CLK_TSADC, 1200000),
    INIT_CLK("CLK_TSADC_TSEN", CLK_TSADC_TSEN, 12000000),
    INIT_CLK("SCLK_SAI0", SCLK_SAI0, 24576000),
    INIT_CLK("SCLK_SAI1", SCLK_SAI1, 24576000),
    INIT_CLK("SCLK_SAI2", SCLK_SAI2, 24576000),
    INIT_CLK("SCLK_SAI3", SCLK_SAI3, 24576000),
    INIT_CLK("SCLK_SAI4", SCLK_SAI4, 24576000),
    INIT_CLK("SCLK_SAI5", SCLK_SAI5, 24576000),
    INIT_CLK("SCLK_SAI6", SCLK_SAI6, 24576000),
    INIT_CLK("SCLK_SAI7", SCLK_SAI7, 24576000),
    { /* sentinel */ },
};
#endif

const struct clk_unused clks_unused[] =
{
    {0, 9, 0xffffffff}, /* disable npu */
    {0, 12, 0x00180018}, /* disable star1 */
    {0, 13, 0x00070007}, /* disable star1 */
    {0, 14, 0x00700070}, /* disable usb */
    {0, 15, 0x00020002}, /* disable usb */
    {0, 34, 0x07c007c0}, /* disable vop and sdmmc */
    {0, 38, 0xf0c0f0c0}, /* disable gmac and emmc */
    {0, 39, 0x00030003}, /* disable gmac*/
    {1, 2, 0x03000300}, /* disable osbphy */
    { /* sentinel */ },
};

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

    clk_init(clk_inits, false);
    /* disable not used clks */
    clk_disable_unused(clks_unused);

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
