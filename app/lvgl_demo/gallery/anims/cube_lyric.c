#include "gallery.h"
#include "cube_lyric.h"

#ifdef USE_OPENGL
void anim_cube_lyric_start(lv_anim_t *a);
void anim_cube_lyric(void *var, int32_t v);
void anim_cube_lyric_end(lv_anim_t *a);

void anim_cube_lyric_sub(void *var, int32_t v);
void anim_cube_lyric_sub_end(lv_anim_t *a);

static lv_anim_t sub_anims[] =
{
    {
        .time = 300,
        .act_time = -500,
        .start_value = 0,
        .current_value = 0,
        .end_value = 90,
        .repeat_cnt = 1,
        .path_cb = lv_anim_path_linear,
        .exec_cb = anim_cube_lyric_sub,
        .deleted_cb = anim_cube_lyric_sub_end
    },
    {
        .time = 3000,
        .start_value = 0,
        .current_value = 0,
        .end_value = 100,
        .repeat_cnt = 1,
        .path_cb = lv_anim_path_linear,
        .exec_cb = anim_cube_lyric,
        .deleted_cb = anim_cube_lyric_end
    }
};

static int line;
static int row;
static int col;

void anim_cube_lyric_render(void)
{
    lv_gl_set_fb(obj_cube);
    lv_gl_obj_set_viewport(NULL, NULL);
    lv_gl_obj_render(obj_lyrics[row].objs[col]);

    /* render to screen */
    lv_gl_set_fb(NULL);
    lv_gl_obj_set_viewport(NULL, &cube_view);
    lv_gl_obj_render(obj_cube);
}

void anim_cube_lyric_start(lv_anim_t *a)
{
    printf("%s\n", __func__);
    anim_cube_start(a);
    obj_cube->scale.x = 0.57;
    obj_cube->scale.y = 0.57;
    obj_cube->scale.z = 0.57;
    //lv_gl_obj_set_view_angle(obj_cube, -10, 15, 0);
    lv_gl_set_render_cb(anim_cube_lyric_render);

    /* start with left side */
    lv_gl_obj_set_angle(obj_cube, 0, 90, 0);

    line = 0;
}

void anim_cube_lyric(void *var, int32_t v)
{
    static int32_t last_v = -1;
    int index = (intptr_t)var;
    int pos = 0;
    lv_slider_set_value(slider, v, LV_ANIM_ON);

    if (last_v == v)
        return;
    last_v = v;

    pos = (float)v / 100 * (obj_lyrics[line].len - 1);
    row = line;
    col = pos;
    //printf("%s %d %d\n", __func__, row, col);

    lv_obj_invalidate(lv_layer_top());
}

static const char *line_str[] =
{
    "CUBE_LEFT",
    "CUBE_RIGHT",
    "CUBE_TOP",
    "CUBE_BOTTOM",
    "CUBE_FRONT",
    "CUBE_BACK",
};

void anim_cube_lyric_sub(void *var, int32_t v)
{
    int side = line / 2;
    int angle;

    printf("%s from %s to %s\n", __func__, line_str[side - 1], line_str[side]);
    switch (side)
    {
    case CUBE_RIGHT:
        /* 0,90,0 to 0,-90,0
         * v from 0 to 90
         */
        lv_gl_obj_set_angle(obj_cube, 0, 90 + (-2 * v), 0);
        printf("%f %f %f\n", obj_cube->self_rot.x, obj_cube->self_rot.y,
            obj_cube->self_rot.z);
        break;
    case CUBE_TOP:
        /* 0,-90,0 to 90,0,0
         * v from 0 to 90
         */
        lv_gl_obj_set_angle(obj_cube, v, -90 + v, 0);
        printf("%f %f %f\n", obj_cube->self_rot.x, obj_cube->self_rot.y,
            obj_cube->self_rot.z);
        break;
    case CUBE_BOTTOM:
        /* 90,0,0 to -90,0,0
         * v from 0 to 90
         */
        lv_gl_obj_set_angle(obj_cube, 90 + (-2 * v), 0, 0);
        printf("%f %f %f\n", obj_cube->self_rot.x, obj_cube->self_rot.y,
            obj_cube->self_rot.z);
        break;
    case CUBE_FRONT:
        /* -90,0,0 to 0,0,0
         * v from 0 to 90
         */
        lv_gl_obj_set_angle(obj_cube, -90 + v, 0, 0);
        printf("%f %f %f\n", obj_cube->self_rot.x, obj_cube->self_rot.y,
            obj_cube->self_rot.z);
        break;
    case CUBE_BACK:
        /* 0,0,0 to 0,180,0
         * v from 0 to 90
         */
        lv_gl_obj_set_angle(obj_cube, 0, v * 2, 0);
        printf("%f %f %f\n", obj_cube->self_rot.x, obj_cube->self_rot.y,
            obj_cube->self_rot.z);
        break;
    }

    lv_obj_invalidate(lv_layer_top());
}

void anim_cube_lyric_sub_end(lv_anim_t *a)
{
    sub_anims[1].current_value = 0;
    /* 300ms per word */
    sub_anims[1].time = obj_lyrics[line].len * 300;
    lv_anim_start(&sub_anims[1]);
}

void anim_cube_lyric_end(lv_anim_t *a)
{
    int index = (intptr_t)a->var;
    lv_obj_t *t;

    line++;
    if (line >= lines)
    {
        animing = 0;
        lv_slider_set_range(slider, 0, 100);
        lv_slider_set_value(slider, 0, LV_ANIM_ON);
        return;
    }

    if (line & 1)
    {
        sub_anims[1].current_value = 0;
        /* 300ms per word */
        sub_anims[1].time = obj_lyrics[line].len * 300;
        lv_anim_start(&sub_anims[1]);
    }
    else
    {
        sub_anims[0].current_value = 0;
        lv_anim_start(&sub_anims[0]);
    }
}
#endif

