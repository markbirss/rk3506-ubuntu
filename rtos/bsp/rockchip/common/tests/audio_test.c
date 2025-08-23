/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    audio_test.c
  * @author  sugar zhang
  * @version V0.1
  * @date    10-Feb-2019
  * @brief   audio test for pisces
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rthw.h>
#include <rtthread.h>

#ifdef RT_USING_COMMON_TEST_AUDIO

#include <stdint.h>
#include "rk_audio.h"
#include "hal_base.h"
#include "drv_heap.h"

#define AUDIO_PERIOD_SIZE  (1024) /* frames */
#define AUDIO_PERIOD_COUNT  (4)
#define AUDIO_PARAMS_CH  (2)
#define AUDIO_PARAMS_SB  (AUDIO_SAMPLEBITS_16)
#define AUDIO_PARAMS_FS  (AUDIO_SAMPLERATE_16000)

static int audio_capture(int argc, char **argv)
{
    struct AUDIO_PARAMS aparams;
    struct audio_buf abuf;
    struct rt_device *pcm_dev;
    char *dev = NULL;
    rt_err_t ret = RT_EOK;

    if (argc < 2)
    {
        rt_kprintf("Usage: %s [card device]\n", argv[0]);

        return 1;
    }
    dev = argv[1];
    rt_kprintf("%s: %s %s\n", __func__, argv[0], argv[1]);

    pcm_dev = rt_device_find(dev);
    if (!pcm_dev)
    {
        rt_kprintf("fail to find device %s\n", dev);

        return 1;
    }

    rt_device_open(pcm_dev, RT_DEVICE_OFLAG_RDONLY);

    /* config stream */
    rt_memset(&aparams, 0x0, sizeof(aparams));
    aparams.channels = AUDIO_PARAMS_CH;
    aparams.sampleRate = AUDIO_PARAMS_FS;
    aparams.sampleBits = AUDIO_PARAMS_SB;

    ret = rt_device_control(pcm_dev, RK_AUDIO_CTL_HW_PARAMS, &aparams);
    RT_ASSERT(ret == RT_EOK);

    rt_memset(&abuf, 0x0, sizeof(abuf));
    abuf.buf_size = abuf.period_size * AUDIO_PERIOD_COUNT;
    abuf.period_size = AUDIO_PERIOD_SIZE * aparams.channels * aparams.sampleBits / 8;
    abuf.buf = (uint8_t *)rt_malloc_uncache(abuf.buf_size);
    RT_ASSERT(abuf.buf != RT_NULL);

    ret = rt_device_control(pcm_dev, RK_AUDIO_CTL_START, NULL);
    RT_ASSERT(ret == RT_EOK);

    return 0;
}

static int audio_playback(int argc, char **argv)
{
    struct AUDIO_PARAMS aparams;
    struct audio_buf abuf;
    struct rt_device *pcm_dev;
    char *dev = NULL;
    rt_err_t ret = RT_EOK;

    if (argc < 2)
    {
        rt_kprintf("Usage: %s [card device]\n", argv[0]);

        return 1;
    }
    dev = argv[1];
    rt_kprintf("%s: %s %s\n", __func__, argv[0], argv[1]);

    pcm_dev = rt_device_find(dev);
    if (!pcm_dev)
    {
        rt_kprintf("fail to find device %s\n", dev);

        return 1;
    }

    rt_device_open(pcm_dev, RT_DEVICE_OFLAG_WRONLY);

    /* config stream */
    rt_memset(&aparams, 0x0, sizeof(aparams));
    aparams.channels = AUDIO_PARAMS_CH;
    aparams.sampleRate = AUDIO_PARAMS_FS;
    aparams.sampleBits = AUDIO_PARAMS_SB;

    ret = rt_device_control(pcm_dev, RK_AUDIO_CTL_HW_PARAMS, &aparams);
    RT_ASSERT(ret == RT_EOK);

    rt_memset(&abuf, 0x0, sizeof(abuf));
    abuf.buf_size = abuf.period_size * AUDIO_PERIOD_COUNT;
    abuf.period_size = AUDIO_PERIOD_SIZE * aparams.channels * aparams.sampleBits / 8;
    abuf.buf = (uint8_t *)rt_malloc_uncache(abuf.buf_size);
    RT_ASSERT(abuf.buf != RT_NULL);

    rt_kprintf("pcm buf: 0x%x, size: 0x%x bytes\n", abuf.buf, abuf.buf_size);
    rt_kprintf("pcm periodsize: 0x%x kbytes\n", abuf.period_size);

    ret = rt_device_control(pcm_dev, RK_AUDIO_CTL_PCM_PREPARE, &abuf);
    RT_ASSERT(ret == RT_EOK);

    ret = rt_device_control(pcm_dev, RK_AUDIO_CTL_START, NULL);
    RT_ASSERT(ret == RT_EOK);

    return 0;
}

static int audio_loopback(int argc, char **argv)
{
    char *v[2];

    if (argc < 3)
    {
        rt_kprintf("Usage: %s [capture device] [playback device]\n", argv[0]);

        return 1;
    }
    rt_kprintf("%s: %s %s %s\n", __func__, argv[0], argv[1], argv[2]);

    v[0] = argv[0];
    v[1] = argv[1];
    audio_capture(2, v);
    v[0] = argv[0];
    v[1] = argv[2];
    audio_playback(2, v);

    return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(audio_capture, audio capture test.e.g: audio_capture sound1c);
MSH_CMD_EXPORT(audio_playback, audio playback test.e.g: audio_playback sound1p);
MSH_CMD_EXPORT(audio_loopback, audio loopback test.e.g: audio_loopback sound1c sound1p);
#endif

#endif
