/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include <rtthread.h>

#include "lvgl.h"

#include "home_ui.h"
#include "ui_intercom_homepage.h"
#include "main.h"
#include <time.h>

#if MULTIMEDIA_EN
#include "rkadk_common.h"
#include "rkadk_media_comm.h"
#include "rkadk_log.h"
#include "rkadk_param.h"
#include "rkadk_player.h"
#include "rk_mpi_vo.h"

#define VO_SUPPORT_SCALE    1

enum
{
    SCALE_4_3,
    SCALE_16_9,
    SCALE_FULL,
};

static RKADK_PLAYER_CFG_S stPlayCfg;
static RKADK_MW_PTR pPlayer = NULL;

static lv_obj_t *main = NULL;
static lv_obj_t *btn_return;

static lv_obj_t *ui_rtsp_label;
static lv_obj_t *ui_rtsp_addr;
static lv_obj_t *ui_rtsp_addr_float = NULL;
static lv_obj_t *kb;

static lv_obj_t *ui_control_box;
static lv_obj_t *ui_setting;
static lv_obj_t *ui_pause;
static lv_obj_t *ui_pause_label;
static lv_obj_t *ui_stop;
static lv_obj_t *ui_replay;

static lv_obj_t *ui_setting_box;
static lv_obj_t *ui_checkbox_audio;
#if VO_SUPPORT_SCALE
static lv_obj_t *ui_checkbox_ratio_box;
static lv_obj_t *ui_checkbox_ratio_4_3;
static lv_obj_t *ui_checkbox_ratio_16_9;
static lv_obj_t *ui_checkbox_stretch;
#endif
static lv_obj_t *player_box = NULL;
static lv_obj_t *icon_box = NULL;
static lv_timer_t *timer_state = NULL;

extern lv_style_t style_txt_s;
extern lv_style_t style_txt_m;

static lv_style_t style_txt;
static lv_style_t style_list;

static RKADK_PLAYER_STATE_E current_state = RKADK_PLAYER_STATE_BUTT;
static RKADK_PLAYER_STATE_E desired_state = RKADK_PLAYER_STATE_BUTT;
static pthread_t video_thread;
static RKADK_BOOL bVideoEnable = true;
static RKADK_BOOL bAudioEnable = false;
static int scale_mode = SCALE_FULL;
static int video_ofs_x;
static int video_ofs_y;
static int video_img_w;
static int video_img_h;
static int video_cont_w;
static int video_cont_h;

static char rtsp_address[128];

static void calc_video_area(void);
static void stop_player(void);
static void rtsp_play_stop_callback(lv_event_t *event);
static void rtsp_play_start_callback(lv_event_t *event);
static void rtsp_play_pause_callback(lv_event_t *event);

static void rkadk_deinit(void)
{
    RKADK_PLAYER_Destroy(pPlayer);
    pPlayer = NULL;
}

static void btn_return_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        intercom_homepage_ui_init();
        stop_player();
        rkadk_deinit();
        if (kb)
        {
            lv_obj_del(kb);
            kb = NULL;
        }
        lv_timer_del(timer_state);
        lv_obj_del(main);
        main = NULL;
        rk_demo_bg_show();
    }
}

static int is_network_enable(void)
{
    int ret = system("ping 114.114.114.114 -c 1 -W 1 > /dev/null");
    return !ret;
}

static void style_init(void)
{
    lv_style_init(&style_txt);
    lv_style_set_text_font(&style_txt, ttf_main_s.font);
    lv_style_set_text_color(&style_txt, lv_color_make(0xff, 0x23, 0x23));

    lv_style_init(&style_list);
    lv_style_set_text_font(&style_list, ttf_main_m.font);
    lv_style_set_text_color(&style_list, lv_color_black());
}

