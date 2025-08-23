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

void uart0_iomux_config(void);
void uart3_iomux_config(void);
void uart4_iomux_config(void);
void uart5_iomux_config(void);
void rt_hw_iomux_config(void);
void i2c0_iomux_config(void);
void i2c2_iomux_config(void);
void rmii0_iomux_config(void);
void rmii1_iomux_config(void);
void can0_iomux_config(void);
void dsmc_host_iomux_config(void);
void dsmc_host_lb_iomux_config(void);
void dsmc_slave_iomux_config(void);
void emmc_iomux_config(void);
void pwm0_iomux_config(void);
void lcdc_iomux_config(void);
void flexbus_adc_iomux_config(void);
void flexbus_dac_iomux_config(void);

#endif
