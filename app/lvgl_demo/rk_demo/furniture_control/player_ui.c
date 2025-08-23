#define MAX_FILE_COUNT 10
#define PATH_VIDEO "/oem/"
#define FRAME_DETECT    0
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <errno.h>
#include "main.h"
#include "player_ui.h"
#include "furniture_control_ui.h"

#if MULTIMEDIA_EN
#include "rkadk_common.h"
#include "rkadk_media_comm.h"
#include "rkadk_log.h"
#include "rkadk_param.h"
#include "rkadk_player.h"
#include "rk_mpi_vo.h"
///////////////////// VARIABLES ////////////////////
static lv_obj_t *main = NULL;
static lv_obj_t *btn_return;
static lv_obj_t *player_box = NULL;
static lv_obj_t *icon_box = NULL;
static lv_obj_t *player_box_button = NULL;
static lv_obj_t *player_start_button = NULL;
static lv_obj_t *player_stop_button = NULL;
static lv_obj_t *player_list_button = NULL;
static lv_obj_t *video_label = NULL;
static lv_obj_t *video_list_box = NULL;
static lv_obj_t *video_list = NULL;

static lv_style_t style_txt;
static lv_style_t style_list;

static RKADK_PLAYER_CFG_S stPlayCfg;
static RKADK_MW_PTR pPlayer = NULL;
int play_flag = 0;
int play_end = 0;
int start_clock = 0;
int play_time = 0;
pthread_t ntid;
///////////////////// TEST LVGL SETTINGS ////////////////////

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

#if FRAME_DETECT
static void *GetFramerate(void *arg)
{
    start_clock = 1;
    while (start_clock)
    {
        sleep(1);
        play_time++;
        printf("Software decodes the frame rate: %d\n",
               RKADK_PLAYER_GetSendFrameNum(pPlayer) / play_time);
    }
}

static void close_framerate_detection(void)
{
    start_clock = 0;
    play_time = 0;
    ntid = (pthread_t)0;
}
#endif

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
        play_flag = 0;
        play_end = 1;
#if FRAME_DETECT
        close_framerate_detection();
#endif
        break;
    case RKADK_PLAYER_EVENT_SOF:
        printf("+++++ RKADK_PLAYER_EVENT_SOF +++++\n");
        break;
    case RKADK_PLAYER_EVENT_SEEK_END:
        printf("+++++ RKADK_PLAYER_EVENT_SEEK_END +++++\n");
        break;
    case RKADK_PLAYER_EVENT_ERROR:
        printf("+++++ RKADK_PLAYER_EVENT_ERROR +++++\n");
        break;
    case RKADK_PLAYER_EVENT_PREPARED:
        printf("+++++ RKADK_PLAYER_EVENT_PREPARED +++++\n");
        play_end = 0;
        play_flag = 0;
        break;
    case RKADK_PLAYER_EVENT_PLAY:
        printf("+++++ RKADK_PLAYER_EVENT_PLAY +++++\n");
        play_flag = 1;
        play_end = 0;
        break;
    case RKADK_PLAYER_EVENT_PAUSED:
        printf("+++++ RKADK_PLAYER_EVENT_PAUSED +++++\n");
        play_flag = 0;
        break;
    case RKADK_PLAYER_EVENT_STOPPED:
        printf("+++++ RKADK_PLAYER_EVENT_STOPPED +++++\n");
        break;
    default:
        printf("+++++ Unknown event(%d) +++++\n", enEvent);
        break;
    }
}

