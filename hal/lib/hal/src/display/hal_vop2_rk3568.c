/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023-2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(SOC_RK3568) && defined(HAL_VOP2_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup VOP2
 *  @{
 */

/** @defgroup VOP2_How_To_Use How To Use
 *  @{

 The VOP2 driver can be used as follows:

 @} */

#include "hal_display.h"
#include "hal_vop2.h"
#include "hal_math.h"

/** @defgroup VOP2_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define DATA_SWAP_MASK 0x1F
#define DSP_BG_SWAP    0x1
#define DSP_RB_SWAP    0x2
#define DSP_RG_SWAP    0x4
#define DSP_DELTA_SWAP 0x8

#define DCLK_DIV2_SHIFT 4
#define DCLK_DIV2_MASK  0x3

#define VOP2_BILI_SCL_DN(src, dst) (((src - 1) << 12) / (dst - 1))
#define VOP2_COMMON_SCL(src, dst)  (((src - 1) << 16) / (dst - 1))

#define VOP2_BILI_SCL_FAC_CHECK(src, dst, fac) \
                (fac * (dst - 1) >> 12 < (src - 1))
#define VOP2_COMMON_SCL_FAC_CHECK(src, dst, fac) \
                (fac * (dst - 1) >> 16 < (src - 1))

#define YRGB_XSCL_MODE_MASK         0x3
#define YRGB_XSCL_MODE_SHIFT        0
#define YRGB_XSCL_FILTER_MODE_MASK  0x3
#define YRGB_XSCL_FILTER_MODE_SHIFT 2
#define YRGB_YSCL_MODE_MASK         0x3
#define YRGB_YSCL_MODE_SHIFT        4
#define YRGB_YSCL_FILTER_MODE_MASK  0x3
#define YRGB_YSCL_FILTER_MODE_SHIFT 6

#define CLUSTER_AXI_ID_MASK  0x1
#define CLUSTER_AXI_ID_SHIFT 13

#define ESMART_AXI_ID_MASK  0x1
#define ESMART_AXI_ID_SHIFT 1

#define RK3568_PLANE_MASK_BASE                                           \
    (HAL_BIT(ROCKCHIP_VOP2_CLUSTER0) | HAL_BIT(ROCKCHIP_VOP2_CLUSTER1) | \
     HAL_BIT(ROCKCHIP_VOP2_ESMART0)  | HAL_BIT(ROCKCHIP_VOP2_ESMART1)  | \
     HAL_BIT(ROCKCHIP_VOP2_SMART0)   | HAL_BIT(ROCKCHIP_VOP2_SMART1))

/********************* Private Variable Definition ***************************/
static struct VOP2_SYS_REG sVop2SysMir;
static struct VOP2_OVERLAY_REG sVop2OverlayMir;
static struct VOP2_POST0_DSP_REG sVop2Post0Mir;
static struct VOP2_POST1_DSP_REG sVop2Post1Mir;
static struct VOP2_POST2_DSP_REG sVop2Post2Mir;
static struct VOP2_CLUSTER_REG sVop2ClusterMir[2];
static struct VOP2_ESMART_REG sVop2EsmartMir[2];
static struct VOP2_SMART_REG sVop2SmartMir[2];

static const char * const VOP2_IRQs[] =
{
    "frame start interrupt status!",
    "new frame start interrupt status!",
    "line flag0 interrupt status!",
    "line flag1 interrupt status!",
    "post buffer empty interrupt status!"
    "field start interrupt status!",
    "display hold valid interrupt status!"
};

static const struct VOP2_VIDEO_PORT_DATA rk3568VopVideoPorts[] = {
    {
        .id = 0,
        .socId = { 0x3568, 0x3566 },
        .feature = VOP_FEATURE_OUTPUT_10BIT | VOP_FEATURE_ALPHA_SCALE |
                   VOP_FEATURE_HDR10 | VOP_FEATURE_OVERSCAN,
        .gammaLutLen = 1024,
        .cubicLutLen = 729, /* 9x9x9 */
        .maxOutput = { 4096, 2304 },
        .preScanMaxDly = { 42, 42, 42, 42 },
    },
    {
        .id = 1,
        .socId = { 0x3568, 0x3566 },
        .feature = VOP_FEATURE_ALPHA_SCALE | VOP_FEATURE_OVERSCAN,
        .gammaLutLen = 1024,
        .maxOutput = { 2048, 1536 },
        .preScanMaxDly = { 40, 40, 40, 40 },
    },
    {
        .id = 2,
        .feature = VOP_FEATURE_ALPHA_SCALE | VOP_FEATURE_OVERSCAN,
        .socId = { 0x3568, 0x3566 },
        .gammaLutLen = 1024,
        .maxOutput = { 1920, 1080 },
        .preScanMaxDly = { 40, 40, 40, 40 },
    },
};

static const struct VOP2_LAYER_DATA rk3568VopLayers[] = {
    {
        .id = 0,
    },

    {
        .id = 1,
    },

    {
        .id = 2,
    },

    {
        .id = 3,
    },

    {
        .id = 4,
    },

    {
        .id = 5,
    },

    {
        .id = 6,
    },

    {
        .id = 7,
    },
};

static const eDISPLAY_dataFormat formatsForSmart[] = {
    ROCKCHIP_FMT_ARGB8888,
    ROCKCHIP_FMT_RGB888,
    ROCKCHIP_FMT_RGB565,
};

/* RK356x can't support uv swap for YUYV and UYVY */
static const eDISPLAY_dataFormat formatsForRk356xEsmart[] = {
    ROCKCHIP_FMT_ARGB8888,
    ROCKCHIP_FMT_RGB888,
    ROCKCHIP_FMT_RGB565,
    ROCKCHIP_FMT_YUV420SP,
    ROCKCHIP_FMT_YUV422SP,
    ROCKCHIP_FMT_YUV444SP,
};

static const eDISPLAY_dataFormat formatsForCluster[] = {
    ROCKCHIP_FMT_ARGB8888,
    ROCKCHIP_FMT_RGB888,
    ROCKCHIP_FMT_RGB565,
    ROCKCHIP_FMT_YUV420SP,
    ROCKCHIP_FMT_YUV422SP,
    ROCKCHIP_FMT_YUV444SP,
};

static const struct VOP2_WIN_DATA rk3568VopWinData[] = {
    {
        .name = "Smart0-win0",
        .physId = ROCKCHIP_VOP2_SMART0,
        .base = 0x400,
        .formats = formatsForSmart,
        .nformats = HAL_ARRAY_SIZE(formatsForSmart),
        .layerSelId = { 3, 3, 3, 0xff },
        .winSelPortOffset = 6,
        .hsuFilterMode = VOP2_SCALE_UP_BIC,
        .hsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .vsuFilterMode = VOP2_SCALE_UP_BIL,
        .vsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .type = DISPLAY_PLANE_TYPE_PRIMARY,
        .maxUpscaleFactor = 8,
        .maxDownscaleFactor = 8,
        .dly = { 20, 47, 41 },
        .feature = WIN_FEATURE_MULTI_AREA,
    },

    {
        .name = "Smart1-win0",
        .physId = ROCKCHIP_VOP2_SMART1,
        .formats = formatsForSmart,
        .nformats = HAL_ARRAY_SIZE(formatsForSmart),
        .base = 0x600,
        .layerSelId = { 7, 7, 7, 0xff },
        .winSelPortOffset = 7,
        .hsuFilterMode = VOP2_SCALE_UP_BIC,
        .hsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .vsuFilterMode = VOP2_SCALE_UP_BIL,
        .vsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .type = DISPLAY_PLANE_TYPE_PRIMARY,
        .maxUpscaleFactor = 8,
        .maxDownscaleFactor = 8,
        .dly = { 20, 47, 41 },
        .feature = WIN_FEATURE_MIRROR | WIN_FEATURE_MULTI_AREA,
    },

    {
        .name = "Esmart1-win0",
        .physId = ROCKCHIP_VOP2_ESMART1,
        .formats = formatsForRk356xEsmart,
        .nformats = HAL_ARRAY_SIZE(formatsForRk356xEsmart),
        .base = 0x200,
        .layerSelId = { 6, 6, 6, 0xff },
        .winSelPortOffset = 5,
        .hsuFilterMode = VOP2_SCALE_UP_BIC,
        .hsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .vsuFilterMode = VOP2_SCALE_UP_BIL,
        .vsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .type = DISPLAY_PLANE_TYPE_PRIMARY,
        .maxUpscaleFactor = 8,
        .maxDownscaleFactor = 8,
        .dly = { 20, 47, 41 },
        .feature = WIN_FEATURE_MIRROR | WIN_FEATURE_MULTI_AREA,
    },

    {
        .name = "Esmart0-win0",
        .physId = ROCKCHIP_VOP2_ESMART0,
        .formats = formatsForRk356xEsmart,
        .nformats = HAL_ARRAY_SIZE(formatsForRk356xEsmart),
        .base = 0x0,
        .layerSelId = { 2, 2, 2, 0xff },
        .winSelPortOffset = 4,
        .hsuFilterMode = VOP2_SCALE_UP_BIC,
        .hsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .vsuFilterMode = VOP2_SCALE_UP_BIL,
        .vsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .type = DISPLAY_PLANE_TYPE_OVERLAY,
        .maxUpscaleFactor = 8,
        .maxDownscaleFactor = 8,
        .dly = { 20, 47, 41 },
        .feature = WIN_FEATURE_MULTI_AREA,
    },

    {
        .name = "Cluster0-win0",
        .physId = ROCKCHIP_VOP2_CLUSTER0,
        .base = 0x00,
        .formats = formatsForCluster,
        .nformats = HAL_ARRAY_SIZE(formatsForCluster),
        .layerSelId = { 0, 0, 0, 0xff },
        .winSelPortOffset = 0,
        .hsuFilterMode = VOP2_SCALE_UP_BIC,
        .hsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .vsuFilterMode = VOP2_SCALE_UP_BIL,
        .vsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .maxUpscaleFactor = 4,
        .maxDownscaleFactor = 4,
        .dly = { 0, 27, 21 },
        .type = DISPLAY_PLANE_TYPE_OVERLAY,
        .feature = WIN_FEATURE_AFBDC | WIN_FEATURE_CLUSTER_MAIN,
    },

    {
        .name = "Cluster0-win1",
        .physId = ROCKCHIP_VOP2_CLUSTER0,
        .base = 0x80,
        .layerSelId = { 0xff, 0xff, 0xff, 0xff },
        .formats = formatsForCluster,
        .nformats = HAL_ARRAY_SIZE(formatsForCluster),
        .hsuFilterMode = VOP2_SCALE_UP_BIC,
        .hsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .vsuFilterMode = VOP2_SCALE_UP_BIL,
        .vsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .maxUpscaleFactor = 4,
        .maxDownscaleFactor = 4,
        .type = DISPLAY_PLANE_TYPE_OVERLAY,
        .feature = WIN_FEATURE_AFBDC | WIN_FEATURE_CLUSTER_SUB,
    },

    {
        .name = "Cluster1-win0",
        .physId = ROCKCHIP_VOP2_CLUSTER1,
        .base = 0x00,
        .formats = formatsForCluster,
        .nformats = HAL_ARRAY_SIZE(formatsForCluster),
        .layerSelId = { 1, 1, 1, 0xff },
        .winSelPortOffset = 1,
        .hsuFilterMode = VOP2_SCALE_UP_BIC,
        .hsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .vsuFilterMode = VOP2_SCALE_UP_BIL,
        .vsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .type = DISPLAY_PLANE_TYPE_OVERLAY,
        .maxUpscaleFactor = 4,
        .maxDownscaleFactor = 4,
        .dly = { 0, 27, 21 },
        .feature = WIN_FEATURE_AFBDC | WIN_FEATURE_CLUSTER_MAIN | WIN_FEATURE_MIRROR,
    },

    {
        .name = "Cluster1-win1",
        .physId = ROCKCHIP_VOP2_CLUSTER1,
        .layerSelId = { 0xff, 0xff, 0xff, 0xff },
        .formats = formatsForCluster,
        .nformats = HAL_ARRAY_SIZE(formatsForCluster),
        .base = 0x80,
        .hsuFilterMode = VOP2_SCALE_UP_BIC,
        .hsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .vsuFilterMode = VOP2_SCALE_UP_BIL,
        .vsdFilterMode = VOP2_SCALE_DOWN_BIL,
        .type = DISPLAY_PLANE_TYPE_OVERLAY,
        .maxUpscaleFactor = 4,
        .maxDownscaleFactor = 4,
        .feature = WIN_FEATURE_AFBDC | WIN_FEATURE_CLUSTER_SUB | WIN_FEATURE_MIRROR,
    },
};

static const struct VOP2_VP_PLANE_MASK rk356x_vpPlaneMask[VOP2_VP_MAX][VOP2_VP_MAX] = {
    { /* one display policy */
        {/* main display */
            .primaryPlaneId = ROCKCHIP_VOP2_SMART0,
            .attachedLayersNr = 6,
            .attachedLayers = {
                ROCKCHIP_VOP2_CLUSTER0, ROCKCHIP_VOP2_ESMART0, ROCKCHIP_VOP2_SMART0,
                ROCKCHIP_VOP2_CLUSTER1, ROCKCHIP_VOP2_ESMART1, ROCKCHIP_VOP2_SMART1
            },
        },
        { /* second display */ },
        { /* third  display */ },
        { /* fourth display */ },
    },

    { /* two display policy */
        {/* main display */
            .primaryPlaneId = ROCKCHIP_VOP2_SMART0,
            .attachedLayersNr = 3,
            .attachedLayers = {
                ROCKCHIP_VOP2_CLUSTER0, ROCKCHIP_VOP2_ESMART0, ROCKCHIP_VOP2_SMART0
            },
        },

        {/* second display */
            .primaryPlaneId = ROCKCHIP_VOP2_SMART1,
            .attachedLayersNr = 3,
            .attachedLayers = {
                ROCKCHIP_VOP2_CLUSTER1, ROCKCHIP_VOP2_ESMART1, ROCKCHIP_VOP2_SMART1
            },
        },
        { /* third  display */ },
        { /* fourth display */ },
    },

    { /* three display policy */
        {/* main display */
            .primaryPlaneId = ROCKCHIP_VOP2_SMART0,
            .attachedLayersNr = 3,
            .attachedLayers = {
                ROCKCHIP_VOP2_CLUSTER0, ROCKCHIP_VOP2_ESMART0, ROCKCHIP_VOP2_SMART0
            },
        },

        {/* second display */
            .primaryPlaneId = ROCKCHIP_VOP2_SMART1,
            .attachedLayersNr = 2,
            .attachedLayers = {
                ROCKCHIP_VOP2_CLUSTER1, ROCKCHIP_VOP2_SMART1
            },
        },

        {/* third  display */
            .primaryPlaneId = ROCKCHIP_VOP2_ESMART1,
            .attachedLayersNr = 1,
            .attachedLayers = { ROCKCHIP_VOP2_ESMART1 },
        },

        { /* fourth display */ },
    },

    { /* reserved for four display policy */ },
};

static struct VOP_DUMP_REGS rk3568DumpRegs[] = {
    { VOP2_SYS_BASE, "SYS", 0, 0, 0, 0, 0 },
    { VOP2_OVERLAY_BASE, "OVL", 0, 0, 0, 0, 0 },
    { VOP2_POST0_DSP_BASE, "VP0", 0x0, 0x1, 31, 0, 0 },
    { VOP2_POST1_DSP_BASE, "VP1", 0x0, 0x1, 31, 0, 0 },
    { VOP2_POST2_DSP_BASE, "VP2", 0x0, 0x1, 31, 0, 0 },
    { VOP2_CLUSTER0_BASE, "Cluster0", 0x0, 0x1, 0, 1, 0 },
    { VOP2_CLUSTER1_BASE, "Cluster1", 0x0, 0x1, 0, 1, 0 },
    { VOP2_ESMART0_BASE, "Esmart0", 0x10, 0x1, 0, 1, 0 },
    { VOP2_ESMART1_BASE, "Esmart1", 0x10, 0x1, 0, 1, 0 },
    { VOP2_SMART0_BASE, "Smart0", 0x10, 0x1, 0, 1, 0 },
    { VOP2_SMART1_BASE, "Smart1", 0x10, 0x1, 0, 1, 0 },
    { VOP2_HDR10_BASE, "HDR", 0, 0, 0, 0, 0 },
};

static const struct VOP2_DATA sRK3568Vop = {
    .version = VOP_VERSION_RK3568,
    .nrVps = 3,
    .nrMixers = 5,
    .nrLayers = 6,
    .nrGammas = 1,
    .maxInput = { 4096, 2304 },
    .maxOutput = { 4096, 2304 },
    .vp = rk3568VopVideoPorts,
    .layer = rk3568VopLayers,
    .win = rk3568VopWinData,
    .winSize = HAL_ARRAY_SIZE(rk3568VopWinData),
    .planeMask = rk356x_vpPlaneMask[0],
    .planeMaskBase = RK3568_PLANE_MASK_BASE,
    .dumpRegs = rk3568DumpRegs,
    .dumpRegsSize = HAL_ARRAY_SIZE(rk3568DumpRegs),
};

/********************* Private Function Definition ***************************/
HAL_UNUSED static uint32_t VOP2_Read(__IO uint32_t *hwReg)
{
#ifdef REGS_DEBUG
    HAL_DBG("{%s-%d}reg %08lx = %08lx\n", __func__, __LINE__, (long unsigned int)hwReg, READ_REG(*hwReg));
#endif

    return READ_REG(*hwReg);
}

HAL_UNUSED static uint32_t VOP2_MaskRead(__IO uint32_t *hwReg,
                                         uint32_t shift, uint32_t mask)
{
    return (READ_REG(*hwReg) & mask) >> shift;
}

HAL_UNUSED static void VOP2_MaskWriteNoBackup(__IO uint32_t *mirReg, __IO uint32_t *hwReg,
                                              uint32_t shift, uint32_t mask,
                                              uint32_t v)
{
    uint32_t mirVal = *mirReg;

    v = (mirVal & ~mask) | ((v << shift) & mask);

    WRITE_REG(*hwReg, v);
}

static void VOP2_MaskWrite(__IO uint32_t *mirReg, __IO uint32_t *hwReg,
                           uint32_t shift, uint32_t mask,
                           uint32_t v)
{
    if (mirReg) {
        uint32_t mirVal = *mirReg;

        v = (mirVal & ~mask) | ((v << shift) & mask);
        *mirReg = v;
    } else {
        v = ((v << shift) & mask) | (mask << 16);
    }

#ifdef REGS_DEBUG
    HAL_DBG("{%s-%d}reg %08lx = %08lx\n", __func__, __LINE__, (long unsigned int)hwReg, v);
#endif
    WRITE_REG(*hwReg, v);
}

static void VOP2_Write(__IO uint32_t *mirReg, __IO uint32_t *hwReg,
                       uint32_t v)
{
    if (mirReg) {
        *mirReg = v;
    }

#ifdef REGS_DEBUG
    HAL_DBG("{%s-%d}reg %08lx = %08lx\n", __func__, __LINE__, (long unsigned int)hwReg, v);
#endif
    WRITE_REG(*hwReg, v);
}

static void VOP2_GrfMaskWrite(__IO uint32_t *hwReg,
                              uint32_t shift, uint32_t mask,
                              uint32_t v)
{
    v = ((v << shift) & mask) | (mask << 16);

    WRITE_REG(*hwReg, v);
}

static void VOP2_CopyRegToMir(uint32_t *vopReg, uint32_t *regMir, uint32_t regLen)
{
    uint32_t i;

    for (i = 0; i < regLen; i++) {
        regMir[i] = vopReg[i];
    }
}

static void VOP2_RegsBackup(void)
{
    VOP2_CopyRegToMir((uint32_t *)VOP2_SYS, (uint32_t *)&sVop2SysMir, sizeof(*VOP2_SYS));
    VOP2_CopyRegToMir((uint32_t *)VOP2_OVERLAY, (uint32_t *)&sVop2OverlayMir, sizeof(*VOP2_OVERLAY));
    VOP2_CopyRegToMir((uint32_t *)VOP2_POST0_DSP, (uint32_t *)&sVop2Post0Mir, sizeof(*VOP2_POST0_DSP));
    VOP2_CopyRegToMir((uint32_t *)VOP2_POST1_DSP, (uint32_t *)&sVop2Post1Mir, sizeof(*VOP2_POST1_DSP));
    VOP2_CopyRegToMir((uint32_t *)VOP2_POST2_DSP, (uint32_t *)&sVop2Post2Mir, sizeof(*VOP2_POST2_DSP));
    VOP2_CopyRegToMir((uint32_t *)VOP2_CLUSTER0, (uint32_t *)&sVop2ClusterMir[0], sizeof(*VOP2_CLUSTER0));
    VOP2_CopyRegToMir((uint32_t *)VOP2_CLUSTER1, (uint32_t *)&sVop2ClusterMir[1], sizeof(*VOP2_CLUSTER1));
    VOP2_CopyRegToMir((uint32_t *)VOP2_ESMART0, (uint32_t *)&sVop2EsmartMir[0], sizeof(*VOP2_ESMART0));
    VOP2_CopyRegToMir((uint32_t *)VOP2_ESMART1, (uint32_t *)&sVop2EsmartMir[1], sizeof(*VOP2_ESMART1));
    VOP2_CopyRegToMir((uint32_t *)VOP2_SMART0, (uint32_t *)&sVop2SmartMir[0], sizeof(*VOP2_SMART0));
    VOP2_CopyRegToMir((uint32_t *)VOP2_SMART1, (uint32_t *)&sVop2SmartMir[1], sizeof(*VOP2_SMART1));
}

static struct VOP2_POST0_DSP_REG *VOP2_GetPostRegMir(int crtcId)
{
    if (crtcId == 0) {
        return (struct VOP2_POST0_DSP_REG *)&sVop2Post0Mir;
    } else if (crtcId == 1) {
        return (struct VOP2_POST0_DSP_REG *)&sVop2Post1Mir;
    } else {
        return (struct VOP2_POST0_DSP_REG *)&sVop2Post2Mir;
    }
}

static struct VOP2_POST0_DSP_REG *VOP2_GetPostReg(int crtcId)
{
    if (crtcId == 0) {
        return (struct VOP2_POST0_DSP_REG *)VOP2_POST0_DSP;
    } else if (crtcId == 1) {
        return (struct VOP2_POST0_DSP_REG *)VOP2_POST1_DSP;
    } else {
        return (struct VOP2_POST0_DSP_REG *)VOP2_POST2_DSP;
    }
}

static struct VOP2_CLUSTER_REG *VOP2_GetClusterRegMir(uint8_t physId)
{
    switch (physId) {
    case ROCKCHIP_VOP2_CLUSTER0:

        return (struct VOP2_CLUSTER_REG *)&sVop2ClusterMir[0];
    case ROCKCHIP_VOP2_CLUSTER1:

        return (struct VOP2_CLUSTER_REG *)&sVop2ClusterMir[1];
    }

    return NULL;
}

static struct VOP2_CLUSTER_REG *VOP2_GetClusterReg(uint8_t physId)
{
    switch (physId) {
    case ROCKCHIP_VOP2_CLUSTER0:

        return (struct VOP2_CLUSTER_REG *)&VOP2_CLUSTER0[0];
    case ROCKCHIP_VOP2_CLUSTER1:

        return (struct VOP2_CLUSTER_REG *)&VOP2_CLUSTER1[1];
    }

    return NULL;
}

static struct VOP2_ESMART_REG *VOP2_GetEsmartRegMir(uint8_t physId)
{
    switch (physId) {
    case ROCKCHIP_VOP2_ESMART0:

        return &sVop2EsmartMir[0];
    case ROCKCHIP_VOP2_ESMART1:

        return &sVop2EsmartMir[1];
    case ROCKCHIP_VOP2_SMART0:

        return (struct VOP2_ESMART_REG *)&sVop2SmartMir[0];
    case ROCKCHIP_VOP2_SMART1:

        return (struct VOP2_ESMART_REG *)&sVop2SmartMir[1];
    }

    return NULL;
}

static struct VOP2_ESMART_REG *VOP2_GetEsmartReg(uint8_t physId)
{
    switch (physId) {
    case ROCKCHIP_VOP2_ESMART0:

        return &VOP2_ESMART0[0];
    case ROCKCHIP_VOP2_ESMART1:

        return &VOP2_ESMART1[0];
    case ROCKCHIP_VOP2_SMART0:

        return (struct VOP2_ESMART_REG *)&VOP2_SMART0[0];
    case ROCKCHIP_VOP2_SMART1:

        return (struct VOP2_ESMART_REG *)&VOP2_SMART1[0];
    }

    return NULL;
}

static HAL_Check VOP2_IsClusterWin(uint8_t physId)
{
    if (physId == ROCKCHIP_VOP2_CLUSTER0 || physId == ROCKCHIP_VOP2_CLUSTER1) {
        return HAL_TRUE;
    } else {
        return HAL_FALSE;
    }
}

static HAL_Check VOP2_IsEsmartWin(uint8_t physId)
{
    if (physId == ROCKCHIP_VOP2_ESMART0 || physId == ROCKCHIP_VOP2_ESMART1 ||
        physId == ROCKCHIP_VOP2_SMART0 || physId == ROCKCHIP_VOP2_SMART1) {
        return HAL_TRUE;
    } else {
        return HAL_FALSE;
    }
}

static inline int VOP2_RectWidth(const struct WIN_RECT *r)
{
    return r->x2 - r->x1;
}

static inline int VOP2_RectHeight(const struct WIN_RECT *r)
{
    return r->y2 - r->y1;
}

static bool isUvSwap(uint32_t busFormat, uint32_t outputMode)
{
    /*
     * FIXME:
     *
     * There is no media type for YUV444 output,
     * so when out_mode is AAAA or P888, assume output is YUV444 on
     * yuv format.
     *
     * From H/W testing, YUV444 mode need a rb swap.
     */
    if (busFormat == MEDIA_BUS_FMT_YVYU8_1X16 ||
        busFormat == MEDIA_BUS_FMT_VYUY8_1X16 ||
        busFormat == MEDIA_BUS_FMT_YVYU8_2X8 ||
        busFormat == MEDIA_BUS_FMT_VYUY8_2X8 ||
        ((busFormat == MEDIA_BUS_FMT_YUV8_1X24 ||
          busFormat == MEDIA_BUS_FMT_YUV10_1X30) &&
         (outputMode == ROCKCHIP_OUT_MODE_AAAA ||
          outputMode == ROCKCHIP_OUT_MODE_P888))) {
        return true;
    } else {
        return false;
    }
}

static bool isYuvOutput(uint32_t busFormat)
{
    switch (busFormat) {
    case MEDIA_BUS_FMT_YUV8_1X24:
    case MEDIA_BUS_FMT_YUV10_1X30:
    case MEDIA_BUS_FMT_YUYV10_1X20:
    case MEDIA_BUS_FMT_UYYVYY8_0_5X24:
    case MEDIA_BUS_FMT_UYYVYY10_0_5X30:
    case MEDIA_BUS_FMT_YUYV8_2X8:
    case MEDIA_BUS_FMT_YVYU8_2X8:
    case MEDIA_BUS_FMT_UYVY8_2X8:
    case MEDIA_BUS_FMT_VYUY8_2X8:
    case MEDIA_BUS_FMT_YUYV8_1X16:
    case MEDIA_BUS_FMT_YVYU8_1X16:
    case MEDIA_BUS_FMT_UYVY8_1X16:
    case MEDIA_BUS_FMT_VYUY8_1X16:

        return true;
    default:

        return false;
    }
}

static bool VOP2_IsOuputYcSwap(uint32_t busFormat)
{
    switch (busFormat) {
    case MEDIA_BUS_FMT_YUYV8_1X16:
    case MEDIA_BUS_FMT_YVYU8_1X16:
    case MEDIA_BUS_FMT_YUYV8_2X8:
    case MEDIA_BUS_FMT_YVYU8_2X8:
        return true;
    default:
        return false;
    }
}

static inline uint32_t scl_cal_scale(uint32_t src, uint32_t dst, uint32_t shift)
{
    return ((src * 2 - 3) << (shift - 1)) / (dst - 1);
}

static inline uint32_t scl_cal_scale2(uint32_t src, uint32_t dst)
{
    return ((src - 1) << 12) / (dst - 1);
}

static void VOP2_EnableDclk(uint32_t vpId)
{
    switch (vpId) {
    case 0:
        HAL_CRU_ClkEnable(DCLK0_VOP_GATE);
        break;
    case 1:
        HAL_CRU_ClkEnable(DCLK1_VOP_GATE);
        break;
    case 2:
        HAL_CRU_ClkEnable(DCLK2_VOP_GATE);
        break;
    default:
        break;
    }
}

static void VOP2_DisableDclk(uint32_t vpId)
{
    switch (vpId) {
    case 0:
        HAL_CRU_ClkDisable(DCLK0_VOP_GATE);
        break;
    case 1:
        HAL_CRU_ClkDisable(DCLK1_VOP_GATE);
        break;
    case 2:
        HAL_CRU_ClkDisable(DCLK2_VOP_GATE);
        break;
    default:
        break;
    }
}

static uint32_t VOP2_GetDclk(uint32_t vpId)
{
    uint32_t retRate;

    switch (vpId) {
    case 0:
        retRate = HAL_CRU_ClkGetFreq(DCLK_VOP0);
        break;
    case 1:
        retRate = HAL_CRU_ClkGetFreq(DCLK_VOP1);
        break;
    case 2:
        retRate = HAL_CRU_ClkGetFreq(DCLK_VOP2);
        break;
    default:
        retRate = 0;
        break;
    }

    return retRate;
}

static uint32_t VOP2_SetDclk(uint32_t vpId, uint32_t rate)
{
    int ret;
    uint32_t retRate;

    switch (vpId) {
    case 0:
        ret = HAL_CRU_ClkSetFreq(DCLK_VOP0, rate);
        retRate = HAL_CRU_ClkGetFreq(DCLK_VOP0);
        break;
    case 1:
        ret = HAL_CRU_ClkSetFreq(DCLK_VOP1, rate);
        retRate = HAL_CRU_ClkGetFreq(DCLK_VOP1);
        break;
    case 2:
        ret = HAL_CRU_ClkSetFreq(DCLK_VOP2, rate);
        retRate = HAL_CRU_ClkGetFreq(DCLK_VOP2);
        break;
    default:
        ret = -1;
        retRate = 0;
        break;
    }

    if (ret < 0) {
        HAL_DBG_ERR("dclk_vp%ld SetFreq failed: %d\n", vpId, ret);
    }

    return retRate;
}

static inline eVOP2_scaleMode VOP2_GetSclMode(int src, int dst)
{
    if (src < dst) {
        return SCALE_UP;
    } else if (src > dst) {
        return SCALE_DOWN;
    }

    return SCALE_NONE;
}

static uint16_t VOP2_ScaleFactor(eVOP2_scaleMode mode,
                                 int32_t filterMode,
                                 uint32_t src, uint32_t dst)
{
    uint32_t fac = 0;
    int i = 0;

    if (mode == SCALE_NONE) {
        return HAL_OK;
    }

    /*
     * A workaround to avoid zero div.
     */
    if ((dst == 1) || (src == 1)) {
        dst = dst + 1;
        src = src + 1;
    }

    if ((mode == SCALE_DOWN) && (filterMode == VOP2_SCALE_DOWN_BIL)) {
        fac = VOP2_BILI_SCL_DN(src, dst);
        for (i = 0; i < 100; i++) {
            if (VOP2_BILI_SCL_FAC_CHECK(src, dst, fac)) {
                break;
            }
            fac -= 1;
            HAL_DBG("down fac cali: src:%ld, dst:%ld, fac:0x%lx\n", src, dst, fac);
        }
    } else {
        fac = VOP2_COMMON_SCL(src, dst);
        for (i = 0; i < 100; i++) {
            if (VOP2_COMMON_SCL_FAC_CHECK(src, dst, fac)) {
                break;
            }
            fac -= 1;
            HAL_DBG("up fac cali:  src:%ld, dst:%ld, fac:0x%lx\n", src, dst, fac);
        }
    }

    return fac;
}

static bool VOP2_WinDitherUp(uint32_t format)
{
    switch (format) {
    case ROCKCHIP_FMT_RGB565:

        return true;
    default:

        return false;
    }
}

static const char *VOP2_GetPlaneName(int planeId)
{
    switch (planeId) {
    case ROCKCHIP_VOP2_CLUSTER0:
        return "Cluster0";
    case ROCKCHIP_VOP2_CLUSTER1:
        return "Cluster1";
    case ROCKCHIP_VOP2_ESMART0:
        return "Esmart0";
    case ROCKCHIP_VOP2_ESMART1:
        return "Esmart1";
    case ROCKCHIP_VOP2_SMART0:
        return "Smart0";
    case ROCKCHIP_VOP2_SMART1:
        return "Smart1";
    default:
        return "Unknown";
    }
}

/*
 * if adjusted mode update, return true, else return false
 */
static bool VOP2_CrtcModeUpdate(struct CRTC_STATE *pCrtcState)
{
    struct DISPLAY_MODE_INFO *mode = &pCrtcState->adjustedMode;
    uint32_t hsyncLen = mode->crtcHsyncEnd - mode->crtcHsyncStart;
    uint32_t hdisplay = mode->crtcHdisplay;
    uint32_t htotal = mode->crtcHtotal;
    uint32_t hactSt = mode->crtcHtotal - mode->crtcHsyncStart;
    uint32_t hactEnd = hactSt + hdisplay;
    uint32_t vdisplay = mode->crtcVdisplay;
    uint32_t vtotal = mode->crtcVtotal;
    uint32_t vsyncLen = mode->crtcVsyncEnd - mode->crtcVsyncStart;
    uint32_t vactSt = mode->crtcVtotal - mode->crtcVsyncStart;
    uint32_t vactEnd = vactSt + vdisplay;

    uint32_t htotalSync = htotal << 16 | hsyncLen;
    uint32_t hactiveStEnd = hactSt << 16 | hactEnd;
    uint32_t vtotalSync = vtotal << 16 | vsyncLen;
    uint32_t vactiveStEnd = vactSt << 16 | vactEnd;
    uint32_t crtcClock = mode->crtcClock * 100;

    if (htotalSync != VOP2_Read(&VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_HTOTAL_HS_END) ||
        hactiveStEnd != VOP2_Read(&VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_HACT_ST_END) ||
        vtotalSync != VOP2_Read(&VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_VTOTAL_VS_END) ||
        vactiveStEnd != VOP2_Read(&VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_VACT_ST_END) ||
        crtcClock != VOP2_GetDclk(pCrtcState->vpId)) {
        return true;
    }

    return false;
}

static const struct VOP2_WIN_DATA *VOP2_FindWinDataByPhysId(uint8_t physId)
{
    const struct VOP2_WIN_DATA *winData;
    uint32_t i;

    for (i = 0; i < sRK3568Vop.winSize; i++) {
        winData = &sRK3568Vop.win[i];
        if (winData->physId == physId) {
            return winData;
        }
    }

    return NULL;
}

static void VOP2_DisableAllPlanesForCrtc(struct CRTC_STATE *pCrtcState)
{
    const struct VOP2_WIN_DATA *winData;
    uint32_t i;
    uint32_t waitDisableWinPhyIds[VOP2_LAYER_MAX];
    uint32_t waitDisableWinNum = 0;
    uint32_t cfgDone = VOP2_SYS_SYS_REG_CFG_DONE_SW_GLOBAL_REGDONE_EN_MASK |
                       HAL_BIT(pCrtcState->vpId) | (HAL_BIT(pCrtcState->vpId) << 16);

    for (i = 0; i < pCrtcState->bindingWinNum; i++) {
        winData = VOP2_FindWinDataByPhysId(pCrtcState->bindingWinPhyIds[i]);
        if (winData) {
            if (HAL_VOP2_GetWinStatus(pCrtcState->bindingWinPhyIds[i]) &&
                !HAL_VOP2_DisableWin(pCrtcState->bindingWinPhyIds[i])) {
                waitDisableWinPhyIds[waitDisableWinNum] = pCrtcState->bindingWinPhyIds[i];
                waitDisableWinNum++;
            }
        }
    }

    if (waitDisableWinNum) {
        VOP2_Write(&sVop2SysMir.SYS_REG_CFG_DONE,
                   &VOP2_SYS->SYS_REG_CFG_DONE, cfgDone);
        HAL_DelayMs(100);
        for (i = 0; i < waitDisableWinNum; i++) {
            winData = VOP2_FindWinDataByPhysId(waitDisableWinPhyIds[i]);
            if (HAL_VOP2_GetWinStatus(pCrtcState->bindingWinPhyIds[i])) {
                HAL_DBG_ERR("failed to disable win: %s\n", winData->name);
            }
        }
    }
}

static void VOP2_PostConfig(struct CRTC_STATE *pCrtcState)
{
    const struct VOP2_VIDEO_PORT_DATA *vpData = &rk3568VopVideoPorts[pCrtcState->vpId];
    struct DISPLAY_MODE_INFO *mode = &pCrtcState->adjustedMode;
    uint16_t vtotal = mode->crtcVtotal;
    uint16_t hactSt = mode->crtcHtotal - mode->crtcHsyncStart;
    uint16_t vactSt = mode->crtcVtotal - mode->crtcVsyncStart;
    uint16_t hdisplay = mode->crtcHdisplay;
    uint16_t vdisplay = mode->crtcVdisplay;
    uint16_t hsize = hdisplay;
    uint16_t vsize = vdisplay;
    uint16_t hactEnd, vact_end;
    uint32_t val;

    hsize = HAL_ROUND_DOWN(hsize, 2);
    vsize = HAL_ROUND_DOWN(vsize, 2);

    hactSt += 0;
    hactEnd = hactSt + hsize;
    val = hactSt << 16;
    val |= hactEnd;
    VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_HACT_INFO,
               &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_HACT_INFO,
               val);
    vactSt += 0;
    vact_end = vactSt + vsize;
    val = vactSt << 16;
    val |= vact_end;
    VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_VACT_INFO,
               &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_VACT_INFO,
               val);
    val = scl_cal_scale2(vdisplay, vsize) << 16;
    val |= scl_cal_scale2(hdisplay, hsize);
    VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_SCL_FACTOR_YRGB,
               &VOP2_GetPostReg(pCrtcState->vpId)->POST_SCL_FACTOR_YRGB,
               val);
