/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-10     Cliff Chen   first implementation
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
#ifdef RT_USING_PMIC
#include "drv_pmic.h"
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

#ifdef RT_USING_PMIC_RK816
#include <rk816.h>
#endif

#ifdef RT_USING_COREDUMP
#include "../drivers/coredump/drv_coredump.h"
#endif

#ifdef RT_USING_MODULE
#define DATA_EXEC_FLAG   0U
#else
#define DATA_EXEC_FLAG   1U
#endif

#ifdef RT_USING_CRU
RT_WEAK const struct clk_init clk_inits[] =
{
    INIT_CLK("SCLK_SHRM", SCLK_SHRM, 10 * MHZ),
    INIT_CLK("PCLK_SHRM", PCLK_SHRM, 10 * MHZ),
    INIT_CLK("PCLK_ALIVE", PCLK_ALIVE, 10 * MHZ),
    INIT_CLK("HCLK_ALIVE", HCLK_ALIVE, 10 * MHZ),
    INIT_CLK("HCLK_M4", HCLK_M4, 10 * MHZ),
    INIT_CLK("ACLK_LOGIC", ACLK_LOGIC, 10 * MHZ),
    INIT_CLK("HCLK_LOGIC", HCLK_LOGIC, 10 * MHZ),
    INIT_CLK("PCLK_LOGIC", PCLK_LOGIC, 10 * MHZ),
    INIT_CLK("SCLK_SFC_SRC", SCLK_SFC_SRC, 5 * MHZ),
    INIT_CLK("SCLK_SFC1_SRC", SCLK_SFC1_SRC, 5 * MHZ),
    INIT_CLK("PLL_GPLL", PLL_GPLL, 1188 * MHZ),
    INIT_CLK("PLL_CPLL", PLL_CPLL, 1188 * MHZ),
    INIT_CLK("SCLK_SFC_SRC", SCLK_SFC_SRC, 50 * MHZ),
    INIT_CLK("HCLK_M4", HCLK_M4, 300 * MHZ),
    INIT_CLK("ACLK_DSP", ACLK_DSP, 400 * MHZ),
    INIT_CLK("ACLK_LOGIC", ACLK_LOGIC, 300 * MHZ),
    INIT_CLK("HCLK_LOGIC", HCLK_LOGIC, 150 * MHZ),
    INIT_CLK("PCLK_LOGIC", PCLK_LOGIC, 150 * MHZ),
    INIT_CLK("SCLK_SHRM", SCLK_SHRM, 300 * MHZ),
    INIT_CLK("PCLK_SHRM", PCLK_SHRM, 100 * MHZ),
    INIT_CLK("PCLK_ALIVE", PCLK_ALIVE, 100 * MHZ),
    INIT_CLK("HCLK_ALIVE", HCLK_ALIVE, 100 * MHZ),
    { /* sentinel */ },
};

RT_WEAK const struct clk_unused clks_unused[] =
{
    {0, 0, 0x00030003},
    {0, 5, 0x00ee00ee},
    {0, 6, 0x048d048d},
    {0, 7, 0x00110011},
    {0, 11, 0x40e040e0},
    {0, 12, 0x90709070},
    {0, 13, 0xe203e203},
    {0, 14, 0xa6e1a6e1},
    { /* sentinel */ },
};
#endif

#if defined(RT_USING_UART0)
RT_WEAK const struct uart_board g_uart0_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart0",
};
#endif /* RT_USING_UART0 */

#if defined(RT_USING_UART1)
RT_WEAK const struct uart_board g_uart1_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart1",
};
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
RT_WEAK const struct uart_board g_uart2_board =
{
    .baud_rate = ROCKCHIP_UART_BAUD_RATE_DEFAULT,
    .dev_flag = ROCKCHIP_UART_SUPPORT_FLAG_DEFAULT,
    .bufer_size = RT_SERIAL_RB_BUFSZ,
    .name = "uart2",
};
#endif /* RT_USING_UART2 */

#ifdef RT_USING_SDIO
RT_WEAK struct rk_mmc_platform_data rk_mmc_table[] =
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

#ifdef RT_USING_PMIC_RK816
RT_WEAK struct pwr_i2c_desc pmic_pwr_i2c_desc =
{
    .name = "i2c0",
    .addr = RK816_I2C_ADDR,
};
#endif

