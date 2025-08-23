/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-12     Steven Liu   first implementation
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"

#ifdef RT_USING_GMAC
#include "drv_gmac.h"
#endif


#ifdef RT_USING_GMAC
const struct rockchip_eth_config rockchip_eth_config_table[] =
{
#ifdef RT_USING_GMAC0
    {
        .id = GMAC0,
        .mode = RGMII_MODE,
        .phy_addr = 1,

        .external_clk = false,

        .reset_gpio_bank = GPIO2,
        .reset_gpio_num = GPIO_PIN_D3,
        .reset_delay_ms = {0, 20, 100},

        .tx_delay = 0x3C,
        .rx_delay = 0x2f,
    },
#endif

#ifdef RT_USING_GMAC1
    {
        .id = GMAC1,
        .mode = RGMII_MODE,
        .phy_addr = 1,

        .external_clk = false,

        .reset_gpio_bank = GPIO2,
        .reset_gpio_num = GPIO_PIN_D1,
        .reset_delay_ms = {0, 20, 100},

        .tx_delay = 0x4f,
        .rx_delay = 0x26,
    },
#endif
};
#endif
