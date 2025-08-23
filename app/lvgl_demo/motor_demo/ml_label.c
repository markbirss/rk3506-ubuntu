/*
 * Copyright (c) 2024 Rockchip, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#define _GNU_SOURCE
#include <lvgl/lvgl.h>
#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "ml_label.h"

struct ml_label
{
    lv_obj_t *lv_obj;
    int txt_idx;
    struct ml_label *next;
};

static const char *txt[][2] =
{
    {"多轴控制演示", "Multi-Axis Control DEMO"},
    {"总览", "Overview"},
    {"复位", "Reset"},
    {"两轴同步", "Sync"},
    {"抖动:", "Jitter:"},
    {"X轴", "Axis-X"},
    {"Y轴", "Axis-Y"},
    {"当前位置", "Current Position"},
    {"目标位置", "Target Position"},
    {"确认", "Confirm"},
    {"关闭", "Close"},
    {"语言", "Language"},
    {"系统版本", "System Version"},
    {"硬件版本", "Hardware Version"},
};

static struct ml_label header = {NULL, 0, NULL};
/* lan = 0 Chinese, 1 English */
static int lan = 0;

lv_obj_t *ml_label_create(lv_obj_t *parent, int txt_idx)
{
    struct ml_label *label;

    if (txt_idx > TXT_MAX)
        return NULL;

    label = malloc(sizeof(struct ml_label));
    if (!label)
        return NULL;

    label->lv_obj = lv_label_create(parent);
    label->txt_idx = txt_idx;
    lv_label_set_text(label->lv_obj, txt[txt_idx][lan]);

    label->next = header.next;
    header.next = label;

    return label->lv_obj;
}

void ml_label_set_text(lv_obj_t *obj, int txt_idx)
{
    struct ml_label *label = NULL;
    struct ml_label *p;
    int found;

    if (txt_idx > TXT_MAX)
        return;

    if (header.next)
    {
        p = header.next;
        do
        {
            if (p->lv_obj == obj)
            {
                label = p;
                break;
            }
            p = p->next;
        }
        while (p);
    }

    if (!label)
    {
        label = malloc(sizeof(struct ml_label));
        if (!label)
            return;
        label->lv_obj = obj;
        label->next = header.next;
        header.next = label;
    }

    label->txt_idx = txt_idx;
    lv_label_set_text(label->lv_obj, txt[txt_idx][lan]);
}

void ml_label_destroy(lv_obj_t *obj)
{
    struct ml_label *prev;
    struct ml_label *p;

    if (!header.next)
        return;

    prev = &header;
    p = prev->next;
    do
    {
        if (p->lv_obj == obj)
        {
            prev->next = p->next;
            lv_obj_del(obj);
            free(p);
            break;
        }
        prev = p;
        p = prev->next;
    }
    while (p);
}

void ml_label_set_language(int language)
{
    struct ml_label *p;

    if (language == lan)
        return;

    lan = language;
    if (!header.next)
        return;

    p = header.next;
    do
    {
        if (p->lv_obj)
            lv_label_set_text(p->lv_obj, txt[p->txt_idx][lan]);
        p = p->next;
    }
    while (p);
}

