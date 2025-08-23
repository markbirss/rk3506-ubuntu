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
 */

#ifndef SRC_TESTS_RT_MPI_COMMON_TEST_COMM_APP_VDEC_H_
#define SRC_TESTS_RT_MPI_COMMON_TEST_COMM_APP_VDEC_H_

#include "rk_common.h"
#include "rk_comm_video.h"
#include "test_common.h"
#include "test_comm_tmd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef RK_S32 (*TEST_VDEC_OnRecvFrame)(RK_S32 s32VdecChn, VIDEO_FRAME_INFO_S *pstFrame);

typedef struct rkTEST_VDEC_STREAM_S {
    const char *pSrcFilePath;
    RK_S32 s32ReadLoopCnt;
    // don't rewrite inner params
    STREAM_INFO_S *pstStreamInfo;
} TEST_VDEC_STREAM_S;

typedef struct rkTEST_VDEC_BIND_SRC_S {
    RK_CODEC_ID_E enCodecId;
    RK_U32 u32PicWidth;
    RK_U32 u32PicHeight;
    MPP_CHN_S stSrcChn;
} TEST_VDEC_BIND_SRC_S;

typedef struct rkTEST_VDEC_CFG_S {
    const char *pDstFilePath;
    RK_S32 s32VdecChn;
    RK_U32 u32CompressMode;
    VIDEO_MODE_E enVideoMode;
    VIDEO_DISPLAY_MODE_E enDispMode;
    TEST_MPI_SOURCE_E enVdecSource;

    union {
        TEST_VDEC_STREAM_S stStream;
        TEST_VDEC_BIND_SRC_S stBindSrc;
    };
} TEST_VDEC_CFG_S;

RK_S32 TEST_COMM_APP_VDEC_Start(TEST_VDEC_CFG_S *pstCfg);
RK_S32 TEST_COMM_APP_VDEC_Stop(TEST_VDEC_CFG_S *pstCfg);

RK_S32 TEST_COMM_APP_VDEC_StartProcWithDstChn(TEST_VDEC_CFG_S *pstCfg, MPP_CHN_S *pstDestChn);
RK_S32 TEST_COMM_APP_VDEC_StopProcWithDstChn(TEST_VDEC_CFG_S *pstCfg, MPP_CHN_S *pstDestChn);

RK_S32 TEST_COMM_APP_VDEC_ProcWithGetFrame(TEST_VDEC_CFG_S *pstCfg, TEST_VDEC_OnRecvFrame pRecvCB);

RK_S32 TEST_COMM_APP_VDEC_WaitUntilEos(TEST_VDEC_CFG_S *pstCfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif  // SRC_TESTS_RT_MPI_COMMON_TEST_COMM_APP_VDEC_H_
