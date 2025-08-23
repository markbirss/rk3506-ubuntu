/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_DW_MIPI_DSI_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup DW_MIPI_DSI
 *  @{
 */

/** @defgroup DW_MIPI_DSI_How_To_Use How To Use
 *  @{

 The DW MIPI DSI driver can be used as follows:

   - Initialize the MIPI DSI interface using HAL_DW_MIPI_DSI_Init():
      - Provide the base register address.
      - Specify the data rate in Mbps for the lane.

   - Configure message transmission modes:
      - Invoke HAL_DW_MIPI_DSI_MsgHsModeConfig() for High-Speed mode.
      - Invoke HAL_DW_MIPI_DSI_MsgLpModeConfig() for Low-Power mode.

   - Enable and disable the MIPI DSI interface:
      - Use HAL_DW_MIPI_DSI_Enable() to activate the interface.
      - Use HAL_DW_MIPI_DSI_Disable() to deactivate the interface.

   - Send packets via the MIPI DSI interface:
      - Utilize HAL_DW_MIPI_DSI_SendPacket() to send packets with specified data type and payload.

   - Configure various modes and timings:
      - Use HAL_DW_MIPI_DSI_ModeConfig() to configure display mode.
      - Use HAL_DW_MIPI_DSI_PacketHandlerConfig() to configure packet handling.
      - Configure vertical timing with HAL_DW_MIPI_DSI_VerticalTimingConfig().
      - Configure DPI with HAL_DW_MIPI_DSI_DpiConfig().
      - Configure line timer with HAL_DW_MIPI_DSI_LineTimerConfig().

   - Manage power and error handling:
      - Initialize the PHY with HAL_DW_MIPI_DSI_PhyInit().
      - Power on the PHY with HAL_DW_MIPI_DSI_PhyPowerOn().
      - Power off the PHY with HAL_DW_MIPI_DSI_PhyPowerOff().
      - Clear errors with HAL_DW_MIPI_DSI_ClrErr().

 @} */

/** @defgroup DW_MIPI_DSI_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
#define CMD_PKT_STATUS_TIMEOUT_RETRIES 100

/********************* Private Structure Definition **************************/
typedef enum {
    DPI_COLOR_CODING_16BIT_1,
    DPI_COLOR_CODING_16BIT_2,
    DPI_COLOR_CODING_16BIT_3,
    DPI_COLOR_CODING_18BIT_1,
    DPI_COLOR_CODING_18BIT_2,
    DPI_COLOR_CODING_24BIT,
} eDW_MIPI_DSI_dpiColorCoding;

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/
static uint32_t DW_MIPI_DSI_CheckFifoStatus(struct MIPI_DSI_REG *pReg, uint32_t flag)
{
    return READ_BIT(pReg->CMD_PKT_STATUS, flag);
}

static HAL_Status DW_MIPI_DSI_WaitFifoNotFull(struct MIPI_DSI_REG *pReg, uint32_t flag)
{
    uint32_t retries = 0;

    while (DW_MIPI_DSI_CheckFifoStatus(pReg, flag)) {
        if (++retries > CMD_PKT_STATUS_TIMEOUT_RETRIES) {
            HAL_DBG_ERR("%s: flag:%" PRIx32 " timeout for wait not full\n", __func__, flag);

            return HAL_TIMEOUT;
        }
        HAL_DelayUs(1);
    }

    return HAL_OK;
}

static HAL_Status DW_MIPI_DSI_WaitFifoEmpty(struct MIPI_DSI_REG *pReg)
{
    uint32_t retries = 0;

    while (!DW_MIPI_DSI_CheckFifoStatus(pReg, MIPI_DSI_CMD_PKT_STATUS_GEN_CMD_EMPTY_MASK)) {
        if (++retries > CMD_PKT_STATUS_TIMEOUT_RETRIES) {
            HAL_DBG_ERR("%s: timeout for wait empty\n", __func__);

            return HAL_TIMEOUT;
        }
        HAL_DelayUs(1);
    }

    return HAL_OK;
}

static uint32_t DW_MIPI_DSI_GetHcomponentLbcc(uint32_t hcomponent, uint16_t laneMbps, uint32_t clk)
{
    uint32_t lbcc, lbccTmp;

    lbccTmp = hcomponent * laneMbps * 1000 / 8;
    lbcc = HAL_DIV_ROUND_UP(lbccTmp, clk);

    return lbcc;
}

/********************* Public Function Definition ***************************/
/**
 * @brief  Send DSI Command Packet.
 * @param  pReg: DSI reg base.
 * @param  dataType: DSI Command Data Type.
 * @param  payloadLen: DSI Command Data Len.
 * @param  payload: DSI Command Data.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_SendPacket(struct MIPI_DSI_REG *pReg, eDW_MIPI_DSI_dataType dataType,
                                      uint8_t payloadLen, const uint8_t *payload)
{
    uint32_t temp, val;
    HAL_Status ret;

    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));
    HAL_ASSERT(dataType != 0);
    HAL_ASSERT(payload != NULL);

    ret = DW_MIPI_DSI_WaitFifoNotFull(pReg, MIPI_DSI_CMD_PKT_STATUS_GEN_CMD_FULL_MASK);
    if (ret != HAL_OK) {
        return ret;
    }

    val = (0x0 << 6) | dataType;
    switch (dataType) {
    case DW_MIPI_DSI_DCS_COMPRESSION_MODE:
    case DW_MIPI_DSI_DCS_SHORT_WRITE:
    case DW_MIPI_DSI_DCS_SHORT_WRITE_PARAM:
    case DW_MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM:
    case DW_MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM:
    case DW_MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM:
    {
        if (payloadLen > 0) {
            val |= payload[0] << 8;
        }
        if (payloadLen > 1) {
            val |= payload[1] << 16;
        }
        break;
    }
    case DW_MIPI_DSI_DCS_LONG_WRITE:
    case DW_MIPI_DSI_GENERIC_LONG_WRITE:
    case DW_MIPI_DSI_PPS_LONG_WRITE:
    {
        /* Send payload */
        ret = DW_MIPI_DSI_WaitFifoNotFull(pReg, MIPI_DSI_CMD_PKT_STATUS_GEN_PLD_W_FULL_MASK);

        if (ret != HAL_OK) {
            return ret;
        }

        val |= (payloadLen << 8);
        while (payloadLen) {
            if (payloadLen < 4) {
                temp = 0;
                memcpy(&temp, payload, payloadLen);
                WRITE_REG(pReg->GEN_PLD_DATA, temp);
                payloadLen = 0;
            } else {
                memcpy(&temp, payload, sizeof(uint32_t));
                WRITE_REG(pReg->GEN_PLD_DATA, temp);
                payload += 4;
                payloadLen -= 4;
            }
        }
        break;
    }
    default:
        HAL_DBG_ERR("%s, Unsupport dataType:0x%x\n", __func__, dataType);
    }

    /* Send packet header */
    WRITE_REG(pReg->GEN_HDR, val);
    ret = DW_MIPI_DSI_WaitFifoEmpty(pReg);

    return ret;
}

