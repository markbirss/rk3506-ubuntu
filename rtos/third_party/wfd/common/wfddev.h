/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __WFDDEV_H__
#define __WFDDEV_H__

#if defined(__QNXNTO__)
#define _QNX_SOURCE
#include <screen/iomsg.h>
#include <screen/screen.h>
#include <sys/mman.h>
#include <sys/slogcodes.h>
#include <sys/neutrino.h>
#include <sys/queue.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <sys/trace.h>
#include <sys/platform.h>
#endif

#include <wfd.h>
#include <wfdext.h>
#include <wfdplatform.h>
#include <wfd_rockchip.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

typedef int __errno_t;

#if defined(__RT_THREAD__)
#include <common/util.h>
#include <stdatomic.h>

#define atomic_add_value atomic_fetch_add
#define atomic_sub_value atomic_fetch_sub

struct refcounter {
	atomic_int rc_ctr;
};
#else
#include <atomic.h>

struct refcounter {
	unsigned rc_ctr;
};

#define SLOGC_SELF (_SLOGC_GRAPHICS_DISPLAY)
#define WFD_DEVICE_SHARE_QNX INT32_C(0x42557568)

#define thread_local __thread
#endif

#define REFCOUNTER_INITIALIZER	((const struct refcounter){ .rc_ctr = 0 })

#define PRIhdl PRIu32
#define EMPTY_MACRO
#define ARRAY_ENTRIES(x)	( sizeof (x) / sizeof (x)[0] )

#define USEC_TO_NS(x)		(UINT64_C(1000) * (x))
#define MSEC_TO_NS(x)		(UINT64_C(1000) * 1000 * (x))

#define DEBUG_LEVEL1		(1 << 0)
#define DEBUG_LEVEL2		(1 << 1)
#define DEBUG_TRACE_CALL	(1 << 2)
#define DEBUG_TRACE_API		(1 << 3)
#define DEBUG_TRACE_F		(1 << 4)
#define DEBUG_TRACE_IRQ		(1 << 5)

#define SLOG_ERR(STR, ...) slogf(SLOGC_SELF, _SLOG_ERROR, "%s ERROR: " STR, __func__, ##__VA_ARGS__);

#define DEBUG1(...) do {					\
	if (dss_get_debug_level_mask() & DEBUG_LEVEL1)		\
		slogf(SLOGC_SELF, _SLOG_DEBUG1, __VA_ARGS__);	\
	} while (0)


#define DEBUG2(...) do {					\
	if (dss_get_debug_level_mask() & DEBUG_LEVEL2)		\
		slogf(SLOGC_SELF, _SLOG_DEBUG2, __VA_ARGS__);	\
	} while(0)

#define TRACE_CALL(STR, ...) do {				\
	if (dss_get_debug_level_mask() & DEBUG_TRACE_CALL)	\
		slogf(SLOGC_SELF, _SLOG_DEBUG2, "TRACE: %s " STR,  __func__, ##__VA_ARGS__); \
	} while(0)

#define DO_TRACE_API(STR, ...) do {				\
	if (dss_get_debug_level_mask() & DEBUG_TRACE_API)	\
		slogf(SLOGC_SELF, _SLOG_DEBUG2, "TRACE: %s" STR, ##__VA_ARGS__); \
	} while(0)

#define TRACE_IRQ(STR, ...) do {				\
	if (dss_get_debug_level_mask() & DEBUG_TRACE_IRQ)	\
		slogf(SLOGC_SELF, _SLOG_DEBUG2, "TRACE: IRQ " STR, ##__VA_ARGS__); \
	} while(0)

#define DO_TRACE(STR, ...) do {					\
	if (dss_get_debug_level_mask() & DEBUG_TRACE_F)		\
		slogf(SLOGC_SELF, _SLOG_DEBUG2, "TRACE: %s " STR, ##__VA_ARGS__); \
	} while (0)

#ifndef TRACE_API_F
#  define TRACE_API_F(STR, ...) DO_TRACE_API(" " STR, __func__, ##__VA_ARGS__)
#endif

#ifndef TRACE_API
#  define TRACE_API TRACE_API_F("")
#endif

#ifndef TRACE_F
#  define TRACE_F(STR, ...) DO_TRACE(" " STR, __func__, ##__VA_ARGS__)
#endif

#ifndef TRACE
#  define TRACE TRACE_F("")
#endif

struct wfd_err_state {
	unsigned wfderr;
	pthread_key_t tls_key;
};

struct wfd_device {
	WFDDevice handle;
	struct wfd_err_state err_state;
	struct refcounter ref;

