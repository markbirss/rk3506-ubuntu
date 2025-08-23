/**
  * Copyright (c) 2022 Rockchip Electronic Co.,Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    drv_isp_module.c
  * @version V0.0.1
  * @date    6-July-2022
  * @brief
  *
  ******************************************************************************
  */

#include "drv_isp3.h"
#include "regs_v3x.h"

#if defined(RT_USING_ISP3)

#define ISP_DEBUG                         0
#if ISP_DEBUG
#include <stdio.h>
#define ISP_DBG(...)                      \
            do {                          \
                rt_kprintf("[ISP]:");     \
                rt_kprintf(__VA_ARGS__);  \
                }while(0)
#define rk_isp_function_enter()           rt_kprintf("[ISP]:(%s) enter \n",__FUNCTION__)
#define rk_isp_function_exit()            rt_kprintf("[ISP]:(%s) exit \n",__FUNCTION__)
#else
#define ISP_DBG(...)
#define rk_isp_function_enter()
#define rk_isp_function_exit()
#endif

#define ISP_INFO(dev,arg...)              rt_kprintf("[%s]:",dev->name);rt_kprintf(arg)

static void isp_rawaelite_cfg(struct rk_isp_dev *dev)
{
    struct rk_camera_mbus_framefmt *mbus_fmt = &dev->input.cam_info.mbus_fmt;
    uint32_t w = mbus_fmt->width, h = mbus_fmt->height;
    uint32_t val, wnd_num = 5;

    RKISP_WRITE(ISP3X_RAWAE_LITE_OFFSET, 0);
    val = ((h / wnd_num - 2) & ~1) << 16 |
          ((w / wnd_num - 2) & ~1);
    RKISP_WRITE(ISP3X_RAWAE_LITE_BLK_SIZ, val);

    RKISP_WRITE(ISP3X_RAWAE_LITE_CTRL, 0x3);
}

static void isp_rawaebig_cfg(struct rk_isp_dev *dev)
{
    struct rk_camera_mbus_framefmt *mbus_fmt = &dev->input.cam_info.mbus_fmt;
    uint32_t w = mbus_fmt->width, h = mbus_fmt->height;
    uint32_t val, wnd_num = 5;

    RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_OFFSET, 0);
    val = ((h / wnd_num - 2) & ~1) << 16 |
          ((w / wnd_num - 2) & ~1);
    RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_BLK_SIZE, val);

    val = 2 << 16 | 2;
    RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_WND1_OFFSET, val);
    val = 100 << 16 | 100;
    RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_WND1_SIZE, val);

    val = 2 << 16 | 150;
    RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_WND2_OFFSET, val);
    val = 100 << 16 | 100;
    RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_WND2_SIZE, val);

    val = 150 << 16 | 2;
    RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_WND3_OFFSET, val);
    val = 100 << 16 | 100;
    RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_WND3_SIZE, val);

    val = 150 << 16 | 150;
    RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_WND4_OFFSET, val);
    val = 100 << 16 | 100;
    RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_WND4_SIZE, val);

    RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_CTRL, 0xf3);
}

