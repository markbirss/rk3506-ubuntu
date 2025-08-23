/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtthread.h>
#include <rtdevice.h>

#include "hal_base.h"
#include "board.h"

#ifdef RT_USING_HDMI_SWITCH_DRIVERS

#include "rk_hdmi_repeat.h"

static struct rk_hdmi_repeat *g_rk_rpt = RT_NULL;

rt_err_t hdmi_repeat_register(struct hdmi_repeat *rpt)
{
    if (rpt)
    {
        g_rk_rpt->rpt = rpt;
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

static rt_err_t rk_hdmi_rpt_audio_path_set(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->ado_path_set)
        {
            ret = rpt->ops->ado_path_set(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_audio_path_get(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->ado_path_get)
        {
            ret = rpt->ops->ado_path_get(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_e_arc_vol_set(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->e_arc_vol_set)
        {
            ret = rpt->ops->e_arc_vol_set(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_e_arc_vol_get(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->e_arc_vol_get)
        {
            ret = rpt->ops->e_arc_vol_get(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_power_mode_ctrl(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->power_mode_ctrl)
        {
            ret = rpt->ops->power_mode_ctrl(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_get_repeat_status(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->get_repeat_status)
        {
            ret = rpt->ops->get_repeat_status(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_fw_update(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->fw_update)
        {
            ret = rpt->ops->fw_update(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_reg_ado_chg_hook(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->reg_ado_chg_hook)
        {
            ret = rpt->ops->reg_ado_chg_hook(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_unreg_ado_chg_hook(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->unreg_ado_chg_hook)
        {
            ret = rpt->ops->unreg_ado_chg_hook(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_reg_cec_vol_chg_hook(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->reg_cec_vol_chg_hook)
        {
            ret = rpt->ops->reg_cec_vol_chg_hook(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_unreg_cec_vol_chg_hook(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->unreg_cec_vol_chg_hook)
        {
            ret = rpt->ops->unreg_cec_vol_chg_hook(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_reg_rx_mute_hook(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->reg_rx_mute_hook)
        {
            ret = rpt->ops->reg_rx_mute_hook(rpt, data);
        }
    }

    return ret;
}

static rt_err_t rk_hdmi_rpt_unreg_rx_mute_hook(struct rk_hdmi_repeat *rk_rpt, void *data)
{
    rt_err_t ret = -RT_ERROR;

    struct hdmi_repeat *rpt = rk_rpt->rpt;

    if (rpt)
    {
        if (rpt->ops->unreg_rx_mute_hook)
        {
            ret = rpt->ops->unreg_rx_mute_hook(rpt, data);
        }
    }

    return ret;
}

rt_err_t rk_hdmi_repeat_control(rt_device_t dev, int cmd, void *args)
{
    struct rk_hdmi_repeat *rk_rpt = dev->user_data;
    rt_err_t ret = -RT_ERROR;

    switch (cmd)
    {
    case RK_REPEAT_CTL_ADO_PATH_SET:
        ret = rk_hdmi_rpt_audio_path_set(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_ADO_PATH_GET:
        ret = rk_hdmi_rpt_audio_path_get(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_E_ARC_VOL_SET:
        ret = rk_hdmi_rpt_e_arc_vol_set(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_E_ARC_VOL_GET:
        ret = rk_hdmi_rpt_e_arc_vol_get(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_REG_ADO_CHG_HOOK:
        ret = rk_hdmi_rpt_reg_ado_chg_hook(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_UNREG_ADO_CHG_HOOK:
        ret = rk_hdmi_rpt_unreg_ado_chg_hook(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_REG_CEC_VOL_CHG_HOOK:
        ret = rk_hdmi_rpt_reg_cec_vol_chg_hook(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_UNREG_CEC_VOL_CHG_HOOK:
        ret = rk_hdmi_rpt_unreg_cec_vol_chg_hook(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_REG_RX_MUTE_HOOK:
        ret = rk_hdmi_rpt_reg_rx_mute_hook(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_UNREG_RX_MUTE_HOOK:
        ret = rk_hdmi_rpt_unreg_rx_mute_hook(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_POWER_MODE_CTRL:
        ret = rk_hdmi_rpt_power_mode_ctrl(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_GET_REPEAT_STATUS:
        ret = rk_hdmi_rpt_get_repeat_status(rk_rpt, args);
        break;
    case RK_REPEAT_CTL_FW_UPDATE:
        ret = rk_hdmi_rpt_fw_update(rk_rpt, args);
        break;
    default:
        rt_kprintf("rk repeat: unsupport cmd: %d\n", cmd);
        break;
    }

    if (ret)
    {
        rt_kprintf("rk repeat: cmd: %d err:%d\n", cmd, ret);
    }

    return ret;
}

#ifdef RT_USING_DEVICE_OPS
static const struct rt_device_ops rk_hdmi_repeat_ops =
{
    .control = rk_hdmi_repeat_control,
};
#endif

rt_err_t rk_hdmi_repeat_register(struct rk_hdmi_repeat *rk_rpt, const char *name)
{
    rt_err_t ret = -RT_ERROR;
    if (rk_rpt)
    {
        rk_rpt->dev.type = RT_Device_Class_Miscellaneous;
#ifdef RT_USING_DEVICE_OPS
        rk_rpt->dev.ops = &rk_hdmi_repeat_ops;
#else
        rk_rpt->dev.init = RT_NULL;
        rk_rpt->dev.open = RT_NULL;
        rk_rpt->dev.close = RT_NULL;
        rk_rpt->dev.read  = RT_NULL;
        rk_rpt->dev.write = RT_NULL;
        rk_rpt->dev.control = rk_hdmi_repeat_control;
#endif /* RT_USING_DEVICE_OPS */

        rk_rpt->dev.user_data = rk_rpt;
        ret = rt_device_register(&rk_rpt->dev, name, RT_DEVICE_FLAG_RDWR);
    }

    return ret;
}

int rk_hdmi_repeat_init(void)
{

    g_rk_rpt = rt_calloc(1, sizeof(struct rk_hdmi_repeat));
    RT_ASSERT(g_rk_rpt);

    rk_hdmi_repeat_register(g_rk_rpt, RK_REPEAT_DEV_NAME);

    return 0;
}

INIT_PREV_EXPORT(rk_hdmi_repeat_init);

#endif
