#include <math.h>
#include <time.h>
#include <lvgl/lvgl.h>

#include "asr.h"
#include "main.h"
#include "setting_ui.h"
#include "ui_resource.h"

#include "ui_intercom_homepage.h"
#include "smart_home_ui.h"
#include "furniture_control_ui.h"
#include "wifibt.h"

static lv_obj_t *scr = NULL;
static lv_obj_t *bg_pic;
static lv_obj_t *ui_label_time;
static lv_obj_t *ui_label_date;
#if WIFIBT_EN
static lv_obj_t *ui_wifi;
#endif
static lv_timer_t *timer_date;
static lv_timer_t *timer_snapshot;

static lv_obj_t *main = NULL;
static lv_obj_t *ui_box_main = NULL;
static lv_obj_t *ui_smart_home;
static lv_obj_t *ui_furniture_control;
static lv_obj_t *ui_phone;
static lv_obj_t *ui_setting;
static lv_img_dsc_t *bg_snapshot = NULL;

static void page_switch(lv_event_t *e);

static struct btn_desc home_btn[] =
{
#if SMART_HOME
    {
        .obj  = &ui_smart_home,
        .img  = IMG_SMART_HOME,
        .text = "智能家居",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = page_switch,
        .user_data = (void *)smart_home_ui_init,
    },
#endif
#if FURNITURE_CTRL
    {
        .obj  = &ui_furniture_control,
        .img  = IMG_FURNITURE,
        .text = "家电显控",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = page_switch,
        .user_data = (void *)furniture_control_ui_init,
    },
#endif
#if PHONE_PAGE
    {
        .obj  = &ui_phone,
        .img  = IMG_PHONE,
        .text = "楼宇对讲",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = page_switch,
        .user_data = (void *)intercom_homepage_ui_init,
    },
#endif
    {
        .obj  = &ui_setting,
        .img  = IMG_SETTING,
        .text = "系统设置",
        .w    = 1,
        .h    = 1,
        .draw = common_draw,
        .cb   = page_switch,
        .user_data = (void *)setting_ui_init,
    }
};

static lv_coord_t *col_dsc;
static lv_coord_t *row_dsc;

struct btn_matrix_desc home_desc =
{
    .pad = 5,
    .gap = 20,
    .desc = home_btn,
    .btn_cnt = ARRAY_SIZE(home_btn),
};

static void page_switch(lv_event_t *e)
{
    void (*func)(void) = lv_event_get_user_data(e);

    if (func)
    {
        func();
        lv_timer_del(timer_date);
        lv_obj_del(main);
        main = NULL;
    }
}

