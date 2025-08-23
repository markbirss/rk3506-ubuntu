/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/

#include "lv_app_main.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_fs.h"
#if defined(RT_LV_DEMO_MUSIC) || defined(RT_LV_DEMO_BENCHMARK)
#include "lv_demos.h"
#endif

#define NUMBER(x)       NUMBER_(x)
#define NUMBER_(x)       #x
#define MAJOR           NUMBER(LVGL_VERSION_MAJOR)
#define MINOR           NUMBER(LVGL_VERSION_MINOR)
#define PATCH           NUMBER(LVGL_VERSION_PATCH)

static void lvgl_task(void *arg)
{
    uint32_t event;
    rt_err_t ret = RT_EOK;

    rt_size_t total, used, maxm;
    rt_memory_info(&total, &used, &maxm);
    printf("sram total=%d, used=%d, maxm=%d\n", total, used, maxm);

    lv_init();

#ifdef RT_USING_RKADK
    lv_port_disp_init(LV_DISP_ROT_90);
    lv_port_indev_init(LV_DISP_ROT_90);
#else
    lv_port_disp_init(LV_DISP_ROT_NONE);
    lv_port_indev_init(LV_DISP_ROT_NONE);
#endif

#if LV_USE_FILESYSTEM
    lv_port_fs_init();
#endif

    printf("\n Welcome to the LVGL v" MAJOR "." MINOR "." PATCH "\n");

#ifdef RT_LV_DEMO_MUSIC
    lv_demo_music();
#elif defined(RT_LV_DEMO_BENCHMARK)
    lv_demo_benchmark();
#elif defined(RT_LV_RK_DEMO)
    lv_rk_demo();
#endif

    while (1)
    {
        lv_task_handler();
        rt_thread_mdelay(2);
    }
}

int lvgl_app_init(void)
{
    rt_thread_t lvgl;

    lvgl = rt_thread_create("lvgl", lvgl_task, RT_NULL, 64*1024, 4, 20);
    rt_thread_startup(lvgl);

    return RT_EOK;
}

INIT_APP_EXPORT(lvgl_app_init);
