/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <stdbool.h>
#include <stdlib.h>

#include "wfddev.h"

#define HANDLE_OFFSET 64
#define HANDLE_MAX UINT32_MAX
#define MAX_NUM_HANDLES (HANDLE_MAX - HANDLE_OFFSET)

/*** general handle management ***/

static bool wfd_hdl_entries_realloc(struct wfd_hdl_list *list, size_t entry_count)
{
	const size_t newsize = entry_count * sizeof(struct wfd_hdl_entry);
	struct wfd_hdl_entry *newent = NULL;

	if (newsize >= entry_count)
		newent = realloc(list->entries, newsize);

	if (newent) {
		size_t i = list->entries ? list->entry_count : 0;
		while (i < entry_count) {
			newent[i++].data = NULL;
			++list->free_count;
		}
		list->entries = newent;
		list->entry_count = entry_count;
	}

	return newent != NULL;
}

void wfd_hdl_list_init(struct wfd_hdl_list *list, size_t init_count)
{
	list->entries = NULL;
	list->entry_count = init_count;
	list->free_count = 0;
	list->lastidx = 0;

	if (init_count)
		wfd_hdl_entries_realloc(list, init_count);
}

void wfd_hdl_list_deinit(struct wfd_hdl_list *list)
{
	assert(list);
	free(list->entries);
	list->entries = NULL;
	list->free_count = 0;
}

WFDHandle wfd_hdl_entry_alloc(struct wfd_hdl_list *list, struct refcounter *ref, unsigned tag)
{
	struct wfd_hdl_entry *entry;
	WFDHandle handle = WFD_INVALID_HANDLE;
	size_t i;

	assert(ref);
	assert(HANDLE_OFFSET > 0);
	if (!list->free_count) {
		size_t entry_count = list->entry_count;

		if (entry_count == 0)
			entry_count = 64;
		else if (list->entries)
			entry_count *= 2;

		if (entry_count > MAX_NUM_HANDLES)
			entry_count = MAX_NUM_HANDLES;

		if (entry_count <= list->entry_count ||
		    !wfd_hdl_entries_realloc(list, entry_count))
			goto out;
	}

	assert(list->free_count > 0);
	for (i = list->lastidx + 1; i != list->lastidx; ++i) {

		i %= list->entry_count;
		entry = &list->entries[i];
		if (entry->data == NULL) {
			// HANDLE_OFFSET is a offset of WFD_INVALID_HANDLE
			handle = (WFDHandle)(i + HANDLE_OFFSET);
			assert(handle != WFD_INVALID_HANDLE);
			list->lastidx = i;

			entry->data = ref;
			entry->tag = tag;
			--list->free_count;
			refcount_inc(entry->data, "handle");
			goto out;
		}
	}
	assert(0 && "free_count was nonzero but nothing's free");
out:
	TRACE_API_F(" handle=%d ref(%p count: %d)tag: %d", handle, ref, refcount_check(ref), tag);
	return handle;
}

static struct wfd_hdl_entry *wfd_hdl_entry_find_by_index(const struct wfd_hdl_list *list,
							 WFDHandle handle)
{
	size_t i = (size_t)handle - HANDLE_OFFSET;

	if (handle == WFD_INVALID_HANDLE || i > list->entry_count) {
		return NULL;
	} else {
		struct wfd_hdl_entry *entry = &list->entries[i];
		return entry->data ? entry : NULL;
	}
}

struct refcounter *wfd_hdl_entry_find_by_tag(const struct wfd_hdl_list *list, WFDHandle handle,
					     unsigned tag, const void *refkey)
{
	struct wfd_hdl_entry *entry = wfd_hdl_entry_find_by_index(list, handle);

	if (entry) {
		if (entry->tag == tag || !tag) {
			assert(entry->data);
			refcount_inc(entry->data, refkey);
			return entry->data;
		}
	}

	return NULL;
}

unsigned wfd_hdl_entry_free(struct wfd_hdl_list *list, WFDHandle handle)
{
	unsigned ret = 0;
	struct wfd_hdl_entry *entry = wfd_hdl_entry_find_by_index(list, handle);

	if (entry) {
		ret = refcount_dec_value(entry->data, "handle");
		entry->data = NULL;
		++list->free_count;
	}

	return ret;
}

enum handle_tag {
	WFD_TAG_NONE = 0,
	WFD_DEVICE_TAG,
	WFD_PORT_TAG,
	WFD_PORT_MODE_TAG,
	WFD_PIPE_TAG,
	WFD_SOURCE_TAG,
};

