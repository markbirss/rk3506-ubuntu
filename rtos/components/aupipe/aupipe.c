/**
  * Copyright (c) 2022 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include "aupipe.h"

struct obj_template
{
    ApObject *template;
    int ref;
    size_t size;
};

struct object_factory
{
    struct obj_template *objs;
    rt_mutex_t lock;
    size_t size;
    size_t count;
};

static struct object_factory *factory = NULL;

int aupipe_init(void)
{
    factory = rt_malloc(sizeof(struct object_factory));
    if (!factory)
    {
        LOG_E("malloc factory failed");
        goto new_factory_err;
    }
    factory->lock = rt_mutex_create("factory", RT_IPC_FLAG_FIFO);
    if (!factory->lock)
    {
        LOG_E("create factory lock failed");
        goto new_factory_lock_err;
    }
    factory->size = 5;
    factory->count = 0;
    factory->objs = rt_malloc(sizeof(struct obj_template) * factory->size);
    if (!factory->objs)
    {
        LOG_E("malloc failed");
        goto new_factory_objs_err;
    }

    return RT_EOK;

new_factory_objs_err:
    rt_mutex_delete(factory->lock);
new_factory_lock_err:
    rt_free(factory);
    factory = NULL;
new_factory_err:

    return -RT_ERROR;
}
INIT_COMPONENT_EXPORT(aupipe_init);

Aupipe *aupipe_new(void)
{
    Aupipe *aupipe = rt_malloc(sizeof(Aupipe));
    if (!aupipe)
    {
        LOG_E("Create aupipe failed");
        goto null_aupipe;
    }
    memset(aupipe, 0, sizeof(Aupipe));

    aupipe->event = aupipe_event_new();
    if (!aupipe->event)
    {
        LOG_E("Create aupipe event failed");
        goto null_event;
    }

    aupipe_list_init(&aupipe->objs);
    aupipe->state = AUPIPE_NULL;

    return aupipe;

null_event:
    rt_free(aupipe);
null_aupipe:
    return NULL;
}

static int call_obj_set_state(ApObject *obj)
{
    Aupipe *aupipe = obj->parent;
    return obj->set_state(obj, aupipe->state_change);
}

extern void aupipe_dump_list(ApList *list);
static int aupipe_poll_objs(Aupipe *aupipe, poll_func func)
{
    ApList *list = &aupipe->objs;
    ApList *last = aupipe->objs.last;

    do
    {
        list = list->next;
        if (func((ApObject *)list->data) != RT_EOK)
            return -RT_ERROR;
    }
    while (list != last);

    return RT_EOK;
}

ApEventMsg *aupipe_wait_event(Aupipe *aupipe)
{
    return aupipe_event_pull(aupipe->event);
}

int aupipe_new_event(Aupipe *aupipe, ApEventMsg *msg)
{
    return aupipe_event_push(aupipe->event, msg);
}

int aupipe_set_state(Aupipe *aupipe, uint32_t state)
{
    uint32_t new_state;
    uint32_t state_change;

    if (aupipe->state != state)
    {
        if (aupipe->state < state)
        {
            do
            {
                new_state = aupipe->state << 1;
                LOG_I("%s %x => %x, %x", __func__, aupipe->state, new_state, state);
                state_change = STATE_CHANGE(aupipe->state, new_state);
                aupipe->state_change = state_change;
                if (aupipe_poll_objs(aupipe, call_obj_set_state) != RT_EOK)
                    return -RT_ERROR;
                aupipe->state = new_state;
            }
            while (aupipe->state < state);
        }
        else
        {
            do
            {
                new_state = aupipe->state >> 1;
                LOG_I("%s %x => %x, %x", __func__, aupipe->state, new_state, state);
                state_change = STATE_CHANGE(aupipe->state, new_state);
                aupipe->state_change = state_change;
                if (aupipe_poll_objs(aupipe, call_obj_set_state) != RT_EOK)
                    return -RT_ERROR;
                aupipe->state = new_state;
            }
            while (aupipe->state > state);
        }
    }

    return RT_EOK;
}

void aupipe_delete(Aupipe *aupipe)
{
    ApList *list;

    while (!aupipe_list_is_empty(&aupipe->objs))
    {
        list = aupipe_list_pop_tail(&aupipe->objs);
        rt_free(list);
    }

    aupipe_event_delete(aupipe->event);
    rt_free(aupipe);
}

int aupipe_add_object(Aupipe *aupipe, ApObject *obj, ...)
{
    va_list valist;
    ApList *list;

    va_start(valist, obj);
    while (obj != NULL)
    {
        obj->parent = aupipe;
        list = rt_malloc(sizeof(ApList));
        if (!list)
            return -RT_ERROR;
        list->data = obj;
        aupipe_list_insert_head(&aupipe->objs, list);
        obj = va_arg(valist, ApObject *);
    }
    va_end(valist);

    return RT_EOK;
}

int aupipe_link_object(ApObject *obj, ...)
{
    ApObject *obj_src, *obj_sink;
    ApPad *pad_src, *pad_sink;
    va_list valist;
    int ret = RT_EOK;

    obj_src = obj_sink = obj;
    va_start(valist, obj);
    while (1)
    {
        obj_src = obj_sink;
        obj_sink = va_arg(valist, ApObject *);
        if ((obj_src == NULL) || (obj_sink == NULL))
        {
            LOG_I("End of valist");
            break;
        }
        if (!obj_src->get_pad || !obj_sink->get_pad)
        {
            LOG_E("no found get_pad");
            ret = -RT_ERROR;
            break;
        }
        pad_src = obj_src->get_pad(obj_src, PAD_TYPE_SRC);
        pad_sink = obj_sink->get_pad(obj_sink, PAD_TYPE_SINK);
        if (!pad_src || !pad_sink)
        {
            LOG_E("cannot get pad");
            ret = -RT_ERROR;
            break;
        }
        pad_src->peer = pad_sink;
        pad_sink->peer = pad_src;
    }
    va_end(valist);

    return ret;
}

void aupipe_list_object_template(void)
{
    for (int i = 0; i < factory->count; i++)
        printf("[%d] %s\n", i, factory->objs[i].template->name);
}

ApObject *aupipe_object_new(char *temp_name, char *name)
{
    ApObject *obj;
    char temp[32];

    if (factory == NULL)
    {
        LOG_E("factory is null");
        return NULL;
    }

    for (int i = 0; i < factory->count; i++)
    {
        if (strcmp(temp_name, factory->objs[i].template->name) == 0)
        {
            obj = rt_malloc(factory->objs[i].size);
            if (!obj)
            {
                LOG_E("malloc failed");
                return NULL;
            }
            memset(obj, 0, factory->objs[i].size);
            memcpy(obj, factory->objs[i].template, sizeof(ApObject));
            obj->src_links = obj->sink_links = NULL;
            if (name != NULL)
            {
                strcpy(obj->name, name);
            }
            else
            {
                snprintf(temp, sizeof(temp), "%s%d%c",
                         factory->objs[i].template->name,
                         factory->objs[i].ref, '\0');
                memcpy(obj->name, temp, sizeof(obj->name));
            }
            factory->objs[i].ref++;

            return obj;
        }
    }

    LOG_W("No object named %s", temp_name);

    return NULL;
}

void aupipe_object_delete(ApObject *obj)
{
    rt_free(obj);
}

int aupipe_object_register(ApObject *obj, size_t obj_size)
{
    struct obj_template *new_objs;

    if (factory == NULL)
    {
        LOG_E("factory is null");
        return -RT_ERROR;
    }

    rt_mutex_take(factory->lock, RT_WAITING_FOREVER);

    for (int i = 0; i < factory->count; i++)
    {
        if (strcmp(obj->name, factory->objs[i].template->name) == 0)
        {
            LOG_E("%s is already registered", obj->name);
            return -RT_ERROR;
        }
    }
    if (factory->count == factory->size)
    {
        factory->size *= 2;
        new_objs = (struct obj_template *)rt_realloc(factory->objs, sizeof(struct obj_template) * factory->size);
        if (!new_objs)
        {
            LOG_E("expansion failed");
            return -RT_ERROR;
        }
        factory->objs = new_objs;
    }
    factory->objs[factory->count].template = obj;
    factory->objs[factory->count].size = obj_size;
    factory->objs[factory->count].ref = 0;
    factory->count++;

    rt_mutex_release(factory->lock);

    return RT_EOK;
}

void aupipe_value_set(ApValue *value, void *arg)
{
    switch (value->type)
    {
    case VALUE_TYPE_INT:
    {
        int *val = (int *)arg;
        value->v.int_v = *val;
        break;
    }
    case VALUE_TYPE_FLOAT:
    {
        float *val = (float *)arg;
        value->v.float_v = *val;
        break;
    }
    case VALUE_TYPE_STRING:
    {
        char **val = (char **)arg;
        value->v.str_v = *val;
        break;
    }
    default:
    {
        LOG_E("Error value type");
        break;
    }
    }
}

void aupipe_value_get(ApValue *value, void *arg)
{
    switch (value->type)
    {
    case VALUE_TYPE_INT:
    {
        int *val = (int *)arg;
        *val = value->v.int_v;
        break;
    }
    case VALUE_TYPE_FLOAT:
    {
        float *val = (float *)arg;
        *val = value->v.float_v;
        break;
    }
    case VALUE_TYPE_STRING:
    {
        char **val = (char **)arg;
        *val = value->v.str_v;
        break;
    }
    default:
    {
        LOG_E("Error value type");
        break;
    }
    }
}

ApBuffer *aupipe_buffer_new(int type, size_t size)
{
    int tsize;

    switch (type)
    {
    case DATA_TYPE_AUDIO:
        tsize = sizeof(ApAudioBuffer);
        break;
    default:
        tsize = sizeof(ApBuffer);
        break;
    }
    ApBuffer *pbuffer = rt_malloc(tsize);
    if (!pbuffer)
        goto no_pbuffer;
    pbuffer->type = type;
    pbuffer->len = size;
    pbuffer->data = rt_malloc(size);
    if (!pbuffer->data)
        goto no_databuf;
    pbuffer->ref = 0;
    pbuffer->ref_lock = rt_mutex_create("ref_lock", RT_IPC_FLAG_FIFO);
    if (!pbuffer->ref_lock)
        goto no_lock;

    return pbuffer;

no_lock:
    rt_free(pbuffer->data);
no_databuf:
    rt_free(pbuffer);
no_pbuffer:

    return NULL;
}

ApBuffer *aupipe_buffer_ref(ApBuffer *buffer)
{
    rt_mutex_take(buffer->ref_lock, RT_WAITING_FOREVER);
    buffer->ref++;
    rt_mutex_release(buffer->ref_lock);

    return buffer;
}

void aupipe_buffer_unref(ApBuffer *buffer)
{
    rt_mutex_take(buffer->ref_lock, RT_WAITING_FOREVER);
    if (buffer->ref)
        buffer->ref--;
    rt_mutex_release(buffer->ref_lock);

    if (!buffer->ref)
    {
        rt_free(buffer->data);
        rt_mutex_delete(buffer->ref_lock);
        rt_free(buffer);
    }
}

