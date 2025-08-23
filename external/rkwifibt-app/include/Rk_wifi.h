#ifndef __RK_WIFI_H__
#define __RK_WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

#define RK_WIFI_VERSION "V1.1"

#define RK_WIFI_SAVED_INFO_MAX 10
#define SSID_MAX_LEN 32
#define BSSID_BUF_LEN 20
#define STATE_BUF_LEN 20

#define SSID_BUF_LEN SSID_MAX_LEN * 4 + 1

typedef enum {
	DHCP_SYSTEM,
	DHCP_DHCPCD,
	DHCP_UDHCPC,
} RK_WIFI_DHCP_SERVER;

typedef enum {
	RK_WIFI_State_IDLE = 0,
	RK_WIFI_State_CONNECTING,
	RK_WIFI_State_CONNECTFAILED,
	RK_WIFI_State_CONNECTFAILED_WRONG_KEY,
	RK_WIFI_State_CONNECTED,
	RK_WIFI_State_DISCONNECTED,
	RK_WIFI_State_OPEN,
	RK_WIFI_State_OFF,
	RK_WIFI_State_SCAN_RESULTS,
	RK_WIFI_State_DHCP_OK,
} RK_WIFI_RUNNING_State_e;

typedef enum {
	NONE = 0,	//open system
	WPA,		//TKIP and AES
	WEP,		//WEPXX with shared key mode
	WPA3,		//WPA3-SAE Mode (MFPC=1, MFPR=1)
	WPA2_WPA3	//WPA3-SAE Transition Mode (MFPC=1, MFPR=0)
} RK_WIFI_KEY_MGMT;

typedef struct {
	int id;
	char bssid[BSSID_BUF_LEN];
	char ssid[SSID_MAX_LEN * 4 + 1];
	int freq;
	char mode[20];
	char wpa_state[20];
	char ip_address[20];
	char mac_address[20];
	char key_mgmt[20];
	int reason;
} RK_WIFI_INFO_Connection_s;

typedef struct {
	int id;
	char bssid[BSSID_BUF_LEN];
	char ssid[SSID_MAX_LEN * 4 + 1];
	char state[STATE_BUF_LEN];
	char key_mgmt[20];
} RK_WIFI_SAVED_INFO_s;

typedef int(*RK_wifi_state_callback)(RK_WIFI_RUNNING_State_e state, RK_WIFI_INFO_Connection_s *info);

/**
 * @brief  Set the DHCP server for the WiFi module.
 *
 * @param dhcp The DHCP server to be set. Possible values are:
 *             - DHCP_SYSTEM: use system DHCP server.
 *             - DHCP_DHCPCD: use dhcpcd
 *             - DHCP_UDHCPC: use udhcpc
 */
void RK_wifi_set_dhcp_server(RK_WIFI_DHCP_SERVER dhcp);


/**
 * Registers a callback function for WiFi state changes.
 *
 * @param cb The callback function to be registered.
 *
 * @return 1 if the callback function is successfully registered, otherwise 0.
 *
 * @throws None
 */
int RK_wifi_register_callback(RK_wifi_state_callback cb);


/**
 * Retrieves the connection information of the running WiFi.
 *
 * @param pInfo Pointer to a RK_WIFI_INFO_Connection_s structure to store the connection information.
 *
 * @return 0 on success, -1 on failure.
 *
 * @throws None.
 */
int RK_wifi_running_getConnectionInfo(RK_WIFI_INFO_Connection_s* pInfo);

/**
 * @brief  Enable or disable the WiFi module.
 *
 * @param enable 1 to enable WiFi, 0 to disable WiFi.
 * @param conf_dir The directory where WiFi configuration files are stored.
 *                 If NULL, the default configuration directory is used.
 *                 (/data/cfg/wpa_supplicant.conf)
 *
 * @return 0 on success, -1 on failure.
 *
 * @throws None
 */
int RK_wifi_enable(int enable, const char *conf_dir);

/**
 * @brief  Start sta basic scanning in all channels
 */
int RK_wifi_scan(void);

/**
 * @brief  Get station scan result.
 */
char *RK_wifi_scan_r(void);

/**
 * @brief  Connects to a Wi-Fi network.
 *
 * @param ssid The SSID (network name) of the Wi-Fi network.
 * @param psk The pre-shared key (password) of the Wi-Fi network.
 * @param key_mgmt The key management method for the Wi-Fi network.
 *                 Possible values are:
 *                 - WPA: WPA/WPA2 Personal
 *                 - WEP: Wired Equivalent Privacy
 *                 - WPA3: WPA3 Personal
 * @param bssid The BSSID (network address) of the Wi-Fi network,
 *              in the format "xx:xx:xx:xx:xx:xx". If NULL, the first
 *              network with the specified SSID will be connected.
 *
 * @return 0 on success, -1 on failure.
 *
 * @throws None
 */
int RK_wifi_connect(char *ssid, const char *psk, RK_WIFI_KEY_MGMT key_mgmt, char *bssid);

/**
 * @brief  Start sta basic scanning in all channels
 */
int RK_wifi_disconnect_network(void);

/**
 * @brief  Get original macaddr of wlan0
 */
int RK_wifi_get_mac(char *wifi_mac);

/**
 * @brief  Start sta basic scanning in all channels
 */
int RK_wifi_connect_with_ssid(const char *ssid, RK_WIFI_KEY_MGMT key_mgmt);
int RK_wifi_forget_with_ssid(const char *ssid, RK_WIFI_KEY_MGMT key_mgmt);

/**
 * @brief  Get status of sta
 */
int RK_wifi_running_getState(RK_WIFI_RUNNING_State_e* pState);

/**
 * @brief  Start all sta saved info.
 */
int RK_wifi_getSavedInfo(RK_WIFI_SAVED_INFO_s **pInfo, int *ap_cnt);

/**
 * @brief  Cancel the current "Connecting" but not connected state.
 */
int RK_wifi_cancel(void);

/**
 * @brief  Remove all sta info
 */
int RK_wifi_reset(void);

/**
 * @brief  scan for softap config "ONLY"
 */
char *RK_wifi_scan_for_softap(void);

/**
 * @brief  Get wifi so version
 */
char *RK_wifi_version(void);

/**
 * @brief  Set bssid
 */
void RK_wifi_set_bssid(char *bssid);

#ifdef __cplusplus
}
#endif

#endif
