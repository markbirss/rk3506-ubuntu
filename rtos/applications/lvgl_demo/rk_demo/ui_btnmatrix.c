
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
*/
#include <rtthread.h>
#include "lvgl.h"

#include "main.h"
#include "ui_resource.h"
#include "ui_btnmatrix.h"

void common_draw(lv_obj_t *parent, struct btn_desc *desc)
{
    lv_obj_t *obj, *img, *label;

    obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_center(obj);

    if (desc->img)
    {
        img = lv_img_create(obj);
        lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_img_set_src(img, desc->img);
    }

    if (desc->text)
    {
        label = lv_label_create(obj);
        lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_style(label, &style_txt_m, LV_PART_MAIN);
        lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
        lv_label_set_text(label, desc->text);
    }
}

static int area_empty(lv_coord_t *map, lv_coord_t w, lv_coord_t h,
                      lv_coord_t stride)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (map[i * stride + j])
                return 0;
        }
    }

    return 1;
}

static void area_fill(lv_coord_t *map, lv_coord_t w, lv_coord_t h,
                      lv_coord_t stride)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            map[i * stride + j] = 1;
        }
    }
}

lv_obj_t *ui_btnmatrix_create(lv_obj_t *parent, struct btn_matrix_desc *desc)
{
    lv_obj_t *main;
    const lv_coord_t *tmp;
    lv_coord_t cols = 0;
    lv_coord_t rows = 0;
    lv_coord_t x, y, w, h;
    lv_coord_t *map;
    int found;

    tmp = desc->col_dsc;
    while (*tmp != LV_GRID_TEMPLATE_LAST)
    {
        cols++;
        tmp++;
        if (cols > 100)
        {
            printf("invalid col_dsc\n");
            return NULL;
        }
    }

    tmp = desc->row_dsc;
    while (*tmp != LV_GRID_TEMPLATE_LAST)
    {
        rows++;
        tmp++;
        if (rows > 100)
        {
            printf("invalid row_dsc\n");
            return NULL;
        }
    }

    map = calloc(cols * rows, sizeof(lv_coord_t));

    main = lv_obj_create(parent);
    lv_obj_remove_style_all(main);
    lv_obj_set_style_grid_column_dsc_array(main, desc->col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(main, desc->row_dsc, 0);
    lv_obj_set_layout(main, LV_LAYOUT_GRID);
    lv_obj_set_size(main, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(main, desc->pad, LV_PART_MAIN);
    lv_obj_set_style_pad_gap(main, desc->gap, LV_PART_MAIN);

    for (int i = 0; i < desc->btn_cnt; i++)
    {
        lv_obj_t *obj;

        if (desc->desc[i].w > cols || desc->desc[i].h > rows)
        {
            printf("item too large [%dx%d] > [%dx%d]\n",
                   desc->desc[i].w, desc->desc[i].h, cols, rows);
            continue;
        }

        if (desc->desc[i].w || desc->desc[i].h)
        {
            found = 0;
            w = desc->desc[i].w;
            h = desc->desc[i].h;
            for (int r = 0; r <= (rows - h); r++)
            {
                for (int c = 0; c <= (cols - w); c++)
                {
                    if (area_empty(&map[r * cols + c], w, h, cols))
                    {
                        x = c;
                        y = r;
                        found = 1;
                        break;
                    }
                }
                if (found)
                    break;
            }
            if (!found)
            {
                printf("No empty area\n");
                continue;
            }
            area_fill(&map[y * cols + x], w, h, cols);
        }
        else
        {
            x = desc->desc[i].area.x1;
            y = desc->desc[i].area.y1;
            w = desc->desc[i].area.x2 - desc->desc[i].area.x1;
            h = desc->desc[i].area.y2 - desc->desc[i].area.y1;
        }

        if (desc->desc[i].create)
        {
            obj = desc->desc[i].create(main);
        }
        else
        {
            obj = lv_obj_create(main);
            lv_obj_remove_style_all(obj);
            lv_obj_set_style_bg_color(obj, MAIN_COLOR, LV_PART_MAIN);
            lv_obj_set_style_bg_color(obj, MAIN_COLOR_PRESS,
                                      LV_PART_MAIN | LV_STATE_PRESSED);
            lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
            lv_obj_set_style_shadow_width(obj, 1, LV_PART_MAIN);
            lv_obj_set_style_shadow_spread(obj, 2, LV_PART_MAIN);
            lv_obj_set_style_shadow_color(obj, lv_color_black(), LV_PART_MAIN);
            lv_obj_set_style_shadow_opa(obj, LV_OPA_20, LV_PART_MAIN);
            lv_obj_set_style_radius(obj, 16, LV_PART_MAIN);
        }
        lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, x, w,
                             LV_GRID_ALIGN_STRETCH, y, h);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_refr_size(obj);
        lv_obj_refr_pos(obj);
        if (desc->desc[i].cb)
            lv_obj_add_event_cb(obj, desc->desc[i].cb, LV_EVENT_CLICKED,
                                desc->desc[i].user_data);

        *desc->desc[i].obj = obj;

        if (desc->desc[i].draw)
            desc->desc[i].draw(obj, &desc->desc[i]);
    }

    free(map);

    return main;
}

