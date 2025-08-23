/**
  * Copyright (c) 2022 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_sc230ai.c
  * @version V0.0.1
  *
  * Change Logs:
  * 2022-07-07     ISP Team      first implementation
  *
  ******************************************************************************
  */

#include "camera.h"
#include <rthw.h>
#include <rtdevice.h>
#include <rtthread.h>
#include "hal_base.h"
#include <rtconfig.h>
#include "board.h"
#include "drv_clock.h"
#include "board.h"

#ifdef RT_USING_SC230AI
#define SC230AI_DEBUG_INFO      0

#if SC230AI_DEBUG_INFO
#include <stdio.h>
#define SC230AI_DEBUG(...)               rk_kprintf("[SC230AI]:");rk_kprintf(__VA_ARGS__)
#else
#define SC230AI_DEBUG(...)
#endif

#define SC230AI_INFO(dev,arg...)         rk_kprintf("[%s]:",dev->name);rk_kprintf(arg)

#define RT_USING_SC230AI_OPS 0

#define SC230AI_DEVICE_NAME          "sensor_0"
#define I2C_BUS_NAME                "i2c4"
#define SC230AI_7BIT_ADDRESS         (0x30)
#define SC230AI_REG_ID_H_ADDRESS     (0x3107)
#define SC230AI_REG_ID_L_ADDRESS     (0x3108)
#define SC230AI_SENSOR_ID            (0xcb34)
#define SC230AI_REG_EXP_LONG_H           0x3e00
#define SC230AI_REG_EXP_LONG_M           0x3e01
#define SC230AI_REG_EXP_LONG_L           0x3e02
#define SC230AI_REG_EXP_SHORT_H          0x3e22
#define SC230AI_REG_EXP_SHORT_M          0x3e04
#define SC230AI_REG_EXP_SHORT_L          0x3e05
#define SC230AI_REG_DIG_GAIN            0x3e06
#define SC230AI_REG_DIG_FINE_GAIN       0x3e07
#define SC230AI_REG_ANA_GAIN            0x3e09
#define SC230AI_REG_SDIG_GAIN           0x3e10
#define SC230AI_REG_SDIG_FINE_GAIN      0x3e11
#define SC230AI_REG_SANA_GAIN           0x3e12
#define SC230AI_REG_SANA_FINE_GAIN      0x3e13
#define SC230AI_GAIN_MIN                1000
#define SC230AI_GAIN_MAX                1722628       //108.512*15.875*1000
#define SC230AI_GAIN_STEP               1
#define SC230AI_GAIN_DEFAULT            1000
#define SC230AI_LGAIN                   0
#define SC230AI_SGAIN                   1

#define REG_END                     (0x0)
#define REG_DELAY                   (0xff)

#define SENSOR_ID(_msb, _lsb)       ((_msb) << 8 | (_lsb))
#define ARRAY_SIZE(x)               (sizeof(x) / sizeof((x)[0]))

#define SC230AI_PIN_CTRL_ENABLE          1
#define SC230AI_I2C_DEBUG_ENABLE         0
#define SC230AI_TESTPATTERN_ENABLE       0

/* Compiler Related Definitions */
#define rk_inline                               rt_inline

/* redefine macro function */
#define RK_ALIGN(size, align)                   RT_ALIGN(size, align)
#define rk_container_of(ptr, type, member)      rt_container_of(ptr, type, member)

/* redefine system variables */
typedef rt_mutex_t                              rk_mutex_t;
typedef rt_sem_t                                rk_semaphore_t;
typedef struct rt_device                        rk_device;
typedef struct rt_i2c_bus_device                rk_i2c_bus_device;
typedef struct clk_gate                         rk_clk_gate;

/* define private system variables */
typedef rt_list_t                               rk_list_node;
typedef rt_list_t                               rk_queue_list;

#define rk_list_first_entry(ptr, type, member)              rt_list_first_entry(ptr, type, member)
#define rk_list_for_each_entry(pos, head, member)           rt_list_for_each_entry(pos, head, member)
#define rk_kprintf                                          rt_kprintf

/* redefine the basic data type */
typedef rt_err_t                        ret_err_t;
typedef rt_size_t                       ret_size_t;
typedef rt_base_t                       dt_base_t;
typedef rt_ubase_t                      dt_ubase_t;
typedef rt_tick_t                       rk_tick_t;
typedef int                             dt_cmd_t;

#define RK_NULL                          RT_NULL
#define MACRO_ASSERT(EX)                 RT_ASSERT(EX)

/* redefine system err code */
#define RET_SYS_EOK                     (RT_EOK)
#define RET_SYS_ERROR                   (-RT_ERROR)
#define RET_SYS_ETIMEOUT                (-RT_ETIMEOUT)  /**< Timed out */
#define RET_SYS_EFULL                   (-RT_EFULL)     /**< The resource is full */
#define RET_SYS_EEMPTY                  (-RT_EEMPTY)    /**< The resource is empty */
#define RET_SYS_ENOMEM                  (-RT_ENOMEM)    /**< No memory */
#define RET_SYS_ENOSYS                  (-RT_ENOSYS)    /**< No system */
#define RET_SYS_EBUSY                   (-RT_EBUSY)     /**< Busy */
#define RET_SYS_EIO                     (-RT_EIO)       /**< IO error */
#define RET_SYS_EINTR                   (-RT_EINTR)     /**< Interrupted system call */
#define RET_SYS_EINVAL                  (-RT_EINVAL)    /**< Invalid argument */

#if 0
struct mclk
{
    rk_clk_gate *gate;
    eCLOCK_Name clk_name;
};
#endif

#define I2C_WRITE_CONTINUE

struct SC230AI_sensor_reg
{
    uint16_t reg_addr;
    uint8_t val;
};

struct v4l2_fract
{
    uint32_t numerator;
    uint32_t denominator;
};

struct sc230ai_mode
{
    uint32_t bus_fmt;
    uint32_t width;
    uint32_t height;
    struct v4l2_fract max_fps;
    uint32_t hts_def;
    uint32_t vts_def;
    uint32_t exp_def;
    /* const struct sensor_reg *reg_list; */
#ifdef I2C_WRITE_CONTINUE
    const uint8_t *reg_list;
#else
    const struct SC230AI_sensor_reg *reg_list;
#endif
    const int reg_list_size;
    uint32_t hdr_mode;
};

#ifdef I2C_WRITE_CONTINUE

