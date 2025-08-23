/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_VOP2_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup VOP2
 *  @{
 */

#ifndef _HAL_VOP2_H_
#define _HAL_VOP2_H_

#include "hal_display.h"

/***************************** MACRO Definition ******************************/
/** @defgroup VOP2_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/*
 * major: IP major version, used for IP structure
 * minor: big feature change under same structure
 * build: RTL current SVN number
 */
#define VOP2_VERSION(major, minor, build) ((major) << 24 | (minor) << 16 | (build))
#define VOP2_MAJOR(version)               (((version) >> 24) & 0xff)
#define VOP2_MINOR(version)               (((version) >> 16) & 0xff)
#define VOP2_BUILD(version)               ((version) & 0xffff)

#define VOP_VERSION_RK3528 VOP2_VERSION(0x50, 0x17, 0x1263)
#define VOP_VERSION_RK3562 VOP2_VERSION(0x50, 0x17, 0x4350)
#define VOP_VERSION_RK3568 VOP2_VERSION(0x40, 0x15, 0x8023)
#define VOP_VERSION_RK3576 VOP2_VERSION(0x50, 0x19, 0x9765)
#define VOP_VERSION_RK3588 VOP2_VERSION(0x40, 0x17, 0x6786)

#define ROCKCHIP_MAX_CRTC 4

#define VOP2_LAYER_MAX   8
#define VOP2_SOC_VARIANT 4

#define VOP2_MAX_VP_OUTPUT_WIDTH 4096

/* KHz */
#define VOP2_MAX_DCLK_RATE 600000

/*
 * vop2 dsc id
 */
#define ROCKCHIP_VOP2_DSC_8K 0
#define ROCKCHIP_VOP2_DSC_4K 1

/*
 * vop2 internal power domain id,
 * should be all none zero, 0 will be
 * treat as invalid;
 */
#define VOP2_PD_CLUSTER0 HAL_BIT(0)
#define VOP2_PD_CLUSTER1 HAL_BIT(1)
#define VOP2_PD_CLUSTER2 HAL_BIT(2)
#define VOP2_PD_CLUSTER3 HAL_BIT(3)
#define VOP2_PD_DSC_8K   HAL_BIT(5)
#define VOP2_PD_DSC_4K   HAL_BIT(6)
#define VOP2_PD_ESMART   HAL_BIT(7)
#define VOP2_PD_CLUSTER  HAL_BIT(8)

#define VOP2_PLANE_NO_SCALING HAL_BIT(16)

#define VOP_FEATURE_OUTPUT_10BIT HAL_BIT(0)
#define VOP_FEATURE_AFBDC        HAL_BIT(1)
#define VOP_FEATURE_ALPHA_SCALE  HAL_BIT(2)
#define VOP_FEATURE_HDR10        HAL_BIT(3)
#define VOP_FEATURE_NEXT_HDR     HAL_BIT(4)
/* a feature to splice two windows and two vps to support resolution > 4096 */
#define VOP_FEATURE_SPLICE      HAL_BIT(5)
#define VOP_FEATURE_OVERSCAN    HAL_BIT(6)
#define VOP_FEATURE_VIVID_HDR   HAL_BIT(7)
#define VOP_FEATURE_POST_ACM    HAL_BIT(8)
#define VOP_FEATURE_POST_CSC    HAL_BIT(9)
#define VOP_FEATURE_POST_FRC_V2 HAL_BIT(10)
#define VOP_FEATURE_POST_SHARP  HAL_BIT(11)

/*
 * display output interface supported by rockchip lcdc
 */
#define ROCKCHIP_OUT_MODE_P888       0
#define ROCKCHIP_OUT_MODE_BT1120     0
#define ROCKCHIP_OUT_MODE_P666       1
#define ROCKCHIP_OUT_MODE_P565       2
#define ROCKCHIP_OUT_MODE_BT656      5
#define ROCKCHIP_OUT_MODE_S666       9
#define ROCKCHIP_OUT_MODE_S888       8
#define ROCKCHIP_OUT_MODE_YUV422     9
#define ROCKCHIP_OUT_MODE_S565       10
#define ROCKCHIP_OUT_MODE_S888_DUMMY 12
#define ROCKCHIP_OUT_MODE_YUV420     14
/* for use special outface */
#define ROCKCHIP_OUT_MODE_AAAA 15

#define VOP_OUTPUT_IF_RGB    HAL_BIT(0)
#define VOP_OUTPUT_IF_BT1120 HAL_BIT(1)
#define VOP_OUTPUT_IF_BT656  HAL_BIT(2)
#define VOP_OUTPUT_IF_LVDS0  HAL_BIT(3)
#define VOP_OUTPUT_IF_LVDS1  HAL_BIT(4)
#define VOP_OUTPUT_IF_MIPI0  HAL_BIT(5)
#define VOP_OUTPUT_IF_MIPI1  HAL_BIT(6)
#define VOP_OUTPUT_IF_eDP0   HAL_BIT(7)
#define VOP_OUTPUT_IF_eDP1   HAL_BIT(8)
#define VOP_OUTPUT_IF_DP0    HAL_BIT(9)
#define VOP_OUTPUT_IF_DP1    HAL_BIT(10)
#define VOP_OUTPUT_IF_HDMI0  HAL_BIT(11)
#define VOP_OUTPUT_IF_HDMI1  HAL_BIT(12)
#define VOP_OUTPUT_IF_DP2    HAL_BIT(13)

#define ROCKCHIP_OUTPUT_DUAL_CHANNEL_LEFT_RIGHT_MODE HAL_BIT(0)
#define ROCKCHIP_OUTPUT_DUAL_CHANNEL_ODD_EVEN_MODE   HAL_BIT(1)
#define ROCKCHIP_OUTPUT_DATA_SWAP                    HAL_BIT(2)
#define ROCKCHIP_OUTPUT_MIPI_DS_MODE                 HAL_BIT(3)

#define WIN_FEATURE_HDR2SDR      HAL_BIT(0)
#define WIN_FEATURE_SDR2HDR      HAL_BIT(1)
#define WIN_FEATURE_PRE_OVERLAY  HAL_BIT(2)
#define WIN_FEATURE_AFBDC        HAL_BIT(3)
#define WIN_FEATURE_CLUSTER_MAIN HAL_BIT(4)
#define WIN_FEATURE_CLUSTER_SUB  HAL_BIT(5)
/* a mirror win can only get fb address
 * from source win:
 * Cluster1---->Cluster0
 * Esmart1 ---->Esmart0
 * Smart1  ---->Smart0
 * This is a feather on rk3566
 */
#define WIN_FEATURE_MIRROR          HAL_BIT(6)
#define WIN_FEATURE_MULTI_AREA      HAL_BIT(7)
#define WIN_FEATURE_Y2R_13BIT_DEPTH HAL_BIT(8)
#define WIN_FEATURE_DCI             HAL_BIT(9)

#define V4L2_COLORSPACE_BT709F  0xfe
#define V4L2_COLORSPACE_BT2020F 0xff

/** Add brief to here */
/***************************** Structure Definition **************************/
/**
 *  Add multi line brief to here
 *  ...
 */

/*
 *  the delay number of a window in different mode.
 */
typedef enum {
    ROCKCHIP_VOP_VP0 = 0,
    ROCKCHIP_VOP_VP1,
    ROCKCHIP_VOP_VP2,
    ROCKCHIP_VOP_VP3,
} eVOP2_vpId;

typedef enum {
    CSC_BT601L,
    CSC_BT709L,
    CSC_BT601F,
    CSC_BT2020,
    CSC_BT709L_13BIT,
    CSC_BT709F_13BIT,
    CSC_BT2020L_13BIT,
    CSC_BT2020F_13BIT,
} eVOP2_cscFormat;

typedef enum {
    CSC_10BIT_DEPTH,
    CSC_13BIT_DEPTH,
} eVOP2_cscBitDepth;

typedef enum {
    HSYNC_POSITIVE = 0,
    VSYNC_POSITIVE = 1,
    DEN_NEGATIVE   = 2,
    DCLK_INVERT    = 3
} eVOP2_pol;

typedef enum {
    BCSH_OUT_MODE_BLACK,
    BCSH_OUT_MODE_BLUE,
    BCSH_OUT_MODE_COLOR_BAR,
    BCSH_OUT_MODE_NORMAL_VIDEO,
} eVOP2_bcshOutMode;

typedef enum {
    RGB888_TO_RGB565 = 0x0,
    RGB888_TO_RGB666 = 0x1
} eVOP2_ditherDownMode;

typedef enum {
    DITHER_DOWN_ALLEGRO = 0x0,
    DITHER_DOWN_FRC     = 0x1
} eVOP2_ditherDownModeSel;

typedef enum {
    VOP2_DLY_MODE_DEFAULT,   /**< default mode */
    VOP2_DLY_MODE_HISO_S,    /** HDR in SDR out mode, as a SDR window */
    VOP2_DLY_MODE_HIHO_H,    /** HDR in HDR out mode, as a HDR window */
    VOP2_DLY_MODE_MAX,
} eVOP2_winDlyMode;

/* This define must same with kernel win phy id */
typedef enum {
    ROCKCHIP_VOP2_CLUSTER0 = 0,
    ROCKCHIP_VOP2_CLUSTER1,
    ROCKCHIP_VOP2_ESMART0,
    ROCKCHIP_VOP2_ESMART1,
    ROCKCHIP_VOP2_SMART0,
    ROCKCHIP_VOP2_SMART1,
    ROCKCHIP_VOP2_CLUSTER2,
    ROCKCHIP_VOP2_CLUSTER3,
    ROCKCHIP_VOP2_ESMART2,
    ROCKCHIP_VOP2_ESMART3,
    ROCKCHIP_VOP2_LAYER_MAX,
} eVOP2_layerPhyId;

typedef enum {
    ALPHA_STRAIGHT,
    ALPHA_INVERSE,
} eVOP2_alphaMode;

typedef enum {
    ALPHA_GLOBAL,
    ALPHA_PER_PIX,
    ALPHA_PER_PIX_GLOBAL,
} eVOP2_globalBlendMode;

typedef enum {
    ALPHA_SATURATION,
    ALPHA_NO_SATURATION,
} eVOP2_alphaCalMode;

typedef enum {
    ALPHA_SRC_PRE_MUL,
    ALPHA_SRC_NO_PRE_MUL,
} eVOP2_colorMode;

typedef enum {
    ALPHA_ZERO,
    ALPHA_ONE,
    ALPHA_SRC,
    ALPHA_SRC_INVERSE,
    ALPHA_SRC_GLOBAL,
    ALPHA_DST_GLOBAL,
} eVOP2_factorMode;

typedef enum {
    SRC_FAC_ALPHA_ZERO,
    SRC_FAC_ALPHA_ONE,
    SRC_FAC_ALPHA_DST,
    SRC_FAC_ALPHA_DST_INVERSE,
    SRC_FAC_ALPHA_SRC,
    SRC_FAC_ALPHA_SRC_GLOBAL,
} eVOP2_srcFactorMode;

typedef enum {
    DST_FAC_ALPHA_ZERO,
    DST_FAC_ALPHA_ONE,
    DST_FAC_ALPHA_SRC,
    DST_FAC_ALPHA_SRC_INVERSE,
    DST_FAC_ALPHA_DST,
    DST_FAC_ALPHA_DST_GLOBAL,
} eVOP2_dstFactorMode;

typedef enum {
    VOP2_SCALE_UP_NRST_NBOR,
    VOP2_SCALE_UP_BIL,
    VOP2_SCALE_UP_BIC,
    VOP2_SCALE_UP_ZME,
} eVOP2_scaleUpMode;

typedef enum {
    VOP2_SCALE_DOWN_NRST_NBOR,
    VOP2_SCALE_DOWN_BIL,
    VOP2_SCALE_DOWN_AVG,
    VOP2_SCALE_DOWN_ZME,
} eVOP2_scaleDownMode;

typedef enum {
    SCALE_NONE = 0x0,
    SCALE_UP   = 0x1,
    SCALE_DOWN = 0x2
} eVOP2_scaleMode;

typedef enum {
    VOP2_VP0,
    VOP2_VP1,
    VOP2_VP2,
    VOP2_VP3,
    VOP2_VP_MAX,
} eVOP2_videoPortsId;

typedef enum {
    VOP2_FMT_ARGB8888 = 0,
    VOP2_FMT_RGB888,
    VOP2_FMT_RGB565,
    VOP2_FMT_XRGB101010,
    VOP2_FMT_YUV420SP,
    VOP2_FMT_YUV422SP,
    VOP2_FMT_YUV444SP,
    VOP2_FMT_YUYV422 = 8,
    VOP2_FMT_YUYV420,
    VOP2_FMT_VYUY422,
    VOP2_FMT_VYUY420,
    VOP2_FMT_YUV420SP_TILE_8x4 = 0x10,
    VOP2_FMT_YUV420SP_TILE_16x2,
    VOP2_FMT_YUV422SP_TILE_8x4,
    VOP2_FMT_YUV422SP_TILE_16x2,
    VOP2_FMT_YUV420SP_10,
    VOP2_FMT_YUV422SP_10,
    VOP2_FMT_YUV444SP_10,
} eVOP2_dataFormat;

typedef enum {
    VOP2_AFBC_FMT_RGB565,
    VOP2_AFBC_FMT_ARGB2101010 = 2,
    VOP2_AFBC_FMT_YUV420_10BIT,
    VOP2_AFBC_FMT_RGB888,
    VOP2_AFBC_FMT_ARGB8888,
    VOP2_AFBC_FMT_YUV420       = 9,
    VOP2_AFBC_FMT_YUV422       = 0xb,
    VOP2_AFBC_FMT_YUV422_10BIT = 0xe,
    VOP2_AFBC_FMT_INVALID      = -1,
} eVOP2_afbcFormat;

typedef enum {
    VOP2_TILED_8X8_FMT_YUV420SP = 0xc,
    VOP2_TILED_8X8_FMT_YUV422SP,
    VOP2_TILED_8X8_FMT_YUV444SP,
    VOP2_TILED_8X8_FMT_YUV400SP,
    VOP2_TILED_8X8_FMT_YUV420SP_10 = 0x1c,
    VOP2_TILED_8X8_FMT_YUV422SP_10,
    VOP2_TILED_8X8_FMT_YUV444SP_10,
    VOP2_TILED_8X8_FMT_YUV400SP_10,
    VOP2_TILED_FMT_INVALID = -1,
} eVOP2_tiledFormat;

typedef enum {
    VOP3_TILED_4X4_FMT_YUV420SP = 0xc,
    VOP3_TILED_4X4_FMT_YUV422SP,
    VOP3_TILED_4X4_FMT_YUV444SP,
    VOP3_TILED_4X4_FMT_YUV400SP,
    VOP3_TILED_4X4_FMT_YUV420SP_10 = 0x1c,
    VOP3_TILED_4X4_FMT_YUV422SP_10,
    VOP3_TILED_4X4_FMT_YUV444SP_10,
    VOP3_TILED_4X4_FMT_YUV400SP_10,

    VOP3_TILED_8X8_FMT_YUV420SP = 0x2c,
    VOP3_TILED_8X8_FMT_YUV422SP,
    VOP3_TILED_8X8_FMT_YUV444SP,
    VOP3_TILED_8X8_FMT_YUV400SP,
    VOP3_TILED_8X8_FMT_YUV420SP_10 = 0x3c,
    VOP3_TILED_8X8_FMT_YUV422SP_10,
    VOP3_TILED_8X8_FMT_YUV444SP_10,
    VOP3_TILED_8X8_FMT_YUV400SP_10,

    VOP3_TILED_FMT_INVALID = -1,
} eVOP3_tiledFormat;

typedef enum {
    ROCKCHIP_FMT_ARGB8888 = 0,
    ROCKCHIP_FMT_RGB888,
    ROCKCHIP_FMT_RGB565,
    ROCKCHIP_FMT_YUV420SP = 4,
    ROCKCHIP_FMT_YUV422SP,
    ROCKCHIP_FMT_YUV444SP,
} eDISPLAY_dataFormat;

typedef enum {
    DISPLAY_COLOR_YCBCR_BT601,
    DISPLAY_COLOR_YCBCR_BT709,
    DISPLAY_COLOR_YCBCR_BT2020,
    DISPLAY_COLOR_ENCODING_MAX,
} eDISPLAY_colorEncoding;

typedef enum {
    DISPLAY_COLOR_YCBCR_LIMITED_RANGE,
    DISPLAY_COLOR_YCBCR_FULL_RANGE,
    DISPLAY_COLOR_RANGE_MAX,
} eDISPLAY_colorRange;

typedef enum {
    DISPLAY_PLANE_TYPE_OVERLAY,
    DISPLAY_PLANE_TYPE_PRIMARY,
    DISPLAY_PLANE_TYPE_CURSOR,
} eDISPLAY_planeType;

struct VOP_RECT {
    int width;
    int height;
};

struct VOP2_VIDEO_PORT_DATA {
    uint8_t id;
    uint8_t spliceVpId;
    uint16_t lutDmaRid;
    uint16_t gammaLutLen;
    uint16_t cubicLutLen;
    uint32_t feature;
    uint32_t dclkMax;
    uint64_t socId[VOP2_SOC_VARIANT];
    struct VOP_RECT maxOutput;

    const uint8_t preScanMaxDly[4];
    const uint8_t hdrvividDly[10];
    const uint32_t sdr2hdrDly;
    const uint32_t layerMixDly;
    const uint32_t hdrMixDly;
    const uint32_t winDly;
    const struct VOP_INTR *intr;
    const struct VOP_HDR_TABLE *hdrTable;
};

struct VOP2_LAYER_DATA {
    uint8_t id;
};

struct VOP2_WIN_DATA {
    const char *name;
    uint8_t physId;
    uint8_t spliceWinId;
    uint8_t pdId;
    uint8_t axiId;
    uint8_t axiYrgbId;
    uint8_t axiUvId;
    uint8_t possibleCrtcs;
    uint32_t base;
    uint32_t nformats;
    uint32_t areaSize;
    uint32_t maxUpscaleFactor;
    uint32_t maxDownscaleFactor;
    uint64_t feature;
    eDISPLAY_planeType type;
    /*
     * vertical/horizontal scale up/down filter mode
     */
    const uint8_t hsuFilterMode;
    const uint8_t hsdFilterMode;
    const uint8_t vsuFilterMode;
    const uint8_t vsdFilterMode;
    const uint8_t hsdPreFilterMode;
    const uint8_t vsdPreFilterMode;
    const uint8_t layerSelId[ROCKCHIP_MAX_CRTC];
    const uint8_t winSelPortOffset;
    const uint8_t dly[VOP2_DLY_MODE_MAX];
    const uint32_t supportedRotations;
    const uint64_t *formatModifiers;
    const eDISPLAY_dataFormat *formats;
};

struct VOP_DUMP_REGS {
    uint32_t base;
    const char *name;
    uint32_t stateBase;
    uint32_t stateMask;
    uint32_t stateShift;
    bool enableState;
    uint32_t size;
};

struct VOP2_VP_PLANE_MASK {
    int8_t cursorPlaneId;
    uint8_t primaryPlaneId; /* use this win to show logo */
    uint8_t attachedLayersNr; /* number layers attach to this vp */
    uint8_t attachedLayers[VOP2_LAYER_MAX]; /* the layers attached to this vp */
    uint32_t planeMask;
};

struct VOP2_DSC_DATA {
    uint8_t id;
    uint8_t pdId;
    uint8_t maxSliceNum;
    uint8_t maxLinebufDepth;    /* used to generate the bitstream */
    uint8_t minBitsPerPixel;    /* bit num after encoder compress */
};

struct VOP2_POWER_DOMAIN_DATA {
    uint8_t id;
    uint8_t parentId;
    uint32_t moduleIdMask;
};

struct VOP2_DATA {
    uint8_t nrDscs;
    uint8_t nrDscEcw;
    uint8_t nrDscBufferFlow;
    uint8_t nrMixers;
    uint8_t nrLayers;
    uint8_t nrAxiIntr;
    uint8_t nrGammas;
    uint8_t nrConns;
    uint8_t nrPds;
    uint8_t nrMemPgs;
    uint8_t esmartLbMode;
    int32_t irq;
    uint32_t version;
    uint32_t feature;
    uint32_t nrVps;
    uint32_t dumpRegsSize;
    uint32_t regBase;
    uint32_t regLen;
    uint32_t regsbakLen;
    uint32_t planeMaskBase;
    uint32_t winSize;
    bool delayedPd;
    struct VOP_RECT maxInput;
    struct VOP_RECT maxOutput;

    const struct VOP_INTR *axiIntr;
    const struct VOP2_CTRL *ctrl;
    const struct VOP2_DSC_DATA *dsc;
    const struct DSC_ERROR_INFO *dscErrorEcw;
    const struct DSC_ERROR_INFO *dscErrorBufferFlow;
    const struct VOP2_WIN_DATA *win;
    const struct VOP2_VIDEO_PORT_DATA *vp;
    const struct VOP2_CONNECTOR_IF_DATA *conn;
    const struct VOP2_WB_DATA *wb;
    const struct VOP2_LAYER_DATA *layer;
    const struct VOP2_POWER_DOMAIN_DATA *pd;
    const struct VOP2_POWER_DOMAIN_DATA *memPg;
    const struct VOP_CSC_TABLE *cscTable;
    const struct VOP_HDR_TABLE *hdrTable;
    const struct VOP_GRF_CTRL *sysGrf;
    const struct VOP_GRF_CTRL *grf;
    const struct VOP_GRF_CTRL *vo0Grf;
    const struct VOP_GRF_CTRL *vo1Grf;
    const struct VOP_DUMP_REGS *dumpRegs;
    const struct VOP2_VP_PLANE_MASK *planeMask;
};

struct CRTC_STATE {
    uint8_t vpId;
    uint8_t modeUpdate;
    int32_t outputType;
    int32_t outputMode;
    int32_t output_bpc;
    int32_t leftMargin;
    int32_t rightMargin;
    int32_t topMargin;
    int32_t bottomMargin;
    int32_t vdisplay;
    uint32_t outputFlags;
    uint32_t bindingWinPhyIds[VOP2_LAYER_MAX];
    uint32_t bindingWinNum;
    uint32_t busFormat;
    uint32_t yuvOverlay;
    uint32_t background;
    uint32_t bgOvlDly;
    uint64_t outputIf;
    uint64_t winMask;
    bool spliceMode;
    bool hdrIn;
    bool hdrOut;
    struct DISPLAY_MODE_INFO adjustedMode;
};

struct WIN_RECT {
    int x1, y1, x2, y2;
};

struct PLANE_STATE {
    uint8_t xmirrorEn;
    uint8_t ymirrorEn;
    uint8_t rotateEn90;
    uint8_t rotateEn270;
    uint8_t afbcHalfBlockEn;
    uint8_t tiledEn;
    uint8_t globalAlpha;
    uint8_t physId;
    uint8_t winId;
    int32_t blendMode;
    int32_t format;
    uint32_t yrgbMst;
    uint32_t uvMst;
    uint32_t rbSwap;
    uint32_t uvSwap;
    uint32_t cscMode;
    uint32_t csc13bitEn;
    uint32_t afbcFormat;
    uint32_t afbcRbSwap;
    uint32_t afbcUvSwap;
    uint32_t afbcStride;
    uint32_t transformOffset;
    uint32_t lbMode;
    uint32_t stride;
    uint32_t uvStride;
    uint64_t colorKey;
    uint64_t offset;
    bool afbcEn;
    bool r2yEn;
    bool y2rEn;
    bool isYuv;
    struct WIN_RECT src;
    struct WIN_RECT dst;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup VOP2_Public_Function_Declare Public Function Declare
 *  @{
 */
HAL_Status HAL_VOP2_GetPlatformData(struct VOP2_DATA *pVop2Data);
HAL_Status HAL_VOP2_Init(struct CRTC_STATE *pCrtcState);
HAL_Status HAL_VOP2_SetupDlyForVp(struct CRTC_STATE *pCrtcState);
HAL_Status HAL_VOP2_AtomicEnable(struct CRTC_STATE *pCrtcState);
HAL_Status HAL_VOP2_AtomicDisable(struct CRTC_STATE *pCrtcState);
HAL_Status HAL_VOP2_AtomicUpdateWin(struct CRTC_STATE *pCrtcState, struct PLANE_STATE *pPlaneState);
HAL_Status HAL_VOP2_DisableWin(uint8_t physId);
HAL_Status HAL_VOP2_InitOverlay(uint8_t physId, uint8_t vpId, uint8_t layerId, uint16_t portMuxCfg);
uint32_t HAL_VOP2_IrqHandler(struct CRTC_STATE *pCrtcState);
HAL_Status HAL_VOP2_SetupAlpha(uint8_t mixerId, uint32_t srcColorCtrl, uint32_t dstColorCtrl,
                               uint32_t srcAlphaCtrl, uint32_t dstAlphaCtrl, bool isCluster);
HAL_Check HAL_VOP2_GetWinStatus(uint8_t physId);
HAL_Status HAL_VOP2_DumpRegs(bool isFullDump);
/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_VOP2_MODULE_ENABLED */
