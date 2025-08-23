/**
  * Copyright (c) 2019 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    camera.h
  * @version V0.0.1
  * @brief   camera device abstract for rk625
  *
  * Change Logs:
  * Date           Author          Notes
  * 2019-03-29     ISP Team      first implementation
  *
  ******************************************************************************
  */
#ifndef __CAMERA_H__
#define __CAMERA_H__

#if defined(__RT_THREAD__)
#include "adapter/adapter.h"
#include "drv_clock.h"
#include "camera_mediabus.h"

#elif defined(__RK_OS__)
#include "driver/adapter.h"
#include "driver/camera_mediabus.h"
#endif

#if defined(RT_USING_CAMERA) || defined(CONFIG_DRIVER_CAMERA)
#define CAMERA_MODULE_COMPILED
#else
#undef CAMERA_MODULE_COMPILED
#endif

#ifdef CAMERA_MODULE_COMPILED

/* Exported macro ------------------------------------------------------------*/
#define RK_CAMERA_DEVICE_NAME_SIZE                  (12)
#define RK_CAMERA_I2C_NAME_SIZE                     (12)

#define RK_DEVICE_CTRL_DEVICE_INIT                  (0)
#define RK_DEVICE_CTRL_CAMERA_STREAM_ON             (1)
#define RK_DEVICE_CTRL_CAMERA_STREAM_OFF            (2)
#define RK_DEVICE_CTRL_CAMERA_GET_AEC               (3)
#define RK_DEVICE_CTRL_CAMERA_SET_AEC               (4)
#define RK_DEVICE_CTRL_CAMERA_GET_FORMAT            (5)
#define RK_DEVICE_CTRL_CAMERA_SET_FORMAT            (6)
#define RK_DEVICE_CTRL_CAMERA_GET_BLANK             (7)
#define RK_DEVICE_CTRL_CAMERA_GET_ITF_CFG           (8)
#define RK_VIDIOC_SENSOR_MODE_DATA                  (9)
#define RK_VIDIOC_SENSOR_CONFIGINFO                 (10)
#define RK_DEVICE_CTRL_CAMERA_IQ_INFO               (11)
#define RK_V4L2_CID_AUTO_FPS                        (12)
#define RK_DEVICE_CTRL_CAMERA_GET_FRMINTVAL         (13)
#define RK_DEVICE_CTRL_CAMERA_SET_FLIPMIRROR        (14)
#define RK_DEVICE_CTRL_CAMERA_GET_EXP_INF           (15)
#define RK_DEVICE_CTRL_CAMERA_SET_EXP_VAL           (16)
#define RK_DEVICE_CTRL_CAMERA_CTRL                  (17)
#define RK_DEVICE_CTRL_CID_STOP_IR_FILTER           (18)
#define RK_DEVICE_CTRL_CID_OPEN_IR_LED              (19)
#define RK_DEVICE_CTRL_CID_OPEN_WHITE_LED           (20)
#define RK_DEVICE_CTRL_CID_OPEN_BLUE_LED            (21)
#define RK_DEVICE_CTRL_CID_OPEN_RED_LED             (22)
#define RK_DEVICE_CTRL_CID_GET_LIGHTS_SENSOR_VAL    (23)

#define CSI2_DT_YUV420_8b   (0x18)
#define CSI2_DT_YUV420_10b  (0x19)
#define CSI2_DT_YUV422_8b   (0x1E)
#define CSI2_DT_YUV422_10b  (0x1F)
#define CSI2_DT_RGB444      (0x20)
#define CSI2_DT_RGB555      (0x21)
#define CSI2_DT_RGB565      (0x22)
#define CSI2_DT_RGB666      (0x23)
#define CSI2_DT_RGB888      (0x24)
#define CSI2_DT_RAW6        (0x28)
#define CSI2_DT_RAW7        (0x29)
#define CSI2_DT_RAW8        (0x2A)
#define CSI2_DT_RAW10       (0x2B)
#define CSI2_DT_RAW12       (0x2C)

#define CAMERA_BOOT_RETRY_NUM 10

enum v4l2_mbus_pixelcode
{
    V4L2_MBUS_FMT_FIXED = 0x0001,

