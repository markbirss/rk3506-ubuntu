/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <assert.h>
#if defined(__QNXNTO__)
#include <atomic.h>
#endif
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>  // memcpy

#include "wfddev.h"
#include "dss_service.h"

#define FIXED_GAMMA_VALUE 1

static _Bool component_float_valid(WFDfloat f)
{
	return f >= 0 && f <= 1;
}

static _Bool component_int_valid(WFDint i)
{
	return i >= 0 && i <= 255;
}

static WFDint component_float_convert_int(WFDfloat f)
{
	if (!(f > 0))
		return 0;
	else if (f >= 1)
		return 255;
	else
		return (int)(f * 255);
}

static WFDfloat component_int_convert_float(WFDint i)
{
	if (i <= 0)
		return 0;
	else if (i >= 255)
		return 1;
	else
		return (WFDfloat)i / (WFDfloat)255;
}

void wfd_port_ref_release(struct wfd_port *port, const void *refkey)
{
	TRACE_F("port=%p", port);
	if (port) {
		if (refcount_dec_value(&port->ref, refkey) == 0)
			wfd_hdl_send_ref_zero_signal();
	}
}

static void wfd_release_port_mode_ref(struct wfd_port_mode *mode, const void *refkey)
{
	TRACE_F("mode=%p", mode);
	if (mode)
		refcount_dec(&mode->ref, refkey);
}

void wfd_port_ref_replace(struct wfd_port **slot, struct wfd_port *port, const void *refkey)
{
	if (*slot != port) {
		if (*slot) {
			wfd_port_ref_release(*slot, refkey);
			*slot = NULL;
		}
		if (port) {
			refcount_inc(&port->ref, refkey);
			*slot = port;
		}
	}
}

void wfd_port_mode_ref_replace(struct wfd_port_mode **slot, struct wfd_port_mode *mode, const void *refkey)
{
	if (*slot != mode) {
		if (*slot) {
			wfd_release_port_mode_ref(*slot, refkey);
			*slot = NULL;
		}
		if (mode) {
			refcount_inc(&mode->ref, refkey);
			*slot = mode;
		}
	}
}

void wfd_port_handles_invalidate(struct wfd_port *port)
{
	assert(port);
	assert_device_locked(port->dev);

	if (port->handle != WFD_INVALID_HANDLE) {
		DEBUG2("Freeing port handle %"PRIhdl" (for port %p)",
				port->handle, port);
		wfd_port_free_handle(port);
	}
}

void wfd_port_refs_clear(struct wfd_port *port)
{
	assert(port);

	wfd_port_mode_ref_replace(&port->pending.mode, NULL, wfdSetPortMode);
	wfd_port_mode_ref_replace(&port->pending.mode, NULL, wfdSetPortMode);
}

bool wfd_find_device_port_by_handle(WFDDevice device_handle, WFDPort port_handle,
				    struct wfd_device **pdevice, struct wfd_port **pport,
				    const void *refkey)
{
	TRACE;
	struct wfd_device *dev;
	struct wfd_port *port;

	assert(pdevice);
	assert(pport);
	TRACE_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl, device_handle, port_handle);

	dev = wfd_find_deivce_by_handle(device_handle, refkey);
	if (dev) {
		port = wfd_find_port_by_handle(port_handle, refkey);
		if (port) {
			if (port->dev == dev) {
				*pdevice = dev;
				*pport = port;
				TRACE_F("returning dev=%p port=%p", dev, port);
				return true;
			}
			DEBUG2("%s: port handle %"PRIhdl" refers to wrong device"
			       " (found dev handle %"PRIhdl", wanted %"PRIhdl")",
			       __func__, port_handle, port->dev->handle, device_handle);
			wfd_port_ref_release(port, refkey);
		}
		wfd_device_err_store(dev, WFD_ERROR_BAD_HANDLE);
		wfd_device_ref_release(dev, refkey);
	}

	TRACE_F("fail");

	return false;
}

static bool wfd_find_device_port_mode_by_handle(WFDDevice device_handle, WFDPort port_handle,
						WFDPortMode port_mode_handle,
						struct wfd_device **pdevice, struct wfd_port **pport,
						struct wfd_port_mode **pport_mode, const void *refkey)
{
	TRACE;
	struct wfd_device *dev;
	struct wfd_port *port;
	struct wfd_port_mode *mode;
	WFDErrorCode wfderr;