static const uint8_t g_SC230AI_init_reg_table[] =
{
    0x3, 0x01, 0x03, 0x01,
    0x3, 0x36, 0xe9, 0x80,
    0x3, 0x37, 0xf9, 0x80,
    0x3, 0x30, 0x1f, 0x2d,
    0x3, 0x32, 0x03, 0x3c,
    0x7, 0x32, 0x07, 0x03, 0x02, 0x80, 0x01, 0xe0,
    0x8, 0x32, 0x0e, 0x02, 0x32, 0x00, 0xa2, 0x00, 0x02,
    0x3, 0x32, 0x15, 0x31,
    0x3, 0x32, 0x20, 0x01,
    0x3, 0x33, 0x01, 0x09,
    0x3, 0x33, 0x06, 0x48,
    0x6, 0x33, 0x08, 0x18, 0x68, 0x00, 0xc0,
    0x3, 0x33, 0x1f, 0x59,
    0x4, 0x33, 0x33, 0x10, 0x40,
    0x5, 0x33, 0x5d, 0x60, 0x06, 0x08,
    0x3, 0x33, 0x64, 0x5e,
    0x4, 0x33, 0x7c, 0x02, 0x0a,
    0xf, 0x33, 0x90, 0x01, 0x0b, 0x0f, 0x0c, 0x0d, 0x60, 0x48, 0x49, 0x4f, 0x0a, 0x0f, 0x14, 0x60,
    0x3, 0x33, 0xa2, 0x04,
    0x3, 0x33, 0xaf, 0x40,
    0x3, 0x33, 0xb1, 0x80,
    0x3, 0x33, 0xb9, 0x0a,
    0x3, 0x33, 0xf9, 0x70,
    0x5, 0x33, 0xfb, 0x90, 0x4b, 0x5f,
    0x3, 0x34, 0x9f, 0x03,
    0x4, 0x34, 0xa6, 0x4b, 0x4f,
    0x5, 0x34, 0xab, 0xe0, 0x01, 0x00,
    0x4, 0x34, 0xf8, 0x5f, 0x10,
    0x3, 0x36, 0x30, 0xc0,
    0x3, 0x36, 0x37, 0x29,
    0x3, 0x36, 0x3b, 0x20,
    0x3, 0x36, 0x70, 0x09,
    0x5, 0x36, 0x74, 0xb0, 0x80, 0x88,
    0x4, 0x36, 0x7c, 0x40, 0x49,
    0x5, 0x36, 0x90, 0x44, 0x44, 0x54,
    0x4, 0x36, 0x9c, 0x49, 0x4f,
    0x7, 0x36, 0xae, 0x4b, 0x4f, 0x87, 0x9b, 0xb7,
    0x3, 0x36, 0xd0, 0x01,
    0x3, 0x36, 0xeb, 0x04,
    0x3, 0x37, 0x0f, 0x01,
    0x3, 0x37, 0x22, 0x17,
    0x3, 0x37, 0x28, 0x90,
    0x7, 0x37, 0xb0, 0x17, 0x17, 0x97, 0x4b, 0x4f,
    0x5, 0x37, 0xfb, 0x24, 0x10, 0x22,
    0x3, 0x39, 0x01, 0x02,
    0x3, 0x39, 0x04, 0x04,
    0x3, 0x39, 0x1f, 0x04,
    0x4, 0x39, 0x33, 0x84, 0x02,
    0x6, 0x39, 0x40, 0x62, 0x00, 0x04, 0x03,
    0x4, 0x3e, 0x01, 0x45, 0xb0,
    0x3, 0x44, 0x0e, 0x02,
    0x3, 0x45, 0x0d, 0x11,
    0x3, 0x48, 0x19, 0x05,
    0x3, 0x48, 0x1b, 0x03,
    0x3, 0x48, 0x1d, 0x0a,
    0x3, 0x48, 0x1f, 0x02,
    0x3, 0x48, 0x21, 0x08,
    0x3, 0x48, 0x23, 0x03,
    0x3, 0x48, 0x25, 0x02,
    0x3, 0x48, 0x29, 0x04,
    0x3, 0x50, 0x00, 0x46,
    0x3, 0x50, 0x10, 0x01,
    0x4, 0x57, 0x8a, 0x10, 0x08,
    0x7, 0x57, 0x91, 0x04, 0x00, 0x10, 0x08, 0x00,
    0x3, 0x57, 0x99, 0x06,
    0x3, 0x57, 0xad, 0x00,
    0x4, 0x59, 0x00, 0xf1, 0x04,
    0x10, 0x5a, 0xe0, 0xfe, 0x40, 0x3f, 0x38, 0x28, 0x3f, 0x38, 0x28, 0x3f, 0x3c, 0x2c, 0x3f, 0x3c, 0x2c,
    0xe, 0x5a, 0xf4, 0x3f, 0x38, 0x28, 0x3f, 0x38, 0x28, 0x3f, 0x3c, 0x2c, 0x3f, 0x3c, 0x2c,
    0x3, 0x36, 0xe9, 0x20,
    0x3, 0x37, 0xf9, 0x24,
    0x47,

};

static const uint8_t g_SC230AI_1080p_reg_table[] =
{
    0x3, 0x01, 0x03, 0x01,
    0x3, 0x01, 0x00, 0x00,
    0x3, 0x36, 0xe9, 0x80,
    0x3, 0x37, 0xf9, 0x80,
    0x3, 0x30, 0x1f, 0x01,
    0x4, 0x32, 0x0e, 0x05, 0x46,
    0x3, 0x32, 0x21, 0x66,
    0x3, 0x33, 0x01, 0x07,
    0x3, 0x33, 0x04, 0x50,
    0x3, 0x33, 0x06, 0x70,
    0x6, 0x33, 0x08, 0x18, 0x68, 0x01, 0x20,
    0x3, 0x33, 0x14, 0x15,
    0x4, 0x33, 0x1e, 0x41, 0x59,
    0x4, 0x33, 0x33, 0x10, 0x40,
    0x5, 0x33, 0x5d, 0x60, 0x06, 0x08,
    0x3, 0x33, 0x64, 0x5e,
    0x4, 0x33, 0x7c, 0x02, 0x0a,
    0xf, 0x33, 0x90, 0x01, 0x0b, 0x0f, 0x09, 0x0d, 0x60, 0x48, 0x49, 0x4b, 0x06, 0x0a, 0x0d, 0x60,
    0x3, 0x33, 0xa2, 0x04,
    0x3, 0x33, 0xad, 0x2c,
    0x3, 0x33, 0xaf, 0x40,
    0x3, 0x33, 0xb1, 0x80,
    0x3, 0x33, 0xb3, 0x40,
    0x3, 0x33, 0xb9, 0x0a,
    0x3, 0x33, 0xf9, 0x78,
    0x5, 0x33, 0xfb, 0xa0, 0x4f, 0x5f,
    0x3, 0x34, 0x9f, 0x03,
    0xa, 0x34, 0xa6, 0x4b, 0x5f, 0x30, 0x20, 0x01, 0x28, 0x01, 0x58,
    0x4, 0x34, 0xf8, 0x7f, 0x10,
    0x3, 0x36, 0x30, 0xc0,
    0x4, 0x36, 0x32, 0x54, 0x44,
    0x4, 0x36, 0x3b, 0x20, 0x08,
    0x3, 0x36, 0x70, 0x09,
    0x5, 0x36, 0x74, 0xb0, 0x80, 0x88,
    0x4, 0x36, 0x7c, 0x40, 0x49,
    0x5, 0x36, 0x90, 0x33, 0x33, 0x43,
    0x4, 0x36, 0x9c, 0x49, 0x4f,
    0x7, 0x36, 0xae, 0x4b, 0x4f, 0x87, 0x9b, 0xb7,
    0x3, 0x36, 0xd0, 0x01,
    0x3, 0x37, 0x22, 0x97,
    0x3, 0x37, 0x24, 0x22,
    0x3, 0x37, 0x28, 0x90,
    0x4, 0x39, 0x01, 0x02, 0xc5,
    0x3, 0x39, 0x04, 0x04,
    0x6, 0x39, 0x07, 0x00, 0x41, 0x00, 0x00,
    0x4, 0x39, 0x33, 0x84, 0x0a,
    0x6, 0x39, 0x40, 0x64, 0x00, 0x04, 0x0b,
    0x5, 0x3e, 0x00, 0x00, 0x8c, 0x10,
    0x3, 0x44, 0x0e, 0x02,
    0x3, 0x45, 0x0d, 0x11,
    0x3, 0x50, 0x10, 0x01,
    0x8, 0x57, 0x87, 0x08, 0x03, 0x00, 0x10, 0x08, 0x00,
    0x8, 0x57, 0x90, 0x08, 0x04, 0x00, 0x10, 0x08, 0x00,
    0x3, 0x57, 0x99, 0x06,
    0x3, 0x57, 0xad, 0x00,
    0x10, 0x5a, 0xe0, 0xfe, 0x40, 0x3f, 0x38, 0x28, 0x3f, 0x38, 0x28, 0x3f, 0x3c, 0x2c, 0x3f, 0x3c, 0x2c,
    0xe, 0x5a, 0xf4, 0x3f, 0x38, 0x28, 0x3f, 0x38, 0x28, 0x3f, 0x3c, 0x2c, 0x3f, 0x3c, 0x2c,
    0x3, 0x36, 0xe9, 0x20,
    0x3, 0x37, 0xf9, 0x27,
    0x3b,
};

