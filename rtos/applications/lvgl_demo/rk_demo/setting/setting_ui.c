/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include <rtthread.h>

#include "lvgl.h"

#include "about_ui.h"
#include "home_ui.h"
#include "language_and_date_ui.h"
#include "main.h"
#include "wallpaper_ui.h"
#include "ui_resource.h"
#include "ui_btnmatrix.h"

enum
{
    SUBMENU_MIN = 0,
    SUBMENU_WALLPAPER = SUBMENU_MIN,
    SUBMENU_LANGUAGE_DATE,
    SUBMENU_ABOUT,
#if WIFIBT_EN
    SUBMENU_WIFI,
#endif
#if BT_EN
    SUBMENU_BT,
#endif
    SUBMENU_MAX,
};

struct submenu_s
{
    char *name;
    void (*init)(void);
    void (*deinit)(void);
    lv_obj_t *menu;
};

static lv_obj_t *main = NULL;
static lv_obj_t *btn_return;
static lv_obj_t *ui_cont;
static lv_obj_t *ui_box_main;
static lv_obj_t *ui_sliders;
#if WIFIBT_EN
static lv_obj_t *ui_wifi;
#endif
#if BT_EN
static lv_obj_t *ui_bt;
#endif
static lv_obj_t *ui_wallpaper;
static lv_obj_t *ui_data;
static lv_obj_t *ui_about;

static lv_obj_t *submenu_mask;
static lv_obj_t *submenu_area;
static lv_obj_t *cancel_btn;

static lv_timer_t *timer;

static lv_obj_t *sub_menu[SUBMENU_MAX];
static int cur_menu = SUBMENU_MAX;
static struct submenu_s submenu_desc[SUBMENU_MAX];

static void sliders_draw(lv_obj_t *parent, struct btn_desc *desc);
static void menu_switch_cb(lv_event_t *e);
static void wifi_switch_cb(lv_event_t *e);
#if BT_EN
static void switch_toggled(lv_event_t *e);
#endif

#define SUBMENU_COMMON_DEFINE(enum_t, name) \
static void submenu_##name(void)  \
{   \
    lv_obj_scroll_to_y(submenu_area, 0, LV_ANIM_OFF); \
    if (!submenu_desc[enum_t].menu)\
        submenu_desc[enum_t].menu = menu_##name##_init(submenu_area);\
}   \
static void submenu_##name##_destroy(void)  \
{   \
    if (submenu_desc[enum_t].menu)\
    {   \
        menu_##name##_deinit();\
        submenu_desc[enum_t].menu = NULL;\
    }   \
}

#if WIFIBT_EN
SUBMENU_COMMON_DEFINE(SUBMENU_WIFI, wifi)
#endif
#if BT_EN
SUBMENU_COMMON_DEFINE(SUBMENU_BT, bt)
#endif
SUBMENU_COMMON_DEFINE(SUBMENU_WALLPAPER, wallpaper)
SUBMENU_COMMON_DEFINE(SUBMENU_LANGUAGE_DATE, language)
SUBMENU_COMMON_DEFINE(SUBMENU_ABOUT, about)

static struct submenu_s submenu_desc[SUBMENU_MAX] =
{
    {"锁屏和壁纸",  submenu_wallpaper,  submenu_wallpaper_destroy, NULL},
    {"语言和日期",  submenu_language,  submenu_language_destroy, NULL},
    {"关于",        submenu_about,  submenu_about_destroy, NULL}
#if WIFIBT_EN
    {"WIFI",        submenu_wifi,    submenu_wifi_destroy, NULL},
#endif
#if BT_EN
    {"蓝牙",        submenu_bt,      submenu_bt_destroy,   NULL},
#endif
};

static struct btn_desc setting_btn[] =
{
    {
        .obj  = &ui_sliders,
        .w    = 2,
        .h    = 1,
        .draw = sliders_draw,
    },
#if WIFIBT_EN
    {
        .obj  = &ui_wifi,
        .img  = ICON_WIFI,
        .text = "WiFi",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = wifi_switch_cb,
        .user_data = (void *)SUBMENU_WIFI
    },
#endif
#if BT_EN
    {
        .obj  = &ui_bt,
        .img  = ICON_BT,
        .text = "蓝牙",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = switch_toggled,
        .user_data = (void *)SUBMENU_BT
    },
#endif
    {
        .obj  = &ui_wallpaper,
        .img  = ICON_WALLPAPER,
        .text = "锁屏和壁纸",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = menu_switch_cb,
        .user_data = (void *)SUBMENU_WALLPAPER
    },
    {
        .obj  = &ui_data,
        .img  = ICON_DATA,
        .text = "语言和日期",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = menu_switch_cb,
        .user_data = (void *)SUBMENU_LANGUAGE_DATE
    },
    {
        .obj  = &ui_about,
        .img  = ICON_ABOUT,
        .text = "关于",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = menu_switch_cb,
        .user_data = (void *)SUBMENU_ABOUT
    }
};