/**
 * @brief  Config DSI Send Message In Hs Mode.
 * @param  pReg: DSI reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_MsgHsModeConfig(struct MIPI_DSI_REG *pReg)
{
    uint32_t lpMask = MIPI_DSI_CMD_MODE_CFG_GEN_SW_0P_TX_MASK | MIPI_DSI_CMD_MODE_CFG_GEN_SW_1P_TX_MASK |
                      MIPI_DSI_CMD_MODE_CFG_GEN_SW_2P_TX_MASK | MIPI_DSI_CMD_MODE_CFG_GEN_SR_0P_TX_MASK |
                      MIPI_DSI_CMD_MODE_CFG_GEN_SR_1P_TX_MASK | MIPI_DSI_CMD_MODE_CFG_GEN_SR_2P_TX_MASK |
                      MIPI_DSI_CMD_MODE_CFG_GEN_LW_TX_MASK | MIPI_DSI_CMD_MODE_CFG_DCS_SW_0P_TX_MASK |
                      MIPI_DSI_CMD_MODE_CFG_DSC_SW_1P_TX_MASK | MIPI_DSI_CMD_MODE_CFG_DCS_SR_0P_TX_MASK |
                      MIPI_DSI_CMD_MODE_CFG_DSC_LW_TX_MASK | MIPI_DSI_CMD_MODE_CFG_MAX_RD_PKT_SIZE_MASK;

    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));

    CLEAR_BIT(pReg->VID_MODE_CFG, MIPI_DSI_CMD_MODE_CFG_GEN_SW_0P_TX_MASK);
    CLEAR_BIT(pReg->CMD_MODE_CFG, lpMask);
    SET_BIT(pReg->LPCLK_CTRL, MIPI_DSI_LPCLK_CTRL_PHY_TXREQUESTCLKHS_MASK);

    return HAL_OK;
}

/**
 * @brief  Config DSI Send Message In Lp Mode.
 * @param  pReg: DSI reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_MsgLpModeConfig(struct MIPI_DSI_REG *pReg)
{
    uint32_t lpMask = MIPI_DSI_CMD_MODE_CFG_GEN_SW_0P_TX_MASK | MIPI_DSI_CMD_MODE_CFG_GEN_SW_1P_TX_MASK |
                      MIPI_DSI_CMD_MODE_CFG_GEN_SW_2P_TX_MASK | MIPI_DSI_CMD_MODE_CFG_GEN_SR_0P_TX_MASK |
                      MIPI_DSI_CMD_MODE_CFG_GEN_SR_1P_TX_MASK | MIPI_DSI_CMD_MODE_CFG_GEN_SR_2P_TX_MASK |
                      MIPI_DSI_CMD_MODE_CFG_GEN_LW_TX_MASK | MIPI_DSI_CMD_MODE_CFG_DCS_SW_0P_TX_MASK |
                      MIPI_DSI_CMD_MODE_CFG_DSC_SW_1P_TX_MASK | MIPI_DSI_CMD_MODE_CFG_DCS_SR_0P_TX_MASK |
                      MIPI_DSI_CMD_MODE_CFG_DSC_LW_TX_MASK | MIPI_DSI_CMD_MODE_CFG_MAX_RD_PKT_SIZE_MASK;

    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));

    SET_BIT(pReg->VID_MODE_CFG, MIPI_DSI_CMD_MODE_CFG_GEN_SW_0P_TX_MASK);
    SET_BIT(pReg->CMD_MODE_CFG, lpMask);

    return HAL_OK;
}

/** @} */

