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
#include "board.h"

#define DIN_HIGH  HAL_GPIO_SetPinLevel(AIP1640_DATA_GPIO_BANKC, AIP1640_DATA_GPIO, GPIO_HIGH)
#define DIN_LOW   HAL_GPIO_SetPinLevel(AIP1640_DATA_GPIO_BANKC, AIP1640_DATA_GPIO, GPIO_LOW)
#define CLK_HIGH  HAL_GPIO_SetPinLevel(AIP1640_CLK_GPIO_BANKC, AIP1640_CLK_GPIO, GPIO_HIGH)
#define CLK_LOW   HAL_GPIO_SetPinLevel(AIP1640_CLK_GPIO_BANKC, AIP1640_CLK_GPIO, GPIO_LOW)


extern struct aip1640_info *g_aip1640_info;

void hw_aip1640_delay(void)
{
    HAL_DelayUs(50);
}

void hw_aip1640_init(void)
{
    CLK_LOW;
    hw_aip1640_delay();
    DIN_HIGH;
    hw_aip1640_delay();
    CLK_HIGH;
    hw_aip1640_delay();
    DIN_LOW;
    hw_aip1640_delay();
}

void hw_aip1640_start(void)
{
    CLK_LOW;
    hw_aip1640_delay();
    DIN_HIGH;
    hw_aip1640_delay();
    CLK_HIGH;
    hw_aip1640_delay();
    DIN_LOW;
    hw_aip1640_delay();
}

void hw_aip1640_stop(void)
{
    CLK_LOW;
    hw_aip1640_delay();
    DIN_LOW;
    hw_aip1640_delay();
    CLK_HIGH;
    hw_aip1640_delay();
    DIN_HIGH;
    hw_aip1640_delay();
}

void hw_aip1640_write_data(char dat)
{
    char temp1 = 0;
    CLK_LOW;
    hw_aip1640_delay();
    for (temp1 = 0; temp1 < 8; temp1++)
    {
        if (dat & 0x01)
        {
            DIN_HIGH;
            hw_aip1640_delay();
        }
        else
        {
            DIN_LOW;
            hw_aip1640_delay();
        }
        CLK_HIGH;
        hw_aip1640_delay();
        dat >>= 1;
        CLK_LOW;
        hw_aip1640_delay();
    }
}

void hw_aip1640_display_dimmer(int dim)
{
    hw_aip1640_start();
    hw_aip1640_write_data(dim);
    hw_aip1640_stop();
}

void hw_aip1640_display(char *dat)
{
    int i;

    hw_aip1640_start();
    hw_aip1640_write_data(0x40);
    hw_aip1640_stop();

    hw_aip1640_start();
    hw_aip1640_write_data(0xC0);
    for (i = 0; i < 16; i++)
    {
        hw_aip1640_write_data(dat[i]);
    }
    hw_aip1640_stop();

    hw_aip1640_start();
    hw_aip1640_write_data(g_aip1640_info->dim_val);
    hw_aip1640_stop();

}
#endif