	pthread_mutex_t lock;
	pthread_cond_t condvar;

	WFDint wfd_dev_id;

	SLIST_HEAD(, wfd_port) port_list;
	SLIST_HEAD(, wfd_pipe) pipe_list;
	LIST_HEAD(, wfd_source) deleted_srclist;
	struct wfddevice_cfg *device_cfg;

	unsigned init_lock : 1;
	unsigned init_condvar : 1;
	unsigned init_err_state : 1;
	unsigned geterror_use_tls : 1;
};

struct wfd_port_settings {
	uint8_t bg_r, bg_g, bg_b;
	struct wfd_port_mode *mode;
};

#define WFD_PORT_CHANGES_MODE		BIT(0)

struct wfd_port {
	WFDPort handle;
	struct refcounter ref;
	struct wfd_device *dev;
	struct wfd_port_settings pending, committed;
	WFDint wfd_port_id, wfd_port_type;

	/*
	 * one frame time
	 */
	uint64_t period_ns;
	/*
	 * condvar for vsync
	 */
	pthread_mutex_t vsync_lock;
	pthread_cond_t vsync_cond;
	struct timespec start_ts, end_ts;

	SLIST_ENTRY(wfd_port) dev_portlist_entry;
	SLIST_HEAD(, wfd_port_mode) modes;
	struct wfdport_cfg *port_cfg;

	// bitfields
	unsigned in_use : 1;
	unsigned index : 6;
	/*
	 * extened
	 */
	unsigned long changes;
};

struct wfd_port_state {
	struct wfd_port_mode *mode;
};

struct wfd_port_mode {
	WFDPortMode handle;
	struct refcounter ref;
	struct wfd_port *port;
	struct wfdmode_timing timing;

	SLIST_ENTRY(wfd_port_mode) port_modelist_entry;
};

struct wfd_pipe_state {
	struct wfd_source *src;
};

struct wfd_pipe_settings {
	struct wfd_port *port;
	struct wfd_source *src;
	WFDTransparency transparency;
	WFDRect src_rect, dst_rect;
	unsigned tscolor;
	int rotation;

	unsigned global_alpha : 8;
	unsigned mirror : 1;
	unsigned flip : 1;
};

struct wfd_pipe {
	WFDPipeline handle;
	struct refcounter ref;
	struct wfd_device *dev;
	LIST_HEAD(, wfd_source) srclist;

	WFDTransition src_transition;
	struct wfd_pipe_settings pending, committed;

	WFDint *bindable_ports, *bindable_ports_to_advertise;
	WFDint wfd_pipe_id;
	WFDint zorder;
	WFDint max_width, max_height;

	struct wfd_port *flip_port;
	SLIST_ENTRY(wfd_pipe) dev_pipelist_entry;

	// bitfields
	unsigned in_use : 1;
	unsigned index : 6;

	char *name;
};

struct wfd_source {
	LIST_ENTRY(wfd_source) srclist_entry;
	struct wfd_device *dev;
	struct wfd_pipe *pipe;
#if defined(__RT_THREAD__)
	win_image_t *winimg;
#else
	struct scrmem *scrmem;
#endif
	struct refcounter ref;
	WFDSource handle;
};

struct wfd_commit_state {
	struct wfd_device *dev;
	struct wfd_port *port;
	struct wfd_pipe *pipe;
	struct timespec start_ts, end_ts;
	WFDCommitType type;
	uint64_t port_idx_mask, pipe_idx_mask;
};

struct wfd_hdl_entry {
	struct refcounter *data;
	unsigned tag;
};

struct wfd_hdl_list {
	size_t entry_count;
	size_t free_count;
	size_t lastidx;
	struct wfd_hdl_entry *entries;
};

static inline int wfd_err_state_init(struct wfd_err_state *es)
{
	es->wfderr = WFD_ERROR_NONE;
	return pthread_key_create(&es->tls_key, _NULL);
}

static inline void wfd_err_state_destroy(struct wfd_err_state *es)
{
	pthread_key_delete(es->tls_key);
}

static inline void wfd_err_state_store(struct wfd_err_state *es, WFDErrorCode wfderr)
{
	if (wfderr) {
#if defined(__RT_THREAD__)
		es->wfderr = wfderr;
#else
		/* only store the first error */
		_smp_cmpxchg(&es->wfderr, WFD_ERROR_NONE, (unsigned)wfderr);
		if (_NULL == pthread_getspecific(es->tls_key)) {
			pthread_setspecific(es->tls_key, (void*)(_Intptrt)wfderr);
		}
#endif
	}
}

