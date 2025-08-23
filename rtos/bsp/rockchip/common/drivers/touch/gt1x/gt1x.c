/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-09-01     tony.zheng     the first version
 */

#include <rtthread.h>
#include <rtdevice.h>

#include <string.h>
#include <stdlib.h>

#include "hal_base.h"
#include "board.h"

#include "touch.h"
#include "gt1x.h"

//#define GT1X_DEBUG

static struct rt_i2c_client *gt1x_client;
static rt_touch_t touch_device = RT_NULL;

static CHIP_TYPE_T gt1x_chip_type = CHIP_TYPE_NONE;
static uint8_t gt1x_config[GTP_CONFIG_MAX_LENGTH] = { 0 };
static uint32_t gt1x_cfg_length = GTP_CONFIG_MAX_LENGTH;

static struct gt1x_version_info gt1x_version =
{
    .product_id = {0},
    .patch_id = 0,
    .mask_id = 0,
    .sensor_id = 0,
    .match_opt = 0
};

static uint8_t gt1x_wakeup_level = 0;
static uint8_t  gt1x_init_failed = 0;
static uint8_t  gt1x_int_type = 0;
static uint32_t gt1x_abs_x_max = 0;
static uint32_t gt1x_abs_y_max = 0;

static rt_err_t gt1x_write_reg(struct rt_i2c_client *dev, rt_uint16_t cmd, rt_uint8_t write_len, rt_uint8_t *write_data)
{
    struct rt_i2c_msg msgs;
    rt_uint8_t *data;

    data = (rt_uint8_t *)rt_calloc(1, write_len + GTP_ADDR_LENGTH);
    RT_ASSERT(data != RT_NULL);

    data[0] = (cmd >> 8) & 0xFF;
    data[1] = cmd & 0xFF;
    memcpy(&data[GTP_ADDR_LENGTH], write_data, write_len);

    msgs.addr  = dev->client_addr;
    msgs.flags = RT_I2C_WR;
    msgs.buf   = data;
    msgs.len   = write_len + GTP_ADDR_LENGTH;

    if (rt_i2c_transfer(dev->bus, &msgs, 1) == 1)
    {
        rt_free(data);
        return RT_EOK;
    }
    else
    {
        rt_free(data);
        return -RT_ERROR;
    }
}

static rt_err_t gt1x_read_regs(struct rt_i2c_client *dev, rt_uint16_t cmd, rt_uint8_t read_len, rt_uint8_t *read_buf)
{
    struct rt_i2c_msg msgs[2];
    rt_uint8_t cmd_buf[2];

    cmd_buf[0] = (rt_uint8_t)(cmd >> 8);
    cmd_buf[1] = (rt_uint8_t)(cmd & 0xff);

    msgs[0].addr  = dev->client_addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = cmd_buf;
    msgs[0].len   = 2;

    msgs[1].addr  = dev->client_addr;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf   = read_buf;
    msgs[1].len   = read_len;

    if (rt_i2c_transfer(dev->bus, msgs, 2) == 2)
    {
        return RT_EOK;
    }
    return -RT_ERROR;
}

static rt_err_t gt1x_control(struct rt_touch_device *device, int cmd, void *data)
{
    rt_err_t ret;

    if (cmd == RT_TOUCH_CTRL_GET_ID)
    {
        rt_uint8_t *id = (rt_uint8_t *)data;
        id[0] = 'g';
        id[1] = 't';
        ret = gt1x_read_regs(gt1x_client, GTP_REG_VERSION, 6, &id[2]);
        RT_ASSERT(ret == RT_EOK);
    }
    else if (cmd == RT_TOUCH_CTRL_GET_INFO)
    {
        struct rt_touch_info *info = (struct rt_touch_info *)data;
        rt_uint8_t opr_buf[7] = {0};
        ret = gt1x_read_regs(gt1x_client, GTP_REG_CONFIG_DATA, 7, opr_buf);
        RT_ASSERT(ret == RT_EOK);
        info->range_x = (opr_buf[2] << 8) + opr_buf[1];
        info->range_y = (opr_buf[4] << 8) + opr_buf[3];
        info->point_num = opr_buf[5] & 0x0f;
    }

    return RT_EOK;
}

