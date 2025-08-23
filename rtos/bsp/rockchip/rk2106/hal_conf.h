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

#define SYS_TIMER TIMER1

#ifdef RT_USING_ACODEC
#define HAL_ACODEC_MODULE_ENABLED
#endif

#ifdef RT_USING_CRU
#define HAL_CRU_MODULE_ENABLED
#endif

#ifdef RT_USING_DMA
#define HAL_DMA_MODULE_ENABLED
#endif

#ifdef RT_USING_DMA_DWDMA
#define HAL_DWDMA_MODULE_ENABLED
#endif

#ifdef RT_USING_UART
#define HAL_UART_MODULE_ENABLED
#endif

#ifdef RT_USING_I2S
#define HAL_I2S_MODULE_ENABLED
#endif

#ifdef RT_USING_I2STDM
#define HAL_I2STDM_MODULE_ENABLED
#endif

#ifdef RT_USING_PIN
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PINCTRL_MODULE_ENABLED
#endif

#ifdef RT_USING_USB_DEVICE
#define HAL_PCD_MODULE_ENABLED
#endif

#define HAL_NVIC_MODULE_ENABLED
#define HAL_TIMER_MODULE_ENABLED

#ifdef RT_USING_SARADC
#define HAL_SARADC_MODULE_ENABLED
#endif

#ifdef RT_USING_WDT
#define HAL_WDT_MODULE_ENABLED
#define HAL_WDT_DYNFREQ_FEATURE_ENABLED
#endif

#ifdef RT_USING_SYSTICK
#define HAL_SYSTICK_MODULE_ENABLED
#endif

#endif /* _HAL_CONF_H_ */