#define POST_HORIZONTAL_SCALEDOWN_EN(x) ((x) << 0)
#define POST_VERTICAL_SCALEDOWN_EN(x)   ((x) << 1)
    VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_SCL_CTRL,
               &VOP2_GetPostReg(pCrtcState->vpId)->POST_SCL_CTRL,
               POST_HORIZONTAL_SCALEDOWN_EN(hdisplay != hsize) |
               POST_VERTICAL_SCALEDOWN_EN(vdisplay != vsize));
    if (mode->flags & VIDEO_MODE_FLAG_INTERLACE) {
        uint16_t vactStF1 = vtotal + vactSt + 1;
        uint16_t vactEndF1 = vactStF1 + vsize;

        val = vactStF1 << 16 | vactEndF1;
        VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_VACT_INFO_F1,
                   &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_VACT_INFO_F1,
                   val);
    }

    /*
     * BCSH[R2Y] -> POST Linebuffer[post scale] -> the background R2Y will be deal by post_dsp_out_r2y
     *
     * POST Linebuffer[post scale] -> ACM[R2Y] -> the background R2Y will be deal by ACM[R2Y]
     */
    if (vpData->feature & VOP_FEATURE_POST_ACM) {
        VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_WIN_BYPASS_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_WIN_BYPASS_MASK, pCrtcState->yuvOverlay);
    } else {
        VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_WIN_BYPASS_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_WIN_BYPASS_MASK, isYuvOutput(pCrtcState->busFormat));
    }
}

