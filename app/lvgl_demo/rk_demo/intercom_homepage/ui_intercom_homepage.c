#include <time.h>
#include <lvgl/lvgl.h>

#include "layout/tile_layout.h"
#include "main.h"
#include "ui_resource.h"

#include "ui_intercom_homepage.h"

static lv_obj_t *btn_return;

static lv_obj_t *main = NULL;
static lv_obj_t *tv;
static lv_obj_t *tl;

void intercom_call_ui_init();
void monitor_ui_init();
static void page_switch(lv_event_t *e);

#define APP(x) {  \
    .w =    1,  \
    .h =    1,  \
    .init = app_init,  \
    .userdata = x,  \
}

struct intercom_private
{
    char *text;
    void (*cb)(void);
};

static struct intercom_private private[] =
{
    {
        "视频监控",
#if MULTIMEDIA_EN
        monitor_ui_init,
#else
        NULL,
#endif
    },
    {
        "对讲呼叫",
#if MULTIMEDIA_EN
        intercom_call_ui_init,
#else
        NULL,
#endif
    },
    {"安防报警", NULL},
    {"信息", NULL},
    {"家电控制", NULL},
    {"留影留言", NULL},
    {"电梯召唤", NULL},
    {"呼叫管理员", NULL},
    {"图片管理", NULL},
    {"家人留言", NULL},
};

static int apps = ARRAY_SIZE(private);

static void page_switch(lv_event_t *e)
{
    void (*func)(void) = lv_event_get_user_data(e);

    if (func)
    {
        func();
        lv_obj_del(main);
        main = NULL;
    }
}

static void app_init(lv_obj_t *parent, void *userdata)
{
    struct intercom_private *private = (struct intercom_private *)userdata;
    lv_obj_t *obj;
    lv_obj_t *label;

    lv_obj_set_style_pad_all(parent, 5, LV_PART_MAIN);

    obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_color(obj, MAIN_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_color(obj, MAIN_COLOR_PRESS,
                              LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(obj, 1, LV_PART_MAIN);
    lv_obj_set_style_shadow_spread(obj, 2, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(obj, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(obj, LV_OPA_20, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, 16, LV_PART_MAIN);

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(obj, page_switch, LV_EVENT_CLICKED, private->cb);

    label = lv_label_create(obj);
    lv_label_set_text(label, private->text);
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_style(label, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(label);
}

static void btn_return_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        home_ui_init();
        lv_obj_del(main);
        main = NULL;
    }
}

void intercom_homepage_ui_init()
{
    lv_obj_t *obj;
    lv_obj_t *tl_item;
    int cnt = 0;
    int idx = 0;

    if (main)
        return;

    main = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(main);
    lv_obj_set_style_pad_all(main, 0, LV_PART_MAIN);
    lv_obj_set_size(main, lv_pct(100), lv_pct(100));
    lv_obj_refr_size(main);

    btn_return = ui_return_btn_create(main, btn_return_cb, "楼宇对讲");

    tv = lv_tileview_create(main);
    lv_obj_remove_style_all(tv);
    lv_obj_set_size(tv, lv_pct(100), lv_pct(90));
    lv_obj_set_pos(tv, 0, lv_pct(10));
//    lv_obj_add_event_cb(tv, scroll_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_refr_size(tv);

    while (idx != apps)
    {
        printf("page %d\n", cnt);
        obj = lv_tileview_add_tile(tv, cnt++, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
        lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);

        tl = tile_layout_create(obj, 0, 0, 150);

        while (idx != apps)
        {
            tl_item = tile_layout_new_item(tl, 1, 1, 0);
            if (!tl_item)
                break;
            app_init(tl_item, &private[idx]);
            idx++;
        }
    }
}

