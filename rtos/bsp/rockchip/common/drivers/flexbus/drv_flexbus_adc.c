/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_flexbus_adc.c
  * @author  Wesley Yao
  * @version V1.0
  * @date    19-July-2024
  * @brief   flexbus adc mode driver
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rthw.h>

#if defined(RT_USING_FLEXBUS) && defined(RT_USING_FLEXBUS_ADC)
#include <rtdbg.h>
#include <rtdef.h>
#include <rtthread.h>
#include "dma.h"
#include "drv_flexbus.h"
#include "hal_base.h"
#include "hal_bsp.h"

#if defined(RT_USING_CRU)
#include "drv_clock.h"
#endif

struct flexbus_adc_dev
{
    struct rt_adc_device dev;
    const struct HAL_FLEXBUS_ADC_DEV *hal_dev;
    struct FLEXBUS_ADC_HANDLE instance;
    struct rt_completion completion;
    struct rt_mutex lock;
};

#define FLEXBUS_ADC_NAME "fb_adc"
#define FLEXBUS_ADC_TIMEOUT rt_tick_from_millisecond(100)
#define FLEXBUS_ADC_REPEAT 0x40

static struct flexbus_adc_dev flexbus_adc =
{
    .hal_dev = &g_flexbusADCDev,
    .instance =
    {
        .slaveMode = 1,
        .cPol = 1,
        .cPha = 1,
        .freeSclk = 1,
        .cutOff = 0,
        .dfs = 16,
        .rate = 100000000,
    },
};

static void flexbus_adc_isr(struct flexbus_dev *flexbus, rt_uint32_t isr)
{
    struct flexbus_adc_dev *flexbus_adc = flexbus->fb1_data;
    struct FLEXBUS_ADC_HANDLE *pFBADC = &flexbus_adc->instance;
    HAL_Status ret;

    ret = HAL_FLEXBUS_ADC_Isr(pFBADC, isr);

    if (pFBADC->continueMode)
    {
        if (ret)
        {
            HAL_FLEXBUS_ADC_ReadDisable(pFBADC);
        }
        pFBADC->continueMode = 0;
        return;
    }

    rt_completion_done(&flexbus_adc->completion);
}

static rt_err_t flexbus_adc_read_raw(struct rt_adc_device *device, rt_uint32_t channel,
                                     rt_uint32_t *value)
{
    struct flexbus_adc_dev *flexbus_adc = device->parent.user_data;
    struct FLEXBUS_ADC_HANDLE *pFBADC = &flexbus_adc->instance;
    rt_uint16_t *pBuf;
    rt_uint32_t val_mask;
    rt_err_t ret;

    rt_mutex_take(&flexbus_adc->lock, RT_WAITING_FOREVER);
    if (HAL_FLEXBUS_ADC_QueryStatus(pFBADC))
    {
        LOG_E("flexbus adc is busy!\n");
        ret = -RT_EBUSY;
        goto err_mutex_release;
    }

    ret = HAL_FLEXBUS_ADC_ReadBlockEnable(pFBADC, pFBADC->pDstBuf0, pFBADC->dstBufLen,
                                          HAL_FLEXBUS_MIN_RX_NUM_OF_DFS_MIN);
    if (ret)
    {
        goto err_disable;
    }

    ret = rt_completion_wait(&flexbus_adc->completion, FLEXBUS_ADC_TIMEOUT);
    if (ret != RT_EOK)
    {
        LOG_E("%s timeout!\n", __func__);
        goto err_disable;
    }
    if (pFBADC->result != FLEXBUS_ADC_DONE)
    {
        goto err_disable;
    }

    rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, pFBADC->pDstBuf0,
                         HAL_DIV_ROUND_UP(HAL_FLEXBUS_MIN_RX_NUM_OF_DFS_MIN * pFBADC->dfs, 8));
    switch (pFBADC->dfs)
    {
    case 4:
        val_mask = 0xf;
        break;
    case 8:
        val_mask = 0xff;
        break;
    case 16:
    default:
        val_mask = 0xffff;
        break;
    }
    pBuf = pFBADC->pDstBuf0;
    *value = pBuf[0] & val_mask;

