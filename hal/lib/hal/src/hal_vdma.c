/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_VDMA_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup VDMA
 *  @{
 */

/** @defgroup VDMA_How_To_Use How To Use
 *  @{

 The VDMA driver can be used as follows:

 - Invoke HAL_VDMA_Init() to initialize vdma.
 - Invoke HAL_VDMA_Config() to config samplerate, width.
 - Incoke HAL_VDMA_PrepDmaCyclic() to setup vdma cyclic.
 - Invoke HAL_VDMA_Enable() to enable vdma.
 - Invoke HAL_VDMA_Disable() to disable vdma.
 - Invoke HAL_VDMA_DeInit() to deinitialize vdma.
 - More details refer to APIs' descriptions as below.

 @} */

/** @defgroup VDMA_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
/* CONTROL */
#define VDMA_EN                  (1 << VDMA_CONTROL_VDMA_EN_SHIFT)
#define VDMA_DIS                 (0 << VDMA_CONTROL_VDMA_EN_SHIFT)
#define VDMA_SOURCE_PDM0         (0 << VDMA_CONTROL_SOURCE_SELECT_SHIFT)
#define VDMA_SOURCE_BURST8       (5 << VDMA_CONTROL_SOURCE_BURST_SHIFT)
#define VDMA_SOURCE_BURST_NUM(x) ((x - 1) << VDMA_CONTROL_SOURCE_BURST_NUM_SHIFT)
#define VDMA_INCR_LENGTH(x)      (x << VDMA_CONTROL_INCR_LENGTH_SHIFT)
#define VMDA_ADDR_INCR           (0 << VDMA_CONTROL_SOURCE_FIXADDR_EN_SHIFT)
#define VMDA_ADDR_FIX            (1 << VDMA_CONTROL_SOURCE_FIXADDR_EN_SHIFT)
#define VDMA_MODE_DATA_STORE     (1 << VDMA_CONTROL_VDMA_MODE_SHIFT)
#define VDMA_MODE_DATA_NOT_STORE (2 << VDMA_CONTROL_VDMA_MODE_SHIFT)
#define VDMA_CHANNELS(x)         ((x - 1) << VDMA_CONTROL_VOICE_CHANNEL_NUM_SHIFT)
#define VDMA_CHANNEL_WITH_16     (0 << VDMA_CONTROL_VOICE_CHANNEL_BITWIDTH_SHIFT)
#define VDMA_CHANNEL_WITH_24     (1 << VDMA_CONTROL_VOICE_CHANNEL_BITWIDTH_SHIFT)
#define VDMA_HIGH_24BIT_VALID    (0 << VDMA_CONTROL_VOICE_24BIT_ALIGN_MODE_SHIFT)
#define VDMA_LOW_24BIT_VALID     (1 << VDMA_CONTROL_VOICE_24BIT_ALIGN_MODE_SHIFT)

/* AUX_CON0 */
#define VDMA_BUS_WRITE_EN                   (1 << VDMA_AUX_CON0_BUS_WRITE_EN_SHIFT)
#define VDMA_BUS_WRITE_DIS                  (0 << VDMA_AUX_CON0_BUS_WRITE_EN_SHIFT)
#define VDMA_RAM_INTERFACE_EN               (0 << VDMA_AUX_CON0_DIS_RAM_ITF_SHIFT)
#define VDMA_RAM_INTERFACE_DIS              (1 << VDMA_AUX_CON0_DIS_RAM_ITF_SHIFT)
#define VDMA_DATA_TRANS_TRIG_INT_EN         (1 << VDMA_AUX_CON0_DATA_TRANS_TRIG_INT_EN_SHIFT)
#define VDMA_DATA_TRANS_TRIG_INT_DIS        (0 << VDMA_AUX_CON0_DATA_TRANS_TRIG_INT_EN_SHIFT)
#define VDMA_BUS_WRITE_ADDRESS_EN           (1 << VDMA_AUX_CON0_BUS_WRITE_ADDR_MODE_SHIFT)
#define VDMA_BUS_AHB_REWORK_FROM_SATRT_ADDR (1 << VDMA_AUX_CON0_BUS_WRITE_REWORK_ADDR_MODE_SHIFT)
#define VDMA_RAM_REWORK_FROM_SATRT_ADDR     (1 << VDMA_AUX_CON0_RAM_WRITE_REWORK_ADDR_MODE_SHIFT)

/* AUX_CON1 */
#define VDMA_INT_DATA_TRANS_KBYTE (0 << VDMA_AUX_CON1_DATA_TRANS_INT_MODE_SEL_SHIFT)
#define VDMA_INT_DATA_TRANS_WORD  (1 << VDMA_AUX_CON1_DATA_TRANS_INT_MODE_SEL_SHIFT)
#define VDMA_DATA_TRANS_WORD(x)   ((x - 1) << VDMA_AUX_CON1_DATA_TRANS_WORD_THD_SHIFT)

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup VDMA_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 ...to do or delete this row

 *  @{
 */

/**
 * @brief  vdma suspend.
 * @param  vdma: the handle of vdma.
 * @return HAL_Status
 */
HAL_Status HAL_VDMA_Supsend(struct HAL_VDMA_DEV *vdma)
{
    return HAL_OK;
}

/**
 * @brief  vdma resume.
 * @param  vdma: the handle of vdma.
 * @return HAL_Status
 */
HAL_Status HAL_VDMA_Resume(struct HAL_VDMA_DEV *vdma)
{
    return HAL_OK;
}

/** @} */

/** @defgroup VDMA_Exported_Functions_Group2 State and Errors Functions

 This section provides functions allowing to get the status of the module:

 *  @{
 */

/** @} */

/** @defgroup VDMA_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

/** @} */

/** @defgroup VDMA_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 ...to do or delete this row

 *  @{
 */

/**
 * @brief  Init vdma controller.
 * @param  vdma: the handle of vdma.
 * @param  config: init config.
 * @return HAL_Status
 */
HAL_Status HAL_VDMA_Init(struct HAL_VDMA_DEV *vdma, struct AUDIO_INIT_CONFIG *config)
{
    struct VDMA_REG *reg = vdma->pReg;

    HAL_ASSERT(IS_VDMA_INSTANCE(reg));

    MODIFY_REG(reg->VS_ADDR,
               VDMA_VS_ADDR_VS_ADDR_MASK,
               vdma->vsAddr);
    MODIFY_REG(reg->AUX_CON0,
               VDMA_AUX_CON0_BUS_WRITE_REWORK_ADDR_MODE_MASK |
               VDMA_AUX_CON0_BUS_WRITE_ADDR_MODE_MASK |
               VDMA_AUX_CON0_DATA_TRANS_TRIG_INT_EN_MASK |
               VDMA_AUX_CON0_DIS_RAM_ITF_MASK |
               VDMA_AUX_CON0_BUS_WRITE_EN_MASK,
               VDMA_BUS_AHB_REWORK_FROM_SATRT_ADDR |
               VDMA_BUS_WRITE_ADDRESS_EN |
               VDMA_DATA_TRANS_TRIG_INT_EN |
               VDMA_RAM_INTERFACE_DIS |
               VDMA_BUS_WRITE_EN);
    MODIFY_REG(reg->CONTROL,
               VDMA_CONTROL_SOURCE_SELECT_MASK |
               VDMA_CONTROL_SOURCE_BURST_MASK |
               VDMA_CONTROL_SOURCE_FIXADDR_EN_MASK |
               VDMA_CONTROL_VDMA_MODE_MASK,
               VDMA_SOURCE_PDM0 |
               VDMA_SOURCE_BURST8 |
               VMDA_ADDR_FIX |
               VDMA_MODE_DATA_STORE);

    return HAL_OK;
}

/**
 * @brief  DeInit vdma controller.
 * @param  vdma: the handle of vdma.
 * @return HAL_Status
 */
HAL_Status HAL_VDMA_DeInit(struct HAL_VDMA_DEV *vdma)
{
    return HAL_OK;
}

/** @} */

/** @defgroup VDMA_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Enable vdma controller.
 * @param  vdma: the handle of vdma.
 * @return HAL_Status
 */
HAL_Status HAL_VDMA_Enable(struct HAL_VDMA_DEV *vdma)
{
    struct VDMA_REG *reg = vdma->pReg;

    HAL_ASSERT(IS_VDMA_INSTANCE(reg));

    MODIFY_REG(reg->INT, VDMA_INT_VDMA_DATA_TRANS_INT_EN_MASK,
               VDMA_INT_VDMA_DATA_TRANS_INT_EN_MASK);
    MODIFY_REG(reg->CONTROL, VDMA_CONTROL_VDMA_EN_MASK,
               VDMA_EN);

    return HAL_OK;
}

/**
 * @brief  Disable vdma controller.
 * @param  vdma: the handle of vdma.
 * @return HAL_Status
 */
HAL_Status HAL_VDMA_Disable(struct HAL_VDMA_DEV *vdma)
{
    struct VDMA_REG *reg = vdma->pReg;

    HAL_ASSERT(IS_VDMA_INSTANCE(reg));

    MODIFY_REG(reg->INT, VDMA_INT_VDMA_DATA_TRANS_INT_EN_MASK, 0);
    MODIFY_REG(reg->CONTROL, VDMA_CONTROL_VDMA_EN_MASK, VDMA_DIS);

    return HAL_OK;
}

/**
 * @brief  Config vdma controller.
 * @param  vdma: the handle of vdma.
 * @param  params: audio params.
 * @return HAL_Status
 */
HAL_Status HAL_VDMA_Config(struct HAL_VDMA_DEV *vdma, struct AUDIO_PARAMS *params)
{
    struct VDMA_REG *reg = vdma->pReg;
    uint32_t val = 0;

    HAL_ASSERT(IS_VDMA_INSTANCE(reg));
    switch (params->sampleBits) {
    case 16:
        val = VDMA_CHANNEL_WITH_16;
        break;
    case 24:
        val = VDMA_CHANNEL_WITH_24;
        break;
    default:
        val = VDMA_CHANNEL_WITH_16;
    }

    val |= VDMA_CHANNELS(params->channels);

    MODIFY_REG(reg->CONTROL,
               VDMA_CONTROL_VOICE_CHANNEL_NUM_MASK |
               VDMA_CONTROL_VOICE_CHANNEL_BITWIDTH_MASK,
               val);

    return HAL_OK;
}

/**
 * @brief Prepare a cyclic dma transfer for the vdma
 *
 * @param vdma: tthe handle of struct HAL_VDMA_DEV.
 * @param dmaAddr: the memory addr.
 * @param len: data len.
 * @param periodLen: periodic len.
 * @param direction: transfer direction.
 * @param callback: callback function.
 * @param cparam: callback param.
 *
 * @return
 *        - HAL_OK on success.
 *        - HAL_ERROR on fail.
 */
HAL_Status HAL_VDMA_PrepDmaCyclic(struct HAL_VDMA_DEV *vdma, uint32_t dmaAddr,
                                  uint32_t len, uint32_t periodLen,
                                  eDMA_TRANSFER_DIRECTION direction,
                                  VDMA_Callback callback, void *cparam)
{
    struct VDMA_REG *reg = vdma->pReg;

    HAL_ASSERT(IS_VDMA_INSTANCE(reg));

    MODIFY_REG(reg->RAM_START_ADDR_BUS,
               VDMA_RAM_START_ADDR_BUS_RAM_START_ADDR_BUS_MASK,
               dmaAddr);
    MODIFY_REG(reg->RAM_END_ADDR_BUS,
               VDMA_RAM_END_ADDR_BUS_RAM_BEGIN_ADDR_BUS_MASK,
               dmaAddr + len);
    /* set trans intterrupt size */
    MODIFY_REG(reg->AUX_CON1,
               VDMA_AUX_CON1_DATA_TRANS_INT_MODE_SEL_MASK |
               VDMA_AUX_CON1_DATA_TRANS_WORD_THD_MASK,
               VDMA_INT_DATA_TRANS_WORD |
               VDMA_DATA_TRANS_WORD(periodLen / 4));
    vdma->callback = callback;

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

#endif
