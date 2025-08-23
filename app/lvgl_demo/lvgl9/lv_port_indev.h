/**
 * @file lv_port_indev.h
 *
 */

#ifndef LV_PORT_INDEV_H
#define LV_PORT_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

void lv_port_indev_init(int rot);

#if defined(USE_SENSOR) && USE_SENSOR
lv_indev_t *lv_port_indev_get_lsensor(void);

lv_indev_t *lv_port_indev_get_psensor(void);
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_INDEV_H*/

