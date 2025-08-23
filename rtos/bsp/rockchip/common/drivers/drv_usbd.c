/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_usbd.c
  * @version V0.0
  * @brief   usb device driver
  *
  * Change Logs:
  * Date           Author          Notes
  * 2019-04-02     Liangfeng Wu    the first version
  *
  ******************************************************************************
  */

#include <rthw.h>
#include <rtthread.h>

#if defined(RT_USING_USB_DEVICE)
#include "hal_base.h"
#include "hal_bsp.h"
#include "drv_clock.h"
#include "drv_usbd.h"
#include "drv_pm.h"
#ifdef RT_USING_USB_PMIC_VBUS
#include "drv_pmic.h"
#endif
#include "board.h"

#define WORK_QUEUE_STACK_SIZE           512
#define WORK_QUEUE_PRIORITY             0
#define EP_STATUS_POLL_INTERVAL_MS      5
#define EP_START_POLL_TIMEOUT           (100 * EP_STATUS_POLL_INTERVAL_MS)
#define EP_XFER_COMPL_POLL_TIMEOUT      (10 * EP_STATUS_POLL_INTERVAL_MS)

struct rockchip_usbd
{
    struct PCD_HANDLE pcd;
    struct udcd udc;
    rt_uint8_t id;
    /* Interrupt workqueue */
    struct rt_workqueue *isr_workqueue;
    struct rt_work isr_work;
};

static struct rockchip_usbd g_usbd;

static const char *chg_to_string(ePCD_bcdMsg msg)
{
    switch (msg)
    {
    case PCD_BCD_STD_DOWNSTREAM_PORT:
        return "USB_SDP_CHARGER";
    case PCD_BCD_CHARGING_DOWNSTREAM_PORT:
        return "USB_CDP_CHARGER";
    case PCD_BCD_DEDICATED_CHARGING_PORT:
        return "USB_DCP_CHARGER";
    case PCD_BCD_FLOATING_CHARGING_PORT:
        return "USB_FLOATING_CHARGER";
    default:
        return "INVALID_CHARGER";
    }
}

static void usb_pcd_lowlevel_hw_init(struct PCD_HANDLE *pcd)
{
    uint8_t i;

    /* Initialize Low Level Driver */
    HAL_PCD_Init(pcd);

#ifndef RT_USB_DEVICE_UVC
    HAL_PCDEx_SetRxFiFo(pcd, 0x118);
#else
    /*
     * Set smaller RxFiFo for 3 * UVC interfaces, then
     * it can assign enough TxFiFo size (1024 Bytes)
     * for each UVC streaming IN endpoint to get higher
     * performance.
     */
    HAL_PCDEx_SetRxFiFo(pcd, 0x8C);
#endif

    /* Set TxFIFOs according to the g_usb_ep_pool array */
    for (i = 0; i < pcd->cfg.epNum + 1; i++)
    {
        if (g_usb_ep_pool[i].addr == 0xFF)
            break;

        if (g_usb_ep_pool[i].dir == USB_DIR_IN || g_usb_ep_pool[i].dir == USB_DIR_INOUT)
            HAL_PCDEx_SetTxFiFo(pcd, g_usb_ep_pool[i].addr,
                                (g_usb_ep_pool[i].maxpacket >> 2));
    }

    HAL_PCD_Start(pcd);
}

void USB_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_PCD_IRQHandler(&g_usbd.pcd);
    /* leave interrupt */
    rt_interrupt_leave();
}

void HAL_PCD_ResetCallback(struct PCD_HANDLE *pcd)
{
    pm_runtime_request(PM_RUNTIME_ID_USB);
    /* open ep0 OUT and IN */
    HAL_PCD_EPOpen(pcd, 0x00, pcd->cfg.ep0Mps, EP_TYPE_CTRL);
    HAL_PCD_EPOpen(pcd, 0x80, pcd->cfg.ep0Mps, EP_TYPE_CTRL);
    rt_usbd_reset_handler(&g_usbd.udc);
}

void HAL_PCD_SetupStageCallback(struct PCD_HANDLE *pcd)
{
    rt_usbd_ep0_setup_handler(&g_usbd.udc, (struct urequest *)pcd->setupBuf);
}

