/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (c) 2020 Fuzhou Rockchip Electronics Co., Ltd
 */

/**
 * @file  hal_conf_template.h
 */

#ifndef _HAL_CONF_H_
#define _HAL_CONF_H_

#include "rtconfig.h"

#define SOC_RK3588
#define HAL_MCU_CORE
#define SOC_RK3588

/* Cache maintain need the decoded addr, it must be matched with pre-loader */
#if defined(HAL_PMU_MCU_CORE)
#define HAL_CACHE_DECODED_ADDR_BASE          0x07b00000
#elif defined(HAL_DDR_MCU_CORE)
#define HAL_CACHE_DECODED_ADDR_BASE          0x300000  /* Not really necessary */
#elif defined(HAL_NPU_MCU_CORE)
#define HAL_CACHE_DECODED_ADDR_BASE          0x400000
#endif

#define SYS_TIMER TIMER11 /* System timer designation (RK TIMER) */

#if defined(RT_USING_CACHE)
#define HAL_DCACHE_MODULE_ENABLED
#define HAL_ICACHE_MODULE_ENABLED
#endif

#if defined(RT_USING_I2C)
#define HAL_I2C_MODULE_ENABLED
#endif

#ifdef RT_USING_UART
#define HAL_UART_MODULE_ENABLED
#endif

#ifdef RT_USING_WDT
#define HAL_WDT_MODULE_ENABLED
#define HAL_WDT_DYNFREQ_FEATURE_ENABLED
#endif

#ifdef RT_USING_SPI
#define HAL_SPI_MODULE_ENABLED
#endif

#ifdef RT_USING_32K_TICK_SRC
#define SYSTICK_EXT_SRC      32768
#else
#define SYSTICK_EXT_SRC      PLL_INPUT_OSC_RATE
#endif

#ifdef RT_USING_PIN
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PINCTRL_MODULE_ENABLED
#define HAL_GPIO_VIRTUAL_MODEL_FEATURE_ENABLED
#endif

#ifdef RT_USING_CRU
#define HAL_CRU_MODULE_ENABLED
#endif

#ifdef RT_USING_PWM
#define HAL_PWM_MODULE_ENABLED
#endif

#ifdef RT_USING_SARADC
#define HAL_SARADC_MODULE_ENABLED
#endif

#define HAL_INTMUX_MODULE_ENABLED
#define HAL_IRQ_HANDLER_MODULE_ENABLED
#define HAL_NVIC_MODULE_ENABLED
#define HAL_SYSTICK_MODULE_ENABLED
#define HAL_TIMER_MODULE_ENABLED
#define HAL_MBOX_MODULE_ENABLED

#endif /* _HAL_CONF_H_ */

