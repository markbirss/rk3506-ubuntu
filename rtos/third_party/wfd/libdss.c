/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <string.h>

#include "dss_service.h"

#if defined(__QNXNTO__)
#include <memobj/physmem.h>
#include <screen/private/scrmem.h>
#endif

#define memclear(s) memset(&s, 0, sizeof(s))

static uint32_t wfd_debug_level_mask;

uint32_t dss_get_debug_level_mask(void)
{
	return wfd_debug_level_mask;
}

struct wfd_device *dss_alloc_wfd_device(WFDint id)
{
	struct wfd_device *dev;
	struct wfd_dev_data dev_data;
	int fd;
	int ret;

	memclear(dev_data);

#if defined(__RT_THREAD__)
	fd = 0;
#else
	fd = open("/dev/dss_service", O_RDWR);
	if (fd <=0) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "open /dev/dss_service failed: %s", strerror(errno));
		return NULL;
	}
#endif

	dev = dss_calloc(1, sizeof(struct wfd_device));
	if (!dev) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed: %s", "malloc", strerror(errno));
		goto err_malloc;
	}

	dev->handle = WFD_INVALID_HANDLE;
	dev->ref = REFCOUNTER_INITIALIZER;
	dev->wfd_dev_id = WFD_DEFAULT_DEVICE_ID;

	if (!wfd_hdl_set_device_in_use(id)) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "acquire device id failed: %s", strerror(errno));
		goto err_devctl;
	}
	dev->wfd_dev_id = id;

	if (!wfd_device_bind_handle(dev)) {
		TRACE_API_F("%s failed", "wfd_device_bind_handle");
	}

	dev_data.handle = dev->handle;
	dev_data.wfd_dev_id = dev->wfd_dev_id;

	ret = devctl(fd, DSS_SRV_ALLOC_WFDDEV, &dev_data, sizeof(dev_data), NULL);
	if (ret != EOK) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "DSS_SRV_ALLOC_WFDDEV failed: %s", strerror(errno));
		goto err_devctl;
	}

	wfd_debug_level_mask = dev_data.debug_mask;

	TRACE_API_F("alloc wfd device with dss fd: %d handle: %d", fd, dev->handle);

	dev->handle = ((unsigned int)fd << 16) | dev->handle;

	return dev;

err_devctl:
	free(dev);
err_malloc:
#if defined(__QNXNTO__)
	close(fd);
#endif
	return NULL;
}

void dss_close(struct wfd_device *dev)
{
	int fd = (int)(dev->handle >> 16);

#if defined(__QNXNTO__)
	close(fd);
#endif

	TRACE_API_F("close dss fd: %d handle: %d", fd, dev->handle);
}

struct wfd_port *dss_alloc_wfd_port(struct wfd_device *dev, WFDint wfd_port_id, unsigned int index)
{
	struct wfd_port *port;
	struct wfd_port_data port_data;
	uint32_t vp_id = index & 63u;
	int fd = (int)(dev->handle >> 16);
	int ret;

	memclear(port_data);
	port = dss_calloc(1, sizeof(struct wfd_port));
	if (!port) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed: %s", "malloc", strerror(errno));
		return NULL;
	}

	port->handle = WFD_INVALID_HANDLE;
	port->ref = REFCOUNTER_INITIALIZER;
	port->dev = dev;
	port->wfd_port_id = wfd_port_id;
	port->index = index & 63u;

	port_data.wfd_port_id = port->wfd_port_id;
	port_data.vp_id = vp_id;
	port_data.index = port->index;

	ret = devctl(fd, DSS_SRV_ALLOC_WFDPORT, &port_data, sizeof(port_data), NULL);
	if (ret != EOK) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "DSS_SRV_ALLOC_WFDPORT failed: %s", strerror(errno));
		goto err_devctl;
	}

	return port;

err_devctl:
	free(port);
	return NULL;
}

void dss_wfd_modeset_enable(struct wfd_port *port)
{
	struct wfd_device *dev = port->dev;
	struct wfd_modeset_data modeset_data;
	int fd = (int)(dev->handle >> 16);
	int ret;

	memclear(modeset_data);
	modeset_data.wfd_port_id = port->wfd_port_id;
	modeset_data.timging = port->pending.mode->timing;

	ret = devctl(fd, DSS_SRV_MODESET_ENABLE, &modeset_data, sizeof(modeset_data), NULL);
	if (ret != EOK)
		slogf(SLOGC_SELF, _SLOG_ERROR, "DSS_SRV_MODESET_ENABLE failed: %s", strerror(errno));
}

