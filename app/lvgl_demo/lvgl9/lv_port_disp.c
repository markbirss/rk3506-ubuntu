/**
 * @file lv_port_disp.c
 *
 */

#include <stdlib.h>
#include <lvgl/lvgl.h>
#include <lvgl/src/drivers/display/drm/lv_linux_drm.h>
#include <lvgl/src/drivers/display/rkadk/rkadk.h>
#include <lvgl/src/drivers/sdl/lv_sdl_window.h>

void lv_port_disp_init(lv_coord_t hor_res, lv_coord_t ver_res, int rot)
{
    lv_display_rotation_t lvgl_rot = LV_DISPLAY_ROTATION_0;
    lv_display_t *disp;

    switch (rot)
    {
    case 0:
        lvgl_rot = LV_DISPLAY_ROTATION_0;
        break;
    case 90:
        lvgl_rot = LV_DISPLAY_ROTATION_90;
        break;
    case 180:
        lvgl_rot = LV_DISPLAY_ROTATION_180;
        break;
    case 270:
        lvgl_rot = LV_DISPLAY_ROTATION_270;
        break;
    default:
        LV_LOG_ERROR("Unsupported rotation %d", rot);
        break;
    }

#if defined(LV_USE_LINUX_DRM) && LV_USE_LINUX_DRM
    LV_LOG_USER("LV_USE_LINUX_DRM");
    disp = lv_drm_disp_create(rot);
#endif

#if defined(LV_USE_SDL) && LV_USE_SDL
    LV_LOG_USER("LV_USE_SDL");
    disp = lv_sdl_window_create(hor_res, ver_res);
#endif

#if defined(LV_USE_RKADK) && LV_USE_RKADK
    LV_LOG_USER("LV_USE_RKADK");
    disp = lv_rkadk_disp_create(lvgl_rot);
#endif
}

