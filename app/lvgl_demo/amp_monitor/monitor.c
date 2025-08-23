#include <lvgl/lvgl.h>

#include "cpu.h"
#include "main.h"
#include "soc.h"

static pthread_t socket_tid;
static pthread_t rpmsg_tid;

static lv_ft_info_t ttf_main;
static lv_ft_info_t ttf_main_s;
static lv_obj_t *scr;
static lv_obj_t *chart;
static lv_obj_t *title;
static lv_obj_t *axis_title_x;
static lv_obj_t *axis_title_y;
static lv_obj_t *tag[2];
static lv_obj_t *tag_title[2];
static lv_obj_t *tag_now[2];
static lv_obj_t *tag_avg[2];
static lv_obj_t *tag_max[2];
static lv_obj_t *tag_min[2];
static lv_chart_series_t *ser[2];
static lv_timer_t *timer;
static lv_coord_t y_range = 200;
static lv_coord_t y_ticks = 3;
static lv_obj_t **tick_label_y = NULL;
static lv_obj_t *tick_label_x[31];

static lv_coord_t samples[300];
static lv_coord_t sample_x[300];
static lv_coord_t sample_y[300] = {0, };
static lv_coord_t sample2_x[300] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60};
static lv_coord_t sample2_y[300] = {0, };

static lv_obj_t *time_label;
static lv_obj_t *hw_version;
static lv_obj_t *btn_reset;
static lv_obj_t *label_reset;
static lv_obj_t *cpu_usage;
static lv_obj_t *label_cfg;

static struct timeval tstart;

static char *kernel_version;
static char *compatible_name;
static char *soc_name;

static void chart_y_tick_update(int new_ticks)
{
    int pad_top = lv_obj_get_style_pad_top(chart, LV_PART_MAIN);
    int pad_bottom = lv_obj_get_style_pad_bottom(chart, LV_PART_MAIN);
    int y = pad_top;
    int gap_y;

    if (tick_label_y)
    {
        if (new_ticks == y_ticks)
            return;

        for (int i = 0; i < y_ticks; i++)
            lv_obj_del(tick_label_y[i]);
        free(tick_label_y);
    }

    y_ticks = new_ticks;
    lv_chart_set_div_line_count(chart, y_ticks, 4);
    tick_label_y = calloc(y_ticks, sizeof(lv_obj_t *));

    gap_y = (lv_obj_get_height(chart) - pad_top - pad_bottom) / (y_ticks - 1);

    for (int i = 0; i < y_ticks; i++)
    {
        tick_label_y[i] = lv_label_create(scr);
        if (i)
            lv_label_set_text_fmt(tick_label_y[i], "10^%d", i);
        else
            lv_label_set_text(tick_label_y[i], "0");
        lv_obj_set_style_text_font(tick_label_y[i], ttf_main_s.font,
                                   LV_PART_MAIN);
        lv_obj_refr_size(tick_label_y[i]);
        lv_obj_align_to(tick_label_y[i], chart, LV_ALIGN_OUT_LEFT_BOTTOM,
                        -10, -(y - lv_obj_get_height(tick_label_y[i]) / 2));
        y += gap_y;
    }
}

static void chart_range_update(void)
{
    int max = y_range;
    int max_log10;

    for (int i = 0; i < ARRAY_SIZE(sample_y); i++)
    {
        if (sample_y[i] > max)
            max = sample_y[i];
        if (sample2_y[i] > max)
            max = sample2_y[i];
    }

    max_log10 = max / 100 + (max % 100 ? 1 : 0);
    if ((max_log10 * 100) != y_range)
    {
        y_range = max_log10 * 100;
        lv_chart_set_range(chart,
                           LV_CHART_AXIS_PRIMARY_Y, 0, y_range);
        chart_y_tick_update(max_log10 + 1);
    }
}

