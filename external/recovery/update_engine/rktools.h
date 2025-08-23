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

#include <stdbool.h>

#define MTD_PATH "/proc/mtd"
#define CMDLINE_LENGTH 2048
#define EMMC_POINT_NAME "emmc_point_name"
#define NAND_DRIVER_DEV_LBA "/dev/mtd0"



//获取本地版本号
bool getLocalVersion(char *version, int maxLength);
//获取服务器版本号，用于从服务端下载固件前，先比较版本号
bool getRemoteVersion(char *url, char *version, int maxLength);
//从环境变量获取flash 的挂载点
void getFlashPoint(char *path);
//获取flash 的大小，M为单位，和flash 的块数
int getFlashSize(char *path, long long* flash_size, long long* block_num);
int getFlashInfo (size_t *total_size, size_t *block_size, size_t *page_size);


//itoa(number,string,10);//按十进制转换
//itoa(number,string,16);//按16进制转换
char* itoa(int num, char* str, int radix);

bool isMtdDevice();
int getCurrentSlot();
#endif
