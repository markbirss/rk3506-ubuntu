/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_spi.c
  * @author  David Wu
  * @version V0.1
  * @date    26-Feb-2019
  * @brief   SPI Driver
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup SPI
 *  @{
 */

/** @defgroup SPI_How_To_Use How To Use
 *  @{

 See more information, click [here](https://www.rt-thread.org/document/site/programming-manual/device/spi/spi/)

 @} */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_SPI) && (defined(RT_USING_SPI0) || defined(RT_USING_SPI1) || defined(RT_USING_SPI2))

#include "hal_base.h"
#include "drv_clock.h"
#include "drv_pm.h"
#include "drv_spi.h"
#include "dma.h"
#include "hal_bsp.h"

#define SPI_DEBUG 0 /* 1-master 2-slave */

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
#define spi_err(dev, fmt, ...) \
do { \
    rt_kprintf("%s: " fmt, ((struct rt_device *)dev)->parent.name, ##__VA_ARGS__); \
} while(0)

#define ROCKCHIP_SPI_CS_0 (0)
#define ROCKCHIP_SPI_CS_1 (1)

#define RXBUSY (1 << 0)
#define TXBUSY (1 << 1)

#define ROCKCHIP_SPI_TX_IDLE_TIMEOUT (20)

static int32_t spi_dbg_hex(char *s, void *buf, uint32_t width, uint32_t len)
{
    uint32_t i, j;
    unsigned char *p8 = (unsigned char *)buf;
    unsigned short *p16 = (unsigned short *)buf;
    uint32_t *p32 = (uint32_t *)buf;

    j = 0;
    for (i = 0; i < len; i++)
    {
        if (j == 0)
        {
            rt_kprintf("[spi] %s 0x%p+0x%lx: ", s, buf, i * width);
        }

        if (width == 4)
            rt_kprintf("0x%08lx,", p32[i]);
        else if (width == 2)
            rt_kprintf("0x%04x,", p16[i]);
        else
            rt_kprintf("0x%02x,", p8[i]);

        if (++j >= 4)
        {
            j = 0;
            rt_kprintf("\n");
        }
    }
    rt_kprintf("\n");

    return HAL_OK;
}

struct rockchip_spi_cs
{
    rt_int8_t pin;
};

/* SPI Controller driver's private data. */
struct rockchip_spi
{
    /* SPI bus */
    struct rt_spi_bus bus;
    struct rt_spi_device rt_spi_dev[2]; /* each cs one dev */
    struct rockchip_spi_cs rt_spi_cs[2];

    /* hardware related */
    const struct HAL_SPI_DEV *hal_dev;
    void (*isr)(void);
    rt_uint16_t dma_burst_size;
    struct rt_mutex spi_lock;
    struct clk_gate *pclk_gate;
    struct clk_gate *sclk_gate;

    /* status */
    rt_int32_t error;
    struct rt_completion done;
    struct rt_semaphore sem_lock;

    /* Hal */
    struct SPI_HANDLE instance;

    /* DMA */
    rt_uint32_t state;
    struct rt_device *dma;
    struct rt_dma_transfer tx_dma_xfer;
    struct rt_dma_transfer rx_dma_xfer;
    bool no_dma;
};

/**
 * rockchip_spi_irq - spi irq handler
 * @spi: SPI private structure which contains SPI specific data
 */
static void rockchip_spi_irq(struct rockchip_spi *spi)
{
    struct SPI_HANDLE *pSPI = &spi->instance;
    int status;

    /* enter interrupt */
    rt_interrupt_enter();

    spi_dbg(&spi->bus.parent, "%s isr=%x\n", __func__, pSPI->pReg->ISR);

    status = HAL_SPI_IrqHandler(pSPI);
    if (status != HAL_BUSY)
    {
        spi->error = status;
        if (spi->error)
            spi_dbg(&spi->bus.parent, "irq handle error: %d.\n", spi->error);
        rt_completion_done(&spi->done);
    }

    /* leave interrupt */
    rt_interrupt_leave();
}

/**
 * rockchip_spi_configure - probe function for SPI Controller
 * @device: SPI device structure
 * @configuration: SPI configuration structure
 * @return: return error code
 */
static rt_err_t rockchip_spi_configure(struct rt_spi_device *device,
                                       struct rt_spi_configuration *configuration)
{
    struct rockchip_spi *spi = device->bus->parent.user_data;
    struct SPI_HANDLE *pSPI = &spi->instance;
    struct SPI_CONFIG *pSPIConfig = &pSPI->config;

#ifdef RT_USING_DMA
    if (spi->dma == NULL && !spi->no_dma)
    {
        spi->dma = rt_dma_get((uint32_t)(spi->hal_dev->txDma.dmac));
        if (spi->dma == NULL)
        {
            spi_err(&spi->bus.parent, "%s get dma failed, CPU/IT instead\n", __func__);
            spi->no_dma = true;
        }
    }
#endif

    if (!configuration->max_hz)
        return RT_EINVAL;

    rt_mutex_take(&spi->spi_lock, RT_WAITING_FOREVER);

    /* Data width */
    if (configuration->data_width <= 8)
        pSPIConfig->nBytes = CR0_DATA_FRAME_SIZE_8BIT;
    else if (configuration->data_width <= 16)
        pSPIConfig->nBytes = CR0_DATA_FRAME_SIZE_16BIT;
    else
    {
        rt_mutex_release(&spi->spi_lock);
        return -RT_EINVAL;
    }
    /* CPOL */
    if (configuration->mode & RT_SPI_CPOL)
        pSPIConfig->clkPolarity = CR0_POLARITY_HIGH;
    else
        pSPIConfig->clkPolarity = CR0_POLARITY_LOW;

    /* CPHA */
    if (configuration->mode & RT_SPI_CPHA)
        pSPIConfig->clkPhase = CR0_PHASE_2EDGE;
    else
        pSPIConfig->clkPhase = CR0_PHASE_1EDGE;

    /* MSB or LSB */
    if (configuration->mode & RT_SPI_MSB)
        pSPIConfig->firstBit = CR0_FIRSTBIT_MSB;
    else
        pSPIConfig->firstBit = CR0_FIRSTBIT_LSB;

    if (configuration->mode & RT_SPI_SLAVE)
        pSPIConfig->opMode = CR0_OPM_SLAVE;
    else
        pSPIConfig->opMode = CR0_OPM_MASTER;

    if (pSPIConfig->speed == configuration->max_hz)
    {
        rt_mutex_release(&spi->spi_lock);
        return RT_EOK;
    }

    /* RSD */
    pSPIConfig->rsd = CR0_RSD(configuration->reserved & RK_SPI_RESERVED_RSD_MASK);

    pSPIConfig->speed = configuration->max_hz;
    if (pSPIConfig->opMode == CR0_OPM_MASTER)
    {
        if (pSPIConfig->speed > HAL_SPI_MASTER_MAX_SCLK_OUT)
            pSPIConfig->speed = HAL_SPI_MASTER_MAX_SCLK_OUT;

        /*
         * Some platforms are unable to divide the frequency to a more
         * accurate working clock, so it's to set a higher frequency
         * working clock and the frequency is divided internally by the
         * controller.
         */
        if (spi->hal_dev->maxFreq)
            pSPI->maxFreq = spi->hal_dev->maxFreq;
        else if (pSPIConfig->speed > (HAL_SPI_MASTER_MAX_SCLK_OUT >> 1))
            pSPI->maxFreq = 2 * pSPIConfig->speed;
        else
            pSPI->maxFreq = 3 * HAL_SPI_MASTER_MAX_SCLK_OUT;

        clk_set_rate(spi->hal_dev->clkId,  pSPI->maxFreq);
        pSPI->maxFreq = clk_get_rate(spi->hal_dev->clkId);
    }
    else
    {
        if (pSPIConfig->speed > HAL_SPI_SLAVE_MAX_SCLK_OUT)
            pSPIConfig->speed = HAL_SPI_SLAVE_MAX_SCLK_OUT;

        if (spi->hal_dev->maxFreq)
            pSPI->maxFreq = spi->hal_dev->maxFreq;
        else if (pSPIConfig->speed > (HAL_SPI_SLAVE_MAX_SCLK_OUT >> 1))
            pSPI->maxFreq = 4 * pSPIConfig->speed;
        else
            pSPI->maxFreq = 4 * HAL_SPI_SLAVE_MAX_SCLK_OUT;

        clk_set_rate(spi->hal_dev->clkId, pSPI->maxFreq);
        pSPI->maxFreq = clk_get_rate(spi->hal_dev->clkId);
    }
    pSPI->config.configured = false;

    spi_dbg(&spi->bus.parent, "SPI SCLK %dHz, speed %dHz, width=%d, mode=%x, resersved=%x\n",
            pSPI->maxFreq, pSPIConfig->speed,
            configuration->data_width, configuration->mode, configuration->reserved);
    rt_mutex_release(&spi->spi_lock);

    return RT_EOK;
}

static void rockchip_spi_dma_handle_err(struct rockchip_spi *spi)
{
    struct SPI_HANDLE *pSPI = &spi->instance;

    rt_sem_take(&spi->sem_lock, RT_WAITING_FOREVER);
    /*
     * For DMA mode, we need terminate DMA channel and flush
     * fifo for the next transfer if DMA thansfer timeout.
     * handle_err() was called by core if transfer failed.
     * Maybe it is reasonable for error handling here.
     */
    if (HAL_SPI_IsDmaXfer(pSPI))
    {
        if (spi->state & RXBUSY)
        {
            rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_STOP,
                              &spi->rx_dma_xfer);
            rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_RELEASE_CHANNEL,
                              &spi->rx_dma_xfer);
            HAL_SPI_FlushFifo(pSPI);
        }

        if (spi->state & TXBUSY)
        {
            rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_STOP,
                              &spi->tx_dma_xfer);
            rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_RELEASE_CHANNEL,
                              &spi->tx_dma_xfer);
        }
    }
    rt_sem_release(&spi->sem_lock);
}

