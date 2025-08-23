#ifndef __MAIN_H__
#define __MAIN_H__

#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include <lvgl/lvgl.h>

#include "home_ui.h"
#include "lv_port_init.h"
#include "ui_btnmatrix.h"
#include "ui_common.h"
#include "ui_resource.h"

#define SMART_HOME      1
#define FURNITURE_CTRL  1
#define PHONE_PAGE      1
#define ROCKIT_EN   MULTIMEDIA_EN
#ifndef WIFIBT_EN
#define WIFIBT_EN   0
#endif
#define BT_EN       WIFIBT_EN
#ifndef ASR_EN
#define ASR_EN      0
#endif

#define ARRAY_SIZE(x)   (sizeof(x) / sizeof(x[0]))
#define ALIGN(x, a) (((x) + (a - 1)) & ~(a - 1))
#define RK_PCT_W(x) (scr_w ? ((int)((float)scr_w * x / 100)) : lv_pct((int)x))
#define RK_PCT_H(x) (scr_h ? ((int)((float)scr_h * x / 100)) : lv_pct((int)x))

extern lv_ft_info_t ttf_main_s;
extern lv_ft_info_t ttf_main_m;
extern lv_ft_info_t ttf_main_l;

extern lv_style_t style_txt_s;
extern lv_style_t style_txt_m;
extern lv_style_t style_txt_l;

extern lv_dir_t scr_dir;
extern lv_coord_t scr_w;
extern lv_coord_t scr_h;

void backlight_set_timeout(int timeout);

#endif

