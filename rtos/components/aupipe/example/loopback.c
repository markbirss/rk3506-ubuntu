/**
  * Copyright (c) 2022 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include "aupipe.h"

static Aupipe *aupipe = NULL;

void loopback_aupipe(void *arg)
{
    ApObject *audiosrc;
    ApObject *audiosink;
    ApObject *queue;
    ApEventMsg *msg;
    int ret;

    int type = DATA_TYPE_AUDIO;
    uint16_t ch_map[2] = {0x0000, 0x0101};
    ApLink *links = aupipe_link_new(1, &type);;
    ApAudioLink *aud_link;
    RT_ASSERT(links != NULL);
    aud_link = links->desc[0];
    aud_link->in_channels = 8;
    aud_link->out_channels = 2;
    aud_link->channel_map = ch_map;

    aupipe_list_object_template();

    aupipe = aupipe_new();
    if (!aupipe)
        goto null_aupipe;

    audiosrc = aupipe_object_new("audiosrc", NULL);
    if (!audiosrc)
    {
        LOG_E("Create audiosrc failed");
        goto null_audiosrc;
    }
    LOG_I("Create %p %s", audiosrc, audiosrc->name);
    audiosrc->init(audiosrc);

    queue = aupipe_object_new("queue", NULL);
    if (!queue)
    {
        LOG_E("Create queue failed");
        goto null_queue;
    }
    LOG_I("Create %p %s", queue, queue->name);
    queue->init(queue);

    audiosink = aupipe_object_new("audiosink", NULL);
    if (!audiosink)
    {
        LOG_E("Create audiosink failed");
        goto null_audiosink;
    }
    LOG_I("Create %p %s", audiosink, audiosink->name);
    audiosink->src_links = links;
    audiosink->init(audiosink);

    ret = aupipe_add_object(aupipe, audiosrc, queue, audiosink, NULL);
    RT_ASSERT(ret == RT_EOK);
    ret = aupipe_link_object(audiosrc, queue, audiosink, NULL);
    RT_ASSERT(ret == RT_EOK);

    aupipe_set_state(aupipe, AUPIPE_PLAYING);

    msg = aupipe_wait_event(aupipe);
    if (msg->need_free)
        rt_free(msg);

    aupipe_set_state(aupipe, AUPIPE_NULL);

    audiosink->deinit(audiosink);
    audiosrc->deinit(audiosrc);
    queue->deinit(queue);

    aupipe_link_delete(links);

    aupipe_object_delete(audiosink);
null_audiosink:
    aupipe_object_delete(queue);
null_queue:
    aupipe_object_delete(audiosrc);
null_audiosrc:
    aupipe_delete(aupipe);
null_aupipe:
    aupipe = NULL;

    return;
}

int loopback(int argc, char *argv[])
{
    rt_thread_t tid;

    tid = rt_thread_create("loopback", loopback_aupipe, NULL,
                           4096, RT_THREAD_PRIORITY_MAX / 2, 10);
    if (tid)
        rt_thread_startup(tid);

    return RT_EOK;
}

int loopback_quit(int argc, char *argv[])
{
    ApEventMsg *msg;

    if (aupipe)
    {
        msg = rt_malloc(sizeof(ApEventMsg));
        if (!msg)
            return -RT_ERROR;
        msg->type = EVENT_TYPE_STOP;
        msg->need_free = 1;
        if (aupipe_new_event(aupipe, msg))
        {
            LOG_E("Stop failed\n");
            free(msg);
            return -RT_ERROR;
        }
    }

    return RT_EOK;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(loopback, loopback);
MSH_CMD_EXPORT(loopback_quit, quit loopback);
#endif

