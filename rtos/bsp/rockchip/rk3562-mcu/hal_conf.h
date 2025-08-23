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

#define HAL_MCU_CORE
#define SOC_RK3562

/* Cache maintain need the decoded addr, it must be matched with pre-loader */
#if defined(HAL_BUS_MCU_CORE)
#define HAL_CACHE_DECODED_ADDR_BASE          0x08200000  /* Not really necessary */
#endif

#define SYS_TIMER TIMER5 /* System timer designation (RK TIMER) */

#if defined(HAL_PMU_MCU_CORE) && defined(RT_USING_CACHE)
#error "there is no cache in rk3562 pmu mcu!"
#endif

#if defined(RT_USING_CACHE)
#define HAL_DCACHE_MODULE_ENABLED
#define HAL_ICACHE_MODULE_ENABLED
#endif

#ifdef RT_USING_CRU
#define HAL_CRU_MODULE_ENABLED
#endif

#ifdef RT_USING_I2C
#define HAL_I2C_MODULE_ENABLED
#endif

#ifdef RT_USING_PIN
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PINCTRL_MODULE_ENABLED
#endif

#ifdef RT_USING_PWM
#define HAL_PWM_MODULE_ENABLED
#endif

#ifdef RT_USING_SPI
#define HAL_SPI_MODULE_ENABLED
#endif

#ifdef RT_USING_MULTI_SARADC
#define HAL_SARADC_MODULE_ENABLED
#define RT_USING_SARADC0
#define RT_USING_SARADC1
#endif

#ifdef RT_USING_UART
#define HAL_UART_MODULE_ENABLED
#endif

#ifdef RT_USING_WDT
#define HAL_WDT_MODULE_ENABLED
#define HAL_WDT_DYNFREQ_FEATURE_ENABLED
#endif

#ifdef RT_USING_PIN
#define HAL_PINCTRL_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
#define HAL_GPIO_VIRTUAL_MODEL_FEATURE_ENABLED
#endif

#define HAL_CRU_MODULE_ENABLED
#define HAL_INTMUX_MODULE_ENABLED
#define HAL_MBOX_MODULE_ENABLED
#define HAL_NVIC_MODULE_ENABLED
#define HAL_SYSTICK_MODULE_ENABLED
#define HAL_TIMER_MODULE_ENABLED

/* HAL_DBG SUB CONFIG */
#define HAL_DBG_USING_RTT_SERIAL
#define HAL_DBG_ON
#define HAL_DBG_INFO_ON
#define HAL_DBG_WRN_ON
#define HAL_DBG_ERR_ON
#define HAL_ASSERT_ON

#endif /* _HAL_CONF_H_ */

