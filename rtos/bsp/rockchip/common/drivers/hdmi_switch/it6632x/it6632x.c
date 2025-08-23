/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <rtthread.h>
#include <rtdevice.h>

#include "hal_base.h"
#include "board.h"

#ifdef RT_USING_IT6632X

#include "it6632x.h"

static it6632x_device_t *g_it6632x_dev = RT_NULL;

//#define IT6632X_DEBUG
#ifndef IT6632X_RST_ON
#define IT6632X_RST_ON PIN_HIGH
#endif

#ifndef IT6632X_RST_OFF
#define IT6632X_RST_OFF PIN_LOW
#endif

#ifndef IT6632X_RXMUTE_UNMUTE
#define IT6632X_RXMUTE_UNMUTE PIN_LOW
#endif

#define SB_STA_CHANGE(a, b, c)      do{ (a) = ((a) & (~(b))) | ((c) & (b));}while(0)
#define IgnorAdoSta  (0)//(ADO_CONF_ACTIVE_MASK)

static rt_uint8_t  g_u8SbRxSel = 0;            // 0xF0
static rt_uint8_t  g_u8SbAudioMode = 0;            // 0xF1
static rt_uint8_t  g_u8SbTxSta = 0;            // 0xF2

static rt_uint8_t  g_u8IteCecReq = 0;
static rt_uint8_t  g_u8IteRdy = 0;
static rt_uint8_t  g_u8SbPower = 0;
static rt_uint8_t  g_u8SbCecMode = SB_CEC_AUTO;
//rt_uint8_t    g_u8ArcRetryDelayCnt = 0;
static rt_uint8_t  g_u8SbEdidPaAB = 0xFF;
static rt_uint8_t  g_u8SbEdidPaCD = 0xFF;
static rt_uint8_t  g_u8SbInfoCA = 0;
static rt_uint8_t  g_u8SbVolume;
static rt_uint8_t  g_u8SbLaten = 0;

static rt_uint32_t g_u32SbAdoDecInfo = 0;
static rt_uint8_t  g_u8ChipId = SB_I2C_NO_ITE;

static void SB_I2C_Dump(void);

static void SB_SysI2cChange(rt_uint8_t u8I2cChg);
static rt_err_t it6632x_write_regs(void *write_buf, rt_uint8_t write_len);
static rt_err_t  it6632x_read_regs(rt_uint8_t cmd, rt_uint8_t read_len, rt_uint8_t *read_buf);
static rt_int32_t it6632x_get_status(struct it6632x_status *status);
static rt_int32_t it6632x_get_hdmi_decoderinfo(struct it6632x_ado_info *ado_info);

static rt_err_t SB_I2C_Read(rt_uint8_t u8Offset, rt_uint8_t u8ByteNo, rt_uint8_t *pu8Data)
{
    return it6632x_read_regs(u8Offset, u8ByteNo, pu8Data);
}

static rt_err_t SB_I2C_Write(rt_uint8_t u8Offset, rt_uint8_t u8ByteNo, rt_uint8_t *pu8Data)
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

    ret = it6632x_write_regs(data_buf, u8ByteNo + 1);

    rt_free(data_buf);
    return ret;
}

//|     0xD0 Header Block
//|3 | 2 | 1 | 0 | 3 | 2 | 1 | 0 | - | - |
//|  initiator   |   destination |EOM|ACK|
static rt_err_t IT66322_send_cec_command(rt_uint8_t *ceccmd, rt_uint8_t len)
{
    if (len > 16 || len < 1)
    {
        rt_kprintf("%s incorrect CEC command length: %d \n", __func__, len);
        return -RT_ERROR;
    }

    rt_kprintf("%s CEC command: ", __func__);
    for (rt_uint8_t i = 0; i < len; i++)
        rt_kprintf("[0x%x] ", ceccmd[i]);
    rt_kprintf("\n");

    SB_I2C_Write(I2C_CEC_TRANS_DATA, len, ceccmd);
    SB_I2C_Write(I2C_SYS_CEC_TRANS_CNT, 1, &len);

    SB_SysI2cChange(I2C_UPD_CEC_FIRE_SET);

    return RT_EOK;
}

#define ARC_ONLY_OPTION     I2C_MODE_ARC_ONLY_CLR

/*
 *If it6632x recived cecuserctl mute, this func will be call, user can
 *choose ctrl local amp vol or send cec cmd to ctrl stb vol by hook implementation
 */
static void SB_CecUserCtl_Mute(void)
{
    rk_rpt_cec_cmd_t op = RK_REPEAT_CEC_CTL_VOL_MUTE;
    rt_kprintf("Mute-UnMute \n");
    if (g_it6632x_dev->cec_vol_change_hook != RT_NULL)
        g_it6632x_dev->cec_vol_change_hook((void *)&op);
}

/*
 *If it6632x recived cecuserctl vol up, this func will be call, user can
 *choose ctrl local amp vol or send cec cmd to ctrl stb vol by hook implementation
 */
static void SB_CecUserCtl_VolumeUp(void)
{
    rk_rpt_cec_cmd_t op = RK_REPEAT_CEC_CTL_VOL_UP;
    rt_kprintf("Volume+ \n");
    if (g_it6632x_dev->cec_vol_change_hook != RT_NULL)
        g_it6632x_dev->cec_vol_change_hook((void *)&op);
}

/*
 *If it6632x recived cecuserctl vol down, this func will be call, user can
 *choose ctrl local amp vol or send cec cmd to ctrl stb vol by hook implementation
 */
static void SB_CecUserCtl_VolumeDown(void)
{
    rk_rpt_cec_cmd_t op = RK_REPEAT_CEC_CTL_VOL_DOWN;
    rt_kprintf("Volume- \n");
    if (g_it6632x_dev->cec_vol_change_hook != RT_NULL)
        g_it6632x_dev->cec_vol_change_hook((void *)&op);
}

static void SB_AudioIn_SPDIF(rt_uint32_t u32SbAdoInfo, rt_uint8_t u8SbInfoCA)
{
    if ((u32SbAdoInfo & ADO_CONF_CH_MASK) == ADO_CONF_CH_0) // FOR ARC SPDIF IN
    {
        g_it6632x_dev->audio_brief_info.audio_in = 2;
        rt_kprintf("ARC input \n");
        if (g_it6632x_dev->audio_change_hook != RT_NULL)
            g_it6632x_dev->audio_change_hook((void *)&g_it6632x_dev->audio_brief_info);
    }
    else
    {
        g_it6632x_dev->audio_brief_info.audio_in = 1;
        rt_kprintf("SPDIF, not ARC \n");
        if (g_it6632x_dev->audio_change_hook != RT_NULL)
            g_it6632x_dev->audio_change_hook((void *)&g_it6632x_dev->audio_brief_info);
    }
}

static void SB_AudioIn_I2S(rt_uint32_t u32SbAdoInfo, rt_uint8_t u8SbInfoCA)
{
    rt_kprintf("I2S input \n");
    g_it6632x_dev->audio_brief_info.audio_in = 0;
    if (g_it6632x_dev->audio_change_hook != RT_NULL)
        g_it6632x_dev->audio_change_hook((void *)&g_it6632x_dev->audio_brief_info);
}

#if 0
rt_uint8_t  const u8SBSpkAlloc[] = {0x83, 0x4F, 0x00, 0x00};
rt_uint8_t  const u8SBAdb[] =
{
    0x35,
    0x09, 0x7F, 0x07,
    0x0F, 0x7F, 0x07,
    0x15, 0x07, 0x50,
    0x3D, 0x1E, 0xC0,
    0x57, 0x07, 0x03,
    0x5F, 0x7E, 0x01,
    0x67, 0x7E, 0x01,
};
#else
static rt_uint8_t  const u8SBSpkAlloc[] = {0x83, 0x6F, 0x0F, 0x0C};
static rt_uint8_t  const u8SBAdb[] =
{
    0x3B,               // Tag=1 (Audio Data Block), Length=27
    0x0F, 0x7F, 0x07,   // LPCM : 8-ch, 32~192K
    0x15, 0x07, 0x50,   // AC-3 : 6-ch, 32~48K
    0x35, 0x06, 0x3C,   // AAC  : 6-ch, 44~48K
    0x3E, 0x1E, 0xC0,   // DTS  : 7-ch, 44~96K
    0x4D, 0x02, 0x00,   // DSD  : 6-ch, 44K
    0x57, 0x06, 0x00,   // HBR  : 8-ch, 44~48K (Dolby Digital)
    0x5F, 0x7E, 0x01,   // HBR  : 8-ch, 44~192K (DTS-HD)
    0x67, 0x7E, 0x00,   // HBR  : 8-ch, 44~192K (Dolby TrueHD) MAT
    0xFF, 0x7F, 0x6F,   // LPCM : 16-ch, 32~192K (3D Audio)
};
#endif
#define SB_ADB_LEN  (sizeof(u8SBAdb))

#define SB_VOL_MAX      (100)
#define SB_VOL_DEFAULT  (30)

static rt_uint8_t  const u8eArcCapTable[] =
{
    0x01,               // Capabilities Data Structure Version = 0x01
    0x01, 0x26,         // BLOCK_ID=1, 38-byte change by peter for SL-870 HFR5-2-36
    0x3B,               // Tag=1 (Audio Data Block), Length=27
    0x0F, 0x7F, 0x07,   // LPCM : 8-ch, 32~192K
    0x15, 0x07, 0x50,   // AC-3 : 6-ch, 32~48K
    0x35, 0x06, 0x3C,   // AAC  : 6-ch, 44~48K
    0x3E, 0x1E, 0xC0,   // DTS  : 7-ch, 44~96K
    0x4D, 0x02, 0x00,   // DSD  : 6-ch, 44K
    0x57, 0x06, 0x00,   // HBR  : 8-ch, 44~48K (Dolby Digital)
    0x5F, 0x7E, 0x01,   // HBR  : 8-ch, 44~192K (DTS-HD)
    0x67, 0x7E, 0x00,   // HBR  : 8-ch, 44~192K (Dolby TrueHD) MAT
    0xFF, 0x7F, 0x6F,   // LPCM : 16-ch, 32~192K (3D Audio)
    //0x20 offset
    0x83,               // Tag=4 (Speaker Allocation Data Block), 3-bye
    0x6F, 0x0F, 0x0C,

    //0x24 offset
    0xE5, 0x13,            // change by peter for SL-870 HFR5-2-36
    0x00, 0x6F, 0x0F, 0x0C,

    0x02, 0x0A,         // BLOCK_ID=2, 10-byte    // should be marked for QD980 HFR5-2-26

    0x20, 0x00, 0x00, 0x00, 0x00,
    0x21, 0x01, 0x01, 0x01, 0x01,

    0x03, 0x01,         // BLOCK_ID=3, 1-byte
    0x89,               // Supports_AI=1, ONE_BIT_AUDIO_LAYOUT=1 (12-ch), MULTI_CH_LPCM_LAYOUT=1 (16-ch)
};

#define EARC_CAP_TABLE_SIZE     sizeof(u8eArcCapTable)
//#if (EARC_CAP_TABLE_SIZE > 0x100)
//#pragma message("EARC_CAP_TABLE_SIZE > 0x100")
//#endif
static rt_uint8_t  const u8CecVendorId[3] = {0x00, 0xE0, 0x36};
static rt_uint8_t  const u8CecOsdName[14] = "SOUNDBAR";
static rt_uint8_t   const g_u8OSDString[13] = "";

static void SB_I2C_Dump(void)
{
    rt_uint16_t u16Cnt;
    rt_uint8_t  u8I2cData[0x100];
    rt_uint8_t  *pu8I2cData;

    for (u16Cnt = 0; u16Cnt < 0x100; u16Cnt += 0x10)
    {
        SB_I2C_Read(u16Cnt, 0x10, &u8I2cData[u16Cnt]);
    }
    for (u16Cnt = 0; u16Cnt < 0x100; u16Cnt += 0x10)
    {
        pu8I2cData = &u8I2cData[u16Cnt];
        rt_kprintf("\r0x%02X: %02X %02X %02X %02X  %02X %02X %02X %02X   %02X %02X %02X %02X  %02X %02X %02X %02X\n", u16Cnt, pu8I2cData[0], pu8I2cData[1],
                   pu8I2cData[2], pu8I2cData[3], pu8I2cData[4], pu8I2cData[5], pu8I2cData[6], pu8I2cData[7], pu8I2cData[8], pu8I2cData[9], pu8I2cData[10], pu8I2cData[11],
                   pu8I2cData[12], pu8I2cData[13], pu8I2cData[14], pu8I2cData[15]);
    }
    if (1)
    {
        rt_kprintf("SB_Power[%d]\n", (u8I2cData[I2C_SYS_ADO_MODE] & 0x03));
        rt_kprintf("Rx_Sel[%d], AdoDecode_RxSel[%d]\n", (u8I2cData[I2C_SYS_RX_SEL] & 0x0F), (u8I2cData[I2C_SYS_RX_SEL] >> 4));
        rt_kprintf("Tx_DisOut[%d], Tx_AdoMute[%d]\n", (u8I2cData[I2C_SYS_ADO_MODE] & 0x80) >> 7, (u8I2cData[I2C_SYS_ADO_MODE] & 0x40) >> 6);
        rt_kprintf("TV_CEC[%d], SB_CEC[%d]\n", (u8I2cData[I2C_SYS_TX_STA] & 0x02) >> 1, (u8I2cData[I2C_SYS_ADO_MODE] & 0x20) >> 5);
        if (g_u8SbCecMode)
        {
            rt_kprintf("SB CEC Off\n");
        }
        else
        {
            rt_kprintf("SB CEC Auto\n");
        }
        rt_kprintf("AudioSystemEnable[%d], eARC/ARC_enable[%d]\n", (u8I2cData[I2C_SYS_ADO_MODE] & 0x08) >> 3, (u8I2cData[I2C_SYS_ADO_MODE] & 0x10) >> 4);
        rt_kprintf("Audio Ready, HDMI[%d], eARC[%d], ARC[%d]\n", ((u8I2cData[I2C_ADO_SRC_STA] & 0x4) >> 2), ((u8I2cData[I2C_ADO_SRC_STA] & 0x2) >> 1), (u8I2cData[I2C_ADO_SRC_STA] & 0x1));
        rt_kprintf("Audio Sourc Sel[%d] \n", u8I2cData[I2C_ADO_SEL]);

        rt_kprintf("SbTxSta = %X, SbAudioMode = %X\n", g_u8SbTxSta, g_u8SbAudioMode);
    }
    rt_kprintf("\n\n\n");
}