void dss_wfd_port_disable(struct wfd_port *port)
{
	struct wfd_device *dev = port->dev;
	struct wfd_port_data port_data;
	int fd = (int)(dev->handle >> 16);
	int ret;

	memclear(port_data);

	port_data.wfd_port_id = port->wfd_port_id;

	ret = devctl(fd, DSS_SRV_DISABLE_WFDPORT, &port_data, sizeof(port_data), NULL);
	if (ret != EOK)
		slogf(SLOGC_SELF, _SLOG_ERROR, "DSS_SRV_DISABLE_WFDPORT failed: %s", strerror(errno));
}

struct wfd_port_mode *dss_get_port_modes(struct wfd_port *port)
{
	struct wfd_device *dev = port->dev;
	struct wfd_mode_data mode_data;
	struct wfd_port_mode *mode;
	int fd = (int)(dev->handle >> 16);
	int ret;

	memclear(mode_data);
	mode_data.wfd_port_id = port->wfd_port_id;

	ret = devctl(fd, DSS_SRV_GET_PORTMODE, &mode_data, sizeof(mode_data), NULL);
	if (ret != EOK)
		slogf(SLOGC_SELF, _SLOG_ERROR, "DSS_SRV_GET_PORTMODE failed: %s", strerror(errno));

	mode = dss_calloc(1, sizeof(struct wfd_port_mode));
	if (mode) {
		mode->handle = WFD_INVALID_HANDLE;
		mode->ref = REFCOUNTER_INITIALIZER;
		mode->port = port;
		mode->timing = mode_data.vm;
	}

	return mode;
}

void dss_set_plane_mask(struct wfd_device *dev)
{
	int fd = (int)(dev->handle >> 16);
	struct wfd_plane_mask_data plane_mask_data;
	struct wfd_port *port;
	struct wfd_pipe *pipe;
	unsigned long plane_mask;
	int nr_ports = 0;
	int ret;

	memclear(plane_mask_data);

	SLIST_FOREACH(port, &dev->port_list, dev_portlist_entry) {
		plane_mask = 0;
		SLIST_FOREACH(pipe, &dev->pipe_list, dev_pipelist_entry) {
			if (wfd_config_find_id_in_array(pipe->bindable_ports_to_advertise, port->wfd_port_id)) {
				plane_mask  |= BIT(pipe->wfd_pipe_id - 1);
			}
		}
		plane_mask_data.plane_mask[nr_ports] = plane_mask;
		nr_ports++;

	}

	plane_mask_data.nr_ports = nr_ports;

	ret = devctl(fd, DSS_SRV_SET_PLANEMASK, &plane_mask_data, sizeof(plane_mask_data), NULL);
	if (ret != EOK)
		slogf(SLOGC_SELF, _SLOG_ERROR, "DSS_SRV_SET_PLANMASK failed: %s", strerror(errno));
}

void dss_wait_vsync(struct wfd_port *port)
{
	struct wfd_device *dev = port->dev;
	int fd = (int)(dev->handle >> 16);
	struct wfd_port_data port_data;
	int ret;

	memclear(port_data);

	port_data.wfd_port_id = port->wfd_port_id;

	ret = devctl(fd, DSS_SRV_WAIT_VBLANK, &port_data, sizeof(port_data), NULL);
	if (ret != EOK)
		slogf(SLOGC_SELF, _SLOG_ERROR, "DSS_SRV_WAIT_VBLANK failed: %s", strerror(errno));
}

void dss_port_flush(struct wfd_port *port, const struct wfd_commit_state *commit_state)
{
	struct wfd_device *dev = port->dev;
	int fd = (int)(dev->handle >> 16);
	struct wfd_commit_data commit_data;
	int ret;

	memclear(commit_data);

	commit_data.wfd_port_id = port->wfd_port_id;
	commit_data.pipe_idx_mask = commit_state->pipe_idx_mask;

	ret = devctl(fd, DSS_SRV_PORT_COMMIT, &commit_data, sizeof(commit_data), NULL);
	if (ret != EOK)
		slogf(SLOGC_SELF, _SLOG_ERROR, "DSS_SRV_PORT_COMMIT failed: %s", strerror(errno));
}

