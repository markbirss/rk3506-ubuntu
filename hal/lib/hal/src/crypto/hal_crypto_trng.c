/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(RKMCU_RK2206) && defined(HAL_CRYPTO_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup CRYPTO
 *  @{
 */

/** @defgroup TRNG_How_To_Use How To Use
 *  @{

 The TRNG driver can be used as follows:

 @} */

/** @defgroup TRNG_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define CRYPTO_WRITE_MASK_ALL (0xffffu)
#define _SBF(s, v)            ((v) << (s))

/* RNG_CTL */
#define CRYPTO_RNG_64_BIT_LEN        _SBF(CRYPTO_RNG_CTL_RNG_LEN_SHIFT, 0x00)
#define CRYPTO_RNG_128_BIT_LEN       _SBF(CRYPTO_RNG_CTL_RNG_LEN_SHIFT, 0x01)
#define CRYPTO_RNG_192_BIT_LEN       _SBF(CRYPTO_RNG_CTL_RNG_LEN_SHIFT, 0x02)
#define CRYPTO_RNG_256_BIT_LEN       _SBF(CRYPTO_RNG_CTL_RNG_LEN_SHIFT, 0x03)
#define CRYPTO_RNG_FATESY_SOC_RING   _SBF(CRYPTO_RNG_CTL_RING_SEL_SHIFT, 0x00)
#define CRYPTO_RNG_SLOWER_SOC_RING_0 _SBF(CRYPTO_RNG_CTL_RING_SEL_SHIFT, 0x01)
#define CRYPTO_RNG_SLOWER_SOC_RING_1 _SBF(CRYPTO_RNG_CTL_RING_SEL_SHIFT, 0x02)
#define CRYPTO_RNG_SLOWEST_SOC_RING  _SBF(CRYPTO_RNG_CTL_RING_SEL_SHIFT, 0x03)
#define CRYPTO_RNG_ENABLE            HAL_BIT(CRYPTO_RNG_CTL_RNG_ENABLE_SHIFT)
#define CRYPTO_RNG_START             HAL_BIT(CRYPTO_RNG_CTL_RNG_START_SHIFT)

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/** @} */
/********************* Public Function Definition ***************************/

/** @defgroup TRNG_Exported_Functions_Group1 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:

 *  @{
 */

/**
 * @brief  get TRNG data
 * @param  pCrypto: the handle of crypto.
 * @param  pTrng: trng buffer.
 * @param  len: trng buffer length.
 * @return HAL_Status
 */
HAL_Status HAL_CRYPTO_Trng(struct CRYPTO_DEV *pCrypto, uint8_t *pTrng, uint32_t len)
{
    uint32_t i, ctrl = 0;
    uint32_t buf[8];

    HAL_ASSERT(pCrypto);
    HAL_ASSERT(pTrng);

    if (len > RK_TRNG_MAX_SIZE) {
        return HAL_ERROR;
    }

    memset(buf, 0, sizeof(buf));

    /* enable osc_ring to get entropy, sample period is set as 100 */
    WRITE_REG(CRYPTO->RNG_SAMPLE_CNT, 100);

    ctrl |= CRYPTO_RNG_256_BIT_LEN;
    ctrl |= CRYPTO_RNG_SLOWER_SOC_RING_0;
    ctrl |= CRYPTO_RNG_ENABLE;
    ctrl |= CRYPTO_RNG_START;

    WRITE_REG_MASK_WE(CRYPTO->RNG_CTL, CRYPTO_WRITE_MASK_ALL, ctrl);

    while (READ_REG(CRYPTO->RNG_CTL) & CRYPTO_RNG_START) {
        ;
    }

    for (i = 0; i < 8; i++) {
        buf[i] = READ_REG(CRYPTO->RNG_DOUT[i]);
    }

    /* close TRNG */
    WRITE_REG(CRYPTO->RNG_CTL, 0 | CRYPTO_WRITE_MASK_ALL);

    memcpy(pTrng, buf, len);

    return HAL_OK;
}
/** @} */

/** @} */

/** @} */

#endif /* MCU_RK2206 && HAL_CRYPTO_MODULE_ENABLED */
