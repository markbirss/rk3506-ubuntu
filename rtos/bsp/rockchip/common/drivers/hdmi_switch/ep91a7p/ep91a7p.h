/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __EP91A7P_H__
#define __EP91A7P_H__

#include "../rk_hdmi_repeat.h"

struct ep91a7p_volume_info
{
    rt_uint8_t volume;
    rt_uint8_t is_mute;
};

typedef enum
{
    RT_EP91A7P_CTL_ARC_SET_MAX_VOL,
    RT_EP91A7P_CTL_PRIMARY_SEL,
    RT_EP91A7P_CTL_DUMP,
} ep91a7p_ctrl_t;

typedef enum
{
    EP91A7P_MODE_POWER_OFF,
    EP91A7P_MODE_POWER_ON,
} ep91a7p_power_mode_t;

struct ep91a7p_gi
{
    rt_uint8_t tx_info;
    rt_uint8_t video_latency;
};

struct ep91a7p_gc
{
    rt_uint8_t ctl;
    rt_uint8_t rx_sel;
    rt_uint8_t ctl2;
    rt_uint8_t cec_volume;
    rt_uint8_t link;
};

struct ep91a7p_ai
{
    rt_uint8_t system_status_0;
    rt_uint8_t system_status_1;
    rt_uint8_t audio_status;
    rt_uint8_t cs[5];
    rt_uint8_t cc;
    rt_uint8_t ca;
};

typedef struct
{
    struct rt_device dev;
    struct rt_i2c_client     *i2c_client;

    struct hdmi_repeat rpt;

    struct rk_rpt_audio_info ado_info;

    struct ep91a7p_gi gi;
    struct ep91a7p_gc gc;
    struct ep91a7p_ai ai;

    rt_uint8_t cur_arc_en;
    rt_uint8_t cur_earc_en;
    rt_uint8_t cur_hdmi_en;
    rt_uint8_t hdmi_rx_plug_in;
    rt_uint8_t hdmi_tx_plug_in;

    rk_rpt_audio_path_t last_ado_path;
    rk_rpt_audio_path_t last_vaild_ado_path;

    void (*audio_change_hook)(void *arg);
    void (*cec_vol_change_hook)(void *arg);
    void (*rx_mute_hook)(void *arg);

    rt_sem_t    isr_sem;
    rt_sem_t    rx_mute_sem;
    rt_thread_t int_tid;
    rt_thread_t rxmute_tid;

    rt_uint8_t dev_status;           //0:not present down,1:init,2:ready,3.isp
} ep91a7p_device_t;

#define EP91A7P_VOL_MAX      (100)
#define EP91A7P_VOL_DEFAULT  (30)

#define EP_VENDOR_ID_1         (0x00)
#define EP_VENDOR_ID_2         (0x01)
#define EP_DEVICE_ID_1         (0x02)
#define EP_DEVICE_ID_2         (0x03)
#define EP_VERSION_VER         (0x04)
#define EP_VERSION_YEAR        (0x05)
#define EP_VERSION_MONTH       (0x06)
#define EP_VERSION_DATE        (0x07)
#define EP_GENER_INF_0         (0x08)
#define EP_GENER_INF_1         (0x09)
#define EP_GENER_INF_2         (0x0A)
#define EP_GENER_INF_3         (0x0B)
#define EP_GENER_INF_4         (0x0C)
#define EP_GENER_INF_5         (0x0D)
#define EP_GENER_INF_6         (0x0E)
#define EP_ISP_MODE            (0x0F)
#define EP_GENERAL_CTL_0       (0x10)
#define EP_GENERAL_CTL_1       (0x11)
#define EP_GENERAL_CTL_2       (0x12)
#define EP_GENERAL_CTL_3       (0x13)
#define EP_GENERAL_CTL_4       (0x14)
#define EP_CEC_EVENT_CODE      (0x15)
#define EP_CEC_PARAM_1         (0x16)
#define EP_CEC_PARAM_2         (0x17)
#define EP_CEC_PARAM_3         (0x18)
#define EP_CEC_PARAM_4         (0x19)

