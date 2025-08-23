/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include <rtthread.h>

#include "lvgl.h"

#include "main.h"
#include "ui_resource.h"

#if BT_EN

#include "cJSON.h"
#include "wifibt.h"

static lv_obj_t *bg;

static lv_obj_t *part_switch;
static lv_obj_t *bt_label;
static lv_obj_t *bt_switch;

static struct wifibt_cmdarg cmdarg;
static struct bt_info new_info;

static void switch_toggled(lv_event_t *e)
{
    lv_obj_t *sw = lv_event_get_target(e);

    if (lv_obj_has_state(sw, LV_STATE_CHECKED))
        cmdarg.cmd = BT_ENABLE;
    else
        cmdarg.cmd = BT_DISABLE;
    bt_query(&cmdarg, sizeof(cmdarg));
}

lv_obj_t *menu_bt_init(lv_obj_t *parent)
{
    bg = lv_obj_create(parent);
    lv_obj_remove_style_all(bg);
    lv_obj_set_size(bg, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(bg, LV_FLEX_FLOW_COLUMN);
    lv_obj_center(bg);

    part_switch = lv_obj_create(bg);
    lv_obj_set_width(part_switch, lv_pct(100));
    lv_obj_set_height(part_switch, LV_SIZE_CONTENT);
    lv_obj_add_style(part_switch, &style_txt_s, LV_PART_MAIN);

    bt_label = lv_label_create(part_switch);
    lv_obj_align(bt_label, LV_ALIGN_LEFT_MID, 0, 0);
    lv_label_set_text(bt_label, "BT");
    lv_obj_add_style(bt_label, &style_txt_s, LV_PART_MAIN);
    bt_switch = lv_switch_create(part_switch);
    lv_obj_align(bt_switch, LV_ALIGN_RIGHT_MID, 0, 0);

    cmdarg.cmd = BT_INFO;
    cmdarg.val = &new_info;
    bt_query_wait(&cmdarg, sizeof(cmdarg));
    if (new_info.bt_state == BT_STATE_ON)
        lv_obj_add_state(bt_switch, LV_STATE_CHECKED);
    else
        lv_obj_clear_state(bt_switch, LV_STATE_CHECKED);

    lv_obj_add_event_cb(bt_switch, switch_toggled, LV_EVENT_VALUE_CHANGED, NULL);

    return bg;
}

void menu_bt_deinit(void)
{
    lv_obj_del(bg);
    bg = NULL;
}
#endif