static void SB_SysI2cChange(rt_uint8_t u8I2cChg)
{
    if (u8I2cChg)
    {
        rt_uint8_t  u8Temp;
        SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8Temp);
        u8I2cChg |= u8Temp;
        SB_I2C_Write(I2C_SYS_CHANGE, 1, &u8I2cChg);
        rt_kprintf("SB Write = %X\n", u8I2cChg);

#ifdef IT6632X_DEBUG
        if ((u8I2cChg & I2C_INIT_MASK) == 0x00)
        {
            SB_I2C_Dump();
        }
#endif
    }
}

static rt_uint8_t SB_DefaultSetting(void)
{
    rt_uint8_t  u8I2cReg[3];
    rt_uint8_t  u8RxMode;
    rt_uint8_t  u8I2cChg = 0;

    rt_kprintf("SB_DefaultSetting \n");

    g_u8SbVolume = SB_VOL_DEFAULT;
    SB_I2C_Write(I2C_ADO_VOL, 1, &g_u8SbVolume);
    u8I2cReg[0] = SB_VOL_MAX;
    u8I2cReg[1] = 0;
    u8I2cReg[2] = 0;
    SB_I2C_Write(I2C_VOLUME_MAX, 3, u8I2cReg);
    u8I2cChg |= I2C_UPD_ADO_SET;

    u8I2cReg[0] = I2C_EDID_UPD_ADB0_SET;    // 8
    SB_I2C_Write(I2C_EDID_UPDATE, 1, u8I2cReg);
    SB_I2C_Write(I2C_EDID_SPK_ALLOC, 4, (rt_uint8_t *)u8SBSpkAlloc);
    SB_I2C_Write(I2C_EDID_ADB0, 31, (rt_uint8_t *)u8SBAdb);
    u8I2cChg |= I2C_UPD_EDID_SET;

    g_u8SbRxSel = I2C_HDMI_SELECT_R0 | I2C_HDMI_ADO_SEL_R0;     // 00;
    SB_I2C_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);

    u8RxMode = I2C_RX1_ACTIVE_FORCE | I2C_RX1_HDCP_RPT_SET | I2C_RX1_HDCP_VER_AUTO | I2C_RX1_EDID_SB;   // 0x85;
    SB_I2C_Write(I2C_SYS_RX_MODE_0, 1, &u8RxMode);
    SB_I2C_Write(I2C_SYS_RX_MODE_1, 1, &u8RxMode);

    //  g_u8SbAudioMode = 0x20;
    //  SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
    u8I2cChg |= I2C_UPD_SYS_SET;

    u8RxMode = 0;
    SB_I2C_Write(I2C_SYS_CEC_TRANS_CNT, 1, &u8RxMode);
    u8RxMode = 0x87;//eDeviceVendorID[0x87]; //eVendorCommandWithID[0xA0]
    if ((g_u8ChipId == SB_I2C_IT6622B) || (g_u8ChipId == SB_I2C_IT6622A))
    {
        SB_I2C_Write(I2C_SYS_CEC_LATCH_CNT_6622, 1, &u8RxMode);
    }
    else
    {
        SB_I2C_Write(I2C_SYS_CEC_LATCH_CNT, 1, &u8RxMode);
    }
    SB_I2C_Write(I2C_CEC_TRANS_DATA, 1, &u8RxMode);
    u8RxMode = 0x89; // eVendorCommand
    SB_I2C_Write(I2C_CEC_TRANS_DATA + 1, 1, &u8RxMode);
    u8RxMode = 0x00;
    SB_I2C_Write(I2C_CEC_TRANS_DATA + 2, 1, &u8RxMode);
    u8I2cChg |= I2C_UPD_CEC_FIRE_SET;
    return u8I2cChg;
}

static rt_uint8_t SB_PowerOff(void)
{
    rt_uint8_t  u8I2cChg = 0;
    rt_kprintf("SB_PowerOff \n");
    g_u8SbPower = SB_POWER_OFF;
    //  u8I2cChg = SB_DefaultSetting();
    // 00: power down mode
    SB_STA_CHANGE(g_u8SbAudioMode, I2C_MODE_POWER_MASK, I2C_MODE_POWER_DOWN);
    SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
    u8I2cChg |= I2C_UPD_SYS_SET;
    return u8I2cChg;
}

static rt_uint8_t SB_PowerOn(void)
{
    rt_uint8_t  u8I2cChg = 0;
    rt_uint8_t  u8RxMode;
    rt_kprintf("SB_PowerOn \n");
    g_u8SbPower = SB_POWER_ON;
    // Enable HDMI audio decode
    switch (g_u8SbRxSel & 0x0F)
    {
    case    0:
        g_u8SbRxSel = I2C_HDMI_SELECT_R0 | I2C_HDMI_ADO_SEL_R0;
        break;
    case    1:
        g_u8SbRxSel = I2C_HDMI_SELECT_R1 | I2C_HDMI_ADO_SEL_R1;
        break;
    default:
        g_u8SbRxSel = I2C_HDMI_SELECT_R0 | I2C_HDMI_ADO_SEL_R0;
        break;
    }
    SB_I2C_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);

    // 1: power on mode
    g_u8SbAudioMode = I2C_MODE_POWER_ON | I2C_MODE_ADO_SYS_EN | I2C_MODE_EARC_EN | I2C_MODE_CEC_EN | I2C_MODE_TX_ADO_EN | I2C_MODE_TX_OUT_EN | ARC_ONLY_OPTION;  // 0x39;
    SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);

    // SB CEC auto mode
    g_u8SbCecMode = SB_CEC_AUTO;

    // Rx Force ON & using SB ADB
    u8RxMode = I2C_RX0_ACTIVE_FORCE | I2C_RX0_HDCP_RPT_SET | I2C_RX0_HDCP_VER_23 | I2C_RX0_EDID_SB;//u8RxMode;
    SB_I2C_Write(I2C_SYS_RX_MODE_0, 1, &u8RxMode);
    u8RxMode = I2C_RX1_ACTIVE_FORCE | I2C_RX1_HDCP_RPT_SET | I2C_RX1_HDCP_VER_23 | I2C_RX1_EDID_SB;//u8RxMode;
    SB_I2C_Write(I2C_SYS_RX_MODE_1, 1, &u8RxMode);

    u8I2cChg |= I2C_UPD_SYS_SET;

    return u8I2cChg;
}

static rt_uint8_t SB_Standby(void)
{
    rt_kprintf("SB_Standby \n");
    g_u8SbPower = SB_STANDBY;

    SB_STA_CHANGE(g_u8SbAudioMode, I2C_MODE_POWER_MASK | I2C_MODE_ADO_SYS_MASK | I2C_MODE_EARC_MASK | I2C_MODE_CEC_MASK
                  , I2C_MODE_POWER_ON | I2C_MODE_ADO_SYS_DIS | I2C_MODE_EARC_DIS | I2C_MODE_CEC_EN);

    SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);

    return I2C_UPD_SYS_SET;
}

static rt_uint8_t SB_PassThrough(void)
{
    rt_uint8_t  u8RxMode;
    rt_kprintf("SB_PassThrough \n");
    g_u8SbPower = SB_PASS_THROUGH;

    // Disable HDMI Audio decode
    SB_STA_CHANGE(g_u8SbRxSel, I2C_HDMI_ADO_SEL_MASK, I2C_HDMI_ADO_SEL_NONE);
    SB_I2C_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);

    // Power On, Audio System Off, eARC/ARC Off, SB CEC Off, Tx Output & audio out.
    g_u8SbCecMode = SB_CEC_OFF;
    g_u8SbAudioMode = I2C_MODE_POWER_ON | I2C_MODE_ADO_SYS_DIS | I2C_MODE_EARC_DIS | I2C_MODE_CEC_DIS | I2C_MODE_TX_ADO_EN | I2C_MODE_TX_OUT_EN | ARC_ONLY_OPTION ;// 0x01;
    SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);

    // Rx active when Tx select to.
    u8RxMode = I2C_RX0_ACTIVE_BY_SEL | I2C_RX0_HDCP_RPT_CLR | I2C_RX0_HDCP_VER_AUTO | I2C_RX0_EDID_TV;//u8RxMode;
    SB_I2C_Write(I2C_SYS_RX_MODE_0, 1, &u8RxMode);
    u8RxMode = I2C_RX1_ACTIVE_BY_SEL | I2C_RX1_HDCP_RPT_CLR | I2C_RX1_HDCP_VER_AUTO | I2C_RX1_EDID_TV;
    SB_I2C_Write(I2C_SYS_RX_MODE_1, 1, &u8RxMode);

    return I2C_UPD_SYS_SET;
}

static rt_uint8_t SB_EdidChg(rt_uint8_t bEdidMode)
{
    rt_uint8_t u8I2cChg = 0;
    rt_uint8_t  u8EdidMode[2];

    SB_I2C_Read(I2C_SYS_RX_MODE_0, 2, u8EdidMode);

    if (bEdidMode != (u8EdidMode[0] & 0x01))
    {
        SB_STA_CHANGE(u8EdidMode[0], I2C_RX0_EDID_MASK, bEdidMode);
        u8I2cChg = I2C_UPD_SYS_SET;
    }

    if (bEdidMode != (u8EdidMode[1] & 0x01))
    {
        SB_STA_CHANGE(u8EdidMode[1], I2C_RX0_EDID_MASK, bEdidMode);
        u8I2cChg = I2C_UPD_SYS_SET;
    }

    if (u8I2cChg)
    {
        SB_I2C_Write(I2C_SYS_RX_MODE_0, 2, u8EdidMode);
    }
    return u8I2cChg;
}

static void SB_I2cWriteEarcCap(rt_uint8_t u8Bank)
{
#define EARC_CAP_BANK_SIZE      (0x80)
#define EARC_ZERO_SIZE          (0x20)

    rt_uint8_t  u8eArcZero[EARC_ZERO_SIZE];
    rt_uint8_t  u8Cnt, u8Temp;
    rt_uint8_t  *pu8Ptr = u8eArcZero;
    rt_uint16_t u16Temp = u8Bank * EARC_CAP_BANK_SIZE;

    if (EARC_CAP_TABLE_SIZE > 0x100)
    {
        rt_kprintf("ERROR, Wrong eARC capability size %d\n", EARC_CAP_TABLE_SIZE);
        return;
    }

    if (EARC_CAP_TABLE_SIZE > (u16Temp + EARC_CAP_BANK_SIZE))
    {
        u8Cnt = EARC_CAP_BANK_SIZE;
    }
    else
    {
        for (u8Temp = 0; u8Temp < EARC_ZERO_SIZE; u8Temp++)
        {
            *pu8Ptr++ = 0;
        }
        if (EARC_CAP_TABLE_SIZE > u16Temp)
        {
            u8Cnt = EARC_CAP_TABLE_SIZE - u16Temp;
        }
        else
        {
            u8Cnt = 0;
        }
    }
    SB_I2C_Write(0x40, u8Cnt, (rt_uint8_t *)&u8eArcCapTable[u16Temp]);

    for (; u8Cnt < EARC_CAP_BANK_SIZE;)
    {
        if ((u8Cnt + EARC_ZERO_SIZE) < EARC_CAP_BANK_SIZE)
        {
            u8Temp = EARC_ZERO_SIZE;
        }
        else
        {
            u8Temp = EARC_CAP_BANK_SIZE - u8Cnt;
        }
        SB_I2C_Write(0x40 + u8Cnt, u8Temp, u8eArcZero);
        u8Cnt += u8Temp;
    }
}

static rt_uint8_t   const u8IteChipId[][4] =
{
    {SB_I2C_IT66322, IT66322_CHIP_ID_0, IT66322_CHIP_ID_1, IT66322_CHIP_ID_2},
    {SB_I2C_IT66323, IT66323_CHIP_ID_0, IT66323_CHIP_ID_1, IT66323_CHIP_ID_2},
    {SB_I2C_IT66324, IT66324_CHIP_ID_0, IT66324_CHIP_ID_1, IT66324_CHIP_ID_2},
    {SB_I2C_IT6622A, IT6622A_CHIP_ID_0, IT6622A_CHIP_ID_1, IT6622A_CHIP_ID_2},
    {SB_I2C_IT6622B, IT6622B_CHIP_ID_0, IT6622B_CHIP_ID_1, IT6622B_CHIP_ID_2},
    {SB_I2C_IT66320, IT66320_CHIP_ID_0, IT66320_CHIP_ID_1, IT66320_CHIP_ID_2},
};

