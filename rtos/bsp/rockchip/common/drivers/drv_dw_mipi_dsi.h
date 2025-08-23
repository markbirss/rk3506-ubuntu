/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_dw_mipi_dsi.h
  * @author  Homgming Zou
  * @date    13-Sep-2024
  * @brief   dw mipi dsi driver
  *
  ******************************************************************************
  */

#ifndef _DRV_DW_MIPI_DSI_H_
#define _DRV_DW_MIPI_DSI_H_

/*******************************************************************************
 * Included Files
 ******************************************************************************/

#include "drv_display.h"

#ifdef RT_USING_DW_MIPI_DSI

#include "hal_base.h"

/*******************************************************************************
 * Pre-processor Definitions
 ******************************************************************************/

/*******************************************************************************
 * Public Types
 ******************************************************************************/

/*******************************************************************************
 * Public Data
 ******************************************************************************/
struct dw_mipi_dsi
{
    struct udevice *dev;
    struct dw_mipi_dsi *master;
    struct dw_mipi_dsi *slave;
    struct DISPLAY_MODE_INFO mode;
    struct display_state *state;
    bool data_swap;
    bool dual_channel;
    bool disable_hold_mode;
    bool prepared;
    int id;
    struct MIPI_DSI_REG *base;
    struct GRF_REG *grf;
    uint32_t channel;
    uint8_t lanes;
    uint16_t format;
    uint32_t mode_flags;
    unsigned int lane_mbps; /* per lane */
};
/*******************************************************************************
 * Inline Functions
 ******************************************************************************/

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

#endif
#endif /* _DRV_DW_MIPI_DSI_H_ */
