/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd
 */

/**
 * @file  hal_conf_template.h
 */

#ifndef _HAL_CONF_H_
#define _HAL_CONF_H_

#include "rtconfig.h"

#define HAL_MCU_CORE
#define __RISC_V

#ifdef RT_USING_UART
#define HAL_UART_MODULE_ENABLED
#endif

#ifdef RT_USING_CRU
#define HAL_CRU_MODULE_ENABLED
#endif

#define HAL_DBG_USING_RTT_SERIAL 1   /* redirect the hal log to rtt console */

#endif /* _HAL_CONF_H_ */