/** @defgroup DSI_Exported_Functions_Group3 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 *  @{
 */
/**
 * @brief  DSI Init.
 * @param  pReg: DSI reg base.
 * @param  laneMbps: DSI per lane mbps.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_Init(struct MIPI_DSI_REG *pReg, uint16_t laneMbps)
{
    uint32_t val;

    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));
    HAL_ASSERT(laneMbps != 0);

    val = HAL_DIV_ROUND_UP(laneMbps >> 3, 20);
    WRITE_REG(pReg->PWR_UP, 0);
    WRITE_REG(pReg->CLKMGR_CFG, 10 << MIPI_DSI_CLKMGR_CFG_TO_CLK_DIVISION_SHIFT |
              val << MIPI_DSI_CLKMGR_CFG_TX_ESC_CLK_DIVISION_SHIFT);

    return HAL_OK;
}

/**
 * @brief  DSI Dpi Config.
 * @param  pReg: DSI reg base.
 * @param  pModeInfo: display mode info.
 * @param  busFormat: display busFormat.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_DpiConfig(struct MIPI_DSI_REG *pReg,
                                     struct DISPLAY_MODE_INFO *pModeInfo,
                                     uint16_t busFormat)
{
    uint32_t lpcmdTime;
    uint32_t color = 0, polarity = 0;

    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));
    HAL_ASSERT(pModeInfo != NULL);
    HAL_ASSERT(busFormat != 0);

    switch (busFormat) {
    case MEDIA_BUS_FMT_RGB888_1X24:
        color = DPI_COLOR_CODING_24BIT;
        break;
    case MEDIA_BUS_FMT_RGB666_1X24_CPADHI:
        color = DPI_COLOR_CODING_18BIT_2 | MIPI_DSI_DPI_COLOR_CODING_LOOSELY18_EN_MASK;
        break;
    case MEDIA_BUS_FMT_RGB666_1X18:
        color = DPI_COLOR_CODING_18BIT_1;
        break;
    case MEDIA_BUS_FMT_RGB565_1X16:
        color = DPI_COLOR_CODING_16BIT_1;
        break;
    }

    lpcmdTime = 4 << MIPI_DSI_DPI_LP_CMD_TIM_OUTVACT_IPCMD_TIME_SHIFT | 4 << MIPI_DSI_DPI_LP_CMD_TIM_INVACT_IPCMD_TIME_SHIFT;

    WRITE_REG(pReg->DPI_COLOR_CODING, color);
    WRITE_REG(pReg->DPI_CFG_POL, polarity);
    WRITE_REG(pReg->DPI_LP_CMD_TIM, lpcmdTime);

    return HAL_OK;
}

/**
 * @brief  DSI Packet Handler Config.
 * @param  pReg: DSI reg base.
 * @param  pModeInfo: display mode info.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_PacketHandlerConfig(struct MIPI_DSI_REG *pReg,
                                               struct DISPLAY_MODE_INFO *pModeInfo)
{
    uint32_t val = MIPI_DSI_PCKHDL_CFG_BTA_EN_MASK | MIPI_DSI_PCKHDL_CFG_ECC_RX_EN_MASK | MIPI_DSI_PCKHDL_CFG_CRC_RX_EN_MASK | MIPI_DSI_PCKHDL_CFG_EOTP_TX_EN_MASK;

    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));
    HAL_ASSERT(pModeInfo != NULL);

    if (pModeInfo->flags & DSI_MODE_EOT_PACKET) {
        val &= ~MIPI_DSI_PCKHDL_CFG_EOTP_TX_EN_MASK;
    }

    WRITE_REG(pReg->PCKHDL_CFG, val);

    return HAL_OK;
}

/**
 * @brief  DSI Mode Config.
 * @param  pReg: DSI reg base.
 * @param  pModeInfo: display mode info.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_ModeConfig(struct MIPI_DSI_REG *pReg,
                                      struct DISPLAY_MODE_INFO *pModeInfo)
{
    uint32_t val = MIPI_DSI_VID_MODE_CFG_IP_VSA_EN_MASK | MIPI_DSI_VID_MODE_CFG_IP_VFP_EN_MASK | MIPI_DSI_VID_MODE_CFG_IP_VBP_EN_MASK |
                   MIPI_DSI_VID_MODE_CFG_IP_VACT_EN_MASK | MIPI_DSI_VID_MODE_CFG_IP_HBP_EN_MASK | MIPI_DSI_VID_MODE_CFG_IP_HFP_EN_MASK;

    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));
    HAL_ASSERT(pModeInfo != NULL);

    if (pModeInfo->flags & DSI_MODE_VIDEO) {
        if (pModeInfo->flags & DSI_MODE_VIDEO_BURST) {
            val |= 0x2 << MIPI_DSI_VID_MODE_CFG_VID_MODE_TYPE_SHIFT;
        } else if (pModeInfo->flags & DSI_MODE_VIDEO_SYNC_PULSE) {
            val |= 0 << MIPI_DSI_VID_MODE_CFG_VID_MODE_TYPE_SHIFT;
        } else {
            val |= MIPI_DSI_VID_MODE_CFG_VID_MODE_TYPE_MASK;
        }

        WRITE_REG(pReg->VID_MODE_CFG, val);
        WRITE_REG(pReg->VID_PKT_SIZE, pModeInfo->crtcHdisplay);
    } else {
        WRITE_REG(pReg->TO_CNT_CFG, 1000 << MIPI_DSI_TO_CNT_CFG_HSTX_TO_CNT_SHIFT |
                  1000 << MIPI_DSI_TO_CNT_CFG_LPRX_TO_CNT_SHIFT);
        WRITE_REG(pReg->BTA_TO_CNT, 0xd00);
    }
    if (pModeInfo->flags & DSI_CLOCK_NON_CONTINUOUS) {
        SET_BIT(pReg->LPCLK_CTRL, MIPI_DSI_LPCLK_CTRL_AUTO_CLKLANE_CTRL_MASK);
    }

    SET_BIT(pReg->MODE_CFG, MIPI_DSI_MODE_CFG_CMD_VIDEO_MODE_MASK);
    WRITE_REG(pReg->PWR_UP, 0x1);

    return HAL_OK;
}

/**
 * @brief  DSI Line Timer Config.
 * @param  pReg: DSI reg base.
 * @param  laneMbps: DSI per lane mbps.
 * @param  pModeInfo: display mode info.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_LineTimerConfig(struct MIPI_DSI_REG *pReg,
                                           uint16_t laneMbps,
                                           struct DISPLAY_MODE_INFO *pModeInfo)
{
    uint16_t hsa = pModeInfo->crtcHsyncEnd - pModeInfo->crtcHsyncStart;
    uint16_t hbp = pModeInfo->crtcHtotal - pModeInfo->crtcHsyncEnd;
    uint32_t clk = pModeInfo->crtcClock;
    uint32_t lpcc;

    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));
    HAL_ASSERT(laneMbps != 0);
    HAL_ASSERT(pModeInfo != 0);

    lpcc = DW_MIPI_DSI_GetHcomponentLbcc(hsa, laneMbps, clk);
    WRITE_REG(pReg->VID_HSA_TIME, lpcc);
    lpcc = DW_MIPI_DSI_GetHcomponentLbcc(hbp, laneMbps, clk);
    WRITE_REG(pReg->VID_HBP_TIME, lpcc);
    lpcc = DW_MIPI_DSI_GetHcomponentLbcc(pModeInfo->crtcHtotal, laneMbps, clk);
    WRITE_REG(pReg->VID_HLINE_TIME, lpcc);

    return HAL_OK;
}

/**
 * @brief  DSI Line Timer Update.
 * @param  pReg: DSI reg base.
 * @param  laneMbps: DSI per lane mbps.
 * @param  pModeInfo: display mode info.
 * @param  pDisplayRect: display rect.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_UpdateLineTimer(struct MIPI_DSI_REG *pReg,
                                           uint16_t laneMbps,
                                           struct DISPLAY_MODE_INFO *pModeInfo,
                                           struct DISPLAY_RECT *pDisplayRect)
{
    uint16_t hsa = pModeInfo->crtcHsyncEnd - pModeInfo->crtcHsyncStart;
    uint16_t hbp = pDisplayRect->w - pModeInfo->crtcHsyncEnd;
    uint32_t clk = pModeInfo->crtcClock;
    uint32_t lpcc;

    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));
    HAL_ASSERT(laneMbps != 0);
    HAL_ASSERT(pModeInfo != 0);
    HAL_ASSERT(pDisplayRect != NULL);

    CLEAR_BIT(pReg->CMD_MODE_CFG, MIPI_DSI_CMD_MODE_CFG_TEAR_FX_EN_MASK);
    WRITE_REG(pReg->EDPI_CMD_SIZE, pDisplayRect->w);
    CLEAR_BIT(pReg->CMD_MODE_CFG, MIPI_DSI_CMD_MODE_CFG_DSC_LW_TX_MASK);
    SET_BIT(pReg->CMD_MODE_CFG, MIPI_DSI_CMD_MODE_CFG_TEAR_FX_EN_MASK);

    lpcc = DW_MIPI_DSI_GetHcomponentLbcc(hsa, laneMbps, clk);
    WRITE_REG(pReg->VID_HSA_TIME, lpcc);
    lpcc = DW_MIPI_DSI_GetHcomponentLbcc(hbp, laneMbps, clk);
    WRITE_REG(pReg->VID_HBP_TIME, lpcc);
    lpcc = DW_MIPI_DSI_GetHcomponentLbcc(pDisplayRect->w, laneMbps, clk);
    WRITE_REG(pReg->VID_HLINE_TIME, lpcc);

    return HAL_OK;
}

/**
 * @brief  DSI Vertical Timing Config.
 * @param  pReg: DSI reg base.
 * @param  pModeInfo: display mode info.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_VerticalTimingConfig(struct MIPI_DSI_REG *pReg,
                                                struct DISPLAY_MODE_INFO *pModeInfo)
{
    uint16_t vfp = pModeInfo->crtcVsyncStart - pModeInfo->crtcVdisplay;
    uint16_t vbp = pModeInfo->crtcVtotal - pModeInfo->crtcVsyncEnd;
    uint16_t vsa = pModeInfo->crtcVsyncEnd - pModeInfo->crtcVsyncStart;

    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));
    HAL_ASSERT(pModeInfo != NULL);

    WRITE_REG(pReg->VID_VSA_LINES, vsa);
    WRITE_REG(pReg->VID_VBP_LINES, vbp);
    WRITE_REG(pReg->VID_VFP_LINES, vfp);
    WRITE_REG(pReg->VID_VACTIVE_LINES, pModeInfo->crtcVdisplay);

    return HAL_OK;
}

/**
 * @brief  DSI Dphy Timing Config.
 * @param  pReg: DSI reg base.
 * @param  pModeInfo: display mode info.
 * @param  lanes: number of mipi data lines.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_DphyTimingConfig(struct MIPI_DSI_REG *pReg,
                                            struct DISPLAY_MODE_INFO *pModeInfo,
                                            uint8_t lanes)
{
    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));
    HAL_ASSERT(pModeInfo != 0);
    HAL_ASSERT(lanes != 0);

    WRITE_REG(pReg->PHY_TMR_LPCLK_CFG, 0x40 << MIPI_DSI_PHY_TMR_LPCLK_CFG_PHY_CLKLP2HS_TIME_SHIFT |
              0x40 << MIPI_DSI_PHY_TMR_LPCLK_CFG_PHY_CLKHS2LP_TIME_SHIFT);
    WRITE_REG(pReg->PHY_TMR_CFG, 0x14 << MIPI_DSI_PHY_TMR_CFG_PHY_HS2LP_TIME_SHIFT |
              0x10 << MIPI_DSI_PHY_TMR_CFG_PHY_LP2HS_TIME_SHIFT | 10000 << MIPI_DSI_PHY_TMR_CFG_MAX_RD_TIME_SHIFT);
    WRITE_REG(pReg->PHY_IF_CFG, 0x5 << MIPI_DSI_PHY_IF_CFG_PHY_STOP_WAIT_TIME_SHIFT |
              (lanes - 1) << MIPI_DSI_PHY_IF_CFG_N_LANES_SHIFT);

    return HAL_OK;
}

/**
 * @brief  DSI Enable.
 * @param  pReg: DSI reg base.
 * @param  pModeInfo: display mode info.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_Enable(struct MIPI_DSI_REG *pReg,
                                  struct DISPLAY_MODE_INFO *pModeInfo)
{
    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));
    HAL_ASSERT(pModeInfo != 0);

    SET_BIT(pReg->LPCLK_CTRL, MIPI_DSI_LPCLK_CTRL_PHY_TXREQUESTCLKHS_MASK);

    if (pModeInfo->flags & DSI_MODE_VIDEO) {
        CLEAR_BIT(pReg->MODE_CFG, MIPI_DSI_MODE_CFG_CMD_VIDEO_MODE_MASK);
    } else {
        SET_BIT(pReg->MODE_CFG, MIPI_DSI_MODE_CFG_CMD_VIDEO_MODE_MASK);
        WRITE_REG(pReg->EDPI_CMD_SIZE, pModeInfo->crtcHdisplay);
    }
    WRITE_REG(pReg->PWR_UP, 0x1);

    return HAL_OK;
}

/**
 * @brief  DSI Disable.
 * @param  pReg: DSI reg base.
 * @param  pModeInfo: display mode info.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_Disable(struct MIPI_DSI_REG *pReg,
                                   struct DISPLAY_MODE_INFO *pModeInfo)
{
    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));
    HAL_ASSERT(pModeInfo != 0);

    WRITE_REG(pReg->PWR_UP, 0x0);
    CLEAR_BIT(pReg->LPCLK_CTRL, MIPI_DSI_LPCLK_CTRL_PHY_TXREQUESTCLKHS_MASK);

    if (pModeInfo->flags & DSI_MODE_VIDEO) {
        SET_BIT(pReg->MODE_CFG, MIPI_DSI_MODE_CFG_CMD_VIDEO_MODE_MASK);
    } else {
        WRITE_REG(pReg->EDPI_CMD_SIZE, 0);
    }

    WRITE_REG(pReg->PWR_UP, 0x1);

    return HAL_OK;
}

/**
 * @brief  DSI Phy Init.
 * @param  pReg: DSI reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_PhyInit(struct MIPI_DSI_REG *pReg)
{
    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));

    CLEAR_BIT(pReg->PHY_RSTZ, MIPI_DSI_PHY_RSTZ_PHY_ENABLECLK_MASK);
    HAL_DelayUs(1);
    CLEAR_BIT(pReg->PHY_RSTZ, MIPI_DSI_PHY_RSTZ_PHY_SHUTDOWNZ_MASK);
    HAL_DelayUs(1);
    CLEAR_BIT(pReg->PHY_RSTZ, MIPI_DSI_PHY_RSTZ_PHY_RSTZ_MASK);
    HAL_DelayUs(1);
    SET_BIT(pReg->PHY_TST_CTRL[1], MIPI_DSI_PHY_TST_CTRL0_PHY_TESTCLR_MASK);

    WRITE_REG_MASK_WE(GRF->SOC_CON6, GRF_SOC_CON6_GRF_DSIPHY_TURNDISABLE_MASK, 0);
    WRITE_REG_MASK_WE(GRF->SOC_CON6, GRF_SOC_CON6_GRF_DSIPHY_LANE0_FRCTXSTOPM_MASK |
                      GRF_SOC_CON6_GRF_DSIPHY_LANE1_FRCTXSTOPM_MASK |
                      GRF_SOC_CON6_GRF_DSIPHY_LANE2_FRCTXSTOPM_MASK |
                      GRF_SOC_CON6_GRF_DSIPHY_LANE3_FRCTXSTOPM_MASK, 0);
    WRITE_REG_MASK_WE(GRF->SOC_CON6, GRF_SOC_CON6_GRF_DSIPHY_FORCERXMODE_MASK, 0);
    HAL_DelayUs(1);

    WRITE_REG_MASK_WE(GRF->SOC_CON6, GRF_SOC_CON6_GRF_DSIPHY_PHYENABLE_0_MASK, 1 << GRF_SOC_CON6_GRF_DSIPHY_PHYENABLE_0_SHIFT);
    WRITE_REG_MASK_WE(GRF->SOC_CON6, GRF_SOC_CON6_GRF_DSIPHY_PHYENABLE_1_MASK, 1 << GRF_SOC_CON6_GRF_DSIPHY_PHYENABLE_1_SHIFT);

    CLEAR_BIT(pReg->PHY_TST_CTRL[1], MIPI_DSI_PHY_TST_CTRL0_PHY_TESTCLR_MASK);
    HAL_DelayUs(1);
    CLEAR_BIT(pReg->PHY_RSTZ, MIPI_DSI_PHY_RSTZ_PHY_ENABLECLK_MASK);
    HAL_DelayUs(1);

    return HAL_OK;
}

/**
 * @brief  DSI Phy PowerOn.
 * @param  pReg: DSI reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_PhyPowerOn(struct MIPI_DSI_REG *pReg)
{
    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));

    SET_BIT(pReg->PHY_RSTZ, MIPI_DSI_PHY_RSTZ_PHY_SHUTDOWNZ_MASK);
    SET_BIT(pReg->PHY_RSTZ, MIPI_DSI_PHY_RSTZ_PHY_RSTZ_MASK);
    HAL_DelayUs(2);

    WRITE_REG(pReg->PWR_UP, 0x1);

    return HAL_OK;
}

/**
 * @brief  DSI Phy PowerOff.
 * @param  pReg: DSI reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_PhyPowerOff(struct MIPI_DSI_REG *pReg)
{
    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));

    SET_BIT(pReg->PHY_RSTZ, MIPI_DSI_PHY_RSTZ_PHY_RSTZ_MASK);
    WRITE_REG(pReg->PWR_UP, 0);

    return HAL_OK;
}

/**
 * @brief  DSI Phy Clear Err.
 * @param  pReg: DSI reg base.
 * @return HAL_Status.
 */
HAL_Status HAL_DW_MIPI_DSI_ClrErr(struct MIPI_DSI_REG *pReg)
{
    HAL_ASSERT(IS_MIPI_DSI_INSTANCE(pReg));

    WRITE_REG(pReg->INT_MSK[0], 0);
    WRITE_REG(pReg->INT_MSK[1], 0);

    return HAL_OK;
}
/** @} */

/** @} */

/** @} */

#endif /* HAL_DW_MIPI_DSI_MODULE_ENABLED */
