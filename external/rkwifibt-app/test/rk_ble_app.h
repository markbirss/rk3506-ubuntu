#ifndef __RK_BLE_APP_H__
#define __RK_BLE_APP_H__

#include <RkBle.h>

#ifdef __cplusplus
extern "C" {
#endif

void rk_ble_wifi_init(char *data);
void rk_ble_wifi_deinit(char *data);
void rk_bt_wifi_init_onoff_test(char *data);
void rk_ble_wifi_init_onoff_test(char *data);

#ifdef __cplusplus
}
#endif

#endif /* __RK_BLE_APP_H__ */
