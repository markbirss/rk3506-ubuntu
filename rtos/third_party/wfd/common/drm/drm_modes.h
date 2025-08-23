/*
 * Copyright 2022, Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _DRM_MODES_H
#define _DRM_MODES_H

#define u32	uint32_t
#define DRM_DISPLAY_INFO_LEN	32
#define DRM_CONNECTOR_NAME_LEN	32
#define DRM_DISPLAY_MODE_LEN	32
#define DRM_PROP_NAME_LEN	32

#define DRM_MODE_TYPE_BUILTIN	(1<<0)
#define DRM_MODE_TYPE_CLOCK_C	((1<<1) | DRM_MODE_TYPE_BUILTIN)
#define DRM_MODE_TYPE_CRTC_C	((1<<2) | DRM_MODE_TYPE_BUILTIN)
#define DRM_MODE_TYPE_PREFERRED	(1<<3)
#define DRM_MODE_TYPE_DEFAULT	(1<<4)
#define DRM_MODE_TYPE_USERDEF	(1<<5)
#define DRM_MODE_TYPE_DRIVER	(1<<6)

/* Video mode flags */
/* bit compatible with the xorg definitions. */
#define DRM_MODE_FLAG_PPIXDATA			BIT(31)

/* Panel Mirror control */
#define DRM_MODE_FLAG_XMIRROR			(1<<28)
#define DRM_MODE_FLAG_YMIRROR			(1<<29)
#define DRM_MODE_FLAG_XYMIRROR			(DRM_MODE_FLAG_XMIRROR | DRM_MODE_FLAG_YMIRROR)

/* Picture aspect ratio options */
#define DRM_MODE_PICTURE_ASPECT_NONE		0
#define DRM_MODE_PICTURE_ASPECT_4_3		1
#define DRM_MODE_PICTURE_ASPECT_16_9		2
#define DRM_MODE_PICTURE_ASPECT_64_27		3
#define DRM_MODE_PICTURE_ASPECT_256_135		4

#if defined(__RT_THREAD__)
/* This is for connectors with multiple signal types. */
/* Try to match DRM_MODE_CONNECTOR_X as closely as possible. */
enum drm_mode_subconnector {
	DRM_MODE_SUBCONNECTOR_Automatic   = 0,  /* DVI-I, TV     */
	DRM_MODE_SUBCONNECTOR_Unknown     = 0,  /* DVI-I, TV, DP */
	DRM_MODE_SUBCONNECTOR_VGA	  = 1,  /*            DP */
	DRM_MODE_SUBCONNECTOR_DVID	  = 3,  /* DVI-I      DP */
	DRM_MODE_SUBCONNECTOR_DVIA	  = 4,  /* DVI-I         */
	DRM_MODE_SUBCONNECTOR_Composite   = 5,  /*        TV     */
	DRM_MODE_SUBCONNECTOR_SVIDEO	  = 6,  /*        TV     */
	DRM_MODE_SUBCONNECTOR_Component   = 8,  /*        TV     */
	DRM_MODE_SUBCONNECTOR_SCART	  = 9,  /*        TV     */
	DRM_MODE_SUBCONNECTOR_DisplayPort = 10, /*            DP */
	DRM_MODE_SUBCONNECTOR_HDMIA       = 11, /*            DP */
	DRM_MODE_SUBCONNECTOR_Native      = 15, /*            DP */
	DRM_MODE_SUBCONNECTOR_Wireless    = 18, /*            DP */
};
#endif

#define DRM_MODE_CONNECTOR_Unknown	0
#define DRM_MODE_CONNECTOR_VGA		1
#define DRM_MODE_CONNECTOR_DVII		2
#define DRM_MODE_CONNECTOR_DVID		3
#define DRM_MODE_CONNECTOR_DVIA		4
#define DRM_MODE_CONNECTOR_Composite	5
#define DRM_MODE_CONNECTOR_SVIDEO	6
#define DRM_MODE_CONNECTOR_LVDS		7
#define DRM_MODE_CONNECTOR_Component	8
#define DRM_MODE_CONNECTOR_9PinDIN	9
#define DRM_MODE_CONNECTOR_DisplayPort	10
#define DRM_MODE_CONNECTOR_HDMIA	11
#define DRM_MODE_CONNECTOR_HDMIB	12
#define DRM_MODE_CONNECTOR_TV		13
#define DRM_MODE_CONNECTOR_eDP		14
#define DRM_MODE_CONNECTOR_VIRTUAL      15
#define DRM_MODE_CONNECTOR_DSI		16
#define DRM_MODE_CONNECTOR_DPI		17

#define DRM_EDID_PT_HSYNC_POSITIVE (1 << 1)
#define DRM_EDID_PT_VSYNC_POSITIVE (1 << 2)
#define DRM_EDID_PT_SEPARATE_SYNC  (3 << 3)
#define DRM_EDID_PT_STEREO         (1 << 5)
#define DRM_EDID_PT_INTERLACED     (1 << 7)

/* see also http://vektor.theorem.ca/graphics/ycbcr/ */
enum v4l2_colorspace {
	/*
	 * Default colorspace, i.e. let the driver figure it out.
	 * Can only be used with video capture.
	 */
	V4L2_COLORSPACE_DEFAULT       = 0,

