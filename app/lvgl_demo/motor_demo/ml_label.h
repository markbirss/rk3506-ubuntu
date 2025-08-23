#ifndef __ML_LABEL_H__
#define __ML_LABEL_H__

enum
{
    TXT_TITLE = 0,
    TXT_OVERVIEW,
    TXT_RESET,
    TXT_SYNC,
    TXT_JITTER,
    TXT_AXIS_X,
    TXT_AXIS_Y,
    TXT_CUR_POS,
    TXT_TAR_POS,
    TXT_CONFIRM,
    TXT_CLOSE,
    TXT_LANGUAGE,
    TXT_SYS_VER,
    TXT_HW_VER,
    TXT_MAX
};

lv_obj_t *ml_label_create(lv_obj_t *parent, int txt_idx);
void ml_label_set_text(lv_obj_t *obj, int txt_idx);
void ml_label_destroy(lv_obj_t *obj);
void ml_label_set_language(int language);

#endif

