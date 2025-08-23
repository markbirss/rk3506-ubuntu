#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <sys/time.h>

#include <RkBtBase.h>
#include <RkBtSink.h>
#include <RkBtSource.h>
#include <RkBle.h>
#include <RkBleClient.h>

//vendor code for broadcom
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <sys/ioctl.h>
#include <glib.h>

enum{
	A2DP_SOURCE,
	A2DP_SINK
};

enum{
	ACL_NORMAL_PRIORITY,
	ACL_HIGH_PRIORITY
};
int vendor_set_high_priority(char *ba, uint8_t priority, uint8_t direction);
//vendor code for broadcom end

#include "bt_test.h"
#include "utility.h"

/* AD SERVICE_UUID */
#define AD_SERVICE_UUID16	"1111"
#define AD_SERVICE_UUID32	"00002222"
#define AD_SERVICE_UUID128	"00002222-0000-1000-8000-00805f9b34fb"

/* GAP/GATT Service UUID */
#define BLE_UUID_SERVICE	"00001111-0000-1000-8000-00805F9B34FB"
#define BLE_UUID_WIFI_CHAR	"00002222-0000-1000-8000-00805F9B34FB"
#define BLE_UUID_SEND		"dfd4416e-1810-47f7-8248-eb8be3dc47f9"
#define BLE_UUID_RECV		"9884d812-1810-4a24-94d3-b2c11a851fac"
#define SERVICE_UUID		"00001910-0000-1000-8000-00805f9b34fb"

#define BLE_UUID_SERVICE1	"00001234-0000-1000-8000-00805F9B34FB"
#define BLE_UUID_WIFI_CHAR1	"00001235-0000-1000-8000-00805F9B34FB"
#define BLE_UUID_SEND1		"00001236-1810-47f7-8248-eb8be3dc47f9"
#define BLE_UUID_RECV1		"00001237-1810-4a24-94d3-b2c11a851fac"
#define SERVICE_UUID1		"00001238-0000-1000-8000-00805f9b34fb"
#define TMP "00002a05-0000-1000-8000-00805f9b34fb"


/*Notification Source: UUID 9FBF120D-6301-42D9-8C58-25E699A21DBD (notifiable)*/
#define ANCS_NOTIFICATION_SOURCE "9FBF120D-6301-42D9-8C58-25E699A21DBD"
/* Control Point: UUID 69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9 (writeable with response) */
#define ANCS_CONTROL_POINT "69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9"
/* Data Source: UUID 22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB (notifiable) */
#define ANCS_DATA_SOURCE "22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB"

static bool ancs_is_support = false;
static bool ancs_is_enable = false;
static uint16_t last_info = 0;

static int spp_fd = 0;

//static struct timeval start, now;
//static ssize_t totalBytes;

static void bt_test_ble_recv_data_callback(const char *uuid, char *data, int *len, RK_BLE_GATT_STATE state);

static int ble_direct_enable_adv(void);
static int ble_direct_close_adv(void);
static int ble_direct_set_adv_param(char *ble_name , char *ble_mac_addr);
static volatile bool ble_direct_flag = false;
/*
 * This structure must be initialized before use!
 *
 * The following variables will be updated by librkwifibt.so
 * bool init;
 * bool power;
 * bool pairable;
 * bool discoverable;
 * bool scanning;
 */
static RkBtContent bt_content;

static void bt_init_static_var(void)
{
	ancs_is_support = false;
	ancs_is_enable = false;
	ble_direct_flag = false;
}

/* BT base api */

void at_evt_callback(char *at_evt);

static char rfcomm_remote_address[18];
gboolean bt_open_rfcomm(gpointer data)
{
	rk_bt_rfcomm_open(rfcomm_remote_address, at_evt_callback);

	return false;
}

/*
gboolean bt_reenable_ancs(gpointer data)
{
	//ble paired && connect == true
	printf("	ble info: 0x%x\n", rdev->info & 0x0F00);
	if (((last_info & 0x0F00) != 0x0F00) &&
		((rdev->info & 0x0F00) == 0x0F00)) {
		RK_BLE_CLIENT_SERVICE_INFO info;
		printf("	ble addr: %s\n", rdev->remote_address);
		if (!rk_ble_client_get_service_info(rdev->remote_address, &info)) {
			printf("	service cnt: %d\n", info.service_cnt);
			for(int i = 0; i < info.service_cnt; i++) {
				printf("	search ancs: %s\n", info.service[i].uuid);
				if (!strcasecmp("7905F431-B5CE-4E99-A40F-4B1E122D00D0",
					info.service[i].uuid))
					ancs_is_support = true;
			}
		} else
			printf("	get service info failed\n");
	
		if (ancs_is_support && ancs_is_enable) {
			bool notifying = rk_ble_client_is_notifying(ANCS_NOTIFICATION_SOURCE);
			printf("%s notifying %s\n", ANCS_NOTIFICATION_SOURCE, notifying ? "yes" : "no");
			rk_ble_client_ancs(true);
		}
	}
}
*/

gboolean bt_reconect_last_dev(gpointer data)
{
	int i, count;
	struct remote_dev *rdev = NULL;
	printf("bt_reconect_last_dev\n");

	//Get all devices
	if (bt_get_devices(&rdev, &count) < 0) {
		printf("Can't get scan list!");
		return false;
	}

	if (count == 0)
		return false;

	printf("rdev: %p, count: %d\n", rdev, count);
	i = count - 1;
	for (; i >= 0; i--) {
		if (rdev[i].connected)
			printf("Connected Device %s (%s:%s)\n",
					rdev[i].remote_address,
					rdev[i].remote_address_type,
					rdev[i].remote_alias);
		else
			printf("%s Device %s (%s:%s)\n",
				rdev[i].paired ? "Paired" : "Scaned",
				rdev[i].remote_address,
				rdev[i].remote_address_type,
				rdev[i].remote_alias);

		if (!rdev[i].connected && rdev[i].paired) {
			//printf("Reconnect device %s\n", rdev[i].remote_address);
			//rk_adapter_connect(rdev[i].remote_address, NULL);
			rk_bt_connect_by_addr(rdev[i].remote_address, "bredr");
			return false;
		}
	}
	return false;
}

/*
 * !!!Never write or call delaying or blocking code or functions within this function.
 * !!!切勿在此函数内编写或调用延迟或阻塞的代码或函数。
 *
 * !!!The rdev of some events is NULL. You must determine whether rdev is NULLL, otherwise a crash will occur.
 * !!!某些event的rdev是NULL，必须判断rdev是否为NULLL,否则会出现crash
 */
