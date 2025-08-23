/*
 * Copyright (c) 2023 Rockchip, Inc. All Rights Reserved.
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

#include <fcntl.h>
#include <lvgl/lvgl.h>
#include <lvgl/lv_conf.h>
#include <sched.h>
#include <pthread.h>


#include "main.h"
#include "home_ui.h"
#include "ui_resource.h"

#if ROCKIT_EN
#include "rk_defines.h"
#include "rk_mpi_sys.h"
#endif

#include "wifibt.h"

lv_ft_info_t ttf_main_s;
lv_ft_info_t ttf_main_m;
lv_ft_info_t ttf_main_l;

lv_style_t style_txt_s;
lv_style_t style_txt_m;
lv_style_t style_txt_l;

lv_dir_t scr_dir;
lv_coord_t scr_w;
lv_coord_t scr_h;

static int quit = 0;

#if USE_SENSOR
#define SYS_NODE    "/sys/devices/platform/backlight/backlight/backlight/brightness"
static lv_timer_t *lsensor_timer;
static lv_timer_t *psensor_timer;
static lv_timer_t *backlight_timer;
/* light sensor level from 0 to 7 */
static int brightness[8] = {45, 75, 105, 135, 165, 195, 225, 255};
static int backlight_en = 1;
static int backlight_level = ARRAY_SIZE(brightness) - 1;
static int backlight_fd = -1;
#endif
static int backlight_timeout = 5000;
static int start_tick = 0;

extern void rk_demo_init(void);

void backlight_set_timeout(int timeout)
{
    backlight_timeout = timeout;
}

static void sigterm_handler(int sig)
{
    fprintf(stderr, "signal %d\n", sig);
    quit = 1;
}

static void check_scr(void)
{
    scr_w = LV_HOR_RES;
    scr_h = LV_VER_RES;

    if (scr_w > scr_h)
        scr_dir = LV_DIR_HOR;
    else
        scr_dir = LV_DIR_VER;

    printf("%s %dx%d\n", __func__, scr_w, scr_h);
}

#if USE_SENSOR
static void touchpad_feedback(lv_indev_drv_t *drv, uint8_t code)
{
    if ((code >= LV_EVENT_PRESSED) && (code <= LV_EVENT_RELEASED))
    {
        start_tick = lv_tick_get();
    }
}

static int level_to_brightness(int level)
{
    if (level < 0)
        level = 0;
    if (level >= ARRAY_SIZE(brightness))
        level > ARRAY_SIZE(brightness) - 1;

    return brightness[level];
}

static void update_backlight(int en, int value)
{
    static uint8_t last_value = 0;
    uint8_t final_value;
    char val[8];

    if (backlight_fd <= 0)
        return;

    if (en)
    {
        final_value = value;
    }
    else
    {
        final_value = 0;
    }
    if (last_value == final_value)
        return;

    if ((!!last_value) != (!!final_value))
    {
        /* Switch all indev state */
        lv_indev_enable(NULL, !!final_value);
    }
    last_value = final_value;

    snprintf(val, sizeof(val), "%d", final_value);
    if (write(backlight_fd, &val, strlen(val)) <= 0)
        LV_LOG_ERROR("update backlight failed");
}

static void backlight_cb(lv_timer_t *timer)
{
    static int cur_en = 1;
    static int cur_value = 255;
    int value_step;
    int target_value;

    target_value = level_to_brightness(backlight_level);
    if (cur_value != target_value)
    {
        if (cur_value > target_value)
        {
            value_step = (int)((cur_value - target_value) / 2.0);
            if (value_step == 0)
                value_step = 2;
            if ((cur_value - value_step) < target_value)
                cur_value = target_value;
            else
                cur_value -= value_step;
        }
        else
        {
            value_step = (int)((target_value - cur_value) / 2.0);
            if (value_step == 0)
                value_step = 2;
            if ((cur_value + value_step) > target_value)
                cur_value = target_value;
            else
                cur_value += value_step;
        }
    }

    if (cur_en != backlight_en)
    {
        if (cur_en == 0)
        {
            cur_en = backlight_en;
        }
        else if (backlight_timeout > 0)
        {
            if (start_tick == 0)
                start_tick = lv_tick_get();
            else if (lv_tick_elaps(start_tick) > backlight_timeout)
                cur_en = backlight_en;
        }
    }
    else
    {
        start_tick = 0;
    }

    update_backlight(cur_en, cur_value);

    if (!cur_en)
    {
        lv_indev_t *indev;
        lv_indev_data_t data;
        uint32_t tick;

        indev = lv_indev_get_next(NULL);
        while (indev)
        {
            _lv_indev_read(indev, &data);
            if (data.state == LV_INDEV_STATE_PRESSED)
            {
                /* Wait release */
                tick = lv_tick_get();
                do
                {
                    _lv_indev_read(indev, &data);
                    if (lv_tick_elaps(tick) > 10000)
                    {
                        LV_LOG_WARN("Long-press 10s timeout");
                        break;
                    }
                }
                while (data.state == LV_INDEV_STATE_PRESSED);

                cur_en = 1;
                update_backlight(cur_en, cur_value);
                break;
            }
            indev = lv_indev_get_next(indev);
        }
    }
}

