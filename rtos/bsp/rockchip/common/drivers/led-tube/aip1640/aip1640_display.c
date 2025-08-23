/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtthread.h>
#include <rtdevice.h>

#ifdef RT_USING_AIP1640
#include "aip1640_main.h"
#include "aip1640_serial.h"
#include "aip1640_display.h"

extern struct aip1640_info *g_aip1640_info;
enum clear_flag
{
    CLEAR_ICON = 1,
    CLEAR_CHAR,
};

const static char alpha_letter[52] =
{
    LETTER_A, LETTER_A1, LETTER_B, LETTER_B1, LETTER_C, LETTER_C1, LETTER_D, LETTER_D1, LETTER_E, LETTER_E1,
    LETTER_F, LETTER_F1, LETTER_G, LETTER_G1, LETTER_H, LETTER_H1, LETTER_I, LETTER_I1, LETTER_J, LETTER_J1,
    LETTER_K, LETTER_K1, LETTER_L, LETTER_L1, LETTER_M, LETTER_M1, LETTER_N, LETTER_N1, LETTER_O, LETTER_O1,
    LETTER_P, LETTER_P1, LETTER_Q, LETTER_Q1, LETTER_R, LETTER_R1, LETTER_S, LETTER_S1, LETTER_T, LETTER_T1,
    LETTER_U, LETTER_U1, LETTER_V, LETTER_V1, LETTER_W, LETTER_W1, LETTER_X, LETTER_X1, LETTER_Y, LETTER_Y1,
    LETTER_Z, LETTER_Z1
};

const static char digit_number[20] =
{
    NUM_0, NUM_0X, NUM_1, NUM_1X, NUM_2, NUM_2X, NUM_3, NUM_3X, NUM_4, NUM_4X, NUM_5, NUM_5X, NUM_6, NUM_6X,
    NUM_7, NUM_7X, NUM_8, NUM_8X, NUM_9, NUM_9X
};

int string_to_int(char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    else if (ch >= 'A' && ch <= 'Z')
    {
        return ch - 'A';
    }
    else if (ch >= 'a' && ch <= 'z')
    {
        return ch - 'a';
    }
    return -1;
}

void rt_hw_aip1640_reset(int flag)
{
    if (flag == CLEAR_ICON)
    {
        int j;
        for (j = 0; j < 13; j++)
        {
            if ((j != 5) && (j != 8))
            {
                g_aip1640_info->aip1640_buf[j] &= 0x7F;
            }
            if (j == 12)
            {
                g_aip1640_info->aip1640_buf[j] &= 0x30;
            }
        }
    }

    if (flag == CLEAR_CHAR)
    {
        g_aip1640_info->aip1640_buf[0] &= 0x80;
        g_aip1640_info->aip1640_buf[1] &= 0x80;
        g_aip1640_info->aip1640_buf[2] &= 0x80;
        g_aip1640_info->aip1640_buf[3] &= 0x80;
        g_aip1640_info->aip1640_buf[4] &= 0x80;
        g_aip1640_info->aip1640_buf[5] &= 0x0;
        g_aip1640_info->aip1640_buf[6] &= 0x80;
        g_aip1640_info->aip1640_buf[7] &= 0x80;
        g_aip1640_info->aip1640_buf[8] &= 0x0;
        g_aip1640_info->aip1640_buf[9] &= 0x80;
        g_aip1640_info->aip1640_buf[10] &= 0x0;
        g_aip1640_info->aip1640_buf[11] &= 0x80;
    }

}

