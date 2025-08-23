/**
 * @file evdev.h
 *
 */

#ifndef EVDEV_H
#define EVDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USE_EVDEV) && USE_EVDEV

#include <lvgl.h>

/*********************
 *      DEFINES
 *********************/
#undef EVDEV_NAME
#define EVDEV_NAME              "/dev/input/event2"
#define EVDEV_SWAP_AXES         0
#define DEFAULT_EVDEV_HOR_MIN   0
#define DEFAULT_EVDEV_HOR_MAX   720
#define DEFAULT_EVDEV_VER_MIN   0
#define DEFAULT_EVDEV_VER_MAX   1280

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the evdev
 */
int evdev_init(lv_display_t *disp, int rot);
/**
 * reconfigure the device file for evdev
 * @param dev_name set the evdev device filename
 * @return true: the device file set complete
 *         false: the device file doesn't exist current system
 */
int evdev_set_file(lv_display_t *disp, char *dev_name);
/**
 * Get the current position and state of the evdev
 * @param data store the evdev data here
 * @return false: because the points are not buffered, so no more data to be read
 */
void evdev_read(lv_indev_t *drv, lv_indev_data_t *data);

#if USE_SENSOR
int evdev_init_psensor(void);

void *evdev_get_psensor(void);

int evdev_init_lsensor(void);

void *evdev_get_lsensor(void);

void evdev_sensor_read(lv_indev_t *drv, lv_indev_data_t *data);
#endif

/**********************
 *      MACROS
 **********************/

#endif /* defined(USE_EVDEV) && USE_EVDEV */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* EVDEV_H */