#define ITE_CHIP_SUP_MAX    (sizeof(u8IteChipId)/sizeof(u8IteChipId[0]))

static void SB_I2C_ClearInt(void)
{
    rt_uint8_t  u8Data;

    switch (g_u8ChipId)
    {
    case    SB_I2C_IT66322:
    case    SB_I2C_IT66323:
    case    SB_I2C_IT6622A:
        u8Data = 0;
        break;
    case    SB_I2C_IT66324:
    case    SB_I2C_IT6622B:
    case    SB_I2C_IT66320:
        u8Data = 0xFF;
        break;
    default:
        rt_kprintf("****************** ITE not found ****************\r\n");
        return;
    }
    SB_I2C_Write(I2C_SYS_INT, 1, &u8Data);
}

static rt_uint8_t   SB_I2C_ChipGet(void)
{
    rt_uint8_t  u8Ver[9];
    rt_uint8_t  u8Cnt;

    SB_I2C_Read(I2C_ITE_CHIP, 9, u8Ver);
    rt_kprintf("SB Protocol Version: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", u8Ver[0], u8Ver[1], u8Ver[2], u8Ver[3], u8Ver[4], u8Ver[5], u8Ver[6], u8Ver[7], u8Ver[8]);

    for (u8Cnt = 0; u8Cnt < ITE_CHIP_SUP_MAX; u8Cnt++)
    {
        if ((u8Ver[0] == u8IteChipId[u8Cnt][1]) && (u8Ver[1] == u8IteChipId[u8Cnt][2]) && (u8Ver[2] == u8IteChipId[u8Cnt][3]))
        {
            return u8IteChipId[u8Cnt][0];
        }
    }

    return SB_I2C_NO_ITE;
}

static void SB_I2cIrq(void)
{
    // SW3 press
    static rt_uint8_t   u8I2cIntRec = 0;
    rt_uint8_t  u8I2cChg = 0;
    rt_uint8_t  u8I2cInt;
    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);
    SB_I2C_Read(I2C_SYS_INT, 1, &u8I2cInt);

    if (u8I2cInt)
    {
        //rt_kprintf("SB Update = %X, %X\n", u8I2cInt, u8I2cChg);
        if (g_u8ChipId == SB_I2C_NO_ITE)
        {
            g_u8ChipId = SB_I2C_ChipGet();
        }

        if (g_u8ChipId == SB_I2C_NO_ITE)
        {
            rt_kprintf("********************* ITE chip not found *********************\n");
            return;
        }

#ifdef IT6632X_USE_RESET_PIN
        if ((u8I2cInt & I2C_INT_READY_MASK) == I2C_INT_READY_SET)
#else
        if (g_u8IteRdy != 1)
#endif
        {
            rt_uint8_t  u8Data = 0;
            u8Data += 0;
            u8I2cIntRec |= u8I2cInt & (~I2C_INT_READY_SET);
            SB_I2C_ClearInt();
            SB_I2C_Write(I2C_SYS_CHANGE, 1, &u8Data);
            if (u8I2cChg == 0)
            {
                SB_I2cWriteEarcCap(0);
                u8I2cChg = I2C_INIT_SET | I2C_INIT_STAG_0 | I2C_INIT_RDY_SET;   //0x81
            }
            else if (u8I2cChg == (I2C_INIT_SET | I2C_INIT_STAG_0))
            {
                SB_I2cWriteEarcCap(1);
                u8I2cChg = I2C_INIT_SET | I2C_INIT_STAG_1 | I2C_INIT_RDY_SET;   //0x83;
            }
            else if (u8I2cChg == (I2C_INIT_SET | I2C_INIT_STAG_1))
            {
                SB_I2C_Write(0x40, 0x03, (rt_uint8_t *)u8CecVendorId);
                SB_I2C_Write(0x43, 0x0D, (rt_uint8_t *)g_u8OSDString) ; //CEC OSD string
                SB_I2C_Write(0x50, 0x0E, (rt_uint8_t *)u8CecOsdName);
                u8I2cChg = I2C_INIT_SET | I2C_INIT_STAG_2 | I2C_INIT_RDY_SET;   //0x85;
            }
            else if (u8I2cChg == (I2C_INIT_SET | I2C_INIT_STAG_2))
            {
                u8I2cChg = 0;
                if (u8I2cIntRec)
                {
                    u8I2cChg |= I2C_UPD_TRI_INT_SET;
                }
                g_u8IteRdy = 1;
                g_it6632x_dev->status.dev_status = RK_REPEAT_READY;
                u8I2cChg |= SB_DefaultSetting();
                u8I2cChg |= SB_PowerOn();//SB_Standby();
            }
            rt_kprintf("SB init = %X\n", u8I2cChg);
        }
        else
        {
            u8I2cInt |= u8I2cIntRec;
            u8I2cIntRec = 0;
            SB_I2C_ClearInt();
            u8I2cChg = 0;
#ifdef IT6632X_DEBUG
            SB_I2C_Dump();
#endif
            it6632x_get_status(&g_it6632x_dev->status);

            if ((u8I2cInt & I2C_INT_AUDIO_MASK) == I2C_INT_AUDIO_SET)
            {
                rt_uint8_t  u8IT66322AdoInfo[5];
                rt_uint32_t u32IT66322AdoInfo;
                rt_uint8_t  u8IT66322CA;
                rt_uint8_t  u8AudioVolume;
                rt_uint8_t  u8eArcLatency;

                SB_I2C_Read(I2C_ADO_VOL, 1, &u8AudioVolume);
                if (g_u8SbVolume != u8AudioVolume)
                {
                    rt_kprintf("+u8SbVol=%d, u8AudioVolume=%d\n", g_u8SbVolume, u8AudioVolume);
                    if ((g_u8SbVolume & I2C_ADO_VOL_MUTE_MASK) != (u8AudioVolume & I2C_ADO_VOL_MUTE_MASK))
                    {
                        SB_CecUserCtl_Mute();
                    }
                    else
                    {
                        if (g_u8SbVolume > u8AudioVolume)
                        {
                            SB_CecUserCtl_VolumeDown();
                        }
                        else
                        {
                            SB_CecUserCtl_VolumeUp();
                        }
                    }
                    g_u8SbVolume = u8AudioVolume;
                    rt_kprintf("-u8SbVol=%d, u8AudioVolume=%d\n", g_u8SbVolume, u8AudioVolume);
                }

                SB_I2C_Read(I2C_ADO_INFO, 5, u8IT66322AdoInfo);
                u8IT66322CA = u8IT66322AdoInfo[4];
                u32IT66322AdoInfo = u8IT66322AdoInfo[3];
                u32IT66322AdoInfo <<= 8;
                u32IT66322AdoInfo |= u8IT66322AdoInfo[2];
                u32IT66322AdoInfo <<= 8;
                u32IT66322AdoInfo |= u8IT66322AdoInfo[1];
                u32IT66322AdoInfo <<= 8;
                u32IT66322AdoInfo |= u8IT66322AdoInfo[0];
                if (((g_u32SbAdoDecInfo & (~IgnorAdoSta)) != (u32IT66322AdoInfo & (~IgnorAdoSta))) || (g_u8SbInfoCA != u8IT66322CA))
                {
                    rt_kprintf("g_u32SbAdoDecInfo = %lX, u32IT66322AdoInfo = %lX, g_u8SbInfoCA = %X, u8IT66322CA = %X\n", g_u32SbAdoDecInfo, u32IT66322AdoInfo, g_u8SbInfoCA, u8IT66322CA);

                    g_u32SbAdoDecInfo = u32IT66322AdoInfo;
                    g_u8SbInfoCA = u8IT66322CA;

                    if (g_u32SbAdoDecInfo & ADO_CONF_ACTIVE_MASK)
                    {
                        it6632x_get_hdmi_decoderinfo(&g_it6632x_dev->ado_info);

                        if ((g_u32SbAdoDecInfo & (ADO_CONF_SEL_MASK)) == (ADO_CONF_SEL_SPDIF))
                        {
                            SB_AudioIn_SPDIF(g_u32SbAdoDecInfo, g_u8SbInfoCA);
                        }
                        else    // FOR I2S IN
                        {
                            SB_AudioIn_I2S(g_u32SbAdoDecInfo, g_u8SbInfoCA);
                        }
                    }
                }

                SB_I2C_Read(I2C_EARC_LATEN, 1, &u8eArcLatency);
                if (u8eArcLatency == (g_u8SbLaten ^ 0xFF))
                {
                    rt_uint8_t  u8eArcLatenReq;
                    SB_I2C_Read(I2C_EARC_LATEN_REQ, 1, &u8eArcLatenReq);
#if 1   // set g_u8SbLaten to DSP
                    g_u8SbLaten = u8eArcLatenReq;
#endif
                    SB_I2C_Write(I2C_EARC_LATEN, 1, &g_u8SbLaten);
                    u8eArcLatenReq = u8eArcLatenReq ^ 0xFF;
                    SB_I2C_Write(I2C_EARC_LATEN_REQ, 1, &u8eArcLatenReq);
                    u8I2cChg |= I2C_UPD_ADO_SET;
                    rt_kprintf("eARC Latency Req = %d ms, Latency = %d ms\n", u8eArcLatenReq, g_u8SbLaten);
                }
            }

            if ((u8I2cInt & I2C_INT_TV_LATENCY_MASK) == I2C_INT_TV_LATENCY_SET)
            {
                rt_uint8_t  u8EdidData[2];
                SB_I2C_Read(I2C_EDID_PA_AB, 2, u8EdidData);
                if ((g_u8SbEdidPaAB != u8EdidData[0]) || (g_u8SbEdidPaCD != u8EdidData[1]))
                {
                    rt_kprintf("PA change %02X%02X->%02X%02X\n", g_u8SbEdidPaAB, g_u8SbEdidPaCD, u8EdidData[0], u8EdidData[1]);
                    g_u8SbEdidPaAB = u8EdidData[0];
                    g_u8SbEdidPaCD = u8EdidData[1];
                }
            }
#if (EN_CEC_SYS == iTE_TRUE)
            if ((u8I2cInt & I2C_INT_SYS_MASK) == I2C_INT_SYS_SET)
            {
                rt_uint8_t  u8I2c0xF1[4];
                rt_uint8_t  u8Temp;

                SB_I2C_Read(I2C_SYS_ADO_MODE, 4, u8I2c0xF1);
                u8Temp = g_u8SbTxSta ^ u8I2c0xF1[1];
                g_u8SbTxSta = u8I2c0xF1[1];

                if (u8Temp)
                {
                    if (u8Temp & I2C_TV_STA_HPD_MASK)
                    {
                        rt_kprintf("TV HPD = %d\n", g_u8SbTxSta & I2C_TV_STA_HPD_MASK);
                    }

                    if (u8Temp & I2C_TV_STA_CEC_MASK)
                    {
                        if ((g_u8SbTxSta & I2C_TV_STA_CEC_MASK) == I2C_TV_STA_CEC_ON)
                        {
                            g_u8SbAudioMode |= I2C_MODE_CEC_EN;
                        }
                        else
                        {
                        }
                        SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
                        u8I2cChg |= I2C_UPD_SYS_SET;
                    }
                    if (u8Temp & I2C_TV_STA_PWR_ON_MASK)
                    {
                        if ((g_u8SbTxSta & I2C_TV_STA_PWR_ON_MASK) == I2C_TV_STA_PWR_ON_SET)
                        {
                            rt_kprintf("TV Power ON \n");
                            if (g_u8SbCecMode == SB_CEC_AUTO)
                            {
                                if ((g_u8SbAudioMode & I2C_MODE_POWER_MASK) == I2C_MODE_POWER_ON)
                                {
                                    g_u8SbAudioMode |= I2C_MODE_CEC_EN | I2C_MODE_EARC_EN | I2C_MODE_ADO_SYS_EN;// 0x38;//0x20;
                                    rt_kprintf("SB CEC ON, Audio system On, eARC/ARC Enable \n");
                                }
                            }
                        }
                        else
                        {
                            rt_kprintf("TV Power OFF \n");
                            if (g_u8SbCecMode == SB_CEC_AUTO)
                            {
                                //g_u8SbAudioMode &= ~(I2C_MODE_CEC_EN | I2C_MODE_EARC_EN | I2C_MODE_ADO_SYS_EN);//~0x38;//0x20;
                                rt_kprintf("SB CEC OFF, Audio system OFF, eARC/ARC Disable \n");
                            }

                        }
                        SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
                        u8I2cChg |= I2C_UPD_SYS_SET;
                    }
                    if (u8Temp & 0x10)
                    {
                        rt_uint8_t  u8AdoSel;
                        if ((g_u8SbAudioMode & I2C_MODE_CEC_MASK) == I2C_MODE_CEC_EN)
                        {
                            if ((g_u8SbTxSta & I2C_TV_STA_REQ_ADO_MASK) == I2C_TV_STA_REQ_ADO_SET)
                            {
                                rt_kprintf("CEC On switch to TV\n");
                                u8AdoSel = 1;
                            }
                            else
                            {
                                rt_kprintf("CEC On switch to HDMI\n");
                                u8AdoSel = 0;
                            }
                        }
                        else
                        {
                            rt_kprintf("CEC Off switch to HDMI\n");
                            u8AdoSel = 0;
                            SB_STA_CHANGE(g_u8SbTxSta, I2C_TV_STA_REQ_ADO_MASK, I2C_TV_STA_REQ_ADO_CLR);
                        }
                        SB_I2C_Write(I2C_ADO_SEL, 1, &u8AdoSel);
                        u8I2cChg |= I2C_UPD_ADO_SET;
                    }
                }

                if (u8I2c0xF1[2])
                {
                    g_u8IteCecReq = u8I2c0xF1[2];
                    rt_kprintf("CEC REQ [0x%02X]\n", g_u8IteCecReq);
                    if (g_u8IteCecReq & I2C_CEC_CMD_SWITCH_RX_MASK)
                    {
                        if (u8I2c0xF1[1] & I2C_TV_STA_REQ_ADO_MASK)
                        {
                        }
                        else
                        {
                            switch (u8I2c0xF1[1] & I2C_TV_STA_REQ_SWITCH_MASK)
                            {
                            case    I2C_TV_STA_REQ_SWITCH_R0:
                                g_u8SbRxSel = I2C_HDMI_SELECT_R0 | I2C_HDMI_ADO_SEL_R0;
                                break;
                            case    I2C_TV_STA_REQ_SWITCH_R1:
                                g_u8SbRxSel = I2C_HDMI_SELECT_R1 | I2C_HDMI_ADO_SEL_R1;
                                break;
                            default:
                                break;
                            }

                            //u8Temp = I2C_HDMI_SEL_W_INF | g_u8SbRxSel;
                            u8Temp = g_u8SbRxSel;
                            if (g_u8IteCecReq & I2C_CEC_CMD_ROUT_CHG_MASK)
                            {
                                u8Temp |= I2C_HDMI_SEL_W_INF;
                            }
                            SB_I2C_Write(I2C_SYS_RX_SEL, 1, &u8Temp);
                            if ((g_u8SbAudioMode & I2C_MODE_TX_OUT_MASK) == I2C_MODE_TX_OUT_DIS)
                            {
                                SB_STA_CHANGE(g_u8SbAudioMode, I2C_MODE_TX_OUT_MASK, I2C_MODE_TX_OUT_EN);
                                SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
                            }
                            u8I2cChg |= I2C_UPD_SYS_SET;
                            rt_kprintf("CEC Routing Change to R%d\n", g_u8SbRxSel & 0xF);
                        }
                    }
                    if (g_u8IteCecReq & I2C_CEC_CMD_LATCH_MASK)
                    {
                        rt_uint8_t  u8CecCmd[0x10], u8Cnt, u8LatchCnt;
                        SB_I2C_Read(I2C_CEC_LATCH_DATA, 0x10, u8CecCmd);
                        if ((g_u8ChipId == SB_I2C_IT6622B) || (g_u8ChipId == SB_I2C_IT6622A))
                        {
                            SB_I2C_Read(I2C_SYS_CEC_LATCH_CNT_6622, 1, &u8LatchCnt);
                        }
                        else
                        {
                            SB_I2C_Read(I2C_SYS_CEC_LATCH_CNT, 0x01, &u8LatchCnt);
                        }
                        rt_kprintf("CEC Command Latch 0x%02X, Cnt = %d\n", u8CecCmd[1], u8LatchCnt);
                        for (u8Cnt = 0; u8Cnt < u8LatchCnt; u8Cnt++)
                        {
                            rt_kprintf(" %02X", u8CecCmd[u8Cnt]);
                        }
                        rt_kprintf("\n");

                    }
                    if (g_u8IteCecReq & (I2C_CEC_CMD_SYS_ADO_ON_MASK | I2C_CEC_CMD_ACTIVE_SRC_MASK))
                    {
                        u8I2cChg |= SB_PowerOn();
                    }
                    if (g_u8IteCecReq & I2C_CEC_CMD_STANDBY_MASK)
                    {
                        u8I2cChg |= SB_Standby();
                    }

                    if (g_u8IteCecReq & I2C_CEC_CMD_SYS_ADO_ON_MASK)
                    {
                        if (g_u8SbPower != SB_POWER_OFF)
                        {
                            u8I2cChg |= SB_PowerOn();
                            u8I2cChg |= SB_EdidChg(1);
                        }
                    }
                    else if (g_u8IteCecReq & I2C_CEC_CMD_SYS_ADO_OFF_MASK)
                    {
                        if (0)  // may need enable for some TV no toggle HPD when change to eARC mode from TV Speaker,but Sony 50X90J will show "audio system is unavailable" temporary then go back to audio system
                        {
                            u8I2cChg |= SB_Standby();
                            u8I2cChg |= SB_EdidChg(0);
                        }
                    }

                    u8Temp = 0;
                    SB_I2C_Write(I2C_SYS_CEC_RECEIVE, 1, &u8Temp);
                }
                if (u8I2c0xF1[3])
                {
                    rt_uint8_t  u8CecReq2 = u8I2c0xF1[3];
                    rt_kprintf("CEC REQ2 [0x%02X]\n", u8CecReq2);
                    if (u8CecReq2 & I2C_CEC_CMD_POWER_MASK)
                    {
                        if ((g_u8SbAudioMode & I2C_MODE_POWER_MASK) == I2C_MODE_POWER_ON)
                        {
                            u8I2cChg |= SB_PowerOff();
                        }
                        else
                        {
                            u8I2cChg |= SB_PowerOn();
                        }
                    }
                    if (u8CecReq2 & I2C_CEC_CMD_POWER_ON_MASK)
                    {
                        u8I2cChg |= SB_PowerOn();
                    }
                    if (u8CecReq2 & I2C_CEC_CMD_POWER_OFF_MASK)
                    {
                        u8I2cChg |= SB_PowerOff();
                    }
                    u8CecReq2 = 0;
                    SB_I2C_Write(I2C_SYS_CEC_RECEIVE2, 1, &u8CecReq2);
                }
            }
#endif
        }
        SB_SysI2cChange(u8I2cChg);
    }
}

