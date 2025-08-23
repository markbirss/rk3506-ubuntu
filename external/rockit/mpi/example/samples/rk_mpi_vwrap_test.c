/*
 * Copyright 2018 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/prctl.h>

#include "rk_debug.h"
#include "rk_mpi_sys.h"
#include "rk_mpi_vi.h"
#include "rk_mpi_venc.h"
#include "rk_mpi_mb.h"

#include "test_comm_argparse.h"
#define VI_MAX_DEV_NUM 2

typedef struct _rkTestCtx {
    RK_S32 s32LoopCount;
    RK_S32 s32FrameCount;
    RK_S32 s32SwitchCount;
    RK_S32 s32AutoStartPipe;
    pthread_t tid[VI_MAX_DEV_NUM];
    RK_U32 u32Width[VI_MAX_DEV_NUM];
    RK_U32 u32Height[VI_MAX_DEV_NUM];
    RK_U32 u32WrapLine[VI_MAX_DEV_NUM];
    RK_U32 u32MinWrapLine[VI_MAX_DEV_NUM];
    RK_U32 u32MinWidth[VI_MAX_DEV_NUM];
    RK_U32 u32MaxWidth[VI_MAX_DEV_NUM];
    RK_S32 s32WidthStep[VI_MAX_DEV_NUM];
    RK_U32 u32MinHeight[VI_MAX_DEV_NUM];
    RK_U32 u32MaxHeight[VI_MAX_DEV_NUM];
    RK_S32 s32HeightStep[VI_MAX_DEV_NUM];
    RK_U32 u32ViDevPipeMode;
    RK_U32 u32WrapEn;
    RK_U32 u32CamNum;
    RK_CODEC_ID_E enCodecType[VI_MAX_DEV_NUM];
    FILE *fp[VI_MAX_DEV_NUM];
    RK_CHAR *pfileDir;
    RK_U32 u32BufCnt[VI_MAX_DEV_NUM];
    RK_U32 srcFrameRate[VI_MAX_DEV_NUM];
    RK_U32 dstFrameRate[VI_MAX_DEV_NUM];
    RK_U32 u32Mirror[VI_MAX_DEV_NUM];
    RK_U32 u32Flip[VI_MAX_DEV_NUM];
} TEST_CTX_S;


typedef struct _rkVencCfg {
    RK_U32 u32ChnId;
    TEST_CTX_S *pCtx;
} TEST_VENC_CFG_S;

TEST_VENC_CFG_S stVencCfg[0];
static int quit = 0;
static void sigterm_handler(int sig) {
    fprintf(stderr, "signal %d\n", sig);
    quit = 1;
}

RK_U64 TEST_COMM_GetNowUs() {
	struct timespec time = {0, 0};
	clock_gettime(CLOCK_MONOTONIC, &time);
	return (RK_U64)time.tv_sec * 1000000 + (RK_U64)time.tv_nsec / 1000; /* microseconds */
}

static void change_cur_w_h(TEST_CTX_S *pTestCtx, int chId) {
    pTestCtx->u32Width[chId] +=  pTestCtx->s32WidthStep[chId];
    if (pTestCtx->u32Width[chId] > pTestCtx->u32MaxWidth[chId]) {
        pTestCtx->u32Width[chId] = pTestCtx->u32MinWidth[chId];
        pTestCtx->u32Height[chId] +=  pTestCtx->s32HeightStep[chId];
    } else if (pTestCtx->u32Width[chId] < pTestCtx->u32MinWidth[chId]) {
        pTestCtx->u32Width[chId] = pTestCtx->u32MaxWidth[chId];
        pTestCtx->u32Height[chId] +=  pTestCtx->s32HeightStep[chId];
    }

    if (pTestCtx->u32Height[chId] > pTestCtx->u32MaxHeight[chId]) {
        pTestCtx->u32Height[chId] = pTestCtx->u32MinHeight[chId];
    } else if (pTestCtx->u32Height[chId] < pTestCtx->u32MinHeight[chId]) {
        pTestCtx->u32Height[chId] = pTestCtx->u32MaxHeight[chId];
    }
}

static void change_wrap_line(TEST_CTX_S *pTestCtx, int chId) {
    pTestCtx->u32WrapLine[chId]--;
    for(; pTestCtx->u32WrapLine[chId] > pTestCtx->u32MinWrapLine[chId]; pTestCtx->u32WrapLine[chId] -= 32) {
        if (!(pTestCtx->u32MaxHeight[chId] % pTestCtx->u32WrapLine[chId]))
            break;
    }
    if (pTestCtx->u32WrapLine[chId] < pTestCtx->u32MinWrapLine[chId])
        pTestCtx->u32WrapLine[chId] = pTestCtx->u32MaxHeight[chId];
}

