#ifndef __MAIN_H__
#define __MAIN_H__

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <malloc.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <lvgl/lvgl.h>

#include "lv_port_init.h"
#include "timestamp.h"

#define ENABLE_MOTOR_CONTROL            0
#define ENABLE_POSITION_CONTROL         1

#define ALIGN(x, a)     (((x) + (a - 1)) & ~(a - 1))
#define FAKE_FD         1234
#define SRC_PNG(x)      "A:/usr/share/resource/"#x".png"
#define SRC_FONT(x)     "/usr/share/resource/"x
#define SYS_FONT(x)     "/usr/share/fonts/source-han-sans-cn/"x".otf"
#define METER_TICKS     13
#define PI              3.14159265358979f

struct tick
{
    lv_obj_t *obj;
    lv_obj_t *label;
    lv_point_t p[3];
};

struct meter
{
    lv_obj_t *cont;
    lv_obj_t *main;
    lv_obj_t *adorn;
    lv_obj_t *needle;
    lv_obj_t *label_val;
    lv_obj_t *label_uint;
    lv_obj_t *label_status;
    lv_obj_t *label_reverse;
    lv_timer_t *timer;
    int tar_speed;
    struct tick ticks[METER_TICKS];
    void *slave;
};

extern lv_obj_t *label_jitter;
extern lv_ft_info_t ttf_main;
extern lv_ft_info_t ttf_main_s;

void motor_start(void *s, int speed);
void motor_stop(void *s);
int motor_check(void *s);
void motor_set_position(void *s, int angle);
void update_tick_points(lv_point_t *p, int angle);
void update_needle_points(lv_point_t *p, int angle);
void speed_control_ui(lv_obj_t *cont_main,
                      struct meter *meters, int mode);
void position_control_ui(lv_obj_t *cont_main,
                         struct meter *meters, int mode);

#endif

