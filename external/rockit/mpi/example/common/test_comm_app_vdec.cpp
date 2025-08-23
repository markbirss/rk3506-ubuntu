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

#include <unistd.h>
#include <pthread.h>
#include "rk_debug.h"
#include "rk_mpi_mb.h"
#include "rk_mpi_sys.h"
#include "rk_mpi_vdec.h"

#include "test_comm_app_vdec.h"
#include "test_comm_vdec.h"
#include "test_comm_utils.h"

#define MAX_TIME_OUT_MS     40

static void TEST_COMM_APP_VDEC_GetFrameProc(TEST_VDEC_CFG_S *pstCfg, TEST_VDEC_OnRecvFrame pRecvCB) {
    RK_S32 s32Ret;
    RK_S32 s32FrameCount = 0;
    RK_S32 s32ThreadExit = 0;
    RK_S32 s32IsEosFrame = 0;
    FILE *pOutputFp = RK_NULL;
    VIDEO_FRAME_INFO_S sFrame;

    memset(&sFrame, 0, sizeof(VIDEO_FRAME_INFO_S));

    if (pstCfg->pDstFilePath != RK_NULL) {
        pOutputFp = fopen(pstCfg->pDstFilePath, "wb");
        if (pOutputFp == RK_NULL) {
            RK_LOGE("can't open output file %s\n", pstCfg->pDstFilePath);
            return;
        }
    }

    while (!s32ThreadExit) {
        s32Ret = RK_MPI_VDEC_GetFrame(pstCfg->s32VdecChn, &sFrame, MAX_TIME_OUT_MS);
        if (s32Ret >= 0) {
            if (RK_MPI_MB_GetLength(sFrame.stVFrame.pMbBlk) > 0) {
                TEST_COMM_DumpFrame2File(&sFrame, pOutputFp);
                s32FrameCount++;
                RK_LOGD("get chn %d frame %d", pstCfg->s32VdecChn, s32FrameCount);
            }

            s32IsEosFrame = (sFrame.stVFrame.u32FrameFlag & FRAME_FLAG_SNAP_END);
            if (pRecvCB != RK_NULL) {
                s32Ret = pRecvCB(pstCfg->s32VdecChn, &sFrame);
                if (s32Ret != RK_SUCCESS || s32IsEosFrame) {
                    break;
                } else {
                    continue;
                }
            }

            if (s32IsEosFrame) {
                RK_MPI_VDEC_ReleaseFrame(pstCfg->s32VdecChn, &sFrame);
                RK_LOGI("chn %d reach eos frame.", pstCfg->s32VdecChn);
                break;
            }

            RK_MPI_VDEC_ReleaseFrame(pstCfg->s32VdecChn, &sFrame);
        } else {
            if (s32ThreadExit)
                break;

            usleep(1000llu);
        }
    }

    if (pOutputFp)
        fclose(pOutputFp);
}

static void* TEST_COMM_APP_VDEC_GetFrameProcCB(void *pArgs) {
    TEST_VDEC_CFG_S *pstCfg = reinterpret_cast<TEST_VDEC_CFG_S *>(pArgs);

    TEST_COMM_APP_VDEC_GetFrameProc(pstCfg, RK_NULL);
    return RK_NULL;
}

