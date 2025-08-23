#ifndef __HOME_UI_H__
#define __HOME_UI_H__

#include "lvgl/lvgl.h"
#include "ui_resource.h"

lv_img_dsc_t *get_bg_snapshot(void);
void rk_demo_init(void);
void home_ui_init(void);
void rk_demo_bg_hide(void);
void rk_demo_bg_show(void);
void rk_demo_bg_set_img(const char *img);

#endif

