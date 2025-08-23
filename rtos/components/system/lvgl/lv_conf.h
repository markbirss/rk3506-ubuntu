/**
 * @file lv_conf.h
 * Configuration file for v8.3.3
 */

/*
 * Copy this file as `lv_conf.h`
 * 1. simply next to the `lvgl` folder
 * 2. or any other places and
 *    - define `LV_CONF_INCLUDE_SIMPLE`
 *    - add the path as include path
 */

/* clang-format off */
#if 1 /*Set it to "1" to enable content*/

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>
#include "rtconfig.h"
//#include "drv_panel.h"
//#include "drv_panel_cfg.h"
//
//#define LV_HOR_RES              RT_HW_LCD_XRES
//#define LV_VER_RES              RT_HW_LCD_YRES

#define LV_COLOR_CHROMA_KEY     lv_color_hex(LV_COLOR_CHROMA_KEY_HEX)

#define LV_MEM_CUSTOM           1
#define LV_MEM_CUSTOM_INCLUDE   <rtthread.h>
#define LV_MEM_BUF_MAX_NUM      16
#define LV_MEM_CUSTOM_ALLOC     rt_malloc
#define LV_MEM_CUSTOM_FREE      rt_free
#define LV_MEM_CUSTOM_REALLOC   rt_realloc

#define LV_MEMCPY_MEMSET_STD    1

#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
#define LV_TICK_CUSTOM_INCLUDE "rtthread.h"         /*Header for the system time function*/
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (rt_tick_get())    /*Expression evaluating to current system time in ms*/
#endif   /*LV_TICK_CUSTOM*/

#define LV_ASSERT_HANDLER_INCLUDE <rtthread.h>
#define LV_ASSERT_HANDLER RT_ASSERT(0);

/*Change the built in (v)snprintf functions*/
#define LV_SPRINTF_CUSTOM 1
#if LV_SPRINTF_CUSTOM
#define LV_SPRINTF_INCLUDE <stdio.h>
#define lv_snprintf  snprintf
#define lv_vsnprintf vsnprintf
#else   /*LV_SPRINTF_CUSTOM*/
#define LV_SPRINTF_USE_FLOAT 0
#endif  /*LV_SPRINTF_CUSTOM*/

/*Optionally declare custom fonts here.
 *You can use these fonts as default font too and they will be available globally.
 *E.g. #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)*/
#define LV_FONT_CUSTOM_DECLARE

/*Always set a default font*/
#if LV_FONT_DEFAULT_MONTSERRAT_8
#define LV_FONT_DEFAULT &lv_font_montserrat_8
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_12
#define LV_FONT_DEFAULT &lv_font_montserrat_12
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_14
#define LV_FONT_DEFAULT &lv_font_montserrat_14
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_16
#define LV_FONT_DEFAULT &lv_font_montserrat_16
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_18
#define LV_FONT_DEFAULT &lv_font_montserrat_18
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_20
#define LV_FONT_DEFAULT &lv_font_montserrat_20
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_22
#define LV_FONT_DEFAULT &lv_font_montserrat_22
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_24
#define LV_FONT_DEFAULT &lv_font_montserrat_24
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_26
#define LV_FONT_DEFAULT &lv_font_montserrat_26
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_28
#define LV_FONT_DEFAULT &lv_font_montserrat_28
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_30
#define LV_FONT_DEFAULT &lv_font_montserrat_30
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_32
#define LV_FONT_DEFAULT &lv_font_montserrat_32
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_34
#define LV_FONT_DEFAULT &lv_font_montserrat_34
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_36
#define LV_FONT_DEFAULT &lv_font_montserrat_36
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_38
#define LV_FONT_DEFAULT &lv_font_montserrat_38
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_40
#define LV_FONT_DEFAULT &lv_font_montserrat_40
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_42
#define LV_FONT_DEFAULT &lv_font_montserrat_42
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_44
#define LV_FONT_DEFAULT &lv_font_montserrat_44
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_46
#define LV_FONT_DEFAULT &lv_font_montserrat_46
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_48
#define LV_FONT_DEFAULT &lv_font_montserrat_48
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_12_SUBPX
#define LV_FONT_DEFAULT &lv_font_montserrat_12_subpx
#endif
#if LV_FONT_DEFAULT_MONTSERRAT_28_COMPRESSED
#define LV_FONT_DEFAULT &lv_font_montserrat_28_compressed
#endif
#if LV_FONT_DEFAULT_DEJAVU_16_PERSIAN_HEBREW
#define LV_FONT_DEFAULT &lv_font_dejavu_16_persian_hebrew
#endif
#if LV_FONT_DEFAULT_SIMSUN_16_CJK
#define LV_FONT_DEFAULT &lv_font_simsun_16_cjk
#endif
#if LV_FONT_DEFAULT_UNSCII_8
#define LV_FONT_DEFAULT &lv_font_unscii_8
#endif
#if LV_FONT_DEFAULT_UNSCII_16
#define LV_FONT_DEFAULT &lv_font_unscii_16
#endif

/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/

#endif /*End of "Content enable"*/
