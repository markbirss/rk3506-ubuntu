/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wfddev.h"

#if defined(__RT_THREAD__)
#include "drv_heap.h"
#else
#include <screen/iomsg.h>
#include <screen/private/scrmem.h>
#include <screen/private/scrmem_open.h>
#include <screen/screen.h>
#include <screen/iomsg.h>
#include <screen/private/scrmem.h>
#include <screen/private/scrmem_open.h>
#include <screen/screen.h>
#endif

static void wfd_free_source(struct wfd_source *source)
{
	if (source) {
		assert(source->handle == WFD_INVALID_HANDLE);
		assert(!source->pipe);
#if defined(__QNXNTO__)
		scrmem_close(source->scrmem);
#endif
	}
	free(source);
}

void wfd_source_ref_release(struct wfd_source *source, const void *refkey)
{
	TRACE_F("source=%p", source);
	if (source) {
		if (refcount_dec_value(&source->ref, refkey) == 0)
			wfd_hdl_send_ref_zero_signal();
	}
}

void wfd_source_ref_replace(struct wfd_source **slot, struct wfd_source *source, const void *refkey)
{
	if (*slot != source) {
		if (*slot) {
			wfd_source_ref_release(*slot, refkey);
			*slot = NULL;
		}
		if (source) {
			refcount_inc(&source->ref, refkey);
			*slot = source;
		}
	}
}

static bool wfd_find_deivce_source_by_handle(WFDDevice device_handle, WFDSource source_handle,
					     struct wfd_device **pdevice, struct wfd_source **psource,
					     const void *refkey)
{
	struct wfd_device *device;
	struct wfd_source *source;
	struct wfd_device *device_from_pipe = NULL;

	assert(pdevice);
	assert(psource);

	TRACE_F("device_handle=%"PRIhdl" source_handle=%"PRIhdl, device_handle, source_handle);

	device = wfd_find_deivce_by_handle(device_handle, refkey);
	if (device) {
		source = wfd_find_source_by_handle(source_handle, refkey);
		if (source) {
			if (source->pipe)
				device_from_pipe = source->pipe->dev;

			if (device_from_pipe == device) {
				*pdevice = device;
				*psource = source;
				TRACE_F("returning device=%p source=%p", device, source);
				return true;
			}

			DEBUG2("%s: source handle %"PRIhdl" refers to wrong device"
			       " (found device handle %"PRIhdl", wanted %"PRIhdl")",
			       __func__, source_handle,
			       device_from_pipe ? device_from_pipe->handle : 0, device_handle);

			wfd_source_ref_release(source, refkey);
		}
		wfd_device_err_store(device, WFD_ERROR_BAD_HANDLE);
		wfd_device_ref_release(device, refkey);
	}

	TRACE_F("fail");

	return false;
}

void wfd_device_sources_destroy(struct wfd_device *device)
{
	struct wfd_source *source, *next_source;
	LIST_HEAD(, wfd_source) srclist_to_free = LIST_HEAD_INITIALIZER();

	assert_device_locked(device);

	source = LIST_FIRST(&device->deleted_srclist);
	while (source != NULL) {
		next_source = LIST_NEXT(source, srclist_entry);
		if (refcount_check(&source->ref) == 0) {
			LIST_REMOVE(source, srclist_entry);
			LIST_INSERT_HEAD(&srclist_to_free, source, srclist_entry);
		}
		source = next_source;
	}

	unlock_device(device);
	while ((source = LIST_FIRST(&srclist_to_free)) != NULL) {
		LIST_REMOVE(source, srclist_entry);
		wfd_free_source(source);
	}
	lock_device(device);
}

static int wfd_create_source_from_image(struct wfd_source **psource, struct wfd_device *device,
					struct wfd_pipe *pipe, win_image_t *winimg)
{
	assert(winimg);
	assert(psource);

	struct wfd_source *source = NULL;
	int err = WFD_ERROR_NONE;

