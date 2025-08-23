#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <linux/prctl.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/prctl.h>
#include <pthread.h>

#include <Rk_wifi.h>
#include "rk_ble_app.h"
#include "utility.h"
#include "bt_test.h"

/* Immediate wifi Service UUID */
#define AD_SERVICE_UUID16	"2222"
#define BLE_UUID_SERVICE	"00008888-0000-1000-8000-00805F9B34FB"
#define BLE_UUID_WIFI_CHAR	"00009999-0000-1000-8000-00805F9B34FB"
#define BLE_UUID_PROXIMITY	"7B931104-1810-4CBC-94DA-875C8067F845"

#define UUID_MAX_LEN 36

typedef enum {
	RK_BLE_WIFI_State_IDLE = 0,
	RK_BLE_WIFI_State_CONNECTTING,
	RK_BLE_WIFI_State_SUCCESS,
	RK_BLE_WIFI_State_FAIL,
	RK_BLE_WIFI_State_WRONGKEY_FAIL,
	RK_BLE_WIFI_State_DISCONNECT
} RK_BLE_WIFI_State_e;

const char *MSG_BLE_WIFI_LIST_FORMAT = "{\"cmd\":\"wifilists\", \"ret\":%s}";

static char rk_wifi_list_buf[20 * 1024];
static int scanr_len = 0, scanr_len_use = 0;

static pthread_t wificonfig_tid = 0;
static pthread_t wificonfig_scan_tid = 0;

static char wifi_ssid[256];
static char wifi_password[256];

//static unsigned int ble_mtu = 0;
static RkBtContent bt_content;

typedef int (*RK_blewifi_state_callback)(RK_BLE_WIFI_State_e state);

#define RK_BLE_DATA_CMD_LEN      12
#define RK_BLE_DATA_SSID_LEN     32
#define RK_BLE_DATA_PSK_LEN      32

/* 消息开头必须为0x01，结尾必须为0x04 */
typedef struct {
	unsigned char start;               // 段， 01Byte， 固定为 0x01
	char cmd[RK_BLE_DATA_CMD_LEN];     // 段， 12Byte， 值为："wifisetup"、"wifilists"
	char ssid[RK_BLE_DATA_SSID_LEN];   // 段， 32Byte， 需要连接的 WiFi ssid， 长度不足 32 的部分填充 0
	char psk[RK_BLE_DATA_PSK_LEN];     // 段， 32Byte， 需要连接的 wifi password， 长度不足 32 的部分填充 0
	//unsigned char end;               // 段， 01Byte， 固定位 0x04
} RockChipBleData;

static bool confirm_done = 0;

static int rk_blewifi_state_callback(RK_WIFI_RUNNING_State_e state, RK_WIFI_INFO_Connection_s *info)
{
	printf("[RK] %s state: %d\n", __func__, state);
	switch(state) {
	case RK_WIFI_State_CONNECTED:
		rk_ble_send_notify(BLE_UUID_WIFI_CHAR, "wifi ok", 7);
		break;
	case RK_WIFI_State_CONNECTFAILED:
	case RK_WIFI_State_CONNECTFAILED_WRONG_KEY:
		rk_ble_send_notify(BLE_UUID_WIFI_CHAR, "wifi fail", 9);
		break;
	default:
		break;
	}

	return 0;
}

void *rk_config_wifi_thread(void *arg)
{
	printf("[RK] rk_config_wifi_thread\n");

	prctl(PR_SET_NAME,"rk_config_wifi_thread");

	RK_wifi_connect(wifi_ssid, wifi_password, WPA3, NULL);

	return NULL;
}

