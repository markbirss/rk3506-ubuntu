/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    spi_test.c
  * @author  David Wu
  * @version V0.1
  * @date    10-Mar-2019
  * @brief   spi test for pisces
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_COMMON_TEST_SPI

#include <stdbool.h>
#include <drivers/spi.h>
#include "hal_base.h"
#include "drv_spi.h"

struct spi_test_data
{
    struct rt_spi_device *spi_device;
    uint32_t mode;
    uint32_t spi_mode;
    uint32_t bit_first;
    uint32_t max_speed_hz;
    uint32_t data_width;
};

static struct spi_test_data g_spi_test_data;

int32_t spi_dbg_hex(char *s, void *buf, uint32_t width, uint32_t len)
{
    uint32_t i, j;
    unsigned char *p8 = (unsigned char *)buf;
    unsigned short *p16 = (unsigned short *)buf;
    uint32_t *p32 = (uint32_t *)buf, next = width == 1 ? 16 : 4;

    j = 0;
    for (i = 0; i < len; i++)
    {
        if (j == 0)
        {
            rt_kprintf("[SPI TEST] %s 0x%p+0x%lx: ", s, buf, i * width);
        }

        if (width == 4)
            rt_kprintf("0x%08lx,", p32[i]);
        else if (width == 2)
            rt_kprintf("0x%04x,", p16[i]);
        else
            rt_kprintf("0x%02x,", p8[i]);

        if (++j >= next)
        {
            j = 0;
            rt_kprintf("\n");
        }
    }
    rt_kprintf("\n");

    return HAL_OK;
}

static int spi_test_write(struct spi_test_data *data, const void *txbuf, size_t n)
{
    struct rt_spi_device *spi_device = data->spi_device;
    int ret = -RT_ERROR;

    /* send data */
    ret = rt_spi_transfer(spi_device, txbuf, RT_NULL, n);
    if (ret != n)
    {
        rt_kprintf("%s ret=%d != len=%d\n", __func__, ret, n);
    }

    return ret;
}

static int spi_test_read(struct spi_test_data *data, void *rxbuf, size_t n)
{
    struct rt_spi_device *spi_device = data->spi_device;
    int ret = -RT_ERROR;

    /* send data */
    ret = rt_spi_transfer(spi_device, RT_NULL, rxbuf, n);
    if (ret != n)
    {
        rt_kprintf("%s ret=%d != len=%d\n", __func__, ret, n);
    }

    return ret;
}

static int spi_test_write_and_read(struct spi_test_data *data, const void *tx_buf, void *rx_buf, size_t len)
{
    struct rt_spi_device *spi_device = data->spi_device;
    int ret = -RT_ERROR;

    ret = rt_spi_transfer(spi_device, tx_buf, rx_buf, len);
    if (ret != len)
    {
        rt_kprintf("%s ret=%d != len=%d\n", __func__, ret, len);
    }

    return ret;
}

static int __unused spi_test_write_then_read(struct spi_test_data *data, const void *txbuf, size_t tx_n,
        void *rxbuf, size_t rx_n)
{
    struct rt_spi_device *spi_device = data->spi_device;
    int ret = -RT_ERROR;

    ret = rt_spi_send_then_recv(spi_device, txbuf, tx_n, rxbuf, rx_n);

    return (ret == rx_n) ? RT_EOK : -RT_EIO;
}

static void spi_test_show_usage()
{
    rt_kprintf("1. spi_test config <spi_device> <is_slave> <spi_mode> <is_msb> <speed> <data_width>\n");
    rt_kprintf("2. spi_test read   <spi_device> <loops> <size>\n");
    rt_kprintf("3. spi_test write  <spi_device> <loops> <size>\n");
    rt_kprintf("4. spi_test duplex <spi_device> <loops> <size>\n");
    rt_kprintf("5. spi_test cs     <spi_device> <cs_state:1-take, 0-release>\n");
    rt_kprintf("6. spi_test bus    <spi_device> <bus_state:1-take, 0-release>\n");
    rt_kprintf("7. spi_test rate   <spi_device>\n");
    rt_kprintf("8. spi_test duplex_thread   <spi_device> <loops> <size> <gap_ms> <random_pattern>\n");
    rt_kprintf("like:\n");
    rt_kprintf("\tspi_test config spi1_0 0 3 0 24000000 8\n");
    rt_kprintf("\tspi_test read   spi1_0 1 256\n");
    rt_kprintf("\tspi_test write  spi1_0 1 256\n");
    rt_kprintf("\tspi_test duplex   spi1_0 1 256\n");
    rt_kprintf("\tspi_test cs     spi1_0 1\n");
    rt_kprintf("\tspi_test bus    spi1_0 1\n");
    rt_kprintf("\tspi_test rate   spi1_0\n");
    rt_kprintf("\tspi_test duplex_thread   spi1_0 1 256 10 1\n");
}

