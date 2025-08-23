/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#ifndef _HAL_CONF_H_
#define _HAL_CONF_H_

/* CPU config */
#define SOC_RV1106
#define HAL_AP_CORE
#define SYS_TIMER TIMER5 /* System timer designation (RK TIMER) */

/* HAL Driver Config */
#define HAL_CRU_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_PINCTRL_MODULE_ENABLED
#define HAL_PWM_MODULE_ENABLED
#define HAL_SARADC_MODULE_ENABLED
#define HAL_TIMER_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED

/* HAL_DBG SUB CONFIG */
#ifdef HAL_DBG_ON
#define HAL_DBG_USING_LIBC_PRINTF
#define HAL_DBG_INFO_ON
#define HAL_DBG_WRN_ON
#define HAL_DBG_ERR_ON
#define HAL_ASSERT_ON
#endif

#endif
