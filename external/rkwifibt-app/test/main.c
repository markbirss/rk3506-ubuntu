/*
 * Copyright (c) 2017 Rockchip, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/select.h>
#include <linux/input.h>
#include <stdbool.h>

#include <signal.h>

#include "bt_test.h"
#include "rk_ble_app.h"
#include "rk_wifi_test.h"
#include "utility.h"

static bool main_loop_flag;

static void rkwifibt_test_bluetooth();
static void rkwifibt_test_wifi_config();
static void rkwifibt_test_network_config();

typedef struct {
	const char *cmd;
	const char *desc;
	void (*action)(void);
} menu_command_t;

static menu_command_t menu_command_table[] = {
	{"BT", "show bluetooth test cmd menu", rkwifibt_test_bluetooth},
	{"WiFi", "show wifi config test cmd menu", rkwifibt_test_wifi_config},
	{"Network", "show ble wifi config test cmd menu", rkwifibt_test_network_config},
};

static void show_bt_cmd();
static void show_wifi_cmd();
static void show_config_cmd();

typedef struct {
	const char *cmd;
	void (*action)(char *data);
} command_t;

static command_t network_config_command_table[] = {
	{"", NULL},
	{"ble config wifi start     	(num_index)", rk_ble_wifi_init},
	{"ble config wifi stop      	(num_index)", rk_ble_wifi_deinit},
	{"ble config wifi looptest  	(num_index)", rk_ble_wifi_init_onoff_test},
	{"softap config start       	(num_index)", rk_wifi_softap_start},
	{"softap config stop        	(num_index)", rk_wifi_softap_stop},
};

static command_t wifi_config_command_table[] = {
	{"", NULL},
	{"wifi open                 	(num_index)", rk_wifi_open},
	{"wifi close                	(num_index)", rk_wifi_close},
	{"wifi connect              	(num_index input ssid password WPA/WPA3/WEP/NONE)", rk_wifi_connect},
	{"wifi get saved info       	(num_index)", rk_wifi_getSavedInfo},
	{"wifi get currit info      	(num_index)", rk_wifi_getConnectionInfo},
	{"wifi connect with ssid    	(num_index input ssid WPA/WPA3/WEP/NONE)", rk_wifi_connect_with_ssid},
	{"wifi cancel               	(num_index)", rk_wifi_cancel},
	{"wifi forget with ssid     	(num_index input ssid WPA/WPA3/WEP/NONE))", rk_wifi_forget_with_ssid},
	{"wifi discon               	(num_index)", rk_wifi_disconnect},
	{"wifi version              	(num_index)", rk_wifi_version},
	{"wifi switch test          	(num_index)", rk_wifi_onoff_test},
};

static command_t bt_command_table[] = {
	{"", NULL},
	{"bt init                   	(num_index)", bt_test_bluetooth_init},
	{"bt switch test loop       	(num_index input test_cnt)", bt_test_bluetooth_onoff_init},
	{"bt version                	(num_index)", bt_test_version},
	{"bt adapter power          	(num_index input on/off)", bt_test_set_power},
	{"bt adapter discoverable   	(num_index input on/off)", bt_test_set_discoverable},
	{"bt adapter pairable       	(num_index input on/off)", bt_test_set_pairable},
	{"bt start scan             	(num_index input auto/bredr/le)", bt_test_start_discovery},
	{"bt stop scan              	(num_index)", bt_test_cancel_discovery},
	{"bt get remote device info 	(num_index input addr)", bt_test_read_remote_device_info},
	{"bt get all devices info   	(num_index)", bt_test_get_all_devices},
	{"bt pair ble by addr       	(num_index input addr)", bt_test_pair_by_addr},
	{"bt connect by addr        	(num_indexcaddr_type)", bt_test_connect_by_addr},
	{"bt connect spp by addr    	(num_index input addr)", bt_test_connect_spp_by_addr},
	{"bt disconnect by addr     	(num_index input addr addr_type)", bt_test_disconnect_by_addr},
	{"bt_test_remove_by_addr    	(num_index input addr addr_type)", bt_test_unpair_by_addr},
	{"bt set a2dp volume        	(num_index input volume)", bt_test_a2dp_test_volume},
	{"bt set bredr name         	(num_index input name)", bt_test_set_local_name},
	{"bt enable a2dp sink       	(num_index)", bt_test_enable_a2dp_sink},
	{"bt enable a2dp source     	(num_index)", bt_test_enable_a2dp_source},
	{"bt a2dp source play       	(num_index)", bt_test_source_play},
	{"bt a2dp sink avrcp control	(num_index input play/pause/next/previous)", bt_test_sink_media_control},
	{"ble adv start             	(num_index)", bt_test_ble_start},
	{"ble adv stop              	(num_index)", bt_test_ble_stop},
	{"ble set adv interval      	(num_index)", bt_test_ble_set_adv_interval},
	{"ble server send notify    	(num_index input raw(ascii))", bt_test_ble_write},
	{"ble server changed        	(num_index)", bt_test_ble_service_changed},
	{"ble client get s/c/d info 	(num_index input uuid)", bt_test_ble_client_get_service_info},
	{"ble client read           	(num_index input uuid)", bt_test_ble_client_read},
	{"ble client write          	(num_index input uuid)", bt_test_ble_client_write},
	{"ble client enable notify  	(num_index input uuid)", bt_test_ble_client_notify_on},
	{"ble client disable notify 	(num_index input uuid)", bt_test_ble_client_notify_off},
	{"ble client ancs           	(num_index input on/off)", bt_test_ble_client_enable_ancs},
	{"bt get phone book         	(num_index input addr)", bt_test_pbap_get_vcf},
	{"bt opp send file          	(num_index input addr file_path)", bt_test_opp_send},
	{"bt send hfp at command    	(num_index input ATCMD)", bt_test_rfcomm_send},
	{"bt adapter connect        	(num_index)", bt_test_adapter_connect},
	{"bt_server_close           	(num_index)", bt_test_bluetooth_deinit},
};

static void show_config_cmd() {
	unsigned int i;
	for (i = 1; i < sizeof(network_config_command_table) /
		 sizeof(network_config_command_table[0]); i++)
		printf("%02d.  %s \n", i, network_config_command_table[i].cmd);
}

static void show_wifi_cmd() {
	unsigned int i;
	for (i = 1; i < sizeof(wifi_config_command_table)
		 / sizeof(wifi_config_command_table[0]); i++)
		printf("%02d.  %s \n", i, wifi_config_command_table[i].cmd);
}

static void show_bt_cmd() {
	unsigned int i;
	for (i = 1; i < sizeof(bt_command_table)
		 / sizeof(bt_command_table[0]); i++)
		printf("%02d.  %s \n", i, bt_command_table[i].cmd);
}

static void show_help(char *bin_name) {
	unsigned int i;
	printf("%s [Usage]:\n", bin_name);
	for (i = 0; i < sizeof(menu_command_table)
		 /sizeof(menu_command_t); i++)
		printf("\t\"%s %s\":%s.\n", bin_name,
				menu_command_table[i].cmd,
				menu_command_table[i].desc);
}

static void rkwifibt_test_network_config()
{
	int i, item_cnt;
	char *input_start;
	char cmdBuf[64] = {0};
	char szBuf[64] = {0};
	char szBuf_space[64] = {0};

	item_cnt = sizeof(network_config_command_table) / sizeof(command_t);
	show_config_cmd();

	while (main_loop_flag) {
		printf("Please input number or help to run: \n");

		memset(szBuf, 0, sizeof(szBuf));
		if (fgets(szBuf_space, 64, stdin) == NULL)
			continue;

		if (!strncmp("help", szBuf_space, 4) || !strncmp("h", szBuf_space, 1))
			show_config_cmd();

		strncpy(szBuf, szBuf_space, strlen(szBuf_space) - 1);

		input_start = strstr(szBuf, "input");
		if (input_start == NULL) {
			i = atoi(szBuf);
			if ((i >= 1) && (i < item_cnt))
				network_config_command_table[i].action(NULL);
		} else {
			memset(cmdBuf, 0, sizeof(cmdBuf));
			strncpy(cmdBuf, szBuf, strlen(szBuf) - strlen(input_start) - 1);
			i = atoi(cmdBuf);
			if ((i >= 1) && (i < item_cnt))
				network_config_command_table[i].action(input_start + strlen("input") + 1);
		}
	}

	return;
}

static void rkwifibt_test_wifi_config()
{
	int i, item_cnt;
	char *input_start;
	char cmdBuf[256] = {0};
	char szBuf[256] = {0};
	char szBuf_space[256] = {0};

	item_cnt = sizeof(wifi_config_command_table) / sizeof(command_t);
	show_wifi_cmd();

	while (main_loop_flag) {
		printf("Please input number or help to run: \n");

		memset(szBuf, 0, sizeof(szBuf));
		if (fgets(szBuf_space, 64, stdin) == NULL)
			continue;

		if (!strncmp("help", szBuf_space, 4) || !strncmp("h", szBuf_space, 1))
			show_wifi_cmd();

		strncpy(szBuf, szBuf_space, strlen(szBuf_space) - 1);

		input_start = strstr(szBuf, "input");
		if (input_start == NULL) {
			i = atoi(szBuf);
			if ((i >= 1) && (i < item_cnt))
				wifi_config_command_table[i].action(NULL);
		} else {
			memset(cmdBuf, 0, sizeof(cmdBuf));
			strncpy(cmdBuf, szBuf, strlen(szBuf) - strlen(input_start) - 1);
			i = atoi(cmdBuf);
			if ((i >= 1) && (i < item_cnt))
				wifi_config_command_table[i].action(input_start + strlen("input") + 1);
		}
	}

	return;
}

static void rkwifibt_test_bluetooth()
{
	int i, item_cnt;
	char *input_start;
	char cmdBuf[64] = {0};
	char szBuf[64] = {0};
	char szBuf_space[64] = {0};

	item_cnt = sizeof(bt_command_table) / sizeof(command_t);
	show_bt_cmd();

	while (main_loop_flag) {
		memset(szBuf, 0, sizeof(szBuf));
		printf("Please input number or help to run: \n");

		if (fgets(szBuf_space, 64, stdin) == NULL)
			continue;

		if (!strncmp("help", szBuf_space, 4)
			|| !strncmp("h", szBuf_space, 1)
			|| !strncmp("H", szBuf_space, 1)
			)
			show_bt_cmd();

		strncpy(szBuf, szBuf_space, strlen(szBuf_space) - 1);

		input_start = strstr(szBuf, "input");
		if (input_start == NULL) {
			i = atoi(szBuf);
			if ((i >= 1) && (i < item_cnt))
				bt_command_table[i].action(NULL);
		} else {
			memset(cmdBuf, 0, sizeof(cmdBuf));
			strncpy(cmdBuf, szBuf, strlen(szBuf) - strlen(input_start) - 1);
			i = atoi(cmdBuf);
			if ((i >= 1) && (i < item_cnt))
				bt_command_table[i].action(input_start + strlen("input") + 1);
		}
	}

	return;
}

static void main_loop_stop(int sig)
{
	/* Call to this handler restores the default action, so on the
	 * second call the program will be forcefully terminated.
	 */
	struct sigaction sigact = { .sa_handler = SIG_DFL };
	sigaction(sig, &sigact, NULL);
	main_loop_flag = false;

	bt_test_bluetooth_deinit(NULL);
	//exec_command_system("echo 0 > /sys/class/rfkill/rfkill0/state");
}

