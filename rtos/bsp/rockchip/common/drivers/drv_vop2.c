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

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup VOP2
 *  @{
 */

/** @defgroup VOP2_How_To_Use How To Use
 *  @{

 @} */

#include <rthw.h>
#include <rtdevice.h>
#include <rtthread.h>
#ifdef RT_USING_VOP2
#include <wfd.h>
#include <wfdext.h>

#include "drv_vop2.h"

/********************* Private MACRO Definition ******************************/
#define to_rockchip_crtc_state(s) rt_container_of(s, struct rockchip_crtc_state, base)

/********************* Private Variable Definition ******************************/
static uint32_t dss_debug_level_mask;

static struct vop2 *gptr_vop2;

/********************* Private Function Definition ***************************/
/** @defgroup VOP2_Private_Function Private Function
 *  @{
 */

static inline struct vop2_plane_state *to_vop2_plane_state(struct drm_plane_state *pstate)
{
    char *p = (char *)pstate - offsetof(struct vop2_plane_state, base);
    return (struct vop2_plane_state *)p;
}

static inline struct vop2_win *to_vop2_win(struct drm_plane *plane)
{
    char *p = (char *)plane - offsetof(struct vop2_win, base);
    return (struct vop2_win *)p;
}

static inline struct vop2_video_port *to_vop2_video_port(struct drm_crtc *crtc)
{
    struct rockchip_crtc *rockchip_crtc;
    char *p = (char *)crtc - offsetof(struct rockchip_crtc, crtc);

    rockchip_crtc = (struct rockchip_crtc *)p;
    p = (char *)rockchip_crtc - offsetof(struct vop2_video_port, rockchip_crtc);

    return (struct vop2_video_port *)p;
}

static inline struct vop2_video_port *wfd_port_to_vp(struct wfd_port *port)
{
    char *p = (char *)port - offsetof(struct vop2_video_port, wfd_port);
    return (struct vop2_video_port *)p;
}

struct vop2 *wfd_dev_to_vop2(struct wfd_device *dev)
{
    char *p = (char *)dev - offsetof(struct vop2, wfd_dev);
    return (struct vop2 *)p;
}

static inline bool vop2_plane_active(struct drm_plane_state *pstate)
{
    if (!pstate || !pstate->fb)
        return false;
    else
        return true;
}

static inline bool is_vop3(struct vop2 *vop2)
{
    if (vop2->version == VOP_VERSION_RK3568 || vop2->version == VOP_VERSION_RK3588)
        return false;
    else
        return true;
}

static bool is_alpha_support(uint32_t format)
{
    switch (format)
    {
    case DRM_FORMAT_ARGB8888:
    case DRM_FORMAT_ABGR8888:
        return true;
    default:
        return false;
    }
}

static inline bool vop2_multi_area_sub_window(struct vop2_win *win)
{
    return (win->parent && (win->feature & WIN_FEATURE_MULTI_AREA));
}

static inline bool vop2_cluster_window(struct vop2_win *win)
{
    return (win->feature & (WIN_FEATURE_CLUSTER_MAIN | WIN_FEATURE_CLUSTER_SUB));
}

static inline bool vop2_cluster_sub_window(struct vop2_win *win)
{
    return (win->feature & WIN_FEATURE_CLUSTER_SUB);
}

static inline bool vop2_has_feature(struct vop2 *vop2, uint64_t feature)
{
    return (vop2->data->feature & feature);
}

static struct drm_crtc_state *vop2_crtc_duplicate_state(struct drm_crtc *crtc)
{
    struct vop2_video_port *vp = to_vop2_video_port(crtc);
    struct rockchip_crtc_state *vcstate;
    struct CRTC_STATE *crtc_state;

    vcstate = dss_calloc(1, sizeof(*vcstate));
    if (!vcstate)
        return NULL;
    crtc_state = &vcstate->crtc_state;

    vcstate->vp_id = vp->id;
    vcstate->left_margin = 100;
    vcstate->right_margin = 100;
    vcstate->top_margin = 100;
    vcstate->bottom_margin = 100;
    vcstate->background = 0;

    crtc_state->vpId = vp->id;
    crtc_state->leftMargin = 100;
    crtc_state->rightMargin = 100;
    crtc_state->topMargin = 100;
    crtc_state->bottomMargin = 100;
    crtc_state->background = 0;

    return &vcstate->base;
}

static void rk3568_dsi_init(struct connector *connector)
{
    // todo
    return;
}

static void rk3568_vop2_video_port_init(struct vop2_video_port *vp)
{
    uint32_t i;

    for (i = 0; i < vp->num_connectors; i++)
    {
        struct connector *connector = &vp->connectors[i];
        struct vop2_connector_state *conn_state = &vp->conn_state;

        if (connector->connector_type == DRM_MODE_CONNECTOR_DSI)
            rk3568_dsi_init(connector);

        conn_state->output_mode = connector->output_mode;
        conn_state->type = connector->connector_type;
        if (connector->bus_format)
            conn_state->bus_format = connector->bus_format;
    }
}

static void vop2_video_port_init(struct vop2_video_port *vp)
{
    struct wfd_device *wfd_dev = vp->wfd_port.dev;
    struct vop2 *vop2 = wfd_dev_to_vop2(wfd_dev);

    if (vop2->version == VOP_VERSION_RK3568)
        rk3568_vop2_video_port_init(vp);
}

static int vop2_wfd_pthread_mutext_init(struct wfd_port *port)
{
    pthread_mutexattr_t attr;
    bool init_attr;
    int err;

    err = pthread_mutexattr_init(&attr);
    init_attr = (err == EOK);

#ifndef NDEBUG
    // Use error-checking mutexes in debug builds,
    // for the assert_device_(un)locked macros.
    if (!err)
    {
        err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    }
#endif

    if (!err)
    {
        err = pthread_mutex_init(&port->vsync_lock, &attr);
    }

    if (init_attr)
    {
        (void)pthread_mutexattr_destroy(&attr);
    }
    if (err)
    {
        slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed: %s",
              "vysnc mutex initialisation", strerror(err));
    }

    return err;
}

static int vop2_wfd_pthread_cond_init(struct wfd_port *port)
{
    pthread_condattr_t attr;
    bool init_attr;
    int err;

    err = pthread_condattr_init(&attr);
    init_attr = (err == EOK);

    if (!err)
    {
        err = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    }

    if (!err)
    {
        err = pthread_cond_init(&port->vsync_cond, &attr);
    }

    if (init_attr)
    {
        (void)pthread_condattr_destroy(&attr);
    }

    if (err)
    {
        slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed: %s",
              "vsync condvar initialisation", strerror(err));
    }

    return err;
}

static void wfdmode_timing_convert_to_split_mode(struct wfdmode_timing *t)
{
    t->pixel_clock_kHz *= 2;
    t->hpixels *= 2;
    t->hsw *= 2;
    t->hfp *= 2;
    t->hbp *= 2;
}

void vop2_connector_pre_enable(struct wfd_port *port)
{
    struct vop2_video_port *vp = wfd_port_to_vp(port);
    int i;

    for (i = 0; i < vp->num_connectors; i++)
    {
        DEBUG1("connector[%d] pre enable: type = %ld, id = %ld\n", i, vp->connectors[i].connector_type, vp->connectors[i].id);
        // todo
        // connector_pre_enable(&vp->connectors[i])
    }
}

void vop2_connector_enable(struct wfd_port *port)
{
    struct vop2_video_port *vp = wfd_port_to_vp(port);
    int i;

    for (i = 0; i < vp->num_connectors; i++)
    {
        DEBUG1("connector[%d] enable: type = %ld, id = %ld\n", i, vp->connectors[i].connector_type, vp->connectors[i].id);
        // todo
        // connector_enable(&vp->connectors[i]);
    }
}

void vop2_connector_disable(struct wfd_port *port)
{
    struct vop2_video_port *vp = wfd_port_to_vp(port);
    uint32_t i;

    for (i = 0; i < vp->num_connectors; i++)
    {
        DEBUG1("connector[%d] disable: type = %ld, id = %ld\n", i, vp->connectors[i].connector_type, vp->connectors[i].id);
        // todo
        // connector_disable(&vp->connectors[i]);
    }
}

void vop2_connector_post_disable(struct wfd_port *port)
{
    struct vop2_video_port *vp = wfd_port_to_vp(port);
    uint32_t i;

    for (i = 0; i < vp->num_connectors; i++)
    {
        DEBUG1("connector[%d] post disable: type = %ld, id = %ld\n", i, vp->connectors[i].connector_type, vp->connectors[i].id);
        // todo
        // connector_post_disable(&vp->connectors[i]);
    }
}

static uint64_t drm_mode_vrefresh(const struct drm_display_mode *mode)
{
    unsigned int num, den;

    if (mode->htotal == 0 || mode->vtotal == 0)
        return 0;

    num = mode->clock;
    den = mode->htotal * mode->vtotal;

    if (mode->flags & VIDEO_MODE_FLAG_INTERLACE)
        num *= 2;
    if (mode->flags & VIDEO_MODE_FLAG_DBLSCAN)
        den *= 2;
    if (mode->vscan > 1)
        den *= mode->vscan;

    return DIV_ROUND_CLOSEST_ULL(mul_u32_u32(num, 1000), den);
}

void drm_mode_from_timing(const struct wfdmode_timing *timing,
                          struct drm_display_mode *mode)
{
    mode->clock = timing->pixel_clock_kHz;
    mode->hdisplay = timing->hpixels;
    mode->hsync_start = mode->hdisplay + timing->hfp;
    mode->hsync_end = mode->hsync_start + timing->hsw;
    mode->htotal = mode->hsync_end + timing->hbp;
    mode->vdisplay = timing->vlines;
    mode->vsync_start = mode->vdisplay + timing->vfp;
    mode->vsync_end = mode->vsync_start + timing->vsw;
    mode->vtotal = mode->vsync_end + timing->vbp;

    mode->crtc_clock = mode->clock;
    mode->crtc_hdisplay = mode->hdisplay;
    mode->crtc_hsync_start = mode->hsync_start;
    mode->crtc_hsync_end = mode->hsync_end;
    mode->crtc_htotal = mode->htotal;
    mode->crtc_hskew = mode->hskew;
    mode->crtc_vdisplay = mode->vdisplay;
    mode->crtc_vsync_start = mode->vsync_start;
    mode->crtc_vsync_end = mode->vsync_end;
    mode->crtc_vtotal = mode->vtotal;

    /*
     * we should do a convert here;
     */
    mode->vrefresh = drm_mode_vrefresh(mode);

    mode->flags = 0;

    if (timing->flags & WFDMODE_INVERT_HSYNC)
        mode->flags |= VIDEO_MODE_FLAG_NHSYNC;

    if (timing->flags & WFDMODE_INVERT_VSYNC)
        mode->flags |= VIDEO_MODE_FLAG_NVSYNC;

    if (timing->flags & WFDMODE_INTERLACE)
        mode->flags |= VIDEO_MODE_FLAG_INTERLACE;

    if (timing->flags & WFDMODE_DOUBLECLOCK)
        mode->flags |= VIDEO_MODE_FLAG_DBLCLK;
}

static void drm_mode_to_display_mode(struct drm_display_mode *adjusted_mode, struct DISPLAY_MODE_INFO *adjustedMode)
{
    adjustedMode->clock = adjusted_mode->clock;
    adjustedMode->hdisplay = adjusted_mode->hdisplay;
    adjustedMode->hsyncStart = adjusted_mode->hsync_start;
    adjustedMode->hsyncEnd = adjusted_mode->hsync_end;
    adjustedMode->htotal = adjusted_mode->htotal;
    adjustedMode->vdisplay = adjusted_mode->vdisplay;
    adjustedMode->vsyncStart = adjusted_mode->vsync_start;
    adjustedMode->vsyncEnd = adjusted_mode->vsync_end;
    adjustedMode->vtotal = adjusted_mode->vtotal;
    adjustedMode->crtcClock = adjusted_mode->crtc_clock;
    adjustedMode->crtcHdisplay = adjusted_mode->crtc_hdisplay;
    adjustedMode->crtcHsyncStart = adjusted_mode->crtc_hsync_start;
    adjustedMode->crtcHsyncEnd = adjusted_mode->crtc_hsync_end;
    adjustedMode->crtcHtotal = adjusted_mode->crtc_htotal;
    adjustedMode->crtcVdisplay = adjusted_mode->crtc_vdisplay;
    adjustedMode->crtcVsyncStart = adjusted_mode->crtc_vsync_start;
    adjustedMode->crtcVsyncEnd = adjusted_mode->crtc_vsync_end;
    adjustedMode->crtcVtotal = adjusted_mode->crtc_vtotal;
    adjustedMode->flags = adjusted_mode->flags;
    adjustedMode->vrefresh = adjusted_mode->vrefresh;
}