//show icon
void rt_hw_aip1640_icon(const char *dat)
{
    rt_hw_aip1640_reset(CLEAR_ICON);

    if (strstr(dat, "USB") != NULL)
    {
        g_aip1640_info->aip1640_buf[12] |= 0x04;
        g_aip1640_info->mode_buf = 12;
        g_aip1640_info->mode_val = 0x04;
        if (strstr(dat, "NOUSB") != NULL)
            g_aip1640_info->aip1640_buf[12] &= 0x0f;
    }
    else if (strstr(dat, "SD") != NULL)
    {
        g_aip1640_info->aip1640_buf[12] |= 0x08;
        g_aip1640_info->mode_buf = 12;
        g_aip1640_info->mode_val = 0x08;
        if (strstr(dat, "NOSD") != NULL)
            g_aip1640_info->aip1640_buf[12] &= 0x0f;
    }
    else if (strstr(dat, "HDMI") != NULL)
    {
        g_aip1640_info->aip1640_buf[6] |= 0x80;
        g_aip1640_info->aip1640_buf[12] &= 0x0f;
        g_aip1640_info->mode_buf = 6;
        g_aip1640_info->mode_val = 0x80;
    }
    else if (strstr(dat, "BT") != NULL)
    {
        g_aip1640_info->aip1640_buf[12] |= 0x01;
        g_aip1640_info->mode_buf = 12;
        g_aip1640_info->mode_val = 0x01;
        if (strstr(dat, "NO BT") != NULL)
            g_aip1640_info->aip1640_buf[12] &= 0x0f;
    }
    else if (strstr(dat, "MIC") != NULL)
    {
        g_aip1640_info->aip1640_buf[7] |= 0x80;
        g_aip1640_info->mode_buf = 7;
        g_aip1640_info->mode_val = 0x80;
    }
    else if (strstr(dat, "COAX") != NULL)
    {
        g_aip1640_info->aip1640_buf[3] |= 0x80;
        g_aip1640_info->aip1640_buf[12] &= 0x0f;
        g_aip1640_info->mode_buf = 3;
        g_aip1640_info->mode_val = 0x80;
    }
    if (!strcmp(dat, "ARC"))
    {
        g_aip1640_info->aip1640_buf[0] |= 0x80;
        g_aip1640_info->aip1640_buf[1] &= 0x7f;
        g_aip1640_info->aip1640_buf[12] &= 0x0f;
        g_aip1640_info->mode_buf = 0;
        g_aip1640_info->mode_val = 0x80;
    }
    else if (!strcmp(dat, "EARC"))
    {
        g_aip1640_info->aip1640_buf[0] &= 0x7f;
        g_aip1640_info->aip1640_buf[1] |= 0x80;
        g_aip1640_info->aip1640_buf[12] &= 0x0f;
        g_aip1640_info->mode_buf = 1;
        g_aip1640_info->mode_val = 0x80;
    }
    else if (!strcmp(dat, "EARC "))
    {
        g_aip1640_info->aip1640_buf[0] &= 0x7f;
        g_aip1640_info->aip1640_buf[1] &= 0x7f;
        g_aip1640_info->aip1640_buf[12] &= 0x0f;
        g_aip1640_info->mode_buf = 0;
        g_aip1640_info->mode_val = 0x0;
    }
    else if (strstr(dat, "OPT") != NULL)
    {
        g_aip1640_info->aip1640_buf[2] |= 0x80;
        g_aip1640_info->aip1640_buf[12] &= 0x0f;
        g_aip1640_info->mode_buf = 2;
        g_aip1640_info->mode_val = 0x80;
    }
    else if (strstr(dat, "AUX") != NULL)
    {
        g_aip1640_info->aip1640_buf[7] |= 0x80;
        g_aip1640_info->aip1640_buf[12] &= 0x0f;
        g_aip1640_info->mode_buf = 7;
        g_aip1640_info->mode_val = 0x80;
    }
    else
    {
        g_aip1640_info->aip1640_buf[g_aip1640_info->mode_buf] |= g_aip1640_info->mode_val;
    }
}

void rt_hw_aip1640_play_status(const char *dat)
{
    if (strstr(dat, "PLAY") != NULL)
    {
        g_aip1640_info->aip1640_buf[12] &= 0xef;
        g_aip1640_info->aip1640_buf[12] |= 0x20;
    }
    else if (strstr(dat, "PAUSE") != NULL)
    {
        g_aip1640_info->aip1640_buf[12] &= 0xdf;
        g_aip1640_info->aip1640_buf[12] |= 0x10;
    }
    hw_aip1640_display(g_aip1640_info->aip1640_buf);
}

void rt_hw_aip1640_display_info(const char *dat)
{
    int i, j, k = 0;

    rt_hw_aip1640_icon(dat);
    rt_hw_aip1640_reset(CLEAR_CHAR);

    for (i = 0; i < 5; i++)
    {
        if (dat[i] == '\0')
            break;
        if (('0' <= dat[i]) && (dat[i] <= '9'))
        {
            j = string_to_int(dat[i]);
            g_aip1640_info->aip1640_buf[k] |= digit_number[j * 2];
            g_aip1640_info->aip1640_buf[k + 1] |= digit_number[j * 2 + 1];
            k += 2;
        }
        else if ((('A' <= dat[i]) && (dat[i] <= 'Z')) || (('a' <= dat[i]) && (dat[i] <= 'z')))
        {

            j = string_to_int(dat[i]);
            g_aip1640_info->aip1640_buf[k] |= alpha_letter[j * 2];
            g_aip1640_info->aip1640_buf[k + 1] |= alpha_letter[j * 2 + 1];
            k += 2;
        }
        else if (dat[i] == '+')
        {
            g_aip1640_info->aip1640_buf[k] |= ADD;
            g_aip1640_info->aip1640_buf[k + 1] |= ADDX;
            k += 2;
        }
        else if (dat[i] == '-')
        {
            g_aip1640_info->aip1640_buf[k] |= MINUS;
            g_aip1640_info->aip1640_buf[k + 1] |= MINUSX;
            k += 2;
        }
        else if (dat[i] == '_')
        {
            g_aip1640_info->aip1640_buf[k] |= UNDERLINE;
            g_aip1640_info->aip1640_buf[k + 1] |= UNDERLINEX;
            k += 2;
        }
        else if (dat[i] == ' ')
        {
            g_aip1640_info->aip1640_buf[k] |= BLACKSPACE;
            g_aip1640_info->aip1640_buf[k + 1] |= BLACKSPACEX;
            k += 2;
        }
    }
    hw_aip1640_display(g_aip1640_info->aip1640_buf);
}

void rt_hw_aip1640_display_dimmer(const char *dat)
{
    if (strstr(dat, "DIM 0") != NULL)
    {
        g_aip1640_info->dim_val = 0x80;
    }
    else if (strstr(dat, "DIM 1") != NULL)
    {
        g_aip1640_info->dim_val = 0x88;
    }
    else if (strstr(dat, "DIM 2") != NULL)
    {
        g_aip1640_info->dim_val = 0x8a;
    }
    else if (strstr(dat, "DIM 3") != NULL)
    {
        g_aip1640_info->dim_val = 0x8c;
    }
    else
    {
        g_aip1640_info->dim_val = 0x8f;
    }
    hw_aip1640_display(g_aip1640_info->aip1640_buf);
}
#endif
