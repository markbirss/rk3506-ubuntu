/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#if defined(HAL_VOP_MODULE_ENABLED) || defined(HAL_VOP2_MODULE_ENABLED) || defined(HAL_DSI_MODULE_ENABLED) || defined(HAL_DW_MIPI_DSI_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup DISPLAY
 *  @{
 */

#ifndef _HAL_DISPLAY_H_
#define _HAL_DISPLAY_H_

/***************************** MACRO Definition ******************************/
/** @defgroup DISPLAY_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/** RK display connector type */
#define RK_DISPLAY_CONNECTOR_UNKNOWN 0
#define RK_DISPLAY_CONNECTOR_RGB     1
#define RK_DISPLAY_CONNECTOR_DSI     2

/** video mode flag default polarity is positive */
#define VIDEO_MODE_FLAG_NHSYNC    HAL_BIT(0)
#define VIDEO_MODE_FLAG_NVSYNC    HAL_BIT(1)
#define VIDEO_MODE_FLAG_NPIXDATA  HAL_BIT(2)
#define VIDEO_MODE_FLAG_DEN       HAL_BIT(3)
#define VIDEO_MODE_FLAG_INTERLACE HAL_BIT(4)
#define VIDEO_MODE_FLAG_DBLSCAN   HAL_BIT(5)
#define VIDEO_MODE_FLAG_CSYNC     HAL_BIT(6)
#define VIDEO_MODE_FLAG_PCSYNC    HAL_BIT(7)
#define VIDEO_MODE_FLAG_NCSYNC    HAL_BIT(8)
#define VIDEO_MODE_FLAG_HSKEW     HAL_BIT(9) /* hskew provided */
#define VIDEO_MODE_FLAG_BCAST     HAL_BIT(10)
#define VIDEO_MODE_FLAG_PIXMUX    HAL_BIT(11)
#define VIDEO_MODE_FLAG_DBLCLK    HAL_BIT(12)
#define VIDEO_MODE_FLAG_CLKDIV2   HAL_BIT(13)
/** video mode */
#define DSI_MODE_VIDEO HAL_BIT(4)
/** video burst mode */
#define DSI_MODE_VIDEO_BURST HAL_BIT(5)
/** video pulse mode */
#define DSI_MODE_VIDEO_SYNC_PULSE HAL_BIT(6)
/** enable auto vertical count mode */
#define DSI_MODE_VIDEO_AUTO_VERT HAL_BIT(7)
/** disable EoT packets in HS mode */
#define DSI_MODE_EOT_PACKET HAL_BIT(8)
/** device supports non-continuous clock behavior (DSI spec 5.6.1) */
#define DSI_CLOCK_NON_CONTINUOUS HAL_BIT(9)
/** transmit data in low power */
#define DSI_MODE_LPM HAL_BIT(10)
#define DSC_ENABLE   HAL_BIT(11)

/** media bus format from DRM define */
/* RGB - next is	0x1024 */
#define MEDIA_BUS_FMT_RGB444_1X12           0x1016
#define MEDIA_BUS_FMT_RGB444_2X8_PADHI_BE   0x1001
#define MEDIA_BUS_FMT_RGB444_2X8_PADHI_LE   0x1002
#define MEDIA_BUS_FMT_RGB555_2X8_PADHI_BE   0x1003
#define MEDIA_BUS_FMT_RGB555_2X8_PADHI_LE   0x1004
#define MEDIA_BUS_FMT_RGB565_1X16           0x1017
#define MEDIA_BUS_FMT_BGR565_2X8_BE         0x1005
#define MEDIA_BUS_FMT_BGR565_2X8_LE         0x1006
#define MEDIA_BUS_FMT_RGB565_2X8_BE         0x1007
#define MEDIA_BUS_FMT_RGB565_2X8_LE         0x1008
#define MEDIA_BUS_FMT_RGB666_1X18           0x1009
#define MEDIA_BUS_FMT_RBG888_1X24           0x100e
#define MEDIA_BUS_FMT_RGB666_1X24_CPADHI    0x1015
#define MEDIA_BUS_FMT_RGB666_1X7X3_SPWG     0x1010
#define MEDIA_BUS_FMT_RGB666_1X7X3_JEIDA    0x101b
#define MEDIA_BUS_FMT_BGR888_1X24           0x1013
#define MEDIA_BUS_FMT_SRGB666_3X8           0x101b
#define MEDIA_BUS_FMT_BGR888_3X8            0x101b
#define MEDIA_BUS_FMT_GBR888_1X24           0x1014
#define MEDIA_BUS_FMT_RGB888_1X24           0x100a
#define MEDIA_BUS_FMT_RGB888_2X12_BE        0x100b
#define MEDIA_BUS_FMT_RGB888_2X12_LE        0x100c
#define MEDIA_BUS_FMT_SRGB888_3X8           0x101c
#define MEDIA_BUS_FMT_RGB888_3X8            0x101c
#define MEDIA_BUS_FMT_SBGR888_3X8           0x101d
#define MEDIA_BUS_FMT_SRBG888_3X8           0x101e
#define MEDIA_BUS_FMT_RGB888_1X7X4_SPWG     0x1011
#define MEDIA_BUS_FMT_RGB888_1X7X4_JEIDA    0x1012
#define MEDIA_BUS_FMT_ARGB8888_1X32         0x100d
#define MEDIA_BUS_FMT_RGB888_1X32_PADHI     0x100f
#define MEDIA_BUS_FMT_RGB101010_1X30        0x1018
#define MEDIA_BUS_FMT_RGB121212_1X36        0x1019
#define MEDIA_BUS_FMT_RGB161616_1X48        0x101a
#define MEDIA_BUS_FMT_SRGB888_DUMMY_4X8     0x101f
#define MEDIA_BUS_FMT_RGB888_DUMMY_4X8      0x101f
#define MEDIA_BUS_FMT_SBGR888_DUMMY_4X8     0x1020
#define MEDIA_BUS_FMT_BGR888_DUMMY_4X8      0x1020
#define MEDIA_BUS_FMT_SRBG888_DUMMY_4X8     0x1021
#define MEDIA_BUS_FMT_RGB101010_1X7X5_SPWG  0x1022
#define MEDIA_BUS_FMT_RGB101010_1X7X5_JEIDA 0x1023
#define MEDIA_BUS_FMT_SRGB666_3X6           0x1100
#define MEDIA_BUS_FMT_RGB666_3X6            0x1100

/* YUV (including grey) - next is	0x202e */
#define MEDIA_BUS_FMT_Y8_1X8           0x2001
#define MEDIA_BUS_FMT_UV8_1X8          0x2015
#define MEDIA_BUS_FMT_UYVY8_1_5X8      0x2002
#define MEDIA_BUS_FMT_VYUY8_1_5X8      0x2003
#define MEDIA_BUS_FMT_YUYV8_1_5X8      0x2004
#define MEDIA_BUS_FMT_YVYU8_1_5X8      0x2005
#define MEDIA_BUS_FMT_UYVY8_2X8        0x2006
#define MEDIA_BUS_FMT_VYUY8_2X8        0x2007
#define MEDIA_BUS_FMT_YUYV8_2X8        0x2008
#define MEDIA_BUS_FMT_YVYU8_2X8        0x2009
#define MEDIA_BUS_FMT_Y10_1X10         0x200a
#define MEDIA_BUS_FMT_Y10_2X8_PADHI_LE 0x202c
#define MEDIA_BUS_FMT_UYVY10_2X10      0x2018
#define MEDIA_BUS_FMT_VYUY10_2X10      0x2019
#define MEDIA_BUS_FMT_YUYV10_2X10      0x200b
#define MEDIA_BUS_FMT_YVYU10_2X10      0x200c
#define MEDIA_BUS_FMT_Y12_1X12         0x2013
#define MEDIA_BUS_FMT_UYVY12_2X12      0x201c
#define MEDIA_BUS_FMT_VYUY12_2X12      0x201d
#define MEDIA_BUS_FMT_YUYV12_2X12      0x201e
#define MEDIA_BUS_FMT_YVYU12_2X12      0x201f
#define MEDIA_BUS_FMT_Y14_1X14         0x202d
#define MEDIA_BUS_FMT_UYVY8_1X16       0x200f
#define MEDIA_BUS_FMT_VYUY8_1X16       0x2010
#define MEDIA_BUS_FMT_YUYV8_1X16       0x2011
#define MEDIA_BUS_FMT_YVYU8_1X16       0x2012
#define MEDIA_BUS_FMT_YDYUYDYV8_1X16   0x2014
#define MEDIA_BUS_FMT_UYVY10_1X20      0x201a
#define MEDIA_BUS_FMT_VYUY10_1X20      0x201b
#define MEDIA_BUS_FMT_YUYV10_1X20      0x200d
#define MEDIA_BUS_FMT_YVYU10_1X20      0x200e
#define MEDIA_BUS_FMT_VUY8_1X24        0x2024
#define MEDIA_BUS_FMT_YUV8_1X24        0x2025
#define MEDIA_BUS_FMT_UYYVYY8_0_5X24   0x2026
#define MEDIA_BUS_FMT_UYVY12_1X24      0x2020
#define MEDIA_BUS_FMT_VYUY12_1X24      0x2021
#define MEDIA_BUS_FMT_YUYV12_1X24      0x2022
#define MEDIA_BUS_FMT_YVYU12_1X24      0x2023
#define MEDIA_BUS_FMT_YUV10_1X30       0x2016
#define MEDIA_BUS_FMT_UYYVYY10_0_5X30  0x2027
#define MEDIA_BUS_FMT_AYUV8_1X32       0x2017
#define MEDIA_BUS_FMT_UYYVYY12_0_5X36  0x2028
#define MEDIA_BUS_FMT_YUV12_1X36       0x2029
#define MEDIA_BUS_FMT_YUV16_1X48       0x202a
#define MEDIA_BUS_FMT_UYYVYY16_0_5X48  0x202b

/***************************** Structure Definition **************************/

/**
 *  Add multi line brief to here
 *  ...
 */
struct DISPLAY_MODE_INFO {
    uint32_t clock;
    uint16_t hdisplay;
    uint16_t hsyncStart;
    uint16_t hsyncEnd;
    uint16_t htotal;
    uint16_t vdisplay;
    uint16_t vsyncStart;
    uint16_t vsyncEnd;
    uint16_t vtotal;
    uint32_t crtcClock;
    uint16_t crtcHdisplay;
    uint16_t crtcHsyncStart;
    uint16_t crtcHsyncEnd;
    uint16_t crtcHtotal;
    uint16_t crtcVdisplay;
    uint16_t crtcVsyncStart;
    uint16_t crtcVsyncEnd;
    uint16_t crtcVtotal;
    uint16_t flags;
    uint16_t vrefresh;
    uint16_t mcuCsRw;
    uint16_t mcuRwPw;
    uint16_t mcuRwCs;
};

/**
 * struct DISPLAY_RECT
 */
struct DISPLAY_RECT {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup DISPLAY_Public_Function_Declare Public Function Declare
 *  @{
 */

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_VOP_MODULE_ENABLED || HAL_VOP2_MODULE_ENABLED || HAL_DSI_MODULE_ENABLED || HAL_DW_MIPI_DSI_MODULE_ENABLED */
