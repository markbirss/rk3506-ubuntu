/*
 * Copyright 2022 Rockchip Electronics Co. LTD
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "rk_debug.h"
#include "rk_comm_pvs.h"
#include "rk_mpi_pvs.h"
#include "rk_mpi_mb.h"
#include "rk_mpi_cal.h"
#include "rk_mpi_sys.h"
#include "test_comm_argparse.h"
#include "test_comm_utils.h"
#include "test_comm_app_vdec.h"
#include "test_comm_app_vo.h"

typedef struct _rkMpiPvsTestCtx {
    const char           *pSrcFilePath;
    const char           *pDstFilePath;
    RK_S32                s32DevId;
    RK_S32                s32ChnId;

    // source parameters
    RK_U32                u32SrcWidth;
    RK_U32                u32SrcHeight;
    RK_U32                u32SrcVirWidth;
    RK_U32                u32SrcVirHeight;
    RK_U32                u32SrcBufferSize;
    RK_S32                s32SrcFrameRate;
    RK_S32                s32RecvThreshold;
    PIXEL_FORMAT_E        enSrcPixelFormat;
    COMPRESS_MODE_E       enSrcCompressMode;

    // dest parameters
    RK_S32                s32StitchMode;
    RK_S32                s32StitchFrmCnt;
    PVS_DEV_ATTR_S        stDevAttr;
    VIDEO_PROC_DEV_TYPE_E enVProcDev;

    RK_U32                u32RCNum;
    RK_S32                s32LoopCount;
    RK_U32                u32TestMode;
    RK_U32                u32TotalChn;
    pthread_t            *pSendFrameThreads;
} TEST_PVS_CTX_S;

static MB_POOL test_pvs_create_src_pool(TEST_PVS_CTX_S *ctx) {
    PIC_BUF_ATTR_S stPicBufAttr;
    MB_PIC_CAL_S stPicCalResult;
    MB_POOL_CONFIG_S stPvsPoolCfg;

    stPicBufAttr.u32Width = ctx->u32SrcWidth;
    stPicBufAttr.u32Height = ctx->u32SrcHeight;
    stPicBufAttr.enPixelFormat = ctx->enSrcPixelFormat;
    stPicBufAttr.enCompMode = ctx->enSrcCompressMode;
    // NOTE: relative to the input image, not a best practice
    if (ctx->enSrcCompressMode == COMPRESS_AFBC_16x16) {
        RK_MPI_CAL_VGS_GetPicBufferSize(&stPicBufAttr, &stPicCalResult);
    } else {
        RK_MPI_CAL_COMM_GetPicBufferSize(&stPicBufAttr, &stPicCalResult);
    }

    ctx->u32SrcVirWidth = stPicCalResult.u32VirWidth;
    ctx->u32SrcVirHeight = stPicCalResult.u32VirHeight;
    ctx->u32SrcBufferSize = stPicCalResult.u32MBSize;

    memset(&stPvsPoolCfg, 0, sizeof(MB_POOL_CONFIG_S));
    stPvsPoolCfg.u32MBCnt  = 4;
    stPvsPoolCfg.u64MBSize = ctx->u32SrcBufferSize;
    stPvsPoolCfg.enAllocType = MB_ALLOC_TYPE_DMA;
    stPvsPoolCfg.enRemapMode = MB_REMAP_MODE_CACHED;
    stPvsPoolCfg.bPreAlloc   = RK_TRUE;

    return RK_MPI_MB_CreatePool(&stPvsPoolCfg);
}

static void test_pvs_send_frame_with_file(TEST_PVS_CTX_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    RK_U32 u32ReadSize = 0;
    RK_U64 pts = 0;
    RK_U8 *pVirAddr = RK_NULL;
    MB_BLK blk = RK_NULL;
    MB_POOL srcPvsPool = MB_INVALID_POOLID;
    VIDEO_FRAME_INFO_S stSrcFrameInfo;
    FILE *fpIn = RK_NULL;
    RK_S32 s32LoopCnt = ctx->s32LoopCount == 0 ? 1 : ctx->s32LoopCount;

    srcPvsPool = test_pvs_create_src_pool(ctx);

    stSrcFrameInfo.stVFrame.u32Width = ctx->u32SrcWidth;
    stSrcFrameInfo.stVFrame.u32Height = ctx->u32SrcHeight;
    stSrcFrameInfo.stVFrame.u32VirWidth = ctx->u32SrcVirWidth;
    stSrcFrameInfo.stVFrame.u32VirHeight = ctx->u32SrcVirHeight;
    stSrcFrameInfo.stVFrame.enCompressMode = ctx->enSrcCompressMode;
    stSrcFrameInfo.stVFrame.enPixelFormat = ctx->enSrcPixelFormat;

    if (ctx->pSrcFilePath != RK_NULL) {
        fpIn = fopen(ctx->pSrcFilePath, "r");
    } else {
        RK_LOGE("src file %s not existed", ctx->pSrcFilePath);
        goto __FAILED;
    }

    while (!feof(fpIn)) {
        blk = RK_MPI_MB_GetMB(srcPvsPool, 0, RK_TRUE);
        pVirAddr = reinterpret_cast<RK_U8 *>(RK_MPI_MB_Handle2VirAddr(blk));
        u32ReadSize = fread(pVirAddr, 1, ctx->u32SrcBufferSize, fpIn);
        if (u32ReadSize != ctx->u32SrcBufferSize) {
            RK_LOGE("no complete frame read: %d, require: %d", u32ReadSize, ctx->u32SrcBufferSize);
            RK_MPI_MB_ReleaseMB(blk);

            // reach eos
            if (u32ReadSize == 0) {
                if (s32LoopCnt == -1 || --s32LoopCnt > 0) {
                    fseek(fpIn, 0, SEEK_SET);
                    RK_LOGD("stream loop count %d", s32LoopCnt);
                    continue;
                }
            }
            break;
        }

        RK_MPI_SYS_MmzFlushCache(blk, RK_FALSE);
        stSrcFrameInfo.stVFrame.u64PTS = pts;
        stSrcFrameInfo.stVFrame.pMbBlk = blk;

        s32Ret = RK_MPI_PVS_SendFrame(ctx->s32DevId, ctx->s32ChnId, &stSrcFrameInfo);
        if (RK_SUCCESS != s32Ret) {
            RK_MPI_PVS_ReleaseFrame(&stSrcFrameInfo);
            RK_LOGE("Send frame failed: %d", ctx->s32ChnId);
        }
        RK_MPI_MB_ReleaseMB(blk);

        usleep(1000000 / ctx->s32SrcFrameRate);
        pts += 1000000 / ctx->s32SrcFrameRate;
    }

__FAILED:
    RK_LOGI("stop sending frame");
    if (fpIn != RK_NULL)
        fclose(fpIn);
    RK_MPI_MB_DestroyPool(srcPvsPool);
}

RK_S32 test_pvs_vdec_bind_pvs(TEST_PVS_CTX_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    TEST_VDEC_CFG_S stVdecCfg;
    TEST_VO_CFG_S stVoCfg;
    MPP_CHN_S stDestChn;

    memset(&stVdecCfg, 0, sizeof(TEST_VDEC_CFG_S));
    stVdecCfg.s32VdecChn = ctx->s32ChnId;
    stVdecCfg.u32CompressMode = ctx->enSrcCompressMode;
    stVdecCfg.enVideoMode = VIDEO_MODE_FRAME;
    stVdecCfg.enDispMode = VIDEO_DISPLAY_MODE_PLAYBACK;
    stVdecCfg.enVdecSource = TEST_MPI_SOURCE_SEND;
    stVdecCfg.stStream.pSrcFilePath = ctx->pSrcFilePath;
    stVdecCfg.stStream.s32ReadLoopCnt = ctx->s32LoopCount;

    stDestChn.enModId = RK_ID_PVS;
    stDestChn.s32DevId = ctx->s32DevId;
    stDestChn.s32ChnId = ctx->s32ChnId;
    TEST_COMM_APP_VDEC_StartProcWithDstChn(&stVdecCfg, &stDestChn);

    TEST_COMM_APP_VDEC_WaitUntilEos(&stVdecCfg);

    TEST_COMM_APP_VDEC_StopProcWithDstChn(&stVdecCfg, &stDestChn);

    return s32Ret;
}

static void test_pvs_send_frame_with_vdec(TEST_PVS_CTX_S *ctx) {
    test_pvs_vdec_bind_pvs(ctx);
}

static void *test_pvs_send_frame(void *pArgs) {
    TEST_PVS_CTX_S *ctx = reinterpret_cast<TEST_PVS_CTX_S *>(pArgs);

    if (ctx->u32TestMode == 2) {
        test_pvs_send_frame_with_vdec(ctx);
    } else {
        test_pvs_send_frame_with_file(ctx);
    }
    return RK_NULL;
}

static RK_S32 test_pvs_bind_vo(TEST_PVS_CTX_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    TEST_VO_CFG_S stVoCfg;

    memset(&stVoCfg, 0, sizeof(TEST_VO_CFG_S));
    stVoCfg.enIntfType = VO_INTF_HDMI;
    stVoCfg.enIntfSync = VO_OUTPUT_1080P60;
    stVoCfg.enLayerPixFmt = RK_FMT_RGB888;
    stVoCfg.u32VoDev = 0;
    stVoCfg.u32VoLayer = RK356X_VOP_LAYER_CLUSTER0;
    stVoCfg.u32LayerWidth = 1920;
    stVoCfg.u32LayerHeight = 1080;
    stVoCfg.u32LayerRCNum = 1;
    TEST_COMM_APP_VO_Start(&stVoCfg);

    stSrcChn.enModId = RK_ID_PVS;
    stSrcChn.s32DevId = ctx->s32DevId;
    stSrcChn.s32ChnId = 0;

    stDestChn.enModId = RK_ID_VO;
    stDestChn.s32DevId = stVoCfg.u32VoDev;
    stDestChn.s32ChnId = stVoCfg.u32VoLayer;

    s32Ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed with %#x!", s32Ret);
        return RK_FAILURE;
    }

    return s32Ret;
}

static RK_S32 test_pvs_unbind_vo(TEST_PVS_CTX_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    TEST_VO_CFG_S stVoCfg;

    memset(&stVoCfg, 0, sizeof(TEST_VO_CFG_S));
    stVoCfg.u32VoDev = 0;
    stVoCfg.u32VoLayer = RK356X_VOP_LAYER_CLUSTER0;

    stSrcChn.enModId = RK_ID_PVS;
    stSrcChn.s32DevId = ctx->s32DevId;
    stSrcChn.s32ChnId = 0;

    stDestChn.enModId = RK_ID_VO;
    stDestChn.s32DevId = stVoCfg.u32VoDev;
    stDestChn.s32ChnId = stVoCfg.u32VoLayer;

    s32Ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed with %#x!", s32Ret);
    }

    TEST_COMM_APP_VO_Stop(&stVoCfg);

    return s32Ret;
}

static void *test_pvs_get_frame(void *pArgs) {
    TEST_PVS_CTX_S *ctx = reinterpret_cast<TEST_PVS_CTX_S *>(pArgs);
    VIDEO_FRAME_INFO_S stDstFrameInfo;
    RK_S32 s32StitchFrmCnt = ctx->s32StitchFrmCnt;
    RK_S32 s32Ret = RK_SUCCESS;
    RK_S32 s32MilliSec = 30;
    FILE *fpOut = RK_NULL;

    if (ctx->pDstFilePath != RK_NULL) {
        fpOut = fopen(ctx->pDstFilePath, "w");
    }

    do {
        s32Ret = RK_MPI_PVS_GetFrame(ctx->s32DevId, &stDstFrameInfo, s32MilliSec);
        if (RK_SUCCESS != s32Ret) {
            continue;
        }

        TEST_COMM_DumpFrame2File(&stDstFrameInfo, fpOut);
        s32Ret = RK_MPI_PVS_ReleaseFrame(&stDstFrameInfo);
        s32StitchFrmCnt--;
    } while (s32StitchFrmCnt);

    if (fpOut != RK_NULL)
        fclose(fpOut);

    return RK_NULL;
}

static RK_S32 test_pvs_dev_start(TEST_PVS_CTX_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;

    s32Ret = RK_MPI_PVS_SetVProcDev(ctx->s32DevId, ctx->enVProcDev);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("set proc dev %d failed", ctx->s32DevId);
    }
    s32Ret = RK_MPI_PVS_SetDevAttr(ctx->s32DevId, &ctx->stDevAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("set dev %d attr failed", ctx->s32DevId);
        return s32Ret;
    }
    s32Ret = RK_MPI_PVS_EnableDev(ctx->s32DevId);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("enable dev %d failed", ctx->s32DevId);
        return s32Ret;
    }

    return s32Ret;
}

static RK_S32 test_pvs_chn_start(TEST_PVS_CTX_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    RK_U32 u32RcNum = ctx->u32RCNum;
    PVS_CHN_ATTR_S stChnAttr;
    PVS_CHN_PARAM_S stChnParam;
    RK_U32 u32Width = ctx->stDevAttr.stSize.u32Width / u32RcNum;
    RK_U32 u32Height = ctx->stDevAttr.stSize.u32Height / u32RcNum;
    RK_S32 s32ChnId = 0;

    memset(&stChnParam, 0, sizeof(PVS_CHN_PARAM_S));
    stChnParam.enStitchMod = (PVS_STITCH_MODE_E)ctx->s32StitchMode;
    stChnParam.s32ChnFrmRate = ctx->s32SrcFrameRate;
    stChnParam.s32RecvThreshold = ctx->s32RecvThreshold;
    for (RK_S32 j = 0; j < u32RcNum; j++) {
        for (RK_S32 i = 0; i < u32RcNum; i++) {
            s32ChnId = i + j * u32RcNum;

            memset(&stChnAttr, 0, sizeof(PVS_CHN_ATTR_S));
            stChnAttr.stRect.s32X = i * u32Width;
            stChnAttr.stRect.s32Y = j * u32Height;
            stChnAttr.stRect.u32Width = u32Width;
            stChnAttr.stRect.u32Height = u32Height;

            s32Ret = RK_MPI_PVS_SetChnAttr(ctx->s32DevId, s32ChnId, &stChnAttr);
            if (s32Ret != RK_SUCCESS) {
                RK_LOGE("failed to set chn %d attr", s32ChnId);
            }
            s32Ret = RK_MPI_PVS_SetChnParam(ctx->s32DevId, s32ChnId, &stChnParam);
            if (s32Ret != RK_SUCCESS) {
                RK_LOGE("failed to set chn %d param", s32ChnId);
            }
            s32Ret = RK_MPI_PVS_EnableChn(ctx->s32DevId, s32ChnId);
            if (s32Ret != RK_SUCCESS) {
                RK_LOGE("enable channel %d failed", s32ChnId);
                return s32Ret;
            }
        }
    }

    return s32Ret;
}

static RK_S32 test_pvs_start(TEST_PVS_CTX_S *ctx) {
    RK_U32 totalChn = ctx->u32TotalChn;
    TEST_PVS_CTX_S stPvsCtx[PVS_MAX_CHN_NUM];

    test_pvs_dev_start(ctx);
    test_pvs_chn_start(ctx);

    ctx->pSendFrameThreads = reinterpret_cast<pthread_t *>(calloc(totalChn, sizeof(pthread_t)));
    for (PVS_CHN i = 0; i < totalChn; i++) {
        memcpy(&stPvsCtx[i], ctx, sizeof(TEST_PVS_CTX_S));
        stPvsCtx[i].s32ChnId = i;
        pthread_create(&(ctx->pSendFrameThreads[i]), 0, test_pvs_send_frame, reinterpret_cast<void *>(&stPvsCtx[i]));
    }

    return RK_SUCCESS;
}

static void test_pvs_stop(TEST_PVS_CTX_S *ctx) {
    RK_LOGE("test_pvs_stop");
    for (PVS_CHN i = 0; i < ctx->u32TotalChn; i++) {
        if (ctx->pSendFrameThreads[i])
            pthread_join(ctx->pSendFrameThreads[i], RK_NULL);
        RK_MPI_PVS_DisableChn(ctx->s32DevId, i);
    }
    RK_LOGE("test_pvs_stop dev");
    if (ctx->pSendFrameThreads != RK_NULL) {
        free(ctx->pSendFrameThreads);
        ctx->pSendFrameThreads = RK_NULL;
    }

    RK_MPI_PVS_DisableDev(ctx->s32DevId);
    RK_LOGE("test_pvs_stop dev ok");
}

static void test_pvs_proc(TEST_PVS_CTX_S *ctx) {
    pthread_t getFrameThread;

    ctx->u32TotalChn = ctx->u32RCNum * ctx->u32RCNum;
    test_pvs_start(ctx);

    if (ctx->u32TestMode >= 1) {
        test_pvs_bind_vo(ctx);
    } else {
        pthread_create(&getFrameThread, 0, test_pvs_get_frame, reinterpret_cast<void *>(ctx));
        pthread_join(getFrameThread, RK_NULL);
    }

    test_pvs_stop(ctx);
    if (ctx->u32TestMode >= 1) {
        test_pvs_unbind_vo(ctx);
    }
}

static const char *const usages[] = {
    "./rk_mpi_pvs_test [-i X] [--src_w X] [--src_h X]...",
    NULL,
};

int main(int argc, const char **argv) {
    TEST_PVS_CTX_S ctx;

    memset(&ctx, 0, sizeof(TEST_PVS_CTX_S));
    ctx.u32RCNum = 1;
    ctx.s32LoopCount = 1;
    ctx.s32DevId = 0;
    ctx.s32SrcFrameRate = 30;
    ctx.s32StitchFrmCnt = 30;
    ctx.s32RecvThreshold = 2;
    ctx.stDevAttr.s32StitchFrmRt = 30;
    ctx.stDevAttr.stSize.u32Width = 1920;
    ctx.stDevAttr.stSize.u32Height = 1080;

    struct argparse_option options[] {
        OPT_HELP(),
        OPT_GROUP("basic options:"),
        OPT_STRING('i', "input", &(ctx.pSrcFilePath),
                   "input file path. <required>", NULL, 0, 0),
        OPT_STRING('o', "output", &(ctx.pDstFilePath),
                   "output file path", NULL, 0, 0),
        OPT_INTEGER('n', "loop_cnt", &(ctx.s32LoopCount),
                    "input loop count, default(1)", NULL, 0, 0),
        OPT_INTEGER('r', "rc_num", &(ctx.u32RCNum),
                    "input row/column num, default(1)", NULL, 0, 0),
        OPT_INTEGER('\0', "src_w", &(ctx.u32SrcWidth),
                    "input source width. <required>", NULL, 0, 0),
        OPT_INTEGER('\0', "src_h", &(ctx.u32SrcHeight),
                    "input source height. <required>", NULL, 0, 0),
        OPT_INTEGER('\0', "src_comp_mode", &(ctx.enSrcCompressMode),
                    "pvs source compress mode, default(0); 0: NONE, 1: AFBC_16X16", NULL, 0, 0),
        OPT_INTEGER('\0', "src_pix_fmt", &(ctx.enSrcPixelFormat),
                    "pvs source pixel format, default(0); 0: YUV420SP", NULL, 0, 0),
        OPT_INTEGER('\0', "src_frm_rate", &(ctx.s32SrcFrameRate),
                    "pvs source frame rate, default(30)", NULL, 0, 0),
        OPT_INTEGER('\0', "proc_dev", &(ctx.enVProcDev),
                    "pvs process device, default(0); 0: GPU, 1: RGA", NULL, 0, 0),
        OPT_INTEGER('m', "stitch_mode", &(ctx.s32StitchMode),
                    "pvs channel get frame mode, default(0); 0: PREVIEW, 1: PLAYBACK", NULL, 0, 0),
        OPT_INTEGER('\0', "stitch_frm_cnt", &(ctx.s32StitchFrmCnt),
                    "pvs stitch total frame count, default(30)", NULL, 0, 0),
        OPT_INTEGER('\0', "stitch_frm_rate", &(ctx.stDevAttr.s32StitchFrmRt),
                    "pvs stitching frame rate, default(30)", NULL, 0, 0),
        OPT_INTEGER('w', "dst_w", &(ctx.stDevAttr.stSize.u32Width),
                    "output destination width, default(1920)", NULL, 0, 0),
        OPT_INTEGER('h', "dst_h", &(ctx.stDevAttr.stSize.u32Height),
                    "output destination height, default(1080)", NULL, 0, 0),
        OPT_INTEGER('\0', "dst_comp_mode", &(ctx.stDevAttr.enCompMode),
                    "pvs destination compress mode, default(0); 0: NONE, 1: AFBC_16X16", NULL, 0, 0),
        OPT_INTEGER('\0', "dst_pix_fmt", &(ctx.stDevAttr.enPixelFormat),
                    "pvs destination pixel format, default(0); 0: YUV420SP, 65542: RGB888", NULL, 0, 0),
        OPT_INTEGER('t', "test_mode", &(ctx.u32TestMode),
                    "pvs test mode. default(0); 0: Pvs only, 1: Pvs->Vo, 2: Vdec->Pvs->Vo", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nselect a test case to run.",
                                 "\nuse --help for details.");

    argc = argparse_parse(&argparse, argc, argv);

    if (RK_MPI_SYS_Init() != RK_SUCCESS) {
        goto __FAILED;
    }

    test_pvs_proc(&ctx);

    if (RK_MPI_SYS_Exit() != RK_SUCCESS) {
        goto __FAILED;
    }

    RK_LOGI("test running success");
    return RK_SUCCESS;
__FAILED:
    RK_LOGE("test running failed!");
    return RK_FAILURE;
}