#else
static const struct SC230AI_sensor_reg g_SC230AI_init_reg_table[] =
{
    {0x0103, 0x01},
    {0x36e9, 0x80},
    {0x37f9, 0x80},
    {0x301f, 0x2d},
    {0x3203, 0x3c},
    {0x3207, 0x03},
    {0x3208, 0x02},
    {0x3209, 0x80},
    {0x320a, 0x01},
    {0x320b, 0xe0},
    {0x320e, 0x02},
    {0x320f, 0x32},
    {0x3210, 0x00},
    {0x3211, 0xa2},
    {0x3212, 0x00},
    {0x3213, 0x02},
    {0x3215, 0x31},
    {0x3220, 0x01},
    {0x3301, 0x09},
    {0x3306, 0x48},
    {0x3308, 0x18},
    {0x3309, 0x68},
    {0x330a, 0x00},
    {0x330b, 0xc0},
    {0x331f, 0x59},
    {0x3333, 0x10},
    {0x3334, 0x40},
    {0x335d, 0x60},
    {0x335e, 0x06},
    {0x335f, 0x08},
    {0x3364, 0x5e},
    {0x337c, 0x02},
    {0x337d, 0x0a},
    {0x3390, 0x01},
    {0x3391, 0x0b},
    {0x3392, 0x0f},
    {0x3393, 0x0c},
    {0x3394, 0x0d},
    {0x3395, 0x60},
    {0x3396, 0x48},
    {0x3397, 0x49},
    {0x3398, 0x4f},
    {0x3399, 0x0a},
    {0x339a, 0x0f},
    {0x339b, 0x14},
    {0x339c, 0x60},
    {0x33a2, 0x04},
    {0x33af, 0x40},
    {0x33b1, 0x80},
    {0x33b9, 0x0a},
    {0x33f9, 0x70},
    {0x33fb, 0x90},
    {0x33fc, 0x4b},
    {0x33fd, 0x5f},
    {0x349f, 0x03},
    {0x34a6, 0x4b},
    {0x34a7, 0x4f},
    {0x34ab, 0xe0},
    {0x34ac, 0x01},
    {0x34ad, 0x00},
    {0x34f8, 0x5f},
    {0x34f9, 0x10},
    {0x3630, 0xc0},
    {0x3637, 0x29},
    {0x363b, 0x20},
    {0x3670, 0x09},
    {0x3674, 0xb0},
    {0x3675, 0x80},
    {0x3676, 0x88},
    {0x367c, 0x40},
    {0x367d, 0x49},
    {0x3690, 0x44},
    {0x3691, 0x44},
    {0x3692, 0x54},
    {0x369c, 0x49},
    {0x369d, 0x4f},
    {0x36ae, 0x4b},
    {0x36af, 0x4f},
    {0x36b0, 0x87},
    {0x36b1, 0x9b},
    {0x36b2, 0xb7},
    {0x36d0, 0x01},
    {0x36eb, 0x04},
    {0x370f, 0x01},
    {0x3722, 0x17},
    {0x3728, 0x90},
    {0x37b0, 0x17},
    {0x37b1, 0x17},
    {0x37b2, 0x97},
    {0x37b3, 0x4b},
    {0x37b4, 0x4f},
    {0x37fb, 0x24},
    {0x37fc, 0x10},
    {0x37fd, 0x22},
    {0x3901, 0x02},
    {0x3904, 0x04},
    {0x391f, 0x04},
    {0x3933, 0x84},
    {0x3934, 0x02},
    {0x3940, 0x62},
    {0x3941, 0x00},
    {0x3942, 0x04},
    {0x3943, 0x03},
    {0x3e01, 0x45},
    {0x3e02, 0xb0},
    {0x440e, 0x02},
    {0x450d, 0x11},
    {0x4819, 0x05},
    {0x481b, 0x03},
    {0x481d, 0x0a},
    {0x481f, 0x02},
    {0x4821, 0x08},
    {0x4823, 0x03},
    {0x4825, 0x02},
    {0x4829, 0x04},
    {0x5000, 0x46},
    {0x5010, 0x01},
    {0x578a, 0x10},
    {0x578b, 0x08},
    {0x5791, 0x04},
    {0x5792, 0x00},
    {0x5793, 0x10},
    {0x5794, 0x08},
    {0x5795, 0x00},
    {0x5799, 0x06},
    {0x57ad, 0x00},
    {0x5900, 0xf1},
    {0x5901, 0x04},
    {0x5ae0, 0xfe},
    {0x5ae1, 0x40},
    {0x5ae2, 0x3f},
    {0x5ae3, 0x38},
    {0x5ae4, 0x28},
    {0x5ae5, 0x3f},
    {0x5ae6, 0x38},
    {0x5ae7, 0x28},
    {0x5ae8, 0x3f},
    {0x5ae9, 0x3c},
    {0x5aea, 0x2c},
    {0x5aeb, 0x3f},
    {0x5aec, 0x3c},
    {0x5aed, 0x2c},
    {0x5af4, 0x3f},
    {0x5af5, 0x38},
    {0x5af6, 0x28},
    {0x5af7, 0x3f},
    {0x5af8, 0x38},
    {0x5af9, 0x28},
    {0x5afa, 0x3f},
    {0x5afb, 0x3c},
    {0x5afc, 0x2c},
    {0x5afd, 0x3f},
    {0x5afe, 0x3c},
    {0x5aff, 0x2c},
    {0x36e9, 0x20},
    {0x37f9, 0x24},
    {REG_END, 0x00},

};

