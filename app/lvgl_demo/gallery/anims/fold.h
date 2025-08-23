#ifndef __FOLD_H__
#define __FOLD_H__

void anim_fold_start(lv_anim_t *a);
void anim_fold(void *var, int32_t v);
void anim_fold_end(lv_anim_t *a);

#define ANIM_FOLD   {               \
    .time = 1000,                   \
    .start_value = 0,               \
    .current_value = 0,             \
    .end_value = 480,               \
    .repeat_cnt = 1,                \
    .start_cb = anim_fold_start,    \
    .path_cb = lv_anim_path_linear, \
    .exec_cb = anim_fold,           \
    .deleted_cb = anim_fold_end,    \
}

#endif