static void isp_awb_cfg(struct rk_isp_dev *dev)
{
    struct rk_camera_mbus_framefmt *mbus_fmt = &dev->input.cam_info.mbus_fmt;
    uint32_t w = mbus_fmt->width, h = mbus_fmt->height;
    uint32_t val;

    RKISP_WRITE(ISP3X_RAWAWB_BLK_CTRL, 0);
    RKISP_WRITE(ISP3X_RAWAWB_YWEIGHT_CURVE_XCOOR03, 0);
    RKISP_WRITE(ISP3X_RAWAWB_YWEIGHT_CURVE_XCOOR47, 0);
    RKISP_WRITE(ISP3X_RAWAWB_YWEIGHT_CURVE_XCOOR8, 0);
    RKISP_WRITE(ISP3X_RAWAWB_YWEIGHT_CURVE_YCOOR03, 0);
    RKISP_WRITE(ISP3X_RAWAWB_YWEIGHT_CURVE_YCOOR47, 0);
    RKISP_WRITE(ISP32_RAWAWB_EXC_WP_WEIGHT0_3, 0);
    RKISP_WRITE(ISP32_RAWAWB_EXC_WP_WEIGHT4_6, 0);

    RKISP_WRITE(ISP3X_RAWAWB_WIN_OFFS, 0);
    val = h << 16 | w;
    RKISP_WRITE(ISP3X_RAWAWB_WIN_SIZE, val);
    val = (230 << 4) << 16 | (230 << 4);
    RKISP_WRITE(ISP3X_RAWAWB_LIMIT_RG_MAX, val);
    RKISP_WRITE(ISP3X_RAWAWB_LIMIT_BY_MAX, val);
    val = (16 * 3) << 16 | (16 * 3);
    RKISP_WRITE(ISP3X_RAWAWB_LIMIT_RG_MIN, val);
    RKISP_WRITE(ISP3X_RAWAWB_LIMIT_BY_MIN, val);
    val = 256 << 16;
    RKISP_WRITE(ISP3X_RAWAWB_YWEIGHT_CURVE_YCOOR8, val);
    val = 256 << 16 | 256;
    RKISP_WRITE(ISP3X_RAWAWB_PRE_WBGAIN_INV, val);

    val = 256 << 16 | 256;
    RKISP_WRITE(ISP32_ISP_AWB1_GAIN_G, val);
    RKISP_WRITE(ISP32_ISP_AWB1_GAIN_RB, val);
    val = 0x101c6881;
    RKISP_WRITE(ISP3X_RAWAWB_CTRL, val);
}

static void isp_awbgain_cfg(struct rk_isp_dev *dev)
{
    struct rkisp_params_buffer *buf = &dev->params;
    struct isp_awbgain_cfg *awbgain = &buf->params.awbgain;
    uint32_t val;

    val = awbgain->gr << 16 | awbgain->gb;
    RKISP_WRITE(ISP3X_ISP_AWB_GAIN0_G, val);
    RKISP_WRITE(ISP3X_ISP_AWB_GAIN1_G, val);
    RKISP_WRITE(ISP3X_ISP_AWB_GAIN2_G, val);
    val = awbgain->r << 16 | awbgain->b;
    RKISP_WRITE(ISP3X_ISP_AWB_GAIN0_RB, val);
    RKISP_WRITE(ISP3X_ISP_AWB_GAIN1_RB, val);
    RKISP_WRITE(ISP3X_ISP_AWB_GAIN2_RB, val);
}