static void crtc_state_init(struct drm_crtc *crtc, struct CRTC_STATE *crtc_state)
{
    struct rockchip_crtc_state *vcstate = to_rockchip_crtc_state(crtc->state);

    drm_mode_to_display_mode(&crtc->state->adjusted_mode, &crtc_state->adjustedMode);

    crtc_state->outputType = vcstate->output_type;
    crtc_state->outputMode = vcstate->output_mode;
    crtc_state->outputFlags = vcstate->output_flags;
    crtc_state->spliceMode = vcstate->splice_mode;
    crtc_state->vdisplay = vcstate->vdisplay;
    crtc_state->outputIf = vcstate->output_if;
    crtc_state->busFormat = vcstate->bus_format;
    crtc_state->yuvOverlay = vcstate->yuv_overlay;
}

/*
 * phys_id is used to identify a main window(Cluster Win/Smart Win, not
 * include the sub win of a cluster or the multi area) that can do
 * overlay in main overlay stage.
 */
static struct vop2_win *vop2_find_win_by_phys_id(struct vop2 *vop2, uint64_t phys_id)
{
    struct vop2_win *win;
    uint32_t i;

    for (i = 0; i < vop2->registered_num_wins; i++)
    {
        win = &vop2->win[i];
        if (win->phys_id == phys_id)
            return win;
    }

    return NULL;
}

/*
 * VOP2 architecture
 *
 +----------+   +-------------+
 |  Cluster |   | Sel 1 from 6
 |  window0 |   |    Layer0   |              +---------------+    +-------------+    +-----------+
 +----------+   +-------------+              |N from 6 layers|    |             |    | 1 from 3  |
 +----------+   +-------------+              |   Overlay0    |    | Video Port0 |    |    RGB    |
 |  Cluster |   | Sel 1 from 6|              |               |    |             |    +-----------+
 |  window1 |   |    Layer1   |              +---------------+    +-------------+
 +----------+   +-------------+                                                      +-----------+
 +----------+   +-------------+                               +-->                   | 1 from 3  |
 |  Esmart  |   | Sel 1 from 6|              +---------------+    +-------------+    |   LVDS    |
 |  window0 |   |   Layer2    |              |N from 6 Layers     |             |    +-----------+
 +----------+   +-------------+              |   Overlay1    +    | Video Port1 | +--->
 +----------+   +-------------+   -------->  |               |    |             |    +-----------+
 |  Esmart  |   | Sel 1 from 6|   -------->  +---------------+    +-------------+    | 1 from 3  |
 |  Window1 |   |   Layer3    |                               +-->                   |   MIPI    |
 +----------+   +-------------+                                                      +-----------+
 +----------+   +-------------+              +---------------+    +-------------+
 |  Smart   |   | Sel 1 from 6|              |N from 6 Layers|    |             |    +-----------+
 |  Window0 |   |    Layer4   |              |   Overlay2    |    | Video Port2 |    | 1 from 3  |
 +----------+   +-------------+              |               |    |             |    |   HDMI    |
 +----------+   +-------------+              +---------------+    +-------------+    +-----------+
 |  Smart   |   | Sel 1 from 6|                                                      +-----------+
 |  Window1 |   |    Layer5   |                                                      |  1 from 3 |
 +----------+   +-------------+                                                      |    eDP    |
 *                                                                                   +-----------+
 */
static void vop2_layer_map_initial(struct vop2 *vop2)
{
    struct vop2_layer *layer;
    struct vop2_video_port *vp;
    struct vop2_win *win;
    unsigned long win_mask;
    uint32_t used_layers = 0;
    uint32_t port_mux_cfg = 0;
    uint32_t port_mux;
    uint32_t vp_id;
    uint32_t nr_layers;
    unsigned long phys_id;
    uint8_t i, j;

    for (i = 0; i < vop2->data->nrVps; i++)
    {
        vp_id = i;
        j = 0;
        vp = &vop2->vps[vp_id];
        vp->win_mask = vp->plane_mask;
        nr_layers = (uint32_t)hweight_long(vp->win_mask);
        win_mask = vp->win_mask;
        for_each_set_bit(phys_id, &win_mask, ROCKCHIP_MAX_LAYER)
        {
            layer = &vop2->layers[used_layers + j];
            win = vop2_find_win_by_phys_id(vop2, (uint32_t)phys_id);
            HAL_VOP2_InitOverlay(phys_id, vp_id, layer->id, 0);
            win->vp_mask = BIT(i);
            win->old_vp_mask = win->vp_mask;
            layer->win_phys_id = win->phys_id;
            win->layer_id = layer->id;
            j++;
            DEBUG1("layer%d select %s for vp%d phys_id: %d\n", layer->id, win->name, vp_id, phys_id);
        }
        used_layers += nr_layers;
    }

    /*
     * The last Video Port(VP2 for RK3568, VP3 for RK3588) is fixed
     * at the last level of the all the mixers by hardware design,
     * so we just need to handle (nrVps - 1) vps here.
     */
    used_layers = 0;
    for (i = 0; i < vop2->data->nrVps - 1; i++)
    {
        vp = &vop2->vps[i];
        used_layers += (uint32_t)hweight_long(vp->win_mask);
        if (used_layers == 0)
            port_mux = 8;
        else
            port_mux = used_layers - 1;
        port_mux_cfg |= port_mux << (vp->id * 4);
    }

    /* the last VP is fixed */
    if (vop2->data->nrVps >= 1)
        port_mux_cfg |= (uint32_t)(7 << (4 * (vop2->data->nrVps - 1)));
    vop2->port_mux_cfg = (uint16_t)port_mux_cfg;
    HAL_VOP2_InitOverlay(0, 0, 0, vop2->port_mux_cfg);
}

static uint16_t vop2_calc_bg_ovl_and_port_mux(struct vop2_video_port *vp)
{
    struct vop2_video_port *prev_vp;
    struct vop2 *vop2 = vp->vop2;
    struct drm_crtc *crtc;
    struct rockchip_crtc_state *vcstate;
    struct CRTC_STATE *crtc_state;
    const struct VOP2_DATA *vop2_data = vop2->data;
    uint32_t port_mux_cfg = 0;
    uint32_t port_mux;
    uint32_t used_layers = 0;
    uint8_t i;

    for (i = 0; i < vop2_data->nrVps - 1; i++)
    {
        prev_vp = &vop2->vps[i];
        used_layers += (uint32_t)hweight_long(prev_vp->win_mask);
        if (vop2->version == VOP_VERSION_RK3588)
        {
            if (vop2->vps[0].hdr10_at_splice_mode && i == 0)
                used_layers += 1;
            if (vop2->vps[0].hdr10_at_splice_mode && i == 1)
                used_layers -= 1;
        }
        /*
         * when a window move from vp0 to vp1, or vp0 to vp2,
         * it should flow these steps:
         * (1) first commit, disable this windows on VP0,
         *     keep the win_mask of VP0.
         * (2) second commit, set this window to VP1, clear
         *     the corresponding win_mask on VP0, and set the
         *     corresponding win_mask on VP1.
         *  This means we only know the decrease of the windows
         *  number of VP0 until VP1 take it, so the port_mux of
         *  VP0 should change at VP1's commit.
         */
        if (used_layers == 0)
            port_mux = 8;
        else
            port_mux = used_layers - 1;

        port_mux_cfg |= port_mux << (prev_vp->id * 4);

        if (port_mux > vop2_data->nrMixers)
            prev_vp->bg_ovl_dly = 0;
        else
            prev_vp->bg_ovl_dly = (vop2_data->nrMixers - port_mux) << 1;
        crtc = &prev_vp->rockchip_crtc.crtc;
        vcstate = to_rockchip_crtc_state(crtc->state);
        crtc_state = &vcstate->crtc_state;
        crtc_state->bgOvlDly = prev_vp->bg_ovl_dly;
    }

    port_mux_cfg |= (uint32_t)(7 << (4 * (vop2->data->nrVps - 1)));

    return (uint16_t)port_mux_cfg;
}

static void vop2_setup_port_mux(struct vop2_video_port *vp)
{
    struct vop2 *vop2 = vp->vop2;
    uint16_t  port_mux_cfg;

    port_mux_cfg = vop2_calc_bg_ovl_and_port_mux(vp);
    if (vop2->port_mux_cfg != port_mux_cfg)
    {
        HAL_VOP2_InitOverlay(0, 0, 0, port_mux_cfg);
        vp->skip_vsync = true;
        vop2->port_mux_cfg = port_mux_cfg;
    }
}

static uint32_t vop2_find_start_mixer_id_for_vp(struct vop2 *vop2, uint32_t port_id)
{
    struct vop2_video_port *vp;
    unsigned long used_layer = 0;
    uint32_t i;

    for (i = 0; i < port_id; i++)
    {
        vp = &vop2->vps[i];
        used_layer += hweight_long(vp->win_mask);
    }

    return (uint32_t)used_layer;
}

static void vop2_parse_alpha(struct vop2_alpha_config *alpha_config,
                             struct vop2_alpha *alpha)
{
    u8 src_glb_alpha_en = (alpha_config->src_glb_alpha_value == 0xff) ? 0 : 1;
    u8 dst_glb_alpha_en = (alpha_config->dst_glb_alpha_value == 0xff) ? 0 : 1;
    bool src_color_mode = alpha_config->src_premulti_en ? ALPHA_SRC_PRE_MUL : ALPHA_SRC_NO_PRE_MUL;
    bool dst_color_mode = alpha_config->dst_premulti_en ? ALPHA_SRC_PRE_MUL : ALPHA_SRC_NO_PRE_MUL;

    alpha->src_color_ctrl.val = 0;
    alpha->dst_color_ctrl.val = 0;
    alpha->src_alpha_ctrl.val = 0;
    alpha->dst_alpha_ctrl.val = 0;

    if (!alpha_config->src_pixel_alpha_en)
        alpha->src_color_ctrl.bits.blend_mode = ALPHA_GLOBAL;
    else if (alpha_config->src_pixel_alpha_en && !src_glb_alpha_en)
        alpha->src_color_ctrl.bits.blend_mode = ALPHA_PER_PIX;
    else
        alpha->src_color_ctrl.bits.blend_mode = ALPHA_PER_PIX_GLOBAL;

    alpha->src_color_ctrl.bits.alpha_en = 1;

    if (alpha->src_color_ctrl.bits.blend_mode == ALPHA_GLOBAL)
    {
        alpha->src_color_ctrl.bits.color_mode = src_color_mode;
        alpha->src_color_ctrl.bits.factor_mode = SRC_FAC_ALPHA_SRC_GLOBAL;
    }
    else if (alpha->src_color_ctrl.bits.blend_mode == ALPHA_PER_PIX)
    {
        alpha->src_color_ctrl.bits.color_mode = src_color_mode;
        alpha->src_color_ctrl.bits.factor_mode = SRC_FAC_ALPHA_ONE;
    }
    else
    {
        alpha->src_color_ctrl.bits.color_mode = ALPHA_SRC_PRE_MUL;
        alpha->src_color_ctrl.bits.factor_mode = SRC_FAC_ALPHA_SRC_GLOBAL;
    }
    alpha->src_color_ctrl.bits.glb_alpha = alpha_config->src_glb_alpha_value;
    alpha->src_color_ctrl.bits.alpha_mode = ALPHA_STRAIGHT;
    alpha->src_color_ctrl.bits.alpha_cal_mode = ALPHA_SATURATION;

    alpha->dst_color_ctrl.bits.alpha_mode = ALPHA_STRAIGHT;
    alpha->dst_color_ctrl.bits.alpha_cal_mode = ALPHA_SATURATION;
    alpha->dst_color_ctrl.bits.blend_mode = ALPHA_GLOBAL;
    alpha->dst_color_ctrl.bits.glb_alpha = alpha_config->dst_glb_alpha_value;
    alpha->dst_color_ctrl.bits.color_mode = dst_color_mode;
    alpha->dst_color_ctrl.bits.factor_mode = ALPHA_SRC_INVERSE;

    alpha->src_alpha_ctrl.bits.alpha_mode = ALPHA_STRAIGHT;
    alpha->src_alpha_ctrl.bits.blend_mode = alpha->src_color_ctrl.bits.blend_mode;
    alpha->src_alpha_ctrl.bits.alpha_cal_mode = ALPHA_SATURATION;
    alpha->src_alpha_ctrl.bits.factor_mode = ALPHA_ONE;

    alpha->dst_alpha_ctrl.bits.alpha_mode = ALPHA_STRAIGHT;
    if (alpha_config->dst_pixel_alpha_en && !dst_glb_alpha_en)
        alpha->dst_alpha_ctrl.bits.blend_mode = ALPHA_PER_PIX;
    else
        alpha->dst_alpha_ctrl.bits.blend_mode = ALPHA_PER_PIX_GLOBAL;
    alpha->dst_alpha_ctrl.bits.alpha_cal_mode = ALPHA_NO_SATURATION;
    alpha->dst_alpha_ctrl.bits.factor_mode = ALPHA_SRC_INVERSE;
}

/*
 * src: top layer
 * dst: bottom layer.
 * Cluster mixer default use win1 as top layer
 */
