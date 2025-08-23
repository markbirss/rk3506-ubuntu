/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-12     Steven Liu   first implementation
 */

#ifndef _HAL_CONF_H_
#define _HAL_CONF_H_

#include "rtconfig.h"

/* HAL CPU config */
#define SOC_RK3308
#define HAL_AP_CORE
#define SYS_TIMER TIMER5 /* System timer designation (RK TIMER) */

/* RT-Thread Tick Timer */
#ifndef RT_USING_SYSTICK
#define TICK_TIMER TIMER4
#define TICK_IRQn  TIMER4_IRQn
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
#define HAL_GIC_PREEMPT_FEATURE_ENABLED
#endif

#ifdef RT_USING_CRU
#define HAL_CRU_MODULE_ENABLED
#endif

#ifdef RT_USING_PIN
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PINCTRL_MODULE_ENABLED
#endif

#ifdef RT_USING_I2C
#define HAL_I2C_MODULE_ENABLED
#endif

#ifdef RT_USING_ACODEC
#define HAL_ACODEC_MODULE_ENABLED
#endif

#ifdef RT_USING_I2S
#define HAL_I2S_MODULE_ENABLED
#endif

#ifdef RT_USING_I2STDM
#define HAL_I2STDM_MODULE_ENABLED
#endif

#ifdef RT_USING_DMA_PL330
#define HAL_PL330_MODULE_ENABLED
#endif

#ifdef RT_USING_PWM
#define HAL_PWM_MODULE_ENABLED
#endif

#ifdef RT_USING_SARADC
#define HAL_SARADC_MODULE_ENABLED
#endif

#ifdef RT_USING_SNOR
#define HAL_SFC_MODULE_ENABLED
#define HAL_SNOR_MODULE_ENABLED
#define HAL_SNOR_SFC_HOST
#endif

#ifdef RT_USING_SPINAND
#define HAL_SPINAND_MODULE_ENABLED
#ifdef RT_USING_SPINAND_SFC_HOST
#define HAL_SFC_MODULE_ENABLED
#endif
#endif

#ifdef RT_USING_SPI
#define HAL_SPI_MODULE_ENABLED
#endif

#ifdef RT_USING_TSADC
#define HAL_TSADC_MODULE_ENABLED
#define TSADC_NONLINEAR
#endif

#ifdef RT_USING_UART
#define HAL_UART_MODULE_ENABLED
#endif

#ifdef RT_USING_SDIO
#define HAL_SDIO_MODULE_ENABLED
#endif

#ifdef RT_USING_GMAC
#define HAL_GMAC1000_MODULE_ENABLED
#endif

#ifdef RT_LWIP_USING_HW_CHECKSUM
#define HAL_GMAC1000_HWCHECKSUM_FEATURE_ENABLED
#endif

#ifdef RT_USING_USB_DEVICE
#define HAL_PCD_MODULE_ENABLED
#endif

#ifdef RT_USING_USB_HOST
#define HAL_HCD_MODULE_ENABLED
#endif

#ifdef RT_USING_USBH_EHCI
#define HAL_EHCI_MODULE_ENABLED
#endif

#ifdef RT_USING_USBH_OHCI
#define HAL_OHCI_MODULE_ENABLED
#endif

#ifdef RT_USING_WDT
#define HAL_WDT_MODULE_ENABLED
#endif

#ifdef RT_USING_VOP
#define HAL_VOP_MODULE_ENABLED
#endif

/* HAL_DBG SUB CONFIG */
#define HAL_DBG_USING_RTT_SERIAL
#define HAL_DBG_ON
//#define HAL_DBG_INFO_ON
#define HAL_DBG_WRN_ON
#define HAL_DBG_ERR_ON
#define HAL_ASSERT_ON

#endif