static void isp_bls_cfg(struct rk_isp_dev *dev)
{
    struct isp_input_fmt *input_fmt = &dev->input.input_fmt;
    struct rkisp_params_buffer *buf = &dev->params;
    struct isp_bls_cfg *cfg = &buf->params.bls;

    switch (input_fmt->mbus_code)
    {
    case MEDIA_BUS_FMT_SBGGR8_1X8:
    case MEDIA_BUS_FMT_SBGGR10_1X10:
    case MEDIA_BUS_FMT_SBGGR12_1X12:
        RKISP_WRITE(ISP3X_BLS_D_FIXED, cfg->r);
        RKISP_WRITE(ISP3X_BLS_C_FIXED, cfg->gr);
        RKISP_WRITE(ISP3X_BLS_B_FIXED, cfg->gb);
        RKISP_WRITE(ISP3X_BLS_A_FIXED, cfg->b);
        break;
    case MEDIA_BUS_FMT_SGBRG8_1X8:
    case MEDIA_BUS_FMT_SGBRG10_1X10:
    case MEDIA_BUS_FMT_SGBRG12_1X12:
        RKISP_WRITE(ISP3X_BLS_D_FIXED, cfg->gr);
        RKISP_WRITE(ISP3X_BLS_C_FIXED, cfg->r);
        RKISP_WRITE(ISP3X_BLS_B_FIXED, cfg->b);
        RKISP_WRITE(ISP3X_BLS_A_FIXED, cfg->gb);
        break;
    case MEDIA_BUS_FMT_SGRBG8_1X8:
    case MEDIA_BUS_FMT_SGRBG10_1X10:
    case MEDIA_BUS_FMT_SGRBG12_1X12:
        RKISP_WRITE(ISP3X_BLS_D_FIXED, cfg->gb);
        RKISP_WRITE(ISP3X_BLS_C_FIXED, cfg->b);
        RKISP_WRITE(ISP3X_BLS_B_FIXED, cfg->r);
        RKISP_WRITE(ISP3X_BLS_A_FIXED, cfg->gr);
        break;
    case MEDIA_BUS_FMT_SRGGB8_1X8:
    case MEDIA_BUS_FMT_SRGGB10_1X10:
    case MEDIA_BUS_FMT_SRGGB12_1X12:
        RKISP_WRITE(ISP3X_BLS_D_FIXED, cfg->b);
        RKISP_WRITE(ISP3X_BLS_C_FIXED, cfg->gb);
        RKISP_WRITE(ISP3X_BLS_B_FIXED, cfg->gr);
        RKISP_WRITE(ISP3X_BLS_A_FIXED, cfg->r);
        break;
    }
    RKISP_WRITE(ISP3X_BLS_CTRL, 1);
    RKISP_WRITE(ISP32_BLS_ISP_OB_PREDGAIN, 0);
}

static void rk_isp_read_stats(struct rk_isp_dev *dev)
{
    struct rkisp_stats_buffer *buf = &dev->stats;
    uint32_t i, val, ctrl, ris;

    ris = RKISP_READ(ISP3X_ISP_3A_RIS);
    if (ris)
    {
        RKISP_WRITE(ISP3X_ISP_ICR, ris);

        buf->meas_type = 0;
        buf->frame_id = dev->frame_id;
        if (ris & ISP3X_3A_RAWAE_CH0)
        {
            ctrl = RKISP_READ(ISP3X_RAWAE_LITE_CTRL);
            if (ctrl & BIT(31))
            {
                for (i = 0; i < ISP_RAWAELITE_MEAN_NUM; i++)
                {
                    val = RKISP_READ(ISP3X_RAWAE_LITE_RO_MEAN + i * 4);
                    buf->stats.rawae0.data[i].channelg_xy = val & 0xfff;
                    buf->stats.rawae0.data[i].channelb_xy = (val >> 12) & 0x3ff;
                    buf->stats.rawae0.data[i].channelr_xy = (val >> 22) & 0x3ff;
                }
                RKISP_WRITE(ISP3X_RAWAE_LITE_CTRL, ctrl);
                buf->meas_type |= ISP_MODULE_RAWAE0;
            }
        }

        if (ris & ISP3X_3A_RAWAE_CH1)
        {
            ctrl = RKISP_READ(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_CTRL);
            if (ctrl & BIT(31))
            {
                for (i = 0; i < ISP_RAWAELITE_MEAN_NUM; i++)
                {

                    val = RKISP_READ(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_RO_MEAN_BASE_ADDR + i * 4);
                    buf->stats.rawae1.data[i].channelg_xy = val & 0xfff;
                    buf->stats.rawae1.data[i].channelb_xy = (val >> 12) & 0x3ff;
                    buf->stats.rawae1.data[i].channelr_xy = (val >> 22) & 0x3ff;
                }
                RKISP_WRITE(ISP3X_RAWAE_BIG2_BASE + ISP3X_RAWAE_BIG_CTRL, ctrl);
                buf->meas_type |= ISP_MODULE_RAWAE1;
            }
        }

        if (ris & ISP3X_3A_RAWAWB)
        {
            ctrl = RKISP_READ(ISP3X_RAWAWB_CTRL);
            if (ctrl & BIT(31))
            {
                buf->stats.rawawb.rgain_nor = RKISP_READ(ISP3X_RAWAWB_SUM_RGAIN_NOR_0);
                buf->stats.rawawb.bgain_nor = RKISP_READ(ISP3X_RAWAWB_SUM_BGAIN_NOR_0);
                buf->stats.rawawb.wp_num_nor = RKISP_READ(ISP3X_RAWAWB_WP_NUM_NOR_0);
                buf->stats.rawawb.rgain_big = RKISP_READ(ISP3X_RAWAWB_SUM_RGAIN_BIG_0);
                buf->stats.rawawb.bgain_big = RKISP_READ(ISP3X_RAWAWB_SUM_BGAIN_BIG_0);
                buf->stats.rawawb.wp_num_big = RKISP_READ(ISP3X_RAWAWB_WP_NUM_BIG_0);
                buf->stats.rawawb.wp_num2 = RKISP_READ(ISP3X_RAWAWB_WPNUM2_0);
                RKISP_WRITE(ISP3X_RAWAWB_CTRL, ctrl);
                buf->meas_type |= ISP_MODULE_RAWAWB;
            }
        }

        if (buf->meas_type)
            rt_sem_release(dev->stats_sem);
    }
}

