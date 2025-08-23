/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-10     Cliff      add board.h to this bsp
 *
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "board_base.h"

#ifdef RT_USING_CODEC_ES8311
#define PA_GPIO_BANK     GPIO_BANK0
#define PA_GPIO          GPIO0
#define PA_PIN           GPIO_PIN_B1
#define PA_PIN_FUNC_GPIO PIN_CONFIG_MUX_FUNC0
#endif

#endif
