/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include "main.h"
#if MULTIMEDIA_EN

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/poll.h>
#include <unistd.h>

#include "rk_defines.h"
#include "rk_debug.h"
#include "rk_mpi_adec.h"
#include "rk_mpi_ao.h"
#include "rk_mpi_mb.h"
#include "rk_mpi_sys.h"

#define ADEC_EN             1

#define AUDIO_RATE          AUDIO_SAMPLE_RATE_16000
#define AUDIO_BITS          AUDIO_BIT_WIDTH_16
#define AUDIO_CH            2
#define AUDIO_CH_IN         1

#define AUDIO_PERIOD_SZ     256
#define AUDIO_PERIOD_CNT    4

static AUDIO_DEV aoDevId = 0;
static AIO_ATTR_S aoAttr;
static AO_CHN aoChn = 0;
static ADEC_CHN_ATTR_S adAttr;
static ADEC_CHN adChn = 0;
static MPP_CHN_S aoBindAttr, adecBindAttr;

static AUDIO_SOUND_MODE_E sound_mode(int ch)
{
    switch (ch)
    {
    case 1:
        return AUDIO_SOUND_MODE_MONO;
    case 2:
        return AUDIO_SOUND_MODE_STEREO;
    default:
        return AUDIO_SOUND_MODE_BUTT;
    }
}

#if 0
int ao_init(void)
{
    AO_CHN_PARAM_S pstParams;
    RK_S32 result;

    memset(&aoAttr, 0, sizeof(AIO_ATTR_S));
    snprintf(aoAttr.u8CardName,
             sizeof(aoAttr.u8CardName), "default");
    aoAttr.soundCard.channels = AUDIO_CH;
    aoAttr.soundCard.sampleRate = AUDIO_RATE;
    aoAttr.soundCard.bitWidth = AUDIO_BITS;

    aoAttr.enBitwidth = AUDIO_BITS;
    aoAttr.enSamplerate = AUDIO_RATE;
    aoAttr.enSoundmode = sound_mode(AUDIO_CH_IN);
    aoAttr.u32FrmNum = AUDIO_PERIOD_CNT;
    aoAttr.u32PtNumPerFrm = AUDIO_PERIOD_SZ;
    aoAttr.u32EXFlag = 0;
    aoAttr.u32ChnCnt = 2;

    RK_LOGI("rate %d bit %d ch %d", aoAttr.enSamplerate, aoAttr.enBitwidth,
            aoAttr.enSoundmode);
    result = RK_MPI_AO_SetPubAttr(aoDevId, &aoAttr);
    if (result != 0)
    {
        RK_LOGE("ai set attr fail, reason = %x", result);
        return -1;
    }

    result = RK_MPI_AO_Enable(aoDevId);
    if (result != 0)
    {
        RK_LOGE("ai enable fail, reason = %x", result);
        return -1;
    }

    memset(&pstParams, 0, sizeof(AO_CHN_PARAM_S));
    pstParams.enLoopbackMode = AUDIO_LOOPBACK_NONE;
    result = RK_MPI_AO_SetChnParams(aoDevId, aoChn, &pstParams);
    if (result != RK_SUCCESS)
    {
        RK_LOGE("ai set channel params, aoChn = %d", aoChn);
        goto ai_chn_err;
    }

    RK_MPI_AO_SetTrackMode(aoDevId, AUDIO_TRACK_OUT_STEREO);

    result = RK_MPI_AO_EnableChn(aoDevId, aoChn);
    if (result != 0)
    {
        RK_LOGE("ai enable channel fail, aoChn = %d, reason = %x", aoChn, result);
        goto ai_chn_err;
    }

    result = RK_MPI_AO_EnableReSmp(aoDevId, aoChn,
                                   AUDIO_RATE);

#if ADEC_EN
    adAttr.stCodecAttr.enType = RK_AUDIO_ID_PCM_ALAW;
    adAttr.stCodecAttr.u32Channels = AUDIO_CH_IN;
    adAttr.stCodecAttr.u32SampleRate = AUDIO_RATE;
    adAttr.stCodecAttr.u32BitPerCodedSample = 4;
    adAttr.enType = RK_AUDIO_ID_PCM_ALAW;
    adAttr.enMode = ADEC_MODE_STREAM;
    adAttr.u32BufCount = 4;
    adAttr.u32BufSize = 50 * 1024;
    adAttr.u32Depth = 0;

    result = RK_MPI_ADEC_CreateChn(adChn, &adAttr);
    if (result)
    {
        RK_LOGE("create adec chn %d err:0x%x\n", adChn, result);
        goto adec_err;
    }

    adecBindAttr.enModId = RK_ID_ADEC;
    adecBindAttr.s32DevId = aoDevId;
    adecBindAttr.s32ChnId = adChn;
    aoBindAttr.enModId = RK_ID_AO;
    aoBindAttr.s32DevId = aoDevId;
    aoBindAttr.s32ChnId = aoChn;

    result = RK_MPI_SYS_Bind(&adecBindAttr, &aoBindAttr);
    if (result)
    {
        RK_LOGE("bind ai adec failed:0x%x\n", result);
        goto bind_err;
    }
#endif

    return RK_SUCCESS;

#if ADEC_EN
bind_err:
    RK_MPI_ADEC_DestroyChn(adChn);
adec_err:
    RK_MPI_AO_DisableChn(aoDevId, aoChn);
#endif
ai_chn_err:
    RK_MPI_AO_Disable(aoDevId);

    return RK_FAILURE;
}
#endif