void *rk_ble_send_data(void *arg)
{
	int len, send_max_len = 120;
	char *data;
	char *wifilist;

	prctl(PR_SET_NAME,"rk_ble_send_data");

scan_retry:
	printf("[RK] RK_wifi_scan ...\n");
	RK_wifi_scan();
	usleep(800000);

	//get scan list
	wifilist = RK_wifi_scan_r();

	//scan list is null
	if (wifilist == NULL)
		goto scan_retry;

	//scan list too few
	if (wifilist && (strlen(wifilist) < 3)) {
		free(wifilist);
		goto scan_retry;
	}

	//copy to static buff
	memset(rk_wifi_list_buf, 0, sizeof(rk_wifi_list_buf));
	snprintf(rk_wifi_list_buf, sizeof(rk_wifi_list_buf), MSG_BLE_WIFI_LIST_FORMAT, wifilist);
	scanr_len = strlen(rk_wifi_list_buf);
	scanr_len_use = 0;
	printf("[RK] wifi scan_r: %s, len: %d\n", rk_wifi_list_buf, scanr_len);

	//free
	free(wifilist);

	//max att mtu
	send_max_len = bt_content.ble_content.att_mtu;

	while (scanr_len) {
		printf("[RK] %s: wifi use: %d, remain len: %d\n", __func__, scanr_len_use, scanr_len);
		len = (scanr_len > send_max_len) ? send_max_len : scanr_len;
		data = rk_wifi_list_buf + scanr_len_use;

		//send data
		confirm_done = false;
		rk_ble_send_notify(BLE_UUID_WIFI_CHAR, data, len);

		//waiting write resp
		while (!confirm_done) {
			usleep(10 * 1000);
		}
		printf("[RK] received confirm\n");

		//resize
		scanr_len -= len;
		scanr_len_use += len;
	}

	return NULL;
}

static void ble_wifi_recv_data_cb(const char *uuid, char *data, int *len)
{
	if (!strcmp(uuid, BLE_UUID_WIFI_CHAR)) {
		if (strncmp(data, "wifi_scan", 9) == 0) {
			pthread_create(&wificonfig_scan_tid, NULL, rk_ble_send_data, NULL);
		} else if (strncmp(data, "wifi_setup", 10) == 0) {
			char cmd[12];
			memset(cmd, 0, 12);
			memset(wifi_ssid, 0, 256);
			memset(wifi_password, 0, 256);
			sscanf(data, "%s %s %s", cmd, wifi_ssid, wifi_password);
			printf("cmd: %s, ssid: %s, psk: %s\n", cmd, wifi_ssid, wifi_password);
			pthread_create(&wificonfig_tid, NULL, rk_config_wifi_thread, NULL);
		}
	}
}

static void bt_test_ble_recv_data_callback(const char *uuid, char *data, int *len, RK_BLE_GATT_STATE state)
{
	switch (state) {
	//SERVER ROLE
	case RK_BLE_GATT_SERVER_READ_BY_REMOTE:
		//The remote dev reads characteristic and put data to *data.
		printf("+++ ble server is read by remote uuid: %s\n", uuid);
		*len = strlen("hello rockchip");
		memcpy(data, "hello rockchip", strlen("hello rockchip"));
		break;
	case RK_BLE_GATT_SERVER_WRITE_BY_REMOTE:
		//The remote dev writes data to characteristic so print there.
		printf("+++ ble server is writeen by remote uuid: %s\n", uuid);
		for (int i = 0 ; i < *len; i++) {
			printf("%02x ", data[i]);
		}
		printf("\n");
		//wifi config handle
		ble_wifi_recv_data_cb(uuid, data, len);
		break;
	case RK_BLE_GATT_SERVER_ENABLE_NOTIFY_BY_REMOTE:
	case RK_BLE_GATT_SERVER_DISABLE_NOTIFY_BY_REMOTE:
		//The remote dev enable notify for characteristic
		printf("+++ ble server notify is %s by remote uuid: %s\n",
				(state == RK_BLE_GATT_SERVER_ENABLE_NOTIFY_BY_REMOTE) ? "enable" : "disabled", uuid);
		break;
	case RK_BLE_GATT_MTU:
		bt_content.ble_content.att_mtu = *(uint16_t *)data;
		printf("+++ ble server MTU: %d ===\n", *(uint16_t *)data);
		break;
	case RK_BLE_GATT_SERVER_INDICATE_RESP_BY_REMOTE:
		//The service sends notify to remote dev and recv indicate from remote dev.
		printf("+++ ble server receive remote indicate resp uuid: %s\n", uuid);

		//set confirm flag
		confirm_done = true;
		break;
	default:
		break;
	}
}

