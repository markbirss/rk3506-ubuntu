/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#ifndef _HAL_CONF_H_
#define _HAL_CONF_H_

/* CPU config */
#define SOC_RV1103B
#define __RISC_V
#define HAL_MCU_CORE
#define SYS_TIMER TIMER5 /* System timer designation (RK TIMER) */

/* HAL Driver Config */
#define HAL_FSPI_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_HWSPINLOCK_MODULE_ENABLED
#define HAL_MBOX_MODULE_ENABLED
#define HAL_PINCTRL_MODULE_ENABLED
#define HAL_PWM_MODULE_ENABLED
#define HAL_QPIPSRAM_MODULE_ENABLED
#define HAL_SNOR_MODULE_ENABLED
#define HAL_SPI2AHB_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_TIMER_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED

/* HAL_DBG SUB CONFIG */
//#define HAL_DBG_ON

#ifdef HAL_DBG_ON
#define HAL_DBG_USING_LIBC_PRINTF
#define HAL_DBG_INFO_ON
#define HAL_DBG_WRN_ON
#define HAL_DBG_ERR_ON
#define HAL_ASSERT_ON
#endif

#if defined(HAL_SNOR_MODULE_ENABLED)
#define HAL_SNOR_FSPI_HOST
#define HAL_FSPI_XIP_ENABLE
#endif

#endif