//25fps
static const struct SC230AI_sensor_reg g_SC230AI_1080p_reg_table[] =
{
    {0x0103, 0x01},
    {0x0100, 0x00},
    {0x36e9, 0x80},
    {0x37f9, 0x80},
    {0x301f, 0x01},
    {0x320e, 0x05},
    {0x320f, 0x46},
    {0x3221, 0x66},
    {0x3301, 0x07},
    {0x3304, 0x50},
    {0x3306, 0x70},
    {0x3308, 0x18},
    {0x3309, 0x68},
    {0x330a, 0x01},
    {0x330b, 0x20},
    {0x3314, 0x15},
    {0x331e, 0x41},
    {0x331f, 0x59},
    {0x3333, 0x10},
    {0x3334, 0x40},
    {0x335d, 0x60},
    {0x335e, 0x06},
    {0x335f, 0x08},
    {0x3364, 0x5e},
    {0x337c, 0x02},
    {0x337d, 0x0a},
    {0x3390, 0x01},
    {0x3391, 0x0b},
    {0x3392, 0x0f},
    {0x3393, 0x09},
    {0x3394, 0x0d},
    {0x3395, 0x60},
    {0x3396, 0x48},
    {0x3397, 0x49},
    {0x3398, 0x4b},
    {0x3399, 0x06},
    {0x339a, 0x0a},
    {0x339b, 0x0d},
    {0x339c, 0x60},
    {0x33a2, 0x04},
    {0x33ad, 0x2c},
    {0x33af, 0x40},
    {0x33b1, 0x80},
    {0x33b3, 0x40},
    {0x33b9, 0x0a},
    {0x33f9, 0x78},
    {0x33fb, 0xa0},
    {0x33fc, 0x4f},
    {0x33fd, 0x5f},
    {0x349f, 0x03},
    {0x34a6, 0x4b},
    {0x34a7, 0x5f},
    {0x34a8, 0x30},
    {0x34a9, 0x20},
    {0x34aa, 0x01},
    {0x34ab, 0x28},
    {0x34ac, 0x01},
    {0x34ad, 0x58},
    {0x34f8, 0x7f},
    {0x34f9, 0x10},
    {0x3630, 0xc0},
    {0x3632, 0x54},
    {0x3633, 0x44},
    {0x363b, 0x20},
    {0x363c, 0x08},
    {0x3670, 0x09},
    {0x3674, 0xb0},
    {0x3675, 0x80},
    {0x3676, 0x88},
    {0x367c, 0x40},
    {0x367d, 0x49},
    {0x3690, 0x33},
    {0x3691, 0x33},
    {0x3692, 0x43},
    {0x369c, 0x49},
    {0x369d, 0x4f},
    {0x36ae, 0x4b},
    {0x36af, 0x4f},
    {0x36b0, 0x87},
    {0x36b1, 0x9b},
    {0x36b2, 0xb7},
    {0x36d0, 0x01},
    {0x3722, 0x97},
    {0x3724, 0x22},
    {0x3728, 0x90},
    {0x3901, 0x02},
    {0x3902, 0xc5},
    {0x3904, 0x04},
    {0x3907, 0x00},
    {0x3908, 0x41},
    {0x3909, 0x00},
    {0x390a, 0x00},
    {0x3933, 0x84},
    {0x3934, 0x0a},
    {0x3940, 0x64},
    {0x3941, 0x00},
    {0x3942, 0x04},
    {0x3943, 0x0b},
    {0x3e00, 0x00},
    {0x3e01, 0x8c},
    {0x3e02, 0x10},
    {0x440e, 0x02},
    {0x450d, 0x11},
    {0x5010, 0x01},
    {0x5787, 0x08},
    {0x5788, 0x03},
    {0x5789, 0x00},
    {0x578a, 0x10},
    {0x578b, 0x08},
    {0x578c, 0x00},
    {0x5790, 0x08},
    {0x5791, 0x04},
    {0x5792, 0x00},
    {0x5793, 0x10},
    {0x5794, 0x08},
    {0x5795, 0x00},
    {0x5799, 0x06},
    {0x57ad, 0x00},
    {0x5ae0, 0xfe},
    {0x5ae1, 0x40},
    {0x5ae2, 0x3f},
    {0x5ae3, 0x38},
    {0x5ae4, 0x28},
    {0x5ae5, 0x3f},
    {0x5ae6, 0x38},
    {0x5ae7, 0x28},
    {0x5ae8, 0x3f},
    {0x5ae9, 0x3c},
    {0x5aea, 0x2c},
    {0x5aeb, 0x3f},
    {0x5aec, 0x3c},
    {0x5aed, 0x2c},
    {0x5af4, 0x3f},
    {0x5af5, 0x38},
    {0x5af6, 0x28},
    {0x5af7, 0x3f},
    {0x5af8, 0x38},
    {0x5af9, 0x28},
    {0x5afa, 0x3f},
    {0x5afb, 0x3c},
    {0x5afc, 0x2c},
    {0x5afd, 0x3f},
    {0x5afe, 0x3c},
    {0x5aff, 0x2c},
    {0x36e9, 0x20},
    {0x37f9, 0x27},
    {0x0100, 0x01},
    {REG_END, 0x00},
};
#endif

static const struct sc230ai_mode supported_modes[] =
{
    {
        .bus_fmt = MEDIA_BUS_FMT_SBGGR10_1X10,
        .width = 640,
        .height = 480,
        .max_fps = {
            .numerator = 10000,
            .denominator = 1200000,
        },
        .exp_def = 0x46,
        .hts_def = 0x898,
        .vts_def = 0x0232,
        .reg_list = g_SC230AI_init_reg_table,
        .reg_list_size = ARRAY_SIZE(g_SC230AI_init_reg_table),
        .hdr_mode = NO_HDR,
    },
    {
        .bus_fmt = MEDIA_BUS_FMT_SBGGR10_1X10,
        .width = 1920,
        .height = 1080,
        .max_fps = {
            .numerator = 10000,
            .denominator = 250000,
        },
        .exp_def = 0x46,
        .hts_def = 0x44C * 2,
        .vts_def = 0x0546,
        .reg_list = g_SC230AI_1080p_reg_table,
        .reg_list_size = ARRAY_SIZE(g_SC230AI_1080p_reg_table),
        .hdr_mode = NO_HDR,
    }
};

struct SC230AI_dev
{
    struct rk_camera_device parent;
    rk_device dev;
    char name[RK_CAMERA_DEVICE_NAME_SIZE];

#if RT_USING_SC230AI_OPS
    struct SC230AI_ops *ops;
#endif

    int32_t pin_rst;
    int32_t pin_pwdn;
    int32_t pin_clkout;
    char i2c_name[RK_CAMERA_I2C_NAME_SIZE];
    rk_i2c_bus_device *i2c_bus;
    struct rt_mutex mutex_lock;
    struct rk_camera_exp_val init_exp;
    const struct sc230ai_mode *cur_mode;
    const struct sc230ai_mode *dst_mode;
    bool has_init_exp;
    bool streaming;
};
typedef struct SC230AI_dev *rt_SC230AI_dev_t;

