/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include <drm/drm_util.h>
#include <drm/phy.h>
#include <drm/panel.h>

struct connector_funcs;

enum connector_hpd_type {
	CONNECTOR_FORCE_HPD,
	CONNECTOR_GPIO_HPD,
	CONNECTOR_IOMUX_HPD,
};

struct connector_hpd_config {
	uint32_t hpd_type;
	uint32_t bank_id;
	uint32_t pin_id;
	uint32_t func_id;
};

struct connector_phy_config {
	uint32_t lanes;
	bool flip;
};

struct connector_ext_config {
	struct connector_hpd_config hpd_config;
	struct connector_phy_config phy_config;
};

struct connector {
	uint32_t connector_type;
	uint32_t id;
	uint32_t output_mode;
	struct wfd_port *port;
	struct phy *phy;
	void *private;
	const struct connector_funcs *funcs;
	struct panel *panel;
	struct connector_ext_config ext_config;
	u32 bus_format;
	u8 bpc;
	bool split_mode;
};

struct connector_funcs {
	void (*pre_enable)(struct connector *connector);
	void (*enable)(struct connector *connector);
	void (*disable)(struct connector *connector);
	void (*post_disable)(struct connector *connector);
	void (*send_cmds)(struct connector *connector, const struct panel_cmd_seq *panel_init_seq);
};

void connector_pre_enable(struct connector *connector);
void connector_enable(struct connector *connector);
void connector_disable(struct connector *connector);
void connector_post_disable(struct connector *connector);

#endif
