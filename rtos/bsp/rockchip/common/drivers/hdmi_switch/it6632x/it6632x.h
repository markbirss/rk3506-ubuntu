/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef __IT6632X_H__
#define __IT6632X_H__

#include "../rk_hdmi_repeat.h"

#define IT6632x_THREAD_STACK_SIZE       2048
#define IT6632x_THREAD_PRIORITY         10

typedef enum
{
    IT6632X_MODE_STANDBY,
    IT6632X_MODE_PASSTHROUGH,
    IT6632X_MODE_POWER_ON,
    IT6632X_MODE_POWER_OFF,
    IT6632X_MODE_POWER_MAX,
} it6632x_power_mode_t;

typedef enum
{
    IT6632X_AUDIO_CHANNEL_COUNT_0 = 0,
    IT6632X_AUDIO_CHANNEL_COUNT_2 = 2,
    IT6632X_AUDIO_CHANNEL_COUNT_3 = 3,
    IT6632X_AUDIO_CHANNEL_COUNT_4 = 4,
    IT6632X_AUDIO_CHANNEL_COUNT_5 = 5,
    IT6632X_AUDIO_CHANNEL_COUNT_6 = 6,
    IT6632X_AUDIO_CHANNEL_COUNT_7 = 7,
    IT6632X_AUDIO_CHANNEL_COUNT_8 = 8,
    IT6632X_AUDIO_CHANNEL_COUNT_10 = 10,
    IT6632X_AUDIO_CHANNEL_COUNT_12 = 12,
} it6632x_audio_channel_count_t;

typedef enum
{
    IT6632X_AUDIO_SAMPLE_RATE_44_1k = 0x00,
    IT6632X_AUDIO_SAMPLE_RATE_48k = 0x02,
    IT6632X_AUDIO_SAMPLE_RATE_32k = 0x03,
    IT6632X_AUDIO_SAMPLE_RATE_384k = 0x05,
    IT6632X_AUDIO_SAMPLE_RATE_88_2k = 0x08,
    IT6632X_AUDIO_SAMPLE_RATE_768k = 0x09,
    IT6632X_AUDIO_SAMPLE_RATE_96k = 0x0A,
    IT6632X_AUDIO_SAMPLE_RATE_64k = 0x0B,
    IT6632X_AUDIO_SAMPLE_RATE_176_4k = 0x0C,
    IT6632X_AUDIO_SAMPLE_RATE_352k = 0x0D,
    IT6632X_AUDIO_SAMPLE_RATE_192k = 0x0E,
    IT6632X_AUDIO_SAMPLE_RATE_1536k = 0x0F,
    IT6632X_AUDIO_SAMPLE_RATE_256k = 0x1B,
    IT6632X_AUDIO_SAMPLE_RATE_1411k = 0x1D,
    IT6632X_AUDIO_SAMPLE_RATE_128k = 0x2B,
    IT6632X_AUDIO_SAMPLE_RATE_705k = 0x2D,
    IT6632X_AUDIO_SAMPLE_RATE_1024k = 0x35,
    IT6632X_AUDIO_SAMPLE_RATE_512k = 0x3B,
    IT6632X_AUDIO_SAMPLE_RATE_REFER_HEADER = 0x3E,
    IT6632X_AUDIO_SAMPLE_RATE_ERR = 0x3F,
} it6632x_audio_sample_rate_t;

typedef enum
{
    IT6632X_AUDIO_INTERFACE_I2S = 0,
    IT6632X_AUDIO_INTERFACE_SPDIF = 1,
} it6632x_output_interface_t;

typedef enum
{
    IT6632X_AUDIO_BITS_16 = 0,
    IT6632X_AUDIO_BITS_18 = 1,
    IT6632X_AUDIO_BITS_20 = 2,
    IT6632X_AUDIO_BITS_24 = 3,
} it6632x_audio_bits_t;

typedef enum
{
    IT6632X_AUDIO_MAX20_WORD_LENGTH_NO_INDICATED = 0,
    IT6632X_AUDIO_MAX24_WORD_LENGTH_NO_INDICATED = 1,
    IT6632X_AUDIO_MAX20_WORD_LENGTH_16 = 2,
    IT6632X_AUDIO_MAX24_WORD_LENGTH_20 = 3,
    IT6632X_AUDIO_MAX20_WORD_LENGTH_18 = 4,
    IT6632X_AUDIO_MAX24_WORD_LENGTH_22 = 5,
    IT6632X_AUDIO_MAX20_WORD_LENGTH_19 = 8,
    IT6632X_AUDIO_MAX24_WORD_LENGTH_23 = 9,
    IT6632X_AUDIO_MAX20_WORD_LENGTH_20 = 0x0a,
    IT6632X_AUDIO_MAX24_WORD_LENGTH_24 = 0x0b,
    IT6632X_AUDIO_MAX20_WORD_LENGTH_17 = 0x0c,
    IT6632X_AUDIO_MAX24_WORD_LENGTH_21 = 0x0d,
} it6632x_ext_audio_bits_t;

struct ado_i2s_format
{
    rt_uint8_t is_32_bit_wide;                           //always "1":I2S 32-bit wide
    rt_uint8_t left_right_justified;                     //0:left justified 1:right justified
    rt_uint8_t data_1T_delay_to_ws;                      //0:data 1t delay correspond to ws,1:data no delay correspond to ws
    rt_uint8_t ws_is_left_or_right_channel;              //0:ws = 0 is left channel, 1:WS = 0 is right channel
    rt_uint8_t msb_or_lsb;                               //0:msb shift first, 1:lsb shift first
};

struct it6632x_ado_info
{
    it6632x_audio_channel_count_t audio_channel_number;
    it6632x_audio_sample_rate_t audio_sample_frequency;
    it6632x_output_interface_t audio_output_inetrface;
    rk_rpt_audio_type_t audio_type;
    it6632x_audio_bits_t audio_sample_word_length;
    struct ado_i2s_format i2s_format;
    rt_uint8_t is_multi_stream_audio;                    //1:is multi stream audio
    rt_uint8_t is_tdm;                                   //1:is tdm
    rt_uint8_t audio_layout;                             //0:layout 0, 1:layout 1
    rt_uint8_t is_3d_audio;                              //1:is 3d audio
    rt_uint8_t new_auido_input;                          //new audio received
    /*
     * audio_sample_word_length_1:
     * 0 max20 word length no indicated
     * 1 max24 word length no indicated
     * 2 max20 word length 16
     * 3 max24 word length 20
     * 4 max20 word length 18
     * 5 max24 word length 22
     * 8 max20 word length 19
     * 9 max24 word length 23
     * 0x0a max20 word length 20
     * 0x0b max24 word length 24
     * 0x0c max20 word length 17
     * 0x0d max24 word length 21
     */
    it6632x_ext_audio_bits_t audio_sample_word_length_1;
    rt_uint8_t ca;                                         //refer to CEA-861
};

struct audio_info
{
    rt_uint8_t audio_channel_number;        //channel number
    rt_uint32_t audio_sample_frequency;     //sample frequency
    rt_uint8_t audio_output_inetrface;      //0:i2s,1:spidf
    rt_uint8_t audio_type;                  //0:lpcm,1:nlpcm,2:hbr,3:dsd
    rt_uint8_t audio_sample_word_length;    //0:16bit,1:18bit,2:20bit,3:24bit
    struct ado_i2s_format i2s_format;
    rt_uint8_t is_multi_stream_audio;       //1:is multi stream audio
    rt_uint8_t is_tdm;                      //1:is tdm
    rt_uint8_t audio_layout;                //0:layout 0, 1:layout 1
    rt_uint8_t is_3d_audio;                 //1:is 3d audio
    /*
     * audio_sample_word_length_1:
     * 0 max20 word length no indicated
     * 1 max24 word length no indicated
     * 2 max20 word length 16
     * 3 max24 word length 20
     * 4 max20 word length 18
     * 5 max24 word length 22
     * 8 max20 word length 19
     * 9 max24 word length 23
     * 0x0a max20 word length 20
     * 0x0b max24 word length 24
     * 0x0c max20 word length 17
     * 0x0d max24 word length 21
     */
    rt_uint8_t audio_sample_word_length_1;
    rt_uint8_t ca;                         //refer to CEA-861
    rt_uint8_t audio_in;                   //0:i2s,1:spdif,2,spdif from arc
};

