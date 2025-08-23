/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_dsmc_host.c
  * @version V1.0
  * @brief   dsmc_host controller interface
  *
  * Change Logs:
  * Date           Author          Notes
  * 2024-09-12     Zhihuan He   the first version
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup DSMC_HOST
 *  @{
 */

/** @defgroup DSMC_HOST_How_To_Use How To Use
 *  @{

    DSMC_HOST is a interface for Hyper Psram, Xccela Psram and local bus interface.
    It needs to be combined with the corresponding driver layer to
    complete the transmission of the protocol.

 - deivice Layer(drv_dsmc_host)
 - DSMC_HOST controller Layer DRV_DSMC_HOST
 - DSMC_HOST lower Layer HAL_DSMC_HOST

 @} */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "hal_bsp.h"

#ifdef RT_USING_DSMC_HOST

#include "dma.h"
#include "drv_clock.h"
#include "drv_dsmc_host.h"

/********************* Private MACRO Definition ******************************/
/** @defgroup DSMC_HOST_Private_Macro Private Macro
 *  @{
 */

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(ar)              (sizeof(ar)/sizeof(ar[0]))
#endif

#define RXBUSY (1 << 0)
#define TXBUSY (1 << 1)

/** @} */  // DSMC_HOST_Private_Macro

/********************* Private Structure Definition **************************/
/** @defgroup DSMC_HOST_Private_Structure Private Structure
 *  @{
 */

/** @} */  // DSMC_HOST_Private_Structure

/********************* Private Variable Definition ***************************/
/** @defgroup DSMC_HOST_Variable_Macro Variable Macro
 *  @{
 */

/** @} */  // DSMC_HOST_Variable_Macro

/********************* Private Function Definition ***************************/
/** @defgroup DSMC_HOST_Private_Function Private Function
 *  @{
 */

static rt_err_t DSMC_HOST_ReadData(struct rockchip_rt_dsmc_host *dsmc_host, uint32_t cs,
                                   uint32_t region, unsigned long addr_offset, uint32_t *data)
{
    struct DSMC_MAP *map = &dsmc_host->host->dsmcHostDev.ChipSelMap[cs].regionMap[region];

    *data = *((volatile uint32_t *)(map->phys + addr_offset));

    return RT_EOK;
}

static rt_err_t DSMC_HOST_WriteData(struct rockchip_rt_dsmc_host *dsmc_host, uint32_t cs,
                                    uint32_t region, unsigned long addr_offset, uint32_t val)
{
    struct DSMC_MAP *map = &dsmc_host->host->dsmcHostDev.ChipSelMap[cs].regionMap[region];

    *((volatile uint32_t *)(map->phys + addr_offset)) = val;

    return RT_EOK;
}

static void rockchip_dsmc_host_lb_dma_txcb(void *data)
{
    struct rockchip_rt_dsmc_host *dsmc_host = data;

    dsmc_host->host->dmaReq[dsmc_host->ops_dma_tx_req].status &= ~TXBUSY;

    HAL_DSMC_HOST_DisableDmaRequest(dsmc_host->host, dsmc_host->ops_cs);
    HAL_DSMC_HOST_InterruptUnmask(dsmc_host->host, dsmc_host->ops_cs);
}

static void rockchip_dsmc_host_lb_dma_rxcb(void *data)
{
    struct rockchip_rt_dsmc_host *dsmc_host = data;

    dsmc_host->host->dmaReq[dsmc_host->ops_dma_rx_req].status &= ~RXBUSY;

    HAL_DSMC_HOST_DisableDmaRequest(dsmc_host->host, dsmc_host->ops_cs);
    HAL_DSMC_HOST_InterruptUnmask(dsmc_host->host, dsmc_host->ops_cs);
}

static rt_err_t DSMC_HOST_CopyFrom(struct rockchip_rt_dsmc_host *dsmc_host, uint32_t cs,
                                   uint32_t region, uint32_t from, uint32_t dst_phys, size_t size)
{
    rt_err_t ret = RT_EOK;
    struct HAL_DSMC_DMA_DATA *dmaReq;
    struct HAL_DSMC_HOST *host = dsmc_host->host;
    struct DSMC_MAP *map = &dsmc_host->host->dsmcHostDev.ChipSelMap[cs].regionMap[region];
    struct DSMC_CONFIG_CS *ChipSelCfg = &dsmc_host->host->dsmcHostDev.ChipSelCfg[cs];

    if (!dsmc_host->dma)
    {
        rt_kprintf("DSMC_HOST: copy_from: no dma, enable dma firstly!\n");
        return -RT_EINVAL;
    }

    if (!(host->dmaReq[0].status & (RXBUSY | TXBUSY)))
    {
        dsmc_host->ops_dma_rx_req = 0;
    }
    else if (!(host->dmaReq[1].status & (RXBUSY | TXBUSY)))
    {
        dsmc_host->ops_dma_rx_req = 1;
    }
    else
    {
        rt_kprintf("DSMC_HOST: copy_from: the transfer is busy!\n");
        return -RT_EBUSY;
    }
    dmaReq = &host->dmaReq[dsmc_host->ops_dma_rx_req];

    rt_memset(&dsmc_host->xfer, 0x0, sizeof(struct rt_dma_transfer));

    dsmc_host->xfer.direction = DMA_DEV_TO_MEM;
    dsmc_host->xfer.dma_req_num = dmaReq->dmaReqCh;
    dsmc_host->xfer.src_addr = map->phys + from;
    dsmc_host->xfer.src_addr_width = dmaReq->brstSize;
    dsmc_host->xfer.src_maxburst = dmaReq->brstLen;

    dsmc_host->xfer.dst_addr = dst_phys;
    dsmc_host->xfer.dst_addr_width = dmaReq->brstSize;
    dsmc_host->xfer.dst_maxburst = dmaReq->brstLen;

    dsmc_host->xfer.len = size;
    dsmc_host->ops_cs = cs;

    dsmc_host->xfer.callback = rockchip_dsmc_host_lb_dma_rxcb;
    dsmc_host->xfer.cparam = dsmc_host;

    ret = rt_device_control(dsmc_host->dma, RT_DEVICE_CTRL_DMA_REQUEST_CHANNEL,
                            &dsmc_host->xfer);
    if (ret)
        return ret;

    ret = rt_device_control(dsmc_host->dma, RT_DEVICE_CTRL_DMA_SINGLE_PREPARE,
                            &dsmc_host->xfer);
    if (ret)
        goto dma_release;

    ret = rt_device_control(dsmc_host->dma, RT_DEVICE_CTRL_DMA_START,
                            &dsmc_host->xfer);
    if (ret)
        goto dma_release;

    dmaReq->status |= RXBUSY;

    HAL_DSMC_HOST_InterruptMask(host, ChipSelCfg->intEn);
    HAL_DSMC_HOST_EnableDmaRequest(host, dsmc_host->xfer.len, dsmc_host->ops_dma_rx_req, cs);
    HAL_DSMC_HOST_LbDmaTrigger(host, cs);

    return ret;

dma_release:
    rt_device_control(dsmc_host->dma, RT_DEVICE_CTRL_DMA_RELEASE_CHANNEL,
                      &dsmc_host->xfer);

    return ret;
}
static rt_err_t DSMC_HOST_CopyFromState(struct rockchip_rt_dsmc_host *dsmc_host)
{
    struct HAL_DSMC_DMA_DATA *dmaReq = &dsmc_host->host->dmaReq[dsmc_host->ops_dma_rx_req];

    if (dmaReq->status & RXBUSY)
        return -RT_EBUSY;
    else
        return 0;
}

static rt_err_t DSMC_HOST_CopyTo(struct rockchip_rt_dsmc_host *dsmc_host, uint32_t cs,
                                 uint32_t region, uint32_t src_phys, uint32_t to, size_t size)
{
    rt_err_t ret = RT_EOK;
    struct HAL_DSMC_DMA_DATA *dmaReq;
    struct HAL_DSMC_HOST *host = dsmc_host->host;
    struct DSMC_MAP *map = &dsmc_host->host->dsmcHostDev.ChipSelMap[cs].regionMap[region];
    struct DSMC_CONFIG_CS *ChipSelCfg = &dsmc_host->host->dsmcHostDev.ChipSelCfg[cs];

    if (!dsmc_host->dma)
    {
        rt_kprintf("DSMC_HOST: copy_to: no dma, enable dma firstly!\n");
        return -RT_EINVAL;
    }

    if (!(host->dmaReq[0].status & (RXBUSY | TXBUSY)))
    {
        dsmc_host->ops_dma_tx_req = 0;
    }
    else if (!(host->dmaReq[1].status & (RXBUSY | TXBUSY)))
    {
        dsmc_host->ops_dma_tx_req = 1;
    }
    else
    {
        rt_kprintf("DSMC_HOST: copy_to: the transfer is busy!\n");
        return -RT_EBUSY;
    }
    rt_memset(&dsmc_host->xfer, 0x0, sizeof(struct rt_dma_transfer));

    dmaReq = &host->dmaReq[dsmc_host->ops_dma_tx_req];

    dsmc_host->xfer.direction = DMA_MEM_TO_DEV;
    dsmc_host->xfer.dma_req_num = dmaReq->dmaReqCh;
    dsmc_host->xfer.src_addr = src_phys;
    dsmc_host->xfer.src_addr_width = dmaReq->brstSize;
    dsmc_host->xfer.src_maxburst = dmaReq->brstLen;

    dsmc_host->xfer.dst_addr = map->phys + to;
    dsmc_host->xfer.dst_addr_width = dmaReq->brstSize;
    dsmc_host->xfer.dst_maxburst = dmaReq->brstLen;

    dsmc_host->xfer.len = size;
    dsmc_host->ops_cs = cs;

    dsmc_host->xfer.callback = rockchip_dsmc_host_lb_dma_txcb;
    dsmc_host->xfer.cparam = dsmc_host;

    ret = rt_device_control(dsmc_host->dma, RT_DEVICE_CTRL_DMA_REQUEST_CHANNEL,
                            &dsmc_host->xfer);
    if (ret)
        return ret;

    ret = rt_device_control(dsmc_host->dma, RT_DEVICE_CTRL_DMA_SINGLE_PREPARE,
                            &dsmc_host->xfer);
    if (ret)
        goto dma_release;

    ret = rt_device_control(dsmc_host->dma, RT_DEVICE_CTRL_DMA_START,
                            &dsmc_host->xfer);
    if (ret)
        goto dma_release;

    dmaReq->status |= TXBUSY;

    HAL_DSMC_HOST_InterruptMask(host, ChipSelCfg->intEn);
    HAL_DSMC_HOST_EnableDmaRequest(host, dsmc_host->xfer.len, dsmc_host->ops_dma_tx_req, cs);
    HAL_DSMC_HOST_LbDmaTrigger(host, cs);

    return ret;

dma_release:
    rt_device_control(dsmc_host->dma, RT_DEVICE_CTRL_DMA_RELEASE_CHANNEL,
                      &dsmc_host->xfer);

    return ret;
}

static rt_err_t DSMC_HOST_CopyToState(struct rockchip_rt_dsmc_host *dsmc_host)
{
    struct HAL_DSMC_DMA_DATA *dmaReq = &dsmc_host->host->dmaReq[dsmc_host->ops_dma_tx_req];

    if (dmaReq->status & TXBUSY)
        return -RT_EBUSY;
    else
        return 0;
}

static struct dsmc_host_ops rockchip_dsmc_ops =
{
    .read = DSMC_HOST_ReadData,
    .write = DSMC_HOST_WriteData,
    .copy_from = DSMC_HOST_CopyFrom,
    .copy_from_state = DSMC_HOST_CopyFromState,
    .copy_to = DSMC_HOST_CopyTo,
    .copy_to_state = DSMC_HOST_CopyToState,
};

static rt_err_t rockchip_dsmc_host_dma_configure(struct rockchip_rt_dsmc_host *dsmc_host)
{
#ifdef RT_USING_DMA
    if (dsmc_host->dma == NULL && !dsmc_host->no_dma)
    {
        dsmc_host->dma = rt_dma_get((uint32_t)(dsmc_host->host->dmaReq[0].dmac));
        if (dsmc_host->dma == NULL)
        {
            rt_kprintf("DSMC_HOST: %s get dma failed!\n", __func__);
            dsmc_host->no_dma = true;
        }
    }
#endif
    return RT_EOK;
}

static void dsmc_host_data_2_dsmc(uint32_t *data)
{
    uint32_t byte0, byte3;

    byte0 = (*data >> 0) & 0xFF;
    byte3 = (*data >> 24) & 0xFF;

    *data &= 0x00FFFF00;

    *data |= (byte3 << 0);
    *data |= (byte0 << 24);
}

static rt_err_t dsmc_host_dll_training_method(struct rockchip_rt_dsmc_host *dsmc_host, uint32_t cs,
        uint32_t byte, uint32_t dll_num)
{
    uint32_t i, j;
    uint32_t data;
    uint32_t size = 0x100;
    struct dsmc_host_ops *ops = dsmc_host->ops;
    struct HAL_DSMC_HOST *host = dsmc_host->host;
    struct HAL_DSMC_HOST_DEV *HostDev = &host->dsmcHostDev;
    struct DSMC_CONFIG_CS *cfg = &HostDev->ChipSelCfg[cs];
    uint32_t pattern[] = {0x5aa5f00f, 0xffff0000};
    uint32_t mask;

    if (cfg->ioWidth == MCR_IOWIDTH_X8)
    {
        mask = 0xffffffff;
    }
    else
    {
        if (byte == 0)
            mask = 0x0000ffff;
        else
            mask = 0xffff0000;
    }

    HAL_DSMC_HOST_DllSetting(host, cs, byte, dll_num);
    for (j = 0; j < ARRAY_SIZE(pattern); j++)
    {
        dsmc_host_data_2_dsmc(&pattern[j]);
        for (i = 0; i < size; i += 4)
            ops->write(dsmc_host, cs, 0, i, (pattern[j] + i) & mask);

#ifdef RT_USING_CACHE
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH,
                             (void *)HostDev->ChipSelMap[cs].regionMap[0].phys,
                             size);
#endif

        for (i = 0; i < size; i += 4)
        {
            ops->read(dsmc_host, cs, 0, i, &data);
            if (data != ((pattern[j] + i) & mask))
                return -RT_EINVAL;
        }
    }
    return RT_EOK;
}

