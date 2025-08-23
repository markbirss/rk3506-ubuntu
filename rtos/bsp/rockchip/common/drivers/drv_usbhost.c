/*
 * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * @file    drv_usbhost.c
 * @author  Frank Wang
 * @version V0.1
 * @date    28-Apr-2024
 * @brief   usb2 host driver
 *
 ******************************************************************************
 */
#include <rtconfig.h>
#if defined(RT_USING_USBH_EHCI) || defined(RT_USING_USBH_OHCI)
#include <rtdevice.h>
#include <rthw.h>
#include <drivers/usb_host.h>
#include "drv_heap.h"
#include "drv_clock.h"
#include "hal_base.h"
#include "hal_bsp.h"
#include "board.h"

#ifdef FL_SIZE
#define RT_EHCI_FL_SIZE    FL_SIZE
#else
#define RT_EHCI_FL_SIZE    512
#endif
#define RT_USBHOST_HUB_POLLING_INTERVAL    (500)  /* 500ms */
#define RT_MAX_USBH_PORT    1 /* USB1.1 + USB2.0 shared port */
#define RT_MAX_USBH_PIPE    16
#define RT_MAX_USBH_HUB_PORT_DEV    USB_HUB_PORT_NUM

struct rt_usbh_port_dev
{
    rt_bool_t rh_parent;
    struct USB_DEV *udev;
    struct USB_EP_INFO *ep_info[RT_MAX_USBH_PIPE];
    struct urequest setup_req[RT_MAX_USBH_PIPE];
    struct rt_completion utr_completion;
    int port_num;
    rt_bool_t enum_done;
#if defined(RT_USING_CACHE)
    void *pipe_buf[RT_MAX_USBH_PIPE];
#endif
};

struct rt_usbh_port
{
    struct rt_usbh_port_dev rhub_dev;
    struct rt_usbh_port_dev hub_dev[RT_MAX_USBH_HUB_PORT_DEV];
};

struct rt_usbh_dev
{
    struct uhcd uhcd;
    struct USB_HCD_HANDLE hcd_hdl;
    struct rt_usbh_port ports[RT_MAX_USBH_PORT];
    rt_thread_t polling_thread;
    struct USB_DEV *udev_list;
};

/* Private variables */
static struct rt_usbh_dev g_usbh;
/* Non-cacheable Memory alloc/free */

static void *rt_usbh_malloc_uncache_align(uint32_t size, uint32_t align)
{
    void *align_ptr;
    void *ptr;
    uint32_t align_size;
    align_size = RT_ALIGN(size, align) + align;
    ptr = rt_malloc_uncache(align_size);
    if (ptr != RT_NULL)
    {
        /* the allocated memory block is aligned */
        if (((rt_uint32_t)ptr & (align - 1)) == 0)
        {
            align_ptr = (void *)((rt_uint32_t)ptr + align);
        }
        else
        {
            align_ptr = (void *)(((rt_uint32_t)ptr + (align - 1)) & ~(align - 1));
        }
        /* set the pointer before alignment pointer to the real pointer */
        *((rt_uint32_t *)((rt_uint32_t)align_ptr - sizeof(void *))) = (rt_uint32_t)ptr;
        ptr = align_ptr;
    }
    return ptr;
}

struct USB_DEV *rt_usbh_alloc_udev(void)
{
    struct USB_DEV  *udev;
    udev = (struct USB_DEV *)rt_malloc(sizeof(*udev));
    if (!udev)
    {
        rt_kprintf("%s failed\n", __func__);
        return RT_NULL;
    }
    rt_memset(udev, 0, sizeof(*udev));
    udev->next = g_usbh.udev_list; /* chain to global device list */
    g_usbh.udev_list = udev;
    return udev;
}