void bt_test_state_cb(RkBtRemoteDev *rdev, RK_BT_STATE state)
{
	switch (state) {
	//BASE STATE
	case RK_BT_STATE_TURNING_ON:
		printf("++ RK_BT_STATE_TURNING_ON\n");
		break;
	case RK_BT_STATE_INIT_ON:
		printf("++ RK_BT_STATE_INIT_ON\n");
		//only test
		//exec_command_system("hciconfig hci0 reset");
		break;
	case RK_BT_STATE_INIT_OFF:
		printf("++ RK_BT_STATE_INIT_OFF\n");
		break;

	//SCAN STATE
	case RK_BT_STATE_SCAN_NEW_REMOTE_DEV:
		if (rdev != NULL) {
			if (rdev->paired)
				printf("+ PAIRED_DEV: [%s|%d]:%s:%s\n", rdev->remote_address, rdev->rssi,
						rdev->remote_address_type, rdev->remote_alias);
			else
				printf("+ SCAN_NEW_DEV: [%s|%d]:%s:%s\n", rdev->remote_address, rdev->connected,
						rdev->remote_address_type, rdev->remote_alias);
		}
		break;
	case RK_BT_STATE_SCAN_CHG_REMOTE_DEV:
		if (rdev != NULL) {
			printf("+ %s: addr: %s, rssi: %d, type: %s, name: %s|%s\n",
					rdev->connected ? "CONN_CHG_DEV" : "SCAN_CHG_DEV",
					rdev->remote_address,
					rdev->rssi,
					rdev->remote_address_type,
					rdev->remote_alias,
					rdev->change_name);

			if (!strcmp(rdev->change_name, "UUIDs")) {
				for (int index = 0; index < 36; index++) {
					if (!strcmp(rdev->remote_uuids[index], "NULL"))
						break;
					printf("\tUUIDs: %s\n", rdev->remote_uuids[index]);
				}
			} else if (!strcmp(rdev->change_name, "Icon")) {
				printf("\tIcon: %s\n", rdev->icon);
			} else if (!strcmp(rdev->change_name, "Class")) {
				printf("\tClass: 0x%x\n", rdev->cod);
			} else if (!strcmp(rdev->change_name, "Modalias")) {
				printf("\tModalias: %s\n", rdev->modalias);
			} else if (!strcmp(rdev->change_name, "MTU")) {
				printf("CONN_CHG_DEV att_mtu: %d\n", rdev->att_mtu);
				bt_content.ble_content.att_mtu = rdev->att_mtu;
			} else if (!strcmp(rdev->change_name, "Reason")) {
				printf("CONN_CHG_DEV dis_dev_addr_type: %s, reason: %s\n",
						rdev->discon_addr_type ? "ble_device" : "bredr_device",
						DIS_REASON_ID[rdev->discon_reason]);
			} else if (!strcmp(rdev->change_name, "ServicesResolved")) {
				//re-enable ancs
				if (ancs_is_support && ancs_is_enable && rdev->svc_refreshed) {
					bool notifying = rk_ble_client_is_notifying(ANCS_NOTIFICATION_SOURCE);
					printf("%s notifying %s\n", ANCS_NOTIFICATION_SOURCE, notifying ? "yes" : "no");
					rk_ble_client_ancs(true);
				}
			}
		}
		break;
	case RK_BT_STATE_SCAN_DEL_REMOTE_DEV:
		if (rdev != NULL)
			printf("+ SCAN_DEL_DEV: [%s]:%s:%s\n", rdev->remote_address,
					rdev->remote_address_type, rdev->remote_alias);
		//bt_init_static_var();
		break;

	//LINK STATE
	case RK_BT_STATE_BLE_CONNECTED:
	case RK_BT_STATE_BLE_DISCONN:
		if (rdev != NULL)
			printf("+ %s [%s|%d]:%s:%s\n", rdev->ble_connected ? "STATE_BLE_CONNECTED" : "STATE_BLE_DISCONNECTED",
					rdev->remote_address,
					rdev->rssi,
					rdev->remote_address_type,
					rdev->remote_alias);
		break;
	case RK_BT_STATE_CONNECTED:
	case RK_BT_STATE_DISCONN:
		if (rdev != NULL)
			printf("+ %s [%s|%d]:%s:%s\n", rdev->br_connected ? "STATE_CONNECTED" : "STATE_DISCONNECTED",
					rdev->remote_address,
					rdev->rssi,
					rdev->remote_address_type,
					rdev->remote_alias);

		if (state == RK_BT_STATE_CONNECTED && !strcmp(rdev->remote_address_type, "public")) {
				memcpy(bt_content.connected_a2dp_addr, rdev->remote_address, 18);
		}

		if (ble_direct_flag && (state == RK_BT_STATE_DISCONN)) {
			ble_direct_enable_adv();
		}

		break;
	case RK_BT_STATE_INFO_CHANGE:
		if (rdev != NULL) {
			if (last_info != rdev->info)
				last_info = rdev->info;
		}

		break;
	case RK_BT_STATE_BLE_PAIRED:
	case RK_BT_STATE_BLE_PAIR_NONE:
		if (rdev != NULL)
			printf("+ %s [%s|%d]:%s:%s\n", rdev->ble_paired ? "STATE_BLE_PAIRED" : "STATE_BLE_PAIR_NONE",
					rdev->remote_address,
					rdev->rssi,
					rdev->remote_address_type,
					rdev->remote_alias);
		break;
	case RK_BT_STATE_PAIRED:
	case RK_BT_STATE_PAIR_NONE:
		if (rdev != NULL)
			printf("+ %s [%s|%d]:%s:%s\n", rdev->br_paired ? "STATE_PAIRED" : "STATE_PAIR_NONE",
					rdev->remote_address,
					rdev->rssi,
					rdev->remote_address_type,
					rdev->remote_alias);
		break;
	case RK_BT_STATE_BONDED:
	case RK_BT_STATE_BLE_BONDED:
		if (rdev != NULL)
			printf("+ %s [%s|%d]:%s:%s\n", rdev->br_bonded ? "STATE_BONDED" : "STATE_BLE_BONDED",
				rdev->remote_address,
				rdev->rssi,
				rdev->remote_address_type,
				rdev->remote_alias);
		break;
	case RK_BT_STATE_DEL_DEV_OK:
		if (rdev != NULL)
			printf("+ RK_BT_STATE_DEL_DEV_OK: %s:%s:%s\n",
				rdev->remote_address,
				rdev->remote_address_type,
				rdev->remote_alias);
		break;
	case RK_BT_STATE_PAIR_FAILED:
		printf("+ STATE_BOND/PAIR FAILED\n");
		break;

	case RK_BT_STATE_CONNECT_FAILED:
		if (rdev != NULL)
			printf("+ STATE_FAILED [%s|%d]:%s:%s reason: %s\n",
					rdev->remote_address,
					rdev->rssi,
					rdev->remote_address_type,
					rdev->remote_alias,
					rdev->change_name);
		break;
	case RK_BT_STATE_DISCONN_ALREADY:
		printf("+ STATE_DISCONNECTED: RK_BT_STATE_DISCONN_ALREADY\n");
		break;
	case RK_BT_STATE_DISCONN_FAILED:
		printf("+ STATE_FAILED: RK_BT_STATE_DISCONN_FAILED\n");
		break;

	case RK_BT_STATE_CONNECTED_ALREADY:
		printf("+ STATE_CONNECTED: RK_BT_STATE_CONNECTED_ALREADY\n");
		break;
	case RK_BT_STATE_CONNECT_FAILED_INVAILD_ADDR:
		printf("+ STATE_FAILED: RK_BT_STATE_CONNECT_FAILED_INVAILD_ADDR\n");
		break;
	case RK_BT_STATE_CONNECT_FAILED_NO_FOUND_DEVICE:
		printf("+ STATE_FAILED: RK_BT_STATE_CONNECT_FAILED_NO_FOUND_DEVICE\n");
		break;
	case RK_BT_STATE_CONNECT_FAILED_SCANNING:
		printf("+ STATE_FAILED: RK_BT_STATE_CONNECT_FAILED_SCANNING\n");
		break;

	case RK_BT_STATE_DEL_DEV_FAILED:
		printf("+ STATE_FAILED: RK_BT_STATE_DEL_DEV_FAILED\n");
		break;

	//MEDIA A2DP SOURCE
	case RK_BT_STATE_SRC_ADD:
	case RK_BT_STATE_SRC_DEL:
		if (rdev != NULL) {
			printf("+ STATE SRC MEDIA %s [%s|%d]:%s:%s\n",
					(state == RK_BT_STATE_SRC_ADD) ? "ADD" : "DEL",
					rdev->remote_address,
					rdev->rssi,
					rdev->remote_address_type,
					rdev->remote_alias);
			printf("+ codec: %s, freq: %s, chn: %s\n",
						rdev->media.codec == 0 ? "SBC" : "UNKNOW",
						rdev->media.sbc.frequency == 1 ? "48K" : "44.1K",
						rdev->media.sbc.channel_mode == 1 ? "JOINT_STEREO" : "STEREO");
		}
		break;

	//MEDIA AVDTP TRANSPORT
	case RK_BT_STATE_TRANSPORT_VOLUME:
		if (rdev != NULL)
			printf("+ STATE AVDTP TRASNPORT VOLUME[%d] [%s|%d]:%s:%s\n",
					rdev->media.volume,
					rdev->remote_address,
					rdev->rssi,
					rdev->remote_address_type,
					rdev->remote_alias);
		break;
	case RK_BT_STATE_TRANSPORT_IDLE:
		if (rdev != NULL) {
			printf("+ STATE AVDTP TRASNPORT IDLE [%s|%d]:%s:%s\n",
					rdev->remote_address,
					rdev->rssi,
					rdev->remote_address_type,
					rdev->remote_alias);
			//low priority for broadcom
			vendor_set_high_priority(rdev->remote_address, ACL_NORMAL_PRIORITY,
									 bt_content.profile & PROFILE_A2DP_SINK_HF ? A2DP_SINK : A2DP_SOURCE);
		}
		break;
	case RK_BT_STATE_TRANSPORT_PENDING:
		if (rdev != NULL)
			printf("+ STATE AVDTP TRASNPORT PENDING [%s|%d]:%s:%s\n",
				rdev->remote_address,
				rdev->rssi,
				rdev->remote_address_type,
				rdev->remote_alias);
		break;
	case RK_BT_STATE_TRANSPORT_ACTIVE:
		if (rdev != NULL) {
			printf("+ STATE AVDTP TRASNPORT ACTIVE [%s|%d]:%s:%s\n",
				rdev->remote_address,
				rdev->rssi,
				rdev->remote_address_type,
				rdev->remote_alias);
			//high priority for broadcom
			//vendor_set_high_priority(rdev->remote_address, ACL_HIGH_PRIORITY,
			//					 bt_content.profile & PROFILE_A2DP_SINK_HF ? A2DP_SINK : A2DP_SOURCE);
		}
		break;
	case RK_BT_STATE_TRANSPORT_SUSPENDING:
		if (rdev != NULL)
			printf("+ STATE AVDTP TRASNPORT SUSPEND [%s|%d]:%s:%s\n",
				rdev->remote_address,
				rdev->rssi,
				rdev->remote_address_type,
				rdev->remote_alias);
		break;

	//MEDIA A2DP SINK
	case RK_BT_STATE_SINK_ADD:
	case RK_BT_STATE_SINK_DEL:
		if (rdev != NULL) {
			printf("+ STATE SINK MEDIA %s [%s|%d]:%s:%s\n",
				(state == RK_BT_STATE_SINK_ADD) ? "ADD" : "DEL",
				rdev->remote_address,
				rdev->rssi,
				rdev->remote_address_type,
				rdev->remote_alias);
			printf("+ codec: %s, freq: %s, chn: %s\n",
					rdev->media.codec == 0 ? "SBC" : "UNKNOW",
					rdev->media.sbc.frequency == 1 ? "48K" : "44.1K",
					rdev->media.sbc.channel_mode == 1 ? "JOINT_STEREO" : "STEREO");

			if (state == RK_BT_STATE_SINK_ADD) {
				//rk_bt_pbap_get_vcf(rdev->remote_address, "pb", "/data/pb.vcf");
				memset(rfcomm_remote_address, 0, 17);
				strncpy(rfcomm_remote_address, rdev->remote_address, 17);
				g_timeout_add(1000, bt_open_rfcomm, NULL);
				//rk_bt_rfcomm_open(rdev->remote_address, at_evt_callback);
			} else if (state == RK_BT_STATE_SINK_DEL) {
				//rk_bt_rfcomm_close();
			}
		}
		break;
	case RK_BT_STATE_SINK_PLAY:
		if (rdev != NULL)
			printf("+ STATE SINK PLAYER PLAYING [%s|%d]:%s:%s\n",
				rdev->remote_address,
				rdev->rssi,
				rdev->remote_address_type,
				rdev->remote_alias);
		break;
	case RK_BT_STATE_SINK_STOP:
		if (rdev != NULL)
			printf("+ STATE SINK PLAYER STOP [%s|%d]:%s:%s\n",
				rdev->remote_address,
				rdev->rssi,
				rdev->remote_address_type,
				rdev->remote_alias);
		break;
	case RK_BT_STATE_SINK_PAUSE:
		if (rdev != NULL)
			printf("+ STATE SINK PLAYER PAUSE [%s|%d]:%s:%s\n",
				rdev->remote_address,
				rdev->rssi,
				rdev->remote_address_type,
				rdev->remote_alias);
		break;
    case RK_BT_STATE_SINK_TRACK:
        printf("+ STATE SINK TRACK INFO [%s|%d]:%s:%s track[%s]-[%s]\n",
            rdev->remote_address,
            rdev->rssi,
            rdev->remote_address_type,
            rdev->remote_alias,
            rdev->title,
            rdev->artist);
    break;
    case RK_BT_STATE_SINK_POSITION:
        printf("+ STATE SINK TRACK POSITION:[%s|%d]:%s:%s [%u-%u]\n",
                rdev->remote_address,
                rdev->rssi,
                rdev->remote_address_type,
                rdev->remote_alias,
                rdev->player_position,
                rdev->player_total_len);
    break;

	//ADV
	case RK_BT_STATE_ADAPTER_BLE_ADV_START:
		bt_content.ble_content.ble_advertised = true;
		printf("RK_BT_STATE_ADAPTER_BLE_ADV_START successful\n");
		break;
	case RK_BT_STATE_ADAPTER_BLE_ADV_STOP:
		bt_content.ble_content.ble_advertised = false;
		printf("RK_BT_STATE_ADAPTER_BLE_ADV_STOP successful\n");
		break;

	//ADAPTER STATE
	case RK_BT_STATE_ADAPTER_NO_DISCOVERYABLED:
		bt_content.discoverable = false;
		printf("RK_BT_STATE_ADAPTER_NO_DISCOVERYABLED\n");
		//re-discoverable
		//rk_bt_set_discoverable(1);
		break;
	case RK_BT_STATE_ADAPTER_DISCOVERYABLED:
		bt_content.discoverable = true;
		printf("RK_BT_STATE_ADAPTER_DISCOVERYABLED\n");
		break;
	case RK_BT_STATE_ADAPTER_NO_PAIRABLED:
		bt_content.pairable = false;
		printf("RK_BT_STATE_ADAPTER_NO_PAIRABLED successful\n");
		break;
	case RK_BT_STATE_ADAPTER_PAIRABLED:
		bt_content.pairable = true;
		printf("RK_BT_STATE_ADAPTER_PAIRABLED successful\n");
		break;
	case RK_BT_STATE_ADAPTER_NO_SCANNING:
		bt_content.scanning = false;
		printf("RK_BT_STATE_ADAPTER_NO_SCANNING successful\n");
		break;
	case RK_BT_STATE_ADAPTER_SCANNING:
		bt_content.scanning = true;
		printf("RK_BT_STATE_ADAPTER_SCANNING successful\n");
		break;
	case RK_BT_STATE_ADAPTER_POWER_ON:
		bt_content.power = true;
		printf("RK_BT_STATE_ADAPTER_POWER_ON successful\n");
		//rk_bt_set_discoverable(1);
		break;
	case RK_BT_STATE_ADAPTER_POWER_OFF:
		bt_content.power = false;
		printf("RK_BT_STATE_ADAPTER_POWER_OFF successful\n");
		break;
	case RK_BT_STATE_ADAPTER_LOCAL_NAME:
		printf("RK_BT_STATE_ADAPTER_LOCAL_NAME successful\n");
		break;
	case RK_BT_STATE_COMMAND_RESP_OK:
		printf("RK_BT_STATE CMD OK\n");
		break;
	case RK_BT_STATE_COMMAND_RESP_ERR:
		printf("RK_BT_STATE CMD ERR\n");
		break;
	case RK_BT_STATE_ANCS:
		ancs_is_support = true;
		printf("RK_BT_STATE_ANCS\n");
		break;
	case RK_BT_STATE_SPP_CONNECTED:
		spp_fd = (int)rdev;
		printf("RK_BT_STATE_SPP_CONNECTED [fd: %d]\n", spp_fd);
		write(spp_fd, "hello spp from client", strlen("hello spp from client"));
		break;
	case RK_BT_STATE_SPP_RECV_DATA:
		printf("RK_BT_STATE_SPP_RECV_DATA\n");
		int len = *((int *)rdev);
		char *data = (char *)rdev + 4;
		printf("SPP DATA[%d]: %s\n", len, data);
		write(spp_fd, data, len);
		break;
	case RK_BT_STATE_SPP_DISCONNECTED:
		printf("RK_BT_STATE_SPP_DISCONNECTED\n");
		break;
	default:
		if (rdev != NULL)
			printf("+ DEFAULT STATE %d: %s:%s:%s RSSI: %d [CBP: %d:%d:%d]\n", state,
				rdev->remote_address,
				rdev->remote_address_type,
				rdev->remote_alias,
				rdev->rssi,
				rdev->connected,
				rdev->paired,
				rdev->bonded);
		break;
	}
}