static void open_file(TEST_CTX_S *pTestCtx, const RK_CHAR *pfileDir, int chId) {
    char inputPath[256], outputPath[256];
    const char *postfix = "h264";
    if (pTestCtx->enCodecType[chId] == RK_VIDEO_ID_AVC)
        postfix = "h264";
    else if (pTestCtx->enCodecType[chId] == RK_VIDEO_ID_HEVC)
        postfix = "h265";
    else if (pTestCtx->enCodecType[chId] == RK_VIDEO_ID_JPEG)
        postfix = "jpg";
    else if (pTestCtx->enCodecType[chId] == RK_VIDEO_ID_MJPEG)
        postfix = "mjpg";
    else {
        printf("nonsupport codec type: %d\n", pTestCtx->enCodecType[chId]);
    }

    memset(outputPath, 0, 256);
    sprintf(outputPath, "%s/venc_chn%d_%dx%d_%d.%s", pfileDir, chId,
        pTestCtx->u32Width[chId], pTestCtx->u32Height[chId], pTestCtx->u32WrapLine[chId], postfix);
    printf("#Output path: %s\n", outputPath);

    pTestCtx->fp[chId] = fopen(outputPath, "w");
    if (!pTestCtx->fp[chId])
        printf("open output file(%s) failed, errno[%d, %s]\n", outputPath, errno, strerror(errno));
}

static void close_file(TEST_CTX_S *pTestCtx, int chId) {
    if (pTestCtx->fp[chId]) {
        printf("close venc[%d] input file\n", chId);
        fclose(pTestCtx->fp[chId]);
    }
}

static void *get_venc_stream(void *arg) {
    RK_S32 s32Ret;
    RK_S32 loopCount = 0;
    RK_S32 switchCount = 0;
    void *pData = RK_NULL;
    VENC_PACK_S stPack;
    VENC_STREAM_S stFrame;
    RK_U64 u64prepts = 0;
    prctl(PR_SET_NAME, "get_venc_stream");
    TEST_VENC_CFG_S *pstVencCfg = (TEST_VENC_CFG_S *)arg;
    TEST_CTX_S *pTestCtx = pstVencCfg->pCtx;
    VENC_CHN_ATTR_S stAttr;
    VI_CHN_ATTR_S stChnAttr;
    MPP_CHN_S stSrcChn, stDestChn;
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32DevId = pstVencCfg->u32ChnId;
    stSrcChn.s32ChnId = 0;

    stDestChn.enModId = RK_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = pstVencCfg->u32ChnId;

    stFrame.pstPack = &stPack;
    while (!quit) {
        s32Ret = RK_MPI_VENC_GetStream(pstVencCfg->u32ChnId, &stFrame, 5000);
        if (s32Ret == RK_SUCCESS) {
            if (pTestCtx->fp[pstVencCfg->u32ChnId]) {
                pData = RK_MPI_MB_Handle2VirAddr(stFrame.pstPack->pMbBlk);
                fwrite(pData, 1, stFrame.pstPack->u32Len, pTestCtx->fp[pstVencCfg->u32ChnId]);
                fflush(pTestCtx->fp[pstVencCfg->u32ChnId]);
            }
            RK_U64 nowUs = TEST_COMM_GetNowUs();

            printf("--- %d, chn:%d, seq:%d, len:%d, pts=%lld, diff=%lld, delay=%lldus\n",
                loopCount, pstVencCfg->u32ChnId, stFrame.u32Seq, stFrame.pstPack->u32Len,
                stFrame.pstPack->u64PTS, stFrame.pstPack->u64PTS - u64prepts, nowUs - stFrame.pstPack->u64PTS);

            u64prepts = stFrame.pstPack->u64PTS;
            s32Ret = RK_MPI_VENC_ReleaseStream(pstVencCfg->u32ChnId, &stFrame);
            if (s32Ret != RK_SUCCESS)
                printf("RK_MPI_VENC_ReleaseStream[%d] failed: %x\n", pstVencCfg->u32ChnId, s32Ret);

            loopCount++;
        } else {
            printf("RK_MPI_VENC_GetStream[%d] timeout: %x\n", pstVencCfg->u32ChnId, s32Ret);
        }

        if ((pTestCtx->s32FrameCount >= 0) && (loopCount >= pTestCtx->s32FrameCount)) {
            loopCount = 0;
            switchCount++;
            if (pTestCtx->pfileDir)
                close_file(pTestCtx, pstVencCfg->u32ChnId);
            // 1, unbind
            s32Ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
            if (s32Ret != RK_SUCCESS) {
                RK_LOGE("RK_MPI_SYS_UnBind fail %x", s32Ret);
            }
            change_cur_w_h(pTestCtx, pstVencCfg->u32ChnId);
            if (pTestCtx->pfileDir)
                open_file(pTestCtx, pTestCtx->pfileDir, pstVencCfg->u32ChnId);
            RK_MPI_VENC_GetChnAttr(0, &stAttr);
            stAttr.stVencAttr.u32PicWidth = pTestCtx->u32Width[pstVencCfg->u32ChnId];
            stAttr.stVencAttr.u32PicHeight = pTestCtx->u32Height[pstVencCfg->u32ChnId];
            stAttr.stVencAttr.u32VirWidth =  pTestCtx->u32Width[pstVencCfg->u32ChnId];
            stAttr.stVencAttr.u32VirHeight = pTestCtx->u32Height[pstVencCfg->u32ChnId];
            RK_MPI_VENC_SetChnAttr(0, &stAttr);

            // 3, set vi
            RK_MPI_VI_GetChnAttr(0, 0, &stChnAttr);
            stChnAttr.stSize.u32Width = pTestCtx->u32Width[pstVencCfg->u32ChnId];
            stChnAttr.stSize.u32Height = pTestCtx->u32Height[pstVencCfg->u32ChnId];
            s32Ret = RK_MPI_VI_SetChnAttr(0, 0, &stChnAttr);
            if (s32Ret != RK_SUCCESS) {
                RK_LOGD("%d:RK_MPI_VI_SetChnAttr %d x %d fail, ret: %x", pstVencCfg->u32ChnId,
                        pTestCtx->u32Width[pstVencCfg->u32ChnId], pTestCtx->u32Height[pstVencCfg->u32ChnId],
                        s32Ret);
            } else {
                RK_LOGD("====%d: RK_MPI_VI_SetChnAttr %d x %d success====", pstVencCfg->u32ChnId,
                        pTestCtx->u32Width[pstVencCfg->u32ChnId], pTestCtx->u32Height[pstVencCfg->u32ChnId]);
            }

            // 4, bind
            s32Ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
            if (s32Ret != RK_SUCCESS) {
                RK_LOGE("bind %x ch venc failed", s32Ret);
            }

            if ((pTestCtx->s32SwitchCount >=0) && (switchCount >= pTestCtx->s32SwitchCount)) {
                quit = 1;
                break;
            }
        }
    }

    printf("Exit get venc[%d] stream thread\n", pstVencCfg->u32ChnId);
    system("cat /proc/vcodec/enc/venc_info | grep -A 2 RealFps");
    return NULL;
 }

