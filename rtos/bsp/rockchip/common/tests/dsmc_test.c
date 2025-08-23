/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    dsmc_test.c
  * @version V1.0
  * @brief   dsmc test
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-10-10     Zhihuan He   the first version
  *
  ******************************************************************************
  */

#include <stdint.h>

#include <rthw.h>
#include <rtthread.h>

#ifdef RT_USING_COMMON_TEST_DSMC
#include "hal_base.h"
#include "dma.h"
#include "drv_dsmc_host.h"

#define IO_BW_32        0
#define IO_BW_16        1
#define IO_BW_8         2
#define IO_TYPE_0       0

#define DMA_SIZE        (0x10000)
#define COUNTS    100

static struct rt_device *dma;
static rt_sem_t mem_sem;

static void m2m_complete(void *param)
{
    rt_sem_release(mem_sem);
}

void dma_memcpy(int *dst_mem, int *src_mem, int size)
{
    struct rt_dma_transfer *m2m_xfer;
    rt_err_t ret;
    int i, len;
    uint32_t tick_s, tick_e; /* ms */

    m2m_xfer = (struct rt_dma_transfer *)rt_malloc(sizeof(*m2m_xfer));
    mem_sem = rt_sem_create("memSem", 0, RT_IPC_FLAG_FIFO);
    RT_ASSERT(m2m_xfer != RT_NULL);
    RT_ASSERT(src_mem != RT_NULL);
    RT_ASSERT(dst_mem != RT_NULL);
    RT_ASSERT(mem_sem != RT_NULL);

    len = size / sizeof(int);
    for (i = 0; i < len; i++)
        src_mem[i] = len - i;
    rt_memset(dst_mem, 0x0, size);
    rt_memset(src_mem, 0x6, size);
    HAL_DCACHE_CleanInvalidateByRange((uint32_t)src_mem, size);
    HAL_DCACHE_CleanInvalidateByRange((uint32_t)dst_mem, size);

    dma = rt_device_find("dmac0");

    /* memcpy test */
    rt_memset(m2m_xfer, 0x0, sizeof(*m2m_xfer));
    m2m_xfer->direction = RT_DMA_MEM_TO_MEM;
    m2m_xfer->dst_addr = (rt_uint32_t)dst_mem;
    m2m_xfer->src_addr = (rt_uint32_t)src_mem;
    m2m_xfer->len = size;
    m2m_xfer->callback = m2m_complete;
    m2m_xfer->cparam = m2m_xfer;
    ret = rt_device_control(dma, RT_DEVICE_CTRL_DMA_REQUEST_CHANNEL, m2m_xfer);
    RT_ASSERT(ret == RT_EOK);

    tick_s = HAL_GetTick();

    for (i = 0; i < COUNTS; i++)
    {
        /* dma copy start */
        ret = rt_device_control(dma, RT_DEVICE_CTRL_DMA_SINGLE_PREPARE, m2m_xfer);
        RT_ASSERT(ret == RT_EOK);
        ret = rt_device_control(dma, RT_DEVICE_CTRL_DMA_START, m2m_xfer);
        RT_ASSERT(ret == RT_EOK);

        /* wait for complete */
        ret = rt_sem_take(mem_sem, RT_WAITING_FOREVER);
        RT_ASSERT(ret == RT_EOK);

        ret = rt_device_control(dma, RT_DEVICE_CTRL_DMA_STOP, m2m_xfer);
        RT_ASSERT(ret == RT_EOK);
    }

    tick_e = HAL_GetTick();

    ret = rt_memcmp(src_mem, dst_mem, size);

    rt_kprintf("dma memcpy [%s]: avg: %7u MB/S with src: 0x%x dst: 0x%x len: %u counts: %u\n",
               ret ? "FAIL" : "PASS", size * COUNTS / (tick_e - tick_s) / 1000,
               src_mem, dst_mem, size, COUNTS);

    ret = rt_device_control(dma, RT_DEVICE_CTRL_DMA_RELEASE_CHANNEL, m2m_xfer);
    RT_ASSERT(ret == RT_EOK);

    tick_s = HAL_GetTick();
    for (i = 0; i < COUNTS; i++)
        rt_memcpy(dst_mem, src_mem, size);
    tick_e = HAL_GetTick();

    ret = rt_memcmp(dst_mem, src_mem, size);

    rt_kprintf("cpu memcpy [%s]: avg: %7u MB/S with src: 0x%x dst: 0x%x len: %u counts: %u\n",
               ret ? "FAIL" : "PASS", size * COUNTS / (tick_e - tick_s) / 1000,
               src_mem, dst_mem, size, COUNTS);

    rt_sem_delete(mem_sem);
    rt_free(m2m_xfer);
}