void HAL_PCD_DataInStageCallback(struct PCD_HANDLE *pcd, uint8_t epnum)
{
    if (epnum == 0)
    {
        rt_usbd_ep0_in_handler(&g_usbd.udc);
    }
    else
    {
        rt_usbd_ep_in_handler(&g_usbd.udc, 0x80 | epnum,
                              pcd->inEp[epnum].xferCount);
    }
}

void HAL_PCD_ConnectCallback(struct PCD_HANDLE *pcd)
{
    rt_usbd_connect_handler(&g_usbd.udc);
}

void HAL_PCD_SOFCallback(struct PCD_HANDLE *pcd)
{
    rt_usbd_sof_handler(&g_usbd.udc);
}

void HAL_PCD_DisconnectCallback(struct PCD_HANDLE *pcd)
{
    rt_usbd_disconnect_handler(&g_usbd.udc);
    pm_runtime_release(PM_RUNTIME_ID_USB);
}

void HAL_PCD_DataOutStageCallback(struct PCD_HANDLE *pcd, uint8_t epnum)
{
    if (epnum != 0)
    {
        rt_usbd_ep_out_handler(&g_usbd.udc, epnum, pcd->outEp[epnum].xferCount);
    }
    else
    {
        rt_usbd_ep0_out_handler(&g_usbd.udc, pcd->outEp[0].xferCount);
    }
}

void HAL_PCDEx_BcdCallback(struct PCD_HANDLE *pcd, ePCD_bcdMsg msg)
{
    pcd->bcdState = msg;
    RT_DEBUG_LOG(RT_DEBUG_USB, ("charger is %s\n", chg_to_string(msg)));

    if (msg == PCD_BCD_STD_DOWNSTREAM_PORT || msg == PCD_BCD_CHARGING_DOWNSTREAM_PORT)
    {
        clk_enable_by_id(g_usbdDev.hclkGateID);
        clk_enable_by_id(g_usbdDev.utmiclkGateID);
        HAL_USB_PhyResume(g_usbd.id);
        usb_pcd_lowlevel_hw_init(pcd);
        RT_DEBUG_LOG(RT_DEBUG_USB, ("resume usb phy and ctrl\n"));
    }
    else
    {
        HAL_USB_PhySuspend(g_usbd.id);
        clk_disable_by_id(g_usbdDev.hclkGateID);
        clk_disable_by_id(g_usbdDev.utmiclkGateID);
        RT_DEBUG_LOG(RT_DEBUG_USB, ("suspend usb phy and ctrl\n"));
    }
}

ePCD_bcdMsg usb_get_bc_info(void)
{
    return g_usbd.pcd.bcdState;
}

static rt_err_t usb_ep_set_stall(rt_uint8_t address)
{
    HAL_PCD_EPSetStall(&g_usbd.pcd, address);
    return RT_EOK;
}

static rt_err_t usb_ep_clear_stall(rt_uint8_t address)
{
    HAL_PCD_EPClrStall(&g_usbd.pcd, address);
    return RT_EOK;
}

static rt_err_t usb_set_address(rt_uint8_t address)
{
    HAL_PCD_SetAddress(&g_usbd.pcd, address);
    return RT_EOK;
}

static rt_err_t usb_set_config(rt_uint8_t address)
{
    return RT_EOK;
}

static rt_err_t usb_ep_enable(uep_t ep)
{
    RT_ASSERT(ep != RT_NULL);
    RT_ASSERT(ep->ep_desc != RT_NULL);
    HAL_PCD_EPOpen(&g_usbd.pcd, ep->ep_desc->bEndpointAddress,
                   ep->ep_desc->wMaxPacketSize, ep->ep_desc->bmAttributes);
    return RT_EOK;
}

static rt_err_t usb_ep_disable(uep_t ep)
{
    RT_ASSERT(ep != RT_NULL);
    RT_ASSERT(ep->ep_desc != RT_NULL);
    HAL_PCD_EPClose(&g_usbd.pcd, ep->ep_desc->bEndpointAddress);
    return RT_EOK;
}

static rt_size_t usb_ep_read(rt_uint8_t address, void *buffer)
{
    rt_size_t size = 0;

    RT_ASSERT(buffer != RT_NULL);
    return size;
}

