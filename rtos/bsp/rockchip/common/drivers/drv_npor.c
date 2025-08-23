/**
  * Copyright (c) 2023 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_npor.c
  * @version V1.0
  * @brief   npor controller interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-10-14     Dingqiang Lin   the first version
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup NPOR
 *  @{
 */

/** @defgroup NPOR_How_To_Use How To Use
 *  @{

    RKNPOR interrupts when monitoring voltage below 2.92V.

 @} */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "hal_bsp.h"
#include "hal_base.h"

#include "drv_clock.h"

#if defined(RT_USING_NPOR)

/********************* Private MACRO Definition ******************************/
/********************* Private Structure Definition **************************/
/********************* Private Variable Definition ***************************/
/** @defgroup NPOR_Private_Macro Variable Macro
 *  @{
 */

/** @} */  // NPOR_Variable_Macro

/********************* Private Function Definition ***************************/
/** @defgroup NPOR_Private_Function Private Function
 *  @{
 */

static void rockchip_powergood_irq(void *param)
{
    rt_base_t level;

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    /* enter interrupt */
    rt_interrupt_enter();

    while (!HAL_NPOR_IsPowergood())
        ;

    rt_kprintf("%s voltage jitter detected\n", __func__);

    /* leave interrupt */
    rt_interrupt_leave();

    /* enable interrupt */
    rt_hw_interrupt_enable(level);
}
/** @} */  // NPOR_Private_Function

/** @defgroup NPOR_Public_Functions Public Functions
 *  @{
 */

/**
 * @brief  Init NPOR framwork and apply to use.
 * @attention The NPOR will be enabled when board initialization, do not dynamically switch NPOR
 *      unless specifically required.
 */
int rockchip_rt_hw_npor_init(void)
{
    rt_hw_interrupt_install(NPOR_POWERGOOD_IRQn, (void *)rockchip_powergood_irq, RT_NULL, RT_NULL);
    rt_hw_interrupt_umask(NPOR_POWERGOOD_IRQn);

    return 0;
}

INIT_PREV_EXPORT(rockchip_rt_hw_npor_init);

/** @} */  // NPOR_Public_Function

#endif

/** @} */  // NPOR

/** @} */  // RKBSP_Common_Driver
