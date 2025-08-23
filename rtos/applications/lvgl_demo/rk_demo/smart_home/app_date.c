/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include <rtthread.h>
#include "lvgl.h"

#include "main.h"
#include "smart_home_ui.h"

void app_date_init(lv_obj_t *parent, void *userdata)
{
    lv_obj_t *obj, *label;

    obj = lv_label_create(parent);
    lv_label_set_text(obj, "10:32");
    lv_obj_add_style(obj, &style_txt_l, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, -10);

    label = lv_label_create(parent);
    lv_label_set_text(label, "7月13日 周四");
    lv_obj_add_style(label, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_align_to(label, obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}