static rt_size_t usb_ep_read_prepare(rt_uint8_t address, void *buffer, rt_size_t size)
{
    HAL_PCD_EPReceive(&g_usbd.pcd, address, buffer, size);
    return size;
}

static rt_size_t usb_ep_write(rt_uint8_t address, void *buffer, rt_size_t size)
{
    HAL_PCD_EPTransmit(&g_usbd.pcd, address, buffer, size);
    return size;
}

static rt_err_t usb_ep0_send_status(void)
{
    HAL_PCD_EPTransmit(&g_usbd.pcd, 0x00, NULL, 0);
    return RT_EOK;
}

static rt_err_t usb_suspend(void)
{
    return RT_EOK;
}

static rt_err_t usb_wakeup(void)
{
    return RT_EOK;
}

static rt_err_t usb_ep_poll_enable(uep_t ep)
{
    rt_err_t ret = RT_EOK;

    RT_ASSERT(ep != RT_NULL);
    RT_ASSERT(ep->ep_desc != RT_NULL);
    if (HAL_PCD_EPPollEn(&g_usbd.pcd, ep->ep_desc->bEndpointAddress) != HAL_OK)
        ret = RT_ERROR;

    return ret;
}

static rt_err_t usb_ep_poll_status(uep_t ep)
{
    rt_uint16_t poll_time = 0;
    rt_uint8_t ep_xfer_start = 0;
    rt_uint8_t ep_xfer_compl = 0;
    rt_err_t ret = RT_EOK;

    RT_ASSERT(ep != RT_NULL);
    RT_ASSERT(ep->ep_desc != RT_NULL);

    while (poll_time < EP_START_POLL_TIMEOUT)
    {
        if (HAL_PCD_EPXferStartPoll(&g_usbd.pcd, ep->ep_desc->bEndpointAddress) == HAL_OK)
        {
            ep_xfer_start = 1;
            break;
        }

        rt_thread_mdelay(EP_STATUS_POLL_INTERVAL_MS);
        poll_time += EP_STATUS_POLL_INTERVAL_MS;
    }

    if (!ep_xfer_start)
    {
        rt_kprintf("wait ep xfer start timeout\n");
        return RT_ERROR;
    }

    poll_time = 0;
    while (poll_time < EP_XFER_COMPL_POLL_TIMEOUT)
    {
        if (HAL_PCD_EPXferCompletePoll(&g_usbd.pcd, ep->ep_desc->bEndpointAddress) == HAL_OK)
        {
            ep_xfer_compl = 1;
            break;
        }

        rt_thread_mdelay(EP_STATUS_POLL_INTERVAL_MS);
        poll_time += EP_STATUS_POLL_INTERVAL_MS;
    }

    if (!ep_xfer_compl)
    {
        rt_kprintf("ep frame toggle\n");
        HAL_PCD_EPFrameToggle(&g_usbd.pcd, ep->ep_desc->bEndpointAddress);
    }

    return ret;
}

#if (defined(RT_USING_USB_PMIC_VBUS) || defined(USB_VBUS_PIN) || defined(RT_USING_BVALID_IRQ)) && defined(USB_INNO_PHY_BCD_DETECT)
static void usb_inno_phy_bcd_detect(struct PCD_HANDLE *pcd)
{
    rt_uint16_t dcd_time = 0;
    ePCD_bcdMsg msg = PCD_BCD_DEFAULT_STATE;

    HAL_USB_PhyInit();
    HAL_USB_InnoPhy_SetChgMode(RT_TRUE);
    HAL_USB_InnoPhy_DCD_Det(RT_TRUE);

    while (dcd_time < CHG_DCD_DET_TIMEOUT)
    {
        rt_thread_mdelay(CHG_DCD_DET_TIME_MS);
        dcd_time += CHG_DCD_DET_TIME_MS;
        if (dcd_time >= CHG_DCD_DET_DBNC)
        {
            msg = HAL_USB_InnoPhy_DCD_State();
            if (msg == PCD_BCD_CONTACT_DETECTION)
                break;
        }
    }

    HAL_USB_InnoPhy_DCD_Det(RT_FALSE);

    if (msg == PCD_BCD_CONTACT_DETECTION)
    {
        HAL_USB_InnoPhy_Primary_Det(RT_TRUE);
        rt_thread_mdelay(CHG_PRIMARY_DET_TIME_MS);
        msg = HAL_USB_InnoPhy_Primary_State();
    }
    else
    {
        msg = PCD_BCD_FLOATING_CHARGING_PORT;
        goto out;
    }

    HAL_USB_InnoPhy_Primary_Det(RT_FALSE);

    if (msg != PCD_BCD_STD_DOWNSTREAM_PORT)
    {
        HAL_USB_InnoPhy_Secondary_Det(RT_TRUE);
        rt_thread_mdelay(CHG_SECONDARY_DET_TIME_MS);
        msg = HAL_USB_InnoPhy_Secondary_State();
        HAL_USB_InnoPhy_Secondary_Det(RT_FALSE);
    }

out:
    HAL_USB_InnoPhy_SetChgMode(RT_FALSE);

    /* Check if the USB still remains connected */
    if (!HAL_USB_InnoPhy_GetBvalid())
        msg = PCD_BCD_DEFAULT_STATE;

    HAL_PCDEx_BcdCallback(pcd, msg);
}
#endif