static void rockchip_spi_dma_rxcb(void *data)
{
    struct rockchip_spi *spi = data;

    rt_sem_take(&spi->sem_lock, RT_WAITING_FOREVER);

    spi->state &= ~RXBUSY;
    if (!(spi->state & TXBUSY))
        rt_completion_done(&spi->done);
    rt_sem_release(&spi->sem_lock);
}

static void rockchip_spi_dma_txcb(void *data)
{
    struct rockchip_spi *spi = data;

    rt_sem_take(&spi->sem_lock, RT_WAITING_FOREVER);

    spi->state &= ~TXBUSY;
    if (!(spi->state & RXBUSY))
        rt_completion_done(&spi->done);
    rt_sem_release(&spi->sem_lock);
}

static rt_err_t rockchip_spi_wait_idle(struct rockchip_spi *spi, bool forever)
{
    struct SPI_HANDLE *pSPI = &spi->instance;
    rt_err_t ret = RT_EBUSY;
    uint32_t timeout;

    if (HAL_SPI_QueryBusState(pSPI) == HAL_OK)
    {
        return 0;
    }

    timeout = rt_tick_get() + ROCKCHIP_SPI_TX_IDLE_TIMEOUT; /* some tolerance */
    do
    {
#ifndef RT_USING_SPI_SLAVE_FLEXIBLE_LENGTH
        /* If CS release is detected, the transmission is considered to have stopped */
        if (HAL_SPI_IsCsInactive(pSPI))
        {
            ret = 0;
            break;
        }
#endif

        ret = HAL_SPI_QueryBusState(pSPI);
        if (ret == HAL_OK)
            break;
    }
    while (timeout > rt_tick_get() || forever);

    return ret;
}

