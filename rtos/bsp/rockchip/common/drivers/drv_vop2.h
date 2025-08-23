/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_vop2.c
  * @version V0.1
  * @brief   display vop2 driver
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-08-15     Damon Ding      first implementation
  *
  ******************************************************************************
  */

#ifndef __DRV_VOP2_H__
#define __DRV_VOP2_H__

/*******************************************************************************
 * Included Files
 ******************************************************************************/
#include "hal_base.h"

#include <drm/connector.h>
#include <drm/drm_util.h>
#include <common/wfddev.h>
#include <common/dss_service.h>

/*******************************************************************************
 * Pre-processor Definitions
 ******************************************************************************/
#define AFBC_FORMAT_MOD_YTR     (1ULL <<  4)

#define ROCKCHIP_MAX_LAYER      16

#define V4L2_COLORSPACE_BT709F  0xfe
#define V4L2_COLORSPACE_BT2020F 0xff

/*******************************************************************************
 * Public Types
 ******************************************************************************/
struct vop2_layer
{
    uint8_t id;
    /*
     * @win_phys_id: window id of the layer selected.
     * Every layer must make sure to select different
     * windows of others.
     */
    uint8_t win_phys_id;
};

struct vop2_cluster
{
    bool splice_mode;
    struct vop2_win *main;
    struct vop2_win *sub;
};

struct vop2_win
{
    const char *name;
    struct vop2 *vop2;
    struct vop2_win *parent;
    struct drm_plane base;

    /*
     * This is for cluster window
     *
     * A cluster window can split as two windows:
     * a main window and a sub window.
     */
    bool two_win_mode;

    /**
     * ---------------------------
     * |          |              |
     * | Left     |  Right       |
     * |          |              |
     * | Cluster0 |  Cluster1    |
     * ---------------------------
     */

    /*
     * @splice_mode_right: As right part of the screen in splice mode.
     */
    bool splice_mode_right;

    /**
     * @splice_win: splice win which used to splice for a plane
     * hdisplay > 4096
     */
    struct vop2_win *splice_win;
    struct vop2_win *left_win;

    uint8_t splice_win_id;

    struct vop2_power_domain *pd;

    /**
     * @phys_id: physical id for cluster0/1, esmart0/1, smart0/1
     * Will be used as a identification for some register
     * configuration such as OVL_LAYER_SEL/OVL_PORT_SEL.
     */
    uint8_t phys_id;

    /**
     * @win_id: graphic window id, a cluster maybe split into two
     * graphics windows.
     */
    uint32_t win_id;
    /**
     * @area_id: multi display region id in a graphic window, they
     * share the same win_id.
     */
    uint32_t area_id;
    /**
     * @plane_id: unique plane id.
     */
    uint8_t plane_id;
    /**
     * @layer_id: id of the layer which the window attached to
     */
    uint8_t layer_id;
    const uint8_t *layer_sel_id;
    /**
     * @vp_mask: Bitmask of video_port0/1/2 this win attached to,
     * one win can only attach to one vp at the one time.
     */
    unsigned long vp_mask;
    /**
     * @old_vp_mask: Bitmask of video_port0/1/2 this win attached of last commit,
     * this is used for trackng the change of VOP2_PORT_SEL register.
     */
    unsigned long old_vp_mask;
    uint32_t zpos;
    uint32_t offset;
    uint8_t axi_id;
    uint8_t axi_yrgb_id;
    uint8_t axi_uv_id;
    uint8_t scale_engine_num;
    uint8_t possible_crtcs;
    enum drm_plane_type type;
    unsigned int max_upscale_factor;
    unsigned int max_downscale_factor;
    unsigned int supported_rotations;
    const uint8_t *dly;
    /*
     * vertical/horizontal scale up/down filter mode
     */
    uint8_t hsu_filter_mode;
    uint8_t hsd_filter_mode;
    uint8_t vsu_filter_mode;
    uint8_t vsd_filter_mode;
    uint8_t hsd_pre_filter_mode;
    uint8_t vsd_pre_filter_mode;