//char abc[1];
void bt_test_version(char *data)
{
	printf("RK BT VERSION: %s\n", rk_bt_version());
	//test AddressSanitizer
	//abc[99] = 1;
}

void bt_test_source_play(char *data)
{
	char rsp[64], aplay[128];
	exec_command("hcitool con | grep ACL | awk '{print $3}'", rsp, 64);
	if (rsp[0]) {
		rsp[17] = 0;
		sprintf(aplay, "aplay -D bluealsa:DEV=%s,PROFILE=a2dp /data/test.wav", rsp);
		exec_command_system(aplay);
	}
}

/*
 * This function is a callback from the rk_bt_set_profile() API.
 * It gets called whenever the profile is changed.
 *
 * The function first checks if the bluealsa service is running
 * If it is, it unloads the bluetooth modules to avoid the
 * conflict with bluealsa.
 *
 * Next, it checks if bluealsa is running.
 * If it is, it kills the process to start a new one with updated profile.
 *
 * Depending on the profile, it starts either a2dp-sink,
 * hfp-hf or a2dp-source, hfp-ag with bluealsa.
 *
 * The function returns true if successful, false otherwise.
 */
static bool bt_test_audio_server_cb(bool enable)
{
	char rsp[64];

	/* Print the current profile set on the bt_content struct */
	printf("%s bt_content.profile: 0x%x, a2dp-sink: %d, a2dp-source: %d\n", __func__, bt_content.profile,
			(bt_content.profile & PROFILE_A2DP_SINK_HF),
			(bt_content.profile & PROFILE_A2DP_SOURCE_AG));

	if (enable == false) {
		/* stop necessary services */
		kill_task("bluealsa");
		kill_task("bluealsa-aplay");
		return true;
	}

	/* The pulseaudio is used. */
	if (bt_content.bluealsa == false)
		return true;

	/*
	 * If pulseaudio is running, unload the bluetooth modules
	 * to avoid the conflict with bluealsa.
	 */
	if (get_ps_pid("pulseaudio")) {
		exec_command("pactl list modules | grep bluetooth", rsp, 64);
		if (rsp[0]) {
			exec_command_system("pactl unload-module module-bluetooth-policy");
			exec_command_system("pactl unload-module module-bluetooth-discover");
		}
	}

	/* restart bluealsa */
	kill_task("bluealsa");
	kill_task("bluealsa-aplay");
	//kill_task("pulseaudio");

	/*
	 * Start bluealsa service with the appropriate profile
	 * based on the profile set on bt_content struct
	 */
	if ((bt_content.profile & PROFILE_A2DP_SINK_HF) == PROFILE_A2DP_SINK_HF) {
		exec_command_system("bluealsa -S --profile=a2dp-sink --profile=hfp-hf &");
		//exec_command_system("bluealsa -S --profile=a2dp-sink &");
		exec_command_system("bluealsa-aplay -S --profile-a2dp 00:00:00:00:00:00 &");
		//exec_command_system("bluealsa-aplay -S --profile-sco 00:00:00:00:00:00 &");
	} else if ((bt_content.profile & PROFILE_A2DP_SOURCE_AG) == PROFILE_A2DP_SOURCE_AG) {
		exec_command_system("bluealsa -S --profile=a2dp-source --profile=hfp-ag --a2dp-volume &");
	}

	/* Reconnect last device */
	//g_timeout_add_seconds(2, bt_reconect_last_dev, NULL);

	return true;
}

/* For buildroot OS:
 * The wifibt-init.sh or bt_init.sh used for hci0 init/stop
 */
#define BUILDROOT_OS	1

/* For Debian OS:
 *	The script /usr/bin/wifibt-init.sh used for hci0 init/stop
 *	The "systemctl start/stop bluetoothd" used for bluetoothd init/stop
 */
#define DEBIAN_OS		2

/* For Custom: Third BT vendor
 * power control:
 * exec_command_system("echo 0 > /sys/class/rfkill/rfkill0/state && sleep 0.5 && echo 1 > /sys/class/rfkill/rfkill0/state");
 * exec_command_system("hciconfig_vendor/dev/ttyS1 &");
 */
#define CUSTOM_INIT		3

static bool bt_init_realek(int os)
{
	/* 需要Realtek提供定制的firmware
	 * Realtek SCO OVER PCM
	 * Realtek:
	 * 8KHZ PCM采样率
	 * 16bit data位
	 * slave模式（clk由主控提供)
	 * data采用msb模式
	 * short frame sync 短同步
	 * fs 下降沿开始发送和接收data
	 * fs 之间2个slot，然后有效数据在第一个slot
	 * clk 频率为：2 * 16 * 8 = 256KHZ
	 */
	/* Bluetooth Controller Init: firmware download and to create hci0 */
	/* Reset BT_REG_ON */
	exec_command_system("insmod hci_uart.ko");
	kill_task("rtk_hciattach");
	exec_command_system("echo 0 > /sys/class/rfkill/rfkill0/state && sleep 0.6 && echo 1 > /sys/class/rfkill/rfkill0/state");
	exec_command_system("rtk_hciattach -n -s 115200 /dev/ttyS1 rtk_h5 &");

	//Ensure hci0 exist
	int times = 100;
	do {
		if (access("/sys/class/bluetooth/hci0", F_OK) == 0)
			break;
		usleep(100 * 1000);

		if (times == 0) {
			printf("ERR: hci0 not init!\n");
			return false;
		}
	} while (times--);

	return true;
}

static bool bt_init_broadocm(int os)
{
	/* stop necessary services */
	kill_task("brcm_patchram_plus1");
	//kill_task("rtk_hciattach");
	//exec_command_system("rmmod hci_uart.ko");
	/*
	* exec brcm_patchram_plus1 to init hci0 for broadcom chip!
	*
	* if hfp profile is used: (only for broadcom chip)
	* if you use SCO PCM 8K: you should use：--scopcm=0,1,0,0,0,0,0,3,0,0
	* if you use SCO PCM 16K(mSBC): you should use --scopcm=0,2,0,0,0,0,0,3,0,0
	* 
	* scopcm:
	* sco_routing: sco_routing is 0 for PCM, 1 for Transport, 2 for Codec and 3 for I2S
	* pcm_interface_rate is 0 for 128KBps, 1 for 256 KBps, 2 for 512KBps, 3 for 1024KBps, and 4 for 2048Kbps
	* frame_type is 0 for short and 1 for long
	* sync_mode is 0 for slave and 1 for master
	* clock_mode is 0 for slave and 1 for master
	* lsb_first is 0 for false aand 1 for true
	* fill_bits is the value in decimal for unused bits
	* fill_method is 0 for 0's and 1 for 1's, 2 for signed and 3 for programmable
	* fill_num is the number or bits to fill
	* right_justify is 0 for false and 1 for true
	* 
	* i2s: --i2s=0,0,1,2
	* i2s_enable is 0 for disable and 1 for enable
	* is_master is 0 for slave and 1 for master
	* sample_rate is 0 for 8KHz, 1 for 16Khz and 2 for 4 KHz
	* clock_rate is 0 for 128KHz, 1 for 256KHz, 2 for 512 KHz, 3 for 1024 KHz and 4 for 2048 KHz.
	* 
	//exec_command_system("brcm_patchram_plus1 --enable_hci --scopcm=0,1,0,1,1,0,0,3,0,0 --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /lib/firmware/ /dev/ttyS1 &");
	//exec_command_system("brcm_patchram_plus1 --enable_hci --scopcm=0,2,0,1,1,0,0,3,0,0 --i2s=0,1,1,2 --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /lib/firmware/ /dev/ttyS4 &");
	//exec_command_system("brcm_patchram_plus1 --enable_hci --scopcm=0,2,0,0,0,0,0,3,0,0 --i2s=0,0,1,2 --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /lib/firmware/ /dev/ttyS1 &");
	//exec_command_system("brcm_patchram_plus1 --enable_hci --scopcm=0,2,0,1,1,0,0,3,0,0 --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /lib/firmware/ /dev/ttyS4 &");
	*/
	/* Bluetooth Controller Init: firmware download and to create hci0 */
	/* Reset BT_REG_ON */
	kill_task("brcm_patchram_plus1");
	exec_command_system("echo 0 > /sys/class/rfkill/rfkill0/state && sleep 0.6 && echo 1 > /sys/class/rfkill/rfkill0/state");
	exec_command_system("brcm_patchram_plus1 --enable_hci \
		--scopcm=0,2,0,1,1,0,0,3,0,0 --i2s=0,1,1,2 --no2bytes \
		--use_baudrate_for_download --tosleep 200000 --baudrate 1500000 \
		--patchram /lib/firmware/ /dev/ttyS4 &");

	//Ensure hci0 exist
	int times = 100;
	do {
		if (access("/sys/class/bluetooth/hci0", F_OK) == 0)
			break;
		usleep(100 * 1000);

		if (times == 0) {
			printf("ERR: hci0 not init!\n");
			return false;
		}
	} while (times--);

	return true;
}

//第三方的BT模块: 找蓝牙原厂或模块厂获取“如何初始化蓝牙的文档”
//蓝牙的文档，里面会有如何初始化的描述，一般都会有厂家定制的hciattach初始化程序
static bool bt_init_third_vendor(void)
{
	int times = 100;

	//NOTE: 还有的蓝牙厂家(希微)是kernel初始化的，比如加载xxx.ko后，内核自动生成hci0节点，此时无需做任何事情
	bool hci0_by_kernel = false;
	if (hci0_by_kernel)
		goto hci0;

	/* Reset BT_REG_ON */
	exec_command_system("echo 0 > /sys/class/rfkill/rfkill0/state && sleep 0.6 && echo 1 > /sys/class/rfkill/rfkill0/state");

	//使用厂家提供的命令替换下面的rtk_hciattach及其参数，每家的参数不一样
	kill_task("vendor_hciattach");
	/* Bluetooth Controller Init: firmware download and to create hci0 */
	exec_command_system("vendor_hciattach -n -s 115200 /dev/ttyS1 xxx &");

hci0:
	//Ensure hci0 exist
	do {
		if (access("/sys/class/bluetooth/hci0", F_OK) == 0)
			break;
		usleep(100 * 1000);

		if (times == 0) {
			printf("ERR: hci0 not init!\n");
			return false;
		}
	} while (times--);

	return true;
}

