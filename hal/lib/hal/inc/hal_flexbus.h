/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_FLEXBUS_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FLEXBUS
 *  @{
 */

#ifndef __HAL_FLEXBUS_H
#define __HAL_FLEXBUS_H

#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup FLEXBUS_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/* FLEXBUS tx_rx_mode */
#define FLEXBUS_TX_AND_RX  (0x0 << FLEXBUS_COM_CTL_TX_RX_MODE_SHIFT)
#define FLEXBUS_TX_ONLY    (0x1 << FLEXBUS_COM_CTL_TX_RX_MODE_SHIFT)
#define FLEXBUS_RX_ONLY    (0x2 << FLEXBUS_COM_CTL_TX_RX_MODE_SHIFT)
#define FLEXBUS_TX_THEN_RX (0x3 << FLEXBUS_COM_CTL_TX_RX_MODE_SHIFT)

/***************************** Structure Definition **************************/

/** @brief  FLEXBUS0 opmode definition */
typedef enum {
    FLEXBUS0_OPMODE_NULL = 0,
    FLEXBUS0_OPMODE_DAC,
    FLEXBUS0_OPMODE_SPI,
    FLEXBUS0_OPMODE_MAX
} eFLEXBUS0_OpMode;

/** @brief  FLEXBUS1 opmode definition */
typedef enum {
    FLEXBUS1_OPMODE_NULL = 0,
    FLEXBUS1_OPMODE_ADC,
    FLEXBUS1_OPMODE_CIF,
    FLEXBUS1_OPMODE_MAX
} eFLEXBUS1_OpMode;

/** @brief  FLEXBUS HW information definition */
struct HAL_FLEXBUS_DEV {
    struct FLEXBUS_REG *pReg;
    eCLOCK_Name txclkID;
    eCLOCK_Name rxclkID;
    uint32_t txclkGateID;
    uint32_t rxclkGateID;
    uint32_t aclkGateID;
    uint32_t hclkGateID;
    IRQn_Type irqNum;
    eFLEXBUS0_OpMode opMode0;
    eFLEXBUS1_OpMode opMode1;
};

/** @brief  FLEXBUS DFS reg definition */
struct HAL_FLEXBUS_DFS_REG {
    uint32_t dfs1Bit;
    uint32_t dfs2Bit;
    uint32_t dfs4Bit;
    uint32_t dfs8Bit;
    uint32_t dfs16Bit;
    uint32_t dfsMask;
};

/** @brief  FLEXBUS Configuration Structure definition */
struct FLEXBUS_HANDLE;

struct FLEXBUS_HANDLE {
    struct FLEXBUS_REG *pReg;
    eFLEXBUS0_OpMode opMode0;
    eFLEXBUS1_OpMode opMode1;
    const struct HAL_FLEXBUS_DFS_REG *pDfsReg;
    uint32_t txwatStartMax;
};

/** @} */

/***************************** Function Declare ******************************/
/** @defgroup FLEXBUS_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_FLEXBUS_Init(struct FLEXBUS_HANDLE *pFB, struct FLEXBUS_REG *pREG,
                            eFLEXBUS0_OpMode opMode0, eFLEXBUS1_OpMode opMode1);
HAL_Status HAL_FLEXBUS_DeInit(struct FLEXBUS_HANDLE *pFB);
void HAL_FLEXBUS_InitConfig(struct FLEXBUS_HANDLE *pFB);
void HAL_FLEXBUS_GrfConfig(struct FLEXBUS_HANDLE *pFB, bool slaveMode, bool cPol, bool cPha);

/** @} */

#endif

/** @} */

/** @} */

#endif  /* HAL_FLEXBUS_MODULE_ENABLED */
