/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_SPI2AHB_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup SPI2AHB
 *  @{
 */

/** @defgroup SPI2AHB_How_To_Use How To Use
 *  @{

 The SPI2AHB is a spi slave controller which support receiving data from spi master.

 The SPI2AHB driver can be used as follows:

 - Initial SPI2AHB controller by calling HAL_SPI2AHB_Init().
 - Setting transfer data frame by calling HAL_SPI2AHB_FrameConfig().
 - Enable controller by calling HAL_SPI2AHB_Enable().

 @} */

/** @defgroup SPI2AHB_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup SPI2AHB_Exported_Functions_Group4 Init and DeInit Functions
 *  @{
 */

/**
 * @brief  SPI2AHB initial.
 * @param  pReg: SPI2AHB register base.
 * @param  ctrl: union SPI2AHB_CTRL for controller configuration.
 * @return HAL_Status
 */
HAL_Status HAL_SPI2AHB_Init(struct SPI2AHB_REG *pReg, uint32_t ctrl)
{
    HAL_ASSERT(IS_SPI2AHB_INSTANCE(pReg));

    WRITE_REG(pReg->SPI_CTRL, ctrl);
    WRITE_REG(pReg->IMR, 0xFFFFFFFF);
    WRITE_REG(pReg->ICLR, 0xFFFFFFFF);
    WRITE_REG(pReg->DMA_CTRL, 0);

    return HAL_OK;
}

/**
 * @brief  Deinit SPI2AHB.
 * @param  pReg: SPI2AHB register base.
 * @return HAL_Status
 */
HAL_Status HAL_SPI2AHB_DeInit(struct SPI2AHB_REG *pReg)
{
    HAL_ASSERT(IS_SPI2AHB_INSTANCE(pReg));

    HAL_SPI2AHB_Enable(pReg);

    return HAL_OK;
}

/** @} */

/** @defgroup SPI2AHB_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Set data frame.
 * @param  pReg: SPI2AHB register base.
 * @param  addr: SPI2AHB target memory address.
 * @param  length: SPI2AHB target data frame length in byte.
 * @return HAL_Status
 */
HAL_Status HAL_SPI2AHB_FrameConfig(struct SPI2AHB_REG *pReg, uint32_t addr, uint32_t length)
{
    HAL_ASSERT(IS_SPI2AHB_INSTANCE(pReg));

    /* frame setting */
    pReg->FRAME_ADDR = addr;
    pReg->FRAME_BNUM = length;

    return HAL_OK;
}

/**
 * @brief  Enable SPI2AHB.
 * @param  pReg: SPI2AHB register base.
 * @return HAL_Status
 */
HAL_Status HAL_SPI2AHB_Enable(struct SPI2AHB_REG *pReg)
{
    HAL_ASSERT(IS_SPI2AHB_INSTANCE(pReg));

    pReg->SPI_EN = 1;

    return HAL_OK;
}

/**
 * @brief  Disable SPI2AHB.
 * @param  pReg: SPI2AHB register base.
 * @return HAL_Status
 */
HAL_Status HAL_SPI2AHB_Disable(struct SPI2AHB_REG *pReg)
{
    HAL_ASSERT(IS_SPI2AHB_INSTANCE(pReg));

    pReg->SPI_EN = 0;

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif /* HAL_SPI2AHB_MODULE_ENABLED */
