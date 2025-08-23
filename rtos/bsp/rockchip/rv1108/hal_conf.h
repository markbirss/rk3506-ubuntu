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
#define SYS_TIMER TIMER1 /* System timer designation (RK TIMER) */

#define HAL_DBG_USING_RTT_SERIAL 1   /* redirect the hal log to rtt console */

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

#ifdef RT_USING_I2C
#define HAL_I2C_MODULE_ENABLED
#endif

#ifdef RT_USING_I2S
#define HAL_I2S_MODULE_ENABLED
#endif

#ifdef RT_USING_PDM
#define HAL_PDM_MODULE_ENABLED
#endif

#ifdef RT_USING_PIN
#define HAL_GPIO_MODULE_ENABLED
#endif

#ifdef RT_USING_PM
#define HAL_PM_SLEEP_MODULE_ENABLED
#endif

#ifdef RT_USING_PMU
#define HAL_PD_MODULE_ENABLED
#endif

#ifdef RT_USING_SNOR
#define HAL_SNOR_MODULE_ENABLED
#ifdef RT_USING_SNOR_SFC_HOST
#define HAL_SFC_MODULE_ENABLED
#define HAL_SNOR_SFC_HOST
#endif
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

#if defined(RT_USING_CACHE)
//#define HAL_DCACHE_MODULE_ENABLED
//#define HAL_ICACHE_MODULE_ENABLED
#endif

#define HAL_TIMER_MODULE_ENABLED

#endif /* _HAL_CONF_H_ */
