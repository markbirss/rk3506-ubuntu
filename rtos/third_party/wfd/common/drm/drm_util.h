
/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef _DRM_UTIL_H
#define _DRM_UTIL_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#if defined(__RT_THREAD__)
#include <wfdplatform.h>
#include <common/util.h>
#include <common/drm/drm_fourcc.h>
#else
#include <hw/inout.h>
#include <hw/utils.h>
#endif

#include "drm/drm_modes.h"
#include "drm/drm_rect.h"

#if defined(__RT_THREAD__)
#define udelay(us)	rt_hw_us_delay(us)
#define mdelay(ms)	rt_thread_mdelay(ms)
#else
#define udelay(us)	nanospin_ns(1000 * (us))
#define mdelay(ms)	delay(ms)

#define __packed        __attribute__((packed))
#endif

/**
 * do_div - returns 2 values: calculate remainder and update new dividend
 * @n: uint64_t dividend (will be updated)
 * @base: uint32_t divisor
 *
 * Summary:
 * ``uint32_t remainder = n % base;``
 * ``n = n / base;``
 *
 * Return: (uint32_t)remainder
 *
 * NOTE: macro parameter @n is evaluated multiple times,
 * beware of side effects!
 */
# define do_div(n,base) ({					\
	uint64_t __base = (base);				\
	uint64_t __rem;						\
	__rem = ((uint64_t)(n)) % __base;			\
	(n) = ((uint64_t)(n)) / __base;				\
	__rem;							\
 })

#define DIV_ROUND_DOWN_ULL(ll, d) \
	({ unsigned long long _tmp = (ll); do_div(_tmp, d); _tmp; })

#define DIV_ROUND_UP_ULL(ll, d) \
	DIV_ROUND_DOWN_ULL((unsigned long long)(ll) + (d) - 1, (d))

/*
 * Same as above but for u64 dividends. divisor must be a 32-bit
 * number.
 */
#define DIV_ROUND_CLOSEST_ULL(x, divisor)(		\
{							\
	typeof(divisor) __d = divisor;			\
	typeof(x) _tmp = (x) + (__d) / 2;		\
	do_div(_tmp, __d);				\
	_tmp;						\
}							\
)

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#ifndef mul_u32_u32
/*
 * Many a GCC version messes this up and generates a 64x64 mult :-(
 */
static inline uint64_t mul_u32_u32(u32 a, u32 b)
{
	return (uint64_t)a * b;
}
#endif

#define ALIGN_DOWN(x, a)	__ALIGN_KERNEL((x) - ((a) - 1), (a))

#define __bf_shf(x) (__builtin_ffsll(x) - 1)

#define FIELD_FIT(_mask, _val)						\
	({								\
		!((((typeof(_mask))_val) << __bf_shf(_mask)) & ~(_mask)); \
	})

#define FIELD_PREP(_mask, _val)		\
	({	\
		((typeof(_mask))(_val) << __bf_shf(_mask)) & (_mask); \
	})

#define FIELD_GET(_mask, _reg)                                          \
	({ \
		(typeof(_mask))(((_reg) & (_mask)) >> __bf_shf(_mask));   \
	})

#if defined(__RT_THREAD__)
static inline unsigned long __ffs(unsigned long word)
{
	unsigned long num = 0;

#if BITS_PER_LONG == 64
	if ((word & 0xffffffff) == 0) {
		num += 32;
		word >>= 32;
	}
#endif
	if ((word & 0xffff) == 0) {
		num += 16;
		word >>= 16;
	}
	if ((word & 0xff) == 0) {
		num += 8;
		word >>= 8;
	}
	if ((word & 0xf) == 0) {
		num += 4;
		word >>= 4;
	}
	if ((word & 0x3) == 0) {
		num += 2;
		word >>= 2;
	}
	if ((word & 0x1) == 0)
		num += 1;
	return num;
}

static inline unsigned long _find_next_bit(const unsigned long *addr1,
		const unsigned long *addr2, unsigned long nbits,
		unsigned long start, unsigned long invert)
{
	unsigned long tmp, mask;

	if (start >= nbits)
		return nbits;

	tmp = addr1[start / BITS_PER_LONG];
	if (addr2)
		tmp &= addr2[start / BITS_PER_LONG];
	tmp ^= invert;

	/* Handle 1st word. */
	mask = BITMAP_FIRST_WORD_MASK(start);

	tmp &= mask;

	start = round_down(start, BITS_PER_LONG);

	while (!tmp) {
		start += BITS_PER_LONG;
		if (start >= nbits)
			return nbits;

		tmp = addr1[start / BITS_PER_LONG];
		if (addr2)
			tmp &= addr2[start / BITS_PER_LONG];
		tmp ^= invert;
	}

	#ifdef __cplusplus
		return __min(start + __ffs(tmp), nbits);
	#else
		return min(start + __ffs(tmp), nbits);
	#endif
}

static inline unsigned long find_next_zero_bit(const unsigned long *addr, unsigned long size,
				 unsigned long offset)
{
	return _find_next_bit(addr, NULL, size, offset, ~0UL);
}

static inline unsigned long find_next_bit(const unsigned long *addr, unsigned long size,
			    unsigned long offset)
{
	return _find_next_bit(addr, NULL, size, offset, 0UL);
}
#endif

/**
 * __ffs - find first bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */

#define for_each_set_bit(bit, addr, size) \
	for ((bit) = find_first_bit((addr), (size));		\
	     (bit) < (size);					\
	     (bit) = find_next_bit((addr), (size), (bit) + 1))

static inline unsigned int generic_hweight32(unsigned int w)
{
	unsigned int res = (w & 0x55555555) + ((w >> 1) & 0x55555555);
        res = (res & 0x33333333) + ((res >> 2) & 0x33333333);
        res = (res & 0x0F0F0F0F) + ((res >> 4) & 0x0F0F0F0F);
        res = (res & 0x00FF00FF) + ((res >> 8) & 0x00FF00FF);
        return (res & 0x0000FFFF) + ((res >> 16) & 0x0000FFFF);
}

