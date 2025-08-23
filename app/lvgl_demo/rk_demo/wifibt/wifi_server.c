#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "Rk_wifi.h"
#include "wifibt.h"

#if WIFIBT_EN

#define log(fmt, ...)    printf("[WiFi] (%s %d) "fmt "\n", __func__, __LINE__, ##__VA_ARGS__)

static int fd[2];
static sem_t sem;
static int wifi_result = 0;
static volatile bool rkwifi_gonff = false;
static RK_WIFI_RUNNING_State_e wifi_state = 0;
static int listening = 0;

int wifi_scanning_done(void)
{
    if (wifi_result)
    {
        wifi_result = 0;
        return 1;
    }

    return 0;
}

int wifi_init_done(void)
{
    return listening;
}

int wifi_enabled(void)
{
    return (rkwifi_gonff == false) ? 0 : 1;
}

int wifi_connected(void)
{
    return wifi_state == RK_WIFI_State_CONNECTED ||
           wifi_state == RK_WIFI_State_DHCP_OK;
}

static void printf_connect_info(RK_WIFI_INFO_Connection_s *info)
{
    if (!info)
        return;

    log("	id: %d\n", info->id);
    log("	bssid: %s\n", info->bssid);
    log("	ssid: %s\n", info->ssid);
    log("	freq: %d\n", info->freq);
    log("	mode: %s\n", info->mode);
    log("	wpa_state: %s\n", info->wpa_state);
    log("	ip_address: %s\n", info->ip_address);
    log("	mac_address: %s\n", info->mac_address);
}

static int rk_wifi_state_callback(RK_WIFI_RUNNING_State_e state,
                                  RK_WIFI_INFO_Connection_s *info)
{
    printf("%s state: %d\n", __func__, state);

    if (state != RK_WIFI_State_SCAN_RESULTS)
        wifi_state = state;

    switch (state)
    {
    case RK_WIFI_State_IDLE:
        break;
    case RK_WIFI_State_CONNECTING:
        break;
    case RK_WIFI_State_CONNECTFAILED:
        printf("RK_WIFI_State_CONNECTFAILED\n");
        break;
    case RK_WIFI_State_CONNECTFAILED_WRONG_KEY:
        printf("RK_WIFI_State_CONNECTFAILED_WRONG_KEY\n");
        break;
    case RK_WIFI_State_CONNECTED:
        printf("RK_WIFI_State_CONNECTED\n");
        //printf_connect_info(info);
        //RK_wifi_get_connected_ap_rssi();
        break;
    case RK_WIFI_State_DISCONNECTED:
        printf("RK_WIFI_State_DISCONNECTED\n");
        break;
    case RK_WIFI_State_OPEN:
        rkwifi_gonff = true;
        printf("RK_WIFI_State_OPEN\n");
        break;
    case RK_WIFI_State_OFF:
        rkwifi_gonff = false;
        printf("RK_WIFI_State_OFF\n");
        break;
    case RK_WIFI_State_SCAN_RESULTS:
        printf("RK_WIFI_State_SCAN_RESULTS\n");
        wifi_result = 1;
        break;
    case RK_WIFI_State_DHCP_OK:
        printf("RK_WIFI_State_DHCP_OK\n");
        break;
    }

    return 0;
}

int wifi_query_wait(void *buf, int len)
{
    struct wifibt_cmdarg *cmdarg = (struct wifibt_cmdarg *)buf;

    if (!listening)
        return -1;

    cmdarg->wait = true;
    int ret = write(fd[1], buf, len);
    if (ret <= 0)
        return ret;
    sem_wait(&sem);

    return ret;
}

int wifi_query(void *buf, int len)
{
    struct wifibt_cmdarg *cmdarg = (struct wifibt_cmdarg *)buf;

    if (!listening)
        return -1;

    cmdarg->wait = false;

    log("%d", fd[1]);
    return write(fd[1], buf, len);
}

static void *wifi_server(void *arg)
{
    struct wifibt_cmdarg *cmdarg;
    intptr_t intval;
    char buf[1024];
    char **key;
    int len;

    //wait hci0 appear
    int times = 1000;
    while (times-- > 0 && access("/sys/class/net/wlan0", F_OK))
    {
        usleep(100 * 1000);
    }

    if (access("/sys/class/net/wlan0", F_OK) != 0)
    {
        log("The wlan0 init failure!\n");
        return NULL;
    }
    else
    {
        log("The wlan0 have done.\n");
    }

    if (access("/oem/cfg/wpa_supplicant.conf", F_OK) != 0)
        if (system("mkdir -p /oem/cfg && cp /etc/wpa_supplicant.conf /oem/cfg/"))
            log("Copy wpa_supplicant failed");

    listening = 1;
    while (1)
    {
        int num = read(fd[0], buf, sizeof(buf));
        cmdarg = (struct wifibt_cmdarg *)buf;
        switch (cmdarg->cmd)
        {
        case WIFI_ENABLE:
            log("WIFI_ENABLE\n");
            RK_wifi_register_callback(rk_wifi_state_callback);
            if (RK_wifi_enable(1, "/oem/cfg/wpa_supplicant.conf") < 0)
                log("RK_wifi_enable 1 fail!\n");
            break;
        case WIFI_DISABLE:
            log("WIFI_DISABLE\n");
            if (RK_wifi_enable(0, NULL) < 0)
                log("RK_wifi_enable 0 fail!\n");
            break;
        case WIFI_SCAN:
            log("WIFI_SCAN\n");
            RK_wifi_scan();
            break;
        case WIFI_CONNECT:
            log("WIFI_CONNECT\n");
            key = cmdarg->val;
            RK_wifi_connect(key[0], key[1], *key[2], NULL);
            break;
        case WIFI_DISCONNECT:
            log("WIFI_DISCONNECT\n");
            RK_wifi_disconnect_network();
            break;
        default:
            log("Unknow cmd %d\n", cmdarg->cmd);
            break;
        }
        if (cmdarg->wait)
            sem_post(&sem);
    }
}

int run_wifi_server(void)
{
    pthread_t tid;
    int ret;

    sem_init(&sem, 0, 0);

    ret = pipe(fd);
    if (ret != 0)
    {
        log("wifibt server init failed\n");
        return ret;
    }
    log("%d %d", fd[0], fd[1]);

    ret = pthread_create(&tid, NULL, wifi_server, NULL);
    if (ret < 0)
    {
        close(fd[0]);
        close(fd[1]);
        log("wifibt server start failed\n");
    }

    return ret;
}
#endif