static void it6632x_power_mode(it6632x_power_mode_t mode)
{
    rt_uint8_t  u8I2cChg = 0;
    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);

    if (!g_u8IteRdy)
    {
        rt_kprintf("it6632x not ready\n");
    }
    switch (mode)
    {
    case IT6632X_MODE_STANDBY:
        u8I2cChg |= SB_Standby();
        break;
    case IT6632X_MODE_POWER_ON:
        u8I2cChg |= SB_PowerOn();
        break;
    case IT6632X_MODE_POWER_OFF:
        u8I2cChg |= SB_PowerOff();
        break;
    case IT6632X_MODE_PASSTHROUGH:
        u8I2cChg |= SB_PassThrough();
        break;
    default:
        rt_kprintf("NoSupport\n");
#ifdef IT6632X_DEBUG
        SB_I2C_Dump();
#endif
        break;
    }

    SB_SysI2cChange(u8I2cChg);
}

static rt_err_t it6632x_ado_path_set(struct hdmi_repeat *rpt, void *data)
{
    rt_uint8_t  u8I2cChg = 0;
    rt_uint8_t  u8Temp;
    rt_uint8_t  no_config = 0;

    rk_rpt_audio_path_t ado_path = *((rt_uint8_t *)data);

    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);

    if (!g_u8IteRdy)
    {
        rt_kprintf("it6632x not ready\n");
    }

    if (g_u8SbPower != SB_POWER_OFF)
    {
        switch (ado_path)
        {
        case RK_REPEAT_AUDIO_PATH_HDMI_0:
            rt_kprintf("swith ado path to HDMI 0 \n");
            g_u8SbRxSel = I2C_HDMI_SELECT_R0 | I2C_HDMI_ADO_SEL_R0 | I2C_HDMI_SEL_W_CHG;
            SB_I2C_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);
            if (g_u8SbAudioMode & I2C_MODE_TX_OUT_MASK)
            {
                SB_STA_CHANGE(g_u8SbAudioMode, I2C_MODE_TX_OUT_MASK, I2C_MODE_TX_OUT_EN);
            }
            u8Temp = I2C_ADO_MUX_SEL_HDMI;
            break;
        case RK_REPEAT_AUDIO_PATH_HDMI_1:
            if (g_u8ChipId != SB_I2C_IT66320)
            {
                rt_kprintf("swith ado path to HDMI 1 \n");
                g_u8SbRxSel = I2C_HDMI_SELECT_R1 | I2C_HDMI_ADO_SEL_R1 | I2C_HDMI_SEL_W_CHG;
                SB_I2C_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);
                if (g_u8SbAudioMode & I2C_MODE_TX_OUT_MASK)
                {
                    SB_STA_CHANGE(g_u8SbAudioMode, I2C_MODE_TX_OUT_MASK, I2C_MODE_TX_OUT_EN);
                }
                u8Temp = I2C_ADO_MUX_SEL_HDMI;
            }
            else
            {
                no_config = 1;
            }
            break;
        case RK_REPEAT_AUDIO_PATH_HDMI_2:
            if (g_u8ChipId != SB_I2C_IT66320)
            {
                rt_kprintf("swith ado path to HDMI 2 \n");
                g_u8SbRxSel = I2C_HDMI_SELECT_R2 | I2C_HDMI_ADO_SEL_R2 | I2C_HDMI_SEL_W_CHG;
                SB_I2C_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);
                if (g_u8SbAudioMode & I2C_MODE_TX_OUT_MASK)
                {
                    SB_STA_CHANGE(g_u8SbAudioMode, I2C_MODE_TX_OUT_MASK, I2C_MODE_TX_OUT_EN);
                }
                u8Temp = I2C_ADO_MUX_SEL_HDMI;
            }
            else
            {
                no_config = 1;
            }
            break;
        case RK_REPEAT_AUDIO_PATH_ARC_EARC:
            rt_kprintf("swith ado path to ARC/eARC\n");
            g_u8SbAudioMode |= I2C_MODE_ADO_SYS_EN | I2C_MODE_EARC_EN | I2C_MODE_CEC_EN; // 0x38;
            u8Temp = I2C_ADO_MUX_SEL_EARC;
            break;
        case RK_REPEAT_AUDIO_PATH_EXT_I2S1:
            rt_kprintf("swith ado path to ext_i2s1\n");
            u8Temp = I2C_ADO_MUX_SEL_EXT_I2S1;
            g_u8SbAudioMode |= I2C_MODE_TX_OUT_DIS;
            break;
        case RK_REPEAT_AUDIO_PATH_EXT_I2S2:
            rt_kprintf("swith ado path to ext_i2s2\n");
            u8Temp = I2C_ADO_MUX_SEL_EXT_I2S2;
            g_u8SbAudioMode |= I2C_MODE_TX_OUT_DIS;
            break;
        case RK_REPEAT_AUDIO_PATH_EXT_I2S3:
            rt_kprintf("swith ado path to ext_i2s3\n");
            u8Temp = I2C_ADO_MUX_SEL_EXT_I2S3;
            g_u8SbAudioMode |= I2C_MODE_TX_OUT_DIS;
            break;
        case RK_REPEAT_AUDIO_PATH_EXT_SPDIF1:
            rt_kprintf("swith ado path to ext_spdif1\n");
            u8Temp = I2C_ADO_MUX_SEL_EXT_SPDIF1;
            g_u8SbAudioMode |= I2C_MODE_TX_OUT_DIS;
            break;
        case RK_REPEAT_AUDIO_PATH_EXT_SPDIF2:
            rt_kprintf("swith ado path to ext_spdif2\n");
            u8Temp = I2C_ADO_MUX_SEL_EXT_SPDIF2;
            g_u8SbAudioMode |= I2C_MODE_TX_OUT_DIS;
            break;
        case RK_REPEAT_AUDIO_PATH_EXT_SPDIF3:
            rt_kprintf("swith ado path to ext_spdif3\n");
            u8Temp = I2C_ADO_MUX_SEL_EXT_SPDIF3;
            g_u8SbAudioMode |= I2C_MODE_TX_OUT_DIS;
            break;
        default:
            rt_kprintf("NoSupport\n");
            no_config = 1;
#ifdef IT6632X_DEBUG
            SB_I2C_Dump();
#endif
            break;
        }

        if (!no_config)
        {
            u8I2cChg |= (I2C_UPD_ADO_SET | I2C_UPD_SYS_SET);
            SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
            SB_I2C_Write(I2C_ADO_SEL, 1, &u8Temp);
        }
    }
    else
    {
        rt_kprintf("it6632x not ready\n");
    }

    SB_SysI2cChange(u8I2cChg);

    return RT_EOK;
}

static rt_err_t it6632x_ado_path_get(struct hdmi_repeat *rpt, void *data)
{
    rt_uint8_t temp;
    rt_uint8_t *ado_path = (rt_uint8_t *)data;

    SB_I2C_Read(I2C_ADO_SEL, 1, &temp);
    switch (temp & 0x0f)
    {
    case 0:
        SB_I2C_Read(I2C_SYS_RX_SEL, 1, &temp);
        temp = temp >> 4;
        if (temp == 0)
        {
            *ado_path = RK_REPEAT_AUDIO_PATH_HDMI_0;
        }
        else if (temp == 1)
        {
            *ado_path = RK_REPEAT_AUDIO_PATH_HDMI_1;
        }
        else if (temp == 2)
        {
            *ado_path = RK_REPEAT_AUDIO_PATH_HDMI_2;
        }
        else
        {
            *ado_path = RK_REPEAT_AUDIO_PATH_UNKNOW;
        }
        break;
    case 1:
        *ado_path = RK_REPEAT_AUDIO_PATH_ARC_EARC;
        break;
    case 2:
        *ado_path = RK_REPEAT_AUDIO_PATH_EXT_I2S1;
        break;
    case 3:
        *ado_path = RK_REPEAT_AUDIO_PATH_EXT_I2S2;
        break;
    case 4:
        *ado_path = RK_REPEAT_AUDIO_PATH_EXT_I2S3;
        break;
    case 5:
        *ado_path = RK_REPEAT_AUDIO_PATH_EXT_SPDIF1;
        break;
    case 6:
        *ado_path = RK_REPEAT_AUDIO_PATH_EXT_SPDIF2;
        break;
    case 7:
        *ado_path = RK_REPEAT_AUDIO_PATH_EXT_SPDIF3;
        break;
    default:
        *ado_path = RK_REPEAT_AUDIO_PATH_UNKNOW;
        break;
    }
    return RT_EOK;
}