static void VOP2_DitherSetup(struct CRTC_STATE *pCrtcState)
{
    uint32_t crtcId = pCrtcState->vpId;
    bool preDitherDownEn = false;

    switch (pCrtcState->busFormat) {
    case MEDIA_BUS_FMT_RGB565_1X16:
    case MEDIA_BUS_FMT_RGB565_2X8_LE:
        VOP2_MaskWrite(&VOP2_GetPostRegMir(crtcId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(crtcId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_MASK, true);
        VOP2_MaskWrite(&VOP2_GetPostRegMir(crtcId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(crtcId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DITHER_DOWN_MODE_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DITHER_DOWN_MODE_MASK, RGB888_TO_RGB565);
        preDitherDownEn = true;
        break;
    case MEDIA_BUS_FMT_RGB666_1X18:
    case MEDIA_BUS_FMT_RGB666_1X24_CPADHI:
    case MEDIA_BUS_FMT_RGB666_1X7X3_SPWG:
    case MEDIA_BUS_FMT_RGB666_3X6:
        VOP2_MaskWrite(&VOP2_GetPostRegMir(crtcId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(crtcId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_MASK, true);
        VOP2_MaskWrite(&VOP2_GetPostRegMir(crtcId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(crtcId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DITHER_DOWN_MODE_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DITHER_DOWN_MODE_MASK, RGB888_TO_RGB666);
        preDitherDownEn = true;
        break;
    case MEDIA_BUS_FMT_YUYV8_1X16:
    case MEDIA_BUS_FMT_YUV8_1X24:
    case MEDIA_BUS_FMT_UYYVYY8_0_5X24:
        VOP2_MaskWrite(&VOP2_GetPostRegMir(crtcId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(crtcId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_MASK, false);
        preDitherDownEn = true;
        break;
    case MEDIA_BUS_FMT_YUYV10_1X20:
    case MEDIA_BUS_FMT_YUV10_1X30:
    case MEDIA_BUS_FMT_UYYVYY10_0_5X30:
    case MEDIA_BUS_FMT_RGB101010_1X30:
        VOP2_MaskWrite(&VOP2_GetPostRegMir(crtcId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(crtcId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_MASK, false);
        preDitherDownEn = false;
        break;
    case MEDIA_BUS_FMT_RGB888_3X8:
    case MEDIA_BUS_FMT_RGB888_DUMMY_4X8:
    case MEDIA_BUS_FMT_RGB888_1X24:
    case MEDIA_BUS_FMT_RGB888_1X7X4_SPWG:
    case MEDIA_BUS_FMT_RGB888_1X7X4_JEIDA:
    default:
        VOP2_MaskWrite(&VOP2_GetPostRegMir(crtcId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(crtcId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_MASK, false);
        preDitherDownEn = true;
        break;
    }

    VOP2_MaskWrite(&VOP2_GetPostRegMir(crtcId)->POST_DSP_CTRL,
                   &VOP2_GetPostReg(crtcId)->POST_DSP_CTRL,
                   VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_SHIFT,
                   VOP2_POST0_DSP_POST_DSP_CTRL_PRE_DITHER_DOWN_EN_MASK, preDitherDownEn);
    VOP2_MaskWrite(&VOP2_GetPostRegMir(crtcId)->POST_DSP_CTRL,
                   &VOP2_GetPostReg(crtcId)->POST_DSP_CTRL,
                   VOP2_POST0_DSP_POST_DSP_CTRL_DITHER_DOWN_SEL_SHIFT,
                   VOP2_POST0_DSP_POST_DSP_CTRL_DITHER_DOWN_SEL_MASK, DITHER_DOWN_ALLEGRO);
}

static void VOP2_PostColorSwap(struct CRTC_STATE *pCrtcState)
{
    uint32_t dataSwap = 0;

    if (isUvSwap(pCrtcState->busFormat, pCrtcState->outputMode)) {
        dataSwap = DSP_RB_SWAP;
    }

    VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                   &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                   VOP2_POST0_DSP_POST_DSP_CTRL_DSP_BG_SWAP_SHIFT,
                   DATA_SWAP_MASK, dataSwap);
}

static void VOP2_CfgUpdate(struct CRTC_STATE *pCrtcState)
{
    const struct VOP2_VIDEO_PORT_DATA *vpData = &rk3568VopVideoPorts[pCrtcState->vpId];
    uint32_t val;
    uint32_t r, g, b;
    int outMode;

    if ((pCrtcState->outputMode == ROCKCHIP_OUT_MODE_AAAA &&
         !(vpData->feature & VOP_FEATURE_OUTPUT_10BIT)) ||
        pCrtcState->outputIf & VOP_OUTPUT_IF_BT656) {
        outMode = ROCKCHIP_OUT_MODE_P888;
    } else {
        outMode = pCrtcState->outputMode;
    }
    VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                   &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                   VOP2_POST0_DSP_POST_DSP_CTRL_DSP_OUT_MODE_SHIFT,
                   VOP2_POST0_DSP_POST_DSP_CTRL_DSP_OUT_MODE_MASK, outMode);

    VOP2_PostColorSwap(pCrtcState);

    VOP2_DitherSetup(pCrtcState);

    VOP2_MaskWrite(&sVop2OverlayMir.OVERLAY_CTRL, &VOP2_OVERLAY->OVERLAY_CTRL,
                   VOP2_OVERLAY_OVERLAY_CTRL_VP0_OVERLAY_MODE_SHIFT + pCrtcState->vpId,
                   VOP2_OVERLAY_OVERLAY_CTRL_VP0_OVERLAY_MODE_MASK, pCrtcState->yuvOverlay);
    /*
     * userspace specified background.
     */
    if (pCrtcState->background) {
        r = (pCrtcState->background & 0xff0000) >> 16;
        g = (pCrtcState->background & 0xff00) >> 8;
        b = (pCrtcState->background & 0xff);
        r <<= 2;
        g <<= 2;
        b <<= 2;
        val = (r << 20) | (g << 10) | b;
    } else {
        if (pCrtcState->yuvOverlay) {
            val = 0x20010200;
        } else {
            val = 0;
        }
    }
    VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_BG,
               &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_BG,
               val);
}

static void VOP2_CrtcVblankEnable(struct CRTC_STATE *pCrtcState)
{
    if (pCrtcState->vpId == 0) {
        VOP2_MaskWrite(NULL, &VOP2_SYS->PORT0_INTR_CLR,
                       VOP2_SYS_PORT0_INTR_CLR_INTR_CLR_FS_FIELD_SHIFT,
                       VOP2_SYS_PORT0_INTR_CLR_INTR_CLR_FS_FIELD_MASK, 1);
        VOP2_MaskWrite(NULL, &VOP2_SYS->PORT0_INTR_EN,
                       VOP2_SYS_PORT0_INTR_EN_INTR_EN_FS_FIELD_SHIFT,
                       VOP2_SYS_PORT0_INTR_EN_INTR_EN_FS_FIELD_MASK, 1);
    } else if (pCrtcState->vpId == 1) {
        VOP2_MaskWrite(NULL, &VOP2_SYS->PORT1_INTR_CLR,
                       VOP2_SYS_PORT1_INTR_CLR_INTR_CLR_FS_FIELD_SHIFT,
                       VOP2_SYS_PORT1_INTR_CLR_INTR_CLR_FS_FIELD_MASK, 1);
        VOP2_MaskWrite(NULL, &VOP2_SYS->PORT1_INTR_EN,
                       VOP2_SYS_PORT1_INTR_EN_INTR_EN_FS_FIELD_SHIFT,
                       VOP2_SYS_PORT1_INTR_EN_INTR_EN_FS_FIELD_MASK, 1);
    } else {
        VOP2_MaskWrite(NULL, &VOP2_SYS->PORT2_INTR_CLR,
                       VOP2_SYS_PORT2_INTR_CLR_INTR_CLR_FS_FIELD_SHIFT,
                       VOP2_SYS_PORT2_INTR_CLR_INTR_CLR_FS_FIELD_MASK, 1);
        VOP2_MaskWrite(NULL, &VOP2_SYS->PORT2_INTR_EN,
                       VOP2_SYS_PORT2_INTR_EN_INTR_EN_FS_FIELD_SHIFT,
                       VOP2_SYS_PORT2_INTR_EN_INTR_EN_FS_FIELD_MASK, 1);
    }
}

static void VOP2_DspHoldValidIrqEnable(struct CRTC_STATE *pCrtcState)
{
    if (pCrtcState->vpId == 0) {
        VOP2_MaskWrite(NULL, &VOP2_SYS->PORT0_INTR_CLR,
                       VOP2_SYS_PORT0_INTR_CLR_INTR_CLR_DSP_HOLD_VALID_SHIFT,
                       VOP2_SYS_PORT0_INTR_CLR_INTR_CLR_DSP_HOLD_VALID_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.PORT0_INTR_EN, &VOP2_SYS->PORT0_INTR_EN,
                       VOP2_SYS_PORT0_INTR_EN_INTR_EN_DSP_HOLD_VALID_SHIFT,
                       VOP2_SYS_PORT0_INTR_EN_INTR_EN_DSP_HOLD_VALID_MASK, 1);
    } else if (pCrtcState->vpId == 1) {
        VOP2_MaskWrite(NULL, &VOP2_SYS->PORT1_INTR_CLR,
                       VOP2_SYS_PORT1_INTR_CLR_INTR_CLR_DSP_HOLD_VALID_SHIFT,
                       VOP2_SYS_PORT1_INTR_CLR_INTR_CLR_DSP_HOLD_VALID_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.PORT1_INTR_EN, &VOP2_SYS->PORT1_INTR_EN,
                       VOP2_SYS_PORT1_INTR_EN_INTR_EN_DSP_HOLD_VALID_SHIFT,
                       VOP2_SYS_PORT1_INTR_EN_INTR_EN_DSP_HOLD_VALID_MASK, 1);
    } else {
        VOP2_MaskWrite(NULL, &VOP2_SYS->PORT2_INTR_CLR,
                       VOP2_SYS_PORT2_INTR_CLR_INTR_CLR_DSP_HOLD_VALID_SHIFT,
                       VOP2_SYS_PORT2_INTR_CLR_INTR_CLR_DSP_HOLD_VALID_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.PORT2_INTR_EN, &VOP2_SYS->PORT2_INTR_EN,
                       VOP2_SYS_PORT2_INTR_EN_INTR_EN_DSP_HOLD_VALID_SHIFT,
                       VOP2_SYS_PORT2_INTR_EN_INTR_EN_DSP_HOLD_VALID_MASK, 1);
    }
}

static void VOP2_DspHoldValidIrqDisable(struct CRTC_STATE *pCrtcState)
{
    if (pCrtcState->vpId == 0) {
        VOP2_MaskWrite(&sVop2SysMir.PORT0_INTR_EN, &VOP2_SYS->PORT0_INTR_EN,
                       VOP2_SYS_PORT0_INTR_EN_INTR_EN_DSP_HOLD_VALID_SHIFT,
                       VOP2_SYS_PORT0_INTR_EN_INTR_EN_DSP_HOLD_VALID_MASK, 0);
    } else if (pCrtcState->vpId == 1) {
        VOP2_MaskWrite(&sVop2SysMir.PORT1_INTR_EN, &VOP2_SYS->PORT1_INTR_EN,
                       VOP2_SYS_PORT1_INTR_EN_INTR_EN_DSP_HOLD_VALID_SHIFT,
                       VOP2_SYS_PORT1_INTR_EN_INTR_EN_DSP_HOLD_VALID_MASK, 0);
    } else {
        VOP2_MaskWrite(&sVop2SysMir.PORT2_INTR_EN, &VOP2_SYS->PORT2_INTR_EN,
                       VOP2_SYS_PORT2_INTR_EN_INTR_EN_DSP_HOLD_VALID_SHIFT,
                       VOP2_SYS_PORT2_INTR_EN_INTR_EN_DSP_HOLD_VALID_MASK, 0);
    }
}

static void VOP2_SetupScale(struct PLANE_STATE *pPlaneState, uint32_t srcW, uint32_t srcH,
                            uint32_t dstW, uint32_t dstH)
{
    const struct VOP2_WIN_DATA *winData = &rk3568VopWinData[pPlaneState->winId];
    uint16_t yrgbHorSclMode, yrgbVerSclMode;
    uint16_t hsclFilterMode, vsclFilterMode;
    uint8_t ygt2 = 0, ygt4 = 0;
    uint32_t xfac = 0, yfac = 0;

    if (winData->vsdFilterMode == VOP2_SCALE_DOWN_ZME) {
        if (srcH >= (8 * dstH)) {
            ygt4 = 1;
            srcH >>= 2;
        } else if (srcH >= (4 * dstH)) {
            ygt2 = 1;
            srcH >>= 1;
        }
    } else {
        if (srcH >= (4 * dstH)) {
            ygt4 = 1;
            srcH >>= 2;
        } else if (srcH >= (2 * dstH)) {
            ygt2 = 1;
            srcH >>= 1;
        }
    }

    yrgbHorSclMode = VOP2_GetSclMode(srcW, dstW);
    yrgbVerSclMode = VOP2_GetSclMode(srcH, dstH);

    if (yrgbHorSclMode == SCALE_UP) {
        hsclFilterMode = winData->hsuFilterMode;
    } else {
        hsclFilterMode = winData->hsdFilterMode;
    }

    if (yrgbVerSclMode == SCALE_UP) {
        vsclFilterMode = winData->vsuFilterMode;
    } else {
        vsclFilterMode = winData->vsdFilterMode;
    }

    /*
     * RK3568 VOP Esmart/Smart dspW should be even pixel
     * at scale down mode
     */
    if ((yrgbHorSclMode == SCALE_DOWN) && (dstW & 0x1)) {
        HAL_DBG_WRN("win dstW[%ld] should align as 2 pixel\n", dstW);
        dstW += 1;
    }

    xfac = VOP2_ScaleFactor(yrgbHorSclMode, hsclFilterMode, srcW, dstW);
    yfac = VOP2_ScaleFactor(yrgbVerSclMode, vsclFilterMode, srcH, dstH);

    if (winData->feature & WIN_FEATURE_CLUSTER_MAIN) {
        VOP2_Write(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_SCL_FACTOR_YRGB,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_SCL_FACTOR_YRGB,
                   yfac << 16 | xfac);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL1,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL1,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_HOR_SCL_MODE_SHIFT,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_HOR_SCL_MODE_MASK, yrgbHorSclMode);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL1,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL1,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_VER_SCL_MODE_SHIFT,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_VER_SCL_MODE_MASK, yrgbVerSclMode);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL1,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL1,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_VSD_GT2_SHIFT,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_VSD_GT2_MASK, 0);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL1,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL1,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_VSD_GT4_SHIFT,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_VSD_GT4_MASK, 0);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL1,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL1,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_VSU_MODE_SHIFT,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_VSU_MODE_MASK, ygt2);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL1,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL1,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_VSD_MODE_SHIFT,
                       VOP2_CLUSTER_WIN0_CTRL1_WIN0_YRGB_VSD_MODE_MASK, ygt4);
    } else {
        VOP2_Write(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_SCL_FACTOR_YRGB,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_SCL_FACTOR_YRGB,
                   yfac << 16 | xfac);
        VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_MST_CTL,
                       &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_MST_CTL,
                       VOP2_ESMART_REGION0_MST_CTL_REGION0_YRGB_GT2_SHIFT,
                       VOP2_ESMART_REGION0_MST_CTL_REGION0_YRGB_GT2_MASK, ygt2);
        VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_MST_CTL,
                       &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_MST_CTL,
                       VOP2_ESMART_REGION0_MST_CTL_REGION0_YRGB_GT4_SHIFT,
                       VOP2_ESMART_REGION0_MST_CTL_REGION0_YRGB_GT4_MASK, ygt4);
        VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_SCL_CTRL,
                       &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_SCL_CTRL,
                       YRGB_XSCL_MODE_SHIFT,
                       YRGB_XSCL_MODE_MASK, yrgbHorSclMode);
        VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_SCL_CTRL,
                       &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_SCL_CTRL,
                       YRGB_YSCL_MODE_SHIFT,
                       YRGB_YSCL_MODE_MASK, yrgbVerSclMode);
        VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_SCL_CTRL,
                       &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_SCL_CTRL,
                       YRGB_XSCL_FILTER_MODE_SHIFT,
                       YRGB_XSCL_FILTER_MODE_MASK, hsclFilterMode);
        VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_SCL_CTRL,
                       &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_SCL_CTRL,
                       YRGB_YSCL_FILTER_MODE_SHIFT,
                       YRGB_YSCL_FILTER_MODE_MASK, vsclFilterMode);
    }
}

