/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    .c
  * @author  Jon Lin
  * @version V1.0
  * @date    2024-05-25
  * @brief   spi2apb flash object master drivesr
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rtthread.h>

#ifdef RT_USING_COMMON_TEST_SPI2APB_FLASH_OBJ

#include <stdbool.h>
#include <drivers/spi.h>
#include "hal_base.h"
#include "spi2apb_flash_obj.h"

// #define MST_DEBUG

#ifdef MST_DEBUG
#define mst_dbg(...)     rt_kprintf(__VA_ARGS__)
#else
#define mst_dbg(...)
#endif

#define     SPI_ST_WRITE_ERR                    (1<<0)
#define     SPI_ST_WRITE_OVERFLOW               (1<<1)
#define     SPI_ST_UNFINISHED                   (1<<2)
#define     SPI_ST_TRANS_CONFLICT               (1<<3)

#define     SPI_ST_READ_ERR                     (1<<8)
#define     SPI_ST_READ_UNDERFLOW               (1<<9)
#define     SPI_ST_PRE_READ_ERR                 (1<<10)

extern uint32_t __spi2apb_buffer_start__[];
#define SRAM_SPI2APB_BASE (uint32_t)__spi2apb_buffer_start__

struct spi2apb_flash_dev
{
    struct rt_spi_device *spi_device;
};

static rt_uint8_t read_cmd_phase = 0x55;
static rt_uint8_t write_cmd_phase = 0xa5;
static struct spi2apb_flash_dev g_spi2apb_flash_dev;

static void spi2apb_flash_obj_mst_show_usage()
{
    rt_kprintf("1. spi2apb_flash_obj_mst spi2apb_progsram <spi_dev> <size>\n");
    rt_kprintf("2. spi2apb_flash_obj_mst spi2apb_readsram <spi_dev> <size>\n");
    rt_kprintf("3. spi2apb_flash_obj_mst spi2apb_writemsg <spi_dev> <reg> <val>\n");
    rt_kprintf("4. spi2apb_flash_obj_mst spi2apb_querymsg <spi_dev>\n");
    rt_kprintf("4. spi2apb_flash_obj_mst spi2apb_querystatus <spi_dev>\n");
    rt_kprintf("5. spi2apb_flash_obj_mst write_sector <spi_dev> <sector> <val> <loops>\n");
    rt_kprintf("6. spi2apb_flash_obj_mst read_sector <spi_dev> <sector> <loops>\n");
    rt_kprintf("7. spi2apb_flash_obj_mst stress <spi_dev> <sector> <n_sec>\n");
    rt_kprintf("like:\n");
    rt_kprintf("\tspi2apb_flash_obj_mst spi2apb_progsram spi1_0 32\n");
    rt_kprintf("\tspi2apb_flash_obj_mst spi2apb_readsram spi1_0 32\n");
    rt_kprintf("\tspi2apb_flash_obj_mst spi2apb_writemsg spi1_0 1 55\n");
    rt_kprintf("\tspi2apb_flash_obj_mst spi2apb_querymsg spi1_0\n");
    rt_kprintf("\tspi2apb_flash_obj_mst spi2apb_querystatus spi1_0\n");
    rt_kprintf("\tspi2apb_flash_obj_mst write_sector spi1_0 512 1 10\n");
    rt_kprintf("\tspi2apb_flash_obj_mst read_sector spi1_0 512 10\n");
    rt_kprintf("\tspi2apb_flash_obj_mst stress spi1_0 512 32\n");
}

static int32_t flash_obj_dbg_hex(char *s, void *buf, uint32_t width, uint32_t len)
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

static int spi2apb_flash_obj_mst_spi_write(struct spi2apb_flash_dev *data, const void *txbuf, size_t n)
{
    struct rt_spi_device *spi_device = data->spi_device;
    int ret = -RT_ERROR;

    /* send data */
    ret = rt_spi_transfer(spi_device, txbuf, RT_NULL, n);
    if (ret != n)
    {
        rt_kprintf("%s ret_length=0x%x != len=0x%x\n", __func__, ret, n);
    }

    return (ret == n) ? RT_EOK : ret;
}