static void param_init(RKADK_PLAYER_FRAME_INFO_S *pstFrmInfo)
{
    RKADK_CHECK_POINTER_N(pstFrmInfo);

    memset(pstFrmInfo, 0, sizeof(RKADK_PLAYER_FRAME_INFO_S));
    pstFrmInfo->u32FrmInfoX = video_ofs_x;
    pstFrmInfo->u32FrmInfoY = video_ofs_y;
    pstFrmInfo->u32DispWidth = video_cont_w;
    pstFrmInfo->u32DispHeight = video_cont_h;
    pstFrmInfo->u32ImgWidth = video_img_w;
    pstFrmInfo->u32ImgHeight = video_img_h;

#if USE_RK3506
#if (LV_COLOR_DEPTH == 16)
    pstFrmInfo->u32VoFormat = VO_FORMAT_RGB565;
#elif (LV_COLOR_DEPTH == 32)
    pstFrmInfo->u32VoFormat = VO_FORMAT_RGB888;
#else
#error "Only support DEPTH 16 or 32"
#endif
    pstFrmInfo->u32VoLay = -1; // rkadk select the default first device
    pstFrmInfo->u32VoChn = 1; // ui is 1 . play is 0
    pstFrmInfo->u32VoDev = -1; // rkadk select the default first device
    pstFrmInfo->enIntfSync = RKADK_VO_OUTPUT_DEFAULT;
    pstFrmInfo->enVoSpliceMode = SPLICE_MODE_RGA; // rkadk depend chips
#else
    pstFrmInfo->u32VoFormat = VO_FORMAT_NV12;
    pstFrmInfo->u32EnIntfType = DISPLAY_TYPE_LCD;
    pstFrmInfo->u32VoLay = 1;
    pstFrmInfo->enIntfSync = RKADK_VO_OUTPUT_DEFAULT;
    pstFrmInfo->enVoSpliceMode = SPLICE_MODE_BYPASS;
#endif

    pstFrmInfo->u32BorderColor = 0x0000FA;
    pstFrmInfo->bMirror = RKADK_FALSE;
    pstFrmInfo->bFlip = RKADK_FALSE;
    pstFrmInfo->u32Rotation = 0;
    pstFrmInfo->stSyncInfo.bIdv = RKADK_TRUE;
    pstFrmInfo->stSyncInfo.bIhs = RKADK_TRUE;
    pstFrmInfo->stSyncInfo.bIvs = RKADK_TRUE;
    pstFrmInfo->stSyncInfo.bSynm = RKADK_TRUE;
    pstFrmInfo->stSyncInfo.bIop = RKADK_TRUE;
    pstFrmInfo->stSyncInfo.u16FrameRate = 30;
    pstFrmInfo->stSyncInfo.u16PixClock = 65000;
    pstFrmInfo->stSyncInfo.u16Hact = 1200;
    pstFrmInfo->stSyncInfo.u16Hbb = 24;
    pstFrmInfo->stSyncInfo.u16Hfb = 240;
    pstFrmInfo->stSyncInfo.u16Hpw = 136;
    pstFrmInfo->stSyncInfo.u16Hmid = 0;
    pstFrmInfo->stSyncInfo.u16Vact = 1200;
    pstFrmInfo->stSyncInfo.u16Vbb = 200;
    pstFrmInfo->stSyncInfo.u16Vfb = 194;
    pstFrmInfo->stSyncInfo.u16Vpw = 6;

    return;
}

