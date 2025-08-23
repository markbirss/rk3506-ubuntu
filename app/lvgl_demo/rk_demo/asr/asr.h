#ifndef __ASR_H__
#define __ASR_H__

#include <lvgl/lvgl.h>

void asr_update(int id);
void printf_asr_cmds(void);

void asr_icon_create(lv_obj_t *parent);
void asr_icon_destroy(void);

void asr_audio_init(void);
void asr_audio_deinit(void);

#endif

