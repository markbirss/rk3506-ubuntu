/*
 * Copyright (C) 2023 Rockchip Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _UPDATE_H
#define _UPDATE_H
#include <stdbool.h>

typedef int (*update_func)(char *src_path, void* pupdate_cmd);
typedef struct {
    char name[32];
    bool need_update;
    bool is_ab;
    long long size;
    long long offset;
    long long flash_offset;
    char dest_path[100];
    bool skip_verify;
    update_func cmd;
} UPDATE_CMD, *PUPDATE_CMD;

typedef enum {
    RK_UPGRADE_FINISHED,
    RK_UPGRADE_START,
    RK_UPGRADE_ERR,
} RK_Upgrade_Status_t;

typedef void(*RK_upgrade_callback)(void *user_data, RK_Upgrade_Status_t status);
typedef void (*RK_print_callback)(char *pszPrompt);

void RK_ota_set_url(char *url, char *savepath);
bool RK_ota_set_partition(int partition);
void RK_ota_start(RK_upgrade_callback cb, RK_print_callback print_cb);
//void RK_ota_stop();
int RK_ota_get_progress();
void RK_ota_get_sw_version(char *buffer, int maxLength);
bool RK_ota_check_version(char *url);

#endif
