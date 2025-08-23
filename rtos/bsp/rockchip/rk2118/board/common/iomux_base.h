/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-02-07     Cliff Chen   first implementation
 */

#ifndef __BOARD_IOMUX_BASE_H__
#define __BOARD_IOMUX_BASE_H__

void dsp_jtag_iomux_config(void);
void fspi0_iomux_config(void);
void uart2_iomux_config(void);
void lcdc_iomux_config(void);
void mcu_jtag_m0_iomux_config(void);
void mcu_jtag_m1_iomux_config(void);
void spi2_iomux_config(void);
void i2c2_iomux_config(void);
void can_iomux_config(void);
void uart0_iomux_config(void);
void sai_mclkout_config_all(void);
void sai_mclkout_config(int saiId);
void sai_mclkin_config(int saiId);
void sai7_iomux_config(void);
void sai4_iomux_config(void);
void sai5_iomux_config(void);
void sai0_iomux_config(void);
void spi0_m1_iomux_config(void);
void sai1_iomux_config(void);
void sai2_iomux_config(void);
void pdm_iomux_config(void);
void rt_hw_iomux_config(void);

#endif