struct SC230AI_ops
{
    ret_err_t (*init)(struct rk_camera_device *dev);
    ret_err_t (*open)(struct rk_camera_device *dev, uint16_t oflag);
    ret_err_t (*close)(struct rk_camera_device *dev);
    ret_err_t (*control)(struct rk_camera_device *dev, int cmd, void *arg);
};

const unsigned char sc230ai_camcfg[] =
{
    0x2C, 0x01, 0x00, 0x00, 0x32, 0x02, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x98, 0x08, 0x00, 0x00,
    0x46, 0x05, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x32, 0x00,
    0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0xC4, 0x00, 0x00, 0x00, 0xC4, 0x00, 0x00, 0x00,
    0xC4, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0xC4, 0x00, 0x00, 0x00, 0x0C, 0x02, 0x00, 0x00,
    0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
    0xCA, 0x00, 0x00, 0x00, 0x1C, 0x02, 0x00, 0x00, 0x1C, 0x02, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00,
    0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0xCA, 0x00, 0x00, 0x00, 0x1C, 0x02, 0x00, 0x00,
    0x1C, 0x02, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x66, 0x06, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0xF8, 0x7F, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC4, 0x00, 0x00, 0x00, 0xC4, 0x00, 0x00, 0x00, 0x89, 0x01, 0x00, 0x00, 0x89, 0x01, 0x00, 0x00,
    0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x66, 0x06, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xB7, 0x02, 0x00, 0x00, 0x7D, 0x01, 0x00, 0x00, 0x87, 0x03, 0x00, 0x00, 0xF3, 0x02,
    0x47, 0xA1, 0x34, 0x00, 0x7A, 0x54, 0xAA, 0x02,
};

static struct SC230AI_dev g_SC230AI;

static ret_err_t SC230AI_write_reg(rk_i2c_bus_device *bus,
                                   uint16_t reg, uint8_t data)
{
    uint8_t send_buf[3];
    struct rt_i2c_msg msgs;
    int ret = 0;

    MACRO_ASSERT(bus != RK_NULL);

    send_buf[0] = ((reg >> 8) & 0xff);
    send_buf[1] = ((reg >> 0) & 0xff);
    send_buf[2] = data;

    msgs.addr = SC230AI_7BIT_ADDRESS;
    msgs.flags = RT_I2C_WR;
    msgs.buf = send_buf;
    msgs.len = 3;
    ret = rt_i2c_transfer(bus, &msgs, 1);

    if (ret == 1)
    {
        SC230AI_DEBUG("(%s):s0.0 i2c_bus ok\n");
        return RET_SYS_EOK;
    }
    else
    {
        SC230AI_DEBUG("(%s):s0.0 i2c_bus error\n");
        return RET_SYS_ERROR;
    }
}

#ifdef I2C_WRITE_CONTINUE

#define MAX_I2C_MSG 80
static int SC230AI_write_multiple_reg_continue(struct SC230AI_dev *dev,
        const uint8_t *i2c_data, int len)
{
    uint16_t i;
    rk_i2c_bus_device *i2c_bus;
    struct rt_i2c_msg msgs[MAX_I2C_MSG];
    int ret = 0;
    int offset = 0;

    MACRO_ASSERT(dev != RK_NULL);
    MACRO_ASSERT(i2c_data != RK_NULL);

    i2c_bus = dev->i2c_bus;
    MACRO_ASSERT(i2c_bus != RK_NULL);

    for (i = 0; i < i2c_data[len - 1]; i++)
    {
        msgs[i].addr = SC230AI_7BIT_ADDRESS;
        msgs[i].flags = RT_I2C_WR;
        msgs[i].buf = (uint8_t *)&i2c_data[offset + 1];
        msgs[i].len = i2c_data[offset];
        offset += (i2c_data[offset] + 1);
    }
#if 0
    ret = rt_i2c_transfer(i2c_bus, msgs, i2c_data[len - 1]);
#else
    for (i = 0; i < i2c_data[len - 1]; i++)
        ret |= rt_i2c_transfer(i2c_bus, &msgs[i], 1);
#endif
    if (ret == 1)
    {
        SC230AI_DEBUG("(%s):s0.0 i2c_bus ok\n");
        return RET_SYS_EOK;
    }
    else
    {
        SC230AI_DEBUG("(%s):s0.0 i2c_bus error\n");
        return RET_SYS_ERROR;
    }

}

#else
static ret_err_t SC230AI_write_reg_continue(rk_i2c_bus_device *bus,
        char *data, uint32_t len)
{
    struct rt_i2c_msg msgs;
    int ret = 0;

    MACRO_ASSERT(bus != RK_NULL);

    msgs.addr = SC230AI_7BIT_ADDRESS;
    msgs.flags = RT_I2C_WR;
    msgs.buf = data;
    msgs.len = len;
    ret = rt_i2c_transfer(bus, &msgs, 1);

    if (ret == 1)
    {
        SC230AI_DEBUG("(%s):s0.0 i2c_bus ok\n");
        return RET_SYS_EOK;
    }
    else
    {
        SC230AI_DEBUG("(%s):s0.0 i2c_bus error\n");
        return RET_SYS_ERROR;
    }
}

#define I2C_WRITE_DEBUG
static void SC230AI_write_multiple_reg(struct SC230AI_dev *dev,
                                       const struct SC230AI_sensor_reg *reg, int len)
{
    uint16_t i;
    rk_i2c_bus_device *i2c_bus;
    int k = 0;
    char *data = rt_malloc(len);
    uint16_t reg_addr;
#ifdef I2C_WRITE_DEBUG
    int j = 0;
    int cnt = 0;
#endif

    MACRO_ASSERT(dev != RK_NULL);
    MACRO_ASSERT(reg != RK_NULL);

    i2c_bus = dev->i2c_bus;
    MACRO_ASSERT(i2c_bus != RK_NULL);

    for (i = 0;; i++)
    {
        if (reg[i].reg_addr == REG_END)
        {
            if (k > 0)
            {
#ifdef I2C_WRITE_DEBUG
                cnt++;
                rt_kprintf("0x%x, ", k + 2);
                for (j = 0; j < k + 2; j++)
                    rt_kprintf("0x%02x, ", data[j]);
                rt_kprintf("\n");
#endif
                SC230AI_write_reg_continue(i2c_bus, data, k + 2);
                k = 0;
            }
            break;
        }

        if (reg[i].reg_addr == REG_DELAY)
        {
            if (k > 0)
            {
#ifdef I2C_WRITE_DEBUG
                cnt++;
                rt_kprintf("0x%x, ", k + 2);
                for (j = 0; j < k + 2; j++)
                    rt_kprintf("0x%02x, ", data[j]);
                rt_kprintf("\n");
#endif
                SC230AI_write_reg_continue(i2c_bus, data, k + 2);
                k = 0;
            }
            HAL_DelayUs(reg[i].val);
        }
        else
        {
            if (k == 0)
            {
                reg_addr = reg[i].reg_addr;
                data[0] = ((reg_addr >> 8) & 0xff);
                data[1] = ((reg_addr >> 0) & 0xff);
                data[2] = reg[i].val;
                k++;
            }
            else
            {
                if ((reg[i - 1].reg_addr + 1) == reg[i].reg_addr)
                {
                    data[k + 2] = reg[i].val;
                    k++;
                    //rt_kprintf(">>>k %d, addr %04x\n", k, reg[i].reg_addr);
                }
                else
                {
#ifdef I2C_WRITE_DEBUG
                    cnt++;
                    rt_kprintf("0x%x, ", k + 2);
                    for (j = 0; j < k + 2; j++)
                        rt_kprintf("0x%02x, ", data[j]);
                    rt_kprintf("\n");
#endif
                    SC230AI_write_reg_continue(i2c_bus, data, k + 2);
                    reg_addr = reg[i].reg_addr;
                    data[0] = ((reg_addr >> 8) & 0xff);
                    data[1] = ((reg_addr >> 0) & 0xff);
                    data[2] = reg[i].val;
                    k = 1;
                }

            }
        }
    }
#ifdef I2C_WRITE_DEBUG
    rt_kprintf("0x%x,\n", cnt);
#endif

}
#endif