typedef enum
{
    IT6632X_SPK_CH_FL_FR   = 1 << 0, // Front Left & Front Right
    IT6632X_SPK_CH_LFE     = 1 << 1, // Low Frequency Effect
    IT6632X_SPK_CH_FC      = 1 << 2, // Front center
    IT6632X_SPK_CH_RL_RR   = 1 << 3, // Rear Left & Rear Right
    IT6632X_SPK_CH_RC      = 1 << 4, // Rear Center
    IT6632X_SPK_CH_FLC_FRC = 1 << 5, // Front Left Center and Front Right Center
    IT6632X_SPK_CH_RLC_RRC = 1 << 6, // Rear Left Center and Rear Right Center
} it6632x_speaker_allocation_t;

typedef struct cec_cmd
{
    rt_uint8_t *cmd;
    rt_uint8_t len;
} cec_cmd_t;

struct it6632x_status
{
    rt_uint8_t power;                //0:power down,1:power up
    rt_uint8_t rx_sel;               //hdmi rx port select, 0:Rx0, 1:Rx1 2:Rx2 3:disable tx
    rt_uint8_t ado_decode_sel;       //hdmi audio decoder source select,0:Rx0,1:Rx1,2:Rx2,other: disable audio decoder
    rt_uint8_t tx_dis_out;           //0:hdmi tx enable output,1:hdmi Tx disable output
    rt_uint8_t tx_ado_mute;          //0:hdmi tx output with audio,1:hdmi tx output without audio
    rt_uint8_t tv_cec;               //tv's cec status,0:cec off,1:cec on
    rt_uint8_t sb_cec;               //sb cec status,0:cec off,1:cec on
    rt_uint8_t audio_system_enable;  //0:audio system off,1:audio system on
    rt_uint8_t earc_arc_enable;      //0:earc/arc disable,1:earc/crc enable
    rt_uint8_t hdmi_audio_ready;     //hdmi audio status,0:no hdmi audio,1:hdmi audio out
    rt_uint8_t earc_audio_ready;     //earc status,0:no earc audio,1:earc audio out
    rt_uint8_t arc_audio_ready;      //0:arc off,1:arc on
    /*
     * audio_source_sel:
     * 0:hdmi,1:arc/earc,2:ext_i2s1,3:ext_i2s2,
     * 4:4xt_i2s3,5:ext_spdif1,6:ext_spdif2,
     * 7:ext_spdif3,8:ext_spdif4,other:no output
     */
    rt_uint8_t audio_source_sel;
    rt_uint8_t dev_status;           //0:not present down,1:init,2:ready,3.isp
};

typedef struct
{
    struct rt_device dev;
    struct rt_i2c_client     *i2c_client;
    struct rt_i2c_bus_device *i2c_bus;
    struct it6632x_ado_info ado_info;
    struct audio_info audio_brief_info;    //brief info
    struct rk_rpt_audio_info rk_ado_info;
    struct it6632x_status status;

    struct hdmi_repeat rpt;

    void (*audio_change_hook)(void *arg);
    void (*cec_vol_change_hook)(void *arg);
    void (*rx_mute_hook)(void *arg);
    rt_sem_t    isr_sem;
    rt_sem_t    rx_mute_sem;
    rt_thread_t int_tid;
    rt_thread_t rxmute_tid;
    rt_mutex_t  mutex_lock;

} it6632x_device_t;

typedef enum
{
    RT_IT6632X_CTRL_ARC_SET_MAX_VOL,
    RT_IT6632X_CTRL_REG_DUMP,
    RT_IT6632X_CTRL_AUDIO_SYSTEM_ENABLE,
    RT_IT6632X_CTRL_EARC_ENABLE,
    RT_IT6632X_CTRL_CEC_ENABLE,
    RT_IT6632X_CTRL_CEC_CMD_SEND,
    RT_IT6632X_CTRL_TX_AUDIO_MUTE,
    RT_IT6632X_CTRL_TX_VIDEO_MUTE,
    RT_IT6632X_CTRL_HDCP_RPT_ENABLE,

    RT_IT6632X_CTRL_GET_STATUS,
    RT_IT6632X_CTRL_HDMI_GET_DECODERINFO,
} it6632x_ctrl_t;

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif

#define SB_I2C_NO_ITE   (0)
#define SB_I2C_IT66322  (1)
#define SB_I2C_IT66323  (2)
#define SB_I2C_IT66324  (3)
#define SB_I2C_IT6622A  (4)
#define SB_I2C_IT6622B  (5)
#define SB_I2C_IT66320  (6)

#define IT66322_CHIP_ID_0           (0x66)
#define IT66322_CHIP_ID_1           (0x32)
#define IT66322_CHIP_ID_2           (0x20)

#define IT66323_CHIP_ID_0           (0x66)
#define IT66323_CHIP_ID_1           (0x32)
#define IT66323_CHIP_ID_2           (0x30)

#define IT66324_CHIP_ID_0           (0x66)
#define IT66324_CHIP_ID_1           (0x32)
#define IT66324_CHIP_ID_2           (0x40)

#define IT6622A_CHIP_ID_0           (0x66)
#define IT6622A_CHIP_ID_1           (0x22)
#define IT6622A_CHIP_ID_2           (0x00)

#define IT6622B_CHIP_ID_0           (0x66)
#define IT6622B_CHIP_ID_1           (0x22)
#define IT6622B_CHIP_ID_2           (0xB0)

#define IT66320_CHIP_ID_0           (0x66)
#define IT66320_CHIP_ID_1           (0x32)
#define IT66320_CHIP_ID_2           (0x00)

#if (IS_IT663XX == IT66322)
#define ITE_CHIP_ID_0           (IT66322_CHIP_ID_0)
#define ITE_CHIP_ID_1           (IT66322_CHIP_ID_1)
#define ITE_CHIP_ID_2           (IT66322_CHIP_ID_2)
#elif (IS_IT663XX == IT66323)
#define ITE_CHIP_ID_0           (IT66323_CHIP_ID_0)
#define ITE_CHIP_ID_1           (IT66323_CHIP_ID_1)
#define ITE_CHIP_ID_2           (IT66323_CHIP_ID_2)
#else
#define ITE_CHIP_ID_0           (IT66322_CHIP_ID_0)
#define ITE_CHIP_ID_1           (IT66322_CHIP_ID_1)
#define ITE_CHIP_ID_2           (IT66322_CHIP_ID_2)
#endif


#define ITE_INT_INTERVAL        (5)

#define SB_CEC_AUTO             (0)     // follow TV
#define SB_CEC_OFF              (1)

#define SB_POWER_OFF            (0)
#define SB_POWER_ON             (1)
#define SB_STANDBY              (2)
#define SB_PASS_THROUGH         (3)

#define SB_AUDIO_ARC            (0x04000400)

