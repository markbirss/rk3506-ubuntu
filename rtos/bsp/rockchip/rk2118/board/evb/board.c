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
#ifdef RT_USING_GMAC
#include "drv_gmac.h"
#endif
#ifdef RT_USING_I2C
#include "drv_i2c.h"
#endif
#ifdef RT_USING_PWM_REGULATOR
#include "drv_pwm_regulator.h"
#endif
#ifdef HAL_PWR_MODULE_ENABLED
#include "drv_regulator.h"
#endif

#ifdef RT_USING_PWM_REMOTECTL
#include "drv_pwm_remotectl.h"
#endif

#ifdef RT_USING_PM
#include "drv_pm.h"
#endif

#ifdef RT_USING_SDIO
#include "drv_sdio.h"
#include <drivers/mmcsd_core.h>
#endif

#ifdef RT_USING_SDIO
#include "drv_sdio.h"
#include <drivers/mmcsd_core.h>
#endif

#ifdef RT_USING_CODEC_TAS5805M
#include "tas5805m.h"
#endif

#ifdef RT_USING_RKPLAYER_APP
#include "rkplayer.h"
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
        .is_pwr_gpio = false,
    },
#endif
#ifdef RT_USING_SDIO1
    {
        .flags = MMCSD_BUSWIDTH_8 | MMCSD_MUTBLKWRITE | MMCSD_SUP_HIGHSPEED,
        .irq = SDMMC_IRQn,
        .base = MMC1_BASE,
        .clk_id = CCLK_SRC_EMMC,
        .clk_gate = CCLK_SRC_EMMC_GATE,
        .hclk_gate = HCLK_EMMC_GATE,
        .freq_min = 100000,
        .freq_max = 50000000,
        .control_id = 1,
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

#ifdef RT_USING_GMAC
const struct rockchip_eth_config rockchip_eth_config_table[] =
{
#ifdef RT_USING_GMAC0
    {
        .id = GMAC,
        .mode = RMII_MODE,
        .phy_addr = 0,

        .external_clk = false,

        .reset_gpio_bank = GPIO0,
        .reset_gpio_num = GPIO_PIN_A0,
        .reset_delay_ms = {1, 20, 100},
    },
#endif
};
#endif

#ifdef HAL_PWR_MODULE_ENABLED
#ifdef RT_USING_PWM_REGULATOR
RT_WEAK struct pwr_pwm_info_desc pwm_pwr_desc[] =
{
    {
        .name = "pwm0",
        .chanel = 3,
        .invert = true,
    },
    {
        .name = "pwm0",
        .chanel = 2,
        .invert = true,
    },
    { /* sentinel */ },
};

RT_WEAK struct regulator_desc regulators[] =
{
    {
        .flag = REGULATOR_FLG_PWM | REGULATOR_FLG_LOCK,
        .desc.pwm_desc = {
            .flag = DESC_FLAG_LINEAR(PWR_CTRL_VOLT_RUN | PWR_CTRL_PWR_EN),
            .info = {
                .pwrId = PWR_ID_CORE,
            },
            .pwrId = PWR_ID_CORE,
            .period = 25000,
            .minVolt = 810000,
            .maxVlot = 1000000,
            .voltage = 900000,
            .pwm = &pwm_pwr_desc[0],
        },
    },
    {
        .flag = REGULATOR_FLG_PWM | REGULATOR_FLG_LOCK,
        .desc.pwm_desc = {
            .flag = DESC_FLAG_LINEAR(PWR_CTRL_VOLT_RUN | PWR_CTRL_PWR_EN),
            .info = {
                .pwrId = PWR_ID_DSP_CORE,
            },
            .pwrId = PWR_ID_DSP_CORE,
            .period = 25000,
            .minVolt = 800000,
            .maxVlot = 1100000,
            .voltage = 900000,
            .pwm = &pwm_pwr_desc[1],
        },
    },
    { /* sentinel */ },
};

RT_WEAK const struct regulator_init regulator_inits[] =
{
    REGULATOR_INIT("vdd_core",  PWR_ID_CORE,       900000, 1, 900000, 1),
    REGULATOR_INIT("vdd_dsp",   PWR_ID_DSP_CORE,   1000000, 1, 900000, 1),
    { /* sentinel */ },
};
#endif
#endif

#ifdef RT_USING_PWM_REMOTECTL
const struct remotectl_pwm_info rtl_pwm_info_table[] =
{
    {
        .name = "pwm0",
        .channel = 1,
        .pwrmatch_value = 0xdf20fb04,
        .rtl_usercode = 0x04fb,
    },
    { /* sentinel */ }
};
#endif

#ifdef RT_USING_CODEC_TAS5805M
const struct tas5805m_board_cfg tas5805m_devices[] =
{
    {
        .id = 0,
        .i2c_bus_name = "i2c0",
        .i2c_addr = 0x2c,
        .pdn_gpio_bank = GPIO_BANK3,
        .pdn_gpio_grp = GPIO3,
        .pdn_gpio_pin = GPIO_PIN_A4,
        .adr_gpio_bank = GPIO_BANK3,
        .adr_gpio_grp = GPIO3,
        .adr_gpio_pin = GPIO_PIN_A5,
    },
    { .id = 1, .i2c_bus_name = "i2c0", .i2c_addr = 0x2d },
    { .id = 2, .i2c_bus_name = "i2c0", .i2c_addr = 0x2e },
    { .id = -1 },
};
#endif

#ifdef RT_USING_RKPLAYER_APP
const struct rkplayer_config_info config_info_table[] =
{
    {
        .uart_port = "no",
        .spidif_in = "spdif1",
        .i2s_in = "sai0",
        .aux_in = "sai4",
        .opt_in = "spdif1",
        .bt_in = "sai4",
        .bt_slave = true,
        .led_name = "no",
    },
    { /* sentinel */ }
};
#endif

void usb_vbus_pin_enable()
{
    WRITE_REG(GPIO2_IOC->GPIO_SARADC_CON, 0x00800080);
}

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

#ifdef RT_USING_PM
    rk_rt_pm_init();
#endif

    /* Initial usart deriver, and set console device */
    rt_hw_usart_init();

#ifdef RT_USING_PWM_REGULATOR
#ifdef HAL_PWR_MODULE_ENABLED
    regulator_desc_init(regulators);
#endif
#endif

#ifdef RT_USING_SDIO
    rt_board_mmc_init();
#endif

    usb_phy_init();

    clk_init(clk_inits, false);

    /* Update system core clock after clk_init */
    SystemCoreClockUpdate();

    usb_vbus_pin_enable();

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