static int sc230ai_get_gain_reg(uint8_t *again, uint8_t *dgain,
                                uint8_t *dgain_fine, uint32_t total_gain)
{
    int ret = 0;

    if (total_gain < SC230AI_GAIN_MIN)
        total_gain = SC230AI_GAIN_MIN;
    else if (total_gain > SC230AI_GAIN_MAX)
        total_gain = SC230AI_GAIN_MAX;

    if (total_gain < 2000)          /* 1 ~ 2 gain*/
    {
        *again = 0x00;
        *dgain = 0x00;
        *dgain_fine = total_gain * 128 / 1000;
    }
    else if (total_gain < 3391)     /* 2 ~ 3.391 gain*/
    {
        *again = 0x01;
        *dgain = 0x00;
        *dgain_fine = total_gain * 128 / 1000 / 2;
    }
    else if (total_gain < 3391 * 2)         /* 3.391 ~ 6.782 gain*/
    {
        *again = 0x40;
        *dgain = 0x00;
        *dgain_fine = total_gain * 128 / 3391;
    }
    else if (total_gain < 3391 * 4)         /* 6.782 ~ 13.564 gain*/
    {
        *again = 0x48;
        *dgain = 0x00;
        *dgain_fine = total_gain * 128 / 3391 / 2;
    }
    else if (total_gain < 3391 * 8)         /* 13.564 ~ 27.128 gain*/
    {
        *again = 0x49;
        *dgain = 0x00;
        *dgain_fine = total_gain * 128 / 3391 / 4;
    }
    else if (total_gain < 3391 * 16)        /* 27.128 ~ 54.256 gain*/
    {
        *again = 0x4b;
        *dgain = 0x00;
        *dgain_fine = total_gain * 128 / 3391 / 8;
    }
    else if (total_gain < 3391 * 32)        /* 54.256 ~ 108.512 gain*/
    {
        *again = 0x4f;
        *dgain = 0x00;
        *dgain_fine = total_gain * 128 / 3391 / 16;
    }
    else if (total_gain < 3391 * 64)        /* 108.512 ~ 217.024 gain*/
    {
        *again = 0x5f;
        *dgain = 0x00;
        *dgain_fine = total_gain * 128 / 3391 / 32;
    }
    else if (total_gain < 3391 * 128)       /* 217.024 ~ 434.048 gain*/
    {
        *again = 0x5f;
        *dgain = 0x01;
        *dgain_fine = total_gain * 128 / 3391 / 64;
    }
    else if (total_gain < 3391 * 256)       /* 434.048 ~ 868.096 gain*/
    {
        *again = 0x5f;
        *dgain = 0x03;
        *dgain_fine = total_gain * 128 / 3391 / 128;
    }
    else if (total_gain < 3391 * 512)       /* 868.096 ~ 1736.192 gain*/
    {
        *again = 0x5f;
        *dgain = 0x07;
        *dgain_fine = total_gain * 128 / 3391 / 128;
    }

    return ret;
}

static rt_err_t rk_sc230ai_set_expval(struct SC230AI_dev *dev, struct rk_camera_exp_val *exp)
{
    rt_err_t ret = RET_SYS_EOK;
    uint32_t l_exp_time;
    uint32_t l_a_gain;
    uint8_t a_gain, d_gain, d_gain_fine;

    SC230AI_DEBUG("(%s) enter \n", __FUNCTION__);

    MACRO_ASSERT(dev != RK_NULL);

    rt_mutex_take(&dev->mutex_lock, RT_WAITING_FOREVER);

    if (!dev->has_init_exp && !dev->streaming)
    {
        dev->init_exp = *exp;
        dev->has_init_exp = true;
        SC230AI_DEBUG("sc230ai don't stream, record exp for hdr!\n");

        rt_mutex_release(&dev->mutex_lock);
        return ret;
    }

    l_exp_time = exp->reg_time[0] * 2;
    l_a_gain = exp->reg_gain[0] * 50;// camcfg_sc230ai.ini set gain_C1 = 20 instead of 1000

    SC230AI_DEBUG("sc230ai: exp req: L_exp: 0x%x, 0x%x\n",
                  l_exp_time, l_a_gain);

    ret  = SC230AI_write_reg(dev->i2c_bus, SC230AI_REG_EXP_LONG_H,
                             (uint8_t)((l_exp_time >> 12) & 0xf));
    ret |= SC230AI_write_reg(dev->i2c_bus, SC230AI_REG_EXP_LONG_M,
                             (uint8_t)((l_exp_time >> 4) & 0xff));
    ret |= SC230AI_write_reg(dev->i2c_bus, SC230AI_REG_EXP_LONG_L,
                             (uint8_t)((l_exp_time & 0xf) << 4));
    sc230ai_get_gain_reg(&a_gain, &d_gain, &d_gain_fine, l_a_gain);

    SC230AI_DEBUG("%s: a_gain: 0x%x d_gain: 0x%x, d_gain_fine: 0x%x\n", __func__, a_gain, d_gain, d_gain_fine);

    ret = SC230AI_write_reg(dev->i2c_bus, SC230AI_REG_ANA_GAIN, a_gain);
    ret |= SC230AI_write_reg(dev->i2c_bus, SC230AI_REG_DIG_GAIN, d_gain);
    ret |= SC230AI_write_reg(dev->i2c_bus, SC230AI_REG_DIG_FINE_GAIN, d_gain_fine);

    rt_mutex_release(&dev->mutex_lock);

    SC230AI_DEBUG("(%s) exit \n", __FUNCTION__);

    return ret;
}

