/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-07     aozima       the first version
 * 2022-05-14     Stanley Lwin add pwm function
 * 2022-07-25     liYony       fix complementary outputs and add usage information in finsh
 */

#include <rtdevice.h>

static rt_err_t _pwm_control(rt_device_t dev, int cmd, void *args)
{
    rt_err_t result = RT_EOK;
    struct rt_device_pwm *pwm = (struct rt_device_pwm *)dev;

    if (pwm->ops->control)
    {
        result = pwm->ops->control(pwm, cmd, args);
    }

    return result;
}


/*
pos: channel
void *buffer: rt_uint32_t pulse[size]
size : number of pulse, only set to sizeof(rt_uint32_t).
*/
static rt_size_t _pwm_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_err_t result = RT_EOK;
    struct rt_device_pwm *pwm = (struct rt_device_pwm *)dev;
    rt_uint32_t *pulse = (rt_uint32_t *)buffer;
    struct rt_pwm_configuration configuration = {0};

    configuration.channel = (pos > 0) ? (pos) : (-pos);

    if (pwm->ops->control)
    {
        result = pwm->ops->control(pwm, PWM_CMD_GET,  &configuration);
        if (result != RT_EOK)
        {
            return 0;
        }

        *pulse = configuration.pulse;
    }

    return size;
}

/*
pos: channel
void *buffer: rt_uint32_t pulse[size]
size : number of pulse, only set to sizeof(rt_uint32_t).
*/
static rt_size_t _pwm_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    rt_err_t result = RT_EOK;
    struct rt_device_pwm *pwm = (struct rt_device_pwm *)dev;
    rt_uint32_t *pulse = (rt_uint32_t *)buffer;
    struct rt_pwm_configuration configuration = {0};

    configuration.channel = (pos > 0) ? (pos) : (-pos);

    if (pwm->ops->control)
    {
        result = pwm->ops->control(pwm, PWM_CMD_GET, &configuration);
        if (result != RT_EOK)
        {
            return 0;
        }

        configuration.pulse = *pulse;

        result = pwm->ops->control(pwm, PWM_CMD_SET, &configuration);
        if (result != RT_EOK)
        {
            return 0;
        }
    }

    return size;
}

#ifdef RT_USING_DEVICE_OPS
static const struct rt_device_ops pwm_device_ops =
{
    RT_NULL,
    RT_NULL,
    RT_NULL,
    _pwm_read,
    _pwm_write,
    _pwm_control
};
#endif /* RT_USING_DEVICE_OPS */

rt_err_t rt_device_pwm_register(struct rt_device_pwm *device, const char *name, const struct rt_pwm_ops *ops, const void *user_data)
{
    rt_err_t result = RT_EOK;

    rt_memset(device, 0, sizeof(struct rt_device_pwm));

#ifdef RT_USING_DEVICE_OPS
    device->parent.ops = &pwm_device_ops;
#else
    device->parent.init = RT_NULL;
    device->parent.open = RT_NULL;
    device->parent.close = RT_NULL;
    device->parent.read  = _pwm_read;
    device->parent.write = _pwm_write;
    device->parent.control = _pwm_control;
#endif /* RT_USING_DEVICE_OPS */

    device->parent.type         = RT_Device_Class_PWM;
    device->ops                 = ops;
    device->parent.user_data    = (void *)user_data;

    result = rt_device_register(&device->parent, name, RT_DEVICE_FLAG_RDWR);

    return result;
}

rt_err_t rt_pwm_enable(struct rt_device_pwm *device, int channel)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = (channel > 0) ? (channel) : (-channel);         /* Make it is positive num forever */
    configuration.complementary = (channel > 0) ? (RT_FALSE) : (RT_TRUE);   /* If nagetive, it's complementary */
    result = rt_device_control(&device->parent, PWM_CMD_ENABLE, &configuration);

    return result;
}

rt_err_t rt_pwm_disable(struct rt_device_pwm *device, int channel)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = (channel > 0) ? (channel) : (-channel);         /* Make it is positive num forever */
    configuration.complementary = (channel > 0) ? (RT_FALSE) : (RT_TRUE);   /* If nagetive, it's complementary */
    result = rt_device_control(&device->parent, PWM_CMD_DISABLE, &configuration);

    return result;
}

rt_err_t rt_pwm_set_internal(struct rt_device_pwm *device, int channel, rt_uint32_t period, rt_uint32_t pulse, rt_uint32_t polarity, enum rt_pwm_aligned_mode aligned)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = (channel > 0) ? (channel) : (-channel);
    configuration.period = period;
    configuration.pulse = pulse;
    configuration.polarity = polarity ? RT_TRUE : RT_FALSE;
    configuration.aligned = aligned;
    result = rt_device_control(&device->parent, PWM_CMD_SET, &configuration);

    return result;
}

