/**
 * @file evdev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "evdev.h"
#if USE_EVDEV != 0 || USE_BSD_EVDEV

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#if USE_BSD_EVDEV
#include <dev/dev/input.h>
#else
#include <linux/input.h>
#endif
#include <libevdev-1.0/libevdev/libevdev.h>
#include <dirent.h>

#include "main.h"
#include "kalman_filter.h"
/*********************
 *      DEFINES
 *********************/

#define PSENSOR_IOCTL_MAGIC             'p'
#define PSENSOR_IOCTL_GET_ENABLED       _IOR(PSENSOR_IOCTL_MAGIC, 1, int *)
#define PSENSOR_IOCTL_ENABLE            _IOW(PSENSOR_IOCTL_MAGIC, 2, int *)
#define PSENSOR_IOCTL_DISABLE           _IOW(PSENSOR_IOCTL_MAGIC, 3, int *)

#define LIGHTSENSOR_IOCTL_MAGIC         'l'
#define LIGHTSENSOR_IOCTL_GET_ENABLED   _IOR(LIGHTSENSOR_IOCTL_MAGIC, 1, int *)
#define LIGHTSENSOR_IOCTL_ENABLE        _IOW(LIGHTSENSOR_IOCTL_MAGIC, 2, int *)
#define LIGHTSENSOR_IOCTL_SET_RATE      _IOW(LIGHTSENSOR_IOCTL_MAGIC, 3, short)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct evdev_record
{
    int x;
    int y;
    int button;
} evdev_record_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
int map(int x, int in_min, int in_max, int out_min, int out_max);

/**********************
 *  STATIC VARIABLES
 **********************/
static struct libevdev *evdev = NULL;
static int evdev_fd = -1;
static int evdev_min_x = DEFAULT_EVDEV_HOR_MIN;
static int evdev_max_x = DEFAULT_EVDEV_HOR_MAX;
static int evdev_min_y = DEFAULT_EVDEV_VER_MIN;
static int evdev_max_y = DEFAULT_EVDEV_VER_MAX;
static int evdev_calibrate = 0;
static evdev_record_t evdev_val[2];
static int touch_up = 0;
static pthread_t evdev_tid;
static pthread_mutex_t evdev_lock;

static int evdev_key_val;
static int evdev_button;

static int evdev_rot;

#define FILE_DEBUG      0
#if FILE_DEBUG
static FILE *raw_point;
static FILE *lv_raw_point;
static FILE *lv_fix_point;
#endif

static KalmanFilter kfx;
static KalmanFilter kfy;

#if USE_SENSOR
static int psensor_event_id = -1;
static int lsensor_event_id = -1;
static int psensor_fd = -1;
static int lsensor_fd = -1;
#endif
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#define TP_NAME_LEN (32)
static char tp_event[TP_NAME_LEN] = EVDEV_NAME;
static void *evdev_thread(void *arg);
/**
 * Get touchscreen device event no
 */
