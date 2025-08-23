/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2020-2021 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#if defined(HAL_PCD_MODULE_ENABLED) || defined(HAL_HCD_MODULE_ENABLED)

/** @addtogroup RK_HAL_Driver
 *  @{
 */

/** @addtogroup USB_PHY
 *  @brief USB PHY HAL module driver
 *  @{
 */

/** @defgroup USB_PHY_How_To_Use How To Use
 *  @{

 The USB PHY HAL driver can be used as follows:

 @} */

/** @defgroup USB_PHY_Private_Definition Private Definition
 *  @{
 */
/********************* Private MACRO Definition ******************************/
/********************* Private Structure Definition **************************/
/********************* Private Variable Definition ***************************/
/********************* Private Function Definition ***************************/
/** @} */
/********************* Public Function Definition ****************************/
/** @defgroup USB_PHY_Exported_Functions_Group1 Suspend and Resume Functions

 This section provides functions allowing to suspend and resume the module:

 *  @{
 */

/**
 * @brief  Suspend the USB PHY.
 * @param  id: USB PHY id
 * @return HAL status
 */
HAL_Status HAL_USB_PhySuspend(uint8_t id)
{
    switch (id) {
    case 0:
#ifdef USB_PHY_CON_BASE
        WRITE_REG_MASK_WE(USB_PHY_CON_BASE, USB_PHY_SUSPEND_MASK,
                          USB_PHY_SUSPEND_VAL << USB_PHY_CON_SHIFT);
#endif
        break;
    case 1:
#ifdef USB_PHY1_CON_BASE
        WRITE_REG_MASK_WE(USB_PHY1_CON_BASE, USB_PHY1_SUSPEND_MASK,
                          USB_PHY1_SUSPEND_VAL << USB_PHY1_CON_SHIFT);
#endif
        break;
    default:
        break;
    }

    return HAL_OK;
}

/**
 * @brief  Resume the USB PHY.
 * @param  id: USB PHY id
 * @return HAL status
 */
HAL_Status HAL_USB_PhyResume(uint8_t id)
{
    switch (id) {
    case 0:
#ifdef USB_PHY_CON_BASE
        WRITE_REG_MASK_WE(USB_PHY_CON_BASE, USB_PHY_RESUME_MASK,
                          USB_PHY_RESUME_VAL << USB_PHY_CON_SHIFT);
#endif
        break;
    case 1:
#ifdef USB_PHY1_CON_BASE
        WRITE_REG_MASK_WE(USB_PHY1_CON_BASE, USB_PHY1_RESUME_MASK,
                          USB_PHY1_RESUME_VAL << USB_PHY1_CON_SHIFT);
#endif
        break;
    default:
        break;
    }

    return HAL_OK;
}

#ifdef USB_PHY_BVALID_IRQ_CON_BASE
/**
 * @brief  Enable USB Bvalid Irq.
 * @return HAL status
 */
HAL_Status HAL_USB_PhyBvalidIrqEnable(uint8_t enable)
{
    if (enable) {
        /* Clear bvalid rise and fall irq state */
        WRITE_REG_MASK_WE(USB_PHY_BVALID_IRQ_CLR_BASE,
                          USB_PHY_BVALID_RISE_IRQ_CLR_MASK |
                          USB_PHY_BVALID_FALL_IRQ_CLR_MASK,
                          USB_PHY_BVALID_RISE_IRQ_CLR_EN |
                          USB_PHY_BVALID_FALL_IRQ_CLR_EN);

        /* Enable bvalid rise and fall irq */
        WRITE_REG_MASK_WE(USB_PHY_BVALID_IRQ_CON_BASE,
                          USB_PHY_BVALID_RISE_IRQ_CON_MASK |
                          USB_PHY_BVALID_FALL_IRQ_CON_MASK,
                          USB_PHY_BVALID_RISE_IRQ_CON_EN |
                          USB_PHY_BVALID_FALL_IRQ_CON_EN);
    } else {
        /* Disable bvalid rise and fall irq */
        WRITE_REG_MASK_WE(USB_PHY_BVALID_IRQ_CON_BASE,
                          USB_PHY_BVALID_RISE_IRQ_CON_MASK |
                          USB_PHY_BVALID_FALL_IRQ_CON_MASK,
                          0);
    }

    return HAL_OK;
}

/**
 * @brief  Get Bvalid Irq Rise status.
 * @return irq_status.
 */
uint8_t HAL_USB_PhyBvalidIrqRise_Status(void)
{
    uint8_t irq_status;

    irq_status = READ_BIT(USB_PHY_BVALID_IRQ_STATUS_BASE,
                          USB_PHY_BVALID_RISE_IRQ_STATUS_MASK) >>
                 USB_PHY_BVALID_RISE_IRQ_STATUS_SHIFT;

    return irq_status;
}

/**
 * @brief  Get Bvalid Irq Fall status.
 * @return irq_status.
 */
