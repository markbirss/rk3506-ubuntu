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
#include "rk_mpi_aenc.h"
#include "rk_mpi_ai.h"
#include "rk_mpi_mb.h"
#include "rk_mpi_sys.h"

#define AENC_EN             1
#define AVQE_EN             1

#define AUDIO_RATE          AUDIO_SAMPLE_RATE_16000
#define AUDIO_BITS          AUDIO_BIT_WIDTH_16
#define AUDIO_CH_CAP        1
#define AUDIO_CH_REF        1
#define AUDIO_CH            (AUDIO_CH_CAP + AUDIO_CH_REF)
#define AUDIO_CH_OUT        1

#define AUDIO_PERIOD_SZ     256
#define AUDIO_PERIOD_CNT    4

static AUDIO_DEV aiDevId = 0;
static AIO_ATTR_S aiAttr;
static AI_CHN aiChn = 0;
static AENC_CHN_ATTR_S aeAttr;
static AENC_CHN aeChn = 0;
static MPP_CHN_S aiBindAttr, aencBindAttr;
static RK_S32 aiVqeChn = 0;
static AI_VQE_CONFIG_S stAiVqeConfig, stAiVqeConfig2;

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
int ai_init(void)
{
    AI_CHN_PARAM_S pstParams;
    RK_S32 result;

    memset(&aiAttr, 0, sizeof(AIO_ATTR_S));
    snprintf(aiAttr.u8CardName,
             sizeof(aiAttr.u8CardName), "default");
    aiAttr.soundCard.channels = AUDIO_CH;
    aiAttr.soundCard.sampleRate = AUDIO_RATE;
    aiAttr.soundCard.bitWidth = AUDIO_BITS;

    aiAttr.enBitwidth = AUDIO_BITS;
    aiAttr.enSamplerate = AUDIO_RATE;
    aiAttr.enSoundmode = sound_mode(AUDIO_CH_OUT);
    aiAttr.u32FrmNum = AUDIO_PERIOD_CNT;
    aiAttr.u32PtNumPerFrm = 1024;//AUDIO_PERIOD_SZ;
    aiAttr.u32EXFlag = 0;
    aiAttr.u32ChnCnt = 2;

    result = RK_MPI_AI_SetPubAttr(aiDevId, &aiAttr);
    if (result != 0)
    {
        RK_LOGE("ai set attr fail, reason = %x", result);
        return -1;
    }

    result = RK_MPI_AI_Enable(aiDevId);
    if (result != 0)
    {
        RK_LOGE("ai enable fail, reason = %x", result);
        return -1;
    }

    memset(&pstParams, 0, sizeof(AI_CHN_PARAM_S));
    pstParams.enLoopbackMode = AUDIO_LOOPBACK_NONE;
    pstParams.s32UsrFrmDepth = -1;
    result = RK_MPI_AI_SetChnParam(aiDevId, aiChn, &pstParams);
    if (result != RK_SUCCESS)
    {
        RK_LOGE("ai set channel params, aiChn = %d", aiChn);
        goto ai_chn_err;
    }

#if AVQE_EN
    memset(&stAiVqeConfig, 0, sizeof(AI_VQE_CONFIG_S));
    stAiVqeConfig.enCfgMode = AIO_VQE_CONFIG_LOAD_FILE;
    memcpy(stAiVqeConfig.aCfgFile, "/usr/vqefiles/config_aivqe.json",
           strlen("/usr/vqefiles/config_aivqe.json"));

    stAiVqeConfig.s32WorkSampleRate = AUDIO_RATE;
    stAiVqeConfig.s32FrameSample = AUDIO_PERIOD_SZ;
    result = RK_MPI_AI_SetVqeAttr(aiDevId, aiVqeChn, 0, 0, &stAiVqeConfig);
    if (result != RK_SUCCESS)
    {
        RK_LOGE("%s: SetVqeAttr(%d,%d) failed with %#x",
                __func__, aiDevId, aiVqeChn, result);
        goto vqe_err;
    }

    result = RK_MPI_AI_GetVqeAttr(aiDevId, aiVqeChn, &stAiVqeConfig2);
    if (result != RK_SUCCESS)
    {
        RK_LOGE("%s: SetVqeAttr(%d,%d) failed with %#x",
                __func__, aiDevId, aiChn, result);
        goto vqe_err;
    }

    result = memcmp(&stAiVqeConfig, &stAiVqeConfig2, sizeof(AI_VQE_CONFIG_S));
    if (result != RK_SUCCESS)
    {
        RK_LOGE("%s: set/get vqe config is different: %x", __func__, result);
        goto vqe_err;
    }

    result = RK_MPI_AI_EnableVqe(aiDevId, aiVqeChn);
    if (result != RK_SUCCESS)
    {
        RK_LOGE("%s: EnableVqe(%d,%d) failed with %#x",
                __func__, aiDevId, aiVqeChn, result);
        goto vqe_err;
    }
#endif

    result = RK_MPI_AI_EnableChn(aiDevId, aiChn);
    if (result != 0)
    {
        RK_LOGE("ai enable channel fail, aiChn = %d, reason = %x", aiChn, result);
        goto ai_chn_err;
    }

#if AENC_EN
    aeAttr.enType = RK_AUDIO_ID_PCM_ALAW;
    //aeAttr.stCodecAttr.enType = RK_AUDIO_ID_PCM_ALAW;
    aeAttr.stCodecAttr.u32Channels = AUDIO_CH_OUT;
    aeAttr.stCodecAttr.u32SampleRate = AUDIO_RATE;
    aeAttr.stCodecAttr.enBitwidth = AUDIO_BITS;
    //aeAttr.stCodecAttr.pstResv = RK_NULL;
    aeAttr.u32BufCount = 4;
    aeAttr.u32Depth = aeAttr.u32BufCount;

    result = RK_MPI_AENC_CreateChn(aeChn, &aeAttr);
    if (result)
    {
        RK_LOGE("create aenc chn %d err:0x%x\n", aeChn, result);
        goto aenc_err;
    }

    aiBindAttr.enModId = RK_ID_AI;
    aiBindAttr.s32DevId = aiDevId;
    aiBindAttr.s32ChnId = aiChn;
    aencBindAttr.enModId = RK_ID_AENC;
    aencBindAttr.s32DevId = aiDevId;
    aencBindAttr.s32ChnId = aeChn;

    result = RK_MPI_SYS_Bind(&aiBindAttr, &aencBindAttr);
    if (result)
    {
        RK_LOGE("bind ai aenc failed:0x%x\n", result);
        goto bind_err;
    }
#endif

    return RK_SUCCESS;

#if AENC_EN
bind_err:
    RK_MPI_AENC_DestroyChn(aeChn);
aenc_err:
    RK_MPI_AI_DisableChn(aiDevId, aiChn);
#endif
ai_chn_err:
#if AVQE_EN
    RK_MPI_AI_DisableVqe(aiDevId, aiVqeChn);
vqe_err:
#endif
    RK_MPI_AI_Disable(aiDevId);

    return RK_FAILURE;
}
#endif