static int VOP2_SetClusterWin(struct PLANE_STATE *pPlaneState, struct CRTC_STATE *pCrtcState)
{
    struct DISPLAY_MODE_INFO *adjustedMode = &pCrtcState->adjustedMode;
    struct WIN_RECT *src = &pPlaneState->src;
    struct WIN_RECT *dst = &pPlaneState->dst;
    int actualW, actualH, dspW, dspH;
    uint32_t actInfo, dspInfo, dspSt, dspStx, dspSty;
    uint32_t cfgDone = VOP2_SYS_SYS_REG_CFG_DONE_SW_GLOBAL_REGDONE_EN_MASK |
                       HAL_BIT(pCrtcState->vpId) | (HAL_BIT(pCrtcState->vpId) << 16);
    uint32_t afbcTileNum;
    uint32_t afbcXoffset;
    bool ditherUp;

    actualW = VOP2_RectWidth(src) >> 16;
    actualH = VOP2_RectHeight(src) >> 16;

    dspW = VOP2_RectWidth(dst);
    if (dst->x1 + dspW > (int)adjustedMode->hdisplay) {
        HAL_DBG_WRN("vp%d %s dest->x1[%d] + dspW[%d] exceed mode hdisplay[%d]\n",
                    pCrtcState->vpId, VOP2_GetPlaneName(pPlaneState->physId), dst->x1, dspW, adjustedMode->hdisplay);
        dspW = (int)adjustedMode->hdisplay - dst->x1;
        if (dspW < 4) {
            dspW = 4;
        }
        actualW = dspW * actualW / VOP2_RectWidth(dst);
    }
    dspH = VOP2_RectHeight(dst);
    if (dst->y1 + dspH > (int)adjustedMode->vdisplay) {
        HAL_DBG_WRN("vp%d %s dest->y1[%d] + dspH[%d] exceed mode vdisplay[%d]\n",
                    pCrtcState->vpId, VOP2_GetPlaneName(pPlaneState->physId), dst->y1, dspH, adjustedMode->vdisplay);
        dspH = (int)adjustedMode->vdisplay - dst->y1;
        if (dspH < 4) {
            dspH = 4;
        }
        actualH = dspH * actualH / VOP2_RectHeight(dst);
    }

    actInfo = (actualH - 1) << 16;
    actInfo |= (actualW - 1) & 0xffff;

    dspInfo = (dspH - 1) << 16;
    dspInfo |= (dspW - 1) & 0xffff;

    dspStx = dst->x1;
    dspSty = dst->y1;
    dspSt = dspSty << 16 | (dspStx & 0xffff);

    HAL_DBG("vp%d update %s[%dx%d->%dx%d@(%ld, %ld)] fmt[0x%08lx] addr[0x%08lx]\n",
            pCrtcState->vpId, VOP2_GetPlaneName(pPlaneState->physId),
            actualW, actualH, dspW, dspH,
            dspStx, dspSty,
            pPlaneState->format, pPlaneState->yrgbMst);

    if (pPlaneState->afbcEn) {
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->CLUSTER_CTRL,
                       &VOP2_GetClusterReg(pPlaneState->physId)->CLUSTER_CTRL,
                       VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_EN_SHIFT,
                       VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_EN_MASK, 1);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_CTRL,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_CTRL,
                       VOP2_CLUSTER_WIN0_AFBCD_CTRL_WIN0_AFBCD_PIXEL_PACKING_FMT_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_CTRL_WIN0_AFBCD_PIXEL_PACKING_FMT_MASK, pPlaneState->afbcFormat);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_CTRL,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_CTRL,
                       VOP2_CLUSTER_WIN0_AFBCD_CTRL_WIN0_AFBCD_RB_SWAP_EN_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_CTRL_WIN0_AFBCD_RB_SWAP_EN_MASK, pPlaneState->rbSwap);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_CTRL,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_CTRL,
                       VOP2_CLUSTER_WIN0_AFBCD_CTRL_WIN0_AFBCD_UV_SWAP_EN_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_CTRL_WIN0_AFBCD_UV_SWAP_EN_MASK, pPlaneState->uvSwap);

        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_MASK_EN,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_MASK_EN,
                       VOP2_CLUSTER_WIN0_AFBCD_MASK_EN_WIN0_AFBCD_GATING_EN_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_MASK_EN_WIN0_AFBCD_GATING_EN_MASK, 0);

        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_CTRL,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_CTRL,
                       VOP2_CLUSTER_WIN0_AFBCD_CTRL_WIN0_AFBCD_BLOCK_SPLIT_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_CTRL_WIN0_AFBCD_BLOCK_SPLIT_MASK, 0);

        VOP2_Write(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_HDR_PTR,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_HDR_PTR, pPlaneState->yrgbMst);
        VOP2_Write(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_SIZE,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_SIZE, actInfo);
        VOP2_Write(&VOP2_GetClusterRegMir(pPlaneState->physId)->RESERVED003C[0],
                   &VOP2_GetClusterReg(pPlaneState->physId)->RESERVED003C[0], pPlaneState->transformOffset);

        afbcXoffset = (uint32_t)(src->x1 >> 16);
        VOP2_Write(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_PIC_OFFSET,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_PIC_OFFSET, (afbcXoffset | (uint32_t)src->y1));
        VOP2_Write(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_DIS_OFFSET,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_DIS_OFFSET, (uint32_t)(dst->x1 | (dst->y1 << 16)));

        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_VIR_WIDTH,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_VIR_WIDTH,
                       VOP2_CLUSTER_WIN0_AFBCD_VIR_WIDTH_WIN0_AFBCD_PIC_VIR_WIDTH_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_VIR_WIDTH_WIN0_AFBCD_PIC_VIR_WIDTH_MASK, pPlaneState->afbcStride);

        afbcTileNum = (uint32_t)HAL_ALIGN(actualW, 16) >> 4;
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_VIR_WIDTH,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_VIR_WIDTH,
                       VOP2_CLUSTER_WIN0_AFBCD_VIR_WIDTH_WIN0_AFBCD_TAIL_NUM_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_VIR_WIDTH_WIN0_AFBCD_TAIL_NUM_MASK, afbcTileNum);

        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_MODE,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_MODE,
                       VOP2_CLUSTER_WIN0_AFBCD_MODE_WIN0_XMIR_EN_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_MODE_WIN0_XMIR_EN_MASK, pPlaneState->xmirrorEn);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_MODE,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_MODE,
                       VOP2_CLUSTER_WIN0_AFBCD_MODE_WIN0_YMIR_EN_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_MODE_WIN0_YMIR_EN_MASK, pPlaneState->ymirrorEn);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_MODE,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_MODE,
                       VOP2_CLUSTER_WIN0_AFBCD_MODE_WIN0_ROT270_EN_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_MODE_WIN0_ROT270_EN_MASK, pPlaneState->rotateEn270);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_MODE,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_MODE,
                       VOP2_CLUSTER_WIN0_AFBCD_MODE_WIN0_ROT90_EN_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_MODE_WIN0_ROT90_EN_MASK, pPlaneState->rotateEn90);

        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_AFBCD_CTRL,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_AFBCD_CTRL,
                       VOP2_CLUSTER_WIN0_AFBCD_MODE_WIN0_ROT90_EN_SHIFT,
                       VOP2_CLUSTER_WIN0_AFBCD_MODE_WIN0_ROT90_EN_MASK, pPlaneState->afbcHalfBlockEn);
    } else {
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->CLUSTER_CTRL,
                       &VOP2_GetClusterReg(pPlaneState->physId)->CLUSTER_CTRL,
                       VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_EN_SHIFT,
                       VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_EN_MASK, 0);
    }

    VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL0,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL0,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_DATA_FMT_SHIFT,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_DATA_FMT_MASK, pPlaneState->format);
    VOP2_Write(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_YRGB_MST,
               &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_YRGB_MST,
               pPlaneState->yrgbMst);
    VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_VIR,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_VIR,
                   VOP2_CLUSTER_WIN0_VIR_WIN0_VIR_STRIDE_SHIFT,
                   VOP2_CLUSTER_WIN0_VIR_WIN0_VIR_STRIDE_MASK, pPlaneState->uvStride);
    if (pPlaneState->isYuv) {
        VOP2_Write(&VOP2_GetClusterRegMir(pPlaneState->physId)->RESERVED0014,
                   &VOP2_GetClusterReg(pPlaneState->physId)->RESERVED0014,
                   pPlaneState->yrgbMst);
        VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_VIR,
                       &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_VIR,
                       VOP2_ESMART_REGION0_VIR_REGION0_VIR_STRIDE_UV_SHIFT,
                       VOP2_ESMART_REGION0_VIR_REGION0_VIR_STRIDE_UV_MASK, pPlaneState->uvStride);
    }

    VOP2_Write(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_ACT_INFO,
               &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_ACT_INFO,
               actInfo);
    VOP2_Write(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_DSP_INFO,
               &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_DSP_INFO,
               dspInfo);
    VOP2_Write(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_DSP_ST,
               &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_DSP_ST,
               dspSt);

    VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL0,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL0,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_CSC_R2Y_EN_SHIFT,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_CSC_R2Y_EN_MASK, pPlaneState->r2yEn);
    VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL0,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL0,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_CSC_Y2R_EN_SHIFT,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_CSC_Y2R_EN_MASK, pPlaneState->y2rEn);
    VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL0,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL0,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_CSC_MODE_SHIFT,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_CSC_MODE_MASK, pPlaneState->cscMode);

    ditherUp = VOP2_WinDitherUp(pPlaneState->format);
    VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL0,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL0,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_DITHER_UP_EN_SHIFT,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_DITHER_UP_EN_MASK, ditherUp);

    VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->WIN0_CTRL0,
                   &VOP2_GetClusterReg(pPlaneState->physId)->WIN0_CTRL0,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_EN_SHIFT,
                   VOP2_CLUSTER_WIN0_CTRL0_WIN0_EN_MASK, true);

    VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->CLUSTER_CTRL,
                   &VOP2_GetClusterReg(pPlaneState->physId)->CLUSTER_CTRL,
                   VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_LB_MODE_SHIFT,
                   VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_LB_MODE_MASK, pPlaneState->lbMode);
    VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->CLUSTER_CTRL,
                   &VOP2_GetClusterReg(pPlaneState->physId)->CLUSTER_CTRL,
                   VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_LB_MODE_SHIFT,
                   VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_LB_MODE_MASK, pPlaneState->lbMode == 1 ? 3 : 0);
    VOP2_MaskWrite(&VOP2_GetClusterRegMir(pPlaneState->physId)->CLUSTER_CTRL,
                   &VOP2_GetClusterReg(pPlaneState->physId)->CLUSTER_CTRL,
                   VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_EN_SHIFT,
                   VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_EN_MASK, true);

    VOP2_Write(&sVop2SysMir.SYS_REG_CFG_DONE,
               &VOP2_SYS->SYS_REG_CFG_DONE, cfgDone);

    return HAL_OK;
}

