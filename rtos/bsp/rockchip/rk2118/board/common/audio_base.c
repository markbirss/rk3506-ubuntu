/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-05-01     Sugar Zhang  first implementation
 */

#include "board.h"
#include "hal_base.h"
#include "hal_bsp.h"

#ifdef RT_USING_AUDIO
#include "rk_audio.h"

RT_WEAK const struct audio_card_desc rk_board_audio_cards[] =
{
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
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_SAI5
    {
        .name = "sai5",
        .dai =  SAI5,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_SAI6
    {
        .name = "sai6",
        .dai =  SAI6,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_I2S,
        .mclkfs = 256,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_SAI7
    {
        .name = "sai7",
        .dai =  SAI7,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
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
#ifdef RT_USING_AUDIO_CARD_SPDIFRX1
    {
        .name = "dir1",
        .dai =  SPDIFRX1,
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

    { /* sentinel */ }
};

#endif