	source = calloc(1, sizeof(struct wfd_source));
	if (!source) {
		err = WFD_ERROR_OUT_OF_MEMORY;
		goto out;
	}
	source->ref = REFCOUNTER_INITIALIZER;
	source->handle = WFD_INVALID_HANDLE;
	source->dev = device;

#if defined(__RT_THREAD__)
	source->winimg = winimg;
#else
	err = scrmem_open_from_win_image(&source->scrmem, winimg, O_RDONLY, NULL);
#endif
out:
	if (!err) {
		if (pipe) {
			lock_device(pipe->dev);
			LIST_INSERT_HEAD(&pipe->srclist, source, srclist_entry);
			unlock_device(pipe->dev);

			source->pipe = pipe;
			refcount_inc(&pipe->ref, source);
		}
		*psource = source;
	} else {
		err = WFD_ERROR_ILLEGAL_ARGUMENT;
		wfd_free_source(source);
	}

	return err;
}

void wfd_set_source_unused(struct wfd_device *device, struct wfd_source *source)
{
	assert_device_locked(device);

	wfd_source_free_handle(source);
	if (source->pipe) {
		LIST_REMOVE(source, srclist_entry);
		wfd_pipe_ref_release(source->pipe, source);
		source->pipe = NULL;
	}
	LIST_INSERT_HEAD(&device->deleted_srclist, source, srclist_entry);
}

static inline int wfd_format_get_bpp(WFDint format)
{
	int bpp;

	switch(format) {
	case WFD_FORMAT_YUV420:
	case WFD_FORMAT_NV12:
	case WFD_FORMAT_YV12:
		bpp = 1;
		break;
	case WFD_FORMAT_RGBA4444:
	case WFD_FORMAT_RGBX4444:
	case WFD_FORMAT_RGBA5551:
	case WFD_FORMAT_RGBX5551:
	case WFD_FORMAT_RGB565:
	case WFD_FORMAT_UYVY:
	case WFD_FORMAT_YUY2:
	case WFD_FORMAT_YVYU:
	case WFD_FORMAT_V422:
		bpp = 2;
		break;
	case WFD_FORMAT_RGB888:
		bpp = 3;
		break;
	case WFD_FORMAT_RGBA8888:
	case WFD_FORMAT_BGRA8888:
	case WFD_FORMAT_RGBX8888:
	case WFD_FORMAT_BGRX8888:
		bpp = 4;
		break;
	default:
		DEBUG1("%s unknow format: %d\n", __func__, format);
		return 0;
	}

	return bpp;
}

/*** Standard API functions start ***/

#if defined(__QNXNTO__)
#if defined __aarch64__
# include <aarch64/mmu.h>
#elif defined __arm__
# include <arm/mmu.h>
#endif
#endif

