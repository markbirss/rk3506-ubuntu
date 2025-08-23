/**
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 ******************************************************************************
 * @file    rga_test.c
 * @author  cerf yu
 * @version V0.1
 * @date    23-Jul-2024
 * @brief   rga test
 *
 ******************************************************************************
 */
#include <rtdevice.h>
#include <rtthread.h>
#include <rthw.h>

#include <soc.h>

#include <stdio.h>
#include <string.h>

#include "rga.h"
#include "RgaUtils.h"
#include "im2d.h"

void rga_copy_test(void *arg)
{
    int ret = 0;
    int i;
    char *va;
    int src_width, src_height, src_format;
    int dst_width, dst_height, dst_format;
    char *src_buf, *dst_buf;
    int src_buf_size, dst_buf_size;

    rga_buffer_t src_img, dst_img;

    rt_memset(&src_img, 0, sizeof(src_img));
    rt_memset(&dst_img, 0, sizeof(dst_img));

    src_width = 240;
    src_height = 240;
    src_format = RK_FORMAT_RGBA_8888;

    dst_width = 240;
    dst_height = 240;
    dst_format = RK_FORMAT_RGBA_8888;

    src_buf_size = src_width * src_height * get_bpp_from_format(src_format);
    dst_buf_size = dst_width * dst_height * get_bpp_from_format(dst_format);

    src_buf = (char *)rt_malloc(src_buf_size);
    if (src_buf == NULL)
    {
        rt_kprintf("failed to alloc src_buf\n");
        return;
    }

    dst_buf = (char *)rt_malloc(dst_buf_size);
    if (dst_buf == NULL)
    {
        rt_kprintf("failed to alloc dst_buf\n");
        rt_free(src_buf);
        return;
    }

    rt_memset(src_buf, 0x12, src_buf_size);
    rt_memset(dst_buf, 0x80, dst_buf_size);

    rt_kprintf("original src_buf dump:\n");
    for (i = 0; i < 4; i++)
    {
        va = src_buf + i * 4 * sizeof(*va);
        rt_kprintf("i = %d, [%p] [0x%x, 0x%x, 0x%x, 0x%x]\n", i, va, va[0], va[1], va[2], va[3]);
    }

    rt_kprintf("original dst_buf dump:\n");
    for (i = 0; i < 4; i++)
    {
        va = dst_buf + i * 4 * sizeof(*va);
        rt_kprintf("i = %d, [%p] [0x%x, 0x%x, 0x%x, 0x%x]\n", i, va, va[0], va[1], va[2], va[3]);
    }

    src_img = wrapbuffer_physicaladdr(src_buf, src_width, src_height, src_format);
    dst_img = wrapbuffer_physicaladdr(dst_buf, dst_width, dst_height, dst_format);

    /*
     * Copy the src image to the dst buffer.
        --------------        --------------
        |            |        |            |
        |  src_image |   =>   |  dst_image |
        |            |        |            |
        --------------        --------------
     */

    ret = imcheck(src_img, dst_img, (im_rect) {}, (im_rect) {});
    if (IM_STATUS_NOERROR != ret)
    {
        rt_kprintf("%d, check error! %s\n", __LINE__, imStrError((IM_STATUS)ret));
        goto release_buffer;
    }

    ret = imcopy(src_img, dst_img);
    if (ret == IM_STATUS_SUCCESS)
    {
        rt_kprintf("%s running success!\n", __func__);
    }
    else
    {
        rt_kprintf("%s running failed, %s\n", __func__, imStrError((IM_STATUS)ret));
        goto release_buffer;
    }

    rt_kprintf("dst_buf dump:\n");
    for (i = 0; i < 4; i++)
    {
        va = dst_buf + i * 4 * sizeof(*va);
        rt_kprintf("i = %d, [%p] [0x%x, 0x%x, 0x%x, 0x%x]\n", i, va, va[0], va[1], va[2], va[3]);
    }

release_buffer:
    if (src_buf)
        rt_free(src_buf);
    if (dst_buf)
        rt_free(dst_buf);

    return;
}

int rga_test(void)
{
    rt_thread_t tid = RT_NULL;

    tid = rt_thread_create("rga_test", rga_copy_test, NULL, 8192, 16, 10);

    if (tid)
        rt_thread_startup(tid);

    return 0;
}
MSH_CMD_EXPORT(rga_test, rga_test)
