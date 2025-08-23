/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_FLEXBUS_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup FLEXBUS
 *  @{
 */

/** @defgroup FLEXBUS_How_To_Use How To Use
 *  @{

 The FLEXBUS driver can be used as follows:

 - Declare a FLEXBUS_HANDLE handle structure, for example:
   ```
   FLEXBUS_HANDLE instance;
   ```

 - Invoke HAL_FLEXBUS_Init() API to configure config FLEXBUS0_OpMode and FLEXBUS1_OpMode.

 - Invoke HAL_FLEXBUS_DeInit() if necessary.

 @} */

/** @defgroup FLEXBUS_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/********************* Private Variable Definition ***************************/

static const struct HAL_FLEXBUS_DFS_REG s_flexbusDfsRegV0 = {
    .dfs2Bit = 0x0,
    .dfs4Bit = 0x1,
    .dfs8Bit = 0x2,
    .dfs16Bit = 0x3,
    .dfsMask = 0x3,
};

static const struct HAL_FLEXBUS_DFS_REG s_flexbusDfsRegV1 = {
    .dfs1Bit = (0x0 << 29),
    .dfs2Bit = (0x1 << 29),
    .dfs4Bit = (0x2 << 29),
    .dfs8Bit = (0x3 << 29),
    .dfs16Bit = (0x4 << 29),
    .dfsMask = (0x7 << 29),
};

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup FLEXBUS_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit module as follows:

 *  @{
 */

/**
 * @brief  Initialize the FLEXBUS.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 * @param  pREG: FLEXBUS reg base.
 * @param  opMode0: FLEXBUS0 opmode.
 * @param  opMode1: FLEXBUS1 opmode.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_Init(struct FLEXBUS_HANDLE *pFB, struct FLEXBUS_REG *pREG,
                            eFLEXBUS0_OpMode opMode0, eFLEXBUS1_OpMode opMode1)
{
    HAL_ASSERT(pFB != NULL);

    pFB->pReg = pREG;
    HAL_ASSERT(IS_FLEXBUS_INSTANCE(pFB->pReg));

    pFB->opMode0 = opMode0;
    pFB->opMode1 = opMode1;

    if (pFB->opMode0 == FLEXBUS0_OPMODE_NULL && pFB->opMode1 == FLEXBUS1_OPMODE_NULL) {
        return HAL_NODEV;
    }

    HAL_FLEXBUS_InitConfig(pFB);

    if (pFB->opMode0 != FLEXBUS0_OPMODE_NULL && pFB->opMode1 != FLEXBUS1_OPMODE_NULL) {
        WRITE_REG(pFB->pReg->COM_CTL, FLEXBUS_TX_AND_RX);
    } else if (pFB->opMode0 != FLEXBUS0_OPMODE_NULL) {
        WRITE_REG(pFB->pReg->COM_CTL, FLEXBUS_TX_ONLY);
    } else {
        WRITE_REG(pFB->pReg->COM_CTL, FLEXBUS_RX_ONLY);
    }

    switch (READ_REG(pFB->pReg->REVISION) >> 24 & 0xff) {
    case 0x0:
        pFB->pDfsReg = &s_flexbusDfsRegV0;
        break;
    case 0x1:
        pFB->pDfsReg = &s_flexbusDfsRegV1;
        break;
    default:
        return HAL_NODEV;
    }

    return HAL_OK;
}

/**
 * @brief  DeInitialize the FLEXBUS.
 * @param  pFB: pointer to a FLEXBUS_HANDLE structure that contains
 *              the configuration information for FLEXBUS module.
 * @return HAL status
 */
HAL_Status HAL_FLEXBUS_DeInit(struct FLEXBUS_HANDLE *pFB)
{
    HAL_ASSERT(pFB != NULL);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif  /* HAL_FLEXBUS_MODULE_ENABLED */