static rt_err_t rockchip_dsmc_host_dll_training(struct rockchip_rt_dsmc_host *dsmc_host)
{
    rt_err_t ret;
    uint32_t cs, byte, dir;
    uint32_t dll_max = 0xff, dll_min = 0;
    int dll, dll_step = 10;
    struct DSMC_CONFIG_CS *cfg;
    struct HAL_DSMC_HOST *host = dsmc_host->host;

    for (cs = 0; cs < DSMC_MAX_SLAVE_NUM; cs++)
    {
        cfg = &host->dsmcHostDev.ChipSelCfg[cs];
        if (cfg->deviceType == DSMC_UNKNOWN_DEVICE)
            continue;
        for (byte = MCR_IOWIDTH_X8; byte <= cfg->ioWidth; byte++)
        {
            dir = 0;
            dll = cfg->dqsDLL.rxDLLDQS[byte];
            while ((dll >= 0x0 && dll <= 0xff))
            {
                ret = dsmc_host_dll_training_method(dsmc_host, cs, byte, dll);
                if (ret)
                {
                    if (dir)
                        dll_max = dll - dll_step;
                    else
                        dll_min = dll + dll_step;
                    dll = cfg->dqsDLL.rxDLLDQS[byte];
                    dir++;
                }
                else if (((dll + dll_step) > 0xff) || ((dll - dll_step) < 0))
                {
                    if (dir)
                        dll_max = dll;
                    else
                        dll_min = dll;
                    dll = cfg->dqsDLL.rxDLLDQS[byte];
                    dir++;
                }
                if (dir > 1)
                    break;
                if (dir)
                    dll += dll_step;
                else
                    dll -= dll_step;
            }
            dll = (dll_max + dll_min) / 2;
            if ((dll >= 0xff) || (dll <= 0))
            {
                rt_kprintf("DSMC_HOST: cs%d byte%d dll training error(0x%x)\n",
                           cs, byte, dll);
                return -RT_EINVAL;
            }
            rt_kprintf("DSMC_HOST: cs%d byte%d dll delay line result 0x%x\n", cs, byte, dll);
            HAL_DSMC_HOST_DllSetting(host, cs, byte, dll);
        }
    }

    return RT_EOK;
}

