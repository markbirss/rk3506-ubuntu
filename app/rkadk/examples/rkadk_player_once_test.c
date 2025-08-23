/*
 * Copyright (c) 2021 Rockchip, Inc. All Rights Reserved.
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

#include "rkadk_common.h"
#include "rkadk_media_comm.h"
#include "rkadk_log.h"
#include "rkadk_player.h"
#include "rkadk_demuxer.h"
#ifndef OS_RTT
#include "rkdemuxer.h"
#else
#include "../../common_algorithm/misc/include/rkdemuxer.h"
#endif
#include <math.h>

#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

extern int optind;
extern char *optarg;
static bool is_quit = false;
static RKADK_CHAR optstr[] = "i:x:y:W:H:r:a:s:P:I:F:l:O:S:w:C:mfvbh";

static void print_usage(const RKADK_CHAR *name) {
  printf("usage example:\n");
  printf("\t%s [-i xxx.mp4] [-x 180] [-y 320] [-W 360] [-H 640] [-r 90] "
         "[-m] [-f] [-v] [-b]\n",
         name);
  printf("\t-i: input url, Default: /etc/bsa_file/8k8bpsMono.wav\n");
  printf("\t-x: display x coordinate, Default: 0\n");
  printf("\t-y: display y coordinate, Default: 0\n");
  printf("\t-W: display width, Default: Physical screen width\n");
  printf("\t-H: display height, Default: Physical screen height\n");
  printf("\t-r: rotation, option: 0(0), 1(90), 2(180), 3(270), Default: 0\n");
  printf("\t-m: mirror enable, Default: disable\n");
  printf("\t-f: flip enable, Default: disable\n");
  printf("\t-a: set audio enable/disable, option: 0, 1; Default: enable\n");
  printf("\t-v: video enable, Default: disable\n");
  printf("\t-s: vo layer splice mode, option: 0(RGA), 1(GPU), 2(ByPass); Default: 0\n");
  printf("\t-P: display pixel type, option: 0(RGB888), 1(NV12), 2(RGB565), 3(RGB444); Default: 0\n");
  printf("\t-I: Type of a VO interface, option: 0(DEFAILT), 1(MIPI), 2(LCD); Default: 1106: 0, other chip: 1\n");
  printf("\t-F: vo display framerete, Default: 30\n");
  printf("\t-b: Black Backgound enable, Default: disable\n");
  printf("\t-l: vo layer id, Default: 0\n");
  printf("\t-O: Vdec output buffer count, Default: 3\n");
  printf("\t-S: Vdec stream(input) buffer count, Default: 3\n");
  printf("\t-w: Vdec waterline(frames), Default: 0\n");
  printf("\t-C: Ao sound card name, Default: RV1106/RV1103/RK3506 = hw:0,0, other chip = default\n");
  printf("\t-h: help\n");
}

static void sigterm_handler(int sig) {
  fprintf(stderr, "signal %d\n", sig);
  is_quit = true;
}

static RKADK_VOID PlayerEventFnTest(RKADK_MW_PTR pPlayer,
                                    RKADK_PLAYER_EVENT_E enEvent,
                                    RKADK_VOID *pData) {
  switch (enEvent) {
  case RKADK_PLAYER_EVENT_STATE_CHANGED:
    printf("+++++ RKADK_PLAYER_EVENT_STATE_CHANGED +++++\n");
    break;
  case RKADK_PLAYER_EVENT_EOF:
    printf("+++++ RKADK_PLAYER_EVENT_EOF +++++\n");
    is_quit = true;
    break;
  case RKADK_PLAYER_EVENT_SOF:
    printf("+++++ RKADK_PLAYER_EVENT_SOF +++++\n");
    break;
  case RKADK_PLAYER_EVENT_SEEK_END:
    printf("+++++ RKADK_PLAYER_EVENT_SEEK_END +++++\n");
    break;
  case RKADK_PLAYER_EVENT_ERROR:
    printf("+++++ RKADK_PLAYER_EVENT_ERROR +++++\n");
    break;
  case RKADK_PLAYER_EVENT_PREPARED:
    printf("+++++ RKADK_PLAYER_EVENT_PREPARED +++++\n");
    break;
  case RKADK_PLAYER_EVENT_PLAY:
    printf("+++++ RKADK_PLAYER_EVENT_PLAY +++++\n");
    break;
  case RKADK_PLAYER_EVENT_PAUSED:
    printf("+++++ RKADK_PLAYER_EVENT_PAUSED +++++\n");
    break;
  case RKADK_PLAYER_EVENT_STOPPED:
    printf("+++++ RKADK_PLAYER_EVENT_STOPPED +++++\n");
    break;
  default:
    printf("+++++ Unknown event(%d) +++++\n", enEvent);
    break;
  }
}

void param_init(RKADK_PLAYER_FRAME_INFO_S *pstFrmInfo) {
  RKADK_CHECK_POINTER_N(pstFrmInfo);

  memset(pstFrmInfo, 0, sizeof(RKADK_PLAYER_FRAME_INFO_S));
  pstFrmInfo->u32DispWidth = 720;
  pstFrmInfo->u32DispHeight = 1280;
  pstFrmInfo->u32ImgWidth = pstFrmInfo->u32DispWidth;
  pstFrmInfo->u32ImgHeight = pstFrmInfo->u32DispHeight;
  pstFrmInfo->u32VoFormat = VO_FORMAT_RGB888;
#if defined(RK3506) || defined(RV1106_1103)
  pstFrmInfo->u32EnIntfType = DISPLAY_TYPE_DEFAULT;
#else
  pstFrmInfo->u32EnIntfType = DISPLAY_TYPE_MIPI;
#endif
  pstFrmInfo->enIntfSync = RKADK_VO_OUTPUT_DEFAULT;
  pstFrmInfo->u32BorderColor = 0x0000FA;
  pstFrmInfo->bMirror = RKADK_FALSE;
  pstFrmInfo->bFlip = RKADK_FALSE;
  pstFrmInfo->u32Rotation = 1;
  pstFrmInfo->stSyncInfo.bIdv = RKADK_TRUE;
  pstFrmInfo->stSyncInfo.bIhs = RKADK_TRUE;
  pstFrmInfo->stSyncInfo.bIvs = RKADK_TRUE;
  pstFrmInfo->stSyncInfo.bSynm = RKADK_TRUE;
  pstFrmInfo->stSyncInfo.bIop = RKADK_TRUE;
  pstFrmInfo->stSyncInfo.u16FrameRate = 30;
  pstFrmInfo->stSyncInfo.u16PixClock = 65000;
  pstFrmInfo->stSyncInfo.u16Hact = 1200;
  pstFrmInfo->stSyncInfo.u16Hbb = 24;
  pstFrmInfo->stSyncInfo.u16Hfb = 240;
  pstFrmInfo->stSyncInfo.u16Hpw = 136;
  pstFrmInfo->stSyncInfo.u16Hmid = 0;
  pstFrmInfo->stSyncInfo.u16Vact = 1200;
  pstFrmInfo->stSyncInfo.u16Vbb = 200;
  pstFrmInfo->stSyncInfo.u16Vfb = 194;
  pstFrmInfo->stSyncInfo.u16Vpw = 6;
  pstFrmInfo->enVoSpliceMode = SPLICE_MODE_RGA;

  return;
}

RKADK_VOID *GetPosition(RKADK_VOID *arg) {
    RKADK_S64 position = 0;
    while (!is_quit) {
      position = RKADK_PLAYER_GetCurrentPosition(arg);
      printf("position = %lld\n", position);
      usleep(1000000);
    }

  return NULL;
}

int main(int argc, char *argv[]) {
  int c, ret;
  char *file = "/userdata/16000_2.mp3";
  RKADK_BOOL bVideoEnable = false;
  RKADK_BOOL bAudioEnable = true;
  RKADK_BOOL bBlackBackgroundEnable = false;
  RKADK_MW_PTR pPlayer = NULL;
  pthread_t getPosition = 0;
  int u32VoFormat = -1, u32SpliceMode = -1, u32IntfType = -1;
  RKADK_PLAYER_CFG_S stPlayCfg;
  RKADK_U32 u32Waterline = 0;

  memset(&stPlayCfg, 0, sizeof(RKADK_PLAYER_CFG_S));
  param_init(&stPlayCfg.stFrmInfo);

#if defined(RK3506) || defined(RV1106_1103)
  stPlayCfg.stAudioCfg.pSoundCard = "hw:0,0";
#else
  stPlayCfg.stAudioCfg.pSoundCard = "default";
#endif

  while ((c = getopt(argc, argv, optstr)) != -1) {
    switch (c) {
    case 'i':
      file = optarg;
      break;
    case 'x':
      stPlayCfg.stFrmInfo.u32FrmInfoX = atoi(optarg);
      break;
    case 'y':
      stPlayCfg.stFrmInfo.u32FrmInfoY = atoi(optarg);
      break;
    case 'W':
      stPlayCfg.stFrmInfo.u32DispWidth = atoi(optarg);
      break;
    case 'H':
      stPlayCfg.stFrmInfo.u32DispHeight = atoi(optarg);
      break;
    case 'r':
      stPlayCfg.stFrmInfo.u32Rotation = atoi(optarg);
      break;
    case 'F':
      stPlayCfg.stFrmInfo.stSyncInfo.u16FrameRate = atoi(optarg);
      break;
    case 'm':
      stPlayCfg.stFrmInfo.bMirror = true;
      break;
    case 'f':
      stPlayCfg.stFrmInfo.bFlip = true;
      break;
    case 'l':
      stPlayCfg.stFrmInfo.u32VoLay = atoi(optarg);
      break;
    case 'O':
      stPlayCfg.stVdecCfg.u32FrameBufCnt = atoi(optarg);
      break;
    case 'S':
      stPlayCfg.stVdecCfg.u32StreamBufCnt = atoi(optarg);
      break;
    case 'w':
      u32Waterline = atoi(optarg);
      break;
    case 'v':
      bVideoEnable = true;
      break;
    case 'b':
      bBlackBackgroundEnable = true;
      break;
    case 'a':
      bAudioEnable = atoi(optarg);
      break;
    case 's':
      u32SpliceMode = atoi(optarg);
      break;
    case 'P':
      u32VoFormat = atoi(optarg);
      break;
    case 'I':
      u32IntfType = atoi(optarg);
      break;
    case 'C':
      stPlayCfg.stAudioCfg.pSoundCard = optarg;
      break;
    case 'h':
    default:
      print_usage(argv[0]);
      optind = 0;
      return 0;
    }
  }
  optind = 0;

  RKADK_LOGP("#play file: %s, bVideoEnable: %d, bAudioEnable: %d",file, bVideoEnable, bAudioEnable);
  RKADK_LOGP("#video display rect[%d, %d, %d, %d], u32SpliceMode: %d, u32VoFormat: %d, fps: %d, pSoundCard: %s",
              stPlayCfg.stFrmInfo.u32FrmInfoX, stPlayCfg.stFrmInfo.u32FrmInfoY,
              stPlayCfg.stFrmInfo.u32DispWidth, stPlayCfg.stFrmInfo.u32DispHeight,
              u32SpliceMode, u32VoFormat, stPlayCfg.stFrmInfo.stSyncInfo.u16FrameRate,
              stPlayCfg.stAudioCfg.pSoundCard);
  RKADK_LOGP("u32Waterline: %d, u32FrameBufCnt: %d, u32StreamBufCnt: %d",
              u32Waterline, stPlayCfg.stVdecCfg.u32FrameBufCnt, stPlayCfg.stVdecCfg.u32StreamBufCnt);

  if (u32SpliceMode == 1)
    stPlayCfg.stFrmInfo.enVoSpliceMode = SPLICE_MODE_GPU;
  else if (u32SpliceMode == 2)
    stPlayCfg.stFrmInfo.enVoSpliceMode = SPLICE_MODE_BYPASS;

  if (u32VoFormat == 1)
    stPlayCfg.stFrmInfo.u32VoFormat = VO_FORMAT_NV12;
  else if (u32VoFormat == 2)
    stPlayCfg.stFrmInfo.u32VoFormat = VO_FORMAT_RGB565;
  else if (u32VoFormat == 3)
    stPlayCfg.stFrmInfo.u32VoFormat = VO_FORMAT_RGB444;

  if (u32IntfType == 1)
    stPlayCfg.stFrmInfo.u32EnIntfType = DISPLAY_TYPE_MIPI;
  else if (u32IntfType == 2)
    stPlayCfg.stFrmInfo.u32EnIntfType = DISPLAY_TYPE_LCD;

  signal(SIGINT, sigterm_handler);

  RKADK_MPI_SYS_Init();

  if (bAudioEnable)
    stPlayCfg.bEnableAudio = true;
  if (bVideoEnable)
    stPlayCfg.bEnableVideo = true;
  if (bBlackBackgroundEnable)
    stPlayCfg.bEnableBlackBackground = true;

  stPlayCfg.pfnPlayerCallback = PlayerEventFnTest;
  stPlayCfg.stAudioCfg.u32SpeakerVolume = 70;

  if (RKADK_PLAYER_Create(&pPlayer, &stPlayCfg)) {
    RKADK_LOGE("RKADK_PLAYER_Create failed");
    return -1;
  }

  if (u32Waterline > 0)
    RKADK_PLAYER_SetVdecWaterline(pPlayer, u32Waterline);

  ret = RKADK_PLAYER_SetDataSource(pPlayer, file);
  if (ret) {
    RKADK_LOGE("SetDataSource failed, ret = %d", ret);
    goto __EXIT;
  }

  ret = RKADK_PLAYER_Prepare(pPlayer);
  if (ret) {
    RKADK_LOGE("Prepare failed, ret = %d", ret);
    goto __EXIT;
  }


  ret = RKADK_PLAYER_Play(pPlayer);
  if (ret) {
    RKADK_LOGE("Play failed, ret = %d", ret);
    return -1;
  }

  //pthread_create(&getPosition, 0, GetPosition, pPlayer);

  while (!is_quit) {
    usleep(100000);
  }

__EXIT:

  RKADK_PLAYER_Destroy(pPlayer);

  //if (getPosition)
  //  pthread_join(getPosition, RKADK_NULL);

  pPlayer = NULL;
  RKADK_MPI_SYS_Exit();
  return 0;
}
