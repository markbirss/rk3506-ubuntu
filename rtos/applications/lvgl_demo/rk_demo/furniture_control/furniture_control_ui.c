/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include <rtthread.h>
#include "lvgl.h"

#include "furniture_control_ui.h"
#include "home_ui.h"
#include "icebox_ui.h"
#include "main.h"
#include "coffee_machine.h"
#include "player_ui.h"
#include "ui_btnmatrix.h"

///////////////////// VARIABLES ////////////////////
extern uint32_t LV_EVENT_GET_COMP_CHILD;

static lv_obj_t *main = NULL;
static lv_obj_t *ui_box_main = NULL;
static lv_obj_t *furniture_control_ui_box = NULL;
static lv_obj_t *furniture_control_ui_icebox_box = NULL;
static lv_obj_t *furniture_control_ui_player_box = NULL;
static lv_obj_t *furniture_control_ui_coffee_box = NULL;
static lv_obj_t *btn_return;
static lv_obj_t *v_bg;
static lv_img_dsc_t *bg_snapshot;
///////////////////// TEST LVGL SETTINGS ////////////////////

///////////////////// ANIMATIONS ////////////////////

static void btn_return_cb(lv_event_t *event)
{
    printf("page_jump_return_home_callback is into \n");
    home_ui_init();
    lv_obj_del(main);
    main = NULL;
}

static void furniture_control_page_jump_icebox_callback(lv_event_t *event)
{
    printf("furniture_control_page_jump_icebox_callback is into \n");
    icebox_ui_init();
    lv_obj_del(main);
    main = NULL;
}

static void furniture_control_page_jump_coffee_machine_callback(
    lv_event_t *event)
{
    printf("furniture_control_page_jump_coffee_machine_callback is into \n");
    coffee_machine_ui_init();
    lv_obj_del(main);
    main = NULL;
}

static void furniture_control_page_jump_player_callback(lv_event_t *event)
{
#if MULTIMEDIA_EN
    printf("furniture_control_page_jump_player_callback is into \n");
    player_ui_init();
    lv_obj_del(main);
    main = NULL;
#endif
}

static struct btn_desc fc_btn[] =
{
    {
        .obj  = &furniture_control_ui_icebox_box,
        .img  = IMG_ICEBOX,
        .text = "每日菜谱",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = furniture_control_page_jump_icebox_callback,
    },
    {
        .obj  = &furniture_control_ui_player_box,
        .img  = IMG_PLAYER,
        .text = "宣传视频",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = furniture_control_page_jump_player_callback,
    },
    {
        .obj  = &furniture_control_ui_coffee_box,
        .img  = IMG_COFFEE,
        .text = "咖啡机",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = furniture_control_page_jump_coffee_machine_callback,
    }
};

static lv_coord_t col_dsc[] = {200, 200, 200, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {200, LV_GRID_TEMPLATE_LAST};

static struct btn_matrix_desc btn_desc =
{
    .col_dsc = col_dsc,
    .row_dsc = row_dsc,
    .pad = 5,
    .gap = 40,
    .desc = fc_btn,
    .btn_cnt = sizeof(fc_btn) / sizeof(fc_btn[0]),
};


///////////////////// SCREENS ////////////////////

void furniture_control_ui_init(void)
{
    lv_area_t area;

    if (main)
        return;

    main = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(main);
    lv_obj_set_style_pad_all(main, 0, LV_PART_MAIN);
    lv_obj_set_size(main, lv_pct(100), lv_pct(100));
    lv_obj_refr_size(main);

    btn_return = ui_return_btn_create(main, btn_return_cb, "家电显控");

    furniture_control_ui_box = lv_obj_create(main);
    lv_obj_remove_style_all(furniture_control_ui_box);
    lv_obj_set_width(furniture_control_ui_box, lv_pct(100));
    lv_obj_set_height(furniture_control_ui_box, lv_pct(33));
    lv_obj_center(furniture_control_ui_box);
    lv_obj_refr_size(furniture_control_ui_box);
    lv_obj_refr_pos(furniture_control_ui_box);

    bg_snapshot = get_bg_snapshot();
    v_bg = lv_img_create(furniture_control_ui_box);
    lv_obj_set_width(v_bg, lv_pct(100));
    lv_obj_set_height(v_bg, lv_pct(100));
    lv_obj_refr_size(v_bg);
    lv_obj_refr_pos(v_bg);
    lv_obj_get_content_coords(v_bg, &area);
    lv_img_set_src(v_bg, bg_snapshot);
    lv_img_set_offset_x(v_bg, -area.x1);
    lv_img_set_offset_y(v_bg, -area.y1);
    lv_obj_clear_flag(v_bg, LV_OBJ_FLAG_SCROLLABLE);

    if (scr_dir == LV_DIR_HOR)
        btn_desc.gap = RK_PCT_H(5);
    else
        btn_desc.gap = RK_PCT_W(2);

    ui_box_main = ui_btnmatrix_create(v_bg, &btn_desc);
    lv_obj_center(ui_box_main);

    lv_obj_set_style_bg_opa(furniture_control_ui_icebox_box, LV_OPA_COVER,
                            LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(furniture_control_ui_player_box, LV_OPA_COVER,
                            LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(furniture_control_ui_coffee_box, LV_OPA_COVER,
                            LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(furniture_control_ui_icebox_box, LV_OPA_TRANSP,
                            LV_PART_MAIN);
    lv_obj_set_style_bg_opa(furniture_control_ui_player_box, LV_OPA_TRANSP,
                            LV_PART_MAIN);
    lv_obj_set_style_bg_opa(furniture_control_ui_coffee_box, LV_OPA_TRANSP,
                            LV_PART_MAIN);
    lv_obj_set_style_shadow_width(furniture_control_ui_icebox_box, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(furniture_control_ui_player_box, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(furniture_control_ui_coffee_box, 0, LV_PART_MAIN);
}

