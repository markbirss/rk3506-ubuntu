/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __AIP1640_DISPLAY_H__
#define __AIP1640_DISPLAY_H__

#ifdef RT_USING_AIP1640
#include <rtthread.h>
#include <rtdevice.h>

#include <unistd.h>
#include <stdio.h>

#include <string.h>
#include <stdlib.h>

#define AX (1<<0)
#define BX (1<<1)
#define CX (1<<2)
#define DX (1<<3)
#define EX (1<<4)
#define FX (1<<5)
#define GX (1<<6)
#define HX (1<<7)

#define LETTER_A   (AX|BX|CX|EX|FX|GX)
#define LETTER_A1   GX

#define LETTER_B   (AX|BX|CX|DX)
#define LETTER_B1  (BX|GX|EX)

#define LETTER_C   (AX|FX|EX|DX)
#define LETTER_C1   (0)

#define LETTER_D   (AX|BX|CX|DX)
#define LETTER_D1  (BX|EX)

#define LETTER_E   (AX|FX|GX|EX|DX)
#define LETTER_E1   (GX)

#define LETTER_F  (AX|FX|EX|GX)
#define LETTER_F1  (GX)

#define LETTER_G   (AX|FX|EX|DX|CX)
#define LETTER_G1  (GX)

#define LETTER_H  (FX|EX|BX|CX|GX)
#define LETTER_H1  (GX)

#define LETTER_I   (AX|DX)
#define LETTER_I1  (BX|EX)

#define LETTER_J  (AX|DX|EX)
#define LETTER_J1  (BX|EX)

#define LETTER_K  (EX|FX|GX)
#define LETTER_K1 (CX|DX)

#define LETTER_L  (FX|EX|DX)
#define LETTER_L1  (0)

#define LETTER_M  (FX|EX|BX|CX)
#define LETTER_M1  (AX|CX)

#define LETTER_N  (FX|EX|BX|CX)
#define LETTER_N1 (AX|DX)

#define LETTER_O  (AX|BX|CX|DX|EX|FX)
#define LETTER_O1 (0)

#define LETTER_P   (AX|BX|EX|FX|GX)
#define LETTER_P1   (GX)

#define LETTER_Q  (AX|BX|CX|DX|EX|FX)
#define LETTER_Q1   (DX)

#define LETTER_R  (AX|BX|EX|FX|GX)
#define LETTER_R1  (DX|GX)

#define LETTER_S  (AX|CX|DX|FX|GX)
#define LETTER_S1 (GX)

#define LETTER_T  (AX)
#define LETTER_T1 (BX|EX)

#define LETTER_U  (FX|EX|DX|CX|BX)
#define LETTER_U1  (0)

#define LETTER_V  (EX|FX)
#define LETTER_V1  (CX|FX)

#define LETTER_W   (FX|EX|CX|BX)
#define LETTER_W1  (FX|DX)

#define LETTER_X  (0)
#define LETTER_X1  (AX|CX|FX|DX)

#define LETTER_Y  (0)
#define LETTER_Y1 (AX|CX|EX)

#define LETTER_Z  (AX|DX)
#define LETTER_Z1 (CX|FX)

#define NUM_0   (AX|BX|CX|DX|EX|FX)
#define NUM_0X   (0)

#define NUM_1    (BX|CX)
#define NUM_1X   (0)

#define NUM_2    (AX|BX|EX|DX|GX)
#define NUM_2X   (GX)

#define NUM_3    (AX|BX|CX|DX|GX)
#define NUM_3X    (GX)

#define NUM_4    (BX|CX|FX|GX)
#define NUM_4X   (GX)

#define NUM_5    (AX|FX|GX|CX|DX)
#define NUM_5X   (GX)

#define NUM_6    (AX|FX|EX|DX|CX|GX)
#define NUM_6X    (GX)

#define NUM_7    (AX|BX|CX)
#define NUM_7X   (0)

#define NUM_8   (AX|BX|CX|DX|EX|FX|GX)
#define NUM_8X   (GX)

#define NUM_9   (AX|BX|CX|DX|FX|GX)
#define NUM_9X   (GX)

#define ADD       (GX)
#define ADDX      (GX|BX|EX)

#define MINUS     (GX)
#define MINUSX    (GX)

#define UNDERLINE   DX
#define UNDERLINEX   0

#define BLACKSPACE   0
#define BLACKSPACEX  0

void rt_hw_aip1640_display_info(const char *dat);
void rt_hw_aip1640_play_status(const char *dat);
void rt_hw_aip1640_display_dimmer(const char *dat);
#endif
#endif