uint8_t HAL_USB_PhyBvalidIrqFall_Status(void)
{
    uint8_t irq_status;

    irq_status = READ_BIT(USB_PHY_BVALID_IRQ_STATUS_BASE,
                          USB_PHY_BVALID_FALL_IRQ_STATUS_MASK) >>
                 USB_PHY_BVALID_FALL_IRQ_STATUS_SHIFT;

    return irq_status;
}

/**
 * @brief Clear Bvalid Irq Rise status.
 */
void HAL_USB_PhyBvalidIrqRise_Clear(void)
{
    WRITE_REG_MASK_WE(USB_PHY_BVALID_IRQ_CLR_BASE,
                      USB_PHY_BVALID_RISE_IRQ_CLR_MASK,
                      USB_PHY_BVALID_RISE_IRQ_CLR_EN);
}

/**
 * @brief Clear Bvalid Irq Fall status.
 */
void HAL_USB_PhyBvalidIrqFall_Clear(void)
{
    WRITE_REG_MASK_WE(USB_PHY_BVALID_IRQ_CLR_BASE,
                      USB_PHY_BVALID_FALL_IRQ_CLR_MASK,
                      USB_PHY_BVALID_FALL_IRQ_CLR_EN);
}

#else
HAL_Status HAL_USB_PhyBvalidIrqEnable(uint8_t enable)
{
    return HAL_OK;
}

uint8_t HAL_USB_PhyBvalidIrqRise_Status(void)
{
    return 0;
}

uint8_t HAL_USB_PhyBvalidIrqFall_Status(void)
{
    return 0;
}

void HAL_USB_PhyBvalidIrqRise_Clear(void)
{
}

void HAL_USB_PhyBvalidIrqFall_Clear(void)
{
}
#endif

/** @} */

/** @defgroup PCD_Exported_Functions_Group4 Init and DeInit Functions

 This section provides functions allowing to init and deinit the module:
 *  @{
 */

/**
 * @brief  Initialize the USB PHY.
 * @return HAL status
 * @attention these APIs allow direct use in the HAL layer.
 */
#if defined(USB_M31PHY_BASE)
HAL_Status HAL_USB_PhyInit(void)
{
    HAL_CRU_ClkResetDeassert(SRST_USB2PHYPO);
    /* Select USB controller UTMI interface to phy */
    WRITE_REG_MASK_WE(USB_PHY_CON_BASE, USB_PHY_RESUME_MASK,
                      USB_PHY_RESUME_VAL << USB_PHY_CON_SHIFT);
    /* Wait for UTMI clk stable */
    HAL_DelayMs(2);

    return HAL_OK;
}
#elif defined(USB_SNPS_PHY_BASE)
HAL_Status HAL_USB_PhyInit(void)
{
    /* GRF, 0x0348: bit[3:0] = 0x01 */
    WRITE_REG_MASK_WE(USB_PHY_CON_BASE,
                      USB_PHY_SUSPEND_MASK,
                      USB_PHY_SUSPEND_VAL << USB_PHY_CON_SHIFT);

    /* Wait for UTMI clk stable */
    HAL_CRU_ClkResetAssert(SRST_OTG_USBPHY);
    HAL_DelayUs(15);

    /* GRF, 0x0348: bit[3:0] = 0x02 */
    WRITE_REG_MASK_WE(USB_PHY_CON_BASE,
                      USB_PHY_RESUME_MASK,
                      USB_PHY_RESUME_VAL << USB_PHY_CON_SHIFT);
    HAL_DelayUs(1500);
    HAL_CRU_ClkResetDeassert(SRST_OTG_USBPHY);
    HAL_DelayUs(2);

    return HAL_OK;
}
#elif defined(USB_INNO_PHY_BASE)
HAL_Status HAL_USB_PhyInit(void)
{
    /* Reset USB PHY only for Swallow FPGA */
#if defined(SOC_SWALLOW) && defined(IS_FPGA)
    WRITE_REG(*(uint32_t *)(CRU_BASE + 0x1004U), 0x1);
    HAL_DelayUs(500);
    WRITE_REG(*(uint32_t *)(CRU_BASE + 0x1004U), 0x0);
    HAL_DelayUs(2000);
#elif defined(RKMCU_RK2118)
    /* Set bvalid and iddig comes from USBPHY */
    WRITE_REG_MASK_WE(USB_PHY_CON_BASE,
                      GRF_SOC_CON24_USBOTG_UTMI_IDDIG_SEL_MASK |
                      GRF_SOC_CON24_USBOTG_UTMI_BVALID_SEL_MASK,
                      0 << GRF_SOC_CON24_USBOTG_UTMI_IDDIG_SEL_SHIFT |
                      0 << GRF_SOC_CON24_USBOTG_UTMI_BVALID_SEL_SHIFT);

    /* Set HS disconnect detect mode to single ended detect mode */
    WRITE_REG(*(uint32_t *)(USB_INNO_PHY_BASE + 0x0070U), 0xb4);
#endif

    return HAL_OK;
}
#else
HAL_Status HAL_USB_PhyInit(void)
{
    return HAL_OK;
}
#endif

/** @} */

/** @} */

/** @} */

#endif /* defined (HAL_PCD_MODULE_ENABLED) || defined (HAL_HCD_MODULE_ENABLED) */
