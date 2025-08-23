/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-06-01     tyustli     the first version
 */

#include <rtthread.h>
#include <rtdevice.h>

#include <string.h>
#include <stdlib.h>

#include "hal_base.h"
#include "board.h"

#include "touch.h"
#include "st1232.h"

//#define ST1232_DEBUG

static struct rt_i2c_client *st1232_client;
static rt_touch_t touch_device = RT_NULL;

static rt_err_t st1232_ts_write_data(struct rt_i2c_client *dev, rt_uint8_t write_len, rt_uint8_t *write_data)
{
    struct rt_i2c_msg msgs;

    msgs.addr  = dev->client_addr;
    msgs.flags = RT_I2C_WR;
    msgs.buf   = write_data;
    msgs.len   = write_len;

    if (rt_i2c_transfer(dev->bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

static rt_err_t st1232_ts_read_data(struct rt_i2c_client *dev, rt_uint16_t cmd, rt_uint8_t read_len, rt_uint8_t *read_buf)
{
    struct rt_i2c_msg msgs[2];
    rt_uint8_t cmd_buf[1];

    cmd_buf[0] = (rt_uint8_t)(cmd & 0xff);

    msgs[0].addr  = dev->client_addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = cmd_buf;
    msgs[0].len   = 1;

    msgs[1].addr  = dev->client_addr;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf   = read_buf;
    msgs[1].len   = read_len;

    if (rt_i2c_transfer(dev->bus, msgs, 2) == 2)
    {
#ifdef ST1232_DEBUG
        rt_uint16_t i;
        rt_kprintf("start read regs(0x%04x):", cmd);
        for (i = 0; i < read_len; i++)
        {
            rt_kprintf(" 0x%02x", read_buf[i]);
        }
        rt_kprintf("\n");
#endif
        return RT_EOK;
    }

    return -RT_ERROR;
}

static rt_err_t st1232_ts_wait_ready(void)
{
    rt_err_t ret;
    rt_uint8_t status = 0xff;
    unsigned int retries;

    for (retries = 100; retries; retries--)
    {
        ret = st1232_ts_read_data(st1232_client, REG_STATUS, 1, &status);
        if (!ret)
        {
            switch (status)
            {
            case STATUS_NORMAL | ERROR_NONE:
            case STATUS_IDLE | ERROR_NONE:
                return RT_EOK;
            }
        }
        rt_thread_mdelay(10);
    }
    return -RT_ERROR;
}

static rt_err_t st1232_ts_control(struct rt_touch_device *device, int cmd, void *data)
{
    rt_err_t ret;

    if (cmd == RT_TOUCH_CTRL_GET_ID)
    {
        rt_uint8_t *id = (rt_uint8_t *)data;
        id[0] = 's';
        id[1] = 't';
        id[2] = '1';
        id[3] = '2';
        id[4] = '3';
        id[5] = '3';
        id[6] = 0;
    }
    else if (cmd == RT_TOUCH_CTRL_GET_INFO)
    {
        struct rt_touch_info *info = (struct rt_touch_info *)data;
        rt_uint8_t opr_buf[7] = {0};
        ret = st1232_ts_read_data(st1232_client, REG_XY_RESOLUTION, 3, opr_buf);
        RT_ASSERT(ret == RT_EOK);

        info->range_x = (((opr_buf[0] & 0x0070) << 4) | opr_buf[1]);// - 1;
        info->range_y = (((opr_buf[0] & 0x0007) << 8) | opr_buf[2]);// - 1;
        info->point_num = ST_TS_MAX_FINGERS;

        rt_kprintf("max_X = %d, max_Y=%d\n", info->range_x, info->range_y);
    }

    return RT_EOK;
}

static int16_t pre_x[ST_TS_MAX_FINGERS] = {-1, -1, -1, -1, -1};
static int16_t pre_y[ST_TS_MAX_FINGERS] = {-1, -1, -1, -1, -1};
static int16_t pre_w[ST_TS_MAX_FINGERS] = {-1, -1, -1, -1, -1};
static rt_uint8_t s_tp_dowm[ST_TS_MAX_FINGERS];
static struct rt_touch_data *read_data;

static void st1232_ts_touch_up(void *buf, int8_t id)
{
    read_data = (struct rt_touch_data *)buf;

    if (s_tp_dowm[id] == 1)
    {
        s_tp_dowm[id] = 0;
        read_data[id].event = RT_TOUCH_EVENT_UP;
    }
    else
    {
        read_data[id].event = RT_TOUCH_EVENT_NONE;
    }

    read_data[id].timestamp = rt_touch_get_ts();
    read_data[id].width = pre_w[id];
    read_data[id].x_coordinate = pre_x[id];
    read_data[id].y_coordinate = pre_y[id];
    read_data[id].track_id = id;

    pre_x[id] = -1;  /* last point is none */
    pre_y[id] = -1;
    pre_w[id] = -1;
}

static void st1232_ts_touch_down(void *buf, int8_t id, int16_t x, int16_t y, int16_t w)
{
    read_data = (struct rt_touch_data *)buf;

    if (s_tp_dowm[id] == 1)
    {
        read_data[id].event = RT_TOUCH_EVENT_MOVE;

    }
    else
    {
        read_data[id].event = RT_TOUCH_EVENT_DOWN;
        s_tp_dowm[id] = 1;
    }

    read_data[id].timestamp = rt_touch_get_ts();
    read_data[id].width = w;
    read_data[id].x_coordinate = x;
    read_data[id].y_coordinate = y;
    read_data[id].track_id = id;

    pre_x[id] = x; /* save last point */
    pre_y[id] = y;
    pre_w[id] = w;
}

static rt_size_t st1232_ts_read_point(struct rt_touch_device *touch, void *buf, rt_size_t read_num)
{
    rt_err_t ret;
    rt_uint32_t i;
    rt_uint8_t point_status = 0;
    rt_uint8_t touch_num = 0;
    rt_uint8_t stxx_buf[4 * ST_TS_MAX_FINGERS] = {0};
    rt_uint8_t read_buf[8 * ST_TS_MAX_FINGERS] = {0};

    int8_t read_id = 0;
    rt_uint8_t read_index = 0;
    int16_t input_x = 0;
    int16_t input_y = 0;
    int16_t input_w = 0;
    static rt_uint8_t pre_touch = 0;
    static int8_t pre_id[ST_TS_MAX_FINGERS] = {0};

    rt_device_control(&touch->parent, RT_TOUCH_CTRL_DISABLE_INT, RT_NULL);

    /* point status register */
    if (st1232_ts_read_data(st1232_client, REG_XY_COORDINATES, 4 * ST_TS_MAX_FINGERS, stxx_buf) != RT_EOK)
    {
        rt_kprintf("read point failed\n");
        goto exit_;
    }

    touch_num = 0;
    rt_memset(read_buf, 0x00, 8 * ST_TS_MAX_FINGERS);

    for (i = 0; i < ST_TS_MAX_FINGERS; i++)
    {
        rt_uint8_t *p_stbuf = &stxx_buf[i * 4];
        rt_uint8_t *p_rdbuf = &read_buf[i * 8];

        if (p_stbuf[0] & 0x80) // check validate data
        {
            touch_num++;

            p_rdbuf[0] = i;                         // read_id
            p_rdbuf[1] = p_stbuf[1];                // x low
            p_rdbuf[2] = (p_stbuf[0] & 0x70) >> 4;  // x high
            p_rdbuf[3] = p_stbuf[2];                // y low
            p_rdbuf[4] = (p_stbuf[0] & 0x07);       // y high
            p_rdbuf[5] = 20;                        // size low
            p_rdbuf[6] = 0;                         // size high
        }
    }

#ifdef ST1232_DEBUG
    if (touch_num)
    {
        for (i = 0; i < touch_num; i++)
        {
            rt_kprintf("figers = %d, id = %d, x = %d, y = %d\n",
                       touch_num,
                       read_buf[i * 8 + 0] & 0x0f, //id
                       480 - (read_buf[i * 8 + 1] | (read_buf[i * 8 + 2] << 8)),   // x
                       read_buf[i * 8 + 3] | (read_buf[i * 8 + 4] << 8)            // y
                      );
        }
    }
    else
    {
        rt_kprintf("figers = %d, id = %d, x = %d, y = %d\n", 0, 0, 0, 0);
    }
#endif

    if (pre_touch > touch_num)                                       /* point up */
    {
        for (read_index = 0; read_index < pre_touch; read_index++)
        {
            rt_uint8_t j;

            for (j = 0; j < touch_num; j++)                          /* this time touch num */
            {
                read_id = read_buf[j * 8] & 0x0F;

                if (pre_id[read_index] == read_id)                   /* this id is not free */
                    break;

                if (j >= touch_num - 1)
                {
                    rt_uint8_t up_id;
                    up_id = pre_id[read_index];
                    st1232_ts_touch_up(buf, up_id);
                }
            }
        }
    }

    if (touch_num)                                                 /* point down */
    {
        rt_uint8_t off_set;

        for (read_index = 0; read_index < touch_num; read_index++)
        {
            off_set = read_index * 8;
            read_id = read_buf[off_set] & 0x0f;
            pre_id[read_index] = read_id;
            input_x = read_buf[off_set + 1] | (read_buf[off_set + 2] << 8); /* x */
            input_x = 480 - input_x; /* covert x */
            input_y = read_buf[off_set + 3] | (read_buf[off_set + 4] << 8); /* y */
            input_w = read_buf[off_set + 5] | (read_buf[off_set + 6] << 8); /* size */

            st1232_ts_touch_down(buf, read_id, input_x, input_y, input_w);
        }
    }
    else if (pre_touch)
    {
        for (read_index = 0; read_index < pre_touch; read_index++)
        {
            st1232_ts_touch_up(buf, pre_id[read_index]);
        }
    }

    pre_touch = touch_num;

exit_:
    rt_device_control(&touch->parent, RT_TOUCH_CTRL_ENABLE_INT, RT_NULL);

    return read_index;
}

static struct rt_touch_ops touch_ops =
{
    .touch_readpoint = st1232_ts_read_point,
    .touch_control   = st1232_ts_control,
};

int rt_hw_st1232_ts_init(void)
{
    rt_err_t ret;
    struct rt_touch_config cfg;
    struct rt_device_pin_mode rst_pin;
    const char *touch_name = TOUCH_DEV_NAME;

    RT_ASSERT(TOUCH_RST_PIN != 0);
    RT_ASSERT(TOUCH_IRQ_PIN != 0);
    RT_ASSERT(TOUCH_I2C_DEV != 0);

    /* touch config init */
    rst_pin.pin = TOUCH_RST_PIN;
    cfg.irq_pin.pin  = TOUCH_IRQ_PIN;
    cfg.irq_pin.mode = PIN_MODE_INPUT_PULLDOWN;
    cfg.dev_name = TOUCH_I2C_DEV;

    /* st1232 hardware init */
    /* before here, IOMUX must be initialized in board_xxxx.c*/
    rt_pin_mode(rst_pin.pin, PIN_MODE_OUTPUT);
    rt_pin_mode(cfg.irq_pin.pin, PIN_MODE_OUTPUT);
    rt_pin_write(rst_pin.pin, PIN_LOW);
    rt_thread_mdelay(10);
    rt_pin_write(rst_pin.pin, PIN_HIGH);
    rt_thread_mdelay(10);
    rt_pin_mode(cfg.irq_pin.pin, PIN_MODE_INPUT_PULLDOWN);
    rt_thread_mdelay(100);

    /* i2c interface bus */
    st1232_client = (struct rt_i2c_client *)rt_calloc(1, sizeof(struct rt_i2c_client));
    st1232_client->client_addr = ST1232_DEVICE_ADDRESS;
    st1232_client->bus = (struct rt_i2c_bus_device *)rt_device_find(cfg.dev_name);
    RT_ASSERT(st1232_client->bus != RT_NULL);

    ret = rt_device_open((rt_device_t)st1232_client->bus, RT_DEVICE_FLAG_RDWR);
    RT_ASSERT(ret == RT_EOK);

    /* Wait until device is ready */
    ret = st1232_ts_wait_ready();
    RT_ASSERT(ret == RT_EOK);

    /* register touch device */
    touch_device = (rt_touch_t)rt_calloc(1, sizeof(struct rt_touch_device));
    RT_ASSERT(touch_device != RT_NULL);

    touch_device->info.type = RT_TOUCH_TYPE_CAPACITANCE;
    touch_device->info.vendor = RT_TOUCH_VENDOR_GT;
    rt_memcpy(&touch_device->config, &cfg, sizeof(struct rt_touch_config));
    touch_device->ops = &touch_ops;
    rt_hw_touch_register(touch_device, touch_name, RT_DEVICE_FLAG_INT_RX, RT_NULL);

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_st1232_ts_init);