#define I2C_ITE_CHIP            (0x00)
#define I2C_FW_MAJOR_VERSION    (0x03)
#define I2C_FW_MINOR_VERSION    (0x06)
#define I2C_ADO_SRC_STA         (0x10)
#define I2C_ADO_VOL             (0x11)
#define I2C_ADO_DIS_OUT         (0x12)
#define I2C_VDO_LATENCY         (0x13)
#define I2C_LATENCY_FLAGS       (0x14)
#define I2C_ADO_OUTPUT_DELAY    (0x15)
#define I2C_ADO_INFO            (0x16)
#define I2C_ADO_CA              (0x1A)
#define I2C_VOLUME_MAX          (0x1B)
#define I2C_ADO_SEL             (0x1C)
#define I2C_MCLK_FREQ_SEL       (0x1D)
#define I2C_ADO_INFOPKT         (0x1E)
#define I2C_ADO_CHSTA           (0x24)
#define I2C_EARC_LATEN          (0x29)
#define I2C_EARC_LATEN_REQ      (0x2A)
#define I2C_EDID_UPDATE         (0x40)
#define I2C_EDID_VPID           (0x41)
#define I2C_EDID_PNAME          (0x4B)
#define I2C_EDID_PDESC          (0x5D)
#define I2C_EDID_SPK_ALLOC      (0x6F)
#define I2C_EDID_ADB0           (0x73)
#define I2C_EDID_ADB1           (0x92)
#define I2C_EDID_V_LAT          (0xB1)
#define I2C_EDID_A_LAT          (0xB2)
#define I2C_EDID_IV_LAT         (0xB3)
#define I2C_EDID_IA_LAT         (0xB4)
#define I2C_EDID_PA_AB          (0xBA)
#define I2C_EDID_PA_CD          (0xBB)
#define I2C_TV_V_LAT            (0xBC)
#define I2C_TV_A_LAT            (0xBD)
#define I2C_TV_IV_LAT           (0xBE)
#define I2C_TV_IA_LAT           (0xBF)
#define I2C_CEC_TRANS_DATA      (0xD0)
#define I2C_CEC_LATCH_DATA      (0xE0)
#define I2C_SYS_RX_SEL          (0xF0)
#define I2C_SYS_ADO_MODE        (0xF1)
#define I2C_SYS_TX_STA          (0xF2)
#define I2C_SYS_CEC_RECEIVE     (0xF3)
#define I2C_SYS_CEC_RECEIVE2    (0xF4)
#define I2C_SYS_RX_STATUS       (0xF5)
#define I2C_SYS_RX_MODE_0       (0xF6)
#define I2C_SYS_RX_MODE_1       (0xF7)
#define I2C_SYS_RX_MODE_2       (0xF8)
#define I2C_SYS_RX_MODE_3       (0xF9)
#define I2C_SYS_ACTIVE_PA       (0xFB)
#define I2C_SYS_CEC_LATCH_CNT_6622  (0xFB)  // for 6622 only
#define I2C_SYS_CEC_TRANS_CNT   (0xFC)
#define I2C_SYS_CEC_LATCH_CNT   (0xFD)
#define I2C_SYS_CHANGE          (0xFE)
#define I2C_SYS_INT             (0xFF)
/***********  Version 0x00 ~ 0x08 ***********/
/***********  Chip Information 0x00 ~ 0x02 ***********/
/***********  Firmware Major Version 0x03 ~ 0x04 ***********/
/***********  Reserved 0x05 ***********/
/***********  Firmware Minor Version 0x06 ~ 0x08 ***********/
/***********  Reserved 0x09 ~ 0x0F ***********/
/***********  Audio Active 0x10 ***********/
#define I2C_ADO_ACTIVE_ARC_SHIFT    (0)
#define I2C_ADO_ACTIVE_ARC_MASK     (1 << I2C_ADO_ACTIVE_ARC_SHIFT)
#define I2C_ADO_ACTIVE_ARC_CLR      (0 << I2C_ADO_ACTIVE_ARC_SHIFT)
#define I2C_ADO_ACTIVE_ARC_SET      (1 << I2C_ADO_ACTIVE_ARC_SHIFT)

#define I2C_ADO_ACTIVE_EARC_SHIFT   (1)
#define I2C_ADO_ACTIVE_EARC_MASK    (1 << I2C_ADO_ACTIVE_EARC_SHIFT)
#define I2C_ADO_ACTIVE_EARC_CLR     (0 << I2C_ADO_ACTIVE_EARC_SHIFT)
#define I2C_ADO_ACTIVE_EARC_SET     (1 << I2C_ADO_ACTIVE_EARC_SHIFT)

#define I2C_ADO_ACTIVE_HDMI_SHIFT   (2)
#define I2C_ADO_ACTIVE_HDMI_MASK    (1 << I2C_ADO_ACTIVE_HDMI_SHIFT)
#define I2C_ADO_ACTIVE_HDMI_CLR     (0 << I2C_ADO_ACTIVE_HDMI_SHIFT)
#define I2C_ADO_ACTIVE_HDMI_SET     (1 << I2C_ADO_ACTIVE_HDMI_SHIFT)
// (3) ~ (7) Reserved
#define I2C_ADO_EARC_CONNECT_SHIFT  (4)
#define I2C_ADO_EARC_CONNECT_MASK   (1 << I2C_ADO_EARC_CONNECT_SHIFT)
#define I2C_ADO_EARC_CONNECT_CLR    (0 << I2C_ADO_EARC_CONNECT_SHIFT)
#define I2C_ADO_EARC_CONNECT_SET    (1 << I2C_ADO_EARC_CONNECT_SHIFT)
/***********  Audio Volume 0x11 ***********/
#define I2C_ADO_VOL_VALUE_SHIFT     (0)
#define I2C_ADO_VOL_VALUE_MASK      (0x7F << I2C_ADO_VOL_VALUE_SHIFT)

#define I2C_ADO_VOL_MUTE_SHIFT      (7)
#define I2C_ADO_VOL_MUTE_MASK       (1 << I2C_ADO_VOL_MUTE_SHIFT)
#define I2C_ADO_VOL_MUTE_CLR        (0 << I2C_ADO_VOL_MUTE_SHIFT)
#define I2C_ADO_VOL_MUTE_SET        (1 << I2C_ADO_VOL_MUTE_SHIFT)
/***********  Reserved 0x12 ***********/
/***********  Video Latency 0x13 ***********/
/***********  Latency Flags 0x14 ***********/
/***********  Audio Output Delay 0x15 ***********/
/***********  Audio Information 0x16 ~ 0x19 ***********/
/***********  Audio Channel Allocation 0x1A ***********/
/***********  SB Volume Max. 0x1B ***********/
/***********  Audio Mux. Select 0x1C ***********/
#define I2C_ADO_MUX_SEL_SHIFT       (0)
#define I2C_ADO_MUX_SEL_MASK        (0xF << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_HDMI        (0 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EARC        (1 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_I2S1    (2 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_I2S2    (3 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_I2S3    (4 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_SPDIF1  (5 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_SPDIF2  (6 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_SPDIF3  (7 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_SPDIF4  (8 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_MAX     (8 << I2C_ADO_MUX_SEL_SHIFT)

#define I2C_ADO_VOL_SKIP_SEND_SHIFT (4)
#define I2C_ADO_VOL_SKIP_SEND_MASK  (1 << I2C_ADO_VOL_SKIP_SEND_SHIFT)
#define I2C_ADO_VOL_SKIP_SEND_SET   (1 << I2C_ADO_VOL_SKIP_SEND_SHIFT)
#define I2C_ADO_VOL_SKIP_SEND_CLR   (0)
// (5) ~ (7) Reserved
/***********  MCLK Frequency Select 0x1D ***********/
#define I2C_ADO_MCLK_FREQ_SHIFT     (0)
#define I2C_ADO_MCLK_FREQ_MASK      (7 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_128FS     (0 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_256FS     (1 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_384FS     (2 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_512FS     (3 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_640FS     (4 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_768FS     (5 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_894FS     (6 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_1024FS    (7 << I2C_ADO_MCLK_FREQ_SHIFT)
/***********  Audio InforFrame Packet 0x1E ~ 0x23 ***********/
/***********  Audio Channel Status 0x24 ~ 0x28 ***********/
/***********  Reserved 0x29 ~ 0x2F ***********/
/***********  SB EDID 0x40 ~ 0xBF ***********/
#define I2C_EDID_UPD_PID_SHIFT  (0)
#define I2C_EDID_UPD_PID_MASK   (1 << I2C_EDID_UPD_PID_SHIFT)
#define I2C_EDID_UPD_PID_CLR    (0 << I2C_EDID_UPD_PID_SHIFT)
#define I2C_EDID_UPD_PID_SET    (1 << I2C_EDID_UPD_PID_SHIFT)