void rt_usbh_free_udev(struct USB_DEV *udev)
{
    struct USB_DEV  *d;
    if (udev == RT_NULL)
        return;
    /* Remove it from the global device list */
    if (g_usbh.udev_list == udev)
    {
        g_usbh.udev_list = g_usbh.udev_list->next;
    }
    else
    {
        d = g_usbh.udev_list;
        while (d != RT_NULL)
        {
            if (d->next == udev)
            {
                d->next = udev->next;
                break;
            }
            d = d->next;
        }
    }
    rt_free(udev);
}

static struct rt_usbh_port *rt_usbh_get_port_from_pipe(upipe_t pipe)
{
    uinst_t inst;
    int portnum;
    if (pipe->inst->parent_hub->is_roothub)
    {
        /* device ---> root hub */
        inst = pipe->inst;
        portnum = inst->port;
    }
    else
    {
        /* device ---> hub ---> root hub */
        inst = pipe->inst->parent_hub->self;
        portnum = inst->port;
    }
    if (portnum > RT_MAX_USBH_PORT)
    {
        RT_DEBUG_LOG(RT_DEBUG_USB, ("%s ERROR: port index over RT_MAX_USBH_PORT\n", __func__));
        return RT_NULL;
    }
    return &g_usbh.ports[portnum - 1];
}

static struct rt_usbh_port_dev *rt_usbh_get_portdev_from_pipe(upipe_t pipe)
{
    struct rt_usbh_port *port = rt_usbh_get_port_from_pipe(pipe);
    int i;
    if (port == RT_NULL)
        return RT_NULL;
    if (pipe->inst->parent_hub->is_roothub)
    {
        /* device ---> root hub */
        return &port->rhub_dev;
    }
    /* device ---> hub ---> root hub */
    for (i = 0 ; i < RT_MAX_USBH_HUB_PORT_DEV; i ++)
    {
        if (port->hub_dev[i].port_num == pipe->inst->port)
            break;
    }
    if (i >= RT_MAX_USBH_HUB_PORT_DEV)
        return RT_NULL;
    return &port->hub_dev[i];
}

static rt_err_t rt_usbh_reset_port(rt_uint8_t portnum)
{
    struct rt_usbh_port *port;
    if (portnum > RT_MAX_USBH_PORT)
    {
        RT_DEBUG_LOG(RT_DEBUG_USB, ("%s ERROR: port index over RT_MAX_USBH_PORT\n", __func__));
        return -RT_EIO;
    }
    port = &g_usbh.ports[portnum - 1];
    if (port->rhub_dev.udev == RT_NULL)
    {
        RT_DEBUG_LOG(RT_DEBUG_USB, ("%s ERROR: udev not found\n", __func__));
        return -RT_EIO;
    }
    HAL_USBH_ResetPort(&g_usbh.hcd_hdl, port->rhub_dev.udev);
    return RT_EOK;
}

static struct USB_EP_INFO *rt_usbh_get_pipe(struct rt_usbh_port *port,
        struct rt_usbh_port_dev *port_dev, rt_uint8_t *pipe_idx)
{
    rt_uint8_t i;
    struct USB_EP_INFO *ep_info;
    if (!port)
        return RT_NULL;
    for (i = 1; i < RT_MAX_USBH_PIPE; i ++)
    {
        if (port_dev->ep_info[i] == RT_NULL)
            break;
    }
    if (i >= RT_MAX_USBH_PIPE)
        return RT_NULL;
    ep_info = rt_malloc(sizeof(struct USB_EP_INFO));
    if (!ep_info)
    {
        rt_kprintf("%s: ep_info malloc failed\n", __func__);
        return RT_NULL;
    }
    port_dev->ep_info[i] = ep_info;
    *pipe_idx = i;
    return ep_info;
}

static void rt_usbh_free_pipe(struct rt_usbh_port *port,
                              struct rt_usbh_port_dev *port_dev,
                              rt_uint8_t pipe_idx)
{
    if (!port || pipe_idx >= RT_MAX_USBH_PIPE)
        return;
    if (port_dev->ep_info[pipe_idx])
    {
        rt_free(port_dev->ep_info[pipe_idx]);
        port_dev->ep_info[pipe_idx] = RT_NULL;
    }
}

