#include <lvgl/lvgl.h>

#include "gallery.h"

#include "anims/fade_out.h"
#include "anims/fade_slide_out.h"
#include "anims/photo_stream.h"
#include "anims/slide_out.h"

#ifdef USE_OPENGL
#include <lvgl/lv_drivers/sdl/gl/gl.h>

#include "anims/cube_flip.h"
#include "anims/cube_lyric.h"
#include "anims/cube_rotate.h"
#include "anims/fold.h"
#include "anims/roller.h"
#include "anims/stiker.h"

static char *lyric[] =
{
    "左面",
    "Left",
    "右面",
    "Right",
    "顶面",
    "Top",
    "底面",
    "Bottom",
    "前面",
    "Front",
    "背面",
    "Back",
};
int lines = sizeof(lyric) / sizeof(lyric[0]);

lv_gl_tex_t *tex_cube;
lv_gl_tex_t *tex_2d[6];
lv_gl_tex_t *tex_roller;
lv_gl_tex_t *tex_fb;

lv_gl_obj_t *obj_fb;
lv_gl_obj_t *obj_img0;
lv_gl_obj_t *obj_img1;

lv_gl_obj_t *obj_cube;
lv_gl_obj_t *obj_fold[4];
lv_gl_obj_t *obj_roller_items[6];
lv_gl_obj_t *obj_roller;
lyric_row *obj_lyrics;

SDL_Rect view;
SDL_Rect screen;
#endif

int animing = 0;
lv_ft_info_t ttf_main;
lv_obj_t *scr;
lv_obj_t *img1;
lv_obj_t *img2;
lv_obj_t *anim_area;
lv_obj_t *btn_mat;
lv_obj_t *slider;

lv_obj_t *photo_box;
lv_obj_t *photos[6];

static lv_anim_t anims[] =
{
    ANIM_FADE_OUT,
    ANIM_SLIDE_OUT,
    ANIM_FADE_SLIDE_OUT,
#ifdef USE_OPENGL
    ANIM_CUBE_ROTATE,
    ANIM_CUBE_FLIP,
    ANIM_CUBE_LYRIC,
    ANIM_FOLD,
    ANIM_ROLLER,
    ANIM_STIKER,
#endif
    ANIM_PHOTO_STREAM,
};
static int anim_cnt = sizeof(anims) / sizeof(anims[0]);

static const char *btnm_map[] = {
    "fade out", "slide out", "fade slide", "\n",
#ifdef USE_OPENGL
    "cube rotate", "cube flip", "cube lyric", "\n",
    "fold", "roller", "stiker", "\n",
#endif
    "photo stream", ""
};

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        if (id >= anim_cnt)
            return;
        if (!animing)
        {
            lv_anim_start(&anims[id]);
            if (anims[id].start_value > anims[id].end_value)
            {
                lv_slider_set_range(slider,
                    anims[id].end_value,
                    anims[id].start_value);
            }
            else
            {
                lv_slider_set_range(slider,
                    anims[id].start_value,
                    anims[id].end_value);
            }
            lv_slider_set_value(slider, 0, LV_ANIM_OFF);
            animing = 1;
        }
    }
}

static void font_init(void)
{
    printf("%s\n", __func__);
    lv_freetype_init(64, 1, 0);

    ttf_main.weight = 60;
    ttf_main.name = "/usr/share/fonts/source-han-sans-cn/SourceHanSansCN-Regular.otf";
    ttf_main.style = FT_FONT_STYLE_NORMAL;
    lv_ft_font_init(&ttf_main);
}

static label_canvas *create_canvas(lv_color_t color,
    lv_font_t *font)
{
    label_canvas *lc;

    lc = lv_mem_alloc(sizeof(label_canvas));
    if (!lc)
        return NULL;
    lv_memset_00(lc, sizeof(label_canvas));

    lv_draw_label_dsc_init(&lc->label_dsc);
    lc->label_dsc.color = color;
    lc->label_dsc.font = font;
    lc->canvas = lv_canvas_create(NULL);

    return lc;
}

