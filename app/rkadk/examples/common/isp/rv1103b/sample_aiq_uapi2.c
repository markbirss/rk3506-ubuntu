/*
 * Copyright (c) 2024 Rockchip, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifdef RKAIQ
#include "isp/sample_isp.h"
#include <rk_aiq_user_api2_isp.h>

#include "rkadk_log.h"
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  RKADK_S32 s32CamId;
  RKADK_U32 u32StartCnt;
  pthread_mutex_t aiqCtxMutex;
  int gsLDCMode;
  rk_aiq_sys_ctx_t *pstAiqCtx;
} SAMPLE_ISP_HANDLE_S;

rk_aiq_wb_gain_t stGsWbAutoGain = {2.083900, 1.000000, 1.000000, 2.018500};
static SAMPLE_ISP_HANDLE_S gstIspHandle[RKADK_MAX_SENSOR_CNT] = {
    {-1, 0, PTHREAD_MUTEX_INITIALIZER, -1, NULL},
    {-1, 0, PTHREAD_MUTEX_INITIALIZER, -1, NULL},
    {-1, 0, PTHREAD_MUTEX_INITIALIZER, -1, NULL}};

typedef enum _SHUTTERSPEED_TYPE_E {
  SHUTTERSPEED_1_25 = 0,
  SHUTTERSPEED_1_30,
  SHUTTERSPEED_1_75,
  SHUTTERSPEED_1_100,
  SHUTTERSPEED_1_120,
  SHUTTERSPEED_1_150,
  SHUTTERSPEED_1_250,
  SHUTTERSPEED_1_300,
  SHUTTERSPEED_1_425,
  SHUTTERSPEED_1_600,
  SHUTTERSPEED_1_1000,
  SHUTTERSPEED_1_1250,
  SHUTTERSPEED_1_1750,
  SHUTTERSPEED_1_2500,
  SHUTTERSPEED_1_3000,
  SHUTTERSPEED_1_6000,
  SHUTTERSPEED_1_10000,
  SHUTTERSPEED_BUTT
} SHUTTERSPEED_TYPE_E;

typedef struct rk_SHUTTER_ATTR_S {
  SHUTTERSPEED_TYPE_E enShutterSpeed;
  float fExposureTime;
} SHUTTER_ATTR_S;

static SHUTTER_ATTR_S g_stShutterAttr[SHUTTERSPEED_BUTT] = {
    {SHUTTERSPEED_1_25, 1.0 / 25.0},      {SHUTTERSPEED_1_30, 1.0 / 30.0},
    {SHUTTERSPEED_1_75, 1.0 / 75.0},      {SHUTTERSPEED_1_100, 1.0 / 100.0},
    {SHUTTERSPEED_1_120, 1.0 / 120.0},    {SHUTTERSPEED_1_150, 1.0 / 150.0},
    {SHUTTERSPEED_1_250, 1.0 / 250.0},    {SHUTTERSPEED_1_300, 1.0 / 300.0},
    {SHUTTERSPEED_1_425, 1.0 / 425.0},    {SHUTTERSPEED_1_600, 1.0 / 600.0},
    {SHUTTERSPEED_1_1000, 1.0 / 1000.0},  {SHUTTERSPEED_1_1250, 1.0 / 1250.0},
    {SHUTTERSPEED_1_1750, 1.0 / 1750.0},  {SHUTTERSPEED_1_2500, 1.0 / 2500.0},
    {SHUTTERSPEED_1_3000, 1.0 / 3000.0},  {SHUTTERSPEED_1_6000, 1.0 / 6000.0},
    {SHUTTERSPEED_1_10000, 1.0 / 10000.0}};

static int SAMPLE_ISP_FindUsableIdx(RKADK_U32 u32CamId) {
  for (int i = 0; i < RKADK_MAX_SENSOR_CNT; i++) {
    if (gstIspHandle[i].s32CamId == -1) {
      RKADK_LOGP("find usable index[%d] u32CamId[%d]", i, u32CamId);
      return i;
    }
  }

  return -1;
}

static int SAMPLE_ISP_GetIdx(RKADK_U32 u32CamId) {
  for (int i = 0; i < RKADK_MAX_SENSOR_CNT; i++) {
    if (gstIspHandle[i].s32CamId == (RKADK_S32)u32CamId) {
      RKADK_LOGP("find matched index[%d] u32CamId[%d]", i, u32CamId);
      return i;
    }
  }

  return -1;
}

static int SAMPLE_ISP_Init(RKADK_U32 u32CamId, SAMPLE_ISP_PARAM stIspParam) {
  int ret;
  char hdrStr[16];
  rk_aiq_sys_ctx_t *pstAiqCtx;
  rk_aiq_static_info_t atAiqStaticInfo;

  setlinebuf(stdout);

  // must set HDR_MODE, before init
  snprintf(hdrStr, sizeof(hdrStr), "%d", (int)stIspParam.WDRMode);
  setenv("HDR_MODE", hdrStr, 1);

  ret = rk_aiq_uapi2_sysctl_enumStaticMetasByPhyId(u32CamId, &atAiqStaticInfo);
  if (ret) {
    RKADK_LOGE("rk_aiq_uapi2_sysctl_enumStaticMetasByPhyId u32CamId[%d] failed[%d]", u32CamId, ret);
    return ret;
  }

#if 0
  rk_aiq_tb_info_t tb_info;
  memset(&tb_info, 0, sizeof(rk_aiq_tb_info_t));
  tb_info.magic = sizeof(rk_aiq_tb_info_t) - 2;
  tb_info.is_start_once = false;
  tb_info.is_pre_aiq = false;
  tb_info.rtt_share_addr = 0;
  tb_info.prd_type = RK_AIQ_PRD_TYPE_SINGLE_FRAME;
  if (stIspParam.iqFileDir != NULL) {
    RKADK_LOGP("rkaiq use iqfiles from %s", stIspParam.iqFileDir);
    tb_info.iq_bin_mode = RK_AIQ_META_NOT_FULL_IQ_BIN;
  }
  ret = rk_aiq_uapi2_sysctl_preInit_tb_info(atAiqStaticInfo.sensor_info.sensor_name, &tb_info);
  if (ret != 0)
    RKADK_LOGE("rk_aiq_uapi2_sysctl_preInit_tb_info failed %#X!", ret);
#endif

  RKADK_LOGP("CamId: %d, sensor_name is %s, iqfiles is %s", u32CamId,
             atAiqStaticInfo.sensor_info.sensor_name, stIspParam.iqFileDir);

  if(stIspParam.bMultiCam) {
    ret = rk_aiq_uapi2_sysctl_preInit_devBufCnt(atAiqStaticInfo.sensor_info.sensor_name, "rkraw_rx", 2);
    if (ret)
      RKADK_LOGE("rk_aiq_uapi2_sysctl_preInit_devBufCnt u32CamId[%d] failed[%d]", u32CamId, ret);
  }

  pstAiqCtx = rk_aiq_uapi2_sysctl_init(atAiqStaticInfo.sensor_info.sensor_name,
                                      stIspParam.iqFileDir, NULL, NULL);
  if (!pstAiqCtx) {
    RKADK_LOGE("rk_aiq_uapi2_sysctl2_init u32CamId[%d] failed", u32CamId);
    return -1;
  }

  if (stIspParam.bMultiCam)
    rk_aiq_uapi2_sysctl_setMulCamConc(pstAiqCtx, true);

  gstIspHandle[u32CamId].pstAiqCtx = pstAiqCtx;

  RKADK_LOGP("u32CamId[%d] aiq init done", u32CamId);
  return 0;
}

static int SAMPLE_ISP_Run(RKADK_U32 u32CamId, rk_aiq_working_mode_t WDRMode) {
  int ret;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  ret = rk_aiq_uapi2_sysctl_prepare(gstIspHandle[u32CamId].pstAiqCtx, 0, 0,
                                   WDRMode);
  if (ret) {
    RKADK_LOGE("rkaiq engine prepare u32CamId[%d] failed[%d]", u32CamId, ret);
    gstIspHandle[u32CamId].pstAiqCtx = NULL;
    return ret;
  }

  ret = rk_aiq_uapi2_sysctl_start(gstIspHandle[u32CamId].pstAiqCtx);
  if (ret) {
    RKADK_LOGE("rk_aiq_uapi2_sysctl_start u32CamId[%d] failed[%d]", u32CamId, ret);
    gstIspHandle[u32CamId].pstAiqCtx = NULL;
    return ret;
  }

  RKADK_LOGP("u32CamId[%d] aiq prepare and start done", u32CamId);
  return 0;
}

int SAMPLE_ISP_SET_FrameRate(RKADK_U32 u32CamId, RKADK_U32 uFps) {
  int ret = 0;
  ae_api_expSwAttr_t expSwAttr;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  RKADK_LOGP("u32CamId[%d] uFps %d", u32CamId, uFps);
  ret = rk_aiq_user_api2_ae_getExpSwAttr(gstIspHandle[u32CamId].pstAiqCtx, &expSwAttr);
  expSwAttr.commCtrl.frmRate.sw_aeT_frmRate_mode = ae_frmRate_fix_mode;
  expSwAttr.commCtrl.frmRate.sw_aeT_frmRate_val = uFps;
  ret = rk_aiq_user_api2_ae_setExpSwAttr(gstIspHandle[u32CamId].pstAiqCtx, expSwAttr);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_Start(RKADK_U32 u32CamId, SAMPLE_ISP_PARAM stIspParam) {
  int index, ret;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_POINTER(stIspParam.iqFileDir, RKADK_FAILURE);

  index = SAMPLE_ISP_GetIdx(u32CamId);
  if (index < 0) {
    index = SAMPLE_ISP_FindUsableIdx(u32CamId);
    if (index < 0) {
      RKADK_LOGE("u32CamId[%d] not find usable index", u32CamId);
      return -1;
    }
  }

  if (gstIspHandle[index].u32StartCnt != 0) {
    gstIspHandle[index].u32StartCnt++;
    RKADK_LOGP("u32CamId[%d] u32StartCnt: %d", u32CamId, gstIspHandle[index].u32StartCnt);
    return 0;
  }

  ret = SAMPLE_ISP_Init(u32CamId, stIspParam);
  if (ret) {
    RKADK_LOGE("SAMPLE_ISP_Init u32CamId[%d] failed[%d]", u32CamId, ret);
    return ret;
  }

  ret = SAMPLE_ISP_Run(u32CamId, stIspParam.WDRMode);
  if (ret) {
    RKADK_LOGE("SAMPLE_ISP_Run u32CamId[%d] failed[%d]", u32CamId, ret);
    return ret;
  }

  ret = SAMPLE_ISP_SET_FrameRate(u32CamId, stIspParam.fps);
  if (ret) {
    RKADK_LOGE("SAMPLE_ISP_SET_FrameRate u32CamId[%d] failed[%d]", u32CamId, ret);
    return ret;
  }

  gstIspHandle[index].s32CamId = u32CamId;
  gstIspHandle[index].u32StartCnt++;
  RKADK_LOGP("u32CamId[%d] done!", u32CamId);
  return 0;
}

int SAMPLE_ISP_Stop(RKADK_U32 u32CamId) {
  int index, ret;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  index = SAMPLE_ISP_GetIdx(u32CamId);
  if (index < 0) {
    RKADK_LOGE("not find matched u32CamId[%d]", u32CamId);
    return -1;
  }

  if (gstIspHandle[index].u32StartCnt > 1) {
    gstIspHandle[index].u32StartCnt--;
    RKADK_LOGP("u32CamId[%d] u32StartCnt: %d", u32CamId, gstIspHandle[index].u32StartCnt);
    return 0;
  }

  ret = rk_aiq_uapi2_sysctl_stop(gstIspHandle[u32CamId].pstAiqCtx, false);
  if (ret) {
    RKADK_LOGE("rk_aiq_uapi2_sysctl_stop u32CamId[%d] failed[%d]", u32CamId, ret);
    return ret;
  }

  rk_aiq_uapi2_sysctl_deinit(gstIspHandle[u32CamId].pstAiqCtx);

  gstIspHandle[u32CamId].pstAiqCtx = NULL;
  gstIspHandle[index].s32CamId = -1;
  gstIspHandle[index].u32StartCnt = 0;
  RKADK_LOGP("u32CamId[%d] done!", u32CamId);
  return 0;
}

int SAMPLE_ISP_SET_MirrorFlip(RKADK_U32 u32CamId, bool mirror, bool flip) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  RKADK_LOGP("mirror=%d, flip=%d", mirror, flip);
  ret = rk_aiq_uapi2_setMirrorFlip(gstIspHandle[u32CamId].pstAiqCtx, mirror, flip,
                                 4);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_GET_MirrorFlip(RKADK_U32 u32CamId, bool *mirror, bool *flip) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);
  ret =
      rk_aiq_uapi2_getMirrorFlip(gstIspHandle[u32CamId].pstAiqCtx, mirror, flip);
  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_UpdateIq(RKADK_U32 u32CamId, char *iqfile) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_POINTER(iqfile, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  RKADK_LOGP("iqfile: %s", iqfile);
  ret = rk_aiq_uapi2_sysctl_updateIq(gstIspHandle[u32CamId].pstAiqCtx, iqfile);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_FecEn(RKADK_U32 u32CamId, bool bFECEnable) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  RKADK_LOGP("bFECEnable %d", bFECEnable);
  ret = rk_aiq_uapi2_setFecEn(gstIspHandle[u32CamId].pstAiqCtx, bFECEnable);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_FecBypass(RKADK_U32 u32CamId, bool bypass) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  RKADK_LOGP("bypass=%d", bypass);
  ret = rk_aiq_uapi2_setFecBypass(gstIspHandle[u32CamId].pstAiqCtx, bypass);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_EnableLdch(RKADK_U32 u32CamId, bool on, RKADK_U32 level) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  RKADK_LOGP("on = %d, ldc level = %d", on, level);
  if (level < 1)
    level = 1;
  else if (level > 255)
    level = 255;

  ret = rk_aiq_uapi2_setLdchEn(gstIspHandle[u32CamId].pstAiqCtx, on);
  if (on)
    ret |= rk_aiq_uapi2_setLdchCorrectLevel(gstIspHandle[u32CamId].pstAiqCtx,
                                           level);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_GET_LdchAttrib(RKADK_U32 u32CamId,
                                rk_aiq_ldch_attrib_t *attr) {
  int ret = 0;
/*
  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_POINTER(attr, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  memset(attr, 0, sizeof(rk_aiq_ldch_attrib_t));
  ret = rk_aiq_user_api2_aldch_GetAttrib(gstIspHandle[u32CamId].pstAiqCtx, attr);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
*/
  return ret;
}

