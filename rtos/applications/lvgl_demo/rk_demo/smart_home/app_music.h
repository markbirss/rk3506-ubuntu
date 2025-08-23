/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#ifndef __APP_MUSIC_H__
#define __APP_MUSIC_H__

void app_music_init(lv_obj_t *parent, void *userdata);
void app_music_deinit(void *userdata);

#define APP_MUSIC {  \
    .w    = 2,  \
    .h    = 1,  \
    .init = app_music_init,  \
    .deinit = app_music_deinit,  \
}

#endif

