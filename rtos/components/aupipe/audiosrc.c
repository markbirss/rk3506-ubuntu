/**
  * Copyright (c) 2022 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include "aupipe.h"

#ifdef RT_USING_AUPIPE_AUDIOSRC

#include "drv_heap.h"
#include "rk_audio.h"

#undef DBG_SECTION_NAME
#define DBG_SECTION_NAME    "AUDIOSRC"

#define AUDIOSRC_DEFAULT_CARD       "acodecc"
#define AUDIOSRC_DEFAULT_RATE       48000
#define AUDIOSRC_DEFAULT_BITS       16
#define AUDIOSRC_DEFAULT_CHANNELS   8
#define AUDIOSRC_DEFAULT_DURATION   -1

#define AUDIOSRC(obj)               ((ApAudioSrc *)(obj))

enum
{
    PROP_CARD = 0,
    PROP_SAMPLERATE,
    PROP_SAMPLEBITS,
    PROP_CHANNELS,
    PROP_DURATION,
    PROP_MAX,
};

typedef struct audiosrc_object
{
    ApObject base;
    ApPad *src_pad;
    ApProperty props[PROP_MAX];

    rt_thread_t loop;
    int loop_running;
    struct rt_device *card;
    struct AUDIO_PARAMS param;
    struct audio_buf abuf;
    uint32_t periods;
    rt_sem_t sem;
} ApAudioSrc;

static int audiosrc_init(ApObject *obj)
{
    ApAudioSrc *audsrc = AUDIOSRC(obj);

    memset(&audsrc->props, 0, sizeof(audsrc->props));

    strcpy(audsrc->props[PROP_CARD].name, "card");
    audsrc->props[PROP_CARD].value.v.str_v = AUDIOSRC_DEFAULT_CARD;
    audsrc->props[PROP_CARD].value.type = VALUE_TYPE_STRING;

    strcpy(audsrc->props[PROP_SAMPLERATE].name, "rate");
    audsrc->props[PROP_SAMPLERATE].value.v.int_v = AUDIOSRC_DEFAULT_RATE;
    audsrc->props[PROP_SAMPLERATE].value.type = VALUE_TYPE_INT;

    strcpy(audsrc->props[PROP_SAMPLEBITS].name, "bits");
    audsrc->props[PROP_SAMPLEBITS].value.v.int_v = AUDIOSRC_DEFAULT_BITS;
    audsrc->props[PROP_SAMPLEBITS].value.type = VALUE_TYPE_INT;

    strcpy(audsrc->props[PROP_CHANNELS].name, "channels");
    audsrc->props[PROP_CHANNELS].value.v.int_v = AUDIOSRC_DEFAULT_CHANNELS;
    audsrc->props[PROP_CHANNELS].value.type = VALUE_TYPE_INT;

    strcpy(audsrc->props[PROP_DURATION].name, "duration");
    audsrc->props[PROP_DURATION].value.v.int_v = AUDIOSRC_DEFAULT_DURATION;
    audsrc->props[PROP_DURATION].value.type = VALUE_TYPE_INT;

    audsrc->src_pad = aupipe_pad_new(obj);
    RT_ASSERT(audsrc->src_pad != NULL);

    audsrc->sem = rt_sem_create("audsrc", 0, RT_IPC_FLAG_FIFO);
    RT_ASSERT(audsrc->sem != NULL);

    return RT_EOK;
}

static int audiosrc_deinit(ApObject *obj)
{
    ApAudioSrc *audsrc = AUDIOSRC(obj);
    rt_free(audsrc->src_pad);
    rt_sem_delete(audsrc->sem);

    return RT_EOK;
}

static void do_capture(void *arg)
{
    ApAudioSrc *obj = AUDIOSRC(arg);
    ApAudioBuffer *buffer;
    int period_bytes;
    int rate = obj->param.sampleRate;
    int bits = obj->param.sampleBits;
    int ch = obj->param.channels;

    period_bytes = obj->abuf.period_size * obj->param.channels * (obj->param.sampleBits >> 3);

    while (obj->loop_running)
    {
        buffer = (ApAudioBuffer *)aupipe_buffer_new(DATA_TYPE_AUDIO, period_bytes);
        if (!buffer)
        {
            LOG_E("malloc %d failed", period_bytes);
            break;
        }
        buffer->rate = rate;
        buffer->bits = bits;
        buffer->channels = ch;
        if (!rt_device_read(obj->card, 0, buffer->buf.data, obj->abuf.period_size))
        {
            LOG_E("Error capturing sample\n");
            break;
        }
        buffer = (ApAudioBuffer *)aupipe_buffer_ref(&buffer->buf);
        aupipe_pad_push(obj->src_pad, &buffer->buf);
    }
    rt_sem_release(obj->sem);
    obj->loop_running = 0;
}

static int audiosrc_open_card(ApAudioSrc *obj)
{
    char *card;
    int rate;
    int bits;
    int channels;
    int period_size = 1024;
    int period_count = 4;
    int size;
    int duration;

    struct AUDIO_PARAMS *param = &obj->param;
    struct audio_buf *abuf = &obj->abuf;
    rt_err_t ret;

    aupipe_value_get(&obj->props[PROP_CARD].value, &card);
    aupipe_value_get(&obj->props[PROP_SAMPLERATE].value, &rate);
    aupipe_value_get(&obj->props[PROP_SAMPLEBITS].value, &bits);
    aupipe_value_get(&obj->props[PROP_CHANNELS].value, &channels);
    aupipe_value_get(&obj->props[PROP_DURATION].value, &duration);

    obj->card = rt_device_find(card);
    if (!obj->card)
    {
        LOG_E("Can't find sound device: %s\n", card);
        goto no_card;
    }

    ret = rt_device_open(obj->card, RT_DEVICE_OFLAG_RDONLY);
    if (ret < 0)
    {
        rt_kprintf("Failed to open %s, err: %d\n", obj->card->parent.name, ret);
        goto open_failed;
    }

    abuf->period_size = period_size;
    abuf->buf_size = period_size * period_count;
    size = abuf->buf_size * channels * (bits >> 3); /* frames to bytes */
    abuf->buf = rt_malloc_uncache(size);
    if (!abuf->buf)
    {
        rt_kprintf("Buffer alloc failed!\n");
        goto malloc_failed;
    }

    param->channels = channels;
    param->sampleRate = rate;
    param->sampleBits = bits;

    ret = rt_device_control(obj->card, RK_AUDIO_CTL_PCM_PREPARE, abuf);
    RT_ASSERT(ret == RT_EOK);

    ret = rt_device_control(obj->card, RK_AUDIO_CTL_HW_PARAMS, param);
    RT_ASSERT(ret == RT_EOK);

    return RT_EOK;