static void vop2_setup_cluster_alpha(struct vop2 *vop2, struct vop2_cluster *cluster)
{
    struct drm_framebuffer *fb;
    struct vop2_alpha_config alpha_config;
    struct vop2_alpha alpha;
    struct vop2_win *main_win = cluster->main;
    struct vop2_win *sub_win = cluster->sub;
    struct drm_plane *plane;
    struct vop2_plane_state *main_vpstate;
    struct vop2_plane_state *sub_vpstate;
    struct vop2_plane_state *top_win_vpstate;
    struct vop2_plane_state *bottom_win_vpstate;
    bool src_pixel_alpha_en = false;
    uint8_t src_glb_alpha_val = 0xff, dst_glb_alpha_val = 0xff;
    bool premulti_en = false;
    bool swap = false;

    if (!sub_win)
    {
        /* At one win mode, win0 is dst/bottom win, and win1 is a all zero src/top win */

        /*
         * right cluster share the same plane state in splice mode
         */
        if (cluster->splice_mode)
            plane = &main_win->left_win->base;
        else
            plane = &main_win->base;

        top_win_vpstate = NULL;
        bottom_win_vpstate = to_vop2_plane_state(plane->state);
        src_glb_alpha_val = 0;
        dst_glb_alpha_val = bottom_win_vpstate->global_alpha;
    }
    else
    {
        plane = &sub_win->base;
        sub_vpstate = to_vop2_plane_state(plane->state);
        plane = &main_win->base;
        main_vpstate = to_vop2_plane_state(plane->state);
        if (main_vpstate->zpos > sub_vpstate->zpos)
        {
            swap = 1;
            top_win_vpstate = main_vpstate;
            bottom_win_vpstate = sub_vpstate;
        }
        else
        {
            swap = 0;
            top_win_vpstate = sub_vpstate;
            bottom_win_vpstate = main_vpstate;
        }
        src_glb_alpha_val = top_win_vpstate->global_alpha;
        dst_glb_alpha_val = bottom_win_vpstate->global_alpha;
    }

    if (top_win_vpstate)
    {
        fb = top_win_vpstate->base.fb;
        if (!fb)
            return;
        if (top_win_vpstate->base.pixel_blend_mode == DRM_MODE_BLEND_PREMULTI)
            premulti_en = true;
        else
            premulti_en = false;
        src_pixel_alpha_en = is_alpha_support(fb->format->format);
    }
    fb = bottom_win_vpstate->base.fb;
    if (!fb)
        return;
    alpha_config.src_premulti_en = premulti_en;
    alpha_config.dst_premulti_en = false;
    alpha_config.src_pixel_alpha_en = src_pixel_alpha_en;
    alpha_config.dst_pixel_alpha_en = true; /* alpha value need transfer to next mix */
    alpha_config.src_glb_alpha_value = src_glb_alpha_val;
    alpha_config.dst_glb_alpha_value = dst_glb_alpha_val;
    vop2_parse_alpha(&alpha_config, &alpha);

    alpha.src_color_ctrl.bits.src_dst_swap = swap;
    HAL_VOP2_SetupAlpha(main_win->phys_id, alpha.src_color_ctrl.val, alpha.dst_color_ctrl.val,
                        alpha.src_alpha_ctrl.val, alpha.dst_alpha_ctrl.val, true);
}

static void vop2_setup_alpha(struct vop2_video_port *vp,
                             const struct vop2_zpos *vop2_zpos)
{
    struct vop2 *vop2 = vp->vop2;
    unsigned long win_mask = vp->win_mask;
    const struct vop2_zpos *zpos;
    struct vop2_plane_state *vpstate;
    struct vop2_alpha_config alpha_config;
    struct vop2_alpha alpha;
    struct vop2_win *win;
    struct drm_plane_state *pstate;
    struct drm_framebuffer *fb;
    int pixel_alpha_en;
    int premulti_en = 1;
    uint32_t mixer_id;
    unsigned long phys_id;
    unsigned long i;
    bool bottom_layer_alpha_en = false;
    uint8_t dst_global_alpha = 0xff;

    for_each_set_bit(phys_id, &win_mask, ROCKCHIP_MAX_LAYER)
    {
        win = vop2_find_win_by_phys_id(vop2, (uint32_t)phys_id);
        if (win->splice_mode_right)
            pstate = win->left_win->base.state;
        else
            pstate = win->base.state;

        vpstate = to_vop2_plane_state(pstate);

        if (!vop2_plane_active(pstate))
            continue;

        if (vpstate->zpos == 0 && vpstate->global_alpha != 0xff &&
                !vop2_cluster_window(win))
        {
            /*
             * If bottom layer have global alpha effect [except cluster layer,
             * because cluster have deal with bottom layer global alpha value
             * at cluster mix], bottom layer mix need deal with global alpha.
             */
            bottom_layer_alpha_en = true;
            dst_global_alpha = vpstate->global_alpha;
            if (pstate->pixel_blend_mode == DRM_MODE_BLEND_PREMULTI)
                premulti_en = 1;
            else
                premulti_en = 0;

            break;
        }
    }

    mixer_id = vop2_find_start_mixer_id_for_vp(vop2, vp->id);

    if (vop2->version == VOP_VERSION_RK3588 &&
            vp->hdr10_at_splice_mode && vp->id == 0)
        mixer_id++;/* fixed path for rk3588: layer1 -> hdr10_1 */

    alpha_config.dst_pixel_alpha_en = true; /* alpha value need transfer to next mix */
    for (i = 1; i < vp->nr_layers; i++)
    {
        zpos = &vop2_zpos[i];
        win = vop2_find_win_by_phys_id(vop2, zpos->win_phys_id);
        if (win->splice_mode_right)
            pstate = win->left_win->base.state;
        else
            pstate = win->base.state;

        vpstate = to_vop2_plane_state(pstate);
        fb = pstate->fb;
        if (pstate->pixel_blend_mode == DRM_MODE_BLEND_PREMULTI)
            premulti_en = 1;
        else
            premulti_en = 0;
        pixel_alpha_en = is_alpha_support(fb->format->format);

        alpha_config.src_premulti_en = premulti_en;
        if (bottom_layer_alpha_en && i == 1)  /* Cd = Cs + (1 - As) * Cd * Agd */
        {
            alpha_config.dst_premulti_en = false;
            alpha_config.src_pixel_alpha_en = pixel_alpha_en;
            alpha_config.src_glb_alpha_value =  vpstate->global_alpha;
            alpha_config.dst_glb_alpha_value = dst_global_alpha;
        }
        else if (vop2_cluster_window(win))    /* Mix output data only have pixel alpha */
        {
            alpha_config.dst_premulti_en = true;
            alpha_config.src_pixel_alpha_en = true;
            alpha_config.src_glb_alpha_value = 0xff;
            alpha_config.dst_glb_alpha_value = 0xff;
        }
        else    /* Cd = Cs + (1 - As) * Cd */
        {
            alpha_config.dst_premulti_en = true;
            alpha_config.src_pixel_alpha_en = pixel_alpha_en;
            alpha_config.src_glb_alpha_value =  vpstate->global_alpha;
            alpha_config.dst_glb_alpha_value = 0xff;
        }
        vop2_parse_alpha(&alpha_config, &alpha);

        HAL_VOP2_SetupAlpha(mixer_id + i - 1, alpha.src_color_ctrl.val, alpha.dst_color_ctrl.val,
                            alpha.src_alpha_ctrl.val, alpha.dst_alpha_ctrl.val, false);
    }

    /* Transfer pixel alpha value to next mix */
    alpha_config.src_premulti_en = true;
    alpha_config.dst_premulti_en = true;
    alpha_config.src_pixel_alpha_en = false;
    alpha_config.src_glb_alpha_value = 0xff;
    alpha_config.dst_glb_alpha_value = 0xff;
    vop2_parse_alpha(&alpha_config, &alpha);

    for (; i < hweight_long(vp->win_mask); i++)
        HAL_VOP2_SetupAlpha(mixer_id + i - 1, alpha.src_color_ctrl.val, alpha.dst_color_ctrl.val,
                            alpha.src_alpha_ctrl.val, alpha.dst_alpha_ctrl.val, false);
}

static void vop2_plane_destory_state(struct drm_plane *plane)
{
    struct vop2_plane_state *vpstate;

    if (plane->state)
    {
        vpstate = to_vop2_plane_state(plane->state);

        dss_free(vpstate);
        if (plane->state->fb)
        {
            dss_free(plane->state->fb);
            plane->state->fb = NULL;
        }
        plane->state = NULL;
    }
}

static int vop2_zpos_cmp(const void *a, const void *b)
{
    struct vop2_zpos *pa = (struct vop2_zpos *)a;
    struct vop2_zpos *pb = (struct vop2_zpos *)b;

    return pa->zpos - pb->zpos;
}

static void vop2_crtc_atomic_enable(struct drm_crtc *crtc, struct drm_crtc_state *old_state)
{
    struct vop2_video_port *vp = to_vop2_video_port(crtc);
    struct vop2 *vop2 = vp->vop2;
    struct rockchip_crtc_state *vcstate = to_rockchip_crtc_state(crtc->state);
    struct CRTC_STATE *crtc_state = &vcstate->crtc_state;
    unsigned long win_mask;
    unsigned long phys_id;
    uint32_t win_num = 0;

    if (!vop2->active_vp_mask)
    {
        HAL_VOP2_Init(crtc_state);

        vop2_layer_map_initial(vop2);
        vop2->is_enabled = true;
    }
    vop2->enable_count++;

    crtc_state_init(crtc, crtc_state);

    crtc_state->winMask = vp->win_mask;
    win_mask = crtc_state->winMask;
    for_each_set_bit(phys_id, &win_mask, ROCKCHIP_MAX_LAYER)
    {
        crtc_state->bindingWinPhyIds[win_num] = phys_id;
        win_num++;
    }
    crtc_state->bindingWinNum = win_num;

    HAL_VOP2_AtomicEnable(crtc_state);

    vop2->active_vp_mask |= (uint8_t)BIT((uint8_t)vp->id);
    vp->output_if = vcstate->output_if;
}

void wfd_vop2_crtc_enable(struct wfd_port *port)
{
    struct vop2_video_port *vp = wfd_port_to_vp(port);
    struct drm_crtc *crtc = &vp->rockchip_crtc.crtc;
    struct drm_crtc_state *cstate = crtc->state;
    struct wfdmode_timing *tm = &vp->wfd_port.pending.mode->timing;

    drm_mode_from_timing(tm, &cstate->adjusted_mode);

    vop2_crtc_atomic_enable(crtc, cstate);
}

static struct vop2_power_domain *vop2_find_pd_by_id(struct vop2 *vop2, uint8_t id)
{
    struct vop2_power_domain *pd, *n;

    rt_list_for_each_entry_safe(pd, n, &vop2->pd_list_head, list)
    {
        if (pd->data->id == id)
            return pd;
    }

    return NULL;
}

static void vop2_crtc_atomic_disable(struct drm_crtc *crtc, struct drm_crtc_state *old_state)
{
    struct vop2_video_port *vp = to_vop2_video_port(crtc);
    struct vop2 *vop2 = vp->vop2;
    struct rockchip_crtc_state *vcstate = to_rockchip_crtc_state(crtc->state);
    struct CRTC_STATE *crtc_state = &vcstate->crtc_state;

    HAL_VOP2_AtomicDisable(crtc_state);

    vop2->active_vp_mask &= (uint8_t)(~BIT(vp->id));

    vop2->is_enabled = false;

    vp->output_if = 0;
    vcstate->splice_mode = false;
    vp->splice_mode_right = false;
    vp->loader_protect = false;
}

void wfd_vop2_crtc_disable(struct wfd_port *port)
{
    struct vop2_video_port *vp = wfd_port_to_vp(port);
    struct drm_crtc *crtc = &vp->rockchip_crtc.crtc;
    struct drm_crtc_state *cstate = crtc->state;

    vop2_connector_disable(port);
    vop2_connector_post_disable(port);
    vop2_crtc_atomic_disable(crtc, cstate);
}

static void vop2_win_disable(struct vop2_win *win, bool skip_splice_win)
{
    struct vop2 *vop2 = win->vop2;

    DEBUG1("%s %s\n", __func__, win->name);

    /* Disable the right splice win */
    if (win->splice_win && !skip_splice_win)
    {
        vop2_win_disable(win->splice_win, false);
        win->splice_win = NULL;
    }

    if (HAL_VOP2_GetWinStatus(win->phys_id))
    {
        if (win->feature & WIN_FEATURE_CLUSTER_MAIN)
        {
            struct vop2_win *sub_win;
            uint32_t i = 0;

            for (i = 0; i < vop2->registered_num_wins; i++)
            {
                sub_win = &vop2->win[i];

                if ((sub_win->phys_id == win->phys_id) &&
                        (sub_win->feature & WIN_FEATURE_CLUSTER_SUB))
                    HAL_VOP2_DisableWin(sub_win->phys_id);
            }

            DEBUG1("%s %s %d\n", __func__, win->name, __LINE__);
        }
        HAL_VOP2_DisableWin(win->phys_id);
    }

    if (win->left_win && win->splice_mode_right)
    {
        win->left_win = NULL;
        win->splice_mode_right = false;
    }
}

