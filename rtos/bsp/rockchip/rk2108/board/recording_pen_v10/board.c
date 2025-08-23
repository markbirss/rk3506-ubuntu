/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-02     Chris.Zhong      first implementation
 *
 */

#include <rthw.h>
#include <rtthread.h>

#include "hal_base.h"
#include "board.h"

#ifdef RT_USING_AUDIO
#include "rk_audio.h"
#endif
#ifdef RT_USING_CODEC
#include "drv_codecs.h"
#endif
#ifdef RT_USING_PMIC
#include "drv_pmic.h"
#endif
#ifdef HAL_PWR_MODULE_ENABLED
#include "drv_regulator.h"
#endif
#ifdef RT_USING_SDIO
#include "drv_sdio.h"
#include <drivers/mmcsd_core.h>
#endif

struct regulator_desc regulators[] =
{
#ifdef RT_USING_PMIC_RK816
    /* BUCK1 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_BUCK1_2(RK816_ID_DCDC1, PWR_ID_VCC1),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* BUCK2 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_BUCK1_2(RK816_ID_DCDC2, PWR_ID_VCC_1V8_BUCK),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* BUCK4 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_BUCK4(RK816_ID_DCDC4, PWR_ID_VCCIO_3V3),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO1 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO1_4(RK816_ID_LDO1, PWR_ID_VCC_3V0_BL),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO2 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO1_4(RK816_ID_LDO2, PWR_ID_VCC_1V8_LDO),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO3 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO1_4(RK816_ID_LDO3, PWR_ID_VLDO3),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO4 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO1_4(RK816_ID_LDO4, PWR_ID_VCC33_CODEC),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO5 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO5_6(RK816_ID_LDO5, PWR_ID_VCC33_AUDIO),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
    /* LDO6 */
    {
        .flag = REGULATOR_FLG_I2C8 | REGULATOR_FLG_LOCK,
        .desc.i2c_desc = RK816_LDO5_6(RK816_ID_LDO6, PWR_ID_VLDO6),
        .desc.i2c_desc.i2c = &pmic_pwr_i2c_desc,
    },
#endif
    /* RK2108 Internal LDO1 */
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
    /* RK2108 Internal LDO2 */
    {
        .flag = REGULATOR_FLG_INTREG,
        .desc.intreg_desc = {
            .flag = DESC_FLAG_LINEAR(PWR_FLG_VOLT_SSPD | PWR_FLG_VOLT_ST | PWR_FLG_PWR_EN),
            .info = {
                .pwrId = PWR_ID_VCC_MIPI,
            },
            PWR_INTREG_SHIFT_RUN(&PMU->LDO_CON[1], PMU_LDO_CON1_MCU_LDOMIPI_SFT_SHIFT),
            PWR_INTREG_SHIFT_EN(&PMU->LDO_CON[0], PMU_LDO_CON0_LDO_MIPI_EN_SHIFT),
            PWR_INTREG_SHIFT_SSPD(&PMU->LDO_CON[1], PMU_LDO_CON1_PWRMODE_LDOMIPI_ADJ_SHIFT),
            PWR_INTREG_SHIFT_ST(&PMU->LDO_STAT, PMU_LDO_STAT_LDO_MIPI_ADJ_SHIFT),
            .voltMask = PMU_LDO_CON1_MCU_LDOMIPI_SFT_MASK >> PMU_LDO_CON1_MCU_LDOMIPI_SFT_SHIFT,
                    PWR_DESC_LINEAR_VOLT(750000, 1100000, 50000),
        },
    },
    /* RK2108 Internal LDO3 */
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
    { /* sentinel */ },
};

const struct regulator_init regulator_inits[] =
{
#ifdef RT_USING_PMIC_RK816
    /*
     * Since the feedback of dcdc2 is divided into 60%, so in order to
     * get 1.8v, here it must be set to 1.1v.
     */
    REGULATOR_INIT("dcdc1", PWR_ID_VCC1,         1100000, 0, 1100000, 1),
    REGULATOR_INIT("dcdc2", PWR_ID_VCC_1V8_BUCK, 1100000, 1, 1100000, 1),
    REGULATOR_INIT("dcdc4", PWR_ID_VCCIO_3V3,    3300000, 1, 3300000, 1),
    REGULATOR_INIT("ldo1",  PWR_ID_VCC_3V0_BL,   3300000, 0, 3300000, 0),
    REGULATOR_INIT("ldo2",  PWR_ID_VCC_1V8_LDO,  1800000, 0, 1800000, 0),
    REGULATOR_INIT("ldo3",  PWR_ID_VLDO3,        1800000, 0, 1800000, 0),
    REGULATOR_INIT("ldo4",  PWR_ID_VCC33_CODEC,  3300000, 0, 3300000, 0),
    REGULATOR_INIT("ldo5",  PWR_ID_VCC33_AUDIO,  3300000, 0, 3300000, 0),
    REGULATOR_INIT("ldo6",  PWR_ID_VLDO6,        1800000, 0, 1800000, 0),
#endif
    REGULATOR_INIT("vcc_mipi", PWR_ID_VCC_MIPI,        0, 0,       0, 0),
    REGULATOR_INIT("vcc_audio", PWR_ID_VCC_AUDIO,      0, 0,       0, 0),
    { /* sentinel */ },
};

#ifdef RT_USING_SDIO
struct rk_mmc_platform_data rk_mmc_table[] =
{
#ifdef RT_USING_SDIO0
    {
        .flags = MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ | MMCSD_SUP_HIGHSPEED,
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

#ifdef RT_USING_CODEC_ES8156
const struct codec_desc codec_es8156 =
{
    .if_type = IF_TYPE_I2C,
    .name = "es8156",
    .i2c_bus = "i2c0",
    .i2c_addr = 0x08,
};
#endif

#ifdef RT_USING_AUDIO
const struct audio_card_desc rk_board_audio_cards[] =
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
#ifdef RT_USING_AUDIO_CARD_ES8156
    {
        .name = "es8156",
        .dai = I2STDM1,
        .codec = (void *) &codec_es8156,
        .playback = true,
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
    { /* sentinel */ }
};
#endif

#ifdef RT_USING_SENSOR_RK816
/**
 *  ntc table shoud according to the battery Specification to set.
 **/
#include "pmic/rk816.h"
static const rt_uint32_t rk816_ntc_table[] =   /* THIS TABLE'S DATA IS ALL FAKE!!!*/
{
    25062, 12250, 100250, 86125, 76125, 65125, 50125, /* -10 ~ 20'C */
    10000, 8310,  6490,  5830,  4910,  4160,  3540,  /* 25  ~ 55'C */
};

struct rk816_sensor_platform_data rk816_sensor_pdata =
{
    .ntc_table = &rk816_ntc_table[0],
    .ntc_num = 14,            // 等价于：ARRAY_SIZE(rk816_ntc_table)
    .ntc_degree_min = -10,    // rk816_ntc_table[0]对应的温度，即最低温度。
    .ntc_degree_step = 5,     // 温度步进，即上述各ntc值之间的步进
};
#endif