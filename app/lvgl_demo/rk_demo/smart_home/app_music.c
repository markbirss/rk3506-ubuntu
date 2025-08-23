#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include <lvgl/lvgl.h>

#include "main.h"
#include "smart_home_ui.h"
#include "ui_resource.h"
#include "wifibt.h"

#if BT_EN
static lv_obj_t *area_player;
static lv_obj_t *area_status;
static lv_obj_t *area_label;
static lv_obj_t *area_time;
static lv_obj_t *area_btn;
static lv_obj_t *box_btn;
static lv_obj_t *label_singer;
static lv_obj_t *label_song;
static lv_obj_t *label_pos;
static lv_obj_t *label_time;
static lv_obj_t *btn_play;
static lv_obj_t *label_bt_state;
static lv_timer_t *timer;
static lv_timer_t *pos_timer;

enum
{
    MUSIC_BTN_PREV,
    MUSIC_BTN_PLAY,
    MUSIC_BTN_NEXT,
    MUSIC_BTN_VOLUME_DOWN,
    MUSIC_BTN_VOLUME_UP,
    MUSIC_BTN_VOLUME_MUTE,
};

static char *btn_img[] =
{
    LV_SYMBOL_PREV,
    LV_SYMBOL_PLAY,
    LV_SYMBOL_NEXT,
    LV_SYMBOL_VOLUME_MID,
    LV_SYMBOL_VOLUME_MAX,
    LV_SYMBOL_MUTE,
};

static int last_btn = MUSIC_BTN_NEXT;
static int bt_sink_enabled = 0;
static intptr_t volume = 100;
static int mute = 0;
static int g_pos = 0;

static struct wifibt_cmdarg cmdarg;
static struct bt_info new_info;

static void btn_cb(lv_event_t *e)
{
    intptr_t idx = (intptr_t)lv_event_get_user_data(e);

    cmdarg.cmd = BT_INFO;
    cmdarg.val = &new_info;
    bt_query_wait(&cmdarg, sizeof(cmdarg));

    if (new_info.bt_state < BT_STATE_CONNECTED)
        return;

    switch (idx)
    {
    case MUSIC_BTN_PREV:
        cmdarg.cmd = BT_SINK_PREV;
        bt_query(&cmdarg, sizeof(cmdarg));
        break;
    case MUSIC_BTN_PLAY:
        if (new_info.bt_state == BT_STATE_PLAYING)
            cmdarg.cmd = BT_SINK_PAUSE;
        else
            cmdarg.cmd = BT_SINK_PLAY;
        bt_query(&cmdarg, sizeof(cmdarg));
        break;
    case MUSIC_BTN_NEXT:
        cmdarg.cmd = BT_SINK_NEXT;
        bt_query(&cmdarg, sizeof(cmdarg));
        break;
    case MUSIC_BTN_VOLUME_DOWN:
        if (volume >= 10)
            volume -= 10;
        else
            volume = 0;
        cmdarg.cmd = BT_SINK_VOL;
        cmdarg.val = (void *)volume;
        bt_query(&cmdarg, sizeof(cmdarg));
        break;
    case MUSIC_BTN_VOLUME_UP:
        if (volume <= 90)
            volume += 10;
        else
            volume = 100;
        cmdarg.cmd = BT_SINK_VOL;
        cmdarg.val = (void *)volume;
        bt_query(&cmdarg, sizeof(cmdarg));
        break;
    case MUSIC_BTN_VOLUME_MUTE:
        if (mute)
            cmdarg.val = (void *)volume;
        else
            cmdarg.val = (void *)0;
        cmdarg.cmd = BT_SINK_VOL;
        bt_query(&cmdarg, sizeof(cmdarg));
        mute = !mute;
        break;
    }
}

static void update_state_label(lv_obj_t *label, int state)
{
    switch (new_info.bt_state)
    {
    case BT_STATE_OFF:
        lv_label_set_text(label_bt_state,
                          "蓝牙：未开启");
        break;
    case BT_STATE_ON:
        lv_label_set_text(label_bt_state,
                          "蓝牙：等待连接");
        break;
    default:
        lv_label_set_text(label_bt_state,
                          "蓝牙：已连接");
        break;
    }
}

static void pos_timer_cb(struct _lv_timer_t *tmr)
{
    static int bt_state = -1;
    char *title, *artist;
    int pos, dur;

    cmdarg.cmd = BT_INFO;
    cmdarg.val = &new_info;
    bt_query_wait(&cmdarg, sizeof(cmdarg));

    if (new_info.bt_state != bt_state)
    {
        if (new_info.bt_state == BT_STATE_PLAYING)
            lv_label_set_text(btn_play, LV_SYMBOL_PAUSE);
        else
            lv_label_set_text(btn_play, LV_SYMBOL_PLAY);
        bt_state = new_info.bt_state;
    }

    if (new_info.pos_changed)
    {
        g_pos = new_info.pos;
        cmdarg.cmd = BT_SINK_POS_CLEAR;
        bt_query(&cmdarg, sizeof(cmdarg));
    }
    else
    {
        if (new_info.bt_state != BT_STATE_PLAYING)
            return;
        g_pos += 100;
    }
    pos = g_pos / 1000;
    dur = new_info.dur / 1000;
    lv_label_set_text_fmt(label_time, "%02d:%02d / %02d:%02d",
                          pos / 60, pos % 60, dur / 60, dur % 60);

    if (new_info.track_changed)
    {
        title = strdup(new_info.title);
        artist = strdup(new_info.artist);
        lv_label_set_text(label_singer, artist);
        lv_label_set_text(label_song, title);
        if (title)
            free(title);
        if (artist)
            free(artist);
        cmdarg.cmd = BT_SINK_TRACK_CLEAR;
        bt_query(&cmdarg, sizeof(cmdarg));
    }
}

