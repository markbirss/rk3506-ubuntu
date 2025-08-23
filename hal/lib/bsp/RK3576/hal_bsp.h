/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#ifndef __BSP_H__
#define __BSP_H__

#include "hal_base.h"

/***************************** MACRO Definition ******************************/

/***************************** Structure Definition **************************/

#ifdef HAL_CANFD_MODULE_ENABLED
extern const struct HAL_CANFD_DEV g_can0Dev;
extern const struct HAL_CANFD_DEV g_can1Dev;
#endif

#ifdef HAL_PCIE_MODULE_ENABLED
extern struct HAL_PCIE_DEV g_pcie0Dev;
extern struct HAL_PCIE_DEV g_pcie1Dev;
#endif

#ifdef HAL_PDM_MODULE_ENABLED
extern struct HAL_PDM_DEV g_pdm0Dev;
#endif

#ifdef HAL_PWM_MODULE_ENABLED
extern const struct HAL_PWM_DEV g_pwm0Dev;
#if defined(HAL_AP_CORE) || defined(HAL_BUS_MCU_CORE)
extern const struct HAL_PWM_DEV g_pwm1Dev;
extern const struct HAL_PWM_DEV g_pwm2Dev;
#endif
#endif

#ifdef HAL_SPI_MODULE_ENABLED
extern const struct HAL_SPI_DEV g_spi0Dev;
extern const struct HAL_SPI_DEV g_spi1Dev;
extern const struct HAL_SPI_DEV g_spi2Dev;
extern const struct HAL_SPI_DEV g_spi3Dev;
extern const struct HAL_SPI_DEV g_spi4Dev;
#endif

#ifdef HAL_UART_MODULE_ENABLED
extern const struct HAL_UART_DEV g_uart0Dev;
extern const struct HAL_UART_DEV g_uart1Dev;
extern const struct HAL_UART_DEV g_uart2Dev;
extern const struct HAL_UART_DEV g_uart3Dev;
extern const struct HAL_UART_DEV g_uart4Dev;
extern const struct HAL_UART_DEV g_uart5Dev;
extern const struct HAL_UART_DEV g_uart6Dev;
extern const struct HAL_UART_DEV g_uart7Dev;
extern const struct HAL_UART_DEV g_uart8Dev;
extern const struct HAL_UART_DEV g_uart9Dev;
extern const struct HAL_UART_DEV g_uart10Dev;
extern const struct HAL_UART_DEV g_uart11Dev;
#endif

#ifdef HAL_VDMA_MODULE_ENABLED
extern struct HAL_VDMA_DEV g_vdma0Dev;
#endif

/***************************** Function Declare ******************************/
void BSP_Init(void);
void BSP_SetLoaderFlag(void);

#endif
