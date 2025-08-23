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

void rt_hw_iodomain_config(void);
void uart0_m0_iomux_config(void);
void uart2_m1_iomux_config(void);
void uart7_m1_iomux_config(void);
void uart4_m0_iomux_config(void);
void i2c1_m0_iomux_config(void);
void i2s0_8ch_m0_iomux_config(void);
void i2s1_8ch_m0_iomux_config(void);
void gmac0_m0_iomux_config(void);
void rt_hw_iomux_config(void);

#endif
