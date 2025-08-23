#include "gallery.h"

#ifdef USE_OPENGL
void anim_stiker_start(lv_anim_t *a);
void anim_stiker(void *var, int32_t v);
void anim_stiker_end(lv_anim_t *a);

static void anim_stiker_render(void)
{
    lv_gl_set_fb(obj_fb);
    lv_gl_tex_clear(obj_fb->base.tex, 0.0, 0.0, 0.0, 1.0);
    lv_gl_obj_render(obj_img1);
    lv_gl_obj_render(obj_img0);
    /* render to screen */
    lv_gl_set_fb(NULL);
    lv_gl_obj_render(obj_fb);
}

void anim_stiker_start(lv_anim_t *a)
{
    printf("%s\n", __func__);
    common_anim_start();

    lv_gl_set_render_cb(anim_stiker_render);

    obj_img0->base.r.x = 0;
    obj_img0->base.r.y = 0;
    lv_gl_obj_move(obj_img0, obj_fb);

    lv_obj_invalidate(lv_layer_top());
}

void anim_stiker(void *var, int32_t v)
{
    lv_slider_set_value(slider, v, LV_ANIM_ON);

    obj_img0->base.r.x = obj_img0->base.w * (v % 4);
    obj_img0->base.r.y = obj_img0->base.h * (v / 4);
    lv_gl_obj_move(obj_img0, obj_fb);

    lv_obj_invalidate(lv_layer_top());
}

void anim_stiker_end(lv_anim_t *a)
{
    animing = 0;
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 0, LV_ANIM_ON);
}
#endif

