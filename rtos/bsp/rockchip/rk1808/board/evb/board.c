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
#include <mmu.h>

#ifdef RT_USING_CRU
#include "drv_clock.h"
#endif
#ifdef RT_USING_I2C
#include "drv_i2c.h"
#endif
#ifdef RT_USING_PIN
#include "iomux.h"
#endif
#ifdef RT_USING_UART
#include "drv_uart.h"
#endif
#ifdef HAL_PWR_MODULE_ENABLED
#include "drv_regulator.h"
#endif
#ifdef RT_USING_PMIC
#include "drv_pmic.h"
#endif

#ifdef RT_USING_SDIO
#include "drv_sdio.h"
#include <drivers/mmcsd_core.h>
#endif

#ifdef RT_USING_CRU
static const struct clk_init clk_inits[] =
{
    INIT_CLK("PLL_APLL", PLL_APLL, 1200000000),
    INIT_CLK("HSCLK_BUS", HSCLK_BUS, 50000000),
    INIT_CLK("MSCLK_BUS", MSCLK_BUS, 50000000),
    INIT_CLK("LSCLK_BUS", LSCLK_BUS, 50000000),
    INIT_CLK("MSCLK_PERI", MSCLK_PERI, 50000000),
    INIT_CLK("LSCLK_PERI", LSCLK_PERI, 50000000),
    INIT_CLK("PCLK_PDPMU", PCLK_PDPMU, 10000000),
    INIT_CLK("CLK_CRYPTO", CLK_CRYPTO, 50000000),
    INIT_CLK("CLK_CRYPTO_APK", CLK_CRYPTO_APK, 50000000),
    INIT_CLK("CLK_NPU", CLK_NPU, 100000000),
    INIT_CLK("ACLK_NPU", ACLK_NPU, 100000000),
    INIT_CLK("ACLK_NPU2MEM", ACLK_NPU2MEM, 100000000),
    INIT_CLK("HSCLK_PCIE", HSCLK_PCIE, 100000000),
    INIT_CLK("LSCLK_PCIE", LSCLK_PCIE, 100000000),
    INIT_CLK("ACLK_IMEM", ACLK_IMEM, 100000000),
    INIT_CLK("HSCLK_IMEM", HSCLK_IMEM, 100000000),
    INIT_CLK("PLL_GPLL", PLL_GPLL, 1188000000),
    INIT_CLK("PLL_CPLL", PLL_CPLL, 1000000000),
    INIT_CLK("PLL_PPLL", PLL_PPLL, 100000000),
    INIT_CLK("HSCLK_BUS", HSCLK_BUS, 300000000),
    INIT_CLK("MSCLK_BUS", MSCLK_BUS, 200000000),
    INIT_CLK("LSCLK_BUS", LSCLK_BUS, 100000000),
    INIT_CLK("MSCLK_PERI", MSCLK_PERI, 200000000),
    INIT_CLK("LSCLK_PERI", LSCLK_PERI, 100000000),
    INIT_CLK("CLK_CRYPTO", CLK_CRYPTO, 300000000),
    INIT_CLK("CLK_CRYPTO_APK", CLK_CRYPTO_APK, 300000000),
    INIT_CLK("CLK_NPU", CLK_NPU, 400000000),
    INIT_CLK("ACLK_NPU", ACLK_NPU, 300000000),
    INIT_CLK("ACLK_NPU2MEM", ACLK_NPU2MEM, 300000000),
    INIT_CLK("HSCLK_PCIE", HSCLK_PCIE, 300000000),
    INIT_CLK("LSCLK_PCIE", LSCLK_PCIE, 100000000),
    INIT_CLK("ACLK_IMEM", ACLK_IMEM, 300000000),
    INIT_CLK("HSCLK_IMEM", HSCLK_IMEM, 300000000),
    INIT_CLK("PCLK_PDPMU", PCLK_PDPMU, 100000000),
    INIT_CLK("CLK_UART4", CLK_UART4, 16000000),
    INIT_CLK("CLK_SDIO", CLK_SDIO, 50000000),
    INIT_CLK("CLK_SDIO_PLL", CLK_SDIO_PLL, 50000000),
    { /* sentinel */ }
};
#endif