static int dma_test_psram(struct rockchip_rt_dsmc_host *dsmc_host, uint32_t cs)
{
    int *src = NULL, *dst = NULL;
    struct DSMC_MAP *map = &dsmc_host->host->dsmcHostDev.ChipSelMap[cs].regionMap[0];
    uint32_t size = DMA_SIZE;

    rt_kprintf("dma test1: copy from ddr to psram\n");
    src = rt_dma_malloc(size);
    RT_ASSERT(src != RT_NULL);

    dst = (int *)map->phys;

    dma_memcpy(dst, src, size);
    rt_dma_free(src);

    rt_kprintf("dma test2: copy from psram to ddr\n");
    src = (int *)map->phys;

    dst = rt_dma_malloc(size);
    RT_ASSERT(dst != RT_NULL);

    dma_memcpy(dst, src, size);
    rt_dma_free(dst);

    rt_kprintf("dma test3: copy from psram to psram\n");
    src = (int *)map->phys;
    dst = (int *)(map->phys + size);

    dma_memcpy(dst, src, size);
    rt_kprintf("DMA test done\n");

    return 0;
}

void psram_simple_test(struct rockchip_rt_dsmc_host *dsmc_host, uint32_t cs)
{
    uint32_t i, j;
    struct dsmc_host_ops *ops = dsmc_host->ops;
    struct DSMC_MAP *map = &dsmc_host->host->dsmcHostDev.ChipSelMap[cs].regionMap[0];
    uint32_t test_cap;
    uint32_t read, write;
    uint32_t test_data[] = {0x5aa5f00f, 0x0, 0xffffffff, 0x3cc3d22d};

    rt_kprintf("start cs%d simple test\n", cs);

    for (j = 0; j < 4; j++)
    {
        test_cap = map->size;
        rt_kprintf("write, test_cap = 0x%x\n", test_cap);
        rt_kprintf("          write\n");
        for (i = 0; i < test_cap; i += 4)
        {
            ops->write(dsmc_host, cs, 0, i, test_data[j]);
        }
        rt_kprintf("read\n");
        test_cap = map->size;
        rt_kprintf("          read\n");
        for (i = 0; i < test_cap; i += 4)
        {
            ops->read(dsmc_host, cs, 0, i, &read);
            if (read != test_data[j])
            {
                rt_kprintf("addr offset 0x%x: read = 0x%x, wr = 0x%x, error(0x%x)\n",
                           i, read, test_data[j], test_data[j] ^ read);
            }
        }
    }

    rt_kprintf("phase 1: write address to data\n");

    test_cap = map->size;
    for (i = 0; i < test_cap; i += 4)
    {
        ops->write(dsmc_host, cs, 0, i, map->phys + i);
    }
    for (i = 0; i < test_cap; i += 4)
    {
        ops->read(dsmc_host, cs, 0, i, &read);
        write = map->phys + i;
        if (read != write)
        {
            rt_kprintf("addr offset 0x%x: read = 0x%x, wr = 0x%x, error(0x%x)\n",
                       i, read, write, write ^ read);
        }
    }

    rt_kprintf("phase 2: write 0x0f0f5aa5 + address\n");
    test_cap = map->size;
    for (i = 0; i < test_cap; i += 4)
    {
        ops->write(dsmc_host, cs, 0, i, map->phys + i + 0x0f0f5aa5);
    }

    test_cap = map->size;
    for (i = 0; i < test_cap; i += 4)
    {
        ops->read(dsmc_host, cs, 0, i, &read);
        write = map->phys + i + 0x0f0f5aa5;
        if (read != write)
        {
            rt_kprintf("addr offset 0x%x: read = 0x%x, wr = 0x%x, error(0x%x)\n",
                       i, read, write, write ^ read);
        }
    }

    rt_kprintf("phase 3: dma copy by software\n");
    dma_test_psram(dsmc_host, cs);

    rt_kprintf("cs%d simple test done.\n", cs);
}

