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

static int idx = 0;
void app_aircond_init(lv_obj_t *parent, void *userdata)
{
    char *btn_name[] =
    {
        "开启/关闭",
        "制冷模式",
        "制热模式",
        "风力：3"
    };
    lv_align_t align[] =
    {
        LV_ALIGN_TOP_LEFT,
        LV_ALIGN_TOP_RIGHT,
        LV_ALIGN_BOTTOM_LEFT,
        LV_ALIGN_BOTTOM_RIGHT,
    };
    lv_obj_t *area_aircond;
    lv_obj_t *area_temp;
    lv_obj_t *area_btn;
    lv_obj_t *area_set;
    lv_obj_t *obj;
    lv_obj_t *temp;

    area_aircond = lv_obj_create(parent);
    lv_obj_remove_style_all(area_aircond);
    lv_obj_set_size(area_aircond, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(area_aircond, 10, LV_PART_MAIN);
    lv_obj_center(area_aircond);

    obj = lv_label_create(area_aircond);
    lv_obj_align(obj, LV_ALIGN_TOP_LEFT, 0, 0);
    if (userdata)
        lv_label_set_text(obj, userdata);
    else
        lv_label_set_text_fmt(obj, "空调 %d", idx++);
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_white(), LV_PART_MAIN);

    area_temp = lv_obj_create(area_aircond);
    lv_obj_remove_style_all(area_temp);
    lv_obj_set_size(area_temp, lv_pct(100), lv_pct(50));
    lv_obj_align(area_temp, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_clear_flag(area_temp, LV_OBJ_FLAG_SCROLLABLE);

    area_btn = lv_obj_create(area_aircond);
    lv_obj_remove_style_all(area_btn);
    lv_obj_set_size(area_btn, lv_pct(50), lv_pct(50));
    lv_obj_align(area_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_clear_flag(area_btn, LV_OBJ_FLAG_SCROLLABLE);

    area_set = lv_obj_create(area_aircond);
    lv_obj_remove_style_all(area_set);
    lv_obj_set_size(area_set, lv_pct(50), lv_pct(50));
    lv_obj_align(area_set, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_clear_flag(area_set, LV_OBJ_FLAG_SCROLLABLE);

    temp = lv_label_create(area_temp);
    lv_obj_align(temp, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_label_set_text(temp, "24℃");
    lv_obj_add_style(temp, &style_txt_l, LV_PART_MAIN);
    lv_obj_set_style_text_color(temp, lv_color_white(), LV_PART_MAIN);
    lv_obj_refr_size(temp);
    lv_obj_refr_pos(temp);

    obj = lv_label_create(area_temp);
    lv_label_set_text(obj, "当前室温");
    lv_obj_add_style(obj, &style_txt_s, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_white(), LV_PART_MAIN);
    lv_obj_refr_size(obj);
    lv_obj_refr_pos(obj);
    lv_obj_align_to(obj, temp, LV_ALIGN_OUT_LEFT_TOP, 0, 0);

    for (int i = 0; i < 4; i++)
    {
        obj = lv_btn_create(area_btn);
        lv_obj_align(obj, align[i], 0, 0);
        lv_obj_set_size(obj, lv_pct(45), lv_pct(45));
        lv_obj_set_style_radius(obj, lv_pct(50), LV_PART_MAIN);

        obj = lv_label_create(obj);
        lv_label_set_text(obj, btn_name[i]);
        lv_obj_add_style(obj, &style_txt_s, LV_PART_MAIN);
        lv_obj_set_style_text_color(obj, lv_color_white(), LV_PART_MAIN);
        lv_obj_center(obj);
    }

    temp = lv_roller_create(area_set);
    lv_roller_set_options(temp, "16\n17\n18\n19\n20\n21\n22\n23\n"
                          "24\n25\n26\n27\n28\n29\n30\n31\n32",
                          LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(temp, 3);
    lv_roller_set_selected(temp, 8, LV_ANIM_OFF);
    lv_obj_align(temp, LV_ALIGN_CENTER, 0, 0);

    obj = lv_label_create(area_set);
    lv_label_set_text(obj, "设置温度");
    lv_obj_add_style(obj, &style_txt_s, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_white(), LV_PART_MAIN);
    lv_obj_align_to(obj, temp, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}