#if defined(RT_USING_USB_PMIC_VBUS) || defined(USB_VBUS_PIN) || defined(RT_USING_BVALID_IRQ)
static void usb_vbus_isr_work(struct rt_work *work, void *work_data)
{
    struct PCD_HANDLE *pcd = (struct PCD_HANDLE *)work_data;

#ifdef USB_INNO_PHY_BCD_DETECT
    usb_inno_phy_bcd_detect(pcd);
#else
    HAL_PCDEx_BcdDetect(pcd);
#endif
}
#endif

#ifdef USB_VBUS_PIN
static void usb_vbus_pin_isr(void *args)
{
    struct PCD_HANDLE *pcd = args;

    if (PIN_HIGH == rt_pin_read(USB_VBUS_PIN) && pcd->bcdState == PCD_BCD_DEFAULT_STATE)
    {
        /*
         * Handle Charging Process in vbus isr work and
         * set PHY suspend mode in HAL_PCDEx_BcdCallback.
         */
        rt_work_init(&g_usbd.isr_work, usb_vbus_isr_work, (void *)pcd);
        rt_workqueue_dowork(g_usbd.isr_workqueue, &g_usbd.isr_work);
    }
    else
    {
        HAL_USB_PhySuspend(g_usbd.id);
        clk_disable_by_id(g_usbdDev.hclkGateID);
        clk_disable_by_id(g_usbdDev.utmiclkGateID);
        pcd->bcdState = PCD_BCD_DEFAULT_STATE;
        RT_DEBUG_LOG(RT_DEBUG_USB, ("suspend usb phy and ctrl\n"));
    }
}
#endif

#if defined(RT_USING_BVALID_IRQ)
void bvalid_irq_handler()
{
    if (HAL_USB_PhyBvalidIrqRise_Status())
    {
        HAL_USB_PhyBvalidIrqRise_Clear();
        rt_interrupt_enter();
        rt_work_init(&g_usbd.isr_work, usb_vbus_isr_work, &g_usbd.pcd);
        rt_workqueue_dowork(g_usbd.isr_workqueue, &g_usbd.isr_work);
        /* leave interrupt */
        rt_interrupt_leave();
        RT_DEBUG_LOG(RT_DEBUG_USB, ("Bvalid rise irq\n"));
    }
    else if (HAL_USB_PhyBvalidIrqFall_Status())
    {
        HAL_USB_PhyBvalidIrqFall_Clear();
        HAL_USB_PhySuspend(g_usbd.id);
        clk_disable_by_id(g_usbdDev.hclkGateID);
        clk_disable_by_id(g_usbdDev.utmiclkGateID);
        g_usbd.pcd.bcdState = PCD_BCD_DEFAULT_STATE;
        RT_DEBUG_LOG(RT_DEBUG_USB, ("suspend usb phy and ctrl\n"));
    }
}
#endif

