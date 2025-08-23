#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdatomic.h>
//#include <check.h>

#include "utility.h"
#include "Rk_wifi.h"
#include "Rk_softap.h"

typedef struct {
	pthread_mutex_t wifi_mutex;
	/* updates notification */
	pthread_cond_t cond;
	bool power;

	pthread_mutex_t state_mtx;
    int wifi_state;
} WifiStatus;
WifiStatus wifi_status;

static void printf_connect_info(RK_WIFI_INFO_Connection_s *info)
{
	if (!info)
		return;

	printf("	id:          %d\n", info->id);
	printf("	bssid:       %s\n", info->bssid);
	printf("	ssid:        %s\n", info->ssid);
	printf("	freq:        %d\n", info->freq);
	printf("	mode:        %s\n", info->mode);
	printf("	wpa_state:   %s\n", info->wpa_state);
	printf("	ip_address:  %s\n", info->ip_address);
	printf("	mac_address: %s\n", info->mac_address);
	printf("	key_mgmt:    %s\n", info->key_mgmt);
}

static void *__rk_wifi_state_callback(void *data);
static int rk_wifi_state_callback(RK_WIFI_RUNNING_State_e state, RK_WIFI_INFO_Connection_s *info)
{
	pthread_t tid;
	pthread_attr_t attr;

	printf("%s state: %d\n", __func__, state);
	printf_connect_info(info);

	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, __rk_wifi_state_callback, (void *)(long)state);
	pthread_detach(tid);
	pthread_attr_destroy(&attr);

	return 0;
}

static void *__rk_wifi_state_callback(void *data)
{
	RK_WIFI_RUNNING_State_e state = (RK_WIFI_RUNNING_State_e)(long)data;

	pthread_mutex_lock(&wifi_status.state_mtx);
	wifi_status.wifi_state = state;
	pthread_mutex_unlock(&wifi_status.state_mtx);

	if (state == RK_WIFI_State_CONNECTED) {
		printf("RK_WIFI_State_CONNECTED\n");
	} else if (state == RK_WIFI_State_CONNECTFAILED) {
		printf("RK_WIFI_State_CONNECTFAILED\n");
	} else if (state == RK_WIFI_State_CONNECTFAILED_WRONG_KEY) {
		printf("RK_WIFI_State_CONNECTFAILED_WRONG_KEY\n");
	} else if (state == RK_WIFI_State_OPEN) {
		pthread_mutex_lock(&wifi_status.wifi_mutex);
		wifi_status.power = true;
		pthread_mutex_unlock(&wifi_status.wifi_mutex);
		pthread_cond_signal(&wifi_status.cond);
		printf("RK_WIFI_State_OPEN\n");
	} else if (state == RK_WIFI_State_OFF) {
		pthread_mutex_lock(&wifi_status.wifi_mutex);
		wifi_status.power = false;
		pthread_mutex_unlock(&wifi_status.wifi_mutex);
		pthread_cond_signal(&wifi_status.cond);
		printf("RK_WIFI_State_OFF\n");
	} else if (state == RK_WIFI_State_DISCONNECTED) {
		printf("RK_WIFI_State_DISCONNECTED\n");
	} else if (state == RK_WIFI_State_SCAN_RESULTS) {
		char *scan_r;
		printf("RK_WIFI_State_SCAN_RESULTS\n");
		scan_r = RK_wifi_scan_r();
		//printf("%s\n", scan_r);
		free(scan_r);
	} else if (state == RK_WIFI_State_CONNECTING) {
		printf("RK_WIFI_State_CONNECTING\n");
	}

	return 0;
}

static int rk_wifi_softap_state_callback(RK_SOFTAP_STATE state, const char* data)
{
	switch (state) {
	case RK_SOFTAP_STATE_CONNECTTING:
		printf("RK_SOFTAP_STATE_CONNECTTING\n");
		break;
	case RK_SOFTAP_STATE_DISCONNECT:
		printf("RK_SOFTAP_STATE_DISCONNECT\n");
		break;
	case RK_SOFTAP_STATE_FAIL:
		printf("RK_SOFTAP_STATE_FAIL\n");
		break;
	case RK_SOFTAP_STATE_SUCCESS:
		printf("RK_SOFTAP_STATE_SUCCESS\n");
		break;
	default:
		break;
	}

	return 0;
}

void rk_wifi_softap_start(char *data)
{
	printf("%s enter\n", __func__);
	RK_softap_register_callback(rk_wifi_softap_state_callback);
	if (0 != RK_softap_start("Rockchip-SoftAp", RK_SOFTAP_TCP_SERVER)) {
		return;
	}
}

void rk_wifi_softap_stop(char *data)
{
	RK_softap_stop();
}

