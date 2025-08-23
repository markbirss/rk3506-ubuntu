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

#define SYS_TIMER TIMER6 /* System timer designation (RK TIMER) */

#define HAL_DBG_USING_RTT_SERIAL 1   /* redirect the hal log to rtt console */

#ifdef RT_USING_ACDCDIG
#define HAL_ACDCDIG_MODULE_ENABLED
#endif

#ifdef RT_USING_AUDIOPWM
#define HAL_AUDIOPWM_MODULE_ENABLED
#endif

#ifdef RT_USING_CIF
#define HAL_CIF_MODULE_ENABLED
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

#ifdef RT_USING_PWM
#define HAL_PWM_MODULE_ENABLED
#endif

#ifdef RT_USING_SDIO
#define HAL_SDIO_MODULE_ENABLED
#endif

#ifdef RT_USING_SNOR
#define HAL_SNOR_MODULE_ENABLED
#ifdef RT_USING_SNOR_FSPI_HOST
#define HAL_SNOR_FSPI_HOST
#define HAL_FSPI_MODULE_ENABLED
#define HAL_FSPI_XIP_ENABLE
#endif
#ifdef RT_USING_SNOR_SPI_HOST
#define HAL_SNOR_SPI_HOST
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

#ifdef RT_USING_USB_DEVICE
#define HAL_PCD_MODULE_ENABLED
#endif

#ifdef RT_USING_USB_HOST
#define HAL_HCD_MODULE_ENABLED
#endif

#ifdef RT_USING_VAD
#define HAL_VAD_MODULE_ENABLED
#endif

#ifdef RT_USING_VOP
#define HAL_VOP_MODULE_ENABLED
#endif

#ifdef RT_USING_KEYCTRL
#define HAL_KEYCTRL_MODULE_ENABLED
#endif

#ifdef RT_USING_WDT
#define HAL_WDT_MODULE_ENABLED
#endif

#ifdef RT_USING_HYPERPSRAM
#define HAL_HYPERPSRAM_MODULE_ENABLED
#endif

#ifdef RT_USING_PSRAM
#define HAL_PSRAM_MODULE_ENABLED
#define HAL_FSPI_MODULE_ENABLED
#endif

//#define HAL_PWR_INTBUS_MODULE_ENABLED
#if defined(HAL_PWR_INTBUS_MODULE_ENABLED) || defined (HAL_PWR_I2C8_MODULE_ENABLED)
#define HAL_PWR_MODULE_ENABLED
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
