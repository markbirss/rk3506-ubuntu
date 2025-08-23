#ifndef __WIFI_TEST_H__
#define __WIFI_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

void rk_wifi_softap_start(char *data);
void rk_wifi_softap_stop(char *data);
void rk_wifi_open(char *data);
void rk_wifi_close(char *data);
void rk_wifi_connect(char *data);
void rk_wifi_getSavedInfo(char *data);
void rk_wifi_getConnectionInfo(char *data);
void rk_wifi_connect_with_ssid(char *data);
void rk_wifi_cancel(char *data);
void rk_wifi_forget_with_ssid(char *data);
void rk_wifi_disconnect(char *data);
void rk_wifi_version(char *data);
void rk_wifi_onoff_test(char *data);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_TEST_H__ */
