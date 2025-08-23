/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include <rtthread.h>

#ifndef __UI_RESOURCE_H__
#define __UI_RESOURCE_H__

#ifdef RT_USING_DFS_RAMFS
#define PATH_PREFIX     "/ramfs"
#else
#define PATH_PREFIX     "/resource"
#endif

#define MAIN_FONT       PATH_PREFIX"/SmileySans-Oblique.ttf"

// home page
#define IMG_RETURN_BTN  "A:"PATH_PREFIX"/arrow-left-fill.png"
#define IMG_RK_LOGO     "A:"PATH_PREFIX"/logo.png"
#define IMG_PHONE       "A:"PATH_PREFIX"/phone.png"
#define IMG_WIFI_ON     "A:"PATH_PREFIX"/wifi.png"
#define IMG_WIFI_OFF    "A:"PATH_PREFIX"/wifi-off.png"
#define IMG_SMART_HOME  "A:"PATH_PREFIX"/home.png"
#define IMG_SETTING     "A:"PATH_PREFIX"/sliders.png"
#define IMG_FURNITURE   "A:"PATH_PREFIX"/cpu.png"

// setting page
#define ICON_WIFI       "A:"PATH_PREFIX"/wifi-fill.png"
#define ICON_BT         "A:"PATH_PREFIX"/bluetooth-fill.png"
#define ICON_WALLPAPER  "A:"PATH_PREFIX"/image-2-fill.png"
#define ICON_DATA       "A:"PATH_PREFIX"/calendar-2-fill.png"
#define ICON_ABOUT      "A:"PATH_PREFIX"/information-fill.png"
#define IMG_CANCEL      "A:"PATH_PREFIX"/cancel.png"

// furniture control page
#define IMG_ICEBOX          "A:"PATH_PREFIX"/icebox.png"
#define IMG_PLAYER          "A:"PATH_PREFIX"/player.png"
#define IMG_COFFEE          "A:"PATH_PREFIX"/coffee_machine.png"
#define IMG_RICE            "A:"PATH_PREFIX"/rice.png"

#define IMG_PLAY            "A:"PATH_PREFIX"/play-fill.png"
#define IMG_SKIP_BACK       "A:"PATH_PREFIX"/skip-back-fill.png"
#define IMG_SKIP_FORWARD    "A:"PATH_PREFIX"/skip-forward-fill.png"
#define IMG_VOL_DOWN        "A:"PATH_PREFIX"/volume-down-fill.png"
#define IMG_VOL_UP          "A:"PATH_PREFIX"/volume-up-fill.png"
#define IMG_VOL_MUTE        "A:"PATH_PREFIX"/volume-mute-fill.png"

#define WIFI_ON_PIC     "A:"PATH_PREFIX"/wifi.png"
#define WIFI_OFF_PIC    "A:"PATH_PREFIX"/wifi-off.png"
#define WIFI_SCANNING   "A:"PATH_PREFIX"/scanning.png"

#define BG_PIC_0        "A:"PATH_PREFIX"/bg_pic_0.sjpg"
#define BG_PIC_1        "A:"PATH_PREFIX"/bg_pic_1.sjpg"
#define BG_PIC_2        "A:"PATH_PREFIX"/bg_pic_2.sjpg"
#define BG_PIC_3        "A:"PATH_PREFIX"/bg_pic_3.sjpg"
#define MAIN_BG_PIC     BG_PIC_0

#define WALLPAPER_0     "A:"PATH_PREFIX"/wallpaper_0.png"
#define WALLPAPER_1     "A:"PATH_PREFIX"/wallpaper_1.png"
#define WALLPAPER_2     "A:"PATH_PREFIX"/wallpaper_2.png"
#define WALLPAPER_3     "A:"PATH_PREFIX"/wallpaper_3.png"

//coffee machine
#define COFFEE_1     "A:"PATH_PREFIX"/coffee1.png"
#define COFFEE_2     "A:"PATH_PREFIX"/coffee2.png"
#define COFFEE_3     "A:"PATH_PREFIX"/coffee3.png"
#define COFFEE_4     "A:"PATH_PREFIX"/coffee4.png"
#define COFFEE_5     "A:"PATH_PREFIX"/coffee5.png"

//intercom
#define IMG_INTERCOM_PAUSE          "A:"PATH_PREFIX"/pause-fill.png"
#define IMG_INTERCOM_ROUND          "A:"PATH_PREFIX"/round.png"
#define IMG_INTERCOM_WEBCAM         "A:"PATH_PREFIX"/webcam-fill.png"
#define IMG_INTERCOM_ARROWUP        "A:"PATH_PREFIX"/arrow-up-fill.png"
#define IMG_INTERCOM_BACKWARD       "A:"PATH_PREFIX"/backward.png"
#define IMG_INTERCOM_RECTANGLE1     "A:"PATH_PREFIX"/rectangle_1.png"
#define IMG_INTERCOM_CONTUINE       "A:"PATH_PREFIX"/continue.png"

#define BG_COLOR                    (lv_color_hex(0x484B5B))
#define GRAD_COLOR                  (lv_color_hex(0x2C2D35))
#define MAIN_COLOR                  (lv_color_hex(0x434650))
#define MAIN_COLOR_PRESS            (lv_color_hex(0xb2b2b2))
#define HL_ORANGE                   (lv_color_hex(0xFFAB44))
#define HL_BLUE                     (lv_color_hex(0x239CFF))

#endif

