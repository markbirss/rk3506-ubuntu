/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rthw.h>
#include <rtthread.h>

#include "board_cam.h"

#define FLGA_GPIO_OUTPUT         (1 << 25)
#define FLAG_GPIO_DEFAULT_HIGH   (1 << 26)
#define GET_GPIO_PIN_NUM(x)      (1 << (x))

/**
 * @brief  get pwm device's name
 */
const char *cam_get_pwm_name(int pwm_channel)
{
    switch (CAM_PWM_GET_DEVICE(pwm_channel))
    {
    case 0:
        return "pwm0";
        break;

    case 1:
        return "pwm1";
        break;

    case 2:
        return "pwm2";
        break;

    default:
        break;
    }
    return NULL;
}

/**
 * @brief  Set gpio level for RV1106
 */
int cam_set_gpio_level(int gpio_index, int gpio_level)
{
    struct GPIO_REG *pGPIO = NULL;
    int gpio_group = -1;
    int gpio_pin = -1;
    gpio_group = gpio_index / 32;
    gpio_pin = gpio_index % 32;

    switch (gpio_group)
    {
    case 0:
        pGPIO = GPIO0;
        break;
    case 1:
        pGPIO = GPIO1;
        break;
    case 2:
        pGPIO = GPIO2;
        break;
    case 3:
        pGPIO = GPIO3;
        break;
    case 4:
        pGPIO = GPIO4;
        break;
    default:
        return -1;
        break;
    }
    HAL_GPIO_SetPinLevel(pGPIO, GET_GPIO_PIN_NUM(gpio_pin), gpio_level);
    return 0;
}

/**
 * @brief  Config gpio function for RV1106
 */
int cam_cfg_gpio_func(int gpio_index, int gpio_func)
{
    struct GPIO_REG *pGPIO = NULL;
    int gpio_group = -1;
    int gpio_pin = -1;
    int gpio_bank = -1;
    int gpio_direction = -1;
    int gpio_default_level = -1;
    int pin_config_mux = 0;

    gpio_group = gpio_index / 32;
    gpio_pin = gpio_index % 32;

    switch (gpio_group)
    {
    case 0:
        pGPIO = GPIO0;
        gpio_bank = 0;
        break;
    case 1:
        pGPIO = GPIO1;
        gpio_bank = 1;
        break;
    case 2:
        pGPIO = GPIO2;
        gpio_bank = 2;
        break;
    case 3:
        pGPIO = GPIO3;
        gpio_bank = 3;
        break;
    case 4:
        pGPIO = GPIO4;
        gpio_bank = 4;
        break;
    default:
        return -1;
        break;
    }

    if (gpio_func & FLGA_GPIO_OUTPUT)
    {
        gpio_direction = GPIO_OUT;
    }
    else
    {
        gpio_direction = GPIO_IN;
    }
    if (gpio_func & FLAG_GPIO_DEFAULT_HIGH)
    {
        gpio_default_level = GPIO_HIGH;
    }
    else
    {
        gpio_default_level = GPIO_LOW;
    }
    pin_config_mux = (gpio_func | FLAG_MUX) & (~FLGA_GPIO_OUTPUT) & (~FLAG_GPIO_DEFAULT_HIGH);
    HAL_GPIO_SetPinDirection(pGPIO, GET_GPIO_PIN_NUM(gpio_pin), gpio_direction);
    HAL_GPIO_SetPinLevel(pGPIO, GET_GPIO_PIN_NUM(gpio_pin), gpio_default_level);
    HAL_PINCTRL_SetIOMUX(gpio_bank, GET_GPIO_PIN_NUM(gpio_pin), pin_config_mux);
    return 0;
}
