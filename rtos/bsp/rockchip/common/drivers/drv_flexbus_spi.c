/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * @file    drv_flexbus_spi.c
 * @author  Jon Lin
 * @version V1.0
 * @date    2024-09-24
 * @brief   FLEXBUS SPI mode driver
 *
 ******************************************************************************
 */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup FLEXBUS SPI mode
 *  @{
 */

/** @defgroup FLEXBUS_SPI_How_To_Use How To Use
 *  @{

 See more information, click [here](https://www.rt-thread.org/document/site/programming-manual/device/spi/spi/)

 @} */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_FLEXBUS_SPI)

#include "hal_base.h"
#include "drv_clock.h"
#include "drv_pm.h"
#include "hal_bsp.h"

#include "drv_flexbus.h"

#define FLEXBUS_DMA_TIMEOUT_MS          (0x1000)

#define SPI_DEBUG 0

#if SPI_DEBUG
#define spi_dbg(dev, fmt, ...) \
do { \
    rt_kprintf("%s: " fmt, ((struct rt_device *)dev)->parent.name, ##__VA_ARGS__); \
} while(0)
#else
#define spi_dbg(dev, fmt, ...) \
do { \
} while(0)
#endif

static int32_t spi_dbg_hex(char *s, void *buf, uint32_t width, uint32_t len)
{
    uint32_t i, j;
    uint32_t *p32 = (uint32_t *)buf;

    j = 0;
    for (i = 0; i < len; i++)
    {
        if (j == 0)
        {
            rt_kprintf("%s 0x%p+0x%04lx:", s, buf, i * width);
        }
        rt_kprintf(" %08lx", p32[i]);
        if (++j >= 4)
        {
            j = 0;
            rt_kprintf("\n");
        }
    }
    rt_kprintf("\n");

    return HAL_OK;
}

struct rk_flexbus_spi_cs
{
    rt_int8_t pin;
};

/* FLEXBUS SPI mode driver's private data. */
struct rk_flexbus_spi
{
    /* SPI bus */
    struct rt_spi_bus bus;
    struct rt_spi_device rt_spi_dev[1]; /* each cs one dev */
    struct rk_flexbus_spi_cs rt_spi_cs[1];
    uint32_t speed;
    bool irq;

    /* hardware related */
    const struct HAL_FLEXBUS_DEV *hal_dev;
    void (*isr)(void);
    struct rt_mutex spi_lock;
    struct clk_gate *tx_clk_gate;
    struct clk_gate *rx_clk_gate;
    struct clk_gate *aclk_gate;
    struct clk_gate *hclk_gate;

    /* status */
    rt_int32_t error;
    struct rt_completion done;

    /* Hal */
    struct HAL_FLEXBUS_SPI_HANDLE instance;
};

/**
 * rk_flexbus_spi_irq - spi irq handler
 * @spi: SPI private structure which contains SPI specific data
 */
static void rk_flexbus_spi_irq(struct rk_flexbus_spi *spi)
{
    struct HAL_FLEXBUS_SPI_HANDLE *flexbus = &spi->instance;
    int status;

    /* enter interrupt */
    rt_interrupt_enter();

    status = HAL_FLEXBUS_SPI_IrqHandler(flexbus);
    if (status)
    {
        spi->error = status;
        spi_dbg(&spi->bus.parent, "irq handle error: %d.\n", spi->error);
    }
    rt_completion_done(&spi->done);

    /* leave interrupt */
    rt_interrupt_leave();
}

/**
 * rk_flexbus_spi_configure - probe function for SPI Controller
 * @device: SPI device structure
 * @config: SPI config structure
 * @return: return error code
 */
static rt_err_t rk_flexbus_spi_configure(struct rt_spi_device *device,
        struct rt_spi_configuration *config)
{
    struct rk_flexbus_spi *spi = device->bus->parent.user_data;
    struct HAL_FLEXBUS_SPI_HANDLE *flexbus = &spi->instance;
    struct HAL_FLEXBUS_SPI_CONFIG cfg = { 0 };
    rt_err_t ret;

    if (!config->max_hz || (config->data_width > 16))
    {
        rt_kprintf("%s invalid, max_hz=%d width=%d\n", __func__, config->max_hz, config->data_width);
        return -RT_EINVAL;
    }

    if ((config->mode & RT_SPI_SLAVE) || (config->mode & RT_SPI_CS_HIGH))
    {
        rt_kprintf("%s invalid, mode=0x%x\n", __func__, config->mode);
        return -RT_EINVAL;
    }

    rt_mutex_take(&spi->spi_lock, RT_WAITING_FOREVER);

    cfg.width = config->data_width;
    cfg.mode = config->mode & 0x3;
    cfg.lsb = config->mode & RT_SPI_MSB ? 0 : 1;
    cfg.dll = config->reserved & 0xffff;

    ret = HAL_FLEXBUS_SPI_Config(flexbus, &cfg);
    if (ret)
    {
        ret = -RT_ERROR;
    }

    spi->speed = config->max_hz;
    clk_set_rate(spi->hal_dev->txclkID,  spi->speed * 2);

    spi_dbg(&spi->bus.parent, "SPI SCLK %dHz, speed %dHz, width=%d, mode=%x, dll=%x\n",
            spi->speed, cfg.width, cfg->mode, cfg->dll);

    rt_mutex_release(&spi->spi_lock);

    return RT_EOK;
}

static rt_err_t rk_flexbus_spi_wait_idle(struct rk_flexbus_spi *spi, uint32_t timeout_ms)
{
    struct HAL_FLEXBUS_SPI_HANDLE *flexbus = &spi->instance;
    rt_err_t ret = -RT_ETIMEOUT;
    uint32_t timeout;

    if (HAL_FLEXBUS_SPI_IsXferDone(flexbus) == HAL_OK)
    {
        return 0;
    }

    timeout = rt_tick_get() + timeout_ms;
    do
    {
        ret = HAL_FLEXBUS_SPI_IsXferDone(flexbus);
        if (ret == HAL_BUSY)
        {
            continue;
        }
        else if (ret == HAL_OK)
        {
            ret = RT_EOK;
            break;
        }
        else
        {
            ret = -RT_EIO;
            break;
        }
    }
    while (timeout > rt_tick_get());

    return ret;
}

/**
 * rk_flexbus_spi_xfer - spi tranfer api call by spi ops
 * @device: SPI device structure
 * @message: SPI message structure
 * @return: return error code
 */
static rt_uint32_t rk_flexbus_spi_xfer(struct rt_spi_device *device, struct rt_spi_message *message)
{
    struct rk_flexbus_spi *spi = device->bus->parent.user_data;
    struct HAL_FLEXBUS_SPI_HANDLE *flexbus = &spi->instance;
    rt_err_t ret = RT_EOK;

    if ((!message->send_buf == !message->recv_buf) || (message->length > flexbus->maxSize))
    {
        rt_kprintf("%s input invalid, %p-%p len=%d\n", __func__, message->send_buf,
                   message->recv_buf, message->length);
        ret = RT_EINVAL;
        goto out;
    }

    rt_mutex_take(&spi->spi_lock, RT_WAITING_FOREVER);

    if (message->cs_take)
        HAL_FLEXBUS_SPI_SetCS(flexbus, true);

    spi_dbg(&spi->bus.parent, "%s xfer, tx=%p/rx=%p/len=%x\n",
            __func__, message->send_buf, message->recv_buf, message->length);

    spi->error = 0;

    if (message->send_buf)
    {
#ifdef RT_USING_CACHE
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, (void *)message->send_buf, message->length);
#endif
        ret = HAL_FLEXBUS_SPI_SendStart(flexbus, (uint32_t)message->send_buf, message->length);
    }
    else
    {
#ifdef RT_USING_CACHE
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, message->recv_buf, message->length);
#endif
        ret = HAL_FLEXBUS_SPI_RecvStart(flexbus, (uint32_t)message->recv_buf, message->length);
    }

    if (ret != HAL_OK)
    {
        ret = -RT_EIO;
        goto complete;
    }

    if (spi->irq)
    {
        ret = rt_completion_wait(&spi->done, rt_tick_from_millisecond(FLEXBUS_DMA_TIMEOUT_MS));
    }
    else
    {
        ret = rk_flexbus_spi_wait_idle(spi, FLEXBUS_DMA_TIMEOUT_MS);
    }

    if ((ret == RT_EOK) && message->recv_buf)
    {
#ifdef RT_USING_CACHE
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, message->recv_buf, message->length);
#endif
    }