static inline WFDErrorCode wfd_err_state_read_and_clear(struct wfd_err_state *es, _Bool use_tls)
{
	WFDErrorCode wfderr;

#if defined(__RT_THREAD__)
	wfderr = es->wfderr;
	es->wfderr = WFD_ERROR_NONE;
#else
	/* we unconditionally clear the per-device and per-thread errors,
	 * then return one of them
	 */
	wfderr = (WFDErrorCode)_smp_xchg(&es->wfderr, WFD_ERROR_NONE);
	if (use_tls) {
		wfderr = (WFDErrorCode)(_Intptrt)pthread_getspecific(es->tls_key);
	}
	pthread_setspecific(es->tls_key, _NULL);
#endif

	return wfderr;
}

#if REFCOUNT_DEBUG
GCCATTR(warn_unused_result) unsigned refcount_inc_value(struct refcounter *ref, const void *refkey);
GCCATTR(warn_unused_result) unsigned refcount_dec_value(struct refcounter *ref, const void *refkey);
GCCATTR(warn_unused_result) unsigned refcount_check(struct refcounter *ref);
#else
static inline GCCATTR(warn_unused_result) unsigned refcount_inc_value(struct refcounter *ref, const void *refkey)
{
	assert(ref);
	unsigned cnt = atomic_add_value(&ref->rc_ctr, 1) + 1;
	assert(cnt != 0);
	return cnt;
}
static inline GCCATTR(warn_unused_result) unsigned refcount_dec_value(struct refcounter *ref, const void *refkey)
{
	assert(ref);
	unsigned cnt = atomic_sub_value(&ref->rc_ctr, 1) - 1;
	assert(cnt != UINT_MAX);
	return cnt;
}
static inline GCCATTR(warn_unused_result) unsigned refcount_check(struct refcounter *ref)
{
	assert(ref);
	return atomic_add_value(&ref->rc_ctr, 0);
}
#endif

#if defined(NDEBUG) && !REFCOUNT_DEBUG
static inline void refcount_inc(struct refcounter *ref, const void *refkey)
{
	assert(ref);
	atomic_add(&ref->rc_ctr, 1);
}
static inline void refcount_dec(struct refcounter *ref, const void *refkey)
{
	assert(ref);
	atomic_sub(&ref->rc_ctr, 1);
}
#else
#define refcount_inc(...) do {unsigned x = refcount_inc_value(__VA_ARGS__); (void)x;} while(0)
#define refcount_dec(...) do {unsigned x = refcount_dec_value(__VA_ARGS__); (void)x;} while(0)
#endif

static inline struct wfd_device* wfd_device_valid(struct wfd_device *dev)
{
	assert(dev);
	return dev;
}

#define lock_device(DEV) do {						\
	int err = pthread_mutex_lock(&wfd_device_valid((DEV))->lock);	\
	assert(!err && "lock_device");					\
} while(0)

#define lock_device_or_fail(DEV) \
	pthread_mutex_lock(&wfd_device_valid((DEV))->lock)

#define unlock_device(DEV) do {						\
	int err = pthread_mutex_unlock(&wfd_device_valid((DEV))->lock);\
	assert(!err && "unlock_device");				\
} while(0)

#define assert_device_locked(DEV) \
	assert(pthread_mutex_lock(&wfd_device_valid((DEV))->lock) == EDEADLK)

#define assert_device_unlocked(DEV) do {			\
	struct wfd_device *_adev = wfd_device_valid((DEV));	\
	assert(pthread_mutex_lock(&_adev->lock) == EOK		\
		&& pthread_mutex_unlock(&_adev->lock) == EOK	\
		&& "assert_device_unlocked");			\
} while(0)

static inline void wfd_device_err_store(struct wfd_device *dev, WFDErrorCode wfderr)
{
	wfd_err_state_store(&dev->err_state, wfderr);
}

