/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023-2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_VOP_MODULE_ENABLED) && defined(RKMCU_RK2118)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup VOP
 *  @{
 */

/** @defgroup VOP_How_To_Use How To Use
 *  @{

 The VOP driver can be used as follows:

 @} */

#include "hal_math.h"

/** @defgroup VOP_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define ALIGN_BIT(x, a) (((x) + (a) - 1) & ~((a) - 1))

/********************* Private Structure Definition **************************/
typedef enum {
    VOP_WIN1,
} eVOP_WinId;

typedef enum {
    DCLK_DISABLE,
    DCLK_ENABLE,
} eVOP_DclkSwitch;

typedef enum {
    RGB888TORGB565,
    RGB888TORGB666,
} eVOP_DitherDownMode;

typedef enum {
    DITHER_DOWN_ALLEGRO,
    DITHER_DOWN_FRC,
} eVOP_DitherDownSel;

typedef enum {
    OUT_MODE_S888       = 8,
    OUT_MODE_S666       = 9,
    OUT_MODE_S565       = 10,
    OUT_MODE_S888_DUMMY = 12,
} eVOP_OutputMode;

/********************* Private Variable Definition ***************************/
static struct VOP_REG s_vopRegMir;
HAL_UNUSED static const char * const VOP_IRQs[] =
{
    "frame start interrupt status!",
    "new frame start interrupt status!",
    "same address interrupt status!",
    "line flag0 interrupt status!",
    "line flag1 interrupt status!",
    "bus error interrupt status!",
    "reserved interrupt status!",
    "win0 empty interrupt status!",
    "win1 empty interrupt status!",
    "win2 empty interrupt status!",
    "display hold interrupt status!"
    "dma finish interrupt status!"
    "post lb empty interrupt status!"
};

HAL_UNUSED static uint32_t VOP_MaskRead(__IO uint32_t hwReg,
                                        uint32_t shift, uint32_t mask)
{
    return (READ_REG(hwReg) & mask) >> shift;
}

HAL_UNUSED static void VOP_MaskWriteNoBackup(__IO uint32_t *mirReg, __IO uint32_t *hwReg,
                                             uint32_t shift, uint32_t mask,
                                             uint32_t v)
{
    uint32_t mirVal = *mirReg;

    v = (mirVal & ~mask) | ((v << shift) & mask);

    WRITE_REG(*hwReg, v);
}

static void VOP_MaskWrite(__IO uint32_t *mirReg, __IO uint32_t *hwReg,
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

    WRITE_REG(*hwReg, v);
}

static void VOP_Write(__IO uint32_t *mirReg, __IO uint32_t *hwReg,
                      uint32_t v)
{
    if (mirReg) {
        *mirReg = v;
    }

    WRITE_REG(*hwReg, v);
}

HAL_UNUSED static uint8_t VOP_GetFormatLength(uint8_t format)
{
    uint8_t val;

    switch (format) {
    case VOP_FMT_ARGB8888:
        val = 32;
        break;
    case VOP_FMT_RGB888:
        val = 24;
        break;
    case VOP_FMT_RGB565:
        val = 16;
        break;
    default:
        val = 8;
        HAL_DBG_ERR("Unsupport format: %d!\n", format);
        break;
    }

    return val;
}

static inline bool VOP_CheckRbSwap(uint16_t busFormat)
{
    /*
     * The default component order of serial formats
     * is BGR. So it is needed to enable RB swap.
     */
    if (busFormat == MEDIA_BUS_FMT_RGB565_2X8_LE ||
        busFormat == MEDIA_BUS_FMT_SRGB666_3X6 ||
        busFormat == MEDIA_BUS_FMT_SRGB888_3X8 ||
        busFormat == MEDIA_BUS_FMT_SRGB888_DUMMY_4X8) {
        return true;
    } else {
        return false;
    }
}

static inline bool IS_Valid_Win(int winId)
{
    if ((winId != VOP_WIN1)) {
        return false;
    } else {
        return true;
    }
}

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup VOP_Exported_Functions_Group2 get VOP Status and Errors Functions

 This section provides functions allowing to get VOP status:

 *  @{
 */
/**
 * @brief  Get VOP scane line number.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
uint32_t HAL_VOP_GetScanLine(struct VOP_REG *pReg)
{
    return 0;
}

/**
 * @brief  Get the config done state before commit.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
uint8_t HAL_VOP_CommitPrepare(struct VOP_REG *pReg)
{
    return VOP_MaskRead(pReg->INTR_STATUS,
                        VOP_INTR_STATUS_DSP_HOLD_VALID_INTR_RAW_STS_SHIFT,
                        VOP_INTR_STATUS_DSP_HOLD_VALID_INTR_RAW_STS_MASK);
}

/**
 * @brief  Get the current commit active state.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
uint8_t HAL_VOP_CommitPost(struct VOP_REG *pReg)
{
    return VOP_MaskRead(pReg->REG_CFG_DONE,
                        VOP_REG_CFG_DONE_REG_LOAD_GLOBAL_EN_SHIFT,
                        VOP_REG_CFG_DONE_REG_LOAD_GLOBAL_EN_MASK);
}

/** @} */

/** @defgroup VOP_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */
/**
 * @brief  Load VOP bpp format lut.
 * @param  pReg: VOP reg base.
 * @param  winId: VOP win id.
 * @param  lut: look up table.
 * @param  lut_size: lut size.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_LoadLut(struct VOP_REG *pReg, uint8_t winId,
                           uint32_t *lut, uint16_t lut_size)
{
    return HAL_OK;
}

/**
 * @brief  Disable VOP bpp format lut.
 * @param  pReg: VOP reg base.
 * @param  winId: VOP win id.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_DisableLut(struct VOP_REG *pReg, uint8_t winId)
{
    return HAL_OK;
}

/**
 * @brief  VOP MIPI switch.
 * @param  pReg: VOP reg base.
 * @param  path: mipi switch direction.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_MipiSwitch(struct VOP_REG *pReg, eVOP_MipiSwitchPath path)
{
    return HAL_OK;
}

/**
 * @brief  Set display area.
 * @param  pReg: VOP reg base.
 * @param  display_rect: display rect.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_SetArea(struct VOP_REG *pReg,
                           struct DISPLAY_RECT *display_rect)
{
    return HAL_OK;
}

/**
 * @brief  Set plane state.
 * @param  pReg: VOP reg base.
 * @param  pWinState: win state.
 * @param  pModeInfo: VOP output modeinfo.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_SetPlane(struct VOP_REG *pReg,
                            struct CRTC_WIN_STATE *pWinState,
                            struct DISPLAY_MODE_INFO *pModeInfo)
{
    uint32_t val, dspInfo, dspSt, dspStx, dspSty;
    uint32_t yStride;
    uint32_t regOffset = 0;
    uint16_t xVirByte = ALIGN_BIT(pWinState->xVir * VOP_GetFormatLength(pWinState->hwFormat), 32) >> 3;

    if (!IS_Valid_Win(pWinState->winId)) {
        return HAL_INVAL;
    }

    dspInfo = (pWinState->crtcH - 1) << 16 | (pWinState->crtcW - 1);

    dspStx = pWinState->crtcX + pModeInfo->crtcHtotal - pModeInfo->crtcHsyncStart;
    dspSty = pWinState->crtcY + pModeInfo->crtcVtotal - pModeInfo->crtcVsyncStart;
    dspSt = dspSty << 16 | dspStx;

    VOP_MaskWrite(&s_vopRegMir.WIN1_CTRL0 + regOffset,
                  &pReg->WIN1_CTRL0 + regOffset,
                  VOP_WIN1_CTRL0_WIN1_DATA_FMT_SHIFT,
                  VOP_WIN1_CTRL0_WIN1_DATA_FMT_MASK, pWinState->hwFormat);

    pWinState->stride = xVirByte >> 2;
    yStride = pWinState->stride;
    VOP_Write(&s_vopRegMir.WIN1_VIR + regOffset,
              &pReg->WIN1_VIR + regOffset, yStride);
    VOP_Write(&s_vopRegMir.WIN1_MST + regOffset,
              &pReg->WIN1_MST + regOffset, pWinState->yrgbAddr);

    VOP_Write(&s_vopRegMir.WIN1_DSP_INFO + regOffset,
              &pReg->WIN1_DSP_INFO + regOffset, dspInfo);

    VOP_Write(&s_vopRegMir.WIN1_DSP_ST + regOffset,
              &pReg->WIN1_DSP_ST + regOffset, dspSt);

    val = pWinState->alphaEn << VOP_WIN1_ALPHA_CTRL_WIN1_ALPHA_EN_SHIFT |
          pWinState->alphaMode << VOP_WIN1_ALPHA_CTRL_WIN1_ALPHA_MODE_SHIFT |
          pWinState->alphaPreMul << VOP_WIN1_ALPHA_CTRL_WIN1_ALPHA_PRE_MUL_SHIFT |
          pWinState->alphaSatMode << VOP_WIN1_ALPHA_CTRL_WIN1_ALPHA_SAT_MODE_SHIFT |
          pWinState->globalAlphaValue << VOP_WIN1_ALPHA_CTRL_WIN1_ALPHA_VALUE_SHIFT;
    VOP_Write(&s_vopRegMir.WIN1_ALPHA_CTRL + regOffset,
              &pReg->WIN1_ALPHA_CTRL + regOffset, val);

    VOP_Write(&s_vopRegMir.WIN1_COLOR_KEY + regOffset,
              &pReg->WIN1_COLOR_KEY + regOffset, pWinState->colorKey);

    VOP_MaskWrite(&s_vopRegMir.WIN1_CTRL0 + regOffset,
                  &pReg->WIN1_CTRL0 + regOffset,
                  VOP_WIN1_CTRL0_WIN1_EN_SHIFT,
                  VOP_WIN1_CTRL0_WIN1_EN_MASK, pWinState->winEn);

    return HAL_OK;
}

/**
 * @brief  Set config done to commit frame.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_Commit(struct VOP_REG *pReg)
{
    VOP_MaskWriteNoBackup(&s_vopRegMir.REG_CFG_DONE, &pReg->REG_CFG_DONE,
                          VOP_REG_CFG_DONE_REG_LOAD_GLOBAL_EN_SHIFT,
                          VOP_REG_CFG_DONE_REG_LOAD_GLOBAL_EN_MASK, 1);

    VOP_MaskWrite(NULL, &pReg->INTR_CLEAR,
                  VOP_INTR_CLEAR_DSP_HOLD_VALID_INTR_CLR_SHIFT,
                  VOP_INTR_CLEAR_DSP_HOLD_VALID_INTR_CLR_MASK, 1);

    VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_HOLD_MODE_SHIFT,
                  VOP_MCU_MCU_HOLD_MODE_MASK, 0);

    return HAL_OK;
}

/**
 * @brief  Set WMS frame start.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_EdpiFrmSt(struct VOP_REG *pReg)
{
    return HAL_OK;
}

/** @} */

/** @defgroup VOP_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 *  @{
 */

/**
 * @brief  VOP init.
 * @param  pReg: VOP reg base.
 * @param  pModeInfo: VOP putput mode info.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_Init(struct VOP_REG *pReg,
                        struct DISPLAY_MODE_INFO *pModeInfo)
{
    uint32_t i = 0;
    uint32_t *regMir = (uint32_t *)&s_vopRegMir;
    uint32_t *vopReg = (uint32_t *)pReg;
    uint32_t regLen = sizeof(s_vopRegMir) >> 2;

    for (i = 0; i < regLen; i++) {
        regMir[i] = vopReg[i];
    }

    VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                  VOP_DSP_CTRL2_DSP_BLANK_EN_SHIFT,
                  VOP_DSP_CTRL2_DSP_BLANK_EN_MASK,
                  0);

    VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                  VOP_SYS_CTRL2_IMD_VOP_STANDBY_EN_SHIFT,
                  VOP_SYS_CTRL2_IMD_VOP_STANDBY_EN_MASK,
                  0);

    return HAL_OK;
}

/**
 * @brief  VOP deinit.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_DeInit(struct VOP_REG *pReg)
{
    VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                  VOP_SYS_CTRL2_IMD_VOP_STANDBY_EN_SHIFT,
                  VOP_SYS_CTRL2_IMD_VOP_STANDBY_EN_MASK,
                  1);

    return HAL_OK;
}

/**
 * @brief  VOP mode init.
 * @param  pReg: VOP reg base.
 * @param  pModeInfo: VOP output mode info.
 * @param  pPostScaleInfo: VOP post scale info.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_ModeInit(struct VOP_REG *pReg,
                            struct DISPLAY_MODE_INFO *pModeInfo,
                            struct VOP_POST_SCALE_INFO *pPostScaleInfo)
{
    uint16_t dspHsync = pModeInfo->crtcHsyncEnd - pModeInfo->crtcHsyncStart;
    uint16_t dspHtotal = pModeInfo->crtcHtotal;
    uint16_t dspHactSt = pModeInfo->crtcHtotal - pModeInfo->crtcHsyncStart;
    uint16_t dspHactEnd = dspHactSt + pModeInfo->crtcHdisplay;

    uint16_t dspVsync = pModeInfo->crtcVsyncEnd - pModeInfo->crtcVsyncStart;
    uint16_t dspVtotal = pModeInfo->crtcVtotal;
    uint16_t dspVactSt = pModeInfo->crtcVtotal - pModeInfo->crtcVsyncStart;
    uint16_t dspVactEnd = dspVactSt + pModeInfo->crtcVdisplay;

    VOP_Write(&s_vopRegMir.DSP_HTOTAL_HS_END, &pReg->DSP_HTOTAL_HS_END,
              dspHtotal << 16 | dspHsync);
    VOP_Write(&s_vopRegMir.DSP_HACT_ST_END, &pReg->DSP_HACT_ST_END,
              dspHactSt << 16 | dspHactEnd);
    VOP_Write(&s_vopRegMir.DSP_VTOTAL_VS_END, &pReg->DSP_VTOTAL_VS_END,
              dspVtotal << 16 | dspVsync);
    VOP_Write(&s_vopRegMir.DSP_VACT_ST_END, &pReg->DSP_VACT_ST_END,
              dspVactSt << 16 | dspVactEnd);

    return HAL_OK;
}

/**
 * @brief  VOP MCU mode init.
 * @param  pReg: VOP reg base.
 * @param  pMcuTiming: VOP mcu timing info.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_McuModeInit(struct VOP_REG *pReg, struct VOP_MCU_TIMING *pMcuTiming)
{
    VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_FRAME_ST_SHIFT,
                  VOP_MCU_MCU_FRAME_ST_MASK, 0);

    VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_CLK_SEL_SHIFT,
                  VOP_MCU_MCU_CLK_SEL_MASK, 1);

    VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_TYPE_SHIFT,
                  VOP_MCU_MCU_TYPE_MASK, 1);

    VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_HOLD_MODE_SHIFT,
                  VOP_MCU_MCU_HOLD_MODE_MASK, 1);

    VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_PIX_TOTAL_SHIFT,
                  VOP_MCU_MCU_PIX_TOTAL_MASK, pMcuTiming->mcuPixelTotal);

    VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_CS_PST_SHIFT,
                  VOP_MCU_MCU_CS_PST_MASK, pMcuTiming->mcuCsPst);

    VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_CS_PEND_SHIFT,
                  VOP_MCU_MCU_CS_PEND_MASK, pMcuTiming->mcuCsPend);

    VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_RW_PST_SHIFT,
                  VOP_MCU_MCU_RW_PST_MASK, pMcuTiming->mcuRwPst);

    VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_RW_PEND_SHIFT,
                  VOP_MCU_MCU_RW_PEND_MASK, pMcuTiming->mcuRwPend);

    return HAL_OK;
}

/**
 * @brief  VOP post scale init.
 * @param  pReg: VOP reg base.
 * @param  pModeInfo: VOP output mode info.
 * @param  pPostScaleInfo: VOP post scale info.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_PostScaleInit(struct VOP_REG *pReg,
                                 struct DISPLAY_MODE_INFO *pModeInfo,
                                 struct VOP_POST_SCALE_INFO *pPostScaleInfo)
{
    return HAL_OK;
}

/**
 * @brief  VOP post BCSH init.
 * @param  pReg: VOP reg base.
 * @param  pBCSHInfo: VOP BCSH info.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_PostBCSH(struct VOP_REG *pReg,
                            struct VOP_BCSH_INFO *pBCSHInfo)
{
    return HAL_OK;
}

/**
 * @brief  VOP post gamma init, the gamma adjust should be in yuv color space,
           so we need to enable bcsh r2y and post csc en.
 * @param  pReg: VOP reg base.
 * @param  pGammaInfo: VOP gamma info.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_PostGammaInit(struct VOP_REG *pReg,
                                 struct VOP_GAMMA_COE_INFO *pGammaInfo)
{
    return HAL_OK;
}

/**
 * @brief  VOP post color matrix init.
 * @param  pReg: VOP reg base.
 * @param  pColorMatrixInfo: VOP color matrix info.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_PostColorMatrixInit(struct VOP_REG *pReg,
                                       struct VOP_COLOR_MATRIX_INFO *pColorMatrixInfo)
{
    return HAL_OK;
}

/**
 * @brief  VOP post clip init.
 * @param  pReg: VOP reg base.
 * @param  pClipInfo: VOP clip info.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_PostClipInit(struct VOP_REG *pReg,
                                struct VOP_POST_CLIP_INFO *pClipInfo)
{
    return HAL_OK;
}

/**
 * @brief  VOP polarity init.
 * @param  pReg: VOP reg base.
 * @param  pModeInfo: VOP output mode info.
 * @param  ConnectorType: output connector type.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_PolarityInit(struct VOP_REG *pReg,
                                struct DISPLAY_MODE_INFO *pModeInfo,
                                uint16_t ConnectorType)
{
    uint16_t polarity;

    polarity = DCLK_ENABLE;
    polarity |= (pModeInfo->flags & VIDEO_MODE_FLAG_NPIXDATA ? 1 : 0) << 1;
    polarity |= (pModeInfo->flags & VIDEO_MODE_FLAG_NHSYNC ? 1 : 0) << 2;
    polarity |= (pModeInfo->flags & VIDEO_MODE_FLAG_NVSYNC ? 1 : 0) << 3;
    polarity |= (pModeInfo->flags & VIDEO_MODE_FLAG_DEN ? 1 : 0) << 4;
    switch (ConnectorType) {
    case RK_DISPLAY_CONNECTOR_RGB:
        VOP_Write(&s_vopRegMir.DSP_CTRL0, &pReg->DSP_CTRL0,
                  polarity);
        break;
    default:
        HAL_DBG_ERR("Unknown Connector Type: %d\n", ConnectorType);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  VOP output busformat init.
 * @param  pReg: VOP reg base.
 * @param  pModeInfo: VOP output mode info.
 * @param  BusFormat: output busformat.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_OutputInit(struct VOP_REG *pReg,
                              struct DISPLAY_MODE_INFO *pModeInfo,
                              uint16_t BusFormat)
{
    if (VOP_CheckRbSwap(BusFormat)) {
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DSP_RB_SWAP_SHIFT,
                      VOP_DSP_CTRL2_DSP_RB_SWAP_MASK,
                      1);
    } else {
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DSP_RB_SWAP_SHIFT,
                      VOP_DSP_CTRL2_DSP_RB_SWAP_MASK,
                      0);
    }

    switch (BusFormat) {
    case MEDIA_BUS_FMT_RGB565_2X8_LE:
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DITHER_DOWN_SHIFT,
                      VOP_DSP_CTRL2_DITHER_DOWN_MASK,
                      1);
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DITHER_DOWN_MODE_SHIFT,
                      VOP_DSP_CTRL2_DITHER_DOWN_MODE_MASK,
                      RGB888TORGB565);
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DSP_OUT_MODE_SHIFT,
                      VOP_DSP_CTRL2_DSP_OUT_MODE_MASK,
                      OUT_MODE_S565);
        break;
    case MEDIA_BUS_FMT_SRGB666_3X6:
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DITHER_DOWN_SHIFT,
                      VOP_DSP_CTRL2_DITHER_DOWN_MASK,
                      1);
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DITHER_DOWN_MODE_SHIFT,
                      VOP_DSP_CTRL2_DITHER_DOWN_MODE_MASK,
                      RGB888TORGB666);
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DSP_OUT_MODE_SHIFT,
                      VOP_DSP_CTRL2_DSP_OUT_MODE_MASK,
                      OUT_MODE_S666);
        break;
    case MEDIA_BUS_FMT_SRGB888_3X8:
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DITHER_DOWN_SHIFT,
                      VOP_DSP_CTRL2_DITHER_DOWN_MASK,
                      0);
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DSP_OUT_MODE_SHIFT,
                      VOP_DSP_CTRL2_DSP_OUT_MODE_MASK,
                      OUT_MODE_S888);
        break;
    case MEDIA_BUS_FMT_SRGB888_DUMMY_4X8:
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DITHER_DOWN_SHIFT,
                      VOP_DSP_CTRL2_DITHER_DOWN_MASK,
                      0);
        VOP_MaskWrite(&s_vopRegMir.DSP_CTRL2, &pReg->DSP_CTRL2,
                      VOP_DSP_CTRL2_DSP_OUT_MODE_SHIFT,
                      VOP_DSP_CTRL2_DSP_OUT_MODE_MASK,
                      OUT_MODE_S888_DUMMY);
        break;
    default:
        HAL_DBG_ERR("Unknown Bus Format: %d\n", BusFormat);
        break;
    }

    return HAL_OK;
}

/**
 * @brief  VOP edpi init.
 * @param  pReg: VOP reg base.
 * @param  triggerMode: VOP new frame trigger mode,default is 0,
 *         0: TE trigger, VOP commit new frame time depend on TE + wms_st
 *         1: gpio trigger, VOP commit new frame time depend on gpio connect to panel TE pin
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_EdpiInit(struct VOP_REG *pReg, uint8_t triggerMode)
{
    return HAL_OK;
}

/**
 * @brief  Config VOP NOC QOS value.
 * @param  pReg: VOP reg base.
 * @param  nocQosValue: NOC qos value.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_NocQosInit(struct VOP_REG *pReg, uint32_t nocQosValue)
{
    VOP_MaskWrite(&s_vopRegMir.SYS_CTRL1, &pReg->SYS_CTRL1,
                  VOP_SYS_CTRL1_SW_NOC_QOS_EN_SHIFT,
                  VOP_SYS_CTRL1_SW_NOC_QOS_EN_MASK, 1);
    VOP_MaskWrite(&s_vopRegMir.SYS_CTRL1, &pReg->SYS_CTRL1,
                  VOP_SYS_CTRL1_SW_NOC_QOS_VALUE_SHIFT,
                  VOP_SYS_CTRL1_SW_NOC_QOS_VALUE_MASK, nocQosValue);

    return HAL_OK;
}

/**
 * @brief  Config VOP NOC hurry threshold value.
 * @param  pReg: VOP reg base.
 * @param  hurryValue: hurry value.
 * @param  hurryThreshold: hurry threshold.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_NocHurryInit(struct VOP_REG *pReg, uint32_t hurryValue,
                                uint32_t hurryThreshold)
{
    VOP_MaskWrite(&s_vopRegMir.SYS_CTRL1, &pReg->SYS_CTRL1,
                  VOP_SYS_CTRL1_SW_NOC_HURRY_EN_SHIFT,
                  VOP_SYS_CTRL1_SW_NOC_HURRY_EN_MASK, 1);
    VOP_MaskWrite(&s_vopRegMir.SYS_CTRL1, &pReg->SYS_CTRL1,
                  VOP_SYS_CTRL1_SW_NOC_HURRY_VALUE_SHIFT,
                  VOP_SYS_CTRL1_SW_NOC_HURRY_VALUE_MASK, hurryValue);
    VOP_MaskWrite(&s_vopRegMir.SYS_CTRL1, &pReg->SYS_CTRL1,
                  VOP_SYS_CTRL1_SW_NOC_HURRY_THRESHOLD_SHIFT,
                  VOP_SYS_CTRL1_SW_NOC_HURRY_THRESHOLD_MASK,
                  hurryThreshold);

    return HAL_OK;
}

/**
 * @brief  Config VOP AXI outstand number.
 * @param  pReg: VOP reg base.
 * @param  outStandNum: AXI outstand number.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_AxiOutstandInit(struct VOP_REG *pReg, uint32_t outStandNum)
{
    VOP_MaskWrite(&s_vopRegMir.SYS_CTRL1, &pReg->SYS_CTRL1,
                  VOP_SYS_CTRL1_SW_AXI_MAX_OUTSTAND_EN_SHIFT,
                  VOP_SYS_CTRL1_SW_AXI_MAX_OUTSTAND_EN_MASK, 1);
    VOP_MaskWrite(&s_vopRegMir.SYS_CTRL1, &pReg->SYS_CTRL1,
                  VOP_SYS_CTRL1_SW_AXI_MAX_OUTSTAND_NUM_SHIFT,
                  VOP_SYS_CTRL1_SW_AXI_MAX_OUTSTAND_NUM_MASK, outStandNum);

    return HAL_OK;
}

/** @} */

/** @defgroup VOP_Exported_Functions_Group5 Other Functions

 This section provides functions allowing to control uart:

 *  @{
 */
/**
 * @brief  Enable VOP frame start interrupt.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_EnableFsIrq(struct VOP_REG *pReg)
{
    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_FS0_INTR_EN_SHIFT,
                  VOP_INTR_EN_FS0_INTR_EN_MASK, 1);

    return HAL_OK;
}

/**
 * @brief  Disable VOP frame start interrupt.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_DisableFsIrq(struct VOP_REG *pReg)
{
    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_FS0_INTR_EN_SHIFT,
                  VOP_INTR_EN_FS0_INTR_EN_MASK, 0);

    return HAL_OK;
}

/**
 * @brief  Enable VOP frame finish interrupt.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_EnableFrmFshIrq(struct VOP_REG *pReg)
{
    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_DMA_FRM_FSH_INTR_EN_SHIFT,
                  VOP_INTR_EN_DMA_FRM_FSH_INTR_EN_MASK, 1);

    return HAL_OK;
}

/**
 * @brief  Disable VOP frame finish interrupt.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_DisableFrmFshIrq(struct VOP_REG *pReg)
{
    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_DMA_FRM_FSH_INTR_EN_SHIFT,
                  VOP_INTR_EN_DMA_FRM_FSH_INTR_EN_MASK, 0);

    return HAL_OK;
}

/**
 * @brief  Enable VOP line flag interrupt.
 * @param  pReg: VOP reg base.
 * @param  lineFlag0: Line flag0 position.
 * @param  lineFlag1: Line flag1 position.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_EnableLineIrq(struct VOP_REG *pReg,
                                 uint32_t lineFlag0, uint32_t lineFlag1)
{
    VOP_MaskWrite(NULL, &pReg->INTR_EN,
                  VOP_INTR_EN_LINE_FLAG0_INTR_EN_SHIFT,
                  VOP_INTR_EN_LINE_FLAG0_INTR_EN_MASK, 1);

    VOP_MaskWrite(NULL, &pReg->INTR_EN,
                  VOP_INTR_EN_LINE_FLAG1_INTR_EN_SHIFT,
                  VOP_INTR_EN_LINE_FLAG1_INTR_EN_MASK, 1);

    VOP_Write(&s_vopRegMir.LINE_FLAG, &pReg->LINE_FLAG,
              lineFlag1 << 16 | lineFlag0);

    return HAL_OK;
}

/**
 * @brief  Disable VOP line flag interrupt.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_DisableLineIrq(struct VOP_REG *pReg)
{
    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_LINE_FLAG0_INTR_EN_SHIFT,
                  VOP_INTR_EN_LINE_FLAG0_INTR_EN_MASK, 0);

    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_LINE_FLAG1_INTR_EN_SHIFT,
                  VOP_INTR_EN_LINE_FLAG1_INTR_EN_MASK, 0);

    return HAL_OK;
}

/**
 * @brief  Enable VOP debug interrupt.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_EnableDebugIrq(struct VOP_REG *pReg)
{
    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_BUS_ERROR_INTR_EN_SHIFT,
                  VOP_INTR_EN_BUS_ERROR_INTR_EN_MASK, 1);
    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_WIN1_EMPTY_INTR_EN_SHIFT,
                  VOP_INTR_EN_WIN1_EMPTY_INTR_EN_MASK, 1);

    return HAL_OK;
}

/**
 * @brief  Disable VOP debug interrupt.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_DisableDebugIrq(struct VOP_REG *pReg)
{
    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_BUS_ERROR_INTR_EN_SHIFT,
                  VOP_INTR_EN_BUS_ERROR_INTR_EN_MASK, 0);
    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_WIN1_EMPTY_INTR_EN_SHIFT,
                  VOP_INTR_EN_WIN1_EMPTY_INTR_EN_SHIFT, 0);

    return HAL_OK;
}

/**
 * @brief  Enable VOP DSP hold interrupt.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_EnableDspHoldIrq(struct VOP_REG *pReg)
{
    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_DSP_HOLD_VALID_INTR_EN_SHIFT,
                  VOP_INTR_EN_DSP_HOLD_VALID_INTR_EN_MASK, 1);

    return HAL_OK;
}

/**
 * @brief  Disable VOP DSP hold interrupt.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_DisableDspHoldIrq(struct VOP_REG *pReg)
{
    VOP_MaskWrite(NULL, &pReg->INTR_EN, VOP_INTR_EN_DSP_HOLD_VALID_INTR_EN_SHIFT,
                  VOP_INTR_EN_DSP_HOLD_VALID_INTR_EN_MASK, 0);

    return HAL_OK;
}

/**
 * @brief  VOP interrupt handler.
 * @param  pReg: VOP reg base.
 * @return HAL_Status.
 */
uint32_t HAL_VOP_IrqHandler(struct VOP_REG *pReg)
{
    uint32_t i, val;

    val = pReg->INTR_STATUS;

    for (i = 0; i < HAL_ARRAY_SIZE(VOP_IRQs); i++) {
        /**
         * if (val & HAL_BIT(i))
         *     HAL_DBG_ERR("VOP Irq: %s\n", VOP_IRQs[i]);
         */
    }

    pReg->INTR_CLEAR = val & 0xffff;

    return val & 0xffff;
}

/**
 * @brief  VOP interrupt handler.
 * @param  pReg: VOP reg base.
 * @param  type: send cmd or data
 * @param  val: send value
 * @return HAL_Status.
 */
HAL_Status HAL_VOP_SendMcuCmd(struct VOP_REG *pReg, uint8_t type, uint32_t val)
{
    uint32_t regVal;

    switch (type) {
    case MCU_WRCMD:
        VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_RS_SHIFT,
                      VOP_MCU_MCU_RS_MASK, 0);
        VOP_Write(&s_vopRegMir.MCU_RW_BYPASS_PORT, &pReg->MCU_RW_BYPASS_PORT,
                  val);
        VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_RS_SHIFT,
                      VOP_MCU_MCU_RS_MASK, 1);
        break;
    case MCU_WRDATA:
        VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_RS_SHIFT,
                      VOP_MCU_MCU_RS_MASK, 1);
        VOP_Write(&s_vopRegMir.MCU_RW_BYPASS_PORT, &pReg->MCU_RW_BYPASS_PORT,
                  val);
        break;
    case MCU_RDDATA:
        VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_RS_SHIFT,
                      VOP_MCU_MCU_RS_MASK, 1);
        regVal = VOP_MaskRead(pReg->MCU_RW_BYPASS_PORT,
                              VOP_MCU_RW_BYPASS_PORT_MCU_WRITE_DATA_BYPASS_SHIFT,
                              VOP_MCU_RW_BYPASS_PORT_MCU_WRITE_DATA_BYPASS_MASK);
        HAL_DBG("Mcu read 0x%08" PRIx32 ": 0x%08" PRIx32 "\n", val, regVal);
        break;
    case MCU_SETBYPASS:
        VOP_MaskWrite(&s_vopRegMir.MCU, &pReg->MCU, VOP_MCU_MCU_BYPASS_SHIFT,
                      VOP_MCU_MCU_BYPASS_MASK, val ? 1 : 0);
        break;
    default:
        break;
    }

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_VOP_MODULE_ENABLED */