static int spi2apb_flash_obj_mst_spi_write_then_read(struct spi2apb_flash_dev *data, const void *txbuf, size_t tx_n,
        void *rxbuf, size_t rx_n)
{
    struct rt_spi_device *spi_device = data->spi_device;
    int ret = -RT_ERROR;

    ret = rt_spi_send_then_recv(spi_device, txbuf, tx_n, rxbuf, rx_n);

    return ret;
}

static int spi2apb_flash_obj_mst_spi_write_then_write(struct spi2apb_flash_dev *data, const void *txbuf, size_t tx_n,
        void *txbuf2, size_t tx_n2)
{
    struct rt_spi_device *spi_device = data->spi_device;
    int ret = -RT_ERROR;

    ret = rt_spi_send_then_send(spi_device, txbuf, tx_n, txbuf2, tx_n2);

    return ret;
}

/**
  * @brief  Query state abort last transmission.
  * @return uint32_t status.
  */
static uint32_t spi2apb_flash_obj_mst_spi2apb_querystatus(void)
{
    uint8_t cmd[4];
    uint32_t status = 0;

    cmd[0] = 0xff;
    cmd[1] = 0x00;
    cmd[2] = 0x00;
    cmd[3] = 0x00;

    spi2apb_flash_obj_mst_spi_write_then_read(&g_spi2apb_flash_dev, cmd, 4, (uint8_t *)&status, 4);

    return status;
}

/**
  * @brief  Query msg reg2 in spi2apb.
  * @return rt_err_t
  */
static uint32_t spi2apb_flash_obj_mst_spi2apb_querymsg(void)
{
    uint8_t cmd[4];
    uint32_t status = 0;

    cmd[0] = 0xff;
    cmd[1] = 0x01;
    cmd[2] = 0x00;
    cmd[3] = 0x00;

    spi2apb_flash_obj_mst_spi_write_then_read(&g_spi2apb_flash_dev, cmd, 4, (uint8_t *)&status, 4);

    return status;
}

/**
  * @brief  Write msg reg in spi2apb.
  * @param  index: reg index.
  * @param  val: reg value.
  * @param  status: if valid check write success.
  * @return rt_err_t
  */
static rt_err_t spi2apb_flash_obj_mst_spi2apb_writemsg(uint8_t index, uint32_t value, bool status)
{
    uint8_t cmd[8];
    uint32_t val;

    mst_dbg("spi2apb_writemsg reg%d val=0x%x\n", index, value);

    if (index > 2)
        return -RT_EINVAL;

    cmd[0] = 0x11;
    cmd[1] = 0x00;
    cmd[2] = index + 1;
    cmd[3] = 0x00;
    cmd[4] = (value >> 0) & 0xff;
    cmd[5] = (value >> 8) & 0xff;
    cmd[6] = (value >> 16) & 0xff;
    cmd[7] = (value >> 24) & 0xff;

    spi2apb_flash_obj_mst_spi_write(&g_spi2apb_flash_dev, cmd, 8);
    if (!status)
        return 0;

    val = spi2apb_flash_obj_mst_spi2apb_querystatus();
    if (val & (SPI_ST_WRITE_ERR | SPI_ST_WRITE_OVERFLOW | SPI_ST_UNFINISHED | SPI_ST_TRANS_CONFLICT))
    {
        rt_kprintf("writemsg err: 0x%x\n", val);
        return -RT_ERROR;
    }

    return 0;
}

/**
  * @brief  Write data to remote sram by spi2apb interface.
  * @param  addr: pointer to remote sram address.
  * @param  buffer: data buffer.
  * @param  size: data size.
  * @param  trigger: if valid write spi2apb reg1 to trigger a interrupt.
  * @return rt_err_t
  */