static int plc_simple_test(struct rockchip_rt_dsmc_host *dsmc_host, uint32_t cs)
{
    uint32_t i;
    struct dsmc_host_ops *ops = dsmc_host->ops;
    struct DSMC_MAP *map = &dsmc_host->host->dsmcHostDev.ChipSelMap[cs].regionMap[0];
    int *src = NULL, *dst = NULL;
    uint32_t size = DMA_SIZE;
    int ret = 0;
    uint32_t timeout = 1000000;

    rt_kprintf("phase 1: dma memcopy from host DDR to slave memory\n");
    src = rt_dma_malloc(size);
    RT_ASSERT(src != RT_NULL);

    /* init src address */
    rt_memset(src, 0x66, size);

    HAL_DCACHE_CleanInvalidateByRange((uint32_t)src, size);
    HAL_DCACHE_CleanInvalidateByRange((uint32_t)map->phys, size);

    timeout = 10000;
    if (!ops->copy_to(dsmc_host, cs, 0, (uint32_t)src, 0x0, size))
    {
        while (ops->copy_to_state(dsmc_host))
        {
            rt_thread_mdelay(1);
            timeout--;
            if (!timeout)
            {
                rt_kprintf("DSMC: wait copy to complete timeout\n");
                ret = -1;
                goto err;
            }
        }
        rt_memcmp(src, (int *)map->phys, size);
    }
    else
    {
        rt_kprintf("err: phase 1: test skip!!!\n");
    }

    rt_kprintf("phase2 : dma memcopy from slave memory to host DDR\n");
    dst = rt_dma_malloc(size);
    RT_ASSERT(dst != RT_NULL);

    rt_memset(dst, 0x88, size);
    /* init plc src address */
    for (i = 0; i < size; i += 4)
    {
        ops->write(dsmc_host, cs, 0, i, 0x10100000 + i);
    }
    HAL_DCACHE_CleanInvalidateByRange((uint32_t)dst, size);
    HAL_DCACHE_CleanInvalidateByRange((uint32_t)map->phys, size);

    timeout = 10000;
    if (!ops->copy_from(dsmc_host, cs, 0x0, 0, (uint32_t)dst, size))
    {
        while (ops->copy_from_state(dsmc_host))
        {
            rt_thread_mdelay(1);
            timeout--;
            if (!timeout)
            {
                rt_kprintf("DSMC: wait copy from complete timeout\n");
                ret = -1;
                goto err;
            }
        }
        rt_memcmp((int *)map->phys, dst, size);
    }
    else
    {
        rt_kprintf("err: phase 1: test skip!!!\n");
    }

    rt_dma_free(src);
    rt_dma_free(dst);

    rt_kprintf("plc_simple_test test done\n");
    return 0;

err:
    rt_kprintf("plc_simple_test test error!\n");
    return ret;
}

void dsmc_slave_latency_cpu(struct rockchip_rt_dsmc_host *dsmc_host, uint32_t cs)
{
    uint32_t i;
    struct dsmc_host_ops *ops = dsmc_host->ops;
    uint32_t read, counter;
    uint32_t kt[2], diff;

    rt_kprintf("cpu access dsmc latency test\n");

    counter = 1000000;

    kt[0] = HAL_GetTick();
    for (i = 0; i < counter; i++)
    {
        ops->read(dsmc_host, cs, 3, 0x100, &read);
    }
    kt[1] = HAL_GetTick();
    diff = kt[1] - kt[0];
    rt_kprintf("counter %d, cost %ums, read latency %uns\n",
               counter, diff, diff * 1000000 / counter);
    rt_kprintf("read = 0x%x\n", read);

    kt[0] = HAL_GetTick();
    for (i = 0; i < counter; i++)
    {
        ops->write(dsmc_host, cs, 3, 0x100, 0x6);
    }
    kt[1] = HAL_GetTick();
    diff = kt[1] - kt[0];
    rt_kprintf("counter %d, cost %ums, write latency %uns\n",
               counter, diff, diff * 1000000 / counter);
}

