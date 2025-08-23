#ifdef __cplusplus
extern "C" {
#endif

#include "home_ui.h"

struct lv_button_parameter
{
    lv_obj_t *ui_circle;
    lv_obj_t *ui_circle_label;
    lv_coord_t x;
    lv_coord_t y;
    const char *txt;
    lv_coord_t x_po_verify;
    lv_coord_t y_po_verify;
    lv_font_t txt_font;
};

void intercom_homepage_ui_init();

#ifdef __cplusplus
} /*extern "C"*/
#endif
