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

#if ENABLE_MOTOR_CONTROL
#include "Rockchip_MADHT1505BA1.h"
MADHT1505BA1_object slave[2];
#endif

lv_ft_info_t ttf_main;
lv_ft_info_t ttf_main_s;
static struct meter meters[4];
static lv_obj_t *scr;
static lv_obj_t *bg_img;
static lv_obj_t *cont_speed;
static lv_obj_t *cont_position;
static lv_obj_t *mode_label;
static lv_obj_t *mode_switch;
static lv_anim_t slide_in;
static lv_anim_t slide_out;

static int g_indev_rotation = 0;
static int g_disp_rotation = LV_DISP_ROT_90;

static int quit = 0;

int set_speed0 = 0;
int set_speed1 = 0;
int last_speed0 = 0;
int last_speed1 = 0;

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

    ttf_main.weight = 68;
    ttf_main.name = SRC_FONT("SmileySans-Oblique.ttf");
    ttf_main.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&ttf_main);

    ttf_main_s.weight = 48;
    ttf_main_s.name = SRC_FONT("SmileySans-Oblique.ttf");
    ttf_main_s.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&ttf_main_s);
}

static void lvgl_init(void)
{
    lv_port_init();

    font_init();
}

static int motor_init(void)
{
#if ENABLE_MOTOR_CONTROL
    int ret;
    ret = MADHT1505BA1_master_init(3); //bind cpu core 3
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
    while ((MADHT1505BA1_check_motor(&slave[0]) == -1)
            || (MADHT1505BA1_check_motor(&slave[1]) == -1))
    {
        sleep(1);
    }
    printf("motor is ok\n");
#endif
    return 0;
}

void motor_start(void *s, int speed)
{
#if ENABLE_MOTOR_CONTROL
    if (s == &slave[0])
        set_speed0 = speed * 100;
    else
        set_speed1 = speed * 100;
#endif
}

void motor_stop(void *s)
{
#if ENABLE_MOTOR_CONTROL
    if (s == &slave[0])
        set_speed0 = 0;
    else
        set_speed1 = 0;
#endif
}

int motor_check(void *s)
{
#if ENABLE_MOTOR_CONTROL
    return MADHT1505BA1_check_motor(s);
#else
    return 0;
#endif
}

void motor_set_position(void *s, int angle)
{
}

void update_tick_points(lv_point_t *p, int angle)
{
    float degree = angle * 2 * PI / 360.0;
    p[0].x = 240 * cos(degree) + 290;
    p[0].y = 240 * sin(degree) + 290;
    p[1].x = 230 * cos(degree) + 290;
    p[1].y = 230 * sin(degree) + 290;
    p[2].x = 200 * cos(degree) + 290;
    p[2].y = 200 * sin(degree) + 290;
}

void update_needle_points(lv_point_t *p, int angle)
{
    float degree = angle * 2 * PI / 360.0;
    p[0].x = 290 * cos(degree) + 290;
    p[0].y = 290 * sin(degree) + 290;
    p[1].x = 280 * cos(degree) + 290;
    p[1].y = 280 * sin(degree) + 290;
}

static void anim_x_cb(void *var, int32_t v)
{
    lv_obj_set_x(cont_speed, lv_pct(-v));
    lv_obj_set_x(cont_position, lv_pct(100 - v));
}

static void anim_end_cb(lv_anim_t *anim)
{
    lv_obj_add_flag(mode_switch, LV_OBJ_FLAG_CLICKABLE);
}

static void switch_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        if (lv_obj_has_state(obj, LV_STATE_CHECKED))
            lv_anim_start(&slide_in);
        else
            lv_anim_start(&slide_out);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    }
}

static void ui_init(void)
{
    int mode = 0;

    scr = lv_scr_act();
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    if (lv_obj_get_width(scr) >
            lv_obj_get_height(scr))
    {
        mode = 0;
    }
    else
    {
        mode = 1;
    }

    bg_img = lv_img_create(scr);
    if (mode == 0)
        lv_img_set_src(bg_img, SRC_PNG(background));
    else
        lv_img_set_src(bg_img, SRC_PNG(background_ver));
    lv_obj_center(bg_img);

#if ENABLE_MOTOR_CONTROL
    meters[0].slave = &slave[0];
    meters[1].slave = &slave[1];
    meters[2].slave = &slave[0];
    meters[3].slave = &slave[1];
#endif

    cont_speed = lv_obj_create(scr);
    lv_obj_remove_style_all(cont_speed);
    lv_obj_set_size(cont_speed, lv_pct(100), lv_pct(100));
    lv_obj_center(cont_speed);
    speed_control_ui(cont_speed, &meters[0], mode);

#if ENABLE_POSITION_CONTROL
    cont_position = lv_obj_create(scr);
    lv_obj_remove_style_all(cont_position);
    lv_obj_set_size(cont_position, lv_pct(100), lv_pct(100));
    lv_obj_center(cont_position);
    lv_obj_set_x(cont_position, lv_pct(100));

    position_control_ui(cont_position, &meters[2], mode);

    mode_label = lv_label_create(scr);
    lv_obj_align(mode_label, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_label_set_text(mode_label, "位置控制");
    lv_obj_set_style_text_color(mode_label,
                                lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(mode_label,
                               ttf_main.font, LV_PART_MAIN);

    mode_switch = lv_switch_create(scr);
    lv_obj_set_size(mode_switch, 100, 50);
    lv_obj_align_to(mode_switch, mode_label,
                    LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(mode_switch, switch_cb,
                        LV_EVENT_VALUE_CHANGED, NULL);

    lv_anim_init(&slide_in);
    lv_anim_set_values(&slide_in, 0, 100);
    lv_anim_set_time(&slide_in, 300);
    lv_anim_set_exec_cb(&slide_in, anim_x_cb);
    lv_anim_set_deleted_cb(&slide_in, anim_end_cb);

    lv_anim_init(&slide_out);
    lv_anim_set_values(&slide_out, 100, 0);
    lv_anim_set_time(&slide_out, 300);
    lv_anim_set_exec_cb(&slide_out, anim_x_cb);
    lv_anim_set_deleted_cb(&slide_out, anim_end_cb);
#endif
}

static void display_motor_information(void)
{
    uint32_t min = 0;
    uint32_t max = 0;
    int cur_speed = 0;

#if ENABLE_MOTOR_CONTROL
    cur_speed = MADHT1505BA1_check_motor(&slave[0]);
    if (cur_speed != -1)
    {
        if (last_speed0 != set_speed0)
        {
            MADHT1505BA1_motor_start(&slave[0], set_speed0);
            last_speed0 = set_speed0;
        }
    }

    cur_speed = MADHT1505BA1_check_motor(&slave[1]);
    if (cur_speed != -1)
    {
        if (last_speed1 != set_speed1)
        {
            MADHT1505BA1_motor_start(&slave[1], set_speed1);
            last_speed1 = set_speed1;
        }
    }
#endif
    lv_label_set_text_fmt(label_jitter,
                          "Jitter:\nMax: %10u\nMin: %10u", max, min);
}

int main(int argc, char **argv)
{
    int maxpri;
    struct sched_param param;
    int ret = 0;
    int fre = 500;
    if (thread_bind_cpu(1) == -1)
    {
        printf("bind cpu core fail\n");
    }

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