static int VOP2_SetSmartWin(struct PLANE_STATE *pPlaneState, struct CRTC_STATE *pCrtcState)
{
    struct DISPLAY_MODE_INFO *adjustedMode = &pCrtcState->adjustedMode;
    struct WIN_RECT *src = &pPlaneState->src;
    struct WIN_RECT *dst = &pPlaneState->dst;
    int actualW, actualH, dspW, dspH;
    uint32_t actInfo, dspInfo, dspSt, dspStx, dspSty;
    uint32_t cfgDone = VOP2_SYS_SYS_REG_CFG_DONE_SW_GLOBAL_REGDONE_EN_MASK |
                       HAL_BIT(pCrtcState->vpId) | (HAL_BIT(pCrtcState->vpId) << 16);
    bool ditherUp;

    actualW = VOP2_RectWidth(src) >> 16;
    actualH = VOP2_RectHeight(src) >> 16;

    dspW = VOP2_RectWidth(dst);
    if (dst->x1 + dspW > (int)adjustedMode->hdisplay) {
        HAL_DBG_WRN("vp%d %s dest->x1[%d] + dspW[%d] exceed mode hdisplay[%d]\n",
                    pCrtcState->vpId, VOP2_GetPlaneName(pPlaneState->physId), dst->x1, dspW, adjustedMode->hdisplay);
        dspW = (int)adjustedMode->hdisplay - dst->x1;
        if (dspW < 4) {
            dspW = 4;
        }
        actualW = dspW * actualW / VOP2_RectWidth(dst);
    }
    dspH = VOP2_RectHeight(dst);
    if (dst->y1 + dspH > (int)adjustedMode->vdisplay) {
        HAL_DBG_WRN("vp%d %s dest->y1[%d] + dspH[%d] exceed mode vdisplay[%d]\n",
                    pCrtcState->vpId, VOP2_GetPlaneName(pPlaneState->physId), dst->y1, dspH, adjustedMode->vdisplay);
        dspH = (int)adjustedMode->vdisplay - dst->y1;
        if (dspH < 4) {
            dspH = 4;
        }
        actualH = dspH * actualH / VOP2_RectHeight(dst);
    }

    actInfo = (actualH - 1) << 16;
    actInfo |= (actualW - 1) & 0xffff;

    dspInfo = (dspH - 1) << 16;
    dspInfo |= (dspW - 1) & 0xffff;

    dspStx = dst->x1;
    dspSty = dst->y1;
    dspSt = dspSty << 16 | (dspStx & 0xffff);

    HAL_DBG("vp%d update %s[%dx%d->%dx%d@(%ld, %ld)] fmt[0x%08lx] addr[0x%08lx]\n",
            pCrtcState->vpId, VOP2_GetPlaneName(pPlaneState->physId),
            actualW, actualH, dspW, dspH,
            dspStx, dspSty,
            pPlaneState->format, pPlaneState->yrgbMst);

    VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_MST_CTL,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_MST_CTL,
                   VOP2_ESMART_REGION0_MST_CTL_REGION0_DATA_FMT_SHIFT,
                   VOP2_ESMART_REGION0_MST_CTL_REGION0_DATA_FMT_MASK, pPlaneState->format);
    VOP2_Write(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_MST_YRGB,
               &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_MST_YRGB,
               pPlaneState->yrgbMst);
    VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_VIR,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_VIR,
                   VOP2_ESMART_REGION0_VIR_REGION0_VIR_STRIDE_SHIFT,
                   VOP2_ESMART_REGION0_VIR_REGION0_VIR_STRIDE_MASK, pPlaneState->stride);
    if (pPlaneState->isYuv) {
        VOP2_Write(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_MST_CBCR,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_MST_CBCR,
                   pPlaneState->uvMst);
        VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_VIR,
                       &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_VIR,
                       VOP2_ESMART_REGION0_VIR_REGION0_VIR_STRIDE_UV_SHIFT,
                       VOP2_ESMART_REGION0_VIR_REGION0_VIR_STRIDE_UV_MASK, pPlaneState->uvStride);
    }

    VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_MST_CTL,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_MST_CTL,
                   VOP2_ESMART_REGION0_MST_CTL_REGION0_RB_SWAP_SHIFT,
                   VOP2_ESMART_REGION0_MST_CTL_REGION0_RB_SWAP_MASK, pPlaneState->rbSwap);
    VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_MST_CTL,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_MST_CTL,
                   VOP2_ESMART_REGION0_MST_CTL_REGION0_UV_SWAP_SHIFT,
                   VOP2_ESMART_REGION0_MST_CTL_REGION0_UV_SWAP_MASK, pPlaneState->uvSwap);

    VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_VIR,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_VIR,
                   VOP2_ESMART_REGION0_VIR_REGION0_VIR_STRIDE_UV_SHIFT,
                   VOP2_ESMART_REGION0_VIR_REGION0_VIR_STRIDE_UV_MASK, pPlaneState->uvStride);

    VOP2_SetupScale(pPlaneState, actualW, actualH, dspW, dspH);

    VOP2_Write(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_ACT_INFO,
               &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_ACT_INFO,
               actInfo);
    VOP2_Write(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_DSP_INFO,
               &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_DSP_INFO,
               dspInfo);
    VOP2_Write(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_DSP_OFFSET,
               &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_DSP_OFFSET,
               dspSt);

    VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->ESMART_CTRL0,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->ESMART_CTRL0,
                   VOP2_ESMART_ESMART_CTRL0_ESMART_RGB2YUV_EN_SHIFT,
                   VOP2_ESMART_ESMART_CTRL0_ESMART_RGB2YUV_EN_MASK, pPlaneState->r2yEn);
    VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->ESMART_CTRL0,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->ESMART_CTRL0,
                   VOP2_ESMART_ESMART_CTRL0_ESMART_YUV2RGB_EN_SHIFT,
                   VOP2_ESMART_ESMART_CTRL0_ESMART_YUV2RGB_EN_MASK, pPlaneState->y2rEn);
    VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->ESMART_CTRL0,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->ESMART_CTRL0,
                   VOP2_ESMART_ESMART_CTRL0_ESMART_CSC_MODE_SHIFT,
                   VOP2_ESMART_ESMART_CTRL0_ESMART_CSC_MODE_MASK, pPlaneState->cscMode);

    ditherUp = VOP2_WinDitherUp(pPlaneState->format);
    VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_MST_CTL,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_MST_CTL,
                   VOP2_ESMART_REGION0_MST_CTL_REGION0_DITHER_UP_EN_SHIFT,
                   VOP2_ESMART_REGION0_MST_CTL_REGION0_DITHER_UP_EN_MASK, ditherUp);

    VOP2_MaskWrite(&VOP2_GetEsmartRegMir(pPlaneState->physId)->REGION0_MST_CTL,
                   &VOP2_GetEsmartReg(pPlaneState->physId)->REGION0_MST_CTL,
                   VOP2_ESMART_REGION0_MST_CTL_REGION0_MST_EN_SHIFT,
                   VOP2_ESMART_REGION0_MST_CTL_REGION0_MST_EN_MASK, true);

    VOP2_Write(&sVop2SysMir.SYS_REG_CFG_DONE,
               &VOP2_SYS->SYS_REG_CFG_DONE, cfgDone);

    return HAL_OK;
}

/** @} */

/** @defgroup VOP2_Public_Definition Public Definition
 *  @{
 */
/********************* Public Function Definition ****************************/

