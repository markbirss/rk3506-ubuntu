/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef _WFD_H_
# error include <wfd.h> before including <wfdext.h>
#endif
#ifndef __wfdext_h_
#define __wfdext_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <wfdplatform.h>

/*************************************************************/

#if defined(__RT_THREAD__)
#define WFD_EXTNAME_WFD_EGL_IMAGES "wfd_egl_images"
#define WFD_USAGE_DISPLAY      (1 << 0)
#define WFD_USAGE_READ         (1 << 1)
#define WFD_USAGE_WRITE        (1 << 2)
#define WFD_USAGE_NATIVE       (1 << 3)
#define WFD_USAGE_OPENGL_ES1   (1 << 4)
#define WFD_USAGE_OPENGL_ES2   (1 << 5)
#define WFD_USAGE_OPENGL_ES3   (1 << 11)
#define WFD_USAGE_OPENVG       (1 << 6)
#define WFD_USAGE_VIDEO        (1 << 7)
#define WFD_USAGE_CAPTURE      (1 << 8)
#define WFD_USAGE_ROTATION     (1 << 9)
#define WFD_USAGE_OVERLAY      (1 << 10)
#define WFD_USAGE_COMPRESSION  (1 << 12)
#define WFD_USAGE_PHYSICAL     (1 << 13)
#define WFD_USAGE_VULKAN       (1 << 14)
#define WFD_USAGE_UNSYNC       (1 << 28)
#define WFD_USAGE_WRITEBACK    (1 << 31)

#define WFD_FORMAT_BYTE                  1
#define WFD_FORMAT_RGBA4444              2
#define WFD_FORMAT_RGBX4444              3
#define WFD_FORMAT_RGBA5551              4
#define WFD_FORMAT_RGBX5551              5
#define WFD_FORMAT_RGB565                6
#define WFD_FORMAT_RGB888                7
#define WFD_FORMAT_RGBA8888              8
#define WFD_FORMAT_RGBX8888              9
#define WFD_FORMAT_YVU9                 10
#define WFD_FORMAT_YUV420               11
#define WFD_FORMAT_NV12                 12
#define WFD_FORMAT_YV12                 13
#define WFD_FORMAT_UYVY                 14
#define WFD_FORMAT_YUY2                 15
#define WFD_FORMAT_YVYU                 16
#define WFD_FORMAT_V422                 17
#define WFD_FORMAT_AYUV                 18
#define WFD_FORMAT_NV16                 19
#define WFD_FORMAT_P010                 20
#define WFD_FORMAT_BGRA8888             21
#define WFD_FORMAT_BGRX8888             22
#define WFD_FORMAT_RGBA1010102          23
#define WFD_FORMAT_RGBX1010102          24
#define WFD_FORMAT_BGRA1010102          25
#define WFD_FORMAT_BGRX1010102          26
#define WFD_BASE_FORMAT_MASK                (WFDint)0xffff
#define WFD_BASE_FORMAT(x)                  ((WFDint)(x) & WFD_BASE_FORMAT_MASK)

WFD_API_CALL WFDErrorCode WFD_APIENTRY
    wfdCreateWFDEGLImages(WFDDevice device, WFDint width, WFDint height, WFDint format, WFDint usage, WFDint count, WFDEGLImage *images);
WFD_API_CALL WFDErrorCode WFD_APIENTRY
    wfdDestroyWFDEGLImages(WFDDevice device, WFDint count, WFDEGLImage *images);
typedef WFDErrorCode (WFD_APIENTRY PFNWFDCREATEWFDEGLIMAGES) (WFDDevice device, WFDint width, WFDint height, WFDint usage, WFDint count, WFDEGLImage *images);
typedef WFDErrorCode (WFD_APIENTRY PFNWFDDESTROYWFDEGLIMAGES) (WFDDevice device, WFDint count, WFDEGLImage *images);
#endif

#ifdef __cplusplus
}
#endif

#endif