static void chart_update(lv_timer_t *e)
{
    struct timeval tv;
    time_t dur;
    int32_t min, max, avg, cur;
    float usage;

    usage = (float)get_cpu_usage();
    lv_label_set_text_fmt(cpu_usage, "%.1f%% CPU", usage);

    gettimeofday(&tv, NULL);
    dur = tv.tv_sec - tstart.tv_sec;
    lv_label_set_text_fmt(time_label, "%02u:%02u:%02u",
                          dur / 60 / 60,
                          dur / 60 % 60,
                          dur % 60);

    if (socket_read(samples, &min, &max, &avg, &cur))
    {
        lv_label_set_text_fmt(tag_min[0], "min: %.3fus", min / 10.0);
        lv_label_set_text_fmt(tag_max[0], "max: %.3fus", max / 10.0);
        lv_label_set_text_fmt(tag_avg[0], "avg: %.3fus", avg / 10.0);
        lv_label_set_text_fmt(tag_now[0], "now: %.3fus", cur / 10.0);
        for (int i = 0; i < ARRAY_SIZE(sample_y); i++)
            sample_y[i] = log10(samples[i]) * 100;
        lv_obj_invalidate(chart);
    }

    if (rpmsg_read(samples, &min, &max, &avg, &cur))
    {
        lv_label_set_text_fmt(tag_min[1], "min: %.3fus", min / 1000.0);
        lv_label_set_text_fmt(tag_max[1], "max: %.3fus", max / 1000.0);
        lv_label_set_text_fmt(tag_avg[1], "avg: %.3fus", avg / 1000.0);
        lv_label_set_text_fmt(tag_now[1], "now: %.3fus", cur / 1000.0);

        for (int i = 0; i < 7; i++)
            sample2_y[i] = log10(samples[i]) * 100;
        lv_obj_invalidate(chart);
    }

    chart_range_update();
}

static void font_init(void)
{
    printf("%s\n", __func__);
    lv_freetype_init(64, 1, 0);

    ttf_main.weight = 50;
    ttf_main.name =
        "/usr/share/fonts/source-han-sans-cn/SourceHanSansCN-Regular.otf";
    ttf_main.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&ttf_main);

    ttf_main_s.weight = 30;
    ttf_main_s.name =
        "/usr/share/fonts/source-han-sans-cn/SourceHanSansCN-Regular.otf";
    ttf_main_s.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&ttf_main_s);
}

static void reset_cb(lv_event_t *e)
{
    printf("%s\n", __func__);
    memset(sample_y, 0, sizeof(sample_y));
    memset(sample2_y, 0, sizeof(sample2_y));
    y_range = 200;
    lv_chart_set_range(chart,
                       LV_CHART_AXIS_PRIMARY_Y, 0, y_range);
    chart_y_tick_update(y_range / 100 + 1);
    lv_label_set_text(tag_min[0], "min: 0us");
    lv_label_set_text(tag_max[0], "max: 0us");
    lv_label_set_text(tag_avg[0], "avg: 0us");
    lv_label_set_text(tag_now[0], "now: 0us");
    lv_label_set_text(tag_min[1], "min: 0us");
    lv_label_set_text(tag_max[1], "max: 0us");
    lv_label_set_text(tag_avg[1], "avg: 0us");
    lv_label_set_text(tag_now[1], "now: 0us");
    lv_obj_invalidate(chart);
    socket_reset();
    rpmsg_reset();
    gettimeofday(&tstart, NULL);
}

