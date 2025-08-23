/**
  * Copyright (c) 2023 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    modetest.c
  * @version V0.1
  * @brief   modetest for rockchip
  *
  * Change Logs:
  * Date           Author          Notes
  * 2023-11-11     Damon Ding      first implementation
  * 2024-05-11     Damon Ding      add config RT_USING_COMMON_MODETEST
  *
  ******************************************************************************
  */

#include <rtdevice.h>
#include <rthw.h>
#include <rtthread.h>

#ifdef RT_USING_COMMON_MODETEST
#include <getopt.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "display_pattern.h"
#include "dma.h"
#include "drv_display.h"
#include "drv_heap.h"
#include "hal_base.h"

static const uint32_t test_gamma_lut[256] =
{
    0x00000000, 0x00010101, 0x00020202, 0x00030303, 0x00040404, 0x00050505,
    0x00060606, 0x00070707, 0x00080808, 0x00090909, 0x000a0a0a, 0x000b0b0b,
    0x000c0c0c, 0x000d0d0d, 0x000e0e0e, 0x000f0f0f, 0x00101010, 0x00111111,
    0x00121212, 0x00131313, 0x00141414, 0x00151515, 0x00161616, 0x00171717,
    0x00181818, 0x00191919, 0x001a1a1a, 0x001b1b1b, 0x001c1c1c, 0x001d1d1d,
    0x001e1e1e, 0x001f1f1f, 0x00202020, 0x00212121, 0x00222222, 0x00232323,
    0x00242424, 0x00252525, 0x00262626, 0x00272727, 0x00282828, 0x00292929,
    0x002a2a2a, 0x002b2b2b, 0x002c2c2c, 0x002d2d2d, 0x002e2e2e, 0x002f2f2f,
    0x00303030, 0x00313131, 0x00323232, 0x00333333, 0x00343434, 0x00353535,
    0x00363636, 0x00373737, 0x00383838, 0x00393939, 0x003a3a3a, 0x003b3b3b,
    0x003c3c3c, 0x003d3d3d, 0x003e3e3e, 0x003f3f3f, 0x00404040, 0x00414141,
    0x00424242, 0x00434343, 0x00444444, 0x00454545, 0x00464646, 0x00474747,
    0x00484848, 0x00494949, 0x004a4a4a, 0x004b4b4b, 0x004c4c4c, 0x004d4d4d,
    0x004e4e4e, 0x004f4f4f, 0x00505050, 0x00515151, 0x00525252, 0x00535353,
    0x00545454, 0x00555555, 0x00565656, 0x00575757, 0x00585858, 0x00595959,
    0x005a5a5a, 0x005b5b5b, 0x005c5c5c, 0x005d5d5d, 0x005e5e5e, 0x005f5f5f,
    0x00606060, 0x00616161, 0x00626262, 0x00636363, 0x00646464, 0x00656565,
    0x00666666, 0x00676767, 0x00686868, 0x00696969, 0x006a6a6a, 0x006b6b6b,
    0x006c6c6c, 0x006d6d6d, 0x006e6e6e, 0x006f6f6f, 0x00707070, 0x00717171,
    0x00727272, 0x00737373, 0x00747474, 0x00757575, 0x00767676, 0x00777777,
    0x00787878, 0x00797979, 0x007a7a7a, 0x007b7b7b, 0x007c7c7c, 0x007d7d7d,
    0x007e7e7e, 0x007f7f7f, 0x00808080, 0x00818181, 0x00828282, 0x00838383,
    0x00848484, 0x00858585, 0x00868686, 0x00878787, 0x00888888, 0x00898989,
    0x008a8a8a, 0x008b8b8b, 0x008c8c8c, 0x008d8d8d, 0x008e8e8e, 0x008f8f8f,
    0x00909090, 0x00919191, 0x00929292, 0x00939393, 0x00949494, 0x00959595,
    0x00969696, 0x00979797, 0x00989898, 0x00999999, 0x009a9a9a, 0x009b9b9b,
    0x009c9c9c, 0x009d9d9d, 0x009e9e9e, 0x009f9f9f, 0x00a0a0a0, 0x00a1a1a1,
    0x00a2a2a2, 0x00a3a3a3, 0x00a4a4a4, 0x00a5a5a5, 0x00a6a6a6, 0x00a7a7a7,
    0x00a8a8a8, 0x00a9a9a9, 0x00aaaaaa, 0x00ababab, 0x00acacac, 0x00adadad,
    0x00aeaeae, 0x00afafaf, 0x00b0b0b0, 0x00b1b1b1, 0x00b2b2b2, 0x00b3b3b3,
    0x00b4b4b4, 0x00b5b5b5, 0x00b6b6b6, 0x00b7b7b7, 0x00b8b8b8, 0x00b9b9b9,
    0x00bababa, 0x00bbbbbb, 0x00bcbcbc, 0x00bdbdbd, 0x00bebebe, 0x00bfbfbf,
    0x00c0c0c0, 0x00c1c1c1, 0x00c2c2c2, 0x00c3c3c3, 0x00c4c4c4, 0x00c5c5c5,
    0x00c6c6c6, 0x00c7c7c7, 0x00c8c8c8, 0x00c9c9c9, 0x00cacaca, 0x00cbcbcb,
    0x00cccccc, 0x00cdcdcd, 0x00cecece, 0x00cfcfcf, 0x00d0d0d0, 0x00d1d1d1,
    0x00d2d2d2, 0x00d3d3d3, 0x00d4d4d4, 0x00d5d5d5, 0x00d6d6d6, 0x00d7d7d7,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

#define MAX_PLANE_CNT   4

#define SYNC_COMMIT_TIMES  50

#define ALIGN_UP(x, a)  (((x) + (a) - 1) & ~((a) - 1))

struct plane_arg
{
    uint32_t plane_id; /* the id of plane to use */
    uint32_t crtc_id; /* the id of CRTC to bind to */
    uint32_t w, h;
    uint32_t crtc_w, crtc_h;
    uint32_t stride;
    uint32_t zpos;
    uint32_t format;
    int32_t rotation;
    int32_t x, y;
    bool has_position;
    bool test_alpha;
    double scale;
    char format_str[5]; /* need to leave room for terminating \0 */
};

static struct rt_device *g_display_dev;
static struct rt_device *g_backlight_dev;

uint32_t rtt_framebuffer_yrgb[MAX_PLANE_CNT] = {0};
uint32_t rtt_framebuffer_uv[MAX_PLANE_CNT] = {0};

static enum util_fill_pattern pattern = UTIL_PATTERN_SMPTE;
static int solid_value;

static uint32_t framebuffer_alloc(rt_size_t size)
{
#if defined(RT_USING_LARGE_HEAP)
    return (uint32_t)rt_dma_malloc_large(size);
#else
    return (uint32_t)rt_dma_malloc(size);;
#endif
}

static void framebuffer_free(void *base)
{
#if defined(RT_USING_LARGE_HEAP)
    rt_dma_free_large(base);
#else
    rt_dma_free(base);
#endif
}

static uint32_t get_stride(uint32_t format, uint32_t width)
{
    uint32_t stride;

    switch (format)
    {
    case RTGRAPHIC_PIXEL_FORMAT_ARGB888:
    case RTGRAPHIC_PIXEL_FORMAT_ABGR888:
        stride = width * 4;
        break;
    case RTGRAPHIC_PIXEL_FORMAT_RGB888:
    case RTGRAPHIC_PIXEL_FORMAT_BGR888:
        stride = width * 3;
        break;
    case RTGRAPHIC_PIXEL_FORMAT_BGR565:
    case RTGRAPHIC_PIXEL_FORMAT_RGB565:
        stride = width * 2;
        break;
    case RTGRAPHIC_PIXEL_FORMAT_YUV420:
    case RTGRAPHIC_PIXEL_FORMAT_YUV422:
    case RTGRAPHIC_PIXEL_FORMAT_YUV444:
        stride = width;
        break;
    default:
        rt_kprintf("Unsupported format: %d\n", format);
        stride = width * 4;
        break;
    }

    /* align up to 4 byte since vopl requires it */
    return ALIGN_UP(stride, 4);
}

static void usage(void)
{
    rt_kprintf("modetest [-p]\n\n");
    rt_kprintf("\t-p <plane_id>:<w>x<h>:<crtc_w>x<crtc_h>[+<x>+<y>][#<zpos>][@<format>] \tset plane\n");
    rt_kprintf("\t-f <pattern> \tspecify fill pattern\n");
    rt_kprintf("\t-a \ttest alpha\n");
    rt_kprintf("\t-g \ttest gamma\n");
    rt_kprintf("\t-v \ttest sync commit mode\n");
}

static uint32_t parse_plane(struct plane_arg *plane, char *p)
{
    char *end;

    plane->plane_id = strtoul(p, &end, 10);
    if (*end != ':')
        return -RT_EINVAL;

    p = end + 1;
    plane->w = strtoul(p, &end, 10);
    if (*end != 'x')
        return -RT_EINVAL;

    p = end + 1;
    plane->h = strtoul(p, &end, 10);
    if (*end == ':')
    {
        p = end + 1;
        plane->crtc_w = strtoul(p, &end, 10);
        if (*end != 'x')
        {
            fprintf(stderr, "invalid crtc_w/h argument\n");
            return -RT_EINVAL;
        }

        p = end + 1;
        plane->crtc_h = strtoul(p, &end, 10);
    }
    else
    {
        plane->crtc_w = plane->w;
        plane->crtc_h = plane->h;
    }

    if (*end == '+' || *end == '-')
    {
        plane->x = strtol(end, &end, 10);
        if (*end != '+' && *end != '-')
            return -RT_EINVAL;
        plane->y = strtol(end, &end, 10);

        plane->has_position = true;
    }

    if (*end == '#')
    {
        plane->zpos = strtol(end + 1, &end, 10);
    }

    if (*end == '@')
    {
        strncpy(plane->format_str, end + 1, 4);
        plane->format_str[4] = '\0';
    }
    else
    {
        strcpy(plane->format_str, "AR24");
        rt_kprintf("failed to find fromat_str, use AR24 as default\n");
    }
    plane->format = util_format_fourcc(plane->format_str);
    if (plane->format == 0)
    {
        rt_kprintf("unknown format %s, use AR24 as default\n", plane->format_str);
        plane->format = RTGRAPHIC_PIXEL_FORMAT_ARGB888;
    }

    return 0;
}

static void set_planes(struct plane_arg *plane,
                       struct CRTC_WIN_STATE *win_config,
                       struct rt_device_graphic_info *graphic_info,
                       uint32_t plane_count)
{
    int i;

    for (i = 0; i < plane_count; i++)
    {
        win_config[i].winEn = true;
        win_config[i].winUpdate = true;
        win_config[i].winId = plane[i].plane_id;
        win_config[i].zpos = plane[i].zpos;
        win_config[i].format = plane[i].format;
        win_config[i].yrgbLength = 0;
        win_config[i].cbcrLength = 0;
        win_config[i].xVir = plane[i].w;

        win_config[i].srcX = 0;
        win_config[i].srcY = 0;
        win_config[i].srcW = plane[i].w;
        win_config[i].srcH = plane[i].h;

        win_config[i].crtcW = plane[i].crtc_w;
        win_config[i].crtcH = plane[i].crtc_h;

        if (!plane[i].has_position)
        {
            win_config[i].crtcX = (graphic_info->width - win_config[i].crtcW) >> 1;
            win_config[i].crtcY = (graphic_info->height - win_config[i].crtcH) >> 1;
        }
        else
        {
            win_config[i].crtcX = plane[i].x;
            win_config[i].crtcY = plane[i].y;
        }

        win_config[i].alphaEn = 0;
    }
}

static void clear_planes(struct rt_device *dev,
                         struct CRTC_WIN_STATE *win_config,
                         struct rt_device_graphic_info *graphic_info,
                         uint32_t plane_count)
{
    rt_err_t ret;
    int i;

    for (i = 0; i < plane_count; i++)
    {
        win_config[i].winEn = false;
        win_config[i].winUpdate = true;
        ret = rt_device_control(g_display_dev,
                                RK_DISPLAY_CTRL_SET_PLANE, &win_config[i]);
        RT_ASSERT(ret == RT_EOK);
    }

    ret = rt_device_control(g_display_dev, RK_DISPLAY_CTRL_COMMIT, NULL);
    RT_ASSERT(ret == RT_EOK);
    rt_thread_mdelay(500);
}

static bool is_yuv_format(uint32_t format)
{
    switch (format)
    {
    case RTGRAPHIC_PIXEL_FORMAT_YUV420:
    case RTGRAPHIC_PIXEL_FORMAT_YUV422:
    case RTGRAPHIC_PIXEL_FORMAT_YUV444:
        return true;
    default:
        return false;
    }
}

static int set_buffers(struct plane_arg *plane,
                       struct CRTC_WIN_STATE *win_config,
                       struct rt_device_graphic_info *graphic_info,
                       uint32_t plane_count)
{
    uint32_t fb_length;
    bool is_yuv;
    int i;

    for (i = 0; i < plane_count; i++)
    {
        uint32_t pitches[4] = {0};
        void *planes[3] = {0};

        is_yuv = is_yuv_format(win_config[i].format);
        pitches[0] = get_stride(win_config[i].format, win_config[i].srcW);
        fb_length = pitches[0] * win_config[i].srcH;
        rtt_framebuffer_yrgb[i] = framebuffer_alloc(fb_length);
        if (rtt_framebuffer_yrgb[i] == RT_NULL)
        {
            rt_kprintf("Failed to malloc memory: 0x%x\n", fb_length);
            return -RT_ENOMEM;
        }
        rt_memset((void *)rtt_framebuffer_yrgb[i], 0, fb_length);
        planes[0] = (void *)rtt_framebuffer_yrgb[i];

        if (is_yuv)
        {
            rtt_framebuffer_uv[i] = framebuffer_alloc(fb_length);
            if (rtt_framebuffer_uv[i] == RT_NULL)
            {
                rt_kprintf("Failed to malloc memory: 0x%x\n", fb_length);
                return -RT_ENOMEM;
            }
            rt_memset((void *)rtt_framebuffer_yrgb[i], 0, fb_length);
            planes[1] = (void *)rtt_framebuffer_uv[i];
        }

        util_fill_pattern(win_config[i].format, pattern, planes, win_config[i].srcW, win_config[i].srcH, pitches[0], solid_value);
        HAL_DCACHE_CleanByRange((uint32_t)rtt_framebuffer_yrgb[i], fb_length);
        if (is_yuv)
            HAL_DCACHE_CleanByRange((uint32_t)rtt_framebuffer_uv[i], fb_length);

        win_config[i].yrgbAddr = (uint32_t)rtt_framebuffer_yrgb[i];
        win_config[i].cbcrAddr = (uint32_t)rtt_framebuffer_uv[i];
    }

    return 0;
}

static void clear_buffers(struct plane_arg *plane,
                          struct CRTC_WIN_STATE *win_config,
                          struct rt_device_graphic_info *graphic_info,
                          uint32_t plane_count)
{
    bool is_yuv;
    int i;

    for (i = 0; i < plane_count; i++)
    {
        is_yuv = is_yuv_format(win_config[i].format);
        framebuffer_free((void *)rtt_framebuffer_yrgb[i]);
        rtt_framebuffer_yrgb[i] = 0;
        if (is_yuv)
        {
            framebuffer_free((void *)rtt_framebuffer_uv[i]);
            rtt_framebuffer_uv[i] = 0;
        }
    }
}

static void parse_fill_patterns(char *arg)
{
    if (strstr(arg, "0x"))
    {
        pattern = util_pattern_enum("solid");
        solid_value = strtoul(arg, NULL, 16);
    }
    else
    {
        pattern = util_pattern_enum(arg);
    }

    rt_kprintf("pattern: %d, solid value: 0x%x\n", pattern, solid_value);
}

static int modetest(int argc, char **argv)
{
    rt_err_t ret = RT_EOK;
    struct rt_device_graphic_info *graphic_info;
    struct CRTC_WIN_STATE *win_config = NULL;
    struct plane_arg *plane_args = NULL;
    struct crtc_lut_state lut_state = { 0 };
    uint32_t plane_count = 0;
    uint32_t sync_commit_times = 0;
    bool gamma_enable = false;
    bool sync_commit_enable = false;
    int opt, i, j;

    if (argc == 1)
    {
        usage();
        return 0;
    }

    optind = 0;
    while ((opt = getopt(argc, argv, "f:p:agv")) != -1)
    {
        switch (opt)
        {
        case 'f':
            parse_fill_patterns(optarg);
            break;
        case 'p':
            plane_args = rt_realloc(plane_args, (plane_count + 1) * sizeof(struct plane_arg));
            if (!plane_args)
            {
                rt_kprintf("memory allocation failed\n");
                return -RT_ENOMEM;
            }
            rt_memset(&plane_args[plane_count], 0, sizeof(*plane_args));

            if (parse_plane(&plane_args[plane_count], optarg) < 0)
                usage();

            plane_count++;
            break;
        case 'a':
            if (plane_count)
                plane_args[plane_count - 1].test_alpha = true;
            break;
        case 'g':
            gamma_enable = true;
            break;
        case 'v':
            sync_commit_enable = true;
            break;
        default:
            rt_kprintf("Unknown option: %c\n", opt);
            usage();
            return 0;
        }
    }

    rt_kprintf("Enter modetest! \n");

    g_display_dev = rt_device_find("lcd");
    RT_ASSERT(g_display_dev != RT_NULL);

    ret = rt_device_open(g_display_dev, RT_DEVICE_FLAG_RDWR);
    RT_ASSERT(ret == RT_EOK);

    g_backlight_dev = rt_device_find("backlight");
    if (g_backlight_dev)
    {
        int brightness = 100;
        rt_kprintf("Backlight power on\n");
        rt_device_control(g_backlight_dev, RTGRAPHIC_CTRL_POWERON, NULL);
        rt_device_control(g_backlight_dev, RTGRAPHIC_CTRL_RECT_UPDATE,
                          &brightness);
    }
    else
    {
        rt_kprintf("Failed to find backlight dev\n");
    }

    ret = rt_device_control(g_display_dev, RK_DISPLAY_CTRL_AP_COP_MODE, (uint8_t *)0);
    ret = rt_device_control(g_display_dev, RTGRAPHIC_CTRL_POWERON, NULL);
    RT_ASSERT(ret == RT_EOK);

    graphic_info = (struct rt_device_graphic_info *)rt_calloc(1, sizeof(struct rt_device_graphic_info));
    RT_ASSERT(graphic_info != RT_NULL);

    ret = rt_device_control(g_display_dev,
                            RTGRAPHIC_CTRL_GET_INFO, (void *)graphic_info);
    RT_ASSERT(ret == RT_EOK);

    win_config = (struct CRTC_WIN_STATE *)rt_calloc(plane_count, sizeof(struct CRTC_WIN_STATE));
    RT_ASSERT(win_config != RT_NULL);

    set_planes(plane_args, win_config, graphic_info, plane_count);

    clear_planes(g_display_dev, win_config, graphic_info, plane_count);

    ret = set_buffers(plane_args, win_config, graphic_info, plane_count);
    if (ret)
        return ret;

    for (i = 0; i < plane_count; i++)
    {
        win_config[i].winEn = true;

        ret = rt_device_control(g_display_dev,
                                RK_DISPLAY_CTRL_SET_PLANE, &win_config[i]);
        RT_ASSERT(ret == RT_EOK);

        rt_kprintf("Colorbar test [%dx%d->%dx%d@%dx%d]@%s in win%d\n",
                   win_config[i].srcW, win_config[i].srcH, win_config[i].crtcW, win_config[i].crtcH,
                   win_config[i].crtcX, win_config[i].crtcY, plane_args[i].format_str, win_config[i].winId);
    }

    ret = rt_device_control(g_display_dev, RK_DISPLAY_CTRL_COMMIT, NULL);
    RT_ASSERT(ret == RT_EOK);

    rt_thread_mdelay(200);

    for (i = 0; i < plane_count; i++)
    {
        if (plane_args[i].test_alpha)
        {
            rt_kprintf("Alpha test in win%d\n", win_config[i].winId);
            win_config[i].alphaEn = true;
            for (j = 0; j < 0xff; j += 5)
            {
                win_config[i].globalAlphaValue = j;
                win_config[i].alphaMode = VOP_ALPHA_MODE_USER_DEFINED;
                win_config[i].alphaPreMul = VOP_NON_PREMULT_ALPHA;
                ret = rt_device_control(g_display_dev,
                                        RK_DISPLAY_CTRL_SET_PLANE, &win_config[i]);
                RT_ASSERT(ret == RT_EOK);

                ret = rt_device_control(g_display_dev, RK_DISPLAY_CTRL_COMMIT, NULL);
                RT_ASSERT(ret == RT_EOK);

                rt_thread_mdelay(50);
            }
            win_config[i].globalAlphaValue = j;
            win_config[i].alphaEn = false;
        }
    }

    if (gamma_enable)
    {
        lut_state.lut = (uint32_t *)test_gamma_lut;
        lut_state.lut_size = sizeof(test_gamma_lut) / sizeof(test_gamma_lut[0]);
        ret = rt_device_control(g_display_dev, RK_DISPLAY_CTRL_LOAD_LUT, &lut_state);
        RT_ASSERT(ret == RT_EOK);
    }

    if (sync_commit_enable)
    {
        while (sync_commit_times++ <= SYNC_COMMIT_TIMES)
        {
            for (i = 0; i < plane_count; i++)
            {
                win_config[i].winEn = false;

                ret = rt_device_control(g_display_dev,
                                        RK_DISPLAY_CTRL_SET_PLANE, &win_config[i]);
                RT_ASSERT(ret == RT_EOK);
            }

            ret = rt_device_control(g_display_dev,
                                    RK_DISPLAY_CTRL_COMMIT, (void *)DISPLAY_COMMIT_BLOCK);
            RT_ASSERT(ret == RT_EOK);

            for (i = 0; i < plane_count; i++)
            {
                win_config[i].winEn = true;

                ret = rt_device_control(g_display_dev,
                                        RK_DISPLAY_CTRL_SET_PLANE, &win_config[i]);
                RT_ASSERT(ret == RT_EOK);
            }

            ret = rt_device_control(g_display_dev,
                                    RK_DISPLAY_CTRL_COMMIT, (void *)DISPLAY_COMMIT_BLOCK);
            RT_ASSERT(ret == RT_EOK);
        }
    }

    rt_thread_mdelay(1000);

    if (gamma_enable)
    {
        ret = rt_device_control(g_display_dev, RK_DISPLAY_CTRL_DISABLE_LUT, &lut_state);
        RT_ASSERT(ret == RT_EOK);
    }

    for (i = 0; i < plane_count; i++)
    {
        win_config[i].winEn = false;

        ret = rt_device_control(g_display_dev,
                                RK_DISPLAY_CTRL_SET_PLANE, &win_config[i]);
        RT_ASSERT(ret == RT_EOK);
    }

    ret = rt_device_control(g_display_dev, RK_DISPLAY_CTRL_COMMIT, NULL);
    RT_ASSERT(ret == RT_EOK);

    clear_buffers(plane_args, win_config, graphic_info, plane_count);

    if (g_backlight_dev)
    {
        int brightness = 0;
        rt_kprintf("Backlight power off\n");
        ret = rt_device_control(g_backlight_dev, RTGRAPHIC_CTRL_POWEROFF, NULL);
        RT_ASSERT(ret == RT_EOK);
        rt_device_control(g_backlight_dev, RTGRAPHIC_CTRL_RECT_UPDATE,
                          &brightness);
    }
    else
    {
        ret = rt_device_control(g_display_dev, RTGRAPHIC_CTRL_POWEROFF, NULL);
        RT_ASSERT(ret == RT_EOK);
    }

    rt_free(graphic_info);
    rt_free(win_config);
    rt_free(plane_args);

    rt_kprintf("Exit display test!\n");

    return ret;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
MSH_CMD_EXPORT(modetest, display drive mode test. e.g: modetest);
#endif
#endif