	TRACE_F("port_mode_handle=%"PRIhdl, port_mode_handle);

	if (wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, refkey)) {
		mode = wfd_find_port_mode_by_handle(port_mode_handle, refkey);
		if (mode) {
			assert(mode->port);
			if (mode->port == port) {
				*pdevice = dev;
				*pport = port;
				*pport_mode = mode;
				TRACE_F("returning mode=%p", mode);
				return true;
			}
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
			DEBUG2("%s: mode handle %"PRIhdl" refers to wrong port"
			       " (found port %p, wanted %p)",
			       __func__, port_mode_handle, mode->port, port);
			wfd_release_port_mode_ref(mode, refkey);
		} else {
			wfderr = WFD_ERROR_BAD_HANDLE;
		}

		wfd_device_err_store(dev, wfderr);
		wfd_port_ref_release(port, refkey);
		wfd_device_ref_release(dev, refkey);
	}

	return false;
}

WFDint wfd_port_layer_order_get(const struct wfd_port *port, const struct wfd_pipe *pipe)
{
	assert(port);
	assert_device_locked(port->dev);
	assert(pipe);
	assert(pipe->wfd_pipe_id);

	WFDint order = WFD_INVALID_PIPELINE_LAYER;

	if (wfd_config_find_id_in_array(pipe->bindable_ports, port->wfd_port_id)) {
		order = pipe->zorder;
		assert(order != WFD_INVALID_PIPELINE_LAYER);
	}

	return order;
}

static WFDint wfd_get_bindable_pipe_count(const struct wfd_device *dev, const struct wfd_port *port,
					  WFDint *id_list)
{
	assert(dev);
	assert(port);

	struct wfd_pipe *pipe;
	WFDint count = 0;

	SLIST_FOREACH(pipe, &dev->pipe_list, dev_pipelist_entry) {
		if (wfd_config_find_id_in_array(pipe->bindable_ports_to_advertise, port->wfd_port_id)) {
			if (id_list)
				id_list[count] = pipe->wfd_pipe_id;
			++count;
		}
	}

	return count;
}

static double wfd_calculate_port_mode_refresh(const struct wfdmode_timing *timing)
{
	uint32_t htotal = timing->hpixels + timing->hfp + timing->hsw + timing->hbp;
	uint32_t vtotal = timing->vlines + timing->vfp + timing->vsw + timing->vbp;

	return (double)timing->pixel_clock_kHz * 1000 / htotal / vtotal;
}

static void wfd_add_port_mode_to_list(struct wfd_port *port, struct wfd_port_mode **endptr,
				     struct wfd_port_mode *mode)
{
	assert(endptr);

	if (*endptr)
		SLIST_INSERT_AFTER(*endptr, mode, port_modelist_entry);
	else
		SLIST_INSERT_HEAD(&port->modes, mode, port_modelist_entry);

	*endptr = mode;
}

static int wfd_add_port_mode(struct wfd_port *port)
{
	assert(port);
	assert(port->dev);

	struct wfd_port_mode *mode;
	struct wfd_port_mode *last_wfdmode = NULL;

	mode = dss_get_port_modes(port);
	if (!mode)
		slogf(SLOGC_SELF, _SLOG_ERROR, "port%d get mode failed\n", port->wfd_port_id);
	else
		wfd_add_port_mode_to_list(port, &last_wfdmode, mode);

	return 0;
}

static void wfd_free_port_mode_list(struct wfd_port *port)
{
	struct wfd_port_mode *mode;
	while ((mode = SLIST_FIRST(&port->modes)) != NULL) {
		SLIST_REMOVE_HEAD(&port->modes, port_modelist_entry);

		wfd_port_mode_free_handle(mode);
		free(mode);
	}
}