rt_err_t rt_pwm_set_period(struct rt_device_pwm *device, int channel, rt_uint32_t period)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = (channel > 0) ? (channel) : (-channel);
    configuration.period = period;
    result = rt_device_control(&device->parent, PWM_CMD_SET_PERIOD, &configuration);

    return result;
}

rt_err_t rt_pwm_set_pulse(struct rt_device_pwm *device, int channel, rt_uint32_t pulse)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = (channel > 0) ? (channel) : (-channel);
    configuration.pulse = pulse;
    result = rt_device_control(&device->parent, PWM_CMD_SET_PULSE, &configuration);

    return result;
}

rt_err_t rt_pwm_set_offset(struct rt_device_pwm *device, int channel, rt_uint32_t offset)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = channel;
    configuration.offset = offset;
    result = rt_device_control(&device->parent, PWM_CMD_SET_OFFSET, &configuration);

    return result;
}

rt_err_t rt_pwm_set_oneshot(struct rt_device_pwm *device, int channel, rt_uint32_t count)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = channel;
    configuration.count = count;
    result = rt_device_control(&device->parent, PWM_CMD_SET_ONESHOT, &configuration);

    return result;
}

rt_err_t rt_pwm_set_capture(struct rt_device_pwm *device, int channel)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = channel;
    result = rt_device_control(&device->parent, PWM_CMD_SET_CAPTURE, &configuration);

    return result;
}

rt_err_t rt_pwm_lock(struct rt_device_pwm *device, rt_uint32_t channel_mask)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.mask = channel_mask;
    result = rt_device_control(&device->parent, PWM_CMD_LOCK, &configuration);

    return result;
}

rt_err_t rt_pwm_unlock(struct rt_device_pwm *device, rt_uint8_t channel_mask)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.mask = channel_mask;
    result = rt_device_control(&device->parent, PWM_CMD_UNLOCK, &configuration);

    return result;
}

rt_err_t rt_pwm_int_enable(struct rt_device_pwm *device, int channel)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = channel;
    result = rt_device_control(&device->parent, PWM_CMD_INT_ENABLE, &configuration);

    return result;
}

rt_err_t rt_pwm_int_disable(struct rt_device_pwm *device, int channel)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = channel;
    result = rt_device_control(&device->parent, PWM_CMD_INT_DISABLE, &configuration);

    return result;
}

rt_err_t rt_pwm_get(struct rt_device_pwm *device, struct rt_pwm_configuration *cfg)
{
    rt_err_t result = RT_EOK;

    if (!device)
    {
        return -RT_EIO;
    }

    result = rt_device_control(&device->parent, PWM_CMD_GET, cfg);

    return result;
}

rt_err_t rt_pwm_counter_enable(struct rt_device_pwm *device, int channel)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = channel;
    result = rt_device_control(&device->parent, PWM_CMD_CNT_ENABLE, &configuration);

    return result;
}

rt_err_t rt_pwm_counter_disable(struct rt_device_pwm *device, int channel)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = channel;
    result = rt_device_control(&device->parent, PWM_CMD_CNT_DISABLE, &configuration);

    return result;
}

rt_err_t rt_pwm_counter_get_result(struct rt_device_pwm *device, int channel, rt_uint64_t *cnt_res)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = channel;
    configuration.counter_res = 0;
    result = rt_device_control(&device->parent, PWM_CMD_GET_CNT_RES, &configuration);

    *cnt_res = configuration.counter_res;

    return result;
}

rt_err_t rt_pwm_set_freqency_meter(struct rt_device_pwm *device, int channel, rt_uint32_t delay_ms, rt_uint32_t *freq)
{
    rt_err_t result = RT_EOK;
    struct rt_pwm_configuration configuration = {0};

    if (!device)
    {
        return -RT_EIO;
    }

    configuration.channel = channel;
    configuration.delay = delay_ms;
    result = rt_device_control(&device->parent, PWM_CMD_SET_FREQ, &configuration);

    *freq = configuration.freqency;

    return result;
}

#ifdef RT_USING_FINSH
#include <stdlib.h>
#include <string.h>
#include <finsh.h>

