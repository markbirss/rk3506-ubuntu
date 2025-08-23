/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __WFD_ROCKCHIP_H__
#define __WFD_ROCKCHIP_H__

#if defined(__QNXNTO__)
#include <sys/platform.h>
#endif
#include <stdint.h>
#include <rtthread.h>
#include <rthw.h>

#if defined(__RT_THREAD__)
#include "hal_base.h"

#define _BIT	HAL_BIT
#endif

struct wfdmode_timing {
	rt_uint32_t pixel_clock_kHz;
	rt_uint32_t hpixels;
	rt_uint32_t vlines;
	rt_uint16_t hsw;
	rt_uint16_t vsw;
	rt_uint16_t hfp;
	rt_uint16_t vfp;
	rt_uint16_t hbp;
	rt_uint16_t vbp;
	rt_uint32_t flags;
};

struct wfd_keyval {
	const char *key;
	long i;
	void *p;
};

struct mode {
	const struct wfdmode_timing timing;
	const struct wfd_keyval *ext_list;
};

struct wfddevice_cfg {
	const struct wfd_keyval *ext_list;
};

struct wfdport_cfg {
	int id;
	const struct wfd_keyval *ext_list;
	const struct mode *first_mode;
};

struct wfdmode_cfg_list {
	const struct wfdport_cfg *port;
	const struct wfd_keyval *ext_list;
};

enum wfdmode_flags {
	WFDMODE_INVERT_HSYNC = _BIT(0),
	WFDMODE_INVERT_VSYNC = _BIT(1),
	WFDMODE_INVERT_DATA_EN = _BIT(2),
	WFDMODE_INVERT_CLOCK = _BIT(3),
	WFDMODE_INVERT_DATA = _BIT(4),
	WFDMODE_INVERT_HV_SYNC_RF = _BIT(5),
	WFDMODE_INTERLACE = _BIT(8),
	WFDMODE_DOUBLESCAN = _BIT(9),
	WFDMODE_DOUBLECLOCK = _BIT(10),
	WFDMODE_BOTTOM_FIRST = _BIT(11),
	WFDMODE_QUADCLOCK = _BIT(12),
	WFDMODE_PREFERRED = _BIT(31),
};

int wfddevice_cfg_create(struct wfddevice_cfg **device, int deviceid, const struct wfd_keyval *opts);
const struct wfd_keyval *wfddevice_cfg_get_extension(const struct wfddevice_cfg *device, const char *key);
void wfddevice_cfg_destroy(struct wfddevice_cfg *device);
int wfdport_cfg_create(struct wfdport_cfg **port, const struct wfddevice_cfg *device, int portid,
		       const struct wfd_keyval *opts);
const struct wfd_keyval *wfdport_cfg_get_extension(const struct wfdport_cfg *port, const char *key);
void wfdport_cfg_destroy(struct wfdport_cfg *port);
int wfdmode_cfg_list_create(struct wfdmode_cfg_list **list, const struct wfdport_cfg *port,
			    const struct wfd_keyval *opts);
const struct wfd_keyval *wfdmode_cfg_list_get_extension(const struct wfdmode_cfg_list *list,
							const char *key);
void wfdmode_cfg_list_destroy(struct wfdmode_cfg_list *list);
const struct wfdmode_timing *wfdmode_cfg_list_get_next(const struct wfdmode_cfg_list *list,
						       const struct wfdmode_timing *prev_timing);
const struct wfd_keyval *wfdmode_cfg_get_extension(const struct wfdmode_timing *mode, const char* key);

#define WFD_OPT_OUTPUT_IF			"output_if"

#define WFD_OPT_VIDEO_PORT_INDEX		"video_port_index"
enum rockchip_video_port_index {
	ROCKCHIP_VP0,
	ROCKCHIP_VP1,
	ROCKCHIP_VP2,
	ROCKCHIP_VP3,
	ROCKCHIP_VP_MAX
};

#define WFD_OPT_PANEL_DELAY_CONFIG		"panel_delay_config"
struct panel_delay_config {
	/**
	 * @prepare: the time (in milliseconds) that it takes for the panel to
	 *           become ready and start receiving video data
	 * @enable: the time (in milliseconds) that it takes for the panel to
	 *          display the first valid frame after starting to receive
	 *          video data
	 * @disable: the time (in milliseconds) that it takes for the panel to
	 *           turn the display off (no content is visible)
	 * @unprepare: the time (in milliseconds) that it takes for the panel
	 *             to power itself down completely
	 * @reset: the time (in milliseconds) that it takes for the panel
	 *         to reset itself completely
	 * @init: the time (in milliseconds) that it takes for the panel to
	 *	  send init command sequence after reset deassert
	 */
	uint32_t prepare;
	uint32_t unprepare;
	uint32_t enable;
	uint32_t disable;
	uint32_t reset;
	uint32_t init;
};

#define WFD_OPT_PANEL_ENABLE_PIN		"panel_enable_pin"

#define WFD_OPT_PANEL_RESET_PIN			"panel_reset_pin"
enum panel_gpio_active_state {
	PANEL_GPIO_ACTIVE_LOW,
	PANEL_GPIO_ACTIVE_HIGH
};
struct panel_control_pin {
	uint32_t bank_id;
	uint32_t pin_id;
	uint32_t func_id;
	uint8_t  active_state;
	uint32_t bank_pin;
};

#define WFD_OPT_PANEL_BACKLIGHT			"panel_backlight"

#define WFD_OPT_WFD_PORT_NUMBER			"wfd_port_number"

#define WFD_OPT_WFD_PIPELINE_NUMBER		"wfd_pipeline_number"

#define WFD_OPT_WFD_PIPELINE_BINDING		"wfd_pipeline_binding"
struct wfd_pipeline_binding {
	uint8_t pipeline_num;
	uint8_t pipelines[VOP2_LAYER_MAX];
};

#endif