#define I2C_EDID_UPD_PNAME_SHIFT    (1)
#define I2C_EDID_UPD_PNAME_MASK     (1 << I2C_EDID_UPD_PNAME_SHIFT)
#define I2C_EDID_UPD_PNAME_CLR      (0 << I2C_EDID_UPD_PNAME_SHIFT)
#define I2C_EDID_UPD_PNAME_SET      (1 << I2C_EDID_UPD_PNAME_SHIFT)

#define I2C_EDID_UPD_ADB0_SHIFT     (3)
#define I2C_EDID_UPD_ADB0_MASK      (1 << I2C_EDID_UPD_ADB0_SHIFT)
#define I2C_EDID_UPD_ADB0_CLR       (0 << I2C_EDID_UPD_ADB0_SHIFT)
#define I2C_EDID_UPD_ADB0_SET       (1 << I2C_EDID_UPD_ADB0_SHIFT)

#define I2C_EDID_UPD_ADB1_SHIFT     (4)
#define I2C_EDID_UPD_ADB1_MASK      (1 << I2C_EDID_UPD_ADB1_SHIFT)
#define I2C_EDID_UPD_ADB1_CLR       (0 << I2C_EDID_UPD_ADB1_SHIFT)
#define I2C_EDID_UPD_ADB1_SET       (1 << I2C_EDID_UPD_ADB1_SHIFT)

#define I2C_EDID_UPD_LATENCY_SHIFT  (5)
#define I2C_EDID_UPD_LATENCY_MASK   (1 << I2C_EDID_UPD_LATENCY_SHIFT)
#define I2C_EDID_UPD_LATENCY_CLR    (0 << I2C_EDID_UPD_LATENCY_SHIFT)
#define I2C_EDID_UPD_LATENCY_SET    (1 << I2C_EDID_UPD_LATENCY_SHIFT)
/***********  Reserved 0xC0 ~ 0xCF ***********/
/***********  CEC Fire Command Data 0xD0 ~ 0xDF ***********/
/***********  CEC Latch Command Data 0xE0 ~ 0xEF ***********/
/***********  HDMI Rx Select 0xF0 ***********/
#define I2C_HDMI_SELECT_SHIFT   (0)
#define I2C_HDMI_SELECT_MASK    (3 << I2C_HDMI_SELECT_SHIFT)
#define I2C_HDMI_SELECT_R0      (0 << I2C_HDMI_SELECT_SHIFT)
#define I2C_HDMI_SELECT_R1      (1 << I2C_HDMI_SELECT_SHIFT)
#define I2C_HDMI_SELECT_R2      (2 << I2C_HDMI_SELECT_SHIFT)
#define I2C_HDMI_SELECT_NONE    (3 << I2C_HDMI_SELECT_SHIFT)

#define I2C_HDMI_SEL_W_SHIFT    (2)
#define I2C_HDMI_SEL_W_MASK     (3 << I2C_HDMI_SEL_W_SHIFT)
#define I2C_HDMI_SEL_W_NONE     (0 << I2C_HDMI_SEL_W_SHIFT)
#define I2C_HDMI_SEL_W_CHG      (1 << I2C_HDMI_SEL_W_SHIFT)
#define I2C_HDMI_SEL_W_INF      (2 << I2C_HDMI_SEL_W_SHIFT)
#define I2C_HDMI_SEL_W_RES      (3 << I2C_HDMI_SEL_W_SHIFT)

// (2) ~ (3) reserved
#define I2C_HDMI_ADO_SEL_SHIFT  (4)
#define I2C_HDMI_ADO_SEL_MASK   (3 << I2C_HDMI_ADO_SEL_SHIFT)
#define I2C_HDMI_ADO_SEL_R0     (0 << I2C_HDMI_ADO_SEL_SHIFT)
#define I2C_HDMI_ADO_SEL_R1     (1 << I2C_HDMI_ADO_SEL_SHIFT)
#define I2C_HDMI_ADO_SEL_R2     (2 << I2C_HDMI_ADO_SEL_SHIFT)
#define I2C_HDMI_ADO_SEL_NONE   (0xF << I2C_HDMI_ADO_SEL_SHIFT)
// (5) ~ (6) reserved
/***********  Audio Mode 0xF1 ***********/
#define I2C_MODE_POWER_SHFIT    (0)
#define I2C_MODE_POWER_MASK     (1 << I2C_MODE_POWER_SHFIT)
#define I2C_MODE_POWER_DOWN     (0 << I2C_MODE_POWER_SHFIT)
#define I2C_MODE_POWER_ON       (1 << I2C_MODE_POWER_SHFIT)
// (1) reserved
#define I2C_MODE_ARC_ONLY_SHIFT (2)
#define I2C_MODE_ARC_ONLY_MASK  (1 << I2C_MODE_ARC_ONLY_SHIFT)
#define I2C_MODE_ARC_ONLY_SET   (1 << I2C_MODE_ARC_ONLY_SHIFT)
#define I2C_MODE_ARC_ONLY_CLR   (0)

#define I2C_MODE_ADO_SYS_SHIFT  (3)
#define I2C_MODE_ADO_SYS_MASK   (1 << I2C_MODE_ADO_SYS_SHIFT)
#define I2C_MODE_ADO_SYS_DIS    (0 << I2C_MODE_ADO_SYS_SHIFT)
#define I2C_MODE_ADO_SYS_EN     (1 << I2C_MODE_ADO_SYS_SHIFT)

#define I2C_MODE_EARC_SHIFT     (4)
#define I2C_MODE_EARC_MASK      (1 << I2C_MODE_EARC_SHIFT)
#define I2C_MODE_EARC_DIS       (0 << I2C_MODE_EARC_SHIFT)
#define I2C_MODE_EARC_EN        (1 << I2C_MODE_EARC_SHIFT)

#define I2C_MODE_CEC_SHIFT      (5)
#define I2C_MODE_CEC_MASK       (1 << I2C_MODE_CEC_SHIFT)
#define I2C_MODE_CEC_DIS        (0 << I2C_MODE_CEC_SHIFT)
#define I2C_MODE_CEC_EN         (1 << I2C_MODE_CEC_SHIFT)

#define I2C_MODE_TX_ADO_SHIFT   (6)
#define I2C_MODE_TX_ADO_MASK    (1 << I2C_MODE_TX_ADO_SHIFT)
#define I2C_MODE_TX_ADO_EN      (0 << I2C_MODE_TX_ADO_SHIFT)
#define I2C_MODE_TX_ADO_DIS     (1 << I2C_MODE_TX_ADO_SHIFT)

#define I2C_MODE_TX_OUT_SHIFT   (7)
#define I2C_MODE_TX_OUT_MASK    (1 << I2C_MODE_TX_OUT_SHIFT)
#define I2C_MODE_TX_OUT_EN      (0 << I2C_MODE_TX_OUT_SHIFT)
#define I2C_MODE_TX_OUT_DIS     (1 << I2C_MODE_TX_OUT_SHIFT)
/***********  TV Status 0xF2 ***********/
#define I2C_TV_STA_HPD_SHIFT        (0)
#define I2C_TV_STA_HPD_MASK         (1 << I2C_TV_STA_HPD_SHIFT)
#define I2C_TV_STA_HPD_CLR          (0 << I2C_TV_STA_HPD_SHIFT)
#define I2C_TV_STA_HPD_SET          (1 << I2C_TV_STA_HPD_SHIFT)