static struct {
	struct wfd_hdl_list handles;
	pthread_cond_t cond;
	uint64_t devices_in_use;
	pthread_mutex_t lock;

	unsigned ready : 1;
	unsigned cond_init : 1;
} driver_state = {
	.handles =  { .entries = NULL },
	.lock = PTHREAD_MUTEX_INITIALIZER,
};

/*** driver init/deinit/locking ***/

static bool wfd_driver_init_cond()
{
	TRACE;

	if (!driver_state.ready) {
		if (!driver_state.cond_init) {
			if (pthread_cond_init(&driver_state.cond, NULL) != 0) {
				slogf(SLOGC_SELF, _SLOG_WARNING, "pthread_cond_init(conf) failed");
				return false;
			}
			driver_state.cond_init = true;
		}

		wfd_hdl_list_init(&driver_state.handles, 64);
		driver_state.ready = true;
		DEBUG2("wfd %s ok", __func__);
	}

	return true;
}

static inline bool wfd_driver_init_lock()
{
	TRACE;
	int err = pthread_mutex_lock(&driver_state.lock);

	if (err)
		DEBUG1("%s: couldn't acquire driver_state.lock (error %d)", __func__, err);

	return err == 0;
}

static inline void wfd_driver_unlock()
{
	TRACE;
	int err = pthread_mutex_unlock(&driver_state.lock);
	assert(!err);
}

GCCATTR(warn_unused_result) static inline bool wfd_driver_lock()
{
	if (wfd_driver_init_lock()) {
		if (driver_state.ready || wfd_driver_init_cond())
			return true;
		wfd_driver_unlock();
	}

	return false;
}

static inline void wfd_driver_lock_nofail()
{
	bool success = wfd_driver_lock();
	assert(success);
}

GCCATTR(constructor) void wfd_init_driver()
{
	DEBUG1("%s", __func__);
}

GCCATTR(destructor) void wfd_deinit_driver()
{
	DEBUG1("%s starting", __func__);

	if (wfd_driver_lock()) {
		if (driver_state.cond_init) {
			pthread_cond_destroy(&driver_state.cond);
			driver_state.cond_init = 0;
		}

		wfd_hdl_list_deinit(&driver_state.handles);
		driver_state.ready = false;
		pthread_mutex_unlock(&driver_state.lock);
		pthread_mutex_destroy(&driver_state.lock);
	}
	DEBUG2("%s complete", __func__);
}

/*** handle management ***/

/* device handles */

static struct wfd_device *refcounter_to_wfd_device(struct refcounter *ref)
{
	char *p = (char*)ref - offsetof(struct wfd_device, ref);
	return (struct wfd_device*)p;
}

struct wfd_device *wfd_find_deivce_by_handle(WFDDevice device_handle, const void *refkey)
{
	struct wfd_device *dev = NULL;
	struct refcounter *ref;

	/*
	 * we encoder dss_service fd to upper 16 bits
	 */
	device_handle = device_handle & 0xffff;

	if (wfd_driver_lock()) {
		ref = wfd_hdl_entry_find_by_tag(&driver_state.handles, device_handle,
						WFD_DEVICE_TAG, refkey);
		wfd_driver_unlock();
		if (ref) {
			dev = refcounter_to_wfd_device(ref);
			assert(dev);
		} else {
			DEBUG2("%s: bad device handle %"PRIhdl, __func__, device_handle);
		}
	}

	return dev;
}

bool wfd_device_bind_handle(struct wfd_device *dev)
{
	TRACE_F(" dev=%p", dev);
	assert(dev);
	assert(dev->handle == WFD_INVALID_HANDLE);
	bool success = false;

	if (wfd_driver_lock()) {
		dev->handle = wfd_hdl_entry_alloc(&driver_state.handles, &dev->ref, WFD_DEVICE_TAG);
		success = (dev->handle != WFD_INVALID_HANDLE);
		wfd_driver_unlock();
	}

	return success;
}

unsigned wfd_device_free_handle(struct wfd_device *dev)
{
	TRACE_F(" dev=%p", dev);
	assert(dev);
	unsigned ret = 0;
	WFDHandle handle = dev->handle & 0xffff;

	if (wfd_driver_lock()) {
		ret = wfd_hdl_entry_free(&driver_state.handles, handle);
		dev->handle = WFD_INVALID_HANDLE;
		wfd_driver_unlock();
	}

	return ret;
}

/* port handles */

