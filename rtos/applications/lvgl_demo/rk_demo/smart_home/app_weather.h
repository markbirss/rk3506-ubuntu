/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#ifndef __APP_WEATHER_H__
#define __APP_WEATHER_H__

void app_weather_init(lv_obj_t *parent, void *userdata);

#define APP_WEATHER {  \
    .w    = 1,  \
    .h    = 1,  \
    .init = app_weather_init,  \
}

#endif