static void vop2_calc_drm_rect_for_splice(struct vop2_plane_state *vpstate,
        struct drm_rect *left_src, struct drm_rect *left_dst,
        struct drm_rect *right_src, struct drm_rect *right_dst)
{
    struct drm_crtc *crtc = vpstate->base.crtc;
    struct drm_display_mode *mode = &crtc->state->adjusted_mode;
    struct drm_rect *dst = &vpstate->dst;
    struct drm_rect *src = &vpstate->src;
    uint16_t  half_hdisplay = (uint16_t)mode->crtc_hdisplay >> 1;
    int hscale = drm_rect_calc_hscale(src, dst, 0, INT_MAX);
    int dst_w = drm_rect_width(dst);
    int src_w = drm_rect_width(src) >> 16;
    int left_src_w, left_dst_w, right_dst_w;
    struct drm_plane_state *pstate = &vpstate->base;
    struct drm_framebuffer *fb = pstate->fb;

    left_dst_w = min(half_hdisplay, dst->x2) - dst->x1;
    if (left_dst_w < 0)
        left_dst_w = 0;
    right_dst_w = dst_w - left_dst_w;

    if (!right_dst_w)
        left_src_w = src_w;
    else
        left_src_w = (left_dst_w * hscale) >> 16;

    /*
     * Make sure the yrgb/uv mst of right win are byte aligned
     * with full pixel.
     */
    if (right_dst_w)
    {
        if (fb->format->format == DRM_FORMAT_NV15)
            left_src_w &= ~0x7;
        else if (fb->format->format == DRM_FORMAT_NV12)
            left_src_w &= ~0x1;
    }
    left_src->x1 = src->x1;
    left_src->x2 = src->x1 + (left_src_w << 16);
    left_dst->x1 = dst->x1;
    left_dst->x2 = dst->x1 + left_dst_w;
    right_src->x1 = left_src->x2;
    right_src->x2 = src->x2;
    right_dst->x1 = dst->x1 + left_dst_w - half_hdisplay;
    if (right_dst->x1 < 0)
        right_dst->x1 = 0;

    right_dst->x2 = right_dst->x1 + right_dst_w;

    left_src->y1 = src->y1;
    left_src->y2 = src->y2;
    left_dst->y1 = dst->y1;
    left_dst->y2 = dst->y2;
    right_src->y1 = src->y1;
    right_src->y2 = src->y2;
    right_dst->y1 = dst->y1;
    right_dst->y2 = dst->y2;
}

static uint32_t vop2_convert_csc_mode(uint32_t csc_mode, int bit_depth)
{
    switch (csc_mode)
    {
    case V4L2_COLORSPACE_SMPTE170M:
    case V4L2_COLORSPACE_470_SYSTEM_M:
    case V4L2_COLORSPACE_470_SYSTEM_BG:
        return CSC_BT601L;
    case V4L2_COLORSPACE_REC709:
    case V4L2_COLORSPACE_SMPTE240M:
    case V4L2_COLORSPACE_DEFAULT:
        if (bit_depth == CSC_13BIT_DEPTH)
            return CSC_BT709L_13BIT;
        else
            return CSC_BT709L;
    case V4L2_COLORSPACE_JPEG:
        return CSC_BT601F;
    case V4L2_COLORSPACE_BT2020:
        if (bit_depth == CSC_13BIT_DEPTH)
            return CSC_BT2020L_13BIT;
        else
            return CSC_BT2020;
    case V4L2_COLORSPACE_BT709F:
        if (bit_depth == CSC_10BIT_DEPTH)
        {
            DEBUG1("Unsupported bt709f at 10bit csc depth, use bt601f instead\n");
            return CSC_BT601F;
        }
        else
        {
            return CSC_BT709F_13BIT;
        }
    case V4L2_COLORSPACE_BT2020F:
        if (bit_depth == CSC_10BIT_DEPTH)
        {
            DEBUG1("Unsupported bt2020f at 10bit csc depth, use bt601f instead\n");
            return CSC_BT601F;
        }
        else
        {
            return CSC_BT2020F_13BIT;
        }
    default:
        return CSC_BT709L;
    }
}

/*
 * colorspace path:
 *      Input        Win csc                     Output
 * 1. YUV(2020)  --> Y2R->2020To709->R2Y   --> YUV_OUTPUT(601/709)
 *    RGB        --> R2Y                  __/
 *
 * 2. YUV(2020)  --> bypasss               --> YUV_OUTPUT(2020)
 *    RGB        --> 709To2020->R2Y       __/
 *
 * 3. YUV(2020)  --> Y2R->2020To709        --> RGB_OUTPUT(709)
 *    RGB        --> R2Y                  __/
 *
 * 4. YUV(601/709)-> Y2R->709To2020->R2Y   --> YUV_OUTPUT(2020)
 *    RGB        --> 709To2020->R2Y       __/
 *
 * 5. YUV(601/709)-> bypass                --> YUV_OUTPUT(709)
 *    RGB        --> R2Y                  __/
 *
 * 6. YUV(601/709)-> bypass                --> YUV_OUTPUT(601)
 *    RGB        --> R2Y(601)             __/
 *
 * 7. YUV        --> Y2R(709)              --> RGB_OUTPUT(709)
 *    RGB        --> bypass               __/
 *
 * 8. RGB        --> 709To2020->R2Y        --> YUV_OUTPUT(2020)
 *
 * 9. RGB        --> R2Y(709)              --> YUV_OUTPUT(709)
 *
 * 10. RGB       --> R2Y(601)              --> YUV_OUTPUT(601)
 *
 * 11. RGB       --> bypass                --> RGB_OUTPUT(709)
 */

static void vop2_setup_csc_mode(struct vop2_video_port *vp,
                                struct vop2_plane_state *vpstate)
{
    struct drm_plane_state *pstate = &vpstate->base;
    struct rockchip_crtc_state *vcstate = to_rockchip_crtc_state(vp->rockchip_crtc.crtc.state);
    int is_input_yuv = pstate->fb->format->is_yuv;
    uint32_t is_output_yuv = vcstate->yuv_overlay;
    uint32_t input_csc = vpstate->color_space;
    uint32_t output_csc = vcstate->color_space;
    struct vop2_win *win = to_vop2_win(pstate->plane);
    int csc_y2r_bit_depth = CSC_10BIT_DEPTH;

    if (win->feature & WIN_FEATURE_Y2R_13BIT_DEPTH)
        csc_y2r_bit_depth = CSC_13BIT_DEPTH;

    vpstate->y2r_en = 0;
    vpstate->r2y_en = 0;
    vpstate->csc_mode = 0;

    if (is_vop3(vp->vop2))
    {
        if (vpstate->hdr_in)
        {
            if (is_input_yuv)
            {
                vpstate->y2r_en = 1;
                vpstate->csc_mode = vop2_convert_csc_mode(input_csc,
                                    CSC_13BIT_DEPTH);
            }
            return;
        }
        else if (vp->sdr2hdr_en)
        {
            if (is_input_yuv)
            {
                vpstate->y2r_en = 1;
                vpstate->csc_mode = vop2_convert_csc_mode(input_csc,
                                    csc_y2r_bit_depth);
            }
            return;
        }
    }
    else
    {
        /* hdr2sdr and sdr2hdr will do csc itself */
        if (vpstate->hdr2sdr_en)
        {
            /*
             * This is hdr2sdr enabled plane
             * If it's RGB layer do hdr2sdr, we need to do r2y before send to hdr2sdr,
             * because hdr2sdr only support yuv input.
             */
            if (!is_input_yuv)
            {
                vpstate->r2y_en = 1;
                vpstate->csc_mode = vop2_convert_csc_mode(output_csc,
                                    CSC_10BIT_DEPTH);
            }
            return;
        }
        else if (!vpstate->hdr_in && vp->sdr2hdr_en)
        {
            /*
             * This is sdr2hdr enabled plane
             * If it's YUV layer do sdr2hdr, we need to do y2r before send to sdr2hdr,
             * because sdr2hdr only support rgb input.
             */
            if (is_input_yuv)
            {
                vpstate->y2r_en = 1;
                vpstate->csc_mode = vop2_convert_csc_mode(input_csc,
                                    csc_y2r_bit_depth);
            }
            return;
        }
    }

    if (is_input_yuv && !is_output_yuv)
    {
        vpstate->y2r_en = 1;
        vpstate->csc_mode = vop2_convert_csc_mode(input_csc, csc_y2r_bit_depth);
    }
    else if (!is_input_yuv && is_output_yuv)
    {
        vpstate->r2y_en = 1;
        vpstate->csc_mode = vop2_convert_csc_mode(output_csc, CSC_10BIT_DEPTH);
    }
}

/*
 * 0: Full mode, 16 lines for one tail
 * 1: half block mode
 */
static uint8_t vop2_afbc_half_block_enable(struct vop2_plane_state *vpstate)
{
    if (vpstate->rotate_270_en || vpstate->rotate_90_en)
        return 0;
    else
        return 1;
}

static bool vop2_win_rb_swap(uint32_t format)
{
    switch (format)
    {
    case DRM_FORMAT_XBGR2101010:
    case DRM_FORMAT_ABGR2101010:
    case DRM_FORMAT_XBGR8888:
    case DRM_FORMAT_ABGR8888:
    case DRM_FORMAT_BGR888:
    case DRM_FORMAT_BGR565:
        return true;
    default:
        return false;
    }
}

static bool vop2_afbc_rb_swap(uint32_t format)
{
    switch (format)
    {
    case DRM_FORMAT_NV24:
    case DRM_FORMAT_NV30:
        return true;
    default:
        return false;
    }
}

static bool vop2_afbc_uv_swap(uint32_t format)
{
    switch (format)
    {
    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV16:
    case DRM_FORMAT_YUYV:
    case DRM_FORMAT_Y210:
    case DRM_FORMAT_YUV420_8BIT:
    case DRM_FORMAT_YUV420_10BIT:
        return true;
    default:
        return false;
    }
}

static bool vop2_win_uv_swap(uint32_t format)
{
    switch (format)
    {
    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV16:
    case DRM_FORMAT_NV24:
    case DRM_FORMAT_NV15:
    case DRM_FORMAT_NV20:
    case DRM_FORMAT_NV30:
    case DRM_FORMAT_YUYV:
    case DRM_FORMAT_UYVY:
        return true;
    default:
        return false;
    }
}

uint32_t rockchip_drm_get_bpp(const struct drm_format_info *info)
{
    /* use whatever a driver has set */
    if (info->cpp[0])
        return (uint32_t)info->cpp[0] * 8;

    switch (info->format)
    {
    case DRM_FORMAT_YUV420_8BIT:
        return 12;
    case DRM_FORMAT_YUV420_10BIT:
        return 15;
    case DRM_FORMAT_VUY101010:
        return 30;
    default:
        break;
    }

    /* all attempts failed */
    return 0;
}

static eVOP2_dataFormat vop2_convert_format(uint32_t format)
{
    switch (format)
    {
    case DRM_FORMAT_XRGB2101010:
    case DRM_FORMAT_ARGB2101010:
    case DRM_FORMAT_XBGR2101010:
    case DRM_FORMAT_ABGR2101010:
        return VOP2_FMT_XRGB101010;
    case DRM_FORMAT_XRGB8888:
    case DRM_FORMAT_ARGB8888:
    case DRM_FORMAT_XBGR8888:
    case DRM_FORMAT_ABGR8888:
        return VOP2_FMT_ARGB8888;
    case DRM_FORMAT_RGB888:
    case DRM_FORMAT_BGR888:
        return VOP2_FMT_RGB888;
    case DRM_FORMAT_RGB565:
    case DRM_FORMAT_BGR565:
        return VOP2_FMT_RGB565;
    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV21:
    case DRM_FORMAT_YUV420_8BIT:
        return VOP2_FMT_YUV420SP;
    case DRM_FORMAT_NV15:
    case DRM_FORMAT_YUV420_10BIT:
        return VOP2_FMT_YUV420SP_10;
    case DRM_FORMAT_NV16:
    case DRM_FORMAT_NV61:
        return VOP2_FMT_YUV422SP;
    case DRM_FORMAT_NV20:
    case DRM_FORMAT_Y210:
        return VOP2_FMT_YUV422SP_10;
    case DRM_FORMAT_NV24:
    case DRM_FORMAT_NV42:
        return VOP2_FMT_YUV444SP;
    case DRM_FORMAT_NV30:
        return VOP2_FMT_YUV444SP_10;
    case DRM_FORMAT_YUYV:
    case DRM_FORMAT_YVYU:
        return VOP2_FMT_VYUY422;
    case DRM_FORMAT_VYUY:
    case DRM_FORMAT_UYVY:
        return VOP2_FMT_YUYV422;
    default:
        rt_kprintf("unsupported format[%08lx]\n", format);
        return -EINVAL;
    }
}

