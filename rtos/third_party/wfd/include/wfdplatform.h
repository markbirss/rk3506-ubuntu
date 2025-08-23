/* Copyright (c) 2009 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

#ifndef _WFDPLATFORM_H_
#define _WFDPLATFORM_H_

#if defined(__RT_THREAD__)
#include <rtthread.h>
#include <rthw.h>
#else
#include <KHR/khrplatform.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define EOK 0

#if defined(__RT_THREAD__)
#ifndef GCCATTR
# if __GNUC__ >= 4
#  define GCCATTR(x) __attribute__ ((x))
# else
#  define GCCATTR(x)
# endif
#endif

#define SLOG_DUMMY	0
#define SLOGC_SELF	SLOG_DUMMY
#define _SLOG_SHUTDOWN	SLOG_DUMMY
#define _SLOG_CRITICAL	SLOG_DUMMY
#define _SLOG_ERROR	SLOG_DUMMY
#define _SLOG_WARNING	SLOG_DUMMY
#define _SLOG_NOTICE	SLOG_DUMMY
#define _SLOG_INFO	SLOG_DUMMY
#define _SLOG_DEBUG1	SLOG_DUMMY
#define _SLOG_DEBUG2	SLOG_DUMMY

#define slogf(code, severity, fmt, ...) rt_kprintf(fmt "\n", ##__VA_ARGS__)

#define malloc rt_malloc

typedef int errno_t;

typedef rt_int8_t   __s8;
typedef rt_uint8_t  __u8;
typedef rt_int16_t  __s16;
typedef rt_uint16_t __u16;
typedef rt_int32_t  __s32;
typedef rt_uint32_t __u32;
typedef rt_int64_t  __s64;
typedef rt_uint64_t __u64;

typedef rt_int8_t   s8;
typedef rt_uint8_t  u8;
typedef rt_int16_t  s16;
typedef rt_uint16_t u16;
typedef rt_int32_t  s32;
typedef rt_uint32_t u32;
typedef rt_int64_t  s64;
typedef rt_uint64_t u64;

typedef rt_uint16_t  __be16;
typedef rt_uint16_t  __le16;
typedef rt_uint32_t  __le32;
typedef rt_uint32_t  __be32;
typedef rt_uint64_t  __le64;
typedef rt_uint64_t  __be64;

#ifndef WFD_API_CALL
#define WFD_API_CALL
#endif
#ifndef WFD_APIENTRY
#define WFD_APIENTRY
#endif
#ifndef WFD_APIEXIT
#define WFD_APIEXIT
#endif

typedef enum
{ WFD_FALSE = RT_FALSE,
  WFD_TRUE  = RT_TRUE
} WFDboolean;

typedef rt_uint8_t   WFDuint8;
typedef rt_int32_t   WFDint;
typedef float        WFDfloat;
typedef rt_uint32_t  WFDbitfield;
typedef rt_uint32_t  WFDHandle;
typedef rt_uint64_t  WFDtime;

typedef struct win_image {
  int width, height;
  int format;
  int usage;
  int flags;
  int fd;
  unsigned long offset;
  int size;
  int padding;
  unsigned long paddr;
  int strides[2];
  void *vaddr;
  void *cvaddr;
  void *dvaddr;
  int planar_offsets[3];
  unsigned long *pages;
} win_image_t;
#else

#if defined(__QNX__) && defined(__GNUC__)
# ifndef WFD_API_CALL
#   define WFD_API_CALL __attribute__((visibility("default")))
# endif
#endif

#ifndef WFD_API_CALL
#define WFD_API_CALL KHRONOS_APICALL
#endif
#ifndef WFD_APIENTRY
#define WFD_APIENTRY KHRONOS_APIENTRY
#endif
#ifndef WFD_APIEXIT
#define WFD_APIEXIT KHRONOS_APIATTRIBUTES
#endif

typedef enum
{ WFD_FALSE = KHRONOS_FALSE,
  WFD_TRUE  = KHRONOS_TRUE
} WFDboolean;

typedef khronos_uint8_t             WFDuint8;
typedef khronos_int32_t             WFDint;
typedef khronos_float_t             WFDfloat;
typedef khronos_uint32_t            WFDbitfield;
typedef khronos_uint32_t            WFDHandle;
typedef khronos_utime_nanoseconds_t WFDtime;

#endif

#define WFD_FOREVER                 (0xFFFFFFFFFFFFFFFFLL)

#if defined (__QNXNTO__) || defined (__RT_THREAD__)
typedef void*  WFDEGLDisplay; /* An opaque handle to an EGLDisplay */
typedef void*  WFDEGLSync;    /* An opaque handle to an EGLSyncKHR */
typedef struct win_image*         WFDEGLImage;   /* An opaque handle to an EGLImage */
typedef struct win_native_stream* WFDNativeStreamType;
#else
#error "Platform not recognized"
#endif

#define WFD_INVALID_SYNC            ((WFDEGLSync)0)

#ifdef __cplusplus
}
#endif

#endif /* _WFDPLATFORM_H_ */
