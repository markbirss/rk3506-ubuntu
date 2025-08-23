#ifndef __CUBE_LYRIC_H__
#define __CUBE_LYRIC_H__

void anim_cube_lyric_start(lv_anim_t *a);
void anim_cube_lyric(void *var, int32_t v);
void anim_cube_lyric_end(lv_anim_t *a);

#define ANIM_CUBE_LYRIC   {             \
    .time = 600,                        \
    .start_value = 0,                   \
    .current_value = 0,                 \
    .end_value = 100,                   \
    .repeat_cnt = 1,                    \
    .start_cb = anim_cube_lyric_start,  \
    .path_cb = lv_anim_path_linear,     \
    .exec_cb = anim_cube_lyric,         \
    .deleted_cb = anim_cube_lyric_end,  \
}

#endif

