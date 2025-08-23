/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtthread.h>
#include <rtdevice.h>

#include "hal_base.h"
#include "board.h"

#ifdef RT_USING_EP91A7P

#include "ep91a7p.h"

#ifndef EP91A7P_RXMUTE_UNMUTE
#define EP91A7P_RXMUTE_UNMUTE PIN_LOW
#endif
#define BIT(_x)  (1<<(_x))

static const unsigned int ep_samp_freq_table[8] =
{
    32000, 44100, 48000, 88200, 96000, 176400, 192000, 768000
};

static ep91a7p_device_t *g_ep91a7p_dev = RT_NULL;

static struct rk_rpt_audio_info ado_info_backup = {};

static rt_err_t _ep91a7p_write_regs(void *write_buf, rt_uint8_t write_len)
{
    struct rt_i2c_msg msgs[1];
    rt_int32_t ret;

    msgs[0].addr  = g_ep91a7p_dev->i2c_client->client_addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = write_buf;
    msgs[0].len   = write_len;

    ret = rt_i2c_transfer(g_ep91a7p_dev->i2c_client->bus, msgs, 1);
    if (ret == 1)
    {
        return RT_EOK;
    }
    else
    {
        //rt_kprintf("[ep91a7p] failed to write the register! %d\n", ret);
        return -RT_ERROR;
    }
}

static rt_err_t ep91a7p_write_regs(rt_uint8_t u8Offset, rt_uint8_t u8ByteNo, rt_uint8_t *pu8Data)
{
    char *data_buf = RT_NULL;
    rt_err_t ret;
    data_buf = (char *)rt_calloc(1, u8ByteNo + 1);
    if (!data_buf)
    {
        rt_kprintf("i2c write alloc buf size %d fail\n", u8ByteNo);
        return -RT_ERROR;
    }

    data_buf[0] = u8Offset;
    rt_memcpy(data_buf + 1, pu8Data, u8ByteNo);

    ret = _ep91a7p_write_regs(data_buf, u8ByteNo + 1);

    rt_free(data_buf);
    return ret;
}

static rt_err_t  ep91a7p_read_regs(rt_uint8_t cmd, rt_uint8_t read_len, rt_uint8_t *read_buf)
{
    struct rt_i2c_msg msgs[2];
    rt_uint8_t cmd_buf = cmd;

    msgs[0].addr  = g_ep91a7p_dev->i2c_client->client_addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = &cmd_buf;
    msgs[0].len   = 1;

    msgs[1].addr  = g_ep91a7p_dev->i2c_client->client_addr;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf   = read_buf;
    msgs[1].len   = read_len;

    if (rt_i2c_transfer(g_ep91a7p_dev->i2c_client->bus, msgs, 2) == 2)
    {
        return RT_EOK;
    }

    //rt_kprintf("[ep91a7p] failed to read the register!\n");

    return -RT_ERROR;
}

