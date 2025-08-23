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
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "wfddev.h"
#include "dss_service.h"

bool wfd_find_deivce_pipeline_by_handle(WFDDevice device_handle, WFDPipeline pipeline_handle,
					struct wfd_device **pdevice, struct wfd_pipe **ppipe,
					const void *refkey)
{
	struct wfd_device *dev;
	struct wfd_pipe *pipe;
	assert(pdevice);
	assert(ppipe);

	TRACE_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl, device_handle, pipeline_handle);

	dev = wfd_find_deivce_by_handle(device_handle, refkey);
	if (dev) {
		pipe = wfd_find_pipe_by_handle(pipeline_handle, refkey);
		if (pipe) {
			if (pipe->dev == dev) {
				*pdevice = dev;
				*ppipe = pipe;
				TRACE_F("returning dev=%p pipe=%p", dev, pipe);
				return true;
			}
			DEBUG2("%s: pipe handle %"PRIhdl" refers to wrong device"
						" (found dev handle %"PRIhdl", wanted %"PRIhdl")",
					__func__, pipeline_handle, pipe->dev->handle, device_handle);
			wfd_pipe_ref_release(pipe, refkey);
		}
		wfd_device_err_store(dev, WFD_ERROR_BAD_HANDLE);
		wfd_device_ref_release(dev, refkey);
	}

	TRACE_F("fail");

	return false;
}

void wfd_pipe_ref_release(struct wfd_pipe *pipe, const void *refkey)
{
	TRACE_F("pipe=%p", pipe);
	if (pipe) {
		if (refcount_dec_value(&pipe->ref, refkey) == 0)
			wfd_hdl_send_ref_zero_signal();
	}
}

void wfd_pipe_ref_replace(struct wfd_pipe **slot, struct wfd_pipe *pipe, const void *refkey)
{
	if (*slot != pipe) {
		if (*slot) {
			wfd_pipe_ref_release(*slot, refkey);
			*slot = NULL;
		}
		if (pipe) {
			refcount_inc(&pipe->ref, refkey);
			*slot = pipe;
		}
	}
}

static bool wfd_pipe_transparency_valid(const struct wfd_pipe *pipe, WFDbitfield value)
{
	assert(pipe);
	bool valid = (value == WFD_TRANSPARENCY_NONE);

	if (value == WFD_TRANSPARENCY_SOURCE_ALPHA)
		valid = true;

	if (value == WFD_TRANSPARENCY_SOURCE_COLOR)
		valid = true;

	return valid;
}

static bool wfd_rect_attr_valid(struct wfd_pipe *pipe, const WFDint *value)
{
	assert(pipe);
	assert(value);
	bool valid = true && value[0] >= 0 && value[1] >= 0 && value[2] >= 0 && value[3] >= 0 &&
		     value[0] <= pipe->max_width && value[1] <= pipe->max_height &&
		     value[2] <= pipe->max_width && value[3] <= pipe->max_height &&
		     (value[0] + value[2]) <= pipe->max_width &&
		     (value[1] + value[3]) <= pipe->max_height;

	return valid;
}

WFDErrorCode wfd_bind_source_to_pipe(struct wfd_pipe *pipe, struct wfd_source *src,
				     WFDTransition transition, const WFDRect *region)
{
	assert_device_locked(pipe->dev);
	WFDErrorCode wfderr = WFD_ERROR_NONE;

#if WFD_STRICT
	if (src && src->pipe != pipe) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		DEBUG1("%s: bad pipe (source created for pipe id %d"
		       ", but trying to bind to pipe id %d)",
		       __func__, src->pipe->wfd_pipe_id,
		       pipe->wfd_pipe_id);
		goto out;
	}
#endif

	if (transition != WFD_TRANSITION_IMMEDIATE && transition != WFD_TRANSITION_AT_VSYNC) {
		DEBUG1("%s: bad transition %#x", __func__, transition);
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	if (src != pipe->pending.src)
		wfd_source_ref_replace(&pipe->pending.src, src, pipe);

	pipe->src_transition = transition;

out:
	return wfderr;
}

struct wfd_port *wfd_pipe_get_commit_port(const struct wfd_pipe *pipe)
{

