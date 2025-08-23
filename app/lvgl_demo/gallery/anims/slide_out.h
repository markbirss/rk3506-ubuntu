#ifndef __SLIDE_OUT_H__
#define __SLIDE_OUT_H__

void anim_slide_out_start(lv_anim_t *a);
void anim_slide_out(void *var, int32_t v);
void anim_slide_out_end(lv_anim_t *a);

#define ANIM_SLIDE_OUT   {              \
    .time = 1000,                       \
    .start_value = 480,                 \
    .current_value = 480,               \
    .end_value = 0,                     \
    .repeat_cnt = 1,                    \
    .start_cb = anim_slide_out_start,   \
    .path_cb = lv_anim_path_linear,     \
    .exec_cb = anim_slide_out,          \
    .deleted_cb = anim_slide_out_end,   \
}

#endif

