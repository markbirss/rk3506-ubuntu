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

void app_scene_init(lv_obj_t *parent, void *userdata)
{
    lv_obj_t *obj;
    lv_coord_t size;

    lv_obj_refr_size(parent);
    size = lv_obj_get_content_width(parent) / 2;

    obj = lv_btn_create(parent);
    lv_obj_align(obj, LV_ALIGN_CENTER, -size * 2 / 3, 0);
    lv_obj_set_size(obj, size, size);
    lv_obj_set_style_radius(obj, size, LV_PART_MAIN);

    obj = lv_label_create(obj);
    lv_label_set_text(obj, "回家");
    lv_obj_add_style(obj, &style_txt_s, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(obj);

    obj = lv_btn_create(parent);
    lv_obj_align(obj, LV_ALIGN_CENTER, size * 2 / 3, 0);
    lv_obj_set_size(obj, size, size);
    lv_obj_set_style_radius(obj, size, LV_PART_MAIN);

    obj = lv_label_create(obj);
    lv_label_set_text(obj, "外出");
    lv_obj_add_style(obj, &style_txt_s, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(obj);
}