static void spi_test_write_loop(uint32_t loops, void *txbuf, uint32_t size)
{
    uint32_t i, bytes, start_time, end_time, cost_time;
    int ret = 0;

    start_time = rt_tick_get();
    for (i = 0; i < loops; i++)
    {
        ret = spi_test_write(&g_spi_test_data, txbuf, size);
        if (ret != size)
        {
            rt_kprintf("%s failed, ret=%d\n", __func__, ret);
            break;
        }
    }
    end_time = rt_tick_get();
    cost_time = (end_time - start_time) * 1000 / RT_TICK_PER_SECOND;

    bytes = size * loops * 1;
    bytes = bytes / cost_time;
    if (ret > 0)
        rt_kprintf("spi %s write %d*%d cost %ldms speed:%ldKB/S\n", HAL_IS_CACHELINE_ALIGNED(size) ? "DMA" : "CPU", size, loops, cost_time, bytes);
}

static void spi_test_read_loop(uint32_t loops, void *rxbuf, uint32_t size)
{
    uint32_t i, bytes, start_time, end_time, cost_time;
    int ret = 0;

    start_time = rt_tick_get();
    for (i = 0; i < loops; i++)
    {
        rt_memset(rxbuf, 0, size);
        ret = spi_test_read(&g_spi_test_data, rxbuf, size);
        if (ret != size)
        {
            rt_kprintf("%s failed, ret=%d\n", __func__, ret);
            break;
        }
        spi_dbg_hex("read rx", rxbuf, 1, size);
    }
    end_time = rt_tick_get();
    cost_time = (end_time - start_time) * 1000 / RT_TICK_PER_SECOND;

    bytes = size * loops * 1;
    bytes = bytes / cost_time;
    if (ret > 0)
        rt_kprintf("spi %s read %d*%d cost %ldms speed:%ldKB/S\n", HAL_IS_CACHELINE_ALIGNED(size) ? "DMA" : "CPU", size, loops, cost_time, bytes);
    // spi_dbg_hex("read rx", rxbuf, 1, size);
}

static void spi_test_duplex_thread(void *p)
{
    char **argv = (char **)p;
    uint32_t loops = 0, size = 0, i, delay_ms;
    char *txbuf = NULL, *rxbuf = NULL;
    struct spi_test_data spi_test_data;
    bool random;
    int ret = 0;

    memcpy(&spi_test_data, &g_spi_test_data, sizeof(spi_test_data));

    loops = atoi(argv[3]);
    size = atoi(argv[4]);
    delay_ms = atoi(argv[5]);
    random = atoi(argv[6]);

    txbuf = (char *)rt_malloc_align(size, CACHE_LINE_SIZE);
    if (!txbuf)
    {
        rt_kprintf("spi write alloc buf size %d fail\n", size);
        return;
    }

    rxbuf = (char *)rt_malloc_align(size, CACHE_LINE_SIZE);
    if (!rxbuf)
    {
        rt_kprintf("spi read alloc buf size %d fail\n", size);
        rt_free_align(txbuf);
        return;
    }

    srand(rt_tick_get());
    for (i = 0; i < size; i++)
    {
        if (random)
        {
            txbuf[i] = rand() & 0xFF;
        }
        else
        {
            txbuf[i] = i & 0xFF;
        }
    }

    for (i = 0; i < loops; i++)
    {
        rt_memset(rxbuf, 0, size);
        ret = spi_test_write_and_read(&spi_test_data, txbuf, rxbuf, size);
        if (ret != size)
        {
            rt_kprintf("%s failed, ret=%d\n", __func__, ret);
            break;
        }
        if (!random && rt_memcmp(txbuf, rxbuf, size))
        {
            spi_dbg_hex("duplex_thread rx", rxbuf, 4, size > 0x20 ? 0x20 : size / 4);
        }
        if (delay_ms)
            rt_thread_mdelay(delay_ms);
    }
    if (ret > 0)
        rt_kprintf("%s %s finished\n", argv[2], __func__);

    rt_free_align(txbuf);
    rt_free_align(rxbuf);
}

