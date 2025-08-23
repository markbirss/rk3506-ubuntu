/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/

#ifndef __LV_APP_MAIN_H__
#define __LV_APP_MAIN_H__

#include <rtthread.h>

#define LVGL_REGION_X           0
#define LVGL_REGION_Y           0
#define LVGL_FB_WIDTH           LV_HOR_RES
#define LVGL_FB_HEIGHT          LV_VER_RES
#define LVGL_FB_COLOR_DEPTH     LV_COLOR_DEPTH

#define LVGL_EVENT_COMMIT       (0x01UL << 0)
#define LVGL_EVENT_TASK_HANDLER (0x01UL << 1)
#define LVGL_EVENT_REFR_DONE    (0x01UL << 2)

void lvgl_task_commit(void);

#endif