err_disable:
    HAL_FLEXBUS_ADC_ReadDisable(pFBADC);
err_mutex_release:
    rt_mutex_release(&flexbus_adc->lock);

    return ret;
}

static const struct rt_adc_ops flexbus_adc_ops =
{
    .convert = flexbus_adc_read_raw,
};

static int rockchip_flexbus_adc_init(void)
{
    struct flexbus_dev *flexbus;
    struct FLEXBUS_HANDLE *pFB;
    struct FLEXBUS_ADC_HANDLE *pFBADC;

    flexbus = (struct flexbus_dev *)rt_device_find("flexbus");
    if (flexbus == RT_NULL)
    {
        LOG_E("%s failed! Can't find flexbus device!\n", __func__);
        return -RT_ERROR;
    }

    pFB = &flexbus->instance;
    pFBADC = &flexbus_adc.instance;
    if (pFB->opMode1 != FLEXBUS1_OPMODE_ADC)
    {
        LOG_E("%s failed! opMode1 mismatch!\n", __func__);
        return -RT_EINVAL;
    }

#if defined(RT_USING_CRU)
    if (pFBADC->slaveMode)
    {
        if (flexbus_adc.hal_dev->refclkID != CLK_INVALID)
        {
            clk_set_rate(flexbus_adc.hal_dev->refclkID,
                         HAL_MIN(pFBADC->rate, HAL_FLEXBUS_MAX_RX_RATE / 2));
        }
        else
        {
            LOG_W("FLEXBUS_ADC has no refclkID in slaveMode. Please make sure the ADC device has clock source.\n");
        }
    }
    else
    {
        clk_set_rate(flexbus->hal_dev->rxclkID, HAL_MIN(pFBADC->rate * 2, HAL_FLEXBUS_MAX_RX_RATE));
    }
#endif

    if (HAL_FLEXBUS_ADC_Init(pFBADC, pFB) != HAL_OK)
    {
        LOG_E("%s failed! Can't init flexbus adc!\n", __func__);
        return -RT_ERROR;
    }

    pFBADC->dstBufLen = HAL_DIV_ROUND_UP(FLEXBUS_ADC_REPEAT * pFBADC->dfs, 8);
    pFBADC->dstBufLen = ROUND_UP(pFBADC->dstBufLen, 0x40);
    if (pFBADC->dstBufLen < 0x40 || pFBADC->dstBufLen > 0x0fffffc0)
    {
        LOG_E("%s failed! BufLen should >= 0x40 and <= 0x0fffffc0!\n", __func__);
        return -RT_EINVAL;
    }

    pFBADC->pDstBuf0 = rt_dma_malloc(pFBADC->dstBufLen);
    if (pFBADC->pDstBuf0 == RT_NULL)
    {
        LOG_E("%s failed! malloc pDstBuf0 0x%x failed!\n", pFBADC->dstBufLen);
        return -RT_ENOMEM;
    }
    rt_memset((void *)pFBADC->pDstBuf0, 0, pFBADC->dstBufLen);
    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, pFBADC->pDstBuf0, pFBADC->dstBufLen);

    rt_completion_init(&flexbus_adc.completion);
    if (rt_mutex_init(&flexbus_adc.lock, "fb_adc_lock", RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        RT_ASSERT(0);
    }

    flexbus->fb1_data = &flexbus_adc;
    flexbus->fb1_isr = flexbus_adc_isr;

    if (rt_hw_adc_register(&flexbus_adc.dev, FLEXBUS_ADC_NAME, &flexbus_adc_ops, &flexbus_adc) ==
            RT_EOK)
    {
        LOG_D("%s success.\n", __func__);
    }
    else
    {
        LOG_E("%s failed! register failed!\n", __func__);
        return -RT_ERROR;
    }

    return RT_EOK;
}

INIT_DEVICE_EXPORT(rockchip_flexbus_adc_init);

#endif
