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

#include "hal_base.h"
#include "board.h"
#include "hal_bsp.h"

#ifdef RT_USING_PIN
#include "iomux.h"
#endif

#ifdef RT_USING_CRU
#include "drv_clock.h"
#endif

#ifdef RT_USING_UART
#include "drv_uart.h"
#endif

#ifdef RT_USING_AUDIO
#include "rk_audio.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
#include "drv_regulator.h"
#endif

#ifdef RT_USING_I2C
#include "drv_i2c.h"
#endif

#ifdef RT_USING_PMIC
#include "drv_pmic.h"
#endif

#ifdef RT_USING_CRU
static const struct clk_init clk_inits[] =
{
    INIT_CLK("PLL_GPLL", PLL_GPLL, 104000000),
    INIT_CLK("HCLK_MCU", HCLK_MCU, 104000000),
    INIT_CLK("PCLK_MCU", PCLK_MCU, 26000000),
    INIT_CLK("CLK_BT52M", CLK_BT52M, 52000000),
    INIT_CLK("HCLK_BT", HCLK_BT, 104000000),
    INIT_CLK("PCLK_PMU", PCLK_PMU, 26000000),
    INIT_CLK("CLK_SFC", CLK_SFC, 80000000),
    INIT_CLK("PLL_CPLL", PLL_CPLL, 240000000),
    INIT_CLK("CLK_DSP", CLK_DSP, 120000000),
    INIT_CLK("STCLK_M3", STCLK_M3, PLL_INPUT_OSC_RATE),
    { /* sentinel */ }
};
#endif

#ifdef RT_USING_AUDIO
const struct audio_card_desc rk_board_audio_cards[] =
{
#ifdef RT_USING_AUDIO_CARD_PDM_MIC
    {
        .name = "pdm",
        .dai = PDM0,
        .codec = NULL,
        .capture = true,
        .format = AUDIO_FMT_PDM,
    },
#endif
    { /* sentinel */ }
};
#endif

#ifdef HAL_PWR_MODULE_ENABLED

struct pwr_i2c_desc pmic_pwr_i2c_desc =
{
    .name = "i2c2",
    .addr = RK809_I2C_ADDR,
};

static struct regulator_desc regulators[] =
{
    /****** vdd0v5 **********/
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK817_BUCK(RK817_ID_DCDC1, PWR_ID_LOG),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /****** vdd0v65 **********/
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK817_BUCK(RK817_ID_DCDC2, PWR_ID_TOP),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /****** vcc0v65 **********/
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK817_BUCK(RK817_ID_DCDC4, PWR_ID_MEMORY),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    { /* sentinel */ },
};

const struct regulator_init regulator_inits[] =
{
    REGULATOR_INIT("vdd_log", PWR_ID_LOG, 500000, 1, 500000, 1),
    REGULATOR_INIT("vdd_top", PWR_ID_TOP, 650000, 1, 650000, 1),
    REGULATOR_INIT("vdd_memory", PWR_ID_MEMORY, 650000, 1, 650000, 1),
    { /* sentinel */ },
};

#if defined(RT_USING_PMIC_RK817)
const ePMIC_SleepPinFunc g_pmic_sleep_pin_func = PMIC_SLEEP_SLEEP_FUNC;
const ePMIC_SleepPinPolarity g_pmic_sleep_pin_polarity = PMIC_SLEEP_ACTIVE_HIGH;
#endif
#endif

#ifdef RT_USING_I2C
const struct rockchip_i2c_config rockchip_i2c_config_table[] =
{
    {
        .id = I2C2,
        .speed = I2C_100K,
    },
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

static void systick_isr(int vector, void *param)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_IncTick();
    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

/**
 * This function will initial the related board.
 */
void rt_hw_board_init()
{
    /* HAL_Init */
    HAL_Init();

    /* System tick init */
    HAL_SetTickFreq(1000 / RT_TICK_PER_SECOND);
    rt_hw_interrupt_install(SysTick_IRQn, systick_isr, RT_NULL, "tick");
    HAL_NVIC_SetPriority(SysTick_IRQn, NVIC_PERIPH_PRIO_LOWEST, NVIC_PERIPH_SUB_PRIO_LOWEST);
    GRF->MCU_STCALIB = 0; /* Enable reference clock switch */
    HAL_SYSTICK_CLKSourceConfig(HAL_SYSTICK_CLKSRC_EXT);
    HAL_SYSTICK_Config((PLL_INPUT_OSC_RATE / RT_TICK_PER_SECOND) - 1);
    HAL_SYSTICK_Enable();

#ifdef RT_USING_PIN
    rt_hw_iomux_config();
#endif

#ifdef RT_USING_CRU
    clk_init(clk_inits, true);
#endif

#ifdef RT_USING_PMU
    HAL_PD_Off(PD_BT_AUDIO);
#endif

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

#ifdef HAL_PWR_MODULE_ENABLED
    regulator_desc_init(regulators);
#endif

    /* set arm pll */
}

#ifdef RT_USING_CRU
static int clk_protect_critical(void)
{
    uint32_t gate_ids[] = {ACLK_DSP_NOC_GATE, CLK_DSP_GATE};
    struct clk_gate *clk_gate;
    int i;

    for (i = 0; i < HAL_ARRAY_SIZE(gate_ids); i++)
    {
        clk_gate = get_clk_gate_from_id(gate_ids[i]);
        clk_enable(clk_gate);
    }

    return RT_EOK;
}
INIT_PREV_EXPORT(clk_protect_critical);
#endif
