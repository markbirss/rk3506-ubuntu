/**
  * Copyright (c) 2023 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_mcodecs.h
  * @author  Jair Wu
  * @version V0.1
  * @date    14-April-2023
  * @brief   mcodecs driver for rk soc
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#if defined(RT_USING_MULTI_CODECS)

#include "rk_audio.h"

struct rk_codec
{
    struct audio_codec *codec;
    unsigned int fmt;
    unsigned int fmt_msk;
};

struct rk_mcodecs_dev
{
    struct audio_codec codec;
    struct rk_codec *codecs;
    uint8_t num_codecs;
    const struct audio_mcodecs_desc *desc;
};

static inline struct rk_mcodecs_dev *to_mcodecs(struct audio_codec *codec)
{
    return HAL_CONTAINER_OF(codec, struct rk_mcodecs_dev, codec);
}

#endif
