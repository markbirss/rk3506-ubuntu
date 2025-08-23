#include <stdio.h>
#include <stdlib.h>

#include "asr.h"
#include "main.h"

#if ASR_EN
lv_obj_t *icon;
lv_obj_t *label[2];

LV_IMG_DECLARE(asr_logo);
static lv_style_t style_asr;

static lv_anim_t logo_anim;
static lv_anim_t label0_anim;
static lv_anim_t label1_anim;
static lv_anim_t label2_anim;
static lv_timer_t *timer;

struct asr_cmd
{
    const char *txt[3];
    int ans;
};

char *answer_txt[][2] =
{
    {"我在", "I'm here."},
    {"好的", "OK."},
    {"正在执行", "Executing."},
    {"已开启", "Turned on."},
    {"已关闭", "Turned off."},
    {"欢迎回家", "Welcome home."},
    {"再见", "Goodbye."},
};

struct asr_cmd cmds[] =
{
    {"小瑞小瑞.", "XiaoRui, XiaoRui.", "XiaoRui XiaoRui.", 0},
#if 0
    {"打开油烟机.", "Turn on the range hood.", "Da kai you yan ji.", 3},
    {"关闭油烟机.", "Turn off the range hood.", "Guan bi you yan ji.", 4},
    {"打开热水器.", "Turn on the water heater.", "Da kai re shui qi.", 3},
    {"关闭热水器.", "Turn off the water heater.", "Guan bi re shui qi.", 4},
    {"打开机顶盒.", "Turn on the set-top box.", "Da kai ji ding he.", 3},
    {"关闭机顶盒.", "Turn off the set-top box.", "Guan bi ji ding he.", 4},
    {"开启语音控制.", "Enable Voice Control.", "Kai qi yu yin kong zhi.", 1},
    {"打开灯光.", "Turn on the lights.", "Da kai deng guang.", 3},
    {"关闭灯光.", "Turn off the lights.", "Guan bi deng guang.", 4},
    {"打开卧室的灯.", "Turn on the bedroom light.", "Da kai wo shi de deng.", 3},
    {"关掉卧室的灯.", "Turn off the bedroom light.", "Guan diao wo shi de deng.", 4},
    {"打开客厅的灯.", "Turn on the light in the living room.", "Da kai ke ting de deng.", 3},
    {"关掉客厅的灯.", "Turn off the lights in the living room.", "Guan diao ke ting de deng.", 4},
    {"打开厨房的灯.", "Turn on the kitchen light.", "Da kai chu fang de deng.", 3},
    {"关掉厨房的灯.", "Turn off the kitchen lights.", "Guan diao chu fang de deng.", 4},
    {"打开卫生间的灯.", "Turn on the light in the bathroom.", "Da kai wei sheng jian de deng.", 3},
    {"关掉卫生间的灯.", "Turn off the light in the bathroom.", "Guan diao wei sheng jian de deng.", 4},
    {"打开所有的灯.", "Turn on all the lights.", "Ga kai suo you de deng.", 3},
    {"关掉所有的灯.", "Turn off all the lights.", "Guan diao suo you de deng.", 4},
    {"打开台灯.", "Turn on the lamp.", "Da kai tai deng.", 3},
    {"打开小夜灯.", "Turn on the night light.", "Da kai xiao ye deng.", 3},
    {"关闭台灯.", "Turn off the lamp.", "Guan bi tai deng.", 4},
    {"关闭小夜灯.", "Turn off the night light.", "Guan bi xiao ye deng.", 4},
    {"打开电视.", "Turn on the TV.", "Da kai dian shi.", 3},
    {"关闭电视.", "Turn off the TV.", "Guan bi dian shi.", 4},
    {"声音大点.", "Speak louder.", "Sheng yin da dian.", 1},
    {"声音小点.", "Keep your voice down.", "Sheng yin xiao dian.", 1},
    {"上一个频道.", "Last channel.", "Shang yi ge pin dao.", 1},
    {"下一个频道.", "Next channel.", "Xia yi ge pin dao.", 1},
    {"打开空调.", "Turn on the air conditioner.", "Da kai kong tiao.", 3},
    {"关闭空调.", "Turn off the air conditioner.", "Guan bi kong tiao.", 4},
    {"温度高点.", "High temperature.", "Wen du gao dian.", 1},
    {"温度低点.", "Low temperature.", "Wen du di dian.", 1},
    {"制热模式.", "Heating mode.", "Zhi re mo shi.", 1},
    {"制冷模式.", "Refrigeration mode.", "Zhi leng mo shi.", 1},
    {"睡眠模式.", "Sleep mode.", "Shui mian mo shi.", 1},
    {"省电模式.", "Power saving mode.", "Sheng dian mo shi.", 1},
    {"打开扫地机.", "Turn on the sweeper.", "Da kai sao di ji.", 3},
    {"开始清扫.", "Start cleaning up.", "Kai shi qing sao.", 2},
    {"关闭扫地机.", "Turn off the sweeper.", "Guan bi sao di ji.", 4},
    {"停止清扫.", "Stop cleaning.", "Ting zhi qing sao.", 2},
    {"电影频道.", "The Movie Channel.", "Dian ying pin dao.", 1},
    {"音乐频道.", "The Music Channel.", "Yin yue pin dao.", 1},
    {"体育频道.", "The Sports Channel.", "Ti yu pin dao.", 1},
    {"综艺频道.", "The Variety Channel.", "Zong yi pin dao.", 1},
    {"我回来了.", "I'm back.", "Wo hui lai le.", 5},
    {"我要睡觉了.", "I'm going to bed.", "Wo yao shui jiao le.", 1},
    {"我出去了.", "I'm going out.", "Wo chu qu le.", 6},
#else
    {"开灯.", "Turn on the light.", "Kai deng.", -1},
    {"闭灯.", "Turn off the light.", "Guan deng", -1},
    {"打开灯.", "Turn on the light.", "Da kai deng.", 3},
    {"关闭灯.", "Turn off the light.", "Guan bi deng.", 4},
    {"打开调光灯.", "Turn on dimming lights.", "Da kai tiao guan deng.", 3},
    {"关闭调光灯.", "Turn off dimming lights.", "Guan bi tiao guan deng.", 4},
    {"打开射灯.", "Turn on spotlight.", "Da kai she deng.", 3},
    {"关闭射灯.", "Turn off spotlight.", "Guan bi she deng.", 4},
    {"打开筒灯.", "Turn on downlight.", "Da kai tong deng.", 3},
    {"关闭筒灯.", "Turn off downlight.", "Guan bi tong deng.", 4},
    {"打开灯带.", "Turn on light strip.", "Da kai deng dai.", 3},
    {"关闭灯带.", "Turn off light strip.", "Guan bi deng dai.", 4},
    {"打开地暖.", "Turn on floor heating.", "Da kai di nuan.", 3},
    {"关闭地暖.", "Turn off floor heating.", "Guan bi di nuan.", 4},
    {"打开新风.", "Turn on fresh air.", "Da kai xin feng.", 3},
    {"关闭新风.", "Turn off fresh air.", "Guan bi xin feng.", 4},
    {"打开空调.", "Turn on air conditioner.", "Da kai kong tiao.", 3},
    {"关闭空调.", "Turn off air conditioner.", "Guan bi kong tiao.", 4},
    {"打开窗帘.", "Turn on curtain.", "Da kai chuang lian.", 3},
    {"关闭窗帘.", "Turn off curtain.", "Guan bi chuang lian.", 4},
    {"暂停窗帘.", "Puase the curtain.", "Zan ting chuang lian.", 2},
#endif
};

