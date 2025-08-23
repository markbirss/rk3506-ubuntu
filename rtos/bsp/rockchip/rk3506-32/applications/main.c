/*
 * Copyright (c) 2021 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-12     Steven Liu   the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "hal_base.h"

#if defined(HAL_DBG_USING_LIBC_PRINTF) || defined(HAL_DBG_USING_HAL_PRINTF)
static struct UART_REG *pUart = UART4;

#ifdef __GNUC__
__USED int _write(int fd, char *ptr, int len)
{
    int i = 0;

    /*
     * write "len" of char from "ptr" to file id "fd"
     * Return number of char written.
     *
    * Only work for STDOUT, STDIN, and STDERR
     */
    if (fd > 2)
    {
        return -1;
    }

    while (*ptr && (i < len))
    {
        if (*ptr == '\n')
        {
            HAL_UART_SerialOutChar(pUart, '\r');
        }
        HAL_UART_SerialOutChar(pUart, *ptr);

        i++;
        ptr++;
    }

    return i;
}
#else
int fputc(int ch, FILE *f)
{
    if (ch == '\n')
    {
        HAL_UART_SerialOutChar(pUart, '\r');
    }

    HAL_UART_SerialOutChar(pUart, (char)ch);

    return 0;
}
#endif /* end of __GNUC__ */
#endif /* end of HAL_DBG_USING_LIBC_SERIAL */

int main(int argc, char **argv)
{
    rt_kprintf("Hi, this is RT-Thread!!\n");

#ifdef RT_USING_NEW_OTA
    /*
     * rk_ota_set_boot_success will set `successful_boot` flag to 1.
     * Move this function to a location where all of your service
     * has finished to make sure to set `successful_boot` after
     * all of your main service has succeeded.
     */
    rk_ota_set_boot_success();
#endif

    return 0;
}