static inline unsigned int generic_hweight16(unsigned int w)
{
        unsigned int res = (w & 0x5555) + ((w >> 1) & 0x5555);
        res = (res & 0x3333) + ((res >> 2) & 0x3333);
        res = (res & 0x0F0F) + ((res >> 4) & 0x0F0F);
        return (res & 0x00FF) + ((res >> 8) & 0x00FF);
}

static inline unsigned int generic_hweight8(unsigned int w)
{
        unsigned int res = (w & 0x55) + ((w >> 1) & 0x55);
        res = (res & 0x33) + ((res >> 2) & 0x33);
        return (res & 0x0F) + ((res >> 4) & 0x0F);
}

static inline unsigned long generic_hweight64(uint64_t w)
{
	return generic_hweight32((unsigned int)(w >> 32)) +
               generic_hweight32((unsigned int)w);
}

static inline unsigned long hweight_long(unsigned long w)
{
        return sizeof(w) == 4 ? generic_hweight32(w) : generic_hweight64(w);
}

#define hweight32(x) generic_hweight32(x)
#define hweight16(x) generic_hweight16(x)
#define hweight8(x) generic_hweight8(x)

/**************from-linux-drm*****************/

/*
 * 2 plane YCbCr
 * index 0 = Y plane, [39:0] Y3:Y2:Y1:Y0 little endian
 * index 1 = Cr:Cb plane, [39:0] Cr1:Cb1:Cr0:Cb0 little endian
 */
#define DRM_FORMAT_NV20		fourcc_code('N', 'V', '2', '0') /* 2x1 subsampled Cr:Cb plane */
#define DRM_FORMAT_NV30		fourcc_code('N', 'V', '3', '0') /* non-subsampled Cr:Cb plane */

#define AFBC_VENDOR_AND_TYPE_MASK    GENMASK_ULL(63, 52)
#define drm_is_afbc(modifier) \
	(((modifier) & AFBC_VENDOR_AND_TYPE_MASK) == DRM_FORMAT_MOD_ARM_AFBC(0))


#define DRM_MODE_BLEND_PREMULTI         0
#define DRM_MODE_BLEND_COVERAGE         1
#define DRM_MODE_BLEND_PIXEL_NONE       2

enum hdmi_eotf {
	HDMI_EOTF_TRADITIONAL_GAMMA_SDR,
	HDMI_EOTF_TRADITIONAL_GAMMA_HDR,
	HDMI_EOTF_SMPTE_ST2084,
	HDMI_EOTF_BT_2100_HLG,
};

/**
 * struct drm_connector_tv_margins - TV connector related margins
 *
 * Describes the margins in pixels to put around the image on TV
 * connectors to deal with overscan.
 */
struct drm_connector_tv_margins {
	/**
	 * @bottom: Bottom margin in pixels.
	 */
	unsigned int bottom;

	/**
	 * @left: Left margin in pixels.
	 */
	unsigned int left;

	/**
	 * @right: Right margin in pixels.
	 */
	unsigned int right;

	/**
	 * @top: Top margin in pixels.
	 */
	unsigned int top;
};

/**
 * struct drm_tv_connector_state - TV connector related states
 * @subconnector: selected subconnector
 * @margins: TV margins
 * @mode: TV mode
 * @brightness: brightness in percent
 * @contrast: contrast in percent
 * @flicker_reduction: flicker reduction in percent
 * @overscan: overscan in percent
 * @saturation: saturation in percent
 * @hue: hue in percent
 */
struct drm_tv_connector_state {
	enum drm_mode_subconnector subconnector;
	struct drm_connector_tv_margins margins;
	unsigned int mode;
	unsigned int brightness;
	unsigned int contrast;
	unsigned int flicker_reduction;
	unsigned int overscan;
	unsigned int saturation;
	unsigned int hue;
};


 /* struct drm_crtc - central CRTC control structure
 *
 * Each CRTC may have one or more connectors associated with it.  This structure
 * allows the CRTC to be controlled.
 */
struct drm_crtc {
	/** @dev: parent DRM device */
	//struct drm_device *dev;
	/** @port: OF node used by drm_of_find_possible_crtcs(). */
	//struct device_node *port;
	/**
	 * @head:
	 *
	 * List of all CRTCs on @dev, linked from &drm_mode_config.crtc_list.
	 * Invariant over the lifetime of @dev and therefore does not need
	 * locking.
	 */
	//struct list_head head;

	/** @name: human readable name, can be overwritten by the driver */
	char *name;

	/**
	 * @mutex:
	 *
	 * This provides a read lock for the overall CRTC state (mode, dpms
	 * state, ...) and a write lock for everything which can be update
	 * without a full modeset (fb, cursor data, CRTC properties ...). A full
	 * modeset also need to grab &drm_mode_config.connection_mutex.
	 *
	 * For atomic drivers specifically this protects @state.
	 */
	//struct drm_modeset_lock mutex;

	/** @base: base KMS object for ID tracking etc. */
	//struct drm_mode_object base;

	/**
	 * @primary:
	 * Primary plane for this CRTC. Note that this is only
	 * relevant for legacy IOCTL, it specifies the plane implicitly used by
	 * the SETCRTC and PAGE_FLIP IOCTLs. It does not have any significance
	 * beyond that.
	 */
	struct drm_plane *primary;

	/**
	 * @cursor:
	 * Cursor plane for this CRTC. Note that this is only relevant for
	 * legacy IOCTL, it specifies the plane implicitly used by the SETCURSOR
	 * and SETCURSOR2 IOCTLs. It does not have any significance
	 * beyond that.
	 */
	struct drm_plane *cursor;

	/**
	 * @index: Position inside the mode_config.list, can be used as an array
	 * index. It is invariant over the lifetime of the CRTC.
	 */
	unsigned index;

