/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (c) 2018 Fuzhou Rockchip Electronics Co., Ltd
 */

/**
 * @file  hal_conf_template.h
 */

#ifndef _HAL_CONF_H_
#define _HAL_CONF_H_

#include "rtconfig.h"

#define SYS_TIMER TIMER4 /* System timer designation (RK TIMER) */

#define HAL_DBG_USING_RTT_SERIAL 1   /* redirect the hal log to rtt console */

#ifdef RT_USING_DSP
#define HAL_MBOX_MODULE_ENABLED
#define HAL_DSP_MODULE_ENABLED
#endif

#ifdef RT_USING_I2C
#define HAL_I2C_MODULE_ENABLED
#endif

#ifdef RT_USING_I2STDM
#define HAL_I2STDM_MODULE_ENABLED
#endif

#ifdef RT_USING_PDM
#define HAL_PDM_MODULE_ENABLED
#endif

#ifdef RT_USING_PIN
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PINCTRL_MODULE_ENABLED
#endif

#ifdef RT_USING_DMA_PL330
#define g_pl330Dev0 g_pl330Dev
#define HAL_PL330_MODULE_ENABLED
#endif

#if defined(RT_USING_UART)
#define HAL_UART_MODULE_ENABLED
#endif

#ifdef RT_USING_CRU
#define HAL_CRU_MODULE_ENABLED
#endif

#ifdef RT_USING_PWM
#define HAL_PWM_MODULE_ENABLED
#endif

#ifdef RT_USING_PMIC
#define HAL_PWR_I2C8_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#endif

#define HAL_NVIC_MODULE_ENABLED

#ifdef RT_USING_SNOR
#define HAL_SNOR_MODULE_ENABLED
#ifdef RT_USING_SNOR_FSPI_HOST
#define HAL_FSPI_MODULE_ENABLED
#define HAL_SNOR_FSPI_HOST
#endif
#endif

#ifdef RT_USING_SPI
#define HAL_SPI_MODULE_ENABLED
#endif

#define HAL_SYSTICK_MODULE_ENABLED
#define HAL_TIMER_MODULE_ENABLED

#ifdef RT_USING_WDT
#define HAL_WDT_MODULE_ENABLED
#endif

#define HAL_PVTM_MODULE_ENABLED

#endif