static RKADK_VOID PlayerEventFnTest(RKADK_MW_PTR pPlayer,
                                    RKADK_PLAYER_EVENT_E enEvent,
                                    RKADK_VOID *pData)
{
    switch (enEvent)
    {
    case RKADK_PLAYER_EVENT_STATE_CHANGED:
        printf("+++++ RKADK_PLAYER_EVENT_STATE_CHANGED +++++\n");
        break;
    case RKADK_PLAYER_EVENT_EOF:
        printf("+++++ RKADK_PLAYER_EVENT_EOF +++++\n");
        current_state = RKADK_PLAYER_STATE_STOP;
        if (desired_state == RKADK_PLAYER_STATE_STOP)
            desired_state = RKADK_PLAYER_STATE_BUTT;
        break;
    case RKADK_PLAYER_EVENT_SOF:
        printf("+++++ RKADK_PLAYER_EVENT_SOF +++++\n");
        break;
    case RKADK_PLAYER_EVENT_SEEK_END:
        printf("+++++ RKADK_PLAYER_EVENT_SEEK_END +++++\n");
        break;
    case RKADK_PLAYER_EVENT_ERROR:
        printf("+++++ RKADK_PLAYER_EVENT_ERROR +++++\n");
        current_state = RKADK_PLAYER_STATE_STOP;
        if (desired_state != RKADK_PLAYER_STATE_BUTT)
            desired_state = RKADK_PLAYER_STATE_BUTT;
        break;
    case RKADK_PLAYER_EVENT_PREPARED:
        printf("+++++ RKADK_PLAYER_EVENT_PREPARED +++++\n");
        break;
    case RKADK_PLAYER_EVENT_PLAY:
        printf("+++++ RKADK_PLAYER_EVENT_PLAY +++++\n");
        current_state = RKADK_PLAYER_STATE_PLAY;
        if (desired_state == RKADK_PLAYER_STATE_PLAY)
            desired_state = RKADK_PLAYER_STATE_BUTT;
        break;
    case RKADK_PLAYER_EVENT_PAUSED:
        printf("+++++ RKADK_PLAYER_EVENT_PAUSED +++++\n");
        current_state = RKADK_PLAYER_STATE_PAUSE;
        if (desired_state == RKADK_PLAYER_STATE_PAUSE)
            desired_state = RKADK_PLAYER_STATE_BUTT;
        break;
    case RKADK_PLAYER_EVENT_STOPPED:
        printf("+++++ RKADK_PLAYER_EVENT_STOPPED +++++\n");
        current_state = RKADK_PLAYER_STATE_STOP;
        desired_state = RKADK_PLAYER_STATE_BUTT;
        break;
    default:
        printf("+++++ Unknown event(%d) +++++\n", enEvent);
        break;
    }
}

static void rkadk_init(void)
{
    setenv("rt_vo_disable_vop", "0", 1);
    RKADK_MPI_SYS_Init();
    RKADK_PARAM_Init(NULL, NULL);
    memset(&stPlayCfg, 0, sizeof(RKADK_PLAYER_CFG_S));
    param_init(&stPlayCfg.stFrmInfo);
    stPlayCfg.bEnableAudio = false;
    stPlayCfg.bEnableVideo = false;
    if (bAudioEnable)
        stPlayCfg.bEnableAudio = true;
    if (bVideoEnable)
        stPlayCfg.bEnableVideo = true;
    stPlayCfg.stNetStreamCfg.u32IoTimeout = 3 * 1000 * 1000;

    stPlayCfg.pfnPlayerCallback = PlayerEventFnTest;

    stPlayCfg.stNetStreamCfg.transport = "udp";
    stPlayCfg.stVdecCfg.u32FrameBufCnt = 4;

    RK_MPI_VO_SetLayerFlush(0);
    if (RKADK_PLAYER_Create(&pPlayer, &stPlayCfg))
    {
        printf("rkadk: RKADK_PLAYER_Create failed\n");
        return;
    }
}

