/**
  * Copyright (c) 2024 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_touchkey.c
  * @author  Simon Xue
  * @version V0.1
  * @date    28-Feb-2024
  * @brief   touchkey driver
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rthw.h>

#if defined(RT_USING_TOUCHKEY)
#include "hal_base.h"
#include <rtdef.h>
#include <rtthread.h>
#include <rtdbg.h>
#include "drv_clock.h"
#include "drv_touchkey.h"

#define TOUCHKEY_NAME "rk_touchkey0"
#define KEY_RX_BUFFER_SIZE      32
#define KEY_NUM 20

#define TOUCHKEY_STABLE_COUNT 6

struct rt_touchkey_dev
{
    struct rt_device parent;
    struct TOUCH_SENSOR_REG *reg;
    rt_uint32_t  rx_buffer[KEY_RX_BUFFER_SIZE];
    rt_uint32_t read_index, save_index;
    rt_uint32_t delay_count;
    rt_uint32_t touchkey_release;
    rt_timer_t scan_timer;
    struct rt_touchkey_info info;
    rt_uint32_t last_code;
    rt_uint32_t save_code;
};

static struct rt_touchkey_dev touchkey;

struct touchkey_map
{
    rt_uint32_t chn;
    rt_uint32_t keycode;
};

#define TUOCHKEY_CHN0   HAL_BIT(0)
#define TUOCHKEY_CHN1   HAL_BIT(1)
#define TUOCHKEY_CHN2   HAL_BIT(2)
#define TUOCHKEY_CHN3   HAL_BIT(3)
#define TUOCHKEY_CHN4   HAL_BIT(4)
#define TUOCHKEY_CHN5   HAL_BIT(5)
#define TUOCHKEY_CHN6   HAL_BIT(6)

#define KEY_VAL_NONE        ((rt_uint32_t)0x0000)
#define KEY_VAL_PLAY        ((rt_uint32_t)0x0001 << 0)
#define KEY_VAL_MENU        ((rt_uint32_t)0x0001 << 1)
#define KEY_VAL_UP          ((rt_uint32_t)0x0001 << 2)
#define KEY_VAL_FFD         ((rt_uint32_t)0x0001 << 3)
#define KEY_VAL_DOWN        ((rt_uint32_t)0x0001 << 4)
#define KEY_VAL_FFW         ((rt_uint32_t)0x0001 << 5)
#define KEY_VAL_ESC         ((rt_uint32_t)0x0001 << 6)
#define KEY_VAL_VOL         ((rt_uint32_t)0x0001 << 7)

static const struct touchkey_map touchkeymap[] =
{
    {
        TUOCHKEY_CHN0, KEY_VAL_PLAY,
    },
    {
        TUOCHKEY_CHN1, KEY_VAL_MENU,
    },
    {
        TUOCHKEY_CHN2, KEY_VAL_UP,
    },
    {
        TUOCHKEY_CHN3, KEY_VAL_FFD,
    },
    {
        TUOCHKEY_CHN4, KEY_VAL_DOWN,
    },
    {
        TUOCHKEY_CHN5, KEY_VAL_FFW,
    },
    {
        TUOCHKEY_CHN6, KEY_VAL_ESC,
    },
};

#ifdef RT_USING_RTGUI
#include <rtgui/event.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/kbddef.h>

static int touchkey_map_gui[0xff] = {0};

static void rt_touchkeymap_init(void)
{
    touchkey_map_gui[0x1] = RTGUIK_ESCAPE;
    touchkey_map_gui[0xc] = RTGUIK_MINUS;
    touchkey_map_gui[0x39] = RTGUIK_SPACE;
    touchkey_map_gui[0xd] = RTGUIK_KP_EQUALS;
    touchkey_map_gui[0xe] = RTGUIK_BACKSPACE;
    touchkey_map_gui[0xf] = RTGUIK_TAB;
    touchkey_map_gui[0x1c] = RTGUIK_KP_ENTER;
    touchkey_map_gui[0x48] = RTGUIK_UP;
}

static rt_err_t rtgui_touchkey_rx(rt_device_t dev, rt_size_t size)
{
    struct rtgui_event_kbd kbd_event;
    char key_value;

    while (rt_device_read(dev, 0, &key_value, 1) == 1)
    {
        /* init keyboard event */
        RTGUI_EVENT_KBD_INIT(&kbd_event);
        kbd_event.mod  = RTGUI_KMOD_NONE;
        kbd_event.unicode = 0;
        kbd_event.key = RTGUIK_UNKNOWN;

        if (key_value &  0x80)
        {
            kbd_event.type = RTGUI_KEYUP;
        }
        else
        {
            kbd_event.type = RTGUI_KEYDOWN;
        }

        kbd_event.key = touchkey_map_gui[key_value & 0x7F];
    }
    if (kbd_event.key != RTGUIK_UNKNOWN)
    {
        /* post down event */
        rtgui_server_post_event(&(kbd_event.parent), sizeof(kbd_event));
    }

    return RT_EOK;
}