static rt_err_t usb_vbus_irq_init(struct PCD_HANDLE *pcd)
{
#if defined(USB_VBUS_PIN)
    rt_pin_mode(USB_VBUS_PIN, PIN_MODE_INPUT);
    rt_pin_attach_irq(USB_VBUS_PIN, PIN_IRQ_MODE_RISING_FALLING, usb_vbus_pin_isr, (void *)pcd);
    rt_pin_irq_enable(USB_VBUS_PIN, RT_TRUE);

    if (PIN_HIGH == rt_pin_read(USB_VBUS_PIN))
    {
#if defined(USB_INNO_PHY_BCD_DETECT)
        usb_inno_phy_bcd_detect(pcd);
#else
        HAL_PCDEx_BcdDetect(pcd);
#endif
        /* Set PHY suspend in HAL_PCDEx_BcdCallback */
    }
    else
    {
        HAL_USB_PhySuspend(g_usbd.id);
        clk_disable_by_id(g_usbdDev.hclkGateID);
        clk_disable_by_id(g_usbdDev.utmiclkGateID);
        RT_DEBUG_LOG(RT_DEBUG_USB, ("suspend usb phy and ctrl\n"));
    }
#elif defined(RT_USING_BVALID_IRQ)
    rt_hw_interrupt_install(g_usbdDev.BvalidIrqNum, (rt_isr_handler_t)bvalid_irq_handler,
                            RT_NULL, "bvalid_irq");
    rt_hw_interrupt_umask(g_usbdDev.BvalidIrqNum);
    HAL_USB_PhyBvalidIrqEnable(RT_TRUE);

    if (HAL_USB_InnoPhy_GetBvalid())
    {
#if defined(USB_INNO_PHY_BCD_DETECT)
        usb_inno_phy_bcd_detect(pcd);
#else
        HAL_PCDEx_BcdDetect(pcd);
#endif
    }
    else
    {
        HAL_USB_PhySuspend(g_usbd.id);
        clk_disable_by_id(g_usbdDev.hclkGateID);
        clk_disable_by_id(g_usbdDev.utmiclkGateID);
        RT_DEBUG_LOG(RT_DEBUG_USB, ("suspend usb phy and ctrl\n"));
    }
#endif
    return RT_EOK;
}

#ifdef RT_USING_USB_PMIC_VBUS
void usb_plug_in_hook(void)
{
    /*
     * Handle Charging Process in vbus isr work and
     * set PHY suspend mode in HAL_PCDEx_BcdCallback.
     */
    rt_work_init(&g_usbd.isr_work, usb_vbus_isr_work, (void *)&g_usbd.pcd);
    rt_workqueue_dowork(g_usbd.isr_workqueue, &g_usbd.isr_work);
}

void usb_plug_out_hook(void)
{
    HAL_USB_PhySuspend(g_usbd.id);
    clk_disable_by_id(g_usbdDev.hclkGateID);
    clk_disable_by_id(g_usbdDev.utmiclkGateID);
    g_usbd.pcd.bcdState = PCD_BCD_DEFAULT_STATE;
    RT_DEBUG_LOG(RT_DEBUG_USB, ("suspend usb phy and ctrl\n"));
}
#endif

#if defined(RT_USING_PM)
static int rt_usbd_pm_suspend(const struct rt_device *device, rt_uint8_t mode)
{
#if !defined(RT_USING_USB_PMIC_VBUS) && !defined(USB_VBUS_PIN)
    HAL_USB_PhySuspend(g_usbd.id);
    clk_disable_by_id(g_usbdDev.hclkGateID);
    clk_disable_by_id(g_usbdDev.utmiclkGateID);
    RT_DEBUG_LOG(RT_DEBUG_USB, ("suspend usb phy and ctrl\n"));
#endif
    return RT_EOK;
}

static void rt_usbd_pm_resume(const struct rt_device *device, rt_uint8_t mode)
{
#if !defined(RT_USING_USB_PMIC_VBUS) && !defined(USB_VBUS_PIN)
    struct PCD_HANDLE *pcd = (struct PCD_HANDLE *)device->user_data;

    clk_enable_by_id(g_usbdDev.hclkGateID);
    clk_enable_by_id(g_usbdDev.utmiclkGateID);
    HAL_USB_PhyResume(g_usbd.id);
    usb_pcd_lowlevel_hw_init(pcd);
    RT_DEBUG_LOG(RT_DEBUG_USB, ("resume usb phy and ctrl\n"));
#endif
}

static struct rt_device_pm_ops rk_usbd_pm_ops =
{
    .suspend = rt_usbd_pm_suspend,
    .resume = rt_usbd_pm_resume,
};
#endif