static int16_t pre_x[GTP_MAX_TOUCH] = {-1, -1, -1, -1, -1};
static int16_t pre_y[GTP_MAX_TOUCH] = {-1, -1, -1, -1, -1};
static int16_t pre_w[GTP_MAX_TOUCH] = {-1, -1, -1, -1, -1};
static rt_uint8_t s_tp_dowm[GTP_MAX_TOUCH];
static struct rt_touch_data *read_data;

static void gt1x_touch_up(void *buf, int8_t id)
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

static void gt1x_touch_down(void *buf, int8_t id, int16_t x, int16_t y, int16_t w)
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

static rt_size_t gt1x_read_point(struct rt_touch_device *touch, void *buf, rt_size_t read_num)
{
    rt_uint8_t point_status = 0;
    rt_uint8_t touch_num = 0;
    rt_uint8_t write_buf[3];
    rt_uint8_t read_buf[8 * GTP_MAX_TOUCH] = {0};
    rt_uint8_t read_index = 0;
    int8_t read_id = 0;
    int16_t input_x = 0;
    int16_t input_y = 0;
    int16_t input_w = 0;

    static rt_uint8_t pre_touch = 0;
    static int8_t pre_id[GTP_MAX_TOUCH] = {0};

    rt_device_control(&touch->parent, RT_TOUCH_CTRL_DISABLE_INT, RT_NULL);

    /* point status register */
    if (gt1x_read_regs(gt1x_client, GTP_READ_COOR_ADDR, 1, &point_status) != RT_EOK)
    {
        rt_kprintf("read point failed\n");
        goto exit_;
    }

    if (point_status == 0)             /* no data */
    {
        goto exit_;
    }

    if ((point_status & 0x80) == 0)    /* data is not ready */
    {
        goto exit_;
    }

    touch_num = point_status & 0x0f;  /* get point num */

    if (touch_num > GTP_MAX_TOUCH) /* point num is not correct */
    {
        goto exit_;
    }

    /* read point num is read_num */
    if (gt1x_read_regs(gt1x_client, GTP_POINT1_REG, read_num * GTP_POINT_INFO_NUM, read_buf) != RT_EOK)
    {
        rt_kprintf("read point failed\n");
        goto exit_;
    }

#ifdef GT1X_DEBUG
    rt_kprintf("sta = 0x%02x, id = 0x%02x, ", point_status, read_buf[0]);
    rt_kprintf("x = 0x%02x%02x, 0x%04x, %d,  ", read_buf[2], read_buf[1], (read_buf[2] << 8) + read_buf[1], (read_buf[2] << 8) + read_buf[1]);
    rt_kprintf("y = 0x%02x%02x, 0x%04x, %d\n", read_buf[4], read_buf[3], (read_buf[4] << 8) + read_buf[3], (read_buf[4] << 8) + read_buf[3]);
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
                    gt1x_touch_up(buf, up_id);
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
            input_y = read_buf[off_set + 3] | (read_buf[off_set + 4] << 8); /* y */
            input_w = read_buf[off_set + 5] | (read_buf[off_set + 6] << 8); /* size */

            gt1x_touch_down(buf, read_id, input_x, input_y, input_w);
        }
    }
    else if (pre_touch)
    {
        for (read_index = 0; read_index < pre_touch; read_index++)
        {
            gt1x_touch_up(buf, pre_id[read_index]);
        }
    }

    pre_touch = touch_num;

exit_:
    write_buf[0] = 0x00;
    gt1x_write_reg(gt1x_client, GTP_READ_COOR_ADDR, 1, write_buf);

    rt_device_control(&touch->parent, RT_TOUCH_CTRL_ENABLE_INT, RT_NULL);

    return read_index;
}

static struct rt_touch_ops touch_ops =
{
    .touch_readpoint = gt1x_read_point,
    .touch_control   = gt1x_control,
};

static int gt1x_set_reset_status(void)
{
    /* 0x8040 ~ 0x8043 */
    rt_uint8_t value[] = {0xAA, 0x00, 0x56, 0xAA};
    int ret;

    ret = gt1x_write_reg(gt1x_client, GTP_REG_CMD + 1, 3, &value[1]);
    if (ret < 0)
        return ret;

    return gt1x_write_reg(gt1x_client, GTP_REG_CMD, 1, value);
}

