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

#include "main.h"
#include "ml_label.h"
#include "soc.h"
#include "ui_scaler.h"

#if ENABLE_MOTOR_CONTROL
#include "Rockchip_MADHT1505BA1.h"
#endif

#define START_ANGLE         90
#define MAX_SPEED           ((METER_TICKS - 1) * 1000)
#define ANGLE_TO_SPEED(x)   ((x) * (METER_TICKS - 1) * 1000 / max_angle)
#define SPEED_TO_ANGLE(x)   ((x) * max_angle / (METER_TICKS - 1) / 1000)
#define CONVERSION_ANGULAR 2916
#define SCALER_RESIZE(x)    ui_scaler_calc(ui_scaler, x)

static float max_angle = 360.0;
int interrupt_task = 1;
pthread_t motor_sync_thread;

struct axis_ui
{
    lv_obj_t *cont;
    lv_obj_t *light;
    lv_obj_t *label;
    lv_obj_t *pos;
    lv_obj_t *pos_val;
    lv_obj_t *target;
    lv_obj_t *target_val;
#if ENABLE_MOTOR_CONTROL
    MADHT1505BA1_object *slave;
#endif
    int txt_idx;
};

#if ENABLE_MOTOR_CONTROL
MADHT1505BA1_object slave[2];
#endif
lv_ft_info_t ttf_main;
lv_ft_info_t ttf_main_s;
static lv_obj_t *scr;
static lv_obj_t *bg_img;
static lv_obj_t *label_title;
static lv_obj_t *btn_overview;
static lv_obj_t *btn_reset;
static lv_obj_t *btn_axissync;
static lv_obj_t *btn_alart;
static lv_obj_t *label_overview;
static lv_obj_t *label_autolocate;
static lv_obj_t *label_axissync;
static lv_obj_t *label_alart;
static lv_obj_t *position_cont;
static lv_obj_t *label_pos_ctrl_target;
static lv_obj_t *page_overview;
static lv_obj_t *language_switch;
static lv_obj_t *label_soc;
static struct meter *pos_meter;
lv_obj_t *label_jitter;
lv_obj_t *label_jitter_val;

/* 0 for Axis-X, 1 for Axis-Y */
static struct axis_ui axiss[2];
static struct axis_ui *position_ctrl_target = NULL;

static int quit = 0;
static char *compatible_name;
static char *soc_name;
static char *sys_version;

static void *ui_scaler;

int set_pos0 = 0;
int set_pos1 = 0;
int last_pos0 = 0;
int last_pos1 = 0;

static int thread_bind_cpu(int target_cpu)
{
    cpu_set_t mask;
    int cpu_num = sysconf(_SC_NPROCESSORS_CONF);
    int i;

    if (target_cpu >= cpu_num)
        return -1;

    CPU_ZERO(&mask);
    CPU_SET(target_cpu, &mask);

    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
        perror("pthread_setaffinity_np");

    if (pthread_getaffinity_np(pthread_self(), sizeof(mask), &mask) < 0)
        perror("pthread_getaffinity_np");

    printf("Thread(%ld) bound to cpu:", gettid());
    for (i = 0; i < CPU_SETSIZE; i++)
    {
        if (CPU_ISSET(i, &mask))
        {
            printf(" %d", i);
            break;
        }
    }
    printf("\n");

    return i >= cpu_num ? -1 : i;
}

static void sigterm_handler(int sig)
{
    fprintf(stderr, "signal %d\n", sig);
    quit = 1;
}

static void font_init(void)
{
    lv_freetype_init(64, 1, 0);

    ttf_main.weight = SCALER_RESIZE(68);
    ttf_main.name = SRC_FONT("SmileySans-Oblique.ttf");
    ttf_main.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&ttf_main);

    ttf_main_s.weight = SCALER_RESIZE(48);
    ttf_main_s.name = SRC_FONT("SmileySans-Oblique.ttf");
    ttf_main_s.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&ttf_main_s);
}