static struct wfd_port *refcounter_to_wfd_port(struct refcounter *ref)
{
	char *p = (char*)ref - offsetof(struct wfd_port, ref);
	return (struct wfd_port*)p;
}

struct wfd_port *wfd_find_port_by_handle(WFDPort port_handle, const void *refkey)
{
	struct wfd_port *port = NULL;
	struct refcounter *ref;

	if (wfd_driver_lock()) {
		ref = wfd_hdl_entry_find_by_tag(&driver_state.handles, port_handle,
						WFD_PORT_TAG, refkey);
		wfd_driver_unlock();
		if (ref)
			port = refcounter_to_wfd_port(ref);
		else
			DEBUG2("%s: bad port handle %"PRIhdl, __func__, port_handle);
	}

	return port;
}

bool wfd_port_bind_handle(struct wfd_port *port)
{
	TRACE_F(" port=%p", port);
	assert(port);
	assert(port->handle == WFD_INVALID_HANDLE);
	bool success = false;

	if (wfd_driver_lock()) {
		port->handle = wfd_hdl_entry_alloc(&driver_state.handles,
				&port->ref, WFD_PORT_TAG);
		success = (port->handle != WFD_INVALID_HANDLE);
		wfd_driver_unlock();
	}

	return success;
}

unsigned wfd_port_free_handle(struct wfd_port *port)
{
	TRACE_F(" port=%p", port);
	assert(port);
	unsigned ret = 0;

	if (wfd_driver_lock()) {
		ret = wfd_hdl_entry_free(&driver_state.handles, port->handle);
		port->handle = WFD_INVALID_HANDLE;
		wfd_driver_unlock();
	}

	return ret;
}

/* port mode handles */

static struct wfd_port_mode *refcounter_to_wfd_port_mode(struct refcounter *ref)
{
	char *p = (char*)ref - offsetof(struct wfd_port_mode, ref);
	return (struct wfd_port_mode*)p;
}

struct wfd_port_mode *wfd_find_port_mode_by_handle(WFDPortMode port_mode_handle, const void *refkey)
{
	struct wfd_port_mode *port_mode = NULL;
	struct refcounter *ref;

	if (wfd_driver_lock()) {
		ref = wfd_hdl_entry_find_by_tag(&driver_state.handles, port_mode_handle,
						WFD_PORT_MODE_TAG, refkey);
		wfd_driver_unlock();
		if (ref)
			port_mode = refcounter_to_wfd_port_mode(ref);
		else
			DEBUG2("%s: bad port mode handle %"PRIhdl, __func__, port_mode_handle);
	}

	return port_mode;
}

bool wfd_port_mode_bind_handle(struct wfd_port_mode *port_mode)
{
	TRACE_F(" port mode=%p", port_mode);
	assert(port_mode);
	assert(port_mode->handle == WFD_INVALID_HANDLE);
	bool success = false;

	if (wfd_driver_lock()) {
		port_mode->handle = wfd_hdl_entry_alloc(&driver_state.handles, &port_mode->ref,
						       WFD_PORT_MODE_TAG);
		success = (port_mode->handle != WFD_INVALID_HANDLE);
		wfd_driver_unlock();
	}

	return success;
}

unsigned wfd_port_mode_free_handle(struct wfd_port_mode *port_mode)
{
	TRACE_F(" port mode=%p", port_mode);
	unsigned ret = 0;
	assert(port_mode);

	if (wfd_driver_lock()) {
		ret = wfd_hdl_entry_free(&driver_state.handles, port_mode->handle);
		port_mode->handle = WFD_INVALID_HANDLE;
		wfd_driver_unlock();
	}

	return ret;
}

/* pipe handles */

static struct wfd_pipe *refcounter_to_wfd_pipe(struct refcounter *ref)
{
	char *p = (char*)ref - offsetof(struct wfd_pipe, ref);
	return (struct wfd_pipe*)p;
}

struct wfd_pipe *wfd_find_pipe_by_handle(WFDPipeline pipeline_handle, const void *refkey)
{
	struct wfd_pipe *pipe = NULL;
	struct refcounter *ref;

	if (wfd_driver_lock()) {
		ref = wfd_hdl_entry_find_by_tag(&driver_state.handles, pipeline_handle,
						WFD_PIPE_TAG, refkey);
		wfd_driver_unlock();
		if (ref)
			pipe = refcounter_to_wfd_pipe(ref);
		else
			DEBUG2("%s: bad pipe handle %"PRIhdl, __func__, pipeline_handle);
	}

	return pipe;
}