static void lsensor_cb(lv_timer_t *timer)
{
    lv_indev_drv_t *drv = (lv_indev_drv_t *)timer->user_data;
    lv_indev_data_t data;

    drv->read_cb(drv, &data);
    if (data.continue_reading)
        return;
    LV_LOG_TRACE("%d %u", data.state, data.key);
    backlight_level = data.key;
}

static void psensor_cb(lv_timer_t *timer)
{
    lv_indev_drv_t *drv = (lv_indev_drv_t *)timer->user_data;
    lv_indev_data_t data;

    memset(&data, 0, sizeof(data));
    drv->read_cb(drv, &data);
    if (data.continue_reading)
        return;
    LV_LOG_TRACE("%d", data.state);
    backlight_en = data.state;
}
#endif

static void lvgl_init(void)
{
    lv_port_init();

#if USE_SENSOR
    lv_indev_drv_t *lsensor, *psensor;
    lsensor = lv_port_indev_get_lsensor_drv();
    psensor = lv_port_indev_get_psensor_drv();
    if ((lsensor && lsensor->read_cb) &&
            (psensor && psensor->read_cb))
    {
        lsensor_timer = lv_timer_create(lsensor_cb, 100, lsensor);
        psensor_timer = lv_timer_create(psensor_cb, 100, psensor);
        backlight_timer = lv_timer_create(backlight_cb, 50, NULL);
        backlight_fd = open(SYS_NODE, O_RDWR);
        if (backlight_fd <= 0)
            LV_LOG_ERROR("open backlight node failed");
        update_backlight(backlight_en, brightness[backlight_level]);
        backlight_en = 0;

        lv_indev_t *indev = lv_indev_get_next(NULL);
        while (indev)
        {
            lv_indev_drv_t *indev_drv = indev->driver;
            indev_drv->feedback_cb = touchpad_feedback;
            indev = lv_indev_get_next(indev);
        }
    }
#endif

    check_scr();
}

static void font_init(void)
{
    lv_freetype_init(64, 1, 0);

    if (scr_dir == LV_DIR_HOR)
    {
        ttf_main_s.weight = ALIGN(RK_PCT_W(2), 2);
        ttf_main_m.weight = ALIGN(RK_PCT_W(3), 2);
        ttf_main_l.weight = ALIGN(RK_PCT_W(9), 2);
    }
    else
    {
        ttf_main_s.weight = ALIGN(RK_PCT_H(2), 2);
        ttf_main_m.weight = ALIGN(RK_PCT_H(3), 2);
        ttf_main_l.weight = ALIGN(RK_PCT_H(9), 2);
    }

    printf("%s s %d m %d l %d\n", __func__,
           ttf_main_s.weight, ttf_main_m.weight, ttf_main_l.weight);

    ttf_main_s.name = MAIN_FONT;
    ttf_main_s.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&ttf_main_s);

    ttf_main_m.name = MAIN_FONT;
    ttf_main_m.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&ttf_main_m);

    ttf_main_l.name = MAIN_FONT;
    ttf_main_l.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&ttf_main_l);
}

static void style_init(void)
{
    lv_style_init(&style_txt_s);
    lv_style_set_text_font(&style_txt_s, ttf_main_s.font);
    lv_style_set_text_color(&style_txt_s, lv_color_black());

    lv_style_init(&style_txt_m);
    lv_style_set_text_font(&style_txt_m, ttf_main_m.font);
    lv_style_set_text_color(&style_txt_m, lv_color_black());

    lv_style_init(&style_txt_l);
    lv_style_set_text_font(&style_txt_l, ttf_main_l.font);
    lv_style_set_text_color(&style_txt_l, lv_color_black());
}

void app_init(void)
{
    font_init();
    style_init();
}

int main(int argc, char **argv)
{
    signal(SIGINT, sigterm_handler);

    struct sched_param param;
    int max_priority;

    max_priority = sched_get_priority_max(SCHED_FIFO);

    param.sched_priority = max_priority;

    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1)
    {
        perror("sched_setscheduler failed");
    }

#if ROCKIT_EN
    RK_MPI_SYS_Init();
#endif
#if WIFIBT_EN
    run_wifibt_server();
#endif

    lvgl_init();

    app_init();

    rk_demo_init();

    while (!quit)
    {
        lv_task_handler();
        usleep(100);
    }

#if ROCKIT_EN
    RK_MPI_SYS_Exit();
#endif

    return 0;
}