void dsmc_slave_speed_cpu(struct rockchip_rt_dsmc_host *dsmc_host, uint32_t cs)
{
    uint32_t i;
    struct dsmc_host_ops *ops = dsmc_host->ops;
    struct DSMC_MAP *map = &dsmc_host->host->dsmcHostDev.ChipSelMap[cs].regionMap[0];
    uint32_t kt[2], rd_diff, wr_diff;
    uint32_t read;

    rt_kprintf("cpu access dsmc speed\n");

    kt[0] = HAL_GetTick();
    for (i = 0; i < map->size; i += 4)
    {
        ops->write(dsmc_host, cs, 0, i, 0xffffffff);
    }
    HAL_DCACHE_CleanInvalidateByRange((uint32_t)map->phys, map->size);
    kt[1] = HAL_GetTick();
    wr_diff = kt[1] - kt[0];
    rt_kprintf("cpu write %uMB, cost %ums,  %uMB/s\n",
               (uint32_t)map->size / 1024 / 1024,
               wr_diff,
               map->size / wr_diff / 1000);

    kt[0] = HAL_GetTick();
    for (i = 0; i < map->size; i += 4)
    {
        ops->read(dsmc_host, cs, 0, i, &read);
    }
    kt[1] = HAL_GetTick();
    rd_diff = kt[1] - kt[0];

    rt_kprintf("cpu read %uMB, cost %ums,  %uMB/s\n",
               (uint32_t)map->size / 1024 / 1024,
               rd_diff,
               map->size / rd_diff / 1000);

}

static int dsmc_slave_speed_dma(struct rockchip_rt_dsmc_host *dsmc_host, uint32_t cs)
{
    int *src = NULL, *dst = NULL;
    struct DSMC_MAP *map = &dsmc_host->host->dsmcHostDev.ChipSelMap[cs].regionMap[0];
    uint32_t size = DMA_SIZE;

    rt_kprintf("dma access dsmc speed\n");

    rt_kprintf("DMA write DSMC\n");
    src = rt_dma_malloc(size);
    RT_ASSERT(src != RT_NULL);

    dst = (int *)map->phys;

    dma_memcpy(dst, src, size);

    rt_dma_free(src);

    rt_kprintf("DMA read DSMC\n");
    src = (int *)map->phys;
    dst = rt_dma_malloc(size);
    RT_ASSERT(dst != RT_NULL);

    dma_memcpy(dst, src, size);

    rt_dma_free(dst);

    return 0;
}

void dsmc_test(int argc, char **argv)
{
    uint32_t i;
    rt_device_t dev;
    struct rockchip_rt_dsmc_host *dsmc_host;

    rt_kprintf("this is dsmc_test\n");

    dev = rt_device_find("dsmc_host");
    if (dev == RT_NULL)
    {
        rt_kprintf("dsmc_host not found\n");
        return;
    }
    dsmc_host = (struct rockchip_rt_dsmc_host *)dev;

    for (i = 0; i < DSMC_MAX_SLAVE_NUM; i++)
    {
        if (dsmc_host->host->dsmcHostDev.ChipSelCfg[i].deviceType == DEV_UNKNOWN)
            continue;
        psram_simple_test(dsmc_host, i);
        if (dsmc_host->host->dsmcHostDev.ChipSelCfg[i].deviceType == DEV_DSMC_LB)
        {
            plc_simple_test(dsmc_host, i);
            dsmc_slave_latency_cpu(dsmc_host, i);
        }
        dsmc_slave_speed_cpu(dsmc_host, i);
        dsmc_slave_speed_dma(dsmc_host, i);
    }
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(dsmc_test, dsmc tester);
#endif
#endif