	struct wfd_port *commit_port = NULL;

	if (pipe->committed.port)
		commit_port = pipe->committed.port;
	else if (pipe->pending.port)
		commit_port = pipe->pending.port;

	return commit_port;
}

WFDErrorCode wfd_pipe_commit(struct wfd_pipe *pipe, struct wfd_commit_state *commit_state)
{
	assert(commit_state);
	struct wfd_port *port;
	struct timespec vblank_ts;

	DEBUG1("%s commit_state: %p device=%p pipe: %s(%"PRIhdl") transition: 0x%x for port=%d",
	       __func__, commit_state, commit_state->dev, vop2_get_pipeline_name(pipe), pipe->handle, pipe->src_transition,
	       wfd_pipe_get_commit_port(pipe)->wfd_port_id);

	if (pipe->src_transition != WFD_TRANSITION_IMMEDIATE) {
		port = wfd_pipe_get_commit_port(pipe);
		commit_state->pipe_idx_mask |= (uint64_t)1 << pipe->index;

		if (wfd_port_next_vblank_get(&vblank_ts, port, &commit_state->start_ts)) {
			DEBUG2("vblank: %ld:%ld  \n", vblank_ts.tv_sec, vblank_ts.tv_nsec);
			wfd_device_vblank_update(commit_state, &vblank_ts);
			wfd_port_vblank_update(commit_state, port, &vblank_ts);
		}
	}

	return WFD_ERROR_NONE;
}

void wfd_pipe_commit_update(struct wfd_pipe *pipe)
{
	assert_device_locked(pipe->dev);

	wfd_source_ref_replace(&pipe->committed.src, pipe->pending.src, pipe);
	wfd_port_ref_replace(&pipe->committed.port, pipe->pending.port, pipe);
	pipe->committed = pipe->pending;
}

int wfd_pipe_commit_flush(struct wfd_pipe *pipe, const struct wfd_commit_state *commit_state)
{
	assert_device_locked(pipe->dev);

	dss_plane_update(pipe);

	return EOK;
}

void wfd_pipe_handles_invalidate(struct wfd_pipe *pipe)
{
	assert(pipe);
	assert_device_locked(pipe->dev);
	struct wfd_source *src;

	if (pipe->handle != WFD_INVALID_HANDLE) {
		DEBUG2("Freeing pipe handle %"PRIhdl" (for pipe %p)",
				pipe->handle, pipe);
		wfd_pipe_free_handle(pipe);
	}

	while ((src = LIST_FIRST(&pipe->srclist)) != NULL) {
		LIST_REMOVE(src, srclist_entry);
		wfd_set_source_unused(pipe->dev, src);
		assert(LIST_FIRST(&pipe->srclist) != src);
	}
}

void wfd_pipe_ref_clear(struct wfd_pipe *pipe)
{
	assert_device_locked(pipe->dev);

	wfd_bind_pipe_to_port(pipe, NULL);
	wfd_bind_source_to_pipe(pipe, NULL, WFD_TRANSITION_IMMEDIATE, NULL);

	wfd_pipe_commit_update(pipe);
}

static struct wfd_pipe *wfd_find_pipe_by_id(struct wfd_device *dev, WFDint pipe_id)
{
	struct wfd_pipe *pipe = NULL, *tmp;

	SLIST_FOREACH(tmp, &dev->pipe_list, dev_pipelist_entry) {
		if (tmp->wfd_pipe_id == pipe_id) {
			assert(!pipe);
			pipe = tmp;
		}
	}

	return pipe;
}


/*** Standard API functions start ***/