complete:
    if ((ret != RT_EOK) && (ret != HAL_INVAL))
    {
        spi->error = ret;
        rt_kprintf("%s error: %d\n", __func__, spi->error);
        spi_dbg_hex("flexbus:", spi->hal_dev->pReg, 4, 0x80);
    }

    /* Disable SPI when finished. */
    HAL_FLEXBUS_SPI_Stop(flexbus);

out:
    /* cs only for master */
    if (message->cs_release)
        HAL_FLEXBUS_SPI_SetCS(flexbus, false);

    rt_mutex_release(&spi->spi_lock);

    /* Successful to return message length and fail to return 0. */
    return spi->error ? 0 : message->length;
}

static struct rt_spi_ops rk_flexbus_spi_ops =
{
    rk_flexbus_spi_configure,
    rk_flexbus_spi_xfer,
};

/**
 * rk_flexbus_spi_probe - probe function for SPI Controller
 * @spi: SPI private structure which contains SPI specific data
 * @bus_name: SPI bus name
 * @slave: slave or master mode for SPI
 */
static rt_err_t rk_flexbus_spi_probe(struct rk_flexbus_spi *spi, char *bus_name)
{
    struct flexbus_dev *flexbus;
    char *dev_name;
    rt_err_t ret;

    /* register flexbus */
    flexbus = (struct flexbus_dev *)rt_device_find("flexbus");
    if (flexbus == RT_NULL)
    {
        rt_kprintf("%s failed! Can't find flexbus device!\n", __func__);
        return -RT_ERROR;
    }

    spi->instance.fb = &flexbus->instance;

    spi->tx_clk_gate = get_clk_gate_from_id(spi->hal_dev->txclkGateID);
    spi->rx_clk_gate = get_clk_gate_from_id(spi->hal_dev->rxclkGateID);
    spi->aclk_gate = get_clk_gate_from_id(spi->hal_dev->aclkGateID);
    spi->hclk_gate = get_clk_gate_from_id(spi->hal_dev->hclkGateID);

    clk_enable(spi->tx_clk_gate);
    clk_enable(spi->rx_clk_gate);
    clk_enable(spi->aclk_gate);
    clk_enable(spi->hclk_gate);

    HAL_FLEXBUS_SPI_Init(&spi->instance, spi->hal_dev, true);

    rt_completion_init(&spi->done);
    if (rt_mutex_init(&spi->spi_lock, "fb_spi_lock", RT_IPC_FLAG_FIFO) != RT_EOK)
        RT_ASSERT(0);

    spi->bus.parent.user_data = spi;
    spi->irq = true;

    ret = rt_spi_bus_register(&spi->bus, bus_name, &rk_flexbus_spi_ops);
    if (!ret)
    {
        dev_name = rt_malloc(20);
        rt_memset(dev_name, 0, 20);
        strcpy(dev_name, bus_name);
        strcat(dev_name, "_0");
        rt_spi_bus_attach_device(&spi->rt_spi_dev[0], dev_name, bus_name, (void *)&spi->rt_spi_cs[0]);
        rt_free(dev_name);
    }

    return ret;
}

static struct rk_flexbus_spi s_flexbus_spi0;

static void rk_flexbus_spi0_irq(int irq, void *param)
{
    rk_flexbus_spi_irq(&s_flexbus_spi0);
}

static struct rk_flexbus_spi s_flexbus_spi0 =
{
    .hal_dev = &g_flexbusDev,
    .isr = (void *) &rk_flexbus_spi0_irq,
    .rt_spi_cs[0].pin = 0,
};

int rk_rt_hw_spi_init(void)
{
    rk_flexbus_spi_probe(&s_flexbus_spi0, "fb_spi0");

    return 0;
}

INIT_PREV_EXPORT(rk_rt_hw_spi_init);
/** @} */  // FLEXBUS_SPI_Public_Functions

#endif
/** @} */

/** @} */
