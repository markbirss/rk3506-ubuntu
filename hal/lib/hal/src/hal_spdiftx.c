/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_SPDIFTX_MODULE_ENABLED

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup SPDIFTX
 *  @{
 */

/** @defgroup SPDIFTX_How_To_Use How To Use
 *  @{

 The SPDIFTX driver can be used as follows:

 @} */

/** @defgroup SPDIFTX_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
#define SPDIFTX_DMA_BURST_SIZE (8) /* size * width: 8*4 = 32 bytes */

/* CFGR */
#define SPDIFTX_CFGR_MCD(x)      ((x - 1) << SPDIFTX_CFGR_MCD_SHIFT)
#define SPDIFTX_CFGR_CLR         HAL_BIT(SPDIFTX_CFGR_CLR_SHIFT)
#define SPDIFTX_CFGR_CSE_EN      HAL_BIT(SPDIFTX_CFGR_CSE_SHIFT)
#define SPDIFTX_CFGR_CSE_DIS     0
#define SPDIFTX_CFGR_ADJ_LEFT_J  HAL_BIT(SPDIFTX_CFGR_ADJ_SHIFT)
#define SPDIFTX_CFGR_ADJ_RIGHT_J 0
#define SPDIFTX_CFGR_HWT_EN      HAL_BIT(SPDIFTX_CFGR_HWT_SHIFT)
#define SPDIFTX_CFGR_HWT_DIS     0
#define SPDIFTX_CFGR_VDW(x)      ((x) << SPDIFTX_CFGR_VDW_SHIFT)
#define SPDIFTX_CFGR_VDW_16BIT   SPDIFTX_CFGR_VDW(0)
#define SPDIFTX_CFGR_VDW_20BIT   SPDIFTX_CFGR_VDW(1)
#define SPDIFTX_CFGR_VDW_24BIT   SPDIFTX_CFGR_VDW(2)

/* DMACR */
#define SPDIFTX_DMACR_TDE_EN  HAL_BIT(SPDIFTX_DMACR_TDE_SHIFT)
#define SPDIFTX_DMACR_TDE_DIS 0
#define SPDIFTX_DMACR_TDL(x)  ((x) << SPDIFTX_DMACR_TDL_SHIFT)

/* XFER */
#define SPDIFTX_XFER_EN  HAL_BIT(SPDIFTX_XFER_EN_SHIFT)
#define SPDIFTX_XFER_DIS 0

/**
 *      |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * CS0: |   Mode    |        d        |  c  |  b  |  a  |
 * CS1: |               Category Code                   |
 * CS2: |    Channel Number     |     Source Number     |
 * CS3: |    Clock Accuracy     |     Sample Freq       |
 * CS4: |    Ori Sample Freq    |     Word Length       |
 * CS5: |                                   |   CGMS-A  |
 * CS6~CS23: Reserved
 *
 * a: use of channel status block
 * b: linear PCM identification: 0 for lpcm, 1 for nlpcm
 * c: copyright information
 * d: additional format information
 */
#define CS_BYTE     6
#define CS_FRAME(c) ((c) << 16 | (c))

/** AES/IEC958 channel status bits */
#define IEC958_AES0_PROFESSIONAL                         (1<<0)/* 0 = consumer, 1 = professional */
#define IEC958_AES0_NONAUDIO                             (1<<1)/* 0 = audio, 1 = non-audio */
#define IEC958_AES0_PRO_EMPHASIS                         (7<<2)/* mask - emphasis */
#define IEC958_AES0_PRO_EMPHASIS_NOTID                   (0<<2)/* emphasis not indicated */
#define IEC958_AES0_PRO_EMPHASIS_NONE                    (1<<2)/* none emphasis */
#define IEC958_AES0_PRO_EMPHASIS_5015                    (3<<2)/* 50/15us emphasis */
#define IEC958_AES0_PRO_EMPHASIS_CCITT                   (7<<2)/* CCITT J.17 emphasis */
#define IEC958_AES0_PRO_FREQ_UNLOCKED                    (1<<5)/* source sample frequency: 0 = locked, 1 = unlocked */
#define IEC958_AES0_PRO_FS                               (3<<6)/* mask - sample frequency */
#define IEC958_AES0_PRO_FS_NOTID                         (0<<6)/* fs not indicated */
#define IEC958_AES0_PRO_FS_44100                         (1<<6)/* 44.1kHz */
#define IEC958_AES0_PRO_FS_48000                         (2<<6)/* 48kHz */
#define IEC958_AES0_PRO_FS_32000                         (3<<6)/* 32kHz */
#define IEC958_AES0_CON_NOT_COPYRIGHT                    (1<<2)/* 0 = copyright, 1 = not copyright */
#define IEC958_AES0_CON_EMPHASIS                         (7<<3)/* mask - emphasis */
#define IEC958_AES0_CON_EMPHASIS_NONE                    (0<<3)/* none emphasis */
#define IEC958_AES0_CON_EMPHASIS_5015                    (1<<3)/* 50/15us emphasis */
#define IEC958_AES0_CON_MODE                             (3<<6)/* mask - mode */
#define IEC958_AES1_PRO_MODE                             (15<<0)/* mask - channel mode */
#define IEC958_AES1_PRO_MODE_NOTID                       (0<<0)/* not indicated */
#define IEC958_AES1_PRO_MODE_STEREOPHONIC                (2<<0)/* stereophonic - ch A is left */
#define IEC958_AES1_PRO_MODE_SINGLE                      (4<<0)/* single channel */
#define IEC958_AES1_PRO_MODE_TWO                         (8<<0)/* two channels */
#define IEC958_AES1_PRO_MODE_PRIMARY                     (12<<0)/* primary/secondary */
#define IEC958_AES1_PRO_MODE_BYTE3                       (15<<0)/* vector to byte 3 */
#define IEC958_AES1_PRO_USERBITS                         (15<<4)/* mask - user bits */
#define IEC958_AES1_PRO_USERBITS_NOTID                   (0<<4)/* not indicated */
#define IEC958_AES1_PRO_USERBITS_192                     (8<<4)/* 192-bit structure */
#define IEC958_AES1_PRO_USERBITS_UDEF                    (12<<4)/* user defined application */
#define IEC958_AES1_CON_CATEGORY                         0x7f
#define IEC958_AES1_CON_GENERAL                          0x00
#define IEC958_AES1_CON_LASEROPT_MASK                    0x07
#define IEC958_AES1_CON_LASEROPT_ID                      0x01
#define IEC958_AES1_CON_IEC908_CD                        (IEC958_AES1_CON_LASEROPT_ID|0x00)
#define IEC958_AES1_CON_NON_IEC908_CD                    (IEC958_AES1_CON_LASEROPT_ID|0x08)
#define IEC958_AES1_CON_MINI_DISC                        (IEC958_AES1_CON_LASEROPT_ID|0x48)
#define IEC958_AES1_CON_DVD                              (IEC958_AES1_CON_LASEROPT_ID|0x18)
#define IEC958_AES1_CON_LASTEROPT_OTHER                  (IEC958_AES1_CON_LASEROPT_ID|0x78)
#define IEC958_AES1_CON_DIGDIGCONV_MASK                  0x07
#define IEC958_AES1_CON_DIGDIGCONV_ID                    0x02
#define IEC958_AES1_CON_PCM_CODER                        (IEC958_AES1_CON_DIGDIGCONV_ID|0x00)
#define IEC958_AES1_CON_MIXER                            (IEC958_AES1_CON_DIGDIGCONV_ID|0x10)
#define IEC958_AES1_CON_RATE_CONVERTER                   (IEC958_AES1_CON_DIGDIGCONV_ID|0x18)
#define IEC958_AES1_CON_SAMPLER                          (IEC958_AES1_CON_DIGDIGCONV_ID|0x20)
#define IEC958_AES1_CON_DSP                              (IEC958_AES1_CON_DIGDIGCONV_ID|0x28)
#define IEC958_AES1_CON_DIGDIGCONV_OTHER                 (IEC958_AES1_CON_DIGDIGCONV_ID|0x78)
#define IEC958_AES1_CON_MAGNETIC_MASK                    0x07
#define IEC958_AES1_CON_MAGNETIC_ID                      0x03
#define IEC958_AES1_CON_DAT                              (IEC958_AES1_CON_MAGNETIC_ID|0x00)
#define IEC958_AES1_CON_VCR                              (IEC958_AES1_CON_MAGNETIC_ID|0x08)
#define IEC958_AES1_CON_DCC                              (IEC958_AES1_CON_MAGNETIC_ID|0x40)
#define IEC958_AES1_CON_MAGNETIC_DISC                    (IEC958_AES1_CON_MAGNETIC_ID|0x18)
#define IEC958_AES1_CON_MAGNETIC_OTHER                   (IEC958_AES1_CON_MAGNETIC_ID|0x78)
#define IEC958_AES1_CON_BROADCAST1_MASK                  0x07
#define IEC958_AES1_CON_BROADCAST1_ID                    0x04
#define IEC958_AES1_CON_DAB_JAPAN                        (IEC958_AES1_CON_BROADCAST1_ID|0x00)
#define IEC958_AES1_CON_DAB_EUROPE                       (IEC958_AES1_CON_BROADCAST1_ID|0x08)
#define IEC958_AES1_CON_DAB_USA                          (IEC958_AES1_CON_BROADCAST1_ID|0x60)
#define IEC958_AES1_CON_SOFTWARE                         (IEC958_AES1_CON_BROADCAST1_ID|0x40)
#define IEC958_AES1_CON_IEC62105                         (IEC958_AES1_CON_BROADCAST1_ID|0x20)
#define IEC958_AES1_CON_BROADCAST1_OTHER                 (IEC958_AES1_CON_BROADCAST1_ID|0x78)
#define IEC958_AES1_CON_BROADCAST2_MASK                  0x0f
#define IEC958_AES1_CON_BROADCAST2_ID                    0x0e
#define IEC958_AES1_CON_MUSICAL_MASK                     0x07
#define IEC958_AES1_CON_MUSICAL_ID                       0x05
#define IEC958_AES1_CON_SYNTHESIZER                      (IEC958_AES1_CON_MUSICAL_ID|0x00)
#define IEC958_AES1_CON_MICROPHONE                       (IEC958_AES1_CON_MUSICAL_ID|0x08)
#define IEC958_AES1_CON_MUSICAL_OTHER                    (IEC958_AES1_CON_MUSICAL_ID|0x78)
#define IEC958_AES1_CON_ADC_MASK                         0x1f
#define IEC958_AES1_CON_ADC_ID                           0x06
#define IEC958_AES1_CON_ADC                              (IEC958_AES1_CON_ADC_ID|0x00)
#define IEC958_AES1_CON_ADC_OTHER                        (IEC958_AES1_CON_ADC_ID|0x60)
#define IEC958_AES1_CON_ADC_COPYRIGHT_MASK               0x1f
#define IEC958_AES1_CON_ADC_COPYRIGHT_ID                 0x16
#define IEC958_AES1_CON_ADC_COPYRIGHT                    (IEC958_AES1_CON_ADC_COPYRIGHT_ID|0x00)
#define IEC958_AES1_CON_ADC_COPYRIGHT_OTHER              (IEC958_AES1_CON_ADC_COPYRIGHT_ID|0x60)
#define IEC958_AES1_CON_SOLIDMEM_MASK                    0x0f
#define IEC958_AES1_CON_SOLIDMEM_ID                      0x08
#define IEC958_AES1_CON_SOLIDMEM_DIGITAL_RECORDER_PLAYER (IEC958_AES1_CON_SOLIDMEM_ID|0x00)
#define IEC958_AES1_CON_SOLIDMEM_OTHER                   (IEC958_AES1_CON_SOLIDMEM_ID|0x70)
#define IEC958_AES1_CON_EXPERIMENTAL                     0x40
#define IEC958_AES1_CON_ORIGINAL                         (1<<7)/* this bits depends on the category code */
#define IEC958_AES2_PRO_SBITS                            (7<<0)/* mask - sample bits */
#define IEC958_AES2_PRO_SBITS_20                         (2<<0)/* 20-bit - coordination */
#define IEC958_AES2_PRO_SBITS_24                         (4<<0)/* 24-bit - main audio */
#define IEC958_AES2_PRO_SBITS_UDEF                       (6<<0)/* user defined application */
#define IEC958_AES2_PRO_WORDLEN                          (7<<3)/* mask - source word length */
#define IEC958_AES2_PRO_WORDLEN_NOTID                    (0<<3)/* not indicated */
#define IEC958_AES2_PRO_WORDLEN_22_18                    (2<<3)/* 22-bit or 18-bit */
#define IEC958_AES2_PRO_WORDLEN_23_19                    (4<<3)/* 23-bit or 19-bit */
#define IEC958_AES2_PRO_WORDLEN_24_20                    (5<<3)/* 24-bit or 20-bit */
#define IEC958_AES2_PRO_WORDLEN_20_16                    (6<<3)/* 20-bit or 16-bit */
#define IEC958_AES2_CON_SOURCE                           (15<<0)/* mask - source number */
#define IEC958_AES2_CON_SOURCE_UNSPEC                    (0<<0)/* unspecified */
#define IEC958_AES2_CON_CHANNEL                          (15<<4)/* mask - channel number */
#define IEC958_AES2_CON_CHANNEL_UNSPEC                   (0<<4)/* unspecified */
#define IEC958_AES3_CON_FS                               (15<<0)/* mask - sample frequency */
#define IEC958_AES3_CON_FS_44100                         (0<<0)/* 44.1kHz */
#define IEC958_AES3_CON_FS_NOTID                         (1<<0)/* non indicated */
#define IEC958_AES3_CON_FS_48000                         (2<<0)/* 48kHz */
#define IEC958_AES3_CON_FS_32000                         (3<<0)/* 32kHz */
#define IEC958_AES3_CON_FS_22050                         (4<<0)/* 22.05kHz */
#define IEC958_AES3_CON_FS_24000                         (6<<0)/* 24kHz */
#define IEC958_AES3_CON_FS_88200                         (8<<0)/* 88.2kHz */
#define IEC958_AES3_CON_FS_768000                        (9<<0)/* 768kHz */
#define IEC958_AES3_CON_FS_96000                         (10<<0)/* 96kHz */
#define IEC958_AES3_CON_FS_176400                        (12<<0)/* 176.4kHz */
#define IEC958_AES3_CON_FS_192000                        (14<<0)/* 192kHz */
#define IEC958_AES3_CON_CLOCK                            (3<<4)/* mask - clock accuracy */
#define IEC958_AES3_CON_CLOCK_1000PPM                    (0<<4)/* 1000 ppm */
#define IEC958_AES3_CON_CLOCK_50PPM                      (1<<4)/* 50 ppm */
#define IEC958_AES3_CON_CLOCK_VARIABLE                   (2<<4)/* variable pitch */
#define IEC958_AES4_CON_MAX_WORDLEN_24                   (1<<0)/* 0 = 20-bit, 1 = 24-bit */
#define IEC958_AES4_CON_WORDLEN                          (7<<1)/* mask - sample word length */
#define IEC958_AES4_CON_WORDLEN_NOTID                    (0<<1)/* not indicated */
#define IEC958_AES4_CON_WORDLEN_20_16                    (1<<1)/* 20-bit or 16-bit */
#define IEC958_AES4_CON_WORDLEN_22_18                    (2<<1)/* 22-bit or 18-bit */
#define IEC958_AES4_CON_WORDLEN_23_19                    (4<<1)/* 23-bit or 19-bit */
#define IEC958_AES4_CON_WORDLEN_24_20                    (5<<1)/* 24-bit or 20-bit */
#define IEC958_AES4_CON_WORDLEN_21_17                    (6<<1)/* 21-bit or 17-bit */
#define IEC958_AES4_CON_ORIGFS                           (15<<4)/* mask - original sample frequency */
#define IEC958_AES4_CON_ORIGFS_NOTID                     (0<<4)/* not indicated */
#define IEC958_AES4_CON_ORIGFS_192000                    (1<<4)/* 192kHz */
#define IEC958_AES4_CON_ORIGFS_12000                     (2<<4)/* 12kHz */
#define IEC958_AES4_CON_ORIGFS_176400                    (3<<4)/* 176.4kHz */
#define IEC958_AES4_CON_ORIGFS_96000                     (5<<4)/* 96kHz */
#define IEC958_AES4_CON_ORIGFS_8000                      (6<<4)/* 8kHz */
#define IEC958_AES4_CON_ORIGFS_88200                     (7<<4)/* 88.2kHz */
#define IEC958_AES4_CON_ORIGFS_16000                     (8<<4)/* 16kHz */
#define IEC958_AES4_CON_ORIGFS_24000                     (9<<4)/* 24kHz */
#define IEC958_AES4_CON_ORIGFS_11025                     (10<<4)/* 11.025kHz */
#define IEC958_AES4_CON_ORIGFS_22050                     (11<<4)/* 22.05kHz */
#define IEC958_AES4_CON_ORIGFS_32000                     (12<<4)/* 32kHz */
#define IEC958_AES4_CON_ORIGFS_48000                     (13<<4)/* 48kHz */
#define IEC958_AES4_CON_ORIGFS_44100                     (15<<4)/* 44.1kHz */
#define IEC958_AES5_CON_CGMSA                            (3<<0)/* mask - CGMS-A */
#define IEC958_AES5_CON_CGMSA_COPYFREELY                 (0<<0)/* copying is permitted without restriction */
#define IEC958_AES5_CON_CGMSA_COPYONCE                   (1<<0)/* one generation of copies may be made */
#define IEC958_AES5_CON_CGMSA_COPYNOMORE                 (2<<0)/* condition not be used */
#define IEC958_AES5_CON_CGMSA_COPYNEVER                  (3<<0)/* no copying is permitted */

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/**
 * @brief  Create channel status for stream.
 * @param  pReg: the handle of SPDIFTX_REG.
 * @param  rate: sample rate.
 * @param  width: sample width.
 * @return HAL_Status
 */
static HAL_Status HAL_SPDIFTX_CreateChannelStatus(struct SPDIFTX_REG *pReg,
                                                  uint32_t rate, uint32_t width)
{
    uint8_t cs[CS_BYTE];
    uint16_t *fc = (uint16_t *)cs;
    uint32_t fs, ws, i;

    HAL_ASSERT(IS_SPDIFTX_INSTANCE(pReg));

    memset(cs, 0, CS_BYTE);

    cs[0] = IEC958_AES0_CON_NOT_COPYRIGHT | IEC958_AES0_CON_EMPHASIS_NONE;
    cs[1] = IEC958_AES1_CON_GENERAL;
    cs[2] = IEC958_AES2_CON_SOURCE_UNSPEC | IEC958_AES2_CON_CHANNEL_UNSPEC;
    cs[3] = IEC958_AES3_CON_CLOCK_1000PPM | IEC958_AES3_CON_FS_NOTID;

    switch (rate) {
    case 32000:
        fs = IEC958_AES3_CON_FS_32000;
        break;
    case 44100:
        fs = IEC958_AES3_CON_FS_44100;
        break;
    case 48000:
        fs = IEC958_AES3_CON_FS_48000;
        break;
    case 88200:
        fs = IEC958_AES3_CON_FS_88200;
        break;
    case 96000:
        fs = IEC958_AES3_CON_FS_96000;
        break;
    case 176400:
        fs = IEC958_AES3_CON_FS_176400;
        break;
    case 192000:
        fs = IEC958_AES3_CON_FS_192000;
        break;
    default:
        return HAL_INVAL;
    }

    cs[3] &= ~IEC958_AES3_CON_FS;
    cs[3] |= fs;

    switch (width) {
    case 16:
        ws = IEC958_AES4_CON_WORDLEN_20_16;
        break;
    case 18:
        ws = IEC958_AES4_CON_WORDLEN_22_18;
        break;
    case 20:
        ws = IEC958_AES4_CON_WORDLEN_20_16 |
             IEC958_AES4_CON_MAX_WORDLEN_24;
        break;
    case 24:
    case 32:
        /* Assume 24-bit width for 32-bit samples. */
        ws = IEC958_AES4_CON_WORDLEN_24_20 |
             IEC958_AES4_CON_MAX_WORDLEN_24;
        break;

    default:
        return HAL_INVAL;
    }

    cs[4] &= ~IEC958_AES4_CON_WORDLEN;
    cs[4] |= ws;

    for (i = 0; i < CS_BYTE / 2; i++) {
        WRITE_REG(pReg->CHNSR[i], CS_FRAME(fc[i]));
    }

    MODIFY_REG(pReg->CFGR, SPDIFTX_CFGR_CSE_MASK,
               SPDIFTX_CFGR_CSE_EN);

    return HAL_OK;
}

/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup SPDIFTX_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 ...to do or delete this row

 *  @{
 */

/** @} */

/** @defgroup SPDIFTX_Exported_Functions_Group2 State and Errors Functions

 This section provides functions allowing to get the status of the module:

 *  @{
 */

/** @} */

/** @defgroup SPDIFTX_Exported_Functions_Group3 IO Functions

 This section provides functions allowing to IO controlling:

 *  @{
 */

/** @} */

/** @defgroup SPDIFTX_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 ...to do or delete this row

 *  @{
 */

/** @} */

/** @defgroup SPDIFTX_Exported_Functions_Group5 Other Functions
 *  @{
 */

/** @defgroup SPDIFTX_Low_Level_Functions Low Level Functions
 *  @{
 */

/**
 * @brief  Init spdiftx controller.
 * @param  pReg: the handle of SPDIFTX_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFTX_Init(struct SPDIFTX_REG *pReg)
{
    HAL_ASSERT(IS_SPDIFTX_INSTANCE(pReg));

    MODIFY_REG(pReg->CFGR, SPDIFTX_CFGR_HWT_MASK, SPDIFTX_CFGR_HWT_EN);
    MODIFY_REG(pReg->DMACR, SPDIFTX_DMACR_TDL_MASK, SPDIFTX_DMACR_TDL(SPDIFTX_DMA_BURST_SIZE));

    return HAL_OK;
}

/**
 * @brief  DeInit spdiftx controller.
 * @param  pReg: the handle of SPDIFTX_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFTX_DeInit(struct SPDIFTX_REG *pReg)
{
    HAL_ASSERT(IS_SPDIFTX_INSTANCE(pReg));

    return HAL_OK;
}

/**
 * @brief  Config spdiftx controller.
 * @param  pReg: the handle of SPDIFTX_REG.
 * @param  params: audio params.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFTX_Config(struct SPDIFTX_REG *pReg, struct AUDIO_PARAMS *params)
{
    uint32_t val = 0;

    HAL_ASSERT(IS_SPDIFTX_INSTANCE(pReg));
    HAL_ASSERT(params != NULL);

    HAL_SPDIFTX_CreateChannelStatus(pReg, params->sampleRate, params->sampleBits);

    switch (params->sampleBits) {
    case AUDIO_SAMPLEBITS_16:
        val |= SPDIFTX_CFGR_VDW_16BIT;
        break;
    case AUDIO_SAMPLEBITS_24:
        val |= SPDIFTX_CFGR_VDW_24BIT;
        val |= SPDIFTX_CFGR_ADJ_RIGHT_J;
        break;
    case AUDIO_SAMPLEBITS_32:
        val |= SPDIFTX_CFGR_VDW_24BIT;
        val |= SPDIFTX_CFGR_ADJ_LEFT_J;
        break;
    default:
        return HAL_INVAL;
    }

    MODIFY_REG(pReg->CFGR, SPDIFTX_CFGR_CLR_MASK,
               SPDIFTX_CFGR_CLR);

    HAL_DelayUs(1);
    MODIFY_REG(pReg->CFGR, SPDIFTX_CFGR_VDW_MASK | SPDIFTX_CFGR_ADJ_MASK, val);

    return HAL_OK;
}

/**
 * @brief  Enable spdiftx.
 * @param  pReg: the handle of SPDIFTX_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFTX_Enable(struct SPDIFTX_REG *pReg)
{
    HAL_ASSERT(IS_SPDIFTX_INSTANCE(pReg));

    MODIFY_REG(pReg->DMACR, SPDIFTX_DMACR_TDE_MASK, SPDIFTX_DMACR_TDE_EN);
    MODIFY_REG(pReg->XFER, SPDIFTX_XFER_EN_MASK, SPDIFTX_XFER_EN);

    return HAL_OK;
}

/**
 * @brief  Disable spdiftx rx.
 * @param  pReg: the handle of SPDIFTX_REG.
 * @return HAL_Status
 */
HAL_Status HAL_SPDIFTX_Disable(struct SPDIFTX_REG *pReg)
{
    HAL_ASSERT(IS_SPDIFTX_INSTANCE(pReg));

    MODIFY_REG(pReg->DMACR, SPDIFTX_DMACR_TDE_MASK, SPDIFTX_DMACR_TDE_DIS);
    MODIFY_REG(pReg->XFER, SPDIFTX_XFER_EN_MASK, SPDIFTX_XFER_DIS);

    return HAL_OK;
}

/** @} */

/** @} */

/** @} */

/** @} */

#endif /* HAL_SPDIFTX_MODULE_ENABLED */
