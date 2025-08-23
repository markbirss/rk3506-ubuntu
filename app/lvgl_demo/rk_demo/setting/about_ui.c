#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include <lvgl/lvgl.h>

#include "main.h"
#include "ui_resource.h"

static lv_obj_t *bg;

static lv_obj_t *area_tag;
static lv_obj_t *area_info;

static lv_obj_t *cont_update;
static lv_obj_t *btn_update_local;
static lv_obj_t *btn_update_net;

static void update_event_cb(lv_event_t *e)
{
    if (lv_obj_has_flag(cont_update, LV_OBJ_FLAG_HIDDEN))
        lv_obj_clear_flag(cont_update, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(cont_update, LV_OBJ_FLAG_HIDDEN);
    lv_obj_align_to(cont_update, area_info, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
}

lv_obj_t *menu_about_init(lv_obj_t *parent)
{
    lv_obj_t *obj;

    bg = lv_obj_create(parent);
    lv_obj_remove_style_all(bg);
    lv_obj_set_style_pad_all(bg, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(bg, 30, LV_PART_MAIN);
    lv_obj_set_size(bg, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(bg, LV_FLEX_FLOW_ROW);
    lv_obj_clear_flag(bg, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(bg, LV_ALIGN_TOP_MID, 0, 0);

    area_tag = lv_obj_create(bg);
    lv_obj_remove_style_all(area_tag);
    lv_obj_set_size(area_tag, lv_pct(40), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(area_tag, LV_FLEX_FLOW_COLUMN);

    area_info = lv_obj_create(bg);
    lv_obj_remove_style_all(area_info);
    lv_obj_set_size(area_info, lv_pct(50), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(area_info, LV_FLEX_FLOW_COLUMN);

    obj = lv_label_create(area_tag);
    lv_label_set_text(obj, "系统版本");
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);

    obj = lv_label_create(area_tag);
    lv_label_set_text(obj, "软件版本");
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);

    obj = lv_label_create(area_tag);
    lv_label_set_text(obj, "系统升级");
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);

    obj = lv_label_create(area_info);
    lv_label_set_text(obj, "0.1.0-20230712");
    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);

    obj = lv_label_create(area_info);
    lv_label_set_text(obj, "0.1.0-20230712");
    lv_label_set_long_mode(obj, LV_LABEL_LONG_SCROLL);
    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);

    obj = lv_label_create(area_info);
    lv_obj_set_width(obj, lv_pct(100));
    lv_label_set_text(obj, "升级<");
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
    lv_obj_add_event_cb(obj, update_event_cb, LV_EVENT_CLICKED, NULL);

    cont_update = lv_obj_create(bg);
    lv_obj_add_flag(cont_update, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_set_size(cont_update, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align_to(cont_update, area_info, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_flex_flow(cont_update, LV_FLEX_FLOW_ROW);
    btn_update_local = lv_btn_create(cont_update);
    obj = lv_label_create(btn_update_local);
    lv_label_set_text(obj, "本地升级");
    lv_obj_add_style(obj, &style_txt_s, LV_PART_MAIN);
    btn_update_net = lv_btn_create(cont_update);
    obj = lv_label_create(btn_update_net);
    lv_label_set_text(obj, "网络升级");
    lv_obj_add_style(obj, &style_txt_s, LV_PART_MAIN);
    lv_obj_add_flag(cont_update, LV_OBJ_FLAG_HIDDEN);

    return bg;
}

void menu_about_deinit(void)
{
    lv_obj_del(bg);
    bg = NULL;
}

