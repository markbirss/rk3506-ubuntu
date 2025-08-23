#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "home_ui.h"
#include "main.h"
#include "ui_intercom_homepage.h"
#include "audio_server.h"
#include "local_ip.h"

#if MULTIMEDIA_EN
static lv_obj_t *main = NULL;

static lv_obj_t *btn_return;
static lv_obj_t *ui_cont;
static lv_obj_t *ui_remote_ip;
static lv_obj_t *ui_local_ip;
static lv_obj_t *ui_keyboard;
static lv_obj_t *ui_call;

static lv_timer_t *timer;

static void *server = NULL;

static const char *keyboard_map[] =
{
    "1", "2", "3", "\n",
    "4", "5", "6", "\n",
    "7", "8", "9", "\n",
    ".", "0", LV_SYMBOL_BACKSPACE, "\n",
    LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, ""
};

static const lv_btnmatrix_ctrl_t keyboard_ctrl[] =
{
    _LV_BTNMATRIX_WIDTH,
    _LV_BTNMATRIX_WIDTH,
    _LV_BTNMATRIX_WIDTH,
    _LV_BTNMATRIX_WIDTH,

    _LV_BTNMATRIX_WIDTH,
    _LV_BTNMATRIX_WIDTH,
    _LV_BTNMATRIX_WIDTH,
    _LV_BTNMATRIX_WIDTH,

    _LV_BTNMATRIX_WIDTH,
    _LV_BTNMATRIX_WIDTH,
    _LV_BTNMATRIX_WIDTH,
    _LV_BTNMATRIX_WIDTH,

    _LV_BTNMATRIX_WIDTH,
    _LV_BTNMATRIX_WIDTH,
};

static void ui_call_cb(lv_event_t *e)
{
    if (!audio_server_connected(server))
        audio_server_connect(server, lv_textarea_get_text(ui_remote_ip));
    else
        audio_server_disconnect(server);
}

static void btn_return_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        intercom_homepage_ui_init();
        lv_obj_del(main);
        main = NULL;
        lv_timer_del(timer);
        audio_server_del(server);
        server = NULL;
    }
}

static void state_update(lv_timer_t *timer)
{
    static int last_state = STATE_IDLE;
    int state;

    state = audio_server_state(server);

    if (last_state != state)
    {
        last_state = state;
        if (state == STATE_RUNNING)
        {
            lv_label_set_text(ui_call->spec_attr->children[0], "挂断");
            lv_obj_set_style_text_color(ui_call->spec_attr->children[0],
                                        lv_color_make(0xff, 0x0, 0x0),
                                        LV_PART_MAIN);
        }
        else
        {
            lv_label_set_text(ui_call->spec_attr->children[0], "呼叫");
            lv_obj_set_style_text_color(ui_call->spec_attr->children[0],
                                        lv_color_black(),
                                        LV_PART_MAIN);
        }
    }
}

void intercom_call_ui_init()
{
    lv_obj_t *label;
    char *ip;

    if (main)
        return;

    main = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(main);
    lv_obj_set_style_pad_all(main, 0, LV_PART_MAIN);
    lv_obj_set_size(main, lv_pct(100), lv_pct(100));
    lv_obj_refr_size(main);

    btn_return = ui_return_btn_create(main, btn_return_cb, "对讲呼叫");

    ui_cont = lv_obj_create(main);
    lv_obj_remove_style_all(ui_cont);
    lv_obj_set_style_pad_all(ui_cont, 0, LV_PART_MAIN);
    lv_obj_set_size(ui_cont, lv_pct(80), lv_pct(80));
    lv_obj_set_flex_flow(ui_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_cont, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(ui_cont, 10, LV_PART_MAIN);
    lv_obj_center(ui_cont);

    ui_local_ip = lv_label_create(ui_cont);
    lv_obj_set_width(ui_local_ip, lv_pct(100));
    lv_obj_add_style(ui_local_ip, &style_txt_m, LV_PART_MAIN);
    ip = get_local_ip();
    if (ip)
    {
        lv_label_set_text(ui_local_ip, ip);
        free(ip);
    }
    else
    {
        lv_label_set_text(ui_local_ip, "未联网");
    }

    ui_remote_ip = lv_textarea_create(ui_cont);
    lv_obj_set_width(ui_remote_ip, lv_pct(100));
    lv_textarea_set_password_mode(ui_remote_ip, false);
    lv_textarea_set_one_line(ui_remote_ip, true);
    lv_obj_add_style(ui_remote_ip, &style_txt_m, LV_PART_MAIN);

    ui_keyboard = lv_keyboard_create(ui_cont);
    lv_keyboard_set_map(ui_keyboard, LV_KEYBOARD_MODE_USER_1,
                        keyboard_map, keyboard_ctrl);
    lv_keyboard_set_mode(ui_keyboard, LV_KEYBOARD_MODE_USER_1);
    lv_keyboard_set_textarea(ui_keyboard, ui_remote_ip);
    lv_obj_set_style_radius(ui_keyboard, 5, LV_PART_MAIN);

    ui_call = lv_btn_create(ui_cont);
    lv_obj_set_size(ui_call, lv_pct(50), lv_pct(12));
    lv_obj_set_style_bg_color(ui_call, lv_color_white(), LV_PART_MAIN);
    lv_obj_add_event_cb(ui_call, ui_call_cb, LV_EVENT_CLICKED, NULL);
    label = lv_label_create(ui_call);
    lv_label_set_text(label, "呼叫");
    lv_obj_add_style(label, &style_txt_m, LV_PART_MAIN);
    lv_obj_center(label);

    timer = lv_timer_create(state_update, 100, NULL);
    server = audio_server_new();
    if (!server)
        printf("create audio server failed\n");
}
#endif
