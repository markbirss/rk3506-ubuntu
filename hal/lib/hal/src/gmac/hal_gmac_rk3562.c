/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2023 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(SOC_RK3562) && defined(HAL_GMAC_MODULE_ENABLED)

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

#define DELAY_ENABLE(soc, tx, rx)                                          \
    (((tx) ? soc##_GMAC_TXCLK_DLY_ENABLE : soc##_GMAC_TXCLK_DLY_DISABLE) | \
     ((rx) ? soc##_GMAC_RXCLK_DLY_ENABLE : soc##_GMAC_RXCLK_DLY_DISABLE))

#define DELAY_VALUE(soc, tx, rx)                        \
    ((((tx) >= 0) ? soc##_GMAC_CLK_TX_DL_CFG(tx) : 0) | \
     (((rx) >= 0) ? soc##_GMAC_CLK_RX_DL_CFG(rx) : 0))

#define GMAC_RGMII_CLK_DIV_BY_ID(soc, id, div) \
        (soc##_GMAC##id##_CLK_RGMII_DIV##div)

#define GMAC_RMII_CLK_DIV_BY_ID(soc, id, div) \
        (soc##_GMAC##id##_CLK_RMII_DIV##div)

/* RK3562 sys_grf */
#define RK3562_GMAC0_CLK_RMII_MODE  GRF_BIT(5)
#define RK3562_GMAC0_CLK_RGMII_MODE GRF_CLR_BIT(5)

#define RK3562_GMAC0_CLK_RMII_GATE   GRF_BIT(6)
#define RK3562_GMAC0_CLK_RMII_NOGATE GRF_CLR_BIT(6)

#define RK3562_GMAC0_CLK_RMII_DIV2  GRF_BIT(7)
#define RK3562_GMAC0_CLK_RMII_DIV20 GRF_CLR_BIT(7)

#define RK3562_GMAC0_CLK_RGMII_DIV1 \
                (GRF_CLR_BIT(7) | GRF_CLR_BIT(8))
#define RK3562_GMAC0_CLK_RGMII_DIV5 \
                (GRF_BIT(7) | GRF_BIT(8))
#define RK3562_GMAC0_CLK_RGMII_DIV50 \
                (GRF_CLR_BIT(7) | GRF_BIT(8))

#define RK3562_GMAC0_CLK_RMII_DIV2  GRF_BIT(7)
#define RK3562_GMAC0_CLK_RMII_DIV20 GRF_CLR_BIT(7)

#define RK3562_GMAC0_CLK_SELET_CRU GRF_CLR_BIT(9)
#define RK3562_GMAC0_CLK_SELET_IO  GRF_BIT(9)

#define RK3562_GMAC1_CLK_RMII_GATE   GRF_BIT(12)
#define RK3562_GMAC1_CLK_RMII_NOGATE GRF_CLR_BIT(12)

#define RK3562_GMAC1_CLK_RMII_DIV2  GRF_BIT(13)
#define RK3562_GMAC1_CLK_RMII_DIV20 GRF_CLR_BIT(13)

#define RK3562_GMAC1_RMII_SPEED100 GRF_BIT(11)
#define RK3562_GMAC1_RMII_SPEED10  GRF_CLR_BIT(11)

#define RK3562_GMAC1_CLK_SELET_CRU GRF_CLR_BIT(15)
#define RK3562_GMAC1_CLK_SELET_IO  GRF_BIT(15)

/* RK3562 ioc_grf */
#define RK3562_GMAC_RXCLK_DLY_ENABLE  GRF_BIT(1)
#define RK3562_GMAC_RXCLK_DLY_DISABLE GRF_CLR_BIT(1)
#define RK3562_GMAC_TXCLK_DLY_ENABLE  GRF_BIT(0)
#define RK3562_GMAC_TXCLK_DLY_DISABLE GRF_CLR_BIT(0)

#define RK3562_GMAC_CLK_RX_DL_CFG(val) HIWORD_UPDATE(val, 0xFF, 8)
#define RK3562_GMAC_CLK_TX_DL_CFG(val) HIWORD_UPDATE(val, 0xFF, 0)

#define RK3562_GMAC0_IO_EXTCLK_SELET_CRU GRF_CLR_BIT(2)
#define RK3562_GMAC0_IO_EXTCLK_SELET_IO  GRF_BIT(2)

#define RK3562_GMAC1_IO_EXTCLK_SELET_CRU GRF_CLR_BIT(3)
#define RK3562_GMAC1_IO_EXTCLK_SELET_IO  GRF_BIT(3)

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
    if (pGMAC->pReg != GMAC0) {
        return;
    }

    WRITE_REG(SYS_GRF->SOC_CON0, RK3562_GMAC0_CLK_RGMII_MODE);

    WRITE_REG(GPIO4_IOC->MAC0_IO_CON1, DELAY_ENABLE(RK3562, txDelay, rxDelay));
    WRITE_REG(GPIO4_IOC->MAC0_IO_CON0, DELAY_VALUE(RK3562, txDelay, rxDelay));

    WRITE_REG(GPIO2_IOC->MAC1_IO_CON1, DELAY_ENABLE(RK3562, txDelay, rxDelay));
    WRITE_REG(GPIO2_IOC->MAC1_IO_CON0, DELAY_VALUE(RK3562, txDelay, rxDelay));
}

/**
 * @brief  Set RMII Mode.
 * @param  pGMAC: pointer to a GMAC_HANDLE structure that contains
 *                the information for GMAC module.
 */
void HAL_GMAC_SetToRMII(struct GMAC_HANDLE *pGMAC)
{
    if (pGMAC->pReg == GMAC0) {
        WRITE_REG(SYS_GRF->SOC_CON0, RK3562_GMAC0_CLK_RMII_MODE);
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
    unsigned int val = 0, id;
    uint32_t *grfCon;

    id = (pGMAC->pReg == GMAC0) ? 0 : 1;
    switch (speed) {
    case 10:
        if (pGMAC->phyStatus.interface == PHY_INTERFACE_MODE_RMII) {
            if (id > 0) {
                val = GMAC_RMII_CLK_DIV_BY_ID(RK3562, 1, 20);
                WRITE_REG(SYS_GRF->SOC_CON0, RK3562_GMAC1_RMII_SPEED10);
            } else {
                val = GMAC_RMII_CLK_DIV_BY_ID(RK3562, 0, 20);
            }
        } else {
            val = GMAC_RGMII_CLK_DIV_BY_ID(RK3562, 0, 50);
        }
        break;
    case 100:
        if (pGMAC->phyStatus.interface == PHY_INTERFACE_MODE_RMII) {
            if (id > 0) {
                val = GMAC_RMII_CLK_DIV_BY_ID(RK3562, 1, 2);
                WRITE_REG(SYS_GRF->SOC_CON0, RK3562_GMAC1_RMII_SPEED100);
            } else {
                val = GMAC_RMII_CLK_DIV_BY_ID(RK3562, 0, 2);
            }
        } else {
            val = GMAC_RGMII_CLK_DIV_BY_ID(RK3562, 0, 5);
        }
        break;
    case 1000:
        if (pGMAC->phyStatus.interface != PHY_INTERFACE_MODE_RMII) {
            val = GMAC_RGMII_CLK_DIV_BY_ID(RK3562, 0, 1);
        } else {
            goto err;
        }
        break;
    default:
        goto err;
    }

    grfCon = (uint32_t *)((id > 0) ? &(SYS_GRF->SOC_CON1) :
                                     &(SYS_GRF->SOC_CON0));

    WRITE_REG(*grfCon, val);

    return;

err:
    HAL_DBG_ERR("unknown speed value for GMAC speed=%" PRId32 "", speed);

    return;
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

    if (pGMAC->pReg == GMAC0) {
        value = extClk ? RK3562_GMAC0_CLK_SELET_IO :
                         RK3562_GMAC0_CLK_SELET_CRU;
        value |= enable ? RK3562_GMAC0_CLK_RMII_NOGATE :
                          RK3562_GMAC0_CLK_RMII_GATE;
        WRITE_REG(SYS_GRF->SOC_CON0, value);

        value = extClk ? RK3562_GMAC0_IO_EXTCLK_SELET_IO :
                         RK3562_GMAC0_IO_EXTCLK_SELET_CRU;
        WRITE_REG(GPIO4_IOC->MAC0_IO_CON1, value);
        WRITE_REG(GPIO2_IOC->MAC1_IO_CON1, value);
    } else {
        value = extClk ? RK3562_GMAC1_CLK_SELET_IO :
                         RK3562_GMAC1_CLK_SELET_CRU;
        value |= enable ? RK3562_GMAC1_CLK_RMII_NOGATE :
                          RK3562_GMAC1_CLK_RMII_GATE;
        WRITE_REG(SYS_GRF->SOC_CON1, value);

        value = extClk ? RK3562_GMAC1_IO_EXTCLK_SELET_IO :
                         RK3562_GMAC1_IO_EXTCLK_SELET_CRU;
        WRITE_REG(GPIO2_IOC->MAC1_IO_CON1, value);
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
    return HAL_GMAC_SetRGMIISpeed(pGMAC, speed);
}

/** @} */

/** @} */

/** @} */

#endif /* SOC_RK3562 && HAL_GMAC_MODULE_ENABLED */
