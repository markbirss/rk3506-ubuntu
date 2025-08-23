/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#ifndef __SMART_HOME_UI_H__
#define __SMART_HOME_UI_H__

struct app_desc
{
    int w;
    int h;
    void (*init)(lv_obj_t *parent, void *userdata);
    void (*scroll_cb)(lv_event_t *event, void *userdata);
    void (*deinit)(void *userdata);
    void *userdata;
};

void smart_home_ui_init(void);
lv_img_dsc_t *smart_home_ui_bg_blur(void);

#endif

