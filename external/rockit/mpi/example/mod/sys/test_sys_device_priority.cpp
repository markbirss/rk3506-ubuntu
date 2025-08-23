/* Copyright 2020 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#undef DBG_MOD_ID
#define DBG_MOD_ID       RK_ID_SYS

#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

#include "rk_debug.h"
#include "rk_mpi_sys.h"

#include "test_comm_argparse.h"

RK_S32 unit_test_mpi_sys_device_priority() {
    RK_S32 s32Ret = RK_SUCCESS;
    DEVICE_PRIORITY_LEVEL_CONFIG_S priority;

    for (RK_S32 i = 0; i < RK_DID_BUTT; i++) {
        priority.enDeviceId = (DEVICE_ID_E)i;
        for (RK_S32 j = 0; j < 6; j++) {
            priority.s32Level = j;
            s32Ret = RK_MPI_SYS_SetDevicePriority(&priority);
            if (RK_SUCCESS != s32Ret) {
                RK_LOGE("sys device %d priority %d RK_MPI_SYS_SetDevicePriority failed: %#x!",
                        priority.enDeviceId, priority.s32Level, s32Ret);
            } else {
                RK_LOGV("sys device %d priority %d RK_MPI_SYS_SetDevicePriority already.",
                        priority.enDeviceId, priority.s32Level, s32Ret);
            }

            priority.s32Level = 0;
            s32Ret = RK_MPI_SYS_GetDevicePriority(&priority);
            if (RK_SUCCESS != s32Ret) {
                RK_LOGE("sys device %d priority %d RK_MPI_SYS_GetDevicePriority failed: %#x!",
                        priority.enDeviceId, priority.s32Level, s32Ret);
            } else {
                RK_LOGV("sys device %d priority %d RK_MPI_SYS_GetDevicePriority already.",
                        priority.enDeviceId, priority.s32Level, s32Ret);
            }
        }
    }

    return s32Ret;
}

static const char *const usages[] = {
    "./rk_mpi_sys_test...",
    NULL,
};

RK_S32 main(RK_S32 argc, const char **argv) {
    RK_S32 s32Ret = RK_SUCCESS;

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("basic options:"),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nselect a test case to run.",
                                 "\nuse --help for details.");

    argc = argparse_parse(&argparse, argc, argv);

    s32Ret = RK_MPI_SYS_Init();
    if (s32Ret != RK_SUCCESS) {
        return s32Ret;
    }

    s32Ret = unit_test_mpi_sys_device_priority();
    if (s32Ret != RK_SUCCESS) {
        goto __FAILED;
    }

    s32Ret = RK_MPI_SYS_Exit();
    if (s32Ret != RK_SUCCESS) {
        return s32Ret;
    }
    RK_LOGI("test running ok.");
    return RK_SUCCESS;

__FAILED:
    RK_MPI_SYS_Exit();
    RK_LOGE("test running failed!");
    return s32Ret;
}