struct wfd_pipe *dss_alloc_wfd_pipe(struct wfd_device *dev, WFDint wfd_pipe_id, unsigned int index)
{
	struct wfd_pipe *pipe;
	struct wfd_pipe_data pipe_data;
	int fd = (int)(dev->handle >> 16);
	int ret;

	memclear(pipe_data);
	pipe_data.wfd_pipe_id = wfd_pipe_id;
	pipe_data.index = index & 63u;

	pipe = dss_calloc(1, sizeof(struct wfd_pipe));
	if (!pipe)
		return NULL;

	pipe->handle = WFD_INVALID_HANDLE;
	pipe->ref = REFCOUNTER_INITIALIZER;
	pipe->pending.transparency = WFD_TRANSPARENCY_NONE;
	pipe->pending.global_alpha = 255;
	pipe->committed.transparency = WFD_TRANSPARENCY_NONE;
	pipe->committed.global_alpha = 255;
	pipe->wfd_pipe_id = WFD_INVALID_PIPELINE_ID;
	pipe->max_width = 65536;
	pipe->max_height = 65536;
	pipe->dev = dev;
	pipe->wfd_pipe_id = wfd_pipe_id;
	pipe->index = index & 63u;
	// initial zorder is the ID, but load_wfd_pipe_cfg may override it
	pipe->zorder = wfd_pipe_id;

	ret = devctl(fd, DSS_SRV_ALLOC_WFDPIPE, &pipe_data, sizeof(pipe_data), NULL);
	if (ret != EOK) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "DSS_SRV_ALLOC_WFDPIPE failed: %s", strerror(errno));
		goto out;
	} else {
		pipe->name = strdup((const char *)&pipe_data.name);
	}

	return pipe;
out:
	free(pipe);
	return NULL;
}

#if defined(__RT_THREAD__)
static bool dss_init_framebuffer(struct wfd_plane_state *wfd_pstate,
					   struct wfd_source *wfd_src)
{
	struct drm_framebuffer *fb = &wfd_pstate->fb;
	win_image_t *img =  wfd_src->winimg;

	wfd_pstate->yrgb_mst = (uint32_t)img->vaddr;
	wfd_pstate->uv_mst = (uint32_t)(img->vaddr + (uint32_t)img->planar_offsets[1]);
	wfd_pstate->fb_size = img->size;
	wfd_pstate->wfd_format = img->format;
	fb->width = (uint32_t)img->width;
	fb->height = (uint32_t)img->height;
	fb->pitches[0] = (unsigned int)img->strides[0];
	fb->pitches[1] = fb->pitches[0];

	TRACE_CALL("image format %d %dx%d stride %d img_paddr: 0x%x img_uv_paddr: 0x%x img_size:%d\n",
		    img->format, img->width, img->height, img->strides[0],
		    wfd_pstate->yrgb_mst, wfd_pstate->uv_mst, img->size);
	return true;
}
#else
static bool dss_init_framebuffer_by_scrmem(struct wfd_plane_state *wfd_pstate,
					    const struct scrmem *scrmem)
{
	struct drm_framebuffer *fb = &wfd_pstate->fb;
	const struct memobj_phys_segment *locked_memory;
	const win_image_t *img;
	struct memobj *memobj;
	uint32_t locked_paddr;
	uint32_t locked_nbytes;

	memobj = scrmem_get_memobj(scrmem);
	if (!memobj) {
		// Screen is not aware of any memory associated with the image.
		SLOG_ERR("scrmem_get_memobj failed");
		return false;
	} else {

		if (EOK != memobj_lock_phys(memobj)) {
			SLOG_ERR("memobj_lock_phys failed");
			return false;
		}

		locked_memory = memobj_get_phys_layout(memobj);

		if (locked_memory->next) {
			// The memory isn't physically contiguous.
			SLOG_ERR("The memory isn't physicall contiguous");
			return false;
		} else  {
			locked_paddr = (uint32_t)locked_memory->paddr;
			locked_nbytes = (uint32_t)locked_memory->nbytes;
		}
	}

	img = scrmem_get_win_image(scrmem);
	if (img->paddr & 0xff00000000)
		printf("vop2 only support screen memory bellow 4G\n");

	wfd_pstate->yrgb_mst = (uint32_t)locked_paddr;
	wfd_pstate->uv_mst = locked_paddr + (uint32_t)img->planar_offsets[1];
	wfd_pstate->fb_size = locked_nbytes;
	wfd_pstate->wfd_format = img->format;
	fb->width = (uint32_t)img->width;
	fb->height = (uint32_t)img->height;
	fb->pitches[0] = (unsigned int)img->strides[0];
	fb->pitches[1] = fb->pitches[0];

	TRACE_CALL("image format %d %dx%d stride %d img_paddr: 0x%x img_uv_paddr: 0x%x img_size:%d locked_paddr: 0x%x locked_nbytes: %d\n",
		    img->format, img->width, img->height, img->strides[0],
		    wfd_pstate->yrgb_mst, wfd_pstate->uv_mst, img->size, locked_paddr, locked_nbytes);
	return true;
}
#endif