#define EP_SYSTEM_STAT_0       (0x20)
#define EP_SYSTEM_STAT_1       (0x21)
#define EP_AUDIO_STAT          (0x22)
#define EP_CHANNEL_STAT_0      (0x23)
#define EP_CHANNEL_STAT_1      (0x24)
#define EP_CHANNEL_STAT_2      (0x25)
#define EP_CHANNEL_STAT_3      (0x26)
#define EP_CHANNEL_STAT_4      (0x27)
#define EP_ADO_INF_FRAME_0     (0x28)
#define EP_ADO_INF_FRAME_1     (0x29)
#define EP_ADO_INF_FRAME_2     (0x2A)
#define EP_ADO_INF_FRAME_3     (0x2B)
#define EP_ADO_INF_FRAME_4     (0x2C)
#define EP_ADO_INF_FRAME_5     (0x2D)

#define EP_HDMI_VS_1           (0x2E)
#define EP_HDMI_VS_2           (0x2F)

#define EP_ACP_PKT             (0x30)
#define EP_AVI_INF_FRAME_1     (0x31)
#define EP_AVI_INF_FRAME_2     (0x32)
#define EP_AVI_INF_FRAME_3     (0x33)
#define EP_AVI_INF_FRAME_4     (0x34)
#define EP_AVI_INF_FRAME_5     (0x35)
#define EP_GC_PKT_1            (0x36)
#define EP_GC_PKT_2            (0x37)
#define EP_GC_PKT_3            (0x38)

#define EP_ACTIVATION_KEY_1    (0x41)
#define EP_ACTIVATION_KEY_2    (0x42)
#define EP_ACTIVATION_KEY_3    (0x43)
#define EP_ACTIVATION_KEY_4    (0x44)
#define EP_ACTIVATION_KEY_5    (0x45)
#define EP_ACTIVATION_KEY_6    (0x46)
#define EP_ACTIVATION_KEY_7    (0x47)
#define EP_ACTIVATION_KEY_8    (0x48)
#define EP_ACTIVATION_KEY_9    (0x49)
#define EP_ACTIVATION_KEY_10   (0x4A)
#define EP_ACTIVATION_KEY_11   (0x4B)
#define EP_ACTIVATION_KEY_12   (0x4C)
#define EP_ACTIVATION_KEY_13   (0x4D)
#define EP_ACTIVATION_KEY_14   (0x4E)
#define EP_ACTIVATION_KEY_15   (0x4F)
#define EP_ACTIVATION_KEY_16   (0x50)

#define EP_EARC_LATENCY        (0x55)

#define EP_OSD_UPDATE_1        (0x56)
#define EP_OSD_UPDATE_2        (0x57)
#define EP_OSD_UPDATE_3        (0x58)
#define EP_OSD_UPDATE_4        (0x59)
#define EP_OSD_UPDATE_5        (0x5A)
#define EP_OSD_UPDATE_6        (0x5B)
#define EP_OSD_UPDATE_7        (0x5C)
#define EP_OSD_UPDATE_8        (0x5D)
#define EP_OSD_UPDATE_9        (0x5E)
#define EP_OSD_UPDATE_10       (0x5F)
#define EP_OSD_UPDATE_11       (0x60)
#define EP_OSD_UPDATE_12       (0x61)
#define EP_OSD_UPDATE_13       (0x62)
#define EP_OSD_UPDATE_14       (0x63)
#define EP_OSD_UPDATE_15       (0x64)

#define EP_INFO                (0x65)
#define EP_CTL                 (0x66)

#define EP_2CHOICE_MASK        1
#define EP_GC_CEC_VOLUME_MIN   0
#define EP_GC_CEC_VOLUME_MAX   100
#define EP_AI_RATE_MIN         0
#define EP_AI_RATE_MAX         768000
#define EP_AI_CH_COUNT_MIN     0
#define EP_AI_CH_COUNT_MAX     8
#define EP_AI_CH_ALLOC_MIN     0
#define EP_AI_CH_ALLOC_MAX     0xff

/* shift/masks for register bits
 * GI = General Info
 * GC = General Control
 * AI = Audio Info
 */
