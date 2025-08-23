/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include <rtthread.h>
#include "lvgl.h"
#include "main.h"
#include "coffee_machine.h"
#include "furniture_control_ui.h"

///////////////////// VARIABLES ////////////////////
static lv_obj_t *main = NULL;
static lv_obj_t *btn_return;

static lv_obj_t *coffee_machine_box = NULL;
static lv_obj_t *coffee_machine_png_box = NULL;
static lv_obj_t *coffee_machine_box_name = NULL;
static lv_obj_t *coffee_machine_name = NULL;
static lv_obj_t *coffee_machine_button_box = NULL;
static lv_obj_t *coffee_machine_button = NULL;
static lv_obj_t *coffee_machine_button_label = NULL;

static lv_obj_t *coffee_1;
static lv_obj_t *coffee_2;
static lv_obj_t *coffee_3;
static lv_obj_t *coffee_4;
static lv_obj_t *coffee_5;
static lv_obj_t *coffee_6;
static lv_obj_t *coffee_0;
static int scroll_value = 0;
///////////////////// TEST LVGL SETTINGS ////////////////////

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

static void btn_return_cb(lv_event_t *event)
{
    printf("coffee_machine_page_jump_furniture_control_callback is into \n");
    furniture_control_ui_init();
    lv_obj_del(main);
    main = NULL;
}

void coffee_machine_png_box_scroll_callback(lv_event_t *event)
{
    lv_obj_t *screen = lv_event_get_target(event);
    scroll_value = lv_obj_get_scroll_left(screen);
    //printf("%d pixels are scrolled out on the left\n", scroll_value);
    if (scroll_value > 0 && scroll_value <= 200)
    {
        lv_img_set_zoom(coffee_1, 256);
        lv_img_set_zoom(coffee_2, 192);
        lv_label_set_text(coffee_machine_name, "美式咖啡");
    }
    if (scroll_value > 200 && scroll_value <= 600)
    {
        lv_img_set_zoom(coffee_1, 192);
        lv_img_set_zoom(coffee_2, 256);
        lv_img_set_zoom(coffee_3, 192);
        lv_label_set_text(coffee_machine_name, "意式咖啡");
    }
    if (scroll_value > 600 && scroll_value <= 1000)
    {
        lv_img_set_zoom(coffee_2, 192);
        lv_img_set_zoom(coffee_3, 256);
        lv_img_set_zoom(coffee_4, 192);
        lv_label_set_text(coffee_machine_name, "美味拿铁");
    }
    if (scroll_value > 1000 && scroll_value <= 1400)
    {
        lv_img_set_zoom(coffee_3, 192);
        lv_img_set_zoom(coffee_4, 256);
        lv_img_set_zoom(coffee_5, 192);
        lv_label_set_text(coffee_machine_name, "卡布奇诺");
    }
    if (scroll_value > 1400 && scroll_value <= 1800)
    {
        lv_img_set_zoom(coffee_4, 192);
        lv_img_set_zoom(coffee_5, 256);
        lv_label_set_text(coffee_machine_name, "RK咖啡");
    }
}