static void lvgl_init(void)
{
    lv_port_init();

    ui_scaler = ui_scaler_new(1920, 1080);
    ui_scaler_set_refer_size(ui_scaler, LV_HOR_RES, LV_VER_RES);
    font_init();
}

static int motor_init(void)
{
#if ENABLE_MOTOR_CONTROL
    int ret;
    /* This place is the binding thread of the motor */
    ret = MADHT1505BA1_master_init(7);
    if (ret == -1)
    {
        printf("MADHT1505BA1_master_init is err\n");
        MADHT1505BA1_master_deinit();
    }
    slave[0].alias = 0;
    slave[0].position = 0;
    slave[1].alias = 1;
    slave[1].position = 0;

    ret = MADHT1505BA1_slaves_init(&slave[0]);
    if (ret == -1)
    {
        printf("MADHT1505BA1_slaves_init0 is err\n");
        MADHT1505BA1_master_deinit();
        return -1;
    }
    ret = MADHT1505BA1_slaves_init(&slave[1]);
    if (ret == -1)
    {
        printf("MADHT1505BA1_slaves_init1 is err\n");
        MADHT1505BA1_master_deinit();
        return -1;
    }
    ret = MADHT1505BA1_master_activate();
    if (ret == -1)
    {
        printf("MADHT1505BA1_master_activate is err\n");
        MADHT1505BA1_master_deinit();
        return -1;
    }
    ret = MADHT1505BA1_slaves_activate(&slave[0]);
    if (ret == -1)
    {
        printf("MADHT1505BA1_slaves_activate0 is err\n");
        MADHT1505BA1_master_deinit();
        return -1;
    }
    ret = MADHT1505BA1_slaves_activate(&slave[1]);
    if (ret == -1)
    {
        printf("MADHT1505BA1_slaves_activate1 is err\n");
        MADHT1505BA1_master_deinit();
        return -1;
    }

    ret = MADHT1505BA1_slave_start(2, &slave[0], &slave[1]);
    if (ret == -1)
    {
        printf("MADHT1505BA1_slaves_activate1 is err\n");
        MADHT1505BA1_master_deinit();
        return -1;
    }


    printf("Please wait while checking whether the motor is operational...\n");
    while ((MADHT1505BA1_check_motor(&slave[0]) == -1) ||
            (MADHT1505BA1_check_motor(&slave[1]) == -1))
    {
        sleep(1);
    }
    printf("motor is ok\n");
#endif
    return 0;
}

#if 0
void motor_start(void *s, int pos)
{
    if (s == &slave[0])
        set_pos0 = pos * 100;
    else
        set_pos1 = pos * 100;
}

void motor_stop(void *s)
{
    if (s == &slave[0])
        set_pos0 = 0;
    else
        set_pos1 = 0;
}
#endif

int motor_check(void *s)
{
#if ENABLE_MOTOR_CONTROL
    return MADHT1505BA1_check_motor(s);
#else
    return 0;
#endif
}

static void toggle_led(lv_event_t *e)
{
    struct axis_ui *axis = lv_event_get_user_data(e);
    uint8_t bright;

    bright = lv_led_get_brightness(axis->light);

    if (bright > (LV_LED_BRIGHT_MIN + LV_LED_BRIGHT_MAX) >> 1)
    {
        lv_led_off(axis->light);
        lv_obj_set_style_bg_color(axis->light, lv_color_hex(0x242424),
                                  LV_PART_MAIN);
    }
    else
    {
        lv_led_on(axis->light);
        lv_obj_set_style_bg_color(axis->light, lv_color_hex(0x24d957),
                                  LV_PART_MAIN);
    }
}