WFD_API_CALL WFDint WFD_APIENTRY
wfdEnumeratePipelines(WFDDevice device_handle,
		      WFDint *pipeline_ids,
		      WFDint pipeline_ids_count,
		      const WFDint *filter_list) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_ids=%p pipeline_ids_count=%d",
		    device_handle, pipeline_ids, pipeline_ids_count);

	struct wfd_device *dev = wfd_find_deivce_by_handle(device_handle, __func__);
	WFDint found = 0;
	if (!dev) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return 0;
	}

	if (pipeline_ids_count < 0 || (pipeline_ids && !pipeline_ids_count)) {
		wfd_device_err_store(dev, WFD_ERROR_ILLEGAL_ARGUMENT);
	} else if (filter_list && *filter_list != WFD_NONE) {
		/* do nothing */
	} else {
		struct wfd_pipe *pipe;
		lock_device(dev);
		SLIST_FOREACH(pipe, &dev->pipe_list, dev_pipelist_entry) {
			if (pipeline_ids) {
				if (found >= pipeline_ids_count) break;
				pipeline_ids[found] = pipe->wfd_pipe_id;
			}
			++found;
		}
		unlock_device(dev);
	}

	TRACE_API_F("returning found=%d", found);
	wfd_device_ref_release(dev, __func__);

	return found;
}

WFD_API_CALL WFDPipeline WFD_APIENTRY
wfdCreatePipeline(WFDDevice device_handle,
		  WFDint pipe_id,
		  const WFDint *attr_list) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipe_id=%d", device_handle, pipe_id);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	WFDPipeline pipeline_handle;
	struct wfd_device *dev;
	struct wfd_pipe *pipe = NULL;

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
	pipe = wfd_find_pipe_by_id(dev, pipe_id);
	if (!pipe) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
	} else if (pipe->in_use) {
		pipe = NULL;
		wfderr = WFD_ERROR_IN_USE;
	} else {
		pipe->in_use = true;
		refcount_inc(&dev->ref, pipe);
	}
	unlock_device(dev);

	if (wfderr)
		goto out;

	if (!wfd_pipe_bind_handle(pipe))
		wfderr = WFD_ERROR_OUT_OF_MEMORY;

out:
	if (wfderr == WFD_ERROR_NONE) {
		assert(pipe);
		assert(pipe->handle != WFD_INVALID_HANDLE);
	} else {
		if (pipe) {
			assert(pipe->in_use);

			lock_device(dev);
			pipe->in_use = false;
			wfd_device_ref_release(dev, pipe);
			unlock_device(dev);
		}
		pipe = NULL;
		wfd_device_err_store(dev, wfderr);
	}

	pipeline_handle = pipe ? pipe->handle : WFD_INVALID_HANDLE;
	TRACE_CALL("returning pipe=%p(%s) with handle %"PRIhdl, pipe, vop2_get_pipeline_name(pipe), pipeline_handle);
	wfd_device_ref_release(dev, __func__);

	return pipeline_handle;
}

WFD_API_CALL void WFD_APIENTRY
wfdDestroyPipeline(WFDDevice device_handle,
		   WFDPipeline pipeline_handle) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl, device_handle, pipeline_handle);

	struct wfd_device *dev;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		SLOG_ERR("failed: %s", "bad handle(s)");
		return;
	}

	lock_device(pipe->dev);
	wfd_pipe_handles_invalidate(pipe);
	wfd_pipe_ref_clear(pipe);
	wfd_device_sources_destroy(pipe->dev);
	unlock_device(pipe->dev);

	TRACE_CALL("%s succeeded", vop2_get_pipeline_name(pipe));

	wfd_pipe_ref_release(pipe, __func__);
	wfd_hdl_wait_refcount_release(&pipe->ref);

	assert(pipe->in_use);

	lock_device(dev);
	pipe->in_use = false;
	wfd_device_ref_release(dev, pipe);
	unlock_device(dev);

	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL WFDint WFD_APIENTRY
wfdGetPipelineLayerOrder(WFDDevice device_handle,
			 WFDPort port_handle,
			 WFDPipeline pipeline_handle) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" port_handle=%"PRIhdl" pipeline_handle=%"PRIhdl,
		    device_handle, port_handle, pipeline_handle);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	WFDint ret = WFD_INVALID_PIPELINE_LAYER;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;
	struct wfd_port *port = NULL;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return ret;
	}

	port = wfd_find_port_by_handle(port_handle, __func__);
	if (!port || port->dev != dev) {
		wfderr = WFD_ERROR_BAD_HANDLE;
		goto out;
	}

	lock_device(dev);
	ret = wfd_port_layer_order_get(port, pipe);
	if (ret == WFD_INVALID_PIPELINE_LAYER)
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
	unlock_device(dev);
	TRACE_CALL("%s zpos: %d VP%d", vop2_get_pipeline_name(pipe), ret, port->wfd_port_id - 1);
