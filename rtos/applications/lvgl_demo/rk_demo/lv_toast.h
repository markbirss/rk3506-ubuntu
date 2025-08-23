/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/

/**
 * @file lv_toast.h
 *
 */

#ifndef LV_TOAST_H
#define LV_TOAST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
    lv_label_t obj;
    lv_coord_t origin_y;
    lv_coord_t start_offset;
    lv_opa_t opa;
    uint32_t duration;
    uint32_t fade_time;
    uint8_t auto_delete;
} lv_toast_t;

extern const lv_obj_class_t lv_toast_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#define lv_toast_set_text       lv_label_set_text
#define lv_toast_set_text_fmt   lv_label_set_text_fmt

lv_obj_t *lv_toast_create(lv_obj_t *parent);

void lv_toast_show(lv_obj_t *obj);

#define LV_TOAST_SET_FUNC(type, name)   \
    static inline void lv_toast_set_##name(lv_obj_t * obj, type name) \
    {   \
        lv_toast_t * toast = (lv_toast_t *)obj; \
        toast->name = name; \
    }

#define LV_TOAST_GET_FUNC(type, name)   \
    static inline type lv_toast_get_##name(lv_obj_t * obj)    \
    {   \
        lv_toast_t * toast = (lv_toast_t *)obj; \
        return toast->name; \
    }

LV_TOAST_SET_FUNC(lv_coord_t, start_offset)
LV_TOAST_SET_FUNC(lv_opa_t, opa)
LV_TOAST_SET_FUNC(uint32_t, duration)
LV_TOAST_SET_FUNC(uint32_t, fade_time)
LV_TOAST_SET_FUNC(uint8_t, auto_delete)

LV_TOAST_GET_FUNC(lv_coord_t, start_offset)
LV_TOAST_GET_FUNC(lv_opa_t, opa)
LV_TOAST_GET_FUNC(uint32_t, duration)
LV_TOAST_GET_FUNC(uint32_t, fade_time)
LV_TOAST_GET_FUNC(uint8_t, auto_delete)

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TOAST_H*/
