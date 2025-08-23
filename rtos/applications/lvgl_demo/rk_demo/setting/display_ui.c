/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include <rtthread.h>

#include "lvgl.h"

#include "main.h"
#include "ui_resource.h"

static lv_obj_t *bg;

static lv_obj_t *bright_label;
static lv_obj_t *bright_value;
static lv_obj_t *bright_slider;

static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(bright_value, buf);
    lv_obj_align_to(bright_value, slider, LV_ALIGN_OUT_TOP_RIGHT, 0, 0);
}

lv_obj_t *menu_display_init(lv_obj_t *parent)
{
    bg = lv_obj_create(parent);
    lv_obj_set_size(bg, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(bg, LV_FLEX_FLOW_COLUMN);
    lv_obj_center(bg);

    bright_label = lv_label_create(bg);
    lv_label_set_text(bright_label, "亮度");
    lv_obj_add_style(bright_label, &style_txt_m, LV_PART_MAIN);

    bright_slider = lv_slider_create(bg);
    lv_obj_add_event_cb(bright_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED,
                        NULL);
    lv_obj_set_width(bright_slider, lv_pct(100));
    lv_slider_set_range(bright_slider, 10, 100);
    lv_slider_set_value(bright_slider, 100, LV_ANIM_OFF);

    bright_value = lv_label_create(bg);
    lv_label_set_text(bright_value, "100%");
    lv_obj_add_style(bright_value, &style_txt_m, LV_PART_MAIN);
    lv_obj_add_flag(bright_value, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align_to(bright_value, bright_slider, LV_ALIGN_OUT_TOP_RIGHT, 0, 0);

    return bg;
}

void menu_display_deinit(void)
{
    lv_obj_del(bg);
    bg = NULL;
}

