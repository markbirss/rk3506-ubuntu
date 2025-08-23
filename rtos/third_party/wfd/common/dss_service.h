/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __DSS_SERVICE_H__
#define __DSS_SERVICE_H__

#include "wfddev.h"
#include <drm/drm_util.h>
#include <fcntl.h>
#if defined(__QNXNTO__)
#include <devctl.h>
#endif

#define ROCKCHIP_MAX_CONNECTOR    16
#define ROCKCHIP_MAX_CRTC	4

struct wfd_dev_data {
	WFDDevice handle;
	WFDint wfd_dev_id;
	uint32_t debug_mask;
};

struct wfd_port_data {
	WFDint wfd_port_id;
	unsigned index;
	uint32_t vp_id;
};

struct wfd_mode_data {
	WFDint wfd_port_id;
	int count_modes;
	struct wfdmode_timing vm;
};

struct wfd_modeset_data {
	WFDint wfd_port_id;
	struct wfdmode_timing timging;
};

struct wfd_commit_data {
	WFDint wfd_port_id;
	uint64_t port_idx_mask, pipe_idx_mask;
};

struct wfd_plane_mask_data {
	int nr_ports;
	unsigned long plane_mask[ROCKCHIP_MAX_CRTC];
};

struct wfd_plane_state {
	WFDint wfd_port_id;
	WFDint wfd_pipe_id;

	/*
	 * one frame time
	 */
	uint64_t period_ns;
	/*
	 * condvar for vsync
	 */
	struct timespec start_ts, end_ts;

	WFDint zorder;
	struct drm_rect dst;
	struct drm_rect src;
	int wfd_format;
	unsigned global_alpha;
	uint32_t yrgb_mst;
	uint32_t uv_mst;
	uint32_t fb_size;
	struct drm_framebuffer fb;
};

struct wfd_pipe_data {
	WFDint wfd_pipe_id;
	unsigned index;
	WFDint zorder;
	char name[32];
};

#if defined(__RT_THREAD__)
#define _DCMD_MISC 0x00
#define __DIOTF(class, cmd, data) (int)((unsigned)(sizeof(data) << 16) + ((unsigned)(class) << 8) + (unsigned)(cmd))
#define __DIOT(class, cmd, data) (int)((unsigned)(sizeof(data) << 16) + ((unsigned)(class) << 8) + (unsigned)(cmd))

int devctl(int fd, int dcmd, void *data_ptr, size_t nbytes, void *flags);
#endif

#define DSS_SRV_ALLOC_WFDDEV __DIOTF(_DCMD_MISC, 0x101, struct wfd_dev_data)
#define DSS_SRV_ALLOC_WFDPORT __DIOTF(_DCMD_MISC, 0x102, struct wfd_port_data)
#define DSS_SRV_ALLOC_WFDPIPE __DIOTF(_DCMD_MISC, 0x103, struct wfd_pipe_data)
#define DSS_SRV_GET_PORTMODE __DIOTF(_DCMD_MISC, 0x104, struct wfd_mode_data)
#define DSS_SRV_SET_PLANEMASK __DIOTF(_DCMD_MISC, 0x105, struct wfd_plane_mask_data)
#define DSS_SRV_PORT_COMMIT __DIOT(_DCMD_MISC, 0x106, struct wfd_commit_data)
#define DSS_SRV_MODESET_ENABLE __DIOT(_DCMD_MISC, 0x107, struct wfd_modeset_data)
#define DSS_SRV_PLANE_UPDATE __DIOT(_DCMD_MISC, 0x108, struct wfd_plane_state)
#define DSS_SRV_WAIT_VBLANK __DIOT(_DCMD_MISC, 0x109, NULL)
#define DSS_SRV_DISABLE_WFDPORT __DIOTF(_DCMD_MISC, 0x110, struct wfd_port_data)
#define DSS_SRV_DUMP_REGS __DIOTF(_DCMD_MISC, 0x111, bool)

struct wfd_device *dss_alloc_wfd_device(WFDint id);
struct wfd_port *dss_alloc_wfd_port(struct wfd_device *dev, WFDint wfd_port_id, unsigned int index);
struct wfd_pipe *dss_alloc_wfd_pipe(struct wfd_device *dev, WFDint wfd_pipe_id, unsigned int index);
struct wfd_port_mode *dss_get_port_modes(struct wfd_port *port);
void dss_set_plane_mask(struct wfd_device *dev);
void dss_port_flush(struct wfd_port *port, const struct wfd_commit_state *commit_state);
void dss_wfd_modeset_enable(struct wfd_port *port);
void dss_wfd_port_disable(struct wfd_port *port);
void dss_plane_update(struct wfd_pipe *pipe);
void dss_wait_vsync(struct wfd_port *port);
void dss_close(struct wfd_device *dev);
void dss_regs_dump(bool is_full_dump);
const char *wfd_get_commit_type(WFDCommitType type);
const char* vop2_get_pipeline_name(struct wfd_pipe *pipe);
bool wfd_port_state_change(struct wfd_port *port);
void wfd_port_state_clear(struct wfd_port *port);
void wfd_port_state_set(struct wfd_port *port);

#endif
