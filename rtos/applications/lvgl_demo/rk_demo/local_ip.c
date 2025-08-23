/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include <rtthread.h>
#include <lwip/netif.h>

char *get_local_ip(void)
{
    char *ip_out = NULL;
    struct netif *netif_temp = netif_list;

    while (netif_temp != NULL)
    {
        if (netif_is_up(netif_temp))
        {
            const ip_addr_t *ipaddr = &(netif_temp->ip_addr);

            if (ipaddr != NULL)
            {
                if (!ip_addr_isany(ipaddr))
                {
                    char ip_str[16];
                    ip4addr_ntoa_r(ipaddr, ip_str, 16);

                    ip_out = rt_malloc(20);
                    snprintf(ip_out, 20, "IP:%s", ip_str);
                    break;
                }
            }
        }
        netif_temp = netif_temp->next;
    }

    return ip_out;
}
