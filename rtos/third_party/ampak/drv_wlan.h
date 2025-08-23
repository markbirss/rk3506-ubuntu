/*
 * File      : drv_wlan.h
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

#ifndef __DRV_WLAN_H__
#define __DRV_WLAN_H__

#include "rtthread.h"
#include "rtdevice.h"
#include "hal_base.h"
#include "mhd_api.h"

typedef enum
{
    RTHW_MODE_NONE = 0,
    RTHW_MODE_STA,
    RTHW_MODE_AP,
    RTHW_MODE_STA_AP,
    RTHW_MODE_PROMISC,
    RTHW_MODE_P2P
} rthw_mode_t;

int rthw_mhd_init(struct rt_wlan_device *wlan);

rthw_mode_t rthw_wifi_mode_get(void);
int rthw_wifi_stop(void);
int rthw_wifi_start(rthw_mode_t mode);
int rthw_wifi_connect(char *ssid, int ssid_len, char *password, int pass_len, int security_type);
int rthw_wifi_connect_bssid(char *bssid, char *ssid, int ssid_len, char *password, int pass_len, int security_type, int channel);
int rthw_wifi_ap_start(char *ssid, char *password, int channel);
int rthw_wifi_rssi_get(void);
void rthw_wifi_channel_set(int interface, int channel);
int rthw_wifi_channel_get(int interface);
int rthw_wifi_sta_disconnect(void);
int rthw_wifi_ap_disconnect(void);
int rthw_wifi_scan(void *content, struct rt_scan_info *info);
int rthw_wifi_scan_stop();
void rthw_wifi_monitor_enable(int enable);
int rthw_wifi_get_powersave_mode();
int rthw_wifi_set_powersave_enable(int enable);
void rthw_wifi_country_set(rt_uint32_t country_code);
rt_uint32_t rthw_wifi_country_get(void);
int rthw_wifi_mac_set(rt_uint8_t mac[], int interface);
int rthw_wifi_mac_get(rt_uint8_t mac[], int interface);
int rthw_wifi_send(void *buff, int interface);

#endif
