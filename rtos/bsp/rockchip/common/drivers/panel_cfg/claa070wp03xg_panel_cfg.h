/**
  * Copyright (c) 2024 Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    claa070wp03xg_panel_cfg.h
  * @version V0.1
  * @brief   display panel config for claa070wp03xg panel
  *
  *
  ******************************************************************************
  */

#ifndef __CLAA070WP03XG_PANEL_CFG_H__
#define __CLAA070WP03XG_PANEL_CFG_H__

#define RT_HW_LCD_XRES                800     /* LCD PIXEL WIDTH             */
#define RT_HW_LCD_YRES                1280      /* LCD PIXEL HEIGHT            */
#define RT_HW_LCD_PIXEL_CLOCK         25000    /* Pixel clock Khz             */
#define RT_HW_LCD_LEFT_MARGIN         24      /* Horizontal front porch      */
#define RT_HW_LCD_RIGHT_MARGIN        24      /* Horizontal back porch       */
#define RT_HW_LCD_LOWER_MARGIN        4       /* Vertical front porch        */
#define RT_HW_LCD_UPPER_MARGIN        2       /* Vertical back porch         */
#define RT_HW_LCD_HSYNC_LEN           16       /* Horizontal synchronization  */
#define RT_HW_LCD_VSYNC_LEN           2        /* Vertical synchronization    */

#define RT_HW_LCD_CONN_TYPE           RK_DISPLAY_CONNECTOR_RGB
#define RT_HW_LCD_BUS_FORMAT          MEDIA_BUS_FMT_RGB888_1X24
#define RT_HW_LCD_VMODE_FLAG          0
#define RT_HW_LCD_INIT_CMD_TYPE       CMD_TYPE_DEFAULT
#define RT_HW_LCD_DISPLAY_MODE        DISPLAY_VIDEO_MODE
#define RT_HW_LCD_AREA_DISPLAY        DISABLE_AREA_DISPLAY

#define RT_HW_LCD_XACT_ALIGN          1
#define RT_HW_LCD_YACT_ALIGN          1
#define RT_HW_LCD_XPOS_ALIGN          1
#define RT_HW_LCD_YPOS_ALIGN          1

#define RT_HW_LCD_RESET_DELAY_MS      10

const static struct rockchip_cmd cmd_on[] =
{
};

const static struct rockchip_cmd cmd_off[] =
{
};

#endif /* __CLAA070WP03XG_PANEL_CFG_H__ */
