#include "lvgl/lvgl.h"

#include "main.h"
#include "smart_home_ui.h"

static int idx = 0;
void app_switch_init(lv_obj_t *parent, void *userdata)
{
    lv_obj_t *cont, *label, *obj;

    cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_center(cont);

    label = lv_label_create(cont);
    lv_obj_add_style(label, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);

    obj = lv_switch_create(cont);

    if (userdata)
    {
        lv_label_set_text(label, userdata);
        /* FIXME, random or specified state */
        lv_obj_add_state(obj, (((uint8_t *)userdata)[0] & 0x4) ?
                         LV_STATE_CHECKED : LV_STATE_DEFAULT);
    }
    else
    {
        lv_label_set_text_fmt(label, "开关 %d", idx++);
        lv_obj_add_state(obj, (idx % 3 == 0));
    }
}