#ifdef RT_USING_FG_RK816
struct rk816_fg_platform_data rk816_fg_pdata =
{
    .ocv_table = {
        3400, 3650, 3693, 3707, 3731, 3749, 3760,
        3770, 3782, 3796, 3812, 3829, 3852, 3882,
        3915, 3951, 3981, 4047, 4086, 4132, 4182,
    },
    .design_capacity = 4000,
    .design_qmax = 4500,
    .bat_res = 85,
    .power_off_thresd = 3400,
    .sample_res = 20,
    .zero_algorithm_vol = 3950,
    .virtual_power = 0,
    .max_soc_offset = 80,
    .monitor_sec = 5,
};
#endif

#ifdef RT_USING_CHARGER_RK816
struct rk816_charger_platform_data rk816_charger_pdata =
{
    .sample_res = 100, // 100mR
};
#endif

#ifdef RT_USING_SENSOR_RK816
static const rt_uint32_t rk816_ntc_table[] =
{
    42450, 33930, 27280, 22070, 17960, 14700, 12090, /* -10 ~ 20'C' */
    10000, 8310,  6490,  5830,  4910,  4160,  3540,  /* 25  ~ 55'C' */
};

RT_WEAK struct rk816_sensor_platform_data rk816_sensor_pdata =
{
    .ntc_table = rk816_ntc_table,
    .ntc_num = 14,
    .ntc_degree_min = -10,
    .ntc_degree_step = 5,
};
#endif

#ifdef HAL_PWR_MODULE_ENABLED

#ifdef RT_USING_PMIC_RK816
RT_WEAK struct regulator_desc regulators[] =
{
    /* BUCK4 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_BUCK4(RK816_ID_DCDC4, PWR_ID_VCCIO_3V3),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO2 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO1_4(RK816_ID_LDO2, PWR_ID_VCCIO_1V8),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO5 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO5_6(RK816_ID_LDO5, PWR_ID_VCC_AUDIO),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO6 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO5_6(RK816_ID_LDO6, PWR_ID_VCC_MIPI),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    { /* sentinel */ },
};

RT_WEAK const struct regulator_init regulator_inits[] =
{
    REGULATOR_INIT("dcdc4", PWR_ID_VCCIO_3V3,  3300000, 1, 3300000, 1),
    REGULATOR_INIT("ldo2",  PWR_ID_VCCIO_1V8,  1800000, 1, 1800000, 1),
    REGULATOR_INIT("ldo5",  PWR_ID_VCC_AUDIO,  3300000, 1, 3300000, 1),
    REGULATOR_INIT("ldo6",  PWR_ID_VCC_MIPI,   3300000, 1, 3300000, 1),
    { /* sentinel */ },
};

#else