int evdev_get_tp_event(void)
{
    int fd = 0, len = 0;
    int i = 0, input_dev_num = 0;
    DIR *pDir;
    struct dirent *ent = NULL;
    char file_name[TP_NAME_LEN];
    char tp_name[TP_NAME_LEN];
    char *path = "/sys/class/input";

    if ((pDir = opendir(path)) == NULL)
    {
        printf("%s: open %s filed\n", __func__, path);
        return -1;
    }

    while ((ent = readdir(pDir)) != NULL)
    {
        if (strstr(ent->d_name, "input"))
            input_dev_num++;
//            printf("%s: %s input deveices %d\n",
//                   __func__,
//                   ent->d_name,
//                   input_dev_num);
    }
    closedir(pDir);

    for (i = 0; i < input_dev_num; i++)
    {
        sprintf(file_name, "/sys/class/input/input%d/name", i);
        fd = open(file_name, O_RDONLY);
        if (fd == -1)
        {
            printf("%s: open %s failed\n", __func__, file_name);
            continue;
        }

        len = read(fd, tp_name, TP_NAME_LEN);
        close(fd);
        if (len <= 0)
        {
            printf("%s: read %s failed\n", __func__, file_name);
            continue;
        }

        if (len >= TP_NAME_LEN)
            len = TP_NAME_LEN - 1;

        tp_name[len] = '\0';

#if USE_SENSOR
        if (strstr(tp_name, "lightsensor"))
        {
            lsensor_event_id = i;
            continue;
        }

        if (strstr(tp_name, "proximity"))
        {
            psensor_event_id = i;
            continue;
        }
#else
        /*
         * There is a 'ts' in the 'lightsensor', skip it to avoid being
         * treated as a touch device
         */
        if (strstr(tp_name, "lightsensor"))
            continue;
#endif

        if (strstr(tp_name, "ts") || strstr(tp_name, "gsl"))
        {
            sprintf(tp_event, "/dev/input/event%d", i);
            printf("%s: %s = %s%s\n", __func__, file_name, tp_name, tp_event);
            return 0;
        }
    }

    return -1;
}

#if USE_SENSOR
void evdev_sensor_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    struct input_event in;
    struct timeval tv;
    int drv_fd = *(int *)drv->user_data;
    fd_set rdfs;
    int ret;

    FD_ZERO(&rdfs);
    FD_SET(drv_fd, &rdfs);

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    data->continue_reading = 1;
    if (select(drv_fd + 1, &rdfs, NULL, NULL, &tv) <= 0)
    {
        return;
    }

    if ((ret = read(drv_fd, &in, sizeof(in))) > 0)
    {
        if (in.type == EV_ABS)
        {
            if (in.code == ABS_DISTANCE)
            {
                if (in.value == 0)
                    data->state = LV_INDEV_STATE_RELEASED;
                else
                    data->state = LV_INDEV_STATE_PRESSED;
                data->continue_reading = 0;
            }
            else if (in.code == ABS_MISC)
            {
                if (in.value == 0)
                    data->state = LV_INDEV_STATE_RELEASED;
                else
                    data->state = LV_INDEV_STATE_PRESSED;
                data->key = in.value;
                data->continue_reading = 0;
            }
        }
    }
}

void *evdev_get_psensor(void)
{
    return &psensor_fd;
}

int evdev_init_psensor(void)
{
    char event_name[64];
    int fd, ret, enable = 1;

    if (psensor_event_id == -1)
    {
        if (evdev_get_tp_event() < 0)
        {
            printf("%s get event failed\n", __func__);
            return -1;
        }
        if (psensor_event_id == -1)
        {
            printf("%s get psensor event failed\n", __func__);
            return -1;
        }
    }

    fd = open("/dev/psensor", O_RDWR);
    if (fd < 0)
    {
        printf("can't open /dev/psensor!\n");
        return -1;
    }

    ret = ioctl(fd, PSENSOR_IOCTL_ENABLE, &enable);
    if (ret < 0)
    {
        printf("eanble /dev/psensor failed %d!\n", ret);
    }
    else
    {
        printf("enable /dev/psensor successfully!\n");
    }
    close(fd);

    snprintf(event_name, sizeof(event_name),
             "/dev/input/event%d", psensor_event_id);
    fd = open(event_name, O_RDONLY);
    if (fd < 0)
    {
        printf("can't open %s\n", event_name);
        return -1;
    }
    psensor_fd = fd;

    return ret;
}

void *evdev_get_lsensor(void)
{
    return &lsensor_fd;
}