WFD_API_CALL WFDErrorCode WFD_APIENTRY
wfdCreateWFDEGLImages(WFDDevice device_handle,
		      WFDint width, WFDint height, WFDint format,
		      WFDint usage, WFDint count, WFDEGLImage *images) WFD_APIEXIT
{
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *wfddev;
	WFDint i;
	int bpp;
	int granularity = 64; // stride should aligend with 64 byte on mali gpu
	int strides[2] = {0, 0};
	int size;
	win_image_t *img;

	TRACE_CALL("%d x %d  format: %d count=%d\n", width, height, format, count);

	wfddev = wfd_find_deivce_by_handle(device_handle, __func__);
	if (!wfddev) {
		DEBUG1("%s: failed: %s", __func__, "bad handle(s)");
		return WFD_ERROR_BAD_DEVICE;
	}

	for (i = 0; i < count; ++i)
		images[i] = NULL;

	if (width <= 0 || height <= 0 || count <= 0 || !images) {
		DEBUG1("%s: invalid width %d, height %d, count %d, or images %p argument",
			__func__,  width, height, count, images);
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	bpp = wfd_format_get_bpp(format);
	if(!bpp) {
		wfderr = WFD_ERROR_NOT_SUPPORTED;
		goto out;
	}

	if (usage & (WFD_USAGE_OPENGL_ES1 | WFD_USAGE_OPENGL_ES2 | WFD_USAGE_NATIVE))
		usage |= WFD_USAGE_READ;

	strides[0] = (width * bpp + (granularity-1)) & ~(granularity-1);
	if (usage & WFD_USAGE_ROTATION) {
		strides[1] = (height * bpp + (granularity-1)) & ~(granularity-1);
		size = max(height * strides[0], width * strides[1]);
	} else if (format == WFD_FORMAT_NV12){
		size = (height + (height + 1) / 2) * strides[0];
	} else {
		strides[1] = 0;
		size = height * strides[0];
	}

	for (i = 0; i < count; i++) {
		img = calloc(1, sizeof(*img));
		if (!img) {
			DEBUG1("could not allocate native image");
			break;
		}

		img->width = width;
		img->height = height;
		img->format = format;
		img->usage = usage;
		img->size = size;
		img->strides[0] = strides[0];
		img->strides[1] = strides[1];
		if (format == WFD_FORMAT_NV12)
			img->planar_offsets[1] = strides[0] * height;

		img->vaddr = rt_malloc((size_t)img->size);
		images[i] = img;

		DEBUG1("%s img: %p %d x %d format: %d usage: 0x%x paddr:0x%"PRIx64"", __func__,  img, width, height, format, usage, img->paddr);
	}

	if (i < count) {
		for (--i; i >= 0; i--) {
			img = images[i];
			rt_free(img->vaddr);
			free(img);
			images[i] = NULL;
		}
		wfderr = WFD_ERROR_OUT_OF_MEMORY;
		goto out;
	}

out:
	wfd_device_ref_release(wfddev, __func__);
	return wfderr;
}

WFD_API_CALL WFDErrorCode WFD_APIENTRY
wfdDestroyWFDEGLImages(WFDDevice device_handle, WFDint count, WFDEGLImage *images) WFD_APIEXIT
{

	WFDErrorCode wfderr = WFD_ERROR_NONE;
	struct wfd_device *wfddev;
	win_image_t *img;
	WFDint i;

	TRACE_F("device_handle=%"PRIhdl" count=%d\n", device_handle, count);
	wfddev = wfd_find_deivce_by_handle(device_handle, __func__);
	if (!wfddev) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return WFD_ERROR_BAD_DEVICE;
	}

	if (count <= 0 || !images) {
		wfderr = WFD_ERROR_ILLEGAL_ARGUMENT;
		goto out;
	}

	for (i = count-1; i >= 0; i--) {
		img = images[i];

		DEBUG1("%s paddr:0x%"PRIx64"", __func__, img->paddr);

		rt_free(img->vaddr);
		free(img);
	}

out:
	wfd_device_ref_release(wfddev, __func__);
	return wfderr;
}

WFD_API_CALL WFDSource WFD_APIENTRY
wfdCreateSourceFromImage(WFDDevice device_handle,
			 WFDPipeline pipeline_handle,
			 WFDEGLImage image_handle,
			 const WFDint *attribList) WFD_APIEXIT
{
	TRACE_API;
	WFDErrorCode wfderr = WFD_ERROR_NONE;
	WFDSource source_handle;
	struct wfd_device *device = NULL;
	struct wfd_pipe *pipe = NULL;
	struct wfd_source *source = NULL;
	win_image_t *winimg = image_handle;

#if !WFD_STRICT
	if (pipeline_handle == WFD_INVALID_HANDLE) {
		device = wfd_find_deivce_by_handle(device_handle, __func__);
	}
	else
#endif
	{
		wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle,
						   &device, &pipe, __func__);
		if (!device) {
			TRACE_API_F("failed: %s", "bad handle(s)");
			return WFD_INVALID_HANDLE;
		}
	}

	DEBUG1("%s for pipe=%d", __func__, pipe->wfd_pipe_id);
	if (attribList && *attribList != WFD_NONE) {
		// no valid attributes defined in OpenWF 1.0
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
		goto out;
	}

	wfderr = wfd_create_source_from_image(&source, device, pipe, winimg);
	if (wfderr != WFD_ERROR_NONE)
		goto out;

	if (!wfd_source_bind_handle(source))
		wfderr = WFD_ERROR_OUT_OF_MEMORY;