/** @} */  // DSMC_HOST_Private_Function

/********************* Public Function Definition ****************************/
/** @defgroup DSMC_HOST_Public_Functions Public Functions
 *  @{
 */
/**
 * @brief  Init DSMC_HOST framwork and apply to use.
 */
int rockchip_dsmc_host_probe(void)
{
    rt_err_t ret = RT_EOK;
    struct rockchip_rt_dsmc_host *dsmc_host;

    dsmc_host = (struct rockchip_rt_dsmc_host *)rt_malloc(sizeof(struct rockchip_rt_dsmc_host));

    rt_memset(dsmc_host, 0x0, sizeof(struct rockchip_rt_dsmc_host));

    dsmc_host->host = &g_dsmcHostDev;

    if (dsmc_host->host->dsmcHostDev.type == DEV_DSMC_LB)
        dsmc_host_lb_iomux_config();

#ifdef RT_USING_CRU
    ret = clk_set_rate(dsmc_host->host->sclkID, 2 * dsmc_host->host->maxFreq);
    if (ret)
    {
        rt_kprintf("DSMC_HOST: dsmc clk set rate error! ret = 0x%x\n", ret);
        return ret;
    }
    ret = clk_set_rate(dsmc_host->host->sclkRootID, dsmc_host->host->maxFreq);
    if (ret)
    {
        rt_kprintf("DSMC_HOST: dsmc root clk set rate error! ret = 0x%x\n", ret);
        return ret;
    }
#endif

    dsmc_host->ops = &rockchip_dsmc_ops;
    if (HAL_DSMC_HOST_Init(dsmc_host->host) != HAL_OK)
        return RT_ERROR;

    rockchip_dsmc_host_dll_training(dsmc_host);

    rockchip_dsmc_host_dma_configure(dsmc_host);

    rt_device_register(&dsmc_host->parent, "dsmc_host", RT_DEVICE_FLAG_RDWR);

    return ret;
}
INIT_DEVICE_EXPORT(rockchip_dsmc_host_probe);

/** @} */  // DSMC_HOST_Public_Functions

#endif

/** @} */  // DSMC_HOST

/** @} */  // RKBSP_Common_Driver