static rt_err_t spi2apb_flash_obj_mst_spi2apb_progsram(uint32_t addr, uint8_t *buffer, uint32_t size, uint32_t trigger)
{
    uint8_t cmd[16];
    uint32_t val;

    if (addr & 0x3 || size & 0x3)
    {
        rt_kprintf("%s failed, addr=%x size=%x should 4Bytes aligned!\n", __func__, addr, size);
        return -RT_EINVAL;
    }

    memset(cmd, 0xa5, 16);

    cmd[0] = 0x11;
    cmd[1] = 0x00;
    cmd[2] = 0x00;
    cmd[3] = 0x00;
    cmd[4] = (addr & 0xFF);
    cmd[5] = (addr >> 8) & 0xFF;
    cmd[6] = (addr >> 16) & 0xFF;
    cmd[7] = (addr >> 24) & 0xFF;

    spi2apb_flash_obj_mst_spi_write_then_write(&g_spi2apb_flash_dev, cmd, 8, buffer, size);
    HAL_DelayUs(10);

    val = spi2apb_flash_obj_mst_spi2apb_querystatus();
    if (val & (SPI_ST_WRITE_ERR | SPI_ST_WRITE_OVERFLOW | SPI_ST_UNFINISHED | SPI_ST_TRANS_CONFLICT))
    {
        rt_kprintf("progsram addr=0x%x err=0x%x\n", addr, val);
        return -RT_ERROR;
    }

    if (trigger)
    {
        return spi2apb_flash_obj_mst_spi2apb_writemsg(SPI2APB_REG1_TRIGGER_INDEX, trigger, 1);
    }

    return 0;
}

/**
  * @brief  Read data from remote sram by spi2apb interface.
  * @param  addr: pointer to remote sram address.
  * @param  buffer: data buffer.
  * @param  size: data size.
  * @param  trigger: if valid write spi2apb reg1 to trigger a interrupt.
  * @return rt_err_t
  */
static rt_err_t spi2apb_flash_obj_mst_spi2apb_readsram(uint32_t addr, uint8_t *buffer, uint32_t size, uint32_t trigger)
{
    uint8_t cmd[16];
    uint32_t val, *cmd32;

    if (addr & 0x3 || size & 0x3)
    {
        rt_kprintf("%s failed, addr=%x size=%x should 4Bytes aligned!\n", __func__, addr, size);
        return -RT_EINVAL;
    }

    cmd32 = (uint32_t *)&cmd;

    *cmd32 = (size << 14 & 0xffff0000);
    cmd[0] = 0x77;

    cmd[4] = (addr & 0xFF);
    cmd[5] = (addr >> 8) & 0xFF;
    cmd[6] = (addr >> 16) & 0xFF;
    cmd[7] = (addr >> 24) & 0xFF;

    /*dummy*/
    cmd[8] = 0x00;
    cmd[9] = 0x00;
    cmd[10] = 0x00;
    cmd[11] = 0x00;

    /*read begin*/
    cmd[12] = 0xAA;
    cmd[13] = 0x00;
    cmd[14] = 0x00;
    cmd[15] = 0x00;

    spi2apb_flash_obj_mst_spi_write_then_read(&g_spi2apb_flash_dev, cmd, 16, buffer, size);
    HAL_DelayUs(10);

    val = spi2apb_flash_obj_mst_spi2apb_querystatus();
    if (val & (SPI_ST_READ_ERR | SPI_ST_READ_UNDERFLOW | SPI_ST_PRE_READ_ERR))
    {
        rt_kprintf("readsram addr=0x%x size=%x err=0x%x\n", addr, size, val);
        return -RT_ERROR;
    }

    if (trigger)
    {
        return spi2apb_flash_obj_mst_spi2apb_writemsg(SPI2APB_REG1_TRIGGER_INDEX, trigger, 1);
    }

    return 0;
}

static rt_err_t spi2apb_flash_obj_mst_wait_status(rt_uint8_t index, rt_uint8_t expect, rt_uint32_t timeout_ms, rt_uint32_t gap_ms)
{
    spi2apb_flash_obj_status status;
    uint32_t timeout;
    rt_err_t ret = -RT_ETIMEOUT;

    timeout = rt_tick_get() + timeout_ms;
    do
    {
        status.d32 = spi2apb_flash_obj_mst_spi2apb_querymsg();
        mst_dbg("%s wait ready, status=%d mp=%x-sp=%x\n", __func__, status.item.status, index, status.item.index);

        if (gap_ms)
            rt_thread_mdelay(gap_ms);
        if ((status.item.status == expect) && (status.item.index == index))
        {
            ret = 0;
            break;
        }
    }
    while (timeout > rt_tick_get());

    if (ret)
    {
        rt_kprintf("%s wait tiemout, st=%x-%x ph=%x-%x\n", __func__, status.item.status, expect, status.item.index, index);
        rt_kprintf("Setting SPI2APB reg2 to ready at first:\n");
        rt_kprintf("  spi2apb_test write spi2apb 0x500\n");
    }

    return ret;
}

