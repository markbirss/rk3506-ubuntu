#ifndef __TILE_LAYOUT__
#define __TILE_LAYOUT__

void *tile_layout_create(lv_obj_t *parent, int w, int h, int base_size);
lv_obj_t *tile_layout_new_item(void *layout, int cols, int rows, int pad);

#endif