    const uint64_t *format_modifiers;
    const eDISPLAY_dataFormat *formats;
    uint32_t nformats;
    uint64_t feature;

    /*
     * add for wfd
     */
    struct wfd_plane_state wfd_pstate;
    unsigned int wfd_pipe_index;
};

struct vop2_power_domain
{
    struct vop2_power_domain *parent;
    struct vop2 *vop2;
    /*
     * @lock: protect power up/down procedure.
     * power on take effect immediately,
     * power down take effect by vsync.
     * we must check power_domain_status register
     * to make sure the power domain is down before
     * send a power on request.
     *
     */
    unsigned int ref_count;
    bool on;
    /* @vp_mask: Bit mask of video port of the power domain's
     * module attached to.
     * For example: PD_CLUSTER0 belongs to module Cluster0, it's
     * bitmask is the VP which Cluster0 attached to. PD_ESMART is
     * shared between Esmart1/2/3, it's bitmask will be all the VP
     * which Esmart1/2/3 attached to.
     * This is used to check if we can power off a PD by vsync.
     */
    unsigned long vp_mask;

    const struct VOP2_POWER_DOMAIN_DATA *data;
    rt_list_t list;
};

struct vop2_dsc
{
    uint8_t id;
    uint8_t max_slice_num;
    uint8_t max_linebuf_depth;  /* used to generate the bitstream */
    uint8_t min_bits_per_pixel; /* bit num after encoder compress */
    bool enabled;
    int attach_vp_id;
    struct vop2_power_domain *pd;
};

struct dsc_error_info
{
    u32 dsc_error_val;
    char dsc_error_info[50];
};

struct vop2_zpos
{
    struct drm_plane *plane;
    uint32_t win_phys_id;
    int zpos;
};

union vop2_alpha_ctrl
{
    uint32_t val;
    struct
    {
        /* [0:1] */
        uint32_t color_mode: 1;
        uint32_t alpha_mode: 1;
        /* [2:3] */
        uint32_t blend_mode: 2;
        uint32_t alpha_cal_mode: 1;
        /* [5:7] */
        uint32_t factor_mode: 3;
        /* [8:9] */
        uint32_t alpha_en: 1;
        uint32_t src_dst_swap: 1;
        uint32_t reserved: 6;
        /* [16:23] */
        uint32_t glb_alpha: 8;
    } bits;
};

union vop2_bg_alpha_ctrl
{
    uint32_t val;
    struct
    {
        /* [0:1] */
        uint32_t alpha_en: 1;
        uint32_t alpha_mode: 1;
        /* [2:3] */
        uint32_t alpha_pre_mul: 1;
        uint32_t alpha_sat_mode: 1;
        /* [4:7] */
        uint32_t reserved: 4;
        /* [8:15] */
        uint32_t glb_alpha: 8;
    } bits;
};

struct vop2_alpha
{
    union vop2_alpha_ctrl src_color_ctrl;
    union vop2_alpha_ctrl dst_color_ctrl;
    union vop2_alpha_ctrl src_alpha_ctrl;
    union vop2_alpha_ctrl dst_alpha_ctrl;
};

struct vop2_alpha_config
{
    bool src_premulti_en;
    bool dst_premulti_en;
    bool src_pixel_alpha_en;
    bool dst_pixel_alpha_en;
    u8 src_glb_alpha_value;
    u8 dst_glb_alpha_value;
};

struct vop2_plane_state
{
    struct drm_plane_state base;
    struct PLANE_STATE plane_state;
    int format;
    int zpos;
    struct drm_rect src;
    struct drm_rect dst;
    uint32_t yrgb_mst;
    uint32_t uv_mst;
    uint32_t fb_size;
    void *fb_vaddr;
    bool afbc_en;
    bool hdr_in;
    bool hdr2sdr_en;
    bool r2y_en;
    bool y2r_en;
    uint32_t csc_mode;
    uint8_t xmirror_en;
    uint8_t ymirror_en;
    uint8_t rotate_90_en;
    uint8_t rotate_270_en;
    uint8_t afbc_half_block_en;
    uint8_t tiled_en;
    uint32_t eotf;
    uint32_t color_space;
    uint8_t global_alpha;
    int blend_mode;
    uint64_t color_key;
    unsigned long offset;
    int pdaf_data_type;
    bool async_commit;
    struct vop_dump_list *planlist;
};

