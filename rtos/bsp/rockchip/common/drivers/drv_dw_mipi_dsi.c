/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-1.0
  ******************************************************************************
  * @file    drv_dw_mipi_dsi.c
  * @version V0.1
  * @brief   display mipi dsi driver for rocklchip
  *
  * Change Logs:
  * Date           Author           Notes
  * 2024-08-29     Homgming Zou     first implementation
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup DW_MIPI_DSI
 *  @{
 */

/** @defgroup DW_MIPI_DSI_How_To_Use How To Use
 *  @{

 The DW MIPI DSI driver is used to initialize the DSI controller and provides APIs to send commands
 to communicate with the DPHY and panel.

 To utilize the DW MIPI DSI driver, follow these steps:

 - 1) Initialize the DSI controller using dw_mipi_dsi_init().
 - 2) Prepare the DSI controller and PHY with dw_mipi_dsi_prepare().
 - 3) Enable the DSI controller via dw_mipi_dsi_enable().
 - 4) Perform necessary operations such as sending commands to the DPHY and panel using dw_mipi_dsi_transfer().
 - 5) Disable the DSI controller when done with dw_mipi_dsi_disable().

 @} */

#include <rthw.h>
#ifdef RT_USING_DW_MIPI_DSI
#include "drv_display.h"
#include "drv_dw_mipi_dsi.h"

static int mipi_dphy_power_on(struct dw_mipi_dsi *dsi)
{
    HAL_DW_MIPI_DSI_PhyPowerOn(dsi->base);

    dsi->state->phy_state.funcs->power_on(dsi->state);

    return 0;
}

static void dw_mipi_dsi_set_hs_clk(struct dw_mipi_dsi *dsi, unsigned long rate)
{
    rate = dsi->state->phy_state.funcs->set_pll(dsi->state, rate);
    dsi->lane_mbps = rate / 1000 / 1000;
}

static void dw_mipi_dsi_post_disable(struct dw_mipi_dsi *dsi)
{
    if (!dsi->prepared)
        return;

    if (dsi->master)
        dw_mipi_dsi_post_disable(dsi->master);

    HAL_DW_MIPI_DSI_PhyPowerOff(dsi->base);

    dsi->state->phy_state.funcs->power_off(dsi->state);

    dsi->prepared = false;

    if (dsi->slave)
        dw_mipi_dsi_post_disable(dsi->slave);
}

static void dw_mipi_dsi_host_init(struct dw_mipi_dsi *dsi)
{
    HAL_DW_MIPI_DSI_Init(dsi->base, dsi->lane_mbps);
    HAL_DW_MIPI_DSI_DpiConfig(dsi->base, &dsi->mode, dsi->format);
    HAL_DW_MIPI_DSI_PacketHandlerConfig(dsi->base, &dsi->mode);
    HAL_DW_MIPI_DSI_ModeConfig(dsi->base, &dsi->mode);
    HAL_DW_MIPI_DSI_LineTimerConfig(dsi->base, dsi->lane_mbps, &dsi->mode);
    HAL_DW_MIPI_DSI_VerticalTimingConfig(dsi->base, &dsi->mode);
    HAL_DW_MIPI_DSI_DphyTimingConfig(dsi->base, &dsi->mode, RT_HW_LCD_DSI_LANES);
    HAL_DW_MIPI_DSI_ClrErr(dsi->base);
}

static void dw_mipi_dsi_pre_enable(struct dw_mipi_dsi *dsi)
{
    if (dsi->prepared)
        return;

    if (dsi->master)
        dw_mipi_dsi_pre_enable(dsi->master);

    dw_mipi_dsi_host_init(dsi);
    HAL_DW_MIPI_DSI_PhyInit(dsi->base);
    mipi_dphy_power_on(dsi);

    dsi->prepared = true;

    if (dsi->slave)
        dw_mipi_dsi_pre_enable(dsi->slave);
}

/********************* Private Function Definition ***************************/
/** @defgroup DSI_Private_Function Private Function
 *  @{
 */

/**
 * @brief  Dsi init for hardware.
 * @param  state: pointer to the display state structure.
 */
static void rockchip_dsi_init(struct display_state *state)
{
    struct connector_state *conn_state = &state->conn_state;
    struct dw_mipi_dsi *dsi;

    dsi = rt_calloc(1, sizeof(*dsi));
    dsi->format = RT_HW_LCD_BUS_FORMAT;
    dsi->lanes = RT_HW_LCD_DSI_LANES;
    dsi->channel = RT_HW_LCD_DSI_CHANNEL;
    dsi->lane_mbps = RT_HW_LCD_LANE_MBPS;
    dsi->base = MIPI_DSI;
    dsi->grf = GRF;
    dsi->mode_flags = RT_HW_LCD_VMODE_FLAG;
    dsi->state = state;
    conn_state->private = dsi;
    state->phy_state.funcs->init(state);
}

/**
 * @brief  Dsi deinit for hardware.
 * @param  state: pointer to the display state structure.
 */
static void rockchip_dsi_deinit(struct display_state *state)
{
}

/**
 * @brief  Dsi controller and phy prepare.
 * @param  state: pointer to the display state structure.
 */
static void rockchip_dsi_prepare(struct display_state *state)
{
    struct connector_state *conn_state = &state->conn_state;
    struct dw_mipi_dsi *dsi = conn_state->private;
    unsigned long lane_rate;

    memcpy(&dsi->mode, &state->mode, sizeof(struct DISPLAY_MODE_INFO));
    lane_rate = dsi->lane_mbps * 1000000;
    dw_mipi_dsi_set_hs_clk(dsi, lane_rate);
    rt_kprintf("final DSI-Link bandwidth: %u Mbps x %d\n", dsi->lane_mbps, dsi->lanes);

    dw_mipi_dsi_pre_enable(dsi);
}

/**
 * @brief  Dsi controller and phy unprepare.
 * @param  state: pointer to the display state structure.
 */
static void rockchip_dsi_unprepare(struct display_state *state)
{
    struct connector_state *conn_state = &state->conn_state;
    struct dw_mipi_dsi *dsi = conn_state->private;

    dw_mipi_dsi_post_disable(dsi);
}

/**
 * @brief  DSI controller enable.
 * @param  state: pointer to the display state structure.
 */
static void rockchip_dsi_enable(struct display_state *state)
{
    struct connector_state *conn_state = &state->conn_state;
    struct dw_mipi_dsi *dsi = conn_state->private;;
    HAL_DW_MIPI_DSI_Enable(dsi->base, &dsi->mode);
}

/**
 * @brief  DSI controller disable.
 * @param  state: pointer to the display state structure.
 */
static void rockchip_dsi_disable(struct display_state *state)
{
    struct connector_state *conn_state = &state->conn_state;
    struct dw_mipi_dsi *dsi = conn_state->private;

    HAL_DW_MIPI_DSI_Disable(dsi->base, &dsi->mode);
}

/**
 * @brief  Message transfer between dsi and panel.
 * @param  state: pointer to the display state structure.
 * @param  desc: transfer message.
 * return rt_err_t.
 */
static rt_err_t rockchip_dsi_transfer(struct display_state *state, struct rockchip_cmd_desc *desc)
{
    struct connector_state *conn_state = &state->conn_state;
    struct dw_mipi_dsi *dsi = conn_state->private;
    struct DISPLAY_MODE_INFO *mode = &state->mode;

    HAL_Status ret;

    if (mode->flags & DSI_MODE_LPM)
        HAL_DW_MIPI_DSI_MsgLpModeConfig(dsi->base);

    ret = HAL_DW_MIPI_DSI_SendPacket(dsi->base, desc->header.data_type,
                                     desc->header.payload_length, desc->payload);

    if (ret == HAL_OK)
        return RT_EOK;
    else
        return -RT_ETIMEOUT;
}

/** @} */

/** @defgroup DSI_Public_Functions Public Functions
 *  @{
 */

/**
 * @brief  DSI control functions.
 * @param  init: Init DSI hardware config.
 * @param  deinit: DSI deinit.
 * @param  prepare: Prepare DSI.
 * @param  enable: Enable DSI.
 * @param  disable: Disable DSI.
 * @param  transfer: Transfer message.
 */
const struct rockchip_connector_funcs rockchip_dsi_funcs =
{
    .init = rockchip_dsi_init,
    .deinit = rockchip_dsi_deinit,
    .prepare = rockchip_dsi_prepare,
    .unprepare = rockchip_dsi_unprepare,
    .enable = rockchip_dsi_enable,
    .disable = rockchip_dsi_disable,
    .transfer = rockchip_dsi_transfer,
};

/** @} */

#endif

/** @} */

/** @} */
