/*
 * Copyright 2020 Rockchip Electronics Co. LTD
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

#include <string>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>

#include "rk_defines.h"
#include "rk_debug.h"
#include "rk_mpi_cal.h"
#include "rk_mpi_mb.h"
#include "rk_mpi_mmz.h"
#include "rk_mpi_sys.h"
#include "rk_mpi_vi.h"
#include "rk_mpi_venc.h"
#include "rk_mpi_vo.h"

#include "rk_common.h"
#include "rk_comm_venc.h"
#include "rk_comm_vi.h"
#include "rk_comm_vo.h"

#include "test_common.h"
#include "test_comm_utils.h"
#include "test_comm_sys.h"
#include "test_comm_argparse.h"

#define TEST_VI_SENSOR_NUM 4

/* for RK3588 */
#define RK3588_VO_DEV_HDMI         0
#define RK3588_VO_DEV_MIPI         3

static RK_BOOL bExit = RK_FALSE;

typedef enum _rkTEST_DIS_MODE_E {
    TEST_DIS_MODE_VI_ONLY   = 0,
    TEST_DIS_MODE_BIND_VO   = 1,
    TEST_DIS_MODE_BIND_VENC = 2
} TEST_DIS_MODE_E;

typedef enum _rkTEST_DIS_CHANGE_MODE_E {
    TEST_DIS_X_NONE             = 0,
    TEST_DIS_X_FUNC_SWITCH   = 1,
    TEST_DIS_X_CROP_RATIO    = 2,
    TEST_DIS_X_STILL_CROP    = 3
} TEST_DIS_CHANGE_MODE_E;

typedef struct _rkTEST_DIS_CTX_S {
    RK_S32          s32LoopCount;
    TEST_DIS_MODE_E enTestMode;
    RK_U32          u32Width;
    RK_U32          u32Height;
    PIXEL_FORMAT_E  enPixelFormat;
    COMPRESS_MODE_E enCompressMode;
    RK_U32          u32ViCnt;
    RK_U32          u32ViBufCnt;
    RK_U32          u32DisBufCnt;
    RK_BOOL         bUserPicEnabled;
    const RK_CHAR  *pUserPicFile;
    RK_U32          u32CropRatio;
    const RK_CHAR  *pDstFilePath;
    TEST_DIS_CHANGE_MODE_E enChangeMode;
} TEST_DIS_CTX_S;

typedef struct rkVI_CFG_S {
    VI_DEV              s32DevId;
    VI_CHN              s32ChnId;
    VI_PIPE             s32PipeId;
    VI_DEV_ATTR_S       stViDevAttr;
    VI_CHN_STATUS_S     stViChnStatus;
    VI_CHN_ATTR_S       stViChnAttr;
    VI_DEV_BIND_PIPE_S  stBindPipe;
    RK_BOOL             bUserPicEnabled;
    const RK_CHAR      *pUserPicFile;
    VI_USERPIC_ATTR_S   stUsrPic;
    RK_S32              s32LoopCount;
    RK_BOOL             bThreadExit;
    const RK_CHAR      *pDstFilePath;
    TEST_DIS_CHANGE_MODE_E enChangeMode;
} VI_CFG_S;

typedef struct _rkVO_CFG_S {
    VO_DEV                s32DevId;
    VO_CHN                s32ChnId;
    VO_LAYER              s32LayerId;
    VO_VIDEO_LAYER_ATTR_S stVoLayerAttr;
    VO_CHN_ATTR_S         stVoChnAttr;
    VO_CSC_S              stVoCscAttr;
} VO_CFG_S;

typedef struct rkVENC_CFG_S {
    VENC_CHN VencChn;
    VENC_CHN_ATTR_S stAttr;
} VENC_CFG_S;

static RK_S32 readFromPic(VI_CFG_S *ctx, VIDEO_FRAME_S *buffer) {
    FILE           *fp    = NULL;
    RK_S32          s32Ret = RK_SUCCESS;
    MB_BLK          srcBlk = MB_INVALID_HANDLE;
    PIC_BUF_ATTR_S  stPicBufAttr;
    MB_PIC_CAL_S    stMbPicCalResult;

    if (!ctx->pUserPicFile) {
        return RK_ERR_NULL_PTR;
    }

    stPicBufAttr.u32Width      = ctx->stViChnAttr.stSize.u32Width;
    stPicBufAttr.u32Height     = ctx->stViChnAttr.stSize.u32Height;
    stPicBufAttr.enCompMode    = COMPRESS_MODE_NONE;
    stPicBufAttr.enPixelFormat = RK_FMT_YUV420SP;
    s32Ret = RK_MPI_CAL_VGS_GetPicBufferSize(&stPicBufAttr, &stMbPicCalResult);
    if (RK_SUCCESS != s32Ret) {
        RK_LOGE("get user picture buffer size failed %#x!", s32Ret);
        return RK_NULL;
    }

    s32Ret = RK_MPI_MMZ_Alloc(&srcBlk, stMbPicCalResult.u32MBSize, RK_MMZ_ALLOC_CACHEABLE);
    if (RK_SUCCESS != s32Ret) {
        RK_LOGE("all user picture buffer failed %#x!", s32Ret);
        return RK_NULL;
    }

    fp = fopen(ctx->pUserPicFile, "rb");
    if (NULL == fp) {
        RK_LOGE("open %s fail", ctx->pUserPicFile);
        goto __FREE_USER_PIC;
    } else {
        fread(RK_MPI_MB_Handle2VirAddr(srcBlk), 1 , stMbPicCalResult.u32MBSize, fp);
        fclose(fp);
        RK_MPI_SYS_MmzFlushCache(srcBlk, RK_FALSE);
        RK_LOGD("open %s success", ctx->pUserPicFile);
    }

    buffer->u32Width       = ctx->stViChnAttr.stSize.u32Width;
    buffer->u32Height      = ctx->stViChnAttr.stSize.u32Height;
    buffer->u32VirWidth    = ctx->stViChnAttr.stSize.u32Width;
    buffer->u32VirHeight   = ctx->stViChnAttr.stSize.u32Height;
    buffer->enPixelFormat  = RK_FMT_YUV420SP;
    buffer->u32TimeRef     = 0;
    buffer->u64PTS         = 0;
    buffer->enCompressMode = COMPRESS_MODE_NONE;
    buffer->pMbBlk         = srcBlk;

    RK_LOGD("readFromPic width = %d, height = %d size = %d pixFormat = %d",
            ctx->stViChnAttr.stSize.u32Width, ctx->stViChnAttr.stSize.u32Height,
            stMbPicCalResult.u32MBSize, RK_FMT_YUV420SP);
    return RK_SUCCESS;

__FREE_USER_PIC:
    RK_MPI_SYS_MmzFree(srcBlk);

    return RK_NULL;
}

