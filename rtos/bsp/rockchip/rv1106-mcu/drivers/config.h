/**
  * Copyright (c) 2020 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    board.h
  * @author  Jason Zhu
  * @version V0.1
  * @date    26-Feb-2020
  * @brief
  *
  ******************************************************************************
  */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define CONFIG_ADDRESS 0x100000
#define CONFIG_MAGIC   "CONMAG"

#define BOOT_FROM_UNKNOWN 0
#define BOOT_FROM_SPL     1
#define BOOT_FROM_UBOOT   2

/* ISP CONFIG */
#define AE_GAIN_RANGE_NUM     10
#define AE_TIME_FACTOR_NUM    4
#define AE_DOT_NO             6
#define AE_HDR_EXP_NUM        3
#define ADC_CALIB_NO          4
#define ENV_CALIB_NO          2

struct ae_gain_range
{
    uint32_t range_min;
    uint32_t range_max;
    uint32_t C1;
    uint32_t C0;
    uint32_t M0;
    uint32_t minReg;
    uint32_t maxReg;
};

struct ae_start_exp
{
    uint32_t start_exposure_light;
    uint32_t start_exposure_dark;
    uint32_t start_time_light[AE_HDR_EXP_NUM];
    uint32_t start_gain_light[AE_HDR_EXP_NUM];
    uint32_t start_time_dark[AE_HDR_EXP_NUM];
    uint32_t start_gain_dark[AE_HDR_EXP_NUM];

    uint32_t start_time_light_reg[AE_HDR_EXP_NUM];
    uint32_t start_gain_light_reg[AE_HDR_EXP_NUM];
    uint32_t start_time_dark_reg[AE_HDR_EXP_NUM];
    uint32_t start_gain_dark_reg[AE_HDR_EXP_NUM];
};

struct ae_init_info
{
    uint32_t hts;
    uint32_t vts;
    uint32_t fps;
    uint32_t hts_aiq;
    uint32_t vts_aiq;
    uint32_t fps_aiq;
    uint32_t black_lvl;
    uint32_t setpoint;
    uint32_t tolerance;
    uint32_t dampratio;
    struct ae_start_exp start_exp;
    struct ae_gain_range gain_range[AE_GAIN_RANGE_NUM];
    uint32_t gain_range_size;
    uint32_t time_factor[AE_TIME_FACTOR_NUM];
    uint32_t time_dot[AE_DOT_NO];
    uint32_t gain_dot[AE_DOT_NO];
    uint32_t is_lux_en;
    uint32_t adc_calib[ADC_CALIB_NO];
    uint32_t env_calib[ENV_CALIB_NO];
};

struct isp_init_info
{
    uint32_t share_mem_addr;
    uint32_t share_mem_size;
    struct ae_init_info ae_init;
};

struct sensor_init_info
{
    char dev_name[12]; /* sc210iot, imx334, imx415.... */
    uint32_t frame_cnt; /* total frame, 1...n */
    int bitw;   /* pixel bit width 8 10 12... */
    int width;  /* image width */
    int height; /* image height */
    int hdr_mode; /* 0: liner, 1: hdr1, 2: hdr2, 3: hdr3... */
};

struct bord_init_info
{
    int camera_pwdn_gpio_bank;
    int camera_pwdn_gpio_pin;
    int camera_rst_gpio_bank;
    int camera_rst_gpio_pin;
    int camera_mclk_gpio_bank;
    int camera_mclk_gpio_pin;
    int camera_pw_en_bank;
    int camera_pw_en_gpio_pin;
    int ircut_enb_gpio_bank;
    int ircut_enb_gpio_pin;
    int ircut_fbc_gpio_bank;
    int ircut_fbc_gpio_pin;
    int ir_led_en_gpio_bank;
    int ir_led_en_gpio_pin;
    int ir_led_pwm_gpio_bank;
    int ir_led_pwm_gpio_pin;
    int red_led_pwm_gpio_bank;
    int red_led_pwm_gpio_pin;
    int white_led_en_gpio_bank;
    int white_led_en_gpio_pin;
    int white_led_pwm_gpio_bank;
    int white_led_pwm_gpio_pin;
    int blue_led_pwm_gpio_bank;
    int blue_led_pwm_gpio_pin;
    int lights_sensor_adc_ch;
};

struct config_param
{
    char magic[8];
    int is_ready;
    int boot_from;
    struct isp_init_info isp;
    struct sensor_init_info sensor;
    struct bord_init_info board;
    int hash;
};

int config_is_integrity(struct config_param *param);
int config_param_parse(struct config_param *param);

#endif
