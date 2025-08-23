#include "gallery.h"

void anim_fold_start(lv_anim_t *a);
void anim_fold(void *var, int32_t v);
void anim_fold_end(lv_anim_t *a);

#ifdef USE_OPENGL
static void anim_fold_render(void)
{
    /* render to screen */
    lv_gl_set_fb(NULL);
    lv_gl_obj_render(obj_fold[0]);
    lv_gl_obj_render(obj_fold[1]);
    lv_gl_obj_render(obj_fold[2]);
    lv_gl_obj_render(obj_fold[3]);
}

void anim_fold_start(lv_anim_t *a)
{
    SDL_Rect fold_view;

    printf("%s\n", __func__);
    common_anim_start();

//    fold_view.w = 480;
//    fold_view.h = 480;
//    fold_view.x = (view.w - fold_view.w) / 2;
//    fold_view.y = view.y + (view.h - fold_view.h) / 2;
//    printf("fold_view %d %d %d %d\n", fold_view.x, fold_view.y,
//        fold_view.w, fold_view.h);
//    lv_gl_obj_set_viewport(NULL, &fold_view);

    lv_gl_obj_resize(obj_fold[0], NULL);
    lv_gl_obj_reset_points(obj_fold[0]);
    lv_gl_obj_reset_tex_points(obj_fold[0]);
    obj_fold[0]->tp.w /= 2;
    lv_gl_obj_update_vao(obj_fold[0]);

    lv_gl_obj_resize(obj_fold[1], NULL);
    lv_gl_obj_reset_points(obj_fold[1]);
    lv_gl_obj_reset_tex_points(obj_fold[1]);
    obj_fold[1]->tp.w /= 2;
    obj_fold[1]->tp.x = obj_fold[1]->tp.w;
    lv_gl_obj_update_vao(obj_fold[1]);

    lv_gl_obj_resize(obj_fold[2], NULL);
    lv_gl_obj_reset_points(obj_fold[2]);
    lv_gl_obj_reset_tex_points(obj_fold[2]);
    obj_fold[2]->tp.w /= 2;
    lv_gl_obj_update_vao(obj_fold[2]);

    lv_gl_obj_resize(obj_fold[3], NULL);
    lv_gl_obj_reset_points(obj_fold[3]);
    lv_gl_obj_reset_tex_points(obj_fold[3]);
    obj_fold[3]->tp.w /= 2;
    obj_fold[3]->tp.x = obj_fold[3]->tp.w;
    lv_gl_obj_update_vao(obj_fold[3]);

    lv_gl_set_render_cb(anim_fold_render);
}

void anim_fold(void *var, int32_t v)
{
    int index = (intptr_t)var;
    float startx;
    float position;

    lv_slider_set_value(slider, v, LV_ANIM_ON);

    /* max value is 480, see end_value in fold.h */
    position = (float)v / 480;

    /* left part of old picture */

    /*
     * p0 is the bottom left point
     * p2 is the top left point
     * x gose from -1.0(far left) to 0.0(middle)
     */
    obj_fold[0]->p[0].x = (-1.0 + position) / obj_fold[0]->scale.x;
    obj_fold[0]->p[2].x = (-1.0 + position) / obj_fold[0]->scale.x;

    /*
     * p1 is the bottom right point
     * p3 is the top right point
     * x is always 0.0(middle)
     * z gose from 0.0 to 0.2
     */
    obj_fold[0]->p[1].x = 0.0;
    obj_fold[0]->p[1].z = -(position * 0.2);
    obj_fold[0]->p[3].x = 0.0;
    obj_fold[0]->p[3].z = -(position * 0.2);
    lv_gl_obj_update_vao(obj_fold[0]);

    /* right part of old picture */

    /*
     * p1 is the bottom right point
     * p3 is the top right point
     * x gose from 1.0(far right) to 0.0(middle)
     */
    obj_fold[1]->p[1].x = (1.0 - position) / obj_fold[1]->scale.x;
    obj_fold[1]->p[3].x = (1.0 - position) / obj_fold[1]->scale.x;

    /*
     * p0 is the bottom left point
     * p2 is the top left point
     * x is always 0.0(middle)
     * z gose from 0.0 to 0.2
     */
    obj_fold[1]->p[0].x = 0.0;
    obj_fold[1]->p[0].z = -(position * 0.2);
    obj_fold[1]->p[2].x = 0.0;
    obj_fold[1]->p[2].z = -(position * 0.2);
    lv_gl_obj_update_vao(obj_fold[1]);

    /*
     * left part of new picture
     * x gose from -1.5(far left minus its width) to -0.5(middle minus its width)
     */
    obj_fold[2]->move.x = -1.5 + (position * 1.0);
    /*
     * right part of new picture
     * x gose from 1.5(far right plus its width) to 0.5(middle plus its width)
     */
    obj_fold[3]->move.x =  1.5 - (position * 1.0);

    lv_obj_invalidate(lv_layer_top());
}

void anim_fold_end(lv_anim_t *a)
{
    animing = 0;
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 0, LV_ANIM_ON);
}
#endif