// wfdhandle.c
void wfd_hdl_wait_refcount_release(struct refcounter *ref);
void wfd_hdl_send_ref_zero_signal(void);
_Bool wfd_hdl_set_device_in_use(WFDint device_id);
void wfd_hdl_unset_device_in_use(WFDint device_id);
// device handles
_Bool wfd_device_bind_handle(struct wfd_device *dev);
unsigned wfd_device_free_handle(struct wfd_device *dev);
struct wfd_device* wfd_find_deivce_by_handle(WFDDevice device_handle, const void *refkey);
// port handles
_Bool wfd_port_bind_handle(struct wfd_port *port);
unsigned wfd_port_free_handle(struct wfd_port *port);
struct wfd_port* wfd_find_port_by_handle(WFDPort port_handle, const void *refkey);
// port mode handles
_Bool wfd_port_mode_bind_handle(struct wfd_port_mode *port_mode);
unsigned wfd_port_mode_free_handle(struct wfd_port_mode *port_mode);
struct wfd_port_mode* wfd_find_port_mode_by_handle(WFDPortMode port_mode_handle, const void *refkey);
// pipe handles
_Bool wfd_pipe_bind_handle(struct wfd_pipe *pipe);
unsigned wfd_pipe_free_handle(struct wfd_pipe *pipe);
struct wfd_pipe* wfd_find_pipe_by_handle(WFDPipeline pipeline_handle, const void *refkey);
// source handles
_Bool wfd_source_bind_handle(struct wfd_source *source);
unsigned wfd_source_free_handle(struct wfd_source *source);
struct wfd_source* wfd_find_source_by_handle(WFDSource source_handle, const void *refkey);

// wfddevice.c
void wfd_device_ref_release(struct wfd_device *dev, const void *refkey);
void wfd_device_vblank_update(struct wfd_commit_state *commit_state, const struct timespec *ts);
void wfd_port_vblank_update(struct wfd_commit_state *commit_state, struct wfd_port *port, const struct timespec *ts);

// wfdport.c
void wfd_port_handles_invalidate(struct wfd_port *port);
void wfd_port_refs_clear(struct wfd_port *port);
_Bool wfd_find_device_port_by_handle(WFDDevice device_handle, WFDPort port_handle,
				     struct wfd_device **pdevice, struct wfd_port **pport,
				     const void *refkey);
void wfd_port_ref_release(struct wfd_port *port, const void *refkey);
void wfd_port_ref_replace(struct wfd_port **slot, struct wfd_port *port, const void *refkey);
void wfd_port_mode_ref_replace(struct wfd_port_mode **slot, struct wfd_port_mode *mode, const void *refkey);
WFDint wfd_port_layer_order_get(const struct wfd_port *port, const struct wfd_pipe *pipe);
WFDErrorCode wfd_port_commit(struct wfd_port *port, struct wfd_commit_state *commit_state);
void wfd_port_commit_update(struct wfd_port *port);
int wfd_port_commit_flush(struct wfd_port *port, const struct wfd_commit_state *commit_state);
void wfd_bind_pipe_to_port(struct wfd_pipe *pipe, struct wfd_port *port);
_Bool wfd_port_next_vblank_get(struct timespec *ts, struct wfd_port *port, const struct timespec *start);

// wfdpipeline.c
void wfd_pipe_handles_invalidate(struct wfd_pipe *pipe);
void wfd_pipe_ref_clear(struct wfd_pipe *pipe);
void wfd_pipe_ref_release(struct wfd_pipe *pipe, const void *refkey);
void wfd_pipe_ref_replace(struct wfd_pipe **slot, struct wfd_pipe *pipe, const void *refkey);
_Bool wfd_find_deivce_pipeline_by_handle(WFDDevice device_handle, WFDPipeline pipeline_handle,
					 struct wfd_device **pdevice, struct wfd_pipe **ppipe,
					 const void *refkey);
WFDErrorCode wfd_pipe_commit(struct wfd_pipe *pipe, struct wfd_commit_state *commit_state);
struct wfd_port* wfd_pipe_get_commit_port(const struct wfd_pipe *pipe);
void wfd_pipe_commit_update(struct wfd_pipe *pipe);
int wfd_pipe_commit_flush(struct wfd_pipe *pipe, const struct wfd_commit_state *commit_state);

// wfdimageprovider.c
void wfd_source_ref_release(struct wfd_source *source, const void *refkey);
void wfd_source_ref_replace(struct wfd_source **slot, struct wfd_source *src, const void *refkey);
void wfd_set_source_unused(struct wfd_device *device, struct wfd_source *source);
void wfd_device_sources_destroy(struct wfd_device *device);

// wfdconfig.c
errno_t wfd_config_devices_get(WFDint **result_out);
errno_t wfd_config_ports_get(struct wfd_device *dev, WFDint **result_out);
errno_t wfd_config_pipelines_get(struct wfd_device *dev, WFDint **result_out);
errno_t wfd_config_set_pipelines_binding(struct wfd_device *dev, struct wfd_pipe *pipe);
_Bool wfd_config_find_id_in_array(const WFDint *list, WFDint id);

uint32_t dss_get_debug_level_mask(void);

#endif