	/**
	 * @cursor_x: Current x position of the cursor, used for universal
	 * cursor planes because the SETCURSOR IOCTL only can update the
	 * framebuffer without supplying the coordinates. Drivers should not use
	 * this directly, atomic drivers should look at &drm_plane_state.crtc_x
	 * of the cursor plane instead.
	 */
	int cursor_x;
	/**
	 * @cursor_y: Current y position of the cursor, used for universal
	 * cursor planes because the SETCURSOR IOCTL only can update the
	 * framebuffer without supplying the coordinates. Drivers should not use
	 * this directly, atomic drivers should look at &drm_plane_state.crtc_y
	 * of the cursor plane instead.
	 */
	int cursor_y;

	/**
	 * @enabled:
	 *
	 * Is this CRTC enabled? Should only be used by legacy drivers, atomic
	 * drivers should instead consult &drm_crtc_state.enable and
	 * &drm_crtc_state.active. Atomic drivers can update this by calling
	 * drm_atomic_helper_update_legacy_modeset_state().
	 */
	bool enabled;

	/**
	 * @mode:
	 *
	 * Current mode timings. Should only be used by legacy drivers, atomic
	 * drivers should instead consult &drm_crtc_state.mode. Atomic drivers
	 * can update this by calling
	 * drm_atomic_helper_update_legacy_modeset_state().
	 */
	struct drm_display_mode mode;

	/**
	 * @hwmode:
	 *
	 * Programmed mode in hw, after adjustments for encoders, crtc, panel
	 * scaling etc. Should only be used by legacy drivers, for high
	 * precision vblank timestamps in
	 * drm_crtc_vblank_helper_get_vblank_timestamp().
	 *
	 * Note that atomic drivers should not use this, but instead use
	 * &drm_crtc_state.adjusted_mode. And for high-precision timestamps
	 * drm_crtc_vblank_helper_get_vblank_timestamp() used
	 * &drm_vblank_crtc.hwmode,
	 * which is filled out by calling drm_calc_timestamping_constants().
	 */
	struct drm_display_mode hwmode;

	/**
	 * @x:
	 * x position on screen. Should only be used by legacy drivers, atomic
	 * drivers should look at &drm_plane_state.crtc_x of the primary plane
	 * instead. Updated by calling
	 * drm_atomic_helper_update_legacy_modeset_state().
	 */
	int x;
	/**
	 * @y:
	 * y position on screen. Should only be used by legacy drivers, atomic
	 * drivers should look at &drm_plane_state.crtc_y of the primary plane
	 * instead. Updated by calling
	 * drm_atomic_helper_update_legacy_modeset_state().
	 */
	int y;

	/** @funcs: CRTC control functions */
	//const struct drm_crtc_funcs *funcs;

	/**
	 * @gamma_size: Size of legacy gamma ramp reported to userspace. Set up
	 * by calling drm_mode_crtc_set_gamma_size().
	 */
	uint32_t gamma_size;

	/**
	 * @gamma_store: Gamma ramp values used by the legacy SETGAMMA and
	 * GETGAMMA IOCTls. Set up by calling drm_mode_crtc_set_gamma_size().
	 */
	uint16_t *gamma_store;

	/** @helper_private: mid-layer private data */
	//const struct drm_crtc_helper_funcs *helper_private;

	/** @properties: property tracking for this CRTC */
	//struct drm_object_properties properties;

	/**
	 * @state:
	 *
	 * Current atomic state for this CRTC.
	 *
	 * This is protected by @mutex. Note that nonblocking atomic commits
	 * access the current CRTC state without taking locks. Either by going
	 * through the &struct drm_atomic_state pointers, see
	 * for_each_oldnew_crtc_in_state(), for_each_old_crtc_in_state() and
	 * for_each_new_crtc_in_state(). Or through careful ordering of atomic
	 * commit operations as implemented in the atomic helpers, see
	 * &struct drm_crtc_commit.
	 */
	struct drm_crtc_state *state;

	/**
	 * @commit_list:
	 *
	 * List of &drm_crtc_commit structures tracking pending commits.
	 * Protected by @commit_lock. This list holds its own full reference,
	 * as does the ongoing commit.
	 *
	 * "Note that the commit for a state change is also tracked in
	 * &drm_crtc_state.commit. For accessing the immediately preceding
	 * commit in an atomic update it is recommended to just use that
	 * pointer in the old CRTC state, since accessing that doesn't need
	 * any locking or list-walking. @commit_list should only be used to
	 * stall for framebuffer cleanup that's signalled through
	 * &drm_crtc_commit.cleanup_done."
	 */
	//struct list_head commit_list;

	/**
	 * @commit_lock:
	 *
	 * Spinlock to protect @commit_list.
	 */
	//spinlock_t commit_lock;

#ifdef CONFIG_DEBUG_FS
	/**
	 * @debugfs_entry:
	 *
	 * Debugfs directory for this CRTC.
	 */
	struct dentry *debugfs_entry;
#endif

	/**
	 * @crc:
	 *
	 * Configuration settings of CRC capture.
	 */
	//struct drm_crtc_crc crc;

	/**
	 * @fence_context:
	 *
	 * timeline context used for fence operations.
	 */
	unsigned int fence_context;

	/**
	 * @fence_lock:
	 *
	 * spinlock to protect the fences in the fence_context.
	 */
	//spinlock_t fence_lock;
	/**
	 * @fence_seqno:
	 *
	 * Seqno variable used as monotonic counter for the fences
	 * created on the CRTC's timeline.
	 */
	unsigned long fence_seqno;

	/**
	 * @timeline_name:
	 *
	 * The name of the CRTC's fence timeline.
	 */
	char timeline_name[32];

	/**
	 * @self_refresh_data: Holds the state for the self refresh helpers
	 *
	 * Initialized via drm_self_refresh_helper_init().
	 */
	//struct drm_self_refresh_data *self_refresh_data;
};