WFDErrorCode wfd_port_commit(struct wfd_port *port, struct wfd_commit_state *commit_state)
{
	assert(commit_state);
	assert(commit_state->dev);
	assert_device_locked(commit_state->dev);
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_pipe *pipe;
	struct wfd_port *pendport;

	commit_state->port_idx_mask |= (uint64_t)1 << port->index;

	if (port->pending.mode == NULL) {
		wfderr = WFD_ERROR_INCONSISTENCY;
		goto out;
	}

	if (wfd_port_state_change(port)) {
		DEBUG1("VP%d  modes change", port->index);
		dss_wfd_modeset_enable(port);
		wfd_port_state_clear(port);
	}

	DEBUG1("%s device=%p port: %d", __func__, commit_state->dev, port->index);
	SLIST_FOREACH(pipe, &commit_state->dev->pipe_list, dev_pipelist_entry) {
		if (wfd_pipe_get_commit_port(pipe) == port) {
			if (commit_state->type != WFD_COMMIT_ENTIRE_DEVICE) {
				pendport = pipe->pending.port;
				if (pendport && pendport != port) {
					wfderr = WFD_ERROR_INCONSISTENCY;
					goto out;
				}
			}

			wfderr = wfd_pipe_commit(pipe, commit_state);
			if (wfderr != WFD_ERROR_NONE)
				goto out;
		}
	}
out:
	return wfderr;
}

void wfd_port_commit_update(struct wfd_port *port)
{
	assert_device_locked(port->dev);
	wfd_port_mode_ref_replace(&port->committed.mode, port->pending.mode, wfdSetPortMode);
	port->committed = port->pending;
}

void wfd_bind_pipe_to_port(struct wfd_pipe *pipe, struct wfd_port *port)
{
	assert(pipe);
	assert_device_locked(pipe->dev);

	wfd_port_ref_replace(&pipe->pending.port, port, __func__);
}

int wfd_port_commit_flush(struct wfd_port *port, const struct wfd_commit_state *commit_state)
{
	assert_device_locked(port->dev);

	dss_port_flush(port, commit_state);

	return EOK;
}

static struct wfd_port *wfd_find_port_by_id(struct wfd_device *dev, WFDint port_id)
{
	struct wfd_port *port = NULL, *tmp;

	SLIST_FOREACH(tmp, &dev->port_list, dev_portlist_entry) {
		if (tmp->wfd_port_id == port_id) {
			assert(!port);
			port = tmp;
		}
	}

	return port;
}

bool wfd_port_next_vblank_get(struct timespec *ts, struct wfd_port *port, const struct timespec *start)
{
	const uint64_t start_ns = timespec2nsec(start);
	uint64_t period_ns, next_vblank_ns, offset;
	double refresh;
	bool success = false;

	if (port->committed.mode) {
		refresh = wfd_calculate_port_mode_refresh(&port->committed.mode->timing);
		period_ns = (uint64_t)(1000.0 * 1000 * 1000 / refresh);
		port->period_ns = period_ns;
		offset = (start_ns % period_ns);
		DEBUG2("vsync-vp%d start_ns : %lld period_ns: %lld offset: %lld\n", port->wfd_port_id - 1, start_ns, period_ns, offset);
		next_vblank_ns = start_ns;
		if (offset)
			next_vblank_ns += period_ns;

		nsec2timespec(ts, next_vblank_ns);
		success = true;
	}

	return success;
}

/*** Standard API functions start ***/

WFD_API_CALL WFDint WFD_APIENTRY
wfdEnumeratePorts(WFDDevice device_handle,
		  WFDint *port_ids,
		  WFDint port_ids_count,
		  const WFDint *filter_list)
{
	TRACE_API_F("device_handle=%"PRIhdl" port_ids=%p port_ids_count=%d filter_list=%p",
		    device_handle, port_ids, port_ids_count, filter_list);

	struct wfd_device *dev = wfd_find_deivce_by_handle(device_handle, __func__);
	const struct wfd_port *port;
	WFDint found = 0;

	if (!dev) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return found;
	}

	if (port_ids && (port_ids_count <= 0)) {
		wfd_device_err_store(dev, WFD_ERROR_ILLEGAL_ARGUMENT);
	} else if (filter_list && *filter_list != WFD_NONE) {
		/* do nothing */
	} else {
		lock_device(dev);
		SLIST_FOREACH(port, &dev->port_list, dev_portlist_entry) {
			if (port->wfd_port_id == WFD_INVALID_PORT_ID) {
				assert(0 && "port has no wfd_port_id");
			} else {
				if (port_ids) {
					if (port_ids && found >= port_ids_count) break;
					port_ids[found] = port->wfd_port_id;
				}
				++found;
			}
		}
		unlock_device(dev);
	}

	TRACE_API_F("returning found=%d", found);
	wfd_device_ref_release(dev, __func__);

	return found;
}