static bool bt_init_common(int os)
{
	/* waiting for hci0 exited */
	int times = 100;

	if (os > DEBIAN_OS)
		return true;

	kill_task("brcm_patchram_plus1");
	kill_task("rtk_hciattach");
	kill_task("rk_hciattach");

	do {
		if (access("/sys/class/bluetooth/hci0", F_OK) != 0)
			break;
		usleep(100 * 1000);

		if (times == 0) {
			printf("hci0 not exited!\n");
			return false;
		}
	} while (times--);

	/* Bluetooth Controller Init: firmware download and to create hci0 */
	if (os == BUILDROOT_OS) {
		if (!access("/usr/bin/wifibt-init.sh", F_OK))
			exec_command_system("/usr/bin/wifibt-init.sh");
		else if (!access("/usr/bin/bt_init.sh", F_OK))
			exec_command_system("/usr/bin/bt_init.sh");
	} 

	//if (os == DEBIAN_OS) {
	//	exec_command_system("sudo systemctl restart bluetooth");
	//}

	//waiting for hci0
	times = 100;
	do {
		if (access("/sys/class/bluetooth/hci0", F_OK) == 0)
			break;
		usleep(100 * 1000);

		if (times == 0) {
			printf("ERR: hci0 not init!\n");
			return false;
		}
	} while (times--);

	return true;
}

/*
 * Start bluetoothd
 */
static bool bt_start_bluetoothd(int os, bool debug_mode)
{
	int times = 100;

	if (os == DEBIAN_OS) {
		exec_command_system("sudo killall bluetoothd");
		exec_command_system("sudo bluetoothd -n -P battery,hostname,gap,wiimote -f /data/main.conf &");
		goto bluetoothd;
	}

	kill_task("bluetoothd");
	kill_task("obexd");

	if (debug_mode) {
		exec_command_system("btmon -w /data/btsnoop.log > /dev/null &");
		exec_command_system("/usr/libexec/bluetooth/bluetoothd -n -P \
			battery,hostname,gap,wiimote \
			-f /data/main.conf \
			--debug=plugins/policy.c,src/adapter.c,src/device.c &");
	} else {
		exec_command_system("/usr/libexec/bluetooth/bluetoothd -C -n -P profile,battery,hostname,gap,wiimote -f /data/main.conf &");
	}

bluetoothd:
	//waiting for bluetoothd
	times = 100;
	do {
		if (get_ps_pid("bluetoothd"))
			break;

		usleep(100 *1000);

		if (times == 0) {
			printf("bluetoothd not init!\n");
			return false;
		}
	} while (times--);

	//debug
	usleep(500 * 1000);

	//obexd
	if (bt_content.profile & PROFILE_OBEX) {
		exec_command_system("export $(dbus-launch)");
		exec_command_system("/usr/libexec/bluetooth/obexd -r /userdata/ -a -n &");
		//debug: exec_command_system("/usr/libexec/bluetooth/obexd -r /userdata/ -a -n -d &");
		//check bluetoothd
		times = 100;
		do {
			if (get_ps_pid("obexd"))
				break;
			usleep(100 *1000);

			if (times == 0) {
				printf("obexd not init!\n");
				return false;
			}
		} while (times--);
	}

	return true;
}

static bool bt_deinit_vendor(void)
{
	//CLEAN
	exec_command_system("/etc/init.d/S40bluetooth stop");

	kill_task("bluetoothd");
	kill_task("obexd");

	//audio server deinit
	kill_task("bluealsa");
	kill_task("bluealsa-alay");

	//vendor deinit
	//exec_command_system("hciconfig hci0 down sleep 0.2");
	kill_task("brcm_patchram_plus1");
	kill_task("rtk_hciattach");
	kill_task("rk_hciattach");
	return true;
}

/**
 * Callback function for setting vendor profile.
 *
 * This function is called when the vendor profile is changed.
 * It is responsible for starting and stopping the necessary services
 * based on the profile set on bt_content struct.
 *
 * @param enable true if the profile is being enabled, false otherwise
 *
 * @return true if successful, false otherwise
 * 
 * if enable == true
 *  NOTE: This function ensures that the Bluetooth hci0 node is generated and the bluetoothd daemon is running
 *  NOTE: This function ensures that the Bluetooth hci0 node is generated and the bluetoothd daemon is running
 *  NOTE: This function ensures that the Bluetooth hci0 node is generated and the bluetoothd daemon is running
 */
bool bt_test_vendor_cb(bool enable)
{
	/* Debug info:
	 * bluetoothd -d ==> /var/log/messages
	 * btmon -w /data/btsnoop.log
	 */
	bool debug_mode = true;

	/* SDK Supported OS: buildroot | Debian */
	int bt_os = BUILDROOT_OS;

	/* 
	 * Third-party Bluetooth modules
	 * SDK Support BT: Broadcom(新思/英飞凌/Rockchip(rk960)), third_bt必须为false;
	 * 当需要支持第三方蓝牙模块：比如AIC/高拓/希微等蓝牙模块，third_bt为true;
	 */
	bool third_bt = false;

	printf("OS: %d, DEBUG: %d, Third: %d\n", bt_os, debug_mode, third_bt);

	if (enable == true) {
		if (third_bt) {
			/* 其它第三方SDK还未支持的蓝牙模块
			 * 都需要根据原厂文档自行初始化
			 */
			bt_init_third_vendor();
		} else {
			/* 最新SDK集成了初始化脚本，自动初始化蓝牙, 目前仅支持Realtek/Broadcom/Rockchip
			 * 如下两种场景:
			 *  1. 如果旧的SDK没有上述内置的初始化脚本，则可以手动添加初始化流程，只针对broadcom/rtk的模块
			 *  2. 想配置SCO OVER PCM的参数
			 *     注意要手动配置UART口
			 *     bt_init_broadocm(bt_os);
			 *     bt_init_realek(bt_os);
			 */
			bt_init_common(bt_os);
		}

		//Start bluetoothd daemon
		bt_start_bluetoothd(bt_os, debug_mode);
	} else {
		bt_deinit_vendor();
	}

	return true;
}

/*
	BT 4.X
	//LE Set Random Address Command
	hcitool -i hci0 cmd 0x08 0x0005 41 C5 10 C3 9C 04

	//LE SET PARAMETERS
	hcitool -i hci0 cmd 0x08 0x0006 A0 00 A0 00 00 01 00 00 00 00 00 00 00 07 00

	// LE Set Advertising Data Command
	hcitool -i hci0 cmd 0x08 0x0008 1b 02 01 02 03 03 10 19 13 09 52 4f 43 4b 43 48 49 50 5f 41 55 44 49 4f 5f 42 4c 45

	// LE Set Advertising Resp Data Command
	hcitool -i hci0 cmd 0x08 0x0009 17 16 ff 46 00 02 1c 02 04 54 01 00 00 08 54 00 00 00 00 00 00 36 01 00

	// LE Set Advertise Enable/Disable Command
	hcitool -i hci0 cmd 0x08 0x000a 1

	BT 5.X
	#Command Code    LE Set Extended Advertising Disable Command
	hcitool -i hci0 cmd 0x08 0x0039  00 01 01 00 00 00

	#Command Code    LE Remove Advertising Set Command
	hcitool -i hci0 cmd 0x08 0x003C 01

	#Command Code    LE Set Extended Advertising Parameters Command
	hcitool -i hci0 cmd 0x08 0x0036 01 13 00 A0 00 00 A0 00 00 07 01 00 00 00 00 00 00 00 00 7F 01 00 01 00 00

	#Command Code    LE Set Advertising Set Random Address Command
	hcitool -i hci0 cmd 0x08 0x0035 01 45 6E 87 2D 6A 44

	#Command Code    LE Set Extended Advertising Data Command
	hcitool -i hci0 cmd 0x08 0x0037 01 03 01 0D 03 03 0D 18 08 09 54 65 73 74 20 4C 46

	#Command Code    LE Set Extended Advertising Enable Command
	hcitool -i hci0 cmd 0x08 0x0039  01 01 01 00 00 00

	//
	reference: Assigned_Numbers.pdf

	BT 5.X
	#Command Code    LE Set Extended Advertising Disable Command
	hcitool -i hci0 cmd 0x08 0x0039  00 01 01 00 00 00

	#Command Code    LE Remove Advertising Set Command
	hcitool -i hci0 cmd 0x08 0x003C 01

	#Command Code    LE Set Extended Advertising Parameters Command
	Advertising_Handle: 				0x01
	Advertising_Event_Properties: 		0x0013     		//00010011  
														bit0: Connectable advertising
														bit1: Scannable advertising
														bit2: Directed advertising
														bit3: High Duty Cycle Directed Connectable advertising (≤ 3.75 ms Advertising Interval)
														bit4: Use Legacy advertising PDUs
														bit5: Omit advertiser's address from all PDUs ("anonymous advertising")
														bit6: Include TxPower in the extended header of at least one advertising PDU
	Primary_Advertising_Interval_Min: 	0x0000AO		//Range: 0x000020 to 0xFFFFFF Time = N * 0.625 ms Time Range: 20 ms to 10,485.759375 s
	Primary_Advertising_Interval_Max:	0x0000A0		//Range: 0x000020 to 0xFFFFFF Time = N * 0.625 ms Time Range: 20 ms to 10,485.759375 s
	Primary_Advertising_Channel_Map:	0x07			//bit0: CHAN_37 bit1: CHAN_38 bit2: CHAN_39
	Own_Address_Type:					0x01			//0x00: Public Device Address, 0x01: Random Device Address, 0x02/0x03：Controller generated ...
	Peer_Address_Type:					0x00			//0x00 Public Device Address or Public Identity Address, 0x01: Random Device Address or Random (static) Identity Address
	Peer_Address:						0x00		    //6byte
	Advertising_Filter_Policy:			0x00			//0x00: Process scan and connection requests from all devices (i.e., the White List is not in use)
	Advertising_TX_Power:				0x7F			//Range: -127 to +20, 0x7F: Host has no preference
	Primary_Advertising_PHY:			0x01			//0x01: 1M, 0x03: Le Coded
	Secondary_Advertising_Max_Skip:		0x00			//AUX_ADV_IND shall be sent prior to the next advertising event
	Secondary_Advertising_PHY:			0x01			//0x01: 1M, 0x02: 2M, 0x03: Le Coded
	Advertising_SID:					0x00			//0x00 to 0x0F Value of the Advertising SID subfield in the ADI field of the PDU
	Scan_Request_Notification_Enable:	0x00			//0x00: Scan Request Notification is disabled, 0x01: Scan Request Notification is enabled

	hcitool -i hci0 cmd 0x08 0x0036 01 13 00 A0 00 00 A0 00 00 07 01 00 00 00 00 00 00 00 00 7F 01 00 01 00 00

	#Command Code    LE Set Advertising Set Random Address Command
	hcitool -i hci0 cmd 0x08 0x0035 01 45 6E 87 2D 6A 44

	#Command Code    LE Set Extended Advertising Data Command
	Advertising_Handle: 	0x01
	Options:				0x03
		Value Parameter Description
		0x00 Intermediate fragment of fragmented extended advertising data
		0x01 First fragment of fragmented extended advertising data
		0x02 Last fragment of fragmented extended advertising data
		0x03 Complete extended advertising data
		0x04 Unchanged data (just update the Advertising DID)
		All other values Reserved for future use
	Fragment_Preference:	0x01
		Value Parameter Description
		0x00 The Controller may fragment all Host advertising data
		0x01 The Controller should not fragment or should minimize fragmentation of 
		Host advertising data
		All other values Reserved for future use

	Advertising_Data_Length: 0 to 251 The number of octets in the Advertising Data parameter

	Advertising_Data: Size: Advertising_Data_Length octets
	hcitool -i hci0 cmd 0x08 0x0037 01 03 01 0x8 09 54 65 73 74 20 4C 46

	#Command Code    LE Set Extended Scan Response Data command
	hcitool -i hci0 cmd 0x08 0x0038

	#Command Code    LE Set Extended Advertising Enable Command
	hcitool -i hci0 cmd 0x08 0x0039  01 01 01 00 00 00


	BT 4.X
	Advertising_Interval_Min
	Advertising_Interval_Max
	Advertising_Type
	Own_Address_Type			//0x00: Public Device Address, 
								//0x01: Random Device Address,
								//0x02: Controller generates Resolvable Private Address based on the local IRK from the resolving list.
										If the resolving list contains no matching entry, use the public address.
								//0x03: Controller generates Resolvable Private Address based on the local IRK from the resolving list
										If the resolving list contains no matching entry, use the random address from LE_Set_Random_Address
	Peer_Address_Type
	Peer_Address
	Advertising_Channel_Map
	Advertising_Filter_Policy

	DEVICE ADDRESS
	Devices are identified using a device address and an address type

	A device shall use at least one type of device address and may contain both.

	A device's Identity Address is a Public Device Address or Random Static 
	Device Address that it uses in packets it transmits. If a device is using 
	Resolvable Private Addresses, it shall also have an Identity Address.

	Whenever two device addresses are compared, the comparison shall include 
	the device address type (i.e. if the two addresses have different types, they are 
	different even if the two 48-bit addresses are the same).

	1/ Public device address
		The public device address shall be created in accordance with [Vol 2] Part B, 
		Section 1.2, with the exception that the restriction on LAP values does not 
		apply unless the public device address will also be used as a BD_ADDR for a 
		BR/EDR Controller.

	2/ Random device address
		The random device address may be of either of the following:
		• Static address
		• Private address.

		Address [47:46] Sub-Type
		0b00 			Non-resolvable private address
		0b01 			Resolvable private address
		0b10 			Reserved for future use
		0b11 			Static device address
*/

