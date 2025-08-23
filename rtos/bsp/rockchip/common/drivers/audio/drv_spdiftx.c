/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_spdiftx.c
  * @author  sugar zhang
  * @version V0.1
  * @date    1-May-2024
  * @brief   spdiftx driver for rk soc
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#if defined(RT_USING_SPDIFTX) && \
    (defined(RT_USING_SPDIFTX0) || defined(RT_USING_SPDIFTX1) || defined(RT_USING_SPDIFTX2) || defined(RT_USING_SPDIFTX3))

#include "rk_audio.h"
#include "drv_clock.h"

#include "hal_base.h"
#include "hal_bsp.h"

struct rk_spdiftx_dev
{
    struct rt_device parent;
    struct audio_dai dai;
    struct HAL_SPDIFTX_DEV *hSpdiftx;
    struct clk_gate *mclk_gate;
};

static inline struct rk_spdiftx_dev *to_spdiftx(struct audio_dai *dai)
{
    return HAL_CONTAINER_OF(dai, struct rk_spdiftx_dev, dai);
}

static rt_err_t rk_spdiftx_init(struct audio_dai *dai,
                                struct AUDIO_INIT_CONFIG *config)
{
    struct rk_spdiftx_dev *spdiftx = to_spdiftx(dai);

    clk_enable(spdiftx->mclk_gate);

    HAL_SPDIFTX_Init(spdiftx->hSpdiftx->pReg);

    return RT_EOK;
}

static rt_err_t rk_spdiftx_deinit(struct audio_dai *dai)
{
    struct rk_spdiftx_dev *spdiftx = to_spdiftx(dai);

    HAL_SPDIFTX_DeInit(spdiftx->hSpdiftx->pReg);

    clk_disable(spdiftx->mclk_gate);

    return RT_EOK;
}

static rt_err_t rk_spdiftx_set_clk(struct audio_dai *dai,
                                   eAUDIO_streamType stream,
                                   uint32_t freq)
{
    struct rk_spdiftx_dev *spdiftx = to_spdiftx(dai);

    if (stream == AUDIO_STREAM_CAPTURE)
        return RT_EOK;

    clk_set_rate(spdiftx->hSpdiftx->mclk, freq);

    return RT_EOK;
}

static rt_err_t rk_spdiftx_config(struct audio_dai *dai,
                                  eAUDIO_streamType stream,
                                  struct AUDIO_PARAMS *params)
{
    struct rk_spdiftx_dev *spdiftx = to_spdiftx(dai);

    if (stream == AUDIO_STREAM_CAPTURE)
        return RT_EOK;

    HAL_SPDIFTX_Config(spdiftx->hSpdiftx->pReg, params);

    return RT_EOK;
}

static rt_err_t rk_spdiftx_start(struct audio_dai *dai, eAUDIO_streamType stream)
{
    struct rk_spdiftx_dev *spdiftx = to_spdiftx(dai);

    if (stream == AUDIO_STREAM_CAPTURE)
        return RT_EOK;

    HAL_SPDIFTX_Enable(spdiftx->hSpdiftx->pReg);

    return RT_EOK;
}

static rt_err_t rk_spdiftx_stop(struct audio_dai *dai, eAUDIO_streamType stream)
{
    struct rk_spdiftx_dev *spdiftx = to_spdiftx(dai);

    if (stream == AUDIO_STREAM_CAPTURE)
        return RT_EOK;

    HAL_SPDIFTX_Disable(spdiftx->hSpdiftx->pReg);

    return RT_EOK;
}

static const struct audio_dai_ops rk_spdiftx_ops =
{
    .init = rk_spdiftx_init,
    .deinit = rk_spdiftx_deinit,
    .set_clk = rk_spdiftx_set_clk,
    .config = rk_spdiftx_config,
    .start = rk_spdiftx_start,
    .stop = rk_spdiftx_stop,
};

static struct audio_dai *rk_spdiftx_init_dai(struct HAL_SPDIFTX_DEV *hSpdiftx)
{
    struct rk_spdiftx_dev *spdiftx;

    spdiftx = rt_calloc(1, sizeof(*spdiftx));
    RT_ASSERT(spdiftx);

    spdiftx->dai.ops = &rk_spdiftx_ops;
    spdiftx->dai.id = (uint32_t)hSpdiftx->pReg;
    spdiftx->dai.dmaData[AUDIO_STREAM_PLAYBACK] = &hSpdiftx->txDmaData;
    spdiftx->hSpdiftx = hSpdiftx;
    spdiftx->mclk_gate = get_clk_gate_from_id(hSpdiftx->mclkGate);

    return &spdiftx->dai;
}

int rt_hw_spdiftx_init(void)
{
#if defined(RT_USING_SPDIFTX0)
    rk_audio_register_dai(rk_spdiftx_init_dai(&g_spdiftx0Dev));
#endif
#if defined(RT_USING_SPDIFTX1)
    rk_audio_register_dai(rk_spdiftx_init_dai(&g_spdiftx1Dev));
#endif
#if defined(RT_USING_SPDIFTX2)
    rk_audio_register_dai(rk_spdiftx_init_dai(&g_spdiftx2Dev));
#endif
#if defined(RT_USING_SPDIFTX3)
    rk_audio_register_dai(rk_spdiftx_init_dai(&g_spdiftx3Dev));
#endif

    return 0;
}

INIT_DEVICE_EXPORT(rt_hw_spdiftx_init);
#endif