WFD_API_CALL WFDPort WFD_APIENTRY
wfdCreatePort(WFDDevice device_handle,
              WFDint port_id,
              const WFDint *attr_list) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_id=%d attr_list=%p",
		    device_handle, port_id, attr_list);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	WFDPort port_handle;
	struct wfd_device *dev;
	struct wfd_port *port = NULL;
	struct wfd_port_mode *mode;
	int ret;

	dev = wfd_find_deivce_by_handle(device_handle, __func__);
	if (!dev) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return WFD_INVALID_HANDLE;
	}

	if (attr_list && *attr_list != WFD_NONE) {
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		goto out;
	}

	lock_device(dev);
	port = wfd_find_port_by_id(dev, port_id);
	if (!port) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
	} else if (port->in_use) {
		port = NULL;
		wfderr = WFD_ERROR_IN_USE;
	} else {
		port->in_use = true;
		refcount_inc(&dev->ref, port);

		ret = wfd_add_port_mode(port);
		if (ret) {
			if (ret == ENOMEM)
				wfderr = WFD_ERROR_OUT_OF_MEMORY;
			else
				wfderr = WFD_ERROR_INCONSISTENCY;
		}

		if (!wfderr) {
			SLIST_FOREACH(mode, &port->modes, port_modelist_entry) {
				assert(mode->handle == WFD_INVALID_HANDLE);
				if (!wfd_port_mode_bind_handle(mode)) {
					wfderr = WFD_ERROR_OUT_OF_MEMORY;
					break;
				}
			}
		}
	}
	unlock_device(dev);

	if (wfderr)
		goto out;

	if (!wfd_port_bind_handle(port))
		wfderr = WFD_ERROR_OUT_OF_MEMORY;
out:
	if (wfderr == WFD_ERROR_NONE) {
		assert(port);
		assert(port->handle != WFD_INVALID_HANDLE);
	} else {
		if (port) {
			assert(port->in_use);

			lock_device(dev);
			wfd_free_port_mode_list(port);
			port->in_use = false;
			wfd_device_ref_release(dev, port);
			unlock_device(dev);
		}
		port = NULL;
		wfd_device_err_store(dev, wfderr);
	}

	port_handle = port ? port->handle : WFD_INVALID_HANDLE;
	TRACE_API_F("returning port=%p with handle %"PRIhdl, port, port_handle);
	wfd_device_ref_release(dev, __func__);

	return port_handle;
}

WFD_API_CALL void WFD_APIENTRY
wfdDestroyPort(WFDDevice device_handle, WFDPort port_handle) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl, device_handle, port_handle);

	struct wfd_device *dev;
	struct wfd_port *port;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	lock_device(port->dev);
	dss_wfd_port_disable(port);
	wfd_port_handles_invalidate(port);
	wfd_port_refs_clear(port);
	unlock_device(port->dev);

	wfd_port_ref_release(port, __func__);
	wfd_hdl_wait_refcount_release(&port->ref);

	assert(port->in_use);

	lock_device(dev);
	port->in_use = false;
	wfd_device_ref_release(dev, port);
	unlock_device(dev);

	wfd_device_ref_release(dev, __func__);
	DEBUG2("%s succeeded", __func__);
}

WFD_API_CALL WFDint WFD_APIENTRY
wfdGetPortModes(WFDDevice device_handle,
		WFDPort port_handle,
		WFDPortMode *mode_list,
		WFDint modes_count) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" mode_list=%p count=%d",
		    device_handle, port_handle, mode_list, modes_count);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	WFDint found = 0;
	struct wfd_device *dev;
	struct wfd_port *port;
	struct wfd_port_mode *mode;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return found;
	}

	if (mode_list && (modes_count <= 0)) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
	} else {
		lock_device(dev);
		SLIST_FOREACH(mode, &port->modes, port_modelist_entry) {
			if (mode_list) {
				if (found >= modes_count) break;
				mode_list[found] = mode->handle;
			}
			DEBUG1("Getting mode %ux%u @ %.2f Hz on port ID %d",
				(unsigned)mode->timing.hpixels,
				(unsigned)mode->timing.vlines,
				wfd_calculate_port_mode_refresh(&mode->timing),
				port->wfd_port_id);

			++found;
		}
		unlock_device(dev);
	}

	if (wfderr != WFD_ERROR_NONE) {
		assert(found == 0);
		wfd_device_err_store(dev, wfderr);
	}

	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);

	return found;
}

