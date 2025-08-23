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

#include <cstdio>
#include <cstring>
#include "rk_debug.h"
#include "rk_mpi_sys.h"
#include "rk_mpi_vdec.h"

#include "test_comm_argparse.h"
#include "test_comm_utils.h"
#include "test_comm_app_vdec.h"
#include "test_comm_app_vo.h"

typedef struct rkTEST_VDEC_CTX_S {
    const char *pSrcFilePath;
    const char *pDstFilePath;
    RK_S32 s32LoopCount;
    RK_S32 s32VdecChn;
    RK_U32 u32CompressMode;
    RK_U32 u32TestMode;
} TEST_VDEC_CTX_S;

RK_S32 test_mpi_vdec_revc_frame(RK_S32 s32VdecChn, VIDEO_FRAME_INFO_S *pstFrame) {
    RK_LOGE("recv vdec %d frame pts %lld", s32VdecChn, pstFrame->stVFrame.u64PTS);
    RK_MPI_VDEC_ReleaseFrame(s32VdecChn, pstFrame);
    return RK_SUCCESS;
}

RK_S32 test_mpi_vdec_proc(TEST_VDEC_CTX_S *ctx) {
    RK_S32 s32Ret = RK_SUCCESS;
    TEST_VDEC_CFG_S stVdecCfg;
    TEST_VO_CFG_S stVoCfg;
    MPP_CHN_S stDestChn;

    memset(&stVdecCfg, 0, sizeof(TEST_VDEC_CFG_S));
    stVdecCfg.s32VdecChn = ctx->s32VdecChn;
    stVdecCfg.u32CompressMode = ctx->u32CompressMode;
    stVdecCfg.enVideoMode = VIDEO_MODE_FRAME;
    stVdecCfg.enDispMode = VIDEO_DISPLAY_MODE_PLAYBACK;
    stVdecCfg.enVdecSource = TEST_MPI_SOURCE_SEND;
    stVdecCfg.pDstFilePath = ctx->pDstFilePath;
    stVdecCfg.stStream.pSrcFilePath = ctx->pSrcFilePath;
    stVdecCfg.stStream.s32ReadLoopCnt = 1;  // loop outside

    if (ctx->u32TestMode == 0) {
        TEST_COMM_APP_VDEC_ProcWithGetFrame(&stVdecCfg, test_mpi_vdec_revc_frame);
    } else {
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

        stDestChn.enModId = RK_ID_VO;
        stDestChn.s32DevId = stVoCfg.u32VoDev;
        stDestChn.s32ChnId = ctx->s32VdecChn;
        TEST_COMM_APP_VDEC_StartProcWithDstChn(&stVdecCfg, &stDestChn);

        TEST_COMM_APP_VDEC_WaitUntilEos(&stVdecCfg);

        TEST_COMM_APP_VDEC_StopProcWithDstChn(&stVdecCfg, &stDestChn);
        TEST_COMM_APP_VO_Stop(&stVoCfg);
    }
    return s32Ret;
}

static void test_mpi_vdec_show_options(const TEST_VDEC_CTX_S *ctx) {
    RK_PRINT("cmd parse result:\n");
    RK_PRINT("input file name        : %s\n", ctx->pSrcFilePath);
    RK_PRINT("output path            : %s\n", ctx->pDstFilePath);
    RK_PRINT("loop count             : %d\n", ctx->s32LoopCount);
    RK_PRINT("channel index          : %d\n", ctx->s32VdecChn);
    RK_PRINT("output compress mode   : %d\n", ctx->u32CompressMode);
    return;
}

static const char *const usages[] = {
    "./rk_mpi_vdec_test [-i SRC_PATH] [-o OUTPUT_PATH]...",
    NULL,
};

int main(int argc, const char **argv) {
    TEST_VDEC_CTX_S ctx;
    memset(&ctx, 0, sizeof(TEST_VDEC_CTX_S));

    ctx.s32LoopCount = 1;
    ctx.s32VdecChn = 0;
    ctx.u32CompressMode = COMPRESS_MODE_NONE;  // Suggest::COMPRESS_AFBC_16x16;
    ctx.u32TestMode = 0;

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("basic options:"),
        OPT_STRING('i', "input", &(ctx.pSrcFilePath),
                   "input file path. <required>", NULL, 0, 0),
        OPT_STRING('o', "output", &(ctx.pDstFilePath),
                   "output file path", NULL, 0, 0),
        OPT_INTEGER('n', "loop_count", &(ctx.s32LoopCount),
                    "loop running count. default(1)", NULL, 0, 0),
        OPT_INTEGER('c', "channel_index", &(ctx.s32VdecChn),
                    "vdec channel index. default(0).", NULL, 0, 0),
        OPT_INTEGER('\0', "compress_mode", &(ctx.u32CompressMode),
                    "vdec compress mode, default(0); 0: NONE, 1: AFBC_16X16", NULL, 0, 0),
        OPT_INTEGER('m', "mode", &(ctx.u32TestMode),
                    "vdec test mode. default(0); 0: Vdec only, 1: Vdec Bind Vo.", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nselect a test case to run.",
                                 "\nuse --help for details.");

    argc = argparse_parse(&argparse, argc, argv);
    test_mpi_vdec_show_options(&ctx);

    if (RK_MPI_SYS_Init() != RK_SUCCESS) {
        goto __FAILED;
    }

    while (ctx.s32LoopCount > 0) {
        if (test_mpi_vdec_proc(&ctx) < 0) {
            goto __FAILED;
        }
        ctx.s32LoopCount--;
    }

    if (RK_MPI_SYS_Exit() != RK_SUCCESS) {
        goto __FAILED;
    }

    RK_LOGE("test running success!");
    return RK_SUCCESS;
__FAILED:
    RK_LOGE("test running failed! %d count running done not yet.", ctx.s32LoopCount);
    return RK_FAILURE;
}