void monitor(void)
{
    font_init();

    kernel_version = get_kernel_version(NULL);
    compatible_name = get_compatible_name();
    soc_name = get_soc_name(compatible_name);

    scr = lv_scr_act();
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_refr_size(scr);

    hw_version = lv_label_create(scr);
    lv_label_set_text(hw_version, soc_name);
    lv_obj_set_style_text_font(hw_version, ttf_main.font, LV_PART_MAIN);
    lv_obj_align(hw_version, LV_ALIGN_BOTTOM_LEFT, 2, -2);

    time_label = lv_label_create(scr);
    lv_label_set_text(time_label, "00:00:00");
    lv_obj_set_style_text_font(time_label, ttf_main.font, LV_PART_MAIN);
    lv_obj_align_to(time_label, hw_version, LV_ALIGN_OUT_TOP_LEFT, 0, -2);

    title = lv_label_create(scr);
    lv_label_set_text_fmt(title, "cyclictest with kernel-%s", kernel_version);
    lv_obj_set_style_text_font(title, ttf_main.font, LV_PART_MAIN);

    axis_title_x = lv_label_create(scr);
    lv_label_set_text(axis_title_x, "latency in us");
    lv_obj_set_style_text_font(axis_title_x, ttf_main.font, LV_PART_MAIN);

    axis_title_y = lv_label_create(scr);
    lv_label_set_text(axis_title_y, "occurrence");
    lv_obj_set_style_text_font(axis_title_y, ttf_main.font, LV_PART_MAIN);

    for (int i = 0; i < 2; i++)
    {
        tag[i] = lv_obj_create(scr);
        lv_obj_set_size(tag[i], LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(tag[i], LV_FLEX_FLOW_COLUMN);
        tag_title[i] = lv_label_create(tag[i]);
        tag_min[i] = lv_label_create(tag[i]);
        tag_max[i] = lv_label_create(tag[i]);
        tag_avg[i] = lv_label_create(tag[i]);
        tag_now[i] = lv_label_create(tag[i]);
        lv_obj_set_style_text_font(tag_title[i], ttf_main.font, LV_PART_MAIN);
        lv_obj_set_style_text_font(tag_min[i], ttf_main.font, LV_PART_MAIN);
        lv_obj_set_style_text_font(tag_max[i], ttf_main.font, LV_PART_MAIN);
        lv_obj_set_style_text_font(tag_avg[i], ttf_main.font, LV_PART_MAIN);
        lv_obj_set_style_text_font(tag_now[i], ttf_main.font, LV_PART_MAIN);
    }

    lv_obj_set_style_bg_color(tag[0],
                              lv_palette_lighten(LV_PALETTE_PINK, 1),
                              LV_PART_MAIN);
    lv_label_set_text(tag_title[0], kernel_version);
    lv_label_set_text(tag_min[0], "min: 0us");
    lv_label_set_text(tag_max[0], "max: 0us");
    lv_label_set_text(tag_avg[0], "avg: 0us");
    lv_label_set_text(tag_now[0], "now: 0us");

    lv_obj_set_style_bg_color(tag[1],
                              lv_palette_lighten(LV_PALETTE_BLUE, 1),
                              LV_PART_MAIN);
    lv_label_set_text(tag_title[1], "Bare-metal");
    lv_label_set_text(tag_min[1], "min: 0us");
    lv_label_set_text(tag_max[1], "max: 0us");
    lv_label_set_text(tag_avg[1], "avg: 0us");
    lv_label_set_text(tag_now[1], "now: 0us");

    lv_obj_refr_size(tag_title[0]);
    lv_obj_refr_size(tag[0]);
    lv_obj_set_width(tag[1], lv_obj_get_width(tag[0]));

    chart = lv_chart_create(scr);
    lv_obj_set_size(chart, lv_pct(60), lv_pct(70));
    lv_chart_set_div_line_count(chart, y_ticks, 4);
    lv_chart_set_type(chart, LV_CHART_TYPE_SCATTER);
    lv_chart_set_range(chart,
                       LV_CHART_AXIS_PRIMARY_X, 0, 300);
    lv_chart_set_range(chart,
                       LV_CHART_AXIS_PRIMARY_Y, 0, y_range);
    lv_chart_set_axis_tick(chart,
                           LV_CHART_AXIS_PRIMARY_X, 10, 5, 31, 1, false, 0);
    lv_chart_set_axis_tick(chart,
                           LV_CHART_AXIS_PRIMARY_Y, 10, 5, 3, 2, false, 100);

    for (int i = 0; i < ARRAY_SIZE(sample_x); i++)
        sample_x[i] = i;

    lv_chart_set_point_count(chart, ARRAY_SIZE(sample_x));
    ser[0] = lv_chart_add_series(chart,
                                 lv_palette_lighten(LV_PALETTE_PINK, 1), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_x_array(chart, ser[0], sample_x);
    lv_chart_set_ext_y_array(chart, ser[0], sample_y);
    ser[1] = lv_chart_add_series(chart,
                                 lv_palette_lighten(LV_PALETTE_BLUE, 1), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_x_array(chart, ser[1], sample2_x);
    lv_chart_set_ext_y_array(chart, ser[1], sample2_y);

    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_align_to(chart, title, LV_ALIGN_OUT_BOTTOM_MID, -120, 50);
    lv_obj_align_to(axis_title_x, chart, LV_ALIGN_OUT_BOTTOM_RIGHT, -100, 60);
    lv_obj_align_to(axis_title_y, chart, LV_ALIGN_OUT_LEFT_TOP, 0, -80);
    lv_obj_align_to(tag[0], chart, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);
    lv_obj_align_to(tag[1], tag[0], LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    lv_obj_refr_size(chart);

    int pad_left = lv_obj_get_style_pad_left(chart, LV_PART_MAIN);
    int pad_right = lv_obj_get_style_pad_left(chart, LV_PART_MAIN);
    float x = pad_left;
    float gap_x = (float)(lv_obj_get_width(chart) - pad_left - pad_right) /
                  (ARRAY_SIZE(tick_label_x) - 1);

    for (int i = 0; i < ARRAY_SIZE(tick_label_x); i++)
    {
        tick_label_x[i] = lv_label_create(scr);
        lv_label_set_text_fmt(tick_label_x[i], "%d", i);
        lv_obj_set_style_text_font(tick_label_x[i], ttf_main_s.font,
                                   LV_PART_MAIN);
        lv_obj_refr_size(tick_label_x[i]);
        lv_obj_align_to(tick_label_x[i], chart, LV_ALIGN_OUT_BOTTOM_LEFT,
                        (int)x - lv_obj_get_width(tick_label_x[i]) / 2, 10);
        x += gap_x;
    }

    btn_reset = lv_btn_create(scr);
    lv_obj_set_size(btn_reset, 200, 80);
    lv_obj_align(btn_reset, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_add_event_cb(btn_reset, reset_cb, LV_EVENT_CLICKED, NULL);
    label_reset = lv_label_create(btn_reset);
    lv_label_set_text(label_reset, "Reset");
    lv_obj_set_style_text_font(label_reset, ttf_main.font, LV_PART_MAIN);
    lv_obj_center(label_reset);

    cpu_usage = lv_label_create(scr);
    lv_label_set_text(cpu_usage, "0% CPU");
    lv_obj_set_style_text_font(cpu_usage, ttf_main_s.font, LV_PART_MAIN);
    lv_obj_set_style_text_color(cpu_usage, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(cpu_usage, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(cpu_usage, LV_OPA_70, LV_PART_MAIN);
    lv_obj_align(cpu_usage, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    label_cfg = lv_label_create(scr);
    lv_label_set_text(label_cfg, "Kernel-4.19(CPU0/1/2) + Bare-metal(CPU3)");
    lv_obj_set_style_text_font(label_cfg, ttf_main_s.font, LV_PART_MAIN);
    lv_obj_set_style_text_color(label_cfg, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(label_cfg, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(label_cfg, LV_OPA_70, LV_PART_MAIN);
    lv_obj_align_to(label_cfg, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    chart_y_tick_update(y_range / 100 + 1);

    pthread_create(&socket_tid, NULL, socket_thread, NULL);
    pthread_create(&rpmsg_tid, NULL, rpmsg_thread, NULL);
    timer = lv_timer_create(chart_update, 1000, NULL);

    get_cpu_usage();

    gettimeofday(&tstart, NULL);
}