static rt_uint32_t rockchip_spi_calc_burst_size(rt_uint32_t data_len)
{
    rt_uint32_t i;

    /* burst size: 1, 2, 4, 8 */
    for (i = 1; i < 8; i <<= 1)
    {
        if (data_len & i)
            break;
    }

    /* DW_DMA is not support burst 2 */
    if (i == 2)
        i = 1;

    return i;
}

/**
 * rockchip_spi_dma_prepare - spi prepare dma tranfer config
 * @spi: SPI private structure which contains SPI specific data
 * @message: SPI message structure
 * @return: return error code
 */
static rt_err_t rockchip_spi_dma_prepare(struct rockchip_spi *spi, struct rt_spi_message *message)
{
    struct SPI_HANDLE *pSPI = &spi->instance;
    rt_err_t ret;

    rt_memset(&spi->rx_dma_xfer, 0, sizeof(struct rt_dma_transfer));
    rt_memset(&spi->tx_dma_xfer, 0, sizeof(struct rt_dma_transfer));

    spi->state &= ~RXBUSY;
    spi->state &= ~TXBUSY;

    /* Configure rx firstly. */
    if (message->recv_buf)
    {
        spi->rx_dma_xfer.direction = spi->hal_dev->rxDma.direction;
        spi->rx_dma_xfer.dma_req_num = spi->hal_dev->rxDma.channel;
        spi->rx_dma_xfer.src_addr = spi->hal_dev->rxDma.addr;
        spi->rx_dma_xfer.src_addr_width = pSPI->config.nBytes;
        spi->rx_dma_xfer.src_maxburst = spi->dma_burst_size;
        spi->rx_dma_xfer.dst_addr = (rt_uint32_t)message->recv_buf;
        spi->rx_dma_xfer.dst_addr_width = pSPI->config.nBytes;
        spi->rx_dma_xfer.dst_maxburst = spi->dma_burst_size;

        spi->rx_dma_xfer.len = message->length;
        spi->rx_dma_xfer.callback = rockchip_spi_dma_rxcb;
        spi->rx_dma_xfer.cparam = spi;

        ret = rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_REQUEST_CHANNEL,
                                &spi->rx_dma_xfer);
        if (ret)
            return ret;

        ret = rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_SINGLE_PREPARE,
                                &spi->rx_dma_xfer);
        if (ret)
            goto rx_release;