static rt_err_t it6632x_e_arc_vol_set(struct hdmi_repeat *rpt, void *data)
{
    struct rk_rpt_volume_info *info = (struct rk_rpt_volume_info *)data;
    rt_uint8_t  u8I2cChg = 0;

    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);

    if (!g_u8IteRdy)
    {
        rt_kprintf("it6632x not ready\n");
    }

    if (g_u8SbPower != SB_POWER_OFF)
    {
        //0~100
        if (info->volume > SB_VOL_MAX)
            info->volume = SB_VOL_MAX;

        if (info->is_mute)
            g_u8SbVolume = (info->volume | I2C_ADO_VOL_MUTE_SET);
        else
            g_u8SbVolume = (info->volume & ~I2C_ADO_VOL_MUTE_CLR);

        SB_I2C_Write(I2C_ADO_VOL, 1, &g_u8SbVolume);
        u8I2cChg |= I2C_UPD_ADO_SET;
        rt_kprintf("Vol:%d, %s\n", g_u8SbVolume & 0x7f, (g_u8SbVolume & 0x80) ? "mute" : "unmute");
    }
    else
    {
        rt_kprintf("it6632x not ready\n");
    }

    SB_SysI2cChange(u8I2cChg);

    return RT_EOK;
}

static void it6632x_arc_set_max_volume(rt_uint8_t volume_max)
{
    rt_uint8_t  u8I2cChg = 0;
    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);

    if (!g_u8IteRdy)
    {
        rt_kprintf("it6632x not ready\n");
    }

    if (g_u8SbPower != SB_POWER_OFF)
    {
        SB_I2C_Write(I2C_VOLUME_MAX, sizeof(volume_max), &volume_max);
    }
    else
    {
        rt_kprintf("it6632x not ready\n");
    }

    SB_SysI2cChange(u8I2cChg);
}

static rt_err_t it6632x_e_arc_vol_get(struct hdmi_repeat *rpt, void *data)
{
    struct rk_rpt_volume_info *info = (struct rk_rpt_volume_info *)data;
    rt_uint8_t  u8I2cChg = 0;
    rt_uint8_t  u8Temp;

    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);

    if (!g_u8IteRdy)
    {
        rt_kprintf("it6632x not ready\n");
    }

    if (g_u8SbPower != SB_POWER_OFF)
    {
        SB_I2C_Read(I2C_ADO_VOL,  sizeof(u8Temp), &u8Temp);
        info->is_mute = ((u8Temp & 0x80) == 0x80);
        info->volume = u8Temp & 0x7f;

        rt_kprintf("Vol:%d, %s\n", info->volume, info->is_mute ? "mute" : "unmute");
    }
    else
    {
        rt_kprintf("it6632x not ready\n");
    }

    SB_SysI2cChange(u8I2cChg);

    return RT_EOK;
}

static void it6632x_audio_system_enable(rt_uint8_t enable)
{
    rt_uint8_t  u8I2cChg = 0;
    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);

    if (!g_u8IteRdy)
    {
        rt_kprintf("it6632x not ready\n");
    }

    if (g_u8SbPower != SB_POWER_OFF)
    {
        if (enable)
            g_u8SbAudioMode |= I2C_MODE_ADO_SYS_EN;
        else
            g_u8SbAudioMode &= ~(I2C_MODE_ADO_SYS_EN);

        SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
        u8I2cChg |= I2C_UPD_SYS_SET;
        if (g_u8SbAudioMode & I2C_MODE_ADO_SYS_MASK)
        {
            rt_kprintf("Audio System ON \n");
        }
        else
        {
            rt_kprintf("Audio System OFF \n");
        }
    }
    else
    {
        rt_kprintf("it6632x not ready\n");
    }

    SB_SysI2cChange(u8I2cChg);
}

static void it6632x_earc_enable(rt_uint8_t enable)
{
    rt_uint8_t  u8I2cChg = 0;
    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);

    if (!g_u8IteRdy)
    {
        rt_kprintf("it6632x not ready\n");
    }

    if (g_u8SbPower != SB_POWER_OFF)
    {
        if (enable)
            g_u8SbAudioMode |= I2C_MODE_EARC_EN;
        else
            g_u8SbAudioMode &= ~(I2C_MODE_EARC_EN);

        if (g_u8SbAudioMode & I2C_MODE_EARC_MASK)
        {
            g_u8SbAudioMode |= I2C_MODE_ADO_SYS_EN;
        }
        SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
        u8I2cChg |= I2C_UPD_SYS_SET;
        if (g_u8SbAudioMode & I2C_MODE_EARC_MASK)
        {
            rt_kprintf("eARC/ARC ON \n");
        }
        else
        {
            rt_kprintf("eARC/ARC OFF \n");
        }
    }
    else
    {
        rt_kprintf("it6632x not ready\n");
    }

    SB_SysI2cChange(u8I2cChg);
}

static void it6632x_sb_cec_enable(rt_uint8_t enable)
{
    rt_uint8_t  u8I2cChg = 0;
    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);

    if (!g_u8IteRdy)
    {
        rt_kprintf("it6632x not ready\n");
    }

    if (g_u8SbPower != SB_POWER_OFF)
    {
        if (enable)
        {
            rt_uint8_t  u8TxSta;
            g_u8SbCecMode = SB_CEC_AUTO;
            SB_I2C_Read(I2C_SYS_TX_STA, 1, &u8TxSta);
            //check tv cec status
            if (u8TxSta & I2C_TV_STA_CEC_MASK)
            {
                g_u8SbAudioMode |= I2C_MODE_CEC_EN;
            }
            else
            {
                g_u8SbAudioMode &= ~I2C_MODE_CEC_EN;
            }
        }
        else
        {
            g_u8SbCecMode = SB_CEC_OFF;
            g_u8SbAudioMode &= ~I2C_MODE_CEC_EN;
        }

        SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
        u8I2cChg |= I2C_UPD_SYS_SET;
        if (g_u8SbAudioMode & 0x20)
        {
            rt_kprintf("SB CEC ON \n");
        }
        else
        {
            rt_kprintf("SB CEC OFF \n");
        }
    }
    else
    {
        rt_kprintf("it6632x not ready\n");
    }

    SB_SysI2cChange(u8I2cChg);
}

static void it6632x_tx_audio_mute(rt_uint8_t mute)
{
    rt_uint8_t  u8I2cChg = 0;
    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);

    if (!g_u8IteRdy)
    {
        rt_kprintf("it6632x not ready\n");
    }

    if (g_u8SbPower != SB_POWER_OFF)
    {
        if (mute)
            g_u8SbAudioMode |= I2C_MODE_TX_ADO_DIS;
        else
            g_u8SbAudioMode &= ~I2C_MODE_TX_ADO_DIS;

        SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
        u8I2cChg |= I2C_UPD_SYS_SET;
        if (g_u8SbAudioMode & I2C_MODE_TX_ADO_MASK)
        {
            rt_kprintf("Tx Audio Mute \n");
        }
        else
        {
            rt_kprintf("Tx Audio UnMute \n");
        }
    }
    else
    {
        rt_kprintf("it6632x not ready\n");
    }

    SB_SysI2cChange(u8I2cChg);
}

static void it6632x_tx_video_mute(rt_uint8_t mute)
{
    rt_uint8_t  u8I2cChg = 0;
    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);

    if (!g_u8IteRdy)
    {
        rt_kprintf("it6632x not ready\n");
    }

    if (g_u8SbPower != SB_POWER_OFF)
    {
        if (mute)
            g_u8SbAudioMode |= I2C_MODE_TX_OUT_DIS;
        else
            g_u8SbAudioMode &= ~I2C_MODE_TX_OUT_DIS;

        SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
        u8I2cChg |= I2C_UPD_SYS_SET;
        if (g_u8SbAudioMode & I2C_MODE_TX_OUT_MASK)
        {
            rt_kprintf("Tx Disable Output \n");
        }
        else
        {
            rt_kprintf("Tx Output \n");
        }
    }
    else
    {
        rt_kprintf("it6632x not ready\n");
    }

    SB_SysI2cChange(u8I2cChg);
}

static void it6632x_hdcp_repeat(rt_uint8_t enable)
{
    rt_uint8_t  u8I2cChg = 0;
    rt_uint8_t  u8Temp;
    SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);

    if (!g_u8IteRdy)
    {
        rt_kprintf("it6632x not ready\n");
    }

    if (g_u8SbPower != SB_POWER_OFF)
    {
        SB_I2C_Read(I2C_SYS_RX_MODE_0, 1, &u8Temp);
        if (enable)
            u8Temp |= I2C_RX0_HDCP_RPT_SET;
        else
            u8Temp &= ~I2C_RX0_HDCP_RPT_SET;
        SB_I2C_Write(I2C_SYS_RX_MODE_0, 1, &u8Temp);
        rt_kprintf("HDMI 0 -> HDCP Repeater = %d\n", (u8Temp & 0x04) >> 2);
        SB_I2C_Read(I2C_SYS_RX_MODE_1, 1, &u8Temp);

        if (enable)
            u8Temp |= I2C_RX1_HDCP_RPT_SET;
        else
            u8Temp &= ~I2C_RX1_HDCP_RPT_SET;
        SB_I2C_Write(I2C_SYS_RX_MODE_1, 1, &u8Temp);
        rt_kprintf("HDMI 1 -> HDCP Repeater = %d\n", (u8Temp & 0x04) >> 2);
        u8I2cChg |= I2C_UPD_SYS_SET;
    }
    else
    {
        rt_kprintf("it6632x not ready\n");
    }

    SB_SysI2cChange(u8I2cChg);
}