#define EP_GI_ADO_CHF_MASK        0x01
#define EP_GI_CEC_ECF_MASK        0x02
#define EP_GI_ARC_ON_SHIFT        0
#define EP_GI_ARC_ON_MASK         0x01
#define EP_GI_EARC_ON_SHIFT       1
#define EP_GI_EARC_ON_MASK        0x02
#define EP_GI_EARC_SEL_SHIFT      2
#define EP_GI_EARC_SEL_MASK       0x04
#define EP_GI_TX_HOT_PLUG_SHIFT   7
#define EP_GI_TX_HOT_PLUG_MASK    0x80
#define EP_GI_VIDEO_LATENCY_SHIFT 0
#define EP_GI_VIDEO_LATENCY_MASK  0xff

#define EP_GC_POWER_SHIFT      7
#define EP_GC_POWER_MASK       0x80
#define EP_GC_EARC_EN_SHIFT    6
#define EP_GC_EARC_EN_MASK     0x40
#define EP_GC_AUDIO_PATH_SHIFT 5
#define EP_GC_AUDIO_PATH_MASK  0x20
#define EP_GC_CEC_DIS_SHIFT    2
#define EP_GC_CEC_DIS_MASK     0x04
#define EP_GC_CEC_MUTE_SHIFT   1
#define EP_GC_CEC_MUTE_MASK    0x02
#define EP_GC_ARC_EN_SHIFT     0
#define EP_GC_ARC_EN_MASK      0x01
#define EP_GC_ARC_DIS_SHIFT    6
#define EP_GC_ARC_DIS_MASK     0x40
#define EP_GC_EARC_DIS_SHIFT   5
#define EP_GC_EARC_DIS_MASK    0x20
#define EP_GC_RX_SEL_SHIFT     0
#define EP_GC_RX_SEL_MASK      0x07
#define EP_GC_CEC_VOLUME_SHIFT 0
#define EP_GC_CEC_VOLUME_MASK  0xff
#define EP_GC_LINK_ON0_SHIFT   0
#define EP_GC_LINK_ON0_MASK    0x01
#define EP_GC_LINK_ON1_SHIFT   1
#define EP_GC_LINK_ON1_MASK    0x02
#define EP_GC_LINK_ON2_SHIFT   2
#define EP_GC_LINK_ON2_MASK    0x04

#define EP_AI_MCLK_ON_SHIFT    6
#define EP_AI_MCLK_ON_MASK     0x40
#define EP_AI_AVMUTE_SHIFT     5
#define EP_AI_AVMUTE_MASK      0x20
#define EP_AI_EARC_MUTE_SHIFT  2
#define EP_AI_EARC_MUTE_MASK   0x04
#define EP_AI_LAYOUT_SHIFT     0
#define EP_AI_LAYOUT_MASK      0x01
#define EP_AI_DSD_RATE_SHIFT   4
#define EP_AI_DSD_RATE_MASK    0x30
#define EP_AI_EARC_CON_SHIFT   0
#define EP_AI_EARC_CON_MASK    0x01
#define EP_AI_COMP_ADO_SHIFT   7
#define EP_AI_COMP_ADO_MASK    0x80
#define EP_AI_HBR_ADO_SHIFT    5
#define EP_AI_HBR_ADO_MASK     0x20
#define EP_AI_DSD_ADO_SHIFT    4
#define EP_AI_DSD_ADO_MASK     0x10
#define EP_AI_STD_ADO_SHIFT    3
#define EP_AI_STD_ADO_MASK     0x08
#define EP_AI_RATE_MASK        0x07
#define EP_AI_NPCM_MASK        0x02
#define EP_AI_PREEMPH_SHIFT    3
#define EP_AI_PREEMPH_MASK     0x38
#define EP_AI_EARC_MODE_MASK   0x3b
#define EP_AI_EARC_MODE_LPCM_2CH   0x00
#define EP_AI_EARC_MODE_LPCM_MCH   0x20
#define EP_AI_EARC_MODE_IEC61937   0x02
#define EP_AI_EARC_RATE_SHIFT  0
#define EP_AI_EARC_RATE_MASK   0xcf
#define EP_AI_CH_COUNT_MASK    0x07
#define EP_AI_CH_ALLOC_MASK    0xff

#define EP_STATUS_NO_SIGNAL           0
#define EP_STATUS_AUDIO_ACTIVE        1
#endif