static RK_S32 create_vi(VI_CFG_S *ctx) {
    RK_S32 s32Ret = RK_FAILURE;
    /* 0. get dev config status */
    s32Ret = RK_MPI_VI_GetDevAttr(ctx->s32DevId, &ctx->stViDevAttr);
    if (s32Ret == RK_ERR_VI_NOT_CONFIG) {
        /* 0-1.config dev */
        s32Ret = RK_MPI_VI_SetDevAttr(ctx->s32DevId, &ctx->stViDevAttr);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d] RK_MPI_VI_SetDevAttr failed: %#x!",
                    ctx->s32DevId, ctx->s32ChnId, s32Ret);
            goto __FAILED;
        }
    }
    RK_LOGV("vi [%d, %d] RK_MPI_VI_SetDevAttr already.",
            ctx->s32DevId, ctx->s32ChnId);
    /* 1.get  dev enable status */
    s32Ret = RK_MPI_VI_GetDevIsEnable(ctx->s32DevId);
    if (s32Ret != RK_SUCCESS) {
        /* 1-2.enable dev */
        s32Ret = RK_MPI_VI_EnableDev(ctx->s32DevId);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d] RK_MPI_VI_EnableDev failed: %#x!",
                    ctx->s32DevId, ctx->s32ChnId, s32Ret);
            goto __FAILED;
        }
        RK_LOGV("vi [%d, %d] RK_MPI_VI_EnableDev already.",
                ctx->s32DevId, ctx->s32ChnId);

        /* 1-3.bind dev/pipe */
        ctx->stBindPipe.u32Num = ctx->s32PipeId;
        ctx->stBindPipe.PipeId[0] = ctx->s32PipeId;
        s32Ret = RK_MPI_VI_SetDevBindPipe(ctx->s32DevId, &ctx->stBindPipe);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d] RK_MPI_VI_SetDevBindPipe failed: %#x!",
                    ctx->s32DevId, ctx->s32ChnId, s32Ret);
            goto __FAILED;
        }
        RK_LOGV("vi [%d, %d] RK_MPI_VI_SetDevBindPipe already.",
                ctx->s32DevId, ctx->s32ChnId);
    }
    RK_LOGV("vi [%d, %d] RK_MPI_VI_GetDevIsEnable already.",
            ctx->s32DevId, ctx->s32ChnId);
    /* 2.config channel */
    s32Ret = RK_MPI_VI_SetChnAttr(ctx->s32PipeId, ctx->s32ChnId, &ctx->stViChnAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vi [%d, %d, %d] RK_MPI_VI_SetChnAttr failed: %#x!",
                    ctx->s32DevId, ctx->s32PipeId, ctx->s32ChnId, s32Ret);
        goto __FAILED;
    }
    RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_SetChnAttr already.",
            ctx->s32DevId, ctx->s32PipeId, ctx->s32ChnId);

    if (ctx->bUserPicEnabled) {
        ctx->stUsrPic.enUsrPicMode = VI_USERPIC_MODE_PIC;

        if (ctx->stUsrPic.enUsrPicMode == VI_USERPIC_MODE_PIC) {
            s32Ret = readFromPic(ctx, &ctx->stUsrPic.unUsrPic.stUsrPicFrm.stVFrame);
            if (s32Ret != RK_SUCCESS) {
                RK_LOGE("vi [%d, %d] readFromPic fail:%x",
                        ctx->s32DevId, ctx->s32ChnId, s32Ret);
                goto __FAILED;
            }
        } else if (ctx->stUsrPic.enUsrPicMode == VI_USERPIC_MODE_BGC) {
            /* set background color */
            ctx->stUsrPic.unUsrPic.stUsrPicBg.u32BgColor = RGB(0, 0, 128);
        }

        s32Ret = RK_MPI_VI_SetUserPic(ctx->s32PipeId, ctx->s32ChnId, &ctx->stUsrPic);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d, %d] RK_MPI_VI_SetUserPic failed: %#x!",
                    ctx->s32DevId, ctx->s32PipeId, ctx->s32ChnId, s32Ret);
            goto __FREE_USER_PIC;
        }
        RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_SetUserPic already.",
            ctx->s32DevId, ctx->s32PipeId, ctx->s32ChnId);

        s32Ret = RK_MPI_VI_EnableUserPic(ctx->s32PipeId, ctx->s32ChnId);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d, %d] RK_MPI_VI_EnableUserPic failed: %#x!",
                    ctx->s32DevId, ctx->s32PipeId, ctx->s32ChnId, s32Ret);
            goto __FREE_USER_PIC;
        }
        RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_EnableUserPic already.",
            ctx->s32DevId, ctx->s32PipeId, ctx->s32ChnId);
    }

    /* 3.enable channel */
    s32Ret = RK_MPI_VI_EnableChn(ctx->s32PipeId, ctx->s32ChnId);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vi [%d, %d, %d] RK_MPI_VI_EnableChn failed: %#x!",
                    ctx->s32DevId, ctx->s32PipeId, ctx->s32ChnId, s32Ret);
        goto __FAILED;
    }
    RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_EnableChn already.",
            ctx->s32DevId, ctx->s32PipeId, ctx->s32ChnId);

__FREE_USER_PIC:
    if (ctx->bUserPicEnabled &&
        VI_USERPIC_MODE_PIC == ctx->stUsrPic.enUsrPicMode &&
        ctx->stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.pMbBlk) {
        RK_MPI_MMZ_Free(ctx->stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.pMbBlk);
        ctx->stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.pMbBlk = RK_NULL;
    }

__FAILED:

    return s32Ret;
}

static RK_S32 destroy_vi(VI_CFG_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    s32Ret = RK_MPI_VI_DisableChn(ctx->s32PipeId, ctx->s32ChnId);
    if (RK_SUCCESS != s32Ret) {
        RK_LOGE("RK_MPI_VI_DisableChn failed: %#x!",
                ctx->s32DevId, ctx->s32ChnId, s32Ret);
        goto __FAILED;
    } else {
        RK_LOGV("vi [%d, %d] RK_MPI_VI_DisableChn already.",
                ctx->s32DevId, ctx->s32ChnId);
    }
    s32Ret = RK_MPI_VI_DisableDev(ctx->s32DevId);
    if (RK_SUCCESS != s32Ret) {
        RK_LOGE("RK_MPI_VI_DisableDev failed: %#x!",
                ctx->s32DevId, ctx->s32ChnId, s32Ret);
        goto __FAILED;
    } else {
        RK_LOGV("vi [%d, %d] RK_MPI_VI_DisableDev already.",
                ctx->s32DevId, ctx->s32ChnId);
    }

__FAILED:

    if (ctx->bUserPicEnabled &&
        VI_USERPIC_MODE_PIC == ctx->stUsrPic.enUsrPicMode &&
        ctx->stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.pMbBlk) {
        RK_MPI_MMZ_Free(ctx->stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.pMbBlk);
        ctx->stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.pMbBlk = RK_NULL;
    }

    return s32Ret;
}