WFD_API_CALL WFDPortMode WFD_APIENTRY
wfdGetCurrentPortMode(WFDDevice device_handle, WFDPort port_handle) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl, device_handle, port_handle);

	WFDPortMode port_mode_handle = WFD_INVALID_HANDLE;
	struct wfd_device *dev;
	struct wfd_port *port;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return port_mode_handle;
	}

	lock_device(dev);
	if (port->pending.mode)
		port_mode_handle = port->pending.mode->handle;
	unlock_device(dev);

	if (port_mode_handle == WFD_INVALID_HANDLE)
		wfd_device_err_store(dev, WFD_ERROR_NOT_SUPPORTED);

	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);

	return port_mode_handle;
}

WFD_API_CALL void WFD_APIENTRY
wfdSetPortMode(WFDDevice device_handle,
	       WFDPort port_handle,
	       WFDPortMode port_mode_handle) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" port_mode_handle=%"PRIhdl,
		    device_handle, port_handle, port_mode_handle);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;
	struct wfd_port_mode *mode;
	double vrefresh;

	if (!wfd_find_device_port_mode_by_handle(device_handle, port_handle, port_mode_handle,
						 &dev, &port, &mode, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}
	vrefresh = wfd_calculate_port_mode_refresh(&mode->timing);
	DEBUG1("Setting mode %ux%u @ %.2f Hz on VP%d",
		(unsigned)mode->timing.hpixels,
		(unsigned)mode->timing.vlines,
		vrefresh,
		port->wfd_port_id - 1) ;

	lock_device(dev);
	if (port->pending.mode != mode) {
		wfd_port_state_set(port);
		printf("vp%d mode change\n", port->index);
	}
	wfd_port_mode_ref_replace(&port->pending.mode, mode, wfdSetPortMode);
	unlock_device(dev);

	wfd_device_err_store(dev, wfderr);
	wfd_release_port_mode_ref(mode, __func__);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL WFDint WFD_APIENTRY
wfdGetPortModeAttribi(WFDDevice device_handle,
		      WFDPort port_handle,
		      WFDPortMode port_mode_handle,
		      WFDPortModeAttrib attr) WFD_APIEXIT
{
	TRACE_API_F("attr=%#x", attr);

	WFDint value = -1;
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;
	struct wfd_port_mode *mode;

	if (!wfd_find_device_port_mode_by_handle(device_handle, port_handle, port_mode_handle,
						 &dev, &port, &mode, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return value;
	}

	TRACE_CALL("VP%d attr: %#x\n", port->wfd_port_id - 1, attr);

	switch ((int32_t)attr) {
	case WFD_PORT_MODE_WIDTH:
		value = (WFDint)mode->timing.hpixels;
		assert(value >= 0);
		break;
	case WFD_PORT_MODE_HEIGHT:
		value = (WFDint)mode->timing.vlines;
		assert(value >= 0);
		break;
	case WFD_PORT_MODE_REFRESH_RATE:
		value = (WFDint)round(wfd_calculate_port_mode_refresh(&mode->timing));
		assert(value >= 0);
		break;
	case WFD_PORT_MODE_FLIP_MIRROR_SUPPORT:
		value = WFD_FALSE;
		break;
	case WFD_PORT_MODE_ROTATION_SUPPORT:
		value = WFD_ROTATION_SUPPORT_NONE;
		break;
	case WFD_PORT_MODE_INTERLACED:
		value = WFD_FALSE;
		break;
#if WFD_QNX_port_mode_info
	case WFD_PORT_MODE_PREFERRED_QNX:
		value = (mode->timing.flags & WFD_PREFERRED) != 0;
		break;
#endif
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	wfd_device_err_store(dev, wfderr);
	wfd_release_port_mode_ref(mode, __func__);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);
	TRACE_API_F("returning %d (wfderr=%#x)", value, wfderr);

	return value;
}

WFD_API_CALL WFDfloat WFD_APIENTRY
wfdGetPortModeAttribf(WFDDevice device_handle,
		      WFDPort port_handle,
		      WFDPortMode port_mode_handle,
		      WFDPortModeAttrib attr) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" port_mode_handle=%"PRIhdl
		    " attr=%#x", device_handle, port_handle, port_mode_handle, attr);

	WFDfloat value = NAN;
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;
	struct wfd_port_mode *mode;

	if (!wfd_find_device_port_mode_by_handle(device_handle, port_handle, port_mode_handle,
	                               &dev, &port, &mode, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return value;
	}

	TRACE_CALL("VP%d attr: %#x\n", port->wfd_port_id - 1, attr);

	switch ((int32_t)attr) {
	case WFD_PORT_MODE_REFRESH_RATE:
		value = (WFDfloat)wfd_calculate_port_mode_refresh(&mode->timing);
		break;
#if WFD_QNX_port_mode_info
	case WFD_PORT_MODE_ASPECT_RATIO_QNX:
		value = (WFDfloat)mode->timing.hpixels
				/ (WFDfloat)mode->timing.vlines;
		break;
#endif
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	wfd_device_err_store(dev, wfderr);
	wfd_release_port_mode_ref(mode, __func__);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);

	return value;
}

WFD_API_CALL WFDint WFD_APIENTRY
wfdGetPortAttribi(WFDDevice device_handle,
		  WFDPort port_handle,
		  WFDPortConfigAttrib attr) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" attr=%#x",
		    device_handle, port_handle, attr);

	WFDint value = -1;
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		SLOG_ERR("failed: %s", "bad handle(s)");
		return value;
	}

	TRACE_CALL("VP%d attr: %#x\n", port->wfd_port_id - 1, attr);

	lock_device(dev);

	switch (attr) {
	case WFD_PORT_ID:
		value = port->wfd_port_id;
		break;
	case WFD_PORT_TYPE:
		value = port->wfd_port_type;
		break;
	case WFD_PORT_DETACHABLE:
		value = WFD_FALSE;
		break;
	case WFD_PORT_ATTACHED:
		value = WFD_TRUE;
		break;
	case WFD_PORT_FILL_PORT_AREA:
		value = WFD_FALSE;
		break;
	case WFD_PORT_BACKGROUND_COLOR:
		value = 255;
		value |= (WFDint)port->pending.bg_b << 8;
		value |= (WFDint)port->pending.bg_g << 16;
		value |= (WFDint)port->pending.bg_r << 24;
		break;
	case WFD_PORT_FLIP:
	case WFD_PORT_MIRROR:
		value = WFD_FALSE;
		break;
	case WFD_PORT_ROTATION:
		value = 0;
		break;
	case WFD_PORT_POWER_MODE:
		value = WFD_POWER_MODE_ON;
		break;
	case WFD_PORT_PARTIAL_REFRESH_SUPPORT:
	case WFD_PORT_PARTIAL_REFRESH_ENABLE:
		value = WFD_PARTIAL_REFRESH_NONE;
		break;
	case WFD_PORT_PIPELINE_ID_COUNT:
		value = wfd_get_bindable_pipe_count(dev, port, NULL);
		break;
	case WFD_PORT_PROTECTION_ENABLE:
		value = WFD_FALSE;
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	unlock_device(dev);

	wfd_device_err_store(dev, wfderr);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);

	return value;
}

