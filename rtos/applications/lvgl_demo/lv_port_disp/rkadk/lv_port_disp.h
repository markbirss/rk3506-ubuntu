/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#ifndef __LV_PORT_DISP_H__
#define __LV_PORT_DISP_H__

#include <rtthread.h>
#include "lvgl.h"

rt_err_t lv_port_disp_init(lv_disp_rot_t rotate_disp);
rt_err_t lv_port_disp_deinit(void);

#endif