static RK_S32 create_venc(VENC_CFG_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    VENC_RECV_PIC_PARAM_S   stRecvParam;

    memset(&stRecvParam, 0, sizeof(VENC_RECV_PIC_PARAM_S));
    stRecvParam.s32RecvPicNum = -1;

    s32Ret = RK_MPI_VENC_CreateChn(ctx->VencChn, &ctx->stAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("venc [%d] RK_MPI_VENC_CreateChn failed: %#x!",
                ctx->VencChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("venc [%d] RK_MPI_VENC_CreateChn already.", ctx->VencChn);

    s32Ret = RK_MPI_VENC_StartRecvFrame(ctx->VencChn, &stRecvParam);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("venc [%d] RK_MPI_VENC_StartRecvFrame failed: %#x!",
                ctx->VencChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("venc [%d] RK_MPI_VENC_StartRecvFrame already.", ctx->VencChn);

    return RK_SUCCESS;
}

static RK_S32 destroy_venc(VENC_CFG_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;

    s32Ret = RK_MPI_VENC_StopRecvFrame(ctx->VencChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("venc [%d] RK_MPI_VENC_StopRecvFrame failed: %#x!",
                ctx->VencChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("venc [%d] RK_MPI_VENC_StopRecvFrame already.", ctx->VencChn);

    s32Ret = RK_MPI_VENC_DestroyChn(ctx->VencChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("venc [%d] RK_MPI_VENC_DestroyChn failed: %#x!",
                ctx->VencChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("venc [%d] RK_MPI_VENC_DestroyChn already.", ctx->VencChn);

    return RK_SUCCESS;
}

static RK_S32 create_vo(VO_CFG_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    RK_U32 u32DispBufLen;
    VO_PUB_ATTR_S VoPubAttr;
    VO_LAYER VoLayer = ctx->s32LayerId;
    VO_DEV VoDev = ctx->s32DevId;
    VO_CHN VoChn = ctx->s32ChnId;

    memset(&VoPubAttr, 0, sizeof(VO_PUB_ATTR_S));

    s32Ret = RK_MPI_VO_GetPubAttr(VoDev, &VoPubAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_GetPubAttr failed: %#x!",
                VoDev, VoLayer, VoChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_GetPubAttr already.",
            VoDev, VoLayer, VoChn);

    if (RK3588_VO_DEV_HDMI == VoDev) {
        VoPubAttr.enIntfType = VO_INTF_HDMI;
        VoPubAttr.enIntfSync = VO_OUTPUT_1080P60;
    } else if (RK3588_VO_DEV_MIPI == VoDev) {
        VoPubAttr.enIntfType = VO_INTF_MIPI;
        VoPubAttr.enIntfSync = VO_OUTPUT_DEFAULT;
    }

    s32Ret = RK_MPI_VO_SetPubAttr(VoDev, &VoPubAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_GetPubAttr failed: %#x!",
                VoDev, VoLayer, VoChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_SetPubAttr already.",
            VoDev, VoLayer, VoChn);

    s32Ret = RK_MPI_VO_Enable(VoDev);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_Enable failed: %#x!",
                VoDev, VoLayer, VoChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_Enable already.",
            VoDev, VoLayer, VoChn);

    s32Ret = RK_MPI_VO_GetLayerDispBufLen(VoLayer, &u32DispBufLen);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_GetLayerDispBufLen failed: %#x!",
                VoDev, VoLayer, VoChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_GetLayerDispBufLen already.",
            VoDev, VoLayer, VoChn);

    u32DispBufLen = 3;
    s32Ret = RK_MPI_VO_SetLayerDispBufLen(VoLayer, u32DispBufLen);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_GetLayerDispBufLen %d failed: %#x!",
                VoDev, VoLayer, VoChn, u32DispBufLen, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_GetLayerDispBufLen %d already.",
            VoDev, VoLayer, VoChn, u32DispBufLen);

    s32Ret = RK_MPI_VO_BindLayer(VoLayer, VoDev, VO_LAYER_MODE_GRAPHIC);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_BindLayer failed: %#x!",
                VoDev, VoLayer, VoChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_BindLayer already.",
            VoDev, VoLayer, VoChn);

    s32Ret = RK_MPI_VO_SetLayerAttr(VoLayer, &ctx->stVoLayerAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_SetLayerAttr failed: %#x!",
                VoDev, VoLayer, VoChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_SetLayerAttr already.",
            VoDev, VoLayer, VoChn);

#if VO_RGA
    s32Ret = RK_MPI_VO_SetLayerSpliceMode(VoLayer, VO_SPLICE_MODE_RGA);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("RK_MPI_VO_SetLayerSpliceMode failed: %#x", s32Ret);
        return RK_FAILURE;
    }
#endif

    s32Ret = RK_MPI_VO_EnableLayer(VoLayer);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_EnableLayer failed: %#x!",
                VoDev, VoLayer, VoChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_EnableLayer already.",
            VoDev, VoLayer, VoChn);

    s32Ret = RK_MPI_VO_SetChnAttr(VoLayer, VoChn, &ctx->stVoChnAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_SetChnAttr failed: %#x!",
                VoDev, VoLayer, VoChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_SetChnAttr already.",
            VoDev, VoLayer, VoChn);

    s32Ret = RK_MPI_VO_SetLayerCSC(VoLayer, &ctx->stVoCscAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_SetChnAttr failed: %#x!",
                VoDev, VoLayer, VoChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_SetChnAttr already.",
            VoDev, VoLayer, VoChn);

    s32Ret = RK_MPI_VO_EnableChn(VoLayer, VoChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_EnableChn failed: %#x!",
                VoDev, VoLayer, VoChn, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_EnableChn already.",
            VoDev, VoLayer, VoChn);

    return s32Ret;
}

static RK_S32 destroy_vo(VO_CFG_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    VO_LAYER VoLayer = ctx->s32LayerId;
    VO_DEV VoDev = ctx->s32DevId;
    VO_CHN VoChn = ctx->s32ChnId;

    s32Ret = RK_MPI_VO_DisableChn(VoDev, VoChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_DisableChn failed: %#x!",
                VoDev, VoLayer, VoChn);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_DisableChn already.",
            VoDev, VoLayer, VoChn);

    s32Ret = RK_MPI_VO_DisableLayer(VoLayer);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_DisableLayer failed: %#x!",
                VoDev, VoLayer, VoChn);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_DisableLayer already.",
            VoDev, VoLayer, VoChn);

    s32Ret = RK_MPI_VO_Disable(VoDev);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_Disable failed: %#x!",
                VoDev, VoLayer, VoChn);
        return s32Ret;
    }
    RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_Disable already.",
            VoDev, VoLayer, VoChn);

    return s32Ret;
}

