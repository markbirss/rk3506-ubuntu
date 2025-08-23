#include <stdio.h>
#include "main.h"
#include "icebox_ui.h"
#include "furniture_control_ui.h"

///////////////////// VARIABLES ////////////////////
static lv_obj_t *main = NULL;
static lv_obj_t *btn_return;

static lv_obj_t *ui_icebox_box = NULL;
static lv_obj_t *ui_icebox_text_box = NULL;
static lv_obj_t *text_box1 = NULL;
static lv_obj_t *text_box2 = NULL;
static lv_obj_t *ui_icebox_png_box = NULL;

static lv_obj_t *text_bg;
static lv_obj_t *ui_icebox_tem;
static lv_obj_t *ui_Label1;
static lv_obj_t *ui_Label3;
static lv_obj_t *ui_rice;
static lv_img_dsc_t *bg_snapshot;

///////////////////// TEST LVGL SETTINGS ////////////////////

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

static void btn_return_cb(lv_event_t *event)
{
    printf("icebox_page_jump_furniture_control_callback is into \n");
    furniture_control_ui_init();
    lv_obj_del(main);
    main = NULL;
}

void icebox_ui_init(void)
{
    lv_area_t area;

    if (main)
        return;

    main = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(main);
    lv_obj_set_style_pad_all(main, 0, LV_PART_MAIN);
    lv_obj_set_size(main, lv_pct(100), lv_pct(100));
    lv_obj_refr_size(main);

    ui_icebox_box = lv_obj_create(main);
    lv_obj_remove_style_all(ui_icebox_box);
    lv_obj_set_width(ui_icebox_box, lv_pct(100));
    lv_obj_set_height(ui_icebox_box, lv_pct(33));
    lv_obj_align(ui_icebox_box, LV_ALIGN_TOP_LEFT, 0, lv_pct(33));
    lv_obj_refr_pos(ui_icebox_box);
    //lv_obj_set_flex_flow(ui_icebox_box, LV_FLEX_FLOW_ROW);//行

    bg_snapshot = get_bg_snapshot();

    text_bg = lv_img_create(ui_icebox_box);
    lv_obj_set_width(text_bg, lv_pct(100));
    lv_obj_set_height(text_bg, lv_pct(100));
    lv_obj_center(text_bg);
    lv_obj_refr_pos(text_bg);
    lv_obj_refr_size(text_bg);
    lv_obj_get_content_coords(text_bg, &area);
    lv_img_set_src(text_bg, bg_snapshot);
    lv_img_set_offset_x(text_bg, -area.x1);
    lv_img_set_offset_y(text_bg, -area.y1);

    ui_icebox_text_box = lv_obj_create(text_bg);
    lv_obj_remove_style_all(ui_icebox_text_box);
    lv_obj_set_width(ui_icebox_text_box, lv_pct(66));
    lv_obj_set_height(ui_icebox_text_box, lv_pct(80));
    lv_obj_clear_flag(ui_icebox_text_box, LV_OBJ_FLAG_SCROLLABLE);

    text_box1 = lv_obj_create(ui_icebox_text_box);
    lv_obj_remove_style_all(text_box1);
    lv_obj_set_width(text_box1, lv_pct(100));
    lv_obj_set_height(text_box1, lv_pct(30));
    lv_obj_align(text_box1, LV_ALIGN_TOP_LEFT, 0, 0);

    ui_Label1 = lv_label_create(text_box1);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_add_style(ui_Label1, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label1, "每日食谱-炒饭");

    text_box2 = lv_obj_create(ui_icebox_text_box);
    lv_obj_remove_style_all(text_box2);
    lv_obj_set_width(text_box2, lv_pct(100));
    lv_obj_set_height(text_box2, lv_pct(66));
    lv_obj_align(text_box2, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    ui_Label3 = lv_label_create(text_box2);
    lv_obj_set_width(ui_Label3, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label3, LV_SIZE_CONTENT);    /// 1
    lv_obj_add_style(ui_Label3, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_align(ui_Label3, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label3,
                      "将玉米粒、青豆、胡萝卜入锅一起炒匀，\n加适量的盐调味、滴几滴蚝油炒匀。");

    ui_icebox_png_box = lv_img_create(text_bg);
    lv_obj_set_width(ui_icebox_png_box, lv_pct(30));
    lv_obj_set_height(ui_icebox_png_box, lv_pct(100));
    lv_obj_align(ui_icebox_png_box, LV_ALIGN_TOP_RIGHT, 0, 0);

    ui_rice = lv_img_create(ui_icebox_png_box);
    lv_img_set_src(ui_rice, IMG_RICE);
    lv_obj_center(ui_rice);

    btn_return = ui_return_btn_create(main, btn_return_cb, NULL);
}

