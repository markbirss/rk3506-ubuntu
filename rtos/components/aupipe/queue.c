/**
  * Copyright (c) 2022 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include "aupipe.h"

#ifdef RT_USING_AUPIPE_QUEUE

#include "drv_heap.h"

#undef DBG_SECTION_NAME
#define DBG_SECTION_NAME    "QUEUE"

#define QUEUE(obj)               ((ApQueue *)(obj))

#define QUEUE_QUIT          (void *)0xFF00FF00

enum
{
    PROP_SKIP = 0,
    PROP_MAX,
};

typedef struct queue_object
{
    ApObject base;
    ApPad *src_pad;
    ApPad *sink_pad;
    ApProperty props[PROP_MAX];

    rt_thread_t loop;
    int loop_running;
    ApList buf_pool;
    uint32_t skip;
    rt_sem_t sem;
} ApQueue;

static int buffer_handler(ApPad *pad, ApBuffer *buffer);

static int queue_init(ApObject *obj)
{
    ApQueue *queue = QUEUE(obj);

    memset(&queue->props, 0, sizeof(queue->props));

    strcpy(queue->props[PROP_SKIP].name, "skip");
    queue->props[PROP_SKIP].value.v.str_v = 0;
    queue->props[PROP_SKIP].value.type = VALUE_TYPE_INT;

    aupipe_list_init(&queue->buf_pool);

    queue->src_pad = aupipe_pad_new(obj);
    RT_ASSERT(queue->src_pad != NULL);

    queue->sink_pad = aupipe_pad_new(obj);
    RT_ASSERT(queue->sink_pad != NULL);
    queue->sink_pad->process = buffer_handler;

    queue->sem = rt_sem_create("queue", 0, RT_IPC_FLAG_FIFO);
    RT_ASSERT(queue->sem != NULL);

    return RT_EOK;
}

static int queue_deinit(ApObject *obj)
{
    ApQueue *queue = QUEUE(obj);
    rt_free(queue->src_pad);
    rt_free(queue->sink_pad);
    rt_sem_delete(queue->sem);

    return RT_EOK;
}

static int buffer_handler(ApPad *pad, ApBuffer *buffer)
{
    ApQueue *queue = QUEUE(pad->parent);
    ApList *list;

    list = rt_malloc(sizeof(ApList));
    if (!list)
    {
        aupipe_buffer_unref(buffer);
        return -RT_ERROR;
    }
    list->data = buffer;
    aupipe_list_insert_head(&queue->buf_pool, list);

    return RT_EOK;
}

static void do_push(void *arg)
{
    ApQueue *obj = QUEUE(arg);
    ApBuffer *buffer;
    ApList *list;

    while (obj->loop_running)
    {
        if (aupipe_list_is_empty(&obj->buf_pool))
        {
            rt_thread_mdelay(1);
            continue;
        }
        list = aupipe_list_pop_tail(&obj->buf_pool);
        buffer = list->data;
        rt_free(list);
        if (buffer == QUEUE_QUIT)
            break;
        aupipe_pad_push(obj->src_pad, buffer);
    }

    rt_sem_release(obj->sem);
}

static void queue_clear_list(ApQueue *obj)
{
    ApList *list;
    ApBuffer *buffer;

    do
    {
        if (aupipe_list_is_empty(&obj->buf_pool))
            break;
        list = aupipe_list_pop_tail(&obj->buf_pool);
        buffer = (ApBuffer *)list->data;
        rt_free(list);
        if (buffer && buffer != QUEUE_QUIT)
            aupipe_buffer_unref(buffer);
    } while(1);
}

static int queue_set_state(ApObject *obj, int state)
{
    ApQueue *queue = QUEUE(obj);
    switch (state)
    {
    case STATE_NULL_TO_READY:
        LOG_I("STATE_NULL_TO_READY");
        queue->loop = rt_thread_create("queue", do_push, queue,
                                       4096, RT_THREAD_PRIORITY_MAX / 2, 10);
        if (!queue->loop)
        {
            return -RT_ERROR;
        }
        break;
    case STATE_PAUSED_TO_PLAYING:
        LOG_I("STATE_PAUSED_TO_PLAYING");
        queue->loop_running = 1;
        rt_thread_startup(queue->loop);
        break;
    case STATE_PLAYING_TO_PAUSED:
        LOG_I("STATE_PLAYING_TO_PAUSED");
        if (queue->loop_running == 1)
        {
            /* loop still running */
            ApList *list;
            list = rt_malloc(sizeof(ApList));
            if (list)
            {
                /* make sure do_push would not block
                 * in aupipe_list_is_empty
                 * */
                list->data = QUEUE_QUIT;
                aupipe_list_insert_head(&queue->buf_pool, list);
            }
            queue->loop_running = 0;
            if (rt_sem_take(queue->sem, 3000))
            {
                /* Timeout, force delete */
                LOG_W("Timeout");
                rt_thread_delete(queue->loop);
            }
        }
        break;
    case STATE_READY_TO_NULL:
        LOG_I("STATE_READY_TO_NULL");
        queue_clear_list(queue);
        break;
    default:
        break;
    }

    return RT_EOK;
}

static int queue_set_property(ApObject *obj, char *name, void *arg)
{
    ApQueue *queue = QUEUE(obj);
    for (int i = 0; i < PROP_MAX; i++)
    {
        if (strcmp(name, queue->props[i].name) == 0)
        {
            aupipe_value_set(&queue->props[i].value, arg);
            return RT_EOK;
        }
    }

    LOG_W("no property named %s", name);

    return -RT_ERROR;
}

static int queue_get_property(ApObject *obj, char *name, void *arg)
{
    ApQueue *queue = QUEUE(obj);
    for (int i = 0; i < PROP_MAX; i++)
    {
        if (strcmp(name, queue->props[i].name) == 0)
        {
            aupipe_value_get(&queue->props[i].value, arg);
            return RT_EOK;
        }
    }

    LOG_W("no property named %s", name);

    return -RT_ERROR;
}

static ApPad *queue_get_pad(ApObject *obj, int type)
{
    ApQueue *queue = QUEUE(obj);

    if (type == PAD_TYPE_SINK)
        return queue->sink_pad;

    return queue->src_pad;
}

OBJECT_BASE_DEFINE(queue, ApQueue);

#endif