/**
 * @brief  Get VOP2 platform data.
 * @param  pVop2Data: VOP2 platform data structure.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP2_GetPlatformData(struct VOP2_DATA *pVop2Data)
{
    HAL_ASSERT(pVop2Data != NULL);

    memcpy(pVop2Data, &sRK3568Vop, sizeof(struct VOP2_DATA));

    return HAL_OK;
}

/**
 * @brief  Backup VOP2 regs and setup base configurations.
 * @param  pCrtcState: Crtc state sturcture.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP2_Init(struct CRTC_STATE *pCrtcState)
{
    HAL_ASSERT(pCrtcState != NULL);

    VOP2_RegsBackup();

    VOP2_MaskWrite(&sVop2SysMir.SYS_REG_CFG_DONE, &VOP2_SYS->SYS_REG_CFG_DONE,
                   VOP2_SYS_SYS_REG_CFG_DONE_SW_GLOBAL_REGDONE_EN_SHIFT,
                   VOP2_SYS_SYS_REG_CFG_DONE_SW_GLOBAL_REGDONE_EN_MASK, 1);
    /*
     * Disable auto gating, this is a workaround to
     * avoid display image shift when a window enabled.
     */
    VOP2_MaskWrite(&sVop2SysMir.SYS_AUTO_GATING_CTRL_IMD, &VOP2_SYS->SYS_AUTO_GATING_CTRL_IMD,
                   VOP2_SYS_SYS_AUTO_GATING_CTRL_IMD_AUTO_GATING_EN_SHIFT,
                   VOP2_SYS_SYS_AUTO_GATING_CTRL_IMD_AUTO_GATING_EN_MASK, 0);

    VOP2_MaskWrite(&sVop2SysMir.SYS_AUTO_GATING_CTRL_IMD, &VOP2_SYS->SYS_AUTO_GATING_CTRL_IMD,
                   VOP2_SYS_SYS_AUTO_GATING_CTRL_IMD_SMART1_ACLK_GATING_EN_SHIFT,
                   VOP2_SYS_SYS_AUTO_GATING_CTRL_IMD_SMART1_ACLK_GATING_EN_MASK, 0);

    /*
     * Register OVERLAY_LAYER_SEL and OVERLAY_PORT_SEL should take effect immediately,
     * than windows configuration(CLUSTER/ESMART/SMART) can take effect according the
     * video port mux configuration as we wished.
     */
    VOP2_MaskWrite(&sVop2OverlayMir.OVERLAY_CTRL, &VOP2_OVERLAY->OVERLAY_CTRL,
                   VOP2_OVERLAY_OVERLAY_CTRL_LAYER_SEL_REGDONE_IMD_SHIFT,
                   VOP2_OVERLAY_OVERLAY_CTRL_LAYER_SEL_REGDONE_IMD_MASK, 1);

    VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_POL, &VOP2_SYS->SYS_DSP_INFACE_POL,
                   VOP2_SYS_SYS_DSP_INFACE_POL_REGDONE_IMD_EN_SHIFT,
                   VOP2_SYS_SYS_DSP_INFACE_POL_REGDONE_IMD_EN_MASK, 1);

    /* Enable axi irqs */
    VOP2_MaskWrite(NULL, &VOP2_SYS->SYS0_INTR_CLR,
                   VOP2_SYS_SYS0_INTR_CLR_INTR_CLR_BUS_ERROR_SHIFT,
                   VOP2_SYS_SYS0_INTR_CLR_INTR_CLR_BUS_ERROR_MASK, 1);
    VOP2_MaskWrite(NULL, &VOP2_SYS->SYS0_INTR_EN,
                   VOP2_SYS_SYS0_INTR_EN_INTR_EN_BUS0_ERROR_SHIFT,
                   VOP2_SYS_SYS0_INTR_EN_INTR_EN_BUS0_ERROR_MASK, 1);
    VOP2_MaskWrite(NULL, &VOP2_SYS->SYS1_INTR_CLR_SYS,
                   VOP2_SYS_SYS1_INTR_CLR_SYS_INTR_CLR_BUS_ERROR_SHIFT,
                   VOP2_SYS_SYS1_INTR_CLR_SYS_INTR_CLR_BUS_ERROR_MASK, 1);
    VOP2_MaskWrite(NULL, &VOP2_SYS->SYS0_INTR_EN,
                   VOP2_SYS_SYS1_INTR_EN_INTR_EN_BUS1_ERROR_SHIFT,
                   VOP2_SYS_SYS1_INTR_EN_INTR_EN_BUS1_ERROR_MASK, 1);

    VOP2_MaskWrite(NULL, &VOP2_SYS->PORT0_INTR_CLR,
                   VOP2_SYS_PORT0_INTR_CLR_INTR_CLR_POST_BUF_EMPTY_SHIFT,
                   VOP2_SYS_PORT0_INTR_CLR_INTR_CLR_POST_BUF_EMPTY_MASK, 1);
    VOP2_MaskWrite(NULL, &VOP2_SYS->PORT0_INTR_EN,
                   VOP2_SYS_PORT0_INTR_EN_INTR_EN_POST_BUF_EMPTY_SHIFT,
                   VOP2_SYS_PORT0_INTR_EN_INTR_EN_POST_BUF_EMPTY_MASK, 1);
    VOP2_MaskWrite(NULL, &VOP2_SYS->PORT1_INTR_CLR,
                   VOP2_SYS_PORT1_INTR_CLR_INTR_CLR_POST_BUF_EMPTY_SHIFT,
                   VOP2_SYS_PORT1_INTR_CLR_INTR_CLR_POST_BUF_EMPTY_MASK, 1);
    VOP2_MaskWrite(NULL, &VOP2_SYS->PORT1_INTR_EN,
                   VOP2_SYS_PORT1_INTR_EN_INTR_EN_POST_BUF_EMPTY_SHIFT,
                   VOP2_SYS_PORT1_INTR_EN_INTR_EN_POST_BUF_EMPTY_MASK, 1);
    VOP2_MaskWrite(NULL, &VOP2_SYS->PORT2_INTR_CLR,
                   VOP2_SYS_PORT2_INTR_CLR_INTR_CLR_POST_BUF_EMPTY_SHIFT,
                   VOP2_SYS_PORT2_INTR_CLR_INTR_CLR_POST_BUF_EMPTY_MASK, 1);
    VOP2_MaskWrite(NULL, &VOP2_SYS->PORT2_INTR_EN,
                   VOP2_SYS_PORT2_INTR_EN_INTR_EN_POST_BUF_EMPTY_SHIFT,
                   VOP2_SYS_PORT2_INTR_EN_INTR_EN_POST_BUF_EMPTY_MASK, 1);

    return HAL_OK;
}

/**
 * @brief  Setup VOP2 video port delay configurations.
 * @param  pCrtcState: Crtc state sturcture.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP2_SetupDlyForVp(struct CRTC_STATE *pCrtcState)
{
    struct DISPLAY_MODE_INFO *mode;
    const struct VOP2_VIDEO_PORT_DATA *vpData = &rk3568VopVideoPorts[pCrtcState->vpId];
    uint32_t bgDly = vpData->preScanMaxDly[0];
    uint32_t bgOvlDly, preScanDly;
    uint16_t hdisplay, hsyncLen;

    HAL_ASSERT(pCrtcState != NULL);
    mode = &pCrtcState->adjustedMode;
    hdisplay = mode->crtcHdisplay;
    hsyncLen = mode->crtcHsyncEnd - mode->crtcHsyncStart;

    bgOvlDly = pCrtcState->bgOvlDly;
    if (vpData->hdrTable) {
        if (pCrtcState->hdrIn) {
            if (pCrtcState->hdrOut) {
                bgDly = vpData->preScanMaxDly[2];
            }
        } else {
            if (pCrtcState->hdrOut) {
                bgDly = vpData->preScanMaxDly[1];
            } else {
                bgDly = vpData->preScanMaxDly[3];
            }
        }
    }
    bgDly -= bgOvlDly;

    preScanDly = bgDly + (hdisplay >> 1) - 1;
    preScanDly = (preScanDly << 16) | hsyncLen;

    if (pCrtcState->vpId == 0) {
        VOP2_MaskWrite(&sVop2OverlayMir.DP0_BG_MIX_CTRL, &VOP2_OVERLAY->DP0_BG_MIX_CTRL,
                       VOP2_OVERLAY_DP0_BG_MIX_CTRL_DP_BG_DLY_NUM_SHIFT,
                       VOP2_OVERLAY_DP0_BG_MIX_CTRL_DP_BG_DLY_NUM_MASK, bgDly);
    } else if (pCrtcState->vpId == 1) {
        VOP2_MaskWrite(&sVop2OverlayMir.DP1_BG_MIX_CTRL, &VOP2_OVERLAY->DP1_BG_MIX_CTRL,
                       VOP2_OVERLAY_DP1_BG_MIX_CTRL_DP_BG_DLY_NUM_SHIFT,
                       VOP2_OVERLAY_DP1_BG_MIX_CTRL_DP_BG_DLY_NUM_MASK, bgDly);
    } else {
        VOP2_MaskWrite(&sVop2OverlayMir.DP2_BG_MIX_CTRL, &VOP2_OVERLAY->DP2_BG_MIX_CTRL,
                       VOP2_OVERLAY_DP2_BG_MIX_CTRL_DP_BG_DLY_NUM_SHIFT,
                       VOP2_OVERLAY_DP2_BG_MIX_CTRL_DP_BG_DLY_NUM_MASK, bgDly);
    }
    VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_PRE_SCAN_HTIMING,
               &VOP2_GetPostReg(pCrtcState->vpId)->POST_PRE_SCAN_HTIMING,
               preScanDly);

    return HAL_OK;
}

/**
 * @brief  Enable VOP2 atomically.
 * @param  pCrtcState: Crtc state sturcture.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP2_AtomicEnable(struct CRTC_STATE *pCrtcState)
{
    const struct VOP2_VIDEO_PORT_DATA *vpData = &rk3568VopVideoPorts[pCrtcState->vpId];
    struct DISPLAY_MODE_INFO *mode;
    uint32_t cfgDone = VOP2_SYS_SYS_REG_CFG_DONE_SW_GLOBAL_REGDONE_EN_MASK |
                       HAL_BIT(pCrtcState->vpId) | (HAL_BIT(pCrtcState->vpId) << 16);
    uint32_t hsyncLen, hdisplay, htotal, hactSt, hactEnd;
    uint32_t vdisplay, vtotal, vsyncLen, vactSt, vactEnd;
    uint32_t dclkRate;
    uint32_t val;
    uint32_t ret;
    bool interlaced;
    bool dclkInv;
    bool ycSwap;

    HAL_ASSERT(pCrtcState != NULL);
    mode = &pCrtcState->adjustedMode;
    hsyncLen = mode->crtcHsyncEnd - mode->crtcHsyncStart;
    hdisplay = mode->crtcHdisplay;
    htotal = mode->crtcHtotal;
    hactSt = mode->crtcHtotal - mode->crtcHsyncStart;
    hactEnd = hactSt + hdisplay;
    vdisplay = mode->crtcVdisplay;
    vtotal = mode->crtcVtotal;
    vsyncLen = mode->crtcVsyncEnd - mode->crtcVsyncStart;
    vactSt = mode->crtcVtotal - mode->crtcVsyncStart;
    vactEnd = vactSt + vdisplay;
    dclkRate = mode->crtcClock;
    interlaced = !!(mode->flags & VIDEO_MODE_FLAG_INTERLACE);

    HAL_DBG("Update mode to %ldx%d%s%d, type: %ld(if:%llx) for vp%d dclk: %ld\n",
            hdisplay, mode->vdisplay, interlaced ? "i" : "p",
            mode->vrefresh, pCrtcState->outputType, pCrtcState->outputIf,
            pCrtcState->vpId, mode->crtcClock * 1000);

    pCrtcState->vdisplay = (int)vdisplay;
    pCrtcState->modeUpdate = VOP2_CrtcModeUpdate(pCrtcState);
    if (pCrtcState->modeUpdate) {
        VOP2_DisableAllPlanesForCrtc(pCrtcState);
    }

    dclkInv = (mode->flags & VIDEO_MODE_FLAG_NPIXDATA) ? 1 : 0;
    val = (mode->flags & VIDEO_MODE_FLAG_NHSYNC) ? 0 : HAL_BIT(HSYNC_POSITIVE);
    val |= (mode->flags & VIDEO_MODE_FLAG_NVSYNC) ? 0 : HAL_BIT(VSYNC_POSITIVE);

    if (pCrtcState->outputIf & VOP_OUTPUT_IF_RGB) {
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_OUT_EN_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_OUT_EN_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_INFACE_MUX_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_INFACE_MUX_MASK, pCrtcState->vpId);
        VOP2_GrfMaskWrite(&GRF->VO_CON1, GRF_VO_CON1_RGB_DCLK_INV_SEL_SHIFT,
                          GRF_VO_CON1_RGB_DCLK_INV_SEL_MASK, dclkInv);
    }

    if (pCrtcState->outputIf & VOP_OUTPUT_IF_BT1120) {
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_OUT_EN_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_OUT_EN_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_BT1120_OUT_EN_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_BT1120_OUT_EN_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_INFACE_MUX_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_INFACE_MUX_MASK, pCrtcState->vpId);
        VOP2_GrfMaskWrite(&GRF->VO_CON1, GRF_VO_CON1_BT1120_CLK_INV_SEL_SHIFT,
                          GRF_VO_CON1_BT1120_CLK_INV_SEL_MASK, dclkInv);
        ycSwap = VOP2_IsOuputYcSwap(pCrtcState->busFormat);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_CTRL, &VOP2_SYS->SYS_DSP_INFACE_CTRL,
                       VOP2_SYS_SYS_DSP_INFACE_CTRL_BT1120_YC_SWAP_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_CTRL_BT1120_YC_SWAP_MASK, ycSwap);
    }

    if (pCrtcState->outputIf & VOP_OUTPUT_IF_BT656) {
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_OUT_EN_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_OUT_EN_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_BT656_OUT_EN_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_BT656_OUT_EN_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_INFACE_MUX_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_RGB_INFACE_MUX_MASK, pCrtcState->vpId);
        VOP2_GrfMaskWrite(&GRF->VO_CON1, GRF_VO_CON1_BT656_CLK_INV_SEL_SHIFT,
                          GRF_VO_CON1_BT656_CLK_INV_SEL_MASK, dclkInv);
        ycSwap = VOP2_IsOuputYcSwap(pCrtcState->busFormat);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_CTRL, &VOP2_SYS->SYS_DSP_INFACE_CTRL,
                       VOP2_SYS_SYS_DSP_INFACE_CTRL_BT656_YC_SWAP_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_CTRL_BT656_YC_SWAP_MASK, ycSwap);
    }

    if (pCrtcState->outputIf & VOP_OUTPUT_IF_LVDS0) {
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_LVDS_OUT_EN_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_LVDS_OUT_EN_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_LVDS_INFACE_MUX_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_LVDS_INFACE_MUX_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_POL, &VOP2_SYS->SYS_DSP_INFACE_POL,
                       VOP2_SYS_SYS_DSP_INFACE_POL_RGB_LVDS_HSYNC_POL_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_POL_RGB_LVDS_HSYNC_POL_MASK |
                       VOP2_SYS_SYS_DSP_INFACE_POL_RGB_LVDS_VSYNC_POL_MASK |
                       VOP2_SYS_SYS_DSP_INFACE_POL_RGB_LVDS_DEN_POL_MASK, val);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_POL, &VOP2_SYS->SYS_DSP_INFACE_POL,
                       VOP2_SYS_SYS_DSP_INFACE_POL_RGB_LVDS_HSYNC_POL_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_POL_RGB_LVDS_CLK_POL_MASK, dclkInv);
    }

    if (pCrtcState->outputIf & VOP_OUTPUT_IF_MIPI0) {
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_MIPI_OUT_EN_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_MIPI_OUT_EN_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_MIPI_INFACE_MUX_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_MIPI_INFACE_MUX_MASK, pCrtcState->vpId);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_POL, &VOP2_SYS->SYS_DSP_INFACE_POL,
                       VOP2_SYS_SYS_DSP_INFACE_POL_MIPI_HSYNC_POL_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_POL_MIPI_HSYNC_POL_SHIFT |
                       VOP2_SYS_SYS_DSP_INFACE_POL_MIPI_VSYNC_POL_SHIFT |
                       VOP2_SYS_SYS_DSP_INFACE_POL_MIPI_DEN_POL_SHIFT, val);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_POL, &VOP2_SYS->SYS_DSP_INFACE_POL,
                       VOP2_SYS_SYS_DSP_INFACE_POL_MIPI_DCLK_POL_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_POL_MIPI_DCLK_POL_MASK, dclkInv);
    }

    if (pCrtcState->outputIf & VOP_OUTPUT_IF_eDP0) {
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_EDP_OUT_EN_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_EDP_OUT_EN_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_EDP_INFACE_MUX_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_EDP_INFACE_MUX_MASK, pCrtcState->vpId);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_POL, &VOP2_SYS->SYS_DSP_INFACE_POL,
                       VOP2_SYS_SYS_DSP_INFACE_POL_EDP_HSYNC_POL_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_POL_EDP_HSYNC_POL_SHIFT |
                       VOP2_SYS_SYS_DSP_INFACE_POL_EDP_VSYNC_POL_SHIFT |
                       VOP2_SYS_SYS_DSP_INFACE_POL_EDP_DEN_POL_SHIFT, val);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_POL, &VOP2_SYS->SYS_DSP_INFACE_POL,
                       VOP2_SYS_SYS_DSP_INFACE_POL_EDP_DCLK_POL_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_POL_EDP_DCLK_POL_MASK, dclkInv);
    }

    if (pCrtcState->outputIf & VOP_OUTPUT_IF_HDMI0) {
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_HDMI_OUT_EN_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_HDMI_OUT_EN_MASK, 1);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_EN, &VOP2_SYS->SYS_DSP_INFACE_EN,
                       VOP2_SYS_SYS_DSP_INFACE_EN_HDMI_INFACE_MUX_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_EN_HDMI_INFACE_MUX_MASK, pCrtcState->vpId);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_POL, &VOP2_SYS->SYS_DSP_INFACE_POL,
                       VOP2_SYS_SYS_DSP_INFACE_POL_HDMI_HSYNC_POL_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_POL_HDMI_HSYNC_POL_SHIFT |
                       VOP2_SYS_SYS_DSP_INFACE_POL_HDMI_VSYNC_POL_SHIFT |
                       VOP2_SYS_SYS_DSP_INFACE_POL_HDMI_DEN_POL_SHIFT, val);
        VOP2_MaskWrite(&sVop2SysMir.SYS_DSP_INFACE_POL, &VOP2_SYS->SYS_DSP_INFACE_POL,
                       VOP2_SYS_SYS_DSP_INFACE_POL_HDMI_DCLK_POL_SHIFT,
                       VOP2_SYS_SYS_DSP_INFACE_POL_HDMI_DCLK_POL_MASK, dclkInv);
    }

    VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_HTOTAL_HS_END,
               &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_HTOTAL_HS_END,
               (htotal << 16) | hsyncLen);
    val = hactSt << 16;
    val |= hactEnd;
    VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_HACT_ST_END,
               &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_HACT_ST_END,
               val);
    val = vactSt << 16;
    val |= vactEnd;
    VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_VACT_ST_END,
               &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_VACT_ST_END,
               val);
    if (mode->flags & VIDEO_MODE_FLAG_INTERLACE) {
        uint16_t vactStF1 = vtotal + vactSt + 1;
        uint16_t vactEndF1 = vactStF1 + vdisplay;

        val = vactStF1 << 16 | vactEndF1;
        VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_VACT_ST_END_F1,
                   &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_VACT_ST_END_F1,
                   val);

        val = vtotal << 16 | (vtotal + vsyncLen);
        VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_VS_ST_END_F1,
                   &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_VS_ST_END_F1,
                   val);

        VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_INTERLACE_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_INTERLACE_MASK, 1);
        VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_FILED_POL_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_FILED_POL_MASK, 1);
        VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_P2I_EN_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_P2I_EN_MASK, 1);
        vtotal += vtotal + 1;
    } else {
        VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_INTERLACE_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_INTERLACE_MASK, 0);
        VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_P2I_EN_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_P2I_EN_MASK, 0);
    }
    VOP2_Write(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_VTOTAL_VS_END,
               &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_VTOTAL_VS_END,
               (vtotal << 16) | vsyncLen);

    if (mode->flags & VIDEO_MODE_FLAG_DBLCLK ||
        pCrtcState->outputIf & VOP_OUTPUT_IF_BT656) {
        VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_CORE_DCLK_SEL_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_CORE_DCLK_SEL_MASK, 1);
    } else {
        VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                       &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_CORE_DCLK_SEL_SHIFT,
                       VOP2_POST0_DSP_POST_DSP_CTRL_DSP_CORE_DCLK_SEL_MASK, 0);
    }

    if (pCrtcState->outputMode == ROCKCHIP_OUT_MODE_YUV420) {
        VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_MIPI_CTRL,
                       &VOP2_GetPostReg(pCrtcState->vpId)->POST_MIPI_CTRL,
                       DCLK_DIV2_SHIFT,
                       DCLK_DIV2_MASK, 0x3);
    } else {
        VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_MIPI_CTRL,
                       &VOP2_GetPostReg(pCrtcState->vpId)->POST_MIPI_CTRL,
                       DCLK_DIV2_SHIFT,
                       DCLK_DIV2_MASK, 0x0);
    }

    HAL_VOP2_SetupDlyForVp(pCrtcState);

    VOP2_EnableDclk(pCrtcState->vpId);
    ret = VOP2_SetDclk(pCrtcState->vpId, dclkRate * 1000);
    HAL_DBG("set dclk_vp%d to %ld, get %ld\n", pCrtcState->vpId, dclkRate * 1000, ret);

    if (vpData->feature & VOP_FEATURE_OVERSCAN) {
        VOP2_PostConfig(pCrtcState);
    }

    VOP2_CfgUpdate(pCrtcState);

    VOP2_Write(&sVop2SysMir.SYS_REG_CFG_DONE,
               &VOP2_SYS->SYS_REG_CFG_DONE, cfgDone);

    /*
     * when clear standby bits, it will take effect immediately,
     * This means the vp will start scan out immediately with
     * the timing it been configured before.
     * So we must make sure release standby after the display
     * timing is correctly configured.
     * This is important when switch resolution, such as
     * 4K-->720P:
     * if we release standby before 720P timing is configured,
     * the VP will start scan out immediately with 4K timing,
     * when we switch dclk to 74.25MHZ, VP timing is still 4K,
     * so VP scan out with 4K timing at 74.25MHZ dclk, this is
     * very slow, than this will trigger vblank timeout.
     *
     */
    VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                   &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                   VOP2_POST0_DSP_POST_DSP_CTRL_VOP_STANDBY_EN_IMD_SHIFT,
                   VOP2_POST0_DSP_POST_DSP_CTRL_VOP_STANDBY_EN_IMD_MASK, 0);

    VOP2_CrtcVblankEnable(pCrtcState);

    return HAL_OK;
}

