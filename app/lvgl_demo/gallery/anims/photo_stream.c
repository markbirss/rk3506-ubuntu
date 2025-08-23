#include "gallery.h"

static int32_t img_h = 480;
static int32_t gap_h = 20;
static int32_t max_y;
static int32_t boundary;
static lv_timer_t *timer;
static int32_t timer_ofs = 0;
static int32_t touch_ofs_s = 0;
static int32_t touch_ofs = 0;

static void update_y(void)
{
    int32_t y;
    int32_t h;

    for (int i = 0; i < 6; i++)
    {
        h = lv_obj_get_height(photos[i]);
        y = (img_h + gap_h) * i + timer_ofs + touch_ofs;
        while (y + h > max_y)
            y = -h + (y + h - boundary);
        lv_obj_set_y(photos[i], y);
    }
}

static void lv_timer_cb(lv_timer_t *timer)
{
    timer_ofs++;
    if (timer_ofs > boundary)
        timer_ofs -= boundary;

    update_y();
}

static void touch_handler(lv_event_t * e)
{
    static lv_point_t p0;
    lv_point_t p1;
    lv_event_code_t code = lv_event_get_code(e);

    switch(code)
    {
    case LV_EVENT_PRESSED:
        lv_indev_get_point(lv_indev_get_act(), &p0);
        lv_timer_pause(timer);
        break;
    case LV_EVENT_PRESSING:
        lv_indev_get_point(lv_indev_get_act(), &p1);
        touch_ofs = touch_ofs_s + (p1.y - p0.y);
        if (touch_ofs < 0)
            touch_ofs += boundary;
        touch_ofs %= boundary;
        update_y();
        break;
    case LV_EVENT_RELEASED:
        lv_timer_resume(timer);
        if (touch_ofs < 0)
            touch_ofs += boundary;
        touch_ofs %= boundary;
        touch_ofs_s = touch_ofs;
        break;
    }
}

void anim_photo_stream_stop(void)
{
    if (timer)
        lv_timer_pause(timer);
    lv_obj_remove_event_cb(photo_box, touch_handler);
}

void anim_photo_stream_start(lv_anim_t *a)
{
    common_anim_start();
    lv_obj_clear_flag(anim_area, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(photo_box, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(photo_box, touch_handler,
        LV_EVENT_ALL, NULL);
    if (timer)
    {
        lv_timer_resume(timer);
        return;
    }
    timer = lv_timer_create(lv_timer_cb, 10, NULL);
    /* 6 photos with 5 gaps */
    max_y = img_h * 6 + gap_h * 5;
    boundary = (img_h + gap_h) * 6;
    printf("max_y %d\n", max_y);
    printf("boundary %d\n", boundary);
}

void anim_photo_stream(void *var, int32_t v)
{
}

void anim_photo_stream_end(lv_anim_t *a)
{
    animing = 0;
}