/*only support switch between HDR and normal*/
int SAMPLE_ISP_SET_WDRModeDyn(RKADK_U32 u32CamId,
                                rk_aiq_working_mode_t WDRMode) {
  int ret = 0;
  /*
  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);
  ret = rk_aiq_uapi2_sysctl_swWorkingModeDyn(gstIspHandle[u32CamId].pstAiqCtx,
                                            WDRMode);
  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  */
  return ret;
}

int SAMPLE_ISP_SET_Brightness(RKADK_U32 u32CamId, RKADK_U32 value) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  // value[0,255], default 128
  ret = rk_aiq_uapi2_setBrightness(gstIspHandle[u32CamId].pstAiqCtx, value);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_Contrast(RKADK_U32 u32CamId, RKADK_U32 value) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  // value[0,255], default 128
  ret = rk_aiq_uapi2_setContrast(gstIspHandle[u32CamId].pstAiqCtx, value);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_Saturation(RKADK_U32 u32CamId, RKADK_U32 value) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  // value[0,255], default 128
  ret = rk_aiq_uapi2_setSaturation(gstIspHandle[u32CamId].pstAiqCtx, value);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_Sharpness(RKADK_U32 u32CamId, RKADK_U32 value) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  // value[0,100], default 50
  ret = rk_aiq_uapi2_setSharpness(gstIspHandle[u32CamId].pstAiqCtx, value);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_ManualExposureAutoGain(RKADK_U32 u32CamId,
                                            RKADK_U32 u32Shutter) {
  int ret = 0;
  opMode_t mode = OP_AUTO;
  float timeValue;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  ret = rk_aiq_uapi2_setExpGainMode(gstIspHandle[u32CamId].pstAiqCtx, mode);

  timeValue = g_stShutterAttr[u32Shutter % SHUTTERSPEED_BUTT].fExposureTime;
  ret |= rk_aiq_uapi2_setExpManualTime(gstIspHandle[u32CamId].pstAiqCtx, timeValue);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_ManualExposureManualGain(RKADK_U32 u32CamId,
                                              RKADK_U32 u32Shutter,
                                              RKADK_U32 u32Gain) {
  int ret = 0;
  opMode_t mode = OP_MANUAL;
  float timeValue, gainValue;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  ret = rk_aiq_uapi2_setExpGainMode(gstIspHandle[u32CamId].pstAiqCtx, mode);

  timeValue = g_stShutterAttr[u32Shutter % SHUTTERSPEED_BUTT].fExposureTime;
  ret |= rk_aiq_uapi2_setExpManualTime(gstIspHandle[u32CamId].pstAiqCtx, timeValue);

  gainValue = (1.0 + u32Gain);
  ret |= rk_aiq_uapi2_setExpManualGain(gstIspHandle[u32CamId].pstAiqCtx, gainValue);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_AutoExposure(RKADK_U32 u32CamId) {
  int ret = 0;
  opMode_t mode = OP_AUTO;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);
  ret = rk_aiq_uapi2_setExpMode(gstIspHandle[u32CamId].pstAiqCtx, mode);
  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_Exposure(RKADK_U32 u32CamId, bool bIsAutoExposure,
                              RKADK_U32 bIsAGC, RKADK_U32 u32ElectronicShutter,
                              RKADK_U32 u32Agc) {
  int ret = 0;

  if (!bIsAutoExposure) {
    if (bIsAGC) {
      ret = SAMPLE_ISP_SET_ManualExposureAutoGain(u32CamId,
                                                    u32ElectronicShutter);
    } else {
      ret = SAMPLE_ISP_SET_ManualExposureManualGain(
          u32CamId, u32ElectronicShutter, u32Agc);
    }
  } else {
    ret = SAMPLE_ISP_SET_AutoExposure(u32CamId);
  }

  return ret;
}

int SAMPLE_ISP_SET_BackLight(RKADK_U32 u32CamId, bool bEnable,
                               RKADK_U32 u32Strength) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  if (bEnable) {
    ret = rk_aiq_uapi2_setBLCMode(gstIspHandle[u32CamId].pstAiqCtx, true,
                                 AE_MEAS_AREA_AUTO);
    usleep(30000);
    // u32Strength: [1~100]
    ret |= rk_aiq_uapi2_setBLCStrength(gstIspHandle[u32CamId].pstAiqCtx,
                                      u32Strength);
  } else {
    ret = rk_aiq_uapi2_setBLCMode(gstIspHandle[u32CamId].pstAiqCtx, false,
                                 AE_MEAS_AREA_AUTO);
  }

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_LightInhibition(RKADK_U32 u32CamId, bool bEnable,
                                     unsigned char u8Strength,
                                     unsigned char u8Level) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  if (bEnable) {
    ret = rk_aiq_uapi2_setHLCMode(gstIspHandle[u32CamId].pstAiqCtx, true);
    if (!ret) {
      // u8Strength: [0~100]
      ret = rk_aiq_uapi2_setHLCStrength(gstIspHandle[u32CamId].pstAiqCtx,
                                       u8Strength);
      // u8Level: [0~10]
      ret = rk_aiq_uapi2_setDarkAreaBoostStrth(gstIspHandle[u32CamId].pstAiqCtx,
                                              u8Level);
    }
  } else {
    ret = rk_aiq_uapi2_setHLCMode(gstIspHandle[u32CamId].pstAiqCtx, false);
  }

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_DarkAreaBoostStrth(RKADK_U32 u32CamId, RKADK_U32 level) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  if (level < 0)
    level = 0;
  else if (level > 10)
    level = 10;

  RKADK_LOGP("level = %d", level);
  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);
  ret = rk_aiq_uapi2_setDarkAreaBoostStrth(gstIspHandle[u32CamId].pstAiqCtx,
                                          level);
  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_GET_DarkAreaBoostStrth(RKADK_U32 u32CamId, RKADK_U32 *level) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);
  ret = rk_aiq_uapi2_getDarkAreaBoostStrth(gstIspHandle[u32CamId].pstAiqCtx,
                                          level);
  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_AutoWhiteBalance(RKADK_U32 u32CamId) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);
  ret = rk_aiq_uapi2_setWBMode(gstIspHandle[u32CamId].pstAiqCtx, OP_AUTO);
  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_ManualWhiteBalance(RKADK_U32 u32CamId, RKADK_U32 u32RGain,
                                        RKADK_U32 u32GGain,
                                        RKADK_U32 u32BGain) {
  int ret = 0;
  rk_aiq_wb_gain_t gain;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  u32RGain = (u32RGain == 0) ? 1 : u32RGain;
  u32GGain = (u32GGain == 0) ? 1 : u32GGain;
  u32BGain = (u32BGain == 0) ? 1 : u32BGain;
  // gain.bgain =  (b_gain / 255.0f) * 4;//[0,255]->(0.0, 4.0]
  // gain.grgain = (g_gain / 255.0f) * 4;//[0,255]->(0.0, 4.0]
  // gain.gbgain = (g_gain / 255.0f) * 4;//[0,255]->(0.0, 4.0]
  // gain.rgain =  (r_gain / 255.0f) * 4;//[0,255]->(0.0, 4.0]

  //[0,255]->(0.0, 4.0]
  gain.rgain = (u32RGain / 128.0f) * stGsWbAutoGain.rgain;
  //[0,255]->(0.0, 4.0]
  gain.grgain = (u32GGain / 128.0f) * stGsWbAutoGain.grgain;
  //[0,255]->(0.0, 4.0]
  gain.gbgain = (u32GGain / 128.0f) * stGsWbAutoGain.gbgain;
  //[0,255]->(0.0, 4.0]
  gain.bgain = (u32BGain / 128.0f) * stGsWbAutoGain.bgain;

  RKADK_LOGP("convert gain r g g b %f, %f, %f, %f", gain.rgain, gain.grgain,
             gain.gbgain, gain.bgain);
  RKADK_LOGP("auto gain r g g b %f, %f, %f, %f", stGsWbAutoGain.rgain,
             stGsWbAutoGain.grgain, stGsWbAutoGain.gbgain,
             stGsWbAutoGain.bgain);
  ret = rk_aiq_uapi2_setMWBGain(gstIspHandle[u32CamId].pstAiqCtx, &gain);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_GET_WhiteBalanceGain(RKADK_U32 u32CamId,
                                      rk_aiq_wb_gain_t *gain) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  RKADK_LOGP("Rgain = %f, Grgain = %f, Gbgain = %f, Bgain = %f", gain->rgain,
             gain->grgain, gain->gbgain, gain->bgain);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);
  ret = rk_aiq_uapi2_getWBGain(gstIspHandle[u32CamId].pstAiqCtx, gain);
  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);

  return ret;
}

