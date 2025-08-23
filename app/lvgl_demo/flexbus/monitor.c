#include <lvgl/lvgl.h>

#include "main.h"
#include "soc.h"

/*
 * _td  time domain
 * _fd  frequency domain
 */

static lv_ft_info_t ttf_main;
static lv_ft_info_t ttf_main_s;
static lv_obj_t *scr;
static lv_obj_t *title;
static lv_obj_t *chart_td;
static lv_obj_t *title_x_td;
static lv_obj_t *title_y_td;
static lv_obj_t *chart_fd;
static lv_obj_t *title_x_fd;
static lv_obj_t *title_y_fd;
static lv_chart_series_t *ser_td;
static lv_chart_series_t *ser_fd;
static lv_obj_t *label_sr;
static lv_obj_t *label_pr;
static lv_timer_t *timer;

/* samples per frame */
static lv_coord_t spf = 512;
static lv_coord_t y_range = 300;
static lv_coord_t y_range_fft = 300;
static lv_coord_t data_range = 225; /* -1.125 ~ 1.125 */
static lv_coord_t y_ticks = 7;
static lv_obj_t **tick_label_y = NULL;
static lv_coord_t auto_sr = 0;
static lv_coord_t peak_id = 0;

static lv_coord_t *sample_x_td;
static lv_coord_t *sample_x_fd;
static lv_coord_t *samples;
static lv_coord_t *sample_y_td;
static lv_coord_t *sample_y_fd;

static lv_obj_t *time_label;
static lv_obj_t *hw_version;

static lv_obj_t *cont;
static lv_obj_t *btn_reset;
static lv_obj_t *label_reset;
static lv_obj_t *switch_auto;
static lv_obj_t *label_auto;

static struct timeval tstart;

static char *kernel_version;
static char *compatible_name;
static char *soc_name;

extern void rfft(int32_t *data, int32_t max, int32_t *out, int32_t N);

static void update_sample_rate(int rate)
{
    if (flexbus_get_rate() == rate)
        return;

    printf("sr %d => %d\n", flexbus_get_rate(), rate);
    flexbus_set_rate(rate);

    lv_chart_set_range(chart_fd,
                       LV_CHART_AXIS_PRIMARY_X, 0, (int)(flexbus_get_rate() / 1000.0 / 2.0));
    //printf("%d %d\n", (int32_t)(flexbus_get_rate() / 1000.0 / 2.0), spf);
    for (int i = 0; i < spf; i++)
    {
        sample_x_fd[i] = (int32_t)(flexbus_get_rate() / 1000.0 * i / spf);
        //printf("%d %d\n", i, sample_x_fd[i]);
    }

    if (flexbus_get_rate() > 1000000)
        lv_label_set_text_fmt(label_sr, "Sample Rate = %dMHz",
                              flexbus_get_rate() / 1000000);
    else if (flexbus_get_rate() > 1000)
        lv_label_set_text_fmt(label_sr, "Sample Rate = %dKHz",
                              flexbus_get_rate() / 1000);
}

static void update_sample_per_frame(int new)
{
    if (new > 2048)
        new = 2048;
    if (new < 512)
        new = 512;

    if (spf == new)
        return;

    printf("spf %d => %d\n", spf, new);
    spf = new;

    if (samples)     free(samples);
    if (sample_x_td) free(sample_x_td);
    if (sample_x_fd) free(sample_x_fd);
    if (sample_y_td) free(sample_y_td);
    if (sample_y_fd) free(sample_y_fd);

    samples     = calloc(spf, sizeof(lv_coord_t));
    sample_x_td = calloc(spf, sizeof(lv_coord_t));
    sample_x_fd = calloc(spf, sizeof(lv_coord_t));
    sample_y_td = calloc(spf, sizeof(lv_coord_t));
    sample_y_fd = calloc(spf, sizeof(lv_coord_t));

    for (int i = 0; i < spf; i++)
        sample_x_td[i] = i;
    for (int i = 0; i < spf; i++)
        sample_x_fd[i] = (int)(flexbus_get_rate() / 1000.0 * i / spf);

    lv_chart_set_point_count(chart_td, spf);
    lv_chart_set_point_count(chart_fd, spf / 2);

    lv_chart_set_ext_x_array(chart_td, ser_td, sample_x_td);
    lv_chart_set_ext_y_array(chart_td, ser_td, sample_y_td);
    lv_chart_set_ext_x_array(chart_fd, ser_fd, sample_x_fd);
    lv_chart_set_ext_y_array(chart_fd, ser_fd, sample_y_fd);
}

