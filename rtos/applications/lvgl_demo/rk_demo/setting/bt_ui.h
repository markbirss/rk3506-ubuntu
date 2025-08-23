/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#ifndef __BT_UI_H__
#define __BT_UI_H__

lv_obj_t *menu_bt_init(lv_obj_t *parent);
void menu_bt_deinit(void);
int bt_connected(void);

#endif