static void SC230AI_stream_on(struct SC230AI_dev *dev)
{
    SC230AI_DEBUG("(%s) enter \n", __FUNCTION__);

    rk_kprintf("sc230ai on enter tick:%u\n", rt_tick_get());

    MACRO_ASSERT(dev != RK_NULL);

    rt_mutex_take(&dev->mutex_lock, RT_WAITING_FOREVER);

#ifdef I2C_WRITE_CONTINUE
    SC230AI_write_multiple_reg_continue((struct SC230AI_dev *)dev, dev->cur_mode->reg_list, dev->cur_mode->reg_list_size);
#else
    SC230AI_write_multiple_reg((struct SC230AI_dev *)dev, dev->cur_mode->reg_list, dev->cur_mode->reg_list_size);
#endif
    if (dev->has_init_exp)
    {
        rt_mutex_release(&dev->mutex_lock);
        rk_sc230ai_set_expval(dev, &dev->init_exp);
        rt_mutex_take(&dev->mutex_lock, RT_WAITING_FOREVER);
    }
    SC230AI_write_reg(dev->i2c_bus, 0x0100, 0x01);
    rt_mutex_release(&dev->mutex_lock);
    dev->streaming = true;
    SC230AI_DEBUG("(%s) exit \n", __FUNCTION__);
    rk_kprintf("sc230ai on exit tick:%u\n", rt_tick_get());
}

static void SC230AI_stream_off(struct SC230AI_dev *dev)
{
    rk_i2c_bus_device *i2c_bus;

    SC230AI_DEBUG("(%s) enter \n", __FUNCTION__);

    MACRO_ASSERT(dev != RK_NULL);


    i2c_bus = dev->i2c_bus;
    if (i2c_bus)
    {
        rt_mutex_take(&dev->mutex_lock, RT_WAITING_FOREVER);

        SC230AI_write_reg(dev->i2c_bus, 0x0100, 0x00);

        rt_mutex_release(&dev->mutex_lock);
    }
    else
    {
        SC230AI_INFO(dev, "Err: not find out i2c bus!\n");
    }
    dev->streaming = false;

    SC230AI_DEBUG("(%s) exit \n", __FUNCTION__);
}

ret_err_t rk_SC230AI_init(struct rk_camera_device *dev)
{
    SC230AI_DEBUG("(%s) enter \n", __FUNCTION__);

    ret_err_t ret = RET_SYS_EOK;
    struct SC230AI_dev *SC230AI;
    struct rk_camera_device *camera;

    SC230AI = (struct SC230AI_dev *)dev;
    camera = (struct rk_camera_device *)(&SC230AI->parent);
#if RT_USING_SC230AI_OPS
    if (SC230AI->ops->init)
    {
        return (SC230AI->ops->init(SC230AI));
    }
#else

    if (SC230AI)
    {
        camera->info.mbus_fmt.width = 640;
        camera->info.mbus_fmt.height = 480;
        camera->info.mbus_fmt.pixelcode = MEDIA_BUS_FMT_SBGGR10_1X10;//0x0c uyvy;0x08 vyuy;0x04 yvyu;0x00 yuyv
        camera->info.mbus_fmt.field = 0;
        camera->info.mbus_fmt.colorspace = 0;
        camera->info.mbus_config.linked_freq = 185625000;
        camera->info.mbus_config.mbus_type = CAMERA_MBUS_CSI2_DPHY;
        camera->info.mbus_config.flags = MEDIA_BUS_FLAGS_CSI2_LVDS_LANES_2 |
                                         MEDIA_BUS_FLAGS_CSI2_LVDS_CLOCK_MODE_CONTIN;
        camera->cfg_ae_init = (struct ae_init_info *)sc230ai_camcfg;
        camera->info.hdr_mode = 0;

        SC230AI->i2c_bus = (rk_i2c_bus_device *)rt_device_find(SC230AI->i2c_name);

        if (!SC230AI->i2c_bus)
        {
            SC230AI_DEBUG("Warning:not find i2c source 2:%s !!!\n",
                          SC230AI->i2c_name);
            return RET_SYS_ENOSYS;
        }
        else
        {
            SC230AI_DEBUG("(%s):s0 find i2c_bus:%s\n", SC230AI->i2c_name);
        }
    }
    else
    {
        ret = RET_SYS_ENOSYS;
    }
#endif

    SC230AI_DEBUG("(%s) exit \n", __FUNCTION__);
    return ret;

}

static ret_err_t rk_SC230AI_open(struct rk_camera_device *dev, rt_uint16_t oflag)
{
#if RT_USING_SC230AI_OPS
    struct SC230AI_dev *SC230AI;
#endif
    ret_err_t ret = RET_SYS_EOK;

    SC230AI_DEBUG("(%s) enter \n", __FUNCTION__);

    MACRO_ASSERT(dev != RK_NULL);

#if RT_USING_SC230AI_OPS
    SC230AI = (struct SC230AI_dev *)dev
              if (SC230AI->ops->open)
    {
        return (SC230AI->ops->open(SC230AI));
    }
#endif

    SC230AI_DEBUG("(%s) exit \n", __FUNCTION__);

    return ret;
}

ret_err_t rk_SC230AI_close(struct rk_camera_device *dev)
{
#if RT_USING_SC230AI_OPS
    struct SC230AI_dev *SC230AI;
#endif
    uint8_t ret = RET_SYS_EOK;

    SC230AI_INFO(dev, "(%s) enter \n", __FUNCTION__);

    MACRO_ASSERT(dev != RK_NULL);

#if RT_USING_SC230AI_OPS
    SC230AI = (struct SC230AI_dev *)dev;
    if (SC230AI->ops->close)
    {
        return (SC230AI->ops->close(SC230AI));
    }
#endif

    SC230AI_INFO(dev, "(%s) exit \n", __FUNCTION__);
    return ret;

}

static rt_err_t rk_sc230ai_get_expinf(struct SC230AI_dev *dev, struct rk_camera_exp_info *exp)
{
    rt_err_t ret = RET_SYS_EOK;
    const struct sc230ai_mode *mode;

    SC230AI_DEBUG("(%s) enter \n", __FUNCTION__);

    MACRO_ASSERT(dev != RK_NULL);

    mode = dev->cur_mode;
    exp->width = mode->width;
    exp->height = mode->height;
    exp->hts = mode->hts_def;
    exp->vts = mode->vts_def;
    exp->pix_clk = (uint64_t)exp->hts * (uint64_t)exp->vts * (uint64_t)mode->max_fps.denominator /
                   (uint64_t)mode->max_fps.numerator;
    exp->time_valid_delay = 2;
    exp->gain_valid_delay = 2;

    exp->dst_width = dev->dst_mode->width;
    exp->dst_height = dev->dst_mode->height;
    exp->dst_hts = dev->dst_mode->hts_def;
    exp->dst_vts = dev->dst_mode->vts_def;
    exp->dst_pix_clk = (uint64_t)exp->dst_hts * (uint64_t)exp->dst_vts *
                       (uint64_t)dev->dst_mode->max_fps.denominator /
                       (uint64_t)dev->dst_mode->max_fps.numerator;

    SC230AI_DEBUG("(%s) exit \n", __FUNCTION__);
    return ret;
}

static rt_err_t rk_sc230ai_get_intput_fmt(struct SC230AI_dev *dev, struct rk_camera_mbus_framefmt *mbus_fmt)
{
    rt_err_t ret = RET_SYS_EOK;
    const struct sc230ai_mode *mode;

    SC230AI_DEBUG("(%s) enter \n", __FUNCTION__);

    MACRO_ASSERT(dev != RK_NULL);

    mode = dev->cur_mode;
    mbus_fmt->width = mode->width;
    mbus_fmt->height = mode->height;
    mbus_fmt->field = 0;
    mbus_fmt->pixelcode = MEDIA_BUS_FMT_SBGGR10_1X10;

    SC230AI_DEBUG("(%s) exit \n", __FUNCTION__);
    return ret;

}