// mode 0:off, 1:2d, 2:3d, 3: 2d+3d
int SAMPLE_ISP_SET_DNRStrength(RKADK_U32 u32CamId, RKADK_U32 u32Mode,
                                 RKADK_U32 u322DValue, RKADK_U32 u323Dvalue) {
  int ret = 0;
  RKADK_U32 u322DnrDefaultLevel = 50;
  RKADK_U32 u323DnrDefaultLevel = 50;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  RKADK_U32 u32_2d_value = (u322DValue / 128.0f) * u322DnrDefaultLevel;
  RKADK_U32 u32_3d_value = (u323Dvalue / 128.0f) * u323DnrDefaultLevel;
  RKADK_LOGP("mode = %d n_2d_value = %d n_3d_value = %d", u32Mode, u322DValue,
             u323Dvalue);
  RKADK_LOGP("u_2d_value = %d n_3d_value = %d", u32_2d_value, u32_3d_value);
  RKADK_LOGP("u322DnrDefaultLevel = %d u323DnrDefaultLevel = %d",
             u322DnrDefaultLevel, u323DnrDefaultLevel);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  if (u32Mode == 0) {
    ret = rk_aiq_uapi2_sysctl_setModuleCtl(gstIspHandle[u32CamId].pstAiqCtx,
                                          RK_MODULE_NR, true); // 2D
    ret |= rk_aiq_uapi2_sysctl_setModuleCtl(gstIspHandle[u32CamId].pstAiqCtx,
                                           RK_MODULE_TNR, true); // 3D
    ret |= rk_aiq_uapi2_setMSpaNRStrth(gstIspHandle[u32CamId].pstAiqCtx, true,
                                      u322DnrDefaultLevel); //[0,100]
    ret |= rk_aiq_uapi2_setMTNRStrth(gstIspHandle[u32CamId].pstAiqCtx, true,
                                    u323DnrDefaultLevel); //[0,100]
  } else if (u32Mode == 1) {                              // 2D
    ret = rk_aiq_uapi2_sysctl_setModuleCtl(gstIspHandle[u32CamId].pstAiqCtx,
                                          RK_MODULE_NR, true); // 2D
    ret |= rk_aiq_uapi2_sysctl_setModuleCtl(gstIspHandle[u32CamId].pstAiqCtx,
                                           RK_MODULE_TNR, true); // 3D
    ret |= rk_aiq_uapi2_setMSpaNRStrth(gstIspHandle[u32CamId].pstAiqCtx, true,
                                      u32_2d_value); //[0,255] -> [0,100]
    ret |= rk_aiq_uapi2_setMTNRStrth(gstIspHandle[u32CamId].pstAiqCtx, true,
                                    u323DnrDefaultLevel);
  } else if (u32Mode == 2) { // 3D
    ret = rk_aiq_uapi2_sysctl_setModuleCtl(gstIspHandle[u32CamId].pstAiqCtx,
                                          RK_MODULE_NR, true); // 2D
    ret |= rk_aiq_uapi2_sysctl_setModuleCtl(gstIspHandle[u32CamId].pstAiqCtx,
                                           RK_MODULE_TNR, true); // 3D
    ret |= rk_aiq_uapi2_setMSpaNRStrth(gstIspHandle[u32CamId].pstAiqCtx, true,
                                      u322DnrDefaultLevel); //[0,100]->[0,255]
    ret |= rk_aiq_uapi2_setMTNRStrth(gstIspHandle[u32CamId].pstAiqCtx, true,
                                    u32_3d_value);
  } else if (u32Mode == 3) { // 2D+3D
    ret = rk_aiq_uapi2_sysctl_setModuleCtl(gstIspHandle[u32CamId].pstAiqCtx,
                                          RK_MODULE_NR, true); // 2D
    ret |= rk_aiq_uapi2_sysctl_setModuleCtl(gstIspHandle[u32CamId].pstAiqCtx,
                                           RK_MODULE_TNR, true); // 3D
    ret |= rk_aiq_uapi2_setMSpaNRStrth(gstIspHandle[u32CamId].pstAiqCtx, true,
                                      u32_2d_value); //[0,255] -> [0,100]
    ret |= rk_aiq_uapi2_setMTNRStrth(gstIspHandle[u32CamId].pstAiqCtx, true,
                                    u32_3d_value); //[0,255] -> [0,100]
  }

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_GET_DNRStrength(RKADK_U32 u32CamId, RKADK_U32 *u322DValue,
                                 RKADK_U32 *u323Dvalue) {
  int ret = 0;
  bool on;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  ret = rk_aiq_uapi2_getMSpaNRStrth(gstIspHandle[u32CamId].pstAiqCtx, &on,
                                   u322DValue);
  ret |= rk_aiq_uapi2_getMTNRStrth(gstIspHandle[u32CamId].pstAiqCtx, &on,
                                  u323Dvalue);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_Flicker(RKADK_U32 u32CamId, unsigned char u32Flicker) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  if (u32Flicker == 0) {
    // NTSC(60HZ)
    ret = rk_aiq_uapi2_setExpPwrLineFreqMode(gstIspHandle[u32CamId].pstAiqCtx,
                                            EXP_PWR_LINE_FREQ_60HZ);
  } else if (u32Flicker == 1) {
    // PAL(50HZ)
    ret = rk_aiq_uapi2_setExpPwrLineFreqMode(gstIspHandle[u32CamId].pstAiqCtx,
                                            EXP_PWR_LINE_FREQ_50HZ);
  }

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

// mode 0 0ff 1 manual 2 auto
int SAMPLE_ISP_EnableDefog(RKADK_U32 u32CamId, bool on, opMode_t mode,
                             RKADK_U32 level) {
  int ret = 0;
  adehaze_sw_v12_t attr;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  RKADK_LOGP("defog on = %d, mode = %d, level = %d", on, mode, level);

  if (on && mode == 1) {
    attr.mode = DEHAZE_API_MANUAL;
    if (level < 1)
      level = 1;
    else if (level > 100)
      level = 100;
  } else {
    attr.mode = DEHAZE_API_AUTO;
  }
  ret = rk_aiq_user_api2_adehaze_v12_setSwAttrib(gstIspHandle[u32CamId].pstAiqCtx, &attr);
  if (on && mode == 1)
    ret = rk_aiq_uapi2_setMDehazeStrth(gstIspHandle[u32CamId].pstAiqCtx, level);

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_GET_MDhzStrth(RKADK_U32 u32CamId, bool *on, RKADK_U32 *level) {
  int ret = 0;
  adehaze_sw_v12_t attr;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);
  ret = rk_aiq_user_api2_adehaze_v12_getSwAttrib(gstIspHandle[u32CamId].pstAiqCtx, &attr);
  if (attr.mode == DEHAZE_API_MANUAL)
    *on = true;
  else
    *on =false;
  if (*on)
   ret = rk_aiq_uapi2_getMDehazeStrth(gstIspHandle[u32CamId].pstAiqCtx, level);
  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_Correction(RKADK_U32 u32CamId, RKADK_S32 u32Mode,
                                RKADK_U32 u32Value) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);

  if (gstIspHandle[u32CamId].gsLDCMode != u32Mode) {
    if (u32Mode) {
      ret = rk_aiq_uapi2_setLdchEn(gstIspHandle[u32CamId].pstAiqCtx, true);
    } else {
      ret = rk_aiq_uapi2_setLdchEn(gstIspHandle[u32CamId].pstAiqCtx, false);
    }
    gstIspHandle[u32CamId].gsLDCMode = u32Mode;
  }

  if (u32Mode) {
    ret |= rk_aiq_uapi2_setLdchCorrectLevel(gstIspHandle[u32CamId].pstAiqCtx,
                                           (u32Value < 2 ? 2 : u32Value));
  }

  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SET_Crop(RKADK_U32 u32CamId, rk_aiq_rect_t rect) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);
  ret = rk_aiq_uapi2_sysctl_setCrop(gstIspHandle[u32CamId].pstAiqCtx, rect);
  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_Get_AiqHandle(RKADK_U32 u32CamId, RKADK_MW_PTR *ppAiqCtx) {
  int ret = 0;

  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  pthread_mutex_lock(&gstIspHandle[u32CamId].aiqCtxMutex);
  *ppAiqCtx = (RKADK_MW_PTR)gstIspHandle[u32CamId].pstAiqCtx;
  pthread_mutex_unlock(&gstIspHandle[u32CamId].aiqCtxMutex);
  return ret;
}

int SAMPLE_ISP_SingleFrame(RKADK_U32 u32CamId) {
  if (u32CamId >= RKADK_MAX_SENSOR_CNT) {
    printf("Invalid u32CamId[%d]", u32CamId);
    return -1;
  }

  if (!gstIspHandle[u32CamId].pstAiqCtx) {
    printf("pstAiqCtx not init");
    return -1;
  }

  RKADK_LOGP("enter rk_aiq_uapi2_sysctl_pause");
  rk_aiq_uapi2_sysctl_pause(gstIspHandle[u32CamId].pstAiqCtx, true);
  return 0;
}

int SAMPLE_ISP_MultiFrame(RKADK_U32 u32CamId) {
  if (u32CamId >= RKADK_MAX_SENSOR_CNT) {
    printf("Invalid u32CamId[%d]", u32CamId);
    return -1;
  }

  if (!gstIspHandle[u32CamId].pstAiqCtx) {
    printf("pstAiqCtx not init");
    return -1;
  }

  RKADK_LOGP("enter rk_aiq_uapi2_sysctl_resume");
  rk_aiq_uapi2_sysctl_resume(gstIspHandle[u32CamId].pstAiqCtx);
  return 0;
}

int SAMPLE_ISP_GetAINrParams(RKADK_U32 u32CamId, rk_ainr_param *param) {
  int ret;

  RKADK_CHECK_POINTER(param, RKADK_FAILURE);
  RKADK_CHECK_CAMERAID(u32CamId, RKADK_FAILURE);
  RKADK_CHECK_INIT(gstIspHandle[u32CamId].pstAiqCtx, RKADK_FAILURE);

  ret = rk_aiq_uapi2_sysctl_getAinrParams(gstIspHandle[u32CamId].pstAiqCtx, param);
  if (ret) {
    printf("rk_aiq_uapi2_sysctl_getAinrParams failed[%x]", ret);
    return ret;
  }
  return 0;
}

#endif
