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
 * swcactributed under the License is swcactributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef INCLUDE_RT_MPI_RK_COMM_SWCAC_H_
#define INCLUDE_RT_MPI_RK_COMM_SWCAC_H_

#include "rk_common.h"
#include "rk_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define RK_ERR_SWCAC_INVALID_CHNID        RK_DEF_ERR(RK_ID_SWCAC, RK_ERR_LEVEL_ERROR, RK_ERR_INVALID_CHNID)
#define RK_ERR_SWCAC_ILLEGAL_PARAM        RK_DEF_ERR(RK_ID_SWCAC, RK_ERR_LEVEL_ERROR, RK_ERR_ILLEGAL_PARAM)
#define RK_ERR_SWCAC_CHN_UNEXIST          RK_DEF_ERR(RK_ID_SWCAC, RK_ERR_LEVEL_ERROR, RK_ERR_UNEXIST)
#define RK_ERR_SWCAC_NULL_PTR             RK_DEF_ERR(RK_ID_SWCAC, RK_ERR_LEVEL_ERROR, RK_ERR_NULL_PTR)
#define RK_ERR_SWCAC_NOT_SUPPORT          RK_DEF_ERR(RK_ID_SWCAC, RK_ERR_LEVEL_ERROR, RK_ERR_NOT_SUPPORT)
#define RK_ERR_SWCAC_NOT_PERMITTED        RK_DEF_ERR(RK_ID_SWCAC, RK_ERR_LEVEL_ERROR, RK_ERR_NOT_PERM)
#define RK_ERR_SWCAC_NOBUF                RK_DEF_ERR(RK_ID_SWCAC, RK_ERR_LEVEL_ERROR, RK_ERR_NOBUF)
#define RK_ERR_SWCAC_BUF_EMPTY            RK_DEF_ERR(RK_ID_SWCAC, RK_ERR_LEVEL_ERROR, RK_ERR_BUF_EMPTY)
#define RK_ERR_SWCAC_BUF_FULL             RK_DEF_ERR(RK_ID_SWCAC, RK_ERR_LEVEL_ERROR, RK_ERR_BUF_FULL)
#define RK_ERR_SWCAC_SYS_NOTREADY         RK_DEF_ERR(RK_ID_SWCAC, RK_ERR_LEVEL_ERROR, RK_ERR_NOTREADY)
#define RK_ERR_SWCAC_BUSY                 RK_DEF_ERR(RK_ID_SWCAC, RK_ERR_LEVEL_ERROR, RK_ERR_BUSY)

#ifndef RKSWCAC_MAX_CHN_NUM
    #define RKSWCAC_MAX_CHN_NUM      2 // max num of channels supported: Y + UV
#endif
typedef struct rkCAC_EFFECT_ATTR_S {
    RK_U32    u32AutoHighLightDetect;         // {0, 1}
    RK_U32    u32AutoHighLightOffset;         // [0, 127]
    RK_U32    u32FixHighLightBase;            // [0, 255]
    RK_FLOAT  fYCompensate;                   // [0.0f, 1.0f]
    RK_FLOAT  fAutoStrengthU;                 // [0.0f, 1.0f]
    RK_FLOAT  fAutoStrengthV;                 // [0.0f, 1.0f]
    RK_FLOAT  fGrayStrengthU;                 // [0.0f, 1.0f]
    RK_FLOAT  fGrayStrengthV;                 // [0.0f, 1.0f]

    // Reserved params
    RK_FLOAT  fRes1Params[8];             // [i&o] Reserved, do not care, set to 0
    RK_FLOAT  fResNParams[8];             // [i&o] Reserved, do not care, set to 0
} CAC_EFFECT_ATTR_S;

/* The Config of SWCAC */
typedef struct rkSWCAC_CONFIG_S {
    RK_BOOL           bEnable;                /* RW; SWCAC enable */
    CAC_EFFECT_ATTR_S stCacEffectAttr;
} SWCAC_CONFIG_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __RK_COMM_SWCAC_H__ */