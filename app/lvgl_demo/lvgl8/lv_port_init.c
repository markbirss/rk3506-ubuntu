/**
 * @file lv_port_init.c
 *
 */

#include <lvgl/lvgl.h>
#include <lvgl/lv_conf.h>

#include "lv_port_disp.h"
#include "lv_port_indev.h"

/* 0, 90, 180, 270 */
static int g_indev_rotation = 0;
/* 0, 90, 180, 270 */
static int g_disp_rotation = 0;

void lv_port_init(void)
{
    lv_init();

    lv_port_disp_init(0, 0, g_disp_rotation);
    lv_port_indev_init(g_indev_rotation);
}