void rk_ble_wifi_init(char *data)
{
	RkBleGattService *gs;
	struct bt_conf conf;
	static char *chr_props[] = { "read", "write", "indicate", "write-without-response", NULL };

	printf(" %s \n", __func__);

	memset(&bt_content, 0, sizeof(RkBtContent));

	//BREDR CLASS BT NAME
	memset(bt_content.bt_name, 0, sizeof(bt_content.bt_name));
	strcpy(bt_content.bt_name, "rkbt");

	//BLE NAME
	memset(bt_content.ble_content.ble_name, 0, sizeof(bt_content.ble_content.ble_name));
	strcpy(bt_content.ble_content.ble_name, "rkble");

	//1234
	FILE *fp;
    fp = fopen("/data/bt_id.txt", "r");
    if (fp) {
		char bt_id[6];
        fscanf(fp, "%s", bt_id); // Read the MAC address
        fclose(fp);

		char buf[38];
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "%s_%s", bt_content.bt_name, bt_id);
		memcpy(bt_content.bt_name, buf, sizeof(bt_content.bt_name));

		memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "%s_%s", bt_content.ble_content.ble_name, bt_id);
		memcpy(bt_content.ble_content.ble_name, buf, sizeof(bt_content.ble_content.ble_name));
    }

	//IO CAPABILITY
	bt_content.io_capability = IO_CAPABILITY_DISPLAYYESNO;

	//enable ble
	bt_content.profile = PROFILE_BLE;
	if (bt_content.profile & PROFILE_BLE) {
		/* GATT SERVICE/CHARACTERISTIC */
		//SERVICE_UUID
		gs = &(bt_content.ble_content.gatt_instance[0]);
		gs->server_uuid.uuid = BLE_UUID_SERVICE;
		gs->chr_uuid[0].uuid = BLE_UUID_WIFI_CHAR;
		gs->chr_uuid[0].chr_props = chr_props;
		gs->chr_cnt = 1;

		bt_content.ble_content.srv_cnt = 1;

		/* Fill adv data */
		/* Appearance */
		bt_content.ble_content.Appearance = 0x0080;

		/* manufacturer data */
		bt_content.ble_content.manufacturer_id = 0x0059;
		for (int i = 0; i < 16; i++)
			bt_content.ble_content.manufacturer_data[i] = i;

		/* Service UUID */
		bt_content.ble_content.adv_server_uuid.uuid = AD_SERVICE_UUID16;

		//callback
		bt_content.ble_content.cb_ble_recv_fun = bt_test_ble_recv_data_callback;
	}

	rk_bt_register_state_callback(bt_test_state_cb);
	rk_bt_register_vendor_callback(bt_test_vendor_cb);

	//default state
	bt_content.init = false;
	bt_content.connecting = false;
	bt_content.scanning = false;
	bt_content.discoverable = false;
	bt_content.pairable = false;
	bt_content.power = false;

	//global init
	//ancs_is_support = false;
	//ancs_is_enable = false;

	//bt config file
	memset(&conf, 0, sizeof(struct bt_conf));
	//both BR/EDR and LE enabled, "dual", "le" or "bredr"
	conf.mode = "le";
	conf.BleName = bt_content.ble_content.ble_name;
	conf.gatt_client_cache = "no";
	create_bt_conf(&conf);

	rk_debug_init(true);

	rk_bt_init(&bt_content);

	//enable adv
	printf("Start BLE ADV ....\n");
	rk_ble_adv_start();

	//enble wifi
	RK_wifi_register_callback(rk_blewifi_state_callback);
	RK_wifi_enable(1, "/data/cfg/wpa_supplicant.conf");

	printf(" %s end \n", __func__);
	return;
}

void rk_ble_wifi_deinit(char *data)
{
	printf(" %s \n", __func__);

	//disable wifi
	RK_wifi_enable(0, NULL);

	//disable adv
	rk_ble_adv_stop();

	rk_bt_deinit();

	printf(" %s end\n", __func__);
}

void rk_ble_wifi_init_onoff_test(char *data)
{
	int test_cnt = 5000, cnt = 0;

	if (data)
		test_cnt = atoi(data);
	printf("%s test times: %d(%d)\n", __func__, test_cnt, data ? atoi(data) : 0);

	while (cnt < test_cnt) {
		rk_ble_wifi_init(NULL);
		rk_ble_wifi_deinit(NULL);
		printf("BLE WiFi ON/OFF LOOPTEST CNT: [====== %d ======]\n", ++cnt);
	}
}

