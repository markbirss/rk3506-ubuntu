/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-08-14     Jason Zhu  first implementation
 */

#include "board.h"
#include "hal_base.h"
#include "hal_bsp.h"

#ifdef RT_USING_CODEC
#include "drv_codecs.h"

#ifdef RT_USING_CODEC_ES8388
const struct codec_desc codec_es8388 =
{
    .if_type = IF_TYPE_I2C,
    .name = "es8388",
    .i2c_bus = "i2c0",
    .i2c_addr = 0x11,
};
#endif
#endif

#ifdef RT_USING_AUDIO
#include "rk_audio.h"

RT_WEAK const struct audio_card_desc rk_board_audio_cards[] =
{
#ifdef RT_USING_AUDIO_CARD_ACDCDIG_DSM
    {
        .name = "dsm",
        .dai = SAI3,
        .codec = ACDCDIG_DSM,
        .playback = HAL_TRUE,
        .mclkfs = 256,
        .codec_master = true,
        .format = AUDIO_FMT_I2S,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_ACODEC
    {
        .name = "acodec",
        .dai = SAI4,
        .codec = ACODEC,
        .capture = true,
        .playback = HAL_FALSE,
        .codec_master = true,
        .format = AUDIO_FMT_I2S,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_SAI0
    {
        .name = "sai0",
        .dai =  SAI0,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_SAI1
    {
        .name = "sai1",
        .dai =  SAI1,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_SAI2
    {
        .name = "sai2",
        .dai =  SAI2,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_SAI3
    {
        .name = "sai3",
        .dai =  SAI3,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_SAI4
    {
        .name = "sai4",
        .dai =  SAI4,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_SPDIFRX0
    {
        .name = "dir0",
        .dai =  SPDIFRX0,
        .codec = NULL,
        .capture = HAL_TRUE,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_SPDIFTX0
    {
        .name = "dit0",
        .dai =  SPDIFTX0,
        .codec = NULL,
        .playback = HAL_TRUE,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_PDM0
    {
        .name = "pdm0",
        .dai =  PDM,
        .codec = NULL,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_PDM,
        .mclkfs = 256,
    },
#endif

#ifdef RT_USING_AUDIO_CARD_ES8388
    {
        .name = "es8388",
        .dai =  SAI1,
        .codec = (void *) &codec_es8388,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
    { /* sentinel */ }
};

#endif