#ifdef RT_USING_CACHE
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, (void *)message->recv_buf, message->length);
#endif
    }

    if (message->send_buf)
    {
        spi->tx_dma_xfer.direction = spi->hal_dev->txDma.direction;
        spi->tx_dma_xfer.dma_req_num = spi->hal_dev->txDma.channel;
        spi->tx_dma_xfer.dst_addr = spi->hal_dev->txDma.addr;
        spi->tx_dma_xfer.dst_addr_width = pSPI->config.nBytes;
        spi->tx_dma_xfer.dst_maxburst = 8;
        spi->tx_dma_xfer.src_addr = (rt_uint32_t)message->send_buf;
        spi->tx_dma_xfer.src_addr_width = pSPI->config.nBytes;
        spi->tx_dma_xfer.src_maxburst = 8;

        spi->tx_dma_xfer.len = message->length;
        spi->tx_dma_xfer.callback = rockchip_spi_dma_txcb;
        spi->tx_dma_xfer.cparam = spi;

        ret = rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_REQUEST_CHANNEL,
                                &spi->tx_dma_xfer);
        if (ret)
            goto rx_release;

        ret = rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_SINGLE_PREPARE,
                                &spi->tx_dma_xfer);
        if (ret)
            goto tx_release;
#ifdef RT_USING_CACHE
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, (void *)message->send_buf, message->length);
#endif
    }

    if (message->recv_buf)
    {
        spi->state |= RXBUSY;

        /* Start dma transfer. */
        ret = rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_START,
                                &spi->rx_dma_xfer);
        if (ret)
            goto tx_release;
    }

    if (message->send_buf)
    {
        spi->state |= TXBUSY;

        /* Start dma transfer. */
        ret = rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_START,
                                &spi->tx_dma_xfer);
        if (ret)
            goto rx_stop;
    }

    return RT_EOK;

rx_stop:
    if (message->recv_buf)
        rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_STOP,
                          &spi->rx_dma_xfer);

tx_release:
    if (message->send_buf)
        rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_RELEASE_CHANNEL,
                          &spi->tx_dma_xfer);

rx_release:
    if (message->recv_buf)
        rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_RELEASE_CHANNEL,
                          &spi->rx_dma_xfer);

    return ret;
}

/**
 * rockchip_spi_dma_complete - spi compete dma tranfer
 * @spi: SPI private structure which contains SPI specific data
 * @message: SPI message structure
 * @return: return error code
 */
static rt_uint32_t rockchip_spi_dma_complete(struct rockchip_spi *spi, struct rt_spi_message *message)
{
    rt_uint32_t rx_valid = message->length, tx_valid = message->length;

    if (message->recv_buf)
    {
        rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_STOP, &spi->rx_dma_xfer);
#ifdef RT_USING_SPI_SLAVE_FLEXIBLE_LENGTH
        if (spi->state & RXBUSY)
        {
            if (!rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_GET_POSITION, &spi->rx_dma_xfer))
            {
                rx_valid = spi->rx_dma_xfer.position;
            }
            spi_dbg(&spi->bus.parent, "%s rx 0x%x\n", __func__, rx_valid);
        }
#endif
        rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_RELEASE_CHANNEL,
                          &spi->rx_dma_xfer);