static void bt_timer_cb(struct _lv_timer_t *tmr)
{
    static RK_BT_STATE last_state = RK_BT_STATE_NONE;
    cmdarg.cmd = BT_INFO;
    cmdarg.val = &new_info;
    bt_query_wait(&cmdarg, sizeof(cmdarg));

    update_state_label(label_bt_state, new_info.bt_state);
    if ((last_state != BT_STATE_OFF) &&
            (new_info.bt_state == BT_STATE_OFF))
    {
        last_state = new_info.bt_state;
        cmdarg.cmd = BT_ENABLE;
        bt_query(&cmdarg, sizeof(cmdarg));
        return;
    }
    last_state = new_info.bt_state;
    bt_sink_enabled = 1;
}

void app_music_init(lv_obj_t *parent, void *userdata)
{
    lv_obj_t *obj;

    cmdarg.cmd = BT_INFO;
    cmdarg.val = &new_info;
    bt_query_wait(&cmdarg, sizeof(cmdarg));
    if (new_info.bt_state == BT_STATE_OFF)
    {
        bt_sink_enabled = 0;
        cmdarg.cmd = BT_SINK_ENABLE;
        bt_query(&cmdarg, sizeof(cmdarg));
    }
    else
    {
        bt_sink_enabled = 1;
    }

    area_player = lv_obj_create(parent);
    lv_obj_remove_style_all(area_player);
    lv_obj_clear_flag(area_player, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(area_player, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(area_player, 20, LV_PART_MAIN);
    lv_obj_center(area_player);

    area_status = lv_obj_create(area_player);
    lv_obj_remove_style_all(area_status);
    lv_obj_set_width(area_status, lv_pct(100));

    obj = lv_label_create(area_status);
    lv_label_set_text_fmt(obj, "蓝牙音乐 %.*s",
                          sizeof(new_info.bt_name), new_info.bt_name);
    lv_obj_add_style(obj, &style_txt_s, LV_PART_MAIN);
    lv_obj_set_style_text_color(obj, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(obj, LV_ALIGN_TOP_LEFT, 0, 0);

    label_bt_state = lv_label_create(area_status);
    update_state_label(label_bt_state, new_info.bt_state);
    lv_obj_add_style(label_bt_state, &style_txt_s, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_bt_state, lv_color_white(), LV_PART_MAIN);
    lv_obj_align(label_bt_state, LV_ALIGN_TOP_RIGHT, 0, 0);

    area_label = lv_obj_create(area_player);
    lv_obj_remove_style_all(area_label);
    lv_obj_set_size(area_label, lv_pct(40), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(area_label, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(area_label, 10, 0);
    lv_obj_align(area_label, LV_ALIGN_LEFT_MID, 0, 0);

    label_singer = lv_label_create(area_label);
    lv_obj_set_width(label_singer, lv_pct(100));
    lv_label_set_text(label_singer, "歌曲名");
    lv_label_set_long_mode(label_singer, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_add_style(label_singer, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_singer, lv_color_white(), LV_PART_MAIN);

    label_song = lv_label_create(area_label);
    lv_obj_set_width(label_song, lv_pct(100));
    lv_label_set_text(label_song, "歌手名");
    lv_label_set_long_mode(label_song, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_add_style(label_song, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_song, lv_color_white(), LV_PART_MAIN);

    area_time = lv_obj_create(area_player);
    lv_obj_remove_style_all(area_time);
    lv_obj_set_size(area_time, lv_pct(60), 120);
    lv_obj_set_flex_flow(area_time, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(area_time, LV_FLEX_ALIGN_END,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END);
    lv_obj_set_style_pad_row(area_time, 10, 0);
    lv_obj_align(area_time, LV_ALIGN_RIGHT_MID, 0, 0);

    label_time = lv_label_create(area_time);
    lv_label_set_text(label_time, "00:00 / 00:00");
    lv_obj_set_style_text_color(label_time, lv_color_white(), LV_PART_MAIN);

    area_btn = lv_obj_create(area_time);
    lv_obj_remove_style_all(area_btn);
    lv_obj_set_size(area_btn, (last_btn + 1) * 50 + last_btn * 10, 60);
    lv_obj_set_style_pad_column(area_btn, 10, LV_PART_MAIN);
    lv_obj_set_flex_align(area_btn, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (intptr_t i = 0; i <= last_btn; i++)
    {
        obj = lv_btn_create(area_btn);
        lv_obj_set_style_radius(obj, 25, LV_PART_MAIN);
        lv_obj_set_size(obj, 50, 50);
        lv_obj_add_event_cb(obj, btn_cb, LV_EVENT_CLICKED, (void *)i);
        obj = lv_label_create(obj);
        lv_label_set_text(obj, btn_img[i]);
        lv_obj_center(obj);
        if (i == 1)
            btn_play = obj;
    }

    timer = lv_timer_create(bt_timer_cb, 3000, NULL);
    pos_timer = lv_timer_create(pos_timer_cb, 100, NULL);
}

void app_music_deinit(void *userdata)
{
    lv_timer_del(timer);
    lv_timer_del(pos_timer);

    if (bt_sink_enabled)
    {
        cmdarg.cmd = BT_SINK_DISABLE;
        bt_query(&cmdarg, sizeof(cmdarg));
        bt_sink_enabled = 0;
    }
}
#endif