int gt1x_i2c_read_dbl_check(rt_uint16_t addr, rt_uint8_t *buffer, uint32_t len)
{
    rt_uint8_t buf[16] = { 0 };
    rt_uint8_t confirm_buf[16] = { 0 };
    int ret;

    if (len > 16)
    {
        rt_kprintf("i2c_read_dbl_check length %d is too long, exceed %zu\n", len, sizeof(buf));
        return -RT_ERROR;
    }

    memset(buf, 0xAA, sizeof(buf));
    ret = gt1x_read_regs(gt1x_client, addr, len, buf);
    if (ret < 0)
    {
        return ret;
    }

    rt_thread_mdelay(10);
    memset(confirm_buf, 0, sizeof(confirm_buf));
    ret = gt1x_read_regs(gt1x_client, addr, len, confirm_buf);
    if (ret < 0)
    {
        return ret;
    }

    if (!memcmp(buf, confirm_buf, len))
    {
        memcpy(buffer, confirm_buf, len);
        return 0;
    }
    rt_kprintf("i2c read 0x%04X, %d bytes, double check failed!\n", addr, len);
    return -RT_ERROR;
}

int gt1x_get_chip_type(void)
{
    rt_uint8_t opr_buf[4] = { 0x00 };
    rt_uint8_t gt1x_data[] = { 0x02, 0x08, 0x90, 0x00 };
    rt_uint8_t gt9l_data[] = { 0x03, 0x10, 0x90, 0x00 };
    int ret = -1;

    /* chip type already exist */
    if (gt1x_chip_type != CHIP_TYPE_NONE)
    {
        return 0;
    }

    /* read hardware */
    ret = gt1x_i2c_read_dbl_check(GTP_REG_HW_INFO, opr_buf, sizeof(opr_buf));
    if (ret)
    {
        rt_kprintf("I2c communication error.\n");
        return -1;
    }

    /* find chip type */
    if (!memcmp(opr_buf, gt1x_data, sizeof(gt1x_data)))
    {
        gt1x_chip_type = CHIP_TYPE_GT1X;
    }
    else if (!memcmp(opr_buf, gt9l_data, sizeof(gt9l_data)))
    {
        gt1x_chip_type = CHIP_TYPE_GT2X;
    }

    if (gt1x_chip_type != CHIP_TYPE_NONE)
    {
        rt_kprintf("Chip Type: %s\n", (gt1x_chip_type == CHIP_TYPE_GT1X) ? "GT1X" : "GT2X");
        return 0;
    }
    else
    {
        return -1;
    }
}