#if USE_OPENGL
static lv_gl_obj_t *utf8_to_obj(lv_gl_obj_t *parent,
    label_canvas *lc, char *text)
{
    lv_img_dsc_t *img_dsc = lc->img_dsc;
    lv_gl_obj_t *obj;
    lv_gl_tex_t *tex;
    lv_gl_img_t img;
    lv_coord_t data_size;
    lv_point_t size;

    lv_txt_get_size(&size, text, lc->label_dsc.font,
        0, 0, LV_COORD_MAX, 0);

    data_size = lv_img_buf_get_img_size(ALIGN(size.x, 16),
            ALIGN(size.y, 16), LV_IMG_CF_TRUE_COLOR_ALPHA);
    if (!img_dsc ||
        data_size > img_dsc->data_size)
    {
        if (img_dsc)
            lv_img_buf_free(img_dsc);
        img_dsc = lv_img_buf_alloc(ALIGN(size.x, 16),
            ALIGN(size.y, 16), LV_IMG_CF_TRUE_COLOR_ALPHA);
        lc->img_dsc = img_dsc;
        printf("new buf %p\n", img_dsc);
    }
    else
    {
        lv_memset_00((uint8_t *)img_dsc->data,
            img_dsc->data_size);
    }
    lv_canvas_set_buffer(lc->canvas, (void *)img_dsc->data,
        ALIGN(size.x, 16), ALIGN(size.y, 16),
        img_dsc->header.cf);
    lv_canvas_draw_text(lc->canvas,
        ((ALIGN(size.x, 16) - size.x) / 2),
        ((ALIGN(size.y, 16) - size.y) / 2),
        ALIGN(size.x, 16), &lc->label_dsc, text);

    img.pixels = img_dsc->data;
    img.format = LV_GL_FMT_BGRA;
    img.w = ALIGN(size.x, 16);
    img.h = ALIGN(size.y, 16);
    obj = lv_gl_obj_create(img.w, img.h);
    tex = lv_gl_tex_create(GL_TEX_TYPE_2D, 0, 0, &img);
    lv_gl_obj_bind_tex(obj, tex);

    return obj;
}
#endif

