/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-09     Cliff Chen   first implementation
 */

#ifndef __IOMUX_BASE_H__
#define __IOMUX_BASE_H__

void i2c4_m2_iomux_config(void);
void pwm10_m1_iomux_config(void);
void pwm11_m1_iomux_config(void);
void uart0_m1_iomux_config(void);
void uart2_m0_iomux_config(void);
void rt_hw_iomux_config(void);
void pwm7_ch1_iomux_config(void);
void lcdc_rgb3x8_iomux_config(void);

#endif