/**
 * struct drm_crtc_state - mutable CRTC state
 *
 * Note that the distinction between @enable and @active is rather subtle:
 * Flipping @active while @enable is set without changing anything else may
 * never return in a failure from the &drm_mode_config_funcs.atomic_check
 * callback. Userspace assumes that a DPMS On will always succeed. In other
 * words: @enable controls resource assignment, @active controls the actual
 * hardware state.
 *
 * The three booleans active_changed, connectors_changed and mode_changed are
 * intended to indicate whether a full modeset is needed, rather than strictly
 * describing what has changed in a commit. See also:
 * drm_atomic_crtc_needs_modeset()
 *
 * WARNING: Transitional helpers (like drm_helper_crtc_mode_set() or
 * drm_helper_crtc_mode_set_base()) do not maintain many of the derived control
 * state like @plane_mask so drivers not converted over to atomic helpers should
 * not rely on these being accurate!
 */
struct drm_crtc_state {
	/** @crtc: backpointer to the CRTC */
	struct drm_crtc *crtc;

	/**
	 * @enable: Whether the CRTC should be enabled, gates all other state.
	 * This controls reservations of shared resources. Actual hardware state
	 * is controlled by @active.
	 */
	bool enable;

	/**
	 * @active: Whether the CRTC is actively displaying (used for DPMS).
	 * Implies that @enable is set. The driver must not release any shared
	 * resources if @active is set to false but @enable still true, because
	 * userspace expects that a DPMS ON always succeeds.
	 *
	 * Hence drivers must not consult @active in their various
	 * &drm_mode_config_funcs.atomic_check callback to reject an atomic
	 * commit. They can consult it to aid in the computation of derived
	 * hardware state, since even in the DPMS OFF state the display hardware
	 * should be as much powered down as when the CRTC is completely
	 * disabled through setting @enable to false.
	 */
	bool active;

	/**
	 * @planes_changed: Planes on this crtc are updated. Used by the atomic
	 * helpers and drivers to steer the atomic commit control flow.
	 */
	bool planes_changed : 1;

	/**
	 * @mode_changed: @mode or @enable has been changed. Used by the atomic
	 * helpers and drivers to steer the atomic commit control flow. See also
	 * drm_atomic_crtc_needs_modeset().
	 *
	 * Drivers are supposed to set this for any CRTC state changes that
	 * require a full modeset. They can also reset it to false if e.g. a
	 * @mode change can be done without a full modeset by only changing
	 * scaler settings.
	 */
	bool mode_changed : 1;

	/**
	 * @active_changed: @active has been toggled. Used by the atomic
	 * helpers and drivers to steer the atomic commit control flow. See also
	 * drm_atomic_crtc_needs_modeset().
	 */
	bool active_changed : 1;

	/**
	 * @connectors_changed: Connectors to this crtc have been updated,
	 * either in their state or routing. Used by the atomic
	 * helpers and drivers to steer the atomic commit control flow. See also
	 * drm_atomic_crtc_needs_modeset().
	 *
	 * Drivers are supposed to set this as-needed from their own atomic
	 * check code, e.g. from &drm_encoder_helper_funcs.atomic_check
	 */
	bool connectors_changed : 1;
	/**
	 * @zpos_changed: zpos values of planes on this crtc have been updated.
	 * Used by the atomic helpers and drivers to steer the atomic commit
	 * control flow.
	 */
	bool zpos_changed : 1;
	/**
	 * @color_mgmt_changed: Color management properties have changed
	 * (@gamma_lut, @degamma_lut or @ctm). Used by the atomic helpers and
	 * drivers to steer the atomic commit control flow.
	 */
	bool color_mgmt_changed : 1;

	/**
	 * @no_vblank:
	 *
	 * Reflects the ability of a CRTC to send VBLANK events. This state
	 * usually depends on the pipeline configuration. If set to true, DRM
	 * atomic helpers will send out a fake VBLANK event during display
	 * updates after all hardware changes have been committed. This is
	 * implemented in drm_atomic_helper_fake_vblank().
	 *
	 * One usage is for drivers and/or hardware without support for VBLANK
	 * interrupts. Such drivers typically do not initialize vblanking
	 * (i.e., call drm_vblank_init() with the number of CRTCs). For CRTCs
	 * without initialized vblanking, this field is set to true in
	 * drm_atomic_helper_check_modeset(), and a fake VBLANK event will be
	 * send out on each update of the display pipeline by
	 * drm_atomic_helper_fake_vblank().
	 *
	 * Another usage is CRTCs feeding a writeback connector operating in
	 * oneshot mode. In this case the fake VBLANK event is only generated
	 * when a job is queued to the writeback connector, and we want the
	 * core to fake VBLANK events when this part of the pipeline hasn't
	 * changed but others had or when the CRTC and connectors are being
	 * disabled.
	 *
	 * __drm_atomic_helper_crtc_duplicate_state() will not reset the value
	 * from the current state, the CRTC driver is then responsible for
	 * updating this field when needed.
	 *
	 * Note that the combination of &drm_crtc_state.event == NULL and
	 * &drm_crtc_state.no_blank == true is valid and usually used when the
	 * writeback connector attached to the CRTC has a new job queued. In
	 * this case the driver will send the VBLANK event on its own when the
	 * writeback job is complete.
	 */
	bool no_vblank : 1;

	/**
	 * @plane_mask: Bitmask of drm_plane_mask(plane) of planes attached to
	 * this CRTC.
	 */
	u32 plane_mask;

	/**
	 * @connector_mask: Bitmask of drm_connector_mask(connector) of
	 * connectors attached to this CRTC.
	 */
	u32 connector_mask;

	/**
	 * @encoder_mask: Bitmask of drm_encoder_mask(encoder) of encoders
	 * attached to this CRTC.
	 */
	u32 encoder_mask;