void rk_wifi_close(char *data)
{
	if (RK_wifi_enable(0, NULL) < 0)
		printf("RK_wifi_enable 0 fail!\n");
}

void rk_wifi_open(char *data)
{
	printf("%s: ", __func__);

	if (access("/data/cfg/wpa_supplicant.conf", F_OK) == -1) {
		exec_command_system("mkdir -p /data/cfg");
		exec_command_system("cp /etc/wpa_supplicant.conf /data/cfg/wpa_supplicant.conf");
	}

	rk_debug_init(true);
	RK_wifi_set_dhcp_server(DHCP_DHCPCD);
	RK_wifi_register_callback(rk_wifi_state_callback);
	if (RK_wifi_enable(1, "/data/cfg/wpa_supplicant.conf") < 0)
		printf("RK_wifi_enable 1 fail!\n");
}

void rk_wifi_version(char *data)
{
	printf("rk wifi version: %s\n", RK_wifi_version());
}

//data: fish1 rk12345678 WPA NULL
void rk_wifi_connect(char *data)
{
	char *ssid = NULL;
	char *psk = NULL;
	char *key_mgmt = NULL;
	char *bssid = NULL;
	RK_WIFI_KEY_MGMT mgmt = WPA;

	if (data == NULL) {
		printf("%s: invalid input example: ssid psk WPA/WEP/WPA3/NONE]\n", __func__);
		return;
	}

	ssid = strtok(data, " ");
	if (ssid)
		psk = strtok(NULL, " ");
	if (psk)
		key_mgmt = strtok(NULL, " ");
	if (key_mgmt)
		bssid = strtok(NULL, " ");

	if (!key_mgmt){
		printf("%s: invalid input example: ssid psk WPA/WEP/WPA3/NONE]\n", __func__);
		return;
	}

	if (!strcasecmp(key_mgmt, "NONE")) {
		mgmt = NONE;
		psk = NULL;
	} else if (!strcasecmp(key_mgmt, "WEP"))
		mgmt = WEP;
	else if (!strcasecmp(key_mgmt, "WPA3"))
		mgmt = WPA3;
	else if (!strcasecmp(key_mgmt, "WPA"))
		mgmt = WPA;
	else if (!strcasecmp(key_mgmt, "WPA23"))
		mgmt = WPA2_WPA3;

	printf("%s: ssid: %s psk: %s key:%s:%d bssid:%s\n", __func__, ssid, psk, key_mgmt, mgmt, bssid);

	//if (RK_wifi_connect("HKH- -黄开	辉-@#\\/\"\\\\\"", "12345678", mgmt, bssid) < 0)
	if (RK_wifi_connect(ssid, psk, mgmt, bssid) < 0)
		printf("RK_wifi_connect1 fail!\n");
}

void rk_wifi_scan(char *data)
{
	if (RK_wifi_scan() < 0)
		printf("RK_wifi_scan fail!\n");
}

void rk_wifi_getSavedInfo(char *data)
{
	RK_WIFI_SAVED_INFO_s *wsi;
	int ap_cnt = 0;

	RK_wifi_getSavedInfo(&wsi, &ap_cnt);
	if (ap_cnt <= 0) {
		printf("not found saved ap!\n");
		return;
	}

	for (int i = 0; i < ap_cnt; i++) {
		printf("id: %d, name: %s, bssid: %s, state: %s key_mgmt: %s\n",
					wsi[i].id,
					wsi[i].ssid,
					wsi[i].bssid,
					wsi[i].state,
					wsi[i].key_mgmt);
	}

	if (wsi != NULL)
		free(wsi);
}

void rk_wifi_getConnectionInfo(char *data)
{
	RK_WIFI_INFO_Connection_s info;

	if (!RK_wifi_running_getConnectionInfo(&info))
		printf_connect_info(&info);
}

void rk_wifi_connect_with_ssid(char *data)
{
	char *ssid, *key_mgmt = "";
	RK_WIFI_KEY_MGMT mgmt = WPA;

	if (data == NULL) {
		printf("%s: invalid input example: ssid psk WPA/WEP/WPA3/NONE]\n", __func__);
		return;
	}

	ssid = strtok(data, " ");
	if (ssid)
		key_mgmt = strtok(NULL, " ");

	if (key_mgmt == NULL)
		printf("%s: invalid input example: ssid psk WPA/WEP/WPA3/NONE]\n", __func__);

	if (!strcasecmp(key_mgmt, "NONE")) {
		mgmt = NONE;
	} else if (!strcasecmp(key_mgmt, "WEP"))
		mgmt = WEP;
	else if (!strcasecmp(key_mgmt, "WPA3"))
		mgmt = WPA3;
	else if (!strcasecmp(key_mgmt, "WPA"))
		mgmt = WPA;
	else if (!strcasecmp(key_mgmt, "WPA23"))
		mgmt = WPA2_WPA3;

	if (key_mgmt == NULL)
		printf("%s: invalid input example: ssid psk WPA/WEP/WPA3/NONE]\n", __func__);

	if (RK_wifi_connect_with_ssid(data, mgmt) < 0)
		printf("RK_wifi_connect_with_ssid fail!\n");
}

