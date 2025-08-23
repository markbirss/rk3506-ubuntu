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
#include <unistd.h>

#include <lvgl/lvgl.h>

#include "lv_port_init.h"

#define ARRAY_SIZE(x)   (sizeof(x) / sizeof(x[0]))
#define ALIGN(x, a)     (((x) + (a - 1)) & ~(a - 1))

void socket_reset(void);
int socket_read(int32_t *y, int32_t *min, int32_t *max, int32_t *avg,
                int32_t *cur);
void *socket_thread(void *argv);

int rpmsg_read(int32_t *y, int32_t *min, int32_t *max, int32_t *avg,
               int32_t *cur);
void rpmsg_reset(void);
void *rpmsg_thread(void *argv);

#endif

