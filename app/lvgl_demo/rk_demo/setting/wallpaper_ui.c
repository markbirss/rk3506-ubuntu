#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include <lvgl/lvgl.h>

#include "main.h"
#include "ui_resource.h"

static lv_obj_t *bg;

static lv_obj_t *area_screen_timeout;
static lv_obj_t *area_locked_screen;
static lv_obj_t *area_wallpaper;
static int selected_id = 0;

static char *wallpapers_thumb[4] =
{
    WALLPAPER_0,
    WALLPAPER_1,
    WALLPAPER_2,
    WALLPAPER_3,
};

static char *wallpapers[4] =
{
    BG_PIC_0,
    BG_PIC_1,
    BG_PIC_2,
    BG_PIC_3,
};

static int timeout_value[] =
{
    /* 5s 60s 120s 5mins never */
    5000, 60000, 120000, 300000, -1
};

static void wallpaper_cb(lv_event_t *event)
{
    rk_demo_bg_set_img(lv_event_get_user_data(event));
}

static void screen_timeout_cb(lv_event_t *event)
{
    lv_obj_t *obj = lv_event_get_target(event);

    selected_id = lv_dropdown_get_selected(obj);
    if (selected_id > ARRAY_SIZE(timeout_value))
        selected_id = ARRAY_SIZE(timeout_value);
    backlight_set_timeout(timeout_value[selected_id]);
}

lv_obj_t *menu_wallpaper_init(lv_obj_t *parent)
{
    lv_obj_t *obj;
    lv_obj_t *area_depart;

    bg = lv_obj_create(parent);
    lv_obj_remove_style_all(bg);
    lv_obj_set_style_pad_all(bg, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(bg, 30, LV_PART_MAIN);
    lv_obj_set_size(bg, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(bg, LV_FLEX_FLOW_COLUMN);
    lv_obj_align(bg, LV_ALIGN_TOP_MID, 0, 0);

    area_screen_timeout = lv_obj_create(bg);
    lv_obj_set_size(area_screen_timeout, lv_pct(100), LV_SIZE_CONTENT);
    obj = lv_label_create(area_screen_timeout);
    lv_label_set_text(obj, "锁屏时间");
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);
    obj = lv_dropdown_create(area_screen_timeout);
    lv_obj_add_style(obj, &style_txt_s, LV_PART_MAIN);
    lv_obj_add_style(lv_dropdown_get_list(obj), &style_txt_s, LV_PART_MAIN);
    lv_obj_add_event_cb(obj, screen_timeout_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_dropdown_set_options(obj, "5s\n60s\n120s\n5mins\n永不");
    lv_dropdown_set_selected(obj, selected_id);
    lv_obj_align(obj, LV_ALIGN_RIGHT_MID, 0, 0);

    area_locked_screen = lv_obj_create(bg);
    lv_obj_set_size(area_locked_screen, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(area_locked_screen, LV_FLEX_FLOW_COLUMN);

    obj = lv_label_create(area_locked_screen);
    lv_label_set_text(obj, "锁屏");
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_TOP_LEFT, 0, 0);

    area_depart = lv_obj_create(area_locked_screen);
    lv_obj_remove_style_all(area_depart);
    lv_obj_set_size(area_depart, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(area_depart, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(area_depart, 10, 0);
    for (int i = 0; i < 4; i++)
    {
        obj = lv_img_create(area_depart);
        lv_obj_set_size(obj, 160, 120);
        lv_img_set_src(obj, wallpapers_thumb[i]);
    }

    area_wallpaper = lv_obj_create(bg);
    lv_obj_set_size(area_wallpaper, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(area_wallpaper, LV_FLEX_FLOW_COLUMN);

    obj = lv_label_create(area_wallpaper);
    lv_label_set_text(obj, "壁纸");
    lv_obj_add_style(obj, &style_txt_m, LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_TOP_LEFT, 0, 0);

    area_depart = lv_obj_create(area_wallpaper);
    lv_obj_remove_style_all(area_depart);
    lv_obj_set_size(area_depart, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(area_depart, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(area_depart, 10, 0);
    for (int i = 0; i < 4; i++)
    {
        obj = lv_img_create(area_depart);
        lv_obj_set_size(obj, 160, 120);
        lv_img_set_src(obj, wallpapers_thumb[i]);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(obj, wallpaper_cb, LV_EVENT_CLICKED, wallpapers[i]);
    }

    return bg;
}

void menu_wallpaper_deinit(void)
{
    lv_obj_del(bg);
    bg = NULL;
}