static struct rt_usbh_port_dev *rt_usbh_alloc_port_dev(struct rt_usbh_port *port)
{
    int i;
    for (i = 0; i < RT_MAX_USBH_HUB_PORT_DEV; i++)
        if (port->hub_dev[i].udev == RT_NULL)
            break;
    if (i >= RT_MAX_USBH_HUB_PORT_DEV)
        return RT_NULL;
    port->hub_dev[i].udev = rt_usbh_alloc_udev();
    if (port->hub_dev[i].udev == RT_NULL)
        return RT_NULL;
    return &port->hub_dev[i];
}
static rt_err_t rt_usbh_open_pipe(upipe_t pipe)
{
    struct rt_usbh_port *port;
    struct rt_usbh_port_dev *port_dev;
    struct USB_EP_INFO *ep_info;
    int  pksz;
    port = rt_usbh_get_port_from_pipe(pipe);
    if (port == RT_NULL)
    {
        rt_kprintf("%s: get rh port failed\n", __func__);
        goto exit;
    }
    if (port->rhub_dev.udev == RT_NULL)
    {
        rt_kprintf("%s: rh udev is not found\n", __func__);
        goto exit;
    }
    port_dev = rt_usbh_get_portdev_from_pipe(pipe);
    if (!port_dev || !port_dev->udev)
    {
        /* Allocate new dev for hub device */
        port_dev = rt_usbh_alloc_port_dev(port);
        if (!port_dev)
        {
            rt_kprintf("%s port udev allocate failed\n", __func__);
            goto exit;
        }
        port_dev->udev->speed = pipe->inst->speed ? USB_SPEED_FULL : USB_SPEED_HIGH;
        port_dev->udev->ehciPort = port->rhub_dev.udev->ehciPort;
        port_dev->udev->portNum = pipe->inst->port;
        port_dev->port_num = pipe->inst->port;
        port_dev->enum_done = RT_FALSE;
    }
    /* For ep0 control transfer */
    if (!(pipe->ep.bEndpointAddress & 0x7F))
    {
        pipe->pipe_index = 0;
    }
    else
    {
        ep_info = rt_usbh_get_pipe(port, port_dev, &pipe->pipe_index);
        if (ep_info == RT_NULL)
        {
            rt_kprintf("%s: get free pipe failed\n", __func__);
            goto exit;
        }
        ep_info->bEndpointAddress = pipe->ep.bEndpointAddress;
        ep_info->bmAttributes = pipe->ep.bmAttributes;
        pksz = pipe->ep.wMaxPacketSize;
        pksz = (pksz & 0x07ff) * (1 + ((pksz >> 11) & 3));
        ep_info->wMaxPacketSize = pksz;
        ep_info->bInterval = pipe->ep.bInterval;
        ep_info->hwPipe = RT_NULL;
        ep_info->bToggle = 0;
    }
#if defined(RT_USING_CACHE)
    if (!port_dev->pipe_buf[pipe->pipe_index])
    {
        port_dev->pipe_buf[pipe->pipe_index] = rt_malloc_align(512U, CACHE_LINE_SIZE);
        RT_ASSERT(port_dev->pipe_buf[pipe->pipe_index] != RT_NULL);
    }
#endif
    return RT_EOK;
exit:
    return -RT_ERROR;
}