//#define BLE_ADV_CUSTOM
static int ble_direct_set_adv_param(char *ble_name , char *ble_mac_addr)
{
#ifdef BLE_ADV_CUSTOM
	char cmd_para[128];
	char ret_buff[128];
	char temp[32];

	//using ble adv
	ble_direct_flag = true;

	//5.x
	//exec_command("hcitool -i hci0 cmd 0x08 0x0036 01 13 00 A0 00 00 A0 00 00 07 01 00 00 00 00 00 00 00 00 7F 01 00 01 00 00", ret_buff, 128);
	//char CMD_ADV_RESP_DATA[128] = "hcitool -i hci0 cmd 0x08 0x0037 01 03 01";

	//4.x
	/* setup 1: set adv param (default Don't modify) */
	exec_command("hcitool -i hci0 cmd 0x08 0x0006 20 00 20 00 00 01 00 00 00 00 00 00 00 07 00", ret_buff, 128);
	printf("CMD_PARA ret buff: %s\n", ret_buff);

	//set ble macaddr
	memset(cmd_para, 0, 128);
	sprintf(cmd_para, "%s %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx",
			//"hcitool -i hci0 cmd 0x08 0x0035 01",
			"hcitool -i hci0 cmd 0x08 0x0005",
			ble_mac_addr[5], ble_mac_addr[4], ble_mac_addr[3],
			ble_mac_addr[2], ble_mac_addr[1], ble_mac_addr[0]);
	printf("CMD_MAC: %zu, %s\n", strlen(cmd_para), cmd_para);
	exec_command(cmd_para, ret_buff, 128);
	printf("CMD_MAC ret buff: %s\n", ret_buff);

	/* setup 2: ADV DATA 32 Bytes */
	//char CMD_ADV_DATA[128] = "hcitool -i hci0 cmd 0x08 0x0008"; //在后面补32个字节的自定义数据，切记需要以字符串的形式写入！

	//todo
	char CMD_ADV_DATA[256] = "hcitool -i hci0 cmd 0x08 0x0008 0x18 0x11 0x07 0xD0 0x00 0x2D 0x12 0x1E 0x4B 0x0F 0xA4 0x99 0x4E 0xCE 0xB5 0x31 0xF4 0x05 0x79 0x02 0x0A 0x10 0x02 0x01 0x0A 0x00 0x00 0x00 0x00 0x00 0x00 0x00"; //在后面补32个字节的自定义数据，切记需要以字符串的形式写入！
	printf("CMD_ADV_DATA: %s\n", CMD_ADV_DATA);
	exec_command(CMD_ADV_DATA, ret_buff, 256);
	printf("CMD_ADV_DATA ret: %s\n", ret_buff);

	/* setup 3: ADV DATA RESP 32 Bytes */
	char CMD_ADV_RESP_DATA[128] = "hcitool -i hci0 cmd 0x08 0x0009";  //在后面补32个字节的自定义数据，切记需要以字符串的形式写入！
	//example: set ble name (例子仅仅设置了一个名字)
	memset(cmd_para, 0, 128);

	cmd_para[0] = strlen(ble_name) + 1 + 1; //caculate all length

	cmd_para[1] = strlen(ble_name) + 1;		//caculate ble name length
	cmd_para[2] = 0x09; 					//type: complete local name
	sprintf(cmd_para + 3, "%s", ble_name);	//set ble name
	//printf("cmd_para ret: %s\n", cmd_para);

	//append ble name to CMD_ADV_RESP_DATA
	memset(temp, 0, 32);
	for (int i = 0; i < strlen(cmd_para); i++) {
		sprintf(temp, "%02x", cmd_para[i]);
		strcat(CMD_ADV_RESP_DATA, " ");
		strcat(CMD_ADV_RESP_DATA, temp);
	}

	//left all set 00 （剩余不够32byte的全部补0）
	for (int i = 0; i < (32 - 3 - strlen(ble_name)); i++) {
		strcat(CMD_ADV_RESP_DATA, " ");
		strcat(CMD_ADV_RESP_DATA, "00");
	}

	printf("CMD_ADV_RESP_DATA: %s\n", CMD_ADV_RESP_DATA);
	exec_command(CMD_ADV_RESP_DATA, ret_buff, 128);
	printf("CMD_ADV_RESP_DATA ret: %s\n", ret_buff);
#endif

	return 1;
}

static int ble_direct_enable_adv(void)
{
#ifdef BLE_ADV_CUSTOM
	int ret_buff[32];

	//enable adv
	exec_command("hcitool -i hci0 cmd 0x08 0x000a 1", ret_buff, 128);
	printf("enable adv ret: %s\n", ret_buff);
#endif

	return 1;
}

static int ble_direct_close_adv(void)
{
#ifdef BLE_ADV_CUSTOM
	int ret_buff[32];

	ble_direct_flag = false;

	//disable adv
	exec_command("hcitool -i hci0 cmd 0x08 0x000a 0", ret_buff, 128);
	printf("disable adv ret: %s\n", ret_buff);
#endif

	return 1;
}

static void auth_func(uint8_t *confirm, uint32_t *code)
{
	//confirm
	*confirm = 1;

	if (code)
		printf("code: %06u\n", *code);
}

#define BT_CONF_DIR "/data/main.conf"
int create_bt_conf(struct bt_conf *conf)
{
	FILE* fp;
	char cmdline[256] = {0};

	fp = fopen(BT_CONF_DIR, "wt+");
	if (NULL == fp)
		return -1;

	fputs("[General]\n", fp);

	//DiscoverableTimeout
	if (conf->discoverableTimeout) {
		sprintf(cmdline, "DiscoverableTimeout = %s\n", conf->discoverableTimeout);
		fputs(cmdline, fp);
	}

	//BleName
	if (conf->BleName) {
		sprintf(cmdline, "BleName = %s\n", conf->BleName);
		fputs(cmdline, fp);
	}

	//class
	if (conf->Class) {
		sprintf(cmdline, "Class = %s\n", conf->Class);
		fputs(cmdline, fp);
	}

	//SSP
	if (conf->ssp) {
		sprintf(cmdline, "SSP = %s\n", conf->ssp);
		fputs(cmdline, fp);
	}

	//mode
	if (conf->mode) {
		sprintf(cmdline, "ControllerMode = %s\n", conf->mode);
		fputs(cmdline, fp);
	}

	//default always
	conf->JustWorksRepairing = "always";
	sprintf(cmdline, "JustWorksRepairing = %s\n", conf->JustWorksRepairing);
	fputs(cmdline, fp);

	//"SecureConnections"
	if (conf->SecureConnections) {
		sprintf(cmdline, "SecureConnections = %s\n", conf->SecureConnections);
		fputs(cmdline, fp);
	}

	//fputs("Experimental = true\n", fp);
	fputs("[GATT]\n", fp);

	//#Cache = always 
	if (conf->gatt_client_cache) {
		memset(cmdline, 0, sizeof(cmdline));
		sprintf(cmdline, "Cache = %s\n", conf->gatt_client_cache);
		fputs(cmdline, fp);
	}

	if (conf->gatt_client) {
		memset(cmdline, 0, sizeof(cmdline));
		sprintf(cmdline, "Client = %s\n", conf->gatt_client);
		fputs(cmdline, fp);
	}

	sprintf(cmdline, "Privacy = on\n");
	fputs(cmdline, fp);

	fputs("\n", fp);
	fclose(fp);

	system("cat /data/main.conf");
	return 0;
}

