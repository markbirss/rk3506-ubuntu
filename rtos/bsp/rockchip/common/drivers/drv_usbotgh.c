/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2024-09-03     William Wu        the first version
 */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#if defined(RT_USING_DWC2_USBH)
#include "hal_base.h"
#include "hal_bsp.h"
#include "drv_clock.h"
#include "board.h"

#define OTG_HCD_PORT                    1
#define OTG_HCD_CONNECT_DEBOUNCE_MS     500
#define OTG_HCD_WORK_QUEUE_STACK_SIZE   512
#define OTG_HCD_WORK_QUEUE_PRIORITY     0

struct rockchip_usbotgh
{
    const char *name;
    const struct HAL_USB_DEV *dev;
    struct HCD_HANDLE instance;
    struct uhcd *uhcd;
    struct rt_workqueue *usbh_workqueue;
    struct rt_work usbh_work;
    struct rt_completion urb_completion;
    rt_isr_handler_t irq_handler;
    rt_uint8_t id;
    rt_uint16_t pipe_index;
    rt_bool_t connect_status;
    uhcd_ops_t ops;
};
typedef struct rockchip_usbotgh *usbotgh_t;

static int rt_usbotgh_pipe_xfer(upipe_t pipe, rt_uint8_t token,
                                void *buffer, int nbytes, int timeout);
static rt_err_t rt_usbotgh_open_pipe(upipe_t pipe);
static rt_err_t rt_usbotgh_close_pipe(upipe_t pipe);
static void rt_usbotgh_irq(usbotgh_t usbotgh);
static rt_err_t rt_usbotgh_reset_port(usbotgh_t usbotgh);