static eVOP2_afbcFormat vop2_convert_afbc_format(uint32_t format)
{
    switch (format)
    {
    case DRM_FORMAT_XRGB2101010:
    case DRM_FORMAT_ARGB2101010:
    case DRM_FORMAT_XBGR2101010:
    case DRM_FORMAT_ABGR2101010:
        return VOP2_AFBC_FMT_ARGB2101010;
    case DRM_FORMAT_XRGB8888:
    case DRM_FORMAT_ARGB8888:
    case DRM_FORMAT_XBGR8888:
    case DRM_FORMAT_ABGR8888:
        return VOP2_AFBC_FMT_ARGB8888;
    case DRM_FORMAT_RGB888:
    case DRM_FORMAT_BGR888:
        return VOP2_AFBC_FMT_RGB888;
    case DRM_FORMAT_RGB565:
    case DRM_FORMAT_BGR565:
        return VOP2_AFBC_FMT_RGB565;
    case DRM_FORMAT_YUV420_8BIT:
        return VOP2_AFBC_FMT_YUV420;
    case DRM_FORMAT_YUV420_10BIT:
        return VOP2_AFBC_FMT_YUV420_10BIT;
    case DRM_FORMAT_YVYU:
    case DRM_FORMAT_YUYV:
    case DRM_FORMAT_VYUY:
    case DRM_FORMAT_UYVY:
        return VOP2_AFBC_FMT_YUV422;
    case DRM_FORMAT_Y210:
        return VOP2_AFBC_FMT_YUV422_10BIT;

    /* either of the below should not be reachable */
    default:
        DEBUG1("unsupported AFBC format[%08x]\n", format);
        return VOP2_AFBC_FMT_INVALID;
    }

    return VOP2_AFBC_FMT_INVALID;
}

static eVOP3_tiledFormat vop3_convert_tiled_format(uint32_t format, uint32_t tile_mode)
{
    switch (format)
    {
    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV21:
        return tile_mode == ROCKCHIP_TILED_BLOCK_SIZE_8x8 ?
               VOP3_TILED_8X8_FMT_YUV420SP : VOP3_TILED_4X4_FMT_YUV420SP;
    case DRM_FORMAT_NV16:
    case DRM_FORMAT_NV61:
        return tile_mode == ROCKCHIP_TILED_BLOCK_SIZE_8x8 ?
               VOP3_TILED_8X8_FMT_YUV422SP : VOP3_TILED_4X4_FMT_YUV422SP;
    case DRM_FORMAT_NV24:
    case DRM_FORMAT_NV42:
        return tile_mode == ROCKCHIP_TILED_BLOCK_SIZE_8x8 ?
               VOP3_TILED_8X8_FMT_YUV444SP : VOP3_TILED_4X4_FMT_YUV444SP;
    case DRM_FORMAT_NV15:
        return tile_mode == ROCKCHIP_TILED_BLOCK_SIZE_8x8 ?
               VOP3_TILED_8X8_FMT_YUV420SP_10 : VOP3_TILED_4X4_FMT_YUV420SP_10;
    case DRM_FORMAT_NV20:
        return tile_mode == ROCKCHIP_TILED_BLOCK_SIZE_8x8 ?
               VOP3_TILED_8X8_FMT_YUV422SP_10 : VOP3_TILED_4X4_FMT_YUV422SP_10;
    case DRM_FORMAT_NV30:
        return tile_mode == ROCKCHIP_TILED_BLOCK_SIZE_8x8 ?
               VOP3_TILED_8X8_FMT_YUV444SP_10 : VOP3_TILED_4X4_FMT_YUV444SP_10;
    default:
        DEBUG1("unsupported tiled format[%08x]\n", format);
        return VOP3_TILED_FMT_INVALID;
    }

    return VOP3_TILED_FMT_INVALID;
}

static eVOP2_tiledFormat vop2_convert_tiled_format(uint32_t format)
{
    switch (format)
    {
    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV21:
        return VOP2_TILED_8X8_FMT_YUV420SP;
    case DRM_FORMAT_NV16:
    case DRM_FORMAT_NV61:
        return VOP2_TILED_8X8_FMT_YUV422SP;
    case DRM_FORMAT_NV24:
    case DRM_FORMAT_NV42:
        return VOP2_TILED_8X8_FMT_YUV444SP;
    case DRM_FORMAT_NV15:
        return VOP2_TILED_8X8_FMT_YUV420SP_10;
    case DRM_FORMAT_NV20:
        return VOP2_TILED_8X8_FMT_YUV422SP_10;
    case DRM_FORMAT_NV30:
        return VOP2_TILED_8X8_FMT_YUV444SP_10;
    default:
        DEBUG1("unsupported tiled format[%08x]\n", format);
        return VOP2_TILED_FMT_INVALID;
    }

    return VOP2_TILED_FMT_INVALID;
}

/*
 * @xoffset: the src x offset of the right win in splice mode, other wise it
 * must be zero.
 */
static uint32_t vop2_afbc_transform_offset(struct vop2_plane_state *vpstate, uint32_t xoffset)
{
    struct drm_rect *src = &vpstate->src;
    struct drm_framebuffer *fb = vpstate->base.fb;
    uint32_t bpp = rockchip_drm_get_bpp(fb->format);
    uint32_t vir_width = (fb->pitches[0] << 3) / (bpp ? bpp : 1);
    uint32_t width = (uint32_t)drm_rect_width(src) >> 16;
    uint32_t height = (uint32_t)drm_rect_height(src) >> 16;
    uint32_t act_xoffset = (uint32_t)src->x1 >> 16;
    uint32_t act_yoffset = (uint32_t)src->y1 >> 16;
    uint32_t align16_crop = 0;
    uint32_t align64_crop = 0;
    uint32_t height_tmp = 0;
    uint32_t transform_tmp = 0;
    uint32_t transform_xoffset = 0;
    uint32_t transform_yoffset = 0;
    uint32_t top_crop = 0;
    uint32_t top_crop_line_num = 0;
    uint32_t bottom_crop_line_num = 0;

    act_xoffset += xoffset;
    /* 16 pixel align */
    if (height & 0xf)
        align16_crop = 16 - (height & 0xf);

    height_tmp = height + align16_crop;

    /* 64 pixel align */
    if (height_tmp & 0x3f)
        align64_crop = 64 - (height_tmp & 0x3f);

    top_crop_line_num = top_crop << 2;
    if (top_crop == 0)
        bottom_crop_line_num = align16_crop + align64_crop;
    else if (top_crop == 1)
        bottom_crop_line_num = align16_crop + align64_crop + 12;
    else if (top_crop == 2)
        bottom_crop_line_num = align16_crop + align64_crop + 8;

    if (vpstate->xmirror_en)
    {
        if (vpstate->ymirror_en)
        {
            if (vpstate->afbc_half_block_en)
            {
                transform_tmp = act_xoffset + width;
                transform_xoffset = 16 - (transform_tmp & 0xf);
                transform_tmp = bottom_crop_line_num - act_yoffset;
                transform_yoffset = transform_tmp & 0x7;
            }
            else     //FULL MODEL
            {
                transform_tmp = act_xoffset + width;
                transform_xoffset = 16 - (transform_tmp & 0xf);
                transform_tmp = bottom_crop_line_num - act_yoffset;
                transform_yoffset = (transform_tmp & 0xf);
            }
        }
        else if (vpstate->rotate_90_en)
        {
            transform_tmp = bottom_crop_line_num - act_yoffset;
            transform_xoffset = transform_tmp & 0xf;
            transform_tmp = vir_width - width - act_xoffset;
            transform_yoffset = transform_tmp & 0xf;
        }
        else if (vpstate->rotate_270_en)
        {
            transform_tmp = top_crop_line_num + act_yoffset;
            transform_xoffset = transform_tmp & 0xf;
            transform_tmp = act_xoffset;
            transform_yoffset = transform_tmp & 0xf;

        }
        else     //xmir
        {
            if (vpstate->afbc_half_block_en)
            {
                transform_tmp = act_xoffset + width;
                transform_xoffset = 16 - (transform_tmp & 0xf);
                transform_tmp = top_crop_line_num + act_yoffset;
                transform_yoffset = transform_tmp & 0x7;
            }
            else
            {
                transform_tmp = act_xoffset + width;
                transform_xoffset = 16 - (transform_tmp & 0xf);
                transform_tmp = top_crop_line_num + act_yoffset;
                transform_yoffset = transform_tmp & 0xf;
            }
        }
    }
    else if (vpstate->ymirror_en)
    {
        if (vpstate->afbc_half_block_en)
        {
            transform_tmp = act_xoffset;
            transform_xoffset = transform_tmp & 0xf;
            transform_tmp = bottom_crop_line_num - act_yoffset;
            transform_yoffset = transform_tmp & 0x7;
        }
        else     //full_mode
        {
            transform_tmp = act_xoffset;
            transform_xoffset = transform_tmp & 0xf;
            transform_tmp = bottom_crop_line_num - act_yoffset;
            transform_yoffset = transform_tmp & 0xf;
        }
    }
    else if (vpstate->rotate_90_en)
    {
        transform_tmp = bottom_crop_line_num - act_yoffset;
        transform_xoffset = transform_tmp & 0xf;
        transform_tmp = act_xoffset;
        transform_yoffset = transform_tmp & 0xf;
    }
    else if (vpstate->rotate_270_en)
    {
        transform_tmp = top_crop_line_num + act_yoffset;
        transform_xoffset = transform_tmp & 0xf;
        transform_tmp = vir_width - width - act_xoffset;
        transform_yoffset = transform_tmp & 0xf;
    }
    else     //normal
    {
        if (vpstate->afbc_half_block_en)
        {
            transform_tmp = act_xoffset;
            transform_xoffset = transform_tmp & 0xf;
            transform_tmp = top_crop_line_num + act_yoffset;
            transform_yoffset = transform_tmp & 0x7;
        }
        else     //full_mode
        {
            transform_tmp = act_xoffset;
            transform_xoffset = transform_tmp & 0xf;
            transform_tmp = top_crop_line_num + act_yoffset;
            transform_yoffset = transform_tmp & 0xf;
        }
    }

    return (transform_xoffset & 0xf) | ((transform_yoffset & 0xf) << 16);
}

/*
 * A Cluster window has 2048 x 16 line buffer, which can
 * works at 2048 x 16(Full) or 4096 x 8 (Half) mode.
 * for Cluster_lb_mode register:
 * 0: half mode, for plane input width range 2048 ~ 4096
 * 1: half mode, for cluster work at 2 * 2048 plane mode
 * 2: half mode, for rotate_90/270 mode
 *
 */
static uint32_t vop2_get_cluster_lb_mode(struct vop2_win *win, struct vop2_plane_state *vpstate)
{
    if (vpstate->rotate_270_en || vpstate->rotate_90_en)
        return 2;
    else if (win->feature & WIN_FEATURE_CLUSTER_SUB)
        return 1;
    else
        return 0;
}

static void vop2_win_atomic_update(struct vop2_win *win, struct drm_rect *src, struct drm_rect *dst,
                                   struct drm_plane_state *pstate)
{
    struct drm_crtc *crtc = pstate->crtc;
    struct vop2_video_port *vp = to_vop2_video_port(crtc);
    struct rockchip_crtc_state *vcstate = to_rockchip_crtc_state(crtc->state);
    struct CRTC_STATE *crtc_state = &vcstate->crtc_state;
    struct vop2_plane_state *vpstate = to_vop2_plane_state(pstate);
    struct PLANE_STATE *plane_state = &vpstate->plane_state;
    struct vop2 *vop2 = win->vop2;
    struct drm_framebuffer *fb = pstate->fb;
    uint32_t bpp = rockchip_drm_get_bpp(fb->format);
    int actual_w, actual_h;
    uint32_t format;
    uint32_t afbc_format;
    uint32_t rb_swap;
    uint32_t uv_swap;
    uint8_t afbc_half_block_en;
    uint32_t lb_mode;
    uint32_t stride, uv_stride = 0;
    uint32_t transform_offset;
    /* offset of the right window in splice mode */
    uint32_t splice_pixel_offset = 0;
    uint32_t splice_yrgb_offset = 0;
    uint32_t splice_uv_offset = 0;
    uint32_t yrgb_mst;
    uint32_t uv_mst;

    actual_w = drm_rect_width(src) >> 16;
    actual_h = drm_rect_height(src) >> 16;
    if (!actual_w || !actual_h)
    {
        vop2_win_disable(win, true);
        return;
    }
    plane_state->src.x1 = src->x1;
    plane_state->src.x2 = src->x2;
    plane_state->src.y1 = src->y1;
    plane_state->src.y2 = src->y2;
    plane_state->dst.x1 = dst->x1;
    plane_state->dst.x2 = dst->x2;
    plane_state->dst.y1 = dst->y1;
    plane_state->dst.y2 = dst->y2;
    plane_state->physId = win->phys_id;
    plane_state->winId = win->win_id;

    stride = DIV_ROUND_UP(fb->pitches[0], 4);