static void *rtsp_play(void *arg)
{
    if (pPlayer != NULL)
    {
        printf("video_name_callback: stop and deinit pPlayer\n");
        RKADK_PLAYER_Stop(pPlayer);
        rkadk_deinit();
    }
    if (pPlayer == NULL)
    {
        printf("video_name_callback: rkadk_init pPlayer\n");
        rkadk_init();
    }
    strcpy(rtsp_address, lv_textarea_get_text(ui_rtsp_addr));
    int ret = RKADK_PLAYER_SetDataSource(pPlayer, rtsp_address);
    if (ret)
    {
        printf("rkadk: SetDataSource failed, ret = %d\n", ret);
        return NULL;
    }
    printf("RKADK_PLAYER_SetDataSource\n");
    ret = RKADK_PLAYER_Prepare(pPlayer);
    if (ret)
    {
        printf("rkadk: Prepare failed, ret = %d\n", ret);
        return NULL;
    }
    printf("RKADK_PLAYER_Prepare\n");
    desired_state = RKADK_PLAYER_STATE_PLAY;
    ret = RKADK_PLAYER_Play(pPlayer);
    while (current_state != RKADK_PLAYER_STATE_STOP)
    {
        usleep(1000 * 100);
    }

    video_thread = 0;

    return NULL;
}

static void stop_player(void)
{
    if (video_thread)
    {
        RKADK_PLAYER_Stop(pPlayer);
        pthread_join(video_thread, NULL);
        video_thread = 0;
    }
}

static void rtsp_play_stop_callback(lv_event_t *event)
{
    printf("%s in\n", __func__);

    stop_player();
}

static void rtsp_play_start_callback(lv_event_t *event)
{
    printf("%s in\n", __func__);

    if (is_network_enable())
    {
        if (desired_state != RKADK_PLAYER_STATE_BUTT)
        {
            printf("Waiting player handled last state, ignore\n");
            return;
        }
        stop_player();
        current_state = RKADK_PLAYER_STATE_IDLE;
        pthread_create(&video_thread, NULL, rtsp_play, NULL);
    }
    else
    {
        printf("network disable\n");
    }
}

static void rtsp_play_pause_callback(lv_event_t *event)
{
    printf("%s in\n", __func__);

    if (!video_thread)
    {
        rtsp_play_start_callback(event);
        return;
    }

    if (current_state == RKADK_PLAYER_STATE_PLAY)
    {
        if (desired_state != RKADK_PLAYER_STATE_PAUSE)
        {
            desired_state = RKADK_PLAYER_STATE_PAUSE;
            RKADK_PLAYER_Pause(pPlayer);
        }
    }
    else
    {
        if (desired_state != RKADK_PLAYER_STATE_PLAY)
        {
            desired_state = RKADK_PLAYER_STATE_PLAY;
            RKADK_PLAYER_Play(pPlayer);
        }
    }
}

static void kb_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);

    if (ui_rtsp_addr_float)
    {
        lv_textarea_set_text(ui_rtsp_addr,
                             lv_textarea_get_text(ui_rtsp_addr_float));
        lv_obj_del(ui_rtsp_addr_float);
        ui_rtsp_addr_float = NULL;
    }
    lv_obj_del(kb);
    kb = NULL;
    RK_MPI_VO_SetLayerFlush(0);
}

