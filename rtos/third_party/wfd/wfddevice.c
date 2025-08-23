/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#if defined(__RT_THREAD__)
#include <stdatomic.h>
#else
#include <atomic.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/trace.h>
#include <sys/types.h>
#endif

#include <drm/drm_util.h>
#include <drm/connector.h>

#include "wfddev.h"
#include "dss_service.h"

#if defined(__RT_THREAD__)
#  define EXT_IMG  WFD_EXT(WFD_EXTNAME_WFD_EGL_IMAGES)
#endif

#define EXTLIST \
	EXT_IMG \
	/**/

#define WFD_EXT(x) { .name=(x "\0") },
static const struct { char name[24]; }
		extension_list[] = { EXTLIST };
#undef WFD_EXT

// prototypes
static void wfd_destory_device(struct wfd_device* dev);
static int wfd_add_port(struct wfd_device *dev, WFDint wfd_port_id);
static int wfd_add_pipe(struct wfd_device *dev, WFDint wfd_pipe_id);
static int wfd_create_port(struct wfd_device* dev);
static int wfd_create_pipe(struct wfd_device* dev);
static void wfd_free_port(struct wfd_port* port);
static void wfd_free_pipe(struct wfd_pipe* pipe);

static int wfd_create_device(struct wfd_device **pdevice, WFDint id)
{
	assert(pdevice);

	struct wfd_device *dev = NULL;
	pthread_mutexattr_t mutexattr;
	pthread_condattr_t condattr;
	bool mutex_init_attr;
	bool cond_init_attr;
	uint64_t tmp;
	int err = EOK;

	// check whether rtc is exit
	err = ClockTime_r(CLOCK_MONOTONIC, NULL, &tmp);
	if (err)
		goto out;

	dev = dss_alloc_wfd_device(id);
	if (!dev) {
		err = ENOMEM;
		slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed: %s", "malloc", strerror(err));
		goto out;
	}

	err = pthread_mutexattr_init(&mutexattr);
	mutex_init_attr = (err == EOK);
#ifndef NDEBUG
	if (!err)
		err = pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK);
#endif
#if defined(__QNXNTO__)
	if (!err)
		err = pthread_mutexattr_setwakeup_np(&mutexattr, PTHREAD_WAKEUP_ENABLE);
#endif
	if (!err) {
		err = pthread_mutex_init(&dev->lock, &mutexattr);
		if (!err)
			dev->init_lock = true;
	}

	if (mutex_init_attr) {
		pthread_mutexattr_destroy(&mutexattr);
	}
	if (err) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed: %s",
		      "mutex initialisation", strerror(err));
		goto out;
	}
	lock_device(dev);
	dev->init_lock = true;

	err = pthread_condattr_init(&condattr);
	cond_init_attr = (err == EOK);

	if (!err)
		err = pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC);

	if (!err)
		err = pthread_cond_init(&dev->condvar, &condattr);

	if (cond_init_attr)
		pthread_condattr_destroy(&condattr);

	if (err) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed: %s",
		      "condvar initialisation", strerror(err));
		goto out;
	}
	dev->init_condvar = true;

	err = wfd_err_state_init(&dev->err_state);
	if (err) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed: %s",
		      "wfd_err_state_init", strerror(err));
		goto out;
	}
	dev->init_err_state = true;

	err = wfddevice_cfg_create(&dev->device_cfg, dev->wfd_dev_id, NULL);
	if (err) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "failed to create device configs");
		goto out;
	}

	err = wfd_create_port(dev);
	if (err)
		goto out;

	err = wfd_create_pipe(dev);
	if (err)
		goto out;

	dss_set_plane_mask(dev);
out:
	if (!err) {
		unlock_device(dev);
		TRACE_F("success, returning dev=%p", dev);
		*pdevice = dev;
	} else {
		DEBUG1("%s failed, err=%d", __func__, err);
		wfd_destory_device(dev);
	}

	return err;
}

