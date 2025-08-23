/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/

#include <rtthread.h>

#include "drv_display.h"
#include "lv_port_disp.h"

#include "rkadk_media_comm.h"
#include "rkadk_ui.h"

#include "rga.h"
#include "im2d.h"

/**********************
 *      MACROS
 **********************/
#define MB_BUFFER_COUNT     3

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

#define print(msg, ...)    rt_kprintf(stderr, msg, ##__VA_ARGS__);
#define err(msg, ...)  rt_kprintf("error: " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) rt_kprintf(msg "\n", ##__VA_ARGS__)
#define dbg(msg, ...)  {} //print(DBG_TAG ": " msg "\n", ##__VA_ARGS__)

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

struct disp_buffer
{
    unsigned long int size;
    int32_t fd;
    void *map;
};

struct disp_dev
{
    lv_color_t *fb;
    RKADK_MW_PTR ui_ptr;
    lv_disp_rot_t rot;
    MB_BLK draw_buf_blk;
    struct disp_buffer draw_buf;
    RKADK_UI_FRAME_INFO frame_info;
    MB_POOL buf_pool;
    uint32_t width, height;
    uint32_t mm_width, mm_height;
} disp_dev;

/**********************
 *  STATIC VARIABLES
 **********************/


/**********************
 *   STATIC FUNCTIONS
 **********************/
static int32_t get_disp_info(void)
{
    struct display_state *state;
    rt_err_t ret;

    rt_device_t lcd = rt_device_find("lcd");
    RT_ASSERT(lcd != RT_NULL);
    ret = rt_device_open(lcd, RT_DEVICE_OFLAG_RDWR);
    RT_ASSERT(ret == RT_EOK);

    state = (struct display_state *)lcd->user_data;
    disp_dev.width  = state->graphic_info.width;
    disp_dev.height = state->graphic_info.height;

    ret = rt_device_close(lcd);
    RT_ASSERT(ret == RT_EOK);

    disp_dev.mm_width = disp_dev.width;
    disp_dev.mm_height = disp_dev.height;

    info("rk_disp: %dx%d (%dmm X% dmm)", disp_dev.width, disp_dev.height,
         disp_dev.mm_width, disp_dev.mm_height);
}

static int32_t rk_disp_setup(void)
{
    int32_t ret = 0;

    RKADK_MPI_SYS_Init();

    RKADK_UI_ATTR_S ui_attr;

    memset(&ui_attr, 0, sizeof(ui_attr));

    ui_attr.u32VoChn = 2;
    ui_attr.u32VoDev = 0;
    ui_attr.u32VoLay = 0;
    ui_attr.u32DispFrmRt = 30;
    ui_attr.u32DispWidth = disp_dev.width;
    ui_attr.u32DispHeight = disp_dev.height;
    ui_attr.u32ImgWidth = disp_dev.width;
    ui_attr.u32ImgHeight = disp_dev.height;
    ui_attr.enUiVoFormat = VO_FORMAT_RGB888;
    ui_attr.enVoSpliceMode = SPLICE_MODE_RGA;
    ui_attr.u32Rotation = disp_dev.rot;

#ifdef PLATFORM_RV1106
    ui_attr.enUiVoIntfTye = DISPLAY_TYPE_DEFAULT;
#else
    ui_attr.enUiVoIntfTye = DISPLAY_TYPE_MIPI;
#endif

    ret = RKADK_UI_Create(&ui_attr, &disp_dev.ui_ptr);
    if (0 != ret)
    {
        err("%d: RKADK_DISP_Init failed(%d)", __LINE__, ret);
        return -1;
    }

    info("rk_disp: ui created successfullyl.");

    return 0;
}

static void rk_disp_teardown(void)
{
    if (NULL == disp_dev.ui_ptr)
        return;

    int32_t ret = RKADK_UI_Destroy(disp_dev.ui_ptr);
    if (0 != ret)
        err("%d: RKADK_DISP_Init failed(%d)", __LINE__, ret);

    info("rk_disp: ui destroyed successfully.");
}

