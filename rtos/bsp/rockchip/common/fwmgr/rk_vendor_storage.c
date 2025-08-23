/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtdevice.h>
#include <rtthread.h>
#include <string.h>
#include <stdlib.h>

#include "rk_vendor_storage.h"

static int (*_vendor_read)(u32 id, void *pbuf, u32 size);
static int (*_vendor_write)(u32 id, void *pbuf, u32 size);

int rk_vendor_read(u32 id, void *pbuf, u32 size)
{
    if (_vendor_read)
        return _vendor_read(id, pbuf, size);
    return -1;
}

int rk_vendor_write(u32 id, void *pbuf, u32 size)
{
    if (_vendor_write)
        return _vendor_write(id, pbuf, size);
    return -1;
}

int rk_vendor_register(void *read, void *write)
{
    _vendor_read = read;
    _vendor_write =  write;

    return 0;
}

rt_bool_t is_rk_vendor_ready(void)
{
    if (_vendor_read)
        return RT_TRUE;
    return RT_FALSE;
}

static int gen_random_number(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}

void vendor_stress_thread(void *param)
{
    u8 magic = 1;
    u8 read_buf[64];
    u8 write_buf[64];
    int ret;
    int len, id;

    while (1)
    {
        memset(read_buf, 0, 64);
        memset(write_buf, 0, 64);
        memset(write_buf, magic++, 64);

        len = gen_random_number(6, 32);
        id = gen_random_number(1, 16);

        ret = rk_vendor_write(id, write_buf, len);
        if (ret < 0)
        {
            rt_kprintf("rk_vendor_write failed\n");
            return;
        }

        ret = rk_vendor_read(id, read_buf, len);
        if (ret <= 0)
        {
            rt_kprintf("rk_vendor_read failed\n");
            return;
        }

        if (memcmp(read_buf, write_buf, len))
        {
            rt_kprintf("write read not same\n");
            return;
        }

        rt_kprintf("write/read ok: id=%d buf=%02x:%02x:%02x:%02x len=%d\n", id, write_buf[0], write_buf[1], write_buf[2], write_buf[3], len);
        rt_thread_delay(RT_TICK_PER_SECOND / 2);
    }
}

static void vendor_stress_test(void)
{
    rt_thread_t thread;

    thread = rt_thread_create("vendor_stress",
                              vendor_stress_thread, NULL,
                              2048,
                              21, 20);
    if (thread != RT_NULL)
        rt_thread_startup(thread);
}

void vendor_test(int argc, char **argv)
{
    int id;
    int rw = 0;
    int ret;

    if (argc < 2)
    {
        rt_kprintf("vendor_test <r/w/stress> <id> [sn]\n");
        return;
    }

    if (!strcmp(argv[1], "r"))
    {
        rw = 0;
    }
    else if (!strcmp(argv[1], "w"))
    {
        rw = 1;
    }
    else if (!strcmp(argv[1], "stress"))
    {
        vendor_stress_test();
        return;
    }
    else
    {
        rt_kprintf("vendor_test <r/w/stress> <id> [sn]\n");
        return;
    }

    if (rw)
    {
        if (argc != 4)
        {
            rt_kprintf("vendor_test <r/w/stress> <id> sn\n");
            return;
        }

        id = atoi(argv[2]);

        rt_kprintf("rk_vendor_write: id=%d %s len=%d\n", id, argv[3], strlen(argv[3]));
        ret = rk_vendor_write(id, argv[3], strlen(argv[3]));
        if (ret < 0)
        {
            rt_kprintf("rk_vendor_write failed\n");
            return;
        }
    }
    else
    {
        u8 buf[64];

        if (argc != 3)
        {
            rt_kprintf("vendor_test <r/w/stress> <id>\n");
            return;
        }

        id = atoi(argv[2]);

        memset(buf, 0, 64);

        ret = rk_vendor_read(id, buf, 64);
        if (ret <= 0)
        {
            rt_kprintf("rk_vendor_read failed\n");
            return;
        }
        rt_kprintf("rk_vendor_read: id=%d %02x:%02x:%02x:%02x:%02x:%02x len=%d\n", id, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], ret);

    }

}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(vendor_test, vendor storage test: < r / w > <id> [sn]);
#endif

