#ifndef __FADE_OUT_H__
#define __FADE_OUT_H__

void anim_fade_out_start(lv_anim_t *a);
void anim_fade_out(void *var, int32_t v);
void anim_fade_out_end(lv_anim_t *a);

#define ANIM_FADE_OUT   {           \
    .time = 1000,                   \
    .start_value = LV_OPA_COVER,    \
    .current_value = LV_OPA_COVER,  \
    .end_value = LV_OPA_TRANSP,     \
    .repeat_cnt = 1,                \
    .start_cb = anim_fade_out_start,\
    .path_cb = lv_anim_path_linear, \
    .exec_cb = anim_fade_out,       \
    .deleted_cb = anim_fade_out_end,\
}

#endif