malloc_failed:
    rt_device_close(obj->card);
open_failed:
no_card:
    return -RT_ERROR;
}

static int audiosrc_close_card(ApAudioSrc *obj)
{
    struct audio_buf *abuf = &obj->abuf;
    rt_err_t ret;

    ret = rt_device_control(obj->card, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(obj->card, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);

    rt_free_uncache(abuf->buf);
    rt_device_close(obj->card);

    return RT_EOK;
}

static int audiosrc_set_state(ApObject *obj, int state)
{
    ApAudioSrc *audsrc = AUDIOSRC(obj);
    switch (state)
    {
    case STATE_NULL_TO_READY:
        LOG_I("STATE_NULL_TO_READY");
        if (audiosrc_open_card(audsrc) != RT_EOK)
            return -RT_ERROR;
        audsrc->loop = rt_thread_create("audiosrc", do_capture, audsrc,
                                        4096, RT_THREAD_PRIORITY_MAX / 2, 10);
        if (!audsrc->loop)
        {
            audiosrc_close_card(audsrc);
            return -RT_ERROR;
        }
        break;
    case STATE_PAUSED_TO_PLAYING:
        LOG_I("STATE_PAUSED_TO_PLAYING");
        audsrc->loop_running = 1;
        rt_thread_startup(audsrc->loop);
        break;
    case STATE_PLAYING_TO_PAUSED:
        LOG_I("STATE_PLAYING_TO_PAUSED");
        if (audsrc->loop_running == 1)
        {
            /* loop still running */
            audsrc->loop_running = 0;
            if (rt_sem_take(audsrc->sem, 3000))
            {
                /* Timeout, force delete */
                LOG_W("Timeout");
                rt_thread_delete(audsrc->loop);
            }
        }
        break;
    case STATE_READY_TO_NULL:
        LOG_I("STATE_READY_TO_NULL");
        audiosrc_close_card(audsrc);
        break;
    default:
        break;
    }

    return RT_EOK;
}

static int audiosrc_set_property(ApObject *obj, char *name, void *arg)
{
    ApAudioSrc *audsrc = AUDIOSRC(obj);
    for (int i = 0; i < PROP_MAX; i++)
    {
        if (strcmp(name, audsrc->props[i].name) == 0)
        {
            aupipe_value_set(&audsrc->props[i].value, arg);
            return RT_EOK;
        }
    }

    LOG_W("no property named %s", name);

    return -RT_ERROR;
}

static int audiosrc_get_property(ApObject *obj, char *name, void *arg)
{
    ApAudioSrc *audsrc = AUDIOSRC(obj);
    for (int i = 0; i < PROP_MAX; i++)
    {
        if (strcmp(name, audsrc->props[i].name) == 0)
        {
            aupipe_value_get(&audsrc->props[i].value, arg);
            return RT_EOK;
        }
    }

    LOG_W("no property named %s", name);

    return -RT_ERROR;
}

static ApPad *audiosrc_get_pad(ApObject *obj, int type)
{
    ApAudioSrc *audsrc = AUDIOSRC(obj);

    if (type == PAD_TYPE_SINK)
        return NULL;

    return audsrc->src_pad;
}

OBJECT_BASE_DEFINE(audiosrc, ApAudioSrc);

#endif