void common_anim_start(void)
{
    lv_obj_add_flag(anim_area, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(img1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(img2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(photo_box, LV_OBJ_FLAG_HIDDEN);
    anim_photo_stream_stop();
#if USE_OPENGL
    lv_gl_set_render_cb(NULL);
    lv_gl_obj_set_viewport(NULL, &view);
    lv_gl_obj_set_crop(NULL, NULL, 0);
#endif
}

#if USE_OPENGL
static void tex_init(void)
{
    lv_gl_img_t imgs[6];

    imgs[CUBE_LEFT].format   = LV_GL_FMT_BGRA;
    imgs[CUBE_LEFT].pixels   = pic1.data;
    imgs[CUBE_LEFT].w        = pic1.header.w;
    imgs[CUBE_LEFT].h        = pic1.header.h;

    imgs[CUBE_RIGHT].format  = LV_GL_FMT_BGRA;
    imgs[CUBE_RIGHT].pixels  = pic2.data;
    imgs[CUBE_RIGHT].w       = pic2.header.w;
    imgs[CUBE_RIGHT].h       = pic2.header.h;

    imgs[CUBE_TOP].format    = LV_GL_FMT_BGRA;
    imgs[CUBE_TOP].pixels    = pic3.data;
    imgs[CUBE_TOP].w         = pic3.header.w;
    imgs[CUBE_TOP].h         = pic3.header.h;

    imgs[CUBE_BOTTOM].format = LV_GL_FMT_BGRA;
    imgs[CUBE_BOTTOM].pixels = pic4.data;
    imgs[CUBE_BOTTOM].w      = pic4.header.w;
    imgs[CUBE_BOTTOM].h      = pic4.header.h;

    imgs[CUBE_FRONT].format  = LV_GL_FMT_BGRA;
    imgs[CUBE_FRONT].pixels  = pic5.data;
    imgs[CUBE_FRONT].w       = pic5.header.w;
    imgs[CUBE_FRONT].h       = pic5.header.h;

    imgs[CUBE_BACK].format   = LV_GL_FMT_BGRA;
    imgs[CUBE_BACK].pixels   = pic6.data;
    imgs[CUBE_BACK].w        = pic6.header.w;
    imgs[CUBE_BACK].h        = pic6.header.h;

    tex_cube = lv_gl_tex_create(GL_TEX_TYPE_CUBE, 0, 0, imgs);

    tex_2d[0] = lv_gl_tex_create(GL_TEX_TYPE_2D, 0, 0, &imgs[0]);
    tex_2d[1] = lv_gl_tex_create(GL_TEX_TYPE_2D, 0, 0, &imgs[1]);
    tex_2d[2] = lv_gl_tex_create(GL_TEX_TYPE_2D, 0, 0, &imgs[2]);
    tex_2d[3] = lv_gl_tex_create(GL_TEX_TYPE_2D, 0, 0, &imgs[3]);
    tex_2d[4] = lv_gl_tex_create(GL_TEX_TYPE_2D, 0, 0, &imgs[4]);
    tex_2d[5] = lv_gl_tex_create(GL_TEX_TYPE_2D, 0, 0, &imgs[5]);
}
#endif

void gallery(void)
{
    font_init();

    scr = lv_scr_act();
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_refr_size(scr);

    anim_area = lv_obj_create(scr);
    lv_obj_remove_style_all(anim_area);
    lv_obj_set_size(anim_area, lv_pct(100), lv_pct(80));
    lv_obj_align(anim_area, LV_ALIGN_TOP_MID, 0, 0);

    img1 = lv_img_create(anim_area);
    lv_img_set_src(img1, (void *)&pic1);
    lv_obj_center(img1);

    img2 = lv_img_create(anim_area);
    lv_img_set_src(img2, (void *)&pic2);
    lv_obj_center(img2);

    btn_mat = lv_btnmatrix_create(scr);
    lv_obj_set_size(btn_mat, lv_pct(100), lv_pct(20));
    lv_btnmatrix_set_map(btn_mat, btnm_map);
    lv_obj_align(btn_mat, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(btn_mat, event_handler,
        LV_EVENT_ALL, NULL);
    lv_obj_refr_size(btn_mat);

    slider = lv_slider_create(scr);
    lv_obj_set_style_bg_opa(slider, LV_OPA_TRANSP,
        LV_PART_KNOB);
    lv_obj_set_size(slider, lv_pct(100), 2);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 0, LV_ANIM_ON);
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, 0);

#ifdef USE_OPENGL
    screen.x = 0;
    screen.y = 0;
    screen.w  = lv_obj_get_width(scr);
    screen.h  = lv_obj_get_height(scr);

    view.x = 0;
    view.y = lv_obj_get_height(btn_mat);
    view.w = lv_obj_get_width(scr);
    view.h = lv_obj_get_height(scr) - view.y;
    printf("view %d %d %d %d\n", view.x, view.y, view.w, view.h);
    lv_gl_obj_set_viewport(NULL, &view);

    tex_init();

    obj_cube = lv_gl_obj_create(480, 480);
    lv_gl_obj_bind_tex(obj_cube, tex_cube);
    printf("cube %p\n", obj_cube);

    obj_fold[0] = lv_gl_obj_create(screen.w / 2, screen.w);
    obj_fold[1] = lv_gl_obj_create(screen.w / 2, screen.w);
    obj_fold[2] = lv_gl_obj_create(screen.w / 2, screen.w);
    obj_fold[3] = lv_gl_obj_create(screen.w / 2, screen.w);
    lv_gl_obj_bind_tex(obj_fold[0], tex_2d[0]);
    lv_gl_obj_bind_tex(obj_fold[1], tex_2d[0]);
    lv_gl_obj_bind_tex(obj_fold[2], tex_2d[1]);
    lv_gl_obj_bind_tex(obj_fold[3], tex_2d[1]);

    printf("fold %p %p %p %p\n", obj_fold[0], obj_fold[1],
        obj_fold[2], obj_fold[3]);

    obj_fb = lv_gl_obj_create(480, 480);
    obj_fb->scale.x = (float)obj_fb->base.w / view.w;
    obj_fb->scale.y = (float)obj_fb->base.h / view.h;
    obj_img0 = lv_gl_obj_create(120, 120);
    obj_img1 = lv_gl_obj_create(480, 480);
    tex_fb = lv_gl_tex_create(GL_TEX_TYPE_2D, 480, 480, NULL);
    lv_gl_obj_resize(obj_img0, obj_fb);
    lv_gl_obj_resize(obj_img1, obj_fb);
    lv_gl_obj_bind_tex(obj_img0, tex_2d[0]);
    lv_gl_obj_bind_tex(obj_img1, tex_2d[1]);
    lv_gl_obj_bind_tex(obj_fb, tex_fb);

    obj_roller = lv_gl_obj_create(480, 480);
    lv_gl_obj_resize(obj_roller, NULL);
    tex_roller = lv_gl_tex_create(GL_TEX_TYPE_2D, 480, 480, NULL);
    lv_gl_obj_bind_tex(obj_roller, tex_roller);

    lv_gl_set_fb(obj_roller);
    for (int i = 0; i < 6; i++)
    {
        obj_roller_items[i] = lv_gl_obj_create(240, 480);
        lv_gl_obj_resize(obj_roller_items[i], obj_roller);
        lv_gl_obj_bind_tex(obj_roller_items[i], tex_2d[i]);
        /* the full image size is 480x480, just use 240x480 at [120, 0]*/
        obj_roller_items[i]->tp.x = 120;
        obj_roller_items[i]->tp.w = 240;
        lv_gl_obj_update_vao(obj_roller_items[i]);
        obj_roller_items[i]->self_rot.y = (360.0 / 6.0) * i;
    }
    lv_gl_set_fb(NULL);
    printf("roller %p\n", obj_roller);

    label_canvas *lc =
        create_canvas(lv_palette_main(LV_PALETTE_ORANGE),
            ttf_main.font);
    obj_lyrics = lv_mem_alloc(lines * sizeof(*obj_lyrics));
    for (int i = 0; i < lines; i++)
    {
        uint32_t byte_id = 0;
        uint32_t last_byte_id = 0;
        uint32_t word;
        uint32_t words = 0;
        char buf[32];
        while (1)
        {
            word = _lv_txt_encoded_next(lyric[i], &byte_id);
            if (!word)
                break;
            words++;
        }
        obj_lyrics[i].objs =
            lv_mem_alloc(words * sizeof(lv_gl_obj_t *));
        obj_lyrics[i].len = words;
        byte_id = 0;
        for (int j = 0; j < words; j++)
        {
            last_byte_id = byte_id;
            _lv_txt_encoded_next(lyric[i], &byte_id);
            memcpy(buf, lyric[i] + last_byte_id,
                byte_id - last_byte_id);
            buf[byte_id - last_byte_id] = 0;
            obj_lyrics[i].objs[j] =
                utf8_to_obj(obj_cube, lc, buf);
            lv_gl_obj_resize(obj_lyrics[i].objs[j], obj_cube);
        }
    }

    float box_w, box_h;
    float start_x, start_y;
    for (int idx = CUBE_LEFT; idx <= CUBE_BACK; idx++)
    {
        box_h = 2 * (obj_lyrics[idx].objs[0]->scale.y * 2.0);
        start_y = obj_lyrics[0].objs[0]->scale.y + (2.0 - box_h) / 2 - 1.0;
        for (int i = 0; i < 2; i++)
        {
            box_w = obj_lyrics[idx * 2 + i].len *
                (obj_lyrics[idx * 2 + i].objs[0]->scale.x * 2.0);
            for (int j = 0; j < obj_lyrics[idx * 2 + i].len; j++)
            {
                start_x = (2.0 - box_w) / 2 - 1.0;
                obj_lyrics[idx * 2 + i].objs[j]->out_type =
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx;
                obj_lyrics[idx * 2 + i].objs[j]->move.x = start_x +
                    j * (obj_lyrics[i].objs[0]->scale.x * 2.0);
                obj_lyrics[idx * 2 + i].objs[j]->move.y = start_y +
                    i * (obj_lyrics[i].objs[0]->scale.y * 2.0);
            }
        }
    }
#endif

    photo_box = lv_obj_create(anim_area);
    lv_obj_remove_style_all(photo_box);
    lv_obj_set_size(photo_box, lv_pct(100), lv_pct(100));
    lv_obj_refr_size(anim_area);
    lv_obj_refr_size(photo_box);
    lv_obj_add_flag(photo_box, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(photo_box, LV_OBJ_FLAG_SCROLLABLE);
    for (int i = 0; i < 6; i++)
    {
        photos[i] = lv_img_create(photo_box);
        lv_obj_set_pos(photos[i],
            (lv_obj_get_width(photo_box) - pic1.header.w) / 2, 500 * i);
    }
    lv_img_set_src(photos[0], &pic1);
    lv_img_set_src(photos[1], &pic2);
    lv_img_set_src(photos[2], &pic3);
    lv_img_set_src(photos[3], &pic4);
    lv_img_set_src(photos[4], &pic5);
    lv_img_set_src(photos[5], &pic6);
}