static rt_err_t rt_usbh_close_pipe(upipe_t pipe)
{
    struct rt_usbh_port *port;
    struct rt_usbh_port_dev *port_dev;
    int i;
    port = rt_usbh_get_port_from_pipe(pipe);
    if (!port)
        return -RT_EIO;
    port_dev = rt_usbh_get_portdev_from_pipe(pipe);
    /* For ep0 control transfer */
    if (!(pipe->ep.bEndpointAddress & 0x7F))
    {
        if (port_dev && (port_dev->rh_parent == RT_FALSE) &&
                (port_dev->enum_done == RT_TRUE))
        {
            if (port_dev->udev)
            {
                for (i = 0; i < RT_MAX_USBH_PIPE; i++)
                {
                    if (port_dev->ep_info[i] != RT_NULL)
                    {
                        HAL_USBH_QuitXfer(&g_usbh.hcd_hdl, port_dev->udev, port_dev->ep_info[i]);
                    }
                }
                rt_usbh_free_udev(port_dev->udev);
                port_dev->udev = RT_NULL;
            }
        }
    }
    if (port_dev != RT_NULL)
    {
#if defined(RT_USING_CACHE)
        if (port_dev->pipe_buf[pipe->pipe_index])
        {
            rt_free_align(port_dev->pipe_buf[pipe->pipe_index]);
            port_dev->pipe_buf[pipe->pipe_index] = RT_NULL;
        }
#endif
        rt_usbh_free_pipe(port, port_dev, pipe->pipe_index);
    }
    return RT_EOK;
}

static void xfer_done_cb(struct UTR *utr)
{
    struct rt_usbh_port_dev *port_dev = (struct rt_usbh_port_dev *)utr->context;
    /* Transfer done, signal utr_completion */
    rt_completion_done(&(port_dev->utr_completion));
}

static void intr_xfer_done_cb(struct UTR *utr)
{
    struct uhost_msg msg;
    upipe_t pipe = (upipe_t)utr->context;
    if (utr->status)
    {
        rt_kprintf("%s: interrupt xfer failed %d\n", __func__, utr->status);
        return;
    }
    if (pipe->callback)
    {
        msg.type = USB_MSG_CALLBACK;
        msg.content.cb.function = pipe->callback;
        msg.content.cb.context = pipe;
        rt_usbh_event_signal(&g_usbh.uhcd, &msg);
    }
}

static void rt_usbh_setup_utr(struct UTR *utr, struct urequest *setup,
                              void *buffer, int size, uint8_t pipe_index,
                              struct rt_usbh_port_dev *port_dev)
{
    rt_memset(utr, 0, sizeof(struct UTR));
    utr->udev = port_dev->udev;
    utr->dataLen = size;
    utr->buff = buffer;
    utr->pHCD = HAL_USBH_GetHcd(&g_usbh.hcd_hdl, port_dev->udev->ehciPort);
    /* For control ep */
    if (setup)
    {
        rt_memcpy(&utr->setup, setup, sizeof(struct urequest));
        utr->udev->ep0.wMaxPacketSize = port_dev->udev->ep0.wMaxPacketSize;
#if defined(RT_USING_CACHE)
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, &utr->setup, sizeof(struct USB_SETUP_REQ));
#endif
    }
    else
    {
        utr->ep = port_dev->ep_info[pipe_index];
        utr->CompleteCb = xfer_done_cb;
        utr->context = port_dev;
    }
}

static int rt_usbh_ctrl_xfer(struct rt_usbh_port_dev *port_dev,
                             struct urequest *setup, struct UTR *utr,
                             void *buffer, int timeouts)
{
    uint32_t  xfer_len = 0;
    int    ret;
    rt_usbh_setup_utr(utr, setup, buffer, setup->wLength, 0, port_dev);
    ret = HAL_USBH_CtrlXfer(&g_usbh.hcd_hdl, port_dev->udev, &xfer_len, utr, timeouts * 10);
    if (ret < 0)
    {
        rt_kprintf("%s: xfer failed %d\n", __func__, ret);
        return ret;
    }
    if (xfer_len != setup->wLength)
    {
        rt_kprintf("%s: xfer length %d %d\n", __func__, setup->wLength, xfer_len);
    }
    if ((setup->bRequest == USB_REQ_SET_ADDRESS) && ((setup->request_type & 0x60) == USB_REQ_TYPE_DEVICE))
        port_dev->udev->devNum = setup->wValue;
    if ((setup->bRequest == USB_REQ_SET_CONFIGURATION) && ((setup->request_type & 0x60) == USB_REQ_TYPE_DEVICE))
        port_dev->enum_done = RT_TRUE;

    return xfer_len;
}

