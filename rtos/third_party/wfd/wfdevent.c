/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <stdbool.h>

#include "wfddev.h"

struct wfd_event;

static bool wfd_find_deivce_event_by_handle(WFDDevice device_handle, WFDEvent event_handle,
					    struct wfd_device **pdevice, struct wfd_event **pevent,
					    const void *refkey)
{
	struct wfd_device *dev = wfd_find_deivce_by_handle(device_handle, refkey);

	if (dev) {
		wfd_device_err_store(dev, WFD_ERROR_BAD_HANDLE);
		wfd_device_ref_release(dev, refkey);
	}

	return false;
}

static void wfd_event_ref_release(struct wfd_event *event, const void *refkey)
{
	return;
}

/*** Standard API functions start ***/

WFD_API_CALL WFDEvent WFD_APIENTRY
wfdCreateEvent(WFDDevice device_handle, const WFDint *attr_list) WFD_APIEXIT
{
	TRACE_API;
	struct wfd_device *dev = wfd_find_deivce_by_handle(device_handle, __func__);

	if (!dev) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return WFD_INVALID_HANDLE;
	}

	wfd_device_err_store(dev, WFD_ERROR_NOT_SUPPORTED);
	wfd_device_ref_release(dev, __func__);

	return WFD_INVALID_HANDLE;
}

WFD_API_CALL void WFD_APIENTRY
wfdDestroyEvent(WFDDevice device_handle, WFDEvent event_handle) WFD_APIEXIT
{
	TRACE_API;
	struct wfd_device *dev = NULL;
	struct wfd_event *event;

	if (!wfd_find_deivce_event_by_handle(device_handle, event_handle, &dev, &event, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	wfd_device_err_store(dev, WFD_ERROR_NOT_SUPPORTED);
	wfd_event_ref_release(event, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL WFDint WFD_APIENTRY
wfdGetEventAttribi(WFDDevice device_handle,
		   WFDEvent event_handle,
		   WFDEventAttrib attr) WFD_APIEXIT
{
	TRACE_API;
	struct wfd_device *dev = NULL;
	struct wfd_event *event;

	if (!wfd_find_deivce_event_by_handle(device_handle, event_handle, &dev, &event, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return -1;
	}

	wfd_device_err_store(dev, WFD_ERROR_NOT_SUPPORTED);
	wfd_event_ref_release(event, __func__);
	wfd_device_ref_release(dev, __func__);

	return -1;
}

WFD_API_CALL void WFD_APIENTRY
wfdDeviceEventAsync(WFDDevice device_handle,
		    WFDEvent event_handle,
		    WFDEGLDisplay display,
		    WFDEGLSync sync) WFD_APIEXIT
{
	TRACE_API;
	struct wfd_device *dev = NULL;
	struct wfd_event *event;

	if (!wfd_find_deivce_event_by_handle(device_handle, event_handle, &dev, &event, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	wfd_device_err_store(dev, WFD_ERROR_NOT_SUPPORTED);
	wfd_event_ref_release(event, __func__);
	wfd_device_ref_release(dev, __func__);
}

WFD_API_CALL WFDEventType WFD_APIENTRY
wfdDeviceEventWait(WFDDevice device_handle,
		   WFDEvent event_handle,
		   WFDtime timeout) WFD_APIEXIT
{
	TRACE_API;
	struct wfd_device *dev = NULL;
	struct wfd_event *event;

	if (!wfd_find_deivce_event_by_handle(device_handle, event_handle, &dev, &event, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return WFD_EVENT_INVALID;
	}

	wfd_device_err_store(dev, WFD_ERROR_NOT_SUPPORTED);
	wfd_event_ref_release(event, __func__);
	wfd_device_ref_release(dev, __func__);

	return WFD_EVENT_INVALID;
}

WFD_API_CALL void WFD_APIENTRY
wfdDeviceEventFilter(WFDDevice device_handle,
		     WFDEvent event_handle,
		     const WFDEventType *filter) WFD_APIEXIT
{
	TRACE_API;
	struct wfd_device *dev = NULL;
	struct wfd_event *event;

	if (!wfd_find_deivce_event_by_handle(device_handle, event_handle, &dev, &event, __func__)) {
		TRACE_API_F("failed: %s", "bad handle(s)");
		return;
	}

	wfd_device_err_store(dev, WFD_ERROR_NOT_SUPPORTED);
	wfd_event_ref_release(event, __func__);
	wfd_device_ref_release(dev, __func__);
}

/*** Standard API functions end ***/