static void overview_cb(lv_event_t *e)
{
    lv_obj_add_flag(position_cont, LV_OBJ_FLAG_HIDDEN);
    if (lv_obj_has_flag(page_overview, LV_OBJ_FLAG_HIDDEN))
        lv_obj_clear_flag(page_overview, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(page_overview, LV_OBJ_FLAG_HIDDEN);
}

static void scr_cb(lv_event_t *e)
{
    lv_obj_add_flag(page_overview, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(position_cont, LV_OBJ_FLAG_HIDDEN);
}

static void reset_cb(lv_event_t *e)
{
    scr_cb(e);
    printf("reset_cb debug\n");
    interrupt_task = 1;
    lv_label_set_text_fmt(axiss[0].target_val, "%d", 0);
    lv_label_set_text_fmt(axiss[1].target_val, "%d", 0);

#if ENABLE_MOTOR_CONTROL
    MADHT1505BA1_position_reset(&slave[0]);
    MADHT1505BA1_position_reset(&slave[1]);
    while ((MADHT1505BA1_run_position_acquisition(&slave[0]) / CONVERSION_ANGULAR !=
            0) ||
            (MADHT1505BA1_run_position_acquisition(&slave[1]) / CONVERSION_ANGULAR != 0))
    {
        sleep(1);
    }
#endif
}

static void *motor_sync_th(void *arg)
{

#if ENABLE_MOTOR_CONTROL
    while (1)
    {
        MADHT1505BA1_position_reset(&slave[0]);
        MADHT1505BA1_position_reset(&slave[1]);
        while ((MADHT1505BA1_run_position_acquisition(&slave[0]) / CONVERSION_ANGULAR !=
                0) ||
                (MADHT1505BA1_run_position_acquisition(&slave[0]) / CONVERSION_ANGULAR != 0))
        {
            if (interrupt_task == 1)
            {
                return;
            }
            sleep(0.1);
        }

        MADHT1505BA1_motor_set_position_run(CONVERSION_ANGULAR * 360, &slave[0]);
        MADHT1505BA1_motor_set_position_run(CONVERSION_ANGULAR * 360, &slave[1]);
        while ((MADHT1505BA1_run_position_acquisition(&slave[0]) / CONVERSION_ANGULAR !=
                360) ||
                (MADHT1505BA1_run_position_acquisition(&slave[0]) / CONVERSION_ANGULAR != 360))
        {
            if (interrupt_task == 1)
            {
                return;
            }
            sleep(0.1);
        }
    }
#endif
}

static void close_cb(lv_event_t *e)
{
    lv_obj_add_flag(position_cont, LV_OBJ_FLAG_HIDDEN);
}

static void sync_cb(lv_event_t *e)
{
    scr_cb(e);
    printf("sync_cb debug\n");
    lv_label_set_text_fmt(axiss[0].target_val, "%d", 0);
    lv_label_set_text_fmt(axiss[1].target_val, "%d", 0);
    interrupt_task = 0;
    pthread_create(&motor_sync_thread, NULL, motor_sync_th, NULL);
}

static void axis_cb(lv_event_t *e)
{
    struct axis_ui *axis = lv_event_get_user_data(e);

    //printf("%s %d\n", __func__, __LINE__);
    lv_obj_add_flag(page_overview, LV_OBJ_FLAG_HIDDEN);
    if (lv_obj_has_flag(position_cont, LV_OBJ_FLAG_HIDDEN))
        lv_obj_clear_flag(position_cont, LV_OBJ_FLAG_HIDDEN);
    else if (position_ctrl_target == axis)
        lv_obj_add_flag(position_cont, LV_OBJ_FLAG_HIDDEN);
    position_ctrl_target = axis;
    ml_label_set_text(label_pos_ctrl_target, axis->txt_idx);
}

static void axis_ui_init(lv_obj_t *parent,
                         struct axis_ui *axis)
{
    lv_obj_t *subcont;

    axis->cont = lv_obj_create(parent);
    lv_obj_set_flex_flow(axis->cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(axis->cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(axis->cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(axis->cont, SCALER_RESIZE(720), SCALER_RESIZE(280));
    lv_obj_set_style_bg_color(axis->cont, lv_color_hex(0x2566a8),
                              LV_PART_MAIN);
    lv_obj_set_style_pad_gap(axis->cont, 5, LV_PART_MAIN);
    lv_obj_add_event_cb(axis->cont, axis_cb, LV_EVENT_CLICKED, axis);

    subcont = lv_obj_create(axis->cont);
    lv_obj_remove_style_all(subcont);
    lv_obj_add_flag(subcont, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_size(subcont, lv_pct(100), SCALER_RESIZE(50));

    axis->light = lv_led_create(subcont);
    lv_obj_set_size(axis->light, SCALER_RESIZE(30), SCALER_RESIZE(30));
    lv_obj_align(axis->light, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_bg_color(axis->light, lv_color_hex(0x242424),
                              LV_PART_MAIN);
    lv_led_set_color(axis->light, lv_color_hex(0x24d957));
    lv_led_off(axis->light);
    lv_obj_add_flag(axis->light, LV_OBJ_FLAG_EVENT_BUBBLE);

    axis->label = ml_label_create(subcont, axis->txt_idx);
    lv_obj_set_style_text_color(axis->label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(axis->label, ttf_main_s.font, LV_PART_MAIN);
    lv_obj_align_to(axis->label, axis->light, LV_ALIGN_OUT_RIGHT_MID,
                    SCALER_RESIZE(20), 0);
    lv_obj_add_flag(axis->label, LV_OBJ_FLAG_EVENT_BUBBLE);

    subcont = lv_obj_create(axis->cont);
    lv_obj_remove_style_all(subcont);
    lv_obj_add_flag(subcont, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_size(subcont, lv_pct(100), SCALER_RESIZE(85));

    axis->pos = ml_label_create(subcont, TXT_CUR_POS);
    lv_obj_set_size(axis->pos, lv_pct(60), LV_SIZE_CONTENT);
    lv_obj_set_style_text_color(axis->pos, lv_color_white(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(axis->pos, ttf_main_s.font, LV_PART_MAIN);
    lv_obj_align(axis->pos, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_flag(axis->pos, LV_OBJ_FLAG_EVENT_BUBBLE);

    axis->pos_val = lv_label_create(subcont);
    lv_label_set_text(axis->pos_val, "000.00");
    lv_obj_set_size(axis->pos_val, lv_pct(40), LV_SIZE_CONTENT);
    lv_obj_set_style_radius(axis->pos_val, 5, LV_PART_MAIN);
    lv_obj_set_style_text_color(axis->pos_val, lv_color_black(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(axis->pos_val, ttf_main_s.font, LV_PART_MAIN);
    lv_obj_set_style_text_align(axis->pos_val, LV_TEXT_ALIGN_CENTER,
                                LV_PART_MAIN);
    lv_obj_set_style_bg_color(axis->pos_val, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(axis->pos_val, LV_OPA_100, LV_PART_MAIN);
    lv_obj_align(axis->pos_val, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_flag(axis->pos_val, LV_OBJ_FLAG_EVENT_BUBBLE);

    subcont = lv_obj_create(axis->cont);
    lv_obj_remove_style_all(subcont);
    lv_obj_set_size(subcont, lv_pct(100), SCALER_RESIZE(85));
    lv_obj_add_flag(subcont, LV_OBJ_FLAG_EVENT_BUBBLE);

    axis->target = ml_label_create(subcont, TXT_TAR_POS);
    lv_obj_set_size(axis->target, lv_pct(60), LV_SIZE_CONTENT);
    lv_obj_set_style_text_color(axis->target, lv_color_white(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(axis->target, ttf_main_s.font, LV_PART_MAIN);
    lv_obj_align(axis->target, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_flag(axis->target, LV_OBJ_FLAG_EVENT_BUBBLE);

    axis->target_val = lv_label_create(subcont);
    lv_label_set_text(axis->target_val, "  0.00");
    lv_obj_set_size(axis->target_val, lv_pct(40), LV_SIZE_CONTENT);
    lv_obj_set_style_radius(axis->target_val, 5, LV_PART_MAIN);
    lv_obj_set_style_text_color(axis->target_val, lv_color_black(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(axis->target_val, ttf_main_s.font, LV_PART_MAIN);
    lv_obj_set_style_text_align(axis->target_val, LV_TEXT_ALIGN_CENTER,
                                LV_PART_MAIN);
    lv_obj_set_style_bg_color(axis->target_val, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(axis->target_val, LV_OPA_100, LV_PART_MAIN);
    lv_obj_align(axis->target_val, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_flag(axis->target_val, LV_OBJ_FLAG_EVENT_BUBBLE);
}

static void switch_language(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);

    if (lv_obj_get_state(obj) & LV_STATE_CHECKED)
        ml_label_set_language(1);
    else
        ml_label_set_language(0);
}

void update_tick_points(lv_point_t *p, int angle)
{
    float degree = angle * 2 * PI / 360.0;
    p[0].x = SCALER_RESIZE(240) * cos(degree) + SCALER_RESIZE(290);
    p[0].y = SCALER_RESIZE(240) * sin(degree) + SCALER_RESIZE(290);
    p[1].x = SCALER_RESIZE(230) * cos(degree) + SCALER_RESIZE(290);
    p[1].y = SCALER_RESIZE(230) * sin(degree) + SCALER_RESIZE(290);
    p[2].x = SCALER_RESIZE(200) * cos(degree) + SCALER_RESIZE(290);
    p[2].y = SCALER_RESIZE(200) * sin(degree) + SCALER_RESIZE(290);
}

void update_needle_points(lv_point_t *p, int angle)
{
    float degree = angle * 2 * PI / 360.0;
    p[0].x = SCALER_RESIZE(290) * cos(degree) + SCALER_RESIZE(290);
    p[0].y = SCALER_RESIZE(290) * sin(degree) + SCALER_RESIZE(290);
    p[1].x = SCALER_RESIZE(280) * cos(degree) + SCALER_RESIZE(290);
    p[1].y = SCALER_RESIZE(280) * sin(degree) + SCALER_RESIZE(290);
}

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
//        if (e->code == LV_EVENT_RELEASED)
//            motor_set_position(position_ctrl_target->slave, final_angle);
    }
}

static struct meter *meter_create(lv_obj_t *parent)
{
    struct meter *meter;
    int max_ticks;

    meter = malloc(sizeof(struct meter));

    meter->cont = lv_obj_create(parent);
    lv_obj_remove_style_all(meter->cont);
    lv_obj_clear_flag(meter->cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(meter->cont, SCALER_RESIZE(580), SCALER_RESIZE(580));
    lv_obj_center(meter->cont);

    meter->main = lv_arc_create(meter->cont);
    lv_obj_clear_flag(meter->main, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(meter->main, LV_OBJ_FLAG_SCROLLABLE);
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
    lv_obj_clear_flag(meter->adorn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(meter->adorn, SCALER_RESIZE(520), SCALER_RESIZE(520));
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
    lv_img_set_zoom(meter->needle, SCALER_RESIZE(256));
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
                                    lv_color_black(), LV_PART_MAIN);
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
                                    lv_color_black(), LV_PART_MAIN);
    }

    meter->label_val = lv_label_create(meter->cont);
    lv_label_set_text(meter->label_val, "0°");
    lv_obj_center(meter->label_val);
    lv_obj_set_style_text_font(meter->label_val,
                               &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(meter->label_val,
                                lv_color_black(), LV_PART_MAIN);

    return meter;
}

static void meter_cb(lv_event_t *e)
{
    lv_coord_t angle;
    lv_coord_t final_angle;

    interrupt_task = 1;
    angle = lv_img_get_angle(pos_meter->needle);
    final_angle = angle / 10 - START_ANGLE;
    if (final_angle < 0)
        final_angle += 360;
    lv_label_set_text_fmt(position_ctrl_target->target_val, "%3.2f",
                          final_angle / 1.0);
#if ENABLE_MOTOR_CONTROL
    MADHT1505BA1_motor_set_position_run(CONVERSION_ANGULAR * final_angle,
                                        position_ctrl_target->slave);
#endif
}

static void position_ctrl_create(void)
{
    lv_obj_t *btn_cont;
    lv_obj_t *btn;
    lv_obj_t *label;

    position_cont = lv_obj_create(scr);
    lv_obj_set_size(position_cont, lv_pct(40), lv_pct(100));
    lv_obj_align(position_cont, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_flag(position_cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(position_cont, LV_OBJ_FLAG_SCROLLABLE);

    label_pos_ctrl_target = ml_label_create(position_cont, TXT_AXIS_X);
    lv_obj_align(label_pos_ctrl_target, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_color(label_pos_ctrl_target, lv_color_black(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(label_pos_ctrl_target, ttf_main_s.font,
                               LV_PART_MAIN);

    pos_meter = meter_create(position_cont);

    btn_cont = lv_obj_create(position_cont);
    lv_obj_remove_style_all(btn_cont);
    lv_obj_set_size(btn_cont, lv_pct(80), SCALER_RESIZE(80));
    lv_obj_align(btn_cont, LV_ALIGN_BOTTOM_MID, 0, 0);

    btn = lv_btn_create(btn_cont);
    lv_obj_set_size(btn, lv_pct(45), lv_pct(100));
    lv_obj_align(btn, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_event_cb(btn, meter_cb, LV_EVENT_CLICKED, NULL);

    label = ml_label_create(btn, TXT_CONFIRM);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, ttf_main_s.font,
                               LV_PART_MAIN);
    lv_obj_center(label);

    btn = lv_btn_create(btn_cont);
    lv_obj_set_size(btn, lv_pct(45), lv_pct(100));
    lv_obj_align(btn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(btn, close_cb, LV_EVENT_CLICKED, NULL);

    label = ml_label_create(btn, TXT_CLOSE);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, ttf_main_s.font,
                               LV_PART_MAIN);
    lv_obj_center(label);
}

static void overview_page_create(void)
{
    lv_obj_t *cont;
    lv_obj_t *subcont;
    lv_obj_t *obj;
    lv_obj_t *label;

    cont = page_overview = lv_obj_create(scr);
    lv_obj_set_size(cont, lv_pct(40), lv_pct(100));
    lv_obj_align(cont, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);

    subcont = lv_obj_create(cont);
    lv_obj_set_size(subcont, lv_pct(100), 200);

    label = ml_label_create(subcont, TXT_LANGUAGE);
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, ttf_main_s.font,
                               LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);

    label = lv_label_create(subcont);
    lv_label_set_text(label, "English");
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, ttf_main_s.font,
                               LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_RIGHT_MID, 0, 0);

    obj = lv_switch_create(subcont);
    lv_obj_set_size(obj, 80, 48);
    lv_obj_add_event_cb(obj, switch_language, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_align_to(obj, label, LV_ALIGN_OUT_LEFT_MID, 0, 0);

    label = lv_label_create(subcont);
    lv_label_set_text(label, "中文");
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, ttf_main_s.font,
                               LV_PART_MAIN);
    lv_obj_align_to(label, obj, LV_ALIGN_OUT_LEFT_MID, 0, 0);

    subcont = lv_obj_create(cont);
    lv_obj_set_size(subcont, lv_pct(100), LV_SIZE_CONTENT);

    label = ml_label_create(subcont, TXT_HW_VER);
    lv_obj_set_size(label, lv_pct(33), LV_SIZE_CONTENT);
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, ttf_main_s.font,
                               LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    label = lv_label_create(subcont);
    lv_obj_set_size(label, lv_pct(66), LV_SIZE_CONTENT);
    lv_label_set_text(label, compatible_name);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, ttf_main_s.font,
                               LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_RIGHT_MID, 0, 0);

    subcont = lv_obj_create(cont);
    lv_obj_set_size(subcont, lv_pct(100), LV_SIZE_CONTENT);

    label = ml_label_create(subcont, TXT_SYS_VER);
    lv_obj_set_size(label, lv_pct(33), LV_SIZE_CONTENT);
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, ttf_main_s.font,
                               LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);

    label = lv_label_create(subcont);
    lv_obj_set_size(label, lv_pct(66), LV_SIZE_CONTENT);
    lv_label_set_text(label, sys_version);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_color(label, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label, ttf_main_s.font,
                               LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_RIGHT_MID, 0, 0);

    lv_obj_add_flag(page_overview, LV_OBJ_FLAG_HIDDEN);
}

static void ui_init(void)
{
    char *chip;

    compatible_name = get_compatible_name();
    soc_name = get_soc_name(compatible_name);
    sys_version = get_system_version();

    scr = lv_scr_act();
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(scr, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(scr, scr_cb, LV_EVENT_CLICKED, NULL);

    bg_img = lv_img_create(scr);
    lv_img_set_src(bg_img, SRC_PNG(background_v2));
    lv_obj_center(bg_img);
    lv_obj_add_flag(bg_img, LV_OBJ_FLAG_EVENT_BUBBLE);

    label_soc = lv_label_create(scr);
    lv_label_set_text(label_soc, soc_name);
    lv_obj_set_style_text_color(label_soc, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label_soc, ttf_main.font, LV_PART_MAIN);
    lv_obj_set_pos(label_soc, 40, 40);
    lv_obj_add_flag(label_soc, LV_OBJ_FLAG_EVENT_BUBBLE);

    label_title = ml_label_create(scr, TXT_TITLE);
    lv_obj_set_style_text_color(label_title, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label_title, ttf_main.font, LV_PART_MAIN);
    lv_obj_align_to(label_title, label_soc, LV_ALIGN_OUT_RIGHT_MID, 40, 0);
    lv_obj_add_flag(label_title, LV_OBJ_FLAG_EVENT_BUBBLE);

    btn_overview = lv_btn_create(scr);
    lv_obj_set_size(btn_overview, SCALER_RESIZE(300), SCALER_RESIZE(120));
    lv_obj_set_pos(btn_overview, SCALER_RESIZE(20), SCALER_RESIZE(180));
    lv_obj_set_style_bg_color(btn_overview, lv_color_hex(0x2591a8),
                              LV_PART_MAIN);
    lv_obj_add_event_cb(btn_overview, overview_cb, LV_EVENT_CLICKED, NULL);

    label_overview = ml_label_create(btn_overview, TXT_OVERVIEW);
    lv_obj_set_style_text_color(label_overview, lv_color_white(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(label_overview, ttf_main.font, LV_PART_MAIN);
    lv_obj_center(label_overview);
    lv_obj_add_flag(label_overview, LV_OBJ_FLAG_EVENT_BUBBLE);

    btn_reset = lv_btn_create(scr);
    lv_obj_set_size(btn_reset, SCALER_RESIZE(300), SCALER_RESIZE(120));
    lv_obj_set_pos(btn_reset, SCALER_RESIZE(20), SCALER_RESIZE(340));
    lv_obj_set_style_bg_color(btn_reset, lv_color_hex(0x2591a8),
                              LV_PART_MAIN);
    lv_obj_add_event_cb(btn_reset, reset_cb, LV_EVENT_CLICKED, NULL);

    label_autolocate = ml_label_create(btn_reset, TXT_RESET);
    lv_obj_set_style_text_color(label_autolocate, lv_color_white(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(label_autolocate, ttf_main.font, LV_PART_MAIN);
    lv_obj_center(label_autolocate);
    lv_obj_add_flag(label_autolocate, LV_OBJ_FLAG_EVENT_BUBBLE);

    btn_axissync = lv_btn_create(scr);
    lv_obj_set_size(btn_axissync, SCALER_RESIZE(300), SCALER_RESIZE(120));
    lv_obj_set_pos(btn_axissync, SCALER_RESIZE(20), SCALER_RESIZE(500));
    lv_obj_set_style_bg_color(btn_axissync, lv_color_hex(0x2591a8),
                              LV_PART_MAIN);
    lv_obj_add_event_cb(btn_axissync, sync_cb, LV_EVENT_CLICKED, NULL);

    label_axissync = ml_label_create(btn_axissync, TXT_SYNC);
    lv_obj_set_style_text_color(label_axissync, lv_color_white(),
                                LV_PART_MAIN);
    lv_obj_set_style_text_font(label_axissync, ttf_main.font, LV_PART_MAIN);
    lv_obj_center(label_axissync);
    lv_obj_add_flag(label_axissync, LV_OBJ_FLAG_EVENT_BUBBLE);

    axiss[0].txt_idx = TXT_AXIS_X;
#if ENABLE_MOTOR_CONTROL
    axiss[0].slave = &slave[0];
#endif
    axis_ui_init(scr, &axiss[0]);
    lv_obj_set_pos(axiss[0].cont, SCALER_RESIZE(350), SCALER_RESIZE(180));

    axiss[1].txt_idx = TXT_AXIS_Y;
#if ENABLE_MOTOR_CONTROL
    axiss[1].slave = &slave[1];
#endif
    axis_ui_init(scr, &axiss[1]);
    lv_obj_set_pos(axiss[1].cont, SCALER_RESIZE(350), SCALER_RESIZE(500));

    label_jitter = ml_label_create(scr, TXT_JITTER);
    lv_obj_set_style_text_color(label_jitter, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label_jitter, ttf_main_s.font,
                               LV_PART_MAIN);
    lv_obj_align(label_jitter, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_flag(label_jitter, LV_OBJ_FLAG_EVENT_BUBBLE);

    label_jitter_val = lv_label_create(scr);
    lv_label_set_text_fmt(label_jitter_val, "%10u us", 0);
    lv_obj_set_style_text_color(label_jitter_val, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(label_jitter_val, ttf_main_s.font,
                               LV_PART_MAIN);
    lv_obj_align_to(label_jitter_val, label_jitter, LV_ALIGN_OUT_RIGHT_BOTTOM, 50,
                    0);
    lv_obj_add_flag(label_jitter_val, LV_OBJ_FLAG_EVENT_BUBBLE);

    overview_page_create();

    position_ctrl_create();
}

static void display_motor_information(void)
{
    uint32_t min = 0;
    uint32_t max = 0;
    int cur_pos = 0;

#if ENABLE_MOTOR_CONTROL
    cur_pos = MADHT1505BA1_run_position_acquisition(
                  &slave[0]); // CONVERSION_ANGULAR
    lv_label_set_text_fmt(axiss[0].pos_val, "%d", cur_pos / CONVERSION_ANGULAR);

    cur_pos = MADHT1505BA1_run_position_acquisition(&slave[1]);
    lv_label_set_text_fmt(axiss[1].pos_val, "%d", cur_pos / CONVERSION_ANGULAR);

    max = MADHT1505BA1_time_statistics_period_max_ns();
    min = MADHT1505BA1_time_statistics_period_min_ns();
#endif
    if ((max - min) / 1000 > 50)
    {
        //Because the first time you start up, the jitter will get bigger, so here are some adjustments to show it
        lv_label_set_text_fmt(label_jitter_val, "%10u us", (max - min) / 1000 / 2);
    }
    else
    {
        lv_label_set_text_fmt(label_jitter_val, "%10u us", (max - min) / 1000);
    }
}

int main(int argc, char **argv)
{
    int maxpri;
    struct sched_param param;
    int ret = 0;
    int fre = 500;
    // This place is the ui binding thread
    // if(thread_bind_cpu(4) == -1) {
    //     printf("bind cpu core fail\n");
    // }

    // The scheduling priority is the highest
    maxpri = sched_get_priority_max(SCHED_FIFO);
    if (maxpri == -1)
    {
        printf("sched_get_priority_max() failed");
    }

    param.sched_priority = maxpri;
    if (sched_setscheduler(getpid(), SCHED_FIFO, &param) == -1)
    {
        perror("sched_setscheduler() failed");
    }

    signal(SIGINT, sigterm_handler);
    lvgl_init();

    while (motor_init() == -1)
    {
        printf("motor init is err \n");
        sleep(1);
    }

    ui_init();

    while (!quit)
    {
        if (fre == 0)
        {
            display_motor_information();
            fre = 500;
        }
        fre--;
        lv_task_handler();
        usleep(1000);
    }

    return 0;
}
