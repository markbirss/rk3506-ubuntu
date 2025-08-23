#ifndef __APP_SCENE_H__
#define __APP_SCENE_H__

void app_scene_init(lv_obj_t *parent, void *userdata);

#define APP_SCENE {  \
    .w    = 2,  \
    .h    = 1,  \
    .init = app_scene_init,  \
}

#endif