int evdev_init_lsensor(void)
{
    char event_name[64];
    int fd, ret, enable = 1;

    if (lsensor_event_id == -1)
    {
        if (evdev_get_tp_event() < 0)
        {
            printf("%s get event failed\n", __func__);
            return -1;
        }
        if (lsensor_event_id == -1)
        {
            printf("%s get lsensor event failed\n", __func__);
            return -1;
        }
    }

    fd = open("/dev/lightsensor", O_RDWR);
    if (fd < 0)
    {
        printf("can't open /dev/lightsensor!\n");
        return -1;
    }

    ret = ioctl(fd, LIGHTSENSOR_IOCTL_ENABLE, &enable);
    if (ret < 0)
    {
        printf("eanble /dev/lightsensor failed %d!\n", ret);
    }
    else
    {
        printf("enable /dev/lightsensor successfully!\n");
    }
    close(fd);

    snprintf(event_name, sizeof(event_name),
             "/dev/input/event%d", lsensor_event_id);
    fd = open(event_name, O_RDONLY);
    if (fd < 0)
    {
        printf("can't open %s\n", event_name);
        return -1;
    }
    lsensor_fd = fd;

    return ret;
}
#endif

/**
 * Initialize the evdev interface
 */
int evdev_init(lv_disp_drv_t *drv, int rot)
{
    const char *event_name;
    int rc = 1;
    evdev_rot = rot;

#if FILE_DEBUG
    raw_point = fopen("/tmp/raw_point.txt", "wb+");
    lv_raw_point = fopen("/tmp/lv_raw_point.txt", "wb+");
    lv_fix_point = fopen("/tmp/lv_fix_point.txt", "wb+");
#endif

    event_name = getenv("LV_EVENT_NAME");
    if (event_name)
    {
        strncpy(tp_event, event_name, TP_NAME_LEN);
    }
    else if (evdev_get_tp_event() < 0)
    {
        printf("%s get tp event failed\n", __func__);
        return -1;
    }
    return evdev_set_file(drv, tp_event);
}
/**
 * reconfigure the device file for evdev
 * @param dev_name set the evdev device filename
 * @return true: the device file set complete
 *         false: the device file doesn't exist current system
 */
int evdev_set_file(lv_disp_drv_t *drv, char *dev_name)
{
    int disp_hor;
    int disp_ver;
    int rc = 1;

    if (evdev_fd != -1)
        close(evdev_fd);
    evdev_fd = -1;
    if (evdev)
        libevdev_free(evdev);
    evdev = NULL;

#if USE_BSD_EVDEV
    evdev_fd = open(dev_name, O_RDWR | O_NOCTTY);
#else
    evdev_fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
#endif

    if (evdev_fd == -1)
    {
        perror("unable open evdev interface:");
        return -1;
    }

    rc = libevdev_new_from_fd(evdev_fd, &evdev);
    if (rc < 0)
    {
        printf("Failed to init libevdev (%s)\n", strerror(-rc));
    }
    else
    {
        if (libevdev_has_event_type(evdev, EV_ABS))
        {
            const struct input_absinfo *abs;
            if (libevdev_has_event_code(evdev, EV_ABS, ABS_MT_POSITION_X))
            {
                abs = libevdev_get_abs_info(evdev, ABS_MT_POSITION_X);
                printf("EV_ABS ABS_MT_POSITION_X\n");
                printf("\tMin\t%6d\n", abs->minimum);
                printf("\tMax\t%6d\n", abs->maximum);
                if ((evdev_rot == 0) || (evdev_rot == 180))
                {
                    evdev_min_x = abs->minimum;
                    evdev_max_x = abs->maximum;
                }
                else
                {
                    evdev_min_y = abs->minimum;
                    evdev_max_y = abs->maximum;
                }
            }
            if (libevdev_has_event_code(evdev, EV_ABS, ABS_MT_POSITION_Y))
            {
                abs = libevdev_get_abs_info(evdev, ABS_MT_POSITION_Y);
                printf("EV_ABS ABS_MT_POSITION_Y\n");
                printf("\tMin\t%6d\n", abs->minimum);
                printf("\tMax\t%6d\n", abs->maximum);
                if ((evdev_rot == 0) || (evdev_rot == 180))
                {
                    evdev_min_y = abs->minimum;
                    evdev_max_y = abs->maximum;
                }
                else
                {
                    evdev_min_x = abs->minimum;
                    evdev_max_x = abs->maximum;
                }
            }
        }
    }

    fcntl(evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);

    memset(evdev_val, 0, sizeof(evdev_val));
    evdev_key_val = 0;
    evdev_button = LV_INDEV_STATE_REL;

    disp_hor = drv->hor_res;
    disp_ver = drv->ver_res;
    if ((evdev_min_x != 0) ||
            (evdev_max_x != disp_hor) ||
            (evdev_min_y != 0) ||
            (evdev_max_y != disp_ver))
    {
        evdev_calibrate = 1;
        printf("calibrate [%d,%d]x[%d,%d] to %dx%d\n",
               evdev_min_x, evdev_max_x,
               evdev_min_y, evdev_max_y,
               disp_hor, disp_ver);
    }
    printf("evdev_calibrate = %d\n", evdev_calibrate);

    pthread_mutex_init(&evdev_lock, NULL);
    pthread_create(&evdev_tid, NULL, evdev_thread, NULL);

    return 0;
}
/**
 * Get the current position and state of the evdev
 * @param data store the evdev data here
 * @return false: because the points are not buffered, so no more data to be read
 */

