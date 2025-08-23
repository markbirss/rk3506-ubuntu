/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#ifndef __APP_SWITCH_H__
#define __APP_SWITCH_H__

void app_switch_init(lv_obj_t *parent, void *userdata);

#define APP_SWITCH(x) {  \
    .w    = 1,  \
    .h    = 1,  \
    .init = app_switch_init,  \
    .userdata = x,  \
}

#endif

