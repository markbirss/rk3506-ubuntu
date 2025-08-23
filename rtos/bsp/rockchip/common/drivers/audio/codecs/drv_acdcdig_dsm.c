/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-08-15     Jason Zhu    first implementation
 */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_ACDCDIG_DSM

#include "rk_audio.h"
#include "drv_clock.h"
#include "drv_regulator.h"

#include "hal_base.h"
#include "hal_bsp.h"

struct rk_acdcdig_dsm_dev
{
    struct audio_codec codec;
    struct HAL_ACDCDIG_DSM_DEV *hAcdcDigDsm;
#if defined(RT_USING_PMU)
    struct pd *pd;
#endif
};

static inline struct rk_acdcdig_dsm_dev *to_acdcdig_dsm(struct audio_codec *codec)
{
    return HAL_CONTAINER_OF(codec, struct rk_acdcdig_dsm_dev, codec);
}

static rt_err_t rk_acdcdig_dsm_init(struct audio_codec *codec,
                                    struct AUDIO_INIT_CONFIG *config)
{
    struct rk_acdcdig_dsm_dev *dsm = to_acdcdig_dsm(codec);
#ifdef HAL_PWR_MODULE_ENABLED
    struct regulator_desc *ldo_audio;

    ldo_audio = regulator_get_desc_by_pwrid(PWR_ID_VCC_AUDIO);
    if (!ldo_audio)
        return -RT_ERROR;


    regulator_enable(ldo_audio);
#endif
#if defined(RT_USING_PMU)
    if (dsm->pd)
        pd_on(dsm->pd);
#endif

    HAL_ACDCDIG_DSM_Init(dsm->hAcdcDigDsm, config);

    return RT_EOK;
}

static rt_err_t rk_acdcdig_dsm_deinit(struct audio_codec *codec)
{
    struct rk_acdcdig_dsm_dev *dsm = to_acdcdig_dsm(codec);
#ifdef HAL_PWR_MODULE_ENABLED
    struct regulator_desc *ldo_audio;
#endif

    HAL_ACDCDIG_DSM_DeInit(dsm->hAcdcDigDsm);

#if defined(RT_USING_PMU)
    if (dsm->pd)
        pd_off(dsm->pd);
#endif
#ifdef HAL_PWR_MODULE_ENABLED
    ldo_audio = regulator_get_desc_by_pwrid(PWR_ID_VCC_AUDIO);
    if (!ldo_audio)
        return -RT_ERROR;

    regulator_disable(ldo_audio);
#endif

    return RT_EOK;
}

static rt_err_t rk_acdcdig_dsm_set_clk(struct audio_codec *codec,
                                       eAUDIO_streamType stream,
                                       uint32_t freq)
{
    return RT_EOK;
}

static rt_err_t rk_acdcdig_dsm_config(struct audio_codec *codec,
                                      eAUDIO_streamType stream,
                                      struct AUDIO_PARAMS *params)
{
    struct rk_acdcdig_dsm_dev *dsm = to_acdcdig_dsm(codec);

    HAL_ACDCDIG_DSM_Config(dsm->hAcdcDigDsm, stream, params);

    return RT_EOK;
}

static rt_err_t rk_acdcdig_dsm_start(struct audio_codec *codec,
                                     eAUDIO_streamType stream)
{
    struct rk_acdcdig_dsm_dev *dsm = to_acdcdig_dsm(codec);

    HAL_ACDCDIG_DSM_Enable(dsm->hAcdcDigDsm, stream);

    return RT_EOK;
}

static rt_err_t rk_acdcdig_dsm_stop(struct audio_codec *codec,
                                    eAUDIO_streamType stream)
{
    struct rk_acdcdig_dsm_dev *dsm = to_acdcdig_dsm(codec);

    HAL_ACDCDIG_DSM_Disable(dsm->hAcdcDigDsm, stream);

    return RT_EOK;
}

static rt_err_t rk_acdcdig_dsm_set_gain(struct audio_codec *codec,
                                        eAUDIO_streamType stream,
                                        struct AUDIO_DB_CONFIG *db_config)
{
    struct rk_acdcdig_dsm_dev *dsm = to_acdcdig_dsm(codec);

    HAL_ACDCDIG_DSM_SetGain(dsm->hAcdcDigDsm, stream, db_config);

    return RT_EOK;
}

static rt_err_t rk_acdcdig_dsm_get_gain(struct audio_codec *codec,
                                        eAUDIO_streamType stream,
                                        struct AUDIO_DB_CONFIG *db_config)
{
    struct rk_acdcdig_dsm_dev *dsm = to_acdcdig_dsm(codec);

    HAL_ACDCDIG_DSM_GetGain(dsm->hAcdcDigDsm, stream, db_config);

    return RT_EOK;
}

static const struct audio_codec_ops rk_acdcdig_dsm_ops =
{
    .init = rk_acdcdig_dsm_init,
    .deinit = rk_acdcdig_dsm_deinit,
    .set_clk = rk_acdcdig_dsm_set_clk,
    .config = rk_acdcdig_dsm_config,
    .start = rk_acdcdig_dsm_start,
    .stop = rk_acdcdig_dsm_stop,
    .set_gain = rk_acdcdig_dsm_set_gain,
    .get_gain = rk_acdcdig_dsm_get_gain,
};

static struct audio_codec *rk_acdcdig_dsm_init_codec(struct HAL_ACDCDIG_DSM_DEV *hAcdcDigDsm)
{
    struct rk_acdcdig_dsm_dev *dsm;

    dsm = rt_calloc(1, sizeof(*dsm));
    RT_ASSERT(dsm);

    dsm->codec.ops = &rk_acdcdig_dsm_ops;
    dsm->codec.id = (uint32_t)hAcdcDigDsm->pReg;
#if defined(RT_USING_PMU)
    dsm->pd = get_pd_from_id(hAcdcDigDsm->pd);
#endif
    dsm->hAcdcDigDsm = hAcdcDigDsm;

    return &dsm->codec;
}

int rt_hw_acdcdig_dsm_init(void)
{
    rk_audio_register_codec(rk_acdcdig_dsm_init_codec(&g_acdcDigDSMDev));

    return 0;
}

INIT_DEVICE_EXPORT(rt_hw_acdcdig_dsm_init);

#endif
