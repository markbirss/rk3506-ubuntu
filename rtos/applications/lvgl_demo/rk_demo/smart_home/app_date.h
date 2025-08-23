/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#ifndef __APP_DATE_H__
#define __APP_DATE_H__

void app_date_init(lv_obj_t *parent, void *userdata);

#define APP_DATE {  \
    .w    = 1,  \
    .h    = 1,  \
    .init = app_date_init,  \
}

#endif

