#ifndef __UI_SCALER_H__
#define __UI_SCALER_H__

void *ui_scaler_new(int width, int height);
void ui_scaler_set_match_rate(void *scaler, float rate);
void ui_scaler_set_refer_size(void *scaler, int width, int height);
int ui_scaler_calc(void *scaler, int value);
void ui_scaler_del(void *scaler);

#endif