static void param_init(RKADK_PLAYER_FRAME_INFO_S *pstFrmInfo)
{
    RKADK_CHECK_POINTER_N(pstFrmInfo);

    memset(pstFrmInfo, 0, sizeof(RKADK_PLAYER_FRAME_INFO_S));
    pstFrmInfo->u32DispWidth = 720;
#if USE_RK3506
    pstFrmInfo->u32DispHeight = RK_PCT_H(57);
#else
    pstFrmInfo->u32DispHeight = 512; //1280*0.4=512
#endif
    pstFrmInfo->u32ImgWidth = pstFrmInfo->u32DispWidth;
    pstFrmInfo->u32ImgHeight = pstFrmInfo->u32DispHeight;

#if USE_RK3506
#if (LV_COLOR_DEPTH == 16)
    pstFrmInfo->u32VoFormat = VO_FORMAT_RGB565;
#elif (LV_COLOR_DEPTH == 32)
    pstFrmInfo->u32VoFormat = VO_FORMAT_RGB888;
#else
#error "Only support DEPTH 16 or 32"
#endif
    pstFrmInfo->u32VoLay = -1; // rkadk select the default first device
    pstFrmInfo->u32VoChn = 1; // ui is 2 . play is 1
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

static void rkadk_init(void)
{
    setenv("rt_vo_disable_vop", "0", 1);
    RKADK_MPI_SYS_Init();
    RKADK_PARAM_Init(NULL, NULL);
    memset(&stPlayCfg, 0, sizeof(RKADK_PLAYER_CFG_S));
    param_init(&stPlayCfg.stFrmInfo);

    stPlayCfg.stAudioCfg.pSoundCard = "default";
    stPlayCfg.stAudioCfg.u32SpeakerVolume = 70;
    stPlayCfg.bEnableVideo = 1;
    stPlayCfg.bEnableAudio = 1;
    stPlayCfg.stFrmInfo.u32FrmInfoX = 0;
    stPlayCfg.stFrmInfo.u32FrmInfoY = RK_PCT_H(10);
    stPlayCfg.stFrmInfo.u32DispBufLen = 2;
    stPlayCfg.bEnableBlackBackground = true;
    stPlayCfg.pfnPlayerCallback = PlayerEventFnTest;
    stPlayCfg.stVdecCfg.u32FrameBufCnt = 4;

    if (RKADK_PLAYER_Create(&pPlayer, &stPlayCfg))
    {
        printf("rkadk: RKADK_PLAYER_Create failed\n");
        return;
    }
}

static void rkadk_deinit(void)
{
    RKADK_PLAYER_Destroy(pPlayer);
    pPlayer = NULL;
    play_flag = 0;
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

static void btn_return_cb(lv_event_t *event)
{
    printf("player_page_jump_furniture_control_callback is into \n");
    furniture_control_ui_init();
    lv_obj_del(main);
    rkadk_deinit();
    main = NULL;
    video_list_box = NULL;
    play_flag = 0;
    rk_demo_bg_show();
}

void video_name_callback(lv_event_t *event)
{
    char *file_name = lv_event_get_user_data(event);
    char path[50] = "/oem/";
    printf("lvgl get play filename: %s\n", file_name);
    strcat(path, file_name);
    free(file_name);
    printf("video_name select file %s\n", path);
    lv_label_set_text(video_label, path);
    lv_obj_del(video_list_box);
    video_list_box = NULL;
    printf("video_name_callback set player file name is %s\n", path);
    if (pPlayer != NULL)
    {
        printf("video_name_callback: stop and deinit pPlayer\n");
        rkadk_deinit();
    }
    if (pPlayer == NULL)
    {
        printf("video_name_callback: rkadk_init pPlayer\n");
        rkadk_init();
    }
    if (play_flag == 1)
    {
        play_flag = 0;
    }
    int ret = RKADK_PLAYER_SetDataSource(pPlayer, path);
    if (ret)
    {
        printf("rkadk: SetDataSource failed, ret = %d\n", ret);
    }
    ret = RKADK_PLAYER_Prepare(pPlayer);
    if (ret)
    {
        printf("rkadk: Prepare failed, ret = %d\n", ret);
    }
    ret = RKADK_PLAYER_Play(pPlayer);
    if (ret)
    {
        printf("rkadk: Play failed, ret = %d\n", ret);
    }
    play_end = 0;
#if FRAME_DETECT
    pthread_create(&ntid, NULL, GetFramerate, NULL);
    close_framerate_detection();
#endif
}

static char *strlwr(char *s)
{
    char *p = s;
    for (; *s; s++)
    {
        *s = tolower(*s);
    }
    return p;
}

static char *strrstr(const char *str, const char *token)
{
    int len = strlen(token);
    const char *p = str + strlen(str);

    while (str <= --p)
        if (p[0] == token[0] && strncmp(p, token, len) == 0)
            return (char *)p;
    return NULL;
}

int file_is_supported(char *filepath)
{
    int ret = 0;

    if (strstr(filepath, ".") == NULL)
        return 0;

    char *suffix = strlwr(strdup(strrstr(filepath, ".") + 1));
    static const char *formats[] =
    {
        "mp4", "mp3"
    };

    for (int i = 0; i < sizeof(formats) / sizeof(formats[0]); i++)
    {
        if (strcmp(suffix, formats[i]) == 0)
        {
            ret = 1;
            break;
        }
    }

    printf("%s is %ssupported, [%s]\n", filepath, ret ? "" : "not ", suffix);

    free(suffix);

    return ret;
}


void player_list_button_callback(lv_event_t *event)
{
    printf("player_list_button_callback is into \n");
    DIR *dir;
    struct dirent *entry;
    int file_count = 0;

    if (video_list_box == NULL)
    {
        dir = opendir(PATH_VIDEO);
        if (dir == NULL)
        {
            //fprintf(stderr, "err: %s\n", strerror(errno));
            printf("Error opening directory /oem\n");
            return;
        }
        printf("create video_list_box\n");
        video_list_box = lv_obj_create(main);
        //lv_obj_remove_style_all(video_list_box);
        lv_obj_set_width(video_list_box, lv_pct(50));
        lv_obj_set_height(video_list_box, lv_pct(40));
        lv_obj_align_to(video_list_box, player_box_button, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

        video_list = lv_list_create(video_list_box);
        lv_obj_set_size(video_list, lv_pct(100), lv_pct(100));
        lv_obj_center(video_list);
        lv_obj_add_style(video_list, &style_list, LV_PART_MAIN);
        lv_obj_set_style_pad_column(video_list, 10, LV_PART_MAIN);

        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_REG)
            {
                if (file_is_supported(entry->d_name))
                {
                    //add_file_to_list(entry->d_name);
                    lv_obj_t *obj_text = lv_list_add_btn(video_list, NULL, entry->d_name);
                    lv_obj_add_flag(obj_text, LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_add_event_cb(obj_text, video_name_callback, LV_EVENT_CLICKED,
                                        strdup(entry->d_name));
                    file_count++;
                    if (file_count >= MAX_FILE_COUNT)
                    {
                        break;
                    }
                }
            }
        }
        closedir(dir);
    }
    else
    {
        printf("del video_list_box\n");
        lv_obj_del(video_list_box);
        video_list_box = NULL;
        if (play_flag == 0)
            RK_MPI_VO_SetLayerFlush(-1);
    }
}

void player_start_button_callback(lv_event_t *event)
{
    printf("player_start_button_callback into\n");
    char *file = lv_label_get_text(video_label);
    if (strncmp(file, "/oem/", 5))
    {
        printf("rkadk: !!! You have not selected the file to play !!!\n");
        return;
    }
    printf("rkadk: the file to play %s\n", file);
    int ret = 0;
    if ((play_flag == 1) || (play_end == 1))
    {
        printf("Video is playing! replay! \n");
#if FRAME_DETECT
        close_framerate_detection();
#endif
        RKADK_PLAYER_Stop(pPlayer);
        printf("Video is play file: %s \n", file);
        ret = RKADK_PLAYER_SetDataSource(pPlayer, file);
        if (ret)
        {
            printf("rkadk: SetDataSource failed, ret = %d\n", ret);
        }
        ret = RKADK_PLAYER_Prepare(pPlayer);
        if (ret)
        {
            printf("rkadk: Prepare failed, ret = %d\n", ret);
        }
        ret = RKADK_PLAYER_Play(pPlayer);
        if (ret)
        {
            printf("rkadk: Play failed, ret = %d\n", ret);
        }
        play_end = 0;
#if FRAME_DETECT
        pthread_create(&ntid, NULL, GetFramerate, NULL);
#endif
        return;

    }
    else
    {
        ret = RKADK_PLAYER_Play(pPlayer);
        if (ret)
        {
            printf("rkadk: Play failed, ret = %d\n", ret);
        }
#if FRAME_DETECT
        pthread_create(&ntid, NULL, GetFramerate, NULL);
#endif
        return;
    }
}

void player_stop_button_callback(lv_event_t *event)
{
    printf("player_stop_button_callback into\n");
    if (play_flag == 0)
    {
        printf("Video is stop!\n");
        return;
    }
    int ret = RKADK_PLAYER_Pause(pPlayer);
    if (ret)
    {
        printf("rkadk: Pause failed, ret = %d\n", ret);
    }
#if FRAME_DETECT
    close_framerate_detection();
#endif
}

static void player_btn_draw(lv_obj_t *parent, struct btn_desc *desc)
{
    lv_obj_t *obj, *img, *label;

    obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(100));
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(obj, MAIN_COLOR, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, 15, LV_PART_MAIN);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_center(obj);

    if (desc->img)
    {
        img = lv_img_create(obj);
        lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_img_set_src(img, desc->img);
    }

    if (desc->text)
    {
        label = lv_label_create(obj);
        lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_style(label, &style_txt_l, LV_PART_MAIN);
        lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
        lv_label_set_text(label, desc->text);
    }
}

static struct btn_desc player_btn[] =
{
    {
        .obj  = &player_list_button,
        .text  = "list",
        .w    = 1,
        .h    = 1,
        .draw = player_btn_draw,
        .cb   = player_list_button_callback,
    },
    {
        .obj  = &player_start_button,
        .text  = "play",
        .w    = 1,
        .h    = 1,
        .draw = player_btn_draw,
        .cb   = player_start_button_callback,
    },
    {
        .obj  = &player_stop_button,
        .text  = "stop",
        .w    = 1,
        .h    = 1,
        .draw = player_btn_draw,
        .cb   = player_stop_button_callback,
    }
};

static lv_coord_t col_dsc[] = {200, 200, 200, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {200, LV_GRID_TEMPLATE_LAST};

static struct btn_matrix_desc btn_desc =
{
    .col_dsc = col_dsc,
    .row_dsc = row_dsc,
    .pad = 5,
    .gap = 5,
    .desc = player_btn,
    .btn_cnt = sizeof(player_btn) / sizeof(player_btn[0]),
};

///////////////////// SCREENS ////////////////////
void player_ui_init(void)
{
    rk_demo_bg_hide();

    RK_MPI_VO_SetLayerFlush(-1);
    if (main)
        return;

    style_init();
    main = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(main);
    lv_obj_set_style_pad_all(main, 0, LV_PART_MAIN);
    lv_obj_set_size(main, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(main, 0, LV_PART_MAIN);
    lv_obj_refr_size(main);

    icon_box = lv_obj_create(main);
    lv_obj_clear_flag(icon_box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(icon_box, 0, LV_PART_MAIN);
    lv_obj_set_width(icon_box, lv_pct(100));
    lv_obj_set_height(icon_box, lv_pct(10));
    lv_obj_align(icon_box, LV_ALIGN_TOP_LEFT, 0, 0);

    btn_return = ui_return_btn_create(icon_box, btn_return_cb, "宣传视频");

#if USE_RK3506 //Because the rk3506 has hardware rga, video can be scaled
    player_box = lv_obj_create(main);
    lv_obj_set_width(player_box, lv_pct(100));
    lv_obj_set_height(player_box, lv_pct(33));
    lv_obj_align(player_box, LV_ALIGN_TOP_LEFT, 0, lv_pct(67));
#else
    player_box = lv_obj_create(main);
    lv_obj_set_width(player_box, lv_pct(100));
    lv_obj_set_height(player_box, lv_pct(50));
    lv_obj_align(player_box, LV_ALIGN_TOP_LEFT, 0, lv_pct(50));
#endif
    col_dsc[0] = RK_PCT_W(27);
    col_dsc[1] = RK_PCT_W(27);
    col_dsc[2] = RK_PCT_W(27);
    row_dsc[0] = RK_PCT_W(27);
    player_box_button = ui_btnmatrix_create(player_box, &btn_desc);

    lv_obj_set_style_bg_opa(player_start_button, LV_OPA_COVER, LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(player_stop_button, LV_OPA_COVER, LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(player_list_button, LV_OPA_COVER, LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(player_start_button, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(player_stop_button, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(player_list_button, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(player_start_button, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(player_stop_button, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(player_list_button, 0, LV_PART_MAIN);

    video_label = lv_label_create(icon_box);
    lv_obj_set_width(video_label, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(video_label, LV_SIZE_CONTENT);    /// 1
    lv_obj_align(video_label, LV_ALIGN_CENTER, 0, lv_pct(10));
    lv_obj_add_style(video_label, &style_txt_m, LV_PART_MAIN);
    lv_label_set_text(video_label, "");
}
#endif