	/**
	 * @adjusted_mode:
	 *
	 * Internal display timings which can be used by the driver to handle
	 * differences between the mode requested by userspace in @mode and what
	 * is actually programmed into the hardware.
	 *
	 * For drivers using &drm_bridge, this stores hardware display timings
	 * used between the CRTC and the first bridge. For other drivers, the
	 * meaning of the adjusted_mode field is purely driver implementation
	 * defined information, and will usually be used to store the hardware
	 * display timings used between the CRTC and encoder blocks.
	 */
	struct drm_display_mode adjusted_mode;

	/**
	 * @mode:
	 *
	 * Display timings requested by userspace. The driver should try to
	 * match the refresh rate as close as possible (but note that it's
	 * undefined what exactly is close enough, e.g. some of the HDMI modes
	 * only differ in less than 1% of the refresh rate). The active width
	 * and height as observed by userspace for positioning planes must match
	 * exactly.
	 *
	 * For external connectors where the sink isn't fixed (like with a
	 * built-in panel), this mode here should match the physical mode on the
	 * wire to the last details (i.e. including sync polarities and
	 * everything).
	 */
	struct drm_display_mode mode;

	/**
	 * @mode_blob: &drm_property_blob for @mode, for exposing the mode to
	 * atomic userspace.
	 */
	//struct drm_property_blob *mode_blob;

	/**
	 * @degamma_lut:
	 *
	 * Lookup table for converting framebuffer pixel data before apply the
	 * color conversion matrix @ctm. See drm_crtc_enable_color_mgmt(). The
	 * blob (if not NULL) is an array of &struct drm_color_lut.
	 */
	//struct drm_property_blob *degamma_lut;

	/**
	 * @ctm:
	 *
	 * Color transformation matrix. See drm_crtc_enable_color_mgmt(). The
	 * blob (if not NULL) is a &struct drm_color_ctm.
	 */
	//struct drm_property_blob *ctm;

	/**
	 * @gamma_lut:
	 *
	 * Lookup table for converting pixel data after the color conversion
	 * matrix @ctm.  See drm_crtc_enable_color_mgmt(). The blob (if not
	 * NULL) is an array of &struct drm_color_lut.
	 */
	//struct drm_property_blob *gamma_lut;
#if defined(CONFIG_ROCKCHIP_DRM_CUBIC_LUT)
	/**
	 * @cubic_lut:
	 *
	 * Cubic Lookup table for converting pixel data. See
	 * drm_crtc_enable_color_mgmt(). The blob (if not NULL) is a 3D array
	 * of &struct drm_color_lut.
	 */
	struct drm_property_blob *cubic_lut;
#endif
	/**
	 * @target_vblank:
	 *
	 * Target vertical blank period when a page flip
	 * should take effect.
	 */
	u32 target_vblank;

	/**
	 * @async_flip:
	 *
	 * This is set when DRM_MODE_PAGE_FLIP_ASYNC is set in the legacy
	 * PAGE_FLIP IOCTL. It's not wired up for the atomic IOCTL itself yet.
	 */
	bool async_flip;

	/**
	 * @vrr_enabled:
	 *
	 * Indicates if variable refresh rate should be enabled for the CRTC.
	 * Support for the requested vrr state will depend on driver and
	 * hardware capabiltiy - lacking support is not treated as failure.
	 */
	bool vrr_enabled;

	/**
	 * @self_refresh_active:
	 *
	 * Used by the self refresh helpers to denote when a self refresh
	 * transition is occurring. This will be set on enable/disable callbacks
	 * when self refresh is being enabled or disabled. In some cases, it may
	 * not be desirable to fully shut off the crtc during self refresh.
	 * CRTC's can inspect this flag and determine the best course of action.
	 */
	bool self_refresh_active;

	/**
	 * @event:
	 *
	 * Optional pointer to a DRM event to signal upon completion of the
	 * state update. The driver must send out the event when the atomic
	 * commit operation completes. There are two cases:
	 *
	 *  - The event is for a CRTC which is being disabled through this
	 *    atomic commit. In that case the event can be send out any time
	 *    after the hardware has stopped scanning out the current
	 *    framebuffers. It should contain the timestamp and counter for the
	 *    last vblank before the display pipeline was shut off. The simplest
	 *    way to achieve that is calling drm_crtc_send_vblank_event()
	 *    somewhen after drm_crtc_vblank_off() has been called.
	 *
	 *  - For a CRTC which is enabled at the end of the commit (even when it
	 *    undergoes an full modeset) the vblank timestamp and counter must
	 *    be for the vblank right before the first frame that scans out the
	 *    new set of buffers. Again the event can only be sent out after the
	 *    hardware has stopped scanning out the old buffers.
	 *
	 *  - Events for disabled CRTCs are not allowed, and drivers can ignore
	 *    that case.
	 *
	 * For very simple hardware without VBLANK interrupt, enabling
	 * &struct drm_crtc_state.no_vblank makes DRM's atomic commit helpers
	 * send a fake VBLANK event at the end of the display update after all
	 * hardware changes have been applied. See
	 * drm_atomic_helper_fake_vblank().
	 *
	 * For more complex hardware this
	 * can be handled by the drm_crtc_send_vblank_event() function,
	 * which the driver should call on the provided event upon completion of
	 * the atomic commit. Note that if the driver supports vblank signalling
	 * and timestamping the vblank counters and timestamps must agree with
	 * the ones returned from page flip events. With the current vblank
	 * helper infrastructure this can be achieved by holding a vblank
	 * reference while the page flip is pending, acquired through
	 * drm_crtc_vblank_get() and released with drm_crtc_vblank_put().
	 * Drivers are free to implement their own vblank counter and timestamp
	 * tracking though, e.g. if they have accurate timestamp registers in
	 * hardware.
	 *
	 * For hardware which supports some means to synchronize vblank
	 * interrupt delivery with committing display state there's also
	 * drm_crtc_arm_vblank_event(). See the documentation of that function
	 * for a detailed discussion of the constraints it needs to be used
	 * safely.
	 *
	 * If the device can't notify of flip completion in a race-free way
	 * at all, then the event should be armed just after the page flip is
	 * committed. In the worst case the driver will send the event to
	 * userspace one frame too late. This doesn't allow for a real atomic
	 * update, but it should avoid tearing.
	 */
	//struct drm_pending_vblank_event *event;