    if (vpstate->tiled_en)
    {
        if (is_vop3(vop2))
            format = vop3_convert_tiled_format(fb->format->format, vpstate->tiled_en);
        else
            format = vop2_convert_tiled_format(fb->format->format);
    }
    else
    {
        format = vop2_convert_format(fb->format->format);
    }
    plane_state->tiledEn = vpstate->tiled_en;
    plane_state->format = format;

    vop2_setup_csc_mode(vp, vpstate);
    plane_state->y2rEn = vpstate->y2r_en;
    plane_state->r2yEn = vpstate->r2y_en;
    plane_state->cscMode = vpstate->csc_mode;

    afbc_half_block_en = vop2_afbc_half_block_enable(vpstate);

    plane_state->afbcEn = vpstate->afbc_en;
    if (vpstate->afbc_en)
    {
        /* the afbc superblock is 16 x 16 */
        afbc_format = vop2_convert_afbc_format(fb->format->format);
        /* Enable color transform for YTR */
        if (fb->modifier & AFBC_FORMAT_MOD_YTR)
            afbc_format |= (1 << 4);

        /* AFBC pic_vir_width is count by pixel, this is different
         * with WIN_VIR_STRIDE.
         */
        if (!bpp)
        {
            DEBUG1("bpp is zero\n");
            bpp = 1;
        }
        stride = (fb->pitches[0] << 3) / bpp;
        if ((stride & 0x3f) &&
                (vpstate->xmirror_en || vpstate->rotate_90_en || vpstate->rotate_270_en))
            rt_kprintf("vp%ld %s stride[%ld] must align as 64 pixel when enable xmirror/rotate_90/rotate_270[0x%x]\n",
                       vp->id, win->name, stride, pstate->rotation);

        rb_swap = vop2_afbc_rb_swap(fb->format->format);
        uv_swap = vop2_afbc_uv_swap(fb->format->format);
        vpstate->afbc_half_block_en = afbc_half_block_en;
        plane_state->afbcHalfBlockEn = vpstate->afbc_half_block_en;

        transform_offset = vop2_afbc_transform_offset(vpstate, splice_pixel_offset);

        plane_state->afbcFormat = afbc_format;
        plane_state->afbcStride = stride;
        plane_state->afbcRbSwap = rb_swap;
        plane_state->afbcUvSwap = uv_swap;
        plane_state->transformOffset = transform_offset;

        plane_state->xmirrorEn = vpstate->xmirror_en;
        plane_state->ymirrorEn = vpstate->ymirror_en;
        plane_state->rotateEn270 = vpstate->rotate_270_en;
        plane_state->rotateEn90 = vpstate->rotate_90_en;
    }

    yrgb_mst = vpstate->yrgb_mst + splice_yrgb_offset;
    uv_mst = vpstate->uv_mst + splice_uv_offset;

    rb_swap = vop2_win_rb_swap(fb->format->format);
    uv_swap = vop2_win_uv_swap(fb->format->format);
    if (vpstate->tiled_en)
    {
        uv_swap = 1;
        if (vpstate->tiled_en == ROCKCHIP_TILED_BLOCK_SIZE_8x8)
            stride <<= 3;
        else
            stride <<= 2;
    }
    plane_state->rbSwap = rb_swap;
    plane_state->uvSwap = uv_swap;

    if (fb->format->is_yuv)
    {
        plane_state->isYuv = true;
        uv_stride = DIV_ROUND_UP(fb->pitches[1], 4);
    }

    plane_state->stride = stride;
    plane_state->uvStride = uv_stride;
    plane_state->yrgbMst = yrgb_mst;
    plane_state->uvMst = uv_mst;
    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, (void *)yrgb_mst, actual_h * stride);
    if (fb->format->is_yuv)
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, (void *)uv_mst, actual_h * uv_stride);

    if (win->feature & WIN_FEATURE_Y2R_13BIT_DEPTH && !vop2_cluster_window(win))
        plane_state->csc13bitEn = !!(vpstate->csc_mode & CSC_BT709L_13BIT);

    if (vop2_cluster_window(win))
    {
        lb_mode = vop2_get_cluster_lb_mode(win, vpstate);
        plane_state->lbMode = lb_mode;
    }

    HAL_VOP2_AtomicUpdateWin(crtc_state, plane_state);
}

static void vop2_plane_atomic_update(struct drm_plane *plane, struct drm_plane_state *old_state)
{
    struct drm_plane_state *pstate = plane->state;
    struct drm_crtc *crtc = pstate->crtc;
    struct vop2_win *win = to_vop2_win(plane);
    struct vop2_win *splice_win;
    struct vop2_plane_state *vpstate = to_vop2_plane_state(pstate);
    struct rockchip_crtc_state *vcstate = to_rockchip_crtc_state(crtc->state);
    struct vop2 *vop2 = win->vop2;
    struct drm_rect wsrc;
    struct drm_rect wdst;
    /* right part in splice mode */
    struct drm_rect right_wsrc;
    struct drm_rect right_wdst;
    (void) old_state;

    if (vcstate->splice_mode)
    {
        vop2_calc_drm_rect_for_splice(vpstate, &wsrc, &wdst, &right_wsrc, &right_wdst);
        splice_win = win->splice_win;
        vop2_win_atomic_update(splice_win, &right_wsrc, &right_wdst, pstate);
    }
    else
    {
        memcpy(&wsrc, &vpstate->src, sizeof(struct drm_rect));
        memcpy(&wdst, &vpstate->dst, sizeof(struct drm_rect));
    }

    vop2_win_atomic_update(win, &wsrc, &wdst, pstate);

    vop2->is_iommu_needed = true;
}

static uint32_t vop2_convert_wfd_format(int format)
{
    int fmt;

    switch (format)
    {
    case WFD_FORMAT_NV12:       //YCbCr4:2:0 semi planar
        fmt = DRM_FORMAT_NV12;
        break;
    case WFD_FORMAT_RGB565:
        fmt = DRM_FORMAT_RGB565;
        break;
    case WFD_FORMAT_RGB888:
        fmt = DRM_FORMAT_RGB888;
        break;
    case WFD_FORMAT_RGBA8888:
        fmt = DRM_FORMAT_ARGB8888;
        break;
    case WFD_FORMAT_BGRA8888:
        fmt = DRM_FORMAT_ABGR8888;
        break;
    case WFD_FORMAT_RGBX8888:
        fmt = DRM_FORMAT_XRGB8888;
        break;
    case WFD_FORMAT_BGRX8888:
        fmt = DRM_FORMAT_XBGR8888;
        break;
    default:
        SLOG_ERR("WfdToVopFormat: format not supported:%d", format);
        fmt = DRM_FORMAT_XRGB8888;
        break;
    }

    return fmt;
}

static const struct drm_format_info *wfd_format_to_drm_format(int format)
{
    uint32_t fmt = vop2_convert_wfd_format(format);
    return drm_format_info(fmt);
}

static struct vop2_plane_state *vop2_plane_duplicate_state(struct drm_plane *plane)
{
    struct vop2_plane_state *vpstate;
    (void) plane;

    vpstate = dss_calloc(1, sizeof(*vpstate));
    if (!vpstate)
    {
        rt_kprintf("out of memory for vpstate");
        return NULL;
    }

    return vpstate;
}

static int vop2_pd_data_init(struct vop2 *vop2)
{
    const struct VOP2_DATA *vop2_data = vop2->data;
    const struct VOP2_POWER_DOMAIN_DATA *pd_data;
    struct vop2_power_domain *pd;
    int i;

    rt_list_init(&vop2->pd_list_head);

    for (i = 0; i < vop2_data->nrPds; i++)
    {
        pd_data = &vop2_data->pd[i];
        pd = dss_calloc(1, sizeof(*pd));
        if (!pd)
            return -ENOMEM;
        pd->vop2 = vop2;
        pd->data = pd_data;
        pd->vp_mask = 0;
        rt_list_insert_after(&pd->list, &vop2->pd_list_head);
        if (pd_data->parentId)
        {
            pd->parent = vop2_find_pd_by_id(vop2, pd_data->parentId);
            if (!pd->parent)
            {
                rt_kprintf("no parent pd find for pd%d\n", pd->data->id);
                return -EINVAL;
            }
        }
    }

    return 0;
}

static void vop2_dsc_data_init(struct vop2 *vop2)
{
    const struct VOP2_DATA *vop2_data = vop2->data;
    const struct VOP2_DSC_DATA *dsc_data;
    struct vop2_dsc *dsc;
    int i;

    for (i = 0; i < vop2_data->nrDscs; i++)
    {
        dsc = &vop2->dscs[i];
        dsc_data = &vop2_data->dsc[i];
        dsc->id = dsc_data->id;
        dsc->max_slice_num = dsc_data->maxSliceNum;
        dsc->max_linebuf_depth = dsc_data->maxLinebufDepth;
        dsc->min_bits_per_pixel = dsc_data->minBitsPerPixel;
        dsc->attach_vp_id = -1;
        if (dsc_data->pdId)
            dsc->pd = vop2_find_pd_by_id(vop2, dsc_data->pdId);
    }
}

static int vop2_win_init(struct vop2 *vop2)
{
    const struct VOP2_DATA *vop2_data = vop2->data;
    const struct VOP2_LAYER_DATA *layer_data;
    struct vop2_win *win;
    struct vop2_layer *layer;
    char name[DRM_PROP_NAME_LEN + 4];
    unsigned int num_wins = 0;
    uint8_t plane_id = 0;
    uint32_t i, j;

    for (i = 0; i < vop2_data->winSize; i++)
    {
        const struct VOP2_WIN_DATA *win_data = &vop2_data->win[i];

        win = &vop2->win[num_wins];
        win->name = win_data->name;
        win->offset = win_data->base;
        win->type = win_data->type;
        win->formats = win_data->formats;
        win->nformats = win_data->nformats;
        win->format_modifiers = win_data->formatModifiers;
        win->supported_rotations = win_data->supportedRotations;
        win->max_upscale_factor = win_data->maxUpscaleFactor;
        win->max_downscale_factor = win_data->maxDownscaleFactor;
        win->hsu_filter_mode = win_data->hsuFilterMode;
        win->hsd_filter_mode = win_data->hsdFilterMode;
        win->vsu_filter_mode = win_data->vsuFilterMode;
        win->vsd_filter_mode = win_data->vsdFilterMode;
        win->hsd_pre_filter_mode = win_data->hsdPreFilterMode;
        win->vsd_pre_filter_mode = win_data->vsdPreFilterMode;
        win->dly = win_data->dly;
        win->feature = win_data->feature;
        win->phys_id = win_data->physId;
        win->splice_win_id = win_data->spliceWinId;
        win->layer_sel_id = win_data->layerSelId;
        win->win_id = i;
        win->plane_id = plane_id++;
        win->area_id = 0;
        win->zpos = i;
        win->vop2 = vop2;
        win->axi_id = win_data->axiId;
        win->axi_yrgb_id = win_data->axiYrgbId;
        win->axi_uv_id = win_data->axiUvId;
        win->possible_crtcs = win_data->possibleCrtcs;

        if (win_data->pdId)
            win->pd = vop2_find_pd_by_id(vop2, win_data->pdId);

        num_wins++;

        if (!vop2->support_multi_area)
            continue;

        for (j = 0; j < win_data->areaSize; j++)
        {
            struct vop2_win *area = &vop2->win[num_wins];

            area->parent = win;
            area->offset = win->offset;
            area->type = DRM_PLANE_TYPE_OVERLAY;
            area->formats = win->formats;
            area->feature = win->feature;
            area->nformats = win->nformats;
            area->format_modifiers = win->format_modifiers;
            area->max_upscale_factor = win_data->maxUpscaleFactor;
            area->max_downscale_factor = win_data->maxDownscaleFactor;
            area->supported_rotations = win_data->supportedRotations;
            area->hsu_filter_mode = win_data->hsuFilterMode;
            area->hsd_filter_mode = win_data->hsdFilterMode;
            area->vsu_filter_mode = win_data->vsuFilterMode;
            area->vsd_filter_mode = win_data->vsdFilterMode;
            area->hsd_pre_filter_mode = win_data->hsdPreFilterMode;
            area->vsd_pre_filter_mode = win_data->vsdPreFilterMode;
            area->possible_crtcs = win->possible_crtcs;

            area->vop2 = vop2;
            area->win_id = i;
            area->phys_id = win->phys_id;
            area->area_id = j + 1;
            area->plane_id = plane_id++;
            rt_snprintf(name, HAL_MIN(sizeof(name) - 3, strlen(win->name)), "%s", win->name);
            rt_snprintf(name, sizeof(name) + sizeof(area->area_id), "%s%d", name, area->area_id);
            area->name = strdup(name);
            num_wins++;
        }
    }

    vop2->registered_num_wins = num_wins;

    if (!is_vop3(vop2))
    {
        for (i = 0; i < vop2_data->nrLayers; i++)
        {
            layer = &vop2->layers[i];
            layer_data = &vop2_data->layer[i];
            layer->id = layer_data->id;
        }
    }

    return 0;
}

