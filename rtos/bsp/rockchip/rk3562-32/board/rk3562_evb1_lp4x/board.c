/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-09     Cliff Chen   first implementation
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
        .max_speed = 1000,
        .phy_addr = 0,

        .external_clk = false,

        .reset_gpio_bank = GPIO3,
        .reset_gpio_num = GPIO_PIN_A0,
        .reset_delay_ms = {0, 40, 200},

        .tx_delay = 0x3C,
        .rx_delay = 0,
    },
#endif
};
#endif
