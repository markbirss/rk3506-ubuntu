#ifndef __STIKER_H__
#define __STIKER_H__

void anim_stiker_start(lv_anim_t *a);
void anim_stiker(void *var, int32_t v);
void anim_stiker_end(lv_anim_t *a);

#define ANIM_STIKER   {            \
    .time = 1600,                  \
    .start_value = 0,              \
    .current_value = 0,            \
    .end_value = 15,               \
    .repeat_cnt = 1,               \
    .start_cb = anim_stiker_start, \
    .path_cb = lv_anim_path_linear,\
    .exec_cb = anim_stiker,        \
    .deleted_cb = anim_stiker_end, \
}

#endif