static RK_S32 TEST_COMM_APP_VDEC_StartBySend(TEST_VDEC_CFG_S *pstCfg) {
    RK_S32 s32Ret = RK_SUCCESS;
    VDEC_CHN_ATTR_S stVdecAttr;
    VDEC_CHN_PARAM_S stVdecParam;
    STREAM_INFO_S *pstStreamInfo;

    pstStreamInfo = (STREAM_INFO_S *)malloc(sizeof(STREAM_INFO_S));
    memset(pstStreamInfo, 0, sizeof(STREAM_INFO_S));
    memset(&stVdecAttr, 0, sizeof(VDEC_CHN_ATTR_S));
    memset(&stVdecParam, 0, sizeof(VDEC_CHN_PARAM_S));

    pstCfg->stStream.pstStreamInfo = pstStreamInfo;
    pstStreamInfo->VdecChn = pstCfg->s32VdecChn;
    s32Ret = TEST_COMM_FFmParserOpen(pstCfg->stStream.pSrcFilePath, pstStreamInfo);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to open uri %s", pstCfg->stStream.pSrcFilePath);
        return s32Ret;
    }

    stVdecAttr.enMode = pstCfg->enVideoMode;
    stVdecAttr.enType = pstStreamInfo->enCodecId;
    stVdecAttr.u32PicWidth = pstStreamInfo->u32PicWidth;
    stVdecAttr.u32PicHeight = pstStreamInfo->u32PicHeight;
    stVdecParam.enType = pstStreamInfo->enCodecId;
    stVdecParam.stVdecVideoParam.enCompressMode = (COMPRESS_MODE_E)pstCfg->u32CompressMode;

    s32Ret = TEST_VDEC_Start(pstCfg->s32VdecChn, &stVdecAttr, &stVdecParam, pstCfg->enDispMode);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to start vdec %d", pstCfg->s32VdecChn);
        return s32Ret;
    }

    s32Ret = TEST_VDEC_SetReadLoopCount(pstCfg->s32VdecChn, pstCfg->stStream.s32ReadLoopCnt);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to set loop count");
        return s32Ret;
    }

    s32Ret = TEST_VDEC_StartSendStream(pstCfg->s32VdecChn, pstStreamInfo);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to start send stream");
        return s32Ret;
    }

    return RK_SUCCESS;
}

static RK_S32 TEST_COMM_APP_VDEC_StartByBind(TEST_VDEC_CFG_S *pstCfg) {
    RK_S32 s32Ret = RK_SUCCESS;
    VDEC_CHN_ATTR_S stVdecAttr;
    VDEC_CHN_PARAM_S stVdecParam;
    MPP_CHN_S stDstChn;

    memset(&stVdecAttr, 0, sizeof(VDEC_CHN_ATTR_S));
    memset(&stVdecParam, 0, sizeof(VDEC_CHN_PARAM_S));

    stVdecAttr.enMode = pstCfg->enVideoMode;
    stVdecAttr.enType = pstCfg->stBindSrc.enCodecId;
    stVdecAttr.u32PicWidth = pstCfg->stBindSrc.u32PicWidth;
    stVdecAttr.u32PicHeight = pstCfg->stBindSrc.u32PicHeight;
    stVdecParam.enType = pstCfg->stBindSrc.enCodecId;
    stVdecParam.stVdecVideoParam.enCompressMode = (COMPRESS_MODE_E)pstCfg->u32CompressMode;

    s32Ret = TEST_VDEC_Start(pstCfg->s32VdecChn, &stVdecAttr, &stVdecParam, pstCfg->enDispMode);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to start vdec %d", pstCfg->s32VdecChn);
        return s32Ret;
    }

    stDstChn.enModId = RK_ID_VDEC;
    stDstChn.s32DevId = 0;
    stDstChn.s32ChnId = pstCfg->s32VdecChn;

    s32Ret = RK_MPI_SYS_Bind(&pstCfg->stBindSrc.stSrcChn, &stDstChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to bind with %#x", s32Ret);
        return s32Ret;
    }

    return RK_SUCCESS;
}

RK_S32 TEST_COMM_APP_VDEC_Start(TEST_VDEC_CFG_S *pstCfg) {
    if (pstCfg->enVdecSource == TEST_MPI_SOURCE_BIND) {
        return TEST_COMM_APP_VDEC_StartByBind(pstCfg);
    } else if (pstCfg->enVdecSource == TEST_MPI_SOURCE_SEND) {
        return TEST_COMM_APP_VDEC_StartBySend(pstCfg);
    } else {
        RK_LOGE("error vdec input source type %d", pstCfg->enVdecSource);
        return RK_FAILURE;
    }
}

static RK_S32 TEST_COMM_APP_VDEC_StopBySend(TEST_VDEC_CFG_S *pstCfg) {
    RK_S32 s32Ret = RK_SUCCESS;
    STREAM_INFO_S *pstStreamInfo = pstCfg->stStream.pstStreamInfo;

    if (pstStreamInfo == RK_NULL) {
        RK_LOGE("vdec %d is not started", pstCfg->s32VdecChn);
        return RK_FAILURE;
    }

    s32Ret = TEST_VDEC_StopSendStream(pstCfg->s32VdecChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to stop %d vdec send stream", pstCfg->s32VdecChn);
    }
    s32Ret = TEST_VDEC_Stop(pstCfg->s32VdecChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to stop %d vdec channel", pstCfg->s32VdecChn);
    }
    s32Ret = TEST_COMM_FFmParserClose(pstStreamInfo);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to close stream");
    }

    free(pstCfg->stStream.pstStreamInfo);
    pstCfg->stStream.pstStreamInfo = RK_NULL;

    return s32Ret;
}