	/**
	 * @commit:
	 *
	 * This tracks how the commit for this update proceeds through the
	 * various phases. This is never cleared, except when we destroy the
	 * state, so that subsequent commits can synchronize with previous ones.
	 */
	//struct drm_crtc_commit *commit;

	/** @state: backpointer to global drm_atomic_state */
	//struct drm_atomic_state *state;
};

/**
 * struct drm_framebuffer - frame buffer object
 *
 * Note that the fb is refcounted for the benefit of driver internals,
 * for example some hw, disabling a CRTC/plane is asynchronous, and
 * scanout does not actually complete until the next vblank.  So some
 * cleanup (like releasing the reference(s) on the backing GEM bo(s))
 * should be deferred.  In cases like this, the driver would like to
 * hold a ref to the fb even though it has already been removed from
 * userspace perspective. See drm_framebuffer_get() and
 * drm_framebuffer_put().
 *
 * The refcount is stored inside the mode object @base.
 */
struct drm_framebuffer {
	/**
	 * @dev: DRM device this framebuffer belongs to
	 */
	//struct drm_device *dev;
	/**
	 * @head: Place on the &drm_mode_config.fb_list, access protected by
	 * &drm_mode_config.fb_lock.
	 */
	//struct list_head head;

	/**
	 * @base: base modeset object structure, contains the reference count.
	 */
	//struct drm_mode_object base;

	/**
	 * @comm: Name of the process allocating the fb, used for fb dumping.
	 */
	//char comm[TASK_COMM_LEN];

	/**
	 * @format: framebuffer format information
	 */
	const struct drm_format_info *format;
	/**
	 * @funcs: framebuffer vfunc table
	 */
	//const struct drm_framebuffer_funcs *funcs;
	/**
	 * @pitches: Line stride per buffer. For userspace created object this
	 * is copied from drm_mode_fb_cmd2.
	 */
	unsigned int pitches[4];
	/**
	 * @offsets: Offset from buffer start to the actual pixel data in bytes,
	 * per buffer. For userspace created object this is copied from
	 * drm_mode_fb_cmd2.
	 *
	 * Note that this is a linear offset and does not take into account
	 * tiling or buffer laytou per @modifier. It meant to be used when the
	 * actual pixel data for this framebuffer plane starts at an offset,
	 * e.g.  when multiple planes are allocated within the same backing
	 * storage buffer object. For tiled layouts this generally means it
	 * @offsets must at least be tile-size aligned, but hardware often has
	 * stricter requirements.
	 *
	 * This should not be used to specifiy x/y pixel offsets into the buffer
	 * data (even for linear buffers). Specifying an x/y pixel offset is
	 * instead done through the source rectangle in &struct drm_plane_state.
	 */
	unsigned int offsets[4];
	/**
	 * @modifier: Data layout modifier. This is used to describe
	 * tiling, or also special layouts (like compression) of auxiliary
	 * buffers. For userspace created object this is copied from
	 * drm_mode_fb_cmd2.
	 */
	uint64_t modifier;
	/**
	 * @width: Logical width of the visible area of the framebuffer, in
	 * pixels.
	 */
	unsigned int width;
	/**
	 * @height: Logical height of the visible area of the framebuffer, in
	 * pixels.
	 */
	unsigned int height;
	/**
	 * @flags: Framebuffer flags like DRM_MODE_FB_INTERLACED or
	 * DRM_MODE_FB_MODIFIERS.
	 */
	int flags;
	/**
	 * @hot_x: X coordinate of the cursor hotspot. Used by the legacy cursor
	 * IOCTL when the driver supports cursor through a DRM_PLANE_TYPE_CURSOR
	 * universal plane.
	 */
	int hot_x;
	/**
	 * @hot_y: Y coordinate of the cursor hotspot. Used by the legacy cursor
	 * IOCTL when the driver supports cursor through a DRM_PLANE_TYPE_CURSOR
	 * universal plane.
	 */
	int hot_y;
	/**
	 * @filp_head: Placed on &drm_file.fbs, protected by &drm_file.fbs_lock.
	 */
	//struct list_head filp_head;
	/**
	 * @obj: GEM objects backing the framebuffer, one per plane (optional).
	 *
	 * This is used by the GEM framebuffer helpers, see e.g.
	 * drm_gem_fb_create().
	 */
	//.struct drm_gem_object *obj[4];
};

/**
 * enum drm_plane_type - uapi plane type enumeration
 *
 * For historical reasons not all planes are made the same. This enumeration is
 * used to tell the different types of planes apart to implement the different
 * uapi semantics for them. For userspace which is universal plane aware and
 * which is using that atomic IOCTL there's no difference between these planes
 * (beyong what the driver and hardware can support of course).
 *
 * For compatibility with legacy userspace, only overlay planes are made
 * available to userspace by default. Userspace clients may set the
 * DRM_CLIENT_CAP_UNIVERSAL_PLANES client capability bit to indicate that they
 * wish to receive a universal plane list containing all plane types. See also
 * drm_for_each_legacy_plane().
 *
 * WARNING: The values of this enum is UABI since they're exposed in the "type"
 * property.
 */
enum drm_plane_type {
	/**
	 * @DRM_PLANE_TYPE_OVERLAY:
	 *
	 * Overlay planes represent all non-primary, non-cursor planes. Some
	 * drivers refer to these types of planes as "sprites" internally.
	 */
	DRM_PLANE_TYPE_OVERLAY,

	/**
	 * @DRM_PLANE_TYPE_PRIMARY:
	 *
	 * Primary planes represent a "main" plane for a CRTC.  Primary planes
	 * are the planes operated upon by CRTC modesetting and flipping
	 * operations described in the &drm_crtc_funcs.page_flip and
	 * &drm_crtc_funcs.set_config hooks.
	 */
	DRM_PLANE_TYPE_PRIMARY,