static int vi_dev_init(int devId, int autoStartpipe) {
    printf("%s\n", __func__);
    int ret = 0;
    int pipeId = devId;

    VI_DEV_ATTR_S stDevAttr;
    VI_DEV_BIND_PIPE_S stBindPipe;
    memset(&stDevAttr, 0, sizeof(stDevAttr));
    memset(&stBindPipe, 0, sizeof(stBindPipe));

    // 0. get dev config status
    ret = RK_MPI_VI_GetDevAttr(devId, &stDevAttr);
    if (ret == RK_ERR_VI_NOT_CONFIG) {
        // 0-1.config dev
        ret = RK_MPI_VI_SetDevAttr(devId, &stDevAttr);
        if (ret != RK_SUCCESS) {
            printf("RK_MPI_VI_SetDevAttr[%d] failed: %x\n", devId, ret);
            return -1;
        }
    } else {
        printf("RK_MPI_VI_SetDevAttr already\n");
    }

    // 1.get dev enable status
    ret = RK_MPI_VI_GetDevIsEnable(devId);
    if (ret != RK_SUCCESS) {
        // 1-2.enable dev
        ret = RK_MPI_VI_EnableDev(devId);
        if (ret != RK_SUCCESS) {
            printf("RK_MPI_VI_EnableDev[%d] %x\n", devId, ret);
            return -1;
        }

        // 1-3.bind dev/pipe
        stBindPipe.u32Num = 1;
        stBindPipe.PipeId[0] = pipeId;
        stBindPipe.bUserStartPipe[0] = !autoStartpipe;
        ret = RK_MPI_VI_SetDevBindPipe(devId, &stBindPipe);
        if (ret != RK_SUCCESS) {
            printf("RK_MPI_VI_SetDevBindPipe[%d] %x\n", devId, ret);
            return -1;
        }
    } else {
        printf("RK_MPI_VI_EnableDev already\n");
    }

    return 0;
}

