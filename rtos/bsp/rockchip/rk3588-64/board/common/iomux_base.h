/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-12     Steven Liu   first implementation
 */

#ifndef __IOMUX_BASE_H__
#define __IOMUX_BASE_H__

void rt_hw_iodomain_config(void);
void uart2_m0_iomux_config(void);
void uart4_m1_iomux_config(void);
void uart5_m0_iomux_config(void);
void can1_m1_iomux_config(void);
void gmac0_iomux_config(void);
void gmac1_m1_iomux_config(void);
void rt_hw_iomux_config(void);

#endif
