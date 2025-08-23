/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __RK_HDMI_REPEAT_H__
#define __RK_HDMI_REPEAT_H__

#define RK_REPEAT_DEV_NAME "rk_repeat"

#define RK_REPEAT_ISP_SUCCESS         (0)
#define RK_REPEAT_ISP_FW_NOT_FOUND    (-1)
#define RK_REPEAT_ISP_BAD_FW          (-2)
#define RK_REPEAT_ISP_NO_NEED_UPDATE  (-3)
#define RK_REPEAT_ISP_UPDATE_FAILED   (-4)

struct rk_rpt_volume_info
{
    rt_uint8_t volume;
    rt_uint8_t is_mute;
};

typedef enum
{
    RK_REPEAT_RXMUTE_MUTE,
    RK_REPEAT_RXMUTE_UNMUTE,
} rk_rpt_rxmute_mode_t;

typedef enum
{
    RK_REPEAT_AUDIO_PATH_UNKNOW,
    RK_REPEAT_AUDIO_PATH_HDMI_0,
    RK_REPEAT_AUDIO_PATH_HDMI_1,
    RK_REPEAT_AUDIO_PATH_HDMI_2,
    RK_REPEAT_AUDIO_PATH_ARC_EARC,
    RK_REPEAT_AUDIO_PATH_EXT_I2S1,
    RK_REPEAT_AUDIO_PATH_EXT_I2S2,
    RK_REPEAT_AUDIO_PATH_EXT_I2S3,
    RK_REPEAT_AUDIO_PATH_EXT_SPDIF1,
    RK_REPEAT_AUDIO_PATH_EXT_SPDIF2,
    RK_REPEAT_AUDIO_PATH_EXT_SPDIF3,
    RK_REPEAT_AUDIO_PATH_MAX,
} rk_rpt_audio_path_t;

typedef enum
{
    RK_REPEAT_CEC_CTL_VOL_MUTE,
    RK_REPEAT_CEC_CTL_VOL_UP,
    RK_REPEAT_CEC_CTL_VOL_DOWN,
} rk_rpt_cec_cmd_t;

typedef enum
{
    RK_REPEAT_AUDIO_TYPE_LPCM = 0,   //raw data
    RK_REPEAT_AUDIO_TYPE_NLPCM = 1,  //DD,DTS5.1
    RK_REPEAT_AUDIO_TYPE_HBR = 2,    //Dolby TrueHD,DTS-HD
    RK_REPEAT_AUDIO_TYPE_DSD = 3,
} rk_rpt_audio_type_t;

typedef enum
{
    RK_REPEAT_POWER_OFF_MODE,
    RK_REPEAT_LOW_POWER_MODE,
    RK_REPEAT_POWER_ON_MODE,
} rk_rpt_power_mode_t;

typedef enum
{
    RK_REPEAT_NOT_PRESENT,
    RK_REPEAT_INIT,
    RK_REPEAT_READY,
    RK_REPEAT_ISP,
} rk_rpt_status_t;

typedef enum
{
    /*
     * will check fw version, Can only upgrade
     * to a higher version.
     */
    RK_REPEAT_ISP_NORMAL,
    /*
     * as long as a valid firmware is detected,
     * the upgrade will be executed.
     */
    RK_REPEAT_ISP_FORCE,
} rk_rpt_isp_mode_t;

struct rk_rpt_isp_op
{
    rk_rpt_isp_mode_t mode;
    char *fw_path;
};

typedef enum
{
    RK_REPEAT_CTL_BASE = 0x50,
    RK_REPEAT_CTL_ADO_PATH_GET,
    RK_REPEAT_CTL_ADO_PATH_SET,
    RK_REPEAT_CTL_E_ARC_VOL_SET,
    RK_REPEAT_CTL_E_ARC_VOL_GET,
    RK_REPEAT_CTL_REG_ADO_CHG_HOOK,
    RK_REPEAT_CTL_UNREG_ADO_CHG_HOOK,
    RK_REPEAT_CTL_REG_CEC_VOL_CHG_HOOK,
    RK_REPEAT_CTL_UNREG_CEC_VOL_CHG_HOOK,
    RK_REPEAT_CTL_REG_RX_MUTE_HOOK,
    RK_REPEAT_CTL_UNREG_RX_MUTE_HOOK,
    RK_REPEAT_CTL_POWER_MODE_CTRL,
    RK_REPEAT_CTL_GET_REPEAT_STATUS,
    RK_REPEAT_CTL_FW_UPDATE,
} rk_rpt_ctrl_t;

struct rk_rpt_audio_info
{
    rt_uint8_t audio_channel_number;        //channel number
    rt_uint32_t audio_sample_frequency;     //sample frequency
    rt_uint8_t audio_output_inetrface;      //0:i2s,1:spidf
    rt_uint8_t audio_type;                  //0:lpcm,1:nlpcm,2:hbr,3:dsd
    rt_uint8_t audio_sample_word_length;
    rt_uint8_t audio_in;                    //0:i2s,1:spdif,2,spdif from arc
};

struct hdmi_repeat;

struct hdmi_repeat_ops
{
    rt_err_t (*init)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*deinit)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*ado_path_get)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*ado_path_set)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*e_arc_vol_get)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*e_arc_vol_set)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*reg_ado_chg_hook)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*unreg_ado_chg_hook)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*reg_cec_vol_chg_hook)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*unreg_cec_vol_chg_hook)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*reg_rx_mute_hook)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*unreg_rx_mute_hook)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*power_mode_ctrl)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*get_repeat_status)(struct hdmi_repeat *rpt, void *data);
    rt_err_t (*fw_update)(struct hdmi_repeat *rpt, void *data);
};

struct hdmi_repeat
{
    void *private;
    const struct hdmi_repeat_ops *ops;
};

struct rk_hdmi_repeat
{
    struct rt_device dev;
    struct hdmi_repeat *rpt;
};

rt_err_t hdmi_repeat_register(struct hdmi_repeat *rpt);

#endif
