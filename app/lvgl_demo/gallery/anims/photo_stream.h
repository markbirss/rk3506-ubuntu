#ifndef __PHOTO_STREAM_H__
#define __PHOTO_STREAM_H__

void anim_photo_stream_start(lv_anim_t *a);
void anim_photo_stream(void *var, int32_t v);
void anim_photo_stream_end(lv_anim_t *a);

#define ANIM_PHOTO_STREAM   {               \
    .time = 1000,                           \
    .start_value = 0,                       \
    .current_value = 0,                     \
    .end_value = 100,                       \
    .repeat_cnt = 1,                        \
    .start_cb = anim_photo_stream_start,    \
    .path_cb = lv_anim_path_linear,         \
    .exec_cb = anim_photo_stream,           \
    .deleted_cb = anim_photo_stream_end,    \
}

void anim_photo_stream_stop(void);

#endif

