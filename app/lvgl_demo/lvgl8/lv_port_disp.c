/**
 * @file lv_port_disp.c
 *
 */

#include <stdlib.h>
#include <lvgl/lvgl.h>
#include <lv_drivers/display/drm.h>
#include <lv_drivers/rkadk/rkadk.h>
#include <lv_drivers/sdl/sdl_gpu.h>

void lv_port_disp_init(lv_coord_t hor_res, lv_coord_t ver_res, int rot)
{
    lv_disp_rot_t lvgl_rot = LV_DISP_ROT_NONE;
    lv_disp_t *disp;

    switch (rot)
    {
    case 0:
        lvgl_rot = LV_DISP_ROT_NONE;
        break;
    case 90:
        lvgl_rot = LV_DISP_ROT_90;
        break;
    case 180:
        lvgl_rot = LV_DISP_ROT_180;
        break;
    case 270:
        lvgl_rot = LV_DISP_ROT_270;
        break;
    default:
        LV_LOG_ERROR("Unsupported rotation %d", rot);
        break;
    }

#if USE_DRM
    drm_disp_drv_init(rot);
#endif

#if USE_SDL_GPU
    static lv_disp_drv_t disp_drv;
    monitor_init();
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN);
    disp_drv.rotated = lvgl_rot;
    sdl_disp_drv_init(&disp_drv, hor_res, ver_res);

    disp = lv_disp_drv_register(&disp_drv);
#endif

#if USE_RKADK
    rkadk_disp_drv_init(lvgl_rot);
#endif
}

