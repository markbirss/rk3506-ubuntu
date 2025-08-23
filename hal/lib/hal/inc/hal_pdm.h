/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_conf.h"

#ifdef HAL_PDM_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup PDM
 *  @{
 */

#ifndef _HAL_PDM_H_
#define _HAL_PDM_H_

#include "hal_audio.h"
#include "hal_def.h"

/***************************** MACRO Definition ******************************/
/** @defgroup PDM_Exported_Definition_Group1 Basic Definition
 *  @{
 */

/** PDM CLK RATE */
#define PDM_FPGA_CLK_RATE (72000000)
#define PDM_CLK_RATE      (100000000)
/***************************** Structure Definition **************************/

/** pdm dev struct */
struct HAL_PDM_DEV {
    struct PDM_REG *pReg;
    eCLOCK_Name mclk;
    uint32_t mclkGate;
    uint32_t mclkRate;
    eCLOCK_Name clkOut;
    uint32_t clkOutRate;
    eCLOCK_Name hclk;
    uint32_t reset;
    uint32_t gain;
    ePD_Id pd;
    ePDM_mode mode;
    struct AUDIO_DMA_DATA rxDmaData;
};

/** @} */
/***************************** Function Declare ******************************/
/** @defgroup PDM_Public_Function_Declare Public Function Declare
 *  @{
 */

HAL_Status HAL_PDM_Supsend(struct HAL_PDM_DEV *pdm);
HAL_Status HAL_PDM_Resume(struct HAL_PDM_DEV *pdm);
HAL_Status HAL_PDM_Init(struct HAL_PDM_DEV *pdm, struct AUDIO_INIT_CONFIG *config);
HAL_Status HAL_PDM_DeInit(struct HAL_PDM_DEV *pdm);
HAL_Status HAL_PDM_Enable(struct HAL_PDM_DEV *pdm);
HAL_Status HAL_PDM_Disable(struct HAL_PDM_DEV *pdm);
HAL_Status HAL_PDM_Config(struct HAL_PDM_DEV *pdm, struct AUDIO_PARAMS *params);
#if ((PDM_VERSION == 0x23112118U) || (PDM_VERSION == 0x23023576U))
HAL_Status HAL_PDM_MuteByChannel(struct HAL_PDM_DEV *pdm, uint8_t channel);
HAL_Status HAL_PDM_UnmuteByChannel(struct HAL_PDM_DEV *pdm, uint8_t channel);
HAL_Status HAL_PDM_SetGain(struct HAL_PDM_DEV *pdm, uint32_t gain);
uint32_t HAL_PDM_GetGain(struct HAL_PDM_DEV *pdm);
#else
__STATIC_INLINE HAL_Status HAL_PDM_MuteByChannel(struct HAL_PDM_DEV *pdm, uint8_t channel)
{
    return 0;
}

__STATIC_INLINE HAL_Status HAL_PDM_UnmuteByChannel(struct HAL_PDM_DEV *pdm, uint8_t channel)
{
    return 0;
}

__STATIC_INLINE HAL_Status HAL_PDM_SetGain(struct HAL_PDM_DEV *pdm, uint32_t gain)
{
    return 0;
}

__STATIC_INLINE uint32_t HAL_PDM_GetGain(struct HAL_PDM_DEV *pdm)
{
    return 0;
}

#endif

/** @} */

#endif

/** @} */

/** @} */

#endif /* HAL_PDM_MODULE_ENABLED */