static int rt_usbh_bulk_xfer(struct rt_usbh_port_dev *port_dev,
                             struct UTR *utr, int timeouts)
{
    int ret;
    ret = HAL_USBH_BulkXfer(&g_usbh.hcd_hdl, utr);
    if (ret < 0)
        return ret;
    /* Wait transfer done */
    rt_completion_wait(&(port_dev->utr_completion), timeouts);
    return RT_EOK;
}

static int rt_usbh_intr_xfer(upipe_t pipe, struct rt_usbh_port_dev *port_dev,
                             struct UTR *utr, int timeouts)
{
    int ret;
    int retry = 3;
    rt_tick_t delay_tick;
    while (retry > 0)
    {
        ret = HAL_USBH_IntrXfer(&g_usbh.hcd_hdl, utr);
        if (!ret)
            break;
        rt_kprintf("%s: failed to submit interrupt request\n", __func__);
        delay_tick = (pipe->ep.bInterval * RT_TICK_PER_SECOND / 1000) ?
                     (pipe->ep.bInterval * RT_TICK_PER_SECOND / 1000) : 1;
        rt_thread_delay(delay_tick);
        retry --;
    }
    if (ret < 0)
        return ret;
    return RT_EOK;
}

static int rt_usbh_pipe_xfer(upipe_t pipe, rt_uint8_t token, void *buffer,
                             int nbytes, int timeouts)
{
    struct rt_usbh_port *port;
    struct rt_usbh_port_dev *port_dev;
    struct UTR utr;
    int ret;
    int xfer_len = -1;
    void *buffer_nonch = buffer;

    port = rt_usbh_get_port_from_pipe(pipe);
    if (!port)
        goto exit;
    port_dev = rt_usbh_get_portdev_from_pipe(pipe);
    if (!port_dev->udev)
    {
        rt_kprintf("%s: udev not found\n", __func__);
        goto exit;
    }

#if defined(RT_USING_CACHE)
    if (buffer_nonch && nbytes)
    {
        buffer_nonch = port_dev->pipe_buf[pipe->pipe_index];
        rt_memcpy(buffer_nonch, buffer, nbytes);
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, buffer_nonch, nbytes);
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, buffer_nonch, nbytes);
    }
