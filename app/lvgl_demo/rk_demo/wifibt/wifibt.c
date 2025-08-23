#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "wifibt.h"

#define log(fmt, ...)    printf("[WIFIBT] "fmt, ##__VA_ARGS__)

static void *wifibt_init(void *arg)
{
    /* /etc/init.d/S36wifibt-init.sh */
    //system("/usr/bin/wifibt-init.sh start");
}

int run_wifibt_server(void)
{
    int ret;

#if WIFIBT_EN || BT_EN
    pthread_t tid;
    if (pthread_create(&tid, NULL, wifibt_init, NULL))
    {
        log("pthread create err\n");
        return -1;
    }
#endif

#if WIFIBT_EN
    ret = run_wifi_server();
    if (ret != 0)
        log("run_wifi_server failed\n");
#endif
#if BT_EN
    ret = run_bt_server();
    if (ret != 0)
        log("run_bt_server failed\n");
#endif

    return ret;
}