WFD_API_CALL WFDfloat WFD_APIENTRY
wfdGetPortAttribf(WFDDevice device_handle,
		  WFDPort port_handle,
		  WFDPortConfigAttrib attr) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" attr=%#x",
		    device_handle, port_handle, attr);

	WFDfloat value = NAN;
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return value;
	}

	switch (attr) {
	case WFD_PORT_GAMMA:
		value = FIXED_GAMMA_VALUE;
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	wfd_device_err_store(dev, wfderr);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);

	return value;
}

WFD_API_CALL void WFD_APIENTRY
wfdGetPortAttribiv(WFDDevice device_handle,
		   WFDPort port_handle,
		   WFDPortConfigAttrib attr,
		   WFDint count,
		   WFDint *value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" attr=%#x count=%d",
		    device_handle, port_handle, attr, count);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	if (count < 0 || (count > 0 && !value)) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	lock_device(dev);

	switch (attr) {
	case WFD_PORT_NATIVE_RESOLUTION:
		if (count == 2) {
			value[0] = 0;
			value[1] = 0;
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PORT_BACKGROUND_COLOR:
		if (count == 3) {
			value[0] = (WFDint)port->pending.bg_r;
			value[1] = (WFDint)port->pending.bg_g;
			value[2] = (WFDint)port->pending.bg_b;
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PORT_PARTIAL_REFRESH_RECTANGLE:
		if (count == 4) {
			value[0] = 0;
			value[1] = 0;
			value[2] = 0;
			value[3] = 0;
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PORT_PARTIAL_REFRESH_MAXIMUM:
		if (count == 2) {
			value[0] = 0;
			value[1] = 0;
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PORT_BINDABLE_PIPELINE_IDS:
#if !WFD_STRICT
		while (count > wfd_get_bindable_pipe_count(dev, port, NULL)) {
			value[--count] = WFD_INVALID_PIPELINE_ID;
		}
#endif
		if (count == wfd_get_bindable_pipe_count(dev, port, NULL)) {
			wfd_get_bindable_pipe_count(dev, port, value);
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	unlock_device(dev);
out:
	wfd_device_err_store(dev, wfderr);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdGetPortAttribfv(WFDDevice device_handle,
                   WFDPort port_handle,
                   WFDPortConfigAttrib attr,
                   WFDint count,
                   WFDfloat *value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" attr=%#x count=%d",
			device_handle, port_handle, attr, count);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	if (count < 0 || (count > 0 && !value)) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	lock_device(dev);

	switch (attr) {
	case WFD_PORT_PHYSICAL_SIZE:  // in mm
		if (count == 2) {
			int mmsize[] = {0, 0};  // size unknown

			// TOOD: fill from cfglib if possible

			value[0] = (WFDfloat)mmsize[0];  // width
			value[1] = (WFDfloat)mmsize[1];  // height
			DEBUG2("return phys size %dx%d mm", mmsize[0], mmsize[1]);
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PORT_BACKGROUND_COLOR:
		if (count == 3) {
			value[0] = component_int_convert_float(port->pending.bg_r);
			value[1] = component_int_convert_float(port->pending.bg_g);
			value[2] = component_int_convert_float(port->pending.bg_b);
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PORT_GAMMA_RANGE:
		if (count == 2) {
			value[0] = FIXED_GAMMA_VALUE;
			value[1] = FIXED_GAMMA_VALUE;
			// Equal values indicate gamma correction isn't supported.
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	default:
		// Unknown attribute, or not accessible via this function.
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	unlock_device(dev);
out:
	wfd_device_err_store(dev, wfderr);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdSetPortAttribi(WFDDevice device_handle,
		  WFDPort port_handle,
		  WFDPortConfigAttrib attr,
		  WFDint value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" attr=%#x value=0x%x",
			device_handle, port_handle, attr, value);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	TRACE_CALL("VP%d attr: %#x\n", port->wfd_port_id - 1, attr);

	lock_device(dev);

	switch (attr) {
	case WFD_PORT_BACKGROUND_COLOR:
		if ((value & 255) == 255) {
			const uint32_t *v = (const uint32_t*)&value;
			port->pending.bg_r = (uint8_t)((*v >> 24) & 255);
			port->pending.bg_g = (uint8_t)((*v >> 16) & 255);
			port->pending.bg_b = (uint8_t)((*v >> 8) & 255);
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PORT_FLIP:
	case WFD_PORT_MIRROR:
		if (value != WFD_FALSE)
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		break;
	case WFD_PORT_ROTATION:
		if (value != 0)
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		break;
	case WFD_PORT_POWER_MODE:
		if (value != WFD_POWER_MODE_ON)
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		break;
	case WFD_PORT_PROTECTION_ENABLE:
		if (value != WFD_FALSE)
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	unlock_device(dev);

	wfd_device_err_store(dev, wfderr);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdSetPortAttribf(WFDDevice device_handle,
		  WFDPort port_handle,
		  WFDPortConfigAttrib attr,
		  WFDfloat value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" attr=%#x value=%f",
		    device_handle, port_handle, attr, value);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	switch (attr) {
	case WFD_PORT_GAMMA:
		if (value != FIXED_GAMMA_VALUE)
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	wfd_device_err_store(dev, wfderr);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdSetPortAttribiv(WFDDevice device_handle,
		   WFDPort port_handle,
		   WFDPortConfigAttrib attr,
		   WFDint count,
		   const WFDint *value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" attr=%#x count=%d",
			device_handle, port_handle, attr, count);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	if (count < 0 || (count > 0 && !value)) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	lock_device(dev);

	switch (attr) {
	case WFD_PORT_BACKGROUND_COLOR:
		if (count == 3 && component_int_valid(value[0])
			       && component_int_valid(value[1])
			       && component_int_valid(value[2])) {
			port->pending.bg_r = (uint8_t)value[0];
			port->pending.bg_g = (uint8_t)value[1];
			port->pending.bg_b = (uint8_t)value[2];
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PORT_PARTIAL_REFRESH_RECTANGLE:
		if (count == 4 && value[0] == 0 && value[1] == 0
			       && value[2] == 0 && value[3] == 0) {
			/* do nothing */
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	unlock_device(dev);
out:
	wfd_device_err_store(dev, wfderr);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdSetPortAttribfv(WFDDevice device_handle,
		   WFDPort port_handle,
		   WFDPortConfigAttrib attr,
		   WFDint count,
		   const WFDfloat *value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" attr=%#x count=%d",
		    device_handle, port_handle, attr, count);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	if (count < 0 || (count > 0 && !value)) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	lock_device(dev);

	switch (attr) {
	case WFD_PORT_BACKGROUND_COLOR:
		if (count == 3 && component_float_valid(value[0])
			       && component_float_valid(value[1])
			       && component_float_valid(value[2])) {
			port->pending.bg_r = (uint8_t)component_float_convert_int(value[0]);
			port->pending.bg_g = (uint8_t)component_float_convert_int(value[1]);
			port->pending.bg_b = (uint8_t)component_float_convert_int(value[2]);
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	unlock_device(dev);
out:
	wfd_device_err_store(dev, wfderr);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdBindPipelineToPort(WFDDevice device_handle,
		      WFDPort port_handle,
		      WFDPipeline pipeline_handle) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" pipeline_handle=%"PRIhdl,
		    device_handle, port_handle, pipeline_handle);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_port *port;
	struct wfd_pipe *pipe;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		SLOG_ERR("failed: %s", "bad handle(s)");
		return;
	}
	assert(port);

	pipe = wfd_find_pipe_by_handle(pipeline_handle, __func__);
	if (!pipe || pipe->dev != dev) {
		wfderr = WFD_ERROR_BAD_HANDLE;
		goto out;
	}

	TRACE_CALL("pipe=%d(%s) VP%d", pipe->wfd_pipe_id , vop2_get_pipeline_name(pipe), port->wfd_port_id - 1);

	lock_device(dev);
	if (wfd_port_layer_order_get(port, pipe) == WFD_INVALID_PIPELINE_LAYER)
		wfderr = WFD_ERROR_BAD_HANDLE;
	else
		wfd_bind_pipe_to_port(pipe, port);
	unlock_device(dev);
out:
	wfd_device_err_store(dev, wfderr);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);
	TRACE_API_F("returning; wfderr=%#x", wfderr);
}

WFD_API_CALL WFDint WFD_APIENTRY
wfdGetDisplayDataFormats(WFDDevice device_handle,
			 WFDPort port_handle,
			 WFDDisplayDataFormat *format,
			 WFDint format_count) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" count=%d",
		    device_handle, port_handle, format_count);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	WFDint found = 0;
	struct wfd_device *dev;
	struct wfd_port *port;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return 0;
	}
	lock_device(dev);

	if (format_count <= 0 && format) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

out:
	unlock_device(dev);
	wfd_device_err_store(dev, wfderr);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);
	return found;
}

WFD_API_CALL WFDint WFD_APIENTRY
wfdGetDisplayData(WFDDevice device_handle,
		  WFDPort port_handle,
		  WFDDisplayDataFormat format,
		  WFDuint8 *data,
		  WFDint data_count) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" format=%#x",
		    device_handle, port_handle, format);

	struct wfd_device *dev;
	struct wfd_port *port;
	WFDint copied_len = 0;

	if (!wfd_find_device_port_by_handle(device_handle, port_handle, &dev, &port, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return copied_len;
	}

	wfd_device_err_store(dev, WFD_ERROR_NOT_SUPPORTED);
	wfd_port_ref_release(port, __func__);
	wfd_device_ref_release(dev, __func__);

	return copied_len;
}

/*** Standard API functions end ***/
