/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_flexbus_dac.c
  * @author  Wesley Yao
  * @version V1.0
  * @date    19-July-2024
  * @brief   flexbus dac mode driver
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rthw.h>

#if defined(RT_USING_FLEXBUS) && defined(RT_USING_FLEXBUS_DAC)
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

struct flexbus_dac_dev
{
    struct rt_dac_device dev;
    struct FLEXBUS_DAC_HANDLE instance;
    struct rt_completion completion;
    struct rt_mutex lock;
};

#define FLEXBUS_DAC_NAME "fb_dac"
#define FLEXBUS_DAC_TIMEOUT rt_tick_from_millisecond(100)
#define FLEXBUS_DAC_REPEAT 0x40

static struct flexbus_dac_dev flexbus_dac =
{
    .instance =
    {
        .cPol = 1,
        .cPha = 1,
        .freeSclk = 1,
        .cutOff = 0,
        .dfs = 16,
        .rate = 100000000,
    },
};

static void flexbus_dac_isr(struct flexbus_dev *flexbus, rt_uint32_t isr)
{
    struct flexbus_dac_dev *flexbus_dac = flexbus->fb0_data;
    struct FLEXBUS_DAC_HANDLE *pFBDAC = &flexbus_dac->instance;
    HAL_Status ret;

    ret = HAL_FLEXBUS_DAC_Isr(pFBDAC, isr);

    if (pFBDAC->continueMode)
    {
        if (ret)
        {
            HAL_FLEXBUS_DAC_WriteDisable(pFBDAC);
        }
        pFBDAC->continueMode = 0;
        return;
    }

    rt_completion_done(&flexbus_dac->completion);
}

static rt_err_t flexbus_dac_write_raw(struct rt_dac_device *device, rt_uint32_t channel,
                                      rt_uint32_t *value)
{
    struct flexbus_dac_dev *flexbus_dac = device->parent.user_data;
    struct FLEXBUS_DAC_HANDLE *pFBDAC = &flexbus_dac->instance;
    rt_uint16_t *pBuf;
    rt_err_t ret;

    rt_mutex_take(&flexbus_dac->lock, RT_WAITING_FOREVER);
    if (HAL_FLEXBUS_DAC_QueryStatus(pFBDAC))
    {
        LOG_E("flexbus dac is busy!\n");
        ret = -RT_EBUSY;
        goto err_mutex_release;
    }

    pBuf = pFBDAC->pSrcBuf0;
    switch (pFBDAC->dfs)
    {
    case 4:
        pBuf[0] = *value & 0xf;
        break;
    case 8:
        pBuf[0] = *value & 0xff;
        break;
    case 16:
    default:
        pBuf[0] = *value & 0xffff;
        break;
    }
    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, pFBDAC->pSrcBuf0,
                         HAL_DIV_ROUND_UP(HAL_FLEXBUS_MIN_TX_NUM_OF_DFS_MIN * pFBDAC->dfs, 8));

    ret = HAL_FLEXBUS_DAC_WriteBlockEnable(pFBDAC, pFBDAC->pSrcBuf0, pFBDAC->srcBufLen,
                                           HAL_FLEXBUS_MIN_TX_NUM_OF_DFS_MIN);
    if (ret)
    {
        goto err_disable;
    }

    ret = rt_completion_wait(&flexbus_dac->completion, FLEXBUS_DAC_TIMEOUT);
    if (ret != RT_EOK)
    {
        LOG_E("%s timeout!\n", __func__);
    }

err_disable:
    HAL_FLEXBUS_DAC_WriteDisable(pFBDAC);
err_mutex_release:
    rt_mutex_release(&flexbus_dac->lock);

    return ret;
}

static const struct rt_dac_ops flexbus_dac_ops =
{
    .convert = flexbus_dac_write_raw,
};

static int rockchip_flexbus_dac_init(void)
{
    struct flexbus_dev *flexbus;
    struct FLEXBUS_HANDLE *pFB;
    struct FLEXBUS_DAC_HANDLE *pFBDAC;

    flexbus = (struct flexbus_dev *)rt_device_find("flexbus");
    if (flexbus == RT_NULL)
    {
        LOG_E("%s failed! Can't find flexbus device!\n", __func__);
        return -RT_ERROR;
    }

    pFB = &flexbus->instance;
    pFBDAC = &flexbus_dac.instance;
    if (pFB->opMode0 != FLEXBUS0_OPMODE_DAC)
    {
        LOG_E("%s failed! opMode0 mismatch!\n", __func__);
        return -RT_EINVAL;
    }

#if defined(RT_USING_CRU)
    clk_set_rate(flexbus->hal_dev->txclkID, HAL_MIN(pFBDAC->rate * 2, HAL_FLEXBUS_MAX_TX_RATE));
#endif

    if (HAL_FLEXBUS_DAC_Init(pFBDAC, pFB) != HAL_OK)
    {
        LOG_E("%s failed! Can't init flexbus dac!\n", __func__);
        return -RT_ERROR;
    }

    pFBDAC->srcBufLen = HAL_DIV_ROUND_UP(FLEXBUS_DAC_REPEAT * pFBDAC->dfs, 8);
    pFBDAC->srcBufLen = ROUND_UP(pFBDAC->srcBufLen, 0x40);
    if (pFBDAC->srcBufLen < 0x40 || pFBDAC->srcBufLen > 0x0fffffc0)
    {
        LOG_E("%s failed! BufLen should >= 0x40 and <= 0x0fffffc0!\n", __func__);
        return -RT_EINVAL;
    }

    pFBDAC->pSrcBuf0 = rt_dma_malloc(pFBDAC->srcBufLen);
    if (pFBDAC->pSrcBuf0 == RT_NULL)
    {
        LOG_E("%s failed! malloc pSrcBuf0 0x%x failed!\n", pFBDAC->srcBufLen);
        return -RT_ENOMEM;
    }
    rt_memset((void *)pFBDAC->pSrcBuf0, 0, pFBDAC->srcBufLen);
    rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, pFBDAC->pSrcBuf0, pFBDAC->srcBufLen);

    rt_completion_init(&flexbus_dac.completion);
    if (rt_mutex_init(&flexbus_dac.lock, "fb_dac_lock", RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        RT_ASSERT(0);
    }

    flexbus->fb0_data = &flexbus_dac;
    flexbus->fb0_isr = flexbus_dac_isr;

    if (rt_hw_dac_register(&flexbus_dac.dev, FLEXBUS_DAC_NAME, &flexbus_dac_ops, &flexbus_dac) ==
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

INIT_DEVICE_EXPORT(rockchip_flexbus_dac_init);

#endif