static void wfd_destory_device(struct wfd_device *dev)
{
	struct wfd_port *port;
	struct wfd_pipe *pipe;

	/*
	 * related ports/pipelines/sources should be destroyed before.
	 */
	if (!dev)
		return;

	if (dev->init_lock)
		assert_device_locked(dev);

	while (!SLIST_EMPTY(&dev->port_list)) {
		port = SLIST_FIRST(&dev->port_list);
		SLIST_REMOVE_HEAD(&dev->port_list, dev_portlist_entry);
		wfd_free_port(port);
	}

	while (!SLIST_EMPTY(&dev->pipe_list)) {
		pipe = SLIST_FIRST(&dev->pipe_list);
		SLIST_REMOVE_HEAD(&dev->pipe_list, dev_pipelist_entry);
		wfd_free_pipe(pipe);
	}

	if (dev->init_err_state)
		wfd_err_state_destroy(&dev->err_state);

	if (dev->init_condvar)
		pthread_cond_destroy(&dev->condvar);

	if (dev->init_lock) {
		unlock_device(dev);
		pthread_mutex_destroy(&dev->lock);
		dev->init_lock = false;
	}

	if (dev->wfd_dev_id)
		wfd_hdl_unset_device_in_use(dev->wfd_dev_id);

	free(dev);
}

void wfd_device_ref_release(struct wfd_device *dev, const void *refkey)
{
	TRACE_F("dev=%p refcounter(%p count %d)", dev, &dev->ref, refcount_check(&dev->ref));
	if (dev) {
		if (refcount_dec_value(&dev->ref, refkey) == 0)
			wfd_hdl_send_ref_zero_signal();
	}
}

static int wfd_create_port(struct wfd_device *dev)
{
	errno_t err = EOK;
	WFDint *port_ids = NULL;
	int i;

	err = wfd_config_ports_get(dev, &port_ids);
	for (i = 0; port_ids[i] && !err; ++i)
		err = wfd_add_port(dev, port_ids[i]);

	if (err)
		slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed: %s", "port creation", strerror(err));

	free(port_ids);

	return err;
}

static int wfd_add_port(struct wfd_device *dev, WFDint wfd_port_id)
{
	struct wfd_port *port = NULL, *last_port = NULL;
	struct wfd_port *tmp;
	uint8_t index = 0;
	int err = EOK;

	SLIST_FOREACH(tmp, &dev->port_list, dev_portlist_entry) {
		assert(tmp->wfd_port_id != wfd_port_id);
		if (tmp->index >= index) {
			index = (uint8_t)tmp->index;
			++index;
		}
		last_port = tmp;
	}

	if (index >= 64)
		return ENOSPC;

	port = dss_alloc_wfd_port(dev, wfd_port_id, index);
	if (!port)
		goto out;

out:
	if (port && !err) {
		if (last_port)
			SLIST_INSERT_AFTER(last_port, port, dev_portlist_entry);
		else
			SLIST_INSERT_HEAD(&dev->port_list, port, dev_portlist_entry);
	} else {
		wfd_free_port(port);
	}

	return err;
}

static void wfd_free_port(struct wfd_port *port)
{
	free(port);
}

static int wfd_create_pipe(struct wfd_device *dev)
{
	errno_t err = EOK;
	WFDint *pipe_ids = NULL;
	int i;

	err = wfd_config_pipelines_get(dev, &pipe_ids);

	for (i = 0; pipe_ids[i] && !err; ++i)
		err = wfd_add_pipe(dev, pipe_ids[i]);
	if (err)
		slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed: %s", "pipeline creation", strerror(err));

	free(pipe_ids);

	return err;
}

static errno_t wfd_add_pipe(struct wfd_device *dev, WFDint wfd_pipe_id)
{
	struct wfd_pipe *pipe = NULL, *last_pipe = NULL;
	struct wfd_pipe *tmp;
	unsigned int index = 0;
	errno_t err = EOK;

	SLIST_FOREACH(tmp, &dev->pipe_list, dev_pipelist_entry) {
		assert(tmp->wfd_pipe_id != wfd_pipe_id);
		if (tmp->index >= index) {
			index = tmp->index;
			++index;
		}
		last_pipe = tmp;
	}

	if (index >= 64)
		return ENOSPC;

	pipe = dss_alloc_wfd_pipe(dev, wfd_pipe_id, index);
	if (!pipe) {
		slogf(SLOGC_SELF, _SLOG_ERROR, "alloc_wfd_pipe %d failed", wfd_pipe_id);
		goto out;
	}

	err = wfd_config_set_pipelines_binding(dev, pipe);
out:
	if (pipe && !err) {
		if (last_pipe)
			SLIST_INSERT_AFTER(last_pipe, pipe, dev_pipelist_entry);
		else
			SLIST_INSERT_HEAD(&dev->pipe_list, pipe, dev_pipelist_entry);
	} else {
		wfd_free_pipe(pipe);
	}

	return err;
}

