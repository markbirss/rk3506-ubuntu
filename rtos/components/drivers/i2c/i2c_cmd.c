/**
  * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    i2c_cmd.c
  * @author  Jair Wu
  * @version V0.1
  * @date    3-April-2023
  * @brief   i2c cmds
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef RT_USING_I2C

static void list_i2c_bus(void)
{
    int start = 0, stop = 5;
    char name[8];

    for (int i = start; i <= stop; i++)
    {
        snprintf(name, sizeof(name), "i2c%d", i);
        if (rt_device_find(name))
            rt_kprintf("%s\n", name);
    }
}

static struct rt_i2c_bus_device *get_i2c_bus(int no)
{
    char name[8];
    snprintf(name, sizeof(name), "i2c%d", no);

    return rt_i2c_bus_device_find(name);
}

static int i2c_touch(struct rt_i2c_bus_device *i2c_bus, rt_uint16_t addr)
{
    struct rt_i2c_msg msgs[1];
    rt_uint8_t cmd_buf[1] = {0x0,};
    int ret;

    msgs[0].addr  = addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = cmd_buf;
    msgs[0].len   = sizeof(cmd_buf);

    ret = rt_i2c_transfer(i2c_bus, msgs, 1);
    if (ret == 1)
        return RT_EOK;
    else
        return -RT_ERROR;
}

static int i2c_read(struct rt_i2c_bus_device *i2c_bus, rt_uint16_t addr,
                    void *cmd_buf, size_t cmd_len,
                    void *data_buf, size_t data_len)
{
    struct rt_i2c_msg msgs[2];
    int ret;

    msgs[0].addr  = addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = cmd_buf;
    msgs[0].len   = cmd_len;

    msgs[1].addr  = addr;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf   = data_buf;
    msgs[1].len   = data_len;

    ret = rt_i2c_transfer(i2c_bus, msgs, 2);
    if (ret != 2)
    {
        rt_kprintf("%s: 0x%x 0x%x failed: (%d)\n", __func__, addr,
                   *(rt_uint8_t *)cmd_buf, ret);

        return -RT_ERROR;
    }

    return RT_EOK;
}

static int i2c_write(struct rt_i2c_bus_device *i2c_bus, rt_uint16_t addr,
                     void *data_buf, size_t data_len)
{
    struct rt_i2c_msg msgs[1];
    int ret;

    msgs[0].addr  = addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = data_buf;
    msgs[0].len   = data_len;

    ret = rt_i2c_transfer(i2c_bus, msgs, 1);
    if (ret != 1)
    {
        rt_kprintf("%s: 0x%x 0x%x failed: (%d)\n", __func__, addr,
                   *(rt_uint8_t *)data_buf, ret);

        return -RT_ERROR;
    }

    return RT_EOK;
}

static void scan_i2c_bus(struct rt_i2c_bus_device *i2c_bus)
{
    rt_kprintf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    for (int i = 0; i < 128; i += 16)
    {
        rt_kprintf("%02x:", i);
        for (int j = 0; j < 16; j++)
        {
            if (RT_EOK == i2c_touch(i2c_bus, i + j))
                rt_kprintf(" %02x", i + j);
            else
                rt_kprintf(" --");
        }
        rt_kprintf("\n");
    }
}

static void i2cget_usage(void)
{
    rt_kprintf("i2cget I2CBUS ADDRESS <REG0>\n");
}

static void i2cget(int argc, char *argv[])
{
    struct rt_i2c_bus_device *i2c_bus;
    rt_uint8_t cmd_buf[1];
    rt_uint8_t buf[1];

    if (argc < 4)
    {
        i2cget_usage();
        return;
    }

    i2c_bus = get_i2c_bus(atoi(argv[1]));

    if (!i2c_bus)
    {
        rt_kprintf("Cannot get i2c-bus\n");
        return;
    }

    cmd_buf[0] = strtol(argv[3], NULL, 0);
    i2c_read(i2c_bus, strtol(argv[2], NULL, 0),
             cmd_buf, 1, buf, 1);
    rt_kprintf("%02x:%02x\n", cmd_buf[0], buf[0]);
}

static void i2cset_usage(void)
{
    rt_kprintf("i2cset I2CBUS ADDRESS <REG0> <DATA0> ...\n");
}

static void i2cset(int argc, char *argv[])
{
    struct rt_i2c_bus_device *i2c_bus;
    rt_uint8_t cmd_buf[1];
    rt_uint8_t buf[256];

    if (argc < 5)
    {
        i2cset_usage();
        return;
    }

    i2c_bus = get_i2c_bus(atoi(argv[1]));

    if (!i2c_bus)
    {
        rt_kprintf("Cannot get i2c-bus\n");
        return;
    }

    for (int i = 3; i < argc; i++)
        buf[i - 3] = strtol(argv[i], NULL, 0) & 0xff;

    i2c_write(i2c_bus, strtol(argv[2], NULL, 0),
              buf, argc - 3);
    cmd_buf[0] = buf[0];
    i2c_read(i2c_bus, strtol(argv[2], NULL, 0),
             cmd_buf, 1, buf, 1);
    rt_kprintf("%02x:%02x\n", cmd_buf[0], buf[0]);
}

static void i2cdump_usage(void)
{
    rt_kprintf("i2cdump I2CBUS ADDRESS\n");
}

static void i2cdump(int argc, char *argv[])
{
    struct rt_i2c_bus_device *i2c_bus;
    rt_uint8_t cmd_buf[2];
    rt_uint8_t buf[256];

    if (argc < 3)
    {
        i2cdump_usage();
        return;
    }

    i2c_bus = get_i2c_bus(atoi(argv[1]));

    if (!i2c_bus)
    {
        rt_kprintf("Cannot get i2c-bus\n");
        return;
    }

    rt_kprintf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f"
               "  0123456789abcdef\n");
    for (int i = 0; i < sizeof(buf); i += 16)
    {
        rt_kprintf("%02x:", i);
        for (int j = 0; j < 16; j++)
        {
            cmd_buf[0] = i + j;
            if (RT_EOK == i2c_read(i2c_bus, strtol(argv[2], NULL, 0),
                                   cmd_buf, 1,
                                   &buf[i + j], 1))
            {
                rt_kprintf(" %02x", buf[i + j]);
            }
            else
            {
                buf[i + j] = 0xff;
                rt_kprintf(" er");
            }
        }
        rt_kprintf("  ");
        for (int j = 0; j < 16; j++)
        {
            if ((buf[i + j] >= 32) && (buf[i + j] <= 126))
                rt_kprintf("%c", buf[i + j]);
            else
                rt_kprintf(".");
        }
        rt_kprintf("\n");
    }
}

static void i2cdetect_usage(void)
{
    rt_kprintf("i2cdetect -l\n");
    rt_kprintf("i2cdetect I2CBUS\n");
}

static void i2cdetect(int argc, char *argv[])
{
    struct rt_i2c_bus_device *i2c_bus;
    int flags = 1, list = 0;

    if (argc == 1)
    {
        i2cdetect_usage();
        return;
    }

    while ((argc > flags) && (argv[flags][0] == '-'))
    {
        switch (argv[flags][1])
        {
        case 'l':
            list = RT_TRUE;
            break;
        default:
            rt_kprintf("Unsupported option %s\n", argv[flags]);
            i2cdetect_usage();
            return;
        }
        flags++;
    }

    if (list)
    {
        list_i2c_bus();
        return;
    }

    if (argc < flags)
    {
        rt_kprintf("No i2c-bus specified\n");
        i2cdetect_usage();
        return;
    }

    i2c_bus = get_i2c_bus(atoi(argv[flags]));

    if (!i2c_bus)
    {
        rt_kprintf("Cannot get i2c-bus\n");
        return;
    }

    scan_i2c_bus(i2c_bus);
}

MSH_CMD_EXPORT(i2cdump, i2c dump);
MSH_CMD_EXPORT(i2cdetect, i2c detect);
MSH_CMD_EXPORT(i2cset, i2c set);
MSH_CMD_EXPORT(i2cget, i2c get);
#endif