static rt_err_t spi2apb_flash_obj_mst_recovery(void)
{
    spi2apb_flash_obj_packet packet;

    rt_kprintf("%s\n", __func__);

    /* cmd RESET */
    packet.item.index = 0;
    packet.item.cmd = SPI2APB_FLASH_OBJ_CMD_RESET;
    packet.item.addr = 0;
    return spi2apb_flash_obj_mst_spi2apb_writemsg(SPI2APB_REG1_TRIGGER_INDEX, packet.d32, 0);
}

/**
  * @brief  Read sector from remote spinor device.
  * @param  sec: pointer to spinor 4KB sector offset.
  * @param  buffer: data buffer.
  * @return rt_err_t
  */
rt_err_t spi2apb_flash_obj_mst_read_sector(rt_uint32_t sec, void *buffer)
{
    spi2apb_flash_obj_packet packet;
    rt_err_t ret;

    /* cmd READ */
    read_cmd_phase = ~read_cmd_phase;
    packet.item.index = read_cmd_phase;
    packet.item.cmd = SPI2APB_FLASH_OBJ_CMD_READ;
    packet.item.addr = sec;
    spi2apb_flash_obj_mst_spi2apb_writemsg(SPI2APB_REG1_TRIGGER_INDEX, packet.d32, 1);

    /* wait READ_DATA_READY */
    ret = spi2apb_flash_obj_mst_wait_status(packet.item.index, SPI2APB_FLASH_OBJ_SLAVE_READ_DATA_READY, 1000, 1);
    if (ret)
    {
        spi2apb_flash_obj_mst_recovery();
        return ret;
    }

    return spi2apb_flash_obj_mst_spi2apb_readsram(SRAM_SPI2APB_BASE, buffer, SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE, 0);
}

/**
  * @brief  Write sector from remote spinor device.
  * @param  sec: pointer to spinor 4KB sector offset.
  * @param  buffer: data buffer.
  * @return rt_err_t
  */
rt_err_t spi2apb_flash_obj_mst_write_sector(rt_uint32_t sec, void *buffer)
{
    spi2apb_flash_obj_packet packet;
    rt_err_t ret;

    ret = spi2apb_flash_obj_mst_spi2apb_progsram(SRAM_SPI2APB_BASE, buffer, SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE, 0);
    if (ret)
    {
        return -1;
    }

    /* cmd PROG */
    write_cmd_phase = ~write_cmd_phase;
    packet.item.index = write_cmd_phase;
    packet.item.cmd = SPI2APB_FLASH_OBJ_CMD_PROG;
    packet.item.addr = sec;
    spi2apb_flash_obj_mst_spi2apb_writemsg(SPI2APB_REG1_TRIGGER_INDEX, packet.d32, 0);

    /* wait SUCCESS */
    ret = spi2apb_flash_obj_mst_wait_status(packet.item.index, SPI2APB_FLASH_OBJ_SLAVE_SUCCESS, 1000, 1);
    if (ret)
    {
        spi2apb_flash_obj_mst_recovery();
        return ret;
    }

    return 0;
}

rt_err_t spi2apb_flash_obj_mst_init(struct rt_spi_device *spi_device)
{
    struct rt_spi_configuration cfg;
    rt_err_t ret;

    cfg.data_width = 8;
    cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_LSB;
    cfg.max_hz = 25000000;
    rt_spi_configure(spi_device, &cfg);

    rt_kprintf("spi master, mode0, lsb, %dHz speed, data_width=8\n", cfg.max_hz);

    ret = spi2apb_flash_obj_mst_wait_status(0, SPI2APB_FLASH_OBJ_SLAVE_IDLE, 5000, 10);
    if (ret)
    {
        rt_kprintf("%s failed, wait flash_obj device idle failed\n", ret);
        return ret;
    }

    return 0;
}

