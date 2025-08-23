#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include <lvgl/lvgl.h>

#include "main.h"
#include "ui_resource.h"

static lv_obj_t *bg;

static lv_obj_t *volume_label;
static lv_obj_t *volume_value;
static lv_obj_t *volume_slider;

static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(volume_value, buf);
    lv_obj_align_to(volume_value, slider, LV_ALIGN_OUT_TOP_RIGHT, 0, 0);
}

lv_obj_t *menu_volume_init(lv_obj_t *parent)
{
    bg = lv_obj_create(parent);
    lv_obj_set_size(bg, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(bg, LV_FLEX_FLOW_COLUMN);
    lv_obj_center(bg);

    volume_label = lv_label_create(bg);
    lv_label_set_text(volume_label, "音量");
    lv_obj_add_style(volume_label, &style_txt_m, LV_PART_MAIN);

    volume_slider = lv_slider_create(bg);
    lv_obj_add_event_cb(volume_slider, slider_event_cb, LV_EVENT_VALUE_CHANGED,
                        NULL);
    lv_obj_set_width(volume_slider, lv_pct(100));
    lv_slider_set_range(volume_slider, 10, 100);
    lv_slider_set_value(volume_slider, 100, LV_ANIM_OFF);

    volume_value = lv_label_create(bg);
    lv_label_set_text(volume_value, "100%");
    lv_obj_add_style(volume_value, &style_txt_m, LV_PART_MAIN);
    lv_obj_add_flag(volume_value, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align_to(volume_value, volume_slider, LV_ALIGN_OUT_TOP_RIGHT, 0, 0);

    return bg;
}

void menu_volume_deinit(void)
{
    lv_obj_del(bg);
    bg = NULL;
}

