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
#include "rk_mpi_vo.h"

#include "test_comm_app_vo.h"
#include "test_comm_vo.h"

RK_S32 TEST_COMM_APP_VO_Start(TEST_VO_CFG_S *pstCfg) {
    VO_PUB_ATTR_S VoPubAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_CSC_S  VideoCSC;
    VO_CHN_ATTR_S VoChnAttr[128];
    VO_BORDER_S border;
    RK_U32 s32Ret = RK_SUCCESS;
    RK_U32 u32DispWidth = pstCfg->u32LayerWidth;
    RK_U32 u32DispHeight = pstCfg->u32LayerHeight;
    RK_U32 u32ImageWidth = u32DispWidth;
    RK_U32 u32ImageHeight = u32DispHeight;
    RK_U32 u32Rows = pstCfg->u32LayerRCNum;
    RK_S32 i, totalCH;

    memset(&VoPubAttr, 0, sizeof(VO_PUB_ATTR_S));
    memset(&stLayerAttr, 0, sizeof(VO_VIDEO_LAYER_ATTR_S));
    memset(&VideoCSC, 0, sizeof(VO_CSC_S));
    memset(&VoChnAttr, 0, sizeof(VoChnAttr));
    memset(&border, 0, sizeof(VO_BORDER_S));

    VoPubAttr.enIntfType = pstCfg->enIntfType;
    VoPubAttr.enIntfSync = pstCfg->enIntfSync;

    s32Ret = RK_MPI_VO_SetPubAttr(pstCfg->u32VoDev, &VoPubAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to set pub attr to dev %d", pstCfg->u32VoDev);
        return s32Ret;
    }
    s32Ret = RK_MPI_VO_Enable(pstCfg->u32VoDev);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to enable dev %d", pstCfg->u32VoDev);
        return s32Ret;
    }

    TEST_VO_GetDisplaySize(pstCfg->enIntfSync, &u32DispWidth, &u32DispHeight);
    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Width = u32DispWidth;
    stLayerAttr.stDispRect.u32Height = u32DispHeight;
    stLayerAttr.stImageSize.u32Width = u32ImageWidth;
    stLayerAttr.stImageSize.u32Height = u32ImageHeight;
    stLayerAttr.u32DispFrmRt = 30;
    stLayerAttr.enPixFormat = pstCfg->enLayerPixFmt;
    stLayerAttr.bBypassFrame = RK_FALSE;
    stLayerAttr.enCompressMode = COMPRESS_AFBC_16x16;

    VideoCSC.enCscMatrix = VO_CSC_MATRIX_IDENTITY;
    VideoCSC.u32Contrast =  50;
    VideoCSC.u32Hue = 50;
    VideoCSC.u32Luma = 50;
    VideoCSC.u32Satuature = 50;

    s32Ret = RK_MPI_VO_SetLayerAttr(pstCfg->u32VoLayer, &stLayerAttr);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to set layer attr to layer %d", pstCfg->u32VoLayer);
        return s32Ret;
    }
    s32Ret = RK_MPI_VO_EnableLayer(pstCfg->u32VoLayer);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to enable layer %d", pstCfg->u32VoLayer);
        return s32Ret;
    }
    s32Ret = RK_MPI_VO_SetLayerCSC(pstCfg->u32VoLayer, &VideoCSC);
    if (s32Ret != RK_SUCCESS) {
        return s32Ret;
    }

    // configure and enable vo channel
    u32Rows = (u32Rows == 0 ? 1 : u32Rows);
    totalCH = u32Rows * u32Rows;
    for (i = 0; i < totalCH; i++) {
        VoChnAttr[i].bDeflicker = RK_FALSE;
        VoChnAttr[i].u32Priority = 1;
        VoChnAttr[i].stRect.s32X = (u32ImageWidth / u32Rows) * (i % u32Rows);
        VoChnAttr[i].stRect.s32Y = (u32ImageHeight / u32Rows)* (i / u32Rows);
        VoChnAttr[i].stRect.u32Width = u32ImageWidth / u32Rows;
        VoChnAttr[i].stRect.u32Height = u32ImageHeight / u32Rows;
    }

    for (i = 0; i < totalCH; i++) {
        // set attribute of vo chn
        RK_S32 s32ret = RK_MPI_VO_SetChnAttr(pstCfg->u32VoLayer, i, &VoChnAttr[i]);
        if (RK_SUCCESS != s32ret) {
            RK_LOGE("failed to set dev %d chn %d attr", pstCfg->u32VoLayer, i);
            return s32Ret;
        }
        // set border
        border.bBorderEn = RK_TRUE;
        // Navy blue #000080
        // Midnight Blue #191970
        border.stBorder.u32Color = 0x191970;
        border.stBorder.u32LeftWidth = 2;
        border.stBorder.u32RightWidth = 2;
        border.stBorder.u32TopWidth = 2;
        border.stBorder.u32BottomWidth = 2;
        s32Ret = RK_MPI_VO_SetChnBorder(pstCfg->u32VoLayer, i, &border);
        if (s32Ret != RK_SUCCESS) {
            return s32Ret;
        }
        s32Ret = RK_MPI_VO_EnableChn(pstCfg->u32VoLayer, i);
        if (s32Ret != RK_SUCCESS) {
            RK_LOGE("failed to enable layer %d ch %d", pstCfg->u32VoLayer, i);
            return s32Ret;
        }
    }

    return s32Ret;
}

RK_S32 TEST_COMM_APP_VO_Stop(TEST_VO_CFG_S *pstCfg) {
    RK_S32 s32Ret = RK_SUCCESS;

    // disable vo layer
    s32Ret = RK_MPI_VO_DisableLayer(pstCfg->u32VoLayer);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to disable layer %d", pstCfg->u32VoLayer);
    }

    // disable vo dev
    s32Ret = RK_MPI_VO_Disable(pstCfg->u32VoDev);
    if (s32Ret != RK_SUCCESS) {
        RK_LOGE("failed to disable dev %d", pstCfg->u32VoDev);
    }

    return s32Ret;
}

