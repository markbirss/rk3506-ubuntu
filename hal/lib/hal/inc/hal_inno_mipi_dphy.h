/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_INNO_MIPI_DPHY_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup INNO_MIPI_DPHY
 *  @{
 */

#ifndef _HAL_INNO_MIPI_DPHY_H_
#define _HAL_INNO_MIPI_DPHY_H_

/***************************** MACRO Definition ******************************/
/** @defgroup INNO_MIPI_DPHY_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/***************************** Structure Definition **************************/

typedef enum {
    RV1108_INNO_MIPI_DPHY,
    RK1808_INNO_MIPI_DPHY,
    RK3506_INNO_MIPI_DPHY,
} eINNO_MIPI_DPHY_socType;

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup INNO_MIPI_DPHY_Public_Function_Declare Public Function Declare
 *  @{
 */
HAL_Status HAL_INNO_MIPI_DPHY_BgpdEnable(struct MIPI_TX_PHY_REG *pReg);
HAL_Status HAL_INNO_MIPI_DPHY_DaPwrokEnable(struct MIPI_TX_PHY_REG *pReg);
HAL_Status HAL_INNO_MIPI_DPHY_PllLdoEnable(struct MIPI_TX_PHY_REG *pReg);
HAL_Status HAL_INNO_MIPI_DPHY_LaneEnable(struct MIPI_TX_PHY_REG *pReg, uint32_t laneNum);
HAL_Status HAL_INNO_MIPI_DPHY_Reset(struct MIPI_TX_PHY_REG *pReg);
HAL_Status HAL_INNO_MIPI_DPHY_TimingInit(struct MIPI_TX_PHY_REG *pReg, unsigned int laneMbps, uint32_t laneNum, eINNO_MIPI_DPHY_socType socType);
HAL_Status HAL_INNO_MIPI_DPHY_LaneDisable(struct MIPI_TX_PHY_REG *pReg, uint32_t laneNum);
HAL_Status HAL_INNO_MIPI_DPHY_PllLdoDisable(struct MIPI_TX_PHY_REG *pReg);
HAL_Status HAL_INNO_MIPI_DPHY_DaPwrokDisable(struct MIPI_TX_PHY_REG *pReg);
HAL_Status HAL_INNO_MIPI_DPHY_BgpdDisable(struct MIPI_TX_PHY_REG *pReg);
HAL_Status HAL_INNO_MIPI_DPHY_SetPll(struct MIPI_TX_PHY_REG *pReg, uint16_t fbDiv, uint8_t preDiv, eINNO_MIPI_DPHY_socType socType);

/** @} */
#endif

/** @} */

/** @} */

#endif /* HAL_INNO_MIPI_DPHY_MODULE_ENABLED */