RT_WEAK struct regulator_desc regulators[] =
{
    {
        .flag = REGULATOR_FLG_INTREG,
        .desc.intreg_desc = {
            .flag = DESC_FLAG_LINEAR(PWR_FLG_VOLT_SSPD | PWR_FLG_VOLT_ST),
            .info = {
                .pwrId = PWR_ID_CORE,
            },
            PWR_INTREG_SHIFT_RUN(&PMU->LDO_CON[1], PMU_LDO_CON1_MCU_LDOCORE_SFT_SHIFT),
            PWR_INTREG_SHIFT_SSPD(&PMU->LDO_CON[1], PMU_LDO_CON1_PWRMODE_LDOCORE_ADJ_SHIFT),
            PWR_INTREG_SHIFT_ST(&PMU->LDO_STAT, PMU_LDO_STAT_LDO_CORE_ADJ_SHIFT),
            .voltMask = PMU_LDO_CON1_MCU_LDOCORE_SFT_MASK >> PMU_LDO_CON1_MCU_LDOCORE_SFT_SHIFT,
                    PWR_DESC_LINEAR_VOLT(750000, 1100000, 50000),
        },
    },
    {
        .flag = REGULATOR_FLG_INTREG,
        .desc.intreg_desc = {
            .flag = DESC_FLAG_LINEAR(PWR_FLG_VOLT_SSPD | PWR_FLG_VOLT_ST | PWR_FLG_PWR_EN),
            .info = {
                .pwrId = PWR_ID_VCC_MIPI,
            },
            PWR_INTREG_SHIFT_RUN(&PMU->LDO_CON[1], PMU_LDO_CON1_MCU_LDOMIPI_SFT_SHIFT),
            PWR_INTREG_SHIFT_SSPD(&PMU->LDO_CON[1], PMU_LDO_CON1_PWRMODE_LDOMIPI_ADJ_SHIFT),
            PWR_INTREG_SHIFT_ST(&PMU->LDO_STAT, PMU_LDO_STAT_LDO_MIPI_ADJ_SHIFT),
            .voltMask = PMU_LDO_CON1_MCU_LDOMIPI_SFT_MASK >> PMU_LDO_CON1_MCU_LDOMIPI_SFT_SHIFT,
                    PWR_DESC_LINEAR_VOLT(750000, 1100000, 50000),
        },
    },
    {
        .flag = REGULATOR_FLG_INTREG,
        .desc.intreg_desc = {
            .flag = DESC_FLAG_LINEAR(PWR_FLG_PWR_EN),
            .info = {
                .pwrId = PWR_ID_VCC_AUDIO,
            },
            PWR_INTREG_SHIFT_RUN(&PMU->LDO_CON[0], PMU_LDO_CON0_LDO_AUDIO_SFT_SHIFT),
            PWR_INTREG_SHIFT_EN(&PMU->LDO_CON[0], PMU_LDO_CON0_LDO_AUDIO_EN_SHIFT),
            .voltMask = PMU_LDO_CON0_LDO_AUDIO_SFT_MASK >> PMU_LDO_CON0_LDO_AUDIO_SFT_SHIFT,
                    PWR_DESC_LINEAR_VOLT(1500000, 1650000, 50000),
        },
    },
    {
        .flag = REGULATOR_FLG_INTREG,
        .desc.intreg_desc = {
            .flag = DESC_FLAG_LINEAR(PWR_FLG_VOLT_SSPD),
            .info = {
                .pwrId = PWR_ID_DSP_CORE,
            },
            PWR_INTREG_SHIFT_RUN(&PMU->LDO_CON[2], PMU_LDO_CON2_DSP_LDOCORE_SFT_SHIFT),
            PWR_INTREG_SHIFT_SSPD(&PMU->LDO_CON[2], PMU_LDO_CON2_DSPAPM_LDOCORE_ADJ_SHIFT),
            .voltMask = PMU_LDO_CON2_DSP_LDOCORE_SFT_MASK >> PMU_LDO_CON2_DSP_LDOCORE_SFT_SHIFT,
                    PWR_DESC_LINEAR_VOLT(750000, 1100000, 50000),
        },
    },
    {
        .flag = REGULATOR_FLG_INTREG,
        .desc.intreg_desc = {
            .flag = DESC_FLAG_LINEAR(PWR_FLG_VOLT_SSPD),
            .info = {
                .pwrId = PWR_ID_DSP_VCC_MIPI,
            },
            PWR_INTREG_SHIFT_RUN(&PMU->LDO_CON[2], PMU_LDO_CON2_DSP_LDOMIPI_SFT_SHIFT),
            PWR_INTREG_SHIFT_SSPD(&PMU->LDO_CON[2], PMU_LDO_CON2_DSPAPM_LDOMIPI_ADJ_SHIFT),
            .voltMask = PMU_LDO_CON2_DSP_LDOMIPI_SFT_MASK >> PMU_LDO_CON2_DSP_LDOMIPI_SFT_SHIFT,
                    PWR_DESC_LINEAR_VOLT(750000, 1100000, 50000),
        },
    },
    { /* sentinel */ },
};

RT_WEAK const struct regulator_init regulator_inits[] =
{
    REGULATOR_INIT("vdd_dsp_core", PWR_ID_DSP_CORE, 800000, 1, 800000, 1),
    REGULATOR_INIT("vcc_audio", PWR_ID_VCC_AUDIO, 0, 0, 0, 0),
    { /* sentinel */ },
};

#endif

#endif

#ifdef RT_USING_CODEC_ES8388
RT_WEAK const struct codec_desc codec_es8388 =
{
    .if_type = IF_TYPE_I2C,
    .name = "es8388",
    .i2c_bus = "i2c0",
    .i2c_addr = 0x11,
};
#endif