/* bt init */
int bt_test_init(void *arg)
{
	struct bt_conf conf;
	RkBleGattService *gatt;
	FILE *fp;

	/* 
	 * "read"
	 * "write"
	 * "indicate"
	 * "notify"
	 * "write-without-response"
	 * "encrypt-read"
	 */
	static char *chr_props[] = { "read", "write", "notify", "write-without-response", NULL };
	static char *chr_notify_props[] = {"notify", NULL };

	printf("%s \n", __func__);

	//Must determine whether Bluetooth is turned on
	if (rk_bt_is_open()) {
		printf("%s: already open \n", __func__);
		return -1;
	}

	memset(&bt_content, 0, sizeof(RkBtContent));

	//BREDR CLASS BT NAME
	memset(bt_content.bt_name, 0, sizeof(bt_content.bt_name));
	strcpy(bt_content.bt_name, "rkbt");

	//BLE NAME
	memset(bt_content.ble_content.ble_name, 0, sizeof(bt_content.ble_content.ble_name));
	strcpy(bt_content.ble_content.ble_name, "rkble");

	//1234
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

	//OBEX: OPP(File transfer)/PBAP/MAP
	bt_content.profile |= PROFILE_OBEX;

	/*
	 * Only one can be enabled
	 * a2dp sink and hfp-hf
	 * a2dp source and hfp-ag
	 */
	bt_content.profile |= PROFILE_A2DP_SINK_HF;
	bt_content.bluealsa = true; //false: pulseaudio

	// enable ble
	bt_content.profile |= PROFILE_BLE;
	if (bt_content.profile & PROFILE_BLE) {
		/* GATT SERVICE/CHARACTERISTIC */
		//SERVICE_UUID
		gatt = &(bt_content.ble_content.gatt_instance[0]);
		gatt->server_uuid.uuid = SERVICE_UUID;
		gatt->chr_uuid[0].uuid = BLE_UUID_SEND;
		gatt->chr_uuid[0].chr_props = chr_props;

		gatt->chr_uuid[1].uuid = BLE_UUID_RECV;
		gatt->chr_uuid[1].chr_props = chr_notify_props;
		gatt->chr_cnt = 2;

		//SERVICE_UUID1
		/*
		gatt = &(bt_content.ble_content.gatt_instance[1]);
		gatt->server_uuid.uuid = SERVICE_UUID1;
		gatt->chr_uuid[0].uuid = BLE_UUID_SEND1;
		gatt->chr_uuid[0].chr_props = chr_props;
		gatt->chr_uuid[1].uuid = BLE_UUID_RECV1;
		gatt->chr_uuid[1].chr_props = chr_props;
		gatt->chr_cnt = 2;
		*/

		bt_content.ble_content.srv_cnt = 1;

		/* Fill adv data */
		/* Appearance */
		bt_content.ble_content.Appearance = 0x0080;

		/* Tx power */
		//bt_content.ble_content.tx_power = 0x00;

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
	rk_bt_register_audio_server_callback(bt_test_audio_server_cb);

	pthread_mutex_init(&bt_content.bt_mutex, NULL);
	pthread_cond_init(&bt_content.cond, NULL);

	//default state
	bt_content.init = false;
	bt_content.connecting = false;
	bt_content.scanning = false;
	bt_content.discoverable = false;
	bt_content.pairable = false;
	bt_content.power = false;

	//global init
	ancs_is_support = false;
	ancs_is_enable = false;

	//bt config file
	memset(&conf, 0, sizeof(struct bt_conf));
	//both BR/EDR and LE enabled, "dual", "le" or "bredr"
	conf.mode = "dual";
	//0 = disable timer, i.e. stay discoverable forever
	conf.discoverableTimeout = "1800";
	//"audio-headset"
	conf.Class = "0x240414";
	//
	conf.BleName = bt_content.ble_content.ble_name;

	//custom pincode
	conf.ssp = conf.SecureConnections = "on";
	bt_content.pincode = "1234";

	//gatt client (default true)
	//conf.gatt_client = "false";

	conf.gatt_client_cache = "no";

	bt_content.cb_auth_func = auth_func;

	create_bt_conf(&conf);
	bt_init_static_var();

	rk_debug_init(true);

	int max_retries = 3;
	do {
		if (!rk_bt_init(&bt_content))
			break;
	} while (--max_retries);

	if (!bt_content.init) {
		printf("bt_test_init failed!!!\n");
		return -1;
	}

	//printf("ble adv start\n");
	//rk_ble_adv_start();
	//sleep(1);

	rk_bt_set_discoverable(true);
	//rk_bt_set_profile(PROFILE_A2DP_SINK_HF, true);

	return 0;
}

void bt_test_bluetooth_onoff_init(char *data)
{
	int test_cnt = 5000, cnt = 0;

	if (data)
		test_cnt = atoi(data);
	printf("%s test times: %d(%d)\n", __func__, test_cnt, data ? atoi(data) : 0);

	while (cnt < test_cnt) {
		printf("BT TEST INIT START\n");
		if (bt_test_init(NULL))
			goto failed;

		//scan test
		if (rk_bt_start_discovery(SCAN_TYPE_AUTO))
			goto restart_bt;
		while (bt_content.scanning == false) {
			sleep(1);
			printf("BT SCAN ON ...\n");
		}

		sleep(10);

		if (rk_bt_cancel_discovery())
			goto restart_bt;
		while (bt_content.scanning == true) {
			sleep(1);
			printf("BT SCAN OFF ...\n");
		}

		//ble adv tests
		rk_ble_adv_start();
		while (bt_content.ble_content.ble_advertised == false) {
			sleep(1);
			printf("BT ADV ON ...\n");
		}
		sleep(3);
		rk_ble_adv_stop();
		while (bt_content.ble_content.ble_advertised == true) {
			sleep(1);
			printf("BT ADV OFF ...\n");
		}

restart_bt:
		if (rk_bt_deinit())
			goto failed;

		printf("[=================================\n");
		printf("[ BT INIT/ADV/SCAN TEST CNT: %d ]\n", ++cnt);
		printf("[=================================\n");
		sleep(1);
	}

failed:
	printf("BT TEST Failed\n");
	return;
}

void bt_test_bluetooth_init(char *data)
{
	bt_test_init(NULL);

	return;
}

void bt_test_bluetooth_deinit(char *data)
{
	rk_bt_deinit();

	return;
}

void bt_test_get_adapter_info(char *data)
{
	rk_bt_adapter_info(data);
}

/**
 * bt_test_connect_by_addr - Connect to a BT device by address and address type
 * @data: String containing the address and address type (optional)
 *
 * This function connects to a BT device by address and address type.
 *
 * The @data parameter is a string containing the address and address type
 * (optional). The address must be a string representation of a BT address,
 * and the address type must be "bredr" (BR/EDR), "le" (LE), or "auto" (auto
 * detect). If the address type is not specified, it defaults to "bredr".
 *
 * Example usage:
 *
 * "bredr":  remote_address_type == "bredr"
 * "public": remote_address_type == "public"
 * "random": remote_address_type == "random"
 * "auto"
 *
 * bt_test_connect_by_addr 00:11:22:33:44:55 bredr
 * bt_test_connect_by_addr 00:11:22:33:44:55 public
 * bt_test_connect_by_addr 00:11:22:33:44:55 random
 * bt_test_connect_by_addr 00:11:22:33:44:55 auto
 *
 * Return: None
 */
void bt_test_connect_by_addr(char *data)
{
	char *addr = NULL, *addr_type = NULL;

	if (data == NULL) {
		printf("Invaild param!\n");
		return;
	}

	addr = strtok(data, " ");
	if (addr)
		addr_type = strtok(NULL, " ");

	if (!addr_type)
		addr_type = "bredr";

	rk_bt_connect_by_addr(addr, addr_type);
}

void bt_test_connect_spp_by_addr(char *data)
{
	rk_bt_connect_spp_by_addr(data);
}

void bt_test_disconnect_by_addr(char *data)
{
	char *addr = NULL, *addr_type = NULL;

	if (data == NULL) {
		printf("Invaild param!\n");
		return;
	}

	addr = strtok(data, " ");
	if (addr)
		addr_type = strtok(NULL, " ");

	if (!addr_type)
		addr_type = "bredr";

	rk_bt_disconnect_by_addr(data, addr_type);
}

void bt_test_pair_by_addr(char *data)
{
	if (data == NULL) {
		printf("Invaild param!\n");
		return;
	}
	rk_bt_pair_by_addr(data);
}

void bt_test_unpair_by_addr(char *data)
{
	char *addr = NULL, *addr_type = NULL;

	if (data == NULL) {
		printf("Invaild param!\n");
		return;
	}

	addr = strtok(data, " ");
	if (addr)
		addr_type = strtok(NULL, " ");

	if (!addr_type)
		addr_type = "bredr";

	rk_bt_unpair_by_addr(addr, addr_type);
}

void bt_test_start_discovery(char *data)
{
	RK_BT_SCAN_TYPE type;

	if (data == NULL) {
		rk_bt_start_discovery(SCAN_TYPE_AUTO);
		return;
	}

	if (!strcmp(data, "bredr"))
		type = SCAN_TYPE_BREDR;
	else if (!strcmp(data, "le"))
		type = SCAN_TYPE_LE;
	else
		type = SCAN_TYPE_AUTO;

	rk_bt_start_discovery(type);
}

void bt_test_cancel_discovery(char *data)
{
	if (bt_content.scanning == false)
		return;

	rk_bt_cancel_discovery();
}

void bt_test_set_discoverable(char *data)
{
	bool enable;

	if (data == NULL)
		return;

	if (!strcmp(data, "on"))
		enable = 1;
	else if (!strcmp(data, "off"))
		enable = 0;
	else
		return;

	rk_bt_set_discoverable(enable);
	//system("hciconfig hci0 piscan");
	//system("hciconfig -a");
	//system("busybox ps");
	//system("echo 3 > /proc/sys/vm/drop_caches && free");
}

void bt_test_set_pairable(char *data)
{
	bool enable;

	if (data == NULL)
		return;

	if (!strcmp(data, "on"))
		enable = 1;
	else if (!strcmp(data, "off"))
		enable = 0;
	else
		return;

	rk_bt_set_pairable(enable);
}

void bt_test_set_local_name(char *data)
{
	if (data == NULL)
		return;

	rk_bt_set_local_name(data);
}

void bt_test_set_power(char *data)
{
	bool enable;

	if (data == NULL)
		return;

	if (!strcmp(data, "on"))
		enable = 1;
	else if (!strcmp(data, "off"))
		enable = 0;
	else
		return;

	rk_bt_set_power(enable);
}

void bt_test_get_all_devices(char *data)
{
	int i, count;
	struct remote_dev *rdev = NULL;

	//Get all devices
	if (bt_get_devices(&rdev, &count) < 0) {
		printf("Can't get scan list!");
		return;
	}

	for (i = 0; i < count; i++) {
		printf("Device BREDR|LE connected : %d|%d, paired %d|%d, %s|%s [%s]\n",
			rdev[i].br_connected, rdev[i].ble_connected,
			rdev[i].br_paired, rdev[i].ble_paired,
			rdev[i].remote_address,
			rdev[i].remote_address_type,
			rdev[i].remote_alias);
	}
}

static const char *class_to_icon(uint32_t class)
{
	switch ((class & 0x1f00) >> 8) {
	case 0x01:
		return "computer";
	case 0x02:
		switch ((class & 0xfc) >> 2) {
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x05:
			return "phone";
		case 0x04:
			return "modem";
		}
		break;
	case 0x03:
		return "network-wireless";
	case 0x04:
		switch ((class & 0xfc) >> 2) {
		case 0x01:
		case 0x02:
			return "audio-headset";
		case 0x06:
			return "audio-headphones";
		case 0x0b: /* VCR */
		case 0x0c: /* Video Camera */
		case 0x0d: /* Camcorder */
			return "camera-video";
		default:
			return "audio-card";	/* Other audio device */
		}
		break;
	case 0x05:
		switch ((class & 0xc0) >> 6) {
		case 0x00:
			switch ((class & 0x1e) >> 2) {
			case 0x01:
			case 0x02:
				return "input-gaming";
			}
			break;
		case 0x01:
			return "input-keyboard";
		case 0x02:
			switch ((class & 0x1e) >> 2) {
			case 0x05:
				return "input-tablet";
			default:
				return "input-mouse";
			}
		}
		break;
	case 0x06:
		if (class & 0x80)
			return "printer";
		if (class & 0x20)
			return "camera-photo";
		break;
	}

	return NULL;
}

void bt_test_read_remote_device_info(char *data)
{
	struct remote_dev rdev;
	char *t_addr = data;
	char *ios = "";

	if (data == NULL) {
		printf("Invaild param!\n");
		return;
	}

	if (bt_get_dev_info(&rdev, t_addr) < 0)
		return;

	ios = strstr(rdev.modalias, "v");
	if (ios) {
			if (!strncasecmp(ios + 1, "004c", 4) ||
			    !strncasecmp(ios + 1, "05ac", 4))
				ios = "Apple iOS system";
			else
				ios = "";
	}

	printf("Device info: addr:%s:%s, name: %s, class:(0x%x|%s|%s), appearance: 0x%x, RSSI: %d\n",
			rdev.remote_address,
			rdev.remote_address_type,
			rdev.remote_alias,
			rdev.cod, class_to_icon(rdev.cod), ios,
			rdev.appearance,
			rdev.rssi);
	printf("Supported UUIDs: \n");
	for (int index = 0; index < 10; index++) {
		if (!strcmp(rdev.remote_uuids[index], "NULL"))
			break;
		printf("	UUIDs: %s\n", rdev.remote_uuids[index]);
	}
}

/******************************************/
/*               A2DP SINK                */
/******************************************/
void bt_test_sink_media_control(char *data)
{
	if (data == NULL) {
		printf("Invaild param!\n");
		return;
	}
	rk_bt_sink_media_control(data);
}

void bt_test_a2dp_test_volume(char *data)
{
	if (data) {
		printf("===== A2DP Set Volume: %d =====\n", atoi(data));
		rk_bt_sink_set_volume(atoi(data));
	}
}

void bt_test_enable_a2dp_source(char *data)
{
	bool enable;

	if (data == NULL)
		return;

	if (!strcmp(data, "on"))
		enable = 1;
	else if (!strcmp(data, "off"))
		enable = 0;
	else
		return;

	rk_bt_set_profile(PROFILE_A2DP_SOURCE_AG, enable);
}

void bt_test_enable_a2dp_sink(char *data)
{
	bool enable;

	if (data == NULL)
		return;

	if (!strcmp(data, "on"))
		enable = 1;
	else if (!strcmp(data, "off"))
		enable = 0;
	else
		return;

	rk_bt_set_profile(PROFILE_A2DP_SINK_HF, enable);
}

/******************************************/
/*                  BLE                   */
/******************************************/
static void bt_test_ble_recv_data_callback(const char *uuid, char *data, int *len, RK_BLE_GATT_STATE state)
{
	rANCS_EVENT *p_event;

	switch (state) {
	//SERVER ROLE
	case RK_BLE_GATT_SERVER_READ_BY_REMOTE:
		//The remote dev reads characteristic and put data to *data.
		printf("+++ ble server is read by remote uuid: %s\n", uuid);
		*len = strlen("hello rockchip");
		if (*len > bt_content.ble_content.att_mtu)
			*len = bt_content.ble_content.att_mtu;
		memcpy(data, "hello rockchip", *len);
		break;
	case RK_BLE_GATT_SERVER_WRITE_BY_REMOTE:
		//The remote dev writes data to characteristic so print there.
		printf("+++ ble server is writeen by remote uuid: %s\n", uuid);
		for (int i = 0 ; i < *len; i++) {
			printf("%02x ", data[i]);
		}
		printf("\n");
		break;
	case RK_BLE_GATT_SERVER_ENABLE_NOTIFY_BY_REMOTE:
	case RK_BLE_GATT_SERVER_DISABLE_NOTIFY_BY_REMOTE:
		//The remote dev enable notify for characteristic
		printf("+++ ble server notify is %s by remote uuid: %s\n",
				(state == RK_BLE_GATT_SERVER_ENABLE_NOTIFY_BY_REMOTE) ? "enable" : "disabled",
				uuid);
		break;
	case RK_BLE_GATT_MTU:
		//Obsolete
		printf("+++ ble server MTU: %d ===\n", *(uint16_t *)data);
		break;
	case RK_BLE_GATT_SERVER_INDICATE_RESP_BY_REMOTE:
		//The service sends notify to remote dev and recvs indicate from remote dev.
		printf("+++ ble server receive remote indicate resp uuid: %s\n", uuid);
		break;

	//CLIENT ROLE
	case RK_BLE_GATT_SERVER_READ_NOT_PERMIT_BY_REMOTE:
		//error handle: org.bluez.Error.NotPermitted
		printf("+++ ble client recv error: %s +++\n", data);
	case RK_BLE_GATT_CLIENT_READ_BY_LOCAL:
		printf("+++ ble client recv from remote: data uuid:%s, len:%d+++\n", uuid, *len);
		//for (int i = 0 ; i < *len; i++) {
		//	printf("%02x ", data[i]);
		//}
		//printf("\n");

		/*
		//printf("%02x %02x %02x \n", data[0], data[123], data[246]);
		//account data rate
		totalBytes += *len * 8; // 转换为位
		gettimeofday(&now, NULL);
		long elapsed = (now.tv_sec - start.tv_sec) * 1000000 + now.tv_usec - start.tv_usec;
		if (elapsed >= 1000000) { // 每秒计算一次
			printf("Rate: %ld bits/sec [%s]\n", totalBytes / (elapsed / 1000000), uuid);
			totalBytes = 0; // 重置计数器
			start = now; // 重置时间
		}
		*/
		break;
	case RK_BLE_GATT_CLIENT_WRITE_RESP_BY_LOCAL:
		//printf("+++ ble client recv write resp msg: %p %s\n", data, data ? data : "NULL");
		break;
	case RK_BLE_GATT_CLIENT_NOTIFY_ENABLE:
	case RK_BLE_GATT_CLIENT_NOTIFY_DISABLE:
		printf("+++ ble client uuid: %s notify is %s \n",
				uuid,
				(state == RK_BLE_GATT_CLIENT_NOTIFY_ENABLE) ? "enable" : "disabled"
				);
		break;
	case RK_BLE_GATT_CLIENT_ANCS:
		p_event = (rANCS_EVENT *)data;
		printf("ANCS notification NEW [%s] %s %s %s %s\n\n", \
				p_event->appid, \
				p_event->title, \
				p_event->message, \
				p_event->positive_action_label, \
				p_event->negative_action_label);
		break;
	case RK_BLE_GATT_CLIENT_NOTIFY_ERR:
		printf("RK_BLE_GATT_CLIENT_NOTIFY_ERR\n");
	default:
		break;
	}
}

void bt_test_ble_start(char *data)
{
#ifndef BLE_ADV_CUSTOM
	//bt_content.ble_content.ble_name = "RK_BLE";

	//bt_content.ble_content.manufacturer_id = 0x0059;
	//for (int i = 0; i < 16; i++)
	//	bt_content.ble_content.manufacturer_data[i] = i + "A";
	rk_ble_adv_start();
#else
	char mac[6];

	//custom ble macaddr
	//F9:57:15:D8:F4:05
	mac[5] = 0x05;
	mac[4] = 0xF4;
	mac[3] = 0xD8;
	mac[2] = 0x15;
	mac[1] = 0x57;
	mac[0] = 0xF9;

	//70:4A:0E:6C:AC:F5
	mac[5] = 0xF5;
	mac[4] = 0xAC;
	mac[3] = 0x6C;
	mac[2] = 0x0E;
	mac[1] = 0x4A;
	mac[0] = 0x70;

	ble_direct_set_adv_param("Pixoo-le", mac);
	ble_direct_enable_adv();
#endif
}

void bt_test_ble_set_adv_interval(char *data)
{
	//default 100ms, test: 20ms(32 * 0.625) ~ 100ms(160 * 0.625)
	//rk_ble_set_adv_interval(32, 160);
}

void bt_test_ble_write(char *data)
{
	int len;

	if (data == NULL) {
		printf("Invaild param!\n");
		return;
	}

	len = strlen(data) > bt_content.ble_content.att_mtu ?
			bt_content.ble_content.att_mtu :
			strlen(data);

	rk_ble_send_notify(BLE_UUID_SEND, data, len);
}

//ONLY FOR V1.6.1
void bt_test_ble_service_changed(char *data)
{
	rk_ble_service_changed();
}

void bt_test_ble_stop(char *data) 
{
#ifdef BLE_ADV_CUSTOM
	ble_direct_close_adv();
#else
	rk_ble_adv_stop();
#endif
}

/******************************************/
/*               BLE CLIENT               */
/******************************************/
void bt_test_ble_client_get_service_info(char *data)
{
	int i, j, k;
	RK_BLE_CLIENT_SERVICE_INFO info;

	if (!rk_ble_client_get_service_info(data, &info)) {
		printf("+++++ get device(%s) service info +++++\n", data);
		for(i = 0; i < info.service_cnt; i++) {
			printf("service[%d]:\n", i);
			printf("	describe: %s\n", info.service[i].describe);
			printf("	path: %s\n", info.service[i].path);
			printf("	uuid: %s\n", info.service[i].uuid);

			for(j = 0; j < info.service[i].chrc_cnt; j++) {
				printf("	characteristic[%d]:\n", j);
				printf("		describe: %s\n", info.service[i].chrc[j].describe);
				printf("		path: %s\n", info.service[i].chrc[j].path);
				printf("		uuid: %s\n", info.service[i].chrc[j].uuid);
				printf("		props: 0x%x\n", info.service[i].chrc[j].props);
				printf("		ext_props: 0x%x\n", info.service[i].chrc[j].ext_props);
				printf("		perm: 0x%x\n", info.service[i].chrc[j].perm);
				printf("		notifying: %d\n", info.service[i].chrc[j].notifying);

				for(k = 0; k < info.service[i].chrc[j].desc_cnt; k++) {
					printf("		descriptor[%d]:\n", k);

					printf("			describe: %s\n", info.service[i].chrc[j].desc[k].describe);
					printf("			path: %s\n", info.service[i].chrc[j].desc[k].path);
					printf("			uuid: %s\n", info.service[i].chrc[j].desc[k].uuid);
				}
			}
		}
	}
}

void bt_test_ble_client_read(char *data)
{
	rk_ble_client_read(data);
}

void bt_test_ble_client_write(char *data)
{
	int len;
	char *test_data = "helle ble client";

	if (data == NULL) {
		printf("Invaild param!\n");
		return;
	}

	len = strlen(test_data) > bt_content.ble_content.att_mtu ?
			bt_content.ble_content.att_mtu :
			strlen(test_data);

	rk_ble_client_write(data, test_data, len);
}

void bt_test_ble_client_is_notify(char *data)
{
	bool notifying;

	notifying = rk_ble_client_is_notifying(BLE_UUID_SEND);
	printf("%s notifying %s\n", BLE_UUID_SEND, notifying ? "yes" : "no");
}

void bt_test_ble_client_notify_on(char *data)
{
	rk_ble_client_notify(BLE_UUID_SEND, true);
}

void bt_test_ble_client_notify_off(char *data)
{
	rk_ble_client_notify(BLE_UUID_SEND, false);
}

void bt_test_ble_client_enable_ancs(char *data)
{
	bool enable;

	if (!ancs_is_support)
		return;

	if (data == NULL) {
		printf("Invaild param! (xx input on/off)\n");
		return;
	}

	if (!strcmp(data, "on"))
		enable = true;
	else if (!strcmp(data, "off"))
		enable = false;
	else
		return;

	ancs_is_enable = enable;

	rk_ble_client_ancs(enable);
}

/**
 * VENDOR CODE
 */
static int write_flush_timeout(int fd, uint16_t handle,
		unsigned int timeout_ms)
{
	uint16_t timeout = (timeout_ms * 1000) / 625;  // timeout units of 0.625ms
	unsigned char hci_write_flush_cmd[] = {
		0x01,               // HCI command packet
		0x28, 0x0C,         // HCI_Write_Automatic_Flush_Timeout
		0x04,               // Length
		0x00, 0x00,         // Handle
		0x00, 0x00,         // Timeout
	};

	hci_write_flush_cmd[4] = (uint8_t)handle;
	hci_write_flush_cmd[5] = (uint8_t)(handle >> 8);
	hci_write_flush_cmd[6] = (uint8_t)timeout;
	hci_write_flush_cmd[7] = (uint8_t)(timeout >> 8);

	int ret = write(fd, hci_write_flush_cmd, sizeof(hci_write_flush_cmd));
	if (ret < 0) {
		printf("write(): %s (%d)]", strerror(errno), errno);
		return -1;
	} else if (ret != sizeof(hci_write_flush_cmd)) {
		printf("write(): unexpected length %d", ret);
		return -1;
	}
	return 0;
}

static int vendor_high_priority(int fd, uint16_t handle,uint8_t priority, uint8_t direction)
{
	unsigned char hci_high_priority_cmd[] = {
		0x01,               // HCI command packet
		0x1a, 0xfd,         // Write_A2DP_Connection
		0x04,               // Length
		0x00, 0x00,         // Handle
		0x00, 0x00          // Priority, Direction 
	};

	hci_high_priority_cmd[4] = (uint8_t)handle;
	hci_high_priority_cmd[5] = (uint8_t)(handle >> 8);
	hci_high_priority_cmd[6] = (uint8_t)priority;
	hci_high_priority_cmd[7] = (uint8_t)direction; 

	int ret = write(fd, hci_high_priority_cmd, sizeof(hci_high_priority_cmd));
	if (ret < 0) {
		printf("write(): %s (%d)]", strerror(errno), errno);
		return -1;
	} else if (ret != sizeof(hci_high_priority_cmd)) {
		printf("write(): unexpected length %d", ret);
		return -1;
	}
	return 0;
}

static int get_hci_sock(void)
{
	int sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	struct sockaddr_hci addr;
	int opt;

	if (sock < 0) {
		printf("Can't create raw HCI socket!");
		return -1;
	}

	opt = 1;
	if (setsockopt(sock, SOL_HCI, HCI_DATA_DIR, &opt, sizeof(opt)) < 0) {
		printf("Error setting data direction\n");
		return -1;
	}

	/* Bind socket to the HCI device */
	memset(&addr, 0, sizeof(addr));
	addr.hci_family = AF_BLUETOOTH;
	addr.hci_dev = 0;  // hci0
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		printf("Can't attach to device hci0. %s(%d)\n",
				strerror(errno),
				errno);
		return -1;
	}
	return sock;
}