#define I2C_TV_STA_CEC_SHIFT        (1)
#define I2C_TV_STA_CEC_MASK         (1 << I2C_TV_STA_CEC_SHIFT)
#define I2C_TV_STA_CEC_OFF          (0 << I2C_TV_STA_CEC_SHIFT)
#define I2C_TV_STA_CEC_ON           (1 << I2C_TV_STA_CEC_SHIFT)

#define I2C_TV_STA_REQ_SWITCH_SHIFT     (2)
#define I2C_TV_STA_REQ_SWITCH_MASK      (3 << I2C_TV_STA_REQ_SWITCH_SHIFT)
#define I2C_TV_STA_REQ_SWITCH_TV        (3 << I2C_TV_STA_REQ_SWITCH_SHIFT)
#define I2C_TV_STA_REQ_SWITCH_R0        (0 << I2C_TV_STA_REQ_SWITCH_SHIFT)
#define I2C_TV_STA_REQ_SWITCH_R1        (1 << I2C_TV_STA_REQ_SWITCH_SHIFT)
#define I2C_TV_STA_REQ_SWITCH_R2        (2 << I2C_TV_STA_REQ_SWITCH_SHIFT)

#define I2C_TV_STA_REQ_ADO_SHIFT    (4)
#define I2C_TV_STA_REQ_ADO_MASK     (1 << I2C_TV_STA_REQ_ADO_SHIFT)
#define I2C_TV_STA_REQ_ADO_CLR      (0 << I2C_TV_STA_REQ_ADO_SHIFT)
#define I2C_TV_STA_REQ_ADO_SET      (1 << I2C_TV_STA_REQ_ADO_SHIFT)

#define I2C_TV_STA_ADO_ON_SHIFT     (5)
#define I2C_TV_STA_ADO_ON_MASK      (1 << I2C_TV_STA_ADO_ON_SHIFT)
#define I2C_TV_STA_ADO_ON_SET       (1 << I2C_TV_STA_ADO_ON_SHIFT)
#define I2C_TV_STA_ADO_ON_CLR       (0)

#define I2C_TV_STA_PWR_ON_SHIFT     (6)
#define I2C_TV_STA_PWR_ON_MASK      (1 << I2C_TV_STA_PWR_ON_SHIFT)
#define I2C_TV_STA_PWR_ON_SET       (1 << I2C_TV_STA_PWR_ON_SHIFT)
#define I2C_TV_STA_PWR_ON_CLR       (0)
//(7) reserved
/***********  CEC Command Received 0xF3 ***********/
#define I2C_CEC_CMD_STANDBY_SHIFT   (0)
#define I2C_CEC_CMD_STANDBY_MASK    (1 << I2C_CEC_CMD_STANDBY_SHIFT)
#define I2C_CEC_CMD_STANDBY_CLR     (0 << I2C_CEC_CMD_STANDBY_SHIFT)
#define I2C_CEC_CMD_STANDBY_SET     (1 << I2C_CEC_CMD_STANDBY_SHIFT)

#define I2C_CEC_CMD_SYS_ADO_OFF_SHIFT   (1)
#define I2C_CEC_CMD_SYS_ADO_OFF_MASK    (1 << I2C_CEC_CMD_SYS_ADO_OFF_SHIFT)
#define I2C_CEC_CMD_SYS_ADO_OFF_CLR     (0 << I2C_CEC_CMD_SYS_ADO_OFF_SHIFT)
#define I2C_CEC_CMD_SYS_ADO_OFF_SET     (1 << I2C_CEC_CMD_SYS_ADO_OFF_SHIFT)

#define I2C_CEC_CMD_ROUT_CHG_SHIFT      (2)
#define I2C_CEC_CMD_ROUT_CHG_MASK       (1 << I2C_CEC_CMD_ROUT_CHG_SHIFT)
#define I2C_CEC_CMD_ROUT_CHG_SET        (1 << I2C_CEC_CMD_ROUT_CHG_SHIFT)
#define I2C_CEC_CMD_ROUT_CHG_CLR        (0)

#define I2C_CEC_CMD_SET_STREAM_SHIFT    (3)
#define I2C_CEC_CMD_SET_STREAM_MASK     (1 << I2C_CEC_CMD_SET_STREAM_SHIFT)
#define I2C_CEC_CMD_SET_STREAM_SET      (1 << I2C_CEC_CMD_SET_STREAM_SHIFT)
#define I2C_CEC_CMD_SET_STREAM_CLR      (0)

#define I2C_CEC_CMD_ACTIVE_SRC_SHIFT    (4)
#define I2C_CEC_CMD_ACTIVE_SRC_MASK     (1 << I2C_CEC_CMD_ACTIVE_SRC_SHIFT)
#define I2C_CEC_CMD_ACTIVE_SRC_CLR      (0 << I2C_CEC_CMD_ACTIVE_SRC_SHIFT)
#define I2C_CEC_CMD_ACTIVE_SRC_SET      (1 << I2C_CEC_CMD_ACTIVE_SRC_SHIFT)

#define I2C_CEC_CMD_SYS_ADO_ON_SHIFT    (5)
#define I2C_CEC_CMD_SYS_ADO_ON_MASK     (1 << I2C_CEC_CMD_SYS_ADO_ON_SHIFT)
#define I2C_CEC_CMD_SYS_ADO_ON_CLR      (0 << I2C_CEC_CMD_SYS_ADO_ON_SHIFT)
#define I2C_CEC_CMD_SYS_ADO_ON_SET      (1 << I2C_CEC_CMD_SYS_ADO_ON_SHIFT)

#define I2C_CEC_CMD_LATCH_SHIFT         (6)
#define I2C_CEC_CMD_LATCH_MASK          (1 << I2C_CEC_CMD_LATCH_SHIFT)
#define I2C_CEC_CMD_LATCH_CLR           (0 << I2C_CEC_CMD_LATCH_SHIFT)
#define I2C_CEC_CMD_LATCH_SET           (1 << I2C_CEC_CMD_LATCH_SHIFT)

#define I2C_CEC_CMD_SWITCH_RX_SHIFT     (7)
#define I2C_CEC_CMD_SWITCH_RX_MASK      (1 << I2C_CEC_CMD_SWITCH_RX_SHIFT)
#define I2C_CEC_CMD_SWITCH_RX_CLR       (0 << I2C_CEC_CMD_SWITCH_RX_SHIFT)
#define I2C_CEC_CMD_SWITCH_RX_SET       (1 << I2C_CEC_CMD_SWITCH_RX_SHIFT)
/***********  CEC Command Received 2 0xF4 ***********/
#define I2C_CEC_CMD_POWER_SHIFT         (0)
#define I2C_CEC_CMD_POWER_MASK          (1 << I2C_CEC_CMD_POWER_SHIFT)
#define I2C_CEC_CMD_POWER_CLR           (0 << I2C_CEC_CMD_POWER_SHIFT)
#define I2C_CEC_CMD_POWER_SET           (1 << I2C_CEC_CMD_POWER_SHIFT)

#define I2C_CEC_CMD_POWER_ON_SHIFT      (1)
#define I2C_CEC_CMD_POWER_ON_MASK       (1 << I2C_CEC_CMD_POWER_ON_SHIFT)
#define I2C_CEC_CMD_POWER_ON_CLR        (0 << I2C_CEC_CMD_POWER_ON_SHIFT)
#define I2C_CEC_CMD_POWER_ON_SET        (1 << I2C_CEC_CMD_POWER_ON_SHIFT)

#define I2C_CEC_CMD_POWER_OFF_SHIFT     (2)
#define I2C_CEC_CMD_POWER_OFF_MASK      (1 << I2C_CEC_CMD_POWER_OFF_SHIFT)
#define I2C_CEC_CMD_POWER_OFF_CLR       (0 << I2C_CEC_CMD_POWER_OFF_SHIFT)
#define I2C_CEC_CMD_POWER_OFF_SET       (1 << I2C_CEC_CMD_POWER_OFF_SHIFT)
// (3) ~ (7) reserved
/***********  HDMI Rx Status 0xF5 ***********/
#define I2C_R0_STA_SHIFT        (0)
#define I2C_R0_STA_MASK         (3 << I2C_R0_STA_SHIFT)
#define I2C_R0_STA_NO           (0 << I2C_R0_STA_SHIFT)
#define I2C_R0_STA_5V           (1 << I2C_R0_STA_SHIFT)
#define I2C_R0_STA_IN           (2 << I2C_R0_STA_SHIFT)

