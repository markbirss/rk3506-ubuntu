/**
  * Copyright (c) 2023 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_mcodecs.c
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
#include "drv_clock.h"
#include "drv_mcodecs.h"
#include "hal_base.h"
#include "hal_bsp.h"

static inline const uint8_t *get_maps(eAUDIO_streamType stream,
                                      struct rk_mcodecs_dev *mcodecs)
{
    if (stream == AUDIO_STREAM_CAPTURE)
        return mcodecs->desc->capture_mapping;
    else
        return mcodecs->desc->playback_mapping;
}

static rt_err_t rk_mcodecs_init(struct audio_codec *codec,
                                struct AUDIO_INIT_CONFIG *config)
{
    struct rk_mcodecs_dev *mcodecs = to_mcodecs(codec);
    struct audio_codec *child;
    rt_err_t ret = RT_EOK;
    int i;

    for (i = 0; i < mcodecs->num_codecs; i++)
    {
        child = mcodecs->codecs[i].codec;
        if (child->ops && child->ops->init)
        {
            ret = child->ops->init(child, config);
            if (ret < 0)
                return ret;
        }
    }

    return ret;
}

static rt_err_t rk_mcodecs_deinit(struct audio_codec *codec)
{
    struct rk_mcodecs_dev *mcodecs = to_mcodecs(codec);
    struct audio_codec *child;
    rt_err_t ret = RT_EOK;
    int i;

    for (i = 0; i < mcodecs->num_codecs; i++)
    {
        child = mcodecs->codecs[i].codec;
        if (child->ops && child->ops->deinit)
        {
            ret = child->ops->deinit(child);
            if (ret < 0)
                return ret;
        }
    }

    return ret;
}

static rt_err_t rk_mcodecs_set_clk(struct audio_codec *codec,
                                   eAUDIO_streamType stream,
                                   uint32_t freq)
{
    struct rk_mcodecs_dev *mcodecs = to_mcodecs(codec);
    const uint8_t *maps = get_maps(stream, mcodecs);
    struct audio_codec *child;
    rt_err_t ret = RT_EOK;
    int i;

    for (i = 0; i < mcodecs->num_codecs; i++)
    {
        if (!maps[i])
            continue;
        child = mcodecs->codecs[i].codec;
        if (child->ops && child->ops->set_clk)
        {
            ret = child->ops->set_clk(child, stream, freq);
            if (ret < 0)
                return ret;
        }
    }

    return ret;
}

static rt_err_t rk_mcodecs_config(struct audio_codec *codec,
                                  eAUDIO_streamType stream,
                                  struct AUDIO_PARAMS *params)
{
    struct rk_mcodecs_dev *mcodecs = to_mcodecs(codec);
    const uint8_t *maps = get_maps(stream, mcodecs);
    struct audio_codec *child;
    struct AUDIO_PARAMS cparams;
    rt_err_t ret = RT_EOK;
    int i;

    rt_memcpy(&cparams, params, sizeof(*params));

    for (i = 0; i < mcodecs->num_codecs; i++)
    {
        if (!maps[i])
            continue;
        child = mcodecs->codecs[i].codec;
        cparams.channels = maps[i];
        if (child->ops && child->ops->config)
        {
            ret = child->ops->config(child, stream, &cparams);
            if (ret < 0)
                return ret;
        }
    }

    return ret;
}

static rt_err_t rk_mcodecs_start(struct audio_codec *codec, eAUDIO_streamType stream)
{
    struct rk_mcodecs_dev *mcodecs = to_mcodecs(codec);
    const uint8_t *maps = get_maps(stream, mcodecs);
    struct audio_codec *child;
    rt_err_t ret = RT_EOK;
    int i;

    for (i = 0; i < mcodecs->num_codecs; i++)
    {
        if (!maps[i])
            continue;
        child = mcodecs->codecs[i].codec;
        if (child->ops && child->ops->start)
        {
            ret = child->ops->start(child, stream);
            if (ret < 0)
                return ret;
        }
    }

    return ret;
}

static rt_err_t rk_mcodecs_stop(struct audio_codec *codec, eAUDIO_streamType stream)
{
    struct rk_mcodecs_dev *mcodecs = to_mcodecs(codec);
    const uint8_t *maps = get_maps(stream, mcodecs);
    struct audio_codec *child;
    rt_err_t ret = RT_EOK;
    int i;

    for (i = 0; i < mcodecs->num_codecs; i++)
    {
        if (!maps[i])
            continue;
        child = mcodecs->codecs[i].codec;
        if (child->ops && child->ops->stop)
        {
            ret = child->ops->stop(child, stream);
            if (ret < 0)
                return ret;
        }
    }

    return ret;
}

static const struct audio_codec_ops rk_mcodecs_ops =
{
    .init = rk_mcodecs_init,
    .deinit = rk_mcodecs_deinit,
    .set_clk = rk_mcodecs_set_clk,
    .config = rk_mcodecs_config,
    .start = rk_mcodecs_start,
    .stop = rk_mcodecs_stop,
};

static struct audio_codec *rk_mcodecs_init_codec(const struct audio_mcodecs_desc *amd)
{
    struct rk_mcodecs_dev *mcodecs;
    struct rk_codec *codecs;
    uint8_t count = 0, i = 0;

    mcodecs = rt_calloc(1, sizeof(*mcodecs));
    RT_ASSERT(mcodecs);

    /* child codec counts */
    while (amd->codecs[count] != RT_NULL)
        count++;

    codecs = rt_calloc(count, sizeof(*codecs));
    if (!codecs)
        goto err;

    for (i = 0; i < count; i++)
    {
        codecs[i].codec = rk_audio_find_codec((uint32_t)amd->codecs[i]);
        if (!codecs[i].codec)
        {
            rt_kprintf("%s: codec: %p missing\n", __func__, amd->codecs[i]);
            goto err;
        }
    }

    mcodecs->num_codecs = count;
    mcodecs->codecs = codecs;
    mcodecs->desc = amd;
    mcodecs->codec.ops = &rk_mcodecs_ops;
    mcodecs->codec.id = (uint32_t)amd;

    return &mcodecs->codec;
err:
    if (codecs)
        rt_free(codecs);
    if (mcodecs)
        rt_free(mcodecs);

    return RT_NULL;
}

extern const struct audio_mcodecs_desc rk_mcodecs;

int rt_hw_mcodecs_init(void)
{
    rt_err_t ret;

    ret = rk_audio_register_codec(rk_mcodecs_init_codec(&rk_mcodecs));
    if (ret)
        rt_kprintf("%s: mcodecs register failed!\n", __func__);

    return ret;
}

INIT_COMPONENT_EXPORT(rt_hw_mcodecs_init);
#endif
