/**
  * Copyright (c) 2022 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include "aupipe.h"

#ifdef RT_USING_AUPIPE_AUDIOSINK

#include "drv_heap.h"
#include "rk_audio.h"

#undef DBG_SECTION_NAME
#define DBG_SECTION_NAME    "AUDIOSINK"

#define AUDIOSINK_DEFAULT_CARD       "acodecp"
#define AUDIOSINK_DEFAULT_RATE       48000
#define AUDIOSINK_DEFAULT_BITS       16
#define AUDIOSINK_DEFAULT_CHANNELS   2
#define AUDIOSINK_DEFAULT_DURATION   -1

#define AUDIOSINK(obj)               ((ApAudioSink *)(obj))

enum
{
    PROP_CARD = 0,
    PROP_SAMPLERATE,
    PROP_SAMPLEBITS,
    PROP_CHANNELS,
    PROP_DURATION,
    PROP_MAX,
};

typedef struct audiosink_object
{
    ApObject base;
    ApPad *sink_pad;
    ApProperty props[PROP_MAX];

    rt_thread_t loop;
    struct rt_device *card;
    struct AUDIO_PARAMS param;
    struct audio_buf abuf;
    uint32_t periods;
    uint8_t *map;
    short *remap_out;
} ApAudioSink;

static int audiosink_playback(ApPad *pad, ApBuffer *buffer);

static inline void audio_select_channels(uint8_t in_ch, uint8_t out_ch, uint8_t *map,
        short *in_buf, short *out_buf,
        uint32_t period_size)
{
    if (in_ch <= out_ch)
        return;

    for (int i = 0; i < period_size; i++)
        for (int j = 0; j < out_ch; j++)
            out_buf[i * out_ch + j] = in_buf[i * in_ch + map[j]];
}

static int audiosink_init(ApObject *obj)
{
    ApAudioSink *audsink = AUDIOSINK(obj);
    ApLink *links = obj->src_links;
    ApAudioLink *aud_link;

    if (links->nums > 1)
    {
        LOG_E("No support mix now");
        return -RT_ERROR;
    }
    if (links->type[0] != DATA_TYPE_AUDIO)
    {
        LOG_E("Only support audio type");
        return -RT_ERROR;
    }
    aud_link = (ApAudioLink *)links->desc[0];
    if (aud_link->in_channels != aud_link->out_channels)
    {
        audsink->map = rt_malloc(aud_link->out_channels * sizeof(uint8_t));
        RT_ASSERT(audsink->map != NULL);
        LOG_I("Audio channel remap");
        for (int i = 0; i < aud_link->out_channels; i++)
        {
            LOG_I("%d ==> %d", ((aud_link->channel_map[i] >> 8) & 0xff),
                  aud_link->channel_map[i] & 0xff);
            audsink->map[((aud_link->channel_map[i] >> 8) & 0xff)] = aud_link->channel_map[i] & 0xff;
        }
    }
    else
    {
        audsink->map = NULL;
        audsink->remap_out = NULL;
    }

    memset(&audsink->props, 0, sizeof(audsink->props));

    strcpy(audsink->props[PROP_CARD].name, "card");
    audsink->props[PROP_CARD].value.v.str_v = AUDIOSINK_DEFAULT_CARD;
    audsink->props[PROP_CARD].value.type = VALUE_TYPE_STRING;

    strcpy(audsink->props[PROP_SAMPLERATE].name, "rate");
    audsink->props[PROP_SAMPLERATE].value.v.int_v = AUDIOSINK_DEFAULT_RATE;
    audsink->props[PROP_SAMPLERATE].value.type = VALUE_TYPE_INT;

    strcpy(audsink->props[PROP_SAMPLEBITS].name, "bits");
    audsink->props[PROP_SAMPLEBITS].value.v.int_v = AUDIOSINK_DEFAULT_BITS;
    audsink->props[PROP_SAMPLEBITS].value.type = VALUE_TYPE_INT;

    strcpy(audsink->props[PROP_CHANNELS].name, "channels");
    audsink->props[PROP_CHANNELS].value.v.int_v = AUDIOSINK_DEFAULT_CHANNELS;
    audsink->props[PROP_CHANNELS].value.type = VALUE_TYPE_INT;

    strcpy(audsink->props[PROP_DURATION].name, "duration");
    audsink->props[PROP_DURATION].value.v.int_v = AUDIOSINK_DEFAULT_DURATION;
    audsink->props[PROP_DURATION].value.type = VALUE_TYPE_INT;

    audsink->sink_pad = aupipe_pad_new(obj);
    RT_ASSERT(audsink->sink_pad != NULL);
    audsink->sink_pad->process = audiosink_playback;

    return RT_EOK;
}

static int audiosink_deinit(ApObject *obj)
{
    ApAudioSink *audsink = AUDIOSINK(obj);

    rt_free(audsink->sink_pad);
    audsink->sink_pad = NULL;
    if (audsink->map)
    {
        rt_free(audsink->map);
        audsink->map = NULL;
    }

    return RT_EOK;
}

static int audiosink_playback(ApPad *pad, ApBuffer *buffer)
{
    ApAudioSink *obj = AUDIOSINK(pad->parent);
    ApAudioBuffer *aud_buf = (ApAudioBuffer *)buffer;
    short *out = (short *)buffer->data;
    int ret = RT_EOK;

    if (buffer->type != DATA_TYPE_AUDIO)
    {
        aupipe_buffer_unref(buffer);
        return -RT_ERROR;
    }

    if (obj->map)
    {
        out = obj->remap_out;
        audio_select_channels(aud_buf->channels, obj->param.channels, obj->map,
                              buffer->data, out, obj->abuf.period_size);
    }

    if (!rt_device_write(obj->card, 0, out, obj->abuf.period_size))
    {
        rt_kprintf("Error playing sample\n");
        ret = -RT_ERROR;
    }
    aupipe_buffer_unref(buffer);

    return ret;
}

static int audiosink_open_card(ApAudioSink *obj)
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

    ret = rt_device_open(obj->card, RT_DEVICE_OFLAG_WRONLY);
    if (ret < 0)
    {
        LOG_E("Failed to open %s, err: %d\n", obj->card->parent.name, ret);
        goto open_failed;
    }

    abuf->period_size = period_size;
    abuf->buf_size = period_size * period_count;
    size = abuf->buf_size * channels * (bits >> 3); /* frames to bytes */
    abuf->buf = rt_malloc_uncache(size);
    if (!abuf->buf)
    {
        LOG_E("Buffer alloc failed!\n");
        goto malloc_failed;
    }

    /* Channel remap is enable, prepare the output buffer */
    if (obj->map)
    {
        obj->remap_out = rt_malloc(abuf->period_size * channels * (bits >> 3));
        if (!obj->remap_out)
        {
            LOG_E("Malloc remap out failed");
            goto null_remap_out;
        }
    }

    param->channels = channels;
    param->sampleRate = rate;
    param->sampleBits = bits;

    ret = rt_device_control(obj->card, RK_AUDIO_CTL_PCM_PREPARE, abuf);
    RT_ASSERT(ret == RT_EOK);

    ret = rt_device_control(obj->card, RK_AUDIO_CTL_HW_PARAMS, param);
    RT_ASSERT(ret == RT_EOK);

    return RT_EOK;

