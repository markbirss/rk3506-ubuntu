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

static lv_obj_t *area_language;
static lv_obj_t *area_date;
static lv_obj_t *area_time;

static lv_obj_t *label_date;
static lv_obj_t *label_time;
static lv_obj_t *mask;
static lv_obj_t *calendar;
static lv_obj_t *time_roller[3];
static lv_obj_t *btn_confirm;

static void confirm_cb(lv_event_t *e)
{
    int hour, min, sec;
    char buf[10];

    lv_roller_get_selected_str(time_roller[0], buf, sizeof(buf));
    hour = atoi(buf);
    lv_roller_get_selected_str(time_roller[1], buf, sizeof(buf));
    min = atoi(buf);
    lv_roller_get_selected_str(time_roller[2], buf, sizeof(buf));
    sec = atoi(buf);

    lv_label_set_text_fmt(label_time, "%02d:%02d",
                          hour, min);
    // TODO set system time
    lv_obj_del(mask);
}

static void calendar_cb(lv_event_t *e)
{
    lv_calendar_date_t date;

    if (lv_calendar_get_pressed_date(calendar, &date))
    {
        lv_label_set_text_fmt(label_date, "%04d-%02d-%02d",
                              date.year, date.month, date.day);
        lv_obj_del(mask);
    }
}

static void show_calendar(lv_event_t *e)
{
    mask = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(mask);
    lv_obj_set_size(mask, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(mask, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(mask, 150, LV_PART_MAIN);

    calendar = lv_calendar_create(mask);
    lv_obj_set_size(calendar, lv_pct(80), lv_pct(60));
    lv_obj_add_event_cb(calendar, calendar_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(calendar);

    // TODO real date
    lv_calendar_set_today_date(calendar, 2023, 7, 12);
    lv_calendar_set_showed_date(calendar, 2023, 7);
    lv_calendar_header_arrow_create(calendar);
}

static void roller_drawed(lv_event_t *e)
{
    lv_obj_align_to(btn_confirm, time_roller[1], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

static void show_time_setting(lv_event_t *e)
{
    const char *opts[2] =
    {
        "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n"
        "10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n"
        "20\n21\n22\n23",
        "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n"
        "10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n"
        "20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n"
        "30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n"
        "40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n"
        "50\n51\n52\n53\n55\n55\n56\n57\n58\n59\n"
    };
    lv_obj_t *obj;

    mask = lv_obj_create(lv_layer_top());
    lv_obj_remove_style_all(mask);
    lv_obj_set_size(mask, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(mask, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(mask, 150, LV_PART_MAIN);

    time_roller[1] = lv_roller_create(mask);
    lv_roller_set_options(time_roller[1], opts[1], LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(time_roller[1], 3);
    lv_obj_align(time_roller[1], LV_ALIGN_CENTER, 0, 0);
    time_roller[0] = lv_roller_create(mask);
    lv_roller_set_options(time_roller[0], opts[0], LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(time_roller[0], 3);
    lv_obj_align_to(time_roller[0], time_roller[1], LV_ALIGN_OUT_LEFT_MID, -10, 0);
    time_roller[2] = lv_roller_create(mask);
    lv_roller_set_options(time_roller[2], opts[1], LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(time_roller[2], 3);
    lv_obj_align_to(time_roller[2], time_roller[1], LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_add_event_cb(time_roller[1], roller_drawed, LV_EVENT_DRAW_POST_END,
                        NULL);

    // TODO real time
    lv_roller_set_selected(time_roller[0], 15, LV_ANIM_OFF);
    lv_roller_set_selected(time_roller[1], 25, LV_ANIM_OFF);
    lv_roller_set_selected(time_roller[2], 30, LV_ANIM_OFF);

    btn_confirm = lv_btn_create(mask);
    lv_obj_align_to(btn_confirm, time_roller[1], LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_add_event_cb(btn_confirm, confirm_cb, LV_EVENT_CLICKED, NULL);
    obj = lv_label_create(btn_confirm);
    lv_obj_center(obj);
    lv_label_set_text(obj, "确认");
    lv_obj_add_style(obj, &style_txt_s, LV_PART_MAIN);
}

lv_obj_t *menu_language_init(lv_obj_t *parent)
{
    lv_obj_t *obj;

    bg = lv_obj_create(parent);
    lv_obj_remove_style_all(bg);
    lv_obj_set_style_pad_all(bg, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(bg, 30, LV_PART_MAIN);
    lv_obj_set_size(bg, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(bg, LV_FLEX_FLOW_COLUMN);
    lv_obj_align(bg, LV_ALIGN_TOP_MID, 0, 0);

    area_language = lv_obj_create(bg);
    lv_obj_remove_style_all(area_language);
    lv_obj_set_size(area_language, lv_pct(100), LV_SIZE_CONTENT);
    obj = lv_label_create(area_language);
    lv_label_set_text(obj, "语言");
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);
    obj = lv_dropdown_create(area_language);
    lv_obj_add_style(obj, &style_txt_s, LV_PART_MAIN);
    lv_obj_add_style(lv_dropdown_get_list(obj), &style_txt_s, LV_PART_MAIN);
    lv_dropdown_set_options(obj, "中文\nEnglish");
    lv_obj_align(obj, LV_ALIGN_RIGHT_MID, 0, 0);

    area_date = lv_obj_create(bg);
    lv_obj_remove_style_all(area_date);
    lv_obj_set_size(area_date, lv_pct(100), LV_SIZE_CONTENT);
    obj = lv_label_create(area_date);
    lv_label_set_text(obj, "日期");
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);
    label_date = lv_label_create(area_date);
    // TODO real date
    lv_label_set_text(label_date, "2023-07-12");
    lv_obj_add_flag(label_date, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_style(label_date, &style_txt_m, LV_PART_MAIN);
    lv_obj_align(label_date, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(label_date, show_calendar, LV_EVENT_CLICKED, NULL);

    area_time = lv_obj_create(bg);
    lv_obj_remove_style_all(area_time);
    lv_obj_set_size(area_time, lv_pct(100), LV_SIZE_CONTENT);
    obj = lv_label_create(area_time);
    lv_label_set_text(obj, "时间");
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);
    label_time = lv_label_create(area_time);
    // TODO real time
    lv_label_set_text(label_time, "15:25");
    lv_obj_add_flag(label_time, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_style(label_time, &style_txt_m, LV_PART_MAIN);
    lv_obj_align(label_time, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(label_time, show_time_setting, LV_EVENT_CLICKED, NULL);

    return bg;
}

void menu_language_deinit(void)
{
    lv_obj_del(bg);
    bg = NULL;
}