#endif

static void rt_touchkey_ind_callback(void)
{
    if (touchkey.parent.rx_indicate != RT_NULL)
    {
        rt_size_t rx_length;

        /* get rx length */
        rx_length = touchkey.read_index > touchkey.save_index ?
                    KEY_RX_BUFFER_SIZE - touchkey.read_index + touchkey.save_index :
                    touchkey.save_index - touchkey.read_index;

        touchkey.parent.rx_indicate(&touchkey.parent, rx_length);
    }
}

/* save a char to serial buffer */
static void rt_touchkey_savechar(char ch)
{
    rt_base_t level;

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    touchkey.rx_buffer[touchkey.save_index] = ch;
    touchkey.save_index ++;
    if (touchkey.save_index >= KEY_RX_BUFFER_SIZE)
        touchkey.save_index = 0;

    /* if the next position is read index, discard this 'read char' */
    if (touchkey.save_index == touchkey.read_index)
    {
        touchkey.read_index ++;
        if (touchkey.read_index >= KEY_RX_BUFFER_SIZE)
            touchkey.read_index = 0;
    }

    /* enable interrupt */
    rt_hw_interrupt_enable(level);
}

static rt_uint32_t rt_touchkey_channel_to_key(rt_uint32_t raw_status)
{
    rt_uint32_t i;

    for (i = 0; i < HAL_ARRAY_SIZE(touchkeymap); i++)
    {
        if (raw_status == touchkeymap[i].chn)
            return touchkeymap[i].keycode;
    }

    return KEY_VAL_NONE;
}

static void rt_touchkey_scan_timer_func(void *parameter)
{
    rt_uint32_t raw_status = HAL_TouchKey_GetIntRaw(TOUCH_SENSOR);
    rt_uint32_t key_code = 0;

    if (raw_status != 0)
    {
        key_code = rt_touchkey_channel_to_key(raw_status);
        if (key_code != 0)
        {
            if (key_code != touchkey.last_code)
            {
                touchkey.delay_count = 0;
                touchkey.last_code = key_code;
                touchkey.save_code = KEY_VAL_NONE;
            }
            else
            {
                if (touchkey.delay_count++ >= TOUCHKEY_STABLE_COUNT && touchkey.last_code != KEY_VAL_NONE)
                {
                    if (touchkey.save_code != touchkey.last_code)
                        touchkey.save_code = touchkey.last_code;
                }
            }
        }
    }
    else
    {
        rt_timer_stop(touchkey.scan_timer);

        if (touchkey.save_code != KEY_VAL_NONE)
            rt_touchkey_savechar(touchkey.save_code);

        touchkey.save_code = KEY_VAL_NONE;
        touchkey.last_code = KEY_VAL_NONE;
        touchkey.delay_count = 0;
        touchkey.touchkey_release = 1;
    }

    rt_touchkey_ind_callback();
}

static void rt_touchkey_pos_handler(int vector, void *param)
{
    rt_uint32_t pos_irq_st;

    pos_irq_st = HAL_TouchKey_GetIntPos(TOUCH_SENSOR);

    rt_interrupt_enter();

    if (touchkey.touchkey_release)
    {
        touchkey.touchkey_release = 0;
        rt_timer_start(touchkey.scan_timer);
    }

    HAL_TouchKey_ClearIntPos(pos_irq_st, TOUCH_SENSOR);

    rt_interrupt_leave();
}

static void rt_touchkey_neg_handler(int vector, void *param)
{
    rt_uint32_t neg_irq_st;

    neg_irq_st = HAL_TouchKey_GetIntNeg(TOUCH_SENSOR);

    rt_interrupt_enter();

    HAL_TouchKey_ClearIntNeg(neg_irq_st, TOUCH_SENSOR);

    rt_interrupt_leave();
}