#ifdef RT_USING_CACHE
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, (void *)message->recv_buf, message->length);
#endif
    }

    if (message->send_buf)
    {
        rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_STOP, &spi->tx_dma_xfer);
#ifdef RT_USING_SPI_SLAVE_FLEXIBLE_LENGTH
        if (spi->state & TXBUSY)
        {
            if (!rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_GET_POSITION, &spi->tx_dma_xfer))
            {
                tx_valid = spi->tx_dma_xfer.position;
            }
            spi_dbg(&spi->bus.parent, "%s tx 0x%x\n", __func__, tx_valid);
        }
#endif
        rt_device_control(spi->dma, RT_DEVICE_CTRL_DMA_RELEASE_CHANNEL,
                          &spi->tx_dma_xfer);
    }

    return HAL_MIN(tx_valid, rx_valid);
}

/**
 * rockchip_spi_xfer - spi tranfer api call by spi ops
 * @device: SPI device structure
 * @message: SPI message structure
 * @return: return error code
 */
static rt_uint32_t rockchip_spi_xfer(struct rt_spi_device *device, struct rt_spi_message *message)
{
    struct rockchip_spi *spi = device->bus->parent.user_data;
    struct SPI_HANDLE *pSPI = &spi->instance;
    struct rockchip_spi_cs *cs = (struct rockchip_spi_cs *)device->parent.user_data;
    rt_uint32_t timeout, valid = message->length;
    rt_err_t ret = RT_EOK;
#ifndef RT_USING_SPI_PM_PERFORMANCE
    struct clk_gate *pclk_gate, *sclk_gate;
#endif

    /*
     * Check parameter:
     *   1. spi slave(flexible length): Only support dma transfer, defining the burst size.
     */
#ifdef RT_USING_SPI_SLAVE_FLEXIBLE_LENGTH
    if (HAL_SPI_IsSlave(pSPI))
    {
        if (!spi->dma)
        {
            spi_err(&spi->bus.parent, "slave xfer in flex, enable dma firstly!\n");
            return -RT_EINVAL;
        }

        if (!HAL_IS_CACHELINE_ALIGNED(message->send_buf) ||
                !HAL_IS_CACHELINE_ALIGNED(message->recv_buf) ||
                !HAL_IS_CACHELINE_ALIGNED(message->length))
        {
            spi_err(&spi->bus.parent, "slave xfer in flex, tx=%p/rx=%p/len=%x should be cache aligned\n",
                    message->send_buf, message->recv_buf, message->length);
            return -RT_EINVAL;
        }
    }
#if (RT_USING_SLAVE_DMA_RX_BURST_SIZE >= 8)
#error "Define the right RT_USING_SLAVE_DMA_RX_BURST_SIZE!"
#endif
#endif

    rt_mutex_take(&spi->spi_lock, RT_WAITING_FOREVER);

#ifndef RT_USING_SPI_PM_PERFORMANCE
    pm_runtime_request(PM_RUNTIME_ID_SPI);

    pclk_gate = spi->pclk_gate;
    sclk_gate = spi->sclk_gate;

    clk_enable(pclk_gate);
    clk_enable(sclk_gate);
#endif

    if (!HAL_SPI_IsSlave(pSPI))
    {
        if (message->cs_take)
            HAL_SPI_SetCS(pSPI, cs->pin, true);
    }

    if (message->send_buf == NULL && message->recv_buf == NULL)
    {
        ret = RT_EOK;
        goto out;
    }

    /*
     * When support transfer spi slave in flexible length, only the minimum
     * burst size is support.
     */
#ifdef RT_USING_SPI_SLAVE_FLEXIBLE_LENGTH
    spi->dma_burst_size = HAL_SPI_IsSlave(pSPI) ? RT_USING_SLAVE_DMA_RX_BURST_SIZE : rockchip_spi_calc_burst_size(message->length);
#else /* #ifdef RT_USING_SPI_SLAVE_FLEXIBLE_LENGTH */
    spi->dma_burst_size = rockchip_spi_calc_burst_size(message->length);
#endif
    pSPI->dmaBurstSize = spi->dma_burst_size;

    HAL_SPI_Configure(pSPI, message->send_buf, message->recv_buf, message->length);

    spi_dbg(&spi->bus.parent, "%s slave=%d dma=%p(null-no dma) can_dma=%d(1-en) dma_burst=%d\n",
            __func__, HAL_SPI_IsSlave(pSPI), spi->dma, HAL_SPI_CanDma(pSPI), spi->dma_burst_size);
    spi_dbg(&spi->bus.parent, "%s xfer, tx=%p/rx=%p/len=%x\n",
            __func__, message->send_buf, message->recv_buf, message->length);

    spi->error = 0;
    if (!HAL_SPI_IsSlave(pSPI))
    {
        /* Use poll mode for master while less fifo length. */
        if (spi->dma && HAL_SPI_CanDma(pSPI))
        {
            HAL_SPI_DmaTransfer(pSPI);
            ret = rockchip_spi_dma_prepare(spi, message);
            if (RT_EOK != ret)
                goto complete;

            timeout = HAL_SPI_CalculateTimeout(pSPI);
            ret = rt_completion_wait(&spi->done, rt_tick_from_millisecond(timeout));
            if (RT_EOK != ret)
            {
                spi_err(&spi->bus.parent, "%s timer out \n", __func__);
                rockchip_spi_dma_handle_err(spi);
            }
            else
            {
                rockchip_spi_dma_complete(spi, message);
            }
            if (message->send_buf && ret == RT_EOK)
            {
                rockchip_spi_wait_idle(spi, false);
            }
        }
        else
        {
            HAL_SPI_PioTransfer(pSPI);
            if (message->send_buf)
            {
                rockchip_spi_wait_idle(spi, false);
            }
        }
    }
    else
    {
        /* Use IT mode for slave while less fifo length. */
        if (spi->dma && HAL_SPI_CanDma(pSPI))
        {
            HAL_SPI_DmaTransfer(pSPI);
            ret = rockchip_spi_dma_prepare(spi, message);
            if (RT_EOK != ret)
                goto complete;

#if (SPI_DEBUG == 2)
            spi_err(&spi->bus.parent, "slave xfer ready in dma\n");
#endif
            /* Timeout is forever for slave. */
            rt_completion_wait(&spi->done, RT_WAITING_FOREVER);
            valid = rockchip_spi_dma_complete(spi, message);
            if (message->send_buf && ret == RT_EOK)
            {
#ifndef RT_USING_SPI_SLAVE_FLEXIBLE_LENGTH
                rockchip_spi_wait_idle(spi, true);
#endif
            }
        }
        else
        {
            ret = HAL_SPI_ItTransfer(pSPI);
            if (ret)
            {
                if (spi->dma)
                {
                    spi_err(&spi->bus.parent, "input invalid, check buf/len aligned!\n");
                    spi_err(&spi->bus.parent, "len=%d\n", message->length);
                }
                else
                {
                    spi_err(&spi->bus.parent, "input invalid, enable dma firstly!\n");
                }

                goto complete;
            }
            rt_hw_interrupt_umask(spi->hal_dev->irqNum);

#if (SPI_DEBUG == 2)
            spi_err(&spi->bus.parent, "slave xfer ready in it\n");
#endif
            /* Timeout is forever for slave. */
            rt_completion_wait(&spi->done, RT_WAITING_FOREVER);

            rt_hw_interrupt_mask(spi->hal_dev->irqNum);
            if (message->send_buf)
            {
                rockchip_spi_wait_idle(spi, true);
            }
        }
#if (SPI_DEBUG == 2)
        spi_err(&spi->bus.parent, "slave xfer done\n");
#endif
    }

complete:
    if ((ret != RT_EOK) && (ret != HAL_INVAL))
    {
        spi->error = ret;
        spi_err(&spi->bus.parent, "%s error: %d\n", __func__, spi->error);
        spi_dbg_hex("reg:", pSPI->pReg, 4, 0x54);
    }

    /* Disable SPI when finished. */
    HAL_SPI_Stop(pSPI);

out:
    /* cs only for master */
    if (!HAL_SPI_IsSlave(pSPI))
    {
        if (message->cs_release)
            HAL_SPI_SetCS(pSPI, cs->pin, false);
    }

#ifndef RT_USING_SPI_PM_PERFORMANCE
#if (SPI_DEBUG == 0)
    clk_disable(sclk_gate);
    clk_disable(pclk_gate);
#endif
    pm_runtime_release(PM_RUNTIME_ID_SPI);
#endif

    rt_mutex_release(&spi->spi_lock);

    /* Successful to return message length and fail to return 0. */
    return spi->error ? 0 : HAL_MIN(valid, message->length);
}