void spi_test(int argc, char **argv)
{
    struct spi_test_data *data = &g_spi_test_data;
    struct rt_spi_device *spi_device = RT_NULL;
    char *cmd, *txbuf = NULL, *rxbuf = NULL;
    uint32_t bytes, start_time, end_time, cost_time;
    uint32_t loops = 0, size = 0, temp;
    int i, ret = 0;

    if (argc < 3)
        goto out;

    spi_device = (struct rt_spi_device *)rt_device_find(argv[2]);
    if (spi_device ==  RT_NULL)
    {
        rt_kprintf("Did not find device: %s....\n", argv[2]);
        return;
    }
    data->spi_device = spi_device;

    cmd = argv[1];
    if (!rt_strcmp(cmd, "config"))
    {
        struct rt_spi_configuration cfg;
        int mode, is_msb;

        if (argc < 8)
            goto out;

        mode = atoi(argv[3]);
        if (mode == 0)
            data->mode = RT_SPI_MASTER;
        else
            data->mode = RT_SPI_SLAVE;

        data->spi_mode = atoi(argv[4]);
        is_msb = atoi(argv[5]);
        if (is_msb)
            data->bit_first = RT_SPI_MSB;
        else
            data->bit_first = RT_SPI_LSB;
        data->max_speed_hz = atoi(argv[6]);
        data->data_width = atoi(argv[7]);
        if (data->data_width != 8 || data->data_width != 16)
        {
            data->data_width = data->data_width;
        }

        cfg.data_width = data->data_width;
        cfg.mode = data->mode | data->spi_mode | data->bit_first;
        cfg.max_hz = data->max_speed_hz;
        spi_device->config.reserved = RK_SPI_RESERVED_RSD_0;
        rt_spi_configure(spi_device, &cfg);

        rt_kprintf("spi %s, mode%d, %s, %dHz speed, data_width=%d\n",
                   mode ? "slave" : "master", data->spi_mode,
                   is_msb ? "MSB" : "LSB", data->max_speed_hz,
                   data->data_width);
    }
    else if (!rt_strcmp(cmd, "write"))
    {
        loops = atoi(argv[3]);
        size = atoi(argv[4]);

        txbuf = (char *)rt_malloc_align(size, CACHE_LINE_SIZE);
        if (!txbuf)
        {
            rt_kprintf("spi write alloc buf size %d fail\n", size);
            return;
        }

        for (i = 0; i < size; i++)
            txbuf[i] = i % 256;

        spi_test_write_loop(loops, txbuf, size);

        rt_free_align(txbuf);
    }
    else if (!rt_strcmp(cmd, "read"))
    {
        loops = atoi(argv[3]);
        size = atoi(argv[4]);

        rxbuf = (char *)rt_malloc_align(size, CACHE_LINE_SIZE);
        if (!rxbuf)
        {
            rt_kprintf("spi read alloc buf size %d fail\n", size);
            return;
        }
        spi_test_read_loop(loops, rxbuf, size);

        rt_free_align(rxbuf);
    }
    else if (!rt_strcmp(cmd, "duplex"))
    {
        loops = atoi(argv[3]);
        size = atoi(argv[4]);

        txbuf = (char *)rt_malloc_align(size, CACHE_LINE_SIZE);
        if (!txbuf)
        {
            rt_kprintf("spi write alloc buf size %d fail\n", size);
            return;
        }

        rxbuf = (char *)rt_malloc_align(size, CACHE_LINE_SIZE);
        if (!rxbuf)
        {
            rt_kprintf("spi read alloc buf size %d fail\n", size);
            rt_free_align(txbuf);
            return;
        }

        for (i = 0; i < size; i++)
            txbuf[i] = i % 256;

        start_time = rt_tick_get();
        for (i = 0; i < loops; i++)
        {
            ret = spi_test_write_and_read(&g_spi_test_data, txbuf, rxbuf, size);
            if (ret != size)
            {
                rt_kprintf("spi duplex failed, ret=%d\n", ret);
                break;
            }
        }
        end_time = rt_tick_get();
        cost_time = (end_time - start_time) * 1000 / RT_TICK_PER_SECOND;

        if (ret >= 0 && rt_memcmp(txbuf, rxbuf, size))
        {
            spi_dbg_hex("loop rx", rxbuf, 4, size > 0x20 ? 0x20 : size / 4);
            rt_kprintf("spi loop test fail\n");
        }

        bytes = size * loops;
        bytes = bytes / cost_time;
        if (ret >= 0)
            rt_kprintf("spi loop %d*%d cost %ldms speed:%ldKB/S\n", size, loops, cost_time, bytes);

        rt_free_align(txbuf);
        rt_free_align(rxbuf);
    }
    else if (!rt_strcmp(cmd, "cs"))
    {
        temp = atoi(argv[3]);
        if (temp == 1)
        {
            rt_spi_take(spi_device);
        }
        else
        {
            rt_spi_release(spi_device);
        }
    }
    else if (!rt_strcmp(cmd, "bus"))
    {
        struct rt_spi_configuration cfg;

        temp = atoi(argv[3]);

        cfg.data_width = data->data_width;
        cfg.mode = data->mode | data->spi_mode | data->bit_first;
        cfg.max_hz = data->max_speed_hz;
        rt_spi_configure(spi_device, &cfg);

        if (temp == 1)
        {
            rt_spi_take_bus(spi_device);
        }
        else
        {
            rt_spi_release_bus(spi_device);
        }
    }
    else if (!rt_strcmp(cmd, "rate"))
    {
        txbuf = (char *)rt_malloc_align(4096, CACHE_LINE_SIZE);
        if (!txbuf)
        {
            rt_kprintf("spi write alloc buf size %d fail\n", size);
            return;
        }

        loops = 1000;

        size = 1;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 32;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 64;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 65;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 128;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 129;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 256;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 257;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 512;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 513;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 1024;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 1025;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 2048;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 2049;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 4096;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        size = 4097;
        spi_test_read_loop(loops, txbuf, size);
        spi_test_write_loop(loops, txbuf, size);

        rt_free_align(txbuf);
    }
    else if (!rt_strcmp(cmd, "duplex_thread"))
    {
        if (argc < 7)
            goto out;

        rt_thread_t loop_thread = rt_thread_create("spi_duplex", spi_test_duplex_thread, argv, 1024, 5, 10);

        if (loop_thread == RT_NULL)
        {
            rt_kprintf("thread create failed\n");
            return;
        }

        rt_thread_startup(loop_thread);
        rt_thread_mdelay(100);
    }
    else
    {
        goto out;
    }

    return;
out:
    spi_test_show_usage();
    return;
}