static void switch_cb(lv_event_t *e)
{
    auto_sr = lv_obj_has_state(lv_event_get_target(e), LV_STATE_CHECKED);
}

static void reset_cb(lv_event_t *e)
{
    update_sample_rate(100000000);
    update_sample_per_frame(512);
}

static void chart_y_tick_update(int new_ticks)
{
    int pad_top = lv_obj_get_style_pad_top(chart_td, LV_PART_MAIN);
    int pad_bottom = lv_obj_get_style_pad_bottom(chart_td, LV_PART_MAIN);
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
    lv_chart_set_div_line_count(chart_td, y_ticks, 4);
    tick_label_y = calloc(y_ticks, sizeof(lv_obj_t *));

    gap_y = (lv_obj_get_height(chart_td) - pad_top - pad_bottom) / (y_ticks - 1);

    for (int i = 0; i < y_ticks; i++)
    {
        tick_label_y[i] = lv_label_create(scr);
        lv_label_set_text_fmt(tick_label_y[i], "%.1f", i * 0.5 + (-1.5));
        lv_obj_set_style_text_font(tick_label_y[i], ttf_main_s.font,
                                   LV_PART_MAIN);
        lv_obj_refr_size(tick_label_y[i]);
        lv_obj_align_to(tick_label_y[i], chart_td, LV_ALIGN_OUT_LEFT_BOTTOM,
                        -10, -(y - lv_obj_get_height(tick_label_y[i]) / 2));
        y += gap_y;
    }
}