static void *evdev_thread(void *arg)
{
    int type = LV_INDEV_TYPE_POINTER;
    struct input_event in;
    int x = 0;
    int y = 0;
    int button = 0;
    fd_set rdfs;

    FD_ZERO(&rdfs);
    FD_SET(evdev_fd, &rdfs);

    while (1)
    {
        select(evdev_fd + 1, &rdfs, NULL, NULL, NULL);

        while (read(evdev_fd, &in, sizeof(struct input_event)) > 0)
        {
            if (in.type == EV_REL)
            {
                if (in.code == REL_X)
#if EVDEV_SWAP_AXES
                    y += in.value;
#else
                    x += in.value;
#endif
                else if (in.code == REL_Y)
#if EVDEV_SWAP_AXES
                    x += in.value;
#else
                    y += in.value;
#endif
            }
            else if (in.type == EV_ABS)
            {
                if (in.code == ABS_X)
#if EVDEV_SWAP_AXES
                    y = in.value;
#else
                    x = in.value;
#endif
                else if (in.code == ABS_Y)
#if EVDEV_SWAP_AXES
                    x = in.value;
#else
                    y = in.value;
#endif
                else if (in.code == ABS_MT_POSITION_X)
#if EVDEV_SWAP_AXES
                    y = in.value;
#else
                    x = in.value;
#endif
                else if (in.code == ABS_MT_POSITION_Y)
#if EVDEV_SWAP_AXES
                    x = in.value;
#else
                    y = in.value;
#endif
                else if (in.code == ABS_MT_TRACKING_ID)
                {
                    if (in.value == -1)
                    {
                        button = LV_INDEV_STATE_REL;
                        touch_up = 1;
                    }
                    else/* if ((in.value == 0) || (in.value == 1)) */
                    {
                        button = LV_INDEV_STATE_PR;
                    }
                }
            }
            else if (in.type == EV_KEY)
            {
                if (in.code == BTN_MOUSE || in.code == BTN_TOUCH)
                {
                    if (in.value == 0)
                    {
                        evdev_button = LV_INDEV_STATE_REL;
                        button = LV_INDEV_STATE_REL;
                    }
                    else if (in.value == 1)
                    {
                        evdev_button = LV_INDEV_STATE_PR;
                        button = LV_INDEV_STATE_PR;
                    }
                }
                else if (type == LV_INDEV_TYPE_KEYPAD)
                {
                    switch (in.code)
                    {
                    case KEY_BACKSPACE:
                        evdev_key_val = LV_KEY_BACKSPACE;
                        break;
                    case KEY_ENTER:
                        evdev_key_val = LV_KEY_ENTER;
                        break;
                    case KEY_UP:
                        evdev_key_val = LV_KEY_UP;
                        break;
                    case KEY_LEFT:
                        evdev_key_val = LV_KEY_PREV;
                        break;
                    case KEY_RIGHT:
                        evdev_key_val = LV_KEY_NEXT;
                        break;
                    case KEY_DOWN:
                        evdev_key_val = LV_KEY_DOWN;
                        break;
                    default:
                        evdev_key_val = 0;
                        break;
                    }
                }
            }
        }

        pthread_mutex_lock(&evdev_lock);
        evdev_val[0] = evdev_val[1];
        evdev_val[1].x = x;
        evdev_val[1].y = y;
        evdev_val[1].button = button;
        pthread_mutex_unlock(&evdev_lock);

#if FILE_DEBUG
        fprintf(raw_point, "%d\t%d\t%d\n", x, y, button);
        fflush(raw_point);
#endif
    }

    return NULL;
}