#define I2C_R1_STA_SHIFT        (2)
#define I2C_R1_STA_MASK         (3 << I2C_R1_STA_SHIFT)
#define I2C_R1_STA_NO           (0 << I2C_R1_STA_SHIFT)
#define I2C_R1_STA_5V           (1 << I2C_R1_STA_SHIFT)
#define I2C_R1_STA_IN           (2 << I2C_R1_STA_SHIFT)
// (4) ~ (7) reserved
/***********  HDMI Mode Rx0~Rx2 0xF6~0xF8 ***********/
#define I2C_RX_EDID_SHIFT       (0)
#define I2C_RX_EDID_MASK        (1 << I2C_RX_EDID_SHIFT)
#define I2C_RX_EDID_TV          (0 << I2C_RX_EDID_SHIFT)
#define I2C_RX_EDID_SB          (1 << I2C_RX_EDID_SHIFT)

#define I2C_RX_HDCP_VER_SHIFT   (1)
#define I2C_RX_HDCP_VER_MASK    (1 << I2C_RX_HDCP_VER_SHIFT)
#define I2C_RX_HDCP_VER_AUTO    (0 << I2C_RX_HDCP_VER_SHIFT)
#define I2C_RX_HDCP_VER_23      (1 << I2C_RX_HDCP_VER_SHIFT)

#define I2C_RX_HDCP_RPT_SHIFT   (2)
#define I2C_RX_HDCP_RPT_MASK    (1 << I2C_RX_HDCP_RPT_SHIFT)
#define I2C_RX_HDCP_RPT_CLR     (0 << I2C_RX_HDCP_RPT_SHIFT)
#define I2C_RX_HDCP_RPT_SET     (1 << I2C_RX_HDCP_RPT_SHIFT)
// (3) ~ (6)reserved
#define I2C_RX_ACTIVE_SHIFT     (7)
#define I2C_RX_ACTIVE_MASK      (1 << I2C_RX_ACTIVE_SHIFT)
#define I2C_RX_ACTIVE_BY_SEL    (0 << I2C_RX_ACTIVE_SHIFT)
#define I2C_RX_ACTIVE_FORCE     (1 << I2C_RX_ACTIVE_SHIFT)

/***********  HDMI Mode Rx0 0xF6 ***********/
#define I2C_RX0_EDID_SHIFT      (0)
#define I2C_RX0_EDID_MASK       (1 << I2C_RX0_EDID_SHIFT)
#define I2C_RX0_EDID_TV         (0 << I2C_RX0_EDID_SHIFT)
#define I2C_RX0_EDID_SB         (1 << I2C_RX0_EDID_SHIFT)

#define I2C_RX0_HDCP_VER_SHIFT  (1)
#define I2C_RX0_HDCP_VER_MASK   (1 << I2C_RX0_HDCP_VER_SHIFT)
#define I2C_RX0_HDCP_VER_AUTO   (0 << I2C_RX0_HDCP_VER_SHIFT)
#define I2C_RX0_HDCP_VER_23     (1 << I2C_RX0_HDCP_VER_SHIFT)

#define I2C_RX0_HDCP_RPT_SHIFT  (2)
#define I2C_RX0_HDCP_RPT_MASK   (1 << I2C_RX0_HDCP_RPT_SHIFT)
#define I2C_RX0_HDCP_RPT_CLR    (0 << I2C_RX0_HDCP_RPT_SHIFT)
#define I2C_RX0_HDCP_RPT_SET    (1 << I2C_RX0_HDCP_RPT_SHIFT)
// (3) ~ (6)reserved
#define I2C_RX0_ACTIVE_SHIFT    (7)
#define I2C_RX0_ACTIVE_MASK     (1 << I2C_RX0_ACTIVE_SHIFT)
#define I2C_RX0_ACTIVE_BY_SEL   (0 << I2C_RX0_ACTIVE_SHIFT)
#define I2C_RX0_ACTIVE_FORCE    (1 << I2C_RX0_ACTIVE_SHIFT)

/***********  HDMI Mode Rx1 0xF7 ***********/
#define I2C_RX1_EDID_SHIFT      (0)
#define I2C_RX1_EDID_MASK       (1 << I2C_RX1_EDID_SHIFT)
#define I2C_RX1_EDID_TV         (0 << I2C_RX1_EDID_SHIFT)
#define I2C_RX1_EDID_SB         (1 << I2C_RX1_EDID_SHIFT)

#define I2C_RX1_HDCP_VER_SHIFT  (1)
#define I2C_RX1_HDCP_VER_MASK   (1 << I2C_RX1_HDCP_VER_SHIFT)
#define I2C_RX1_HDCP_VER_AUTO   (0 << I2C_RX1_HDCP_VER_SHIFT)
#define I2C_RX1_HDCP_VER_23     (1 << I2C_RX1_HDCP_VER_SHIFT)

#define I2C_RX1_HDCP_RPT_SHIFT  (2)
#define I2C_RX1_HDCP_RPT_MASK   (1 << I2C_RX1_HDCP_RPT_SHIFT)
#define I2C_RX1_HDCP_RPT_CLR    (0 << I2C_RX1_HDCP_RPT_SHIFT)
#define I2C_RX1_HDCP_RPT_SET    (1 << I2C_RX1_HDCP_RPT_SHIFT)
// (3) ~ (6)reserved
#define I2C_RX1_ACTIVE_SHIFT    (7)
#define I2C_RX1_ACTIVE_MASK     (1 << I2C_RX1_ACTIVE_SHIFT)
#define I2C_RX1_ACTIVE_BY_SEL   (0 << I2C_RX1_ACTIVE_SHIFT)
#define I2C_RX1_ACTIVE_FORCE    (1 << I2C_RX1_ACTIVE_SHIFT)
/***********  Reserved 0xF8~ 0xFB ***********/
/***********  CEC Transmit Count 0xFC ***********/
/***********  CEC Received Count 0xFD ***********/
/***********  CEC Command Latch OP Code 0xFD ***********/
/***********  Host Update 0xFE ***********/
#define I2C_INIT_SHIFT          (0)
#define I2C_INIT_MASK           (1 << I2C_INIT_SHIFT)
#define I2C_INIT_SET            (1 << I2C_INIT_SHIFT)
#define I2C_INIT_CLR            (0)

#define I2C_INIT_STAG_SHIFT     (1)
#define I2C_INIT_STAG_MASK      (0x3F << I2C_INIT_STAG_SHIFT)
#define I2C_INIT_STAG_0         (0 << I2C_INIT_STAG_SHIFT)
#define I2C_INIT_STAG_1         (1 << I2C_INIT_STAG_SHIFT)
#define I2C_INIT_STAG_2         (2 << I2C_INIT_STAG_SHIFT)
#define I2C_INIT_STAG_3         (3 << I2C_INIT_STAG_SHIFT)          // Special int setting
#define I2C_INIT_STAG_3F        (0x3F << I2C_INIT_STAG_SHIFT)

#define I2C_INIT_RDY_SHIFT      (7)
#define I2C_INIT_RDY_MASK       (1 << I2C_INIT_RDY_SHIFT)
#define I2C_INIT_RDY_SET        (1 << I2C_INIT_RDY_SHIFT)
#define I2C_INIT_RDY_CLR        (0)
/*********************************************/
#define I2C_UPD_ADO_SHIFT       (1)
#define I2C_UPD_ADO_MASK        (1 << I2C_UPD_ADO_SHIFT)
#define I2C_UPD_ADO_SET         (1 << I2C_UPD_ADO_SHIFT)
#define I2C_UPD_ADO_CLR         (0)
// (2) reserved
#define I2C_UPD_EDID_SHIFT      (3)
#define I2C_UPD_EDID_MASK       (1 << I2C_UPD_EDID_SHIFT)
#define I2C_UPD_EDID_SET        (1 << I2C_UPD_EDID_SHIFT)
#define I2C_UPD_EDID_CLR        (0)

