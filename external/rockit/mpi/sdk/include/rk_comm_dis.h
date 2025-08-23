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

#ifndef INCLUDE_RT_MPI_RK_COMM_DIS_H_
#define INCLUDE_RT_MPI_RK_COMM_DIS_H_

#include "rk_common.h"
#include "rk_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define RK_ERR_DIS_INVALID_CHNID        RK_DEF_ERR(RK_ID_DIS, RK_ERR_LEVEL_ERROR, RK_ERR_INVALID_CHNID)
#define RK_ERR_DIS_ILLEGAL_PARAM        RK_DEF_ERR(RK_ID_DIS, RK_ERR_LEVEL_ERROR, RK_ERR_ILLEGAL_PARAM)
#define RK_ERR_DIS_CHN_UNEXIST          RK_DEF_ERR(RK_ID_DIS, RK_ERR_LEVEL_ERROR, RK_ERR_UNEXIST)
#define RK_ERR_DIS_NULL_PTR             RK_DEF_ERR(RK_ID_DIS, RK_ERR_LEVEL_ERROR, RK_ERR_NULL_PTR)
#define RK_ERR_DIS_NOT_SUPPORT          RK_DEF_ERR(RK_ID_DIS, RK_ERR_LEVEL_ERROR, RK_ERR_NOT_SUPPORT)
#define RK_ERR_DIS_NOT_PERMITTED        RK_DEF_ERR(RK_ID_DIS, RK_ERR_LEVEL_ERROR, RK_ERR_NOT_PERM)
#define RK_ERR_DIS_NOBUF                RK_DEF_ERR(RK_ID_DIS, RK_ERR_LEVEL_ERROR, RK_ERR_NOBUF)
#define RK_ERR_DIS_BUF_EMPTY            RK_DEF_ERR(RK_ID_DIS, RK_ERR_LEVEL_ERROR, RK_ERR_BUF_EMPTY)
#define RK_ERR_DIS_BUF_FULL             RK_DEF_ERR(RK_ID_DIS, RK_ERR_LEVEL_ERROR, RK_ERR_BUF_FULL)
#define RK_ERR_DIS_SYS_NOTREADY         RK_DEF_ERR(RK_ID_DIS, RK_ERR_LEVEL_ERROR, RK_ERR_NOTREADY)
#define RK_ERR_DIS_BUSY                 RK_DEF_ERR(RK_ID_DIS, RK_ERR_LEVEL_ERROR, RK_ERR_BUSY)

/* Different mode of DIS */
typedef enum rkDIS_MODE_E {
    DIS_MODE_4_DOF_GME      = 0,    /* Only use with GME in 4 dof  */
    DIS_MODE_6_DOF_GME,             /* Only use with GME in 6 dof  */
    DIS_MODE_GYRO,                  /* Only use with gryo in 6 dof  */
    DIS_MODE_HYBRID,                /* Both use with GME and gyro in 6 dof */
    DIS_MODE_DOF_BUTT,
} DIS_MODE_E;

/* The motion level of camera */
typedef enum rkDIS_MOTION_LEVEL_E {
    DIS_MOTION_LEVEL_LOW    = 0,    /* Low motion level*/
    DIS_MOTION_LEVEL_NORMAL,        /* Normal motion level */
    DIS_MOTION_LEVEL_HIGH,          /* High motion level */
    DIS_MOTION_LEVEL_BUTT
}DIS_MOTION_LEVEL_E;


/* Different product type used DIS */
typedef enum rkDIS_PDT_TYPE_E {
    DIS_PDT_TYPE_IPC        = 0,    /* IPC product type */
    DIS_PDT_TYPE_DV,                /* DV product type */
    DIS_PDT_TYPE_DRONE,             /* DRONE product type */
    DIS_PDT_TYPE_BUTT
} DIS_PDT_TYPE_E;

/* The Attribute of DIS */
typedef struct rkDIS_ATTR_S {
    RK_BOOL     bEnable;                /* RW; DIS enable */
    RK_U32      u32MovingSubjectLevel;  /* RW; Range:[0,6]; Moving Subject level */
    RK_S32      s32RollingShutterCoef;  /* RW; Range:[-1000,1000]; Rolling shutter coefficients */
    RK_U32      u32Timelag;             /* RW; Range:[0,200000]; Timestamp delay between Gyro and Frame PTS */
    RK_U32      u32ViewAngle;           /* RW; Range:[100,1380]; The horizontal view angle of the captured video */
    RK_U32      u32HorizontalLimit;     /* RW; Range:[0,1000];
                                         * Parameter to limit horizontal drift by large foreground */
    RK_U32      u32VerticalLimit;       /* RW; Range:[0,1000]; Parameter to limit vertical drift by large foreground */
    RK_BOOL     bStillCrop;             /* RW; The stabilization will be not working,
                                         *  but the output image still be cropped */
} DIS_ATTR_S;

/* The Config of DIS */
typedef struct rkDIS_CONFIG_S {
    DIS_MODE_E              enMode;              /* RW; DIS Mode */
    DIS_MOTION_LEVEL_E      enMotionLevel;       /* RW; DIS Motion level of the camera */
    DIS_PDT_TYPE_E          enPdtType;           /* RW; DIS product type*/
    RK_U32                  u32BufNum;           /* RW; Range:[5,10]; Buf num for DIS */
    RK_U32                  u32CropRatio;        /* RW; Range:[50,98]; Crop ratio of output image */
    RK_U32                  u32FrameRate;        /* RW; Range:[25,120]; The input framerate */
    RK_U32                  u32GyroOutputRange;  /* RW; Range:[0,360]; The range of Gyro output in degree */
    RK_U32                  u32GyroDataBitWidth; /* RW; Range:[0,32];
                                                  * The bits used for gyro angular velocity output */
    RK_BOOL                 bCameraSteady;       /* RW; The camera is steady or not */
} DIS_CONFIG_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __RK_COMM_DIS_H__ */