static rt_err_t rt_touchkey_init(rt_device_t dev)
{
    if (!(dev->flag & RT_DEVICE_FLAG_ACTIVATED))
    {
        if (dev->flag & RT_DEVICE_FLAG_INT_RX)
        {
            rt_memset(touchkey.rx_buffer, 0,
                      sizeof(touchkey.rx_buffer));
            touchkey.read_index = touchkey.save_index = 0;
        }

        dev->flag |= RT_DEVICE_FLAG_ACTIVATED;
    }

    if (!touchkey.scan_timer)
    {
        touchkey.scan_timer = rt_timer_create("key_scan", rt_touchkey_scan_timer_func, RT_NULL,
                                              10, RT_TIMER_FLAG_PERIODIC);
        if (!touchkey.scan_timer)
            return RT_ERROR;
    }

    touchkey.save_code = KEY_VAL_NONE;
    touchkey.save_code = KEY_VAL_NONE;
    touchkey.touchkey_release = 1;

    return RT_EOK;
}

static rt_size_t rt_touchkey_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_uint8_t *ptr;
    rt_err_t err_code;
    rt_base_t level;

    ptr = buffer;
    err_code = RT_EOK;

    /* interrupt mode Rx */
    while (size)
    {
        if (touchkey.read_index != touchkey.save_index)
        {
            *ptr++ = touchkey.rx_buffer[touchkey.read_index];
            size --;

            /* disable interrupt */
            level = rt_hw_interrupt_disable();

            touchkey.read_index ++;
            if (touchkey.read_index >= KEY_RX_BUFFER_SIZE)
                touchkey.read_index = 0;

            /* enable interrupt */
            rt_hw_interrupt_enable(level);
        }
        else
        {
            /* set error code */
            err_code = -RT_EEMPTY;
            break;
        }
    }

    /* set error code */
    rt_set_errno(err_code);
    return (rt_uint32_t)ptr - (rt_uint32_t)buffer;
}

static rt_err_t rt_touchkey_control(rt_device_t dev, int cmd, void *args)
{
    switch (cmd)
    {
    case RT_TOUCHKEY_CTRL_GET_INFO:
        rt_memcpy(args, &touchkey.info, sizeof(struct rt_touchkey_info));
        break;
    default:
        break;
    }
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops touckkey_ops =
{
    rt_touchkey_init,
    NULL,
    NULL,
    rt_touchkey_read,
    RT_NULL,
    rt_touchkey_control,
};
#endif

static int rk_touchkey_init(void)
{
    int result = RT_EOK;

    /* clear device structure */
    rt_memset(&(touchkey.parent), 0, sizeof(struct rt_device));

    touchkey.parent.type         = RT_Device_Class_Char;
    touchkey.parent.tx_complete = RT_NULL;
#ifdef RT_USING_DEVICE_OPS
    touchkey.parent.ops = touckkey_ops;
#else
    touchkey.parent.init    = rt_touchkey_init;
    touchkey.parent.open    = NULL;
    touchkey.parent.close   = NULL;
    touchkey.parent.read    = rt_touchkey_read;
    touchkey.parent.write   = RT_NULL;
    touchkey.parent.control = rt_touchkey_control;
#endif
    touchkey.parent.user_data   = RT_NULL;

    touchkey.info.count = KEY_RX_BUFFER_SIZE;

    rt_hw_interrupt_install(TOUCH_KEY_POS_IRQn, rt_touchkey_pos_handler, RT_NULL, "pos_irq");
    rt_hw_interrupt_umask(TOUCH_KEY_POS_IRQn);

    rt_hw_interrupt_install(TOUCH_KEY_NEG_IRQn, rt_touchkey_neg_handler, RT_NULL, "neg_irq");
    rt_hw_interrupt_umask(TOUCH_KEY_NEG_IRQn);

#ifdef RT_USING_RTGUI
    touchkey.parent.rx_indicate = rtgui_touchkey_rx;
    /* init keymap */
    rt_touchkeymap_init();
#endif

    HAL_TouchKey_Init(KEY_NUM, TOUCH_SENSOR);
    /* register touchkey device to RT-Thread */
    result = rt_device_register(&(touchkey.parent), TOUCHKEY_NAME, RT_DEVICE_FLAG_RDONLY | RT_DEVICE_FLAG_INT_RX);

    return result;
}

INIT_DEVICE_EXPORT(rk_touchkey_init);

#endif
