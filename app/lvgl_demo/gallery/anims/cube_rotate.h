#ifndef __CUBE_ROTATE_H__
#define __CUBE_ROTATE_H__

void anim_cube_rotate_start(lv_anim_t *a);
void anim_cube_rotate(void *var, int32_t v);
void anim_cube_rotate_end(lv_anim_t *a);

#define ANIM_CUBE_ROTATE   {            \
    .time = 3000,                       \
    .start_value = 0,                   \
    .current_value = 0,                 \
    .end_value = 360,                   \
    .repeat_cnt = 1,                    \
    .start_cb = anim_cube_rotate_start, \
    .path_cb = lv_anim_path_linear,     \
    .exec_cb = anim_cube_rotate,        \
    .deleted_cb = anim_cube_rotate_end, \
}

#endif

