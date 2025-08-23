/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-10     Cliff      first implementation
 *
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"

#ifdef RT_USING_CODEC
#include "drv_codecs.h"
#endif

#ifdef RT_USING_AUDIO
#include "rk_audio.h"
#endif


#ifdef RT_USING_CODEC_ES8311
const struct codec_desc codec_es8311 =
{
    .if_type = IF_TYPE_I2C,
    .name = "es8311",
    .i2c_bus = "i2c0",
    .i2c_addr = 0x19,
};
#endif

#ifdef RT_USING_AUDIO
extern const struct audio_mdais_desc rk_mdais;
const struct audio_card_desc rk_board_audio_cards[] =
{
#ifdef RT_USING_AUDIO_CARD_ADC_ES8311_ECHO
    {
        .name = "echo",
        .dai = (void *) &rk_mdais,
        .codec = ACDCDIG,
        .capture = true,
        .playback = false,
        .multi_dais = true,
        .format = AUDIO_FMT_PDM,
        .trcm_mode = TRCM_TXONLY,
    },
#endif
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
#ifdef RT_USING_AUDIO_CARD_ES8311
    {
        .name = "es8311",
        .dai = I2STDM1,
        .codec = (void *) &codec_es8311,
        .capture = true,
        .playback = true,
        .mclkfs = 256,
        .format = AUDIO_FMT_I2S,
    },
#endif
    { /* sentinel */ }
};
#endif
