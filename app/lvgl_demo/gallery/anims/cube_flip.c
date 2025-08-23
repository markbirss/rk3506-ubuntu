#include "gallery.h"

#ifdef USE_OPENGL
void anim_cube_flip_start(lv_anim_t *a);
void anim_cube_flip(void *var, int32_t v);
void anim_cube_flip_end(lv_anim_t *a);

static lv_anim_t sub_anims[] =
{
    {
        .time = 300,
        .act_time = -1000,
        .start_value = 90,
        .current_value = 90,
        .end_value = 180,
        .repeat_cnt = 1,
        .path_cb = lv_anim_path_ease_in_out,
        .exec_cb = anim_cube_flip,
        .deleted_cb = anim_cube_flip_end,
        .var = (void *)90,
    },
    {
        .time = 300,
        .act_time = -1000,
        .start_value = 180,
        .current_value = 180,
        .end_value = 270,
        .repeat_cnt = 1,
        .path_cb = lv_anim_path_ease_in_out,
        .exec_cb = anim_cube_flip,
        .deleted_cb = anim_cube_flip_end,
        .var = (void *)180,
    },
    {
        .time = 300,
        .act_time = -1000,
        .start_value = 270,
        .current_value = 270,
        .end_value = 360,
        .repeat_cnt = 1,
        .path_cb = lv_anim_path_ease_in_out,
        .exec_cb = anim_cube_flip,
        .deleted_cb = anim_cube_flip_end,
        .var = (void *)270,
    }
};

SDL_Rect cube_view;

#define ZOOM_IN_SIZE        40
#define ZOOM_IN     do { \
    if (pos < ZOOM_IN_SIZE) \
    { \
        obj_cube->base.r.w = cube_view.w * 0.57 - pos; \
    } \
    else if (pos > (90 - ZOOM_IN_SIZE)) \
    { \
        obj_cube->base.r.w = cube_view.w * 0.57 - (90 - pos); \
    } \
    else \
    { \
        obj_cube->base.r.w = cube_view.w * 0.57 - ZOOM_IN_SIZE; \
    } \
    obj_cube->scale.x = (float)obj_cube->base.r.w / cube_view.w; \
    obj_cube->scale.y = obj_cube->scale.x; \
    obj_cube->scale.z = obj_cube->scale.x; \
} while (0);

void anim_cube_render(void)
{
    /* render to screen */
    lv_gl_set_fb(NULL);
    lv_gl_obj_render(obj_cube);
}

void anim_cube_start(lv_anim_t *a)
{
    int side;

    common_anim_start();
    lv_gl_obj_set_view_angle(obj_cube, 0, 0, 0);
    lv_gl_obj_set_angle(obj_cube, 0, 0, 0);

    /*
     * the distance from the center of the cube to any corner is
     * sqrt(3) / 2 * a, the a is the edge length, and a = 2.0
     * So in order to ensure that the cube will not exceed
     * the coordinate range no matter how it is rotated,
     * let's limit it to 0.57a, 0.57 * 2.0 * sqrt(3) / 2 = 0.9873
     */
    if (view.w > view.h)
        side = view.h / 0.57 + 2;
    else
        side = view.w / 0.57 + 2;
    cube_view.x = (view.w - side) / 2;
    cube_view.y = (side - view.h) / 2;
    cube_view.w = side;
    cube_view.h = side;
    printf("%s %d %d %d %d\n", __func__,
        cube_view.x, cube_view.y, cube_view.w, cube_view.h);

    lv_gl_obj_set_viewport(NULL, &cube_view);
    lv_gl_set_render_cb(anim_cube_render);
}

void anim_cube_flip_start(lv_anim_t *a)
{
    printf("%s\n", __func__);
    anim_cube_start(a);
}

void anim_cube_flip(void *var, int32_t v)
{
    int pos;
    lv_slider_set_value(slider, v, LV_ANIM_ON);

    pos = v - (intptr_t)var;
    ZOOM_IN;
    lv_gl_obj_set_angle(obj_cube, 0, v, 0);
    lv_obj_invalidate(lv_layer_top());
}

void anim_cube_flip_end(lv_anim_t *a)
{
    int var = (intptr_t)a->var;

    if (var == 0)
        lv_anim_start(&sub_anims[0]);
    if (var == 90)
        lv_anim_start(&sub_anims[1]);
    if (var == 180)
        lv_anim_start(&sub_anims[2]);
    if (var == 270)
    {
        animing = 0;
        lv_slider_set_range(slider, 0, 100);
        lv_slider_set_value(slider, 0, LV_ANIM_ON);
    }
}
#endif

