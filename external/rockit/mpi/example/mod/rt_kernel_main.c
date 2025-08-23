/*
 * Copyright 2018 Rockchip Electronics Co. LTD
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
 *
 * author: martin.cheng@rock-chips.com
 *   date: 2018/12/15
 */

#include <string.h>
#include "rk_osal.h"
#include "rk_mpi_sys.h"
#include "test_comm_argparse.h"

int mpi_test_init(void);
int vvi_demo_init(void);
int vrga_demo_init(void);
int venc_demo_init(void);
int rt_kernel_main_enter(int argc, char **argv) {
    if (RK_MPI_SYS_Init() != RK_SUCCESS) {
        goto __FAILED;
    }
    vvi_demo_init();
    vrga_demo_init();
    venc_demo_init();
    mpi_test_init();
    if (RK_MPI_SYS_Exit() != RK_SUCCESS) {
        goto __FAILED;
    }
    return 0;
__FAILED:
    return -1;
}

static void rkDumpsys(void *arg)
{
    char *argvArray = (char *)arg;
    char *buf = (char *)malloc(100 * 1024);
    memset(buf, 0, 100 * 1024);
    RK_MPI_SYS_DumpSys(argvArray, buf, 100 * 1024);
    printf("%s\n", buf);
    free(buf);
}

int dumpsys_enter(int argc, char **argv) {
    char argvArray[128] = {0};
    rkos_task_t *tid = NULL;
    for (int i = 0; i < argc; i++) {
        snprintf(argvArray + strlen(argvArray), 128 - strlen(argvArray), "%s ", argv[i]);
    }

    tid = rkos_kthread_create(rkDumpsys, argvArray, "dumpsys", 40960, 0);
    if (!tid) {
        RK_LOGE("send thread create failed");
        return -1;
    }

    if (tid) {
        rkos_kthread_destory(tid, 1);
        tid = NULL;
    }

    return 0;
}

#ifdef OS_RTT
#include <finsh.h>

int dumpsys(int argc, char **argv)
{
    argparse_excute_main(argc, argv, dumpsys_enter);
    return 0;
}

MSH_CMD_EXPORT(dumpsys, dump rockit module info);

int rt_kernel_main(int argc, char **argv)
{
    argparse_excute_main(argc, argv, rt_kernel_main_enter);
    return 0;
}

MSH_CMD_EXPORT(rt_kernel_main, rockit kernel module test);
#endif