	/* SMPTE 170M: used for broadcast NTSC/PAL SDTV */
	V4L2_COLORSPACE_SMPTE170M     = 1,

	/* Obsolete pre-1998 SMPTE 240M HDTV standard, superseded by Rec 709 */
	V4L2_COLORSPACE_SMPTE240M     = 2,

	/* Rec.709: used for HDTV */
	V4L2_COLORSPACE_REC709        = 3,

	/*
	 * Deprecated, do not use. No driver will ever return this. This was
	 * based on a misunderstanding of the bt878 datasheet.
	 */
	V4L2_COLORSPACE_BT878         = 4,

	/*
	 * NTSC 1953 colorspace. This only makes sense when dealing with
	 * really, really old NTSC recordings. Superseded by SMPTE 170M.
	 */
	V4L2_COLORSPACE_470_SYSTEM_M  = 5,

	/*
	 * EBU Tech 3213 PAL/SECAM colorspace. This only makes sense when
	 * dealing with really old PAL/SECAM recordings. Superseded by
	 * SMPTE 170M.
	 */
	V4L2_COLORSPACE_470_SYSTEM_BG = 6,

	/*
	 * Effectively shorthand for V4L2_COLORSPACE_SRGB, V4L2_YCBCR_ENC_601
	 * and V4L2_QUANTIZATION_FULL_RANGE. To be used for (Motion-)JPEG.
	 */
	V4L2_COLORSPACE_JPEG          = 7,

	/* For RGB colorspaces such as produces by most webcams. */
	V4L2_COLORSPACE_SRGB          = 8,

	/* AdobeRGB colorspace */
	V4L2_COLORSPACE_ADOBERGB      = 9,

	/* BT.2020 colorspace, used for UHDTV. */
	V4L2_COLORSPACE_BT2020        = 10,

	/* Raw colorspace: for RAW unprocessed images */
	V4L2_COLORSPACE_RAW           = 11,

	/* DCI-P3 colorspace, used by cinema projectors */
	V4L2_COLORSPACE_DCI_P3        = 12,
};

#define CRTC_INTERLACE_HALVE_V	(1 << 0) /* halve V values for interlacing */
#define CRTC_STEREO_DOUBLE	(1 << 1) /* adjust timings for stereo modes */
#define CRTC_NO_DBLSCAN		(1 << 2) /* don't adjust doublescan */
#define CRTC_NO_VSCAN		(1 << 3) /* don't adjust doublescan */
#define CRTC_STEREO_DOUBLE_ONLY	(CRTC_STEREO_DOUBLE | CRTC_NO_DBLSCAN | \
				 CRTC_NO_VSCAN)

#define DRM_MODE_FLAG_3D_MAX	DRM_MODE_FLAG_3D_SIDE_BY_SIDE_HALF

#define DRM_MODE_MATCH_TIMINGS		(1 << 0)
#define DRM_MODE_MATCH_CLOCK		(1 << 1)
#define DRM_MODE_MATCH_FLAGS		(1 << 2)
#define DRM_MODE_MATCH_3D_FLAGS		(1 << 3)
#define DRM_MODE_MATCH_ASPECT_RATIO	(1 << 4)

struct drm_display_mode {
	/* Proposed mode values */
	uint32_t clock;		/* in kHz */
	uint32_t hdisplay;
	uint32_t hsync_start;
	uint32_t hsync_end;
	uint32_t htotal;
	uint32_t vdisplay;
	uint32_t vsync_start;
	uint32_t vsync_end;
	uint32_t vtotal;
	uint64_t vrefresh;
	uint32_t vscan;
	unsigned long flags;
	uint32_t picture_aspect_ratio;
	uint32_t hskew;
	uint32_t type;
	/* Actual mode we give to hw */
	uint32_t crtc_clock;         /* in KHz */
	uint32_t crtc_hdisplay;
	uint32_t crtc_hblank_start;
	uint32_t crtc_hblank_end;
	uint32_t crtc_hsync_start;
	uint32_t crtc_hsync_end;
	uint32_t crtc_htotal;
	uint32_t crtc_hskew;
	uint32_t crtc_vdisplay;
	uint32_t crtc_vblank_start;
	uint32_t crtc_vblank_end;
	uint32_t crtc_vsync_start;
	uint32_t crtc_vsync_end;
	uint32_t crtc_vtotal;
	bool invalid;
};

/*
 * Subsystem independent description of a videomode.
 * Can be generated from struct display_timing.
 */
struct videomode {
	unsigned long pixelclock;	/* pixelclock in Hz */

	u32 hactive;
	u32 hfront_porch;
	u32 hback_porch;
	u32 hsync_len;

	u32 vactive;
	u32 vfront_porch;
	u32 vback_porch;
	u32 vsync_len;
	/*
	 * Disable for qnx compile
	 */
	//enum display_flags flags; /* display flags */
};

struct drm_display_mode *drm_mode_create(void);
void drm_mode_destroy(struct drm_display_mode *mode);
bool drm_mode_match(const struct drm_display_mode *mode1,
		    const struct drm_display_mode *mode2,
		    unsigned int match_flags);
bool drm_mode_equal(const struct drm_display_mode *mode1,
		    const struct drm_display_mode *mode2);
void drm_display_mode_to_videomode(const struct drm_display_mode *dmode,
				   struct videomode *vm);

#endif