struct rockchip_dsc_sink_cap
{
    /**
     * @slice_width: the number of pixel columns that comprise the slice width
     * @slice_height: the number of pixel rows that comprise the slice height
     * @block_pred: Does block prediction
     * @native_420: Does sink support DSC with 4:2:0 compression
     * @bpc_supported: compressed bpc supported by sink : 10, 12 or 16 bpc
     * @version_major: DSC major version
     * @version_minor: DSC minor version
     * @target_bits_per_pixel_x16: bits num after compress and multiply 16
     */
    uint16_t slice_width;
    uint16_t slice_height;
    bool block_pred;
    bool native_420;
    uint8_t bpc_supported;
    uint8_t version_major;
    uint8_t version_minor;
    uint16_t target_bits_per_pixel_x16;
};

struct rockchip_crtc_state
{
    struct drm_crtc_state base;
    uint32_t vp_id;
    int output_type;
    int output_mode;
    int output_bpc;
    unsigned int output_flags;
    bool enable_afbc;
    /**
     * @used by wfd
     *
     */
    unsigned long irq_status_val;
    unsigned long irq_status;
    uint64_t vblank_count;

    /**
     * @splice_mode: enabled when display a hdisplay > 4096 on rk3588
     */
    bool splice_mode;

    /**
     * @hold_mode: enabled when it's:
     * (1) mcu hold mode
     * (2) mipi dsi cmd mode
     * (3) edp psr mode
     */
    bool hold_mode;
    /**
     * when enable soft_te, use gpio irq to triggle new fs,
     * otherwise use hardware te
     */
    bool soft_te;

    struct drm_tv_connector_state *tv_state;
    int left_margin;
    int right_margin;
    int top_margin;
    int bottom_margin;
    int vdisplay;
    int afbdc_win_format;
    int afbdc_win_width;
    int afbdc_win_height;
    int afbdc_win_ptr;
    int afbdc_win_id;
    int afbdc_en;
    int afbdc_win_vir_width;
    int afbdc_win_xoffset;
    int afbdc_win_yoffset;
    int dsp_layer_sel;
    unsigned long output_if;
    u32 bus_format;
    u32 bus_flags;
    u32 yuv_overlay;
    u32 post_r2y_en;
    u32 post_y2r_en;
    u32 post_csc_mode;
    u32 bcsh_en;
    u32 color_space;
    u32 eotf;
    u32 background;
    u32 line_flag;
    u8 mode_update;
    u8 dsc_id;
    u8 dsc_enable;

    u8 dsc_slice_num;
    u8 dsc_pixel_num;

    u32 dsc_txp_clk_rate;
    u32 dsc_pxl_clk_rate;
    u32 dsc_cds_clk_rate;

    struct CRTC_STATE crtc_state;

    struct rockchip_dsc_sink_cap dsc_sink_cap;

    int request_refresh_rate;
    int max_refresh_rate;
    int min_refresh_rate;

    struct wfd_commit_data commit_data;
};

struct rockchip_crtc
{
    struct drm_crtc crtc;
};

struct vop2_connector_state
{
    void *private;
    uint32_t bus_format;
    uint32_t output_mode;
    uint32_t type;
    uint64_t output_if;
    uint32_t output_flags;
    uint32_t color_space;
    uint32_t bpc;

    /**
     * @hold_mode: enabled when it's:
     * (1) mcu hold mode
     * (2) mipi dsi cmd mode
     * (3) edp psr mode
     */
    bool hold_mode;
    uint8_t dsc_id;
    uint8_t dsc_slice_num;
    uint8_t dsc_pixel_num;
    uint64_t dsc_txp_clk;
    uint64_t dsc_pxl_clk;
    uint64_t dsc_cds_clk;

