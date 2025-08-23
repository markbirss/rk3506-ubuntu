#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int width;
    int height;
    int refer_width;
    int refer_height;
    float scale_rate;
    float match_rate;
} ui_scaler;

void *ui_scaler_new(int width, int height)
{
    ui_scaler *scaler;

    scaler = malloc(sizeof(ui_scaler));
    if (!scaler) return NULL;

    scaler->width = width;
    scaler->height = height;
    scaler->scale_rate = 1.0f;
    scaler->match_rate = 0.5f;

    return scaler;
}

void ui_scaler_set_refer_size(void *scaler, int width, int height)
{
    ui_scaler *_scaler = (ui_scaler *)scaler;
    float rate_hor, rate_ver;

    if (!scaler) return;

    rate_hor = (float)width / _scaler->width;
    rate_ver = (float)height / _scaler->height;

    _scaler->refer_width = width;
    _scaler->refer_height = height;
    _scaler->scale_rate = rate_hor * (1.0f - _scaler->match_rate) +
                          rate_ver * _scaler->match_rate;
    printf("%d %d %f\n", width, height, _scaler->scale_rate);
}

void ui_scaler_set_match_rate(void *scaler, float rate)
{
    ui_scaler *_scaler = (ui_scaler *)scaler;

    if (!scaler) return;

    _scaler->match_rate = rate;
    ui_scaler_set_refer_size(scaler, _scaler->refer_width,
                             _scaler->refer_height);
}

int ui_scaler_calc(void *scaler, int value)
{
    ui_scaler *_scaler = (ui_scaler *)scaler;

    if (!scaler) return value;

    return (int)(value * _scaler->scale_rate);
}

void ui_scaler_del(void *scaler)
{
    free(scaler);
}