static int get_acl_handle(int fd, char *bdaddr) {
	int i;
	int ret = -1;
	struct hci_conn_list_req *conn_list;
	struct hci_conn_info *conn_info;
	int max_conn = 10;
	char addr[18];

	conn_list = malloc(max_conn * (
		sizeof(struct hci_conn_list_req) + sizeof(struct hci_conn_info)));
	if (!conn_list) {
		printf("Out of memory in %s\n", __FUNCTION__);
		return -1;
	}

	conn_list->dev_id = 0;  /* hardcoded to HCI device 0 */
	conn_list->conn_num = max_conn;

	if (ioctl(fd, HCIGETCONNLIST, (void *)conn_list)) {
		printf("Failed to get connection list\n");
		goto out;
	}
	printf("%s conn_num: %d\n", __func__, conn_list->conn_num);

	for (i=0; i < conn_list->conn_num; i++) {
		conn_info = &conn_list->conn_info[i];
		memset(addr, 0, 18);
		sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x",
				conn_info->bdaddr.b[5],
				conn_info->bdaddr.b[4],
				conn_info->bdaddr.b[3],
				conn_info->bdaddr.b[2],
				conn_info->bdaddr.b[1],
				conn_info->bdaddr.b[0]);
		printf("%s: conn type: %d add: %s:%s\n", __func__, conn_info->type, bdaddr, addr);
		if (conn_info->type == ACL_LINK &&
				!strcasecmp(addr, bdaddr)) {
			ret = conn_info->handle;
			goto out;
		}
	}

	ret = 0;