static void wfd_free_pipe(struct wfd_pipe *pipe)
{
	free(pipe);
}

static int wfd_device_commit_flush(struct wfd_commit_state *commit_state)
{
	assert(commit_state);
	struct wfd_port *port = NULL;
	struct wfd_pipe *pipe;
	int err = EOK;

	assert_device_locked(commit_state->dev);

	// apply pipe settings
	SLIST_FOREACH(pipe, &commit_state->dev->pipe_list, dev_pipelist_entry) {
		uint64_t bit = (uint64_t)1 << pipe->index;
		if (commit_state->pipe_idx_mask & bit) {
			int tmp_err = wfd_pipe_commit_flush(pipe, commit_state);
			if (!err) {
				err = tmp_err;
				port = wfd_pipe_get_commit_port(pipe);
			}
		}
	}

	if (!commit_state->port_idx_mask) {
		if (port)
			dss_port_flush(port, commit_state);
	} else {
		struct wfd_port *port;

		SLIST_FOREACH(port, &commit_state->dev->port_list, dev_portlist_entry) {
			uint64_t bit = (uint64_t)1 << port->index;
			if (commit_state->port_idx_mask & bit) {
				int tmp_err = wfd_port_commit_flush(port, commit_state);
				if (!err)
					err = tmp_err;
			}
		}
	}

	return err;
}

static int wfd_device_wait_vblank(struct wfd_commit_state *commit_state)
{
	assert(commit_state);
	struct wfd_port *port = NULL;
	struct wfd_pipe *pipe;

	SLIST_FOREACH(pipe, &commit_state->dev->pipe_list, dev_pipelist_entry) {
		uint64_t bit = (uint64_t)1 << pipe->index;
		if (commit_state->pipe_idx_mask & bit)
			port = wfd_pipe_get_commit_port(pipe);
	}

	if (port)
		dss_wait_vsync(port);

	return EOK;
}

static WFDErrorCode wfd_device_commit(struct wfd_commit_state *commit_state)
{
	assert(commit_state); assert(commit_state->dev);
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev = commit_state->dev;
	struct wfd_port *port;

	DEBUG1("%s device=%p", __func__, dev);
	SLIST_FOREACH(port, &dev->port_list, dev_portlist_entry) {
		wfderr = wfd_port_commit(port, commit_state);
		if (wfderr)
			goto out;
	}
out:
	return wfderr;
}

static void wfd_device_commit_update(const struct wfd_commit_state *commit_state)
{
	struct wfd_port *port;
	struct wfd_pipe *pipe;

	SLIST_FOREACH(port, &commit_state->dev->port_list, dev_portlist_entry) {
		uint64_t bit = (uint64_t)1 << port->index;
		if (commit_state->port_idx_mask & bit)
			wfd_port_commit_update(port);
	}

	SLIST_FOREACH(pipe, &commit_state->dev->pipe_list, dev_pipelist_entry) {
		uint64_t bit = (uint64_t)1 << pipe->index;
		if (commit_state->pipe_idx_mask & bit)
			wfd_pipe_commit_update(pipe);
	}
}

void wfd_port_vblank_update(struct wfd_commit_state *commit_state, struct wfd_port *port,
			    const struct timespec *ts)
{
	uint64_t old, new;

	old = timespec2nsec(&port->end_ts);
	new = timespec2nsec(ts);

	if (new - old <= UINT64_MAX / 2)
		port->end_ts = *ts;

	port->start_ts = commit_state->start_ts;
}

void wfd_device_vblank_update(struct wfd_commit_state *commit_state, const struct timespec *ts)
{
	uint64_t old, new;

	old = timespec2nsec(&commit_state->end_ts);
	new = timespec2nsec(ts);

	if (new - old <= UINT64_MAX / 2)
		commit_state->end_ts = *ts;
}