static struct rt_spi_ops rockchip_spi_ops =
{
    rockchip_spi_configure,
    rockchip_spi_xfer,
};

/**
 * rockchip_spi_probe - probe function for SPI Controller
 * @spi: SPI private structure which contains SPI specific data
 * @bus_name: SPI bus name
 * @slave: slave or master mode for SPI
 */
static rt_err_t rockchip_spi_probe(struct rockchip_spi *spi, char *bus_name)
{
    char *dev_name;
    rt_err_t ret;

    /* register irq */
    rt_hw_interrupt_install(spi->hal_dev->irqNum, (void *)spi->isr, spi, RT_NULL);
    rt_hw_interrupt_umask(spi->hal_dev->irqNum);

    HAL_SPI_Init(&spi->instance, spi->hal_dev->base, spi->hal_dev->isSlave);
    spi->pclk_gate = get_clk_gate_from_id(spi->hal_dev->pclkGateID);
    spi->sclk_gate = get_clk_gate_from_id(spi->hal_dev->clkGateID);

    rt_completion_init(&spi->done);
    rt_sem_init(&spi->sem_lock, "sem_lock", 1, RT_IPC_FLAG_FIFO);

    if (rt_mutex_init(&spi->spi_lock, "spi_lock", RT_IPC_FLAG_FIFO) != RT_EOK)
        RT_ASSERT(0);

    spi->bus.parent.user_data = spi;

    ret = rt_spi_bus_register(&spi->bus, bus_name, &rockchip_spi_ops);
    if (!ret)
    {
        dev_name = rt_malloc(20);
        rt_memset(dev_name, 0, 20);
        strcpy(dev_name, bus_name);
        strcat(dev_name, "_0");
        rt_spi_bus_attach_device(&spi->rt_spi_dev[0], dev_name, bus_name, (void *)&spi->rt_spi_cs[0]);
        rt_memset(dev_name, 0, 20);
        strcpy(dev_name, bus_name);
        strcat(dev_name, "_1");
        rt_spi_bus_attach_device(&spi->rt_spi_dev[1], dev_name, bus_name, (void *)&spi->rt_spi_cs[1]);
        rt_free(dev_name);
    }
#ifdef RT_USING_SPI_PM_PERFORMANCE
    clk_enable(spi->pclk_gate);
    clk_enable(spi->sclk_gate);
#endif

    return ret;
}

