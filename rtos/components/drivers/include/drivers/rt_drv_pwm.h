/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-07     aozima       the first version
 */

#ifndef __DRV_PWM_H_INCLUDE__
#define __DRV_PWM_H_INCLUDE__

#include <rtthread.h>

#define PWM_CMD_ENABLE      (RT_DEVICE_CTRL_BASE(PWM) + 0)
#define PWM_CMD_DISABLE     (RT_DEVICE_CTRL_BASE(PWM) + 1)
#define PWM_CMD_SET         (RT_DEVICE_CTRL_BASE(PWM) + 2)
#define PWM_CMD_GET         (RT_DEVICE_CTRL_BASE(PWM) + 3)
#define PWMN_CMD_ENABLE     (RT_DEVICE_CTRL_BASE(PWM) + 4)
#define PWMN_CMD_DISABLE    (RT_DEVICE_CTRL_BASE(PWM) + 5)
#define PWM_CMD_SET_PERIOD  (RT_DEVICE_CTRL_BASE(PWM) + 6)
#define PWM_CMD_SET_PULSE   (RT_DEVICE_CTRL_BASE(PWM) + 7)
#define PWM_CMD_SET_ONESHOT (RT_DEVICE_CTRL_BASE(PWM) + 8)
#define PWM_CMD_SET_OFFSET  (RT_DEVICE_CTRL_BASE(PWM) + 9)
#define PWM_CMD_SET_CAPTURE (RT_DEVICE_CTRL_BASE(PWM) + 10)
#define PWM_CMD_LOCK        (RT_DEVICE_CTRL_BASE(PWM) + 11)
#define PWM_CMD_UNLOCK      (RT_DEVICE_CTRL_BASE(PWM) + 12)
#define PWM_CMD_INT_ENABLE  (RT_DEVICE_CTRL_BASE(PWM) + 13)
#define PWM_CMD_INT_DISABLE (RT_DEVICE_CTRL_BASE(PWM) + 14)
#define PWM_CMD_CNT_ENABLE  (RT_DEVICE_CTRL_BASE(PWM) + 15)
#define PWM_CMD_CNT_DISABLE (RT_DEVICE_CTRL_BASE(PWM) + 16)
#define PWM_CMD_GET_CNT_RES (RT_DEVICE_CTRL_BASE(PWM) + 17)
#define PWM_CMD_SET_FREQ    (RT_DEVICE_CTRL_BASE(PWM) + 18)

#define rt_pwm_set(device, channel, period, pulse) rt_pwm_set_internal(device, channel, period, pulse, 0, PWM_UNALIGNED)

enum rt_pwm_aligned_mode
{
    PWM_LEFT_ALIGNED = 1,
    PWM_CENTER_ALIGNED,
    PWM_UNALIGNED,
};

struct rt_pwm_configuration
{
    rt_uint32_t channel; /* 1-n or 0-n, which depends on specific MCU requirements */
    rt_uint32_t period;  /* unit:ns 1ns~4.29s:1Ghz~0.23hz */
    rt_uint32_t pulse;   /* unit:ns (pulse<=period) */
    rt_uint32_t count;   /* (count+1) repeated effective periods */
    rt_uint32_t offset;  /* unit:ns 0ns<=offset<=(period-duty) */
    rt_uint32_t mask;    /* channel mask for lock/unlock, such as 0x5 indicates channel0 and channel2 */

    rt_bool_t polarity;  /* polarity inverted or not */
    /*
     * RT_TRUE  : The channel of pwm is complememtary.
     * RT_FALSE : The channel of pwm is nomal.
    */
    rt_bool_t  complementary;

    rt_uint32_t delay;    /* unit:ms, which indicates timer val in freqency meter mode */
    rt_uint32_t freqency; /* unit:hz, which indicates freqency meter result */

    rt_uint64_t counter_res; /* The number of effective waveforms */

    enum rt_pwm_aligned_mode aligned; /* aligned mode */
};

struct rt_device_pwm;
struct rt_pwm_ops
{
    rt_err_t (*control)(struct rt_device_pwm *device, int cmd, void *arg);
};

struct rt_device_pwm
{
    struct rt_device parent;
    const struct rt_pwm_ops *ops;
};

rt_err_t rt_device_pwm_register(struct rt_device_pwm *device, const char *name, const struct rt_pwm_ops *ops, const void *user_data);

rt_err_t rt_pwm_enable(struct rt_device_pwm *device, int channel);
rt_err_t rt_pwm_disable(struct rt_device_pwm *device, int channel);
rt_err_t rt_pwm_set_internal(struct rt_device_pwm *device, int channel, rt_uint32_t period, rt_uint32_t pulse, rt_uint32_t polarity, enum rt_pwm_aligned_mode aligned);
rt_err_t rt_pwm_set_period(struct rt_device_pwm *device, int channel, rt_uint32_t period);
rt_err_t rt_pwm_set_pulse(struct rt_device_pwm *device, int channel, rt_uint32_t pulse);
rt_err_t rt_pwm_set_offset(struct rt_device_pwm *device, int channel, rt_uint32_t offset);
rt_err_t rt_pwm_set_oneshot(struct rt_device_pwm *device, int channel, rt_uint32_t count);
rt_err_t rt_pwm_set_capture(struct rt_device_pwm *device, int channel);
rt_err_t rt_pwm_lock(struct rt_device_pwm *device, rt_uint32_t channel_mask);
rt_err_t rt_pwm_unlock(struct rt_device_pwm *device, rt_uint8_t channel_mask);
rt_err_t rt_pwm_int_enable(struct rt_device_pwm *device, int channel);
rt_err_t rt_pwm_int_disable(struct rt_device_pwm *device, int channel);
rt_err_t rt_pwm_counter_enable(struct rt_device_pwm *device, int channel);
rt_err_t rt_pwm_counter_disable(struct rt_device_pwm *device, int channel);
rt_err_t rt_pwm_counter_get_result(struct rt_device_pwm *device, int channel, rt_uint64_t *cnt_res);
rt_err_t rt_pwm_set_freqency_meter(struct rt_device_pwm *device, int channel, rt_uint32_t delay_ms, rt_uint32_t *freq);

#endif /* __DRV_PWM_H_INCLUDE__ */