static WFDErrorCode wfd_commit_by_type(struct wfd_commit_state *commit_state)
{
	assert(commit_state->dev);
	assert_device_locked(commit_state->dev);
	WFDErrorCode wfderr = WFD_ERROR_NONE;

	DEBUG1("%s: %p commit_type:%s", __func__, commit_state, wfd_get_commit_type(commit_state->type));
	switch (commit_state->type) {
	case WFD_COMMIT_ENTIRE_DEVICE:
		wfderr = wfd_device_commit(commit_state);
		break;
	case WFD_COMMIT_ENTIRE_PORT:
		wfderr = wfd_port_commit(commit_state->port, commit_state);
		break;
	case WFD_COMMIT_PIPELINE:
		wfderr = wfd_pipe_commit(commit_state->pipe, commit_state);
		break;
	default:
		assert(0 && "unhandled commit_type");
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		break;
	}

	if (wfderr != WFD_ERROR_NONE)
		goto out;

	wfd_device_commit_update(commit_state);
	if (wfd_device_commit_flush(commit_state) != EOK)
		wfderr = WFD_ERROR_INCONSISTENCY;

out:
	return wfderr;
}

static void wfd_device_handles_invalidate(struct wfd_device *dev)
{
	assert_device_locked(dev);
	struct wfd_port *port;
	struct wfd_pipe *pipe;

	DEBUG2("Freeing device handle %"PRIhdl" (for dev %p)", dev->handle, dev);

	wfd_device_free_handle(dev);

	SLIST_FOREACH(port, &dev->port_list, dev_portlist_entry)
		wfd_port_handles_invalidate(port);

	SLIST_FOREACH(pipe, &dev->pipe_list, dev_pipelist_entry)
		wfd_pipe_handles_invalidate(pipe);
}

static void wfd_device_refs_clear(struct wfd_device *dev)
{
	assert_device_locked(dev);
	struct wfd_port *port;
	struct wfd_pipe *pipe;

	SLIST_FOREACH(port, &dev->port_list, dev_portlist_entry)
		wfd_port_refs_clear(port);

	SLIST_FOREACH(pipe, &dev->pipe_list, dev_pipelist_entry)
		wfd_pipe_ref_clear(pipe);
}

/*** Standard API functions start ***/

WFD_API_CALL WFDint WFD_APIENTRY
wfdGetStrings(WFDDevice device_handle,
	      WFDStringID name,
	      const char **strings,
	      WFDint strings_count) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl", stringid=0x%x", device_handle, name);

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	WFDint filled = 0;
	struct wfd_device *dev = NULL;

	if (device_handle != WFD_INVALID_HANDLE) {
		dev = wfd_find_deivce_by_handle(device_handle, __func__);
		if (!dev) {
			TRACE_API_F("failed: %s", "bad handle(s)");
			return 0;
		}
	}

	if (strings && strings_count < 0) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	switch (name) {
	case WFD_VENDOR:
		if (strings_count >= 1)
			strings[filled++] = "WFD Software system";
		break;
	case WFD_RENDERER:
		if (strings_count >= 1)
			strings[filled++] = "WFD Render";
		break;
	case WFD_VERSION:  // WFD spec. version
		if (strings_count >= 1)
			strings[filled++] = "1.0";
		break;
	case WFD_EXTENSIONS:
		while (filled < (WFDint)ARRAY_ENTRIES(extension_list)) {
			const char *ext = &extension_list[filled].name[0];
			if (strings) {
				if (filled < strings_count)
					strings[filled++] = ext;
				else
					break;
			} else {
				++filled;
			}
		}
		break;
	default:
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		break;
	}

out:
	if (wfderr != WFD_ERROR_NONE) {
		if (dev)
			wfd_device_err_store(dev, WFD_ERROR_ILLEGAL_ARGUMENT);
		assert(filled == 0);
	}
	wfd_device_ref_release(dev, __func__);
	return filled;
}

WFD_API_CALL WFDboolean WFD_APIENTRY
wfdIsExtensionSupported(WFDDevice device_handle,
		        const char *string) WFD_APIEXIT
{
	TRACE_API;

	WFDboolean found = WFD_FALSE;
	struct wfd_device *dev = NULL;
	const char *ext;
	int i;

	if (device_handle != WFD_INVALID_HANDLE) {
		dev = wfd_find_deivce_by_handle(device_handle, __func__);
		if (!dev) {
			TRACE_API_F("failed: %s", "bad handle(s)");
			return found;
		}
	}

	for (i = 0; i < (int)ARRAY_ENTRIES(extension_list); ++i) {
		ext = &extension_list[i].name[0];
		if (strcmp(string, ext) == 0) {
			found = WFD_TRUE;
			break;
		}
	}

	DEBUG1("device=%p, string=\"%s\" result=%d", dev, string != NULL ? string : "<null>", found);
#ifndef NDEBUG
	bool hide = false;
	if (!found && strcmp(string, "win_perm_eok_hack") == 0)
		hide = true;
	if (!hide)
		DEBUG2("wfdIsExtensionSupported(\"%s\") result=%d", string, found);
#endif
	wfd_device_ref_release(dev, __func__);

	return found;
}