void gt1x_select_addr(void)
{
    rt_pin_mode(TOUCH_RST_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(TOUCH_RST_PIN, 0);
    rt_pin_mode(TOUCH_IRQ_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(TOUCH_IRQ_PIN, gt1x_client->client_addr == 0x14);
    rt_thread_mdelay(10);
    rt_pin_write(TOUCH_RST_PIN, 1);
    rt_thread_mdelay(10);
}

int gt1x_reset_guitar(void)
{
    int ret;

    gt1x_select_addr();
    rt_thread_mdelay(10);

    /* int synchronization */
    rt_pin_write(TOUCH_IRQ_PIN, 0);
    rt_thread_mdelay(50);
    rt_pin_mode(TOUCH_IRQ_PIN, PIN_MODE_INPUT_PULLDOWN);

    /* this operation is necessary even when the esd check
       fucntion dose not turn on */
    ret = gt1x_set_reset_status();
    return ret;
}

int gt1x_read_version(struct gt1x_version_info *ver_info)
{
    int ret = -1;
    uint8_t buf[12] = { 0 };
    uint32_t mask_id = 0;
    uint32_t patch_id = 0;
    uint8_t product_id[5] = { 0 };
    uint8_t sensor_id = 0;
    uint8_t match_opt = 0;
    unsigned int i, retry = 3;
    uint8_t checksum = 0;

    while (retry--)
    {
        ret = gt1x_i2c_read_dbl_check(GTP_REG_VERSION, buf, sizeof(buf));
        if (!ret)
        {
            checksum = 0;

            for (i = 0; i < sizeof(buf); i++)
            {
                checksum += buf[i];
            }

            if (checksum == 0 &&    /* first 3 bytes must be number or char */
                    IS_NUM_OR_CHAR(buf[0]) && IS_NUM_OR_CHAR(buf[1]) && IS_NUM_OR_CHAR(buf[2]) && buf[10] != 0xFF)      /*sensor id == 0xFF, retry */
            {
                break;
            }
            else
            {
                rt_kprintf("Read version failed!(checksum error)\n");
            }
        }
        else
        {
            rt_kprintf("Read version failed!\n");
        }
        rt_kprintf("Read version : %d\n", retry);
        rt_thread_mdelay(100);
    }

    if (retry <= 0)
    {
        if (ver_info)
            ver_info->sensor_id = 0;
        return -1;
    }

    mask_id = (uint32_t)((buf[7] << 16) | (buf[8] << 8) | buf[9]);
    patch_id = (uint32_t)((buf[4] << 16) | (buf[5] << 8) | buf[6]);
    memcpy(product_id, buf, 4);
    sensor_id = buf[10] & 0x0F;
    match_opt = (buf[10] >> 4) & 0x0F;

#ifdef GT1X_DEBUG
    rt_kprintf("IC VERSION:GT%s_%06X(Patch)_%04X(Mask)_%02X(SensorID)\n", product_id, patch_id, mask_id >> 8, sensor_id);
#endif

    if (ver_info != NULL)
    {
        ver_info->mask_id = mask_id;
        ver_info->patch_id = patch_id;
        memcpy(ver_info->product_id, product_id, 5);
        ver_info->sensor_id = sensor_id;
        ver_info->match_opt = match_opt;
    }
    return 0;
}

int gt1x_send_cfg(uint8_t *config, int cfg_len)
{
    int i;
    int ret = 0;
    int retry = 0;
    uint16_t checksum = 0;

#ifdef GT1X_DEBUG
    rt_kprintf("Driver send config, length:%d\n", cfg_len);
#endif
    for (i = 0; i < cfg_len - 3; i += 2)
    {
        checksum += (config[i] << 8) + config[i + 1];
    }
    if (!checksum)
    {
        rt_kprintf("Invalid config, all of the bytes is zero!\n");
        return -1;
    }
    checksum = 0 - checksum;
#ifdef GT1X_DEBUG
    rt_kprintf("Config checksum: 0x%04X\n", checksum);
#endif
    config[cfg_len - 3] = (checksum >> 8) & 0xFF;
    config[cfg_len - 2] = checksum & 0xFF;
    config[cfg_len - 1] = 0x01;

    while (retry++ < 5)
    {
        ret = gt1x_write_reg(gt1x_client, GTP_REG_CONFIG_DATA, cfg_len, config);
        if (!ret)
        {
            rt_thread_mdelay(200);  /* at least 200ms, wait for storing config into flash. */
#ifdef GT1X_DEBUG
            rt_kprintf("Send config successfully!\n");
#endif
            return 0;
        }
    }
    rt_kprintf("Send config failed!\n");
    return ret;
}

const uint8_t cfg_grp0[] = GTP_CFG_GROUP0;
const uint8_t cfg_grp1[] = GTP_CFG_GROUP1;
const uint8_t cfg_grp2[] = GTP_CFG_GROUP2;
const uint8_t cfg_grp3[] = GTP_CFG_GROUP3;
const uint8_t cfg_grp4[] = GTP_CFG_GROUP4;
const uint8_t cfg_grp5[] = GTP_CFG_GROUP5;

int gt1x_init_panel(void)
{
    int ret = 0;
    uint8_t cfg_len = 0;
    uint8_t sensor_id = 0;

    const uint8_t *cfgs[] =
    {
        cfg_grp0, cfg_grp1, cfg_grp2,
        cfg_grp3, cfg_grp4, cfg_grp5
    };

    uint8_t cfg_lens[] =
    {
        CFG_GROUP_LEN(cfg_grp0),
        CFG_GROUP_LEN(cfg_grp1),
        CFG_GROUP_LEN(cfg_grp2),
        CFG_GROUP_LEN(cfg_grp3),
        CFG_GROUP_LEN(cfg_grp4),
        CFG_GROUP_LEN(cfg_grp5)
    };

#ifdef GT1X_DEBUG
    rt_kprintf("Config groups length:%d,%d,%d,%d,%d,%d\n", cfg_lens[0], cfg_lens[1], cfg_lens[2], cfg_lens[3], cfg_lens[4], cfg_lens[5]);
#endif

    sensor_id = gt1x_version.sensor_id;
    if (sensor_id >= 6 || cfg_lens[sensor_id] < GTP_CONFIG_MIN_LENGTH || cfg_lens[sensor_id] > GTP_CONFIG_MAX_LENGTH)
    {
        sensor_id = 0;
        gt1x_version.sensor_id = 0;
    }

    cfg_len = cfg_lens[sensor_id];

#ifdef GT1X_DEBUG
    rt_kprintf("Config group%d used, length:%d\n", sensor_id, cfg_len);
#endif

    if (cfg_len < GTP_CONFIG_MIN_LENGTH || cfg_len > GTP_CONFIG_MAX_LENGTH)
    {
        rt_kprintf("Config group%d is INVALID! You need to check you header file CFG_GROUP section!\n", sensor_id + 1);
        return -1;
    }

    memset(gt1x_config, 0, sizeof(gt1x_config));
    memcpy(gt1x_config, cfgs[sensor_id], cfg_len);

    /* clear the flag, avoid failure when send the_config of driver. */
    gt1x_config[0] &= 0x7F;

#if GTP_CUSTOM_CFG
    gt1x_config[RESOLUTION_LOC] = (uint8_t) GTP_MAX_WIDTH;
    gt1x_config[RESOLUTION_LOC + 1] = (uint8_t)(GTP_MAX_WIDTH >> 8);
    gt1x_config[RESOLUTION_LOC + 2] = (uint8_t) GTP_MAX_HEIGHT;
    gt1x_config[RESOLUTION_LOC + 3] = (uint8_t)(GTP_MAX_HEIGHT >> 8);

    if (GTP_INT_TRIGGER == 0)   /* RISING  */
    {
        gt1x_config[TRIGGER_LOC] &= 0xfe;
    }
    else if (GTP_INT_TRIGGER == 1)      /* FALLING */
    {
        gt1x_config[TRIGGER_LOC] |= 0x01;
    }
    set_reg_bit(gt1x_config[MODULE_SWITCH3_LOC], 5, !gt1x_wakeup_level);
#endif /* END GTP_CUSTOM_CFG */

    /* match resolution when gt1x_abs_x_max & gt1x_abs_y_max have been set already */
    if ((gt1x_abs_x_max == 0) && (gt1x_abs_y_max == 0))
    {
        gt1x_abs_x_max = (gt1x_config[RESOLUTION_LOC + 1] << 8) + gt1x_config[RESOLUTION_LOC];
        gt1x_abs_y_max = (gt1x_config[RESOLUTION_LOC + 3] << 8) + gt1x_config[RESOLUTION_LOC + 2];
        gt1x_int_type = (gt1x_config[TRIGGER_LOC]) & 0x03;
        gt1x_wakeup_level = !(gt1x_config[MODULE_SWITCH3_LOC] & 0x20);
    }
    else
    {
        gt1x_config[RESOLUTION_LOC] = (uint8_t) gt1x_abs_x_max;
        gt1x_config[RESOLUTION_LOC + 1] = (uint8_t)(gt1x_abs_x_max >> 8);
        gt1x_config[RESOLUTION_LOC + 2] = (uint8_t) gt1x_abs_y_max;
        gt1x_config[RESOLUTION_LOC + 3] = (uint8_t)(gt1x_abs_y_max >> 8);
        set_reg_bit(gt1x_config[MODULE_SWITCH3_LOC], 5, !gt1x_wakeup_level);
        gt1x_config[TRIGGER_LOC] = (gt1x_config[TRIGGER_LOC] & 0xFC) | gt1x_int_type;
    }

#ifdef GT1X_DEBUG
    rt_kprintf("X_MAX=%d,Y_MAX=%d,TRIGGER=0x%02x,WAKEUP_LEVEL=%d\n", gt1x_abs_x_max, gt1x_abs_y_max, gt1x_int_type, gt1x_wakeup_level);
#endif

    gt1x_cfg_length = cfg_len;
    ret = gt1x_send_cfg(gt1x_config, gt1x_cfg_length);
    return ret;
}

int rt_hw_gt1x_init(void)
{
    rt_err_t ret = -1;
    int retry = 0;
    uint8_t reg_val[1];

    struct rt_touch_config cfg;
    const char *touch_name = TOUCH_DEV_NAME;

    RT_ASSERT(TOUCH_RST_PIN != 0);
    RT_ASSERT(TOUCH_IRQ_PIN != 0);
    RT_ASSERT(TOUCH_I2C_DEV != 0);

    /* i2c interface bus */
    gt1x_client = (struct rt_i2c_client *)rt_calloc(1, sizeof(struct rt_i2c_client));
    gt1x_client->client_addr = GTP_ADDRESS_LOW;
    gt1x_client->bus = (struct rt_i2c_bus_device *)rt_device_find(TOUCH_I2C_DEV);
    RT_ASSERT(gt1x_client->bus != RT_NULL);

    ret = rt_device_open((rt_device_t)gt1x_client->bus, RT_DEVICE_FLAG_RDWR);
    RT_ASSERT(ret == RT_EOK);

    /* reset touch */
    while (retry++ < 5)
    {
        gt1x_init_failed = 0;
        /* reset ic */
        ret = gt1x_reset_guitar();
        if (ret != 0)
        {
            rt_kprintf("Reset guitar failed!\n");
            continue;
        }

        /* check main system firmware */
        ret = gt1x_i2c_read_dbl_check(GTP_REG_FW_CHK_MAINSYS, reg_val, 1);
        if (ret != 0)
        {
            continue;
        }
        else if (reg_val[0] != 0xBE)
        {
            rt_kprintf("Check main system not pass[0x%2X].\n", reg_val[0]);
            gt1x_init_failed = 1;
        }

        /* debug info  */
        ret = gt1x_i2c_read_dbl_check(GTP_REG_FW_CHK_SUBSYS, reg_val, 1);
        if (!ret && reg_val[0] == 0xAA)
        {
            rt_kprintf("Check subsystem not pass[0x%2X].\n", reg_val[0]);
        }
        break;
    }

    /* if the initialization fails, set default setting */
    ret |= gt1x_init_failed;
    if (ret)
    {
        rt_kprintf("Init failed, use default setting\n");
        gt1x_abs_x_max = GTP_MAX_WIDTH;
        gt1x_abs_y_max = GTP_MAX_HEIGHT;
        gt1x_int_type = GTP_INT_TRIGGER;
        gt1x_wakeup_level = GTP_WAKEUP_LEVEL;
    }

    /* read version information */
    ret = gt1x_read_version(&gt1x_version);
    RT_ASSERT(ret == RT_EOK);

    /* init and send configs */
    ret = gt1x_init_panel();
    RT_ASSERT(ret == RT_EOK);

    /* register touch device */
    touch_device = (rt_touch_t)rt_calloc(1, sizeof(struct rt_touch_device));
    RT_ASSERT(touch_device != RT_NULL);

    cfg.irq_pin.pin  = TOUCH_IRQ_PIN;
    cfg.irq_pin.mode = PIN_MODE_INPUT_PULLDOWN;
    cfg.dev_name = TOUCH_I2C_DEV;
    rt_memcpy(&touch_device->config, &cfg, sizeof(struct rt_touch_config));

    touch_device->info.type = RT_TOUCH_TYPE_CAPACITANCE;
    touch_device->info.vendor = RT_TOUCH_VENDOR_GT;
    touch_device->ops = &touch_ops;
    rt_hw_touch_register(touch_device, touch_name, RT_DEVICE_FLAG_INT_RX, RT_NULL);

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_gt1x_init);