static int vi_chn_init(TEST_CTX_S *pTestCtx, int pipeId) {
    int ret = 0;
    VI_CHN_ATTR_S stViChnAttr;
    VI_CHN_BUF_WRAP_S stViWrap;

    printf("enter vi_chn_init[%d, %d]\n", pipeId, 0);

    memset(&stViChnAttr, 0, sizeof(VI_CHN_ATTR_S));
    stViChnAttr.stIspOpt.u32BufCount = pTestCtx->u32BufCnt[pipeId];
    stViChnAttr.stIspOpt.enMemoryType = VI_V4L2_MEMORY_TYPE_DMABUF; // VI_V4L2_MEMORY_TYPE_MMAP;
    stViChnAttr.stSize.u32Width = pTestCtx->u32Width[pipeId];
    stViChnAttr.stSize.u32Height = pTestCtx->u32Height[pipeId];
    stViChnAttr.enPixelFormat = RK_FMT_YUV420SP;
    stViChnAttr.stFrameRate.s32SrcFrameRate = pTestCtx->srcFrameRate[pipeId];
    stViChnAttr.stFrameRate.s32DstFrameRate = pTestCtx->dstFrameRate[pipeId];
    stViChnAttr.bMirror = pTestCtx->u32Mirror[pipeId];
    stViChnAttr.bFlip = pTestCtx->u32Flip[pipeId];

    stViChnAttr.stIspOpt.stMaxSize.u32Width =  pTestCtx->u32MaxWidth[pipeId];
    stViChnAttr.stIspOpt.stMaxSize.u32Height = pTestCtx->u32MaxHeight[pipeId];
    stViChnAttr.u32Depth = 0; //0, get fail, 1 - u32BufCount, can get, if bind to other device, must be < u32BufCount

    if (pTestCtx->u32CamNum > 1 && pTestCtx->u32WrapEn) {
        stViChnAttr.stShareBufChn.enModId = RK_ID_VI;
        stViChnAttr.stShareBufChn.s32DevId = 0;
        stViChnAttr.stShareBufChn.s32ChnId = 0;
        stViChnAttr.enAllocBufType = pipeId? VI_ALLOC_BUF_TYPE_CHN_SHARE : VI_ALLOC_BUF_TYPE_INTERNAL;
    }

    printf("vi[%d, %d] enAllocBufType: %d, stShareBufChn.s32ChnId: %d\n",
                    pipeId, 0, stViChnAttr.enAllocBufType, stViChnAttr.stShareBufChn.s32ChnId);

    ret = RK_MPI_VI_SetChnAttr(pipeId, 0, &stViChnAttr);
    if (ret) {
        printf("ERROR: VI set channel attribute fail, ret=%d\n", ret);
        return ret;
    }

    // set wrap mode attr
    memset(&stViWrap, 0, sizeof(VI_CHN_BUF_WRAP_S));
    if (pTestCtx->u32WrapEn) {
        stViWrap.bEnable = RK_TRUE;
    retry:
        stViWrap.u32BufLine = pTestCtx->u32WrapLine[pipeId];
        stViWrap.u32WrapBufferSize = stViWrap.u32BufLine * pTestCtx->u32MaxWidth[pipeId] * 3 / 2; // nv12 (w * wrapLine *3 / 2)
        printf("set channel: %d wrap line: %d, wrapBuffSize = %d\n", 0, pTestCtx->u32WrapLine[pipeId], stViWrap.u32WrapBufferSize);
        ret = RK_MPI_VI_SetChnWrapBufAttr(pipeId, 0, &stViWrap);
        if (ret) {
            printf("ERROR: RK_MPI_VI_SetChnWrapBufAttr failed: %x\n", ret);
            change_wrap_line(pTestCtx, pipeId);
            goto retry;
        }
    }

    ret = RK_MPI_VI_EnableChn(pipeId, 0);
    if (ret) {
        printf("ERROR: RK_MPI_VI_EnableChn failed: %x\n", ret);
        return ret;
    }
    printf("vi dev:%d channle:%d enable success\n", pipeId, 0);
    return ret;
}

static int init_all_vi_wrap(TEST_CTX_S *pTestCtx, int devId) {
    int ret;

    ret = vi_dev_init(devId, pTestCtx->s32AutoStartPipe);
    if (ret) {
        printf("vi_dev_init[%d] failed[%x]", devId, ret);
        return -1;
    }

    ret = vi_chn_init(pTestCtx, devId);
    if (ret) {
        printf("vi_chn_init[%d, %d] failed[%x]", devId, 0, ret);
        return -1;
    }
    printf("vi chn[%d, %d] init ok\n", devId, 0);
    return 0;
}

static int deinit_all_vi_wrap(TEST_CTX_S *pTestCtx, int devId) {
    int ret = 0;
    ret = RK_MPI_VI_DisableChn(devId, 0);
    printf("RK_MPI_VI_DisableChn[%d, %d]: %x\n",devId, 0, ret);
    ret = RK_MPI_VI_DisableDev(devId);
    printf("RK_MPI_VI_DisableDev[%d]: %x\n",devId, ret);
    return ret;
}