static RK_VOID *vi_get_frame(void *pArgs) {
    VI_CFG_S        *pstCtx     = reinterpret_cast<VI_CFG_S *>(pArgs);
    RK_S32           s32Ret     = RK_SUCCESS;
    char             cWritePath[256] = {0};
    VIDEO_FRAME_INFO_S stViFrame;

    memset(&stViFrame, 0, sizeof(VIDEO_FRAME_INFO_S));

    for (RK_S32 loopCount = 0; loopCount < pstCtx->s32LoopCount; loopCount++) {
        s32Ret = RK_MPI_VI_GetChnFrame(pstCtx->s32PipeId, pstCtx->s32ChnId, &stViFrame, -1);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d, %d] RK_MPI_VI_GetChnFrame loopCount %d failed: %#x!",
                    pstCtx->s32DevId, pstCtx->s32PipeId, pstCtx->s32ChnId, loopCount, s32Ret);
        }
        if (pstCtx->pDstFilePath) {
            snprintf(cWritePath, sizeof(cWritePath), "%sres_vi_%dx%d_%d_%d_%d.bin",
                        pstCtx->pDstFilePath, stViFrame.stVFrame.u32VirWidth,
                        stViFrame.stVFrame.u32VirHeight, pstCtx->s32DevId,
                        pstCtx->s32PipeId, pstCtx->s32ChnId, loopCount);
            s32Ret = TEST_COMM_FileWriteOneFrame(cWritePath, &stViFrame);
            if (s32Ret != RK_SUCCESS) {
                RK_LOGE("vi [%d, %d, %d] TEST_COMM_FileWriteOneFrame loopCount %d failed: %#x!",
                        pstCtx->s32DevId, pstCtx->s32PipeId, pstCtx->s32ChnId, loopCount, s32Ret);
                break;
            }
        }

        s32Ret = RK_MPI_VI_ReleaseChnFrame(pstCtx->s32PipeId, pstCtx->s32ChnId, &stViFrame);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d, %d] RK_MPI_VI_ReleaseChnFrame loopCount %d failed: %#x!",
                    pstCtx->s32DevId, pstCtx->s32PipeId, pstCtx->s32ChnId, loopCount, s32Ret);
            break;
        }
    }

    return RK_NULL;
}

static RK_S32 test_vi_get_release_frame_loop(TEST_DIS_CTX_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    VI_CFG_S *pstViCtx = reinterpret_cast<VI_CFG_S *>(malloc(
                            sizeof(VI_CFG_S) * ctx->u32ViCnt));
    DIS_CONFIG_S pstDisConfig;
    DIS_ATTR_S   pstDisAttr;
    pthread_t    viThread[VI_MAX_CHN_NUM] = {0};

    memset(pstViCtx, 0, sizeof(VI_CFG_S) * ctx->u32ViCnt);
    memset(&pstDisConfig, 0, sizeof(DIS_CONFIG_S));
    memset(&pstDisAttr, 0, sizeof(DIS_ATTR_S));

    for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
        /* vi config init */
        pstViCtx[i].s32DevId = i;
        pstViCtx[i].s32PipeId = pstViCtx[i].s32DevId;

        /* only support RK3588 */
        if (ctx->enCompressMode == COMPRESS_MODE_NONE) {
            pstViCtx[i].s32ChnId = 0;      // main path
        } else if (ctx->enCompressMode == COMPRESS_AFBC_16x16) {
            pstViCtx[i].s32ChnId = 2;      // fbc path
        }

        pstViCtx[i].stViChnAttr.stSize.u32Width = ctx->u32Width;
        pstViCtx[i].stViChnAttr.stSize.u32Height = ctx->u32Height;

        pstViCtx[i].stViChnAttr.stIspOpt.enMemoryType       = VI_V4L2_MEMORY_TYPE_DMABUF;
        pstViCtx[i].stViChnAttr.stIspOpt.u32BufCount        = ctx->u32ViBufCnt;
        pstViCtx[i].stViChnAttr.u32Depth                    = 2;
        pstViCtx[i].stViChnAttr.enPixelFormat               = ctx->enPixelFormat;
        pstViCtx[i].stViChnAttr.enCompressMode              = ctx->enCompressMode;
        pstViCtx[i].stViChnAttr.stFrameRate.s32SrcFrameRate = -1;
        pstViCtx[i].stViChnAttr.stFrameRate.s32DstFrameRate = -1;

        if (ctx->bUserPicEnabled) {
            pstViCtx[i].bUserPicEnabled = ctx->bUserPicEnabled;
            pstViCtx[i].pUserPicFile    = ctx->pUserPicFile;
        }
        pstViCtx[i].s32LoopCount = ctx->s32LoopCount;
        pstViCtx[i].pDstFilePath = ctx->pDstFilePath;

        /* vi create */
        s32Ret = create_vi(&pstViCtx[i]);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d,%d] create failed: %#x!",
                    pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                    pstViCtx[i].s32ChnId, s32Ret);
            goto __DESTROY_VI;
        }
        RK_LOGV("vi [%d, %d, %d] create already.",
                pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                pstViCtx[i].s32ChnId);

        /* dis config init */
        pstDisConfig.enMode              = DIS_MODE_4_DOF_GME;
        pstDisConfig.enMotionLevel       = DIS_MOTION_LEVEL_NORMAL;
        pstDisConfig.u32CropRatio        = ctx->u32CropRatio;
        pstDisConfig.u32BufNum           = ctx->u32DisBufCnt;
        pstDisConfig.u32FrameRate        = 30;
        pstDisConfig.enPdtType           = DIS_PDT_TYPE_IPC;
        pstDisConfig.u32GyroOutputRange  = 0;
        pstDisConfig.bCameraSteady       = RK_FALSE;
        pstDisConfig.u32GyroDataBitWidth = 0;

        pstDisAttr.bEnable               = RK_TRUE;
        pstDisAttr.u32MovingSubjectLevel = 0;
        pstDisAttr.s32RollingShutterCoef = 0;
        pstDisAttr.u32Timelag            = 33333;
        pstDisAttr.u32ViewAngle          = 1000;
        pstDisAttr.bStillCrop            = RK_FALSE;
        pstDisAttr.u32HorizontalLimit    = 512;
        pstDisAttr.u32VerticalLimit      = 512;

        s32Ret = RK_MPI_VI_SetChnDISConfig(pstViCtx[i].s32DevId, pstViCtx[i].s32ChnId, &pstDisConfig);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d,%d] RK_MPI_VI_SetChnDISConfig failed: %#x!",
                    pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                    pstViCtx[i].s32ChnId, s32Ret);
            goto __DESTROY_VI;
        }
        RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_SetChnDISConfig already.",
                pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                pstViCtx[i].s32ChnId);

        s32Ret = RK_MPI_VI_SetChnDISAttr(pstViCtx[i].s32DevId, pstViCtx[i].s32ChnId, &pstDisAttr);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d,%d] RK_MPI_VI_SetChnDISAttr failed: %#x!",
                    pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                    pstViCtx[i].s32ChnId, s32Ret);
            goto __DESTROY_VI;
        }
        RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_SetChnDISAttr already.",
                pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                pstViCtx[i].s32ChnId);
    }

    for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
        pthread_create(&viThread[i], 0, vi_get_frame, reinterpret_cast<void *>(&pstViCtx[i]));
    }

    for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
        if (viThread[i])
            pthread_join(viThread[i], RK_NULL);
    }

