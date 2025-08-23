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
RT_WEAK const struct rockchip_eth_config rockchip_eth_config_table[] =
{
#ifdef RT_USING_GMAC0
    {
        .id = GMAC0,
        .mode = RMII_MODE,
        .phy_addr = 1,

        .reset_gpio_bank = GPIO0,
        .reset_gpio_num = GPIO_PIN_C2,
        .reset_delay_ms = {0, 20, 100},
    },
#endif
};
#endif
