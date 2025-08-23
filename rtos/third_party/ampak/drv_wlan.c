/*
 * File      : drv_wlan.c
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
#include <stdint.h>
#include <string.h>

#ifdef RT_USING_WIFI
#include <wlan_dev.h>
#include "wlan_mgnt.h"

#include "drv_wlan.h"
#include "drv_wifi.h"

rthw_mode_t wifi_mode;

rthw_mode_t rthw_wifi_mode_get(void)
{
    return wifi_mode;
}

int rthw_wifi_stop(void)
{
    return mhd_module_exit();
}

void rthw_wlan_netif_rx(int idx, void *buff);
static void rthw_wifi_netif_rx_callback(void *buff, int idx)
{
    rthw_wlan_netif_rx(idx, buff);
}

//int rthw_wifi_start(rthw_mode_t mode)
int rthw_mhd_init(struct rt_wlan_device *wlan)
{
    struct rt_mhd_wifi *mhd_wifi = (struct rt_mhd_wifi *)wlan->user_data;

#ifdef RT_WLAN_PROT_LWIP_ENABLE
    mhd_set_netif_rx_callback(rthw_wifi_netif_rx_callback);
#endif
    //mhd_set_country_code(MHD_MK_CNTRY( 'C', 'N', 0 ));
    //mhd_set_country_code((uint32_t)mhd_wifi->country);
    mhd_gpio_config(0, GPIO_BANK3, 16);  //WL_REGON on GPIO_BANK3 with GPIO_PIN_C0
    mhd_gpio_config(1, GPIO_BANK3, 15);  //WL_HWOOB on GPIO_BANK3 with GPIO_PIN_B7
    return mhd_module_init();
}

int rthw_wifi_connect(char *ssid, int ssid_len, char *password, int pass_len, int security_type)
{
    int ret = 0;

    if (security_type == SECURITY_UNKNOWN)
        ret = mhd_join_ap((const unsigned char *)ssid, password);
    else
        ret = mhd_sta_connect(ssid, NULL, security_type, password, 0);

    if (ret == 0)
    {
        //ret = mhd_wifi_set_listen_interval(10, 0);
        //ret = mhd_wifi_set_listen_interval(10, 1);
    }
    return ret;
}

int rthw_wifi_connect_bssid(char *bssid, char *ssid, int ssid_len, char *password, int pass_len, int security_type, int channel)
{
    int mode;

    mode = rthw_wifi_mode_get();
    if ((mode != RTHW_MODE_STA) && (mode != RTHW_MODE_STA_AP))
    {
        return -1;
    }

    return mhd_sta_connect(ssid, bssid, security_type, password, channel);
}

int rthw_wifi_ap_start(char *ssid, char *password, int channel)
{
    int ret = 0;
    uint32_t ip_address = 0xc0a8a901;

    ret = mhd_softap_start(ssid, password, 2, channel);
#ifndef LWIP_USING_DHCPD
    ret = mhd_softap_start_dhcpd(ip_address);
#endif
    return ret;
}

int rthw_wifi_sta_disconnect(void)
{
    return mhd_sta_disconnect(1);
}

int rthw_wifi_ap_disconnect(void)
{
#ifndef LWIP_USING_DHCPD
    mhd_softap_stop_dhcpd();
#endif
    return mhd_softap_stop(1);
}

int rthw_wifi_rssi_get(void)
{
    int rssi = 0;
    rssi = mhd_sta_get_rssi();
    return rssi;
}

void rthw_wifi_channel_set(int interface, int channel)
{
    mhd_wifi_set_channel(interface, channel);
}

int rthw_wifi_channel_get(int interface)
{
    uint32_t channel;
    mhd_wifi_get_channel(interface, &channel);
    return channel;
}

static rt_wlan_security_t security_map_from_mhd(int security)
{
    int result = SECURITY_OPEN;

    switch (security)
    {
    case MHD_SECURE_OPEN:
        result = SECURITY_OPEN;
        break;
    case MHD_WEP_OPEN:
        result = SECURITY_WEP_PSK;
        break;
    case MHD_WEP_SHARED:
        result = SECURITY_WEP_SHARED;
        break;
    case MHD_WPA_PSK_TKIP:
        result = SECURITY_WPA_TKIP_PSK;
        break;
    case MHD_WPA_PSK_AES:
        result = SECURITY_WPA_AES_PSK;
        break;
    case MHD_WPA2_PSK_AES:
        result = SECURITY_WPA2_AES_PSK;
        break;
    case MHD_WPA2_PSK_TKIP:
        result = SECURITY_WPA2_TKIP_PSK;
        break;
    case MHD_WPA2_PSK_MIXED:
        result = SECURITY_WPA2_MIXED_PSK;
        break;
    case MHD_WPA2_PSK_SHA256:
        result = SECURITY_WPA2_SHA256_PSK;
        break;
    case MHD_WPA3_PSK_SAE:
        result = SECURITY_WPA3_SAE;
        break;
    case MHD_WPS_OPEN:
        result = SECURITY_WPS_OPEN;
        break;
    case MHD_WPS_AES:
        result = SECURITY_WPS_SECURE;
        break;
    case MHD_WPA_ENT_AES:
        result = SECURITY_WPA_AES_ENT;
        break;
    case MHD_WPA_ENT_TKIP:
        result = SECURITY_WPA_TKIP_ENT;
        break;
    case MHD_WPA_ENT_MIXED:
        result = SECURITY_WPA_MIXED_ENT;
        break;
    case MHD_WPA2_ENT_AES:
        result = SECURITY_WPA2_AES_ENT;
        break;
    case MHD_WPA2_ENT_TKIP:
        result = SECURITY_WPA2_TKIP_ENT;
        break;
    case MHD_WPA2_ENT_MIXED:
        result = SECURITY_WPA2_MIXED_ENT;
        break;
    default:
        result = -1;
        break;
    }

    return result;
}

void rthw_scanresult_cb(int event, struct rt_wlan_buff *buff, void *parameter)
{
    if (event == RT_WLAN_EVT_SCAN_REPORT)
    {
    }
}

int rthw_wifi_scan_stop()
{
#if 0
    if (mhd_stop_scan() != 0)
    {
        return -RT_ERROR;
    }
    else
        return RT_EOK;
#else
    return RT_EOK;
#endif
}

static struct rt_wlan_device *wlan_dev = NULL;
static mhd_ap_info_t scan_ap_data;

static void rthw_wifi_scan_report(mhd_ap_info_t *results)
{
    struct rt_wlan_info scan_info;
    struct rt_wlan_buff buff;
    int len;

    memset(&scan_info, 0, sizeof(struct rt_wlan_info));
    len = strlen(results[0].ssid);
    if (len >= 32)
        len = 32;

    scan_info.ssid.len = len;
    memcpy(scan_info.ssid.val, results[0].ssid, len);
    memcpy(scan_info.bssid, results[0].bssid, 6);
    scan_info.channel = results[0].channel;
    scan_info.rssi = results[0].rssi;
    scan_info.datarate = results[0].data_rate * 1000;
    scan_info.security = security_map_from_mhd(results[0].security);

    buff.data = &scan_info;
    buff.len = sizeof(struct rt_wlan_info);
    rt_wlan_dev_indicate_event_handle(wlan_dev, RT_WLAN_DEV_EVT_SCAN_REPORT, &buff);
}

int rthw_wifi_scan(void *content, struct rt_scan_info *info)
{
    rt_err_t              status;
    struct rt_wlan_device *wlan = (struct rt_wlan_device *)content;
    wlan_dev = wlan;

    //rt_wlan_register_event_handler(RT_WLAN_EVT_SCAN_REPORT, rthw_scanresult_cb, &scan_ap_data);

    mhd_scan_ap(rthw_wifi_scan_report, &scan_ap_data);

    rt_wlan_dev_indicate_event_handle(wlan, RT_WLAN_DEV_EVT_SCAN_DONE, RT_NULL);
    return 0;
}

void rthw_wifi_monitor_enable(int enable)
{
    if (enable)
    {
        mhd_wifi_set_raw_packet_processor(rthw_wlan_monitor_callback);
        mhd_enable_monitor_mode();
    }
    else
    {
        mhd_disable_monitor_mode();
        mhd_wifi_set_raw_packet_processor(NULL);
    }
}

int rthw_wifi_set_powersave_enable(int enable)
{
    int time_ms = 0;
    if (enable == 2)
        time_ms = 200;
    if (mhd_sta_set_powersave(enable, time_ms) != 0)
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}

int rthw_wifi_get_powersave_mode()
{
    uint8_t mode, time_ms;
    mhd_sta_get_powersave(&mode, &time_ms);
    return mode;
}

void rthw_wifi_country_set(rt_uint32_t country_code)
{
    mhd_set_country_code(country_code);
}

rt_uint32_t rthw_wifi_country_get(void)
{
    rt_uint32_t country_code;
    mhd_get_country_code(&country_code);
    return country_code;
}

int rthw_wifi_mac_set(rt_uint8_t mac[], int interface)
{
    mhd_mac_t mhd_mac;
    memcpy(mhd_mac.octet, mac, 6);
    return mhd_set_mac_address(mhd_mac);
}

int rthw_wifi_mac_get(rt_uint8_t mac[], int interface)
{
    mhd_mac_t mhd_mac;
    if (mhd_get_mac_address(&mhd_mac) == 0)
        memcpy(mac, mhd_mac.octet, 6);
    return 0;
}

int rthw_wifi_send(void *buff, int interface)
{
    //return mhd_host_network_xmit(buff, interface);
    return mhd_network_xmit(buff, interface);
}
#endif
