/**
  * Copyright (c) 2020 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"

#ifdef RT_USING_PM
#include "drv_pm.h"
#endif

#ifdef RT_USING_CRU
#include "drv_clock.h"
#endif

#ifdef RT_USING_CODEC
#include "drv_codecs.h"
#endif

#ifdef RT_USING_AUDIO
#include "rk_audio.h"
#endif

#ifdef RT_USING_UART
#include "drv_uart.h"
#endif

#ifdef RT_USING_CRU
const struct clk_init clk_inits[] =
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
    INIT_CLK("PLL_CPLL", PLL_CPLL, 384 * MHZ),
    INIT_CLK("SCLK_SFC_SRC", SCLK_SFC_SRC, 50 * MHZ),
    INIT_CLK("HCLK_M4", HCLK_M4, 100 * MHZ),
    INIT_CLK("ACLK_DSP", ACLK_DSP, 100 * MHZ),
    INIT_CLK("ACLK_LOGIC", ACLK_LOGIC, 300 * MHZ),
    INIT_CLK("HCLK_LOGIC", HCLK_LOGIC, 150 * MHZ),
    INIT_CLK("PCLK_LOGIC", PCLK_LOGIC, 150 * MHZ),
    INIT_CLK("SCLK_SHRM", SCLK_SHRM, 100 * MHZ),
    INIT_CLK("PCLK_SHRM", PCLK_SHRM, 100 * MHZ),
    INIT_CLK("PCLK_ALIVE", PCLK_ALIVE, 100 * MHZ),
    INIT_CLK("HCLK_ALIVE", HCLK_ALIVE, 100 * MHZ),
    { /* sentinel */ },
};

#endif

#ifdef RT_USING_PM_DVFS
const struct dvfs_table dvfs_core_table[] =
{
    {
        .freq = 99000000,
        .volt = 800000,
    },
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

const struct dvfs_table dvfs_shrm_table[] =
{
    {
        .freq = 99000000,
        .volt = 800000,
    },
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
#endif

#ifdef RT_USING_CODEC_AW8896
const struct codec_desc codec_aw8896 =
{
    .if_type = IF_TYPE_I2C,
    .name = "aw8896",
    .i2c_bus = "i2c1",
    .i2c_addr = 0x34,
};
#endif

#ifdef RT_USING_AUDIO
const struct audio_card_desc rk_board_audio_cards[] =
{
#ifdef RT_USING_AUDIO_CARD_AW8896
    {
        .name = "aw8896",
        .dai = I2STDM1,
        .codec = (void *) &codec_aw8896,
        .capture = true,
        .playback = true,
        .mclkfs = 256,
        .format = AUDIO_FMT_I2S,
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