static int pwm(int argc, char **argv)
{
    rt_err_t result = -RT_ERROR;
    char *result_str;
    static struct rt_device_pwm *pwm_device = RT_NULL;
    struct rt_pwm_configuration cfg = {0};

    if(argc > 1)
    {
        if(!strcmp(argv[1], "probe"))
        {
            if(argc == 3)
            {
                pwm_device = (struct rt_device_pwm *)rt_device_find(argv[2]);
                result_str = (pwm_device == RT_NULL) ? "failure" : "success";
                rt_kprintf("probe %s %s\n", argv[2], result_str);
            }
            else
            {
                rt_kprintf("pwm probe <device name>                  - probe pwm by name\n");
            }
        }
        else
        {
            if(pwm_device == RT_NULL)
            {
                rt_kprintf("Please using 'pwm probe <device name>' first.\n");
                return -RT_ERROR;
            }
            if(!strcmp(argv[1], "enable"))
            {
                if(argc == 3)
                {
                    result = rt_pwm_enable(pwm_device, atoi(argv[2]));
                    result_str = (result == RT_EOK) ? "success" : "failure";
                    rt_kprintf("%s channel %d is enabled %s \n", pwm_device->parent.parent.name, atoi(argv[2]), result_str);
                }
                else
                {
                    rt_kprintf("pwm enable <channel>                     - enable pwm channel\n");
                    rt_kprintf("    e.g. MSH >pwm enable  1              - PWM_CH1  nomal\n");
                    rt_kprintf("    e.g. MSH >pwm enable -1              - PWM_CH1N complememtary\n");
                }
            }
            else if(!strcmp(argv[1], "disable"))
            {
                if(argc == 3)
                {
                    result = rt_pwm_disable(pwm_device, atoi(argv[2]));
                }
                else
                {
                    rt_kprintf("pwm disable <channel>                    - disable pwm channel\n");
                }
            }
            else if(!strcmp(argv[1], "get"))
            {
                cfg.channel = atoi(argv[2]);
                result = rt_pwm_get(pwm_device, &cfg);
                if(result == RT_EOK)
                {
                    rt_kprintf("Info of device [%s] channel [%d]:\n",pwm_device, atoi(argv[2]));
                    rt_kprintf("period      : %d\n", cfg.period);
                    rt_kprintf("pulse       : %d\n", cfg.pulse);
                    rt_kprintf("polarity    : %s\n", cfg.polarity ? "inversed" : "normal");
                    rt_kprintf("aligned     : %d\n", cfg.aligned);
                    rt_kprintf("Duty cycle  : %d%%\n",(int)(((double)(cfg.pulse)/(cfg.period)) * 100));
                }
                else
                {
                    rt_kprintf("Get info of device: [%s] error.\n", pwm_device);
                }
            }
            else if (!strcmp(argv[1], "set"))
            {
                if(argc == 7)
                {
                    result = rt_pwm_set_internal(pwm_device, atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
                    rt_kprintf("pwm info set on %s at channel %d\n", pwm_device, atoi(argv[2]));
                }
                else
                {
                    rt_kprintf("Set info of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm set <channel> <period> <pulse> <polarity> <aligned>\n");
                }
            }
            else if (!strcmp(argv[1], "offset"))
            {
                if(argc == 4)
                {
                    result = rt_pwm_set_offset(pwm_device, atoi(argv[2]), atoi(argv[3]));
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to set offset %dns on %s at channel %d\n", result_str, atoi(argv[3]), pwm_device, atoi(argv[2]));
                }
                else
                {
                    rt_kprintf("Set offset of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm offset <channel> <offset> <polarity>\n");
                }
            }
            else if (!strcmp(argv[1], "oneshot"))
            {
                if(argc == 4)
                {
                    result = rt_pwm_set_oneshot(pwm_device, atoi(argv[2]), atoi(argv[3]));
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to set oneshot count to %d on %s at channel %d\n", result_str, atoi(argv[3]), pwm_device, atoi(argv[2]));
                }
                else
                {
                    rt_kprintf("Set oneshot count of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm oneshot <channel> <count>\n");
                }
            }
            else if (!strcmp(argv[1], "capture"))
            {
                if(argc == 3)
                {
                    result = rt_pwm_set_capture(pwm_device, atoi(argv[2]));
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to set capture mode on %s at channel %d\n", result_str, pwm_device, atoi(argv[2]));
                }
                else
                {
                    rt_kprintf("Set capture mode of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm capture <channel>\n");
                }
            }
            else if (!strcmp(argv[1], "lock"))
            {
                if(argc == 3)
                {
                    result = rt_pwm_lock(pwm_device, atoi(argv[2]));
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to enable global lock on %s for channel mask 0x%04x\n", result_str, pwm_device, atoi(argv[2]));
                }
                else
                {
                    rt_kprintf("Enable global lock of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm lock <channel_mask>\n");
                }
            }
            else if (!strcmp(argv[1], "unlock"))
            {
                if(argc == 3)
                {
                    result = rt_pwm_unlock(pwm_device, atoi(argv[2]));
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to disable global lock on %s for channel mask 0x%04x\n", result_str, pwm_device, atoi(argv[2]));
                }
                else
                {
                    rt_kprintf("Disable global lock of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm unlock <channel_mask>\n");
                }
            }
            else if (!strcmp(argv[1], "int_enable"))
            {
                if(argc == 2)
                {
                    result = rt_pwm_int_enable(pwm_device, 0);
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to enable interrupt on %s\n", result_str, pwm_device);
                }
                else if(argc == 3)
                {
                    result = rt_pwm_int_enable(pwm_device, atoi(argv[2]));
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to enable interrupt on %s at channel %d\n", result_str, pwm_device, atoi(argv[2]));
                }
                else
                {
                    rt_kprintf("Enable interrupt of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm int_enable\n");
                }
            }
            else if (!strcmp(argv[1], "int_disable"))
            {
                if(argc == 2)
                {
                    result = rt_pwm_int_disable(pwm_device, 0);
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to disable interrupt on %s\n", result_str, pwm_device);
                }
                else if(argc == 3)
                {
                    result = rt_pwm_int_disable(pwm_device, atoi(argv[2]));
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to enable interrupt on %s at channel %d\n", result_str, pwm_device, atoi(argv[2]));
                }
                else
                {
                    rt_kprintf("Disable interrupt of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm int_disable\n");
                }
            }
            else if (!strcmp(argv[1], "cnt_enable"))
            {
                if(argc == 3)
                {
                    result = rt_pwm_counter_enable(pwm_device, atoi(argv[2]));
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to enable counter on %s at channel %d\n", result_str, pwm_device, atoi(argv[2]));
                }
                else
                {
                    rt_kprintf("Enable counter of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm cnt_enable <channel>\n");
                }
            }
            else if (!strcmp(argv[1], "cnt_disable"))
            {
                if(argc == 3)
                {
                    result = rt_pwm_counter_disable(pwm_device, atoi(argv[2]));
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to disable counter on %s at channel %d\n", result_str, pwm_device, atoi(argv[2]));
                }
                else
                {
                    rt_kprintf("Set counter of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm cnt_disable <channel>\n");
                }
            }
            else if (!strcmp(argv[1], "cnt_get_res"))
            {
                if(argc == 3)
                {
                    uint64_t cnt_res;

                    result = rt_pwm_counter_get_result(pwm_device, atoi(argv[2]), &cnt_res);
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to get counter result on %s at channel %d: %lld\n", result_str, pwm_device, atoi(argv[2]), cnt_res);
                }
                else
                {
                    rt_kprintf("Get counter of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm cnt_get_res <channel>\n");
                }
            }
            else if (!strcmp(argv[1], "set_freq"))
            {
                if(argc == 4)
                {
                    uint32_t freq;

                    result = rt_pwm_set_freqency_meter(pwm_device, atoi(argv[2]), atoi(argv[3]), &freq);
                    result_str = (result == RT_EOK) ? "Success" : "Fail";
                    rt_kprintf("%s to set freqency meter on %s at channel %d: %dHz\n", result_str, pwm_device, atoi(argv[2]), freq);
                }
                else
                {
                    rt_kprintf("Set freqency meter of device: [%s] error\n", pwm_device);
                    rt_kprintf("Usage: pwm set_freq <channel> <mdelay>\n");
                }
            }
        }
    }
    else
    {
        rt_kprintf("Usage: \n");
        rt_kprintf("pwm probe   <device name>                                   - probe pwm by name\n");
        rt_kprintf("pwm enable  <channel>                                       - enable pwm channel\n");
        rt_kprintf("pwm disable <channel>                                       - disable pwm channel\n");
        rt_kprintf("pwm get     <channel>                                       - get pwm channel info\n");
        rt_kprintf("pwm set     <channel> <period> <pulse> <polarity> <aligned> - set pwm channel info\n");
        rt_kprintf("pwm offset  <channel> <offset>                              - set pwm channel offset\n");
        rt_kprintf("pwm oneshot <channel> <count>                               - set pwm oneshot mode\n");
        rt_kprintf("pwm capture <channel>                                       - set pwm capture mode\n");
        rt_kprintf("pwm lock    <channel_mask>                                  - enable pwm global lock\n");
        rt_kprintf("pwm unlock  <channel_mask>                                  - disable pwm global lock\n");
        rt_kprintf("pwm int_enable  <channel>                                   - enable pwm interrupt\n");
        rt_kprintf("pwm int_disable <channel>                                   - disable pwm interrupt\n");
        rt_kprintf("pwm cnt_enable  <channel>                                   - enable pwm counter\n");
        rt_kprintf("pwm cnt_disable <channel>                                   - disable pwm counter\n");
        rt_kprintf("pwm cnt_get_res <channel>                                   - get pwm counter result\n");
        rt_kprintf("pwm set_freq    <channel> <mdelay>                          - set pwm frequency meter\n");

        result = - RT_ERROR;
    }

    return RT_EOK;
}
MSH_CMD_EXPORT(pwm, pwm [option]);

#endif /* RT_USING_FINSH */
