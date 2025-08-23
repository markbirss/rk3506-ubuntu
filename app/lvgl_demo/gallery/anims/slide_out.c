#include "gallery.h"

void anim_slide_out_start(lv_anim_t *a)
{
    printf("%s\n", __func__);
    common_anim_start();
    lv_obj_clear_flag(anim_area, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(img1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(img2, LV_OBJ_FLAG_HIDDEN);
}

void anim_slide_out(void *var, int32_t v)
{
    lv_obj_set_x(img2, v - 480);
    lv_obj_set_x(img1, v);
    lv_obj_set_style_img_opa(img2, LV_OPA_COVER, 0);
    lv_obj_set_style_img_opa(img1, LV_OPA_COVER, 0);
    lv_slider_set_value(slider, 480 - v, LV_ANIM_ON);
}

void anim_slide_out_end(lv_anim_t *a)
{
    lv_obj_t *t;

    animing = 0;
    t = img1;
    img1 = img2;
    img2 = t;

    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 0, LV_ANIM_ON);
    lv_obj_add_flag(img1, LV_OBJ_FLAG_HIDDEN);
}

