/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_dsmc_slave.c
  * @version V1.0
  * @brief   dsmc_slave controller interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-10-09     Zhihuan He   the first version
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup DSMC_SLAVE
 *  @{
 */

/** @defgroup DSMC_SLAVE_How_To_Use How To Use
 *  @{

    DSMC_SLAVE is a slave interface for local bus interface.
    It needs to be combined with the corresponding driver layer to
    complete the transmission of the protocol.

 - deivice Layer(drv_dsmc_slave)
 - DSMC_SLAVE controller Layer DRV_DSMC_SLAVE
 - DSMC_SLAVE lower Layer HAL_DSMC_SLAVE

 @} */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "hal_bsp.h"

#ifdef RT_USING_DSMC_SLAVE
/********************* Private MACRO Definition ******************************/
/** @defgroup DSMC_SLAVE_Private_Macro Private Macro
 *  @{
 */

/** @} */  // DSMC_SLAVE_Private_Macro

/********************* Private Structure Definition **************************/
/** @defgroup DSMC_SLAVE_Private_Structure Private Structure
 *  @{
 */

/** @} */  // DSMC_SLAVE_Private_Structure

/********************* Private Variable Definition ***************************/
/** @defgroup DSMC_SLAVE_Variable_Macro Variable Macro
 *  @{
 */

/** @} */  // DSMC_SLAVE_Variable_Macro

/********************* Private Function Definition ***************************/
/** @defgroup DSMC_SLAVE_Private_Function Private Function
 *  @{
 */
static void dsmc_slave_isr(int vector, void *param)
{
    HAL_DSMC_SLAVE_IrqHander((struct HAL_DSMC_SLAVE *)param);
}
/** @} */  // DSMC_SLAVE_Private_Function

/********************* Public Function Definition ****************************/
/** @defgroup DSMC_SLAVE_Public_Functions Public Functions
 *  @{
 */
/**
 * @brief  Init DSMC_SLAVE framwork and apply to use.
 */

int rockchip_dsmc_slave_probe(void)
{
    struct HAL_DSMC_SLAVE *slave = &g_dsmcSlaveDev;

    if (HAL_DSMC_SLAVE_Init(slave) != HAL_OK)
        return RT_ERROR;

    rt_hw_interrupt_install(slave->irqNum, dsmc_slave_isr, slave, "dsmc_slave");
    rt_hw_interrupt_umask(slave->irqNum);

    rt_kprintf("dsmc_slave: rockchip dsmc local bus slave driver initialized\n");

    return 0;
}
INIT_DEVICE_EXPORT(rockchip_dsmc_slave_probe);

/** @} */  // DSMC_SLAVE_Public_Functions

#endif

/** @} */  // DSMC_SLAVE

/** @} */  // RKBSP_Common_Driver