bool wfd_pipe_bind_handle(struct wfd_pipe *pipe)
{
	TRACE_F(" pipe=%p", pipe);
	assert(pipe);
	assert(pipe->handle == WFD_INVALID_HANDLE);
	bool success = false;

	if (wfd_driver_lock()) {
		pipe->handle = wfd_hdl_entry_alloc(&driver_state.handles,
				&pipe->ref, WFD_PIPE_TAG);
		success = (pipe->handle != WFD_INVALID_HANDLE);
		wfd_driver_unlock();
	}

	return success;
}

unsigned wfd_pipe_free_handle(struct wfd_pipe *pipe)
{
	TRACE_F(" pipe=%p", pipe);
	assert(pipe);
	unsigned ret = 0;

	if (wfd_driver_lock()) {
		ret = wfd_hdl_entry_free(&driver_state.handles, pipe->handle);
		pipe->handle = WFD_INVALID_HANDLE;
		wfd_driver_unlock();
	}

	return ret;
}

/* source handles */

static struct wfd_source *refcounter_to_wfd_source(struct refcounter *ref)
{
	char *p = (char*)ref - offsetof(struct wfd_source, ref);
	return (struct wfd_source*)p;
}

struct wfd_source *wfd_find_source_by_handle(WFDSource source_handle, const void *refkey)
{
	struct wfd_source *src = NULL;
	struct refcounter *ref;

	if (wfd_driver_lock()) {
		ref = wfd_hdl_entry_find_by_tag(&driver_state.handles, source_handle,
						WFD_SOURCE_TAG, refkey);
		wfd_driver_unlock();
		if (ref)
			src = refcounter_to_wfd_source(ref);
		else
			DEBUG2("%s: bad source handle %"PRIhdl, __func__, source_handle);
	}

	return src;
}

bool wfd_source_bind_handle(struct wfd_source *source)
{
	TRACE_F(" source=%p", source);
	assert(source);
	assert(source->handle == WFD_INVALID_HANDLE);
	bool success = false;

	if (wfd_driver_lock()) {
		source->handle = wfd_hdl_entry_alloc(&driver_state.handles, &source->ref,
							WFD_SOURCE_TAG);
		success = (source->handle != WFD_INVALID_HANDLE);
		wfd_driver_unlock();
	}

	return success;
}

unsigned wfd_source_free_handle(struct wfd_source *source)
{
	TRACE_F(" source=%p", source);
	assert(source);
	unsigned ret = 0;

	if (wfd_driver_lock()) {
		ret = wfd_hdl_entry_free(&driver_state.handles, source->handle);
		source->handle = WFD_INVALID_HANDLE;
		wfd_driver_unlock();
	}

	return ret;
}


/*** refcounting helpers ***/

void wfd_hdl_wait_refcount_release(struct refcounter *ref)
{
	TRACE;
	assert(ref);
	unsigned c, oldc = 0;
	int err;

	wfd_driver_lock_nofail();
	while ((c = refcount_check(ref)) != 0) {
		if (c != oldc) {
			DEBUG2("Waiting for refcounter %p == 0"
					" (current count %u)", ref, c);
			oldc = c;
		}
		err = pthread_cond_wait(&driver_state.cond, &driver_state.lock);
		assert(!err);
	}
	if (oldc)
		DEBUG2("refcounter %p == 0", ref);
	wfd_driver_unlock();
}

void wfd_hdl_send_ref_zero_signal(void)
{
	TRACE;
	int err;

	wfd_driver_lock_nofail();
	err = pthread_cond_broadcast(&driver_state.cond);
	assert(!err);
	wfd_driver_unlock();
}

bool wfd_hdl_set_device_in_use(WFDint device_id)
{
	bool success = false;

	if ((device_id < 1) || (device_id > 64))
		goto out;

	if (wfd_driver_lock()) {
		const uint64_t dev_mask = (uint64_t)1 << (device_id - 1);
		if (0 == (driver_state.devices_in_use & dev_mask)) {
			driver_state.devices_in_use |= dev_mask;
			success = true;
		}
		wfd_driver_unlock();
	}
out:
	return success;
}

void wfd_hdl_unset_device_in_use(WFDint device_id)
{
	assert((device_id >= 1) && (device_id <= 64));
	const uint64_t dev_mask = (uint64_t)1 << (device_id - 1);

	wfd_driver_lock_nofail();
	assert(driver_state.devices_in_use & dev_mask);
	driver_state.devices_in_use &= ~dev_mask;
	wfd_driver_unlock();
}