static int32_t rk_disp_setup_buffers(void)
{
    int32_t ret;
    uint32_t i, size;
    void *blk = NULL;
    MB_POOL_CONFIG_S pool_cfg;
    RKADK_FORMAT_E format;

    if (LV_COLOR_DEPTH == 32)
    {
        format = RKADK_FMT_BGRA8888;
    }
    else if (LV_COLOR_DEPTH == 16)
    {
        format = RKADK_FMT_BGR565;
    }
    else
    {
        format = -1;
        err("%d: drm_flush rga not supported format\n", __LINE__);
        return -1;
    }

    size = disp_dev.width * disp_dev.height * (LV_COLOR_SIZE >> 3);

    memset(&pool_cfg, 0, sizeof(MB_POOL_CONFIG_S));
    pool_cfg.u64MBSize = size;
    pool_cfg.u32MBCnt  = MB_BUFFER_COUNT;
    pool_cfg.enRemapMode = MB_REMAP_MODE_CACHED;
    pool_cfg.enDmaType = MB_DMA_TYPE_NONE;
    pool_cfg.enAllocType = MB_ALLOC_TYPE_DMA;
    pool_cfg.bPreAlloc = RK_TRUE;
    disp_dev.buf_pool = RK_MPI_MB_CreatePool(&pool_cfg);
    if (disp_dev.buf_pool == -1)
    {
        err("%d: Create pool failed\n", __LINE__);
        return -1;
    }

    disp_dev.frame_info.Format = format;

    if ((disp_dev.rot == LV_DISP_ROT_90) ||
            (disp_dev.rot == LV_DISP_ROT_270))
    {
        disp_dev.frame_info.u32Width = disp_dev.height;
        disp_dev.frame_info.u32Height = disp_dev.width;
    }
    else
    {
        disp_dev.frame_info.u32Width = disp_dev.width;
        disp_dev.frame_info.u32Height = disp_dev.height;
    }

    return 0;
}

static void rk_disp_teardown_buffers(void)
{
    RK_MPI_MB_DestroyPool(disp_dev.buf_pool);
}

int32_t rk_disp_init(lv_disp_rot_t rotate_disp)
{
    int32_t ret;

    disp_dev.rot = rotate_disp;

    ret = get_disp_info();
    if (0 != ret)
    {
        err("%d: get display info failed", __LINE__);
        return -1;
    }

    ret = rk_disp_setup();
    if (0 != ret)
    {
        err("%d: rk_disp_setup failed", __LINE__);
        return -1;
    }

    ret = rk_disp_setup_buffers();
    if (0 != ret)
    {
        err("%d, Allocating display buffer failed", __LINE__);
        goto err;
    }

    return 0;
err:
    rk_disp_teardown();

    return -1;
}

void rk_disp_exit(void)
{
    rk_disp_teardown_buffers();
    rk_disp_teardown();
    RKADK_MPI_SYS_Exit();
}

void rk_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t x;
    int32_t y;
    void *dst_ptr;
    MB_BLK blk;
    lv_coord_t w = (area->x2 - area->x1 + 1);
    lv_coord_t h = (area->y2 - area->y1 + 1);

    dbg("x %d:%d y %d:%d w %d h %d", area->x1, area->x2,
        area->y1, area->y2, w, h);

    for (y = area->y1; y <= area->y2; y++)
    {
        lv_color_t *disp = disp_dev.fb + y * disp_drv->hor_res + area->x1;
        memcpy(disp, color_p, w * sizeof(lv_color_t));
        color_p += w;
    }

    if (!lv_disp_flush_is_last(disp_drv))
    {
        goto end;
    }

    blk = RK_MPI_MB_GetMB(disp_dev.buf_pool, disp_dev.draw_buf.size, RK_TRUE);
    if (!blk)
    {
        err("%d, GetMB failed\n", __LINE__);
        return;
    }
    dst_ptr = RK_MPI_MMZ_Handle2VirAddr(blk);

#if 0
    // CPU
    memcpy(dst_ptr, disp_dev.fb, disp_drv->hor_res * disp_drv->ver_res * (LV_COLOR_DEPTH >> 3));
#else
    // RGA
    rga_buffer_t src_img, dst_img;
    int format;
    int ret;

    if (LV_COLOR_DEPTH == 32)
        format = RK_FORMAT_BGRA_8888;
    else if (LV_COLOR_DEPTH == 16)
        format = RK_FORMAT_BGR_565;
    else
        err("Unsupport format\n");

    src_img = wrapbuffer_physicaladdr(disp_dev.fb, disp_drv->hor_res,
                                      disp_drv->ver_res, format);
    dst_img = wrapbuffer_physicaladdr(dst_ptr, disp_drv->hor_res,
                                      disp_drv->ver_res, format);
    ret = imcheck(src_img, dst_img, (im_rect) {}, (im_rect) {});
    if (ret != IM_STATUS_NOERROR)
    {
        err("%d, check error! %s\n", __LINE__, imStrError((IM_STATUS)ret));
        return;
    }
    ret = imcopy(src_img, dst_img);
    if (ret != IM_STATUS_SUCCESS)
        err("%d, running failed, %s\n", __LINE__, imStrError((IM_STATUS)ret));
