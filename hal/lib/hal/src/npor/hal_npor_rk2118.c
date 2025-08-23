/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(RKMCU_RK2118) && defined(HAL_NPOR_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup NPOR
 *  @{
 */

/** @defgroup NPOR_How_To_Use How To Use
 *  @{

 The NPOR driver can be used as follows:

 @} */

/** @defgroup NPOR_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup NPOR_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Check NPOR detect source power state.
 * @return HAL_Check
 */
HAL_SECTION_SRAM_DATA HAL_Check HAL_NPOR_IsPowergood(void)
{
    return (READ_REG(GRF_PMU->SOC_STATUS) & GRF_PMU_SOC_STATUS_NPOR_POWERGOOD_MASK);
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_NPOR_MODULE_ENABLED */