static void it6632x_show_hdmi_decoderinfo(struct it6632x_ado_info *ado_info)
{

    if (ado_info == RT_NULL)
    {
        rt_kprintf("%s ado_info is null!", __func__);
        return;
    }

    rt_kprintf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    rt_kprintf("it6632x_hdmi_get_decoderinfo:\n");

    rt_kprintf("audio output interface is %s\n", (ado_info->audio_output_inetrface) ? "SPDIF" : "I2S");

    if (!ado_info->audio_output_inetrface)
    {
        rt_kprintf("channel number is %d\n", ado_info->audio_channel_number);

        rt_kprintf("audio sample frequency is ");

        if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_44_1k)
            rt_kprintf("44K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_48k)
            rt_kprintf("48K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_32k)
            rt_kprintf("32K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_384k)
            rt_kprintf("384K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_88_2k)
            rt_kprintf("88K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_768k)
            rt_kprintf("768K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_96k)
            rt_kprintf("96K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_64k)
            rt_kprintf("64K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_176_4k)
            rt_kprintf("176K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_352k)
            rt_kprintf("352K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_192k)
            rt_kprintf("192K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_1536k)
            rt_kprintf("1536K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_256k)
            rt_kprintf("256K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_1411k)
            rt_kprintf("1441K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_128k)
            rt_kprintf("128K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_705k)
            rt_kprintf("705K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_1024k)
            rt_kprintf("1024K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_512k)
            rt_kprintf("512K");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_REFER_HEADER)
            rt_kprintf("refer to header");
        else if (ado_info->audio_sample_frequency == IT6632X_AUDIO_SAMPLE_RATE_ERR)
            rt_kprintf("err");

        rt_kprintf("\n");

        rt_kprintf("audio output interface is %s\n", (ado_info->audio_output_inetrface) ? "SPDIF" : "I2S");

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

        rt_kprintf("audio sample word length is ");
        if (ado_info->audio_sample_word_length == IT6632X_AUDIO_BITS_16)
            rt_kprintf("16bits");
        else if (ado_info->audio_sample_word_length == IT6632X_AUDIO_BITS_18)
            rt_kprintf("18bits");
        else if (ado_info->audio_sample_word_length == IT6632X_AUDIO_BITS_20)
            rt_kprintf("20bits");
        else if (ado_info->audio_sample_word_length == IT6632X_AUDIO_BITS_24)
            rt_kprintf("24bits");

        rt_kprintf("\n");

        rt_kprintf("i2s format detail is:\n");

        if (ado_info->i2s_format.is_32_bit_wide)
            rt_kprintf("I2S 32-bit wide\n");
        else
            rt_kprintf("wrong format !\n");

        if (ado_info->i2s_format.left_right_justified)
            rt_kprintf("Right justified\n");
        else
            rt_kprintf("Left justified\n");

        if (ado_info->i2s_format.data_1T_delay_to_ws)
            rt_kprintf("Data no delay correspond to WS\n");
        else
            rt_kprintf("Data 1T delay correspond to WS\n");

        if (ado_info->i2s_format.ws_is_left_or_right_channel)
            rt_kprintf("WS = 0 is right channel\n");
        else
            rt_kprintf("WS = 0 is left channel\n");

        if (ado_info->i2s_format.msb_or_lsb)
            rt_kprintf("LSB shift first\n");
        else
            rt_kprintf("MSB shift first\n");

        if (ado_info->is_multi_stream_audio)
            rt_kprintf("is multi stream audio\n");
        else
            rt_kprintf("is not multi stream audio\n");

        if (ado_info->is_tdm)
            rt_kprintf("is TDM\n");
        else
            rt_kprintf("is not TDM\n");

        if (ado_info->audio_layout)
            rt_kprintf("audio layout is layout 1\n");
        else
            rt_kprintf("audio layout is layout 0\n");

        if (ado_info->is_3d_audio)
            rt_kprintf("is 3D audio\n");
        else
            rt_kprintf("is not 3D audio\n");

        if (ado_info->new_auido_input)
            rt_kprintf("new audio received\n");
        else
            rt_kprintf("no new audio received\n");

        if (ado_info->audio_sample_word_length_1 & 0x01)
        {
            rt_kprintf("Max length is 24bits, word length is ");
            if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX24_WORD_LENGTH_NO_INDICATED)
                rt_kprintf("no indicated");
            else if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX24_WORD_LENGTH_20)
                rt_kprintf("20bits");
            else if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX24_WORD_LENGTH_22)
                rt_kprintf("22bits");
            else if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX24_WORD_LENGTH_23)
                rt_kprintf("23bits");
            else if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX24_WORD_LENGTH_24)
                rt_kprintf("24bits");
            else if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX24_WORD_LENGTH_21)
                rt_kprintf("21bits");

            rt_kprintf("\n");
        }
        else
        {
            rt_kprintf("Max length is 20bits, word length is ");
            if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX20_WORD_LENGTH_NO_INDICATED)
                rt_kprintf("no indicated");
            else if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX20_WORD_LENGTH_16)
                rt_kprintf("16bits");
            else if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX20_WORD_LENGTH_18)
                rt_kprintf("18bits");
            else if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX20_WORD_LENGTH_19)
                rt_kprintf("19bits");
            else if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX20_WORD_LENGTH_20)
                rt_kprintf("20bits");
            else if (ado_info->audio_sample_word_length_1 == IT6632X_AUDIO_MAX20_WORD_LENGTH_17)
                rt_kprintf("17bits");
            rt_kprintf("\n");
        }
    }

    rt_kprintf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

static rt_int32_t it6632x_get_status(struct it6632x_status *status)
{
    rt_uint8_t data;

    if (status == RT_NULL)
    {
        rt_kprintf("%s status is null!", __func__);
        return -1;
    }
    rt_mutex_take(g_it6632x_dev->mutex_lock, RT_WAITING_FOREVER);

    SB_I2C_Read(I2C_SYS_ADO_MODE, 1, &data);
    status->power = data & 0x03;
    status->tx_dis_out = (data & 0x80) >> 7;
    status->tx_ado_mute = (data & 0x40) >> 6;
    status->sb_cec = (data & 0x20) >> 5;
    status->audio_system_enable = (data & 0x08) >> 3;
    status->earc_arc_enable = (data & 0x10) >> 4;

    SB_I2C_Read(I2C_SYS_RX_SEL, 1, &data);
    status->rx_sel = data & 0x03;
    status->ado_decode_sel = data >> 4;

    SB_I2C_Read(I2C_SYS_TX_STA, 1, &data);
    status->tv_cec = (data & 0x02) >> 1;

    SB_I2C_Read(I2C_ADO_SRC_STA, 1, &data);
    status->hdmi_audio_ready = (data & 0x04) >> 2;
    status->earc_audio_ready = (data & 0x02) >> 1;
    status->arc_audio_ready = (data & 0x01);

    SB_I2C_Read(I2C_ADO_SEL, 1, &data);
    status->audio_source_sel = data & 0x0f;

    rt_mutex_release(g_it6632x_dev->mutex_lock);

    return 0;
}

static void it6632x_set_update(rt_uint8_t type)
{
    rt_uint8_t value = type;
    rt_uint8_t cur_value = 0;
    rt_uint8_t reg = I2C_SYS_CHANGE;
    int max_wait = 3 * 1000 * 1000;
    int sleep_time = 10 * 1000;

    while (true)
    {
        SB_I2C_Read(reg, sizeof(cur_value), &cur_value);
        if (0x00 == cur_value)
        {
            break;
        }

        max_wait -= sleep_time;
        if (max_wait <= 0)
        {
            break;
        }
        rt_thread_mdelay(sleep_time / 1000);
    }

    if (SB_I2C_Write(reg, 1, &value))
    {
        rt_kprintf("I2C_write reg:0x%x failed!\n", reg);
    }
}

static rt_uint32_t map_sample_rate(it6632x_audio_sample_rate_t samplerate)
{
    switch (samplerate)
    {
    case IT6632X_AUDIO_SAMPLE_RATE_32k:
        return 32000;
    case IT6632X_AUDIO_SAMPLE_RATE_44_1k:
        return 44100;
    case IT6632X_AUDIO_SAMPLE_RATE_48k:
        return 48000;
    case IT6632X_AUDIO_SAMPLE_RATE_64k:
        return 64000;
    case IT6632X_AUDIO_SAMPLE_RATE_88_2k:
        return 88200;
    case IT6632X_AUDIO_SAMPLE_RATE_96k:
        return 96000;
    case IT6632X_AUDIO_SAMPLE_RATE_128k:
        return 128000;
    case IT6632X_AUDIO_SAMPLE_RATE_176_4k:
        return 176400;
    case IT6632X_AUDIO_SAMPLE_RATE_192k:
        return 192000;
    case IT6632X_AUDIO_SAMPLE_RATE_256k:
        return 256000;
    case IT6632X_AUDIO_SAMPLE_RATE_352k:
        return 352800;
    case IT6632X_AUDIO_SAMPLE_RATE_384k:
        return 384000;
    case IT6632X_AUDIO_SAMPLE_RATE_512k:
        return 512000;
    case IT6632X_AUDIO_SAMPLE_RATE_705k:
        return 705600;
    case IT6632X_AUDIO_SAMPLE_RATE_768k:
        return 768000;
    case IT6632X_AUDIO_SAMPLE_RATE_1024k:
        return 1024000;
    case IT6632X_AUDIO_SAMPLE_RATE_1411k:
        return 1411200;
    case IT6632X_AUDIO_SAMPLE_RATE_1536k:
        return 1536000;
    case IT6632X_AUDIO_SAMPLE_RATE_REFER_HEADER:
        rt_kprintf("refer to header\n");
        return 0;
    case IT6632X_AUDIO_SAMPLE_RATE_ERR:
        rt_kprintf("rate error\n");
        return 0;
    default:
        return 0;
    }
}

static rt_uint8_t map_sample_word_length(it6632x_audio_bits_t word_length)
{
    switch (word_length)
    {
    case IT6632X_AUDIO_BITS_16:
        return 16;
    case IT6632X_AUDIO_BITS_18:
        return 18;
    case IT6632X_AUDIO_BITS_20:
        return 20;
    case IT6632X_AUDIO_BITS_24:
        return 24;
    default:
        return 0;
    }
}

static void it6632x_get_hdmi_brief_decoderinfo(struct it6632x_ado_info *ado_info, struct audio_info *ado_brief_info)
{
    ado_brief_info->audio_channel_number = ado_info->audio_channel_number;
    ado_brief_info->audio_sample_frequency = map_sample_rate(ado_info->audio_sample_frequency);
    ado_brief_info->audio_output_inetrface = ado_info->audio_output_inetrface;
    ado_brief_info->audio_type = ado_info->audio_type;
    ado_brief_info->audio_sample_word_length = map_sample_word_length(ado_info->audio_sample_word_length);

    ado_brief_info->i2s_format.is_32_bit_wide = ado_info->i2s_format.is_32_bit_wide;
    ado_brief_info->i2s_format.left_right_justified = ado_info->i2s_format.left_right_justified;
    ado_brief_info->i2s_format.data_1T_delay_to_ws = ado_info->i2s_format.data_1T_delay_to_ws;
    ado_brief_info->i2s_format.ws_is_left_or_right_channel = ado_info->i2s_format.ws_is_left_or_right_channel;
    ado_brief_info->i2s_format.msb_or_lsb = ado_info->i2s_format.msb_or_lsb;
    ado_brief_info->is_multi_stream_audio = ado_info->is_multi_stream_audio;
    ado_brief_info->is_tdm = ado_info->is_tdm;
    ado_brief_info->audio_layout = ado_info->audio_layout;
    ado_brief_info->is_3d_audio = ado_info->is_3d_audio;
    ado_brief_info->audio_sample_word_length_1 = ado_info->audio_sample_word_length_1;
}

static rt_int32_t it6632x_get_hdmi_decoderinfo(struct it6632x_ado_info *ado_info)
{
    rt_uint8_t value[4] = {0};
    rt_uint32_t ado_row_data;
    rt_int32_t ret = 0;

    if (ado_info == RT_NULL)
    {
        rt_kprintf("%s ado_info is null!", __func__);
        return -1;
    }
    rt_mutex_take(g_it6632x_dev->mutex_lock, RT_WAITING_FOREVER);

    it6632x_set_update(I2C_UPD_ADO_SET);

    ret = SB_I2C_Read(I2C_ADO_INFO, ARRAY_SIZE(value), &value[0]);

    ado_row_data = (value[3] << 24) | (value[2] << 16) | (value[1] << 8) | (value[0]);

    ado_info->audio_channel_number = ado_row_data & 0x0f;
    ado_info->audio_sample_frequency = (ado_row_data >> 4) & 0x3f;
    ado_info->audio_output_inetrface = (ado_row_data >> 10) & 0x01;
    ado_info->audio_type = (ado_row_data >> 11) & 0x03;
    ado_info->audio_sample_word_length = (ado_row_data >> 13) & 0x03;
    ado_info->i2s_format.is_32_bit_wide = (ado_row_data >> 15) & 0x01;
    ado_info->i2s_format.left_right_justified = (ado_row_data >> 16) & 0x01;
    ado_info->i2s_format.data_1T_delay_to_ws = (ado_row_data >> 17) & 0x01;
    ado_info->i2s_format.ws_is_left_or_right_channel = (ado_row_data >> 18) & 0x01;
    ado_info->i2s_format.msb_or_lsb = (ado_row_data >> 19) & 0x01;
    ado_info->is_multi_stream_audio = (ado_row_data >> 20) & 0x01;
    ado_info->is_tdm = (ado_row_data >> 21) & 0x01;
    ado_info->audio_layout = (ado_row_data >> 24) & 0x01;
    ado_info->is_3d_audio = (ado_row_data >> 25) & 0x01;
    ado_info->new_auido_input = (ado_row_data >> 26) & 0x01;
    ado_info->audio_sample_word_length_1 = (ado_row_data >> 27) & 0x0f;

    ret = SB_I2C_Read(I2C_ADO_CA, 1, &ado_info->ca);

    it6632x_show_hdmi_decoderinfo(ado_info);

    it6632x_get_hdmi_brief_decoderinfo(ado_info, &g_it6632x_dev->audio_brief_info);

    g_it6632x_dev->rk_ado_info.audio_channel_number = g_it6632x_dev->audio_brief_info.audio_channel_number;
    g_it6632x_dev->rk_ado_info.audio_sample_frequency = g_it6632x_dev->audio_brief_info.audio_sample_frequency;
    g_it6632x_dev->rk_ado_info.audio_output_inetrface = g_it6632x_dev->audio_brief_info.audio_output_inetrface;
    g_it6632x_dev->rk_ado_info.audio_type = g_it6632x_dev->audio_brief_info.audio_type;
    g_it6632x_dev->rk_ado_info.audio_sample_word_length = g_it6632x_dev->audio_brief_info.audio_sample_word_length;
    g_it6632x_dev->rk_ado_info.audio_in = g_it6632x_dev->audio_brief_info.audio_in;


    rt_mutex_release(g_it6632x_dev->mutex_lock);

    return ret;
}

static rt_err_t it6632x_reg_ado_chg_hook(struct hdmi_repeat *rpt, void *data)
{
    g_it6632x_dev->audio_change_hook = (void (*)(void *arg))data;
    return RT_EOK;
}

static rt_err_t it6632x_unreg_ado_chg_hook(struct hdmi_repeat *rpt, void *data)
{
    g_it6632x_dev->audio_change_hook = RT_NULL;
    return RT_EOK;
}

static rt_err_t it6632x_reg_cec_vol_chg_hook(struct hdmi_repeat *rpt, void *data)
{
    g_it6632x_dev->cec_vol_change_hook = (void (*)(void *arg))data;
    return RT_EOK;
}

static rt_err_t it6632x_unreg_cec_vol_chg_hook(struct hdmi_repeat *rpt, void *data)
{
    g_it6632x_dev->cec_vol_change_hook = RT_NULL;
    return RT_EOK;
}

#ifdef IT6632X_USE_RXMUTE_PIN
static rt_err_t it6632x_reg_rx_mute_hook(struct hdmi_repeat *rpt, void *data)
{
    g_it6632x_dev->rx_mute_hook = (void (*)(void *arg))data;
    return RT_EOK;
}

static rt_err_t it6632x_unreg_rx_mute_hook(struct hdmi_repeat *rpt, void *data)
{
    g_it6632x_dev->rx_mute_hook = RT_NULL;
    return RT_EOK;
}
#endif

static rt_err_t it6632x_power_mode_ctrl(struct hdmi_repeat *rpt, void *data)
{
    rk_rpt_power_mode_t power_mode = *((rt_uint8_t *)data);

    switch (power_mode)
    {
    case RK_REPEAT_POWER_OFF_MODE:
        //TODO
        it6632x_power_mode(IT6632X_MODE_POWER_OFF);
        break;
    case RK_REPEAT_LOW_POWER_MODE:
        it6632x_power_mode(IT6632X_MODE_POWER_OFF);
        break;
    case RK_REPEAT_POWER_ON_MODE:
        it6632x_power_mode(IT6632X_MODE_POWER_ON);
        break;
    }

    return RT_EOK;
}

static rt_err_t it6632x_get_repeat_status(struct hdmi_repeat *rpt, void *data)
{
    rk_rpt_status_t *status = (rk_rpt_status_t *)data;

    *status = g_it6632x_dev->status.dev_status;

    return RT_EOK;
}

static rt_err_t it6632x_write_regs(void *write_buf, rt_uint8_t write_len)
{
    struct rt_i2c_msg msgs[1];
    rt_int32_t ret;

    msgs[0].addr  = g_it6632x_dev->i2c_client->client_addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = write_buf;
    msgs[0].len   = write_len;

    ret = rt_i2c_transfer(g_it6632x_dev->i2c_client->bus, msgs, 1);
    if (ret == 1)
    {
        return RT_EOK;
    }
    else
    {
        //rt_kprintf("[it6632x] failed to write the register! %d\n", ret);
        return -RT_ERROR;
    }
}

static rt_err_t  it6632x_read_regs(rt_uint8_t cmd, rt_uint8_t read_len, rt_uint8_t *read_buf)
{
    struct rt_i2c_msg msgs[2];
    rt_uint8_t cmd_buf = cmd;

    msgs[0].addr  = g_it6632x_dev->i2c_client->client_addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = &cmd_buf;
    msgs[0].len   = 1;

    msgs[1].addr  = g_it6632x_dev->i2c_client->client_addr;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf   = read_buf;
    msgs[1].len   = read_len;

    if (rt_i2c_transfer(g_it6632x_dev->i2c_client->bus, msgs, 2) == 2)
    {
        return RT_EOK;
    }

    //rt_kprintf("[it6632x] failed to read the register!\n");

    return -RT_ERROR;
}

static void irq_callback(void *args)
{
    it6632x_device_t *dev;

    dev = (it6632x_device_t *)args;

    rt_sem_release(dev->isr_sem);
}

#ifdef IT6632X_USE_RXMUTE_PIN
static void rxmute_irq_callback(void *args)
{
    it6632x_device_t *dev;

    dev = (it6632x_device_t *)args;

    rt_sem_release(dev->rx_mute_sem);
}
#endif

static rt_err_t it6632x_control(rt_device_t dev, int cmd, void *args)
{
    cec_cmd_t *cec_cmd;
    it6632x_device_t *it6632x_dev = dev->user_data;
    struct hdmi_repeat *rpt = &it6632x_dev->rpt;

    switch (cmd)
    {
    case RK_REPEAT_CTL_ADO_PATH_SET:
        it6632x_ado_path_set(rpt, args);
        break;
    case RK_REPEAT_CTL_ADO_PATH_GET:
        it6632x_ado_path_get(rpt, args);
        break;
    case RK_REPEAT_CTL_E_ARC_VOL_SET:
        it6632x_e_arc_vol_set(rpt, args);
        break;
    case RK_REPEAT_CTL_E_ARC_VOL_GET:
        it6632x_e_arc_vol_get(rpt, args);
        break;
    case RT_IT6632X_CTRL_ARC_SET_MAX_VOL:
        it6632x_arc_set_max_volume(*((rt_uint8_t *)args));
        break;
    case RT_IT6632X_CTRL_REG_DUMP:
        SB_I2C_Dump();
        it6632x_get_hdmi_decoderinfo(&g_it6632x_dev->ado_info);
        break;
    case RT_IT6632X_CTRL_AUDIO_SYSTEM_ENABLE:
        it6632x_audio_system_enable(*((rt_uint8_t *)args));
        break;
    case RT_IT6632X_CTRL_EARC_ENABLE:
        it6632x_earc_enable(*((rt_uint8_t *)args));
        break;
    case RT_IT6632X_CTRL_CEC_ENABLE:
        it6632x_sb_cec_enable(*((rt_uint8_t *)args));
        break;
    case RT_IT6632X_CTRL_CEC_CMD_SEND:
        cec_cmd = (cec_cmd_t *)args;
        IT66322_send_cec_command(cec_cmd->cmd, cec_cmd->len);
        break;
    case RT_IT6632X_CTRL_TX_AUDIO_MUTE:
        it6632x_tx_audio_mute(*((rt_uint8_t *)args));
        break;
    case RT_IT6632X_CTRL_TX_VIDEO_MUTE:
        it6632x_tx_video_mute(*((rt_uint8_t *)args));
        break;
    case RT_IT6632X_CTRL_HDCP_RPT_ENABLE:
        it6632x_hdcp_repeat(*((rt_uint8_t *)args));
        break;
    case RT_IT6632X_CTRL_GET_STATUS:
        it6632x_get_status((struct it6632x_status *)args);
        break;
    case RT_IT6632X_CTRL_HDMI_GET_DECODERINFO:
        it6632x_get_hdmi_decoderinfo(&g_it6632x_dev->ado_info);
        break;
    case RK_REPEAT_CTL_REG_ADO_CHG_HOOK:
        it6632x_reg_ado_chg_hook(rpt, args);
        break;
    case RK_REPEAT_CTL_UNREG_ADO_CHG_HOOK:
        it6632x_unreg_ado_chg_hook(rpt, args);
        break;
    case RK_REPEAT_CTL_REG_CEC_VOL_CHG_HOOK:
        it6632x_reg_cec_vol_chg_hook(rpt, args);
        break;
    case RK_REPEAT_CTL_UNREG_CEC_VOL_CHG_HOOK:
        it6632x_unreg_cec_vol_chg_hook(rpt, args);
        break;
#ifdef IT6632X_USE_RXMUTE_PIN
    case RK_REPEAT_CTL_REG_RX_MUTE_HOOK:
        it6632x_reg_rx_mute_hook(rpt, args);
        break;
    case RK_REPEAT_CTL_UNREG_RX_MUTE_HOOK:
        it6632x_unreg_rx_mute_hook(rpt, args);
        break;
#endif
    case RK_REPEAT_CTL_POWER_MODE_CTRL:
        it6632x_power_mode_ctrl(rpt, args);
        break;
    case RK_REPEAT_CTL_GET_REPEAT_STATUS:
        it6632x_get_repeat_status(rpt, args);
        break;
    default:
        rt_kprintf("Unsupport cmd: %d\n", cmd);
        break;
    }
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
static const struct rt_device_ops it6632x_device_ops =
{
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    it6632x_control
};
#endif /* RT_USING_DEVICE_OPS */

static rt_err_t rt_device_it6632x_register(it6632x_device_t *dev, const char *name, const void *user_data)
{
    rt_err_t result = RT_EOK;

#ifdef RT_USING_DEVICE_OPS
    dev->dev.ops = &it6632x_device_ops;
#else
    dev->dev.init = RT_NULL;
    dev->dev.open = RT_NULL;
    dev->dev.close = RT_NULL;
    dev->dev.read  = RT_NULL;
    dev->dev.write = RT_NULL;
    dev->dev.control = it6632x_control;
#endif /* RT_USING_DEVICE_OPS */

    dev->dev.type         = RT_Device_Class_Miscellaneous;
    dev->dev.user_data    = (void *)user_data;

    result = rt_device_register(&dev->dev, name, RT_DEVICE_FLAG_RDWR);

    return result;
}

#ifdef IT6632X_USE_RXMUTE_PIN
static void it6632x_rx_mute_thread(void *arg)
{
    it6632x_device_t *dev = (it6632x_device_t *)arg;
    rt_uint8_t mute_state;

    while (1)
    {
        rt_sem_take(dev->rx_mute_sem, RT_WAITING_FOREVER);

        if (rt_pin_read(IT6632X_RXMUTE_PIN) == IT6632X_RXMUTE_UNMUTE)
        {
            mute_state = RK_REPEAT_RXMUTE_UNMUTE;
        }
        else
        {
            mute_state = RK_REPEAT_RXMUTE_MUTE;
        }

        if (g_it6632x_dev->rx_mute_hook != RT_NULL)
            g_it6632x_dev->rx_mute_hook((void *)&mute_state);
    }
}
#endif

static rt_err_t it6632x_connectivity_check(void)
{

    rt_uint8_t  u8Temp, trycount;

    for (trycount = 0; trycount < 15; trycount ++)
    {
        if (!SB_I2C_Read(I2C_ITE_CHIP, 1, &u8Temp))
        {
            return RT_EOK;
        }
        rt_thread_mdelay(100);
    }
    rt_kprintf("it6632x not present!\n");
    return -RT_ERROR;
}

static rt_err_t it6632x_init(struct hdmi_repeat *rpt, void *data)
{
    return RT_EOK;
}

static rt_err_t it6632x_deinit(struct hdmi_repeat *rpt, void *data)
{
    return RT_EOK;
}

static const struct hdmi_repeat_ops it6632x_ops =
{
    .init = it6632x_init,
    .deinit = it6632x_deinit,
    .ado_path_get = it6632x_ado_path_get,
    .ado_path_set = it6632x_ado_path_set,
    .e_arc_vol_get = it6632x_e_arc_vol_get,
    .e_arc_vol_set = it6632x_e_arc_vol_set,
    .reg_ado_chg_hook = it6632x_reg_ado_chg_hook,
    .unreg_ado_chg_hook = it6632x_unreg_ado_chg_hook,
    .reg_cec_vol_chg_hook = it6632x_reg_cec_vol_chg_hook,
    .unreg_cec_vol_chg_hook = it6632x_unreg_cec_vol_chg_hook,
    .reg_rx_mute_hook = it6632x_reg_rx_mute_hook,
    .unreg_rx_mute_hook = it6632x_unreg_rx_mute_hook,
    .power_mode_ctrl = it6632x_power_mode_ctrl,
    .get_repeat_status = it6632x_get_repeat_status,
};

static rt_err_t s_it6632x_init(it6632x_device_t *dev)
{
    rt_err_t ret;

    dev->mutex_lock = rt_mutex_create("it6632x_op", RT_IPC_FLAG_PRIO);

    dev->isr_sem = rt_sem_create("6632x_isr_sem", 0, RT_IPC_FLAG_PRIO);
    RT_ASSERT(dev->isr_sem);

#ifdef IT6632X_USE_RXMUTE_PIN
    dev->rx_mute_sem = rt_sem_create("it6632x_rxmute_sem", 0, RT_IPC_FLAG_PRIO);
    RT_ASSERT(dev->rx_mute_sem);
#endif


#ifdef IT6632X_USE_RXMUTE_PIN
    dev->rxmute_tid = rt_thread_create("rx_mute", it6632x_rx_mute_thread, dev,
                                       IT6632x_THREAD_STACK_SIZE, IT6632x_THREAD_PRIORITY, 10);
    RT_ASSERT(dev->rxmute_tid != RT_NULL);
    rt_thread_startup(dev->rxmute_tid);
#endif

    ret = rt_device_open((rt_device_t)dev->i2c_client->bus, RT_DEVICE_FLAG_RDWR);
    RT_ASSERT(ret == RT_EOK);

    /* before here, IOMUX must be initialized in board_xxxx.c*/
    //int pin
    rt_pin_mode(IT6632X_INT_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(IT6632X_INT_PIN, PIN_IRQ_MODE_FALLING, irq_callback, (void *)dev);
    rt_pin_irq_enable(IT6632X_INT_PIN, PIN_IRQ_ENABLE);

#ifdef IT6632X_USE_RXMUTE_PIN
    //rx mute pin
    rt_pin_mode(IT6632X_RXMUTE_PIN, PIN_MODE_INPUT);
    rt_pin_attach_irq(IT6632X_RXMUTE_PIN, PIN_IRQ_MODE_RISING_FALLING, rxmute_irq_callback, (void *)dev);
    rt_pin_irq_enable(IT6632X_RXMUTE_PIN, PIN_IRQ_ENABLE);
#endif

    dev->rpt.ops = &it6632x_ops;
    dev->rpt.private = (void *)dev;

    hdmi_repeat_register(&dev->rpt);

    ret = rt_device_it6632x_register(dev, IT6632X_DEVICE_NAME, RT_NULL);
    RT_ASSERT(ret == RT_EOK);

    return RT_EOK;
}

static void it6632x_thread(void *arg)
{
#ifndef IT6632X_USE_RESET_PIN
    rt_uint8_t  u8I2cChg = 0;
#endif
    it6632x_device_t *dev = (it6632x_device_t *)arg;

    if (it6632x_connectivity_check())
    {
        rt_kprintf("it6632x connectivity check failed\n");
        return;
    }

    g_it6632x_dev->status.dev_status = RK_REPEAT_INIT;
    s_it6632x_init(dev);

#ifndef IT6632X_USE_RESET_PIN
    u8I2cChg |= SB_PowerOff();
    SB_SysI2cChange(u8I2cChg);
    rt_thread_mdelay(100);
    u8I2cChg |= SB_PowerOn();
    SB_SysI2cChange(u8I2cChg);
#endif
    while (1)
    {
        if (!g_u8IteRdy)
        {
            rt_thread_mdelay(300);
        }
        else
        {
            rt_sem_take(dev->isr_sem, RT_WAITING_FOREVER);
        }
        SB_I2cIrq();
    }
}

static int rt_hw_it6632x_init(void)
{
    it6632x_device_t *dev;

    /* init it6632x device */
    g_it6632x_dev = dev = (it6632x_device_t *)rt_malloc(sizeof(it6632x_device_t));
    RT_ASSERT(dev != RT_NULL);
    rt_memset((void *)dev, 0, sizeof(it6632x_device_t));
    g_it6632x_dev->status.dev_status = RK_REPEAT_NOT_PRESENT;
#ifdef IT6632X_USE_RESET_PIN
    //reset
    rt_pin_mode(IT6632X_RST_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(IT6632X_RST_PIN, IT6632X_RST_ON);
    rt_thread_mdelay(10);
    rt_pin_write(IT6632X_RST_PIN, IT6632X_RST_OFF);
#endif

    /* i2c interface bus */
    struct rt_i2c_client *i2c_client;
    dev->i2c_client = i2c_client = (struct rt_i2c_client *)rt_malloc(sizeof(struct rt_i2c_client));
    RT_ASSERT(i2c_client != RT_NULL);

    i2c_client->client_addr = IT6632X_I2C_ADDR;
    i2c_client->bus = (struct rt_i2c_bus_device *)rt_device_find(IT6632X_I2C_DEV);
    RT_ASSERT(i2c_client->bus != RT_NULL);

    dev->int_tid = rt_thread_create("it6632x", it6632x_thread, dev,
                                    IT6632x_THREAD_STACK_SIZE, IT6632x_THREAD_PRIORITY, 10);
    RT_ASSERT(dev->int_tid != RT_NULL);
    rt_thread_startup(dev->int_tid);

    return RT_EOK;
}

INIT_DEVICE_EXPORT(rt_hw_it6632x_init);

#ifdef RT_USING_FINSH
#include <finsh.h>
static void it6632x_show_usage()
{
    rt_kprintf("Usage: \n");
    rt_kprintf("it6632x dump                      -dump status \n");
    rt_kprintf("it6632x powermode <mode>          -set power mode 0.pwoer off,1.low power,2.power on\n");
    rt_kprintf("it6632x audiopath <path>          -select path: 1:hdmi0 2:hdmi1 3:hdmi2 4:arc/earc \n");
    rt_kprintf("                                    5:ext_i2s1 6:ext_i2s2 7:ext_i2s3 8:ext_spdif1\n");
    rt_kprintf("                                    9:ext_spdif2 10:ext_spdif3\n");
    rt_kprintf("it6632x arcvolset <vol> <mute>    -set vol and mute/unmute status:it6632x arcvolset 50 umute\n");
    rt_kprintf("it6632x arcvolget                 -get vol and mute/unmute status\n");
    rt_kprintf("it6632x audiosystem <enable>      -enable or disable audio system \n");
    rt_kprintf("it6632x earc <enable>             -enable or disable earc \n");
    rt_kprintf("it6632x cec <enable>              -enable or disable cec \n");
    rt_kprintf("it6632x cecsend [cmd]...          -send cec cmd,\'it6632x cecsend 54 44 42\' means logic addr 0x54\n");
    rt_kprintf("                                   cec cmd user ctl press_volumedown\n");
    rt_kprintf("it6632x txaudio <mute>            -mute or unmute tx audio \n");
    rt_kprintf("it6632x txvideo <mute>            -mute or unmute tx video \n");
    rt_kprintf("it6632x hdcprepeat <enable>       -enable or disable hdcp repeat\n");
    rt_kprintf("it6632x decoderinfo               -dump decoder info \n");
    rt_kprintf("it6632x hookregister <hook type>  -hook type: 1.audio change,2.cec vol change 3.rx mute\n");
    rt_kprintf("it6632x hookunregister <hook type> -hook type: 1.audio change,2.cec vol change 3.rx mute\n");
}

static void it6632x_audio_change_test_hook(void *arg)
{
    struct rk_rpt_audio_info *info = (struct rk_rpt_audio_info *)arg;

    rt_kprintf("it6632x_audio_change_test_hook!\n");
    if (info->audio_in == 0)
        rt_kprintf("i2s in\n");
    else if (info->audio_in == 1)
        rt_kprintf("spidf in\n");
    else if (info->audio_in == 2)
        rt_kprintf("spidf from arc\n");
}

static void it6632x_cec_vol_change_test_hook(void *arg)
{
    rt_kprintf("it6632x_cec_vol_change_test_hook!\n");

    if (*(rk_rpt_cec_cmd_t *)arg == RK_REPEAT_CEC_CTL_VOL_MUTE)
        rt_kprintf("cec vol mute\n");
    else if (*(rk_rpt_cec_cmd_t *)arg == RK_REPEAT_CEC_CTL_VOL_UP)
        rt_kprintf("cec vol up\n");
    else if (*(rk_rpt_cec_cmd_t *)arg == RK_REPEAT_CEC_CTL_VOL_DOWN)
        rt_kprintf("cec vol down\n");
}

#ifdef IT6632X_USE_RXMUTE_PIN
static void it6632x_rx_mute_hook(void *arg)
{
    rt_kprintf("it6632x_rx_mute_hook!\n");

    if (*(rt_uint8_t *)arg == RK_REPEAT_RXMUTE_MUTE)
        rt_kprintf("it6632x_rx_mute!\n");
    else if (*(rt_uint8_t *)arg == RK_REPEAT_RXMUTE_UNMUTE)
        rt_kprintf("it6632x_rx_unmute!\n");
}
#endif

void it6632x(int argc, char **argv)
{
    rt_err_t res = RT_EOK;
    static rt_device_t it6632x_dev = RT_NULL;
    rt_int32_t para = 0;
    rt_uint8_t i;
    rt_uint8_t ceccmd[16];
    cec_cmd_t cec_test_cmd;

    struct rk_rpt_volume_info volume_info;
    if (argc < 2)
        goto out2;

    it6632x_dev = rt_device_find(IT6632X_DEVICE_NAME);
    if (!it6632x_dev)
    {
        rt_kprintf("find %s failed!\n", IT6632X_DEVICE_NAME);
        return;
    }

    res = rt_device_init(it6632x_dev);
    if (res != RT_EOK)
    {
        rt_kprintf("initialize %s failed!\n", IT6632X_DEVICE_NAME);
        return;
    }

    rt_thread_mdelay(20);
    res = rt_device_open(it6632x_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    if (res != RT_EOK)
    {
        rt_kprintf("Failed to open device: %s\n", IT6632X_DEVICE_NAME);
        return;
    }

    if (argc == 2)
    {
        if (!strcmp(argv[1], "dump"))
        {
            rt_device_control(it6632x_dev, RT_IT6632X_CTRL_REG_DUMP, RT_NULL);
        }
        else if (!strcmp(argv[1], "decoderinfo"))
        {
            rt_device_control(it6632x_dev, RT_IT6632X_CTRL_HDMI_GET_DECODERINFO, RT_NULL);
        }
        else if (!strcmp(argv[1], "arcvolget"))
        {
            rt_device_control(it6632x_dev, RK_REPEAT_CTL_E_ARC_VOL_GET, (void *)&volume_info);
            rt_kprintf("it6632x e/arc volue is %d,%s\n", volume_info.volume, volume_info.is_mute ? "mute" : "no mute");
        }
        else
        {
            rt_kprintf("wrong usage, check your param \n");
            goto out1;
        }
    }
    else if (argc == 3)
    {
        if (!strcmp(argv[1], "powermode"))
        {
            para = strtol(argv[2], RT_NULL, 10);
            rt_device_control(it6632x_dev, RK_REPEAT_CTL_POWER_MODE_CTRL, (void *)&para);
        }
        else if (!strcmp(argv[1], "audiopath"))
        {
            para = strtol(argv[2], RT_NULL, 10);
            if (para >= RK_REPEAT_AUDIO_PATH_MAX)
            {
                rt_kprintf("wrong usage, check your param \n");
                goto out1;
            }
            rt_device_control(it6632x_dev, RK_REPEAT_CTL_ADO_PATH_SET, (void *)&para);
        }
        else if (!strcmp(argv[1], "audiosystem"))
        {
            if (!strcmp(argv[2], "enable"))
            {
                para = 1;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_AUDIO_SYSTEM_ENABLE, (void *)&para);
            }
            else if (!strcmp(argv[2], "disable"))
            {
                para = 0;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_AUDIO_SYSTEM_ENABLE, (void *)&para);
            }
            else
            {
                rt_kprintf("wrong usage, check your param \n");
                goto out1;
            }
        }
        else if (!strcmp(argv[1], "earc"))
        {
            if (!strcmp(argv[2], "enable"))
            {
                para = 1;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_EARC_ENABLE, (void *)&para);
            }
            else if (!strcmp(argv[2], "disable"))
            {
                para = 0;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_EARC_ENABLE, (void *)&para);
            }
            else
            {
                rt_kprintf("wrong usage, check your param \n");
                goto out1;
            }
        }
        else if (!strcmp(argv[1], "cec"))
        {
            if (!strcmp(argv[2], "enable"))
            {
                para = 1;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_CEC_ENABLE, (void *)&para);
            }
            else if (!strcmp(argv[2], "disable"))
            {
                para = 0;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_CEC_ENABLE, (void *)&para);
            }
            else
            {
                rt_kprintf("wrong usage, check your param \n");
                goto out1;
            }
        }
        else if (!strcmp(argv[1], "txaudio"))
        {
            if (!strcmp(argv[2], "mute"))
            {
                para = 1;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_TX_AUDIO_MUTE, (void *)&para);
            }
            else if (!strcmp(argv[2], "unmute"))
            {
                para = 0;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_TX_AUDIO_MUTE, (void *)&para);
            }
            else
            {
                rt_kprintf("wrong usage, check your param \n");
                goto out1;
            }
        }
        else if (!strcmp(argv[1], "txvideo"))
        {
            if (!strcmp(argv[2], "mute"))
            {
                para = 1;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_TX_VIDEO_MUTE, (void *)&para);
            }
            else if (!strcmp(argv[2], "unmute"))
            {
                para = 0;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_TX_VIDEO_MUTE, (void *)&para);
            }
            else
            {
                rt_kprintf("wrong usage, check your param \n");
                goto out1;
            }
        }
        else if (!strcmp(argv[1], "hdcprepeat"))
        {
            if (!strcmp(argv[2], "enable"))
            {
                para = 1;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_HDCP_RPT_ENABLE, (void *)&para);
            }
            else if (!strcmp(argv[2], "disable"))
            {
                para = 0;
                rt_device_control(it6632x_dev, RT_IT6632X_CTRL_HDCP_RPT_ENABLE, (void *)&para);
            }
            else
            {
                rt_kprintf("wrong usage, check your param \n");
                goto out1;
            }
        }
        else if (!strcmp(argv[1], "hookregister"))
        {
            para = strtol(argv[2], RT_NULL, 10);
            if (para == 1)
            {
                rt_device_control(it6632x_dev, RK_REPEAT_CTL_REG_ADO_CHG_HOOK, (void *)&it6632x_audio_change_test_hook);
            }
            else if (para == 2)
            {
                rt_device_control(it6632x_dev, RK_REPEAT_CTL_REG_CEC_VOL_CHG_HOOK, (void *)&it6632x_cec_vol_change_test_hook);
            }
            else if (para == 3)
            {
#ifdef IT6632X_USE_RXMUTE_PIN
                rt_device_control(it6632x_dev, RK_REPEAT_CTL_REG_RX_MUTE_HOOK, (void *)&it6632x_rx_mute_hook);
#endif
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
                rt_device_control(it6632x_dev, RK_REPEAT_CTL_UNREG_ADO_CHG_HOOK, RT_NULL);
            }
            else if (para == 2)
            {
                rt_device_control(it6632x_dev, RK_REPEAT_CTL_UNREG_CEC_VOL_CHG_HOOK, RT_NULL);
            }
            else if (para == 3)
            {
                rt_device_control(it6632x_dev, RK_REPEAT_CTL_UNREG_RX_MUTE_HOOK, RT_NULL);
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
            rt_device_control(it6632x_dev, RK_REPEAT_CTL_E_ARC_VOL_SET, (void *)&volume_info);
        }
    }
    else if (argc > 4)
    {
        if (!strcmp(argv[1], "cecsend"))
        {
            rt_memset(ceccmd, 0, sizeof(ceccmd));
            for (i = 0; i < ((argc - 2) > 16 ? 16 : (argc - 2)); i++)
                ceccmd[i] = strtol(argv[i + 2], RT_NULL, 16);

            cec_test_cmd.cmd = ceccmd;
            cec_test_cmd.len = ((argc - 2) > 16 ? 16 : (argc - 2));
            rt_device_control(it6632x_dev, RT_IT6632X_CTRL_CEC_CMD_SEND, (void *)&cec_test_cmd);

        }
    }

out1:
    rt_device_close(it6632x_dev);
    return;
out2:
    it6632x_show_usage();
    return;
}

MSH_CMD_EXPORT(it6632x, it6632x test);
#endif
#endif