    /* RGB - next is 0x100d */
    V4L2_MBUS_FMT_RGB444_2X8_PADHI_BE = 0x1001,
    V4L2_MBUS_FMT_RGB444_2X8_PADHI_LE = 0x1002,
    V4L2_MBUS_FMT_RGB555_2X8_PADHI_BE = 0x1003,
    V4L2_MBUS_FMT_RGB555_2X8_PADHI_LE = 0x1004,
    V4L2_MBUS_FMT_BGR565_2X8_BE = 0x1005,
    V4L2_MBUS_FMT_BGR565_2X8_LE = 0x1006,
    V4L2_MBUS_FMT_RGB565_2X8_BE = 0x1007,
    V4L2_MBUS_FMT_RGB565_2X8_LE = 0x1008,
    V4L2_MBUS_FMT_RGB666_1X18 = 0x1009,
    V4L2_MBUS_FMT_RGB888_1X24 = 0x100a,
    V4L2_MBUS_FMT_RGB888_2X12_BE = 0x100b,
    V4L2_MBUS_FMT_RGB888_2X12_LE = 0x100c,

    /* YUV (including grey) - next is 0x2017 */
    V4L2_MBUS_FMT_Y8_1X8 = 0x2001,
    V4L2_MBUS_FMT_UV8_1X8 = 0x2015,
    V4L2_MBUS_FMT_UYVY8_1_5X8 = 0x2002,
    V4L2_MBUS_FMT_VYUY8_1_5X8 = 0x2003,
    V4L2_MBUS_FMT_YUYV8_1_5X8 = 0x2004,
    V4L2_MBUS_FMT_YVYU8_1_5X8 = 0x2005,
    V4L2_MBUS_FMT_UYVY8_2X8 = 0x2006,
    V4L2_MBUS_FMT_VYUY8_2X8 = 0x2007,
    V4L2_MBUS_FMT_YUYV8_2X8 = 0x2008,
    V4L2_MBUS_FMT_YVYU8_2X8 = 0x2009,
    V4L2_MBUS_FMT_Y10_1X10 = 0x200a,
    V4L2_MBUS_FMT_YUYV10_2X10 = 0x200b,
    V4L2_MBUS_FMT_YVYU10_2X10 = 0x200c,
    V4L2_MBUS_FMT_Y12_1X12 = 0x2013,
    V4L2_MBUS_FMT_UYVY8_1X16 = 0x200f,
    V4L2_MBUS_FMT_VYUY8_1X16 = 0x2010,
    V4L2_MBUS_FMT_YUYV8_1X16 = 0x2011,
    V4L2_MBUS_FMT_YVYU8_1X16 = 0x2012,
    V4L2_MBUS_FMT_YDYUYDYV8_1X16 = 0x2014,
    V4L2_MBUS_FMT_YUYV10_1X20 = 0x200d,
    V4L2_MBUS_FMT_YVYU10_1X20 = 0x200e,
    V4L2_MBUS_FMT_YUV10_1X30 = 0x2016,

    /* Bayer - next is 0x3019 */
    V4L2_MBUS_FMT_SBGGR8_1X8 = 0x3001,
    V4L2_MBUS_FMT_SGBRG8_1X8 = 0x3013,
    V4L2_MBUS_FMT_SGRBG8_1X8 = 0x3002,
    V4L2_MBUS_FMT_SRGGB8_1X8 = 0x3014,
    V4L2_MBUS_FMT_SBGGR10_ALAW8_1X8 = 0x3015,
    V4L2_MBUS_FMT_SGBRG10_ALAW8_1X8 = 0x3016,
    V4L2_MBUS_FMT_SGRBG10_ALAW8_1X8 = 0x3017,
    V4L2_MBUS_FMT_SRGGB10_ALAW8_1X8 = 0x3018,
    V4L2_MBUS_FMT_SBGGR10_DPCM8_1X8 = 0x300b,
    V4L2_MBUS_FMT_SGBRG10_DPCM8_1X8 = 0x300c,
    V4L2_MBUS_FMT_SGRBG10_DPCM8_1X8 = 0x3009,
    V4L2_MBUS_FMT_SRGGB10_DPCM8_1X8 = 0x300d,
    V4L2_MBUS_FMT_SBGGR10_2X8_PADHI_BE = 0x3003,
    V4L2_MBUS_FMT_SBGGR10_2X8_PADHI_LE = 0x3004,
    V4L2_MBUS_FMT_SBGGR10_2X8_PADLO_BE = 0x3005,
    V4L2_MBUS_FMT_SBGGR10_2X8_PADLO_LE = 0x3006,
    V4L2_MBUS_FMT_SBGGR10_1X10 = 0x3007,
    V4L2_MBUS_FMT_SGBRG10_1X10 = 0x300e,
    V4L2_MBUS_FMT_SGRBG10_1X10 = 0x300a,
    V4L2_MBUS_FMT_SRGGB10_1X10 = 0x300f,
    V4L2_MBUS_FMT_SBGGR12_1X12 = 0x3008,
    V4L2_MBUS_FMT_SGBRG12_1X12 = 0x3010,
    V4L2_MBUS_FMT_SGRBG12_1X12 = 0x3011,
    V4L2_MBUS_FMT_SRGGB12_1X12 = 0x3012,

