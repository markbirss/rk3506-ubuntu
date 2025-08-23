/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __PANEL_H__
#define __PANEL_H__

#define MAX_CMD_CNT 				1024
#define MAX_CMD_LEN				5

#include "wfddev.h"

struct panel_cmd_seq {
	uint16_t cmd_cnt;
	uint8_t (*cmd_sequence)[MAX_CMD_LEN];
};

struct panel_funcs;

struct panel {
	const struct panel_funcs *funcs;
	const struct panel_cmd_seq *init_seq;
	const struct panel_cmd_seq *exit_seq;
	const struct wfd_keyval *ext_list;
	void *private_data;
	uint32_t bus_format;
	uint8_t bpc;
};

/**
 *
 * Typical order of function calls:
 * init() -> prepare() -> enable() -> ... -> disable() -> unprepare()
 *
 */
struct panel_funcs {
	/**
	 * @init:
	 *
	 * Get wfd port configuration list and initialize the panel structure.
	 *
	 */
	int (*init)(struct wfd_port *port, struct panel *panel);

	/**
	 * @prepare:
	 *
	 * Turn on panel and perform set up (set control io to active, etc.).
	 *
	 */
	int (*prepare)(struct wfd_port *port, struct panel *panel);

	/**
	 * @unprepare:
	 *
	 * Turn off panel (set control io to inactive, etc.).
	 *
	 */
	int (*unprepare)(struct wfd_port *port, struct panel *panel);

	/**
	 * @enable:
	 *
	 * Enable panel (turn on back light, etc.).
	 *
	 */
	int (*enable)(struct wfd_port *port, struct panel *panel);

	/**
	 * @disable:
	 *
	 * Disable panel (turn off back light, etc.).
	 *
	 */
	int (*disable)(struct wfd_port *port, struct panel *panel);
};

#endif
