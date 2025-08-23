/*
 * Copyright (c) 2024, Rockchip Electronics Co. Ltd. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "tfm_fw_intergrity_api.h"
#include "psa/client.h"
#include "psa_manifest/sid.h"

enum tfm_fw_intergrity_err_t tfm_fw_intergrity_fw_decrypt(struct tfm_fw_intergrity_iovec *iovec)
{
    psa_status_t status = PSA_ERROR_CONNECTION_REFUSED;
    struct psa_invec in_vec[1];

    in_vec[0].base = iovec;
    in_vec[0].len  = sizeof(struct tfm_fw_intergrity_iovec);

    status = psa_call(TFM_FW_INTERGRITY_SERVICE_HANDLE,
                      TFM_FW_INTERGRITY_API_ID_FW_DECRYPT,
                      in_vec, 1, NULL, 0);

    if (status < PSA_SUCCESS) {
        return TFM_FW_INTERGRITY_ERR_SYSTEM_ERROR;
    } else {
        return (enum tfm_fw_intergrity_err_t)status;
    }
}

enum tfm_fw_intergrity_err_t tfm_fw_intergrity_check(uint8_t *value)
{
    psa_status_t status = PSA_ERROR_CONNECTION_REFUSED;
    struct psa_outvec out_vec[1];

    out_vec[0].base = value;
    out_vec[0].len = 1;

    status = psa_call(TFM_FW_INTERGRITY_SERVICE_HANDLE,
                      TFM_FW_INTERGRITY_API_ID_CHECK,
                      NULL, 0, out_vec, 1);

    if (status < PSA_SUCCESS) {
        return TFM_FW_INTERGRITY_ERR_SYSTEM_ERROR;
    } else {
        return (enum tfm_fw_intergrity_err_t)status;
    }
}