#define I2C_UPD_TRI_INT_SHIFT   (5)
#define I2C_UPD_TRI_INT_MASK    (1 << I2C_UPD_TRI_INT_SHIFT)
#define I2C_UPD_TRI_INT_SET     (1 << I2C_UPD_TRI_INT_SHIFT)
#define I2C_UPD_TRI_INT_CLR     (0)

#define I2C_UPD_CEC_FIRE_SHIFT  (6)
#define I2C_UPD_CEC_FIRE_MASK   (1 << I2C_UPD_CEC_FIRE_SHIFT)
#define I2C_UPD_CEC_FIRE_CLR    (0 << I2C_UPD_CEC_FIRE_SHIFT)
#define I2C_UPD_CEC_FIRE_SET    (1 << I2C_UPD_CEC_FIRE_SHIFT)

#define I2C_UPD_SYS_SHIFT       (7)
#define I2C_UPD_SYS_MASK        (1 << I2C_UPD_SYS_SHIFT)
#define I2C_UPD_SYS_CLR         (0 << I2C_UPD_SYS_SHIFT)
#define I2C_UPD_SYS_SET         (1 << I2C_UPD_SYS_SHIFT)
/***********  ITE Interrupt 0xFF ***********/
#define I2C_INT_READY_SHIFT     (0)
#define I2C_INT_READY_MASK      (1 << I2C_INT_READY_SHIFT)
#define I2C_INT_READY_CLR       (0 << I2C_INT_READY_SHIFT)
#define I2C_INT_READY_SET       (1 << I2C_INT_READY_SHIFT)

#define I2C_INT_AUDIO_SHIFT     (1)
#define I2C_INT_AUDIO_MASK      (1 << I2C_INT_AUDIO_SHIFT)
#define I2C_INT_AUDIO_CLR       (0 << I2C_INT_AUDIO_SHIFT)
#define I2C_INT_AUDIO_SET       (1 << I2C_INT_AUDIO_SHIFT)
// (2) reserved
#define I2C_INT_TV_LATENCY_SHIFT    (3)
#define I2C_INT_TV_LATENCY_MASK     (1 << I2C_INT_TV_LATENCY_SHIFT)
#define I2C_INT_TV_LATENCY_CLR      (0 << I2C_INT_TV_LATENCY_SHIFT)
#define I2C_INT_TV_LATENCY_SET      (1 << I2C_INT_TV_LATENCY_SHIFT)

#define I2C_INT_REPLY_INT_SHIFT     (5)
#define I2C_INT_REPLY_INT_MASK      (1 << I2C_INT_REPLY_INT_SHIFT)
#define I2C_INT_REPLY_INT_SET       (1 << I2C_INT_REPLY_INT_SHIFT)
#define I2C_INT_REPLY_INT_CLR       (0)
// (4) ~ (5) reserved
#define I2C_INT_CEC_GOT_SHIFT   (6)
#define I2C_INT_CEC_GOT_MASK    (1 << I2C_INT_CEC_GOT_SHIFT)
#define I2C_INT_CEC_GOT_CLR     (0 << I2C_INT_CEC_GOT_SHIFT)
#define I2C_INT_CEC_GOT_SET     (1 << I2C_INT_CEC_GOT_SHIFT)

#define I2C_INT_SYS_SHIFT       (7)
#define I2C_INT_SYS_MASK        (1 << I2C_INT_SYS_SHIFT)
#define I2C_INT_SYS_CLR         (0 << I2C_INT_SYS_SHIFT)
#define I2C_INT_SYS_SET         (1 << I2C_INT_SYS_SHIFT)

#define MODE_CHG_SB_EDID    (1 << 0)        //  [0], 0xFE[3], EDID update
#define MODE_CHG_ADO        (1 << 1)        //  [1], 0xF1, Audio mode update
#define MODE_CHG_RX_SEL     (1 << 2)        //  [2], 0xF0, Rx Port Select
#define MODE_CHG_RX_EDID    (1 << 3)        //  [3], 0xF5, Rx Edid mode change
#define MODE_CHG_R0         (1 << 4)        //  [4], 0xF6, Rx0 Mode change
#define MODE_CHG_R1         (1 << 5)        //  [5], 0xF7, Rx1 Mode change
//  [6], 0xF8
//  [7], 0xF9

#define EDID_UPDATE_VPID        (1 << 0)
#define EDID_UPDATE_PNAME       (1 << 1)
#define EDID_UPDATE_SPK_ADB0    (1 << 3)
#define EDID_UPDATE_ADB1        (1 << 4)
#define EDID_UPDATE_LATENCY     (1 << 5)