#endif
    /* ctrl xfer */
    if (pipe->ep.bmAttributes == USB_EP_ATTR_CONTROL)
    {
        if (token == USBH_PID_SETUP)
        {
            struct urequest *setup = (struct urequest *)buffer_nonch;
            RT_ASSERT(buffer_nonch != RT_NULL);
            /* Read data from USB device. */
            if (setup->request_type & USB_REQ_TYPE_DIR_IN)
            {
                //Store setup request
                rt_memcpy(&port_dev->setup_req[pipe->pipe_index], setup, sizeof(struct urequest));
                port_dev->udev->ep0.wMaxPacketSize = pipe->ep.wMaxPacketSize;
#if defined(RT_USING_CACHE)
                rt_hw_cpu_dcache_ops(RT_HW_CACHE_FLUSH, &port_dev->setup_req[pipe->pipe_index], sizeof(struct urequest));
#endif
            }
            else
            {
                /* Write data to USB device. */
                ret = rt_usbh_ctrl_xfer(port_dev, setup, &utr, RT_NULL, timeouts);
                if (ret != setup->wLength)
                    goto exit;
            }
        }
        else
        {
            //token == USBH_PID_DATA
            if (buffer_nonch && ((pipe->ep.bEndpointAddress & USB_DIR_MASK) == USB_DIR_IN))
            {
                /* Read data from USB device. */
                ret = rt_usbh_ctrl_xfer(port_dev, &port_dev->setup_req[pipe->pipe_index], &utr, buffer_nonch, timeouts);
                if (ret != nbytes)
                    goto exit;
            }
            else
            {
                RT_DEBUG_LOG(RT_DEBUG_USB, ("%d == USBH_PID_DATA, nil buf-%d \n", token, nbytes));
            }
        }
        xfer_len = nbytes;
        goto exit;
    }
    else
    {
        //others xfer
        rt_usbh_setup_utr(&utr, RT_NULL, buffer_nonch, nbytes, pipe->pipe_index, port_dev);
        if (pipe->ep.bmAttributes == USB_EP_ATTR_BULK)
        {
            rt_completion_init(&(port_dev->utr_completion));
            ret = rt_usbh_bulk_xfer(port_dev, &utr, timeouts);
            if (ret < 0)
            {
                rt_kprintf("%s ERROR: bulk transfer failed\n", __func__);
                goto exit;
            }
        }
        else if (pipe->ep.bmAttributes == USB_EP_ATTR_INT)
        {
            utr.CompleteCb = intr_xfer_done_cb;
            utr.context = pipe;
            ret = rt_usbh_intr_xfer(pipe, port_dev, &utr, timeouts);
            if (ret == RT_EOK)
                xfer_len = nbytes;
            else
                rt_kprintf("%s ERROR: intr transfer failed\n", __func__);
            return xfer_len;
        }
        else if (pipe->ep.bmAttributes == USB_EP_ATTR_ISOC)
        {
            // TODO: ISO transfer
            rt_kprintf("%s: isoc transfer not support\n", __func__);
            goto exit;
        }
    }
    if (!utr.bIsTransferDone)
    {
        rt_kprintf("%s: timeout\n", __func__);
        pipe->status = UPIPE_STATUS_ERROR;
        HAL_USBH_QuitUtr(&g_usbh.hcd_hdl, &utr);
    }
    else
    {
        /* Transfer Done, get status */
        if (utr.status == 0)
            pipe->status = UPIPE_STATUS_OK;
        else if (utr.status == USBH_ERR_STALL)
            pipe->status = UPIPE_STATUS_STALL;
        else
            pipe->status = UPIPE_STATUS_ERROR;
    }
    if (pipe->status != UPIPE_STATUS_OK)
        RT_DEBUG_LOG(RT_DEBUG_USB, ("%s Transfer Done, status: 0x%02x\n", __func__, pipe->status));
    xfer_len = utr.xferLen;
    if (pipe->callback != RT_NULL)
    {
        struct uhost_msg msg;
        msg.type = USB_MSG_CALLBACK;
        msg.content.cb.function = pipe->callback;
        msg.content.cb.context = pipe->user_data;
        rt_usbh_event_signal(&g_usbh.uhcd, &msg);
    }
exit:
#if defined(RT_USING_CACHE)
    if (nbytes && buffer_nonch != buffer)
    {
        rt_hw_cpu_dcache_ops(RT_HW_CACHE_INVALIDATE, buffer_nonch, nbytes);
        rt_memcpy(buffer, buffer_nonch, nbytes);
    }
#endif

    return xfer_len;
}

/* Polling USB root hub status task */
static void rt_usbh_rh_thread_entry(void *parameter)
{
    while (1)
    {
        HAL_USBH_PollingRtHubs(&g_usbh.hcd_hdl, g_usbh.udev_list);
        rt_thread_mdelay(RT_USBHOST_HUB_POLLING_INTERVAL);
    }
}
static void rt_usbh_ehci_irq_isr(void)
{
#ifdef RT_USING_USBH_EHCI
    rt_interrupt_enter();
    HAL_EHCI_IRQHandler(&g_usbh.hcd_hdl.ehci);
    rt_interrupt_leave();
#endif
}
static void usbh_ohci_irq_isr(void)
{
#ifdef RT_USING_USBH_OHCI
    rt_interrupt_enter();
    HAL_OHCI_IRQHandler(&g_usbh.hcd_hdl.ohci);
    rt_interrupt_leave();
#endif
}