	/**
	 * @DRM_PLANE_TYPE_CURSOR:
	 *
	 * Cursor planes represent a "cursor" plane for a CRTC.  Cursor planes
	 * are the planes operated upon by the DRM_IOCTL_MODE_CURSOR and
	 * DRM_IOCTL_MODE_CURSOR2 IOCTLs.
	 */
	DRM_PLANE_TYPE_CURSOR,
};

/**
 * struct drm_plane - central DRM plane control structure
 *
 * Planes represent the scanout hardware of a display block. They receive their
 * input data from a &drm_framebuffer and feed it to a &drm_crtc. Planes control
 * the color conversion, see `Plane Composition Properties`_ for more details,
 * and are also involved in the color conversion of input pixels, see `Color
 * Management Properties`_ for details on that.
 */
struct drm_plane {
	/** @dev: DRM device this plane belongs to */
	//struct drm_device *dev;

	/**
	 * @head:
	 *
	 * List of all planes on @dev, linked from &drm_mode_config.plane_list.
	 * Invariant over the lifetime of @dev and therefore does not need
	 * locking.
	 */
	//struct list_head head;

	/** @name: human readable name, can be overwritten by the driver */
	char *name;

	/**
	 * @mutex:
	 *
	 * Protects modeset plane state, together with the &drm_crtc.mutex of
	 * CRTC this plane is linked to (when active, getting activated or
	 * getting disabled).
	 *
	 * For atomic drivers specifically this protects @state.
	 */
	//struct drm_modeset_lock mutex;

	/** @base: base mode object */
	//struct drm_mode_object base;

	/**
	 * @possible_crtcs: pipes this plane can be bound to constructed from
	 * drm_crtc_mask()
	 */
	uint32_t possible_crtcs;
	/** @format_types: array of formats supported by this plane */
	uint32_t *format_types;
	/** @format_count: Size of the array pointed at by @format_types. */
	unsigned int format_count;
	/**
	 * @format_default: driver hasn't supplied supported formats for the
	 * plane. Used by the drm_plane_init compatibility wrapper only.
	 */
	bool format_default;

	/** @modifiers: array of modifiers supported by this plane */
	uint64_t *modifiers;
	/** @modifier_count: Size of the array pointed at by @modifier_count. */
	unsigned int modifier_count;

	/**
	 * @crtc:
	 *
	 * Currently bound CRTC, only meaningful for non-atomic drivers. For
	 * atomic drivers this is forced to be NULL, atomic drivers should
	 * instead check &drm_plane_state.crtc.
	 */
	struct drm_crtc *crtc;

	/**
	 * @fb:
	 *
	 * Currently bound framebuffer, only meaningful for non-atomic drivers.
	 * For atomic drivers this is forced to be NULL, atomic drivers should
	 * instead check &drm_plane_state.fb.
	 */
	struct drm_framebuffer *fb;

	/**
	 * @old_fb:
	 *
	 * Temporary tracking of the old fb while a modeset is ongoing. Only
	 * used by non-atomic drivers, forced to be NULL for atomic drivers.
	 */
	struct drm_framebuffer *old_fb;

	/** @funcs: plane control functions */
	//const struct drm_plane_funcs *funcs;

	/** @properties: property tracking for this plane */
	//struct drm_object_properties properties;

	/** @type: Type of plane, see &enum drm_plane_type for details. */
	enum drm_plane_type type;

	/**
	 * @index: Position inside the mode_config.list, can be used as an array
	 * index. It is invariant over the lifetime of the plane.
	 */
	unsigned index;

	/** @helper_private: mid-layer private data */
	//const struct drm_plane_helper_funcs *helper_private;

	/**
	 * @state:
	 *
	 * Current atomic state for this plane.
	 *
	 * This is protected by @mutex. Note that nonblocking atomic commits
	 * access the current plane state without taking locks. Either by going
	 * through the &struct drm_atomic_state pointers, see
	 * for_each_oldnew_plane_in_state(), for_each_old_plane_in_state() and
	 * for_each_new_plane_in_state(). Or through careful ordering of atomic
	 * commit operations as implemented in the atomic helpers, see
	 * &struct drm_crtc_commit.
	 */
	struct drm_plane_state *state;

	/**
	 * @alpha_property:
	 * Optional alpha property for this plane. See
	 * drm_plane_create_alpha_property().
	 */
	//struct drm_property *alpha_property;
	/**
	 * @zpos_property:
	 * Optional zpos property for this plane. See
	 * drm_plane_create_zpos_property().
	 */
	//struct drm_property *zpos_property;
	/**
	 * @rotation_property:
	 * Optional rotation property for this plane. See
	 * drm_plane_create_rotation_property().
	 */
	//struct drm_property *rotation_property;
	/**
	 * @blend_mode_property:
	 * Optional "pixel blend mode" enum property for this plane.
	 * Blend mode property represents the alpha blending equation selection,
	 * describing how the pixels from the current plane are composited with
	 * the background.
	 */
	//struct drm_property *blend_mode_property;

	/**
	 * @color_encoding_property:
	 *
	 * Optional "COLOR_ENCODING" enum property for specifying
	 * color encoding for non RGB formats.
	 * See drm_plane_create_color_properties().
	 */
	//struct drm_property *color_encoding_property;
	/**
	 * @color_range_property:
	 *
	 * Optional "COLOR_RANGE" enum property for specifying
	 * color range for non RGB formats.
	 * See drm_plane_create_color_properties().
	 */
	//struct drm_property *color_range_property;
};

/**
 * struct drm_plane_state - mutable plane state
 *
 * Please not that the destination coordinates @crtc_x, @crtc_y, @crtc_h and
 * @crtc_w and the source coordinates @src_x, @src_y, @src_h and @src_w are the
 * raw coordinates provided by userspace. Drivers should use
 * drm_atomic_helper_check_plane_state() and only use the derived rectangles in
 * @src and @dst to program the hardware.
 */