static void ep91a7p_show_hdmi_decoderinfo(struct rk_rpt_audio_info *ado_info)
{
    if (ado_info == RT_NULL)
    {
        rt_kprintf("%s ado_info is null!", __func__);
        return;
    }

    rt_kprintf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    rt_kprintf("ep91a7p_get_decoderinfo:\n");

    rt_kprintf("current mode:");
    if (g_ep91a7p_dev->cur_arc_en == 1)
    {
        rt_kprintf("ARC\n");
    }
    else if (g_ep91a7p_dev->cur_earc_en == 1)
    {
        rt_kprintf("eARC\n");
    }
    else if (g_ep91a7p_dev->cur_hdmi_en == 1)
    {
        rt_kprintf("HDMI\n");
    }

    if (!g_ep91a7p_dev->cur_arc_en)
    {

        rt_kprintf("channel number is %d\n", ado_info->audio_channel_number);

        rt_kprintf("audio sample frequency is %d\n", ado_info->audio_sample_frequency);

        rt_kprintf("audio type is ");
        if (ado_info->audio_type == RK_REPEAT_AUDIO_TYPE_LPCM)
            rt_kprintf("LPCM");
        else if (ado_info->audio_type == RK_REPEAT_AUDIO_TYPE_NLPCM)
            rt_kprintf("NLPCM");
        else if (ado_info->audio_type == RK_REPEAT_AUDIO_TYPE_HBR)
            rt_kprintf("HBR");
        else if (ado_info->audio_type == RK_REPEAT_AUDIO_TYPE_DSD)
            rt_kprintf("DSD");

        rt_kprintf("\n");

        rt_kprintf("audio sample word length is %d\n", ado_info->audio_sample_word_length);
    }

    rt_kprintf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

static rt_uint8_t ep91a7p_get_channel_frmo_ca(rt_uint8_t ca)
{
    rt_uint8_t channel_num;

    switch (ca)
    {
    case 0x00:
        channel_num = 2;
        break;
    case 0x01:
    case 0x02:
    case 0x04:
        channel_num = 3;
        break;
    case 0x03:
    case 0x05:
    case 0x06:
    case 0x08:
    case 0x14:
        channel_num = 4;
        break;
    case 0x07:
    case 0x09:
    case 0x0A:
    case 0x0C:
    case 0x15:
    case 0x16:
    case 0x18:
        channel_num = 5;
        break;
    case 0x0B:
    case 0x0D:
    case 0x0E:
    case 0x10:
    case 0x17:
    case 0x19:
    case 0x1A:
    case 0x1C:
    case 0x20:
    case 0x22:
    case 0x24:
    case 0x26:
        channel_num = 6;
        break;
    case 0x0F:
    case 0x11:
    case 0x12:
    case 0x1B:
    case 0x1D:
    case 0x1E:
    case 0x21:
    case 0x23:
    case 0x25:
    case 0x27:
    case 0x28:
    case 0x2A:
    case 0x2C:
    case 0x2E:
    case 0x30:
        channel_num = 7;
        break;
    case 0x13:
    case 0x1F:
    case 0x29:
    case 0x2B:
    case 0x2D:
    case 0x2F:
    case 0x31:
        channel_num = 8;
        break;
    default:
        rt_kprintf("ep91a7p ca:%X force use 2ch\n", ca);
        channel_num = 2;
        break;
    }

    //rt_kprintf("ep91a7p ca:%X => %dch\n", ca, channel_num);

    return channel_num;
}

static void ep91a7p_get_ado_info(struct rk_rpt_audio_info *ado_info)
{
    rt_uint8_t ep_ado_inf_frame[6] = {0};
    ep91a7p_read_regs(EP_SYSTEM_STAT_0, 1, &g_ep91a7p_dev->ai.system_status_0); //0x20
    ep91a7p_read_regs(EP_AUDIO_STAT, 1, &g_ep91a7p_dev->ai.audio_status); //0x22
    ep91a7p_read_regs(EP_CHANNEL_STAT_0, 5, &g_ep91a7p_dev->ai.cs[0]); //0x23~0x27
    ep91a7p_read_regs(EP_ADO_INF_FRAME_1, 1, &g_ep91a7p_dev->ai.cc); //0x29
    ep91a7p_read_regs(EP_ADO_INF_FRAME_4, 1, &g_ep91a7p_dev->ai.ca); //0x3c
    ep91a7p_read_regs(EP_GENERAL_CTL_1, 1, &g_ep91a7p_dev->gc.rx_sel); //0x11

    ep91a7p_read_regs(EP_ADO_INF_FRAME_0, 6, ep_ado_inf_frame);

    if (g_ep91a7p_dev->ai.audio_status & BIT(3))   //STD_ADO
    {
        //Standard Audio Sample present
        if (g_ep91a7p_dev->ai.cs[0] & BIT(1))    //
        {
            ado_info->audio_type = RK_REPEAT_AUDIO_TYPE_NLPCM;
        }
        else
        {
            ado_info->audio_type = RK_REPEAT_AUDIO_TYPE_LPCM;
        }
    }
    else if (g_ep91a7p_dev->ai.audio_status & EP_AI_HBR_ADO_MASK)     //HBR_ADO
    {
        //High Bit Rate Audio Sample is present
        ado_info->audio_type = RK_REPEAT_AUDIO_TYPE_HBR;
    }
    else if (g_ep91a7p_dev->ai.audio_status & EP_AI_DSD_ADO_MASK)     //DSD_ADO
    {
        //One Bit Audio Sample is present
        ado_info->audio_type = RK_REPEAT_AUDIO_TYPE_DSD;
    }
    else if (g_ep91a7p_dev->ai.audio_status & BIT(6))      //DST_ADO
    {
        //DST Audio Sample is present\n
        ado_info->audio_type = RK_REPEAT_AUDIO_TYPE_DSD;
    }

    ado_info->audio_sample_frequency = ep_samp_freq_table[(g_ep91a7p_dev->ai.audio_status) & EP_AI_RATE_MASK];


    switch (ep_ado_inf_frame[2] & 0x03) //0x2a
    {
    case 0:
        ado_info->audio_sample_word_length = 0;
        break;
    case 1:
        ado_info->audio_sample_word_length = 16;
        break;
    case 2:
        ado_info->audio_sample_word_length = 20;
        break;
    case 3:
        ado_info->audio_sample_word_length = 24;
        break;
    }

    //rt_kprintf("sample word length is %d bits\n", ado_info->audio_sample_word_length);
    if (ado_info->audio_sample_word_length == 0)
    {
        if (g_ep91a7p_dev->ai.cs[4] & BIT(0))
        {
            switch ((g_ep91a7p_dev->ai.cs[4] & 0x0E) >> 1)
            {
            case 0:
                ado_info->audio_sample_word_length = 0;
                break;
            case 1:
                ado_info->audio_sample_word_length = 20;
                break;
            case 2:
                ado_info->audio_sample_word_length = 22;
                break;
            case 4:
                ado_info->audio_sample_word_length = 23;
                break;
            case 5:
                ado_info->audio_sample_word_length = 24;
                break;
            case 6:
                ado_info->audio_sample_word_length = 21;
                break;
            }
        }
        else
        {
            switch ((g_ep91a7p_dev->ai.cs[4] & 0x0E) >> 1)
            {
            case 0:
                ado_info->audio_sample_word_length = 0;
                break;
            case 1:
                ado_info->audio_sample_word_length = 16;
                break;
            case 2:
                ado_info->audio_sample_word_length = 18;
                break;
            case 4:
                ado_info->audio_sample_word_length = 19;
                break;
            case 5:
                ado_info->audio_sample_word_length = 20;
                break;
            case 6:
                ado_info->audio_sample_word_length = 17;
                break;
            }
        }
    }
    //rt_kprintf("final sample word length is %d bits\n", ado_info->audio_sample_word_length);

    if (ado_info->audio_sample_word_length == 0)
    {
        //rt_kprintf("sample word length force 16 bits\n");
        ado_info->audio_sample_word_length = 16;
    }

    if (ado_info->audio_type != RK_REPEAT_AUDIO_TYPE_LPCM)
    {
        ado_info->audio_channel_number = (g_ep91a7p_dev->ai.system_status_0 & EP_AI_LAYOUT_MASK) ? 8 : 2;
    }
    else
    {
        ado_info->audio_channel_number = ep91a7p_get_channel_frmo_ca(g_ep91a7p_dev->ai.ca);
    }

    if (g_ep91a7p_dev->cur_arc_en == 1 && g_ep91a7p_dev->hdmi_tx_plug_in == 1)
    {
        ado_info->audio_output_inetrface = 1;
    }
    if ((g_ep91a7p_dev->cur_earc_en == 1 && g_ep91a7p_dev->hdmi_tx_plug_in == 1) ||
            (g_ep91a7p_dev->cur_hdmi_en == 1 && g_ep91a7p_dev->hdmi_rx_plug_in == 1))
    {

        ado_info->audio_output_inetrface = 0;
    }
}

/*
 *If ep91a7p recived cecuserctl mute, this func will be call, user can
 *choose ctrl local amp vol or send cec cmd to ctrl stb vol by hook implementation
 */
static void ep91a7p_cecuserctl_mute(void)
{
    rk_rpt_cec_cmd_t op = RK_REPEAT_CEC_CTL_VOL_MUTE;
    rt_kprintf("Mute-UnMute \n");
    if (g_ep91a7p_dev->cec_vol_change_hook != RT_NULL)
    {
        g_ep91a7p_dev->cec_vol_change_hook((void *)&op);
    }
}

/*
 *If ep91a7p recived cecuserctl vol up, this func will be call, user can
 *choose ctrl local amp vol or send cec cmd to ctrl stb vol by hook implementation
 */
static void ep91a7p_cecuserctl_volumeup(void)
{
    rk_rpt_cec_cmd_t op = RK_REPEAT_CEC_CTL_VOL_UP;
    rt_kprintf("Volume+ \n");
    if (g_ep91a7p_dev->cec_vol_change_hook != RT_NULL)
    {
        g_ep91a7p_dev->cec_vol_change_hook((void *)&op);
    }
}

/*
 *If ep91a7p recived cecuserctl vol down, this func will be call, user can
 *choose ctrl local amp vol or send cec cmd to ctrl stb vol by hook implementation
 */
static void ep91a7p_cecuserctl_volumedown(void)
{
    rk_rpt_cec_cmd_t op = RK_REPEAT_CEC_CTL_VOL_DOWN;
    rt_kprintf("Volume- \n");
    if (g_ep91a7p_dev->cec_vol_change_hook != RT_NULL)
    {
        g_ep91a7p_dev->cec_vol_change_hook((void *)&op);
    }
}

static rt_uint8_t ep91a7p_is_ado_info_change(struct rk_rpt_audio_info *ado_a, struct rk_rpt_audio_info *ado_b)
{
    if ((ado_a->audio_channel_number != ado_b->audio_channel_number) ||
            (ado_a->audio_sample_frequency != ado_b->audio_sample_frequency) ||
            (ado_a->audio_output_inetrface != ado_b->audio_output_inetrface) ||
            (ado_a->audio_type != ado_b->audio_type) ||
            (ado_a->audio_sample_word_length != ado_b->audio_sample_word_length)
       )
    {
        return 1;
        rt_kprintf("ado change\n");
    }
    rt_kprintf("no ado change\n");
    return 0;
}

static rt_uint8_t ep91a7p_is_ado_info_valid(struct rk_rpt_audio_info *ado_info)
{
    if (g_ep91a7p_dev->cur_arc_en == 1 &&
            g_ep91a7p_dev->hdmi_tx_plug_in == 1 &&
            ado_info->audio_output_inetrface == 1)
    {
        return 1;
    }

    if ((g_ep91a7p_dev->cur_earc_en == 1  &&
            g_ep91a7p_dev->hdmi_tx_plug_in == 1 &&
            ado_info->audio_output_inetrface == 0))
    {
        if (ado_info->audio_sample_frequency != 0)
            return 1;
    }
    if ((g_ep91a7p_dev->cur_hdmi_en == 1  &&
            g_ep91a7p_dev->hdmi_rx_plug_in == 1 &&
            ado_info->audio_output_inetrface == 0))
    {
        if (ado_info->audio_sample_frequency != 0)
            return 1;
    }

    rt_kprintf("not valid\n");
    return 0;
}

static rt_err_t ep91a7p_ado_path_get(struct hdmi_repeat *rpt, void *data);

static void ep91a7p_update_ado_info(rt_uint8_t force_update)
{
    rk_rpt_audio_path_t ado_path = RK_REPEAT_AUDIO_PATH_UNKNOW;

    ep91a7p_get_ado_info(&g_ep91a7p_dev->ado_info);

    //rt_kprintf("cur_earc_en:%d\n", g_ep91a7p_dev->cur_earc_en);
    //rt_kprintf("cur_arc_en:%d\n", g_ep91a7p_dev->cur_arc_en);
    //rt_kprintf("cur_hdmi_en:%d\n", g_ep91a7p_dev->cur_hdmi_en);
    //rt_kprintf("audio_output_inetrface:%d\n", g_ep91a7p_dev->ado_info.audio_output_inetrface);
    //rt_kprintf("rx hotplug:%d\n", g_ep91a7p_dev->hdmi_rx_plug_in);
    //rt_kprintf("tx hotplug:%d\n", g_ep91a7p_dev->hdmi_tx_plug_in);

    if (ep91a7p_is_ado_info_change(&ado_info_backup, &g_ep91a7p_dev->ado_info) || force_update)
    {
        if (force_update)
        {
            rt_kprintf("ep91a7p ado info chg force update\n");
        }
        else
        {
            rt_kprintf("ep91a7p ado chg !!!\n");
        }

        if (ep91a7p_is_ado_info_valid(&g_ep91a7p_dev->ado_info))
        {
            ep91a7p_show_hdmi_decoderinfo(&g_ep91a7p_dev->ado_info);

            ep91a7p_ado_path_get(&g_ep91a7p_dev->rpt, (void *)&ado_path);
            g_ep91a7p_dev->last_vaild_ado_path = ado_path;

            if (g_ep91a7p_dev->last_ado_path != RK_REPEAT_AUDIO_PATH_UNKNOW)
            {
                if (g_ep91a7p_dev->audio_change_hook != RT_NULL)
                {
                    g_ep91a7p_dev->audio_change_hook((void *)&g_ep91a7p_dev->ado_info);
                }
            }

            if (!force_update)
            {
                memcpy(&ado_info_backup, &g_ep91a7p_dev->ado_info, sizeof(struct rk_rpt_audio_info));
            }
        }
    }
}

static rt_uint8_t ep91a7p_parsing_cec_events(void)
{

    return 0;
}

static void ep91a7p_get_general_control(ep91a7p_device_t *ep91a7p)
{
    rt_uint8_t old, change;
    rt_uint8_t val;
    rt_uint8_t ado_change = 0;
    rt_uint8_t arc_on_change = 0;
    rt_uint8_t hot_plug_change = 0;

    old = ep91a7p->gi.tx_info;
    ep91a7p_read_regs(EP_GENER_INF_0, 1, &ep91a7p->gi.tx_info); //0x08
    ep91a7p_read_regs(EP_GENERAL_CTL_1, 1, &g_ep91a7p_dev->gc.rx_sel);
    if (ep91a7p->gi.tx_info == 0xff)
    {
        rt_kprintf("ep91a7p EP_GENER_INF_0 read 0xff\n");
        ep91a7p->gi.tx_info = old;
    }

    change = ep91a7p->gi.tx_info ^ old;
    if (change)
    {
        rt_kprintf("ep91a7p GI[0x08] old: 0x%02x, new: 0x%02x\n",
                   old, ep91a7p->gi.tx_info);
    }
    if (change & EP_GI_TX_HOT_PLUG_MASK)
    {
        val = (ep91a7p->gi.tx_info >> EP_GI_TX_HOT_PLUG_SHIFT) &
              EP_2CHOICE_MASK;
        rt_kprintf("ep91a7p tx hotplug changed to %d\n", val);
        g_ep91a7p_dev->hdmi_tx_plug_in = val;
        hot_plug_change = 1;
    }
    if (change & EP_GI_ARC_ON_MASK)
    {
        val = (ep91a7p->gi.tx_info >> EP_GI_EARC_ON_SHIFT) &
              EP_2CHOICE_MASK;
        rt_kprintf("ep91a7p tx (e)arc_on changed to %d\n", arc_on_change);
        arc_on_change = 1;
    }

    old = g_ep91a7p_dev->gc.link;
    ep91a7p_read_regs(EP_GENERAL_CTL_4, 1, &g_ep91a7p_dev->gc.link); //0x14
    if (g_ep91a7p_dev->gc.link == 0xff)
    {
        rt_kprintf("ep91a7p EP_GENERAL_CTL_4 read 0xff\n");
        g_ep91a7p_dev->gc.link = old;
    }

    change = g_ep91a7p_dev->gc.link ^ old;
    if (change & EP_GC_LINK_ON1_MASK)
    {
        val = (g_ep91a7p_dev->gc.link >> EP_GC_LINK_ON1_SHIFT) &
              EP_2CHOICE_MASK;
        rt_kprintf("ep91a7p rx hotplug changed to %d\n", val);
        g_ep91a7p_dev->hdmi_rx_plug_in = val;
        hot_plug_change = 1;
    }

    ep91a7p_read_regs(EP_GENER_INF_1, 1, &val); //0x09
    if (val == 0xff)
    {
        /* workaround for Nak'ed first read */
        ep91a7p_read_regs(EP_GENER_INF_1, 1, &val);
        if (val == 0xff)
            return; /* assume device not present */
    }

    if (val & EP_GI_ADO_CHF_MASK)
    {
        ado_change = 1;
    }

    if (val & EP_GI_CEC_ECF_MASK)
    {
        ep91a7p_parsing_cec_events();
    }

    old = ep91a7p->gi.video_latency;
    ep91a7p_read_regs(EP_GENER_INF_4, 1, &ep91a7p->gi.video_latency); //0x0c
    if (ep91a7p->gi.video_latency == 0xff)
    {
        rt_kprintf("ep91a7p EP_GENER_INF_4 read 0xff\n");
        ep91a7p->gi.video_latency = old;
    }
    change = ep91a7p->gi.video_latency ^ old;
    if (change & EP_GI_VIDEO_LATENCY_MASK)
    {
        val = ep91a7p->gi.video_latency;
        if (val > 0)
            val = (val - 1) * 2;
        rt_kprintf("ep91a7p video latency changed to %d\n", val);
    }

    old = ep91a7p->gc.ctl;
    ep91a7p_read_regs(EP_GENERAL_CTL_0, 1, &ep91a7p->gc.ctl); //0x10
    if (ep91a7p->gc.ctl == 0xff)
    {
        rt_kprintf("ep91a7p EP_GENERAL_CTL_0 read 0xff\n");
        ep91a7p->gc.ctl = old;
    }
    change = ep91a7p->gc.ctl ^ old;
    if (change & EP_GC_POWER_MASK)
    {
        val = (ep91a7p->gc.ctl >> EP_GC_POWER_SHIFT) &
              EP_2CHOICE_MASK;
        rt_kprintf("ep91a7p power changed to %d\n", val);
    }
    if (change & EP_GC_AUDIO_PATH_MASK)
    {
        val = (ep91a7p->gc.ctl >> EP_GC_AUDIO_PATH_SHIFT) &
              EP_2CHOICE_MASK;
        rt_kprintf("ep91a7p audio_path changed to %d\n", val);
    }
    if (change & EP_GC_CEC_MUTE_MASK)
    {
        val = (ep91a7p->gc.ctl >> EP_GC_CEC_MUTE_SHIFT) &
              EP_2CHOICE_MASK;
        rt_kprintf("ep91a7p cec_mute changed to %d\n", val);
        ep91a7p_cecuserctl_mute();
    }

    if (ep91a7p->gc.ctl & EP_GC_ARC_EN_MASK) //0x10
    {
        ep91a7p->cur_hdmi_en = 0;

        val = (ep91a7p->gi.tx_info & EP_GI_EARC_ON_MASK) >> EP_GI_EARC_ON_SHIFT;
        if (ep91a7p->cur_earc_en != val)
        {
            rt_kprintf("ep91a7p earc_en changed to %d\n", val);
            ep91a7p->cur_earc_en = val;
        }
        val = (ep91a7p->gi.tx_info & EP_GI_ARC_ON_MASK >> EP_GI_ARC_ON_SHIFT);
        if (ep91a7p->cur_arc_en != val)
        {
            rt_kprintf("ep91a7p arc_en changed to %d\n", val);
            ep91a7p->cur_arc_en = val;
            ado_change = 1;
        }

        //if(g_ep91a7p_dev->gc.rx_sel & 0x01)  {
        //  ep91a7p->cur_hdmi_en = 1;
        //}

    }
    else
    {
        if (g_ep91a7p_dev->gc.rx_sel == 0x01)
        {
            ep91a7p->cur_earc_en = 0;
            ep91a7p->cur_arc_en = 0;
            if (ep91a7p->cur_hdmi_en != 1)
            {
                ep91a7p->cur_hdmi_en = 1;
                rt_kprintf("ep91a7p hdmi_en changed to 1\n");
            }
        }
    }

    ep91a7p_read_regs(EP_GENERAL_CTL_3, 1, &val); //0x0c

    if (val != g_ep91a7p_dev->gc.cec_volume)
    {
        if (val > g_ep91a7p_dev->gc.cec_volume)
        {
            ep91a7p_cecuserctl_volumeup();
        }
        else
        {
            ep91a7p_cecuserctl_volumedown();
        }
        g_ep91a7p_dev->gc.cec_volume = val;
    }

    if (ado_change == 1 || arc_on_change == 1 || hot_plug_change == 1)
    {
        ep91a7p_update_ado_info(0);
    }
}

static int ep91a7p_power_mode(ep91a7p_power_mode_t mode)
{
    rt_uint8_t reg;

    ep91a7p_read_regs(EP_GENERAL_CTL_0, 1, &reg);
    if (mode == EP91A7P_MODE_POWER_ON)
    {
        reg |= 0x80;
    }
    else if (mode == EP91A7P_MODE_POWER_OFF)
    {
        reg &= ~0x80;
    }
    return ep91a7p_write_regs(EP_GENERAL_CTL_0, 1, &reg);
}

static int ep91a7p_primary_sel(int primary_sel)
{
    rt_uint8_t reg;

    ep91a7p_read_regs(EP_GENERAL_CTL_1, 1, &reg);
    switch (primary_sel)
    {
    case 1:
        //port 1 select HDMI in
        reg = 0x01;
        ep91a7p_write_regs(EP_GENERAL_CTL_1, 1, &reg);
        break;
    case 5:
        //port 5 select (e)ARC
        reg  = 0x05;
        ep91a7p_write_regs(EP_GENERAL_CTL_1, 1, &reg);
        break;
    default:
        rt_kprintf("Unsupported port select\n");
        break;
    }

    return 0;
}

static rt_err_t ep91a7p_ado_path_set(struct hdmi_repeat *rpt, void *data)
{
    rt_uint8_t reg;
    rk_rpt_audio_path_t ado_path = *((rt_uint8_t *)data);

    memset(&ado_info_backup, 0x00, sizeof(struct rk_rpt_audio_info));

    ep91a7p_read_regs(EP_GENERAL_CTL_0, 1, &reg);
    switch (ado_path)
    {
    case RK_REPEAT_AUDIO_PATH_HDMI_0:
        //HDMI Mode
        reg &= ~BIT(0);
        ep91a7p_write_regs(EP_GENERAL_CTL_0, 1, &reg);
        ep91a7p_primary_sel(1);
        if (g_ep91a7p_dev->last_ado_path == RK_REPEAT_AUDIO_PATH_UNKNOW &&
                g_ep91a7p_dev->last_vaild_ado_path == RK_REPEAT_AUDIO_PATH_HDMI_0)
        {
            g_ep91a7p_dev->last_ado_path = RK_REPEAT_AUDIO_PATH_HDMI_0;
            ep91a7p_update_ado_info(1);
        }
        g_ep91a7p_dev->last_ado_path = RK_REPEAT_AUDIO_PATH_HDMI_0;
        g_ep91a7p_dev->last_vaild_ado_path = RK_REPEAT_AUDIO_PATH_HDMI_0;
        break;
    case RK_REPEAT_AUDIO_PATH_ARC_EARC:
        //(e)ARC/ARC Mode
        reg |= BIT(0);
        ep91a7p_write_regs(EP_GENERAL_CTL_0, 1, &reg);
        ep91a7p_primary_sel(5);
        if (g_ep91a7p_dev->last_ado_path == RK_REPEAT_AUDIO_PATH_UNKNOW &&
                g_ep91a7p_dev->last_vaild_ado_path == RK_REPEAT_AUDIO_PATH_ARC_EARC)
        {
            g_ep91a7p_dev->last_ado_path = RK_REPEAT_AUDIO_PATH_ARC_EARC;
            ep91a7p_update_ado_info(1);
        }
        g_ep91a7p_dev->last_ado_path = RK_REPEAT_AUDIO_PATH_ARC_EARC;
        g_ep91a7p_dev->last_vaild_ado_path = RK_REPEAT_AUDIO_PATH_ARC_EARC;
        break;
    default:
        g_ep91a7p_dev->last_ado_path = RK_REPEAT_AUDIO_PATH_UNKNOW;
        rt_kprintf("ep91a7p un support ado path\n");
        break;
    }

    return RT_EOK;
}

static rt_err_t ep91a7p_ado_path_get(struct hdmi_repeat *rpt, void *data)
{
    rt_uint8_t reg, reg1;
    rt_uint8_t *ado_path = (rt_uint8_t *)data;

    ep91a7p_read_regs(EP_GENERAL_CTL_0, 1, &reg);
    ep91a7p_read_regs(EP_GENERAL_CTL_1, 1, &reg1);

    if (reg & (EP_GC_ARC_EN_MASK))
    {
        if (reg1 == 0x05)
        {
            *ado_path = RK_REPEAT_AUDIO_PATH_ARC_EARC;
        }
        else if (reg1 == 0x01)
        {
            *ado_path = RK_REPEAT_AUDIO_PATH_HDMI_0;
        }
    }
    else
    {
        *ado_path = RK_REPEAT_AUDIO_PATH_HDMI_0;
    }

    return RT_EOK;
}

static void ep91a7p_arc_set_max_volume(rt_uint8_t volume_max)
{

}

static rt_err_t ep91a7p_e_arc_vol_set(struct hdmi_repeat *rpt, void *data)
{
    struct rk_rpt_volume_info *info = (struct rk_rpt_volume_info *)data;
    rt_uint8_t temp;

    ep91a7p_read_regs(EP_GENERAL_CTL_0, 1, &temp);
    //0~100
    if (info->volume > EP91A7P_VOL_MAX)
    {
        info->volume = EP91A7P_VOL_MAX;
    }

    if (info->is_mute)
    {
        temp |= BIT(1);
    }
    else
    {
        temp &= ~BIT(1);
    }
    ep91a7p_write_regs(EP_GENERAL_CTL_0, 1, &temp);

    ep91a7p_write_regs(EP_GENERAL_CTL_3, 1, &info->volume);

    g_ep91a7p_dev->gc.cec_volume = info->volume;

    rt_kprintf("ep91a7p vol:%d, %s\n", info->volume, info->is_mute ? "mute" : "unmute");

    return RT_EOK;
}

static rt_err_t ep91a7p_e_arc_vol_get(struct hdmi_repeat *rpt, void *data)
{
    rt_uint8_t  temp;
    struct rk_rpt_volume_info *info = (struct rk_rpt_volume_info *)data;

    ep91a7p_read_regs(EP_GENERAL_CTL_0, 1, &temp);

    info->is_mute = (temp & EP_GC_CEC_MUTE_MASK) ? 1 : 0;

    ep91a7p_read_regs(EP_GENERAL_CTL_3, 1, &info->volume);

    rt_kprintf("ep91a7p vol:%d, %s\n", info->volume, info->is_mute ? "mute" : "unmute");

    return RT_EOK;
}

static rt_err_t ep91a7p_reg_rx_mute_hook(struct hdmi_repeat *rpt, void *data)
{
    g_ep91a7p_dev->rx_mute_hook = (void (*)(void *arg))data;

    return RT_EOK;
}

static rt_err_t ep91a7p_unreg_rx_mute_hook(struct hdmi_repeat *rpt, void *data)
{
    g_ep91a7p_dev->rx_mute_hook = RT_NULL;

    return RT_EOK;
}

static rt_err_t ep91a7p_reg_ado_chg_hook(struct hdmi_repeat *rpt, void *data)
{
    g_ep91a7p_dev->audio_change_hook = (void (*)(void *arg))data;

    return RT_EOK;
}

static rt_err_t ep91a7p_unreg_ado_chg_hook(struct hdmi_repeat *rpt, void *data)
{
    g_ep91a7p_dev->audio_change_hook = RT_NULL;

    return RT_EOK;
}

static rt_err_t ep91a7p_reg_cec_vol_chg_hook(struct hdmi_repeat *rpt, void *data)
{
    g_ep91a7p_dev->cec_vol_change_hook = (void (*)(void *arg))data;

    return RT_EOK;
}

static rt_err_t ep91a7p_unreg_cec_vol_chg_hook(struct hdmi_repeat *rpt, void *data)
{
    g_ep91a7p_dev->cec_vol_change_hook = RT_NULL;

    return RT_EOK;
}

static rt_err_t ep91a7p_power_mode_ctrl(struct hdmi_repeat *rpt, void *data)
{
    rk_rpt_power_mode_t power_mode = *((rt_uint8_t *)data);
    int i;

    switch (power_mode)
    {
    case RK_REPEAT_POWER_OFF_MODE:
        //TODO
        ep91a7p_power_mode(EP91A7P_MODE_POWER_OFF);
        break;
    case RK_REPEAT_LOW_POWER_MODE:
        ep91a7p_power_mode(EP91A7P_MODE_POWER_OFF);
        break;
    case RK_REPEAT_POWER_ON_MODE:
    default:
        //If ep91a7p enters low power mode, it needs to be awakened through
        //the serial port. The first serial port command may be lost, so we
        //send it several times to ensure successful awakening.
        for (i = 0; i < 5; i++)
        {
            ep91a7p_power_mode(EP91A7P_MODE_POWER_ON);
        }
        break;
    }

    return RT_EOK;
}

static rt_err_t ep91a7p_get_repeat_status(struct hdmi_repeat *rpt, void *data)
{
    rk_rpt_status_t *status = (rk_rpt_status_t *)data;

    *status = g_ep91a7p_dev->dev_status;

    return RT_EOK;
}

static void ep91a7p_isr(void *data)
{
    ep91a7p_device_t *dev;

    dev = (ep91a7p_device_t *)data;

    rt_sem_release(dev->isr_sem);
}


static void ep91a7p_rx_mute_thread(void *arg)
{
    ep91a7p_device_t *dev = (ep91a7p_device_t *)arg;
    rt_uint8_t mute_state;

    while (1)
    {
        rt_sem_take(dev->rx_mute_sem, RT_WAITING_FOREVER);

        if (rt_pin_read(EP91A7P_RXMUTE_PIN) == EP91A7P_RXMUTE_UNMUTE)
        {
            mute_state = RK_REPEAT_RXMUTE_UNMUTE;
        }
        else
        {
            mute_state = RK_REPEAT_RXMUTE_MUTE;
        }

        if (g_ep91a7p_dev->rx_mute_hook != RT_NULL)
            g_ep91a7p_dev->rx_mute_hook((void *)&mute_state);
    }
}

static void ep91a7p_rxmute_irq_callback(void *args)
{
    ep91a7p_device_t *dev;

    dev = (ep91a7p_device_t *)args;

    rt_sem_release(dev->rx_mute_sem);
    rt_sem_release(dev->isr_sem);
}

static void ep91a7p_dump()
{
    rt_uint16_t u16Cnt;
    rt_uint8_t  u8I2cData[0x100];
    rt_uint8_t  *pu8I2cData;

    rt_kprintf("\n============ep91a7p status=============\n");

    rt_kprintf("\n");

    ep91a7p_read_regs(EP_GENERAL_CTL_0, 1, &g_ep91a7p_dev->gc.ctl);
    ep91a7p_read_regs(EP_GENERAL_CTL_1, 1, &g_ep91a7p_dev->gc.rx_sel);
    ep91a7p_read_regs(EP_GENERAL_CTL_2, 1, &g_ep91a7p_dev->gc.ctl2); //0x12
    ep91a7p_read_regs(EP_GENERAL_CTL_4, 1, &g_ep91a7p_dev->gc.link);
    ep91a7p_read_regs(EP_AUDIO_STAT, 1, &g_ep91a7p_dev->ai.audio_status);

    ep91a7p_read_regs(EP_GENER_INF_0, 1, &g_ep91a7p_dev->gi.tx_info);

    ep91a7p_read_regs(EP_SYSTEM_STAT_1, 1, &g_ep91a7p_dev->ai.system_status_1);
    // Check Power Status
    if (g_ep91a7p_dev->gc.ctl & EP_GC_POWER_MASK)
    {
        rt_kprintf("Power[%d]\n", (g_ep91a7p_dev->gc.ctl & EP_GC_POWER_MASK) >> EP_GC_POWER_SHIFT);


        rt_kprintf("CEC[%d] CEC Mute[%d]\n", (~g_ep91a7p_dev->gc.ctl & EP_GC_CEC_DIS_MASK) >> EP_GC_CEC_DIS_SHIFT,
                   (g_ep91a7p_dev->gc.ctl & EP_GC_CEC_MUTE_MASK) >> EP_GC_CEC_MUTE_SHIFT);

        rt_kprintf("AudioSystemEnable[%d], ARC_enable[%d] eARC_enable[%d]\n",
                   g_ep91a7p_dev->gc.ctl & EP_GC_AUDIO_PATH_MASK >> EP_GC_AUDIO_PATH_SHIFT,
                   (~g_ep91a7p_dev->gc.ctl2 & EP_GC_ARC_DIS_MASK) >> EP_GC_ARC_DIS_SHIFT,
                   (~g_ep91a7p_dev->gc.ctl2 & EP_GC_EARC_DIS_MASK) >> EP_GC_EARC_DIS_SHIFT);

        rt_kprintf("Audio Ready, HDMI[%d], ARC[%d], eARC[%d]\n",
                   (g_ep91a7p_dev->gc.link & EP_GC_LINK_ON1_MASK) >> EP_GC_LINK_ON1_SHIFT,
                   (g_ep91a7p_dev->gi.tx_info & EP_GI_ARC_ON_MASK) >> EP_GI_ARC_ON_SHIFT,
                   (g_ep91a7p_dev->gi.tx_info & EP_GI_EARC_ON_MASK) >> EP_GI_EARC_ON_SHIFT
                  );

        rt_kprintf("primary sel[%d] ", (g_ep91a7p_dev->gc.rx_sel & EP_GC_RX_SEL_MASK) >> EP_GC_RX_SEL_SHIFT);
        if (g_ep91a7p_dev->gc.ctl & EP_GC_ARC_EN_MASK)
        {
            rt_kprintf("Audio path from (e)ARC\n");
        }
        else
        {
            rt_kprintf("Audio path from HDMI\n");
        }

        if (g_ep91a7p_dev->gc.link & BIT(7))
        {
            rt_kprintf("Enable SPDIF to IIS\n");
            rt_kprintf("Select the \n");
            switch (g_ep91a7p_dev->gc.link & (BIT(5) | BIT(6)) >> 5)
            {
            case 0:
                rt_kprintf("GPIO0\n");
                break;
            case 1:
                rt_kprintf("GPIO1\n");
                break;
            case 2:
                rt_kprintf("GPIO2\n");
                break;
            case 3:
                rt_kprintf("GPIO3\n");
                break;
            }
            rt_kprintf(" pin for the SPDIF to IIS\n");
        }
        else
        {
            rt_kprintf("Disable SPDIF to IIS\n");
        }

        rt_kprintf("cec volume: %d, %s\n", g_ep91a7p_dev->gc.cec_volume,
                   (g_ep91a7p_dev->gc.ctl & EP_GC_CEC_MUTE_MASK) ? "mute" : "unmute");

        // RX
        //Bit7 Valid clock signal detected at selected HDMI port.
        //Bit6 Valid DE signal detected at HDMI receiver.
        if (g_ep91a7p_dev->ai.system_status_1 & 0xC0)
        {
            rt_kprintf("HDMI RX link ok\n");
        }
        else
        {
            rt_kprintf("HDMI RX link bad\n");
        }
        ep91a7p_update_ado_info(0);

        rt_kprintf("\n");
    }
    else
    {
        rt_kprintf("power off\n");
    }

    for (u16Cnt = 0; u16Cnt < 0x100; u16Cnt += 0x10)
    {
        ep91a7p_read_regs(u16Cnt, 0x10, &u8I2cData[u16Cnt]);
    }
    for (u16Cnt = 0; u16Cnt < 0x100; u16Cnt += 0x10)
    {
        pu8I2cData = &u8I2cData[u16Cnt];
        rt_kprintf("\r0x%02X: %02X %02X %02X %02X  %02X %02X %02X %02X   %02X %02X %02X %02X  %02X %02X %02X %02X\n", u16Cnt, pu8I2cData[0], pu8I2cData[1],
                   pu8I2cData[2], pu8I2cData[3], pu8I2cData[4], pu8I2cData[5], pu8I2cData[6], pu8I2cData[7], pu8I2cData[8], pu8I2cData[9], pu8I2cData[10], pu8I2cData[11],
                   pu8I2cData[12], pu8I2cData[13], pu8I2cData[14], pu8I2cData[15]);
    }
    rt_kprintf("=======================================\n");
}

static rt_err_t ep91a7p_control(rt_device_t dev, int cmd, void *args)
{
    ep91a7p_device_t *ep91a7p_dev = dev->user_data;
    struct hdmi_repeat *rpt = &ep91a7p_dev->rpt;

    switch (cmd)
    {
    case RK_REPEAT_CTL_E_ARC_VOL_SET:
        ep91a7p_e_arc_vol_set(rpt, args);
        break;
    case RK_REPEAT_CTL_E_ARC_VOL_GET:
        ep91a7p_e_arc_vol_get(rpt, args);
        break;
    case RT_EP91A7P_CTL_ARC_SET_MAX_VOL:
        ep91a7p_arc_set_max_volume(*((rt_uint8_t *)args));
        break;
    case RT_EP91A7P_CTL_PRIMARY_SEL:
        ep91a7p_primary_sel(*((rt_uint8_t *)args));
        break;
    case RK_REPEAT_CTL_ADO_PATH_SET:
        ep91a7p_ado_path_set(rpt, args);
        break;
    case RK_REPEAT_CTL_ADO_PATH_GET:
        ep91a7p_ado_path_get(rpt, args);
        break;
    case RT_EP91A7P_CTL_DUMP:
        ep91a7p_dump();
        break;
    case RK_REPEAT_CTL_REG_ADO_CHG_HOOK:
        ep91a7p_reg_ado_chg_hook(rpt, args);
        break;
    case RK_REPEAT_CTL_UNREG_ADO_CHG_HOOK:
        ep91a7p_unreg_ado_chg_hook(rpt, args);
        break;
    case RK_REPEAT_CTL_REG_RX_MUTE_HOOK:
        ep91a7p_reg_rx_mute_hook(rpt, args);
        break;
    case RK_REPEAT_CTL_UNREG_RX_MUTE_HOOK:
        ep91a7p_unreg_rx_mute_hook(rpt, args);
        break;
    case RK_REPEAT_CTL_REG_CEC_VOL_CHG_HOOK:
        ep91a7p_reg_cec_vol_chg_hook(rpt, args);
        break;
    case RK_REPEAT_CTL_UNREG_CEC_VOL_CHG_HOOK:
        ep91a7p_unreg_cec_vol_chg_hook(rpt, args);
        break;
    case RK_REPEAT_CTL_POWER_MODE_CTRL:
        ep91a7p_power_mode_ctrl(rpt, args);
        break;
    case RK_REPEAT_CTL_GET_REPEAT_STATUS:
        ep91a7p_get_repeat_status(rpt, args);
        break;
    default:
        rt_kprintf("Unsupport cmd: %d\n", cmd);
        break;
    }
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
static const struct rt_device_ops ep91a7p_device_ops =
{
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    ep91a7p_control
};
#endif /* RT_USING_DEVICE_OPS */

static rt_err_t rt_device_ep91a7p_register(ep91a7p_device_t *dev, const char *name, const void *user_data)
{
    rt_err_t result = RT_EOK;

#ifdef RT_USING_DEVICE_OPS
    dev->dev.ops = &ep91a7p_device_ops;
#else
    dev->dev.init = RT_NULL;
    dev->dev.open = RT_NULL;
    dev->dev.close = RT_NULL;
    dev->dev.read  = RT_NULL;
    dev->dev.write = RT_NULL;
    dev->dev.control = ep91a7p_control;
#endif /* RT_USING_DEVICE_OPS */

    dev->dev.type         = RT_Device_Class_Miscellaneous;
    dev->dev.user_data    = (void *)user_data;

    result = rt_device_register(&dev->dev, name, RT_DEVICE_FLAG_RDWR);

    return result;
}

static int ep91a7p_default_setting(ep91a7p_device_t *ep91a7p)
{
    rt_uint8_t i;

    ep91a7p->gc.cec_volume = EP91A7P_VOL_DEFAULT;
    ep91a7p_write_regs(EP_GENERAL_CTL_3, 1, &ep91a7p->gc.cec_volume);

    //If ep91a7p enters low power mode, it needs to be awakened through
    //the serial port. The first serial port command may be lost, so we
    //send it several times to ensure successful awakening.
    for (i = 0; i < 5; i++)
    {
        ep91a7p_power_mode(EP91A7P_MODE_POWER_ON);
    }

    rt_thread_mdelay(10);
    ep91a7p_get_general_control(ep91a7p);

    return 0;
}

static rt_err_t ep91a7p_connectivity_check(void)
{

    rt_uint8_t  trycount;

    //If ep91a7p enters low power mode, it needs to be awakened through
    //the serial port. The first serial port command may be lost, so we
    //send it several times to ensure successful awakening.
    for (trycount = 0; trycount < 30; trycount ++)
    {
        if (!ep91a7p_power_mode(EP91A7P_MODE_POWER_ON))
        {
            return RT_EOK;
        }
        rt_thread_mdelay(50);
    }
    rt_kprintf("ep91a7p not present!\n");
    return -RT_ERROR;
}

static rt_err_t ep91a7p_init(struct hdmi_repeat *rpt, void *data)
{

    return RT_EOK;
}

static rt_err_t ep91a7p_deinit(struct hdmi_repeat *rpt, void *data)
{
    return RT_EOK;
}

static const struct hdmi_repeat_ops ep91a7p_ops =
{
    .init = ep91a7p_init,
    .deinit = ep91a7p_deinit,
    .ado_path_get = ep91a7p_ado_path_get,
    .ado_path_set = ep91a7p_ado_path_set,
    .e_arc_vol_get = ep91a7p_e_arc_vol_get,
    .e_arc_vol_set = ep91a7p_e_arc_vol_set,
    .reg_ado_chg_hook = ep91a7p_reg_ado_chg_hook,
    .unreg_ado_chg_hook = ep91a7p_unreg_ado_chg_hook,
    .reg_cec_vol_chg_hook = ep91a7p_reg_cec_vol_chg_hook,
    .unreg_cec_vol_chg_hook = ep91a7p_unreg_cec_vol_chg_hook,
    .reg_rx_mute_hook = ep91a7p_reg_rx_mute_hook,
    .unreg_rx_mute_hook = ep91a7p_unreg_rx_mute_hook,
    .power_mode_ctrl = ep91a7p_power_mode_ctrl,
    .get_repeat_status = ep91a7p_get_repeat_status,
};

static rt_err_t s_ep91a7p_init(ep91a7p_device_t *dev)
{
    rt_err_t ret;

    dev->isr_sem = rt_sem_create("ep91a7p_isr_sem", 0, RT_IPC_FLAG_PRIO);
    RT_ASSERT(dev->isr_sem);

    dev->rx_mute_sem = rt_sem_create("ep91a7p_rxmute_sem", 0, RT_IPC_FLAG_PRIO);
    RT_ASSERT(dev->rx_mute_sem);

    dev->rxmute_tid = rt_thread_create("rx_mute", ep91a7p_rx_mute_thread, dev,
                                       2048, 10, 10);
    RT_ASSERT(dev->rxmute_tid != RT_NULL);
    rt_thread_startup(dev->rxmute_tid);


    ret = rt_device_open((rt_device_t)dev->i2c_client->bus, RT_DEVICE_FLAG_RDWR);
    RT_ASSERT(ret == RT_EOK);

    rt_pin_mode(EP91A7P_INT_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(EP91A7P_INT_PIN, PIN_IRQ_MODE_RISING_FALLING, ep91a7p_isr, (void *)g_ep91a7p_dev);
    rt_pin_irq_enable(EP91A7P_INT_PIN, PIN_IRQ_ENABLE);

    rt_pin_mode(EP91A7P_RXMUTE_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(EP91A7P_RXMUTE_PIN, PIN_IRQ_MODE_RISING_FALLING, ep91a7p_rxmute_irq_callback, (void *)g_ep91a7p_dev);
    rt_pin_irq_enable(EP91A7P_RXMUTE_PIN, PIN_IRQ_ENABLE);

    //wait ready
    ep91a7p_default_setting(dev);

    dev->rpt.ops = &ep91a7p_ops;
    dev->rpt.private = (void *)dev;

    hdmi_repeat_register(&dev->rpt);

    ret = rt_device_ep91a7p_register(dev, EP91A7P_DEVICE_NAME, (void *)dev);
    RT_ASSERT(ret == RT_EOK);

    return RT_EOK;
}

static void ep91a7p_thread(void *arg)
{
    ep91a7p_device_t *dev = (ep91a7p_device_t *)arg;
    rt_uint8_t i;

    if (ep91a7p_connectivity_check())
    {
        rt_kprintf("ep91a7p connectivity check failed\n");
        return;
    }

    g_ep91a7p_dev->dev_status = RK_REPEAT_INIT;

    s_ep91a7p_init(dev);

    g_ep91a7p_dev->dev_status = RK_REPEAT_READY;

    while (1)
    {
        rt_sem_take(dev->isr_sem, RT_WAITING_FOREVER);

        for (i = 0; i < 30; i++)
        {
            ep91a7p_get_general_control(g_ep91a7p_dev);
            rt_thread_mdelay(20);
        }
    }
}

static int rt_hw_ep91a7p_init(void)
{
    ep91a7p_device_t *dev;

    /* init ep91a7p device */
    g_ep91a7p_dev = dev = (ep91a7p_device_t *)rt_calloc(1, sizeof(ep91a7p_device_t));
    RT_ASSERT(dev != RT_NULL);
    rt_memset((void *)dev, 0, sizeof(ep91a7p_device_t));

    g_ep91a7p_dev->dev_status = RK_REPEAT_NOT_PRESENT;

    /* i2c interface bus */
    struct rt_i2c_client *i2c_client;
    dev->i2c_client = i2c_client = (struct rt_i2c_client *)rt_calloc(1, sizeof(struct rt_i2c_client));
    RT_ASSERT(i2c_client != RT_NULL);

    i2c_client->client_addr = EP91A7P_I2C_ADDR;
    i2c_client->bus = (struct rt_i2c_bus_device *)rt_device_find(EP91A7P_I2C_DEV);
    RT_ASSERT(i2c_client->bus != RT_NULL);

    dev->int_tid = rt_thread_create("ep91a7p", ep91a7p_thread, dev,
                                    2048, 10, 10);
    RT_ASSERT(dev->int_tid != RT_NULL);
    rt_thread_startup(dev->int_tid);

    return RT_EOK;
}

INIT_DEVICE_EXPORT(rt_hw_ep91a7p_init);

static void ep91a7p_show_usage()
{
    rt_kprintf("Usage: \n");
    rt_kprintf("ep91a7p dump                       -dump status\n");
    rt_kprintf("ep91a7p powermode <mode>           -set power mode 0.pwoer off,1.low power,2.power on\n");
    rt_kprintf("ep91a7p audiopath <path>           -select path: 1:hdmi0 4:ARC/eARC mode\n");
    rt_kprintf("ep91a7p arcvolset <vol> <mute>     -set vol and mute/unmute status:ep91a7p arcvolset 50 umute\n");
    rt_kprintf("ep91a7p arcvolget                  -get vol and mute/unmute status\n");
    rt_kprintf("ep91a7p hookregister <hook type>   -hook type: 1.audio change,2.cec vol change,3.rx mute\n");
    rt_kprintf("ep91a7p hookunregister <hook type> -hook type: 1.audio change,2.cec vol change,3.rx mute\n");
}

static void ep91a7p_audio_change_test_hook(void *arg)
{
    rt_kprintf("ep91a7p_audio_change_test_hook!\n");
    ep91a7p_dump();
}

static void ep91a7p_cec_vol_change_test_hook(void *arg)
{
    rt_kprintf("ep91a7p_cec_vol_change_test_hook!\n");
    if (*(rk_rpt_cec_cmd_t *)arg == RK_REPEAT_CEC_CTL_VOL_MUTE)
        rt_kprintf("cec vol mute\n");
    else if (*(rk_rpt_cec_cmd_t *)arg == RK_REPEAT_CEC_CTL_VOL_UP)
        rt_kprintf("cec vol up\n");
    else if (*(rk_rpt_cec_cmd_t *)arg == RK_REPEAT_CEC_CTL_VOL_DOWN)
        rt_kprintf("cec vol down\n");
}

static void ep91a7p_rx_mute_test_hook(void *arg)
{
    rt_kprintf("ep91a7p_rx_mute_test_hook!\n");
    if (*(rt_uint8_t *)arg == RK_REPEAT_RXMUTE_MUTE)
        rt_kprintf("ep91a7p_rx_mute!\n");
    else if (*(rt_uint8_t *)arg == RK_REPEAT_RXMUTE_UNMUTE)
        rt_kprintf("ep91a7p_rx_unmute!\n");

}

void ep91a7p(int argc, char **argv)
{
    rt_err_t res = RT_EOK;
    static rt_device_t ep91a7p_dev = RT_NULL;
    rt_int32_t para = 0;

    struct rk_rpt_volume_info volume_info;

    if (argc < 2)
        goto out2;

    ep91a7p_dev = rt_device_find(EP91A7P_DEVICE_NAME);
    if (!ep91a7p_dev)
    {
        rt_kprintf("find %s failed!\n", EP91A7P_DEVICE_NAME);
        return;
    }

    res = rt_device_init(ep91a7p_dev);
    if (res != RT_EOK)
    {
        rt_kprintf("initialize %s failed!\n", EP91A7P_DEVICE_NAME);
        return;
    }

    rt_thread_mdelay(20);
    res = rt_device_open(ep91a7p_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    if (res != RT_EOK)
    {
        rt_kprintf("Failed to open device: %s\n", EP91A7P_DEVICE_NAME);
        return;
    }

    if (argc == 2)
    {
        if (!strcmp(argv[1], "dump"))
        {
            rt_device_control(ep91a7p_dev, RT_EP91A7P_CTL_DUMP, RT_NULL);
        }
        else if (!strcmp(argv[1], "arcvolget"))
        {
            rt_device_control(ep91a7p_dev, RK_REPEAT_CTL_E_ARC_VOL_GET, (void *)&volume_info);
            rt_kprintf("ep91a7p e/arc volue is %d,%s\n", volume_info.volume,
                       volume_info.is_mute ? "mute" : "no mute");
        }
    }
    else if (argc == 3)
    {
        if (!strcmp(argv[1], "powermode"))
        {
            para = strtol(argv[2], RT_NULL, 10);
            rt_device_control(ep91a7p_dev, RK_REPEAT_CTL_POWER_MODE_CTRL, (void *)&para);
        }
        else if (!strcmp(argv[1], "psel"))
        {
            para = strtol(argv[2], RT_NULL, 10);
            rt_device_control(ep91a7p_dev, RT_EP91A7P_CTL_PRIMARY_SEL, (void *)&para);
        }
        else if (!strcmp(argv[1], "audiopath"))
        {
            para = strtol(argv[2], RT_NULL, 10);
            if (para >= RK_REPEAT_AUDIO_PATH_MAX)
            {
                rt_kprintf("wrong usage, check your param \n");
                goto out1;
            }
            rt_device_control(ep91a7p_dev, RK_REPEAT_CTL_ADO_PATH_SET, (void *)&para);
        }
        else if (!strcmp(argv[1], "hookregister"))
        {
            para = strtol(argv[2], RT_NULL, 10);
            if (para == 1)
            {
                rt_device_control(ep91a7p_dev, RK_REPEAT_CTL_REG_ADO_CHG_HOOK, (void *)&ep91a7p_audio_change_test_hook);
            }
            else if (para == 2)
            {
                rt_device_control(ep91a7p_dev, RK_REPEAT_CTL_REG_CEC_VOL_CHG_HOOK, (void *)&ep91a7p_cec_vol_change_test_hook);
            }
            else if (para == 3)
            {
                rt_device_control(ep91a7p_dev, RK_REPEAT_CTL_REG_RX_MUTE_HOOK, (void *)&ep91a7p_rx_mute_test_hook);
            }
            else
            {
                rt_kprintf("wrong usage, check your param \n");
                goto out1;
            }
        }
        else if (!strcmp(argv[1], "hookunregister"))
        {
            para = strtol(argv[2], RT_NULL, 10);
            if (para == 1)
            {
                rt_device_control(ep91a7p_dev, RK_REPEAT_CTL_UNREG_ADO_CHG_HOOK, RT_NULL);
            }
            else if (para == 2)
            {
                rt_device_control(ep91a7p_dev, RK_REPEAT_CTL_UNREG_CEC_VOL_CHG_HOOK, RT_NULL);
            }
            else if (para == 3)
            {
                rt_device_control(ep91a7p_dev, RK_REPEAT_CTL_UNREG_RX_MUTE_HOOK, RT_NULL);
            }
            else
            {
                rt_kprintf("wrong usage, check your param \n");
                goto out1;
            }
        }
    }
    else if (argc == 4)
    {
        if (!strcmp(argv[1], "arcvolset"))
        {
            volume_info.volume = strtol(argv[2], RT_NULL, 10);
            if (!strcmp(argv[3], "mute"))
            {
                volume_info.is_mute = 1;
            }
            else
            {
                volume_info.is_mute = 0;
            }
            rt_device_control(ep91a7p_dev, RK_REPEAT_CTL_E_ARC_VOL_SET, (void *)&volume_info);
        }

    }
out1:
    rt_device_close(ep91a7p_dev);
    return;
out2:
    ep91a7p_show_usage();
    return;
}

MSH_CMD_EXPORT(ep91a7p, ep91a7p test);
#endif