out:
	free(conn_list);
	return ret;
}

/* Request that the ACL link to a given Bluetooth connection be high priority,
 * for improved coexistance support
 */
int vendor_set_high_priority(char *ba, uint8_t priority, uint8_t direction)
{
	int ret;
	int fd = get_hci_sock();
	int acl_handle;

	if (fd < 0)
		return fd;

	acl_handle = get_acl_handle(fd, ba);
	if (acl_handle <= 0) {
		ret = acl_handle;
		goto out;
	}

	ret = vendor_high_priority(fd, acl_handle, priority, direction);
	if (ret < 0)
		goto out;
	ret = write_flush_timeout(fd, acl_handle, 200);

out:
	close(fd);

	return ret;
}

void bt_test_pbap_get_vcf(char *data)
{
	char *addr;
	if (data == NULL)
		addr = bt_content.connected_a2dp_addr;
	else
		addr = data;

	rk_bt_pbap_get_vcf(addr, "pb", "/data/pb.vcf");
	rk_bt_pbap_get_vcf(addr, "ich", "/data/ich.vcf");
	rk_bt_pbap_get_vcf(addr, "och", "/data/och.vcf");
	rk_bt_pbap_get_vcf(addr, "mch", "/data/mch.vcf");
	rk_bt_pbap_get_vcf(addr, "cch", "/data/cch.vcf");
}

void bt_test_opp_send(char *data)
{
	char *addr, *send_file = "";

	if (data != NULL) {
		addr = strtok(data, " ");
		if (addr)
			send_file = strtok(NULL, " ");
	} else
		return;

	printf("addr: %s, send_file: %s\n", addr, send_file);

	rk_bt_opp_send(addr, send_file);
}

/* 
 * HFP

First, in the initialization procedure, the HF shall send the AT+BRSF=<HF supported features> command
to the AG to both notify the AG of the supported features in the HF, as well as to retrieve the supported
features in the AG using the +BRSF result code.

AT String
	//bluez-alsa send HF role supported features (bluez-alsa 发送HF角色支持的特性)
	Command									AT+BRSF=
	HF Supported Features					756
		EC And/Or NR Function				No
		Three-Way Calling					No
		CLI Presentation Capability			Yes
		Voice Recognition Activation		No
		Remote Volume Control				Yes
		Enhanced Call Status				Yes
		Enhanced Call Control				Yes
		Codec Negotiation					Yes
		HF Indicators						No
		eSCO S4 Settings Supported 			Yes
		Enhanced Voice Recognition Status	No
		Voice Recognition Text 				No

iPhone
AT String
	Response                                      +BRSF:
	AG Supported Features                         4079
		EC And/Or NR Function                       Yes
		Three-Way Calling                           Yes
		Voice Recognition Function                  Yes
		In-Band Ring Tone Capability                Yes
		Attach A Phone Number To A Voice Tag        No
		Ability To Reject A Call                    Yes
		Enhanced Call Status                        Yes
		Enhanced Call Control                       Yes
		Extended Error Results                      Yes
		Codec Negotiation                           Yes
		HF Indicators                               Yes
		eSCO S4 Settings Supported                  Yes
		Enhanced Voice Recognition Status           No
		Voice Recognition Text                      No

Android
AT String
	Response                                      +BRSF:
	AG Supported Features                         3943
		EC And/Or NR Function                       Yes
		Three-Way Calling                           Yes
		Voice Recognition Function                  Yes
		In-Band Ring Tone Capability                No
		Attach A Phone Number To A Voice Tag        No
		Ability To Reject A Call                    Yes
		Enhanced Call Status                        Yes
		Enhanced Call Control                       No
		Extended Error Results                      Yes
		Codec Negotiation                           Yes
		HF Indicators                               Yes
		eSCO S4 Settings Supported                  Yes
		Enhanced Voice Recognition Status           No
		Voice Recognition Text                      No

Secondly, in the initialization procedure, if the HF supports the Codec Negotiation feature, it shall check if
the AT+BRSF command response from the AG has indicated that it supports the Codec Negotiation
feature. If both the HF and AG do support the Codec Negotiation feature then the HF shall send the
AT+BAC=<HF available codecs> command to the AG to notify the AG of the available codecs in the HF.2

AT String
	Command	AT+BAC=
	Codec (1)	CVSD
	Codec (2)	mSBC

After having retrieved the supported features in the AG, the HF shall determine which indicators are
supported by the AG, as well as the ordering of the supported indicators. The HF uses
the AT+CIND=? Test command to retrieve information about the supported indicators and their ordering

AT String
	Command
	Test	AT+CIND=?
iPhone
AT String
	Response                                                       +CIND:
	Mobile Termination Indicators                                  ("service",(0-1)),("call",(0-1)),("callsetup",(0-3)),("battchg",(0-5)),("signal",(0-5)),("roam",(0-1)),("callheld",(0-2))
		Indicator (0)
		  Description                                                Service Availability
		  No Home/Roam network available                             0
		  Home/Roam network available                                1
		Indicator (1)
		  Description                                                Call In Progress
		  No call active                                             0
		  A call is active                                           1
		Indicator (2)
		  Description                                                Call Setup
		  Not currently in call set up                               0
		  An incoming call process ongoing                           1
		  An outgoing call set up is ongoing                         2
		  Remote party being alerted in an outgoing call             3
		Indicator (3)
		  Description                                                Battery Charge Level
		  Allowed Range (1)                                          0-5
		Indicator (4)
		  Description                                                Signal Quality
		  Allowed Range (1)                                          0-5
		Indicator (5)
		  Description                                                Roaming
		  Roaming is not active                                      0
		  A roaming is active                                        1
		Indicator (6)
		  Description                                                Call Held
		  No calls held                                              0
		  Call is placed on hold or active/held calls swapped        1
		  Call on hold, no active call                               2

Android
AT String
	Response                      +CIND:
	Mobile Termination Indicators ("call",(0,1)),("callsetup",(0-3)),("service",(0-1)),("signal",(0-5)),("roam",(0,1)),("battchg",(0-5)),("callheld",(0-2))
		Indicator (0)
		  Description              Call In Progress
		Indicator (1)
		  Description              Call Setup
		Indicator (2)
		  Description              Service Availability
		Indicator (3)
		  Description              Signal Quality
		Indicator (4)
		  Description              Roaming
		Indicator (5)
		  Description              Battery Charge Level
		Indicator (6)
		  Description              Call Held
Windows
AT String
	Response 													  +CIND:
	Mobile Termination Indicators								  ("service",(0,1)),("call",(0,1)),("callsetup",(0-3)),("callheld",(0-2)),("signal",(0-5)),("roam",(0,1)),("battchg",(0-5))
		Indicator (0)
		   Description												  Service Availability
		Indicator (1)
		   Description												  Call In Progress
		Indicator (2)
		   Description												  Call Setup
		Indicator (3)
		   Description												  Call Held
		Indicator (4)
		   Description												  Signal Quality
		Indicator (5)
		   Description												  Roaming
		Indicator (6)
		   Description												  Battery Charge Level
 */


/* AT evt callback
 *
 * The iPhone/android/Windows OS Indicator order is different
 * +CIEV: <event>,<value>
 *																	 iPhone			Android
 * no_calls_active; 		//No calls (held or active)				 +CIEV: 2,0		+CIEV: 1,0
 * call_present_active; 	//Call is present (active or held)		 +CIEV: 2,1		+CIEV: 1,1
 *
 *
  Indicator (2)
   Description												  Call Setup
   Not currently in call set up 							  0
   An incoming call process ongoing 						  1
   An outgoing call set up is ongoing						  2
   Remote party being alerted in an outgoing call			  3

   iPhone			Android
   +CIEV: 3,0		+CIEV: 2,0
   +CIEV: 3,1		+CIEV: 2,1
   +CIEV: 3,2		+CIEV: 2,2
   +CIEV: 3,3		+CIEV: 2,3
 */
void at_evt_callback(char *at_evt)
{
	if (at_evt)
		printf("[AT EVT]: %s\n", at_evt);
}

/* 
 * AT CMD:
 * pickup    接听: ATA
 * hangup    挂断: AT+CHUP
 * redial    重拨: AT+BLDN
 * dial  拨打号码: ATD18812345678;  //注意: 拨号要在最后加分号“;”
 * volume    音量: AT+VGS=[0-6]
 * Phone 来电显示: AT+CLIP=1 //at_evt: +CLIP: "059183991906",129,,," (0591) 8399 1906 "
 */
void bt_test_rfcomm_send(char *data)
{
	if (data == NULL) {
		printf("Invaild param!\n");
		return;
	}

	printf("data: %s\n", data);

	rk_bt_rfcomm_send(data);
}

void bt_test_adapter_connect(char *data)
{
	char *addr;
	char *ble_addr_type = "";

	if (data == NULL) {
		printf("Invaild param!\n");
		return;
	}

	printf("data: %s\n", data);

	addr = strtok(data, " ");
	if (addr)
		ble_addr_type = strtok(NULL, " ");

	printf("addr: %s, ble_addr_type: %s\n", addr, ble_addr_type);

	rk_adapter_connect(addr, ble_addr_type);
}
