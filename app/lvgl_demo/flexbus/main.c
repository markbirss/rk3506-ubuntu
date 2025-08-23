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

#include <lvgl/lvgl.h>
#include <lvgl/lv_conf.h>

#include "main.h"

static int quit = 0;

extern void monitor(void);

static void sigterm_handler(int sig)
{
    fprintf(stderr, "signal %d\n", sig);
    quit = 1;
}

int main(int argc, char **argv)
{
    signal(SIGINT, sigterm_handler);
    lv_port_init();

    monitor();

    //get_sysCpuUsage();

    while (!quit)
    {
        lv_task_handler();
        usleep(100);
    }

    return 0;
}