#define DEFINE_ROCKCHIP_USBOTGH(ID)                             \
static void rockchip_usbotgh##ID##_irq(int irq, void *param);   \
static rt_err_t rt_usbotgh##ID##_reset_port(rt_uint8_t port);   \
static struct uhcd_ops rt_usbotgh##ID##_uhcd_ops =              \
{                                                               \
    .reset_port = rt_usbotgh##ID##_reset_port,                  \
    .pipe_xfer = rt_usbotgh_pipe_xfer,                          \
    .open_pipe = rt_usbotgh_open_pipe,                          \
    .close_pipe = rt_usbotgh_close_pipe,                        \
};                                                              \
static struct rockchip_usbotgh rk_usbotgh##ID =                 \
{                                                               \
    .name = "usbotgh"#ID,                                       \
    .dev = &g_usbotgh##ID##Dev,                                 \
    .irq_handler = rockchip_usbotgh##ID##_irq,                  \
    .id = ID,                                                   \
    .ops = &rt_usbotgh##ID##_uhcd_ops                           \
};                                                              \
static void rockchip_usbotgh##ID##_irq(int irq, void *param)    \
{                                                               \
    rt_usbotgh_irq(&rk_usbotgh##ID);                            \
}                                                               \
static rt_err_t rt_usbotgh##ID##_reset_port(rt_uint8_t port)    \
{                                                               \
    rt_usbotgh_reset_port(&rk_usbotgh##ID);                     \
    return RT_EOK;                                              \
}

#ifdef RT_USING_DWC2_USBH0
DEFINE_ROCKCHIP_USBOTGH(0)
#endif

#ifdef RT_USING_DWC2_USBH1
DEFINE_ROCKCHIP_USBOTGH(1)
#endif

static struct rockchip_usbotgh *const rt_usbotgh_table[] =
{
#ifdef RT_USING_DWC2_USBH0
    &rk_usbotgh0,
#endif
#ifdef RT_USING_DWC2_USBH1
    &rk_usbotgh1,
#endif
    RT_NULL
};

static void rt_usb_hcd_connect_work(struct rt_work *work, void *work_data)
{
    usbotgh_t usbotgh = (usbotgh_t)work_data;

    rt_thread_mdelay(OTG_HCD_CONNECT_DEBOUNCE_MS);

    if (HAL_HCD_GetConnStatus(&usbotgh->instance) && !usbotgh->connect_status)
    {
        rt_kprintf("USB device connect\n");
        usbotgh->connect_status = RT_TRUE;
        rt_usbh_root_hub_connect_handler(usbotgh->uhcd, OTG_HCD_PORT, RT_TRUE);
    }
}

void HAL_HCD_Connect_Callback(struct HCD_HANDLE *hhcd)
{
    usbotgh_t usbotgh = (usbotgh_t)hhcd->pData;

    if (!usbotgh->connect_status)
    {
        RT_DEBUG_LOG(RT_DEBUG_USB, ("USB connect debounce\n"));
        rt_work_init(&usbotgh->usbh_work, rt_usb_hcd_connect_work, (void *)usbotgh);
        rt_workqueue_dowork(usbotgh->usbh_workqueue, &usbotgh->usbh_work);
    }
}

void HAL_HCD_Disconnect_Callback(struct HCD_HANDLE *hhcd)
{
    usbotgh_t usbotgh = (usbotgh_t)hhcd->pData;
    uhcd_t hcd = (uhcd_t)usbotgh->uhcd;

    if (usbotgh->connect_status)
    {
        usbotgh->connect_status = RT_FALSE;
        rt_kprintf("USB disconnect\n");
        rt_usbh_root_hub_disconnect_handler(hcd, OTG_HCD_PORT);
    }
}

void HAL_HCD_HCNotifyURBChange_Callback(struct HCD_HANDLE *hhcd, uint8_t chnum, eUSB_OTG_urbState urb_state)
{
    usbotgh_t usbotgh = (usbotgh_t)hhcd->pData;
    rt_completion_done(&usbotgh->urb_completion);
}

static rt_err_t rt_usbotgh_reset_port(usbotgh_t usbotgh)
{
    RT_DEBUG_LOG(RT_DEBUG_USB, ("reset port\n"));
    HAL_HCD_ResetPort(&usbotgh->instance);

    return RT_EOK;
}

static int rt_usbotgh_pipe_xfer(upipe_t pipe, rt_uint8_t token, void *buffer, int nbytes, int timeout)
{
    usbotgh_t usbotgh = (usbotgh_t)pipe->inst->hcd->parent.user_data;
    rt_tick_t delay;
    eUSB_OTG_hcState hc_state;
    eUSB_OTG_urbState urb_state;
    static int bulk_nak_delay = 5;

    while (1)
    {
        if (!usbotgh->connect_status)
        {
            return -1;
        }
        rt_completion_init(&usbotgh->urb_completion);
        HAL_HCD_HCSubmitRequest(&usbotgh->instance,
                                pipe->pipe_index,
                                (pipe->ep.bEndpointAddress & 0x80) >> 7,
                                pipe->ep.bmAttributes,
                                token,
                                buffer,
                                nbytes,
                                0);
        rt_completion_wait(&usbotgh->urb_completion, timeout);

        hc_state = HAL_HCD_HCGetState(&usbotgh->instance, pipe->pipe_index);
        urb_state = HAL_HCD_HCGetURBState(&usbotgh->instance, pipe->pipe_index);

        if (hc_state == HC_NAK)
        {
            RT_DEBUG_LOG(RT_DEBUG_USB, ("nak\n"));
            if (pipe->ep.bmAttributes == USB_EP_ATTR_INT)
            {
                delay = (pipe->ep.bInterval * RT_TICK_PER_SECOND / 1000) > 0 ?
                        (pipe->ep.bInterval * RT_TICK_PER_SECOND / 1000) : 1;
                rt_thread_delay(delay);
            }
            else if (pipe->ep.bmAttributes == USB_EP_ATTR_BULK)
            {
                rt_thread_mdelay(bulk_nak_delay);
                if (bulk_nak_delay < 50)
                    bulk_nak_delay += 5;
            }
            HAL_HCD_HCHalt(&usbotgh->instance, pipe->pipe_index);
            HAL_HCD_HCInit(&usbotgh->instance,
                           pipe->pipe_index,
                           pipe->ep.bEndpointAddress,
                           pipe->inst->address,
                           USB_OTG_SPEED_HIGH,
                           pipe->ep.bmAttributes,
                           pipe->ep.wMaxPacketSize);
            continue;
        }
        else if (hc_state == HC_STALL)
        {
            RT_DEBUG_LOG(RT_DEBUG_USB, ("stall\n"));
            pipe->status = UPIPE_STATUS_STALL;
            if (pipe->callback != RT_NULL)
            {
                pipe->callback(pipe);
            }
            return -1;
        }
        else if (urb_state == URB_ERROR)
        {
            RT_DEBUG_LOG(RT_DEBUG_USB, ("error\n"));
            pipe->status = UPIPE_STATUS_ERROR;
            if (pipe->callback != RT_NULL)
            {
                pipe->callback(pipe);
            }

            if (hc_state == HC_XACTERR)
            {
                HAL_HCD_Init(&usbotgh->instance);
                HAL_HCD_Start(&usbotgh->instance);
                usbotgh->connect_status = RT_FALSE;
            }
            return -1;
        }
        else if ((urb_state == URB_IDLE && hc_state == HC_IDLE) ||
                 (urb_state == URB_NOTREADY && hc_state == HC_XACTERR))
        {
            RT_DEBUG_LOG(RT_DEBUG_USB, ("urb %d and hc %d, reinit hcd\n", urb_state, hc_state));
            HAL_HCD_Init(&usbotgh->instance);
            HAL_HCD_Start(&usbotgh->instance);
            usbotgh->connect_status = RT_FALSE;
        }
        else if (urb_state != URB_NOTREADY && urb_state != URB_NYET)
        {
            RT_DEBUG_LOG(RT_DEBUG_USB, ("ok\n"));
            if (pipe->ep.bmAttributes == USB_EP_ATTR_BULK)
            {
                bulk_nak_delay = 5;
            }
            pipe->status = UPIPE_STATUS_OK;
            if (pipe->callback != RT_NULL)
            {
                pipe->callback(pipe);
            }
            if (pipe->ep.bEndpointAddress & 0x80)
            {
                return HAL_HCD_HCGetXferCount(&usbotgh->instance, pipe->pipe_index);
            }
            return nbytes;
        }
        return -1;
    }
}

static rt_uint8_t  rt_usbotgh_get_free_pipe_index(usbotgh_t usbotgh)
{
    rt_uint8_t idx;

    for (idx = 1; idx < 16; idx++)
    {
        if (!(usbotgh->pipe_index & (0x01 << idx)))
        {
            usbotgh->pipe_index |= (0x01 << idx);
            return idx;
        }
    }

    return 0xff;
}

static void rt_usbotgh_free_pipe_index(usbotgh_t usbotgh, rt_uint8_t index)
{
    usbotgh->pipe_index &= ~(0x01 << index);
}

static rt_err_t rt_usbotgh_open_pipe(upipe_t pipe)
{
    usbotgh_t usbotgh = (usbotgh_t)pipe->inst->hcd->parent.user_data;
    pipe->pipe_index = rt_usbotgh_get_free_pipe_index(usbotgh);

    HAL_HCD_HCInit(&usbotgh->instance,
                   pipe->pipe_index,
                   pipe->ep.bEndpointAddress,
                   pipe->inst->address,
                   USB_OTG_SPEED_HIGH,
                   pipe->ep.bmAttributes,
                   pipe->ep.wMaxPacketSize);
    /* Set DATA0 PID token*/
    if (usbotgh->instance.hc[pipe->pipe_index].epIsIn)
    {
        usbotgh->instance.hc[pipe->pipe_index].toggleIn = 0;
    }
    else
    {
        usbotgh->instance.hc[pipe->pipe_index].toggleOut = 0;
    }

    return RT_EOK;
}

static rt_err_t rt_usbotgh_close_pipe(upipe_t pipe)
{
    usbotgh_t usbotgh = (usbotgh_t)pipe->inst->hcd->parent.user_data;

    HAL_HCD_HCHalt(&usbotgh->instance, pipe->pipe_index);
    rt_usbotgh_free_pipe_index(usbotgh, pipe->pipe_index);

    return RT_EOK;
}

static void rt_usbotgh_irq(usbotgh_t usbotgh)
{
    rt_interrupt_enter();
    HAL_HCD_IRQHandler(&usbotgh->instance);
    rt_interrupt_leave();
}

static void rt_usbotgh_vbus_enable(usbotgh_t usbotgh)
{
    switch (usbotgh->id)
    {
    case 0:
#ifdef USB_OTG_HOST0_VBUS_PIN
        rt_pin_mode(USB_OTG_HOST0_VBUS_PIN, PIN_MODE_OUTPUT);
        rt_pin_write(USB_OTG_HOST0_VBUS_PIN, PIN_HIGH);
#endif
        break;
    case 1:
#ifdef USB_OTG_HOST1_VBUS_PIN
        rt_pin_mode(USB_OTG_HOST1_VBUS_PIN, PIN_MODE_OUTPUT);
        rt_pin_write(USB_OTG_HOST1_VBUS_PIN, PIN_HIGH);
#endif
        break;
    default:
        break;
    }

    return;
}

static rt_err_t rt_usbotgh_hcd_init(rt_device_t device)
{
    usbotgh_t usbotgh = (usbotgh_t)device->user_data;
    struct HCD_HANDLE *hhcd = &usbotgh->instance;
    struct clk_gate *hclk_ctl_gate, *utmi_clk_gate;
    char name[RT_NAME_MAX];

    hhcd->pReg = usbotgh->dev->pReg;;
    hhcd->cfg.hcNum = usbotgh->dev->cfg.hcNum;
    hhcd->cfg.speed = usbotgh->dev->cfg.speed;
    hhcd->cfg.dmaEnable = usbotgh->dev->cfg.dmaEnable;
    hhcd->cfg.phyif = usbotgh->dev->cfg.phyif;;
    hhcd->cfg.sofEnable = usbotgh->dev->cfg.sofEnable;;

    /* Initialize usb clocks */
    hclk_ctl_gate = get_clk_gate_from_id(usbotgh->dev->hclkGateID);
    utmi_clk_gate = get_clk_gate_from_id(usbotgh->dev->utmiclkGateID);
    clk_enable(hclk_ctl_gate);
    clk_enable(utmi_clk_gate);

    /* Host interrupt init */
    rt_hw_interrupt_install(usbotgh->dev->irqNum, (rt_isr_handler_t)usbotgh->irq_handler, RT_NULL, usbotgh->name);
    rt_hw_interrupt_umask(usbotgh->dev->irqNum);

    rt_sprintf(name, "usbwq%d", usbotgh->id);
    usbotgh->usbh_workqueue = rt_workqueue_create(name, OTG_HCD_WORK_QUEUE_STACK_SIZE,
                              OTG_HCD_WORK_QUEUE_PRIORITY);
    RT_ASSERT(usbotgh->usbh_workqueue);

    HAL_USB_PhyResume(usbotgh->id);

    RT_ASSERT(HAL_HCD_Init(hhcd) == HAL_OK);
    HAL_HCD_Start(hhcd);

    rt_usbotgh_vbus_enable(usbotgh);

    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops rt_usbotgh_ops =
{
    .init = rt_usbotgh_hcd_init
};
#endif

int rt_usbotgh_register(void)
{
    struct rockchip_usbotgh *const *usbotgh;

    for (usbotgh = rt_usbotgh_table; *usbotgh != RT_NULL; usbotgh++)
    {
        uhcd_t uhcd = (uhcd_t)rt_malloc(sizeof(struct uhcd));

        RT_ASSERT(uhcd != RT_NULL);
        rt_memset((void *)uhcd, 0, sizeof(struct uhcd));

        uhcd->parent.type = RT_Device_Class_USBHost;
#ifdef RT_USING_DEVICE_OPS
        uhcd->parent.ops = &rt_usbotgh_ops;
#else
        uhcd->parent.init = rt_usbotgh_hcd_init;
#endif
        uhcd->parent.user_data = *usbotgh;
        uhcd->ops = (*usbotgh)->ops;
        uhcd->num_ports = 1;
        (*usbotgh)->instance.pData = *usbotgh;
        (*usbotgh)->uhcd = uhcd;

        rt_device_register((rt_device_t)uhcd, (*usbotgh)->name, 0);
        rt_usb_host_init((*usbotgh)->name);
    }

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_usbotgh_register);
#endif