#ifdef USB_HOST_VBUS_PIN
static void usb_host_vbus_enable(void)
{
    rt_pin_mode(USB_HOST_VBUS_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(USB_HOST_VBUS_PIN, PIN_HIGH);
}
#endif

static rt_err_t rt_usb_hcd_init(rt_device_t device)
{
    struct rt_usbh_dev *hdev = (struct rt_usbh_dev *)device;
    struct clk_gate *usbh_clk, *usbh_arb_clk, *utmi_clk_gate;
    uint32_t size;
    void *coherent_mem;
    /*
     * Alloc coherent memory for EHCI/OHCI HAL libs.
     *
     * Memory layout (size/aligned):
     * ------------------------------------------------------
     * | Pool (16K/64B) | Hcca (256B/256B) | PFList (2K/4K) |
     * ------------------------------------------------------
     */
    size =  MEM_POOL_UNIT_NUM * MEM_POOL_UNIT_SIZE; /* QH/qTD/... pool */
    size += 512; /* OHCI HCCA */
    size += RT_EHCI_FL_SIZE * sizeof(uint32_t) + 4096; /* EHCI PF List */

    coherent_mem = rt_usbh_malloc_uncache_align(size, MEM_POOL_UNIT_SIZE);
    if (!coherent_mem)
    {
        rt_kprintf("%s: coherent memory alloc failed\n", __func__);
        return -RT_ENOMEM;
    }

    rt_memset(coherent_mem, 0, size);
    RT_DEBUG_LOG(1, ("\ncoherent mem, start: 0x%08x, size: %d\n", (uint32_t)coherent_mem,  size));
    hdev->hcd_hdl.ehci.pReg = g_usbhDev.ehciReg;
    hdev->hcd_hdl.ohci.pReg = g_usbhDev.ohciReg;
    /* Initialize usb clocks */
    usbh_clk = get_clk_gate_from_id(g_usbhDev.usbhGateID);
    usbh_arb_clk = get_clk_gate_from_id(g_usbhDev.usbhArbGateID);
    utmi_clk_gate = get_clk_gate_from_id(g_usbhDev.utmiclkGateID);
    clk_enable(usbh_clk);
    clk_enable(usbh_arb_clk);
    clk_enable(utmi_clk_gate);

    HAL_USBH_CoreInit(&hdev->hcd_hdl, &coherent_mem);
    //HAL_USBH_PollingRtHubs(&g_usbh.hcd_hdl, g_usbh.udev_list);
    /* Host interrupt init */

    rt_hw_interrupt_install(g_usbhDev.ehciIrqNum, (rt_isr_handler_t)rt_usbh_ehci_irq_isr, RT_NULL, "ehci_irq");
    rt_hw_interrupt_install(g_usbhDev.ohciIrqNum, (rt_isr_handler_t)usbh_ohci_irq_isr, RT_NULL, "ohci_irq");
    rt_hw_interrupt_umask(g_usbhDev.ehciIrqNum);
    rt_hw_interrupt_umask(g_usbhDev.ohciIrqNum);
    /* create thread for polling usbh port status */
    hdev->polling_thread = rt_thread_create("usbh_drv",
                                            rt_usbh_rh_thread_entry, RT_NULL,
                                            2048, 10, 20);
    RT_ASSERT(hdev->polling_thread != RT_NULL);
    /* startup usb host thread */
    rt_thread_startup(hdev->polling_thread);

#ifdef USB_HOST_VBUS_PIN
    usb_host_vbus_enable();
#endif

    return RT_EOK;
}

HAL_Status HAL_USBH_ConnectCallback(int speed)
{
    int i;
    int port_index;
    struct rt_usbh_port *port;
    struct USB_DEV *udev;
    for (i = 0; i < RT_MAX_USBH_PORT; i++)
    {
        port = &g_usbh.ports[i];
        if (port->rhub_dev.udev == RT_NULL)
            break;
    }
    if (i >= RT_MAX_USBH_PORT)
    {
        rt_kprintf("ERROR: port connect slot is full\n");
        return HAL_NODEV;
    }
    udev = rt_usbh_alloc_udev();
    if (!udev)
        return HAL_ERROR;
    //udev->portNum = 1;
    udev->speed = speed;
    udev->ehciPort = speed < USB_SPEED_HIGH ? 0 : 1 ;
    port_index = i + 1;
    port->rhub_dev.udev = udev;
    port->rhub_dev.rh_parent = RT_TRUE;
    rt_kprintf("USB connect, speed %d\n", speed);
    if (speed == USB_SPEED_HIGH)
        rt_usbh_root_hub_connect_handler(&g_usbh.uhcd, port_index, RT_TRUE);
    else
        rt_usbh_root_hub_connect_handler(&g_usbh.uhcd, port_index, RT_FALSE);
    return HAL_OK;
}

HAL_Status HAL_USBH_DisconnectCallback(struct USB_DEV *udev)
{
    int i;
    int port_index;
    struct rt_usbh_port *port;
    for (i = 0; i < RT_MAX_USBH_PORT; i++)
    {
        port = &g_usbh.ports[i];
        if (port->rhub_dev.udev == udev)
            break;
    }
    if (i >= RT_MAX_USBH_PORT)
    {
        rt_kprintf("ERROR: udev not found\n");
        return HAL_NODEV;
    }
    port_index = i + 1;
    for (i = 0; i < RT_MAX_USBH_PIPE; i++)
    {
        if (port->rhub_dev.ep_info[i] != RT_NULL)
        {
            HAL_USBH_QuitXfer(&g_usbh.hcd_hdl, port->rhub_dev.udev, port->rhub_dev.ep_info[i]);
        }
    }
    rt_usbh_free_udev(port->rhub_dev.udev);
    port->rhub_dev.udev = RT_NULL;
    rt_kprintf("USB disconnect\n");
    rt_usbh_root_hub_disconnect_handler(&g_usbh.uhcd, port_index);
    return HAL_OK;
}

HAL_Status HAL_USBH_MaskIrq(uint8_t isEhci)
{
    int irq = isEhci ? g_usbhDev.ehciIrqNum : g_usbhDev.ohciIrqNum;
    rt_hw_interrupt_mask(irq);
    return HAL_OK;
}
HAL_Status HAL_USBH_UnmaskIrq(uint8_t isEhci)
{
    int irq = isEhci ? g_usbhDev.ehciIrqNum : g_usbhDev.ohciIrqNum;
    rt_hw_interrupt_umask(irq);
    return HAL_OK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops rt_usbh_ops =
{
    .init = rt_usb_hcd_init
};
#endif

/* USB host operations */
static struct uhcd_ops rt_uhcd_ops =
{
    rt_usbh_reset_port,
    rt_usbh_pipe_xfer,
    rt_usbh_open_pipe,
    rt_usbh_close_pipe,
};

int rt_usbhost_register(void)
{
    rt_err_t ret;
    uhcd_t psUHCD;
    psUHCD = (uhcd_t)&g_usbh.uhcd;
    psUHCD->parent.type       = RT_Device_Class_USBHost;
#ifdef RT_USING_DEVICE_OPS
    psUHCD->parent.ops = &rt_usbh_ops;
#else
    psUHCD->parent.init = rt_usb_hcd_init;
#endif
    psUHCD->parent.user_data  = &g_usbh;
    psUHCD->ops               = &rt_uhcd_ops;
    psUHCD->num_ports         = RT_MAX_USBH_PORT;
    ret = rt_device_register(&psUHCD->parent, "usbhost", RT_DEVICE_FLAG_DEACTIVATE);
    RT_ASSERT(ret == RT_EOK);
    /* Initialize the usb host function */
    ret = rt_usb_host_init("usbhost");
    RT_ASSERT(ret == RT_EOK);
    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_usbhost_register);
#endif /* RT_USING_USBH_EHCI || RT_USING_USBH_OHCI */