///////////////////// SCREENS ////////////////////
void coffee_machine_ui_init(void)
{
    if (main)
        return;

    main = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(main);
    lv_obj_set_style_pad_all(main, 0, LV_PART_MAIN);
    lv_obj_set_size(main, lv_pct(100), lv_pct(100));
    lv_obj_refr_size(main);

    btn_return = ui_return_btn_create(main, btn_return_cb, "咖啡机");

    coffee_machine_box = lv_obj_create(main);
    lv_obj_remove_style_all(coffee_machine_box);
    lv_obj_set_width(coffee_machine_box, lv_pct(100));
    lv_obj_set_height(coffee_machine_box, lv_pct(90));
    lv_obj_align(coffee_machine_box, LV_ALIGN_TOP_MID, 0, lv_pct(10));
    lv_obj_set_style_pad_all(coffee_machine_box, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(coffee_machine_box, 0, LV_PART_MAIN);
    lv_obj_set_flex_flow(coffee_machine_box, LV_FLEX_FLOW_COLUMN);//列

    coffee_machine_png_box = lv_obj_create(coffee_machine_box);
    lv_obj_set_scroll_snap_x(coffee_machine_png_box, LV_SCROLL_SNAP_CENTER);
    lv_obj_remove_style_all(coffee_machine_png_box);
    lv_obj_set_width(coffee_machine_png_box, lv_pct(100));
    lv_obj_set_height(coffee_machine_png_box, 300);
    lv_obj_set_style_pad_hor(coffee_machine_png_box, (scr_w - 300) / 2, 0);
    if (coffee_machine_png_box != NULL)
    {
        lv_obj_add_event_cb(coffee_machine_png_box,
                            coffee_machine_png_box_scroll_callback,
                            LV_EVENT_SCROLL, NULL);
    }
    //lv_obj_set_style_radius(coffee_machine_png_box, LV_RADIUS_CIRCLE, 0);
    //lv_obj_set_style_clip_corner(coffee_machine_png_box, true, 0);

    //图片缩放 lv_img_set_zoom
    //设置咖啡图片
    coffee_1 = lv_img_create(coffee_machine_png_box);
    lv_obj_set_width(coffee_1, 300);
    lv_obj_set_height(coffee_1, 300);
    lv_obj_align(coffee_1, LV_ALIGN_CENTER, 0, 10);
    lv_img_set_src(coffee_1, COFFEE_1);
    coffee_2 = lv_img_create(coffee_machine_png_box);
    lv_obj_set_width(coffee_2, 300);
    lv_obj_set_height(coffee_2, 300);
    lv_obj_align(coffee_2, LV_ALIGN_CENTER, 400, 10);
    lv_img_set_src(coffee_2, COFFEE_2);
    lv_img_set_zoom(coffee_2, 192);
    coffee_3 = lv_img_create(coffee_machine_png_box);
    lv_obj_set_width(coffee_3, 300);
    lv_obj_set_height(coffee_3, 300);
    lv_obj_align(coffee_3, LV_ALIGN_CENTER, 800, 10);
    lv_img_set_src(coffee_3, COFFEE_3);
    lv_img_set_zoom(coffee_3, 192);
    coffee_4 = lv_img_create(coffee_machine_png_box);
    lv_obj_set_width(coffee_4, 300);
    lv_obj_set_height(coffee_4, 300);
    lv_obj_align(coffee_4, LV_ALIGN_CENTER, 1200, 10);
    lv_img_set_src(coffee_4, COFFEE_4);
    lv_img_set_zoom(coffee_4, 192);
    coffee_5 = lv_img_create(coffee_machine_png_box);
    lv_obj_set_width(coffee_5, 300);
    lv_obj_set_height(coffee_5, 300);
    lv_obj_align(coffee_5, LV_ALIGN_CENTER, 1600, 10);
    lv_img_set_src(coffee_5, COFFEE_5);
    lv_img_set_zoom(coffee_5, 192);
    //设置咖啡图片

    coffee_machine_box_name = lv_obj_create(coffee_machine_box);
    lv_obj_remove_style_all(coffee_machine_box_name);
    lv_obj_set_width(coffee_machine_box_name, lv_pct(100));

    coffee_machine_name = lv_label_create(coffee_machine_box_name);
    lv_obj_add_style(coffee_machine_name, &style_txt_m, LV_PART_MAIN);
    lv_label_set_text(coffee_machine_name, "美式咖啡");
    lv_obj_set_style_text_color(coffee_machine_name, lv_color_white(),
                                LV_PART_MAIN);
    lv_obj_align(coffee_machine_name, LV_ALIGN_CENTER, 0, 0);

    coffee_machine_button_box = lv_obj_create(coffee_machine_box);
    lv_obj_remove_style_all(coffee_machine_button_box);
    lv_obj_set_width(coffee_machine_button_box, lv_pct(100));
    lv_obj_set_height(coffee_machine_button_box, lv_pct(20));

    coffee_machine_button = lv_btn_create(coffee_machine_button_box);
    lv_obj_set_width(coffee_machine_button, lv_pct(40));
    lv_obj_set_height(coffee_machine_button, lv_pct(100));
    lv_obj_set_style_bg_color(coffee_machine_button, lv_color_hex(0xDED6D6),
                              LV_PART_MAIN);
    lv_obj_align(coffee_machine_button, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_radius(coffee_machine_button, LV_RADIUS_CIRCLE, 0);

    coffee_machine_button_label = lv_label_create(coffee_machine_button);
    lv_obj_add_style(coffee_machine_button_label, &style_txt_m, LV_PART_MAIN);
    lv_obj_align(coffee_machine_button_label, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(coffee_machine_button_label, "开始制作");
}