#ifdef RT_USING_MULTI_DAIS
RT_WEAK const struct audio_mdais_desc rk_mdais =
{
    .dais = { PDM0, I2STDM1 },
    .capture_mapping = { 2, 2 },
    .playback_mapping = { 0, 2 },
};
#endif

#ifdef RT_USING_AUDIO

RT_WEAK void rt_audio_init(void)
{
    /* codec adc enable(analog mic) */
#ifdef RT_USING_ACDCDIG_I2STDM0
    WRITE_REG_MASK_WE(GRF->SOC_CON2, GRF_SOC_CON2_GRF_CON_CODEC_SEL_MASK,
                      1 << GRF_SOC_CON2_GRF_CON_CODEC_SEL_SHIFT);
#elif  defined(RT_USING_ACDCDIG_I2STDM1)
    WRITE_REG_MASK_WE(GRF->SOC_CON2, GRF_SOC_CON2_GRF_CON_CODEC_SEL_MASK,
                      0 << GRF_SOC_CON2_GRF_CON_CODEC_SEL_SHIFT);
    WRITE_REG_MASK_WE(GRF->SOC_CON4, GRF_SOC_CON4_GRF_CON_I2S1_SDI2_FROM_IO_MASK,
                      0x0 << GRF_SOC_CON4_GRF_CON_I2S1_SDI2_FROM_IO_SHIFT);
#elif  defined(RT_USING_ACDCDIG_PDM0)
    WRITE_REG_MASK_WE(GRF->SOC_CON2, GRF_SOC_CON2_GRF_CON_CODEC_SEL_MASK,
                      1 << GRF_SOC_CON2_GRF_CON_CODEC_SEL_SHIFT);
    WRITE_REG_MASK_WE(GRF->SOC_CON4, GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_PDM_MASK,
                      0 << GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_PDM_SHIFT);
#elif  defined(RT_USING_ACDCDIG_PDM0_SDI2)
    WRITE_REG_MASK_WE(GRF->SOC_CON2, GRF_SOC_CON2_GRF_CON_CODEC_SEL_MASK,
                      0 << GRF_SOC_CON2_GRF_CON_CODEC_SEL_SHIFT);
    WRITE_REG_MASK_WE(GRF->SOC_CON4, GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_PDM_MASK,
                      1 << GRF_SOC_CON4_GRF_CON_AUDIO_PRO_APP_PDM_SHIFT);
#endif
}

