/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <drm/connector.h>

void connector_pre_enable(struct connector *connector)
{
	if (connector->funcs->pre_enable)
		connector->funcs->pre_enable(connector);

	if (connector->panel && connector->panel->funcs->init)
		connector->panel->funcs->init(connector->port, connector->panel);

	if (connector->panel && connector->panel->funcs->prepare)
		connector->panel->funcs->prepare(connector->port, connector->panel);

	if (connector->panel && connector->panel->init_seq && connector->funcs->send_cmds)
		connector->funcs->send_cmds(connector, connector->panel->init_seq);
}

void connector_enable(struct connector *connector)
{
	if (connector->funcs->enable)
		connector->funcs->enable(connector);

	if (connector->panel && connector->panel->funcs->enable)
		connector->panel->funcs->enable(connector->port, connector->panel);
}

void connector_disable(struct connector *connector)
{
	if (connector->panel && connector->panel->funcs->disable)
		connector->panel->funcs->disable(connector->port, connector->panel);

	if (connector->funcs->disable)
		connector->funcs->disable(connector);
}

void connector_post_disable(struct connector *connector)
{
	if (connector->panel && connector->panel->funcs->unprepare)
		connector->panel->funcs->unprepare(connector->port, connector->panel);
}