int ai_fetch(int (*hook)(void *, char *, int), void *arg)
{
#if !AENC_EN
    AUDIO_FRAME_S getFrame;
    RK_S32 result;

    result = RK_MPI_AI_GetFrame(aiDevId, aiChn, &getFrame, RK_NULL, -1);
    if (result == 0)
    {
        void *data = RK_MPI_MB_Handle2VirAddr(getFrame.pMbBlk);
        RK_LOGV("data = %p, len = %d", data, getFrame.u32Len);
        if (getFrame.u32Len <= 0)
        {
            RK_LOGD("get ai frame end");
            return 0;
        }
        result = (hook(arg, data, getFrame.u32Len) <= 0) ? RK_FAILURE : RK_SUCCESS;
        RK_MPI_AI_ReleaseFrame(aiDevId, aiChn, &getFrame, RK_NULL);
    }
#else
    AUDIO_STREAM_S pstStream;
    RK_S32 result;
    RK_S32 eos = 0;

    result = RK_MPI_AENC_GetStream(aeChn, &pstStream, -1);
    if (result == RK_SUCCESS)
    {
        MB_BLK bBlk = pstStream.pMbBlk;
        RK_VOID *pstFrame = RK_MPI_MB_Handle2VirAddr(bBlk);
        RK_S32 frameSize = pstStream.u32Len;
        eos = (frameSize <= 0) ? 1 : 0;
        if (pstFrame)
        {
            RK_LOGV("get frame data = %p, size = %d", pstFrame, frameSize);
            result = (hook(arg, pstFrame, frameSize) <= 0) ? RK_FAILURE : RK_SUCCESS;
            RK_MPI_AENC_ReleaseStream(aeChn, &pstStream);
        }
    }
#endif

    return result;
}

int ai_deinit(void)
{
    int ret = 0;
#if AENC_EN
    ret = RK_MPI_SYS_UnBind(&aiBindAttr, &aencBindAttr);
    if (ret)
    {
        RK_LOGE("ai unbind aenc failed:0x%x\n", ret);
    }
#endif

    ret = RK_MPI_AI_DisableChn(aiDevId, aiChn);
    if (ret)
    {
        RK_LOGE("ai disable chn failed:0x%x\n", ret);
    }

#if AVQE_EN
    ret = RK_MPI_AI_DisableVqe(aiDevId, aiVqeChn);
    if (ret)
    {
        RK_LOGE("ai disable vqe failed:0x%x\n", ret);
    }
#endif

    ret = RK_MPI_AI_Disable(aiDevId);
    if (ret)
    {
        RK_LOGE("ai disable failed:0x%x\n", ret);
    }

#if AENC_EN
    ret = RK_MPI_AENC_DestroyChn(aeChn);
    if (ret)
    {
        RK_LOGE("aenc destroy chn failed:0x%x\n", ret);
    }
#endif

    return 0;
}

#ifdef AUDIO_IN_MAIN
static int duration = 0;
static int last_ts = -1;
static int ai_save(void *arg, char *buf, int len)
{
    int ret = fwrite(buf, 1, len, arg);

    duration += len / sizeof(short) / AUDIO_CH_OUT / (AUDIO_RATE / 1000);
    if (duration / 1000 != last_ts)
    {
        last_ts = duration / 1000;
        RK_LOGI("duration: %.3fs %d", duration / 1000.0, len);
    }

    return ret;
}

void main(void)
{
    void *buf;
    int len;
    FILE *fd;
    fd = fopen("/tmp/ch0.pcm", "wb+");

    RK_MPI_SYS_Init();

    ai_init();
    while (duration < 10000)
        ai_fetch(ai_save, fd);
    ai_deinit();

    RK_MPI_SYS_Exit();

    fclose(fd);
}
#endif

#endif