__DESTROY_VI:
    /* destroy vi */
    for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
        s32Ret = destroy_vi(&pstViCtx[i]);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d, %d] destroy_vi failed: %#x!",
                    pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                    pstViCtx[i].s32ChnId, s32Ret);
        }
        RK_LOGV("vi [%d, %d, %d] destroy already.",
                pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                pstViCtx[i].s32ChnId);
    }

    RK_SAFE_FREE(pstViCtx);

    return s32Ret;
}

static RK_S32 TEST_DIS_FuncSwitch(VI_CFG_S *pstCtx) {
    RK_S32       s32Ret = RK_SUCCESS;
    DIS_ATTR_S   pstDisAttr;

    memset(&pstDisAttr, 0, sizeof(DIS_ATTR_S));
    s32Ret = RK_MPI_VI_GetChnDISAttr(pstCtx->s32DevId, pstCtx->s32ChnId, &pstDisAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vi [%d, %d,%d] RK_MPI_VI_GetChnDISAttr failed: %#x!",
                pstCtx->s32DevId, pstCtx->s32PipeId,
                pstCtx->s32ChnId, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_GetChnDISAttr already.",
            pstCtx->s32DevId, pstCtx->s32PipeId,
            pstCtx->s32ChnId);

    RK_LOGD("vi [%d, %d, %d] dis attr bEnable %d -> %d.",
            pstCtx->s32DevId, pstCtx->s32PipeId,
            pstCtx->s32ChnId, pstDisAttr.bEnable, !pstDisAttr.bEnable);

    pstDisAttr.bEnable = (RK_BOOL)!pstDisAttr.bEnable;

    s32Ret = RK_MPI_VI_SetChnDISAttr(pstCtx->s32DevId, pstCtx->s32ChnId, &pstDisAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vi [%d, %d,%d] RK_MPI_VI_SetChnDISAttr failed: %#x!",
                pstCtx->s32DevId, pstCtx->s32PipeId,
                pstCtx->s32ChnId, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_SetChnDISAttr already.",
            pstCtx->s32DevId, pstCtx->s32PipeId,
            pstCtx->s32ChnId);

    return s32Ret;
}

static RK_S32 TEST_DIS_CropRatio(VI_CFG_S *pstCtx) {
    RK_S32       s32Ret = RK_SUCCESS;
    DIS_CONFIG_S pstDisConfig;
    RK_U32       tCropRatio = 50;

    memset(&pstDisConfig, 0, sizeof(DIS_CONFIG_S));
    s32Ret = RK_MPI_VI_GetChnDISConfig(pstCtx->s32DevId, pstCtx->s32ChnId, &pstDisConfig);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vi [%d, %d,%d] RK_MPI_VI_GetChnDISConfig failed: %#x!",
                pstCtx->s32DevId, pstCtx->s32PipeId,
                pstCtx->s32ChnId, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_GetChnDISConfig already.",
            pstCtx->s32DevId, pstCtx->s32PipeId,
            pstCtx->s32ChnId);

    tCropRatio = pstDisConfig.u32CropRatio;
    pstDisConfig.u32CropRatio++;
    if (pstDisConfig.u32CropRatio > 98) {
        pstDisConfig.u32CropRatio = 50;
    }
    RK_LOGD("vi [%d, %d, %d] dis config u32CropRatio %d -> %d.",
            pstCtx->s32DevId, pstCtx->s32PipeId,
            pstCtx->s32ChnId, tCropRatio, pstDisConfig.u32CropRatio);

    s32Ret = RK_MPI_VI_SetChnDISConfig(pstCtx->s32DevId, pstCtx->s32ChnId, &pstDisConfig);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vi [%d, %d,%d] RK_MPI_VI_SetChnDISConfig failed: %#x!",
                pstCtx->s32DevId, pstCtx->s32PipeId,
                pstCtx->s32ChnId, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_SetChnDISConfig already.",
            pstCtx->s32DevId, pstCtx->s32PipeId,
            pstCtx->s32ChnId);

    return s32Ret;
}

static RK_S32 TEST_DIS_StillCrop(VI_CFG_S *pstCtx) {
    RK_S32       s32Ret = RK_SUCCESS;
    DIS_ATTR_S   pstDisAttr;

    memset(&pstDisAttr, 0, sizeof(DIS_ATTR_S));
    s32Ret = RK_MPI_VI_GetChnDISAttr(pstCtx->s32DevId, pstCtx->s32ChnId, &pstDisAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vi [%d, %d,%d] RK_MPI_VI_GetChnDISAttr failed: %#x!",
                pstCtx->s32DevId, pstCtx->s32PipeId,
                pstCtx->s32ChnId, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_GetChnDISAttr already.",
            pstCtx->s32DevId, pstCtx->s32PipeId,
            pstCtx->s32ChnId);

    RK_LOGD("vi [%d, %d, %d] dis attr bStillCrop %d -> %d.",
            pstCtx->s32DevId, pstCtx->s32PipeId,
            pstCtx->s32ChnId, pstDisAttr.bStillCrop, !pstDisAttr.bStillCrop);

    pstDisAttr.bStillCrop = (RK_BOOL)!pstDisAttr.bStillCrop;

    s32Ret = RK_MPI_VI_SetChnDISAttr(pstCtx->s32DevId, pstCtx->s32ChnId, &pstDisAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("vi [%d, %d,%d] RK_MPI_VI_SetChnDISAttr failed: %#x!",
                pstCtx->s32DevId, pstCtx->s32PipeId,
                pstCtx->s32ChnId, s32Ret);
        return s32Ret;
    }
    RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_SetChnDISAttr already.",
            pstCtx->s32DevId, pstCtx->s32PipeId,
            pstCtx->s32ChnId);

    return s32Ret;
}