static void vop2_irq_handle(int irq, void *param)
{
    rt_base_t level;
    uint32_t val;

    /* disable interrupt */
    level = rt_hw_interrupt_disable();
    val = HAL_VOP2_IrqHandler(NULL);
    if (val & VOP2_SYS_PORT1_INTR_EN_INTR_EN_FS_FIELD_MASK)
        rt_event_send(gptr_vop2->frm_st_event, true);

    /* enable interrupt */
    rt_hw_interrupt_enable(level);
}

static int vop2_create_isr_thread(struct vop2 *vop2)
{
    vop2->frm_st_event = rt_event_create("frm_fsh_event", RT_IPC_FLAG_FIFO);

    rt_hw_interrupt_install(VOP2_IRQn, vop2_irq_handle, RT_NULL, RT_NULL);
    rt_hw_interrupt_umask(VOP2_IRQn);

    return EOK;
}

/**
 * @brief  Alloc wfd device, corresponds to the libdss command DSS_SRV_ALLOC_WFDDEV.
 * @param  vop2: vop2 driver private structure.
 * @param  dev_data: wfd device data structure.
 */
static int vop2_wfd_alloc_device(struct vop2 *vop2, struct wfd_dev_data *dev_data)
{
    struct wfd_device *dev = &vop2->wfd_dev;
    int err;

    if (dev->wfd_dev_id)
    {
        DEBUG1("%s wfd_dev: %p is aready initialized\n", __func__, dev);
        goto out;
    }

    dev->wfd_dev_id = dev_data->wfd_dev_id;

    // mutex initialisation
    {
        pthread_mutexattr_t attr;
        bool init_attr;

        err = pthread_mutexattr_init(&attr);
        init_attr = (err == EOK);

#ifndef NDEBUG
        // Use error-checking mutexes in debug builds,
        // for the assert_device_(un)locked macros.
        if (!err)
        {
            err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        }
#endif

        if (!err)
        {
            err = pthread_mutex_init(&dev->lock, &attr);
            if (!err)
                dev->init_lock = true;
        }

        if (init_attr)
        {
            (void)pthread_mutexattr_destroy(&attr);
        }
        if (err)
        {
            slogf(SLOGC_SELF, _SLOG_ERROR, "wfd_dev mutex init failed: %s", strerror(err));
            return err;
        }
    }

    err = wfddevice_cfg_create(&dev->device_cfg, dev->wfd_dev_id, NULL /*opts*/);
    if (err)
        slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed: %s", "wfddevice_cfg_create", strerror(err));

    rt_kprintf("%s dev: %p handle: %ld\n", __func__, dev, dev_data->handle);

out:
    return 0;
}

/**
 * @brief  Alloc wfd port, corresponds to the libdss command DSS_SRV_ALLOC_WFDPORT.
 * @param  vop2: vop2 driver private structure.
 * @param  port_data: wfd port data structure.
 */
static void vop2_wfd_alloc_port(struct vop2 *vop2, struct wfd_port_data *port_data)
{
    struct wfd_device *dev = &vop2->wfd_dev;
    struct drm_crtc_state *cstate;
    struct rockchip_crtc_state *vcstate;
    struct vop2_video_port *vp;
    struct wfd_port *port = NULL, *last_port = NULL;
    const struct wfd_keyval *keyval;
    uint32_t output_if = 0;
    uint32_t vp_id = port_data->vp_id;
    unsigned index = port_data->index;
    int err;

    vp = &vop2->vps[vp_id];
    vp->id = vp_id;
    vp->vop2 = vop2;
    port = &vp->wfd_port;

    if (port->wfd_port_id)
    {
        DEBUG1("%s wfd_port%d is aready initialized\n", __func__, port->wfd_port_id);
        return;
    }

    port->handle = WFD_INVALID_HANDLE;
    port->ref = REFCOUNTER_INITIALIZER;
    port->dev = dev;
    port->wfd_port_id = port_data->wfd_port_id;
    port->index = port_data->index;
    port->pending.mode = dss_calloc(1, sizeof(struct wfd_port_mode));
    cstate = vop2_crtc_duplicate_state(&vp->rockchip_crtc.crtc);
    vp->rockchip_crtc.crtc.state = cstate;
    vcstate = to_rockchip_crtc_state(cstate);

    vop2_wfd_pthread_mutext_init(port);
    vop2_wfd_pthread_cond_init(port);

    // Make sure this port wasn't already created,
    // and find the last pipe in the list.
    {
        struct wfd_port *tmp;
        SLIST_FOREACH(tmp, &dev->port_list, dev_portlist_entry)
        {
            assert(tmp->wfd_port_id != port->wfd_port_id);
            if (tmp->index >= index)
            {
                index = (uint8_t)tmp->index;
                ++index;
            }
            last_port = tmp;
        }
    }

    if (last_port)
    {
        SLIST_INSERT_AFTER(last_port, port, dev_portlist_entry);
    }
    else
    {
        SLIST_INSERT_HEAD(&dev->port_list, port, dev_portlist_entry);
    }

    err = wfdport_cfg_create(&port->port_cfg, dev->device_cfg, port->wfd_port_id, NULL);
    if (err)
    {
        slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed (port id %d): %s",
              "wfdport_cfg_create", port->wfd_port_id, strerror(err));
        return;
    }

    keyval = wfdport_cfg_get_extension(port->port_cfg, WFD_OPT_OUTPUT_IF);
    if (keyval)
    {
        if (keyval->i)
        {
            output_if = (uint32_t)keyval->i;

            if (keyval->p)
            {
                vp->num_connectors = (uint32_t)hweight32(output_if);
                vp->connectors = keyval->p;

                for (uint32_t i = 0; i < vp->num_connectors; i++)
                {
                    struct connector *connector = &vp->connectors[i];

                    connector->port = port;

                    if (connector->panel)
                    {
                        connector->bus_format = connector->panel->bus_format;
                        connector->bpc = connector->panel->bpc;
                    }

                    if (vp->num_connectors > 1)
                        connector->split_mode = true;
                }
            }

            vcstate->output_if = output_if;
            vop2_video_port_init(vp);
        }
    }

    rt_kprintf("create vp%ld: %p output_if:0x%lx\n", vp->id, vp, vcstate->output_if);
}

/**
 * @brief  Get the display mode of port, corresponds to the libdss command DSS_SRV_GET_PORTMODE.
 * @param  vop2: vop2 driver private structure.
 * @param  mode_data: wfd port display mode data structure.
 */
void vop2_wfd_get_port_modes(struct vop2 *vop2, struct wfd_mode_data *mode_data)
{
    struct vop2_video_port *vp;
    struct wfd_port *port;
    struct wfdmode_cfg_list *cfglib_list = NULL;
    const struct wfdmode_timing *vm = NULL;
    struct vop2_connector_state *conn_state;
    int vp_id = mode_data->wfd_port_id - 1;
    int err = EOK;
    int count_modes = 0;

    vp = &vop2->vps[vp_id];
    port = &vp->wfd_port;
    conn_state = &vp->conn_state;

    err = wfdmode_cfg_list_create(&cfglib_list, port->port_cfg, NULL /*opts*/);
    if (err)
    {
        slogf(SLOGC_SELF, _SLOG_ERROR, "%s failed (port id %d): %s", "wfdmode_cfg_list_create", port->wfd_port_id, strerror(err));
        return;
    }

    while ((vm = wfdmode_cfg_list_get_next(cfglib_list, vm)))
    {
        memcpy(&mode_data->vm, vm, sizeof(*vm));
        if (conn_state->output_flags & ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE)
            wfdmode_timing_convert_to_split_mode(&mode_data->vm);
        count_modes++;
        slogf(SLOGC_SELF, _SLOG_ERROR, "vp%d mode%d: %d x %d", vp->id, count_modes, vm->hpixels, vm->vlines);
    }

    mode_data->count_modes = count_modes;

    wfdmode_cfg_list_destroy(cfglib_list);
}

/**
 * @brief  Enable wfd port mode setting, corresponds to the libdss command DSS_SRV_MODESET_ENABLE.
 * @param  vop2: vop2 driver private structure.
 * @param  modeset_data: wfd port mode setting data structure.
 */
void vop2_wfd_modeset_enable(struct vop2 *vop2, struct wfd_modeset_data *modeset_data)
{
    struct vop2_video_port *vp;
    struct wfd_port *port;
    int vp_id = modeset_data->wfd_port_id - 1;

    vp = &vop2->vps[vp_id];
    port = &vp->wfd_port;
    port->pending.mode->timing = modeset_data->timging;

    DEBUG2("%s VP%d %dx%d\n", __func__, vp->id, port->pending.mode->timing.hpixels, port->pending.mode->timing.vlines);
    vop2_connector_pre_enable(port);
    wfd_vop2_crtc_enable(port);
    vop2_connector_enable(port);
}

/**
 * @brief  Update wfd port configurations, corresponds to the libdss command DSS_SRV_PORT_COMMIT.
 * @param  vop2: vop2 driver private structure.
 * @param  commit_data: wfd port commit data structure.
 */
void vop2_wfd_port_flush(struct vop2 *vop2, const struct wfd_commit_data *commit_data)
{
    struct vop2_video_port *vp;
    struct vop2_video_port *splice_vp = NULL;
    struct vop2_zpos *vop2_zpos = NULL;
    struct vop2_zpos *vop2_zpos_splice = NULL;
    struct vop2_cluster cluster;
    const struct VOP2_DATA *vop2_data = vop2->data;
    const struct VOP2_VIDEO_PORT_DATA *vp_data;
    struct drm_crtc *crtc;
    struct drm_crtc_state *cstate;
    struct rockchip_crtc_state *vcstate;
    struct rockchip_crtc_state *splice_vcstate;
    struct CRTC_STATE *crtc_state;
    struct CRTC_STATE *splice_crtc_state = NULL;
    struct vop2_win *win;
    struct drm_plane *plane;
    struct drm_plane_state *pstate;
    struct vop2_plane_state *vpstate;
    int vp_id = commit_data->wfd_port_id - 1;
    uint64_t win_mask;
    uint32_t phys_id;
    uint64_t bit;
    uint8_t nr_layers = 0;
    uint8_t splice_nr_layers = 0;
    uint32_t i;

    vp = &vop2->vps[vp_id];
    vp_data = &vop2_data->vp[vp->id];
    crtc = &vp->rockchip_crtc.crtc;
    cstate = crtc->state;
    vcstate = to_rockchip_crtc_state(cstate);
    crtc_state = &vcstate->crtc_state;
    vcstate->commit_data = *commit_data;

    win_mask = vp->win_mask;

    vop2_zpos = dss_calloc(vop2->data->winSize, sizeof(*vop2_zpos));
    if (!vop2_zpos)
    {
        rt_kprintf("out of memory for vop2_zpos\n");
        return;
    }

    if (vcstate->splice_mode)
    {
        vop2_zpos_splice = dss_calloc(vop2->data->winSize, sizeof(*vop2_zpos));
        if (!vop2_zpos_splice)
            goto out;
        splice_vp = &vop2->vps[vp_data->spliceVpId];
        splice_vcstate = to_rockchip_crtc_state(&vp->rockchip_crtc.crtc.state);
        splice_crtc_state = &splice_vcstate->crtc_state;
    }

    for_each_set_bit(phys_id, (const unsigned long *)&win_mask, (uint64_t)ROCKCHIP_MAX_LAYER)
    {
        win = vop2_find_win_by_phys_id(vop2, phys_id);
        plane = &win->base;
        pstate = plane->state;
        if (!pstate)
            continue;
        vpstate = to_vop2_plane_state(pstate);
        bit = (uint64_t)1 << win->wfd_pipe_index;
        if (commit_data->pipe_idx_mask & bit)
        {
            vop2_zpos[nr_layers].win_phys_id = win->phys_id;
            vop2_zpos[nr_layers].zpos = vpstate->zpos;
            vop2_zpos[nr_layers].plane = plane;
            nr_layers++;
            DEBUG1("%s zorder: %d for vp%d\n", win->name, vpstate->zpos, vp->id);
        }
    }

    DEBUG1("vp%ld %ld windows, active layers %d\n", vp->id, (uint32_t)hweight_long(vp->win_mask), nr_layers);
    if (nr_layers)
    {
        vp->nr_layers = nr_layers;

        qsort(vop2_zpos, nr_layers, sizeof(vop2_zpos[0]), vop2_zpos_cmp);

        vop2_setup_port_mux(vp);
        vop2_setup_alpha(vp, vop2_zpos);
        HAL_VOP2_SetupDlyForVp(crtc_state);

        if (vcstate->splice_mode)
        {
            /* Fixme for VOP3 8K */
            splice_vp->nr_layers = splice_nr_layers;

            qsort(vop2_zpos_splice, splice_nr_layers, sizeof(vop2_zpos_splice[0]), vop2_zpos_cmp);

            vop2_setup_port_mux(splice_vp);
            vop2_setup_alpha(splice_vp, vop2_zpos_splice);
            HAL_VOP2_SetupDlyForVp(splice_crtc_state);
        }
    }
    else
    {
        vop2_calc_bg_ovl_and_port_mux(vp);
        HAL_VOP2_SetupDlyForVp(crtc_state);
        if (vcstate->splice_mode)
            HAL_VOP2_SetupDlyForVp(splice_crtc_state);
    }

    for (i = 0; i < nr_layers; i++)
    {
        plane = vop2_zpos[i].plane;
        struct vop2_win *win = to_vop2_win(plane);
        struct vop2_win *splice_win;

        if (!(win->feature & WIN_FEATURE_CLUSTER_MAIN))
            continue;

        if (win->two_win_mode)
            continue;

        cluster.main = win;
        cluster.sub = NULL;
        cluster.splice_mode = false;
        vop2_setup_cluster_alpha(vop2, &cluster);
        if (vcstate->splice_mode)
        {
            splice_win = win->splice_win;
            cluster.main = splice_win;
            cluster.splice_mode = true;
            vop2_setup_cluster_alpha(vop2, &cluster);
        }
    }

    for (i = 0; i < nr_layers; i++)
    {
        plane = vop2_zpos[i].plane;
        vop2_plane_destory_state(plane);
    }

    if (vcstate->splice_mode)
        dss_free(vop2_zpos_splice);
out:
    dss_free(vop2_zpos);
}

