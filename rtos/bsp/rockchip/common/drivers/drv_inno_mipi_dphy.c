/**
  * Copyright (c) 2024 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_inno_mipi_dphy.c
  * @version V0.0.1
  * @brief   display mipi dphy driver for rocklchip
  *
  * Change Logs:
  * Date           Author            Notes
  * 2024-09-09     Homgming Zou      first implementation
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup INNO_MIPI_DPHY
 *  @{
 */

/** @defgroup INNO_MIPI_DPHY_How_To_Use How To Use
 *  @{

 The INNO MIPI DPHY driver offers a set of APIs to configure the MIPI DPHY, including the following functions:

    - 1) Initialize INNO MIPI DPHY hardware configuration using init_inno_mipi_dphy().
    - 2) Power on the INNO MIPI DPHY with power_on_inno_mipi_dphy().
    - 3) Power off the INNO MIPI DPHY with power_off_inno_mipi_dphy().
    - 4) Set the PLL for the INNO MIPI DPHY using set_inno_mipi_dphy_pll().

 @} */

#include <rthw.h>

#ifdef RT_USING_INNO_MIPI_DPHY
#include "drv_display.h"
#include "drv_inno_mipi_dphy.h"

static unsigned long inno_mipi_dphy_pll_round_rate(unsigned long fin,
        unsigned long fout, uint8_t *prediv, uint16_t *fbdiv)
{
    unsigned long best_freq = 0;
    uint8_t min_prediv, max_prediv;
    uint8_t _prediv, best_prediv = 1;
    uint16_t _fbdiv, best_fbdiv = 1;
    uint32_t min_delta = 0xffffffff;
    uint32_t tmp;
    uint32_t delta;

    fout = fout * 2 / 1000000;
    fin = fin / 1000000;

    min_prediv = HAL_DIV_ROUND_UP(fin, 40);
    max_prediv = fin / 5;
    for (_prediv = min_prediv; _prediv <= max_prediv; _prediv++)
    {
        _fbdiv = fout * _prediv / fin;

        if ((_fbdiv == 15) || (_fbdiv < 12) || (_fbdiv > 511))
            continue;

        tmp = _fbdiv * fin / _prediv;
        delta = abs(fout - tmp);

        if (delta < min_delta)
        {
            best_prediv = _prediv;
            best_fbdiv = _fbdiv;
            min_delta = delta;
            best_freq = tmp * 1000000;
        }
    }

    if (best_freq)
    {
        *prediv = best_prediv;
        *fbdiv = best_fbdiv;
    }

    return best_freq / 2;
}

/********************* Private Function Definition ***************************/
/** @defgroup INNO_MIPI_DPHY_Private_Function Private Function
 *  @{
 */

/**
 * @brief Powers on the INNO MIPI DPHY.
 * @param state: pointer to the display state structure
 * @return 0 on success
 */
static int inno_mipi_dphy_power_on(struct display_state *state)
{
    struct phy_state *phy_state = &state->phy_state;
    struct dphy_state *dphy_state = phy_state->private;

    HAL_INNO_MIPI_DPHY_BgpdEnable(dphy_state->reg);
    HAL_INNO_MIPI_DPHY_DaPwrokEnable(dphy_state->reg);
    HAL_INNO_MIPI_DPHY_PllLdoEnable(dphy_state->reg);
    HAL_INNO_MIPI_DPHY_LaneEnable(dphy_state->reg, dphy_state->lanes);
    HAL_INNO_MIPI_DPHY_Reset(dphy_state->reg);
    HAL_INNO_MIPI_DPHY_TimingInit(dphy_state->reg, dphy_state->lane_mbps,
                                  dphy_state->lanes, dphy_state->soc_type);
    HAL_DelayUs(1);

    return 0;
}

/**
 * @brief Powers off the INNO MIPI DPHY.
 * @param state: Pointer to the display state structure
 * @return 0 on success
 */
static int inno_mipi_dphy_power_off(struct display_state *state)
{
    struct phy_state *phy_state = &state->phy_state;
    struct dphy_state *dphy_state = phy_state->private;

    HAL_INNO_MIPI_DPHY_LaneDisable(dphy_state->reg, dphy_state->lanes);
    HAL_INNO_MIPI_DPHY_PllLdoDisable(dphy_state->reg);
    HAL_INNO_MIPI_DPHY_DaPwrokDisable(dphy_state->reg);
    HAL_INNO_MIPI_DPHY_BgpdDisable(dphy_state->reg);

    return 0;
}

/**
 * @brief Sets the PLL for the INNO MIPI DPHY.
 * @param state: Pointer to the display state structure
 * @param rate: Desired PLL rate
 * @return output frequency after setting PLL
 */
static unsigned long inno_mipi_dphy_set_pll(struct display_state *state, unsigned long rate)
{
    struct phy_state *phy_state = &state->phy_state;
    struct dphy_state *dphy_state = phy_state->private;
    unsigned long fin, fout;
    uint16_t fbdiv = 0;
    uint8_t prediv = 0;

    fin = 24000000;
    fout = inno_mipi_dphy_pll_round_rate(fin, rate, &prediv, &fbdiv);

    rt_kprintf("%s: fin=%lu, fout=%lu, prediv=%u, fbdiv=%u\n",
               __func__, fin, fout, prediv, fbdiv);
    HAL_INNO_MIPI_DPHY_SetPll(dphy_state->reg, fbdiv, prediv, dphy_state->soc_type);
    dphy_state->lane_mbps = fout / 1000000;

    return fout;
}

/**
 * @brief Initializes the INNO MIPI DPHY.
 * @param state: Pointer to the display state structure
 * @return 0 on success
 */
static int inno_mipi_dphy_init(struct display_state *state)
{
    struct phy_state *phy_state = &state->phy_state;
    struct dphy_state *dphy_state;

    dphy_state = rt_calloc(1, sizeof(*dphy_state));
    dphy_state->hw_base = MIPI_TX_PHY_BASE;
    dphy_state->reg = MIPI_TX_PHY;
    dphy_state->lanes = RT_HW_LCD_DSI_LANES;
    dphy_state->lane_mbps = RT_HW_LCD_LANE_MBPS;
#if defined(SOC_RK3506)
    dphy_state->soc_type = RK3506_INNO_MIPI_DPHY;
#endif
    phy_state->private = dphy_state;

    return 0;
}

/** @defgroup INNO_MIPI_DPHY_Public_Functions Public Functions
 *  @{
 */

/**
 * @brief  INNO MIPI DPHY control functions.
 * @param  init: Init INNO MIPI DPHY hardware config.
 * @param  power_on: Power on INNO MIPI DPHY.
 * @param  power_off: Power off INNO MIPI DPHY.
 * @param  set_pll: Set INNO MIPI DPHY PLL.
 */
const struct rockchip_phy_funcs rockchip_dphy_funcs =
{
    .init = inno_mipi_dphy_init,
    .power_on = inno_mipi_dphy_power_on,
    .power_off = inno_mipi_dphy_power_off,
    .set_pll = inno_mipi_dphy_set_pll,
};

/** @} */

#endif

/** @} */

/** @} */