static RK_S32 TEST_COMM_APP_VDEC_StopByBind(TEST_VDEC_CFG_S *pstCfg) {
    RK_S32 s32Ret = RK_SUCCESS;
    MPP_CHN_S stDstChn;

    stDstChn.enModId = RK_ID_VDEC;
    stDstChn.s32DevId = 0;
    stDstChn.s32ChnId = pstCfg->s32VdecChn;

    s32Ret = RK_MPI_SYS_UnBind(&pstCfg->stBindSrc.stSrcChn, &stDstChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to unbind with %#x", s32Ret);
    }

    s32Ret = TEST_VDEC_Stop(pstCfg->s32VdecChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to stop %d vdec channel", pstCfg->s32VdecChn);
    }

    return s32Ret;
}

RK_S32 TEST_COMM_APP_VDEC_Stop(TEST_VDEC_CFG_S *pstCfg) {
    if (pstCfg->enVdecSource == TEST_MPI_SOURCE_BIND) {
        return TEST_COMM_APP_VDEC_StopByBind(pstCfg);
    } else if (pstCfg->enVdecSource == TEST_MPI_SOURCE_SEND) {
        return TEST_COMM_APP_VDEC_StopBySend(pstCfg);
    } else {
        RK_LOGE("error vdec input source type %d", pstCfg->enVdecSource);
        return RK_FAILURE;
    }
}

RK_S32 TEST_COMM_APP_VDEC_StartProcWithDstChn(TEST_VDEC_CFG_S *pstCfg, MPP_CHN_S *pstDestChn) {
    RK_S32 s32Ret = RK_SUCCESS;
    MPP_CHN_S stSrcChn;

    stSrcChn.enModId = RK_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = pstCfg->s32VdecChn;

    s32Ret = RK_MPI_SYS_Bind(&stSrcChn, pstDestChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to bind with %#x", s32Ret);
        return s32Ret;
    }

    s32Ret = TEST_COMM_APP_VDEC_Start(pstCfg);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to start proc with dst channel %#x", s32Ret);
        return s32Ret;
    }

    return s32Ret;
}

RK_S32 TEST_COMM_APP_VDEC_StopProcWithDstChn(TEST_VDEC_CFG_S *pstCfg, MPP_CHN_S *pstDestChn) {
    RK_S32 s32Ret = RK_SUCCESS;
    MPP_CHN_S stSrcChn;

    s32Ret = TEST_COMM_APP_VDEC_Stop(pstCfg);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to stop proc with dst channel %#x", s32Ret);
    }

    stSrcChn.enModId = RK_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = pstCfg->s32VdecChn;

    s32Ret = RK_MPI_SYS_UnBind(&stSrcChn, pstDestChn);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to unbind with %#x", s32Ret);
    }

    return s32Ret;
}

RK_S32 TEST_COMM_APP_VDEC_ProcWithGetFrame(TEST_VDEC_CFG_S *pstCfg, TEST_VDEC_OnRecvFrame pRecvCB) {
    RK_S32 s32Ret = RK_SUCCESS;

    s32Ret = TEST_COMM_APP_VDEC_Start(pstCfg);
    if (s32Ret != RK_SUCCESS) {
        goto __FAILED;
    }

    // Get Decoded Frame
    TEST_COMM_APP_VDEC_GetFrameProc(pstCfg, pRecvCB);
    // pthread_t getFrmThread;
    // pthread_create(&getFrmThread, 0, TEST_COMM_APP_VDEC_GetFrameProcCB, reinterpret_cast<void *>(pstCfg));
    // pthread_join(getFrmThread, RK_NULL);

__FAILED:
    s32Ret = TEST_COMM_APP_VDEC_Stop(pstCfg);
    return s32Ret;
}

RK_S32 TEST_COMM_APP_VDEC_WaitUntilEos(TEST_VDEC_CFG_S *pstCfg) {
    return TEST_VDEC_WaitUntilEos(pstCfg->s32VdecChn);
}

