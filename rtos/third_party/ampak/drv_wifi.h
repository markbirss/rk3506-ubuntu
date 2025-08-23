/*
 * File      : drv_wifi.h
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


#ifndef __DRV_WIFI_H__
#define __DRV_WIFI_H__

#include <netif/ethernetif.h>

#define MAX_ADDR_LEN          (6)

typedef enum
{
    STA_INTERFACE = 0,
    AP_INTERFACE  = 1,
    MAX_INTERFACE
} wifi_interface_t;

struct rt_mhd_wifi
{
#ifdef RT_USING_WIFI
    struct rt_wlan_device *wlan;
#else
    struct eth_device ethif;
#endif
    rt_uint8_t dev_addr[MAX_ADDR_LEN];
    //rt_country_code_t country;
    rt_uint32_t country;
    rt_uint8_t flag;
    int connected;
    int type;
};

int rthw_wifi_low_init(void);
void rthw_wlan_monitor_callback(void *buf, int interface);

#endif /* __DRV_WIFI_H__ */