static RK_S32 venc_chn_init(TEST_CTX_S *pTestCtx, int chId) {
    int ret;
    VENC_CHN_ATTR_S stVencChnAttr;
    VENC_RECV_PIC_PARAM_S stRecvParam;
    VENC_CHN_BUF_WRAP_S stVencChnBufWrap;
    VENC_RC_MODE_E enRcMode = VENC_RC_MODE_BUTT;

    memset(&stVencChnAttr, 0, sizeof(VENC_CHN_ATTR_S));

    RK_U32 u32GopSize = 30; //pVencCfg->u32Framerate;

    stVencChnAttr.stVencAttr.enType = pTestCtx->enCodecType[chId];
    stVencChnAttr.stVencAttr.enPixelFormat = RK_FMT_YUV420SP;

    if (stVencChnAttr.stVencAttr.enType == RK_VIDEO_ID_AVC) {
        enRcMode = VENC_RC_MODE_H264CBR;
        stVencChnAttr.stRcAttr.stH264Cbr.u32BitRate = 10 * 1024;
        stVencChnAttr.stRcAttr.stH264Cbr.u32Gop = u32GopSize;
        stVencChnAttr.stRcAttr.stH264Cbr.fr32DstFrameRateDen = 1;
        stVencChnAttr.stRcAttr.stH264Cbr.fr32DstFrameRateNum = pTestCtx->dstFrameRate[chId];
        stVencChnAttr.stRcAttr.stH264Cbr.u32SrcFrameRateDen = 1;
        stVencChnAttr.stRcAttr.stH264Cbr.u32SrcFrameRateNum = pTestCtx->srcFrameRate[chId];
    } else if (stVencChnAttr.stVencAttr.enType == RK_VIDEO_ID_HEVC) {
        enRcMode = VENC_RC_MODE_H265CBR;
        stVencChnAttr.stRcAttr.stH265Cbr.u32BitRate = 10 * 1024;
        stVencChnAttr.stRcAttr.stH265Cbr.u32Gop = u32GopSize;
        stVencChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRateDen = 1;
        stVencChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRateNum = pTestCtx->dstFrameRate[chId];
        stVencChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRateDen = 1;
        stVencChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRateNum = pTestCtx->srcFrameRate[chId];
    } else if (stVencChnAttr.stVencAttr.enType == RK_VIDEO_ID_MJPEG) {
        enRcMode = VENC_RC_MODE_MJPEGCBR;
        stVencChnAttr.stRcAttr.stMjpegCbr.u32BitRate = 10 * 1024;
        stVencChnAttr.stRcAttr.stMjpegCbr.fr32DstFrameRateDen = 1;
        stVencChnAttr.stRcAttr.stMjpegCbr.fr32DstFrameRateNum = pTestCtx->dstFrameRate[chId];
        stVencChnAttr.stRcAttr.stMjpegCbr.u32SrcFrameRateDen = 1;
        stVencChnAttr.stRcAttr.stMjpegCbr.u32SrcFrameRateNum = pTestCtx->srcFrameRate[chId];
    }

    stVencChnAttr.stRcAttr.enRcMode = enRcMode;
    stVencChnAttr.stVencAttr.u32MaxPicWidth = pTestCtx->u32MaxWidth[chId];
    stVencChnAttr.stVencAttr.u32MaxPicHeight = pTestCtx->u32MaxHeight[chId];
    stVencChnAttr.stVencAttr.u32PicWidth = pTestCtx->u32Width[chId];
    stVencChnAttr.stVencAttr.u32PicHeight = pTestCtx->u32Height[chId];
    stVencChnAttr.stVencAttr.u32VirWidth = pTestCtx->u32Width[chId];
    stVencChnAttr.stVencAttr.u32VirHeight = pTestCtx->u32Height[chId];
    stVencChnAttr.stVencAttr.u32StreamBufCnt = 5;
    stVencChnAttr.stVencAttr.u32BufSize = pTestCtx->u32Width[chId] * pTestCtx->u32Height[chId];
    ret = RK_MPI_VENC_CreateChn(chId, &stVencChnAttr);
    if (ret) {
        printf("RK_MPI_VENC_CreateChn[%d] fialed: %x\n", chId, ret);
        return -1;
    }

    if (pTestCtx->u32WrapEn) {
        memset(&stVencChnBufWrap, 0, sizeof(stVencChnBufWrap));
        stVencChnBufWrap.bEnable = RK_TRUE;
        stVencChnBufWrap.u32BufLine = pTestCtx->u32WrapLine[chId];
        printf("set venc channel wrap line: %d\n", stVencChnBufWrap.u32BufLine);

        ret = RK_MPI_VENC_SetChnBufWrapAttr(chId, &stVencChnBufWrap);
        if (ret)
            printf("RK_MPI_VENC_SetChnBufWrapAttr[%d] failed: %x\n", chId, ret);
    }

    memset(&stRecvParam, 0, sizeof(VENC_RECV_PIC_PARAM_S));
    stRecvParam.s32RecvPicNum = -1;
    ret = RK_MPI_VENC_StartRecvFrame(chId, &stRecvParam);
    if (ret)
        printf("RK_MPI_VENC_StartRecvFrame[%d] failed: %x\n", chId, ret);

    return RK_SUCCESS;
}

static int init_venc_wrap(TEST_CTX_S *pTestCtx, int chId) {
    int ret = 0;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    ret = venc_chn_init(pTestCtx, chId);
    if (ret != RK_SUCCESS) {
        printf("create %d ch venc failed\n", chId);
        return -1;
    }
    printf("venc chn[%d] init ok\n", chId);

    // bind vi to venc
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32DevId = chId;
    stSrcChn.s32ChnId = 0;
    stDestChn.enModId = RK_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = chId;
    ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret != RK_SUCCESS) {
        printf("vi[%d] bind venc[%d] ch venc failed\n", stSrcChn.s32ChnId, stDestChn.s32ChnId);
        goto _FAILED_1;
    }
    printf("vi[%d, %d] bind venc[%d] ok\n", chId, 0, chId);
    stVencCfg[chId].u32ChnId = chId;
    stVencCfg[chId].pCtx = pTestCtx;
    ret = pthread_create(&pTestCtx->tid[chId], NULL, get_venc_stream, &stVencCfg[chId]);
    if (ret) {
        printf("Create get venc mb(%d) thread failed %d\n", chId, ret);
        goto _FAILED;
    }
    return 0;