/**
 * @brief  Disable wfd port, corresponds to the libdss command DSS_SRV_DISABLE_WFDPORT.
 * @param  vop2: vop2 driver private structure.
 * @param  port_data: wfd port data structure.
 */
void wfd_vop2_wfd_port_disable(struct vop2 *vop2, struct wfd_port_data *port_data)
{
    uint32_t vp_id = port_data->wfd_port_id - 1;
    struct vop2_video_port *vp = &vop2->vps[vp_id];
    struct wfd_port *port = &vp->wfd_port;

    DEBUG1("%s vp%d\n", __func__, vp->id);

    wfd_vop2_crtc_disable(port);
}

/**
 * @brief  Set wfd port plane mask, corresponds to the libdss command DSS_SRV_SET_PLANEMASK.
 * @param  vop2: vop2 driver private structure.
 * @param  plane_mask_data: wfd port plane mask data structure.
 */
uint32_t vop2_wfd_set_plane_mask(struct vop2 *vop2, struct wfd_plane_mask_data *plane_mask_data)
{
    struct wfd_device *dev = &vop2->wfd_dev;
    struct wfd_port *port;
    struct vop2_video_port *vp;

    SLIST_FOREACH(port, &dev->port_list, dev_portlist_entry)
    {
        vp = wfd_port_to_vp(port);
        if (vp->id <= plane_mask_data->nr_ports)
            vp->plane_mask = plane_mask_data->plane_mask[vp->id];
        rt_kprintf("create vp%ld: plane_mask: 0x%lx\n", vp->id, vp->plane_mask);
    }

    return 0;
}

/**
 * @brief  Alloc wfd pipline, corresponds to the libdss command DSS_SRV_ALLOC_WFDPIPE.
 * @param  vop2: vop2 driver private structure.
 * @param  pipe_data: wfd pipeline data structure.
 */
void vop2_wfd_alloc_pipe(struct vop2 *vop2, struct wfd_pipe_data *pipe_data)
{
    struct vop2_win *win;
    uint32_t phys_id = (uint32_t)pipe_data->wfd_pipe_id - 1;

    win = vop2_find_win_by_phys_id(vop2, phys_id);
    win->wfd_pipe_index = pipe_data->index;
    strncpy(pipe_data->name, win->name, sizeof(pipe_data->name));
}

/**
 * @brief  Update wfd pipline configurations, corresponds to the libdss command DSS_SRV_PLANE_UPDATE.
 * @param  vop2: vop2 driver private structure.
 * @param  wfd_pstate: wfd pipeline status structure.
 */
void vop2_wfd_plane_update(struct vop2 *vop2, struct wfd_plane_state *wfd_pstate)
{
    struct vop2_video_port *vp;
    struct vop2_win *win;
    struct drm_plane *plane;
    struct drm_plane_state *pstate;
    struct vop2_plane_state *vpstate;
    struct drm_framebuffer *fb;
    struct drm_rect *dst;
    struct drm_rect *src;
    struct wfd_port *port;
    uint32_t dst_width, src_width;
    uint32_t phys_id = (uint32_t)wfd_pstate->wfd_pipe_id - 1;
    int vp_id = wfd_pstate->wfd_port_id - 1;

    vp = &vop2->vps[vp_id];
    port = &vp->wfd_port;
    port->period_ns = wfd_pstate->period_ns;
    port->start_ts = wfd_pstate->start_ts;
    port->end_ts = wfd_pstate->end_ts;

    win = vop2_find_win_by_phys_id(vop2, phys_id);
    win->wfd_pstate = *wfd_pstate;
    plane = &win->base;

    src_width = drm_rect_width(&wfd_pstate->src);
    dst_width = drm_rect_width(&wfd_pstate->dst);

    if (!dst_width || !src_width)
        return vop2_win_disable(win, false);

    vpstate = vop2_plane_duplicate_state(plane);
    if (!vpstate)
        return;

    plane->state = &vpstate->base;

    fb = dss_calloc(1, sizeof(*fb));
    if (!fb)
    {
        rt_kprintf("out of memory for fb");
        vop2_plane_destory_state(plane);
        return;
    }

    pstate = &vpstate->base;
    pstate->fb = fb;
    pstate->plane = plane;
    plane->state->crtc = &vp->rockchip_crtc.crtc;
    src = &vpstate->src;
    dst = &vpstate->dst;

    vpstate->yrgb_mst = wfd_pstate->yrgb_mst;
    vpstate->uv_mst = wfd_pstate->uv_mst;
    vpstate->fb_size = wfd_pstate->fb_size;
    fb->width = wfd_pstate->fb.width;
    fb->height = wfd_pstate->fb.height;
    fb->pitches[0] = wfd_pstate->fb.pitches[0];
    fb->pitches[1] = wfd_pstate->fb.pitches[1];

    fb->format =  wfd_format_to_drm_format(wfd_pstate->wfd_format);

    src->x1 = wfd_pstate->src.x1;
    src->y1 = wfd_pstate->src.y1;
    src->x2 = wfd_pstate->src.x2;
    src->y2 = wfd_pstate->src.y2;

    dst->x1 = wfd_pstate->dst.x1;
    dst->y1 = wfd_pstate->dst.y1;
    dst->x2 = wfd_pstate->dst.x2;
    dst->y2 = wfd_pstate->dst.y2;
    vpstate->zpos = wfd_pstate->zorder;
    vpstate->global_alpha = wfd_pstate->global_alpha;
    vpstate->blend_mode = DRM_MODE_BLEND_PIXEL_NONE;
    vpstate->format = vop2_convert_format(fb->format->format);
    vop2_plane_atomic_update(plane, pstate);
}

/**
 * @brief  Wait for the wfd display vsync, corresponds to the libdss command DSS_SRV_WAIT_VBLANK.
 * @param  vop2: vop2 driver private structure.
 * @param  port_data: wfd port data structure.
 */
void vop2_wfd_wait_vsync(struct vop2 *vop2, struct wfd_port_data *port_data)
{
    uint32_t vp_id = port_data->wfd_port_id - 1;
    struct vop2_video_port *vp = &vop2->vps[vp_id];
    struct wfd_port *port = &vp->wfd_port;
    uint32_t event = false;
    rt_err_t ret;

    DEBUG1("now_ticks = %ld, period_ns = %lld\n", rt_tick_get_millisecond(), port->period_ns);
    ret = rt_event_recv(gptr_vop2->frm_st_event, true,
                        RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                        port->period_ns / 1000, &event);
    if (!event)
        rt_kprintf("wait vsync-vp%ld timeout, ret = %ld\n", vp->id, ret);
}

/**
 * @brief  Dump the vop2 regs for debugging, corresponds to the libdss command DSS_SRV_DUMP_REGS.
 * @param  cmd: the mode for the function of vop2 regs dump: full dump or active dump.
 */
void vop2_wfd_dump_regs(bool is_full_dump)
{
    HAL_VOP2_DumpRegs(is_full_dump);
}

/** @} */

/********************* Public Function Definition ****************************/
/** @defgroup VOP2_Exported_Functions_Group2 get VOP Status and Errors Functions
 *  @{

 This section provides functions allowing to get VOP2 status:

 *  @{
 */

/**
 * @brief  Control vop2 device.
 * @param  fd: vop2 driver private structure.
 * @param  port_data: wfd port data structure.
 */
int devctl(int fd, int dcmd, void *data_ptr, size_t nbytes, void *flags)
{
    struct vop2 *vop2 = gptr_vop2;
    struct wfd_dev_data *dev_data;
    struct wfd_port_data *port_data;
    struct wfd_mode_data *mode_data;
    struct wfd_plane_state *wfd_pstate;
    struct wfd_modeset_data *modeset_data;
    struct wfd_pipe_data *pipe_data;
    struct wfd_plane_mask_data *plane_mask_data;
    struct wfd_commit_data *commit_data;
    bool data;

    switch (dcmd)
    {
    case DSS_SRV_ALLOC_WFDDEV:
        dev_data = (struct wfd_dev_data *)data_ptr;
        vop2_wfd_alloc_device(vop2, dev_data);
        dev_data->debug_mask = vop2->debug_mask;
        break;
    case DSS_SRV_ALLOC_WFDPORT:
        port_data = (struct wfd_port_data *)data_ptr;
        vop2_wfd_alloc_port(vop2, port_data);
        break;
    case DSS_SRV_GET_PORTMODE:
        mode_data = (struct wfd_mode_data *)data_ptr;
        vop2_wfd_get_port_modes(vop2, mode_data);
        nbytes = sizeof(*mode_data);
        break;
    case DSS_SRV_MODESET_ENABLE:
        modeset_data = (struct wfd_modeset_data *)data_ptr;
        vop2_wfd_modeset_enable(vop2, modeset_data);
        break;
    case DSS_SRV_PORT_COMMIT:
        commit_data = (struct wfd_commit_data *)data_ptr;
        vop2_wfd_port_flush(vop2, commit_data);
        break;
    case DSS_SRV_DISABLE_WFDPORT:
        port_data = (struct wfd_port_data *)data_ptr;
        wfd_vop2_wfd_port_disable(vop2, port_data);
        break;
    case DSS_SRV_SET_PLANEMASK:
        plane_mask_data = (struct wfd_plane_mask_data *)data_ptr;
        vop2_wfd_set_plane_mask(vop2, plane_mask_data);
        break;
    case DSS_SRV_ALLOC_WFDPIPE:
        pipe_data = (struct wfd_pipe_data *)data_ptr;
        vop2_wfd_alloc_pipe(vop2, pipe_data);
        break;
    case DSS_SRV_PLANE_UPDATE:
        wfd_pstate = (struct wfd_plane_state *)data_ptr;
        vop2_wfd_plane_update(vop2, wfd_pstate);
        break;
    case DSS_SRV_WAIT_VBLANK:
        port_data = (struct wfd_port_data *)data_ptr;
        vop2_wfd_wait_vsync(vop2, port_data);
        break;
    case DSS_SRV_DUMP_REGS:
        data = *(bool *)data_ptr;
        vop2_wfd_dump_regs(data);
        break;
    default:
        rt_kprintf("unsupported wfd command 0x%x \n", dcmd);
        break;
    }

    return 0;
}

/**
 * @brief  Initialize vop2 driver.
 */
int vop2_init(void)
{
    struct VOP2_DATA *vop2_data;
    struct vop2 *vop2;
    size_t alloc_size;
    size_t num_wins = 0;
    unsigned int i;

    DEBUG1("vop2 init......\n");

    vop2_data = dss_calloc(1, sizeof(*vop2_data));
    if (!vop2_data)
    {
        rt_kprintf("failed to alloc for vop2 data\n");
        return EXIT_FAILURE;
    }
    HAL_VOP2_GetPlatformData(vop2_data);

    for (i = 0; i < vop2_data->winSize; i++)
        num_wins += 1;

    /* Allocate vop2 struct and its vop2_win array */
    alloc_size = sizeof(*vop2) + sizeof(*vop2->win) * num_wins;

    vop2 = dss_calloc(1, alloc_size);
    if (!vop2)
    {
        rt_kprintf("failed to alloc for vop2\n");
        return EXIT_FAILURE;
    }
    vop2->debug_mask = dss_debug_level_mask;
    vop2->data = vop2_data;
    vop2->version = vop2_data->version;
    vop2->irq = vop2_data->irq;

    vop2_pd_data_init(vop2);
    vop2_win_init(vop2);
    vop2_dsc_data_init(vop2);

    vop2_create_isr_thread(vop2);

    gptr_vop2 = vop2;

    return 0;
}
INIT_DEVICE_EXPORT(vop2_init);
/** @} */

#endif

/** @} */

/** @} */
