#ifndef __UI_BTNMATRIX_H__
#define __UI_BTNMATRIX_H__

#include <lvgl/lvgl.h>

struct btn_desc
{
    lv_obj_t **obj;
    void *img;
    char *text;
    lv_area_t area;
    lv_coord_t w;
    lv_coord_t h;
    lv_obj_t *(*create)(lv_obj_t *parent);
    void (*draw)(lv_obj_t *parent, struct btn_desc *desc);
    lv_event_cb_t cb;
    void *user_data;
};

struct btn_matrix_desc
{
    const lv_coord_t *col_dsc;
    const lv_coord_t *row_dsc;
    lv_coord_t pad;
    lv_coord_t gap;
    struct btn_desc *desc;
    lv_coord_t btn_cnt;
};

void common_draw(lv_obj_t *parent, struct btn_desc *desc);
lv_obj_t *ui_btnmatrix_create(lv_obj_t *parent, struct btn_matrix_desc *desc);

#endif

