/**
 * @file lv_port_indev_templ.c
 *
 */

/* Copy this file as "lv_port_indev.c" and set this value to "1" to enable conten */

#include <stdlib.h>
#include <lvgl.h>
#include <lvgl/lv_conf.h>
#include <lv_drivers/sdl/sdl_gpu.h>

#include "lv_port_indev.h"
#include "key.h"

typedef struct _GROUP_NODE
{
    struct _GROUP_NODE *next;
    lv_group_t *group;
} GROUP_NODE;

static int rot_indev;
static lv_indev_t *indev_touchpad;
static lv_indev_t *indev_key;
static lv_indev_t *indev_sdl;

GROUP_NODE *group_list = NULL;

#ifdef USE_SENSOR
static lv_indev_drv_t lsensor_drv;
static lv_indev_drv_t psensor_drv;

lv_indev_drv_t *lv_port_indev_get_lsensor_drv(void)
{
    return &lsensor_drv;
}

lv_indev_drv_t *lv_port_indev_get_psensor_drv(void)
{
    return &psensor_drv;
}
#endif

lv_group_t *lv_port_indev_group_create(void)
{
    struct _GROUP_NODE *group_node = NULL;
    lv_group_t *group = lv_group_create();

    lv_indev_set_group(indev_key, group);
    lv_group_set_default(group);

    group_node = (struct _GROUP_NODE *)malloc(sizeof(struct _GROUP_NODE));
    group_node->group = group;
    group_node->next = NULL;
    if (group_list)
    {
        group_node->next = group_list;
        group_list = group_node;
    }
    else
    {
        group_list = group_node;
    }

    return group;
}

void lv_port_indev_group_destroy(lv_group_t *group)
{
    if (group_list)
    {
        struct _GROUP_NODE *group_node = NULL;
        group_node = group_list;
        if (group_list->group == group)
        {
            group_list = group_list->next;
            if (group_list)
            {
                lv_indev_set_group(indev_key, group_list->group);
                lv_group_set_default(group_list->group);
            }
            else
            {
                lv_indev_set_group(indev_key, NULL);
                lv_group_set_default(NULL);
            }
            free(group_node);
        }
        else
        {
            while (group_node->next)
            {
                struct _GROUP_NODE *group_node_next = group_node->next;
                if (group_node_next->group == group)
                {
                    group_node->next = group_node_next->next;
                    free(group_node_next);
                    break;
                }
                group_node = group_node->next;
            }
        }
        lv_group_del(group);
    }
}

void lv_port_indev_init(int rot)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    static lv_indev_drv_t indev_drv;
    static lv_indev_drv_t key_drv;
    static lv_indev_drv_t sdl_drv;
    lv_disp_t *disp;

    rot_indev = rot;

    /*------------------
     * Touchpad
     * -----------------*/
#if USE_EVDEV != 0 || USE_BSD_EVDEV
    /*Initialize your touchpad if you have*/
    disp = lv_disp_get_default();
    if (evdev_init(disp->driver, rot) == 0)
    {
        /*Register a touchpad input device*/
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.read_cb = evdev_read;
        indev_touchpad = lv_indev_drv_register(&indev_drv);
    }
#endif
#if USE_KEY
    if (key_init() == 0)
    {
        lv_indev_drv_init(&key_drv);
        key_drv.type = LV_INDEV_TYPE_KEYPAD;
        key_drv.read_cb = key_read;
        indev_key = lv_indev_drv_register(&key_drv);
        lv_port_indev_group_create();
    }
#endif

#if USE_SDL_GPU
    lv_indev_drv_init(&sdl_drv);
    sdl_drv.type = LV_INDEV_TYPE_POINTER;
    sdl_drv.read_cb = sdl_mouse_read;
    indev_sdl = lv_indev_drv_register(&sdl_drv);
#endif

#if USE_SENSOR
    lv_indev_drv_init(&lsensor_drv);
    if (evdev_init_lsensor() >= 0)
    {
        lsensor_drv.type = LV_INDEV_TYPE_NONE;
        lsensor_drv.user_data = evdev_get_lsensor();
        lsensor_drv.read_cb = evdev_sensor_read;
        /* DO NOT register the drv to lvgl, just handled by applications */
    }

    lv_indev_drv_init(&psensor_drv);
    if (evdev_init_psensor() >= 0)
    {
        psensor_drv.type = LV_INDEV_TYPE_NONE;
        psensor_drv.user_data = evdev_get_psensor();
        psensor_drv.read_cb = evdev_sensor_read;
        /* DO NOT register the drv to lvgl, just handled by applications */
    }
#endif
}

