/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(RKMCU_RK2118) && defined(HAL_GMAC_MODULE_ENABLED)

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

/* RK2118 grf_soc_con8 */
#define RK2118_MAC_CLK_RMII_MODE GRF_BIT(1)

#define RK2118_MAC_CLK_RMII_GATE   GRF_BIT(2)
#define RK2118_MAC_CLK_RMII_NOGATE GRF_CLR_BIT(2)

#define RK2118_MAC_CLK_RMII_DIV2  GRF_BIT(3)
#define RK2118_MAC_CLK_RMII_DIV20 GRF_CLR_BIT(3)

#define RK2118_MAC_CLK_SELET_CRU GRF_CLR_BIT(5)
#define RK2118_MAC_CLK_SELET_IO  GRF_BIT(5)

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
    WRITE_REG(GRF->SOC_CON8, RK2118_MAC_CLK_RMII_MODE);
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
    uint32_t value;
    bool enable = true;

    value = extClk ? RK2118_MAC_CLK_SELET_IO :
                     RK2118_MAC_CLK_SELET_CRU;
    value |= enable ? RK2118_MAC_CLK_RMII_NOGATE :
                      RK2118_MAC_CLK_RMII_GATE;
    WRITE_REG(GRF->SOC_CON8, value);
}

/**
 * @brief  Set RMII speed.
 * @param  pGMAC: pointer to a GMAC_HANDLE structure that contains
 *                the information for GMAC module.
 * @param  speed: RMII speed 10/100
 */
void HAL_GMAC_SetRMIISpeed(struct GMAC_HANDLE *pGMAC, int32_t speed)
{
    switch (speed) {
    case 10:
        WRITE_REG(GRF->SOC_CON8, RK2118_MAC_CLK_RMII_DIV20);
        break;
    case 100:
        WRITE_REG(GRF->SOC_CON8, RK2118_MAC_CLK_RMII_DIV2);
        break;
    default:
        HAL_DBG_ERR("unknown speed value for MAC speed=%" PRId32 "", speed);

        return;
    }
}

/** @} */

/** @} */

/** @} */

#endif /* RKMCU_RK2118 && HAL_GMAC_MODULE_ENABLED */
