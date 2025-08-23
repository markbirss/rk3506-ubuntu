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

#ifndef _RKTOOLS_H
#define _RKTOOLS_H
#include "common.h"

#define PATH_LEN 50
#define usb_path "/mnt/udisk/"
#define sd_path "/mnt/sdcard/"

#define OFF_VALUE 0
#define ON_VALUE 1

#define EMMC_POINT_NAME "emmc_point_name"
#define SD_POINT_NAME "sd_point_name"
#define SD_POINT_NAME_2 "sd_point_name_2"

static const char *point_items[] = {
    "/dev/mmcblk0",
    "/dev/mmcblk1",
    "/dev/mmcblk2",
    "/dev/mmcblk3",
};

enum type {
    MMC,
    SD,
    SDIO,
    SDcombo,
};

static const char *typeName[] = {
    "MMC",
    "SD",
    "SDIO",
    "SDcombo",
};

char* getSerial();
void setFlashPoint();
extern Volume* volume_for_path(const char* path);
bool isMtdDevice();

#endif