void dss_plane_update(struct wfd_pipe *pipe)
{
	struct wfd_device *dev = pipe->dev;
	struct wfd_port *port = wfd_pipe_get_commit_port(pipe);
	int fd = (int)(dev->handle >> 16);
	struct wfd_plane_state wfd_pstate;
	struct drm_rect *dst = &wfd_pstate.dst;
	struct drm_rect *src = &wfd_pstate.src;
	struct wfd_source *wfd_src;
	int ret;

	memclear(wfd_pstate);
	wfd_pstate.wfd_port_id = port->wfd_port_id;
	wfd_pstate.wfd_pipe_id = pipe->wfd_pipe_id;
	wfd_pstate.period_ns = port->period_ns;
	wfd_pstate.start_ts = port->start_ts;
	wfd_pstate.end_ts = port->end_ts;
	wfd_pstate.zorder = pipe->zorder;
	wfd_pstate.global_alpha = pipe->pending.global_alpha;

	src->x1 = pipe->pending.src_rect.offsetX << 16;
	src->y1 = pipe->pending.src_rect.offsetY << 16;
	src->x2 = (pipe->pending.src_rect.width << 16) + src->x1;
	src->y2 = (pipe->pending.src_rect.height << 16) + src->y1;

	dst->x1 = pipe->pending.dst_rect.offsetX;
	dst->y1 = pipe->pending.dst_rect.offsetY;
	dst->x2 = pipe->pending.dst_rect.width + dst->x1;
	dst->y2 = pipe->pending.dst_rect.height + dst->y1;
	wfd_src = pipe->pending.src;
#if defined(__RT_THREAD__)
	if (wfd_src) {
		if (!dss_init_framebuffer(&wfd_pstate, wfd_src)) {
#else
	if (wfd_src && wfd_src->scrmem) {
		if (!dss_init_framebuffer_by_scrmem(&wfd_pstate, wfd_src->scrmem)) {
#endif
			/*
			 * scrmem init failed, no fb for display
			 */
			dst->x2 = dst->x1;
		}
	} else {
		dst->x2 = dst->x1;
	}

	ret = devctl(fd, DSS_SRV_PLANE_UPDATE, &wfd_pstate, sizeof(wfd_pstate), NULL);
	if (ret != EOK)
		slogf(SLOGC_SELF, _SLOG_ERROR, "DSS_SRV_PLANE_UPDATE failed: %s", strerror(errno));
}

#if defined(__RT_THREAD__)
void dss_regs_dump(bool is_full_dump)
{
	int fd = 0;
	int ret;

	ret = devctl(fd, DSS_SRV_DUMP_REGS, &is_full_dump, sizeof(is_full_dump), NULL);
	if (ret != EOK)
		slogf(SLOGC_SELF, _SLOG_ERROR, "DSS_SRV_REGS_DUMP failed: %s", strerror(errno));
}
#endif

const char *wfd_get_commit_type(WFDCommitType type)
{
	switch (type) {
	case WFD_COMMIT_ENTIRE_DEVICE:
		return "device";
	case WFD_COMMIT_ENTIRE_PORT:
		return "port";
	case WFD_COMMIT_PIPELINE:
		return "pipeline";
	default:
		return "unknown";
	}
}

const char* vop2_get_pipeline_name(struct wfd_pipe *pipe)
{
	return pipe->name;
}

bool wfd_port_state_change(struct wfd_port *port)
{
	if (port->changes & WFD_PORT_CHANGES_MODE)
		return true;
	else
		return false;
}

void wfd_port_state_clear(struct wfd_port *port)
{
	port->changes &= ~WFD_PORT_CHANGES_MODE;
}

void wfd_port_state_set(struct wfd_port *port)
{
	port->changes = WFD_PORT_CHANGES_MODE;
}
