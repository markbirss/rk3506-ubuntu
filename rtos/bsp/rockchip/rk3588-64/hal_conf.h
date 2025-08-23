/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-12     Steven Liu   first implementation
 * 2022-08-22     Steven Liu   update project structure and related code
 */

#ifndef _HAL_CONF_H_
#define _HAL_CONF_H_

#include "rtconfig.h"

/* HAL CPU config */
#define SOC_RK3588
#define HAL_AP_CORE
#define SYS_TIMER TIMER11 /* System timer designation (RK TIMER) */

/* TODO: RT-Thread Tick Timer.
 * Multiple RT-Thread instances in the AMP system need to use different TIMER,
 * and mount the TIMER interrupt to the correct CPU with the board-level
 * interrupt configuration */
#ifndef RT_USING_SYSTICK
#define TICK_TIMER TIMER1
#define TICK_IRQn  TIMER1_IRQn
#else
#define HAL_ARCHTIMER_MODULE_ENABLED
#endif

/* HAL Driver Config */
#define HAL_CPU_TOPOLOGY_MODULE_ENABLED
#define HAL_DCACHE_MODULE_ENABLED
#define HAL_GIC_MODULE_ENABLED
#define HAL_SPINLOCK_MODULE_ENABLED
#define HAL_TIMER_MODULE_ENABLED

#ifdef HAL_GIC_MODULE_ENABLED
#define HAL_GIC_AMP_FEATURE_ENABLED
#define HAL_GIC_WAIT_LINUX_INIT_ENABLED
#endif

#ifdef RT_USING_CRU
#define HAL_CRU_MODULE_ENABLED
#endif

#ifdef RT_USING_PIN
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PINCTRL_MODULE_ENABLED
#define HAL_GPIO_VIRTUAL_MODEL_FEATURE_ENABLED
#endif

#ifdef RT_USING_CAN
#define HAL_CANFD_MODULE_ENABLED
#endif

#ifdef RT_USING_GMAC
#define HAL_GMAC_MODULE_ENABLED
#endif

#ifdef RT_USING_I2STDM
#define HAL_I2STDM_MODULE_ENABLED
#endif

#ifdef RT_USING_MAILBOX
#define HAL_MBOX_MODULE_ENABLED
#endif

#ifdef RT_USING_DW_PCIE
#define HAL_PCIE_MODULE_ENABLED
#endif

#ifdef RT_USING_DMA_PL330
#define HAL_PL330_MODULE_ENABLED
#endif

#ifdef RT_USING_PMU
#define HAL_PD_MODULE_ENABLED
#endif

#ifdef RT_USING_PWM
#define HAL_PWM_MODULE_ENABLED
#endif

#ifdef RT_USING_UART
#define HAL_UART_MODULE_ENABLED
#endif

#ifdef RT_USING_SPI
#define HAL_SPI_MODULE_ENABLED
#endif

/* HAL_DBG SUB CONFIG */
#define HAL_DBG_USING_RTT_SERIAL
#define HAL_DBG_ON
#define HAL_DBG_INFO_ON
#define HAL_DBG_WRN_ON
#define HAL_DBG_ERR_ON
#define HAL_ASSERT_ON

#define HAL_SHARED_DEBUG_UART_LOCK_ID (0U)

#endif