WFD_API_CALL WFDErrorCode WFD_APIENTRY
wfdGetError(WFDDevice device_handle) WFD_APIEXIT
{
	TRACE_API;

	WFDErrorCode wfderr = WFD_ERROR_BAD_DEVICE;
	struct wfd_device *dev = NULL;

	dev = wfd_find_deivce_by_handle(device_handle, __func__);
	if (dev) {
		wfderr = wfd_err_state_read_and_clear(&dev->err_state, dev->geterror_use_tls);
		wfd_device_ref_release(dev, __func__);
	} else {
		TRACE_API_F("failed: %s", "bad handle(s)");
	}

	return wfderr;
}

WFD_API_CALL WFDint WFD_APIENTRY
wfdEnumerateDevices(WFDint *device_ids,
		    WFDint device_ids_count,
		    const WFDint *filter_list) WFD_APIEXIT
{
	TRACE_API;

	errno_t err = EOK;
	WFDint found = 0;
	WFDint wantport = WFD_INVALID_PORT_ID;
	WFDint *dev_ids = NULL;

	if (device_ids_count <= 0 && device_ids)
		goto out;

	if (filter_list) {
		unsigned i = 0;
		for (;;) {
			WFDint key = filter_list[i++];
			switch (key) {
			case WFD_NONE:
				break;
			case WFD_DEVICE_FILTER_PORT_ID:
				wantport = filter_list[i++];
				if (wantport <= 0)
					goto out;
				break;
			default:
				goto out;
			}
		}
	}

	err = wfd_config_devices_get(&dev_ids);
	for (int i = 0; dev_ids[i] && !err; ++i) {
		WFDint wfd_dev_id = dev_ids[i];
		if (found < device_ids_count || !device_ids) {
			bool accept = false;

			if (wantport == WFD_INVALID_PORT_ID) {
				accept = true;
			} else {
				//TODO
			}

			if (accept) {
				if (device_ids)
					device_ids[found] = wfd_dev_id;
				++found;
			}
		}
	}
	free(dev_ids);
out:
	return err ? 0 : found;
}

WFD_API_CALL WFDDevice WFD_APIENTRY
wfdCreateDevice(WFDint device_id, const WFDint *attr_list) WFD_APIEXIT
{
	TRACE_API;

	struct wfd_device *dev = NULL;
	WFDDevice device_handle;

	if (attr_list && *attr_list != WFD_NONE)
		goto out;

	if (device_id == WFD_DEFAULT_DEVICE_ID) {
		WFDint *dev_ids = NULL;
		if (EOK != wfd_config_devices_get(&dev_ids))
			return WFD_INVALID_HANDLE;
		device_id = dev_ids[0];
		free(dev_ids);
	}

	if (EOK != wfd_create_device(&dev, device_id))
		goto out;

out:
	if (dev && dev->handle == WFD_INVALID_HANDLE) {
		// failed
		lock_device(dev);
		wfd_destory_device(dev);
		dev = NULL;
	}

	if (dev)
		DEBUG1("created dev=%p for id=%d, handle=%"PRIhdl, dev, device_id, dev->handle);

	device_handle = dev ? dev->handle : WFD_INVALID_HANDLE;
	TRACE_API_F("returning dev=%p with handle %"PRIhdl, dev, device_handle);

	return device_handle;
}

WFD_API_CALL WFDErrorCode WFD_APIENTRY
wfdDestroyDevice(WFDDevice device_handle) WFD_APIEXIT
{
	TRACE_API;
	struct wfd_device *dev;

	dev = wfd_find_deivce_by_handle(device_handle, __func__);
	if (!dev) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return WFD_ERROR_BAD_DEVICE;
	}

	lock_device(dev);
	dss_close(dev);
	wfd_device_handles_invalidate(dev);
	wfd_device_refs_clear(dev);
	//TODO: destroy pipes/ports
	wfd_device_sources_destroy(dev);
	unlock_device(dev);

	wfd_device_ref_release(dev, __func__);
	wfd_hdl_wait_refcount_release(&dev->ref);

	lock_device(dev);
	wfd_destory_device(dev);
	dev = NULL;

	DEBUG2("%s succeeded", __func__);

	return WFD_ERROR_NONE;
}