#ifdef HAL_PWR_MODULE_ENABLED

static struct pwr_i2c_desc dcdc_pwr_i2c_desc =
{
    .name = "i2c0",
    .addr = TCS4525_I2C_ADDR,
};

struct pwr_i2c_desc pmic_pwr_i2c_desc =
{
    .name = "i2c0",
    .addr = RK809_I2C_ADDR,
};

static struct regulator_desc regulators[] =
{
    /****** vdd_npu **********/
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = {
            .flag = DESC_FLAG_LINEAR(PWR_CTRL_VOLT_SSPD),
            .info = {
                .pwrId = PWR_ID_DSP_CORE,
            },
            .i2c = &dcdc_pwr_i2c_desc,
            PWR_DESC_I2C8_SHIFT_RUN(0x10, 0),
            PWR_DESC_I2C8_SHIFT_SSPD(0x11, 0),
            PWR_DESC_I2C8_SHIFT_EN(0x10, 1 << 7),
            PWR_DESC_I2C8_SHIFT_SSPD_EN(0x11, 1 << 7),
            .voltMask = 0x7f,
            PWR_DESC_LINEAR_VOLT(600000, 1300000, 6250),
        },
    },
    /****** vdd_log **********/
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK817_BUCK(RK817_ID_DCDC1,  PWR_ID_LOG),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,

    },
    /****** vdd_cpu **********/
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK817_BUCK(RK817_ID_DCDC2, PWR_ID_CORE),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    { /* sentinel */ }
};

const struct regulator_init regulator_inits[] =
{
    REGULATOR_INIT("vdd_npu", PWR_ID_DSP_CORE, 875000, 1, 875000, 1),
    REGULATOR_INIT("vdd_log", PWR_ID_LOG, 800000, 1, 800000, 1),
    REGULATOR_INIT("vdd_arm", PWR_ID_CORE, 800000, 1, 800000, 1),
    { /* sentinel */ },
};
#endif

#ifdef RT_USING_SDIO
struct rk_mmc_platform_data rk_mmc_table[] =
{
#ifdef RT_USING_SDIO0
    {
        .flags = MMCSD_BUSWIDTH_4 | MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ | MMCSD_SUP_HIGHSPEED,
        .irq = SDIO_IRQn,
        .base = SDIO_BASE,
        .clk_id = CLK_SDIO_PLL,
        .freq_min = 100000,
        .freq_max = 50000000,
        .control_id = 0,
    },
#endif
    { /* sentinel */ },
};
#endif

struct mem_desc platform_mem_desc[] =
{
    {0xfec01000, 0xfee00000 - 1, 0xfec01000, NORMAL_MEM}, /* SYSTEM_SRAM */
    {0xf8000000, 0xfec00000 - 1, 0xf8000000, DEVICE_MEM},
    {0xfee00000, 0xffde0000 - 1, 0xfee00000, DEVICE_MEM},
};

const rt_uint32_t platform_mem_desc_size = sizeof(platform_mem_desc) / sizeof(platform_mem_desc[0]);

static void set_uart_jtag_iomux(bool uart_en)
{

}

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

#if defined(RT_USING_UART3)
const struct uart_board g_uart3_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart3",
};
#endif /* RT_USING_UART3 */

#if defined(RT_USING_UART4)
const struct uart_board g_uart4_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart4",
};
#endif /* RT_USING_UART4 */

#if defined(RT_USING_UART5)
const struct uart_board g_uart5_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart5",
};
#endif /* RT_USING_UART5 */

#if defined(RT_USING_UART6)
const struct uart_board g_uart6_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart6",
};
#endif /* RT_USING_UART6 */

#if defined(RT_USING_UART7)
const struct uart_board g_uart7_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart7",
};
#endif /* RT_USING_UART7 */

/**
 * This function will initial the related board.
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

#ifdef RT_USING_PIN
    rt_hw_iomux_config();
#endif

#ifdef RT_USING_CRU
    clk_init(clk_inits, true);
#endif

#ifdef HAL_PWR_MODULE_ENABLED
    regulator_desc_init(regulators);
#endif
}