/************************************  Tx ADOIO ENCODER CONFIG *************************************/
//g_u32TxAdoEncConf
// AudCh
#define ADO_CONF_CH_SHIFT                       (0)
#define ADO_CONF_CH_MASK                        (0x0F << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_0                           (0x00 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_2                           (0x02 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_3                           (0x03 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_4                           (0x04 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_5                           (0x05 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_6                           (0x06 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_7                           (0x07 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_8                           (0x08 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_10                          (0x0A << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_12                          (0x0C << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_16                          (0x0D << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_32                          (0x0E << ADO_CONF_CH_SHIFT)
// AudFmt
#define ADO_CONF_SAMPLE_FREQ_SHIFT          (4)
#define ADO_CONF_SAMPLE_FREQ_MASK           (0x3F << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_32K                (0x03 << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_44K                (0x00 << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_48K                (0x02 << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_64K                (0x0B << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_88K                (0x08 << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_96K                (0x0A << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_128K           (0x2B << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_176K           (0x0C << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_192K           (0x0E << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_256K           (0x1B << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_352K           (0x0D << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_384K           (0x05 << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_512K           (0x3B << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_705K           (0x2D << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_768K           (0x09 << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_1024K          (0x35 << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_1411K          (0x1D << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_1536K          (0x15 << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_REFER2HEADER   (0x3E << ADO_CONF_SAMPLE_FREQ_SHIFT)
#define ADO_CONF_SAMPLE_FREQ_ERROR          (0x3F << ADO_CONF_SAMPLE_FREQ_SHIFT)
// AudSel
#define ADO_CONF_SEL_SHIFT                      (10)
#define ADO_CONF_SEL_MASK                       (0x01 << ADO_CONF_SEL_SHIFT)
#define ADO_CONF_SEL_I2S                        (0x00 << ADO_CONF_SEL_SHIFT)
#define ADO_CONF_SEL_SPDIF                      (0x01 << ADO_CONF_SEL_SHIFT)
// AudType
#define ADO_CONF_TYPE_SHIFT                 (11)
#define ADO_CONF_TYPE_MASK                  (0x03 << ADO_CONF_TYPE_SHIFT)
#define ADO_CONF_TYPE_LPCM                  (0x00 << ADO_CONF_TYPE_SHIFT)
#define ADO_CONF_TYPE_NLPCM                 (0x01 << ADO_CONF_TYPE_SHIFT)
#define ADO_CONF_TYPE_HBR                       (0x02 << ADO_CONF_TYPE_SHIFT)
#define ADO_CONF_TYPE_DSD                       (0x03 << ADO_CONF_TYPE_SHIFT)
// AudTypSWL
#define ADO_CONF_BITS_SHIFT                 (13)
#define ADO_CONF_BITS_MASK                  (0x03 << ADO_CONF_BITS_SHIFT)
#define ADO_CONF_BITS_16                        (0x00 << ADO_CONF_BITS_SHIFT)
#define ADO_CONF_BITS_18                        (0x01 << ADO_CONF_BITS_SHIFT)
#define ADO_CONF_BITS_20                        (0x02 << ADO_CONF_BITS_SHIFT)
#define ADO_CONF_BITS_24                        (0x03 << ADO_CONF_BITS_SHIFT)
// AudTypFmt
#define ADO_CONF_FMT_SHIFT                  (15)
#define ADO_CONF_FMT_MASK                       (0x1F << ADO_CONF_FMT_SHIFT)
//#define ADO_CONF_FMT_I2S_STANDARD         (0x00 << ADO_CONF_FMT_SHIFT)
#define ADO_CONF_FMT_I2S_32BIT              (0x01 << ADO_CONF_FMT_SHIFT)
#define ADO_CONF_FMT_JUSTIFIED_LEFT         (0x00 << ADO_CONF_FMT_SHIFT)
#define ADO_CONF_FMT_JUSTIFIED_RIGHT            (0x02 << ADO_CONF_FMT_SHIFT)
#define ADO_CONF_FMT_1T_DELAY_WS            (0x00 << ADO_CONF_FMT_SHIFT)
#define ADO_CONF_FMT_NO_DELAY_WS            (0x04 << ADO_CONF_FMT_SHIFT)
#define ADO_CONF_FMT_WS_CH_LEFT             (0x00 << ADO_CONF_FMT_SHIFT)
#define ADO_CONF_FMT_WS_CH_RIGHT            (0x08 << ADO_CONF_FMT_SHIFT)
#define ADO_CONF_FMT_SHIFT_FIRST_MSB            (0x00 << ADO_CONF_FMT_SHIFT)
#define ADO_CONF_FMT_SHIFT_FIRST_LSB            (0x10 << ADO_CONF_FMT_SHIFT)
// EnMSAud      //only for LPCM/NLPCM and DSD when AudCh is less than or equal to 8
#define ADO_CONF_TYPE_MULTI_STREAM_SHIFT    (20)
#define ADO_CONF_TYPE_MULTI_STREAM_MASK     (0x01 << ADO_CONF_TYPE_MULTI_STREAM_SHIFT)
#define ADO_CONF_TYPE_MULTI_STREAM_SET      (0x01 << ADO_CONF_TYPE_MULTI_STREAM_SHIFT)
#define ADO_CONF_TYPE_MULTI_STREAM_CLR          (0x00 << ADO_CONF_TYPE_MULTI_STREAM_SHIFT)
// EnTDM        // if TRUE, AudSel must be I2S and AudType cannot be DSD and EnAudGen must be TRUE for Internal AudGen
#define ADO_CONF_TDM_EN_SHFIT               (21)
#define ADO_CONF_TDM_EN_MASK                (0x01 << ADO_CONF_TDM_EN_SHFIT)
#define ADO_CONF_TDM_EN_SET                 (0x01 << ADO_CONF_TDM_EN_SHFIT)
#define ADO_CONF_TDM_EN_CLR                 (0x00 << ADO_CONF_TDM_EN_SHFIT)
//(22~23)
#define ADO_CONF_LAYOUT_SHIFT               (24)
#define ADO_CONF_LAYOUT_MASK                (0x01 << ADO_CONF_LAYOUT_SHIFT)
#define ADO_CONF_LAYOUT_0                   (0x00)
#define ADO_CONF_LAYOUT_1                   (0x01 << ADO_CONF_LAYOUT_SHIFT)

#define ADO_CONF_TYPE_3D_SHIFT              (25)
#define ADO_CONF_TYPE_3D_MASK               (0x01 << ADO_CONF_TYPE_3D_SHIFT)
#define ADO_CONF_TYPE_3D_SET                (0x01 << ADO_CONF_TYPE_3D_SHIFT)
#define ADO_CONF_TYPE_3D_CLR                (0x00 << ADO_CONF_TYPE_3D_SHIFT)
//
#define ADO_CONF_ACTIVE_SHIFT                   (26)
#define ADO_CONF_ACTIVE_MASK                    (0x01 << ADO_CONF_ACTIVE_SHIFT)
#define ADO_CONF_ACTIVE_SET                 (0x01 << ADO_CONF_ACTIVE_SHIFT)
#define ADO_CONF_ACTIVE_CLR                 (0x00)

#define ADO_CONF_SAMPLE_WORD_LEN_SHIFT      (27)
#define ADO_CONF_SAMPLE_WORD_LEN_MASK       (0x0F << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_20_NO      (0x00 << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_20_16      (0x02 << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_20_18      (0x04 << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_20_19      (0x08 << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_20_20      (0x0A << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_20_17      (0x0C << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_24_NO      (0x01 << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_24_20      (0x03 << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_24_22      (0x05 << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_24_23      (0x09 << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_24_24      (0x0B << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)
#define ADO_CONF_SAMPLE_WORD_LEN_24_21      (0x0D << ADO_CONF_SAMPLE_WORD_LEN_SHIFT)

//****************************************************************************

#if 0
#define CMD_POWER_STANDBY   (0x50)
#define CMD_PASSTHROUGH     (0x88)
#define CMD_POWER_ON        (0x48)
#define CMD_POWER_OFF       (0xC8)
#define CMD_DUMP_I2C        (0xE0)
//#define CMD_CEC_CTL           (0x30)
#define CMD_VOLUME_MUTE     (0x58)
#define CMD_VOLUME_UP       (0xD8)
#define CMD_VOLUME_DOWN     (0x32)
#define CMD_MODE_HDMI_1     (0xC2)
#define CMD_MODE_HDMI_2     (0xD0)
#define CMD_MODE_ARC        (0x22)
#define CMD_MODE_BT         (0x3A)
#define CMD_ADO_SYS         (0x28)
#define CMD_EARC            (0xA8)
#define CMD_SB_CEC          (0x68)
#define CMD_TX_AUDIO        (0x18)
#define CMD_TX_OUTPUT       (0xE8)
#define CMD_HDCP_RPT        (0xB8)
#else
#define CMD_POWER_STANDBY   (0xC2)
#define CMD_PASSTHROUGH     (0x18)
#define CMD_POWER_ON        (0x22)
#define CMD_POWER_OFF       (0x02)
#define CMD_DUMP_I2C        (0x30)
//#define CMD_CEC_CTL           (0x30)
#define CMD_VOLUME_MUTE     (0x90)
#define CMD_VOLUME_UP       (0xA8)
#define CMD_VOLUME_DOWN     (0xE0)
#define CMD_MODE_HDMI_1     (0xA2)
#define CMD_MODE_HDMI_2     (0x62)
#define CMD_MODE_HDMI_3     (0xE2)
#define CMD_MODE_ARC        (0x10)
#define CMD_MODE_BT         (0x7A)
#define CMD_ADO_SYS         (0x38)
#define CMD_EARC            (0x5A)
#define CMD_SB_CEC          (0x42)
#define CMD_TX_AUDIO        (0x4A)
#define CMD_TX_OUTPUT       (0x52)
#define CMD_HDCP_RPT        (0x68)
#endif

#define CMD_ADO_SEL         (0x01)

//****************************************************************************
#define SPEC_INT_END            (0)
#define SPEC_INT_CEC_PWR        (1)     // 0xF4[0]
#define SPEC_INT_CEC_PWR_ON     (2)     // 0xF4[1]
#define SPEC_INT_CEC_ADO_REQ    (3)     // 0xF3[5]
#define SPEC_INT_CEC_ACT_SRC    (4)     // 0xF3[4]
#define SPEC_INT_CEC_ACT_SRC_RX (5)     // 0xF3[4] + 0xF2[3:2]
#define SPEC_INT_TV_PWR_ON      (6)     // 0xF2[6]
#define SPEC_INT_MAX            (6)

#include <stdio.h>
#endif