out:
	if (wfderr != WFD_ERROR_NONE) {
		wfd_device_err_store(device, wfderr);
		if (source) {
			assert(source->handle == WFD_INVALID_HANDLE);
			wfd_free_source(source); source = NULL;
		}
	}

	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(device, __func__);

	source_handle = source ? source->handle : WFD_INVALID_HANDLE;
	TRACE_API_F("returning source=%p with handle %"PRIhdl, source, source_handle);

	return source_handle;
}

WFD_API_CALL WFDSource WFD_APIENTRY
wfdCreateSourceFromStream(WFDDevice device_handle,
			  WFDPipeline pipeline_handle,
			  WFDNativeStreamType stream,
			  const WFDint *attribList) WFD_APIEXIT
{
	WFDErrorCode wfderr = WFD_ERROR_NOT_SUPPORTED;
	struct wfd_device *device;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &device, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return WFD_INVALID_HANDLE;
	}

	if (attribList && *attribList != WFD_NONE)
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;

	DEBUG1("%s for pipe=%d", __func__, pipe->wfd_pipe_id);

	wfd_device_err_store(device, wfderr);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(device, __func__);

	return WFD_INVALID_HANDLE;
}

WFD_API_CALL void WFD_APIENTRY
wfdDestroySource(WFDDevice device_handle,
		 WFDSource source_handle) WFD_APIEXIT
{
	struct wfd_device *device;
	struct wfd_source *source;

	if (!wfd_find_deivce_source_by_handle(device_handle, source_handle, &device, &source, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	DEBUG1("%s source_handle=%d", __func__, source_handle);

	lock_device(device);
	wfd_set_source_unused(device, source);
	wfd_source_ref_release(source, __func__); source = NULL;
	wfd_device_sources_destroy(device);
	unlock_device(device);

	wfd_device_ref_release(device, __func__);
}

WFD_API_CALL WFDMask WFD_APIENTRY
wfdCreateMaskFromImage(WFDDevice device_handle,
		       WFDPipeline pipeline_handle,
		       WFDEGLImage image_handle,
		       const WFDint *attribList) WFD_APIEXIT
{
	WFDErrorCode wfderr = WFD_ERROR_NOT_SUPPORTED;
	struct wfd_device *device;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &device, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return WFD_INVALID_HANDLE;
	}

	if (attribList && *attribList != WFD_NONE) {
		// no valid attributes defined in OpenWF 1.0
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;
	}

	DEBUG1("%s for pipe=%d", __func__, pipe->wfd_pipe_id);

	wfd_device_err_store(device, wfderr);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(device, __func__);

	return WFD_INVALID_HANDLE;
}

WFD_API_CALL WFDMask WFD_APIENTRY
wfdCreateMaskFromStream(WFDDevice device_handle,
			WFDPipeline pipeline_handle,
			WFDNativeStreamType stream,
			const WFDint *attribList) WFD_APIEXIT
{
	WFDErrorCode wfderr = WFD_ERROR_NOT_SUPPORTED;
	struct wfd_device *device;
	struct wfd_pipe *pipe;

	if (!wfd_find_deivce_pipeline_by_handle(device_handle, pipeline_handle, &device, &pipe, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return WFD_INVALID_HANDLE;
	}

	if (attribList && *attribList != WFD_NONE)
		wfderr = WFD_ERROR_BAD_ATTRIBUTE;

	DEBUG1("%s for pipe=%d", __func__, pipe->wfd_pipe_id);

	wfd_device_err_store(device, wfderr);
	wfd_pipe_ref_release(pipe, __func__);
	wfd_device_ref_release(device, __func__);

	return WFD_INVALID_HANDLE;
}

WFD_API_CALL void WFD_APIENTRY
wfdDestroyMask(WFDDevice device_handle, WFDMask mask_handle) WFD_APIEXIT
{
	struct wfd_device *device = wfd_find_deivce_by_handle(device_handle, __func__);

	if (!device) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	wfd_device_err_store(device, WFD_ERROR_BAD_HANDLE);
	wfd_device_ref_release(device, __func__);
}

/*** Standard API functions end ***/
