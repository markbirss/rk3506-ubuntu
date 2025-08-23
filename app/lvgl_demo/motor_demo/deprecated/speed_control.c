/*
 * Copyright (c) 2024 Rockchip, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#define _GNU_SOURCE
#include <lvgl/lvgl.h>
#include <lvgl/lv_conf.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <pthread.h>

#include "hal_sdl.h"
#include "hal_drm.h"
#include "main.h"

#define START_ANGLE         90
#define MAX_SPEED           ((METER_TICKS - 1) * 1000)
#define ANGLE_TO_SPEED(x)   ((x) * (METER_TICKS - 1) * 1000 / max_angle)
#define SPEED_TO_ANGLE(x)   ((x) * max_angle / (METER_TICKS - 1) / 1000)

static float max_angle = 270.0;

static lv_obj_t *slider0;
static lv_obj_t *slider0_val;
static lv_obj_t *slider1;
static lv_obj_t *slider1_val;
static lv_obj_t *btn0_stop;
static lv_obj_t *btn0_label;
static lv_obj_t *btn1_stop;
static lv_obj_t *btn1_label;
lv_obj_t *label_jitter;

static void button0_stop_event_handler(lv_event_t *e)
{
    struct meter *meter = lv_event_get_user_data(e);
    printf("button 0 stop Clicked\n");
    lv_slider_set_value(slider0, 0, LV_ANIM_ON);
    meter->tar_speed = 0;
    lv_label_set_text(slider0_val, "0.000 RPM");
    motor_stop(meter->slave);
}

static void button1_stop_event_handler(lv_event_t *e)
{
    struct meter *meter = lv_event_get_user_data(e);
    printf("button 1 stop Clicked\n");
    lv_slider_set_value(slider1, 0, LV_ANIM_ON);
    meter->tar_speed = 0;
    lv_label_set_text(slider1_val, "0.000 RPM");
    motor_stop(meter->slave);
}

static void meter_timer_cb(lv_timer_t *timer)
{
    struct meter *meter = timer->user_data;
    int32_t speed = 0;
    int32_t v = 0;
    int32_t reverse = meter->tar_speed < 0;
    int ret = meter->tar_speed ? 1 : 0;

#if ENABLE_MOTOR_CONTROL
    ret = motor_check(meter->slave);
#endif
    switch (ret)
    {
    case -1:
        lv_label_set_text(meter->label_status,
                          "Status : Not available");
        break;
    case 0:
    case 2000:
    case -2000:
        lv_label_set_text(meter->label_status,
                          "Status : Stop");
        break;
    default:
#if ENABLE_MOTOR_CONTROL
        speed = abs(ret) / 100.0;
#else
        srand(time(0));
        speed = abs(meter->tar_speed) +
                (rand() % 300 - 150);
        if (speed < 0) speed = 0;
#endif
        v = SPEED_TO_ANGLE(speed);
        lv_label_set_text(meter->label_status,
                          "Status : Start");
        break;
    }

    if (reverse)
    {
        lv_obj_set_style_text_color(meter->label_reverse,
                                    lv_palette_lighten(LV_PALETTE_GREEN, 1),
                                    LV_PART_MAIN);
        lv_obj_set_style_outline_color(meter->label_reverse,
                                       lv_palette_lighten(LV_PALETTE_GREEN, 1),
                                       LV_PART_MAIN);
    }
    else
    {
        lv_obj_set_style_text_color(meter->label_reverse,
                                    lv_palette_darken(LV_PALETTE_GREY, 5),
                                    LV_PART_MAIN);
        lv_obj_set_style_outline_color(meter->label_reverse,
                                       lv_palette_darken(LV_PALETTE_GREY, 5),
                                       LV_PART_MAIN);
    }
    lv_arc_set_value(meter->main, speed);
    lv_img_set_angle(meter->needle, (START_ANGLE + v) * 10);
    lv_label_set_text_fmt(meter->label_val, "%.3f",
                          (float)speed / 1000.0);
}

static void slider0_cb(lv_event_t *e)
{
    struct meter *meter = lv_event_get_user_data(e);

    if (e->code == LV_EVENT_VALUE_CHANGED)
    {
        lv_label_set_text_fmt(slider0_val, "%.3f RPM",
                              (float)lv_slider_get_value(lv_event_get_target(e)) / 1000.0);
    }
    if (e->code == LV_EVENT_RELEASED)
    {
        meter->tar_speed = lv_slider_get_value(lv_event_get_target(e));
        lv_label_set_text_fmt(slider0_val, "%.3f RPM",
                              (float)meter->tar_speed / 1000.0);
        motor_start(meter->slave, meter->tar_speed);
    }
}

static void slider1_cb(lv_event_t *e)
{
    struct meter *meter = lv_event_get_user_data(e);

    if (e->code == LV_EVENT_VALUE_CHANGED)
    {
        lv_label_set_text_fmt(slider1_val, "%.3f RPM",
                              (float)lv_slider_get_value(lv_event_get_target(e)) / 1000.0);
    }
    if (e->code == LV_EVENT_RELEASED)
    {
        meter->tar_speed = lv_slider_get_value(lv_event_get_target(e));
        lv_label_set_text_fmt(slider1_val, "%.3f RPM",
                              (float)meter->tar_speed / 1000.0);
        motor_start(meter->slave, meter->tar_speed);
    }
}

static void meter_create(struct meter *meter)
{
    meter->main = lv_arc_create(meter->cont);
    lv_obj_clear_flag(meter->main, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(meter->main, lv_pct(100), lv_pct(100));
    lv_arc_set_rotation(meter->main, START_ANGLE);
    lv_arc_set_bg_angles(meter->main, 0, max_angle);
    lv_obj_set_style_arc_width(meter->main, 20,
                               LV_PART_MAIN);
    lv_obj_set_style_arc_width(meter->main, 20,
                               LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(meter->main, LV_OPA_0,
                            LV_PART_MAIN);
    lv_obj_set_style_bg_opa(meter->main, LV_OPA_0,
                            LV_PART_KNOB);
    lv_arc_set_value(meter->main, 0);
    lv_arc_set_range(meter->main, 0, MAX_SPEED);
    lv_obj_center(meter->main);

    meter->adorn = lv_arc_create(meter->cont);
    lv_obj_clear_flag(meter->adorn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(meter->adorn, 520, 520);
    lv_arc_set_rotation(meter->adorn, START_ANGLE);
    lv_arc_set_bg_angles(meter->adorn, 0, max_angle);
    lv_obj_set_style_arc_width(meter->adorn, 2,
                               LV_PART_MAIN);
    lv_obj_set_style_arc_width(meter->adorn, 2,
                               LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(meter->adorn, LV_OPA_0,
                            LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(meter->adorn, LV_OPA_0,
                            LV_PART_KNOB);
    lv_arc_set_value(meter->adorn, 0);
    lv_arc_set_range(meter->adorn, 0, MAX_SPEED);
    lv_obj_center(meter->adorn);

    meter->needle = lv_img_create(meter->cont);
    lv_img_set_src(meter->needle, SRC_PNG(needle));
    lv_img_set_angle(meter->needle, START_ANGLE * 10);
    lv_obj_center(meter->needle);

    for (int i = 0; i < METER_TICKS; i++)
    {
        int w, h;
        meter->ticks[i].obj = lv_line_create(meter->cont);
        update_tick_points(meter->ticks[i].p,
                           START_ANGLE + SPEED_TO_ANGLE(i * 1000));
        lv_obj_set_style_line_color(meter->ticks[i].obj,
                                    lv_color_white(), LV_PART_MAIN);
        lv_obj_set_style_line_width(meter->ticks[i].obj,
                                    4, LV_PART_MAIN);
        lv_line_set_points(meter->ticks[i].obj,
                           meter->ticks[i].p, 2);

        meter->ticks[i].label = lv_label_create(meter->cont);
        lv_label_set_text_fmt(meter->ticks[i].label,
                              "%d", i);
        lv_obj_refr_size(meter->ticks[i].label);
        w = lv_obj_get_width(meter->ticks[i].label);
        h = lv_obj_get_height(meter->ticks[i].label);
        lv_obj_set_pos(meter->ticks[i].label,
                       meter->ticks[i].p[2].x - w / 2,
                       meter->ticks[i].p[2].y - h / 2);
        lv_obj_set_style_text_color(meter->ticks[i].label,
                                    lv_color_white(), LV_PART_MAIN);
    }

    meter->label_val = lv_label_create(meter->cont);
    lv_label_set_text(meter->label_val, "0");
    lv_obj_center(meter->label_val);
    lv_obj_set_style_text_font(meter->label_val,
                               &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(meter->label_val,
                                lv_color_white(), LV_PART_MAIN);

    meter->label_uint = lv_label_create(meter->cont);
    lv_label_set_text(meter->label_uint, "RPM");
    lv_obj_set_style_text_color(meter->label_uint,
                                lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(meter->label_uint,
                               &lv_font_montserrat_32, LV_PART_MAIN);
    lv_obj_align_to(meter->label_uint, meter->label_val,
                    LV_ALIGN_OUT_BOTTOM_MID, 0, 2);

    meter->label_reverse = lv_label_create(meter->cont);
    lv_label_set_text(meter->label_reverse, "R");
    lv_obj_set_style_text_font(meter->label_reverse,
                               &lv_font_montserrat_32, LV_PART_MAIN);
    lv_obj_set_style_text_color(meter->label_reverse,
                                lv_palette_darken(LV_PALETTE_GREY, 5), LV_PART_MAIN);
    lv_obj_set_style_outline_color(meter->label_reverse,
                                   lv_palette_darken(LV_PALETTE_GREY, 5), LV_PART_MAIN);
    lv_obj_set_style_outline_width(meter->label_reverse,
                                   2, LV_PART_MAIN);
    lv_obj_align_to(meter->label_reverse, meter->label_uint,
                    LV_ALIGN_OUT_BOTTOM_MID, 0, 2);

    meter->label_status = lv_label_create(meter->cont);
    lv_label_set_text(meter->label_status, "Status : Stop");
    lv_obj_set_style_text_font(meter->label_status,
                               &lv_font_montserrat_32, LV_PART_MAIN);
    lv_obj_set_style_text_color(meter->label_status,
                                lv_color_white(), LV_PART_MAIN);
    lv_obj_align_to(meter->label_status, meter->label_val,
                    LV_ALIGN_OUT_TOP_MID, 0, -2);

    meter->timer = lv_timer_create(meter_timer_cb, 20,
                                   meter);
}

void speed_control_ui(lv_obj_t *cont_main, struct meter *meters, int mode)
{
    meters[0].cont = lv_obj_create(cont_main);
    lv_obj_remove_style_all(meters[0].cont);
    lv_obj_set_size(meters[0].cont, 580, 580);
    lv_obj_align(meters[0].cont, LV_ALIGN_TOP_LEFT,
                 86, 200);
    meter_create(&meters[0]);

    meters[1].cont = lv_obj_create(cont_main);
    lv_obj_remove_style_all(meters[1].cont);
    lv_obj_set_size(meters[1].cont, 580, 580);
    if (mode == 0)
        lv_obj_align(meters[1].cont, LV_ALIGN_TOP_RIGHT,
                     -86, 200);
    else
        lv_obj_align(meters[1].cont, LV_ALIGN_BOTTOM_LEFT,
                     86, -200);
    meter_create(&meters[1]);

    slider0_val = lv_label_create(cont_main);
    lv_label_set_text(slider0_val, "0.000 RPM");
    lv_obj_set_style_text_font(slider0_val,
                               &lv_font_montserrat_32, LV_PART_MAIN);
    lv_obj_set_style_text_color(slider0_val,
                                lv_color_white(), LV_PART_MAIN);

    if (mode == 0)
        lv_obj_align_to(slider0_val, meters[0].cont,
                        LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    else
        lv_obj_align_to(slider0_val, meters[0].cont,
                        LV_ALIGN_OUT_RIGHT_MID, 50, 0);

    slider0 = lv_slider_create(cont_main);
    lv_slider_set_range(slider0, -MAX_SPEED, MAX_SPEED);
    lv_obj_add_event_cb(slider0, slider0_cb,
                        LV_EVENT_ALL, &meters[0]);
    if (mode == 0)
    {
        lv_obj_set_size(slider0, 580, 50);
        lv_obj_align_to(slider0, slider0_val,
                        LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    }
    else
    {
        lv_obj_set_size(slider0, 50, 580);
        lv_obj_align_to(slider0, slider0_val,
                        LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    }

    slider1_val = lv_label_create(cont_main);
    lv_label_set_text(slider1_val, "0.000 RPM");
    lv_obj_set_style_text_font(slider1_val,
                               &lv_font_montserrat_32, LV_PART_MAIN);
    lv_obj_set_style_text_color(slider1_val,
                                lv_color_white(), LV_PART_MAIN);
    if (mode == 0)
        lv_obj_align_to(slider1_val, meters[1].cont,
                        LV_ALIGN_OUT_BOTTOM_MID, 0, 50);
    else
        lv_obj_align_to(slider1_val, meters[1].cont,
                        LV_ALIGN_OUT_RIGHT_MID, 50, 0);

    slider1 = lv_slider_create(cont_main);
    lv_slider_set_range(slider1, -MAX_SPEED, MAX_SPEED);
    lv_obj_add_event_cb(slider1, slider1_cb,
                        LV_EVENT_ALL, &meters[1]);
    if (mode == 0)
    {
        lv_obj_set_size(slider1, 580, 50);
        lv_obj_align_to(slider1, slider1_val,
                        LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    }
    else
    {
        lv_obj_set_size(slider1, 50, 580);
        lv_obj_align_to(slider1, slider1_val,
                        LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    }

    btn0_stop = lv_btn_create(cont_main);
    lv_obj_set_width(btn0_stop, 240);
    lv_obj_set_height(btn0_stop, 120);
    lv_obj_add_event_cb(btn0_stop,
                        button0_stop_event_handler, LV_EVENT_CLICKED,
                        &meters[0]);
    if (mode == 0)
        lv_obj_align_to(btn0_stop, slider0,
                        LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    else
        lv_obj_align_to(btn0_stop, slider0,
                        LV_ALIGN_OUT_LEFT_BOTTOM, -20, 0);

    btn0_label = lv_label_create(btn0_stop);
    lv_label_set_text(btn0_label, "停止");
    lv_obj_center(btn0_label);
    lv_obj_set_style_text_font(btn0_label,
                               ttf_main_s.font, LV_PART_MAIN);

    btn1_stop = lv_btn_create(cont_main);
    lv_obj_set_width(btn1_stop, 240);
    lv_obj_set_height(btn1_stop, 120);
    lv_obj_add_event_cb(btn1_stop,
                        button1_stop_event_handler, LV_EVENT_CLICKED,
                        &meters[1]);
    if (mode == 0)
        lv_obj_align_to(btn1_stop, slider1,
                        LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    else
        lv_obj_align_to(btn1_stop, slider1,
                        LV_ALIGN_OUT_LEFT_BOTTOM, -20, 0);

    btn1_label = lv_label_create(btn1_stop);
    lv_label_set_text(btn1_label, "停止");
    lv_obj_center(btn1_label);
    lv_obj_set_style_text_font(btn1_label,
                               ttf_main_s.font, LV_PART_MAIN);

    label_jitter = lv_label_create(cont_main);
    lv_label_set_text_fmt(label_jitter,
                          "Jitter:\nMax: %10u\nMin: %10u", 0, 0);
    lv_obj_set_style_text_color(label_jitter,
                                lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label_jitter,
                               &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_center(label_jitter);
}