static RK_S32 free_cb(void *opaque)
{
    if (opaque)
        free(opaque);
    return 0;
}

int ao_push(int (*hook)(void *, char *, int), void *arg)
{
#if !ADEC_EN
    static RK_U64 timeStamp = 0;
    AUDIO_FRAME_S frame;
    MB_EXT_CONFIG_S extConfig;
    RK_S32 result;
    RK_U8 *buf;
    int size = 1024;
    int ret;

    buf = malloc(size * sizeof(RK_U8));
    if ((ret = hook(arg, buf, size)) != size)
    {
        printf("input size error %d\n", ret);
        free(buf);
        return RK_FAILURE;
    }

    frame.u32Len = size;
    frame.u64TimeStamp = timeStamp++;
    frame.enBitWidth = AUDIO_BITS;
    frame.enSoundMode = sound_mode(AUDIO_CH_IN);
    frame.bBypassMbBlk = RK_FALSE;

    memset(&extConfig, 0, sizeof(extConfig));
    extConfig.pOpaque = buf;
    extConfig.pu8VirAddr = buf;
    extConfig.u64Size = size;
    RK_MPI_SYS_CreateMB(&(frame.pMbBlk), &extConfig);

    result = RK_MPI_AO_SendFrame(aoDevId, aoChn, &frame, -1);
    if (result < 0)
    {
        RK_LOGE("send frame fail, result = %X, TimeStamp = %lld, s32MilliSec = -1",
                result, frame.u64TimeStamp);
    }
    RK_MPI_MB_ReleaseMB(frame.pMbBlk);
    free(buf);
#else
    static RK_U64 timeStamp = 0;
    static RK_U64 count = 0;
    AUDIO_STREAM_S stream;
    MB_EXT_CONFIG_S extConfig;
    RK_S32 result;
    RK_U8 *buf;
    int size = 1024;

    buf = malloc(size * sizeof(RK_U8));
    if ((size = hook(arg, buf, size)) <= 0)
    {
        free(buf);
        return RK_FAILURE;
    }

    stream.u32Len = size;
    stream.u64TimeStamp = timeStamp++;
    stream.u32Seq = ++count;
    stream.bBypassMbBlk = RK_FALSE;

    memset(&extConfig, 0, sizeof(extConfig));
    extConfig.pFreeCB = free_cb;
    extConfig.pOpaque = buf;
    extConfig.pu8VirAddr = buf;
    extConfig.u64Size = size;
    RK_MPI_SYS_CreateMB(&(stream.pMbBlk), &extConfig);

    result = RK_MPI_ADEC_SendStream(adChn, &stream, RK_TRUE);
    if (result < 0)
    {
        RK_LOGE("send frame fail, result = %X, TimeStamp = %lld, s32MilliSec = -1",
                result, stream.u64TimeStamp);
    }
    RK_MPI_MB_ReleaseMB(stream.pMbBlk);
#endif

    return result;
}

int ao_deinit(void)
{
    int ret = 0;
#if ADEC_EN
    ret = RK_MPI_SYS_UnBind(&adecBindAttr, &aoBindAttr);
    if (ret)
    {
        RK_LOGE("ai disable chn failed:0x%x\n", ret);
    }

#endif

    ret = RK_MPI_AO_DisableChn(aoDevId, aoChn);
    if (ret)
    {
        RK_LOGE("ai disable chn failed:0x%x\n", ret);
    }

    ret = RK_MPI_AO_Disable(aoDevId);
    if (ret)
    {
        RK_LOGE("ai disable chn failed:0x%x\n", ret);
    }

#if ADEC_EN
    ret = RK_MPI_ADEC_DestroyChn(adChn);
    if (ret)
    {
        RK_LOGE("ai disable chn failed:0x%x\n", ret);
    }
#endif

    return 0;
}

#ifdef AUDIO_OUT_MAIN
static int duration = 0;
static int last_ts = -1;
static int ao_read(void *arg, char *buf, int len)
{
    int ret = fread(buf, 1, len, arg);

    duration += len / sizeof(short) / AUDIO_CH_IN / (AUDIO_RATE / 1000);
    if (duration / 1000 != last_ts)
    {
        last_ts = duration / 1000;
        RK_LOGI("duration: %.3fs %d", duration / 1000.0, len);
    }

    return ret;
}

void main(int argc, char *argv[])
{
    void *buf;
    int len;
    FILE *fd;
    char *file;

    file = (argc > 1) ? argv[1] : "/tmp/out.pcm";
    fd = fopen(file, "rb");
    if (!fd)
    {
        RK_LOGE("cannot open %s", file);
        return;
    }

    RK_MPI_SYS_Init();

    ao_init();
    while (1)
        ao_push(ao_read, fd);
    ao_deinit();

    RK_MPI_SYS_Exit();

    fclose(fd);
}
#endif

#endif
