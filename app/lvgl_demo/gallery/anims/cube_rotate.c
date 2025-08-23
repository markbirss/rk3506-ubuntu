#include "gallery.h"

#ifdef USE_OPENGL
void anim_cube_rotate_start(lv_anim_t *a)
{
    printf("%s\n", __func__);
    anim_cube_start(a);
}

void anim_cube_rotate(void *var, int32_t v)
{
    lv_slider_set_value(slider, v, LV_ANIM_ON);

    obj_cube->scale.x = 0.57;
    obj_cube->scale.y = obj_cube->scale.x;
    obj_cube->scale.z = obj_cube->scale.x;
    lv_gl_obj_set_angle(obj_cube, v, v, 0);
    lv_obj_invalidate(lv_layer_top());
}

void anim_cube_rotate_end(lv_anim_t *a)
{
    animing = 0;
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 0, LV_ANIM_ON);
}
#endif