void rk_wifi_cancel(void *data)
{
	if (RK_wifi_cancel() < 0)
		printf("RK_wifi_cancel fail!\n");
}

void rk_wifi_forget_with_ssid(char *data)
{
	char *ssid, *key_mgmt = "";
	RK_WIFI_KEY_MGMT mgmt = WPA;

	if (data == NULL) {
		printf("%s: invalid input example: ssid psk WPA/WEP/WPA3/NONE]\n", __func__);
		return;
	}

	ssid = strtok(data, " ");
	if (ssid)
		key_mgmt = strtok(NULL, " ");
	if (key_mgmt == NULL)
		printf("%s: invalid input example: ssid psk WPA/WEP/WPA3/NONE]\n", __func__);

	if (!strcasecmp(key_mgmt, "NONE")) {
		mgmt = NONE;
	} else if (!strcasecmp(key_mgmt, "WEP"))
		mgmt = WEP;
	else if (!strcasecmp(key_mgmt, "WPA3"))
		mgmt = WPA3;
	else if (!strcasecmp(key_mgmt, "WPA"))
		mgmt = WPA;
	else if (!strcasecmp(key_mgmt, "WPA23"))
		mgmt = WPA2_WPA3;

	if (RK_wifi_forget_with_ssid(data, mgmt) < 0) {
		printf("rk_wifi_forget_with_ssid fail!\n");
	}
}

void rk_wifi_disconnect(char *data)
{
	RK_wifi_disconnect_network();
}

/**
 * @brief Test case for wifi on/off
 *
 * This test case is to test whether the wifi on/off function works
 * correctly. The test case will turn on the wifi, scan the wifi,
 * and turn off the wifi, and repeat the test for 5000 times.
 *
 * @return None
 */
void rk_wifi_onoff_test(char *data)
{
	int test_cnt = 0x7FFFFFFF; /* Default test times */

	pthread_mutex_init(&wifi_status.wifi_mutex, NULL);
	pthread_cond_init(&wifi_status.cond, NULL);

	/*
	 * Test the wifi on/off function for test_cnt times,
	 * the wifi power on/off will be notified by the callback function
	 */
	for (int cnt = 0; cnt < test_cnt; cnt++) {
		printf("Testing wifi open/close: %d\n", cnt);

		/* Register the callback function */
		RK_wifi_register_callback(rk_wifi_state_callback);

		/* Open wifi function */
		RK_wifi_enable(1, "/data/cfg/wpa_supplicant.conf");
		printf("Wifi power on\n");

		/*
		 * Wait for wifi power on,
		 * the callback function will set the power to true.
		 */
		pthread_mutex_lock(&wifi_status.wifi_mutex);
		while (!wifi_status.power)
			pthread_cond_wait(&wifi_status.cond, &wifi_status.wifi_mutex);
		pthread_mutex_unlock(&wifi_status.wifi_mutex);

		/* Scan wifi function */
		RK_wifi_scan();

		/* Close wifi function */
		RK_wifi_enable(0, NULL);
		printf("Wifi power off\n");

		/*
		 * Wait for wifi power off,
		 * the callback function will set the power to false.
		 */
		pthread_mutex_lock(&wifi_status.wifi_mutex);
		while (wifi_status.power)
			pthread_cond_wait(&wifi_status.cond, &wifi_status.wifi_mutex);
		pthread_mutex_unlock(&wifi_status.wifi_mutex);
	}

	pthread_mutex_destroy(&wifi_status.wifi_mutex);
	pthread_cond_destroy(&wifi_status.cond);
}

#if 0
START_TEST(test_wifi_connect_invalid) {
	rk_wifi_connect_invalid_test();
} END_TEST

START_TEST(test_wifi_on_off) {
	rk_wifi_onoff_test();
} END_TEST

void rk_wifi_auto_test(char *data)
{

	Suite *s = suite_create(__FILE__);
	TCase *tc = tcase_create(__FILE__);
	SRunner *sr = srunner_create(s);

	suite_add_tcase(s, tc);

	tcase_add_test(tc, test_wifi_on_off);
	//tcase_add_test(tc, test_wifi_connect_invalid);

	srunner_run_all(sr, CK_ENV);
	int nf = srunner_ntests_failed(sr);
	srunner_free(sr);

	return nf == 0 ? 0 : 1;
}
#endif