static void rtsp_addr_set(lv_event_t *e)
{
    lv_area_t addr_area;
    lv_area_t kb_area;
    lv_area_t res_area;

    if (kb)
        return;

    kb = lv_keyboard_create(lv_layer_sys());
    lv_obj_set_size(kb, lv_pct(100), lv_pct(30));
    lv_obj_set_align(kb, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_event_cb(kb, kb_cb, LV_EVENT_CANCEL, NULL);
    lv_obj_add_event_cb(kb, kb_cb, LV_EVENT_READY, NULL);

    lv_obj_refr_size(ui_rtsp_addr);
    lv_obj_refr_pos(ui_rtsp_addr);
    lv_obj_get_coords(ui_rtsp_addr, &addr_area);

    lv_obj_refr_size(kb);
    lv_obj_refr_pos(kb);
    lv_obj_get_coords(kb, &kb_area);

    if (_lv_area_intersect(&res_area, &addr_area, &kb_area))
    {
        ui_rtsp_addr_float = lv_textarea_create(lv_layer_sys());
        lv_obj_set_width(ui_rtsp_addr_float, lv_pct(100));
        lv_obj_add_state(ui_rtsp_addr_float, LV_STATE_FOCUSED);
        lv_obj_clear_state(ui_rtsp_addr, LV_STATE_FOCUSED);
        lv_textarea_set_text(ui_rtsp_addr_float,
                             lv_textarea_get_text(ui_rtsp_addr));
        lv_textarea_set_cursor_pos(ui_rtsp_addr_float,
                                   lv_textarea_get_cursor_pos(ui_rtsp_addr));
        //lv_textarea_set_cursor_click_pos(ui_rtsp_addr_float, true);
        lv_textarea_set_password_mode(ui_rtsp_addr_float, false);
        lv_textarea_set_one_line(ui_rtsp_addr_float, true);
        lv_obj_add_style(ui_rtsp_addr_float, &style_txt_m, LV_PART_MAIN);
        lv_keyboard_set_textarea(kb, ui_rtsp_addr_float);
        lv_obj_align_to(ui_rtsp_addr_float, kb, LV_ALIGN_OUT_TOP_MID, 0, 0);
    }
    else
    {
        lv_keyboard_set_textarea(kb, ui_rtsp_addr);
    }
}

static void rtsp_setting_callback(lv_event_t *e)
{
    if (lv_obj_has_flag(ui_setting_box, LV_OBJ_FLAG_HIDDEN))
    {
        lv_obj_clear_flag(ui_setting_box, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(ui_setting_box, LV_OBJ_FLAG_HIDDEN);
        RK_MPI_VO_SetLayerFlush(0);
    }
}

static void ui_ratio_cb(lv_event_t *e)
{
    lv_obj_t *cont = lv_event_get_current_target(e);
    lv_obj_t *act_cb = lv_event_get_target(e);
    lv_obj_t *old_cb = lv_obj_get_child(cont, scale_mode + 1);

    if (act_cb == cont) return;

    lv_obj_clear_state(old_cb, LV_STATE_CHECKED);
    lv_obj_add_state(act_cb, LV_STATE_CHECKED);

    scale_mode = lv_obj_get_index(act_cb) - 1;

    printf("Scale Mode = %d\n", scale_mode);

    calc_video_area();
}

static void ui_checkbox_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    RKADK_BOOL *flag = lv_event_get_user_data(e);
    *flag = lv_obj_has_state(obj, LV_STATE_CHECKED);

    printf("bVideoEnable=%d, bAudioEnable=%d\n", bVideoEnable, bAudioEnable);
}

static void ui_update_state(lv_timer_t *timer)
{
    static RKADK_PLAYER_STATE_E last_state = RKADK_PLAYER_STATE_BUTT;

    if (last_state != current_state)
    {
        last_state = current_state;
        if (current_state == RKADK_PLAYER_STATE_PLAY)
            lv_label_set_text(ui_pause_label, "暂停");
        else
            lv_label_set_text(ui_pause_label, "播放");
    }
}

static void calc_video_area(void)
{
    lv_area_t header_area;
    lv_area_t player_area;
    lv_area_t video_area;

    lv_obj_refr_size(icon_box);
    lv_obj_refr_pos(icon_box);
    lv_obj_get_coords(icon_box, &header_area);

    lv_obj_refr_size(player_box);
    lv_obj_refr_pos(player_box);
    lv_obj_get_coords(player_box, &player_area);

    video_area.x1 = header_area.x1;
    video_area.x2 = header_area.x2;
    video_area.y1 = header_area.y2;
    video_area.y2 = player_area.y1;

    video_cont_w = lv_area_get_width(&video_area);
    video_cont_h = lv_area_get_height(&video_area);

    if (scale_mode == SCALE_FULL)
    {
#if USE_RK3506
        video_img_w = video_cont_w;
        video_img_h = video_cont_h;
#else
        video_img_w = 720;
        video_img_h = 512;
#endif
    }
    else if (scale_mode == SCALE_4_3)
    {
        if ((video_cont_w * 3.0 / video_cont_h) < 4.0)
        {
            video_img_w = video_cont_w;
            video_img_h = video_img_w * 3.0 / 4.0;
        }
        else
        {
            video_img_h = video_cont_h;
            video_img_w = video_img_h * 4.0 / 3.0;
        }
    }
    else if (scale_mode == SCALE_16_9)
    {
        if ((video_cont_w * 9.0 / video_cont_h) < 16.0)
        {
            video_img_w = video_cont_w;
            video_img_h = video_img_w * 9.0 / 16.0;
        }
        else
        {
            video_img_h = video_cont_h;
            video_img_w = video_img_h * 16.0 / 9.0;
        }
    }

    video_ofs_x = video_area.x1 + (video_cont_w - video_img_w) / 2;
    video_ofs_y = video_area.y1 + (video_cont_h - video_img_h) / 2;

    printf("ofs <%d %d>\n", video_ofs_x, video_ofs_y);
    printf("img <%d %d>\n", video_img_w, video_img_h);
    printf("cont<%d %d>\n", video_cont_w, video_cont_h);
    video_cont_w = video_img_w;
    video_cont_h = video_img_h;
}

void monitor_ui_init()
{
    lv_obj_t *obj;
    lv_obj_t *label;
    lv_area_t addr_area;
    lv_area_t btn_area;
    int btn_size;

    rk_demo_bg_hide();

    if (main)
        return;

    RK_MPI_VO_SetLayerFlush(0);
    style_init();

    main = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(main);
    lv_obj_set_style_pad_all(main, 0, LV_PART_MAIN);
    lv_obj_set_size(main, lv_pct(100), lv_pct(100));
    lv_obj_refr_size(main);

    icon_box = lv_obj_create(main);
    lv_obj_clear_flag(icon_box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(icon_box, 0, LV_PART_MAIN);
    lv_obj_set_width(icon_box, lv_pct(100));
    lv_obj_set_height(icon_box, lv_pct(10));
    lv_obj_align(icon_box, LV_ALIGN_TOP_LEFT, 0, 0);

    btn_return = ui_return_btn_create(icon_box, btn_return_cb, "视频监控");

    player_box = lv_obj_create(main);
    lv_obj_set_width(player_box, lv_pct(100));
    lv_obj_set_height(player_box, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(player_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_align(player_box, LV_ALIGN_BOTTOM_MID, 0, 0);

    ui_rtsp_label = lv_label_create(player_box);
    lv_label_set_text(ui_rtsp_label, "RTSP地址:");
    lv_obj_add_style(ui_rtsp_label, &style_txt_s, LV_PART_MAIN);
    lv_obj_refr_size(ui_rtsp_label);
    lv_obj_refr_pos(ui_rtsp_label);

    ui_rtsp_addr = lv_textarea_create(player_box);
    lv_obj_set_width(ui_rtsp_addr, lv_pct(100));
    lv_textarea_set_text(ui_rtsp_addr, "rtsp://192.168.1.120:8554/");
    lv_textarea_set_password_mode(ui_rtsp_addr, false);
    lv_textarea_set_one_line(ui_rtsp_addr, true);
    lv_obj_add_style(ui_rtsp_addr, &style_txt_m, LV_PART_MAIN);
    lv_obj_add_flag(ui_rtsp_addr, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_rtsp_addr, rtsp_addr_set, LV_EVENT_CLICKED, NULL);
    lv_obj_refr_size(ui_rtsp_addr);
    lv_obj_refr_pos(ui_rtsp_addr);
    lv_obj_get_coords(ui_rtsp_addr, &addr_area);

    btn_size = (lv_area_get_width(&addr_area) - 30) / 4;
    ui_control_box = lv_obj_create(player_box);
    lv_obj_remove_style_all(ui_control_box);
    lv_obj_set_size(ui_control_box, lv_pct(100),
                    lv_area_get_height(&addr_area) * 2);
    lv_obj_set_flex_flow(ui_control_box, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_gap(ui_control_box, 10, LV_PART_MAIN);

    ui_setting = lv_btn_create(ui_control_box);
    lv_obj_set_width(ui_setting, btn_size);
    lv_obj_add_event_cb(ui_setting, rtsp_setting_callback,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(ui_setting, LV_GRID_ALIGN_STRETCH, 0, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 1);
    label = lv_label_create(ui_setting);
    lv_label_set_text(label, "设置");
    lv_obj_add_style(label, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(label);

    ui_pause = lv_btn_create(ui_control_box);
    lv_obj_set_width(ui_pause, btn_size);
    lv_obj_add_event_cb(ui_pause, rtsp_play_pause_callback,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(ui_pause, LV_GRID_ALIGN_STRETCH, 1, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 1);
    ui_pause_label = lv_label_create(ui_pause);
    lv_label_set_text(ui_pause_label, "播放");
    lv_obj_add_style(ui_pause_label, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_pause_label, lv_color_white(),
                                LV_PART_MAIN);
    lv_obj_center(ui_pause_label);

    ui_replay = lv_btn_create(ui_control_box);
    lv_obj_set_width(ui_replay, btn_size);
    lv_obj_add_event_cb(ui_replay, rtsp_play_start_callback,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(ui_replay, LV_GRID_ALIGN_STRETCH, 2, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 1);
    label = lv_label_create(ui_replay);
    lv_label_set_text(label, "重置");
    lv_obj_add_style(label, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(label);

    ui_stop = lv_btn_create(ui_control_box);
    lv_obj_set_width(ui_stop, btn_size);
    lv_obj_add_event_cb(ui_stop, rtsp_play_stop_callback,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(ui_stop, LV_GRID_ALIGN_STRETCH, 3, 1,
                         LV_GRID_ALIGN_STRETCH, 0, 1);
    label = lv_label_create(ui_stop);
    lv_label_set_text(label, "停止");
    lv_obj_add_style(label, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(label);

    lv_obj_refr_size(ui_control_box);
    lv_obj_refr_pos(ui_control_box);
    lv_obj_get_coords(ui_control_box, &btn_area);

    if (btn_area.y2 > (scr_h - 1))
    {
        lv_obj_set_width(ui_rtsp_addr, lv_pct(50));
        lv_obj_set_width(ui_control_box, lv_pct(50));
    }

    lv_obj_refr_size(player_box);
    lv_obj_refr_pos(player_box);
    lv_obj_align(player_box, LV_ALIGN_BOTTOM_MID, 0, 0);

    ui_setting_box = lv_obj_create(main);
    lv_obj_set_size(ui_setting_box, lv_pct(50), LV_SIZE_CONTENT);
    lv_obj_add_flag(ui_setting_box, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_flex_flow(ui_setting_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_radius(ui_setting_box, 0, LV_PART_MAIN);
    lv_obj_center(ui_setting_box);

    ui_checkbox_audio = lv_checkbox_create(ui_setting_box);
    lv_checkbox_set_text(ui_checkbox_audio, "音频");
    lv_obj_add_style(ui_checkbox_audio, &style_txt_m, LV_PART_MAIN);
    if (bAudioEnable)
        lv_obj_add_state(ui_checkbox_audio, LV_STATE_CHECKED);
    lv_obj_add_event_cb(ui_checkbox_audio, ui_checkbox_cb,
                        LV_EVENT_VALUE_CHANGED, &bAudioEnable);

#if VO_SUPPORT_SCALE
    ui_checkbox_ratio_box = lv_obj_create(ui_setting_box);
    lv_obj_set_size(ui_checkbox_ratio_box, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(ui_checkbox_ratio_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(ui_checkbox_ratio_box, ui_ratio_cb,
                        LV_EVENT_CLICKED, NULL);

    label = lv_label_create(ui_checkbox_ratio_box);
    lv_label_set_text(label, "视频比例");
    lv_obj_add_style(label, &style_txt_s, LV_PART_MAIN);
    lv_obj_refr_size(label);
    lv_obj_refr_pos(label);

    ui_checkbox_ratio_4_3 = lv_checkbox_create(ui_checkbox_ratio_box);
    lv_checkbox_set_text(ui_checkbox_ratio_4_3, "4:3");
    lv_obj_add_flag(ui_checkbox_ratio_4_3, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_style(ui_checkbox_ratio_4_3, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_radius(ui_checkbox_ratio_4_3, LV_RADIUS_CIRCLE,
                            LV_PART_INDICATOR);
    lv_obj_set_style_bg_img_src(ui_checkbox_ratio_4_3, NULL,
                                LV_PART_INDICATOR | LV_STATE_CHECKED);
    if (scale_mode == SCALE_4_3)
        lv_obj_add_state(ui_checkbox_ratio_4_3, LV_STATE_CHECKED);
    lv_obj_refr_size(ui_checkbox_ratio_4_3);
    lv_obj_refr_pos(ui_checkbox_ratio_4_3);

    ui_checkbox_ratio_16_9 = lv_checkbox_create(ui_checkbox_ratio_box);
    lv_checkbox_set_text(ui_checkbox_ratio_16_9, "16:9");
    lv_obj_add_flag(ui_checkbox_ratio_16_9, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_style(ui_checkbox_ratio_16_9, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_radius(ui_checkbox_ratio_16_9, LV_RADIUS_CIRCLE,
                            LV_PART_INDICATOR);
    lv_obj_set_style_bg_img_src(ui_checkbox_ratio_16_9, NULL,
                                LV_PART_INDICATOR | LV_STATE_CHECKED);
    if (scale_mode == SCALE_16_9)
        lv_obj_add_state(ui_checkbox_ratio_16_9, LV_STATE_CHECKED);
    lv_obj_refr_size(ui_checkbox_ratio_16_9);
    lv_obj_refr_pos(ui_checkbox_ratio_16_9);

    ui_checkbox_stretch = lv_checkbox_create(ui_checkbox_ratio_box);
    lv_checkbox_set_text(ui_checkbox_stretch, "拉伸");
    lv_obj_add_flag(ui_checkbox_stretch, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_style(ui_checkbox_stretch, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_radius(ui_checkbox_stretch, LV_RADIUS_CIRCLE,
                            LV_PART_INDICATOR);
    lv_obj_set_style_bg_img_src(ui_checkbox_stretch, NULL,
                                LV_PART_INDICATOR | LV_STATE_CHECKED);
    if (scale_mode == SCALE_FULL)
        lv_obj_add_state(ui_checkbox_stretch, LV_STATE_CHECKED);
    lv_obj_refr_size(ui_checkbox_stretch);
    lv_obj_refr_pos(ui_checkbox_stretch);
    lv_obj_refr_size(ui_checkbox_ratio_box);
    lv_obj_refr_pos(ui_checkbox_ratio_box);
#endif
    label = lv_label_create(ui_setting_box);
    lv_label_set_text(label, "注：点击重置后生效");
    lv_obj_add_style(label, &style_txt_s, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_palette_darken(LV_PALETTE_GREY, 2),
                                LV_PART_MAIN);

    obj = lv_btn_create(ui_setting_box);
    lv_obj_add_event_cb(obj, rtsp_setting_callback, LV_EVENT_CLICKED, NULL);
    label = lv_label_create(obj);
    lv_label_set_text(label, "关闭");
    lv_obj_add_style(label, &style_txt_m, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_center(label);

    calc_video_area();

    timer_state = lv_timer_create(ui_update_state, 200, NULL);
}
#endif

