/*
 * Copyright (c) 2021 Rockchip, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef __RKADK_LOG_H__
#define __RKADK_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define MODULE_TAG "RKADK"

#define RKADK_LOG_LEVEL_PRINT 0
#define RKADK_LOG_LEVEL_ERROR 1
#define RKADK_LOG_LEVEL_WARN 2
#define RKADK_LOG_LEVEL_INFO 3
#define RKADK_LOG_LEVEL_DEBUG 4

#define RKADK_TRACE(level, fmt, ...)           \
      do {                                          \
          rkadk_log(level, fmt, __FUNCTION__,       \
                  __LINE__, ##__VA_ARGS__);         \
      } while (0)

#define RKADK_LOGP(fmt, ...)  RKADK_TRACE(RKADK_LOG_LEVEL_PRINT,    fmt, ##__VA_ARGS__)
#define RKADK_LOGE(fmt, ...)  RKADK_TRACE(RKADK_LOG_LEVEL_ERROR,   fmt, ##__VA_ARGS__)
#define RKADK_LOGW(fmt, ...)  RKADK_TRACE(RKADK_LOG_LEVEL_WARN,    fmt, ##__VA_ARGS__)
#define RKADK_LOGI(fmt, ...)  RKADK_TRACE(RKADK_LOG_LEVEL_INFO,    fmt, ##__VA_ARGS__)
#define RKADK_LOGD(fmt, ...)  RKADK_TRACE(RKADK_LOG_LEVEL_DEBUG,   fmt, ##__VA_ARGS__)

void rkadk_log(int level, const char *fmt,
                  const char *fname, const int row, ...);
int get_log_level();

#define RKADK_KLOG(format, ...)                                                \
  do {                                                                         \
    FILE *fp = fopen("/dev/kmsg", "w");                                        \
    if (NULL != fp) {                                                          \
      fprintf(fp, "[RKADK]: " format"\n", ##__VA_ARGS__);                      \
      fclose(fp);                                                              \
    }                                                                          \
  } while (0)

#ifdef __cplusplus
}
#endif
#endif
