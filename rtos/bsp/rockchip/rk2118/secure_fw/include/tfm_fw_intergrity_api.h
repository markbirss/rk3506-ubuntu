/*
 * Copyright (c) 2024, Rockchip Electronics Co. Ltd. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_FW_INTERGRITY_API__
#define __TFM_FW_INTERGRITY_API__

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "psa/client.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief TFM secure partition platform API version
 */
#define TFM_FW_INTERGRITY_API_VERSION_MAJOR     (0)
#define TFM_FW_INTERGRITY_API_VERSION_MINOR     (1)

#define TFM_FW_INTERGRITY_API_ID_FW_DECRYPT     (1010)
#define TFM_FW_INTERGRITY_API_ID_CHECK          (1020)

/*!
 * \enum tfm_fw_intergrity_err_t
 *
 * \brief FW Intergrity service error types
 *
 */
enum tfm_fw_intergrity_err_t {
    TFM_FW_INTERGRITY_ERR_SUCCESS = 0,
    TFM_FW_INTERGRITY_ERR_SYSTEM_ERROR,
    TFM_FW_INTERGRITY_ERR_INVALID_PARAM,
    TFM_FW_INTERGRITY_ERR_NOT_SUPPORTED,

    /* Following entry is only to ensure the error code of int size */
    TFM_FW_INTERGRITY_ERR_FORCE_INT_SIZE = INT_MAX
};

struct tfm_fw_intergrity_iovec {
    uint32_t id;
    uint32_t fw_enc_base;
    uint32_t fw_enc_size;
    uint32_t fw_dec_base;
    uint32_t fw_dec_size;
};

/*!
 * \brief Decrypts the firmware using the provided I/O vector
 *
 * \param[in]  iovec  Pointer to a structure that contains the base addresses
 *                    and sizes of the encrypted and decrypted firmware regions.
 *
 * This function decrypts the firmware located at the specified encrypted base
 * address and size, and stores the decrypted firmware at the specified decrypted
 * base address.
 *
 * \return  TFM_FW_INTERGRITY_ERR_SUCCESS if the decryption is successful. Otherwise,
 *          it returns TFM_FW_INTERGRITY_ERR_SYSTEM_ERROR.
 */
enum tfm_fw_intergrity_err_t tfm_fw_intergrity_fw_decrypt(struct tfm_fw_intergrity_iovec *iovec);

enum tfm_fw_intergrity_err_t tfm_fw_intergrity_check(uint8_t *value);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_FW_INTERGRITY_API__ */
