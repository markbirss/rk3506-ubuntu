/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wfddev.h"
#include "drm/drm_util.h"

errno_t wfd_config_devices_get(WFDint **result_out)
{
	WFDint *dev_ids = NULL;

	dev_ids = calloc(2, sizeof(dev_ids[0]));
	if (!dev_ids)
		return ENOMEM;

	// Only support one wfd device
	// wfd device 1
	dev_ids[0] = 1;
	// the dev_ids should be ended with 0
	dev_ids[1] = 0;

	*result_out = dev_ids;

	return 0;
}

errno_t wfd_config_ports_get(struct wfd_device *dev, WFDint **result_out)
{
	const struct wfd_keyval *keyval;
	WFDint *port_ids = NULL;
	int i;

	keyval = wfddevice_cfg_get_extension(dev->device_cfg, WFD_OPT_WFD_PORT_NUMBER);
	if (keyval && keyval->i) {
		port_ids = calloc(keyval->i + 1, sizeof(port_ids[0]));
		if (!port_ids)
			return ENOMEM;

		for (i = 0; i < keyval->i; i++)
			port_ids[i] = i + 1;

		// the port_ids should be ended with 0
		port_ids[i] = 0;
	} else {
		slogf(SLOGC_SELF, _SLOG_ERROR, "failed to get port number");
		return EINVAL;
	}

	*result_out = port_ids;

	return 0;
}

errno_t wfd_config_pipelines_get(struct wfd_device *dev, WFDint **result_out)
{
	const struct wfd_keyval *keyval;
	WFDint *pipe_ids = NULL;
	int i;

	keyval = wfddevice_cfg_get_extension(dev->device_cfg, WFD_OPT_WFD_PIPELINE_NUMBER);
	if (keyval && keyval->i) {
		pipe_ids = calloc(keyval->i + 1, sizeof(pipe_ids[0]));
		if (!pipe_ids)
			return ENOMEM;

		for (i = 0; i < keyval->i; i++)
			pipe_ids[i] = i + 1;

		// the pipe_ids should be ended with 0
		pipe_ids[i] = 0;
	} else {
		slogf(SLOGC_SELF, _SLOG_ERROR, "failed to get pipeline number");
		return EINVAL;
	}

	*result_out = pipe_ids;

	return 0;
}

errno_t wfd_config_set_pipelines_binding(struct wfd_device *dev, struct wfd_pipe *pipe)
{
	const struct wfd_keyval *keyval;
	unsigned int *pipeline_binding;
	unsigned int binding, pipeline_id;
	int port_num, pipeline_num;
	int i, j;

	keyval = wfddevice_cfg_get_extension(dev->device_cfg, WFD_OPT_WFD_PORT_NUMBER);
	if (keyval && keyval->i) {
		port_num = keyval->i;
	} else {
		slogf(SLOGC_SELF, _SLOG_ERROR, "failed to get port number");
		return EINVAL;
	}

	keyval = wfddevice_cfg_get_extension(dev->device_cfg, WFD_OPT_WFD_PIPELINE_BINDING);
	if (keyval && keyval->p) {
		// one pipeline can only be bound to one port
		pipe->bindable_ports = calloc(1, sizeof(pipe->bindable_ports[0]));
		if (!pipe->bindable_ports)
			return ENOMEM;

		pipe->bindable_ports_to_advertise = calloc(1, sizeof(pipe->bindable_ports_to_advertise[0]));
		if (!pipe->bindable_ports)
			return ENOMEM;

		// the start index of port and pipeline is 1
		pipeline_binding = (unsigned int *)keyval->p;
		for (i = 0; i < port_num; i++) {
			binding = *pipeline_binding;
			pipeline_num = hweight32(binding);
			for (j = 0; j < pipeline_num; j++) {
				pipeline_id = __rt_ffs(binding) - 1;
				if (pipeline_id == pipe->wfd_pipe_id - 1) {
					pipe->bindable_ports[0] = i + 1;
					pipe->bindable_ports_to_advertise[0] = i + 1;
					break;
				}
				binding &= ~BIT(pipeline_id);
			}
			if (pipe->bindable_ports[0] || pipe->bindable_ports_to_advertise[0])
				break;
			pipeline_binding++;
		}
	} else {
		slogf(SLOGC_SELF, _SLOG_ERROR, "failed to get pipeline number");
		return EINVAL;
	}

	return 0;
}

_Bool wfd_config_find_id_in_array(const WFDint *array, WFDint id)
{
	while (array[0]) {
		if (array[0] == id)
			return true;
		++array;
	}

	return false;
}