    struct
    {
        uint32_t *lut;
        uint32_t size;
    } gamma;
};

struct vop2_video_port
{
    struct wfd_port wfd_port;
    struct vop2_connector_state conn_state;
    uint32_t id;
    struct connector *connectors;
    uint32_t num_connectors;

    struct rockchip_crtc rockchip_crtc;
    // struct rockchip_mcu_timing mcu_timing;
    struct vop2 *vop2;
    bool layer_sel_update;
    bool xmirror_en;
    bool need_reset_p2i_flag;
    // const struct vop2_video_port_regs *regs;

    /**
     * @hdr_in: Indicate we have a hdr plane input.
     *
     */
    bool hdr_in;
    /**
     * @hdr_out: Indicate the screen want a hdr output
     * from video port.
     *
     */
    bool hdr_out;
    /*
     * @sdr2hdr_en: All the ui plane need to do sdr2hdr for a hdr_out enabled vp.
     *
     */
    bool sdr2hdr_en;
    /**
     * @skip_vsync: skip on vsync when port_mux changed on this vp.
     * a win move from one VP to another need wait one vsync until
     * port_mut take effect before this win can be enabled.
     *
     */
    bool skip_vsync;

    /**
     * @bg_ovl_dly: The timing delay from background layer
     * to overlay module.
     */
    uint32_t bg_ovl_dly;

    /**
     * @hdr_en: Set when has a hdr video input.
     */
    int hdr_en;

    /**
     * -----------------
     * |      |       |
     * | Left | Right |
     * |      |       |
     * | VP0  |  VP1  |
     * -----------------
     * @splice_mode_right: As right part of the screen in splice mode.
     */
    bool splice_mode_right;

    /**
     * @hdr10_at_splice_mode: enable hdr10 at splice mode on rk3588.
     */
    bool hdr10_at_splice_mode;
    /**
     * @left_vp: VP as left part of the screen in splice mode.
     */
    struct vop2_video_port *left_vp;

    /**
     * @win_mask: Bitmask of wins attached to the video port;
     */
    unsigned long win_mask;
    /**
     * @nr_layers: active layers attached to the video port;
     */
    uint8_t nr_layers;

    int cursor_win_id;
    /**
     * @output_if: output connector attached to the video port,
     * this flag is maintained in vop driver, updated in crtc_atomic_enable,
     * cleared in crtc_atomic_disable;
     */
    unsigned long output_if;


    /**
     * @lut: store legacy gamma look up table
     */
    u32 *lut;

    /**
     * @gamma_lut_len: gamma look up table size
     */
    u32 gamma_lut_len;

    /**
     * @gamma_lut_active: gamma states
     */
    bool gamma_lut_active;

    /**
     * @lut_dma_rid: lut dma id
     */
    u16 lut_dma_rid;

    /**
     * @gamma_lut: atomic gamma look up table
     */

    /**
     * @cubic_lut_len: cubic look up table size
     */
    u32 cubic_lut_len;

    /**
     * @cubic_lut_gem_obj: gem obj to store cubic lut
     */

    /**
     * @hdr_lut_gem_obj: gem obj to store hdr lut
     */

    /**
     * @cubic_lut: cubic look up table
     */

    /**
     * @loader_protect: loader logo protect state
     */
    bool loader_protect;

    /**
     * @plane_mask: show the plane attach to this vp,
     * it maybe init at dts file or uboot driver
     */
    unsigned long int plane_mask;

    /**
     * @plane_mask_prop: plane mask interaction with userspace
     */
    /**
     * @feature_prop: crtc feature interaction with userspace
     */

    /**
     * @variable_refresh_rate_prop: crtc variable refresh rate interaction with userspace
     */

    /**
     * @max_refresh_rate_prop: crtc max refresh rate interaction with userspace
     */

    /**
     * @min_refresh_rate_prop: crtc min refresh rate interaction with userspace
     */

    /**
     * @hdr_ext_data_prop: hdr extend data interaction with userspace
     */

