/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-10     Cliff Chen   first implementation
 */

#ifndef __BOARD_BASE_H__
#define __BOARD_BASE_H__

#include "rtconfig.h"

// Internal SRAM memory size[Bytes]
#ifndef RK_SRAM_END
#define RK_SRAM_END             0x20100000
#endif
#ifndef RT_USING_UNCACHE_HEAP
#define RK_SYS_HEAP_END         RK_SRAM_END
#else
#define RK_UNCACHE_HEAP_SIZE    (0x2U << RT_UNCACHE_HEAP_ORDER)
#define RK_SYS_HEAP_END         (RK_SRAM_END - RK_UNCACHE_HEAP_SIZE)
#define RK_UNCACHE_HEAP_START   RK_SYS_HEAP_END
#endif

#ifdef RT_USING_BACKLIGHT
#define LCD_BACKLIGHT_PWM "pwm0"
#define LCD_BACKLIGHT_PWM_CHANNEL 2
#define LCD_BACKLIGHT_PWM_INVERT 1
#endif

//board macro resource
//audio pwm
#ifdef RT_USING_AUDIOPWM
#define AUDIOPWM_PA_GPIO_BANK     GPIO_BANK0
#define AUDIOPWM_PA_GPIO          GPIO0
#define AUDIOPWM_PA_PIN           GPIO_PIN_B1
#define AUDIOPWM_PA_PIN_FUNC_GPIO PIN_CONFIG_MUX_FUNC0
#endif

#ifdef RT_USING_CAMERA
#define CAMERA_PWDN_GPIO_BANK              GPIO_BANK1
#define CAMERA_PWDN_GPIO_GROUP             GPIO1
#define CAMERA_PWDN_GPIO_PIN               GPIO_PIN_A5
#define CAMERA_RST_GPIO_BANK               GPIO_BANK0
#define CAMERA_RST_GPIO_GROUP              GPIO0
#define CAMERA_RST_GPIO_PIN                GPIO_PIN_A0
#endif

#ifdef RT_USING_USB_DEVICE
#define USB_VBUS_PIN            BANK_PIN(GPIO_BANK1, 24)
#endif

#ifdef RT_USING_PMIC_RK816
#define PMIC_INT_PIN            BANK_PIN(GPIO_BANK0, 30)
#endif

#ifdef RT_USING_SPI_SCREEN
#define HAL_SPI_PANEL_SCLK      48000000
#define HAL_PANEL_SPI           "spi2_1"
#define GPIO_DCX_GPIO_PORT      GPIO0
#define GPIO_DCX_GPIO_PIN_OUT   GPIO_PIN_C4
#endif

#ifdef RT_USING_TOUCH
#define TOUCH_IRQ_PIN           BANK_PIN(GPIO_BANK1, 8)
#define TOUCH_RST_PIN           BANK_PIN(GPIO_BANK1, 9)
#define TOUCH_I2C_DEV           "i2c1"
#define TOUCH_DEV_NAME          "gtxx"
#endif

#ifdef RT_USING_SYSTICK
#define TICK_IRQn  SysTick_IRQn
#else
#define TICK_TIMER TIMER4
#define TICK_IRQn  TIMER4_IRQn
#endif

void rt_hw_board_init(void);

#endif
