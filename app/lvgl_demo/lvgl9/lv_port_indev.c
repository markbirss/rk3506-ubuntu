/**
 * @file lv_port_indev.c
 *
 */

#include <stdlib.h>
#include <lvgl.h>
#include <lvgl/src/drivers/sdl/lv_sdl_mouse.h>

#include "evdev.h"

static lv_indev_t *indev_touchpad = NULL;
static lv_indev_t *indev_sdl = NULL;

#if defined(USE_SENSOR) && USE_SENSOR
static lv_indev_t *lsensor = NULL;
static lv_indev_t *psensor = NULL;

lv_indev_t *lv_port_indev_get_lsensor(void)
{
    return lsensor;
}

lv_indev_t *lv_port_indev_get_psensor(void)
{
    return psensor;
}
#endif

void lv_port_indev_init(int rot)
{
    lv_disp_t *disp;

#if defined(USE_EVDEV) && USE_EVDEV
    disp = lv_display_get_default();
    if (evdev_init(disp, rot) == 0)
    {
        indev_touchpad = lv_indev_create();
        lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev_touchpad, evdev_read);
    }
#endif

#if defined(LV_USE_SDL) && LV_USE_SDL
    indev_sdl = lv_sdl_mouse_create();
#endif

#if defined(USE_SENSOR) && USE_SENSOR
    if (evdev_init_lsensor() >= 0)
    {
        lsensor = lv_indev_create();
        lv_indev_set_type(lsensor, LV_INDEV_TYPE_NONE);
        lv_indev_set_read_cb(lsensor, evdev_sensor_read);
        lv_indev_set_user_data(lsensor, evdev_get_lsensor());
        lv_indev_enable(lsensor, 0);
        /* DO NOT enable the drv, just handled by applications */
    }

    if (evdev_init_psensor() >= 0)
    {
        psensor = lv_indev_create();
        lv_indev_set_type(psensor, LV_INDEV_TYPE_NONE);
        lv_indev_set_read_cb(psensor, evdev_sensor_read);
        lv_indev_set_user_data(psensor, evdev_get_psensor());
        lv_indev_enable(psensor, 0);
        /* DO NOT enable the drv, just handled by applications */
    }
#endif
}

