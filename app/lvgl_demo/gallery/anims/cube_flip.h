#ifndef __CUBE_FLIP_H__
#define __CUBE_FLIP_H__

void anim_cube_flip_start(lv_anim_t *a);
void anim_cube_flip(void *var, int32_t v);
void anim_cube_flip_end(lv_anim_t *a);

#define ANIM_CUBE_FLIP   {              \
    .time = 300,                        \
    .start_value = 0,                   \
    .current_value = 0,                 \
    .end_value = 90,                    \
    .repeat_cnt = 1,                    \
    .start_cb = anim_cube_flip_start,   \
    .path_cb = lv_anim_path_ease_in_out,\
    .exec_cb = anim_cube_flip,          \
    .deleted_cb = anim_cube_flip_end,   \
}

#endif

