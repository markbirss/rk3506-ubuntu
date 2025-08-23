#ifndef __UI_COMMON_H__
#define __UI_COMMON_H__

#include <lvgl/lvgl.h>

lv_obj_t *ui_return_btn_create(lv_obj_t *parent, lv_event_cb_t cb,
                               const char *text);

#endif