void spi2apb_flash_obj_mst(int argc, char **argv)
{
    struct spi2apb_flash_dev *data = &g_spi2apb_flash_dev;
    char *cmd, *txbuf = NULL, *rxbuf = NULL;
    uint32_t bytes, loops, start_time, end_time, cost_time, i;

    if (argc < 3)
        goto out;

    if (data->spi_device == RT_NULL)
    {
        data->spi_device = (struct rt_spi_device *)rt_device_find(argv[2]);
        if (data->spi_device ==  RT_NULL)
        {
            rt_kprintf("Did not find device: %s....\n", argv[2]);
            return;
        }
        spi2apb_flash_obj_mst_init(data->spi_device);
    }

    cmd = argv[1];
    if (!rt_strcmp(cmd, "spi2apb_writemsg"))
    {
        uint8_t index;
        uint32_t val;
        int32_t ret;

        if (argc != 5)
            goto out;

        index = atoi(argv[3]) & 0xff;
        val = atoi(argv[4]);
        ret = spi2apb_flash_obj_mst_spi2apb_writemsg(index, val, 0);
        if (ret)
        {
            rt_kprintf("spi2apb_writemsg failed, ret=%d\n", ret);
        }
    }
    else if (!rt_strcmp(cmd, "spi2apb_querymsg"))
    {
        uint32_t val;

        val = spi2apb_flash_obj_mst_spi2apb_querymsg();
        rt_kprintf("spi2apb_querymsg val=0x%x\n", val);
    }
    else if (!rt_strcmp(cmd, "spi2apb_querystatus"))
    {
        uint32_t val;

        val = spi2apb_flash_obj_mst_spi2apb_querystatus();
        rt_kprintf("spi2apb_querystatus val=0x%x\n", val);
    }
    else if (!rt_strcmp(cmd, "spi2apb_progsram"))
    {
        uint32_t addr = SRAM_SPI2APB_BASE;
        uint32_t i, size;
        int32_t ret;

        if (argc != 4)
            goto out;

        size = atoi(argv[3]);
        txbuf = (char *)rt_malloc_align(size, CACHE_LINE_SIZE);
        if (!txbuf)
        {
            rt_kprintf("spi2apb_progsram alloc buf size %d fail\n", size);
            return;
        }

        for (i = 0; i < size; i++)
            txbuf[i] = i % 256;

        ret = spi2apb_flash_obj_mst_spi2apb_progsram(addr, (uint8_t *)txbuf, size, SPI2APB_FLASH_OBJ_CMD_PROG_SRAM_PACKET);
        if (ret)
        {
            rt_kprintf("spi2apb_progsram failed, ret=%d\n", ret);
        }
        rt_free_align(txbuf);
    }
    else if (!rt_strcmp(cmd, "spi2apb_readsram"))
    {
        uint32_t addr = SRAM_SPI2APB_BASE;
        uint32_t size;
        int32_t ret;

        if (argc != 4)
            goto out;

        size = atoi(argv[3]);
        rxbuf = (char *)rt_malloc_align(size, CACHE_LINE_SIZE);
        if (!rxbuf)
        {
            rt_kprintf("spi2apb_readsram alloc buf size %d fail\n", size);
            return;
        }

        rt_memset(rxbuf, 0, size);

        /* wait SUCCESS */
        ret = spi2apb_flash_obj_mst_wait_status(0, SPI2APB_FLASH_OBJ_SLAVE_SUCCESS, 1000, 1);
        if (ret)
        {
            rt_kprintf("spi2apb_readsram fail to wait status, ret=%d\n", ret);
            return;
        }

        ret = spi2apb_flash_obj_mst_spi2apb_readsram(addr, (uint8_t *)rxbuf, size, 0);
        if (ret)
        {
            rt_kprintf("spi2apb_readsram failed, ret=%d\n", ret);
        }
        flash_obj_dbg_hex("spi2apb_readsram rx", rxbuf, 4, size > 0x20 ? 0x20 : size / 4);

        rt_free_align(rxbuf);
    }
    else if (!rt_strcmp(cmd, "read_sector"))
    {
        uint32_t sec;
        int32_t ret;

        if (argc != 5)
            goto out;

        sec = atoi(argv[3]);
        loops = atoi(argv[4]);

        rxbuf = (char *)rt_malloc_align(SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE, CACHE_LINE_SIZE);
        if (!rxbuf)
        {
            rt_kprintf("read_sector alloc buf failed\n");
            return;
        }

        start_time = rt_tick_get();
        for (i = 0; i < loops; i++)
        {
            ret = spi2apb_flash_obj_mst_read_sector(sec, rxbuf);
            if (ret)
            {
                rt_kprintf("read_sector failed, ret=%d loops=%d\n", ret, i);
                break;
            }
        }
        end_time = rt_tick_get();
        cost_time = (end_time - start_time) * 1000 / RT_TICK_PER_SECOND;
        bytes = SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE * loops;
        rt_kprintf("read_sector 4KB cost %ldms speed:%ldKB/S\n", cost_time,  bytes / cost_time);

        flash_obj_dbg_hex("read_sec", rxbuf, 4, 32);
        rt_free_align(rxbuf);
    }
    else if (!rt_strcmp(cmd, "write_sector"))
    {
        uint32_t sec, val;
        int32_t ret;

        if (argc != 6)
            goto out;

        sec = atoi(argv[3]);
        val = atoi(argv[4]);
        loops = atoi(argv[5]);

        txbuf = (char *)rt_malloc_align(SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE, CACHE_LINE_SIZE);
        if (!txbuf)
        {
            rt_kprintf("write_sector alloc buf failed\n");
            return;
        }

        rt_memset(txbuf, val & 0xff, SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE);

        start_time = rt_tick_get();
        for (i = 0; i < loops; i++)
        {
            ret = spi2apb_flash_obj_mst_write_sector(sec, txbuf);
            if (ret)
            {
                rt_kprintf("write_sector failed, ret=%d loops=%d\n", ret, i);
                break;
            }
        }
        end_time = rt_tick_get();
        cost_time = (end_time - start_time) * 1000 / RT_TICK_PER_SECOND;
        bytes = SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE * loops;
        rt_kprintf("write_sector %dKB cost %ldms speed:%ldKB/S\n", bytes / 1024, cost_time,  bytes / cost_time);

        rt_free_align(txbuf);
    }
    else if (!rt_strcmp(cmd, "stress"))
    {
        uint32_t sec, n_sec;
        int32_t ret;

        if (argc != 5)
            goto out;

        sec = atoi(argv[3]);
        n_sec = atoi(argv[4]);

        txbuf = (char *)rt_malloc_align(SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE, CACHE_LINE_SIZE);
        if (!txbuf)
        {
            rt_kprintf("stress alloc buf failed\n");
            return;
        }

        rxbuf = (char *)rt_malloc_align(SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE, CACHE_LINE_SIZE);
        if (!rxbuf)
        {
            rt_kprintf("stress alloc buf failed\n");
            rt_free_align(txbuf);
            return;
        }

        for (i = 0; i < n_sec; i++)
        {
            rt_kprintf("stress test, sec=%d i=%d\n", sec + i, i);
            rt_memset(txbuf, i & 0xff, SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE);
            ret = spi2apb_flash_obj_mst_write_sector(sec + i, txbuf);
            if (ret)
            {
                rt_kprintf("write_sector failed, ret=%d loops=%d\n", ret, i);
                break;
            }

            ret = spi2apb_flash_obj_mst_read_sector(sec + i, rxbuf);
            if (ret)
            {
                rt_kprintf("read_sector failed, ret=%d loops=%d\n", ret, i);
                break;
            }

            if (rt_memcmp(txbuf, rxbuf, SPI2APB_FLASH_OBJ_PACKET_NOR_SECTOR_SIZE))
            {
                rt_kprintf("stress test failed, sec=%d =%d\n", sec + i, i);
                flash_obj_dbg_hex("w", txbuf, 4, 16);
                flash_obj_dbg_hex("r", rxbuf, 4, 16);
                break;
            }
        }
        if (i >= n_sec)
        {
            rt_kprintf("stress test success!\n");
        }

        rt_free_align(rxbuf);
        rt_free_align(txbuf);
    }
    else
    {
        goto out;
    }

    return;
out:
    spi2apb_flash_obj_mst_show_usage();
    return;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(spi2apb_flash_obj_mst, spi2apb_flash_obj master test cmd);
#endif

#endif