out:
	wfd_device_err_store(dev, wfderr);
	wfd_port_ref_release(port, __func__);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);

	return ret;
}

WFD_API_CALL WFDint WFD_APIENTRY
wfdGetPipelineAttribi(WFDDevice device_handle,
		      WFDPipeline pipeline_handle,
		      WFDPipelineConfigAttrib attr) WFD_APIEXIT
{
	WFDint value = -1;
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return value;
	}

	TRACE_CALL("%s attr=%#x", vop2_get_pipeline_name(pipe), attr);

	lock_device(dev);

	switch (attr) {
	case WFD_PIPELINE_ID:
		value = pipe->wfd_pipe_id;
		break;
	case WFD_PIPELINE_PORTID:
		if (pipe->pending.port)
			value = pipe->pending.port->wfd_port_id;
		else
			value = WFD_INVALID_PORT_ID;
		break;
	case WFD_PIPELINE_LAYER: 
		if (pipe->pending.port)
			value = wfd_port_layer_order_get(pipe->pending.port, pipe);
		else
			value = WFD_INVALID_PIPELINE_LAYER;
		DEBUG2("get WFD_PIPELINE_LAYER=%d for pipeline ID %d", value, pipe->wfd_pipe_id);
		break;
	case WFD_PIPELINE_SHAREABLE:
		value = WFD_FALSE;
		break;
	case WFD_PIPELINE_DIRECT_REFRESH:
		value = WFD_TRUE;
		break;
	case WFD_PIPELINE_FLIP:
		value = pipe->pending.flip;
		break;
	case WFD_PIPELINE_MIRROR:
		value = pipe->pending.mirror;
		break;
	case WFD_PIPELINE_ROTATION_SUPPORT:
		value = WFD_ROTATION_SUPPORT_NONE;
		break;
	case WFD_PIPELINE_ROTATION:
		value = pipe->pending.rotation;
		break;
	case WFD_PIPELINE_SCALE_FILTER:
		value = WFD_SCALE_FILTER_NONE;
		break;
	case WFD_PIPELINE_TRANSPARENCY_ENABLE:
		value = pipe->pending.transparency;
		break;
	case WFD_PIPELINE_GLOBAL_ALPHA:
		value = pipe->pending.global_alpha;
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	unlock_device(dev);

	wfd_device_err_store(dev, wfderr);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);
	TRACE_API_F("returning %#x (wfderr %#x)", value, wfderr);

	return value;
}

WFD_API_CALL WFDfloat WFD_APIENTRY
wfdGetPipelineAttribf(WFDDevice device_handle,
		      WFDPipeline pipeline_handle,
		      WFDPipelineConfigAttrib attr) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl" attr=%#x",
		    device_handle, pipeline_handle, attr);

	WFDfloat value = NAN;
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return value;
	}

	TRACE_CALL("%s attr=%#x", vop2_get_pipeline_name(pipe), attr);

	lock_device(dev);

	switch (attr) {
	case WFD_PIPELINE_GLOBAL_ALPHA:
		value = (WFDfloat)pipe->pending.global_alpha / (WFDfloat)255;
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	unlock_device(dev);

	wfd_device_err_store(dev, wfderr);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);

	return value;
}

