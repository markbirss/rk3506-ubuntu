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

static float max_angle = 360.0;

static void needle_cb(lv_event_t *e)
{
    struct meter *meter;
    lv_indev_t *indev;
    lv_area_t a;
    lv_point_t p;
    lv_point_t c;
    lv_coord_t w, h;
    lv_coord_t dx, dy;
    lv_coord_t angle;
    lv_coord_t final_angle;

    meter = lv_event_get_user_data(e);
    if ((e->code == LV_EVENT_PRESSING) ||
            (e->code == LV_EVENT_RELEASED))
    {
        indev = lv_indev_get_act();
        lv_indev_get_point(indev, &p);
        lv_obj_get_coords(lv_event_get_target(e), &a);
        w = a.x2 - a.x1;
        h = a.y2 - a.y1;
        c.x = a.x1 + w / 2;
        c.y = a.y1 + h / 2;
        dx = p.x - c.x;
        dy = p.y - c.y;
        angle = lv_atan2(dy, dx);
        final_angle = angle - START_ANGLE;
        if (final_angle < 0)
            final_angle += 360;
        lv_label_set_text_fmt(meter->label_val,
                              "%d°", final_angle);
        lv_img_set_angle(meter->needle, angle * 10);
        if (e->code == LV_EVENT_RELEASED)
            motor_set_position(meter->slave, final_angle);
    }
}

static void meter_create(struct meter *meter)
{
    int max_ticks;

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
    lv_obj_add_flag(meter->needle, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(meter->needle, needle_cb,
                        LV_EVENT_ALL, meter);

    max_ticks = METER_TICKS;
    if (max_angle >= 360.0)
        max_ticks -= 1;
    for (int i = 0; i < max_ticks; i++)
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
                              "%d°", i * 30);
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
    lv_label_set_text(meter->label_val, "0°");
    lv_obj_center(meter->label_val);
    lv_obj_set_style_text_font(meter->label_val,
                               &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(meter->label_val,
                                lv_color_white(), LV_PART_MAIN);
}

void position_control_ui(lv_obj_t *cont_main, struct meter *meters, int mode)
{
    meters[0].cont = lv_obj_create(cont_main);
    lv_obj_remove_style_all(meters[0].cont);
    lv_obj_set_size(meters[0].cont, 580, 580);
    if (mode == 0)
        lv_obj_align(meters[0].cont, LV_ALIGN_TOP_LEFT,
                     86, 200);
    else
        lv_obj_align(meters[0].cont, LV_ALIGN_TOP_MID,
                     0, 200);
    meter_create(&meters[0]);

    meters[1].cont = lv_obj_create(cont_main);
    lv_obj_remove_style_all(meters[1].cont);
    lv_obj_set_size(meters[1].cont, 580, 580);
    if (mode == 0)
        lv_obj_align(meters[1].cont, LV_ALIGN_TOP_RIGHT,
                     -86, 200);
    else
        lv_obj_align(meters[1].cont, LV_ALIGN_BOTTOM_MID,
                     0, -200);
    meter_create(&meters[1]);
}

