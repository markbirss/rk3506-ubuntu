/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 */

#ifndef __BSP_H__
#define __BSP_H__

#include "hal_base.h"

/***************************** MACRO Definition ******************************/

/***************************** Structure Definition **************************/
#ifdef HAL_ACDCDIG_DSM_MODULE_ENABLED
extern struct HAL_ACDCDIG_DSM_DEV g_acdcDigDSMDev;
#endif

#ifdef HAL_ACODEC_MODULE_ENABLED
extern struct HAL_ACODEC_DEV g_acodecDev;
#endif

#ifdef HAL_CANFD_MODULE_ENABLED
extern const struct HAL_CANFD_DEV g_can0Dev;
extern const struct HAL_CANFD_DEV g_can1Dev;
#endif

#ifdef HAL_DSMC_HOST_MODULE_ENABLED
extern struct HAL_DSMC_HOST g_dsmcHostDev;
#endif

#ifdef HAL_DSMC_SLAVE_MODULE_ENABLED
extern struct HAL_DSMC_SLAVE g_dsmcSlaveDev;
#endif

#ifdef HAL_FLEXBUS_MODULE_ENABLED
extern const struct HAL_FLEXBUS_DEV g_flexbusDev;
#ifdef HAL_FLEXBUS_ADC_MODULE_ENABLED
extern const struct HAL_FLEXBUS_ADC_DEV g_flexbusADCDev;
#endif
#endif

#ifdef HAL_FSPI_MODULE_ENABLED
extern struct HAL_FSPI_HOST g_fspi0Dev;
#endif

#ifdef HAL_GMAC_MODULE_ENABLED
extern const struct HAL_GMAC_DEV g_gmac0Dev;
extern const struct HAL_GMAC_DEV g_gmac1Dev;
#endif

#ifdef HAL_HCD_MODULE_ENABLED
extern const struct HAL_USB_DEV g_usbotgh0Dev;
extern const struct HAL_USB_DEV g_usbotgh1Dev;
#endif

#ifdef HAL_I2C_MODULE_ENABLED
extern const struct HAL_I2C_DEV g_i2c0Dev;
extern const struct HAL_I2C_DEV g_i2c1Dev;
extern const struct HAL_I2C_DEV g_i2c2Dev;
#endif

#ifdef HAL_PCD_MODULE_ENABLED
extern const struct HAL_USB_DEV g_usbdDev;
#endif

#ifdef HAL_PL330_MODULE_ENABLED
extern struct HAL_PL330_DEV g_pl330Dev0;
extern struct HAL_PL330_DEV g_pl330Dev1;
#endif

#ifdef HAL_PWM_MODULE_ENABLED
extern const struct HAL_PWM_DEV g_pwm0Dev;
extern const struct HAL_PWM_DEV g_pwm1Dev;
#endif

#ifdef HAL_SAI_MODULE_ENABLED
extern struct HAL_SAI_DEV g_sai0Dev;
extern struct HAL_SAI_DEV g_sai1Dev;
extern struct HAL_SAI_DEV g_sai2Dev;
extern struct HAL_SAI_DEV g_sai3Dev;
extern struct HAL_SAI_DEV g_sai4Dev;
#endif

#ifdef HAL_SPDIFRX_MODULE_ENABLED
extern struct HAL_SPDIFRX_DEV g_spdifrx0Dev;
#endif

#ifdef HAL_SPDIFTX_MODULE_ENABLED
extern struct HAL_SPDIFTX_DEV g_spdiftx0Dev;
#endif

#ifdef HAL_SPI_MODULE_ENABLED
extern const struct HAL_SPI_DEV g_spi0Dev;
extern const struct HAL_SPI_DEV g_spi1Dev;
#endif

#ifdef HAL_TRNG_MODULE_ENABLED
extern const struct HAL_TRNG_DEV g_trngnsDev;
extern const struct HAL_TRNG_DEV g_trngsDev;
#endif

#ifdef HAL_UART_MODULE_ENABLED
extern const struct HAL_UART_DEV g_uart0Dev;
extern const struct HAL_UART_DEV g_uart1Dev;
extern const struct HAL_UART_DEV g_uart2Dev;
extern const struct HAL_UART_DEV g_uart3Dev;
extern const struct HAL_UART_DEV g_uart4Dev;
extern const struct HAL_UART_DEV g_uart5Dev;
#endif

/***************************** Function Declare ******************************/
void BSP_Init(void);
void BSP_SetLoaderFlag(void);

#endif
