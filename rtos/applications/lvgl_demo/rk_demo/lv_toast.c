/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/

/**
 * @file lv_toast.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_toast.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_toast_class

/* Fade in and out duration, unit ms */
#define FADE_TIME   150
/* Max offset before fade in, unit pixel */
#define MAX_OFFSET  20

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_toast_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_toast_class =
{
    .constructor_cb = lv_toast_constructor,
    .base_class = &lv_label_class,
    .instance_size = sizeof(lv_toast_t),
};
static lv_style_t base_style = {.prop_cnt = 0,};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *lv_toast_create(lv_obj_t *parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

static int remap(int from_min, int from_max, int to_min, int to_max, int val)
{
    float ratio;

    ratio = (float)(val - from_min) / (from_max - from_min);

    return (int)(ratio * (to_max - to_min)) + to_min;
}

static void toast_style_init(void)
{
    if (base_style.prop_cnt != 0)
        return;

    lv_style_init(&base_style);
    lv_style_set_bg_opa(&base_style, 0);
    lv_style_set_bg_color(&base_style, lv_palette_lighten(LV_PALETTE_GREY, 4));
    lv_style_set_text_opa(&base_style, 0);
    lv_style_set_text_color(&base_style, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_pad_all(&base_style, 10);
    lv_style_set_radius(&base_style, 10);
}

static void toast_fade_in_out(void *obj, int32_t x)
{
    lv_toast_t *toast = (lv_toast_t *)obj;
    uint32_t fade_time = toast->fade_time;
    lv_opa_t value = toast->opa;
    lv_coord_t ofs_y = 0;

    if (x < fade_time)
    {
        value = remap(0, fade_time, 0, toast->opa, x);
        ofs_y = remap(0, fade_time, toast->start_offset, 0, x);
    }
    else if (x > toast->duration + fade_time)
    {
        x -= toast->duration + fade_time;
        value = remap(0, fade_time, toast->opa, 0, x);
    }

    lv_obj_set_y(obj, toast->origin_y + ofs_y);
    lv_obj_set_style_bg_opa(obj, value, LV_PART_MAIN);
    lv_obj_set_style_text_opa(obj, value, LV_PART_MAIN);
}

static void toast_deleted_cb(lv_anim_t *a)
{
    lv_obj_t *obj = (lv_obj_t *)a->var;
    lv_toast_t *toast = (lv_toast_t *)obj;

    if (!toast->auto_delete)
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_del(obj);
}

void lv_toast_show(lv_obj_t *obj)
{
    lv_toast_t *toast = (lv_toast_t *)obj;

    if (!lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN))
    {
        /* Animation already start, restart it */
        lv_anim_del(obj, toast_fade_in_out);
    }

    lv_obj_refr_size(obj);
    lv_obj_refr_pos(obj);

    toast->origin_y = lv_obj_get_y(obj);
    lv_obj_set_y(obj, toast->origin_y + toast->start_offset);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, toast_fade_in_out);
    lv_anim_set_time(&a, toast->duration + toast->fade_time * 2);
    lv_anim_set_values(&a, 0, toast->duration + toast->fade_time * 2);
    lv_anim_set_deleted_cb(&a, toast_deleted_cb);
    lv_anim_start(&a);

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

static void lv_toast_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_toast_t *toast = (lv_toast_t *)obj;

    toast_style_init();

    toast->start_offset = MAX_OFFSET;
    toast->opa = LV_OPA_COVER;
    toast->duration = 3000;
    toast->fade_time = FADE_TIME;
    toast->auto_delete = true;

    lv_label_set_text(obj, "Toast");
    lv_obj_add_style(obj, &base_style, LV_PART_MAIN);

    LV_TRACE_OBJ_CREATE("finished");
}

/* Example */
/*
void toast_simple(const char *text)
{
    lv_obj_t *toast = lv_toast_create(lv_layer_sys());
    lv_toast_set_text(toast, text);
    lv_obj_align(toast, LV_ALIGN_TOP_MID, 0, 20);
    lv_toast_show(toast);
}
*/