_FAILED:
    ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    printf("RK_MPI_SYS_UnBind[%d, %d]: %x\n", stSrcChn.s32ChnId, stDestChn.s32ChnId, ret);

_FAILED_1:
    ret = RK_MPI_VENC_DestroyChn(chId);
    printf("RK_MPI_VENC_DestroyChn[%d]: %x\n", chId, ret);

    return -1;
}

static int deinit_venc_wrap(TEST_CTX_S *pTestCtx, int chId) {
    int ret = 0;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    if (pTestCtx->tid[chId]) {
        ret = pthread_join(pTestCtx->tid[chId], NULL);
        if (ret)
            printf("Exit get venc[%d] mb thread failed!\n", chId);
        else
            printf("Exit get venc[%d] mb thread ok\n", chId);
        pTestCtx->tid[chId] = 0;
    }

    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32DevId = chId;
    stSrcChn.s32ChnId = 0;
    stDestChn.enModId = RK_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = chId;
    ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    printf("RK_MPI_SYS_UnBind[%d, %d]: %x\n", stSrcChn.s32ChnId, stDestChn.s32ChnId, ret);

    ret = RK_MPI_VENC_DestroyChn(chId);
    printf("RK_MPI_VENC_DestroyChn[%d]: %x\n", chId, ret);

    return ret;
}

RK_S32 unit_mpi_vplay_test(TEST_CTX_S *pTestCtx) {
    quit = 0;
    for (int i = 0; i < pTestCtx->u32CamNum; i++) {
        if (pTestCtx->pfileDir) {
            open_file(pTestCtx, pTestCtx->pfileDir, i);
        }
        init_all_vi_wrap(pTestCtx, i);
        init_venc_wrap(pTestCtx, i);
    }

    while (!quit) {
        usleep(50000);
    }

    for (int i = 0; i < pTestCtx->u32CamNum; i++) {
        deinit_venc_wrap(pTestCtx, i);
        deinit_all_vi_wrap(pTestCtx, i);
        change_wrap_line(pTestCtx, i);
        if (pTestCtx->pfileDir) {
            close_file(pTestCtx, i);
        }
    }
}

static const char *const usages[] = {
    "./rk_mpi_vwrap_test -c 1 -n 10 -l 10 -s 10000 -o /tmp",
    RK_NULL,
};

void init_cfg(TEST_CTX_S *ctx) {
    ctx->u32CamNum = 1;
    ctx->s32FrameCount = 10;
    ctx->s32AutoStartPipe = 1;
    ctx->u32ViDevPipeMode = VI_DEV_PIPE_ONLINE;
    ctx->u32WrapEn = 1;
    ctx->pfileDir = NULL;
    for (int i = 0; i < VI_MAX_DEV_NUM; i++) {
        ctx->u32Width[i] = 1920;
        ctx->u32Height[i] = 1080;
        ctx->u32WrapLine[i] = 64;
        ctx->u32MinWrapLine[i] = 64;
        ctx->u32MinWidth[i] = 32;
        ctx->u32MaxWidth[i] = 1920;
        ctx->s32WidthStep[i] = -8;
        ctx->u32MinHeight[i] = 32;
        ctx->u32MaxHeight[i] = 1080;
        ctx->s32HeightStep[i] = 8;
        ctx->enCodecType[i] = RK_VIDEO_ID_AVC;
        ctx->u32BufCnt[i] = 3;
        ctx->srcFrameRate[i] = 30;
        ctx->dstFrameRate[i] = 30;
        ctx->u32Mirror[i] = 0;
        ctx->u32Flip[i] = 0;
    }
    ctx->s32LoopCount = 16;
    ctx->s32SwitchCount = 30916;
}