#ifdef LARGE
static lv_coord_t col_dsc[] = {300, 300, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {140, 140, 140, 140, LV_GRID_TEMPLATE_LAST};
#else
static lv_coord_t col_dsc[] = {200, 200, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {140, 140, 140, 140, LV_GRID_TEMPLATE_LAST};
#endif

struct btn_matrix_desc setting_desc =
{
    .col_dsc = col_dsc,
    .row_dsc = row_dsc,
    .pad = 5,
    .gap = 20,
    .desc = setting_btn,
    .btn_cnt = sizeof(setting_btn) / sizeof(setting_btn[0]),
};

static void slider_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(label, buf);
}

static void sliders_draw(lv_obj_t *parent, struct btn_desc *desc)
{
    lv_obj_t *cont_v, *cont_d;
    lv_obj_t *label_v, *label_d;
    lv_obj_t *obj_v, *obj_d;
    lv_obj_t *label_vp, *label_dp;

    lv_obj_clear_flag(parent, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_pad_all(parent, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);

    cont_v = lv_obj_create(parent);
    lv_obj_remove_style_all(cont_v);
    lv_obj_set_size(cont_v, lv_pct(100), lv_pct(50));

    cont_d = lv_obj_create(parent);
    lv_obj_remove_style_all(cont_d);
    lv_obj_set_size(cont_d, lv_pct(100), lv_pct(50));

    obj_v = lv_slider_create(cont_v);
    lv_obj_set_size(obj_v, lv_pct(75), lv_pct(30));
    lv_obj_center(obj_v);
    lv_obj_set_style_bg_opa(obj_v, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_slider_set_range(obj_v, 10, 100);
    lv_slider_set_value(obj_v, 100, LV_ANIM_OFF);
    lv_obj_refr_size(obj_v);
    lv_obj_refr_pos(obj_v);

    label_v = lv_label_create(cont_v);
    lv_label_set_text(label_v, "音量");
    lv_obj_add_style(label_v, &style_txt_s, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_v, lv_color_white(), LV_PART_MAIN);
    lv_obj_align_to(label_v, obj_v, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    label_vp = lv_label_create(cont_v);
    lv_label_set_text(label_vp, "100%");
    lv_obj_add_style(label_vp, &style_txt_s, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_vp, lv_color_white(), LV_PART_MAIN);
    lv_obj_align_to(label_vp, obj_v, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_add_event_cb(obj_v, slider_event_cb, LV_EVENT_VALUE_CHANGED, label_vp);

    obj_d = lv_slider_create(cont_d);
    lv_obj_set_size(obj_d, lv_pct(75), lv_pct(30));
    lv_obj_center(obj_d);
    lv_obj_set_style_bg_opa(obj_d, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_slider_set_range(obj_d, 10, 100);
    lv_slider_set_value(obj_d, 100, LV_ANIM_OFF);
    lv_obj_refr_size(obj_d);
    lv_obj_refr_pos(obj_d);

    label_d = lv_label_create(cont_d);
    lv_label_set_text(label_d, "亮度");
    lv_obj_add_style(label_d, &style_txt_s, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_d, lv_color_white(), LV_PART_MAIN);
    lv_obj_align_to(label_d, obj_d, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    label_dp = lv_label_create(cont_d);
    lv_label_set_text(label_dp, "100%");
    lv_obj_add_style(label_dp, &style_txt_s, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_dp, lv_color_white(), LV_PART_MAIN);
    lv_obj_align_to(label_dp, obj_d, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_add_event_cb(obj_d, slider_event_cb, LV_EVENT_VALUE_CHANGED, label_dp);
}

#if BT_EN
static struct wifibt_cmdarg cmdarg;
static struct bt_info new_info;
static void switch_toggled(lv_event_t *e)
{
    lv_color_t color = lv_obj_get_style_bg_color(ui_bt, LV_PART_MAIN);
    lv_color_t hl = HL_BLUE;

    if (e->code == LV_EVENT_CLICKED)
    {
        if (color.full == hl.full)
        {
            lv_obj_set_style_bg_color(ui_bt, MAIN_COLOR, LV_PART_MAIN);
            cmdarg.cmd = BT_DISABLE;
        }
        else
        {
            lv_obj_set_style_bg_color(ui_bt, HL_BLUE, LV_PART_MAIN);
            cmdarg.cmd = BT_ENABLE;
        }
        bt_query(&cmdarg, sizeof(cmdarg));
    }
}
#endif

static void menu_switch_cb(lv_event_t *e)
{
    intptr_t idx = (intptr_t)lv_event_get_user_data(e);

    if (idx < SUBMENU_MIN || idx >= SUBMENU_MAX)
        return;

    if ((cur_menu >= SUBMENU_MIN)
            && (cur_menu < SUBMENU_MAX)
            && submenu_desc[cur_menu].menu)
    {
        lv_obj_add_flag(submenu_desc[cur_menu].menu,
                        LV_OBJ_FLAG_HIDDEN);
    }

    if (submenu_desc[idx].init)
        submenu_desc[idx].init();

    cur_menu = idx;
    if (submenu_desc[cur_menu].menu)
    {
        lv_obj_clear_flag(submenu_mask, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(submenu_desc[cur_menu].menu,
                          LV_OBJ_FLAG_HIDDEN);
    }
}

static void wifi_switch_cb(lv_event_t *e)
{
#if WIFIBT_EN
    if (wifi_init_done())
    {
        menu_switch_cb(e);
    }
    else
    {
        lv_obj_t *toast = lv_toast_create(lv_layer_sys());
        lv_toast_set_text(toast, "WiFi初始化未完成请稍后重试");
        lv_obj_add_style(toast, &style_txt_m, LV_PART_MAIN);
        lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
        lv_toast_show(toast);
    }
#endif
}

static void btn_return_cb(lv_event_t *e)
{
    switch (e->code)
    {
    case LV_EVENT_CLICKED:
        home_ui_init();
        lv_timer_del(timer);
        for (int i = SUBMENU_MIN; i < SUBMENU_MAX; i++)
        {
            if (submenu_desc[i].deinit)
                submenu_desc[i].deinit();
        }
        lv_obj_del(main);
        main = NULL;
        lv_obj_del(submenu_mask);
        submenu_mask = NULL;
        break;
    default:
        break;
    }
}

static void submenu_mask_cb(lv_event_t *e)
{
    lv_obj_add_flag(submenu_mask, LV_OBJ_FLAG_HIDDEN);
}

static void state_update(lv_timer_t *timer)
{
#if WIFIBT_EN
    if (wifi_enabled())
        lv_obj_set_style_bg_color(ui_wifi, HL_BLUE, LV_PART_MAIN);
    else
        lv_obj_set_style_bg_color(ui_wifi, MAIN_COLOR, LV_PART_MAIN);
#endif

#if BT_EN
    cmdarg.cmd = BT_INFO;
    cmdarg.val = &new_info;
    bt_query_wait(&cmdarg, sizeof(cmdarg));
    if (new_info.bt_state >= BT_STATE_ON)
        lv_obj_set_style_bg_color(ui_bt, HL_BLUE, LV_PART_MAIN);
    else
        lv_obj_set_style_bg_color(ui_bt, MAIN_COLOR, LV_PART_MAIN);
#endif
}

void setting_ui_init(void)
{
    if (main)
    {
        lv_obj_clear_flag(main, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    main = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(main);
    lv_obj_set_style_pad_all(main, 0, LV_PART_MAIN);
    lv_obj_set_size(main, lv_pct(100), lv_pct(100));
    lv_obj_refr_size(main);

    btn_return = ui_return_btn_create(main, btn_return_cb, "系统设置");

    ui_box_main = ui_btnmatrix_create(main, &setting_desc);
    lv_obj_align(ui_box_main, LV_ALIGN_TOP_MID, 0, lv_pct(10));

#if WIFIBT_EN
    if (wifi_enabled())
        lv_obj_set_style_bg_color(ui_wifi, HL_BLUE, LV_PART_MAIN);
    else
        lv_obj_set_style_bg_color(ui_wifi, MAIN_COLOR, LV_PART_MAIN);
#endif

#if BT_EN
    cmdarg.cmd = BT_INFO;
    cmdarg.val = &new_info;
    bt_query_wait(&cmdarg, sizeof(cmdarg));
    if (new_info.bt_state >= BT_STATE_ON)
        lv_obj_set_style_bg_color(ui_bt, HL_BLUE, LV_PART_MAIN);
    else
        lv_obj_set_style_bg_color(ui_bt, MAIN_COLOR, LV_PART_MAIN);
#endif

    submenu_mask = lv_obj_create(lv_scr_act());
    lv_obj_add_flag(submenu_mask, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_style_all(submenu_mask);
    lv_obj_set_size(submenu_mask, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(submenu_mask, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(submenu_mask, LV_OPA_80, LV_PART_MAIN);
    lv_obj_add_event_cb(submenu_mask, submenu_mask_cb, LV_EVENT_CLICKED,
                        submenu_mask);

    submenu_area = lv_obj_create(submenu_mask);
    lv_obj_set_size(submenu_area, lv_pct(70), lv_pct(70));
    lv_obj_set_style_bg_color(submenu_area, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(submenu_area);
    lv_obj_refr_size(submenu_area);
    lv_obj_refr_pos(submenu_area);

    cancel_btn = lv_img_create(submenu_mask);
    lv_obj_add_flag(cancel_btn, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_img_set_src(cancel_btn, IMG_CANCEL);
    lv_obj_align_to(cancel_btn, submenu_area, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    timer = lv_timer_create(state_update, 1000, NULL);
    lv_timer_enable(timer);
}