/**
 * @brief  Disable VOP2 atomically.
 * @param  pCrtcState: Crtc state sturcture.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP2_AtomicDisable(struct CRTC_STATE *pCrtcState)
{
    HAL_ASSERT(pCrtcState != NULL);

    HAL_DBG("Crtc atomic disable vp%d\n", pCrtcState->vpId);

    VOP2_DisableAllPlanesForCrtc(pCrtcState);

    /*
     * Vop standby will take effect at end of current frame,
     * if dsp hold valid irq happen, it means standby complete.
     *
     * we must wait standby complete when we want to disable aclk,
     * if not, memory bus maybe dead.
     */
    VOP2_DspHoldValidIrqEnable(pCrtcState);

    VOP2_MaskWrite(&VOP2_GetPostRegMir(pCrtcState->vpId)->POST_DSP_CTRL,
                   &VOP2_GetPostReg(pCrtcState->vpId)->POST_DSP_CTRL,
                   VOP2_POST0_DSP_POST_DSP_CTRL_VOP_STANDBY_EN_IMD_SHIFT,
                   VOP2_POST0_DSP_POST_DSP_CTRL_VOP_STANDBY_EN_IMD_MASK, 0);

    VOP2_DspHoldValidIrqDisable(pCrtcState);

    VOP2_DisableDclk(pCrtcState->vpId);

    return HAL_OK;
}

/**
 * @brief  Update VOP2 win configurations atomically.
 * @param  pCrtcState: Crtc state sturcture.
 * @param  pPlaneState: Plane state sturcture.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP2_AtomicUpdateWin(struct CRTC_STATE *pCrtcState, struct PLANE_STATE *pPlaneState)
{
    int ret;

    HAL_ASSERT(pCrtcState != NULL);
    HAL_ASSERT(pPlaneState != NULL);

    if (VOP2_IsClusterWin(pPlaneState->physId)) {
        ret = VOP2_SetClusterWin(pPlaneState, pCrtcState);
    } else {
        ret = VOP2_SetSmartWin(pPlaneState, pCrtcState);
    }
    if (ret) {
        HAL_DBG_ERR("failed to set win: %s\n", VOP2_GetPlaneName(pPlaneState->physId));

        return HAL_INVAL;
    }

    HAL_DBG("VOP VP%d enable %s[%dx%d->%dx%d@%dx%d] fmt[%ld] addr[0x%lx]\n",
            pCrtcState->vpId, VOP2_GetPlaneName(pPlaneState->physId),
            VOP2_RectWidth(&pPlaneState->src) >> 16, VOP2_RectHeight(&pPlaneState->src) >> 16,
            VOP2_RectWidth(&pPlaneState->dst), VOP2_RectHeight(&pPlaneState->dst),
            pPlaneState->dst.x1, pPlaneState->dst.y1, pPlaneState->format,
            pPlaneState->yrgbMst);

    return HAL_OK;
}

/**
 * @brief  Disable VOP2 win.
 * @param  physId: VOP2 win physical ID.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP2_DisableWin(uint8_t physId)
{
    const struct VOP2_WIN_DATA *winData = VOP2_FindWinDataByPhysId(physId);

    if (VOP2_IsClusterWin(physId)) {
        if (winData->feature & WIN_FEATURE_CLUSTER_SUB) {
            VOP2_MaskWrite(&VOP2_GetClusterRegMir(physId)->WIN0_CTRL0,
                           &VOP2_GetClusterReg(physId)->WIN0_CTRL0,
                           VOP2_CLUSTER_WIN0_CTRL0_WIN0_EN_SHIFT,
                           VOP2_CLUSTER_WIN0_CTRL0_WIN0_EN_MASK, false);
        } else if (winData->feature & WIN_FEATURE_CLUSTER_MAIN) {
            VOP2_MaskWrite(&VOP2_GetClusterRegMir(physId)->WIN0_CTRL0,
                           &VOP2_GetClusterReg(physId)->WIN0_CTRL0,
                           VOP2_CLUSTER_WIN0_CTRL0_WIN0_EN_SHIFT,
                           VOP2_CLUSTER_WIN0_CTRL0_WIN0_EN_MASK, false);
            VOP2_MaskWrite(&VOP2_GetClusterRegMir(physId)->CLUSTER_CTRL,
                           &VOP2_GetClusterReg(physId)->CLUSTER_CTRL,
                           VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_EN_SHIFT,
                           VOP2_CLUSTER_CLUSTER_CTRL_CLUSTER_EN_MASK, false);
        }
    } else if (VOP2_IsEsmartWin(physId)) {
        VOP2_MaskWrite(&VOP2_GetEsmartRegMir(physId)->REGION0_MST_CTL,
                       &VOP2_GetEsmartReg(physId)->REGION0_MST_CTL,
                       VOP2_ESMART_REGION0_MST_CTL_REGION0_MST_EN_SHIFT,
                       VOP2_ESMART_REGION0_MST_CTL_REGION0_MST_EN_MASK, false);
    } else {
        HAL_DBG_ERR("Unsupported phys id: %d\n", physId);

        return HAL_INVAL;
    }

    return HAL_OK;
}

/**
 * @brief  Disable VOP2 win.
 * @param  physId: VOP2 win physical ID.
 * @param  vpId: VOP2 video port ID.
 * @param  layerId: VOP2 layer ID.
 * @param  portMuxCfg: VOP2 video port mux configurations.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP2_InitOverlay(uint8_t physId, uint8_t vpId, uint8_t layerId, uint16_t portMuxCfg)
{
    const struct VOP2_WIN_DATA *winData;

    if (portMuxCfg) {
        VOP2_MaskWrite(&sVop2OverlayMir.OVERLAY_PORT_SEL, &VOP2_OVERLAY->OVERLAY_PORT_SEL,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_PORT0_MUX_SHIFT,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_PORT0_MUX_MASK |
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_PORT1_MUX_MASK |
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_PORT2_MUX_MASK,
                       portMuxCfg);

        return HAL_OK;
    }

    winData = VOP2_FindWinDataByPhysId(physId);
    if (!winData) {
        return HAL_INVAL;
    }

    VOP2_MaskWrite(&sVop2OverlayMir.OVERLAY_LAYER_SEL, &VOP2_OVERLAY->OVERLAY_LAYER_SEL,
                   layerId * 4,
                   VOP2_OVERLAY_OVERLAY_LAYER_SEL_LAYER0_SEL_MASK << (layerId * 4), winData->layerSelId[vpId]);

    switch (physId) {
    case ROCKCHIP_VOP2_CLUSTER0:
        VOP2_MaskWrite(&sVop2OverlayMir.OVERLAY_PORT_SEL, &VOP2_OVERLAY->OVERLAY_PORT_SEL,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_CLUSTER0_SEL_PORT_SHIFT,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_CLUSTER0_SEL_PORT_MASK, vpId);
        break;
    case ROCKCHIP_VOP2_CLUSTER1:
        VOP2_MaskWrite(&sVop2OverlayMir.OVERLAY_PORT_SEL, &VOP2_OVERLAY->OVERLAY_PORT_SEL,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_CLUSTER1_SEL_PORT_SHIFT,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_CLUSTER1_SEL_PORT_MASK, vpId);
        break;
    case ROCKCHIP_VOP2_ESMART0:
        VOP2_MaskWrite(&sVop2OverlayMir.OVERLAY_PORT_SEL, &VOP2_OVERLAY->OVERLAY_PORT_SEL,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_ESMART0_SEL_PORT_SHIFT,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_ESMART0_SEL_PORT_MASK, vpId);
        break;
    case ROCKCHIP_VOP2_ESMART1:
        VOP2_MaskWrite(&sVop2OverlayMir.OVERLAY_PORT_SEL, &VOP2_OVERLAY->OVERLAY_PORT_SEL,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_ESMART1_SEL_PORT_SHIFT,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_ESMART1_SEL_PORT_MASK, vpId);
        break;
    case ROCKCHIP_VOP2_SMART0:
        VOP2_MaskWrite(&sVop2OverlayMir.OVERLAY_PORT_SEL, &VOP2_OVERLAY->OVERLAY_PORT_SEL,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_SMART0_SEL_PORT_SHIFT,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_SMART0_SEL_PORT_MASK, vpId);
        break;
    case ROCKCHIP_VOP2_SMART1:
        VOP2_MaskWrite(&sVop2OverlayMir.OVERLAY_PORT_SEL, &VOP2_OVERLAY->OVERLAY_PORT_SEL,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_SMART1_SEL_PORT_SHIFT,
                       VOP2_OVERLAY_OVERLAY_PORT_SEL_SMART1_SEL_PORT_MASK, vpId);
        break;
    default:
        return HAL_INVAL;
    }

    return HAL_OK;
}

/**
 * @brief  VOP2 interrupt handler.
 * @param  pCrtcState: Crtc state sturcture.
 * @return VOP2 interrupt status bitmap.
 */
