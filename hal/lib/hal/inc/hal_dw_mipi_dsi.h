/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_DW_MIPI_DSI_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup DW_MIPI_DSI
 *  @{
 */

#ifndef _HAL_DW_MIPI_DSI_H_
#define _HAL_DW_MIPI_DSI_H_

/***************************** MACRO Definition ******************************/
/** @defgroup DW_MIPI_DSI_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/

typedef enum {
    DW_MIPI_DSI_DCS_COMPRESSION_MODE        = 0x07,
    DW_MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM = 0x03,
    DW_MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM = 0x13,
    DW_MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM = 0x23,
    DW_MIPI_DSI_DCS_SHORT_WRITE             = 0x05,
    DW_MIPI_DSI_DCS_SHORT_WRITE_PARAM       = 0x15,
    DW_MIPI_DSI_NULL_PACKET                 = 0x09,
    DW_MIPI_DSI_BLANKING_PACKET             = 0x19,
    DW_MIPI_DSI_GENERIC_LONG_WRITE          = 0x29,
    DW_MIPI_DSI_DCS_LONG_WRITE              = 0x39,
    DW_MIPI_DSI_PPS_LONG_WRITE              = 0x0A,
} eDW_MIPI_DSI_dataType;

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup DSI_Public_Function_Declare Public Function Declare
 *  @{
 */
HAL_Status HAL_DW_MIPI_DSI_DphyTimingConfig(struct MIPI_DSI_REG *pReg,
                                            struct DISPLAY_MODE_INFO *pModeInfo, uint8_t lanes);
HAL_Status HAL_DW_MIPI_DSI_Init(struct MIPI_DSI_REG *pReg, uint16_t laneMbps);
HAL_Status HAL_DW_MIPI_DSI_MsgHsModeConfig(struct MIPI_DSI_REG *pReg);
HAL_Status HAL_DW_MIPI_DSI_MsgLpModeConfig(struct MIPI_DSI_REG *pReg);
HAL_Status HAL_DW_MIPI_DSI_Enable(struct MIPI_DSI_REG *pReg,
                                  struct DISPLAY_MODE_INFO *pModeInfo);
HAL_Status HAL_DW_MIPI_DSI_Disable(struct MIPI_DSI_REG *pReg,
                                   struct DISPLAY_MODE_INFO *pModeInfo);

HAL_Status HAL_DW_MIPI_DSI_SendPacket(struct MIPI_DSI_REG *pReg, eDW_MIPI_DSI_dataType dataType,
                                      uint8_t payloadLen, const uint8_t *payload);
HAL_Status HAL_DW_MIPI_DSI_ModeConfig(struct MIPI_DSI_REG *pReg,
                                      struct DISPLAY_MODE_INFO *pModeInfo);
HAL_Status HAL_DW_MIPI_DSI_PacketHandlerConfig(struct MIPI_DSI_REG *pReg,
                                               struct DISPLAY_MODE_INFO *pModeInfo);
HAL_Status HAL_DW_MIPI_DSI_VerticalTimingConfig(struct MIPI_DSI_REG *pReg,
                                                struct DISPLAY_MODE_INFO *pModeInfo);
HAL_Status HAL_DW_MIPI_DSI_DpiConfig(struct MIPI_DSI_REG *pReg,
                                     struct DISPLAY_MODE_INFO *pModeInfo,
                                     uint16_t busFormat);
HAL_Status HAL_DW_MIPI_DSI_LineTimerConfig(struct MIPI_DSI_REG *pReg,
                                           uint16_t laneMbps,
                                           struct DISPLAY_MODE_INFO *pModeInfo);
HAL_Status HAL_DW_MIPI_DSI_UpdateLineTimer(struct MIPI_DSI_REG *pReg,
                                           uint16_t laneMbps,
                                           struct DISPLAY_MODE_INFO *pModeInfo,
                                           struct DISPLAY_RECT *pDisplayRect);
HAL_Status HAL_DW_MIPI_DSI_PhyInit(struct MIPI_DSI_REG *pReg);
HAL_Status HAL_DW_MIPI_DSI_PhyPowerOn(struct MIPI_DSI_REG *pReg);
HAL_Status HAL_DW_MIPI_DSI_PhyPowerOff(struct MIPI_DSI_REG *pReg);
HAL_Status HAL_DW_MIPI_DSI_ClrErr(struct MIPI_DSI_REG *pReg);
/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_MIPI_DSI_MODULE_ENABLED */
