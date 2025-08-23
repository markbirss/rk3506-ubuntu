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

#define SYS_TIMER TIMER0 /* System timer designation (RK TIMER) */

#ifdef RT_USING_SYSTICK
#define TICK_IRQn  SysTick_IRQn
#else
#define TICK_TIMER TIMER4
#define TICK_IRQn  TIMER4_IRQn
#endif

#ifdef RT_USING_PM_TIMER
#define PM_TIMER TIMER3
#define PM_TIMER_IRQn TIMER3_IRQn
#endif

#define HAL_DBG_USING_RTT_SERIAL 1   /* redirect the hal log to rtt console */

#ifdef RT_USING_ACDCDIG
#define HAL_ACDCDIG_MODULE_ENABLED
#endif

#ifdef RT_USING_CRU
#define HAL_CRU_MODULE_ENABLED
#endif

#ifdef RT_USING_DMA
#define HAL_DMA_MODULE_ENABLED
#endif

#ifdef RT_USING_DMA_PL330
#define HAL_PL330_MODULE_ENABLED
#endif

#ifdef RT_USING_DSI
#define HAL_DSI_MODULE_ENABLED
#endif

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

#ifdef RT_USING_PM
#define HAL_PM_CPU_SLEEP_MODULE_ENABLED
#define HAL_PM_SLEEP_MODULE_ENABLED
#endif

#ifdef RT_USING_PM_RUNTIME
#define HAL_PM_RUNTIME_MODULE_ENABLED
#endif

#ifdef RT_USING_PMU
#define HAL_PD_MODULE_ENABLED
#endif

#ifdef RT_USING_SDIO
#define HAL_SDIO_MODULE_ENABLED
#endif

#ifdef RT_USING_SNOR
#define HAL_SNOR_MODULE_ENABLED
#endif

#ifdef RT_USING_SPI
#define HAL_SPI_MODULE_ENABLED
#endif

#ifdef RT_USING_SPI2APB
#define HAL_SPI2APB_MODULE_ENABLED
#endif

#ifdef RT_USING_UART
#define HAL_UART_MODULE_ENABLED
#endif

#ifdef RT_USING_VAD
#define HAL_VAD_MODULE_ENABLED
#endif

#ifdef RT_USING_VOP
#define HAL_VOP_MODULE_ENABLED
#endif

#ifdef RT_USING_WDT
#define HAL_WDT_MODULE_ENABLED
#define HAL_WDT_DYNFREQ_FEATURE_ENABLED
#endif

#if defined(RT_USING_CACHE)
#define HAL_DCACHE_MODULE_ENABLED
#define HAL_ICACHE_MODULE_ENABLED
#endif

#define HAL_NVIC_MODULE_ENABLED
#define HAL_SYSTICK_MODULE_ENABLED
#define HAL_TIMER_MODULE_ENABLED

#ifdef RT_USING_QPIPSRAM
#define HAL_QPIPSRAM_MODULE_ENABLED
#ifdef RT_USING_QPIPSRAM_FSPI_HOST
#define HAL_FSPI_MODULE_ENABLED
#endif
#endif

#endif /* _HAL_CONF_H_ */
