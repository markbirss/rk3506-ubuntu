#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <lvgl/lvgl.h>

struct tile_info
{
    int len;
    int items;
};

struct tile_map
{
    uint8_t *data;
    int w;
    int h;
};

struct tile_layout
{
    struct tile_map map;
    lv_obj_t *layout;
    int w;
    int h;
    int base_size;
    int cols;
    int rows;
    int32_t *col_dsc;
    int32_t *row_dsc;
};

#define MAP_DATA(m, x, y)   (m->data[m->w * y + ((x & ~7) >> 3)])
#define DATA_MASK(x)        ((1 << (x & 7)))
#define UNMARK_MAP(m, x, y) \
    (MAP_DATA(m, x, y) &= ~DATA_MASK(x))
#define MARK_MAP(m, x, y) \
    (MAP_DATA(m, x, y) |=  DATA_MASK(x))
#define READ_MAP(m, x, y) \
    (MAP_DATA(m, x, y) &   DATA_MASK(x))
#define DEPART_AREA(a)  int x, y, w, h;\
    x = a->x1; \
    y = a->y1; \
    w = a->x2 - a->x1 + 1; \
    h = a->y2 - a->y1 + 1;

static void unmark_area(struct tile_map *map, lv_area_t *area)
{
    DEPART_AREA(area);

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            UNMARK_MAP(map, x + j, y + i);
}

static void mark_area(struct tile_map *map, lv_area_t *area)
{
    DEPART_AREA(area);

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            MARK_MAP(map, x + j, y + i);
}

static int is_free_area(struct tile_map *map, lv_area_t *area)
{
    struct tile_layout *tl = (struct tile_layout *)map;
    DEPART_AREA(area);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (((x + j) == tl->cols) || (y + i) == tl->rows)
                return 0;

            if (READ_MAP(map, x + j, y + i))
                return 0;
        }
    }

    return 1;
}

static void print_map(struct tile_map *map)
{
    for (int i = 0; i < map->h; i++)
    {
        printf("[");
        for (int j = 0; j < map->w * 8; j++)
            printf(" %c", READ_MAP(map, j, i) ? 'X' : '_');
        printf(" ]\n");
    }
    printf("\n");
}

void *tile_layout_create(lv_obj_t *parent, int w, int h, int base_size)
{
    struct tile_layout *tl;

    tl = calloc(1, sizeof(struct tile_layout));

    if (!w)
        w = lv_obj_get_content_width(parent);
    if (!h)
        h = lv_obj_get_content_height(parent);

    tl->cols = (float)w / base_size;
    tl->rows = (float)h / base_size;
    tl->w = tl->cols * base_size;
    tl->h = tl->rows * base_size;
    tl->map.w = ceil((float)tl->cols / 8.);
    tl->map.h = tl->rows;
    tl->map.data = calloc(tl->map.w * tl->map.h, sizeof(uint8_t));
    tl->col_dsc  = malloc((tl->cols + 1) * sizeof(int32_t));
    tl->row_dsc  = malloc((tl->rows + 1) * sizeof(int32_t));

    for (int i = 0; i < tl->cols; i++)
        tl->col_dsc[i] = base_size;
    tl->col_dsc[tl->cols] = LV_GRID_TEMPLATE_LAST;

    for (int i = 0; i < tl->rows; i++)
        tl->row_dsc[i] = base_size;
    tl->row_dsc[tl->rows] = LV_GRID_TEMPLATE_LAST;

    tl->layout = lv_obj_create(parent);
    lv_obj_remove_style_all(tl->layout);
    lv_obj_set_size(tl->layout, tl->w, tl->h);
    lv_obj_set_style_grid_column_dsc_array(tl->layout, tl->col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(tl->layout, tl->row_dsc, 0);
    lv_obj_set_layout(tl->layout, LV_LAYOUT_GRID);
    lv_obj_center(tl->layout);

    return tl;
}

lv_obj_t *tile_layout_new_item(void *layout, int cols, int rows, int pad)
{
    struct tile_layout *tl = layout;
    lv_area_t area;
    lv_obj_t *obj;

    for (int i = 0; i < tl->rows; i++)
    {
        for (int j = 0; j < tl->cols; j++)
        {
            area.x1 = j;
            area.y1 = i;
            area.x2 = j + cols - 1;
            area.y2 = i + rows - 1;
            if (is_free_area(&tl->map, &area))
            {
                mark_area(&tl->map, &area);
                if (pad)
                    return (void *)1;

                obj = lv_obj_create(tl->layout);
                lv_obj_remove_style_all(obj);
                lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, j, cols,
                                     LV_GRID_ALIGN_STRETCH, i, rows);
                lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

                return obj;
            }
        }
    }

    return NULL;
}