static void event_cb(lv_event_t *e)
{
    lv_point_t p;
    lv_chart_get_point_pos_by_id(chart_fd, ser_fd, peak_id, &p);
    lv_coord_t value = sample_x_fd[peak_id];

    if (!peak_id)
    {
        lv_obj_add_flag(label_pr, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    if (value > 1000)
        lv_label_set_text_fmt(label_pr, "%.1fMHz", value / 1000.0);
    else
        lv_label_set_text_fmt(label_pr, "%dKHz", value);

    lv_area_t a;
    a.x1 = chart_fd->coords.x1 + p.x + 2;
    a.x2 = chart_fd->coords.x1 + p.x + 40;
    a.y1 = chart_fd->coords.y1 + p.y + 2;
    a.y2 = chart_fd->coords.y1 + p.y + 40;

    lv_obj_clear_flag(label_pr, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_pos(label_pr, chart_fd->coords.x1 + p.x + 2,
                   chart_fd->coords.y1 + p.y + 2);
}

static void chart_range_update(void)
{
    int max = 30;//y_range_fft;
    int max_freq = 0;
    int peak_freq;

    peak_id = 0;
    for (int i = 1; i < spf / 2; i++)
    {
        //printf("%d %d\n", sample_x_fd[i], sample_y_fd[i]);
        if (sample_y_fd[i] > max)
        {
            max = sample_y_fd[i];
            peak_freq = sample_x_fd[i];
            peak_id = i;
        }
        if (sample_y_fd[i] > 5)
            max_freq = i;
    }

    if (max != y_range_fft)
    {
        y_range_fft = max;
        lv_chart_set_range(chart_fd,
                           LV_CHART_AXIS_PRIMARY_Y, 0, y_range_fft);
    }

    printf("Peak freq = %d kHz\n", peak_freq);
    printf("Max  freq = %d kHz(%d)\n", sample_x_fd[max_freq], max_freq);

    if (auto_sr && /*(max_freq > 0) &&*/
            ((sample_x_fd[max_freq] * 1000) < (flexbus_get_rate() / 100)))
    {
        if (flexbus_get_rate() >= 10000000)
        {
            update_sample_rate(flexbus_get_rate() / 10);
        }
        else if (spf < 2048)
        {
            update_sample_per_frame(spf * 2);
        }
    }
//    if (max_freq == 0)
//    {
//        update_sample_rate(100000000);
//        update_sample_per_frame(512);
//    }
}

static void chart_update(lv_timer_t *e)
{
    struct timeval tv;
    time_t dur;
    int32_t min, max, avg, cur;

    gettimeofday(&tv, NULL);
    dur = tv.tv_sec - tstart.tv_sec;
    lv_label_set_text_fmt(time_label, "%02u:%02u:%02u",
                          dur / 60 / 60,
                          dur / 60 % 60,
                          dur % 60);

    if (flexbus_read(sample_x_td, samples, spf))
    {
        for (int i = 0; i < spf; i++)
            sample_y_td[i] = samples[i] - (data_range / 2) + (y_range / 2);

        rfft(sample_y_td, 225, sample_y_fd, spf);
        chart_range_update();
        lv_obj_invalidate(chart_td);
        lv_obj_invalidate(chart_fd);
    }
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
    lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, 0, 0);

    title = lv_label_create(scr);
    lv_label_set_text_fmt(title, "flexbus adda with kernel-%s", kernel_version);
    lv_obj_set_style_text_font(title, ttf_main.font, LV_PART_MAIN);

    title_x_td = lv_label_create(scr);
    lv_label_set_text(title_x_td, "time");
    lv_obj_set_style_text_font(title_x_td, ttf_main_s.font, LV_PART_MAIN);

    title_y_td = lv_label_create(scr);
    lv_label_set_text(title_y_td, "voltage");
    lv_obj_set_style_text_font(title_y_td, ttf_main_s.font, LV_PART_MAIN);

    chart_td = lv_chart_create(scr);
    lv_obj_set_size(chart_td, lv_pct(80), lv_pct(38));
    lv_chart_set_div_line_count(chart_td, y_ticks, 4);
    lv_chart_set_type(chart_td, LV_CHART_TYPE_SCATTER);
    lv_chart_set_range(chart_td,
                       LV_CHART_AXIS_PRIMARY_X, 0, 512);
    lv_chart_set_range(chart_td,
                       LV_CHART_AXIS_PRIMARY_Y, 0, y_range);
    lv_chart_set_axis_tick(chart_td,
                           LV_CHART_AXIS_PRIMARY_X, 10, 5, 64, 1, false, 0);
    lv_chart_set_axis_tick(chart_td,
                           LV_CHART_AXIS_PRIMARY_Y, 10, 5, 3, 2, false, 100);

    title_x_fd = lv_label_create(scr);
    lv_label_set_text(title_x_fd, "freq (kHz)");
    lv_obj_set_style_text_font(title_x_fd, ttf_main_s.font, LV_PART_MAIN);

    title_y_fd = lv_label_create(scr);
    lv_label_set_text(title_y_fd, "y");
    lv_obj_set_style_text_font(title_y_fd, ttf_main_s.font, LV_PART_MAIN);

    chart_fd = lv_chart_create(scr);
    lv_obj_set_size(chart_fd, lv_pct(80), lv_pct(38));
    lv_chart_set_div_line_count(chart_fd, y_ticks, 4);
    lv_chart_set_type(chart_fd, LV_CHART_TYPE_SCATTER);
    lv_chart_set_range(chart_fd,
                       LV_CHART_AXIS_PRIMARY_X, 0, flexbus_get_rate() / 1000 / 2);
    lv_chart_set_range(chart_fd,
                       LV_CHART_AXIS_PRIMARY_Y, 0, y_range_fft);
    lv_chart_set_axis_tick(chart_fd,
                           LV_CHART_AXIS_PRIMARY_X, 10, 5, 11, 5, true, 50);
    lv_chart_set_axis_tick(chart_fd,
                           LV_CHART_AXIS_PRIMARY_Y, 10, 5, 3, 2, true, 100);
    lv_obj_add_event_cb(chart_fd, event_cb, LV_EVENT_DRAW_POST_END, NULL);

    sample_x_td = calloc(spf, sizeof(lv_coord_t));
    sample_x_fd = calloc(spf, sizeof(lv_coord_t));
    samples = calloc(spf, sizeof(lv_coord_t));
    sample_y_td = calloc(spf, sizeof(lv_coord_t));
    sample_y_fd = calloc(spf, sizeof(lv_coord_t));

    for (int i = 0; i < spf; i++)
        sample_x_td[i] = i;
    for (int i = 0; i < spf; i++)
        sample_x_fd[i] = flexbus_get_rate() / 1000 / spf * i;

    lv_chart_set_point_count(chart_td, spf);
    ser_td = lv_chart_add_series(chart_td,
                                 lv_palette_lighten(LV_PALETTE_PINK, 1), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_x_array(chart_td, ser_td, sample_x_td);
    lv_chart_set_ext_y_array(chart_td, ser_td, sample_y_td);

    lv_chart_set_point_count(chart_fd, spf / 2);
    ser_fd = lv_chart_add_series(chart_fd,
                                 lv_palette_lighten(LV_PALETTE_BLUE, 1), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_x_array(chart_fd, ser_fd, sample_x_fd);
    lv_chart_set_ext_y_array(chart_fd, ser_fd, sample_y_fd);

    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_align(chart_td, LV_ALIGN_CENTER, 0, lv_pct(-22));
    lv_obj_align_to(title_x_td, chart_td, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
    lv_obj_align_to(title_y_td, chart_td, LV_ALIGN_OUT_LEFT_TOP, -80, 0);
    lv_obj_align(chart_fd, LV_ALIGN_CENTER, 0, lv_pct(22));
    lv_obj_align_to(title_x_fd, chart_fd, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 50);
    lv_obj_align_to(title_y_fd, chart_fd, LV_ALIGN_OUT_LEFT_TOP, -80, 0);

    lv_obj_refr_size(chart_td);
    lv_obj_refr_size(chart_fd);

    cont = lv_obj_create(scr);
    lv_obj_set_size(cont, 160, LV_SIZE_CONTENT);
    lv_obj_align_to(cont, chart_td, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);

    label_auto = lv_label_create(cont);
    lv_label_set_text(label_auto, "Auto");
    lv_obj_align(label_auto, LV_ALIGN_TOP_MID, 0, 0);

    switch_auto = lv_switch_create(cont);
    lv_obj_clear_state(switch_auto, LV_STATE_CHECKED);
    lv_obj_set_size(switch_auto, lv_pct(100), 80);
    lv_obj_add_event_cb(switch_auto, switch_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align_to(switch_auto, label_auto, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    btn_reset = lv_btn_create(cont);
    lv_obj_set_size(btn_reset, lv_pct(100), 80);
    lv_obj_add_event_cb(btn_reset, reset_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align_to(btn_reset, switch_auto, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    label_reset = lv_label_create(btn_reset);
    lv_label_set_text(label_reset, "Reset");
    lv_obj_center(label_reset);

    label_sr = lv_label_create(scr);
    lv_label_set_text(label_sr, "Sample Rate = 100MHz");
    lv_obj_align(label_sr, LV_ALIGN_TOP_LEFT, 0, 0);

    label_pr = lv_label_create(scr);
    lv_label_set_text(label_pr, "0KHz");
    lv_obj_add_flag(label_pr, LV_OBJ_FLAG_HIDDEN);

    chart_y_tick_update(y_ticks);

    timer = lv_timer_create(chart_update, 1000, NULL);

    gettimeofday(&tstart, NULL);
}

