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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <errno.h>
#include <wfd.h>
#include <wfd_rockchip.h>

#include "simple_panel.h"

int simple_panel_init(struct wfd_port *port, struct panel *panel)
{
    struct wfdport_cfg *wfdport_cfg = port->port_cfg;
    struct port_cfg *port_cfg = wfdport_cfg_to_port_cfg(port->port_cfg);
    const struct wfd_keyval *cfg_keyval;

    if (port_cfg->initialized)
    {
        printf("connector %d is already initialized\n", port_cfg->output_if);
        return 0;
    }

    cfg_keyval = wfdport_cfg_get_extension(wfdport_cfg, WFD_OPT_OUTPUT_IF);
    if (cfg_keyval)
    {
        port_cfg->output_if = cfg_keyval->i;
    }
    else
    {
        printf("failed to get connector id\n");
        return -EINVAL;
    }

    /* config enable/reset pins */
    cfg_keyval = wfdport_cfg_get_extension(wfdport_cfg, WFD_OPT_PANEL_ENABLE_PIN);
    if (cfg_keyval)
    {
        port_cfg->panel_control.enable_pin = (struct panel_control_pin *)cfg_keyval->p;
        if (port_cfg->panel_control.enable_pin)
            rt_pin_mode(port_cfg->panel_control.enable_pin->bank_pin, PIN_MODE_OUTPUT);
    }

    cfg_keyval = wfdport_cfg_get_extension(wfdport_cfg, WFD_OPT_PANEL_RESET_PIN);
    if (cfg_keyval)
    {
        port_cfg->panel_control.reset_pin = (struct panel_control_pin *)cfg_keyval->p;
        if (port_cfg->panel_control.reset_pin)
            rt_pin_mode(port_cfg->panel_control.reset_pin->bank_pin, PIN_MODE_OUTPUT);
    }

    cfg_keyval = wfdport_cfg_get_extension(wfdport_cfg, WFD_OPT_PANEL_DELAY_CONFIG);
    if (cfg_keyval)
    {
        port_cfg->panel_control.delay_config = (struct panel_delay_config *)cfg_keyval->p;
    }
    else
    {
        SLOG_ERR("failed to get panel delay configs\n");
        return -EINVAL;
    }

    cfg_keyval = wfdport_cfg_get_extension(wfdport_cfg, WFD_OPT_PANEL_BACKLIGHT);
    if (cfg_keyval)
    {
        port_cfg->panel_control.bl_config = (struct pwm_bl_config *)cfg_keyval->p;
    }
    else
    {
        SLOG_ERR("failed to get panel backlight config\n");
        return -EINVAL;
    }

    port_cfg->initialized = true;

    return 0;
}

int simple_panel_prepare(struct wfd_port *port, struct panel *panel)
{
    struct port_cfg *port_cfg = wfdport_cfg_to_port_cfg(port->port_cfg);

    if (port_cfg->prepared)
    {
        printf("connector %d is already prepared\n", port_cfg->output_if);
        return 0;
    }

    //TODO: enable power_supply

    if (port_cfg->panel_control.enable_pin)
        rt_pin_write(port_cfg->panel_control.enable_pin->bank_pin,
                     port_cfg->panel_control.enable_pin->active_state);

    mdelay(port_cfg->panel_control.delay_config->prepare);

    if (port_cfg->panel_control.reset_pin)
        rt_pin_write(port_cfg->panel_control.reset_pin->bank_pin,
                     port_cfg->panel_control.reset_pin->active_state);

    mdelay(port_cfg->panel_control.delay_config->reset);

    if (port_cfg->panel_control.reset_pin)
        rt_pin_write(port_cfg->panel_control.reset_pin->bank_pin,
                     !port_cfg->panel_control.reset_pin->active_state);

    mdelay(port_cfg->panel_control.delay_config->init);

    port_cfg->prepared = true;

    return 0;
}

int simple_panel_unprepare(struct wfd_port *port, struct panel *panel)
{
    struct port_cfg *port_cfg = wfdport_cfg_to_port_cfg(port->port_cfg);

    if (port_cfg->panel_control.reset_pin)
        rt_pin_write(port_cfg->panel_control.reset_pin->bank_pin,
                     port_cfg->panel_control.reset_pin->active_state);

    if (port_cfg->panel_control.enable_pin)
        rt_pin_write(port_cfg->panel_control.enable_pin->bank_pin,
                     !port_cfg->panel_control.enable_pin->active_state);

    //TODO: disable power_supply

    mdelay(port_cfg->panel_control.delay_config->unprepare);

    port_cfg->prepared = false;

    return 0;
}

int simple_panel_enable(struct wfd_port *port, struct panel *panel)
{
    struct port_cfg *port_cfg = wfdport_cfg_to_port_cfg(port->port_cfg);
    int ret = 0;

    if (port_cfg->enabled)
    {
        printf("connector %d is already enabled\n", port_cfg->output_if);
        return 0;
    }

    mdelay(port_cfg->panel_control.delay_config->enable);

    ret = pwm_bl_enable(port_cfg->panel_control.bl_config);
    if (ret)
    {
        SLOG_ERR("failed to enable backlight\n");
        return -EINVAL;
    }

    port_cfg->enabled = true;

    return 0;
}

int simple_panel_disable(struct wfd_port *port, struct panel *panel)
{
    struct port_cfg *port_cfg = wfdport_cfg_to_port_cfg(port->port_cfg);
    int ret = 0;

    ret = pwm_bl_disable(port_cfg->panel_control.bl_config);
    if (ret)
    {
        SLOG_ERR("failed to disable backlight\n");
        return -EINVAL;
    }

    port_cfg->enabled = false;

    mdelay(port_cfg->panel_control.delay_config->disable);

    return 0;
}