static void rk_isp_sof(struct rk_isp_dev *dev)
{
    struct rk_isp_device *device = &dev->parent;

    dev->frame_id++;
    if (device->parent.rx_indicate != RT_NULL)
        device->parent.rx_indicate(&device->parent, dev->frame_id);
}

rt_err_t rk_isp_isr(struct rk_isp_dev *dev, uint32_t mis)
{
    rt_err_t ret = RT_EOK;

    rk_isp_function_enter();

    RT_ASSERT(dev != RT_NULL);

    /* sof event */
    if (mis & ISP3X_V_START)
        rk_isp_sof(dev);

    /* frame end */
    if (mis & ISP3X_FRAME)
    {
        rk_isp_read_stats(dev);
        isp_awbgain_cfg(dev);
    }
    /* update params */
    rk_isp_function_exit();

    return ret;
}

rt_err_t rk_isp_module_init(struct rk_isp_dev *dev)
{
    rt_err_t ret = RT_EOK;

    rk_isp_function_enter();

    RT_ASSERT(dev != RT_NULL);

    dev->frame_id = 0;
    isp_rawaelite_cfg(dev);
    isp_rawaebig_cfg(dev);
    isp_awb_cfg(dev);
    isp_awbgain_cfg(dev);
    isp_bls_cfg(dev);

    rk_isp_function_exit();

    return ret;
}

rt_err_t rk_isp_set_params(struct rk_isp_dev *dev, struct rkisp_params_buffer *buf)
{
    rt_err_t ret = RT_EOK;
    rt_base_t level;

    rk_isp_function_enter();

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(buf != RT_NULL);

    level = rt_hw_interrupt_disable();
    rt_memcpy(&dev->params, buf, sizeof(*buf));
    rt_hw_interrupt_enable(level);

    rk_isp_function_exit();

    return ret;
}

rt_err_t rk_isp_get_stat(struct rk_isp_dev *dev, struct rkisp_stats_buffer *buf)
{
    rt_err_t ret;
    rt_base_t level;

    rk_isp_function_enter();

    RT_ASSERT(dev != RT_NULL);
    RT_ASSERT(buf != RT_NULL);

    ret = rt_sem_take(dev->stats_sem, RT_WAITING_FOREVER);
    if (ret != RT_EOK)
    {
        ISP_INFO(dev, "Err: take stat sem failed!\n");
        return -RT_ERROR;
    }

    level = rt_hw_interrupt_disable();
    rt_memcpy(buf, &dev->stats, sizeof(*buf));
    rt_hw_interrupt_enable(level);

    rk_isp_function_exit();

    return ret;
}

#endif