int _at_spi_test(void)
{
    int ret;
    char *buffer;
    size_t size;
    struct spi_test_data *data = &g_spi_test_data;
    struct rt_spi_device *spi_device = RT_NULL;

    spi_device = (struct rt_spi_device *)rt_device_find("spi1_0");
    if (spi_device ==  RT_NULL)
    {
        rt_kprintf("Did not find device: spi1_0....\n");
        return RT_ERROR;
    }
    data->spi_device = spi_device;
    data->mode = RT_SPI_MASTER;
    data->max_speed_hz = 50000000;

    size = 2048;
    buffer = (char *)rt_malloc_align(size, CACHE_LINE_SIZE);
    if (!buffer)
    {
        rt_kprintf("spi buffer alloc buf size %d fail\n", size);
        return RT_ERROR;
    }

    for (size = 1024; size < 1036; size++)
    {
        ret = spi_test_write(&g_spi_test_data, buffer, size);
        if (ret != size)
        {
            rt_kprintf("spi_test_write %d fail %d\n", size, ret);
            goto out;
        }
        ret = spi_test_read(&g_spi_test_data, buffer, size);
        if (ret != size)
        {
            rt_kprintf("spi_test_read %d fail %d\n", size, ret);
            goto out;
        }
    }
    rt_kprintf("%s test succress\n", __func__);
out:
    rt_free_align(buffer);
    return ret;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(spi_test, spt test cmd);
FINSH_FUNCTION_EXPORT(_at_spi_test, spi test for auto test);
#endif

#endif
