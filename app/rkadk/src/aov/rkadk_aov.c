/*
 * Copyright (c) 2023 Rockchip, Inc. All Rights Reserved.
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

#include "rkadk_common.h"
#include "rkadk_log.h"
#include "rkadk_aov.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

#ifdef RV1106_1103
#define SUSPEND_TIME_REG 0xff300048
#else
#define SUSPEND_TIME_REG 0xff3e0048
#endif

#define SOC_SLEEP_STR "mem"
#define SOC_SLEEP_PATH "/sys/power/state"
static pthread_mutex_t gWakeupRunMutex = PTHREAD_MUTEX_INITIALIZER;
static RKADK_AOV_NOTIFY_CALLBACK gpfnNotifyCallback = NULL;

void RKADK_AOV_WakeupLock() {
  pthread_mutex_lock(&gWakeupRunMutex);
}

void RKADK_AOV_WakeupUnlock() {
  pthread_mutex_unlock(&gWakeupRunMutex);
}

int RKADK_AOV_Init(RKADK_AOV_ARG_S *pstAovAttr) {
  pthread_mutex_init(&gWakeupRunMutex, NULL);
  gpfnNotifyCallback = pstAovAttr->pfnNotifyCallback;
  return 0;
}

int RKADK_AOV_DeInit() {
  pthread_mutex_destroy(&gWakeupRunMutex);
  gpfnNotifyCallback = NULL;
  return 0;
}

int RKADK_AOV_EnterSleep() {
  int fd = -1;
  ssize_t ret = -1;

  //pthread_mutex_lock(&gWakeupRunMutex);
  fd = open(SOC_SLEEP_PATH, O_WRONLY | O_TRUNC);
  if (fd == -1) {
    RKADK_LOGE("Failed to open %s, errno=%d, %s", SOC_SLEEP_PATH, errno, strerror(errno));
    goto EXIT;
  }

  ret = write(fd, SOC_SLEEP_STR, strlen(SOC_SLEEP_STR));
  if (ret == -1) {
    RKADK_LOGE("Failed to write %s, errno=%d, %s", SOC_SLEEP_STR, errno, strerror(errno));
    goto EXIT;
  }

  RKADK_LOGD("echo \"%s\" > %s successfully", SOC_SLEEP_STR, SOC_SLEEP_PATH);
  ret = 0;

EXIT:
  if (fd >= 0)
    close(fd);

  //pthread_mutex_unlock(&gWakeupRunMutex);
  return ret;
}

void RKADK_AOV_Notify(RKADK_AOV_EVENT_E enEvent, void *msg) {
  if (gpfnNotifyCallback)
    gpfnNotifyCallback(enEvent, msg);
  else
    RKADK_LOGW("Unregistered notify callback");
}

static int RKADK_AOV_WriteReg(int addr, int value) {
  int memFd = open("/dev/mem", O_RDWR | O_SYNC);
  if (memFd < 0) {
    perror("Error opening /dev/mem");
    return -1;
  }

  //get page size
  size_t pageSize = getpagesize();

  //calculates the address and offset of the page alignment
  off_t pageBase = (addr & ~(pageSize - 1));
  off_t pageOffset = addr - pageBase;

  //mapped memory
  void *memMap = mmap(NULL, pageSize, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, pageBase);
  if (memMap == MAP_FAILED) {
    RKADK_LOGE("Error mapping memory");
    close(memFd);
    return -1;
  }

  //compute register address
  int *targetReg = (int *)((char *)memMap + pageOffset);

  //write data
  *targetReg = value;

  //unmap memory
  munmap(memMap, pageSize);
  close(memFd);
  return 0;
}


int RKADK_AOV_SetSuspendTime(int u32WakeupSuspendTime) {
  int ret;

#ifdef RV1106_1103
  ret = RKADK_AOV_WriteReg(SUSPEND_TIME_REG, u32WakeupSuspendTime * 32.768);
#else
  ret = RKADK_AOV_WriteReg(SUSPEND_TIME_REG, u32WakeupSuspendTime * 91);
#endif
  if (ret != 0) {
    RKADK_LOGD("Failed to set suspend time!");
    return -1;
  }

  RKADK_LOGD("wakeup susoend time = %d", u32WakeupSuspendTime);
  return 0;
}

#ifdef RV1106_1103
int RKADK_AOV_DisableNonBootCPUs() { return RKADK_SUCCESS; }
int RKADK_AOV_EnableNonBootCPUs() { return RKADK_SUCCESS; }
#else
int RKADK_AOV_DisableNonBootCPUs() {
  int ret, fd;
  const char *off = "0";

  fd = open("/sys/devices/system/cpu/cpu1/online", O_WRONLY);
  if (fd >= 0) {
    ret = write(fd, off, strlen(off));
    if (ret >= 0)
      RKADK_LOGD("disable cpu 1 success");
    else
      RKADK_LOGE("disable cpu 1 failed because %s", strerror(errno));
    close(fd);
  }

  fd = open("/sys/devices/system/cpu/cpu2/online", O_WRONLY);
  if (fd >= 0) {
    ret = write(fd, off, strlen(off));
    if (ret >= 0)
      RKADK_LOGD("disable cpu 2 success");
    else
      RKADK_LOGE("disable cpu 2 failed because %s", strerror(errno));
    close(fd);
  }

  fd = open("/sys/devices/system/cpu/cpu3/online", O_WRONLY);
  if (fd >= 0) {
    ret = write(fd, off, strlen(off));
    if (ret >= 0)
      RKADK_LOGD("disable cpu 3 success");
    else
      RKADK_LOGE("disable cpu 3 failed because %s", strerror(errno));
    close(fd);
  }

  return RKADK_SUCCESS;
}

int RKADK_AOV_EnableNonBootCPUs() {
  int fd;
  const char *on = "1";
  int ret;

  fd = open("/sys/devices/system/cpu/cpu1/online", O_WRONLY);
    if (fd >= 0) {
      ret = write(fd, on, strlen(on));
      if (ret > 0)
        RKADK_LOGD("enable cpu 1 success");
      else
        RKADK_LOGE("enable cpu 1 failed because %s", strerror(errno));
    close(fd);
  }

  fd = open("/sys/devices/system/cpu/cpu2/online", O_WRONLY);
  if (fd >= 0) {
    ret = write(fd, on, strlen(on));
    if (ret > 0)
      RKADK_LOGD("enable cpu 2 success");
    else
      RKADK_LOGE("enable cpu 2 failed because %s", strerror(errno));
    close(fd);
  }

  fd = open("/sys/devices/system/cpu/cpu3/online", O_WRONLY);
  if (fd >= 0) {
    ret = write(fd, on, strlen(on));
    if (ret > 0)
      RKADK_LOGD("enable cpu 3 success");
    else
      RKADK_LOGE("enable cpu 3 failed because %s", strerror(errno));
    close(fd);
  }

  return RKADK_SUCCESS;
}
#endif
