#include "gallery.h"

#ifdef USE_OPENGL
void anim_roller_start(lv_anim_t *a);
void anim_roller(void *var, int32_t v);
void anim_roller_end(lv_anim_t *a);

void anim_roller_render(void)
{
    for (int i = 0; i < 6; i++)
        lv_gl_obj_render(obj_roller_items[i]);
}

void anim_roller_start(lv_anim_t *a)
{
    SDL_Rect v;

    printf("%s\n", __func__);
    common_anim_start();

    v.w = 2000;
    v.h = 2000;
    v.x = (view.w - v.w) / 2 + (screen.w - view.w);
    v.y = (view.h - v.h) / 2 + (screen.h - view.h);
    //lv_gl_obj_set_viewport(obj_roller, &v);
    lv_gl_obj_set_viewport(NULL, &v);

    for (int i = 0; i < 6; i++)
    {
        lv_gl_obj_resize(obj_roller_items[i], NULL);

        /* sqrt(240^2 - 120^2) / 2000 * 2.0 = 0.20785 */
        obj_roller_items[i]->offset.z = 0.20785;
        obj_roller_items[i]->view_rot.x = 30;
        obj_roller_items[i]->view_rot.z = 15;
    }
    lv_gl_set_render_cb(anim_roller_render);
}

void anim_roller(void *var, int32_t v)
{
    int index = (intptr_t)var;
    lv_slider_set_value(slider, v, LV_ANIM_ON);

    for (int i = 0; i < 6; i++)
    {
        obj_roller_items[i]->self_rot.y = (360.0 / 6.0) * i + (float)v;
        if (obj_roller_items[i]->self_rot.y > 360.0)
            obj_roller_items[i]->self_rot.y -= 360.0;
    }

    lv_obj_invalidate(lv_layer_top());
}

void anim_roller_end(lv_anim_t *a)
{
    animing = 0;
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 0, LV_ANIM_ON);
}
#endif