static RK_VOID *vi_dis_change_loop(void *pArgs) {
    VI_CFG_S        *pstCtx     = reinterpret_cast<VI_CFG_S *>(pArgs);
    RK_S32           s32Ret     = RK_SUCCESS;
    RK_S32           s32LoopCount = 0;

    while (!bExit) {
        switch (pstCtx->enChangeMode) {
          case TEST_DIS_X_FUNC_SWITCH:
            s32Ret = TEST_DIS_FuncSwitch(pstCtx);
          break;
          case TEST_DIS_X_CROP_RATIO:
            s32Ret = TEST_DIS_CropRatio(pstCtx);
          break;
          case TEST_DIS_X_STILL_CROP:
            s32Ret = TEST_DIS_StillCrop(pstCtx);
          break;
          default:
          break;
        }
        RK_LOGD("dis change loop cnt %d", s32LoopCount++);
        usleep(100 * 1000);
    }

    return RK_NULL;
}

static RK_VOID sigterm_handler(int sig) {
    RK_PRINT("signal %d\n", sig);
    bExit = RK_TRUE;
}

static RK_S32 test_vi_bind_vo_loop(TEST_DIS_CTX_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    RK_S32 s32LoopCount = 0;
    VI_CFG_S *pstViCtx = RK_NULL;
    VO_CFG_S *pstVoCtx = RK_NULL;
    pstViCtx = reinterpret_cast<VI_CFG_S *>(malloc(sizeof(VI_CFG_S) * ctx->u32ViCnt));
    pstVoCtx = reinterpret_cast<VO_CFG_S *>(malloc(sizeof(VO_CFG_S) * ctx->u32ViCnt));
    pthread_t    viThread[VI_MAX_CHN_NUM] = {0};
    RK_S32 spiltDispWidth = 0;
    RK_S32 spiltDispHeight = 0;
    RK_S32 spiltDispColumn = 1;
    DIS_CONFIG_S pstDisConfig;
    DIS_ATTR_S   pstDisAttr;

    memset(pstViCtx, 0, sizeof(VI_CFG_S) * ctx->u32ViCnt);
    memset(pstVoCtx, 0, sizeof(VO_CFG_S) * ctx->u32ViCnt);
    memset(&pstDisConfig, 0, sizeof(DIS_CONFIG_S));
    memset(&pstDisAttr, 0, sizeof(DIS_ATTR_S));

    for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
        /* vi config init */
        pstViCtx[i].s32DevId = i;
        pstViCtx[i].s32PipeId = pstViCtx[i].s32DevId;
        pstViCtx[i].enChangeMode = ctx->enChangeMode;

        if (ctx->enCompressMode == COMPRESS_MODE_NONE) {
            pstViCtx[i].s32ChnId = 0;      // main path
        } else if (ctx->enCompressMode == COMPRESS_AFBC_16x16) {
            pstViCtx[i].s32ChnId = 2;      // fbc path
        }

        pstViCtx[i].stViChnAttr.stSize.u32Width = ctx->u32Width;
        pstViCtx[i].stViChnAttr.stSize.u32Height = ctx->u32Height;

        pstViCtx[i].stViChnAttr.stIspOpt.enMemoryType       = VI_V4L2_MEMORY_TYPE_DMABUF;
        pstViCtx[i].stViChnAttr.stIspOpt.u32BufCount        = 5;
        pstViCtx[i].stViChnAttr.u32Depth                    = 2;
        pstViCtx[i].stViChnAttr.enPixelFormat               = ctx->enPixelFormat;
        pstViCtx[i].stViChnAttr.enCompressMode              = ctx->enCompressMode;
        pstViCtx[i].stViChnAttr.stFrameRate.s32SrcFrameRate = -1;
        pstViCtx[i].stViChnAttr.stFrameRate.s32DstFrameRate = -1;

        /* vi create */
        s32Ret = create_vi(&pstViCtx[i]);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d,%d] create failed: %x",
                    pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                    pstViCtx[i].s32ChnId, s32Ret);
            goto __DESTROY_VI;
        }
        RK_LOGV("vi [%d, %d, %d] create already.",
                pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                pstViCtx[i].s32ChnId);

        /* dis config init */
        pstDisConfig.enMode              = DIS_MODE_4_DOF_GME;
        pstDisConfig.enMotionLevel       = DIS_MOTION_LEVEL_NORMAL;
        pstDisConfig.u32CropRatio        = ctx->u32CropRatio;
        pstDisConfig.u32BufNum           = ctx->u32DisBufCnt;
        pstDisConfig.u32FrameRate        = 30;
        pstDisConfig.enPdtType           = DIS_PDT_TYPE_IPC;
        pstDisConfig.u32GyroOutputRange  = 0;
        pstDisConfig.bCameraSteady       = RK_FALSE;
        pstDisConfig.u32GyroDataBitWidth = 0;

        pstDisAttr.bEnable               = RK_TRUE;
        pstDisAttr.u32MovingSubjectLevel = 0;
        pstDisAttr.s32RollingShutterCoef = 0;
        pstDisAttr.u32Timelag            = 33333;
        pstDisAttr.u32ViewAngle          = 1000;
        pstDisAttr.bStillCrop            = RK_FALSE;
        pstDisAttr.u32HorizontalLimit    = 512;
        pstDisAttr.u32VerticalLimit      = 512;

        s32Ret = RK_MPI_VI_SetChnDISConfig(pstViCtx[i].s32DevId, pstViCtx[i].s32ChnId, &pstDisConfig);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d,%d] RK_MPI_VI_SetChnDISConfig failed: %#x!",
                    pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                    pstViCtx[i].s32ChnId, s32Ret);
            goto __DESTROY_VI;
        }
        RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_SetChnDISConfig already.",
                pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                    pstViCtx[i].s32ChnId);

        s32Ret = RK_MPI_VI_SetChnDISAttr(pstViCtx[i].s32DevId, pstViCtx[i].s32ChnId, &pstDisAttr);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d,%d] RK_MPI_VI_SetChnDISAttr failed: %#x!",
                    pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                    pstViCtx[i].s32ChnId, s32Ret);
            goto __DESTROY_VI;
        }
        RK_LOGV("vi [%d, %d, %d] RK_MPI_VI_SetChnDISAttr already.",
                pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                pstViCtx[i].s32ChnId);
    }

    if (ctx->u32ViCnt <= 4) {
        spiltDispColumn = 2;
    } else if (ctx->u32ViCnt <= VI_MAX_DEV_NUM) {
        spiltDispColumn = 3;
    }
    spiltDispWidth  = 1920 / spiltDispColumn;
    spiltDispHeight = 1080 / spiltDispColumn;

    for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
    /* vo config init */
        pstVoCtx[i].s32LayerId = 0;
        pstVoCtx[i].s32DevId   = RK3588_VO_DEV_HDMI;
        pstVoCtx[i].s32ChnId   = i;

        if (RK3588_VO_DEV_HDMI == pstVoCtx[i].s32DevId) {
            pstVoCtx[i].stVoLayerAttr.stDispRect.u32Width = 1920;
            pstVoCtx[i].stVoLayerAttr.stDispRect.u32Height = 1080;
        } else if (RK3588_VO_DEV_MIPI == pstVoCtx[i].s32DevId) {
            pstVoCtx[i].stVoLayerAttr.stDispRect.u32Width = 1080;
            pstVoCtx[i].stVoLayerAttr.stDispRect.u32Height = 1920;
        }
        pstVoCtx[i].stVoLayerAttr.enPixFormat = RK_FMT_RGB888;

        pstVoCtx[i].stVoCscAttr.enCscMatrix = VO_CSC_MATRIX_IDENTITY;
        pstVoCtx[i].stVoCscAttr.u32Contrast = 50;
        pstVoCtx[i].stVoCscAttr.u32Hue = 50;
        pstVoCtx[i].stVoCscAttr.u32Luma = 50;
        pstVoCtx[i].stVoCscAttr.u32Satuature = 50;

        pstVoCtx[i].stVoLayerAttr.stDispRect.s32X = 0;
        pstVoCtx[i].stVoLayerAttr.stDispRect.s32Y = 0;
        pstVoCtx[i].stVoLayerAttr.stImageSize.u32Width =
            pstVoCtx[i].stVoLayerAttr.stDispRect.u32Width;
        pstVoCtx[i].stVoLayerAttr.stImageSize.u32Height =
            pstVoCtx[i].stVoLayerAttr.stDispRect.u32Height;
        pstVoCtx[i].stVoLayerAttr.u32DispFrmRt = 30;

        pstVoCtx[i].stVoChnAttr.stRect.s32X = spiltDispWidth * (i % spiltDispColumn);
        pstVoCtx[i].stVoChnAttr.stRect.s32Y = spiltDispHeight * (i / spiltDispColumn);
        pstVoCtx[i].stVoChnAttr.stRect.u32Width = spiltDispWidth;
        pstVoCtx[i].stVoChnAttr.stRect.u32Height = spiltDispHeight;
        pstVoCtx[i].stVoChnAttr.bDeflicker = RK_FALSE;
        pstVoCtx[i].stVoChnAttr.u32Priority = 1;
        pstVoCtx[i].stVoChnAttr.u32FgAlpha = 128;
        pstVoCtx[i].stVoChnAttr.u32BgAlpha = 0;
    /* vo create */

        if (0 == i) {
            s32Ret = create_vo(&pstVoCtx[i]);
            if (s32Ret != RK_SUCCESS) {
                RK_LOGE("vo [%d, %d, %d] create failed: %x",
                        pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId,
                        pstVoCtx[i].s32ChnId, s32Ret);
                goto __DESTROY_VO;
            }
            RK_LOGV("vo [%d, %d, %d] create already.",
                    pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId,
                    pstVoCtx[i].s32ChnId);
        } else {
            s32Ret = RK_MPI_VO_SetChnAttr(pstVoCtx[i].s32LayerId, pstVoCtx[i].s32ChnId, &pstVoCtx[i].stVoChnAttr);
            if (s32Ret != RK_SUCCESS) {
                RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_SetChnAttr failed with %#x!",
                        pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId,
                        pstVoCtx[i].s32ChnId, s32Ret);
                goto __DESTROY_VO;
            }
            RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_SetChnAttr already.",
                pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId,
                pstVoCtx[i].s32ChnId);

            s32Ret = RK_MPI_VO_EnableChn(pstVoCtx[i].s32LayerId, pstVoCtx[i].s32ChnId);
            if (s32Ret != RK_SUCCESS) {
                RK_LOGE("vo [%d, %d, %d] RK_MPI_VO_EnableChn failedd: %#x!",
                        pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId,
                        pstVoCtx[i].s32ChnId, s32Ret);
                goto __DESTROY_VO;
            }
            RK_LOGV("vo [%d, %d, %d] RK_MPI_VO_EnableChn already.",
                    pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId,
                    pstVoCtx[i].s32ChnId);
            }
    }

    // bind vi to vo
    for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
        s32Ret = TEST_SYS_ViBindVo(pstViCtx[i].s32DevId, pstViCtx[i].s32ChnId,
                    pstVoCtx[i].s32DevId, pstVoCtx[i].s32ChnId);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("bind failed: %x, vi [%d, %d, %d] -> vo [%d, %d, %d]",
                s32Ret, pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId, pstVoCtx[i].s32ChnId,
                pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId, pstVoCtx[i].s32ChnId);
            goto __UNBIND_VI_VO;
        }
        RK_LOGV("bind success, vi [%d, %d, %d] -> vo [%d, %d, %d]",
                pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId, pstVoCtx[i].s32ChnId,
                pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId, pstVoCtx[i].s32ChnId);
    }

    signal(SIGINT, sigterm_handler);
    if (ctx->enChangeMode != TEST_DIS_X_NONE) {
        for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
            pthread_create(&viThread[i], 0, vi_dis_change_loop, reinterpret_cast<void *>(&pstViCtx[i]));
        }

        for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
            if (viThread[i])
                pthread_join(viThread[i], RK_NULL);
        }
    } else {
        if (ctx->s32LoopCount > 0) {
            while (!bExit && s32LoopCount < ctx->s32LoopCount) {
                s32LoopCount++;
                usleep(33 * 1000);
            }
        }
    }