uint32_t HAL_VOP2_IrqHandler(struct CRTC_STATE *pCrtcState)
{
    uint32_t i, j;
    uint32_t val = 0, enable;

    HAL_ASSERT(pCrtcState != NULL);

    for (i = 0; i < sRK3568Vop.nrVps; i++) {
        if (i == 0) {
            enable = VOP2_MaskRead(&VOP2_GetPostReg(i)->POST_DSP_CTRL,
                                   VOP2_POST0_DSP_POST_DSP_CTRL_VOP_STANDBY_EN_IMD_SHIFT,
                                   VOP2_POST0_DSP_POST_DSP_CTRL_VOP_STANDBY_EN_IMD_MASK);
            if (enable) {
                continue;
            }
            val = VOP2_Read(&VOP2_SYS->PORT0_INTR_STATUS);
        } else if (i == 1) {
            enable = VOP2_MaskRead(&VOP2_GetPostReg(i)->POST_DSP_CTRL,
                                   VOP2_POST1_DSP_POST_DSP_CTRL_VOP_STANDBY_EN_IMD_SHIFT,
                                   VOP2_POST1_DSP_POST_DSP_CTRL_VOP_STANDBY_EN_IMD_MASK);
            if (enable) {
                continue;
            }
            val = VOP2_Read(&VOP2_SYS->PORT1_INTR_STATUS);
        } else if (i == 2) {
            enable = VOP2_MaskRead(&VOP2_GetPostReg(i)->POST_DSP_CTRL,
                                   VOP2_POST2_DSP_POST_DSP_CTRL_VOP_STANDBY_EN_IMD_SHIFT,
                                   VOP2_POST2_DSP_POST_DSP_CTRL_VOP_STANDBY_EN_IMD_MASK);
            if (enable) {
                continue;
            }
            val = VOP2_Read(&VOP2_SYS->PORT2_INTR_STATUS);
        }
        for (j = 0; j < HAL_ARRAY_SIZE(VOP2_IRQs); j++) {
            if (val & HAL_BIT(j)) {
#ifdef REGS_DEBUG
                HAL_DBG("VOP2 VP%ld Irq: %s\n", i, VOP2_IRQs[i]);
#endif
            }
        }
        if (i == 0) {
            VOP2_MaskWrite(NULL, &VOP2_SYS->PORT0_INTR_CLR,
                           VOP2_SYS_PORT0_INTR_EN_INTR_EN_FS_SHIFT,
                           VOP2_SYS_PORT0_INTR_EN_INTR_EN_FS_MASK |
                           VOP2_SYS_PORT0_INTR_EN_INTR_EN_LINE_FLAG0_MASK |
                           VOP2_SYS_PORT0_INTR_EN_INTR_EN_LINE_FLAG1_MASK |
                           VOP2_SYS_PORT0_INTR_EN_INTR_EN_POST_BUF_EMPTY_MASK |
                           VOP2_SYS_PORT0_INTR_EN_INTR_EN_FS_FIELD_MASK |
                           VOP2_SYS_PORT0_INTR_EN_INTR_EN_DSP_HOLD_VALID_MASK,
                           val);
        } else if (i == 1) {
            VOP2_MaskWrite(NULL, &VOP2_SYS->PORT1_INTR_CLR,
                           VOP2_SYS_PORT1_INTR_EN_INTR_EN_FS_SHIFT,
                           VOP2_SYS_PORT1_INTR_EN_INTR_EN_FS_MASK |
                           VOP2_SYS_PORT1_INTR_EN_INTR_EN_LINE_FLAG0_MASK |
                           VOP2_SYS_PORT1_INTR_EN_INTR_EN_LINE_FLAG1_MASK |
                           VOP2_SYS_PORT1_INTR_EN_INTR_EN_POST_BUF_EMPTY_MASK |
                           VOP2_SYS_PORT1_INTR_EN_INTR_EN_FS_FIELD_MASK |
                           VOP2_SYS_PORT1_INTR_EN_INTR_EN_DSP_HOLD_VALID_MASK,
                           val);
        } else if (i == 2) {
            VOP2_MaskWrite(NULL, &VOP2_SYS->PORT2_INTR_CLR,
                           VOP2_SYS_PORT2_INTR_EN_INTR_EN_FS_SHIFT,
                           VOP2_SYS_PORT2_INTR_EN_INTR_EN_FS_MASK |
                           VOP2_SYS_PORT2_INTR_EN_INTR_EN_LINE_FLAG0_MASK |
                           VOP2_SYS_PORT2_INTR_EN_INTR_EN_LINE_FLAG1_MASK |
                           VOP2_SYS_PORT2_INTR_EN_INTR_EN_POST_BUF_EMPTY_MASK |
                           VOP2_SYS_PORT2_INTR_EN_INTR_EN_FS_FIELD_MASK |
                           VOP2_SYS_PORT2_INTR_EN_INTR_EN_DSP_HOLD_VALID_MASK,
                           val);
        }
    }

    return val & 0xffff;
}

/**
 * @brief  Setup VOP2 alpha configurations.
 * @param  mixerId: VOP2 mixer ID.
 * @param  srcColorCtrl: VOP2 source color configurations.
 * @param  dstColorCtrl: VOP2 destination color configurations.
 * @param  srcAlphaCtrl: VOP2 source alpha configurations.
 * @param  dstAlphaCtrl: VOP2 destination alpha configurations.
 * @param  isCluster: Whether cluster win or not.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP2_SetupAlpha(uint8_t mixerId, uint32_t srcColorCtrl, uint32_t dstColorCtrl,
                               uint32_t srcAlphaCtrl, uint32_t dstAlphaCtrl, bool isCluster)
{
    if (isCluster) {
        switch (mixerId) {
        case ROCKCHIP_VOP2_CLUSTER0:
            VOP2_Write(&sVop2OverlayMir.CLUSTER0_MIX_SRC_COLOR_CTRL, &VOP2_OVERLAY->CLUSTER0_MIX_SRC_COLOR_CTRL,
                       srcColorCtrl);
            VOP2_Write(&sVop2OverlayMir.CLUSTER0_MIX_DST_COLOR_CTRL, &VOP2_OVERLAY->CLUSTER0_MIX_DST_COLOR_CTRL,
                       dstColorCtrl);
            VOP2_Write(&sVop2OverlayMir.CLUSTER0_MIX_SRC_ALPHA_CTRL, &VOP2_OVERLAY->CLUSTER0_MIX_SRC_ALPHA_CTRL,
                       srcAlphaCtrl);
            VOP2_Write(&sVop2OverlayMir.CLUSTER0_MIX_DST_ALPHA_CTRL, &VOP2_OVERLAY->CLUSTER0_MIX_DST_ALPHA_CTRL,
                       dstAlphaCtrl);
            break;
        case ROCKCHIP_VOP2_CLUSTER1:
            VOP2_Write(&sVop2OverlayMir.CLUSTER1_MIX_SRC_COLOR_CTRL, &VOP2_OVERLAY->CLUSTER1_MIX_SRC_COLOR_CTRL,
                       srcColorCtrl);
            VOP2_Write(&sVop2OverlayMir.CLUSTER1_MIX_DST_COLOR_CTRL, &VOP2_OVERLAY->CLUSTER1_MIX_DST_COLOR_CTRL,
                       dstColorCtrl);
            VOP2_Write(&sVop2OverlayMir.CLUSTER1_MIX_SRC_ALPHA_CTRL, &VOP2_OVERLAY->CLUSTER1_MIX_SRC_ALPHA_CTRL,
                       srcAlphaCtrl);
            VOP2_Write(&sVop2OverlayMir.CLUSTER1_MIX_DST_ALPHA_CTRL, &VOP2_OVERLAY->CLUSTER1_MIX_DST_ALPHA_CTRL,
                       dstAlphaCtrl);
            break;
        default:
            return HAL_INVAL;
        }
    } else {
        switch (mixerId) {
        case 0:
            VOP2_Write(&sVop2OverlayMir.MIX0_SRC_COLOR_CTRL, &VOP2_OVERLAY->MIX0_SRC_COLOR_CTRL,
                       srcColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX0_DST_COLOR_CTRL, &VOP2_OVERLAY->MIX0_DST_COLOR_CTRL,
                       dstColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX0_SRC_ALPHA_CTRL, &VOP2_OVERLAY->MIX0_SRC_ALPHA_CTRL,
                       srcAlphaCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX0_DST_ALPHA_CTRL, &VOP2_OVERLAY->MIX0_DST_ALPHA_CTRL,
                       dstAlphaCtrl);
            break;
        case 1:
            VOP2_Write(&sVop2OverlayMir.MIX1_SRC_COLOR_CTRL, &VOP2_OVERLAY->MIX1_SRC_COLOR_CTRL,
                       srcColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX1_DST_COLOR_CTRL, &VOP2_OVERLAY->MIX1_DST_COLOR_CTRL,
                       dstColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX1_SRC_ALPHA_CTRL, &VOP2_OVERLAY->MIX1_SRC_ALPHA_CTRL,
                       srcAlphaCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX1_DST_ALPHA_CTRL, &VOP2_OVERLAY->MIX1_DST_ALPHA_CTRL,
                       dstAlphaCtrl);
            break;
        case 2:
            VOP2_Write(&sVop2OverlayMir.MIX2_SRC_COLOR_CTRL, &VOP2_OVERLAY->MIX2_SRC_COLOR_CTRL,
                       srcColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX2_DST_COLOR_CTRL, &VOP2_OVERLAY->MIX2_DST_COLOR_CTRL,
                       dstColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX2_SRC_ALPHA_CTRL, &VOP2_OVERLAY->MIX2_SRC_ALPHA_CTRL,
                       srcAlphaCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX2_DST_ALPHA_CTRL, &VOP2_OVERLAY->MIX2_DST_ALPHA_CTRL,
                       dstAlphaCtrl);
            break;
        case 3:
            VOP2_Write(&sVop2OverlayMir.MIX3_SRC_COLOR_CTRL, &VOP2_OVERLAY->MIX3_SRC_COLOR_CTRL,
                       srcColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX3_DST_COLOR_CTRL, &VOP2_OVERLAY->MIX3_DST_COLOR_CTRL,
                       dstColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX3_SRC_ALPHA_CTRL, &VOP2_OVERLAY->MIX3_SRC_ALPHA_CTRL,
                       srcAlphaCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX3_DST_ALPHA_CTRL, &VOP2_OVERLAY->MIX3_DST_ALPHA_CTRL,
                       dstAlphaCtrl);
            break;
        case 4:
            VOP2_Write(&sVop2OverlayMir.MIX4_SRC_COLOR_CTRL, &VOP2_OVERLAY->MIX4_SRC_COLOR_CTRL,
                       srcColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX4_DST_COLOR_CTRL, &VOP2_OVERLAY->MIX4_DST_COLOR_CTRL,
                       dstColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX4_SRC_ALPHA_CTRL, &VOP2_OVERLAY->MIX4_SRC_ALPHA_CTRL,
                       srcAlphaCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX4_DST_ALPHA_CTRL, &VOP2_OVERLAY->MIX4_DST_ALPHA_CTRL,
                       dstAlphaCtrl);
            break;
        case 5:
            VOP2_Write(&sVop2OverlayMir.MIX5_SRC_COLOR_CTRL, &VOP2_OVERLAY->MIX5_SRC_COLOR_CTRL,
                       srcColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX5_DST_COLOR_CTRL, &VOP2_OVERLAY->MIX5_DST_COLOR_CTRL,
                       dstColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX5_SRC_ALPHA_CTRL, &VOP2_OVERLAY->MIX5_SRC_ALPHA_CTRL,
                       srcAlphaCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX5_DST_ALPHA_CTRL, &VOP2_OVERLAY->MIX5_DST_ALPHA_CTRL,
                       dstAlphaCtrl);
            break;
        case 6:
            VOP2_Write(&sVop2OverlayMir.MIX6_SRC_COLOR_CTRL, &VOP2_OVERLAY->MIX6_SRC_COLOR_CTRL,
                       srcColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX6_DST_COLOR_CTRL, &VOP2_OVERLAY->MIX6_DST_COLOR_CTRL,
                       dstColorCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX6_SRC_ALPHA_CTRL, &VOP2_OVERLAY->MIX6_SRC_ALPHA_CTRL,
                       srcAlphaCtrl);
            VOP2_Write(&sVop2OverlayMir.MIX6_DST_ALPHA_CTRL, &VOP2_OVERLAY->MIX6_DST_ALPHA_CTRL,
                       dstAlphaCtrl);
            break;
        default:
            return HAL_INVAL;
        }
    }

    return HAL_OK;
}

/**
 * @brief  VOP2 get win enable/disble status.
 * @param  physId: VOP2 win physical ID.
 * @return HAL_Check.
 */
HAL_Check HAL_VOP2_GetWinStatus(uint8_t physId)
{
    HAL_Check status;

    if (VOP2_IsClusterWin(physId)) {
        status = VOP2_MaskRead(&VOP2_GetClusterRegMir(physId)->WIN0_CTRL0,
                               VOP2_CLUSTER_WIN0_CTRL0_WIN0_EN_SHIFT,
                               VOP2_CLUSTER_WIN0_CTRL0_WIN0_EN_MASK) ||
                 VOP2_MaskRead(&VOP2_GetClusterReg(physId)->WIN0_CTRL0,
                               VOP2_CLUSTER_WIN0_CTRL0_WIN0_EN_SHIFT,
                               VOP2_CLUSTER_WIN0_CTRL0_WIN0_EN_MASK);
    } else if (VOP2_IsEsmartWin(physId)) {
        status = VOP2_MaskRead(&VOP2_GetEsmartRegMir(physId)->REGION0_MST_CTL,
                               VOP2_ESMART_REGION0_MST_CTL_REGION0_MST_EN_SHIFT,
                               VOP2_ESMART_REGION0_MST_CTL_REGION0_MST_EN_MASK) ||
                 VOP2_MaskRead(&VOP2_GetEsmartReg(physId)->REGION0_MST_CTL,
                               VOP2_ESMART_REGION0_MST_CTL_REGION0_MST_EN_SHIFT,
                               VOP2_ESMART_REGION0_MST_CTL_REGION0_MST_EN_MASK);
    } else {
        status = HAL_FALSE;
        HAL_DBG_ERR("Unsupported phys id: %d\n", physId);
    }

    return status;
}

/**
 * @brief  VOP2 dump active/full registers for debugging.
 * @param  isFullDump: VOP2 dump mode: active dump/full dump.
 * @return HAL_Check.
 */
HAL_Status HAL_VOP2_DumpRegs(bool isFullDump)
{
    const struct VOP_DUMP_REGS *regs = sRK3568Vop.dumpRegs;
    uint32_t reg0, reg1, reg2, reg3;
    uint32_t regLen;
    uint32_t i, j;
    bool enableState;

    for (i = 0; i < sRK3568Vop.dumpRegsSize; i++) {
        regLen = regs[i].size ? regs[i].size : 68;

        if (!isFullDump) {
            if (regs[i].stateMask) {
                reg0 = regs[i].base + regs[i].stateBase;
                enableState = (VOP2_Read((void *)(reg0)) >> regs[i].stateShift) & regs[i].stateMask;
                if (enableState != regs[i].enableState) {
                    continue;
                }
            }
        }

        HAL_SYSLOG("\n%s:\n", regs[i].name);
        for (j = 0; j < regLen;) {
            reg0 = regs[i].base + (4 * j);
            reg1 = regs[i].base + (4 * (j + 1));
            reg2 = regs[i].base + (4 * (j + 2));
            reg3 = regs[i].base + (4 * (j + 3));
            HAL_SYSLOG("%08lx:  %08lx %08lx %08lx %08lx\n", (uintptr_t)regs[i].base + j * 4,
                       VOP2_Read((void *)(reg0)),
                       VOP2_Read((void *)(reg1)),
                       VOP2_Read((void *)(reg2)),
                       VOP2_Read((void *)(reg3)));
            j += 4;
        }
    }

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_VOP2_MODULE_ENABLED */