struct drm_plane_state {
	/** @plane: backpointer to the plane */
	struct drm_plane *plane;

	/**
	 * @crtc:
	 *
	 * Currently bound CRTC, NULL if disabled. Do not this write directly,
	 * use drm_atomic_set_crtc_for_plane()
	 */
	struct drm_crtc *crtc;

	/**
	 * @fb:
	 *
	 * Currently bound framebuffer. Do not write this directly, use
	 * drm_atomic_set_fb_for_plane()
	 */
	struct drm_framebuffer *fb;

	/**
	 * @fence:
	 *
	 * Optional fence to wait for before scanning out @fb. The core atomic
	 * code will set this when userspace is using explicit fencing. Do not
	 * write this field directly for a driver's implicit fence, use
	 * drm_atomic_set_fence_for_plane() to ensure that an explicit fence is
	 * preserved.
	 *
	 * Drivers should store any implicit fence in this from their
	 * &drm_plane_helper_funcs.prepare_fb callback. See drm_gem_fb_prepare_fb()
	 * and drm_gem_fb_simple_display_pipe_prepare_fb() for suitable helpers.
	 */
	//struct dma_fence *fence;

	/**
	 * @crtc_x:
	 *
	 * Left position of visible portion of plane on crtc, signed dest
	 * location allows it to be partially off screen.
	 */

	int32_t crtc_x;
	/**
	 * @crtc_y:
	 *
	 * Upper position of visible portion of plane on crtc, signed dest
	 * location allows it to be partially off screen.
	 */
	int32_t crtc_y;

	/** @crtc_w: width of visible portion of plane on crtc */
	/** @crtc_h: height of visible portion of plane on crtc */
	uint32_t crtc_w, crtc_h;

	/**
	 * @src_x: left position of visible portion of plane within plane (in
	 * 16.16 fixed point).
	 */
	uint32_t src_x;
	/**
	 * @src_y: upper position of visible portion of plane within plane (in
	 * 16.16 fixed point).
	 */
	uint32_t src_y;
	/** @src_w: width of visible portion of plane (in 16.16) */
	/** @src_h: height of visible portion of plane (in 16.16) */
	uint32_t src_h, src_w;

	/**
	 * @alpha:
	 * Opacity of the plane with 0 as completely transparent and 0xffff as
	 * completely opaque. See drm_plane_create_alpha_property() for more
	 * details.
	 */
	u16 alpha;

	/**
	 * @pixel_blend_mode:
	 * The alpha blending equation selection, describing how the pixels from
	 * the current plane are composited with the background. Value can be
	 * one of DRM_MODE_BLEND_*
	 */
	uint16_t pixel_blend_mode;

	/**
	 * @rotation:
	 * Rotation of the plane. See drm_plane_create_rotation_property() for
	 * more details.
	 */
	unsigned int rotation;

	/**
	 * @zpos:
	 * Priority of the given plane on crtc (optional).
	 *
	 * User-space may set mutable zpos properties so that multiple active
	 * planes on the same CRTC have identical zpos values. This is a
	 * user-space bug, but drivers can solve the conflict by comparing the
	 * plane object IDs; the plane with a higher ID is stacked on top of a
	 * plane with a lower ID.
	 *
	 * See drm_plane_create_zpos_property() and
	 * drm_plane_create_zpos_immutable_property() for more details.
	 */
	unsigned int zpos;

	/**
	 * @normalized_zpos:
	 * Normalized value of zpos: unique, range from 0 to N-1 where N is the
	 * number of active planes for given crtc. Note that the driver must set
	 * &drm_mode_config.normalize_zpos or call drm_atomic_normalize_zpos() to
	 * update this before it can be trusted.
	 */
	unsigned int normalized_zpos;

	/**
	 * @color_encoding:
	 *
	 * Color encoding for non RGB formats
	 */
	//enum drm_color_encoding color_encoding;

	/**
	 * @color_range:
	 *
	 * Color range for non RGB formats
	 */
	//enum drm_color_range color_range;

	/**
	 * @fb_damage_clips:
	 *
	 * Blob representing damage (area in plane framebuffer that changed
	 * since last plane update) as an array of &drm_mode_rect in framebuffer
	 * coodinates of the attached framebuffer. Note that unlike plane src,
	 * damage clips are not in 16.16 fixed point.
	 */
	//struct drm_property_blob *fb_damage_clips;

	/**
	 * @src:
	 *
	 * source coordinates of the plane (in 16.16).
	 *
	 * When using drm_atomic_helper_check_plane_state(),
	 * the coordinates are clipped, but the driver may choose
	 * to use unclipped coordinates instead when the hardware
	 * performs the clipping automatically.
	 */
	/**
	 * @dst:
	 *
	 * clipped destination coordinates of the plane.
	 *
	 * When using drm_atomic_helper_check_plane_state(),
	 * the coordinates are clipped, but the driver may choose
	 * to use unclipped coordinates instead when the hardware
	 * performs the clipping automatically.
	 */
	struct drm_rect src, dst;

	/**
	 * @visible:
	 *
	 * Visibility of the plane. This can be false even if fb!=NULL and
	 * crtc!=NULL, due to clipping.
	 */
	bool visible;

	/**
	 * @commit: Tracks the pending commit to prevent use-after-free conditions,
	 * and for async plane updates.
	 *
	 * May be NULL.
	 */
	//struct drm_crtc_commit *commit;

	/** @state: backpointer to global drm_atomic_state */
	//struct drm_atomic_state *state;
};

static inline void *dss_malloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);

	//printf("dss malloc %zu Byte from %p", size, ptr);

	return ptr;
}

static inline void *dss_calloc(size_t nmemb, size_t size)
{
	void *ptr;

	ptr = calloc(nmemb, size);
	//printf("dss malloc %zu Byte from %p\n", size, ptr);

	return ptr;
}

static inline void dss_free(void *ptr)
{
	//printf("dss malloc free from %p\n", ptr);

	free(ptr);
}

#endif
