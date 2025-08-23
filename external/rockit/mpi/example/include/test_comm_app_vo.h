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

#ifndef SRC_TESTS_RT_MPI_COMMON_TEST_COMM_APP_VO_H_
#define SRC_TESTS_RT_MPI_COMMON_TEST_COMM_APP_VO_H_

#include "rk_common.h"
#include "rk_comm_vo.h"
#include "test_comm_tmd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct rkTEST_VO_CFG_S {
    VO_INTF_TYPE_E enIntfType;
    VO_INTF_SYNC_E enIntfSync;
    PIXEL_FORMAT_E enLayerPixFmt;
    RK_U32 u32VoDev;
    RK_U32 u32VoLayer;
    RK_U32 u32LayerWidth;
    RK_U32 u32LayerHeight;
    RK_U32 u32LayerRCNum;  // 1: 1, 2: 2x2, 3: 3x3, ...
} TEST_VO_CFG_S;

RK_S32 TEST_COMM_APP_VO_Start(TEST_VO_CFG_S *pstCfg);
RK_S32 TEST_COMM_APP_VO_Stop(TEST_VO_CFG_S *pstCfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif  // SRC_TESTS_RT_MPI_COMMON_TEST_COMM_APP_VO_H_
