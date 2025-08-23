/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_otp.c
  * @version V0.1
  * @brief   otp interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-04-30     Finley.Xiao     first implementation
  *
  ******************************************************************************
  */

#include <rtdef.h>
#include <rthw.h>
#include <rtthread.h>

#if defined(RT_USING_OTP)

#include "hal_base.h"
#include "drv_otp.h"

#define ROUNDUP(a, b) ((((a) - 1) / (b) + 1) * (b))
#define ROUNDDOWN(a, b) ((a) / (b) * (b))

extern const struct otp_data g_otp_data;

rt_err_t otp_read(uint8_t offset, uint32_t count, uint8_t *val)
{
    uint32_t nbytes = g_otp_data.nbytes;
    uint32_t addr_start, addr_end, addr_offset, addr_len, out_value;
    uint32_t i = 0;
    uint8_t *buf;

    addr_start = ROUNDDOWN(offset, nbytes) / nbytes;
    addr_end = ROUNDUP(offset + count, nbytes) / nbytes;
    addr_offset = offset % nbytes;
    addr_len = addr_end - addr_start;
    addr_start += g_otp_data.ns_offset;

    buf = rt_calloc(addr_len * nbytes, sizeof(uint8_t));
    if (!buf)
    {
        rt_kprintf("rt_malloc failed\n");
        return -RT_ENOMEM;
    }

    while (addr_len--)
    {
        if (HAL_OTP_Read(g_otp_data.reg, addr_start, &out_value) != HAL_OK)
        {
            rt_kprintf("hal otp read failed\n");
            rt_free(buf);
            return -RT_ERROR;
        }
        memcpy(&buf[i], &out_value, nbytes);
        addr_start++;
        i += nbytes;
    }
    memcpy(val, buf + addr_offset, count);
    rt_free(buf);

    return RT_EOK;
}

void otp_dump(int argc, char **argv)
{
    uint32_t i = 0;
    uint8_t *val;

    val = rt_calloc(g_otp_data.size, sizeof(uint8_t));
    if (!val)
    {
        rt_kprintf("rt_malloc failed\n");
        return;
    }

    if (otp_read(0, g_otp_data.size, val) != RT_EOK)
    {
        rt_kprintf("read otp failed\n");
        rt_free(val);
        return;
    }

    for (i = 0; i < g_otp_data.size; i += 8)
    {
        if ((g_otp_data.size - i) % 8 == 0)
        {
            rt_kprintf("%08x: %02x %02x %02x %02x %02x %02x %02x %02x\n", i,
                       val[i], val[i + 1], val[i + 2], val[i + 3],
                       val[i + 4], val[i + 5], val[i + 6], val[i + 7]);
        }
    }
    rt_free(val);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(otp_dump, otp driver dump. e.g: otp_dump);
#endif

#endif