#endif

    disp_dev.frame_info.pMblk = blk;
    RK_MPI_SYS_MmzFlushCache(blk, RK_FALSE);
    RKADK_UI_Update(disp_dev.ui_ptr, &(disp_dev.frame_info));
    RK_MPI_MB_ReleaseMB(blk);

end:
    lv_disp_flush_ready(disp_drv);
}

void rk_disp_get_sizes(lv_coord_t *width, lv_coord_t *height, uint32_t *dpi)
{
    if (width)
        *width = disp_dev.width;

    if (height)
        *height = disp_dev.height;

    if (dpi && disp_dev.mm_width)
        *dpi = DIV_ROUND_UP(disp_dev.width * 25400, disp_dev.mm_width * 1000);

    return;
}

static void rk_disp_rounder(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    lv_coord_t w;
    lv_coord_t h;

    area->x1 = area->x1 & ~1UL;
    area->y1 = area->y1 & ~1UL;

    w = lv_area_get_width(area);
    h = lv_area_get_height(area);

    w = (w + 1) & ~1UL;
    h = (h + 1) & ~1UL;

    lv_area_set_width(area, w);
    lv_area_set_height(area, h);
}

rt_err_t lv_port_disp_init(lv_disp_rot_t rotate_disp)
{
    lv_coord_t lcd_w, lcd_h;
    static lv_disp_draw_buf_t draw_buf_dsc;
    static lv_disp_drv_t disp_drv; /*Descriptor of a display driver*/

    /*-------------------------
     * Initialize your display
     * -----------------------*/
    int ret = rk_disp_init(rotate_disp);
    if (ret == -1)
    {
        printf("rk_disp_init is fail\n");
        return -RT_ERROR;
    }
    rk_disp_get_sizes(&lcd_w, &lcd_h, NULL);
    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    uint32_t size_in_px_cnt = lcd_w * lcd_h;
    MB_BLK blk;

    ret = RK_MPI_MMZ_Alloc(&blk, size_in_px_cnt * (LV_COLOR_SIZE >> 3),
                           RK_MMZ_ALLOC_CACHEABLE);
    if (0 != ret)
    {
        err("%d: alloc failed!", __LINE__);
        rk_disp_exit();
        return -RT_ERROR;
    }

    disp_dev.draw_buf_blk = blk;
    disp_dev.draw_buf.size = size_in_px_cnt * (LV_COLOR_SIZE >> 3);
    disp_dev.draw_buf.map = RK_MPI_MMZ_Handle2VirAddr(blk);
    disp_dev.draw_buf.fd = RK_MPI_MMZ_Handle2Fd(blk);

    lv_disp_draw_buf_init(&draw_buf_dsc, disp_dev.draw_buf.map, NULL,
                          size_in_px_cnt);

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/
    lv_disp_drv_init(&disp_drv);   /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    if (rotate_disp == LV_DISP_ROT_NONE)
    {
        disp_drv.hor_res = lcd_w;
        disp_drv.ver_res = lcd_h;
    }
    else if (rotate_disp == LV_DISP_ROT_180)
    {
        disp_drv.hor_res = lcd_w;
        disp_drv.ver_res = lcd_h;
    }
    else if (rotate_disp == LV_DISP_ROT_90)
    {
        disp_drv.hor_res = lcd_h;
        disp_drv.ver_res = lcd_w;
    }
    else if (rotate_disp == LV_DISP_ROT_270)
    {
        disp_drv.hor_res = lcd_h;
        disp_drv.ver_res = lcd_w;
    }

    /* Creat display buffer */
    disp_dev.fb = (lv_color_t *)rt_malloc(disp_drv.hor_res * disp_drv.ver_res * sizeof(lv_color_t));
    RT_ASSERT(disp_dev.fb != RT_NULL);

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = rk_disp_flush;
    disp_drv.rounder_cb = rk_disp_rounder;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);

    return RT_EOK;
}

rt_err_t lv_port_disp_deinit(void)
{
    if (NULL != disp_dev.draw_buf_blk)
    {
        RK_MPI_MMZ_Free(disp_dev.draw_buf_blk);
        disp_dev.draw_buf_blk = NULL;
    }

    if (disp_dev.fb)
    {
        rt_free(disp_dev.fb);
    }

    rk_disp_exit();

    return RT_EOK;
}

