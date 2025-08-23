#ifndef __APP_AIRCOND_H__
#define __APP_AIRCOND_H__

void app_aircond_init(lv_obj_t *parent, void *userdata);

#define APP_AIRCOND(x) {  \
    .w    = 2,  \
    .h    = 2,  \
    .init = app_aircond_init,  \
    .userdata = x,  \
}

#endif