int main(int argc, char *argv[])
{
	int i, item_cnt;
	int day, year;
	char month[4];
	const char *dateString = __DATE__;

	if (sscanf(dateString, "%s %d %d", month, &day, &year) == 3) {
		printf("rkwifibt_test version: [%d-%s-%d:%s]\n", year, month, day, __TIME__);
	}

	struct sigaction sigact = { .sa_handler = main_loop_stop };
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGINT, &sigact, NULL);

	main_loop_flag = true;

	//RK_read_version(version, 64);
	item_cnt = sizeof(menu_command_table) / sizeof(menu_command_t);

	if (argc < 2) {
		printf("ERROR:invalid argument.\n");
		show_help(argv[0]);
		return -1;
	}

	if ((!strncmp(argv[1], "-h", 2)) || (!strncmp(argv[1], "help", 4))) {
		show_help(argv[0]);
		return 0;
	}

	for (i = 0; i < item_cnt; i++) {
		if (!strncmp(argv[1], menu_command_table[i].cmd, strlen(menu_command_table[i].cmd)))
			break;
	}

	if (i >= item_cnt) {
		printf("ERROR:invalid menu cmd.\n");
		show_help(argv[0]);
		return -1;
	}

	//ensure wpa_supplicant.conf
	system("ls -al /data/ && cat /data/cfg/wpa_supplicant.conf");
	if (access("/data/cfg/wpa_supplicant.conf", F_OK) != 0) {
		system("mkdir -p /data/cfg");
		system("cp /etc/wpa_supplicant.conf /data/cfg/wpa_supplicant.conf");
	}
	menu_command_table[i].action();

	while (main_loop_flag)
		sleep(1);

	return 0;
}