void evdev_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    int type = drv->type;
    lv_disp_t *disp = drv->disp;
    int hor_res = disp->driver->hor_res;
    int ver_res = disp->driver->ver_res;
    int raw_x, raw_y, button_state;
    int x, y;
    int tmp;
    int first_point = 0;

    if (type == LV_INDEV_TYPE_KEYPAD)
    {
        /* No data retrieved */
        data->key = evdev_key_val;
        data->state = evdev_button;
        return;
    }

    if (type != LV_INDEV_TYPE_POINTER)
        return;
    /*Store the collected data*/

    pthread_mutex_lock(&evdev_lock);

    raw_x = evdev_val[1].x;
    raw_y = evdev_val[1].y;
    button_state = evdev_val[1].button;
    if (evdev_val[0].button == LV_INDEV_STATE_REL)
    {
        first_point = 1;
        touch_up = 0;
    }
    else if (button_state && touch_up)
    {
        /* Some times evdev_read may lost the touch up events due to
         * the renderer has different period with touch screen report events,
         * so add a global variable touch_up to make sure the evdev_read will
         * not lost any touch up events.
         */
        first_point = 1;
        touch_up = 0;
    }

    pthread_mutex_unlock(&evdev_lock);

    switch (evdev_rot)
    {
    case 0:
    default:
        break;
    case 90:
        tmp = raw_x;
        raw_x = raw_y;
        raw_y = evdev_max_y - tmp;
        break;
    case 180:
        tmp = raw_x;
        raw_x = evdev_max_x - raw_y;
        raw_y = evdev_max_y - tmp;
        break;
    case 270:
        tmp = raw_x;
        raw_x = evdev_max_x - raw_y;
        raw_y = tmp;
        break;
    }

    if (evdev_calibrate)
    {
        raw_x = map(raw_x, evdev_min_x, evdev_max_x,
                    0, hor_res);
        raw_y = map(raw_y, evdev_min_y, evdev_max_y,
                    0, ver_res);
    }

    KalmanUpdate(&kfx, raw_x * 1.0, first_point);
    KalmanUpdate(&kfy, raw_y * 1.0, first_point);

    x = (int)kfx.v;
    y = (int)kfy.v;

    data->point.x = x;
    data->point.y = y;
    data->state = button_state;

    if (data->point.x < 0)
        data->point.x = 0;
    if (data->point.y < 0)
        data->point.y = 0;
    if (data->point.x >= hor_res)
        data->point.x = hor_res - 1;
    if (data->point.y >= ver_res)
        data->point.y = ver_res - 1;

#if FILE_DEBUG
    if (data->state)
    {
        fprintf(lv_raw_point, "%d\t%d\t%d\n", raw_x, raw_y, button_state);
        fflush(lv_raw_point);
        fprintf(lv_fix_point, "%d\t%d\t%d\n", data->point.x, data->point.y,
                data->state);
        fflush(lv_fix_point);
    }
#endif

    return ;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
int map(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif
