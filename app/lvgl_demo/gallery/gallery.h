#ifndef __GALLERY_H__
#define __GALLERY_H__

#include <lvgl/lvgl.h>
#include <lvgl/lv_drivers/sdl/gl/gl.h>

#define ALIGN(x, a) (((x) + (a - 1)) & ~(a - 1))

typedef struct
{
    lv_draw_label_dsc_t label_dsc;
    lv_img_dsc_t *img_dsc;
    lv_obj_t *canvas;
} label_canvas;

typedef struct {
    lv_gl_obj_t **objs;
    int len;
} lyric_row;

extern lv_ft_info_t ttf_main;
extern lv_gl_obj_t *obj_fb;
extern lv_gl_obj_t *obj_img0;
extern lv_gl_obj_t *obj_img1;
extern lv_gl_obj_t *obj_cube;
extern lv_gl_obj_t *obj_fold[4];
extern lv_gl_obj_t *obj_roller_items[6];
extern lv_gl_obj_t *obj_roller;
extern lyric_row *obj_lyrics;
extern lv_obj_t *scr;
extern lv_obj_t *img1;
extern lv_obj_t *img2;
extern lv_obj_t *anim_area;
extern lv_obj_t *slider;
extern lv_obj_t *photo_box;
extern lv_obj_t *photos[6];
extern SDL_Rect view;
extern SDL_Rect screen;
extern SDL_Rect cube_view;
extern int animing;
extern int lines;

extern const lv_img_dsc_t pic1;
extern const lv_img_dsc_t pic2;
extern const lv_img_dsc_t pic3;
extern const lv_img_dsc_t pic4;
extern const lv_img_dsc_t pic5;
extern const lv_img_dsc_t pic6;

void common_anim_start(void);
void anim_cube_start(lv_anim_t *a);

#endif

