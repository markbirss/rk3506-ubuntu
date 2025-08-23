/**
  * Copyright (c) 2023 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_sai.c
  * @author  sugar zhang
  * @version V0.1
  * @date    1-Dec-2023
  * @brief   sai driver for rk soc
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#if defined(RT_USING_SAI) && \
    (defined(RT_USING_SAI0) || defined(RT_USING_SAI1) || defined(RT_USING_SAI2) || defined(RT_USING_SAI3) || \
     defined(RT_USING_SAI4) || defined(RT_USING_SAI5) || defined(RT_USING_SAI6) || defined(RT_USING_SAI7) || \
     defined(RT_USING_SAI8) || defined(RT_USING_SAI9) || defined(RT_USING_SAI10)|| defined(RT_USING_SAI11)|| \
     defined(RT_USING_SAI12)|| defined(RT_USING_SAI13)|| defined(RT_USING_SAI14)|| defined(RT_USING_SAI15))

#include "rk_audio.h"
#include "drv_clock.h"
#ifdef RT_USING_PM
#include <drivers/pm.h>
#endif

#include "hal_base.h"
#include "hal_bsp.h"

struct rk_sai_dev
{
    struct rt_device parent;
    struct audio_dai dai;
    struct HAL_SAI_DEV *hSai;
    struct clk_gate *mclk_gate;
#if defined(RT_USING_PMU)
    struct pd *pd;
#endif
};

static inline struct rk_sai_dev *to_sai(struct audio_dai *dai)
{
    return HAL_CONTAINER_OF(dai, struct rk_sai_dev, dai);
}

static rt_err_t rk_sai_init(struct audio_dai *dai,
                            struct AUDIO_INIT_CONFIG *config)
{
    struct rk_sai_dev *sai = to_sai(dai);

#if defined(RT_USING_PMU)
    if (sai->pd)
        pd_on(sai->pd);
#endif
    clk_enable(sai->mclk_gate);

    HAL_SAI_DevInit(sai->hSai, config);

    return RT_EOK;
}

static rt_err_t rk_sai_deinit(struct audio_dai *dai)
{
    struct rk_sai_dev *sai = to_sai(dai);

    HAL_SAI_DevDeInit(sai->hSai);

    clk_disable(sai->mclk_gate);

#if defined(RT_USING_PMU)
    if (sai->pd)
        pd_off(sai->pd);
#endif

    return RT_EOK;
}

static rt_err_t rk_sai_set_clk(struct audio_dai *dai,
                               eAUDIO_streamType stream,
                               uint32_t freq)
{
    struct rk_sai_dev *sai = to_sai(dai);

    clk_set_rate(sai->hSai->mclk, freq);

    return RT_EOK;
}

static rt_err_t rk_sai_config(struct audio_dai *dai,
                              eAUDIO_streamType stream,
                              struct AUDIO_PARAMS *params)
{
    struct rk_sai_dev *sai = to_sai(dai);

    HAL_SAI_DevConfig(sai->hSai, stream, params);

    return RT_EOK;
}

static rt_err_t rk_sai_start(struct audio_dai *dai, eAUDIO_streamType stream)
{
    struct rk_sai_dev *sai = to_sai(dai);

    HAL_SAI_DevEnable(sai->hSai, stream);

    return RT_EOK;
}

static rt_err_t rk_sai_stop(struct audio_dai *dai, eAUDIO_streamType stream)
{
    struct rk_sai_dev *sai = to_sai(dai);

    HAL_SAI_DevDisable(sai->hSai, stream);

    return RT_EOK;
}

#if defined(RT_USING_PM)
static int rk_sai_pm_suspend(const struct rt_device *device, rt_uint8_t mode)
{
    return RT_EOK;
}

static void rk_sai_pm_resume(const struct rt_device *device, rt_uint8_t mode)
{
}

static struct rt_device_pm_ops rk_sai_pm_ops =
{
    .suspend = rk_sai_pm_suspend,
    .resume = rk_sai_pm_resume,
};
#endif

static const struct audio_dai_ops rk_sai_ops =
{
    .init = rk_sai_init,
    .deinit = rk_sai_deinit,
    .set_clk = rk_sai_set_clk,
    .config = rk_sai_config,
    .start = rk_sai_start,
    .stop = rk_sai_stop,
};

static struct audio_dai *rk_sai_init_dai(struct HAL_SAI_DEV *hSai)
{
    struct rk_sai_dev *sai;

    sai = rt_calloc(1, sizeof(*sai));
    RT_ASSERT(sai);

    sai->dai.ops = &rk_sai_ops;
    sai->dai.id = (uint32_t)hSai->pReg;
    sai->dai.dmaData[AUDIO_STREAM_PLAYBACK] = &hSai->txDmaData;
    sai->dai.dmaData[AUDIO_STREAM_CAPTURE] = &hSai->rxDmaData;
    sai->hSai = hSai;
    sai->mclk_gate = get_clk_gate_from_id(hSai->mclkGate);
#if defined(RT_USING_PMU)
    sai->pd = get_pd_from_id(hSai->pd);
#endif
#if defined(RT_USING_PM)
    sai->parent.user_data = sai;
    rt_pm_device_register(&sai->parent, &rk_sai_pm_ops);
#endif

    return &sai->dai;
}

int rt_hw_sai_init(void)
{
#if defined(RT_USING_SAI0)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai0Dev));
#endif
#if defined(RT_USING_SAI1)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai1Dev));
#endif
#if defined(RT_USING_SAI2)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai2Dev));
#endif
#if defined(RT_USING_SAI3)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai3Dev));
#endif
#if defined(RT_USING_SAI4)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai4Dev));
#endif
#if defined(RT_USING_SAI5)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai5Dev));
#endif
#if defined(RT_USING_SAI6)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai6Dev));
#endif
#if defined(RT_USING_SAI7)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai7Dev));
#endif
#if defined(RT_USING_SAI8)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai8Dev));
#endif
#if defined(RT_USING_SAI9)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai9Dev));
#endif
#if defined(RT_USING_SAI10)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai10Dev));
#endif
#if defined(RT_USING_SAI11)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai11Dev));
#endif
#if defined(RT_USING_SAI12)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai12Dev));
#endif
#if defined(RT_USING_SAI13)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai13Dev));
#endif
#if defined(RT_USING_SAI14)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai14Dev));
#endif
#if defined(RT_USING_SAI15)
    rk_audio_register_dai(rk_sai_init_dai(&g_sai15Dev));
#endif

    return 0;
}

INIT_DEVICE_EXPORT(rt_hw_sai_init);
#endif
