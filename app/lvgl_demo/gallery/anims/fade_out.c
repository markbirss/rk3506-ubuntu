#include "gallery.h"

void anim_fade_out_start(lv_anim_t *a)
{
    printf("%s\n", __func__);
    common_anim_start();
    lv_obj_clear_flag(anim_area, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(img1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(img2, LV_OBJ_FLAG_HIDDEN);
}

void anim_fade_out(void *var, int32_t v)
{
    lv_obj_set_x(img2, 0);
    lv_obj_set_x(img1, 0);
    lv_obj_set_style_img_opa(img2, v, 0);
    lv_obj_set_style_img_opa(img1, LV_OPA_COVER - v, 0);
    lv_slider_set_value(slider, LV_OPA_COVER - v, LV_ANIM_ON);
}

void anim_fade_out_end(lv_anim_t *a)
{
    lv_obj_t *t;

    animing = 0;
    t = img1;
    img1 = img2;
    img2 = t;

    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 0, LV_ANIM_ON);
}