static RK_S32 check_options(const TEST_CTX_S *ctx) {
    RK_PRINT("cmd parse result:\n");
    RK_PRINT("u32CamNum             : %d\n", ctx->u32CamNum);
    RK_PRINT("s32FrameCount         : %d\n", ctx->s32FrameCount);
    RK_PRINT("s32AutoStartPipe      : %d\n", ctx->s32AutoStartPipe);
    RK_PRINT("u32ViDevPipeMode      : %d\n", ctx->u32ViDevPipeMode);
    RK_PRINT("u32WrapEn             : %d\n", ctx->u32WrapEn);
    RK_PRINT("pfileDir              : %s\n", ctx->pfileDir);
    for (int i = 0; i < VI_MAX_DEV_NUM; i++) {
        RK_PRINT("u32Width[%d]          : %d\n", i, ctx->u32Width[i]);
        RK_PRINT("u32Height[%d]         : %d\n", i, ctx->u32Height[i]);
        RK_PRINT("u32WrapLine[%d]       : %d\n", i, ctx->u32WrapLine[i]);
        RK_PRINT("u32MinWidth[%d]       : %d\n", i, ctx->u32MinWidth[i]);
        RK_PRINT("u32MaxWidth[%d]       : %d\n", i, ctx->u32MaxWidth[i]);
        RK_PRINT("s32WidthStep[%d]      : %d\n", i, ctx->s32WidthStep[i]);
        RK_PRINT("u32MinHeight[%d]      : %d\n", i, ctx->u32MinHeight[i]);
        RK_PRINT("u32MaxHeight[%d]      : %d\n", i, ctx->u32MaxHeight[i]);
        RK_PRINT("s32HeightStep[%d]     : %d\n", i, ctx->s32HeightStep[i]);
        RK_PRINT("enCodecType[%d]       : %d\n", i, ctx->enCodecType[i]);
        RK_PRINT("u32BufCnt[%d]         : %d\n", i, ctx->u32BufCnt[i]);
        RK_PRINT("srcFrameRate[%d]      : %d\n", i, ctx->srcFrameRate[i]);
        RK_PRINT("dstFrameRate[%d]      : %d\n", i, ctx->dstFrameRate[i]);
        RK_PRINT("u32Mirror[%d]         : %d\n", i, ctx->u32Mirror[i]);
        RK_PRINT("u32Flip[%d]           : %d\n", i, ctx->u32Flip[i]);
    }
    RK_PRINT("s32LoopCount              : %d\n", ctx->s32LoopCount);
    RK_PRINT("s32SwitchCount            : %d\n", ctx->s32SwitchCount);
    return RK_SUCCESS;
__FAILED:
    return RK_FAILURE;
}