WFD_API_CALL void WFD_APIENTRY
wfdDeviceCommit(WFDDevice device_handle,
		WFDCommitType commit_type,
		WFDHandle handle) WFD_APIEXIT
{
	TRACE_API_F("device_handle=%"PRIhdl" commit_type=0x%x handle=%"PRIhdl,
		    device_handle, commit_type, handle);
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_commit_state commit_state = { .type = commit_type };

	if (clock_gettime(CLOCK_MONOTONIC, &commit_state.start_ts) != 0) {
		wfderr = WFD_ERROR_INCONSISTENCY;
		goto out;
	}
	commit_state.end_ts = commit_state.start_ts;

	switch (commit_type) {
	case WFD_COMMIT_ENTIRE_DEVICE:
#if !WFD_STRICT
		if (handle == device_handle)
			handle = WFD_INVALID_HANDLE;
#endif
		if (handle == WFD_INVALID_HANDLE) {
			commit_state.dev = wfd_find_deivce_by_handle(device_handle, __func__);
		} else {
			wfderr = WFD_ERROR_BAD_HANDLE;
			DEBUG1("WFD_COMMIT_ENTIRE_DEVICE specified"
					" with handle != WFD_INVALID_HANDLE");
		}
		break;
	case WFD_COMMIT_ENTIRE_PORT:
		wfd_find_device_port_by_handle(device_handle, (WFDPort)handle,
					       &commit_state.dev, &commit_state.port, __func__);
		if (!commit_state.port || commit_state.port->dev != commit_state.dev)
			wfderr = WFD_ERROR_BAD_HANDLE;
		break;
	case WFD_COMMIT_PIPELINE:
		wfd_find_deivce_pipeline_by_handle(device_handle, (WFDPipeline)handle,
						   &commit_state.dev, &commit_state.pipe, __func__);
		if (!commit_state.pipe || commit_state.pipe->dev != commit_state.dev)
			wfderr = WFD_ERROR_BAD_HANDLE;
		break;
	default:
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		commit_state.dev = wfd_find_deivce_by_handle(device_handle, __func__);
		break;
	}
	if (wfderr || !commit_state.dev)
		goto out;

	lock_device(commit_state.dev);
	wfderr = wfd_commit_by_type(&commit_state);
	unlock_device(commit_state.dev);
	wfd_device_wait_vblank(&commit_state);
out:
	if (commit_state.dev)
		wfd_device_err_store(commit_state.dev, wfderr);
	wfd_port_ref_release(commit_state.port, __func__);
	wfd_pipe_ref_release(commit_state.pipe, __func__);
	wfd_device_ref_release(commit_state.dev, __func__);
	DEBUG1("%s: %p returning; wfderr=%#x", __func__, &commit_state, wfderr);
}

WFD_API_CALL WFDint WFD_APIENTRY
wfdGetDeviceAttribi(WFDDevice device_handle,
		    WFDDeviceAttrib attr) WFD_APIEXIT
{
	TRACE_API;

	WFDint value = -1;
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev = wfd_find_deivce_by_handle(device_handle, __func__);
	if (!dev) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return -1;
	}

	lock_device(dev);
	switch ((int)attr) {
	case WFD_DEVICE_ID:
		value = dev->wfd_dev_id;
		break;
#if defined(__QNXNTO__)
	case WFD_DEVICE_GETERROR_USE_TLS_QNX:
		value = dev->geterror_use_tls;
		break;
#endif
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}
	unlock_device(dev);

	wfd_device_err_store(dev, wfderr);
	wfd_device_ref_release(dev, __func__);

	return value;
}

WFD_API_CALL void WFD_APIENTRY
wfdSetDeviceAttribi(WFDDevice device_handle,
		    WFDDeviceAttrib attr,
		    WFDint value)
{
	TRACE_API;

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *dev;

	dev = wfd_find_deivce_by_handle(device_handle, __func__);
	if (!dev) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	lock_device(dev);
	switch ((int)attr) {
#if defined(__QNXNTO__)
	case WFD_DEVICE_GETERROR_USE_TLS_QNX:
		dev->geterror_use_tls = (value != 0);
		break;
#endif
	default:
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		break;
	}
	unlock_device(dev);

	wfd_device_err_store(dev, wfderr);
	wfd_device_ref_release(dev, __func__);
}

/*** Standard API functions end ***/