__UNBIND_VI_VO:
    for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
        s32Ret = TEST_SYS_ViUnbindVo(pstViCtx[i].s32DevId, pstViCtx[i].s32ChnId,
                    pstVoCtx[i].s32DevId, pstVoCtx[i].s32ChnId);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("unbind failed: %x, vi [%d, %d, %d] -> vo [%d, %d, %d]",
                s32Ret, pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId, pstVoCtx[i].s32ChnId,
                pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId, pstVoCtx[i].s32ChnId);
            goto __UNBIND_VI_VO;
        }
        RK_LOGV("unbind success, vi [%d, %d, %d] -> vo [%d, %d, %d]",
                pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId, pstVoCtx[i].s32ChnId,
                pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId, pstVoCtx[i].s32ChnId);
    }

__DESTROY_VO:
    /* destroy vo */
    for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
        s32Ret = destroy_vo(&pstVoCtx[i]);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vo [%d, %d, %d] destroy_vo failed: %#x!",
                    pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId,
                    pstVoCtx[i].s32ChnId, s32Ret);
        }
        RK_LOGV("vo [%d, %d, %d] destroy_vo already.",
                pstVoCtx[i].s32DevId, pstVoCtx[i].s32LayerId,
                pstVoCtx[i].s32ChnId);
    }

