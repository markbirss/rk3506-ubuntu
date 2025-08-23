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

#include <rtthread.h>
#include <dfs_fs.h>
#include "lvgl.h"

#include "main.h"
#include "home_ui.h"
#include "ui_resource.h"

lv_ft_info_t ttf_main_s;
lv_ft_info_t ttf_main_m;
lv_ft_info_t ttf_main_l;

lv_style_t style_txt_s;
lv_style_t style_txt_m;
lv_style_t style_txt_l;

lv_dir_t scr_dir;
lv_coord_t scr_w;
lv_coord_t scr_h;

static int g_indev_rotation = 0;
static int g_disp_rotation = LV_DISP_ROT_NONE;

static int quit = 0;

static int backlight_timeout = 5000;
static int start_tick = 0;

extern void rk_demo_init(void);

void backlight_set_timeout(int timeout)
{
    backlight_timeout = timeout;
}

static void sigterm_handler(int sig)
{
    //fprintf(stderr, "signal %d\n", sig);
    quit = 1;
}

int app_disp_rotation(void)
{
    return g_disp_rotation;
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

static void lvgl_init(void)
{
    check_scr();
}

static void font_init(void)
{
    lv_freetype_init(64, 1, 0);

    if (scr_dir == LV_DIR_HOR)
    {
        ttf_main_s.weight = ALIGN(RK_PCT_W(2), 2);
        ttf_main_m.weight = ALIGN(RK_PCT_W(3), 2);
        ttf_main_l.weight = ALIGN(RK_PCT_W(6), 2);
    }
    else
    {
        ttf_main_s.weight = ALIGN(RK_PCT_H(2), 2);
        ttf_main_m.weight = ALIGN(RK_PCT_H(3), 2);
        ttf_main_l.weight = ALIGN(RK_PCT_H(6), 2);
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

void lv_rk_demo(void)
{
#ifdef RT_USING_DFS_RAMFS
    /* Use ramfs to optimize lvgl display effect */
    copy("resource/", "/ramfs");
#endif

    lvgl_init();

    app_init();

    rk_demo_init();
}
