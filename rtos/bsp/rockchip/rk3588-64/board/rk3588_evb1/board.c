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

#ifdef RT_USING_AUDIO
#include "rk_audio.h"
#endif

#ifdef RT_USING_GMAC
#include "drv_gmac.h"
#endif

#ifdef RT_USING_AUDIO
const struct audio_card_desc rk_board_audio_cards[] =
{
#ifdef RT_USING_AUDIO_CARD_I2S0_8CH
    {
        .name = "tdm0",
        .dai =  I2STDM0,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_TDM_I2S_HALF_FRAME,
        .trcm_mode = TRCM_TXONLY,
        .mclkfs = 256,
        .txMap = 0x3210,
    },
#endif
#ifdef RT_USING_AUDIO_CARD_I2S1_8CH
    {
        .name = "tdm1",
        .dai =  I2STDM1,
        .codec = NULL,
        .codec_master = HAL_FALSE,
        .clk_invert = HAL_FALSE,
        .playback = HAL_TRUE,
        .capture = HAL_TRUE,
        .format = AUDIO_FMT_TDM_I2S_HALF_FRAME,
        .trcm_mode = TRCM_TXONLY,
        .mclkfs = 256,
        .txMap = 0x3210,
    },
#endif
    { /* sentinel */ }
};
#endif

#ifdef RT_USING_GMAC
RT_WEAK const struct rockchip_eth_config rockchip_eth_config_table[] =
{
#ifdef RT_USING_GMAC0
    {
        .id = GMAC0,
        .mode = RGMII_MODE,
        .phy_addr = 1,

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

        .reset_gpio_bank = GPIO2,
        .reset_gpio_num = GPIO_PIN_D1,
        .reset_delay_ms = {0, 20, 100},

        .tx_delay = 0x4f,
        .rx_delay = 0x26,
    },
#endif
};
#endif