static void date_update(lv_timer_t *timer)
{
    time_t time_ptr;
    struct tm *tim;

    time(&time_ptr);
    tim = localtime(&time_ptr);
    lv_label_set_text_fmt(ui_label_time, "%02d:%02d",
                          tim->tm_hour, tim->tm_min);
    lv_label_set_text_fmt(ui_label_date, "%04d-%02d-%02d",
                          tim->tm_year + 1900, tim->tm_mon + 1, tim->tm_mday);
    lv_obj_align(ui_label_time, LV_ALIGN_TOP_LEFT, 50, 50);
    lv_obj_align_to(ui_label_date, ui_label_time, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

#if WIFIBT_EN
    if (wifi_connected())
    {
        lv_img_set_src(ui_wifi, IMG_WIFI_ON);
    }
    else
    {
        lv_img_set_src(ui_wifi, IMG_WIFI_OFF);
    }
#endif
}

void bg_pic_snapshot_blur(void)
{
    lv_draw_rect_dsc_t dsc;

    if (bg_snapshot)
        return;

    bg_snapshot = lv_snapshot_take(bg_pic, LV_IMG_CF_TRUE_COLOR);
    if (!bg_snapshot)
    {
        printf("snapshot take failed\n");
        return;
    }

    lv_obj_t *canvas = lv_canvas_create(NULL);
    lv_area_t area;
    lv_canvas_set_buffer(canvas, (void *)bg_snapshot->data,
                         bg_snapshot->header.w,
                         bg_snapshot->header.h,
                         bg_snapshot->header.cf);
    area.x1 = 0;
    area.y1 = 0;
    area.x2 = bg_snapshot->header.w - 1;
    area.y2 = bg_snapshot->header.h - 1;
    lv_canvas_blur_ver(canvas, &area, 48);
    lv_canvas_blur_hor(canvas, &area, 48);
    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_opa = LV_OPA_30;
    dsc.bg_color = lv_color_black();
    lv_canvas_draw_rect(canvas, 0, 0,
                        bg_snapshot->header.w,
                        bg_snapshot->header.h, &dsc);
    lv_obj_del(canvas);
}

lv_img_dsc_t *get_bg_snapshot(void)
{
    bg_pic_snapshot_blur();

    return bg_snapshot;
}

static void take_snapshot(lv_timer_t *timer)
{
    bg_pic_snapshot_blur();
    if (timer_snapshot)
    {
        lv_timer_del(timer_snapshot);
        timer_snapshot = NULL;
    }
}

void home_ui_init(void)
{
    lv_area_t area1, area2, area3;
    lv_coord_t item_w;
    lv_coord_t cols, rows;

    if (main)
        return;

    main = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(main);
    lv_obj_set_style_pad_all(main, 10, LV_PART_MAIN);
    lv_obj_set_size(main, lv_pct(100), lv_pct(100));
    lv_obj_refr_size(main);

    if (scr_dir == LV_DIR_HOR)
    {
        item_w = RK_PCT_H(30);
        cols = home_desc.btn_cnt;
        rows = 1;
    }
    else
    {
        item_w = RK_PCT_W(25);
        cols = 2;
        rows = ceil((float)home_desc.btn_cnt / cols);
    }

    col_dsc = calloc(cols + 1, sizeof(lv_coord_t));
    row_dsc = calloc(rows + 1, sizeof(lv_coord_t));
    for (int i = 0; i < cols + 1; i++)
        col_dsc[i] = item_w;
    col_dsc[cols] = LV_GRID_TEMPLATE_LAST;
    for (int i = 0; i < rows + 1; i++)
        row_dsc[i] = item_w;
    row_dsc[rows] = LV_GRID_TEMPLATE_LAST;
    home_desc.col_dsc = col_dsc;
    home_desc.row_dsc = row_dsc;

    ui_box_main = ui_btnmatrix_create(main, &home_desc);
    if (scr_dir == LV_DIR_HOR)
        lv_obj_align(ui_box_main, LV_ALIGN_BOTTOM_MID, 0, -20);
    else
        lv_obj_center(ui_box_main);

#if WIFIBT_EN
    ui_wifi = lv_img_create(main);
    lv_img_set_src(ui_wifi, IMG_WIFI_OFF);
    lv_obj_align(ui_wifi, LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

    ui_label_time = lv_label_create(main);
    lv_obj_set_width(ui_label_time, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_label_time, LV_SIZE_CONTENT);
    lv_obj_add_style(ui_label_time, &style_txt_l, LV_PART_MAIN);

    ui_label_date = lv_label_create(main);
    lv_obj_set_width(ui_label_date, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_label_date, LV_SIZE_CONTENT);
    lv_obj_add_style(ui_label_date, &style_txt_m, LV_PART_MAIN);

    date_update(NULL);
    timer_date = lv_timer_create(date_update, 1000, NULL);
    lv_timer_enable(timer_date);

    /* check area overlap */
    lv_obj_refr_size(ui_label_time);
    lv_obj_refr_size(ui_label_date);
    lv_obj_refr_size(ui_box_main);
    lv_obj_refr_pos(ui_label_time);
    lv_obj_refr_pos(ui_label_date);
    lv_obj_refr_pos(ui_box_main);

    lv_obj_get_content_coords(ui_label_date, &area1);
    lv_obj_get_content_coords(ui_box_main, &area2);
    lv_obj_get_content_coords(main, &area3);

    if ((area3.x2 - area3.x1) < (area2.x2 - area2.x1))
    {
        lv_obj_set_width(ui_box_main, lv_pct(100));
        lv_obj_set_align(ui_box_main, LV_ALIGN_LEFT_MID);
    }

    if (area2.y1 < area1.y2)
        lv_obj_set_y(ui_box_main, area1.y2 - area2.y1 + 5);
}

void rk_demo_init(void)
{
    scr = lv_obj_create(NULL);
    lv_obj_remove_style_all(scr);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    bg_pic = lv_img_create(scr);
    lv_obj_remove_style_all(bg_pic);
    lv_obj_set_size(bg_pic, lv_pct(100), lv_pct(100));
    lv_img_set_src(bg_pic, MAIN_BG_PIC);

    lv_disp_load_scr(scr);

    timer_snapshot = lv_timer_create(take_snapshot, 1000, NULL);

    home_ui_init();

#if ASR_EN
    asr_icon_create(lv_layer_sys());
#endif
}

void rk_demo_bg_set_img(const char *img)
{
    lv_img_set_src(bg_pic, img);
    lv_snapshot_free(bg_snapshot);
    bg_snapshot = NULL;
    bg_pic_snapshot_blur();
}

void rk_demo_bg_hide(void)
{
    if (bg_pic)
        lv_obj_add_flag(bg_pic, LV_OBJ_FLAG_HIDDEN);
}

void rk_demo_bg_show(void)
{
    if (bg_pic)
        lv_obj_clear_flag(bg_pic, LV_OBJ_FLAG_HIDDEN);
}