static rt_err_t usb_pcd_init(rt_device_t device)
{
    struct PCD_HANDLE *pcd;

    /* Set Low Level Driver parameters */
    pcd = (struct PCD_HANDLE *)device->user_data;
    pcd->pReg = g_usbdDev.pReg;
    pcd->cfg.epNum = g_usbdDev.cfg.epNum;
    pcd->cfg.phyif = g_usbdDev.cfg.phyif;
    pcd->cfg.dmaEnable = g_usbdDev.cfg.dmaEnable;
    pcd->cfg.ep0Mps = g_usbdDev.cfg.ep0Mps;
    pcd->cfg.sofEnable = g_usbdDev.cfg.sofEnable;
    pcd->cfg.lpmEnable = g_usbdDev.cfg.lpmEnable;
    pcd->cfg.vbusSensingEnable = g_usbdDev.cfg.vbusSensingEnable;
    if (g_usbd.udc.device_is_hs)
        pcd->cfg.speed = PCD_SPEED_HIGH;
    else
        pcd->cfg.speed = PCD_SPEED_HIGH_IN_FULL;
    pcd->bcdState = PCD_BCD_DEFAULT_STATE;
#ifdef RT_USB_DEVICE_UVC
    pcd->cfg.suspendEnable = RT_TRUE;
#else
    pcd->cfg.suspendEnable = g_usbdDev.cfg.suspendEnable;
#endif

    /* Peripheral interrupt init */
    rt_hw_interrupt_install(g_usbdDev.irqNum, (rt_isr_handler_t)USB_IRQHandler, RT_NULL, "usb_irq");
    rt_hw_interrupt_umask(g_usbdDev.irqNum);

#if !defined(RT_USING_USB_PMIC_VBUS) && !defined(USB_VBUS_PIN) && !defined(RT_USING_BVALID_IRQ)
    /* Initialize usb clocks */
    clk_enable_by_id(g_usbdDev.hclkGateID);
    clk_enable_by_id(g_usbdDev.utmiclkGateID);
    usb_pcd_lowlevel_hw_init(pcd);
#endif

    g_usbd.isr_workqueue = rt_workqueue_create("usb_isr", WORK_QUEUE_STACK_SIZE,
                           WORK_QUEUE_PRIORITY);
    RT_ASSERT(g_usbd.isr_workqueue);
    usb_vbus_irq_init(pcd);

#ifdef RT_USING_USB_PMIC_VBUS
    pmic_plug_in_sethook(usb_plug_in_hook);
    pmic_plug_out_sethook(usb_plug_out_hook);
#endif

#if defined(RT_USING_PM)
    rt_pm_device_register(device, &rk_usbd_pm_ops);
#endif
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops usb_pcd_ops =
{
    usb_pcd_init,
};
#endif

const static struct udcd_ops g_udc_ops =
{
    usb_set_address,
    usb_set_config,
    usb_ep_set_stall,
    usb_ep_clear_stall,
    usb_ep_enable,
    usb_ep_disable,
    usb_ep_read_prepare,
    usb_ep_read,
    usb_ep_write,
    usb_ep0_send_status,
    usb_suspend,
    usb_wakeup,
    usb_ep_poll_enable,
    usb_ep_poll_status,
};

int rt_usbd_register(void)
{
    rt_memset((void *)&g_usbd.udc, 0, sizeof(struct udcd));
    g_usbd.udc.parent.type = RT_Device_Class_USBDevice;
#ifdef RT_USING_DEVICE_OPS
    g_usbd.udc.parent.ops = &usb_pcd_ops;
#else
    g_usbd.udc.parent.init = usb_pcd_init;
#endif
    g_usbd.udc.parent.user_data = &g_usbd.pcd;
    g_usbd.udc.ops = &g_udc_ops;
    /* Register endpoint infomation */
    g_usbd.udc.ep_pool = g_usb_ep_pool;
    g_usbd.udc.ep0.id = &g_usb_ep_pool[0];
#ifdef RT_USING_USBD_SPEED_FULL
    g_usbd.udc.device_is_hs = RT_FALSE;
#else
    g_usbd.udc.device_is_hs = RT_TRUE;
#endif
    rt_device_register((rt_device_t)&g_usbd.udc, "usbd", 0);
    rt_usb_device_init();
    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_usbd_register);
#endif