static rt_err_t rk_sc230ai_set_intput_fmt(struct SC230AI_dev *dev, struct rk_camera_mbus_framefmt *mbus_fmt)
{
    rt_err_t ret = RET_SYS_ENOSYS;
    const struct sc230ai_mode *mode;
    int i = 0;
    bool is_find_fmt = false;

    SC230AI_DEBUG("(%s) enter \n", __FUNCTION__);

    MACRO_ASSERT(dev != RK_NULL);

    for (i = 0; i < ARRAY_SIZE(supported_modes); i++)
    {
        mode = &supported_modes[i];
        if (mbus_fmt->width == mode->width &&
                mbus_fmt->height == mode->height)
        {
            is_find_fmt = true;
            break;
        }
    }
    if (is_find_fmt)
    {
        if (mode->width != dev->cur_mode->width)
        {
            dev->cur_mode = mode;
            rt_mutex_take(&dev->mutex_lock, RT_WAITING_FOREVER);

#ifdef I2C_WRITE_CONTINUE
            SC230AI_write_multiple_reg_continue((struct SC230AI_dev *)dev, dev->cur_mode->reg_list, dev->cur_mode->reg_list_size);
#else
            SC230AI_write_multiple_reg((struct SC230AI_dev *)dev, dev->cur_mode->reg_list, dev->cur_mode->reg_list_size);
#endif
            rt_mutex_release(&dev->mutex_lock);
        }
        ret = RET_SYS_EOK;
    }

    SC230AI_DEBUG("(%s) exit \n", __FUNCTION__);

    return ret;
}


ret_err_t rk_SC230AI_control(struct rk_camera_device *dev,
                             int cmd,
                             void *args)
{
    SC230AI_DEBUG("(%s) enter \n", __FUNCTION__);

    ret_err_t ret = RET_SYS_EOK;
    struct SC230AI_dev *SC230AI;

    MACRO_ASSERT(dev != RK_NULL);
    SC230AI = (struct SC230AI_dev *)dev;
#if RT_USING_SC230AI_OPS
    return (SC230AI->ops->control(SC230AI, cmd, args));
#else
    switch (cmd)
    {

    case RK_DEVICE_CTRL_DEVICE_INIT:
    {
        ret = rk_SC230AI_init(dev);
    }
    break;

    case RK_DEVICE_CTRL_CAMERA_STREAM_ON:
    {
        SC230AI_stream_on(SC230AI);
    }
    break;
    case RK_DEVICE_CTRL_CAMERA_STREAM_OFF:
    {
        SC230AI_stream_off(SC230AI);
    }
    break;
    case RK_DEVICE_CTRL_CAMERA_GET_EXP_INF:
    {
        ret = rk_sc230ai_get_expinf(SC230AI, (struct rk_camera_exp_info *)args);
    }
    break;
    case RK_DEVICE_CTRL_CAMERA_SET_EXP_VAL:
    {
        ret = rk_sc230ai_set_expval(SC230AI, (struct rk_camera_exp_val *)args);
    }
    break;
    case RK_DEVICE_CTRL_CAMERA_GET_FORMAT:
    {
        ret = rk_sc230ai_get_intput_fmt(SC230AI, (struct rk_camera_mbus_framefmt *)args);
    }
    break;
    case RK_DEVICE_CTRL_CAMERA_SET_FORMAT:
    {
        ret = rk_sc230ai_set_intput_fmt(SC230AI, (struct rk_camera_mbus_framefmt *)args);
    }
    break;
    default:
        break;
    }
#endif
    SC230AI_DEBUG("(%s) exit \n", __FUNCTION__);
    return ret;
}

struct rk_camera_ops rk_SC230AI_ops =
{
    rk_SC230AI_init,
    rk_SC230AI_open,
    NULL,
    rk_SC230AI_control
};


int rk_camera_SC230AI_init(void)
{
    ret_err_t ret = RET_SYS_EOK;
    struct SC230AI_dev *instance = &g_SC230AI;
    struct rk_camera_device *camera = &instance->parent;
    struct clk_gate *clkgate;

    camera->ops = &rk_SC230AI_ops;

    rt_strncpy(instance->name, SC230AI_DEVICE_NAME, rt_strlen(SC230AI_DEVICE_NAME));
    rt_strncpy(instance->i2c_name, I2C_BUS_NAME, rt_strlen(I2C_BUS_NAME));

#if SC230AI_I2C_DEBUG_ENABLE
    instance->i2c_bus = (rk_i2c_bus_device *)rt_device_find(instance->i2c_name);
    if (!instance->i2c_bus)
    {
        SC230AI_INFO(instance, "Warning:not find i2c source 3:%s !!!\n", instance->i2c_name);
        ret = RET_SYS_ENOSYS;
        goto ERR;
    }
    else
    {
        SC230AI_DEBUG("(%s):s0.0 find i2c_bus\n");
    }
#endif

    instance->cur_mode = &supported_modes[0];
    instance->dst_mode = &supported_modes[1];

    HAL_GPIO_SetPinDirection(CAMERA_RST_GPIO_GROUP, CAMERA_RST_GPIO_PIN, GPIO_OUT);
    HAL_GPIO_SetPinLevel(CAMERA_RST_GPIO_GROUP, CAMERA_RST_GPIO_PIN, 1);
    clk_set_rate(CAMERA_CLK_REF, 27000000);
    clkgate = get_clk_gate_from_id(CAMERA_CLK_REF);
    clk_enable(clkgate);

    rt_mutex_init(&instance->mutex_lock, "SC230AI_mutex", RT_IPC_FLAG_FIFO);
    MACRO_ASSERT(rt_object_get_type(&instance->mutex_lock.parent.parent) == RT_Object_Class_Mutex);
    camera->i2c_bus = instance->i2c_bus;
    rt_strncpy(camera->name, instance->name, rt_strlen(SC230AI_DEVICE_NAME));
    rk_camera_register(camera, camera->name, instance);
    SC230AI_DEBUG("(%s) exit \n", __FUNCTION__);
    return ret;
}

void SC230AI_detect(void)
{
    struct SC230AI_dev *instance = &g_SC230AI;

    SC230AI_DEBUG("start to detect SC230AI for testing \n");
    SC230AI_DEBUG("dev name:%s\n", instance->name);
    SC230AI_DEBUG("dev i2c_bus:%s\n", instance->i2c_name);
    instance->i2c_bus = (rk_i2c_bus_device *)rt_device_find(instance->i2c_name);
    if (!instance->i2c_bus)
    {
        SC230AI_DEBUG("Warning:not find i2c source 1:%s !!!\n", instance->i2c_name);
        return;
    }
    else
    {
        SC230AI_DEBUG("(%s):s0 find i2c_bus:%s\n", instance->i2c_name);
    }

}
#if defined(__RT_THREAD__)
INIT_DEVICE_EXPORT(rk_camera_SC230AI_init);
#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(SC230AI_detect, check SC230AI is available or not);
#endif
#endif
#endif