WFD_API_CALL void WFD_APIENTRY
wfdGetPipelineAttribiv(WFDDevice device_handle,
		       WFDPipeline pipeline_handle,
		       WFDPipelineConfigAttrib attr,
		       WFDint count,
		       WFDint *value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl" attr=%#x count=%d",
		    device_handle, pipeline_handle, attr, count);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	if (!value || count <= 0) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	TRACE_CALL("%s attr=%#x", vop2_get_pipeline_name(pipe), attr);

	lock_device(dev);

	switch (attr) {
	case WFD_PIPELINE_MAX_SOURCE_SIZE:
		if (count == 2) {
			value[0] = pipe->max_width;
			value[1] = pipe->max_height;
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PIPELINE_SOURCE_RECTANGLE:
		if (count == 4) {
			value[0] = pipe->pending.src_rect.offsetX;
			value[1] = pipe->pending.src_rect.offsetY;
			value[2] = pipe->pending.src_rect.width;
			value[3] = pipe->pending.src_rect.height;
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PIPELINE_DESTINATION_RECTANGLE:
		if (count == 4) {
			value[0] = pipe->pending.dst_rect.offsetX;
			value[1] = pipe->pending.dst_rect.offsetY;
			value[2] = pipe->pending.dst_rect.width;
			value[3] = pipe->pending.dst_rect.height;
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
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdGetPipelineAttribfv(WFDDevice device_handle,
		       WFDPipeline pipeline_handle,
		       WFDPipelineConfigAttrib attr,
		       WFDint count,
		       WFDfloat *value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl" attr=%#x count=%d",
		    device_handle, pipeline_handle, attr, count);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	if (!value || count <= 0) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	TRACE_CALL("%s attr=%#x", vop2_get_pipeline_name(pipe), attr);

	lock_device(dev);

	switch (attr) {
	case WFD_PIPELINE_MAX_SOURCE_SIZE:
		if (count == 2) {
			value[0] = (WFDfloat)pipe->max_width;
			value[1] = (WFDfloat)pipe->max_height;
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PIPELINE_SCALE_RANGE:
		value[0] = 0.125f;
		value[1] = 8.0f;
		break;
	case WFD_PIPELINE_SOURCE_RECTANGLE:
		if (count == 4) {
			value[0] = (WFDfloat)pipe->pending.src_rect.offsetX;
			value[1] = (WFDfloat)pipe->pending.src_rect.offsetY;
			value[2] = (WFDfloat)pipe->pending.src_rect.width;
			value[3] = (WFDfloat)pipe->pending.src_rect.height;
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PIPELINE_DESTINATION_RECTANGLE:
		if (count == 4) {
			value[0] = (WFDfloat)pipe->pending.dst_rect.offsetX;
			value[1] = (WFDfloat)pipe->pending.dst_rect.offsetY;
			value[2] = (WFDfloat)pipe->pending.dst_rect.width;
			value[3] = (WFDfloat)pipe->pending.dst_rect.height;
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
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdSetPipelineAttribi(WFDDevice device_handle,
		      WFDPipeline pipeline_handle,
		      WFDPipelineConfigAttrib attr,
		      WFDint value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl" attr=%#x value=%d",
			device_handle, pipeline_handle, attr, value);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		SLOG_ERR("failed: %s", "bad handle(s)");
		return;
	}

	TRACE_CALL("%s attr=%#x value: 0x%d", vop2_get_pipeline_name(pipe), attr, value);

	lock_device(dev);

	switch (attr) {
	case WFD_PIPELINE_FLIP:
		pipe->pending.flip = (value != WFD_FALSE);
		break;
	case WFD_PIPELINE_MIRROR:
		pipe->pending.mirror = (value != WFD_FALSE);
		break;
	case WFD_PIPELINE_ROTATION:
		switch (value) {
		case 0:
		case 90:
		case 180:
		case 270:
			pipe->pending.rotation = value;
			break;
		default:
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
			break;
		}
		break;
	case WFD_PIPELINE_SCALE_FILTER:
		if (value != WFD_SCALE_FILTER_NONE)
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		break;
	case WFD_PIPELINE_TRANSPARENCY_ENABLE:
#if WFD_STRICT
		if (!wfd_pipe_transparency_valid(pipe, (WFDbitfield)value))
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
#endif
		if (wfderr == WFD_ERROR_NONE)
			pipe->pending.transparency = value;
		break;
	case WFD_PIPELINE_GLOBAL_ALPHA:
		if (value >= 0 && value <= 255)
			pipe->pending.global_alpha = (uint8_t)value;
		else
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	unlock_device(dev);

	wfd_device_err_store(dev, wfderr);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdSetPipelineAttribf(WFDDevice device_handle,
		      WFDPipeline pipeline_handle,
		      WFDPipelineConfigAttrib attr,
		      WFDfloat value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl" attr=%#x value=%f",
		    device_handle, pipeline_handle, attr, value);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	TRACE_CALL("%s attr=%#x", vop2_get_pipeline_name(pipe), attr);

	switch (attr) {
	case WFD_PIPELINE_GLOBAL_ALPHA:
		if (value >= 0 && value <= (WFDfloat)1)
			pipe->pending.global_alpha = (uint8_t)(value * 255);
		else
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	wfd_device_err_store(dev, wfderr);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdSetPipelineAttribiv(WFDDevice device_handle,
		       WFDPipeline pipeline_handle,
		       WFDPipelineConfigAttrib attr,
		       WFDint count,
		       const WFDint *value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl" attr=%#x count=%d",
		   device_handle, pipeline_handle, attr, count);

#if !WFD_STRICT
	switch (attr) {
	case WFD_PIPELINE_TRANSPARENCY_ENABLE:
	case WFD_PIPELINE_GLOBAL_ALPHA:
		if (count == 1 && value != NULL) {
			wfdSetPipelineAttribi(device_handle, pipeline_handle, attr, value[0]);
			return;
		}
		break;
	default:
		break;
	}
#endif

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__))
		return;

	if (!value || count <= 0) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	TRACE_CALL("%s attr=%#x", vop2_get_pipeline_name(pipe), attr);

	lock_device(dev);

	switch (attr) {
	case WFD_PIPELINE_SOURCE_RECTANGLE:
		if (count == 4 && wfd_rect_attr_valid(pipe, value)) {
			pipe->pending.src_rect.offsetX = value[0];
			pipe->pending.src_rect.offsetY = value[1];
			pipe->pending.src_rect.width = value[2];
			pipe->pending.src_rect.height = value[3];
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		DEBUG1("%s %s src pos[%d, %d], rect[%d x %d]",
		       __func__, vop2_get_pipeline_name(pipe), value[0], value[1], value[2],value[3]);
		break;
	case WFD_PIPELINE_DESTINATION_RECTANGLE:
		if (count == 4 && wfd_rect_attr_valid(pipe, value)) {
			pipe->pending.dst_rect.offsetX = value[0];
			pipe->pending.dst_rect.offsetY = value[1];
			pipe->pending.dst_rect.width = value[2];
			pipe->pending.dst_rect.height = value[3];
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		DEBUG1("%s %s dst pos[%d, %d], rect[%d x %d]",
		       __func__, vop2_get_pipeline_name(pipe), value[0], value[1], value[2],value[3]);

		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

	unlock_device(dev);
out:
	wfd_device_err_store(dev, wfderr);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdSetPipelineAttribfv(WFDDevice device_handle,
		       WFDPipeline pipeline_handle,
		       WFDPipelineConfigAttrib attr,
		       WFDint count,
		       const WFDfloat *value) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl" attr=%#x count=%d",
		    device_handle, pipeline_handle, attr, count);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;
	bool valid;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	if (!value || count <= 0) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	TRACE_CALL("%s attr=%#x", vop2_get_pipeline_name(pipe), attr);

	switch (attr) {
	case WFD_PIPELINE_SOURCE_RECTANGLE:
		valid = false;
		if (count == 4) {
			const WFDint irect[] = {
				(WFDint)value[0], (WFDint)value[1],
				(WFDint)value[2], (WFDint)value[3]
			};
			if (wfd_rect_attr_valid(pipe, &irect[0])) {
				pipe->pending.src_rect.offsetX = irect[0];
				pipe->pending.src_rect.offsetY = irect[1];
				pipe->pending.src_rect.width = irect[2];
				pipe->pending.src_rect.height = irect[3];
			}
		}
		if (!valid) {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	case WFD_PIPELINE_DESTINATION_RECTANGLE:
		valid = false;
		if (count == 4) {
			const WFDint irect[] = {
				(WFDint)value[0], (WFDint)value[1],
				(WFDint)value[2], (WFDint)value[3]
			};
			if (wfd_rect_attr_valid(pipe, &irect[0])) {
				pipe->pending.dst_rect.offsetX = irect[0];
				pipe->pending.dst_rect.offsetY = irect[1];
				pipe->pending.dst_rect.width = irect[2];
				pipe->pending.dst_rect.height = irect[3];
			}
		}
		if (!valid) {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}

out:
	wfd_device_err_store(dev, wfderr);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL WFDint WFD_APIENTRY
wfdGetPipelineTransparency(WFDDevice device_handle,
			   WFDPipeline pipeline_handle,
			   WFDbitfield *trans,
			   WFDint transCount) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl" count=%d",
		    device_handle, pipeline_handle, transCount);

	struct wfd_device *dev;
	struct wfd_pipe *pipe;
	WFDint found = 0;
	WFDbitfield bf;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		SLOG_ERR("failed: %s", "bad handle(s)");
		return 0;
	}

	if (transCount < 0 || (trans && transCount == 0)) {
		wfd_device_err_store(dev, WFD_ERROR_ILLEGAL_ARGUMENT);
		goto out;
	}

	for (bf = 0; bf < WFD_TRANSPARENCY_MASK; ++bf) {
		if (wfd_pipe_transparency_valid(pipe, bf)) {
			if (trans) {
				if (found >= transCount) break;
				trans[found] = bf;
			}
			++found;
		}
	}
out:
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);
	return found;
}

WFD_API_CALL void WFD_APIENTRY
wfdSetPipelineTSColor(WFDDevice device_handle,
		      WFDPipeline pipeline_handle,
		      WFDTSColorFormat color_format,
		      WFDint count,
		      const void *color) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl" fmt=%#x count=%d",
		    device_handle, pipeline_handle, color_format, count);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	if (!color) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	if (!wfd_pipe_transparency_valid(pipe, WFD_TRANSPARENCY_SOURCE_COLOR)) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	TRACE_CALL("%s color_format=%#x", vop2_get_pipeline_name(pipe), color_format);

	lock_device(dev);

	switch (color_format) {
	case WFD_TSC_FORMAT_UINT8_RGB_8_8_8_LINEAR:
		if (count == 3) {
			const uint8_t *ch = color;
			unsigned r = ch[0], g = ch[1], b = ch[2];
			pipe->pending.tscolor = (r << 16) | (g << 8) | b;
		} else {
			wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		}
		break;
	default:
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		break;
	}

	unlock_device(dev);
out:
	wfd_device_err_store(dev, wfderr);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdBindSourceToPipeline(WFDDevice device_handle,
			WFDPipeline pipeline_handle,
			WFDSource source_handle,
			WFDTransition transition,
			const WFDRect *region) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl" source_handle=%"PRIhdl
		    " transition=%#x", device_handle, pipeline_handle, source_handle, transition);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;
	struct wfd_source *src = NULL;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	if (source_handle != WFD_INVALID_HANDLE) {
		src = wfd_find_source_by_handle(source_handle, __func__);
		if (!src) {
			wfderr = WFD_ERROR_BAD_HANDLE;
			goto out;
		}
	}

	lock_device(dev);
	wfderr = wfd_bind_source_to_pipe(pipe, src, transition, region);
	unlock_device(dev);
out:
	TRACE_CALL("%s src: %p oldsrc: %p done, wfderr=%#x", vop2_get_pipeline_name(pipe), src, pipe->pending.src, wfderr);
	wfd_device_err_store(dev, wfderr);
	wfd_source_ref_release(src, __func__);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL void WFD_APIENTRY
wfdBindMaskToPipeline(WFDDevice device_handle,
		      WFDPipeline pipeline_handle,
		      WFDMask mask_handle,
		      WFDTransition transition) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" pipeline_handle=%"PRIhdl" mask_handle=%"PRIhdl
			" transition=%#x", device_handle, pipeline_handle, mask_handle, transition);

	struct wfd_device *dev = NULL;
	struct wfd_pipe *pipe = NULL;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &dev, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	TRACE_CALL("%s", vop2_get_pipeline_name(pipe));

	wfd_device_err_store(dev, WFD_ERROR_NOT_SUPPORTED);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(dev, __func__);
}

/*** Standard API functions end ***/