RK_S32 main(RK_S32 argc, const char **argv) {
    TEST_CTX_S ctx;
    VI_PARAM_MOD_S stModParam = {0};
    RK_S32 ret = 0;

    memset(&ctx, 0, sizeof(TEST_CTX_S));
    init_cfg(&ctx);
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("basic options:"),
        OPT_STRING('o', "output", &(ctx.pfileDir),
            "save output path, default: null"),
        OPT_INTEGER('l', "loop", &(ctx.s32LoopCount),
                    "set wrap line swtich counter (default 16)", NULL, 0, 0),
        OPT_INTEGER('s', "switch", &(ctx.s32SwitchCount),
                    "set w*h swtich counter (default 30916)", NULL, 0, 0),
        OPT_INTEGER('w', "wrap", &(ctx.u32WrapEn),
                    "enable wrap (default 1), 1 hard wrap, 2 foft wrap", NULL, 0, 0),
        OPT_INTEGER('a', "startpipe", &(ctx.s32AutoStartPipe),
                    "create and start vi pipe at once (default 1)", NULL, 0, 0),
        OPT_INTEGER('\0', "pmode", &(ctx.u32ViDevPipeMode),
                    "pipe work mode (default 2) \n\t"
                    "1: VI_DEV_PIPE_OFFLINE\n\t"
                    "2: VI_DEV_PIPE_ONLINE\n\t"
                    "3: VI_DEV_PIPE_LEFT_HALF_ONLINE\n\t"
                    "4: VI_DEV_PIPE_RIGHT_HALF_ONLINE\n\t"
                    "5: VI_DEV_PIPE_UNITE_HALF_ONLINE", NULL, 0, 0),
        OPT_INTEGER('n', "frame", &(ctx.s32FrameCount),
                    "switch once per framecount (default 10)", NULL, 0, 0),
        OPT_INTEGER('c', "camrea", &(ctx.u32CamNum),
                    "sensor count (default 1)", NULL, 0, 0),
        OPT_INTEGER('\0', "w0", &(ctx.u32Width[0]),
                    "the first output image width (default 1920)", NULL, 0, 0),
        OPT_INTEGER('\0', "h0", &(ctx.u32Height[0]),
                    "the first output image height (default 1080)", NULL, 0, 0),
        OPT_INTEGER('\0', "wl0", &(ctx.u32WrapLine[0]),
                    "the init wrap line (default 64)", NULL, 0, 0),
        OPT_INTEGER('\0', "wl0", &(ctx.u32MinWrapLine[0]),
                    "the min wrap line (default 64)", NULL, 0, 0),
        OPT_INTEGER('\0', "WI0", &(ctx.u32MinWidth[0]),
                    "the first output image min width (default 32)", NULL, 0, 0),
        OPT_INTEGER('\0', "HI0", &(ctx.u32MinHeight[0]),
                    "the first output image min height (default 32)", NULL, 0, 0),
        OPT_INTEGER('\0', "WM0", &(ctx.u32MaxWidth[0]),
                    "the first output image max width (default 1920)", NULL, 0, 0),
        OPT_INTEGER('\0', "HM0", &(ctx.u32MaxHeight[0]),
                    "the first output image max height (default 1080)", NULL, 0, 0),
        OPT_INTEGER('\0', "SW0", &(ctx.s32WidthStep[0]),
                    "the first output image width step (default -8)", NULL, 0, 0),
        OPT_INTEGER('\0', "SH0", &(ctx.s32HeightStep[0]),
                    "the first output image height step (default 8)", NULL, 0, 0),
        OPT_INTEGER('\0', "buf0", &(ctx.u32BufCnt[0]),
                    "the first output bufcount (default 3)", NULL, 0, 0),
        OPT_INTEGER('\0', "sfr0", &(ctx.srcFrameRate[0]),
                    "the first output source frame rate (default 30)", NULL, 0, 0),
        OPT_INTEGER('\0', "dfr0", &(ctx.dstFrameRate[0]),
                    "the first output dst frame rate (default 30)", NULL, 0, 0),
        OPT_INTEGER('\0', "m0", &(ctx.u32Mirror[0]),
                    "the first output enable mirror (default 0)", NULL, 0, 0),
        OPT_INTEGER('\0', "f0", &(ctx.u32Flip[0]),
                    "the first output enable flip (default 0)", NULL, 0, 0),
        OPT_INTEGER('\0', "w1", &(ctx.u32Width[1]),
                    "the second output image width (default 1920)", NULL, 0, 0),
        OPT_INTEGER('\0', "h1", &(ctx.u32Height[1]),
                    "the second output image height (default 1080)", NULL, 0, 0),
        OPT_INTEGER('\0', "wl1", &(ctx.u32WrapLine[0]),
                    "the init wrap line (default 64)", NULL, 0, 0),
        OPT_INTEGER('\0', "wl1", &(ctx.u32MinWrapLine[1]),
                    "the min wrap line (default 64)", NULL, 0, 0),
        OPT_INTEGER('\0', "WI1", &(ctx.u32MinWidth[1]),
                    "the second output image min width (default 32)", NULL, 0, 0),
        OPT_INTEGER('\0', "HI1", &(ctx.u32MinHeight[1]),
                    "the second output image min height (default 32)", NULL, 0, 0),
        OPT_INTEGER('\0', "WM1", &(ctx.u32MaxWidth[1]),
                    "the second output image max width (default 1920)", NULL, 0, 0),
        OPT_INTEGER('\0', "HM1", &(ctx.u32MaxHeight[1]),
                    "the second output image max height (default 1080)", NULL, 0, 0),
        OPT_INTEGER('\0', "SW1", &(ctx.s32WidthStep[1]),
                    "the second output image width step (default -8)", NULL, 0, 0),
        OPT_INTEGER('\0', "SH1", &(ctx.s32HeightStep[1]),
                    "the second output image height step (default 8)", NULL, 0, 0),
        OPT_INTEGER('\0', "buf1", &(ctx.u32BufCnt[1]),
                    "the second output bufcount (default 3)", NULL, 0, 0),
        OPT_INTEGER('\0', "sfr1", &(ctx.srcFrameRate[1]),
                    "the second output source frame rate (default 30)", NULL, 0, 0),
        OPT_INTEGER('\0', "dfr1", &(ctx.dstFrameRate[1]),
                    "the second output dst frame rate (default 30)", NULL, 0, 0),
        OPT_INTEGER('\0', "m1", &(ctx.u32Mirror[1]),
                    "the second output enable mirror (default 0)", NULL, 0, 0),
        OPT_INTEGER('\0', "f1", &(ctx.u32Flip[1]),
                    "the second output enable flip (default 0)", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nselect a test case to run.",
                                 "\nuse --help for details.");

    argc = argparse_parse(&argparse, argc, argv);
    if (argc < 0)
        return 0;

    if (check_options(&ctx)) {
        RK_LOGE("illegal input parameters");
        argparse_usage(&argparse);
        goto __FAILED;
    }

    if (RK_MPI_SYS_Init() != RK_SUCCESS) {
        goto __FAILED;
    }

    for (int i = 0; i < VI_MAX_DEV_NUM; i++) {
         ctx.u32WrapLine[i] = ctx.u32MinWrapLine[i];
    }

    stModParam.enViModType = VI_DEV_PIPE_MODE;
    stModParam.stDevPipeModParam.enDevPipeMode =  ctx.u32ViDevPipeMode;
    if (ret = RK_MPI_VI_SetModParam(&stModParam)) {
        RK_LOGE("RK_MPI_VI_SetModParam fail:%#X", ret);
    }

    if (ctx.u32WrapEn == 2) {
        stModParam.enViModType = VI_CHN_WRAP_MODE;
        stModParam.stChnWrapModParam.enChnWrapMode = VI_CHN_WRAP_SOFT;
        if (ret = RK_MPI_VI_SetModParam(&stModParam)) {
            RK_LOGE("RK_MPI_VI_SetModParam fail:%#X", ret);
        }
    }

    RK_S32 loopCnt = ctx.s32LoopCount;
    while (loopCnt > 0) {
        if (unit_mpi_vplay_test(&ctx) < 0) {
            goto __FAILED;
        }
        loopCnt--;
    }

    if (RK_MPI_SYS_Exit() != RK_SUCCESS) {
        goto __FAILED;
    }

    RK_LOGE("test running success!");
    return RK_SUCCESS;
__FAILED:
    RK_LOGE("test running failed!");
    return RK_FAILURE;
}
