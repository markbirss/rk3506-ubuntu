/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_ASRC_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup ASRC
 *  @{
 */

#ifndef _HAL_ASRC_
#define _HAL_ASRC_
#include "hal_audio.h"
#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup ASRC_Exported_Definition_Group1 Basic Definition
 *  @{
 */
#define ASRC_FIFO_IN_INCR_DR  0x1000
#define ASRC_FIFO_OUT_INCR_DR 0x5000

/***************************** Structure Definition **************************/

typedef enum {
    LRCK_SEL_MCLK_ASRC0  = 0,
    LRCK_SEL_MCLK_ASRC1  = 1,
    LRCK_SEL_MCLK_ASRC2  = 2,
    LRCK_SEL_MCLK_ASRC3  = 3,
    LRCK_SEL_MCLK_ASRC4  = 0,
    LRCK_SEL_MCLK_ASRC5  = 1,
    LRCK_SEL_MCLK_ASRC6  = 2,
    LRCK_SEL_MCLK_ASRC7  = 3,
    LRCK_SEL_SPDIF_RX0   = 4,
    LRCK_SEL_SPDIF_RX1   = 5,
    LRCK_SEL_SPDIF_TX    = 4,
    LRCK_SEL_PDM_CLK     = 5,
    LRCK_SEL_SAI0_AUDIO0 = 6,
    LRCK_SEL_SAI1_AUDIO0 = 7,
    LRCK_SEL_SAI2_AUDIO0 = 8,
    LRCK_SEL_SAI3_AUDIO0 = 9,
    LRCK_SEL_SAI4_AUDIO0 = 10,
    LRCK_SEL_SAI4_AUDIO1 = 6,
    LRCK_SEL_SAI5_AUDIO1 = 7,
    LRCK_SEL_SAI6_AUDIO1 = 8,
    LRCK_SEL_SAI7_AUDIO1 = 9,
    LRCK_SEL_SAI0_AUDIO1 = 10,
    LRCK_SEL_INVALID     = 15,
} eASRC_lrckSel;

/**
 * enum eASRC_seriesMode - set asrc series mode.
 */
typedef enum {
    ASRC_SERIES_DIS = 0,
    ASRC_SERIES_MASTER,
    ASRC_SERIES_SLAVE,
} eASRC_seriesMode;

struct ASRC_PARAMS {
    eAUDIO_sampleRate sampleRate;
    eAUDIO_sampleBits sampleBits;
    eASRC_lrckSel lrckMux;
    uint32_t lrckDiv;
};

struct HAL_ASRC_DEV {
    struct ASRC_REG *pReg;
    struct AUDIO_DMA_DATA rxDmaData;
    struct AUDIO_DMA_DATA txDmaData;
    eCLOCK_Name mclk;
    eCLOCK_Name lrckRX;
    eCLOCK_Name lrckTX;
    eASRC_seriesMode series;
    eASRC_mode mode;
    IRQn_Type irqNum;
    uint32_t fetchLength;
    uint32_t spinLockId;
    uint32_t mclkGate;
    uint32_t mclkRate;
    uint32_t hrst;
    uint16_t maxChannels;
    uint16_t channels;
    bool groupLink;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup ASRC_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_ASRC_Init(struct HAL_ASRC_DEV *asrc, struct AUDIO_INIT_CONFIG *config);
HAL_Status HAL_ASRC_DeInit(struct HAL_ASRC_DEV *asrc);
HAL_Status HAL_ASRC_Start(struct HAL_ASRC_DEV *asrc);
HAL_Status HAL_ASRC_Stop(struct HAL_ASRC_DEV *asrc);
HAL_Status HAL_ASRC_Config(struct HAL_ASRC_DEV *asrc, struct ASRC_PARAMS *rxParams,
                           struct ASRC_PARAMS *txParams);
HAL_Status HAL_ASRC_SelectSeriesMode(struct HAL_ASRC_DEV *asrc, eASRC_seriesMode mode);
HAL_Status HAL_ASRC_HardMuteByLane(struct HAL_ASRC_DEV *asrc, uint8_t lane);
HAL_Status HAL_ASRC_HardUnmuteByLane(struct HAL_ASRC_DEV *asrc, uint8_t lane);
HAL_Status HAL_ASRC_SoftMuteByLane(struct HAL_ASRC_DEV *asrc, uint8_t lane);
HAL_Status HAL_ASRC_SoftUnmuteByLane(struct HAL_ASRC_DEV *asrc, uint8_t lane);
uint32_t HAL_ASRC_GetOwner(struct HAL_ASRC_DEV *asrc);
HAL_Check HAL_ASRC_TryLock(struct HAL_ASRC_DEV *asrc);
void HAL_ASRC_UnLock(struct HAL_ASRC_DEV *asrc);

/** @} */
#endif

/** @} */

/** @} */

#endif /* HAL_ASRC_MODULE_ENABLED */