    int hdrvivid_mode;

    /**
     * @acm_lut_data_prop: acm lut data interaction with userspace
     */
    /**
     * @post_csc_data_prop: post csc data interaction with userspace
     */
    /**
     * @output_width_prop: vp max output width prop
     */
    /**
     * @output_dclk_prop: vp max output dclk prop
     */

    /**
     * @primary_plane_phy_id: vp primary plane phy id, the primary plane
     * will be used to show uboot logo and kernel logo
     */
    eVOP2_layerPhyId primary_plane_phy_id;

    /**
     * @refresh_rate_change: indicate whether refresh rate change
     */
    bool refresh_rate_change;
};

struct vop2
{
    struct wfd_device wfd_dev;
    pthread_t tid;
    int path_id;
    uintptr_t regs;
    uint32_t *regsbak;
    uintptr_t grf;
    uintptr_t vop_grf;
    uintptr_t vo1_grf;
    uintptr_t sys_pmu;
    int irq;
    int irqid;
    uint32_t debug_mask;

    const struct VOP2_DATA *data;

    uint32_t version;
    struct vop2_dsc dscs[ROCKCHIP_MAX_CRTC];
    struct vop2_video_port vps[ROCKCHIP_MAX_CRTC];
    bool is_iommu_enabled;
    bool is_iommu_needed;
    bool is_enabled;
    bool support_multi_area;
    bool disable_afbc_win;

    /* no move win from one vp to another */
    bool disable_win_move;
    /*
     * Usually we increase old fb refcount at
     * atomic_flush and decrease it when next
     * vsync come, this can make user the fb
     * not been releasced before vop finish use
     * it.
     *
     * But vop decrease fb refcount by a thread
     * vop2_unref_fb_work, which may run a little
     * slow sometimes, so when userspace do a rmfb,
     *
     * see drm_mode_rmfb,
     * it will find the fb refcount is still > 1,
     * than goto a fallback to init drm_mode_rmfb_work_fn,
     * this will cost a long time(>10 ms maybe) and block
     * rmfb work. Some userspace don't have with this(such as vo).
     *
     * Don't reference framebuffer refcount by
     * drm_framebuffer_get as some userspace want
     * rmfb as soon as possible(nvr vo). And the userspace
     * should make sure release fb after it receive the vsync.
     */
    bool skip_ref_fb;

    bool loader_protect;

    bool aclk_rate_reset;
    unsigned long aclk_rate;

    /* Number of win that registered as plane,
     * maybe less than the total number of hardware
     * win.
     */
    uint32_t registered_num_wins;
    uint8_t used_mixers;
    uint8_t esmart_lb_mode;
    /**
     * @active_vp_mask: Bitmask of active video ports;
     */
    uint8_t active_vp_mask;
    uint16_t port_mux_cfg;

    /*
     * Some globle resource are shared between all
     * the vidoe ports(crtcs), so we need a ref counter here.
     */
    unsigned int enable_count;

    rt_list_t pd_list_head;
    struct vop2_layer layers[ROCKCHIP_MAX_LAYER];

    rt_event_t frm_st_event;

    /* must put at the end of the struct */
    struct vop2_win win[8];
};

/*******************************************************************************
 * Public Data
 ******************************************************************************/

/*******************************************************************************
 * Inline Functions
 ******************************************************************************/

/*******************************************************************************
 * Public Functions
 ******************************************************************************/
void vop2_connector_disable(struct wfd_port *port);
void vop2_connector_post_disable(struct wfd_port *port);
struct rockchip_dev_pm_data *vop2_get_pm_data(struct wfd_device *dev);
void wfd_vop2_crtc_disable(struct wfd_port *port);
struct wfd_device *pm_data_to_wfd_dev(struct rockchip_dev_pm_data *pm_data);
void vop2_wfd_port_resume(struct wfd_port *port);
const struct vop2_data *vop2_device_get_match_data(const char *compatible);
struct vop2 *get_vop2_global();

#endif /* __DRV_VOP2_H__ */
