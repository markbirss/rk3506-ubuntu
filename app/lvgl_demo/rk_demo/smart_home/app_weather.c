#include "lvgl/lvgl.h"

#include "main.h"
#include "smart_home_ui.h"

void app_weather_init(lv_obj_t *parent, void *userdata)
{
    lv_obj_t *obj, *label;

    obj = lv_label_create(parent);
    lv_label_set_text(obj, "32℃");
    lv_obj_add_style(obj, &style_txt_l, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, -10);

    label = lv_label_create(parent);
    lv_label_set_text(label, "福州   晴");
    lv_obj_add_style(label, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_align_to(label, obj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
}