__DESTROY_VI:
    /* destroy vi */
    for (RK_S32 i = 0; i < ctx->u32ViCnt; i++) {
        s32Ret = destroy_vi(&pstViCtx[i]);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("vi [%d, %d, %d] destroy_vi failed: %#x!",
                    pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                    pstViCtx[i].s32ChnId, s32Ret);
        }
        RK_LOGV("vi [%d, %d, %d] destroy_vi already.",
                pstViCtx[i].s32DevId, pstViCtx[i].s32PipeId,
                pstViCtx[i].s32ChnId);
    }

    RK_SAFE_FREE(pstViCtx);
    RK_SAFE_FREE(pstVoCtx);

    return s32Ret;
}

static void mpi_dis_test_show_options(const TEST_DIS_CTX_S *ctx) {
    RK_PRINT("cmd parse result:\n");

    RK_PRINT("loop count            : %d\n", ctx->s32LoopCount);
    RK_PRINT("test mode             : %d\n", ctx->enTestMode);
    RK_PRINT("vi dev cnt            : %d\n", ctx->u32ViCnt);
    RK_PRINT("vi pixel format       : %d\n", ctx->enPixelFormat);
    RK_PRINT("vi compress mode      : %d\n", ctx->enCompressMode);
    RK_PRINT("vi output width       : %d\n", ctx->u32Width);
    RK_PRINT("vi output hight       : %d\n", ctx->u32Height);
    RK_PRINT("vi enable pic         : %d\n", ctx->bUserPicEnabled);
    RK_PRINT("user pic file         : %s\n", ctx->pUserPicFile);
    RK_PRINT("vi out buf cnt        : %d\n", ctx->u32ViBufCnt);
    RK_PRINT("dis out buf cnt       : %d\n", ctx->u32DisBufCnt);
    RK_PRINT("dis crop ratio        : %d\n", ctx->u32CropRatio);
}

static const char *const usages[] = {
    "./rk_mpi_dis_test -w 1920 -h 1080",
    RK_NULL,
};

int main(int argc, const char **argv) {
    RK_S32 s32Ret = RK_FAILURE;
    TEST_DIS_CTX_S   ctx;

    memset(&ctx, 0, sizeof(TEST_DIS_CTX_S));

    ctx.enTestMode    = TEST_DIS_MODE_BIND_VO;
    ctx.s32LoopCount  = 100;
    ctx.u32ViCnt      = 1;
    ctx.u32ViBufCnt   = 3;
    ctx.u32DisBufCnt  = 5;
    ctx.u32CropRatio  = 80;
    ctx.enChangeMode  = TEST_DIS_X_STILL_CROP;

    RK_LOGE("test running enter!");

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("basic options:"),
        OPT_INTEGER('\0', "test_mode", &(ctx.enTestMode),
                    "test mode (default 1, 0:vi get&release frame; 1:vi bind vo (HDMI).", NULL, 0, 0),
        OPT_INTEGER('\0', "vi_cnt", &(ctx.u32ViCnt),
                    "vi dev cnt (default 1).", NULL, 0, 0),
        OPT_INTEGER('n', "loop_count", &(ctx.s32LoopCount),
                    "set capture frame count (default 100).", NULL, 0, 0),
        OPT_STRING('o', "output", &(ctx.pDstFilePath),
                    "output file path. e.g.(/userdata/dis/). default(NULL).", NULL, 0, 0),
        OPT_GROUP("vi options:"),
        OPT_INTEGER('m', "src_compress", &(ctx.enCompressMode),
                    "set capture compress mode (default 0, 0: NONE; 1: AFBC).", NULL, 0, 0),
        OPT_INTEGER('w', "width", &(ctx.u32Width),
                    "set capture channel width (default 0) <required>.", NULL, 0, 0),
        OPT_INTEGER('h', "height", &(ctx.u32Height),
                    "set capture channel height (default 0) <required>.", NULL, 0, 0),
        OPT_INTEGER('f', "format", &(ctx.enPixelFormat),
                    "set the format (default 0, 0:RK_FMT_YUV420SP).", NULL, 0, 0),
        OPT_INTEGER('U', "user_pic", &(ctx.bUserPicEnabled),
                    "enable using user specify picture as vi input.", NULL, 0, 0),
        OPT_STRING('\0', "usr_pic_file", &(ctx.pUserPicFile),
                    "user specify picture file.", NULL, 0, 0),
        OPT_INTEGER('\0', "vi_buf_count", &(ctx.u32ViBufCnt),
                    "vi out buf count, range[1, 8] (default 3).", NULL, 0, 0),
        OPT_GROUP("dis options:"),
        OPT_INTEGER('\0', "dis_buf_count", &(ctx.u32DisBufCnt),
                    "dis out buf count, range[1, 8] (default 5).", NULL, 0, 0),
        OPT_INTEGER('\0', "crop_ratio", &(ctx.u32CropRatio),
                    "dis out buf count, range[50, 98] (default 80).", NULL, 0, 0),
        OPT_GROUP("dis change test options:"),
        OPT_INTEGER('\0', "change_test", &(ctx.enChangeMode),
                    "dis params dynamic change test, "
                    "(default 0. 0: NONE, 1: function switch, "
                    "2: crop ratio change, 3: still crop switch).", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nselect a test case to run.",
                                 "\nuse --help for details.");
    argc = argparse_parse(&argparse, argc, argv);

    if (!ctx.u32Width || !ctx.u32Height) {
        argparse_usage(&argparse);
        goto __FAILED;
    }

    mpi_dis_test_show_options(&ctx);

    s32Ret = RK_MPI_SYS_Init();
    if (RK_SUCCESS != s32Ret) {
        RK_LOGE("rk mpi sys init fail!");
        goto __FAILED;
    }

    switch (ctx.enTestMode) {
        case TEST_DIS_MODE_VI_ONLY:
            s32Ret = test_vi_get_release_frame_loop(&ctx);
        break;
        case TEST_DIS_MODE_BIND_VO:
            s32Ret = test_vi_bind_vo_loop(&ctx);
        break;
        default:
            RK_LOGE("unsupport such test mode:%d", ctx.enTestMode);
        break;
    }

__FAILED:

    RK_LOGE("test running exit:%d", s32Ret);
    RK_MPI_SYS_Exit();

    return 0;
}