    /* JPEG compressed formats - next is 0x4002 */
    V4L2_MBUS_FMT_JPEG_1X8 = 0x4001,

    /* Vendor specific formats - next is 0x5002 */

    /* S5C73M3 sensor specific interleaved UYVY and JPEG */
    V4L2_MBUS_FMT_S5C_UYVY_JPEG_1X8 = 0x5001,
};
#define RK_CAMERA_HDREXP_NUM                        (3)
/* Exported types ------------------------------------------------------------*/
struct rk_camera_ctrl
{
    void *ctrl_dev;
    ret_err_t (*control)(void *ctrl_dev, int cid, void *arg);
    ret_err_t (*init)(void *ctrl_dev);
};

struct rk_camera_info
{
    struct rk_camera_mbus_framefmt mbus_fmt;
    struct rk_camera_mbus_config mbus_config;
    uint32_t hdr_mode;
};

enum rk_camera_hdr_mode
{
    NO_HDR = 0,
    HDR_X2 = 5,
    HDR_X3 = 6,
};

struct rk_camera_exp_info
{
    uint64_t pix_clk;
    uint32_t hts;
    uint32_t vts;
    uint32_t width;
    uint32_t height;
    uint64_t dst_pix_clk;
    uint32_t dst_hts;
    uint32_t dst_vts;
    uint32_t dst_width;
    uint32_t dst_height;
    uint32_t time_valid_delay;
    uint32_t gain_valid_delay;
};

struct rk_camera_exp_val
{
    uint32_t reg_time[RK_CAMERA_HDREXP_NUM];
    uint32_t reg_gain[RK_CAMERA_HDREXP_NUM];
};
struct rk_camera_iq_info
{
    void *addr;
};

struct rk_camera_ops;
struct rk_camera_device
{
    rk_device parent;
    char name[RK_CAMERA_DEVICE_NAME_SIZE];
    struct rk_camera_info info;
    const struct rk_camera_ops *ops;
    char i2c_name[RK_CAMERA_I2C_NAME_SIZE];
    rk_i2c_bus_device *i2c_bus;
    struct rk_camera_ctrl *ctrl;
    struct ae_init_info *cfg_ae_init;
#if defined(__RK_OS__)
    uint8_t class_id;
    uint8_t object_id;
#endif
};

struct rk_camera_ops
{
    ret_err_t (*init)(struct rk_camera_device *dev);
    ret_err_t (*open)(struct rk_camera_device *dev, uint16_t oflag);
    ret_err_t (*close)(struct rk_camera_device *dev);
    ret_err_t (*control)(struct rk_camera_device *dev, dt_cmd_t cmd, void *arg);
    ret_err_t (*rx_indicate)(struct rk_camera_device *dev, ret_size_t size);
};

#if defined(__RK_OS__)
HDC rk_rkos_camera_create(uint8_t dev_id, void *arg);
ret_err_t rk_rkos_camera_delete(uint8_t dev_id, void *arg);
#endif

ret_err_t rk_camera_register(struct rk_camera_device *camera,
                             const char *name,
                             void *data);
int pltfrm_camera_module_pix_fmt2csi2_dt(int src_pix_fmt);
ret_err_t rk_camera_ctrl_init(struct rk_camera_device *camera, struct rk_camera_ctrl *ctrl);
#endif

#endif
