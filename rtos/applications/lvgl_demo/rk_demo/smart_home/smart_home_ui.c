/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include <rtthread.h>
#include "lvgl.h"

//#include "asr.h"
#include "home_ui.h"
#include "layout/tile_layout.h"
#include "main.h"
#include "smart_home_ui.h"
#include "ui_resource.h"

/* APPs */
#include "app_aircond.h"
#include "app_date.h"
#include "app_music.h"
#include "app_scene.h"
#include "app_switch.h"
#include "app_weather.h"

static lv_obj_t *main = NULL;
static lv_obj_t *bg_pic;
static lv_obj_t *btn_return;
static lv_obj_t *tv;
static lv_obj_t *tl;
static lv_img_dsc_t *bg_snapshot;

#define APP_PAD(hor, ver) {  \
    .w = hor,  \
    .h = ver,  \
}

static struct app_desc apps_desc[] =
{
    APP_DATE,
    APP_WEATHER,
    APP_SWITCH("客厅灯"),
    APP_SWITCH("卧室灯"),
    APP_SWITCH(NULL),
    APP_SWITCH(NULL),
    APP_SCENE,
#if BT_EN
    APP_MUSIC,
#endif
    APP_AIRCOND("客厅空调"),
    APP_AIRCOND("卧室空调"),
};
static int apps = ARRAY_SIZE(apps_desc);

static void btn_return_cb(lv_event_t *e)
{
    switch (e->code)
    {
    case LV_EVENT_CLICKED:
        home_ui_init();
        for (int i = 0; i < apps; i++)
        {
            if (apps_desc[i].deinit)
                apps_desc[i].deinit(apps_desc[i].userdata);
        }
        lv_obj_del(main);
        main = NULL;
#if ASR_EN
        asr_audio_deinit();
#endif
        break;
    default:
        break;
    }
}

static void scroll_cb(lv_event_t *event)
{
    for (int i = 0; i < apps; i++)
    {
        if (apps_desc[i].scroll_cb)
            apps_desc[i].scroll_cb(event, apps_desc[i].userdata);
    }
}

static void app_bg_update(lv_event_t *event)
{
    lv_img_t *obj;
    lv_area_t area;
    lv_coord_t x, y;

    obj = (lv_img_t *)lv_event_get_target(event);
    lv_obj_get_content_coords((lv_obj_t *)obj, &area);

    x = -area.x1;
    y = -area.y1;
    x = x % obj->w;
    obj->offset.x = x;
    y = y % obj->h;
    obj->offset.y = y;
}

static void init_app_bg(lv_obj_t *obj)
{
    lv_obj_t *app_bg;

    lv_obj_set_style_pad_all(obj, 5, LV_PART_MAIN);

    app_bg = lv_img_create(obj);
    lv_obj_set_size(app_bg, lv_pct(100), lv_pct(100));
    lv_obj_set_style_radius(app_bg, 20, LV_PART_MAIN);
    lv_obj_set_style_clip_corner(app_bg, 1, LV_PART_MAIN);
    lv_img_set_src(app_bg, bg_snapshot);
    lv_obj_add_event_cb(app_bg, app_bg_update, LV_EVENT_DRAW_MAIN_BEGIN, NULL);
}

void smart_home_ui_init(void)
{
    lv_obj_t *obj;
    lv_obj_t *tl_item;
    int cnt = 0;
    int idx = 0;

#if ASR_EN
    asr_audio_init();
#endif

    if (main)
    {
        lv_obj_clear_flag(main, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    bg_snapshot = get_bg_snapshot();

    main = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(main);
    lv_obj_set_style_pad_all(main, 0, LV_PART_MAIN);
    lv_obj_set_size(main, lv_pct(100), lv_pct(100));
    lv_obj_refr_size(main);

    btn_return = ui_return_btn_create(main, btn_return_cb, "智能家居");

    tv = lv_tileview_create(main);
    lv_obj_remove_style_all(tv);
    lv_obj_set_size(tv, lv_pct(100), lv_pct(90));
    lv_obj_set_pos(tv, 0, lv_pct(10));
    lv_obj_add_event_cb(tv, scroll_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_refr_size(tv);

    while (idx != apps)
    {
        printf("page %d\n", cnt);
        obj = lv_tileview_add_tile(tv, cnt++, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
        lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);

        tl = tile_layout_create(obj, 0, 0, 200);

        while (idx != apps)
        {
            tl_item = tile_layout_new_item(tl, apps_desc[idx].w,
                                           apps_desc[idx].h,
                                           !apps_desc[idx].init);
            if (!tl_item)
                break;
            if (apps_desc[idx].init)
            {
                init_app_bg(tl_item);
                apps_desc[idx].init(tl_item, apps_desc[idx].userdata);
            }
            printf("app %d/%d(%dx%d)\n", idx + 1, apps,
                   apps_desc[idx].w, apps_desc[idx].h);
            idx++;
        }
    }
}