RT_WEAK const struct audio_card_desc rk_board_audio_cards[] =
{
#ifdef RT_USING_AUDIO_CARD_ACDCDIG
    {
        .name = "adc",
#ifdef RT_USING_ACDCDIG_I2STDM0
        .dai = I2STDM0,
#elif  defined(RT_USING_ACDCDIG_I2STDM1)
        .dai = I2STDM1,
        .rxMap = 0x3012, /* re-mapping sdi2 to path0 */
#elif  defined(RT_USING_ACDCDIG_PDM0) || defined(RT_USING_ACDCDIG_PDM0_SDI2)
        .dai = PDM0,
#endif
#ifdef RT_USING_VAD
        .vad = VAD,
#endif
        .codec = ACDCDIG,
        .capture = true,
        .mclkfs = 2048,
#if defined(RT_USING_ACDCDIG_PDM0) || defined(RT_USING_ACDCDIG_PDM0_SDI2)
        .format = AUDIO_FMT_PDM,
        .pdm_mode = PDM_HIGH_MODE,
#else
        .format = AUDIO_FMT_I2S,
#endif
    },
#endif
#ifdef RT_USING_AUDIO_CARD_PDM_MIC
    {
        .name = "pdm",
        .dai = PDM0,
#ifdef RT_USING_VAD
        .vad = VAD,
#endif
        .codec = NULL,
        .capture = true,
        .mclkfs = 256,
        .format = AUDIO_FMT_PDM,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_I2S_MIC
    {
        .name = "i2s",
#ifdef RT_USING_I2STDM0_MIC
        .dai = I2STDM0,
#elif  defined(RT_USING_I2STDM1_MIC)
        .dai = I2STDM1,
#endif
        .codec = NULL,
        .capture = true,
        .mclkfs = 256,
        .format = AUDIO_FMT_I2S,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_AUDIOPWM
    {
        .name = "audpwm",
        .dai = AUDIOPWM,
        .codec = NULL,
        .playback = true,
        .mclkfs = 4096,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_PCM_BT
    {
        .name = "pcm",
        .dai = I2STDM0,
        .codec = NULL,
        .capture = true,
        .playback = true,
        .mclkfs = 256,
        .format = AUDIO_FMT_I2S,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_ES8388
    {
        .name = "es8388",
        .dai = I2STDM1,
        .codec = (void *) &codec_es8388,
        .capture = true,
        .playback = true,
        .mclkfs = 256,
        .format = AUDIO_FMT_I2S,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_PDM_ES8388_ECHO
    {
        .name = "echo",
        .dai = (void *) &rk_mdais,
        .codec = (void *) &codec_es8388,
        .capture = true,
        .playback = true,
        .multi_dais = true,
        .trcm_mode = TRCM_TXONLY,
    },
#endif

    { /* sentinel */ }
};
#endif

#ifdef RT_USING_USB_DEVICE
RT_WEAK struct ep_id g_usb_ep_pool[] =
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
RT_WEAK void print_clk_summary_info(void)
{
}
#endif

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

SECTION(".sram.text") void BSP_MPU_Init(void)
{
    static const ARM_MPU_Region_t table[] =
    {
        {
            .RBAR = ARM_MPU_RBAR(0U, 0x04000000U),
            .RASR = ARM_MPU_RASR(0U, ARM_MPU_AP_FULL, 0U, 0U, 1U, 0U, 0U, ARM_MPU_REGION_SIZE_1MB)
        },
        {
            .RBAR = ARM_MPU_RBAR(1U, 0x18000000U),
            .RASR = ARM_MPU_RASR(0U, ARM_MPU_AP_FULL, 0U, 0U, 1U, 0U, 0U, ARM_MPU_REGION_SIZE_32MB)
        },
        {
            .RBAR = ARM_MPU_RBAR(2U, 0x20000000U),
            .RASR = ARM_MPU_RASR(DATA_EXEC_FLAG, ARM_MPU_AP_FULL, 0U, 0U, 1U, 1U, 0U, ARM_MPU_REGION_SIZE_1MB)
        },
        {
            .RBAR = ARM_MPU_RBAR(3U, 0x40000000U),
            .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 0U, 0U, 0U, ARM_MPU_REGION_SIZE_256MB)
        },
        {
            .RBAR = ARM_MPU_RBAR(4U, 0x60000000U),
            .RASR = ARM_MPU_RASR(1U, ARM_MPU_AP_FULL, 0U, 0U, 1U, 1U, 0U, ARM_MPU_REGION_SIZE_256MB)
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

/**
 * @brief system power enable hold
 */
RT_WEAK void system_power_hold(void)
{
    HAL_PINCTRL_SetIOMUX(GPIO_BANK1, GPIO_PIN_B7, PIN_CONFIG_MUX_FUNC0);
    HAL_GPIO_SetPinDirection(GPIO1, GPIO_PIN_B7, GPIO_OUT);
    HAL_GPIO_SetPinLevel(GPIO1, GPIO_PIN_B7, GPIO_HIGH);
}

#ifdef RT_USING_PM_REQ_PWR
#define CORE_PWR_REQ_CNT 3
RT_WEAK uint32_t core_pwr_req[CORE_PWR_REQ_CNT];
RT_WEAK struct req_pwr_desc req_pwr_array[] =
{
    {
        .pwr_id = PWR_ID_CORE,
        .req_ctrl = {
            .info.ttl_req = CORE_PWR_REQ_CNT, /* for core & shrm */
            .req_vals = &core_pwr_req[0],
        }
    },
    { /* sentinel */ },
};
#endif

#ifdef RT_USING_PM_DVFS
RT_WEAK const struct dvfs_table dvfs_core_table[] =
{
    {
        .freq = 297000000,
        .volt = 800000,
    },
    {
        .freq = 396000000,
        .volt = 900000,
    },
    { /* sentinel */ },
};

RT_WEAK const struct dvfs_table dvfs_shrm_table[] =
{
    {
        .freq = 297000000,
        .volt = 800000,
    },
    {
        .freq = 396000000,
        .volt = 850000,
    },
    { /* sentinel */ },
};

RT_WEAK const struct dvfs_table dvfs_dsp_table[] =
{
    {
        .freq = 49500000,
        .volt = 800000,
    },
    {
        .freq = 99000000,
        .volt = 800000,
    },
    {
        .freq = 198000000,
        .volt = 800000,
    },
    {
        .freq = 297000000,
        .volt = 800000,
    },
    {
        .freq = 396000000,
        .volt = 800000,
    },
    { /* sentinel */ },
};

RT_WEAK struct rk_dvfs_desc dvfs_data[] =
{
    {
        .clk_id = SCLK_SHRM,
        .pwr_id = PWR_ID_CORE,
        .tbl_idx = 0,
        .table = &dvfs_shrm_table[0],
    },
    {
        .clk_id = HCLK_M4,
        .pwr_id = PWR_ID_CORE,
        .tbl_idx = 0,
        .table = &dvfs_core_table[0],
    },
    {
        .clk_id = ACLK_DSP,
        .pwr_id = PWR_ID_CORE,
        .tbl_idx = 0,
        .table = &dvfs_dsp_table[0],
    },
    { /* sentinel */ },
};

RT_WEAK struct pm_mode_dvfs pm_mode_data[] =
{
    {
        .clk_id = HCLK_M4,
        .run_tbl_idx = { 1, 0, 0 },
        .sleep_tbl_idx = 0,
    },
    {
        .clk_id = SCLK_SHRM,
        .run_tbl_idx = { 1, 0, 0 },
        .sleep_tbl_idx = 0,
    },
    { /* sentinel */ },
};
#endif

#ifdef RT_USING_COREDUMP
/* cd_area should be in sram */
coredump_area_t cd_area[] =
{
    {
    },  /* Please don't remove this*/
    {
        .magic_id = COREDUMP_MAGIC_ID,
        .start_addr = 0x20000000,
        .end_addr = (uint32_t)RK_SRAM_END,
        .cache = 1,
        .prepare = NULL,
    },
    {
        .magic_id = COREDUMP_MAGIC_ID,
        .start_addr = (uint32_t)_stext,
        .end_addr = (uint32_t)_etext,
        .cache = 1,
        .prepare = NULL,
    },
    { /* sentinel */ },
};
#endif

/**
 * This function will initial RK2108 EVB board.
 */
RT_WEAK void rt_hw_board_init()
{
    /* HAL_Init */
    HAL_Init();

    /* hal bsp init */
    BSP_Init();

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

    system_power_hold();

#ifdef RT_USING_PIN
    rt_hw_iomux_config();
#endif

#ifdef RT_USING_CRU
    clk_init(clk_inits, false);

    /* disable some clks when init, and enabled by device when needed */
    clk_disable_unused(clks_unused);
    if (RT_CONSOLE_DEVICE_UART(0))
        CRU->CRU_CLKGATE_CON[2] = 0x08860886;
    else if (RT_CONSOLE_DEVICE_UART(1))
        CRU->CRU_CLKGATE_CON[2] = 0x080d080d;
    else if (RT_CONSOLE_DEVICE_UART(2))
        CRU->CRU_CLKGATE_CON[2] = 0x008b008b;
    else
        CRU->CRU_CLKGATE_CON[2] = 0x088f088f;

#ifdef RT_USING_SDIO
    CRU->CRU_SDIO_CON[0] = 0x1 | 0x1 << 16;
    CRU->CRU_SDIO_CON[0] = (0x1 << 1) | (0x3 << 17);
    CRU->CRU_SDIO_CON[1] = (0x1 << 1) | (0x3 << 17);
    CRU->CRU_SDIO_CON[0] = 0x1 << 16;
#endif
#endif

#ifdef RT_USING_PMU
    HAL_PD_Off(PD_AUDIO);

    /* If DSP is unused, power down it here */
#ifndef RT_USING_DSP
    HAL_PD_Off(PD_DSP);
#endif
#endif

#ifdef RT_USING_PM
    rk_rt_pm_init();
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

#ifdef HAL_PWR_MODULE_ENABLED
    regulator_desc_init(regulators);
#endif

#ifdef RT_USING_PM_REQ_PWR
    regulator_req_desc_init(req_pwr_array);
#endif

#ifdef RT_USING_PM_DVFS
    dvfs_desc_init(dvfs_data);
    rkpm_register_dvfs_info(pm_mode_data, 0);
#endif

#ifdef RT_USING_AUDIO
    rt_audio_init();
#endif

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}