#define DEFINE_ROCKCHIP_SPI(ID)                                                                       \
                                                                                                      \
static void rockchip_spi##ID##_irq(int irq, void *param);                                             \
                                                                                                      \
static struct rockchip_spi s_spi##ID =                                                                \
{                                                                                                     \
    .hal_dev = &g_spi##ID##Dev,                                                                         \
    .isr = (void *)&rockchip_spi##ID##_irq,                                                           \
    .rt_spi_cs[0].pin = ROCKCHIP_SPI_CS_0,                                                            \
    .rt_spi_cs[1].pin = ROCKCHIP_SPI_CS_1,                                                            \
};                                                                                                    \
                                                                                                      \
static void rockchip_spi##ID##_irq(int irq, void *param)                                              \
{                                                                                                     \
    rockchip_spi_irq(&s_spi##ID);                                                                     \
}                                                                                                     \

#ifdef RT_USING_SPI0
DEFINE_ROCKCHIP_SPI(0)
#endif

#ifdef RT_USING_SPI1
DEFINE_ROCKCHIP_SPI(1)
#endif

#ifdef RT_USING_SPI2
DEFINE_ROCKCHIP_SPI(2)
#endif

int rockchip_rt_hw_spi_init(void)
{
#ifdef RT_USING_SPI0
    rockchip_spi_probe(&s_spi0, "spi0");
#endif

#ifdef RT_USING_SPI1
    rockchip_spi_probe(&s_spi1, "spi1");
#endif

#ifdef RT_USING_SPI2
    rockchip_spi_probe(&s_spi2, "spi2");
#endif

#ifdef RT_USING_SPI_PM_PERFORMANCE
    pm_runtime_request(PM_RUNTIME_ID_SPI);
#endif

    return 0;
}

INIT_PREV_EXPORT(rockchip_rt_hw_spi_init);
/** @} */  // SPI_Public_Functions

#endif
/** @} */

/** @} */
