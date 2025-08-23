/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_pwm.c
  * @author  David Wu
  * @version V0.1
  * @date    26-Apri-2019
  * @brief   PWM Driver
  *
  * Change Logs:
  * Date           Author          Notes
  * 2019-02-20     David Wu        first implementation
  * 2023-12-25     Damon Ding      add support pwm v2
  *
  ******************************************************************************
  */

/** @addtogroup RKBSP_Driver_Reference
 *  @{
 */

/** @addtogroup PWM
 *  @{
 */

/** @defgroup PWM_How_To_Use How To Use
 *  @{

  See more information, click [here](https://www.rt-thread.org/document/site/programming-manual/device/pwm/pwm/)

 @} */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_PWM)

#include "hal_bsp.h"
#include "drv_clock.h"
#include "hal_base.h"

#ifdef RT_USING_PWM_REMOTECTL
#include "drv_pwm_remotectl.h"
#endif

#define RK_PWM_DEBUG 0
#if RK_PWM_DEBUG
#define rk_pwm_dbg(dev, fmt, ...) \
do { \
    rt_kprintf("%s:", ((struct rt_device *)dev)->parent.name); \
    rt_kprintf(fmt, ##__VA_ARGS__); \
} while(0)
#else
#define rk_pwm_dbg(dev, fmt, ...) \
do { \
} while(0)
#endif

/* PWM controller driver's private data. */
struct rockchip_pwm
{
    struct rt_device_pwm dev;
    const char *name;
    /* Hal */
    struct PWM_HANDLE instance;
    const struct HAL_PWM_DEV *hal_dev;
    struct clk_gate *clk_gate;
    struct clk_gate *pclk_gate;
    rt_bool_t is_int_enable;

    /* irq handler */
    rt_isr_handler_t irq_handler;
};

static void rockchip_pwm_irq(struct rockchip_pwm *pwm);

#define DEFINE_ROCKCHIP_PWM(ID)      \
static void rockchip_pwm##ID##_irq(int irq, void *param);   \
static struct rockchip_pwm pwm##ID =                        \
{                                                           \
    .hal_dev = &g_pwm##ID##Dev,                             \
    .name = "pwm"#ID,                                       \
    .irq_handler = rockchip_pwm##ID##_irq,                  \
};                                                          \
static void rockchip_pwm##ID##_irq(int irq, void *param)    \
{                                                           \
    rockchip_pwm_irq(&pwm##ID);                             \
}                                                           \

#ifdef RT_USING_PWM0
DEFINE_ROCKCHIP_PWM(0)
#endif

#ifdef RT_USING_PWM1
DEFINE_ROCKCHIP_PWM(1)
#endif

#ifdef RT_USING_PWM2
DEFINE_ROCKCHIP_PWM(2)
#endif

#ifdef RT_USING_PWM3
DEFINE_ROCKCHIP_PWM(3)
#endif

static struct rockchip_pwm *const rockchip_pwm_table[] =
{
#ifdef RT_USING_PWM0
    &pwm0,
#endif
#ifdef RT_USING_PWM1
    &pwm1,
#endif
#ifdef RT_USING_PWM2
    &pwm2,
#endif
#ifdef RT_USING_PWM3
    &pwm3,
#endif
    RT_NULL
};

#if defined(RT_USING_PWM0) || defined(RT_USING_PWM1) || defined(RT_USING_PWM2) || defined(RT_USING_PWM3)
static void rockchip_pwm_irq(struct rockchip_pwm *pwm)
{
    struct PWM_HANDLE *pPWM = RT_NULL;
    int i;

    RT_ASSERT(pwm != RT_NULL);
    pPWM = &pwm->instance;

    rt_interrupt_enter();

#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
    for (i = 0; i < pPWM->channelNum; i++)
    {
        HAL_PWM_ChannelIRQHandler(pPWM, i);
        if (pPWM->pChHandle[i].mode == HAL_PWM_CAPTURE)
        {
            rk_pwm_dbg(&pwm->dev, "%s chanel%d pos cycles = %ld and neg cycles = %ld\n", pwm->name, i,
                       pPWM->pChHandle[i].result.posCycles, pPWM->pChHandle[i].result.negCycles);
#ifdef RT_USING_PWM_REMOTECTL
            remotectl_get_pwm_period(pwm->name, i, pPWM->freq, pPWM->pChHandle[i].result.negCycles, pPWM->pChHandle[i].result.posCycles);
#endif
        }
    }
#else
    HAL_PWM_IRQHandler(pPWM);

    for (i = 0; i < HAL_PWM_NUM_CHANNELS; i++)
    {
        if (pPWM->mode[i] == HAL_PWM_CAPTURE)
            rk_pwm_dbg(&pwm->dev, "%s chanel%d period cycles = %ld\n", pwm->name, i, pPWM->result[i].period);
    }
#endif

    rt_interrupt_leave();
}
#endif

rt_err_t rockchip_pwm_control(struct rt_device_pwm *device, int cmd, void *arg)
{
    struct rt_pwm_configuration *config = (struct rt_pwm_configuration *)arg;
    struct rockchip_pwm *pwm = (struct rockchip_pwm *)device->parent.user_data;
    struct PWM_HANDLE *pPWM = &pwm->instance;
    struct HAL_PWM_CONFIG hal_config;
    rt_err_t ret = RT_EOK;

    rk_pwm_dbg(device, "cmd: %d\n", cmd);

    if (!pwm->is_int_enable)
        clk_enable(pwm->pclk_gate);

    switch (cmd)
    {
    case PWM_CMD_ENABLE:
        clk_enable(pwm->clk_gate);
        ret = HAL_PWM_Enable(pPWM, config->channel, HAL_PWM_CONTINUOUS);
        break;
    case PWM_CMD_DISABLE:
        clk_disable(pwm->clk_gate);
        ret = HAL_PWM_Disable(pPWM, config->channel);
        break;
    case PWM_CMD_SET:
        hal_config.channel = config->channel;
        hal_config.periodNS = config->period;
        hal_config.dutyNS = config->pulse;
        hal_config.polarity = config->polarity;
        hal_config.alignedMode = config->aligned;
        ret = HAL_PWM_SetConfig(pPWM, config->channel, &hal_config);
        break;
    case PWM_CMD_SET_ONESHOT:
        HAL_PWM_SetOneshot(pPWM, config->channel, config->count);
        clk_enable(pwm->clk_gate);
        ret = HAL_PWM_Enable(pPWM, config->channel, HAL_PWM_ONE_SHOT);
        break;
    case PWM_CMD_SET_OFFSET:
        ret = HAL_PWM_SetOutputOffset(pPWM, config->channel, config->offset);
        break;
    case PWM_CMD_SET_CAPTURE:
        clk_enable(pwm->clk_gate);
#ifdef RT_USING_PWM_REMOTECTL
        HAL_PWM_EnableCaptureInt(pPWM, config->channel, HAL_PWM_CAP_HPR_INT);
#else
        HAL_PWM_EnableCaptureInt(pPWM, config->channel, HAL_PWM_CAP_LPR_INT | HAL_PWM_CAP_HPR_INT);
#endif
        ret = HAL_PWM_Enable(pPWM, config->channel, HAL_PWM_CAPTURE);
        break;
    case PWM_CMD_LOCK:
#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
        HAL_PWM_GlobalUnlock(pPWM, config->mask);
        ret = HAL_PWM_GlobalLock(pPWM, config->mask);
        HAL_PWM_GlobalDisable(pPWM);
#else
        ret = HAL_PWM_GlobalLock(pPWM, config->mask);
#endif
        break;
    case PWM_CMD_UNLOCK:
#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
        ret = HAL_PWM_GlobalUpdate(pPWM);
        HAL_PWM_GlobalEnable(pPWM);
#else
        ret = HAL_PWM_GlobalUnlock(pPWM, config->mask);
#endif
        break;
    case PWM_CMD_INT_ENABLE:
#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
        rt_hw_interrupt_install(pwm->hal_dev->irqNum[config->channel], pwm->irq_handler, NULL, "0" + config->channel);
        rt_hw_interrupt_umask(pwm->hal_dev->irqNum[config->channel]);
#else
        rt_hw_interrupt_install(pwm->hal_dev->irqNum[0], pwm->irq_handler, NULL, NULL);
        rt_hw_interrupt_umask(pwm->hal_dev->irqNum[0]);
#endif
        pwm->is_int_enable = true;
        break;
    case PWM_CMD_INT_DISABLE:
#if (PWM_MAIN_VERSION(PWM_VERSION_ID) >= 4)
        rt_hw_interrupt_mask(pwm->hal_dev->irqNum[config->channel]);
#else
        rt_hw_interrupt_mask(pwm->hal_dev->irqNum[0]);
#endif
        pwm->is_int_enable = false;
        break;
#if defined(RKMCU_RK2118)
    case PWM_CMD_CNT_ENABLE:
        clk_enable(pwm->clk_gate);
        ret = HAL_PWM_EnableCounter(pPWM, config->channel);
        break;
    case PWM_CMD_CNT_DISABLE:
        ret = HAL_PWM_DisableCounter(pPWM, config->channel);
        HAL_PWM_ClearCounterRes(pPWM, config->channel);
        clk_disable(pwm->clk_gate);
        break;
    case PWM_CMD_GET_CNT_RES:
        ret = HAL_PWM_GetCounterRes(pPWM, config->channel, &config->counter_res);
        break;
    case PWM_CMD_SET_FREQ:
        clk_enable(pwm->clk_gate);
        HAL_PWM_EnableFreqMeter(pPWM, config->channel, config->delay);
        ret = HAL_PWM_GetFreqMeterRes(pPWM, config->channel, config->delay, &config->freqency);
        HAL_PWM_DisableFreqMeter(pPWM, config->channel);
        clk_disable(pwm->clk_gate);
        break;
#endif
    default:
        break;
    }

    if (!pwm->is_int_enable)
        clk_disable(pwm->pclk_gate);

    return ret;
}

static struct rt_pwm_ops rockchip_pwm_ops =
{
    rockchip_pwm_control
};

int rockchip_rt_hw_pwm_init(void)
{
    struct rockchip_pwm *const *pwm;
    rt_uint32_t freq;
    const struct HAL_PWM_DEV *hal_dev;

    for (pwm = rockchip_pwm_table; *pwm != RT_NULL; pwm++)
    {
        hal_dev = (*pwm)->hal_dev;
        (*pwm)->clk_gate = get_clk_gate_from_id(hal_dev->clkGateID);
        (*pwm)->pclk_gate = get_clk_gate_from_id(hal_dev->pclkGateID);

        RT_ASSERT((*pwm)->clk_gate != RT_NULL);
        RT_ASSERT((*pwm)->pclk_gate != RT_NULL);

        clk_enable((*pwm)->pclk_gate);
        /* Get clock rate. */
        freq = clk_get_rate(hal_dev->clkID);
        HAL_PWM_Init(&(*pwm)->instance, hal_dev->pReg, freq);

        clk_disable((*pwm)->pclk_gate);
        /* register pwm */
        rt_device_pwm_register(&(*pwm)->dev, (*pwm)->name, &rockchip_pwm_ops, *pwm);
    }

    return 0;
}

INIT_PREV_EXPORT(rockchip_rt_hw_pwm_init);

#endif

/** @} */  // PWM

/** @} */  // RKBSP_Driver_Reference