null_remap_out:
    rt_free_align(abuf->buf);
malloc_failed:
    rt_device_close(obj->card);
open_failed:
no_card:
    return -RT_ERROR;
}

static int audiosink_close_card(ApAudioSink *obj)
{
    struct audio_buf *abuf = &obj->abuf;
    rt_err_t ret;

    ret = rt_device_control(obj->card, RK_AUDIO_CTL_STOP, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = rt_device_control(obj->card, RK_AUDIO_CTL_PCM_RELEASE, NULL);
    RT_ASSERT(ret == RT_EOK);

    if (obj->remap_out)
        rt_free(obj->remap_out);
    rt_free_uncache(abuf->buf);
    rt_device_close(obj->card);

    return RT_EOK;
}

static int audiosink_set_state(ApObject *obj, int state)
{
    ApAudioSink *audsink = AUDIOSINK(obj);
    switch (state)
    {
    case STATE_NULL_TO_READY:
        LOG_I("STATE_NULL_TO_READY");
        if (audiosink_open_card(audsink) != RT_EOK)
            return -RT_ERROR;
        break;
    case STATE_READY_TO_NULL:
        LOG_I("STATE_READY_TO_NULL");
        audiosink_close_card(audsink);
        break;
    default:
        break;
    }

    return RT_EOK;
}

static int audiosink_set_property(ApObject *obj, char *name, void *arg)
{
    ApAudioSink *audsink = AUDIOSINK(obj);
    for (int i = 0; i < PROP_MAX; i++)
    {
        if (strcmp(name, audsink->props[i].name) == 0)
        {
            aupipe_value_set(&audsink->props[i].value, arg);
            return RT_EOK;
        }
    }

    LOG_W("no property named %s", name);

    return -RT_ERROR;
}

static int audiosink_get_property(ApObject *obj, char *name, void *arg)
{
    ApAudioSink *audsink = AUDIOSINK(obj);
    for (int i = 0; i < PROP_MAX; i++)
    {
        if (strcmp(name, audsink->props[i].name) == 0)
        {
            aupipe_value_get(&audsink->props[i].value, arg);
            return RT_EOK;
        }
    }

    LOG_W("no property named %s", name);

    return -RT_ERROR;
}

static ApPad *audiosink_get_pad(ApObject *obj, int type)
{
    ApAudioSink *audsink = AUDIOSINK(obj);

    if (type == PAD_TYPE_SRC)
        return NULL;

    return audsink->sink_pad;
}

OBJECT_BASE_DEFINE(audiosink, ApAudioSink);

#endif

