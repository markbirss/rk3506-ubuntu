/*
 * File      : drv_wifi.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2017, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rtdef.h>
#include <rtthread.h>
#include <rthw.h>
#include <rtdef.h>
#include <wlan_dev.h>
#include <wlan_prot.h>
#include <string.h>

#include "drv_wlan.h"
#include "drv_wifi.h"
#include "pbuf.h"

#define DBG_LEVEL DBG_LOG //DBG_INFO
#define DBG_SECTION_NAME  "MHD"
#include <rtdbg.h>

#define MAX_ADDR_LEN          (6)

#define WIFI_INIT_FLAG        (0x1 << 0)
#define WIFI_MAC_FLAG         (0x1 << 1)
//#define WIFI_TYPE_STA         (0)
//#define WIFI_TYPE_AP          (1)
#define WIFI_COUNTRY_CODE       MHD_MK_CNTRY( 'C', 'N', 0 )

static struct rt_mhd_wifi mhd_dev[MAX_INTERFACE] =
{
    [STA_INTERFACE] =
    {
        .type    = STA_INTERFACE,
        .country = WIFI_COUNTRY_CODE,
    },
    [AP_INTERFACE] =
    {
        .type    = AP_INTERFACE,
        .country = WIFI_COUNTRY_CODE,
    }
};

#if 0
struct mhd_wifi
{
    struct rt_wlan_device *wlan;
    rt_uint8_t dev_addr[MAX_ADDR_LEN];
    rt_uint8_t flag;
    int connected;
    int type;
};
#endif



static struct rt_wlan_device mhd_wlan_sta;
static struct rt_wlan_device mhd_wlan_ap;
static const struct rt_wlan_dev_ops mhd_wlan_ops;
extern rthw_mode_t wifi_mode;

rt_inline struct rt_mhd_wifi *rthw_wifi_get_dev(int idx)
{
#if 0
    if (idx == 0) return &mhd_sta;
    if (idx == 1) return &mhd_ap;
#else
    if (idx < MAX_INTERFACE)
        return &mhd_dev[idx];
    else
#endif
    return RT_NULL;
}

rt_inline int rthw_wifi_get_idx(struct rt_mhd_wifi *wifi)
{
    int mode = rthw_wifi_mode_get();

    if (mode == 1) return 0;
    if (mode == 2) return 1;
    return wifi->type;
}

int rthw_wifi_register(struct rt_mhd_wifi *wifi)
{
    struct rt_wlan_device *wlan = wifi->wlan;

    if ((wifi->flag & WIFI_INIT_FLAG) == 0)
    {
        if (wifi->type == STA_INTERFACE)
        {
            rt_wlan_dev_register(wlan, RT_WLAN_DEVICE_STA_NAME, &mhd_wlan_ops, 0, wifi);
        }
        if (wifi->type == AP_INTERFACE)
        {
            rt_wlan_dev_register(wlan, RT_WLAN_DEVICE_AP_NAME, &mhd_wlan_ops, 0, wifi);
        }

        wifi->flag |= WIFI_INIT_FLAG;
        wifi->wlan = wlan;
        rt_kprintf("F:%s L:%d wifi: 0x%08x wlan: 0x%08x\n", __FUNCTION__, __LINE__, wifi, wlan);
    }
    return RT_EOK;
}

void rthw_wlan_set_netif_info(int idx, rt_device_t *dev, rt_uint8_t *dev_addr)
{
    struct rt_mhd_wifi *wifi = RT_NULL;

    wifi = rthw_wifi_get_dev(idx);
    if (wifi == RT_NULL)
        return;
    LOG_E("F:%s L:%d idx %d  wifi:0x%08x  flag:0x%x \n", __FUNCTION__, __LINE__, idx, wifi, wifi->flag);
    rthw_wifi_register(wifi);
    *dev = &wifi->wlan->device;
    LOG_E("wifi type:%d \n", wifi->type);
}

static int security_map_from_rtthread(int security)
{
    int result = MHD_SECURE_OPEN;

    switch (security)
    {
    case SECURITY_OPEN:
        result = MHD_SECURE_OPEN;
        break;
    case SECURITY_WEP_PSK:
        result = MHD_WEP_OPEN;
        break;
    case SECURITY_WEP_SHARED:
        result = MHD_WEP_SHARED;
        break;
    case SECURITY_WPA_TKIP_PSK:
        result = MHD_WPA_PSK_TKIP;
        break;
    case SECURITY_WPA_AES_PSK:
        result = MHD_WPA_PSK_AES;
        break;
    case SECURITY_WPA2_AES_PSK:
        result = MHD_WPA2_PSK_AES;
        break;
    case SECURITY_WPA2_TKIP_PSK:
        result = MHD_WPA2_PSK_TKIP;
        break;
    case SECURITY_WPA2_MIXED_PSK:
        result = MHD_WPA2_PSK_MIXED;
        break;
    case SECURITY_WPA2_SHA256_PSK:
        result = MHD_WPA2_PSK_SHA256;
        break;
    case SECURITY_WPA3_SAE:
        result = MHD_WPA3_PSK_SAE;
        break;
    case SECURITY_WPS_OPEN:
        result = MHD_WPS_OPEN;
        break;
    case SECURITY_WPS_SECURE:
        result = MHD_WPS_AES;
        break;
    case SECURITY_UNKNOWN:
    default:
        result = MHD_STA_SECURITY_AUTO;
        break;
    }

    return result;
}

void rthw_wlan_monitor_callback(void *buf, int interface)
{
    struct rt_wlan_device *wlan = NULL;
    struct pbuf *p = buf;

    if (interface == 1)
        wlan = mhd_dev[1].wlan;
    else if (interface == 0)
        wlan = mhd_dev[0].wlan;
    else
        return;

    rt_wlan_dev_promisc_handler(wlan, p->payload, p->len);
    pbuf_free(p);
}

static rt_err_t rthw_wlan_init(struct rt_wlan_device *wlan)
{
    //struct rt_mhd_wifi *wifi = wlan->user_data;
    int ret = 0;

    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);

    ret = rthw_mhd_init(wlan);
    //mhd_config_fwlog(1, 100);

    if (ret)
    {
        return ret;
    }

    return RT_EOK;
}

static rt_err_t rthw_wlan_mode(struct rt_wlan_device *wlan, rt_wlan_mode_t mode)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)(wlan->user_data);

    LOG_D("F:%s L:%d mode:%d", __FUNCTION__, __LINE__, mode);

    if (mode == RT_WLAN_STATION)
    {
        if (wifi->type != STA_INTERFACE)
        {
            LOG_D("this wlan not support sta mode");
            return -RT_ERROR;
        }
    }
    else if (mode == RT_WLAN_AP)
    {
        if (wifi->type != AP_INTERFACE)
        {
            LOG_D("this wlan not support ap mode");
            return -RT_ERROR;
        }
    }
    wifi_mode = mode;

    return RT_EOK;
}

static rt_err_t rthw_wlan_scan(struct rt_wlan_device *wlan, struct rt_scan_info *scan_info)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)(wlan->user_data);

    if (wifi->type != STA_INTERFACE)
    {
        LOG_E("this wlan not support scan mode");
        return -RT_ERROR;
    }

    rthw_wifi_scan((void *)wlan, scan_info);
    return RT_EOK;
}

static rt_err_t rthw_wlan_join(struct rt_wlan_device *wlan, struct rt_sta_info *sta_info)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)(wlan->user_data);
    int result = 0, i;
    char *ssid = RT_NULL, *key = RT_NULL;

    if (wifi->type != STA_INTERFACE)
    {
        LOG_E("this wlan not support sta mode");
        return -RT_ERROR;
    }

    for (i = 0; i < RT_WLAN_BSSID_MAX_LENGTH; i++)
    {
        if (sta_info->bssid[i] != 0xff || sta_info->bssid[i] != 0x00)
            break;
    }

    if (i < RT_WLAN_BSSID_MAX_LENGTH && sta_info->channel != 0)
    {
        if (sta_info->ssid.len > 0)
            ssid = (char *)&sta_info->ssid.val[0];
        if (sta_info->key.len > 0)
            key = (char *)&sta_info->key.val[0];
        LOG_D("bssid connect bssid: %02x:%02x:%02x:%02x:%02x:%02x ssid:%s ssid_len:%d key:%s key_len%d security%d",
              sta_info->bssid[0], sta_info->bssid[1], sta_info->bssid[2], sta_info->bssid[3], sta_info->bssid[4], sta_info->bssid[5],
              ssid, sta_info->ssid.len, key, sta_info->key.len, sta_info->security);
        result = rthw_wifi_connect_bssid((char *)sta_info->bssid, ssid, sta_info->ssid.len, key, sta_info->key.len, security_map_from_rtthread(sta_info->security), sta_info->channel);
    }
    else
    {
        LOG_D("ssid %s, key_len %d, key %s i %d\r\n", sta_info->ssid.val, sta_info->key.len, sta_info->key.val, i);
        if (sta_info->key.len == 0)
            sta_info->security = SECURITY_OPEN;
        else if (sta_info->key.len >= 8)
            sta_info->security = SECURITY_UNKNOWN; //using scan to get security type
        result = rthw_wifi_connect((char *)sta_info->ssid.val, sta_info->ssid.len, (char *)sta_info->key.val, sta_info->key.len, security_map_from_rtthread(sta_info->security));
    }

    if (result == 0)
    {
        wifi->connected = 1;
        rt_wlan_dev_indicate_event_handle(wifi->wlan, RT_WLAN_DEV_EVT_CONNECT, RT_NULL);
    }
    else
    {
        rt_wlan_dev_indicate_event_handle(wifi->wlan, RT_WLAN_DEV_EVT_CONNECT_FAIL, RT_NULL);
    }

    if (result != 0)
    {
        LOG_E("connect failed...");
        return -RT_ERROR;
    }

    return RT_EOK;
}

static rt_err_t rthw_wlan_softap(struct rt_wlan_device *wlan, struct rt_ap_info *ap_info)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)(wlan->user_data);
    char *psk = NULL;

    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);

    if (wifi->type != AP_INTERFACE)
    {
        LOG_E("this wlan not support ap mode");
        return -RT_ERROR;
    }

    if (ap_info->security != 0)
        psk = (char *)&ap_info->key.val[0];

    if (rthw_wifi_ap_start((char *)&ap_info->ssid.val[0], psk, ap_info->channel) != 0)
    {
        rt_wlan_dev_indicate_event_handle(wifi->wlan, RT_WLAN_DEV_EVT_AP_STOP, RT_NULL);
        wifi->connected = 0;
        return -RT_ERROR;
    }
    rt_wlan_dev_indicate_event_handle(wifi->wlan, RT_WLAN_DEV_EVT_AP_START, RT_NULL);
    wifi->connected = 1;
    return RT_EOK;
}

static rt_err_t rthw_wlan_disconnect(struct rt_wlan_device *wlan)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)(wlan->user_data);

    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);

    if (wifi->type != STA_INTERFACE)
    {
        LOG_E("this wlan not support sta mode");
        return -RT_ERROR;
    }
    wifi->connected = 0;
    rthw_wifi_sta_disconnect();
    rt_wlan_dev_indicate_event_handle(wifi->wlan, RT_WLAN_DEV_EVT_DISCONNECT, RT_NULL);
    return RT_EOK;
}

static rt_err_t rthw_wlan_ap_stop(struct rt_wlan_device *wlan)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)(wlan->user_data);

    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);

    if (wifi->type != AP_INTERFACE)
    {
        LOG_E("this wlan not support ap mode");
        return -RT_ERROR;
    }

    rthw_wifi_ap_disconnect();
    rt_wlan_dev_indicate_event_handle(wifi->wlan, RT_WLAN_DEV_EVT_AP_STOP, RT_NULL);
    return RT_EOK;
}

static rt_err_t rthw_wlan_ap_deauth(struct rt_wlan_device *wlan, rt_uint8_t mac[])
{
    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);
    return RT_EOK;
}

static rt_err_t rthw_wlan_scan_stop(struct rt_wlan_device *wlan)
{
    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);
    if (rthw_wifi_scan_stop() == 0)
        return RT_EOK;
    else
        return -RT_ERROR;
}

static int rthw_wlan_get_rssi(struct rt_wlan_device *wlan)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)(wlan->user_data);

    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);

    if (wifi->type != STA_INTERFACE)
    {
        LOG_E("this wlan not support sta mode");
        return -RT_ERROR;
    }

    return rthw_wifi_rssi_get();
}

static rt_err_t rthw_wlan_set_powersave(struct rt_wlan_device *wlan, int level)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)(wlan->user_data);

    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);

    if (wifi->type != STA_INTERFACE)
    {
        LOG_E("this wlan not support sta mode");
        return -RT_ERROR;
    }

    return rthw_wifi_set_powersave_enable(level);
}

static int rthw_wlan_get_powersave(struct rt_wlan_device *wlan)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)(wlan->user_data);

    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);

    if (wifi->type != STA_INTERFACE)
    {
        LOG_E("this wlan not support sta mode");
        return -RT_ERROR;
    }

    return rthw_wifi_get_powersave_mode();
}

static rt_err_t rthw_wlan_cfg_promisc(struct rt_wlan_device *wlan, rt_bool_t start)
{
    LOG_E("F:%s L:%d enable %d \n", __FUNCTION__, __LINE__, start);

    if (start)
        rthw_wifi_monitor_enable(1);
    else
        rthw_wifi_monitor_enable(0);

    return RT_EOK;
}

static rt_err_t rthw_wlan_cfg_filter(struct rt_wlan_device *wlan, struct rt_wlan_filter *filter)
{
    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);
    return RT_EOK;
}

static rt_err_t rthw_wlan_set_channel(struct rt_wlan_device *wlan, int channel)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)(wlan->user_data);

    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);
    rthw_wifi_channel_set(wifi->type, channel);

    return RT_EOK;
}

static int rthw_wlan_get_channel(struct rt_wlan_device *wlan)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)(wlan->user_data);
    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);
    return rthw_wifi_channel_get(wifi->type);
}

static rt_err_t rthw_wlan_set_country(struct rt_wlan_device *wlan, rt_country_code_t country_code)
{
    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);
    rthw_wifi_country_set(country_code);
    return RT_EOK;
}

static rt_country_code_t rthw_wlan_get_country(struct rt_wlan_device *wlan)
{
    return rthw_wifi_country_get();
}

static rt_err_t rthw_wlan_set_mac(struct rt_wlan_device *wlan, rt_uint8_t mac[])
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)wlan->user_data;

    if (rthw_wifi_mac_set(mac, wifi->type) == 0)
    {
        LOG_I("interface %d mac: %02x-%02x-%02x-%02x-%02x-%02x\r\n", wifi->type, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return RT_EOK;
    }

    return -RT_ERROR;
}

static rt_err_t rthw_wlan_get_mac(struct rt_wlan_device *wlan, rt_uint8_t mac[])
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)wlan->user_data;
    int idx = wifi->type;

    if (mac == RT_NULL)
    {
        return -RT_ERROR;
    }

    if (rthw_wifi_mac_get(mac, idx) == 0)
    {
        LOG_I("interface %d mac: %02x-%02x-%02x-%02x-%02x-%02x\r\n", idx, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return RT_EOK;
    }

    return -RT_ERROR;
}

static int rthw_wlan_recv(struct rt_wlan_device *wlan, void *buff, int len)
{
    LOG_D("F:%s L:%d", __FUNCTION__, __LINE__);
    return RT_EOK;
}

static int rthw_wlan_send(struct rt_wlan_device *wlan, void *buff, int len)
{
    struct rt_mhd_wifi *wifi = (struct rt_mhd_wifi *)wlan->user_data;
    int idx = wifi->type;
    struct pbuf *p = buff;

    if (!wifi->connected)
    {
        return RT_EOK;
    }
    //rt_kprintf("tx: buf %p len %d tot %d\n", buff, p->len, p->tot_len);
    //RT_ASSERT(len != 0);
#ifdef RT_WLAN_PROT_LWIP_PBUF_FORCE
    rthw_wifi_send(buff, idx);
#else
    extern int mhd_host_network_xmit_buff(struct netif * netif, void *buff, int len);
    mhd_host_network_xmit_buff(netif, p, len);
#endif

    return RT_EOK;
}

void rthw_wlan_netif_rx(int idx, void *buff)
{
    struct rt_mhd_wifi *wifi = rthw_wifi_get_dev(idx);
    struct pbuf *p = buff;

    if (!wifi->connected)
    {
        pbuf_free(p);
        return;
    }

    if (!p)
    {
        LOG_D("%s: error ------ Buffer is NULL\r\n", __func__);
        return;
    }

    //rt_kprintf("rx: buf %p len %d tot %d\n", p, p->len, p->tot_len);
    rt_wlan_dev_report_data(wifi->wlan, p, p->len);
}

static const struct rt_wlan_dev_ops mhd_wlan_ops =
{
    .wlan_init             =     rthw_wlan_init,
    .wlan_mode             =     rthw_wlan_mode,
    .wlan_scan             =     rthw_wlan_scan,
    .wlan_join             =     rthw_wlan_join,
    .wlan_softap           =     rthw_wlan_softap,
    .wlan_disconnect       =     rthw_wlan_disconnect,
    .wlan_ap_stop          =     rthw_wlan_ap_stop,
    .wlan_ap_deauth        =     rthw_wlan_ap_deauth,
    .wlan_scan_stop        =     rthw_wlan_scan_stop,
    .wlan_get_rssi         =     rthw_wlan_get_rssi,
    .wlan_set_powersave    =     rthw_wlan_set_powersave,
    .wlan_get_powersave    =     rthw_wlan_get_powersave,
    .wlan_cfg_promisc      =     rthw_wlan_cfg_promisc,
    .wlan_cfg_filter       =     rthw_wlan_cfg_filter,
    .wlan_set_channel      =     rthw_wlan_set_channel,
    .wlan_get_channel      =     rthw_wlan_get_channel,
    .wlan_set_country      =     rthw_wlan_set_country,
    .wlan_get_country      =     rthw_wlan_get_country,
    .wlan_set_mac          =     rthw_wlan_set_mac,
    .wlan_get_mac          =     rthw_wlan_get_mac,
    .wlan_recv             =     rthw_wlan_recv,
    .wlan_send             =     rthw_wlan_send,
};

int rthw_wifi_low_init(void)
{
    static rt_int8_t _init_flag = 0;

    if (_init_flag)
    {
        return 1;
    }

    rt_memset(mhd_dev, 0, sizeof(struct rt_mhd_wifi) * 2);
    mhd_wlan_sta.mode = RT_WLAN_STATION;
    mhd_dev[STA_INTERFACE].type = STA_INTERFACE;
    mhd_dev[STA_INTERFACE].wlan = &mhd_wlan_sta;

    mhd_wlan_ap.mode = RT_WLAN_AP;
    mhd_dev[AP_INTERFACE].type = AP_INTERFACE;
    mhd_dev[AP_INTERFACE].wlan = &mhd_wlan_ap;

    _init_flag = 1;

    return 0;
}

INIT_DEVICE_EXPORT(rthw_wifi_low_init);


#include "ethernetif.h"
#include "netif.h"
//copy it from wlan_lwip.c
struct lwip_prot_des
{
    struct rt_wlan_prot prot;
    struct eth_device eth;
    rt_int8_t connected_flag;
    struct rt_timer timer;
    struct rt_work work;
};

void *mhd_ext_netif(struct netif *netif, uint8_t interface);
void mhd_netif_assign(rt_wlan_mode_t mode)
{
    struct lwip_prot_des *lwip_prot_sta = mhd_wlan_sta.prot;
    struct lwip_prot_des *lwip_prot_ap = mhd_wlan_ap.prot;
    struct eth_device *eth = NULL;
    struct netif *netif = NULL;
    uint8_t interface = 0;
    if (mode == RT_WLAN_STATION)
    {
        interface = 0;
        eth = &lwip_prot_sta->eth;
        netif = eth->netif;
    }
    else if (mode == RT_WLAN_AP)
    {
        interface = 1;
        eth = &lwip_prot_ap->eth;
        netif = eth->netif;
    }
    mhd_ext_netif(netif, interface);
}
