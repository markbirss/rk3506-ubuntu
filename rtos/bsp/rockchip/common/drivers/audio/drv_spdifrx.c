/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_spdifrx.c
  * @author  sugar zhang
  * @version V0.1
  * @date    1-May-2024
  * @brief   spdifrx driver for rk soc
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#if defined(RT_USING_SPDIFRX) && \
    (defined(RT_USING_SPDIFRX0) || defined(RT_USING_SPDIFRX1) || defined(RT_USING_SPDIFRX2) || defined(RT_USING_SPDIFRX3))

#include "rk_audio.h"
#include "drv_clock.h"

#include "hal_base.h"
#include "hal_bsp.h"

struct rk_spdifrx_dev
{
    struct rt_device parent;
    struct audio_dai dai;
    struct HAL_SPDIFRX_DEV *hSpdifrx;
    struct clk_gate *mclk_gate;
};

static inline struct rk_spdifrx_dev *to_spdifrx(struct audio_dai *dai)
{
    return HAL_CONTAINER_OF(dai, struct rk_spdifrx_dev, dai);
}

static rt_err_t rk_spdifrx_init(struct audio_dai *dai,
                                struct AUDIO_INIT_CONFIG *config)
{
    struct rk_spdifrx_dev *spdifrx = to_spdifrx(dai);

    clk_enable(spdifrx->mclk_gate);

    HAL_SPDIFRX_DevInit(spdifrx->hSpdifrx);

    return RT_EOK;
}

static rt_err_t rk_spdifrx_deinit(struct audio_dai *dai)
{
    struct rk_spdifrx_dev *spdifrx = to_spdifrx(dai);

    HAL_SPDIFRX_DevDeInit(spdifrx->hSpdifrx);

    clk_disable(spdifrx->mclk_gate);

    return RT_EOK;
}

static rt_err_t rk_spdifrx_set_clk(struct audio_dai *dai,
                                   eAUDIO_streamType stream,
                                   uint32_t freq)
{
    struct rk_spdifrx_dev *spdifrx = to_spdifrx(dai);

    if (stream == AUDIO_STREAM_PLAYBACK)
        return RT_EOK;

    clk_set_rate(spdifrx->hSpdifrx->mclk, freq);

    return RT_EOK;
}

static rt_err_t rk_spdifrx_config(struct audio_dai *dai,
                                  eAUDIO_streamType stream,
                                  struct AUDIO_PARAMS *params)
{
    return RT_EOK;
}

static rt_err_t rk_spdifrx_start(struct audio_dai *dai, eAUDIO_streamType stream)
{
    struct rk_spdifrx_dev *spdifrx = to_spdifrx(dai);

    if (stream == AUDIO_STREAM_PLAYBACK)
        return RT_EOK;

    HAL_SPDIFRX_DevEnable(spdifrx->hSpdifrx);

    return RT_EOK;
}

static rt_err_t rk_spdifrx_stop(struct audio_dai *dai, eAUDIO_streamType stream)
{
    struct rk_spdifrx_dev *spdifrx = to_spdifrx(dai);

    if (stream == AUDIO_STREAM_PLAYBACK)
        return RT_EOK;

    HAL_SPDIFRX_DevDisable(spdifrx->hSpdifrx);

    return RT_EOK;
}

static const struct audio_dai_ops rk_spdifrx_ops =
{
    .init = rk_spdifrx_init,
    .deinit = rk_spdifrx_deinit,
    .set_clk = rk_spdifrx_set_clk,
    .config = rk_spdifrx_config,
    .start = rk_spdifrx_start,
    .stop = rk_spdifrx_stop,
};

static struct audio_dai *rk_spdifrx_init_dai(struct HAL_SPDIFRX_DEV *hSpdifrx)
{
    struct rk_spdifrx_dev *spdifrx;

    spdifrx = rt_calloc(1, sizeof(*spdifrx));
    RT_ASSERT(spdifrx);

    spdifrx->dai.ops = &rk_spdifrx_ops;
    spdifrx->dai.id = (uint32_t)hSpdifrx->pReg;
    spdifrx->dai.dmaData[AUDIO_STREAM_CAPTURE] = &hSpdifrx->rxDmaData;
    spdifrx->hSpdifrx = hSpdifrx;
    spdifrx->mclk_gate = get_clk_gate_from_id(hSpdifrx->mclkGate);

    return &spdifrx->dai;
}

int rt_hw_spdifrx_init(void)
{
#if defined(RT_USING_SPDIFRX0)
    rk_audio_register_dai(rk_spdifrx_init_dai(&g_spdifrx0Dev));
#endif
#if defined(RT_USING_SPDIFRX1)
    rk_audio_register_dai(rk_spdifrx_init_dai(&g_spdifrx1Dev));
#endif
#if defined(RT_USING_SPDIFRX2)
    rk_audio_register_dai(rk_spdifrx_init_dai(&g_spdifrx2Dev));
#endif
#if defined(RT_USING_SPDIFRX3)
    rk_audio_register_dai(rk_spdifrx_init_dai(&g_spdifrx3Dev));
#endif

    return 0;
}

INIT_DEVICE_EXPORT(rt_hw_spdifrx_init);
#endif