int wakeup = 0;
int wakeid;
int wake_update = 0;
void cmd_answer(int cmd);

static void logo_anim_cb(void *var, int32_t v)
{
    if (v > 15)
        v = 30 - v;
    lv_obj_align(var, LV_ALIGN_BOTTOM_LEFT, 50, -50 - v);
}

static void label0_anim_cb(void *var, int32_t v)
{
    lv_obj_add_flag(label[1], LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(var, LV_OBJ_FLAG_HIDDEN);
    lv_obj_align(var, LV_ALIGN_BOTTOM_RIGHT, -50, v);
    if (v == -50)
    {
        lv_anim_set_delay(&label1_anim, 200);
        lv_anim_set_delay(&label2_anim, 200);
        lv_anim_start(&label1_anim);
        lv_anim_start(&label2_anim);
    }
}

static void label2_anim_cb(void *var, int32_t v)
{
    lv_obj_align(var, LV_ALIGN_BOTTOM_RIGHT, -50, v);
}

static void label1_anim_cb(void *var, int32_t v)
{
    lv_obj_clear_flag(var, LV_OBJ_FLAG_HIDDEN);
    lv_obj_align(var, LV_ALIGN_BOTTOM_LEFT, 180, v);
//    if (v == -50) {
//        lv_anim_set_delay(&label0_anim, 3000);
//        lv_anim_start(&label0_anim);
//    }
}

static void timer_cb(struct _lv_timer_t *_timer)
{
    static int show = 0;
    if (wakeup)
    {
        if (!show)
        {
            lv_anim_start(&logo_anim);
            lv_obj_clear_flag(icon, LV_OBJ_FLAG_HIDDEN);
            show = 1;
            lv_anim_del(&label0_anim, NULL);
            lv_anim_del(&label1_anim, NULL);
            lv_anim_del(&label2_anim, NULL);
        }
    }
    else
    {
        if (show)
        {
            lv_obj_add_flag(icon, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(label[0], LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(label[1], LV_OBJ_FLAG_HIDDEN);
            show = 0;
            lv_anim_del(&label0_anim, NULL);
            lv_anim_del(&label1_anim, NULL);
            lv_anim_del(&label2_anim, NULL);
        }
    }
    if (!wake_update)
        return;
    wake_update = 0;
    lv_obj_add_flag(label[0], LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(label[1], LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(label[0], cmds[wakeid].txt[0]);
    lv_label_set_text(label[1], answer_txt[cmds[wakeid].ans][0]);
    lv_anim_del(&label0_anim, NULL);
    lv_anim_del(&label1_anim, NULL);
    lv_anim_del(&label2_anim, NULL);
    lv_anim_start(&label0_anim);
}

void asr_icon_create(lv_obj_t *parent)
{
    lv_style_init(&style_asr);
    lv_style_set_bg_opa(&style_asr, LV_OPA_COVER);
    lv_style_set_bg_color(&style_asr, lv_color_make(0xf0, 0xf0, 0xf0));
    lv_style_set_radius(&style_asr, 10);
    lv_style_set_pad_all(&style_asr, 10);
    lv_style_set_text_font(&style_asr, ttf_main_s.font);

    icon = lv_img_create(parent);
    lv_obj_align(icon, LV_ALIGN_BOTTOM_LEFT, 50, -50);
    lv_img_set_src(icon, &asr_logo);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_HIDDEN);

    label[0] = lv_label_create(parent);
    lv_obj_align(label[0], LV_ALIGN_BOTTOM_RIGHT, -50, -100);
    lv_obj_add_style(label[0], &style_asr, LV_PART_MAIN);
    lv_obj_add_flag(label[0], LV_OBJ_FLAG_HIDDEN);

    label[1] = lv_label_create(parent);
    lv_obj_align(label[1], LV_ALIGN_BOTTOM_LEFT, 180, -50);
    lv_obj_add_style(label[1], &style_asr, LV_PART_MAIN);
    lv_obj_add_flag(label[1], LV_OBJ_FLAG_HIDDEN);

    lv_anim_init(&logo_anim);
    lv_anim_set_var(&logo_anim, icon);
    lv_anim_set_values(&logo_anim, 0, 30);
    lv_anim_set_time(&logo_anim, 2000);
    lv_anim_set_exec_cb(&logo_anim, logo_anim_cb);
    lv_anim_set_path_cb(&logo_anim, lv_anim_path_ease_in_out);
    lv_anim_set_repeat_count(&logo_anim, LV_ANIM_REPEAT_INFINITE);

    lv_anim_init(&label0_anim);
    lv_anim_set_var(&label0_anim, label[0]);
    lv_anim_set_values(&label0_anim, -20, -50);
    lv_anim_set_time(&label0_anim, 200);
    lv_anim_set_delay(&label0_anim, 300);
    lv_anim_set_exec_cb(&label0_anim, label0_anim_cb);
    lv_anim_set_path_cb(&label0_anim, lv_anim_path_overshoot);
    lv_anim_set_early_apply(&label0_anim, 0);

    lv_anim_init(&label2_anim);
    lv_anim_set_var(&label2_anim, label[0]);
    lv_anim_set_values(&label2_anim, -51, -100);
    lv_anim_set_time(&label2_anim, 200);
    lv_anim_set_exec_cb(&label2_anim, label2_anim_cb);
    lv_anim_set_path_cb(&label2_anim, lv_anim_path_overshoot);
    lv_anim_set_early_apply(&label2_anim, 0);

    lv_anim_init(&label1_anim);
    lv_anim_set_var(&label1_anim, label[1]);
    lv_anim_set_values(&label1_anim, -20, -50);
    lv_anim_set_time(&label1_anim, 200);
    lv_anim_set_exec_cb(&label1_anim, label1_anim_cb);
    lv_anim_set_path_cb(&label1_anim, lv_anim_path_overshoot);
    lv_anim_set_early_apply(&label1_anim, 0);

    timer = lv_timer_create(timer_cb, 1, NULL);
}

void asr_icon_destroy(void)
{
    lv_timer_del(timer);
    lv_anim_del(&label0_anim, NULL);
    lv_anim_del(&label1_anim, NULL);
    lv_anim_del(&label2_anim, NULL);
    lv_anim_del(&logo_anim, NULL);
    lv_obj_del(label[0]);
    lv_obj_del(label[1]);
    lv_obj_del(icon);
}

void asr_update(int id)
{
    if (id == -1)
    {
        wakeup = 0;
        return;
    }
    if (id > (sizeof(cmds) / sizeof(cmds[0])))
        return;
    if (cmds[id].ans == -1)
        return;
    wakeid = id;
    wake_update = 1;
    wakeup = 1;
    printf("  %s\n", cmds[id].txt[0]);
    cmd_answer(cmds[id].ans);
}

void printf_asr_cmds(void)
{
    int i, max;
    max = (sizeof(cmds) / sizeof(cmds[0]));

    printf("asr cmds:\n");
    for (i = 0; i < max; i++)
    {
        if (cmds[i].ans == -1)
            continue;
        printf("  %s %s\n", cmds[i].txt[0],
               cmds[i].txt[2]);
    }
}
#endif

