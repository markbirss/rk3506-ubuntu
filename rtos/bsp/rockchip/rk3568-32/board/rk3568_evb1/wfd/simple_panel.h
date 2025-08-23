/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    rk3568_evb1.c
  * @version V0.1
  * @brief   wfd simple panel driver
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-08-15     Damon Ding      first implementation
  *
  ******************************************************************************
  */

#ifndef __SIMPLE_PANEL_H__
#define __SIMPLE_PANEL_H__

#include <drm/panel.h>
#include <drm/drm_util.h>

#include "pwm_bl.h"

struct simple_panel_control
{
    struct panel_control_pin *enable_pin;
    struct panel_control_pin *reset_pin;
    struct panel_delay_config *delay_config;
    struct pwm_bl_config *bl_config;
    //TODO: panel_regulator
};

struct port_cfg
{
    struct wfdport_cfg base;
    bool initialized;
    bool enabled;
    bool prepared;
    int output_if;
    struct simple_panel_control panel_control;
};

static inline struct port_cfg *wfdport_cfg_to_port_cfg(struct wfdport_cfg *base)
{
    char *p = (char *)base - offsetof(struct port_cfg, base);
    return (struct port_cfg *)p;
}

int simple_panel_init(struct wfd_port *port, struct panel *panel);
int simple_panel_enable(struct wfd_port *port, struct panel *panel);
int simple_panel_disable(struct wfd_port *port, struct panel *panel);
int simple_panel_prepare(struct wfd_port *port, struct panel *panel);
int simple_panel_unprepare(struct wfd_port *port, struct panel *panel);

#endif
