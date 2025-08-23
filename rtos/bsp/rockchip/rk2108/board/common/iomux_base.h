/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-25     Cliff Chen   first implementation
 */

#ifndef __BOARD_IOMUX_BASE_H__
#define __BOARD_IOMUX_BASE_H__

void pdm_input_iomux_config(void);
void i2s1_input_iomux_config(void);
void i2s1_output_iomux_config(void);
void vicap_iomux_config(void);
void lcdc_iomux_config(void);
void i2c0_m0_iomux_config(void);
void i2c0_m1_iomux_config(void);
void i2c0_m2_iomux_config(void);
void i2c1_m0_iomux_config(void);
void i2c1_m1_iomux_config(void);
void i2c1_m2_iomux_config(void);
void i2c2_m0_iomux_config(void);
void i2c2_m1_iomux_config(void);
void i2c2_m2_iomux_config(void);
void sfc0_iomux_config(void);
void sdio_iomux_config(void);
void dsp_jtag_iomux_config(void);
void uart2_iomux_config(void);
void uart2_rts_iomux_config(int uart_en);
void pcm_iomux_config(void);
void key_ctrl_iomux_config(void);
void uart0_iomux_config(void);
void uart1_m0_iomux_config(void);
void uart1_m1_iomux_config(void);
void uart1_m2_iomux_config(void);
void uart1_m3_iomux_config(void);
void audio_iomux_config(void);
void m4_jtag_iomux_config(void);
void sfc1_iomux_config(void);
void spi1_m0_iomux_config(void);
void spi1_m1_iomux_config(void);
void spi2_m0_iomux_config(void);
void spi2_m1_iomux_config(void);
void rt_hw_iomux_config(void);

#endif
