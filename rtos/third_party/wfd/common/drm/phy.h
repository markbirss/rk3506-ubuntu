/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __PHY_H__
#define __PHY_H__

#include <drm/drm_util.h>
#include <drm/phy-mipi-dphy.h>

struct connector;
struct phy_funcs;

enum phy_mode {
	PHY_MODE_INVALID,
	PHY_MODE_DP,
	PHY_MODE_MIPI_DPHY,
};

struct phy_configure_opts_dp {
	unsigned int link_rate;
	unsigned int lanes;
	unsigned int voltage[4];
	unsigned int pre[4];
	u8 ssc : 1;
	u8 set_rate : 1;
	u8 set_lanes : 1;
	u8 set_voltages : 1;
};

union phy_configure_opts {
	struct phy_configure_opts_dp dp;
	struct phy_configure_opts_mipi_dphy mipi_dphy;
};

struct phy_attrs {
	u32 bus_width;
	u32 max_link_rate;
	enum phy_mode mode;
};

struct phy {
	unsigned long id;
	struct phy_attrs attrs;

	struct connector *connector;
	void *private;
	const struct phy_funcs *funcs;
};

struct phy_funcs {
	int (*power_on)(struct phy *phy);
	int (*power_off)(struct phy *phy);
	int (*set_mode)(struct phy *phy, enum phy_mode mode, int submode);
	int (*configure)(struct phy *phy, union phy_configure_opts *opts);
	unsigned long (*set_pll)(struct phy *phy, unsigned long rate);
	int (*set_bus_width)(struct phy *phy, u32 bus_width);
};

struct phy *phy_create(struct connector *connector, uint32_t id);
void phy_destroy(struct phy *phy);
int phy_power_on(struct phy *phy);
int phy_power_off(struct phy *phy);
int phy_set_mode_ext(struct phy *phy, enum phy_mode mode, int submode);
#define phy_set_mode(phy, mode) \
	phy_set_mode_ext(phy, mode, 0)
int phy_configure(struct phy *phy, union phy_configure_opts *opts);
unsigned long phy_set_pll(struct phy *phy, unsigned long rate);
int phy_set_bus_width(struct phy *phy, u32 bus_width);

static inline enum phy_mode phy_get_mode(struct phy *phy)
{
	return phy->attrs.mode;
}
#endif
