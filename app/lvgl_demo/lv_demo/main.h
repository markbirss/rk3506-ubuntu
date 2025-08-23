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

#define ALIGN(x, a)     (((x) + (a - 1)) & ~(a - 1))

#endif

