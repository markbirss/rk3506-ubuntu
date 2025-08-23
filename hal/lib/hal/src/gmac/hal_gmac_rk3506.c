/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(SOC_RK3506) && defined(HAL_GMAC_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup GMAC
 *  @{
 */

/** @defgroup GMAC_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/

#define HIWORD_UPDATE(val, mask, shift) \
        ((val) << (shift) | (mask) << ((shift) + 16))

#define GRF_BIT(nr)     (1 << (nr) | 1 << (nr+16))
#define GRF_CLR_BIT(nr) (1 << (nr+16))

/* RK3506 grf_soc_con8 & grf_soc_con11 */
#define RK3506_MAC_CLK_RMII_MODE GRF_BIT(1)

#define RK3506_MAC_CLK_RMII_GATE   GRF_BIT(2)
#define RK3506_MAC_CLK_RMII_NOGATE GRF_CLR_BIT(2)

#define RK3506_MAC_CLK_RMII_DIV2  GRF_BIT(3)
#define RK3506_MAC_CLK_RMII_DIV20 GRF_CLR_BIT(3)

#define RK3506_MAC_CLK_SELET_CRU GRF_CLR_BIT(5)
#define RK3506_MAC_CLK_SELET_IO  GRF_BIT(5)

/********************* Private Structure Definition **************************/

/********************* Private Variable Definition ***************************/

/********************* Private Function Definition ***************************/

/** @} */
/********************* Public Function Definition ****************************/

/** @defgroup GMAC_Exported_Functions_Group5 Other Functions
 *  @{
 */

/**
 * @brief  Set RGMII Mode.
 * @param  pGMAC: pointer to a GMAC_HANDLE structure that contains
 *                the information for GMAC module.
 * @param  txDelay: RGMII tx delayline
 * @param  rxDelay: RGMII rx delayline
 */
void HAL_GMAC_SetToRGMII(struct GMAC_HANDLE *pGMAC,
                         int32_t txDelay, int32_t rxDelay)
{
}

/**
 * @brief  Set RMII Mode.
 * @param  pGMAC: pointer to a GMAC_HANDLE structure that contains
 *                the information for GMAC module.
 */
void HAL_GMAC_SetToRMII(struct GMAC_HANDLE *pGMAC)
{
    if (pGMAC->pReg == GMAC0) {
        WRITE_REG(GRF->SOC_CON8, RK3506_MAC_CLK_RMII_MODE);
    } else {
        WRITE_REG(GRF->SOC_CON11, RK3506_MAC_CLK_RMII_MODE);
    }
}

/**
 * @brief  Set RMII speed.
 * @param  pGMAC: pointer to a GMAC_HANDLE structure that contains
 *                the information for GMAC module.
 * @param  speed: RMII speed 10/100
 */
void HAL_GMAC_SetRGMIISpeed(struct GMAC_HANDLE *pGMAC, int32_t speed)
{
}

/**
 * @brief  Set external clock source select.
 * @param  pGMAC: pointer to a GMAC_HANDLE structure that contains
 *                the information for GMAC module.
 * @param  extClk: 0: select clk_mac as the clock of mac
 *                 1: select external phy clock as the clock of mac
 */
void HAL_GMAC_SetExtclkSrc(struct GMAC_HANDLE *pGMAC, bool extClk)
{
    bool enable = true;
    uint32_t value;

    value = extClk ? RK3506_MAC_CLK_SELET_IO :
                     RK3506_MAC_CLK_SELET_CRU;
    value |= enable ? RK3506_MAC_CLK_RMII_NOGATE :
                      RK3506_MAC_CLK_RMII_GATE;
    if (pGMAC->pReg == GMAC0) {
        WRITE_REG(GRF->SOC_CON8, value);
    } else {
        WRITE_REG(GRF->SOC_CON11, value);
    }
}

/**
 * @brief  Set RMII speed.
 * @param  pGMAC: pointer to a GMAC_HANDLE structure that contains
 *                the information for GMAC module.
 * @param  speed: RMII speed 10/100
 */
void HAL_GMAC_SetRMIISpeed(struct GMAC_HANDLE *pGMAC, int32_t speed)
{
    uint32_t value;

    switch (speed) {
    case 10:
        value = RK3506_MAC_CLK_RMII_DIV20;
        break;
    case 100:
        value = RK3506_MAC_CLK_RMII_DIV2;
        break;
    default:
        HAL_DBG_ERR("unknown speed value for MAC speed=%" PRId32 "", speed);

        return;
    }

    if (pGMAC->pReg == GMAC0) {
        WRITE_REG(GRF->SOC_CON8, value);
    } else {
        WRITE_REG(GRF->SOC_CON11, value);
    }
}

/** @} */

/** @} */

/** @} */

#endif /* RKMCU_RK3506 && HAL_GMAC_MODULE_ENABLED */
