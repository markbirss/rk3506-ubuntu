#ifndef __BT_BASE_H__
#define __BT_BASE_H__

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Compatibility patch for glib < 2.68. */
//#if !GLIB_CHECK_VERSION(2, 68, 0)
//# define g_memdup2 g_memdup
//#endif

//old gcc
//#define g_memdup2 g_memdup

#define MAX_NAME_LENGTH			247
#define MAX_UUID_STR_LENGTH		36
#define MAX_UUID_INDEX			36
#define MAX_MTPOINT_STR_LENGTH	64
#define MAX_MTPOINT_MAX			36

//pre-defile (CONST)
#define MAX_GATT_SERVICE			8
#define MAX_GATT_CHARACTERISTIC		8

#define MXA_ADV_DATA_LEN			32
#define DEVICE_ADDR_LEN				6

#define BT_ATT_DEFAULT_LE_MTU		23
#define BT_ATT_MAX_LE_MTU			517
#define BT_ATT_MAX_VALUE_LEN		512
#define BT_ATT_HEADER_LEN			3

#define RK_BT_TRANSPORT_UNKNOWN		0
#define RK_BT_TRANSPORT_BR_EDR		1
#define RK_BT_TRANSPORT_LE			2

//UUID						 0000110A-0000-1000-8000-00805F9B34FB
#define BT_UUID_A2DP_SOURCE "0000110A-0000-1000-8000-00805F9B34FB"
#define BT_UUID_A2DP_SINK   "0000110B-0000-1000-8000-00805F9B34FB"
#define BT_UUID_HSP_HS      "00001108-0000-1000-8000-00805F9B34FB"
#define BT_UUID_HSP_AG      "00001112-0000-1000-8000-00805F9B34FB"
#define BT_UUID_HFP_HF      "0000111E-0000-1000-8000-00805F9B34FB"
#define BT_UUID_HFP_AG      "0000111F-0000-1000-8000-00805F9B34FB"


typedef struct {
	char *uuid;
	char **chr_props;
	bool notify;
	bool indicate;
} BLE_UUID_TYPE;

enum {
	BLE_ADVDATA_TYPE_USER = 0, //deprecated!!!
	BLE_ADVDATA_TYPE_SYSTEM
};

/* BT state */
typedef enum {
	/* adapter state */
	RK_BT_STATE_NONE,
	RK_BT_STATE_INIT_OFF,
	RK_BT_STATE_INIT_ON,
	RK_BT_STATE_TURNING_ON,
	RK_BT_STATE_TURNING_OFF,

	/* adapter SCAN state */
	RK_BT_STATE_SCAN_STARTED,
	RK_BT_STATE_SCAN_START_FAILED,
	RK_BT_STATE_SCAN_NEW_REMOTE_DEV,
	RK_BT_STATE_SCAN_DEL_REMOTE_DEV,
	RK_BT_STATE_SCAN_CHG_REMOTE_DEV,
	RK_BT_STATE_SCAN_STOPPED = 10,

	/* device state */
	RK_BT_STATE_DISCONN_FAILED,
	RK_BT_STATE_DISCONN,
	RK_BT_STATE_BLE_DISCONN,
	RK_BT_STATE_DISCONN_ALREADY,
	RK_BT_STATE_CONNECT_FAILED,
	RK_BT_STATE_CONNECT_BLE_FAILED,
	RK_BT_STATE_CONNECT_FAILED_INVAILD_ADDR,
	RK_BT_STATE_CONNECT_FAILED_NO_FOUND_DEVICE,
	RK_BT_STATE_CONNECT_FAILED_SCANNING,
	RK_BT_STATE_CONNECTED_ALREADY,
	RK_BT_STATE_CONNECTED,
	RK_BT_STATE_SPP_CONNECTED,
	RK_BT_STATE_SPP_RECV_DATA,
	RK_BT_STATE_SPP_DISCONNECTED,
	RK_BT_STATE_BLE_CONNECTED,
	RK_BT_STATE_BOND_NONE,
	RK_BT_STATE_BONDED,
	RK_BT_STATE_BLE_BONDED,
	RK_BT_STATE_PAIR_NONE,
	RK_BT_STATE_BLE_PAIR_NONE,
	RK_BT_STATE_PAIR_FAILED,
	RK_BT_STATE_BLE_PAIR_FAILED,
	RK_BT_STATE_PAIRED,
	RK_BT_STATE_BLE_PAIRED,
	RK_BT_STATE_DEL_DEV_OK,
	RK_BT_STATE_DEL_DEV_FAILED,
	RK_BT_STATE_INFO_CHANGE,

	/* adapter state */
	RK_BT_STATE_ADAPTER_POWER_ON,
	RK_BT_STATE_ADAPTER_POWER_OFF,
	RK_BT_STATE_ADAPTER_DISCOVERYABLED,
	RK_BT_STATE_ADAPTER_NO_DISCOVERYABLED,
	RK_BT_STATE_ADAPTER_PAIRABLED,
	RK_BT_STATE_ADAPTER_NO_PAIRABLED,
	RK_BT_STATE_ADAPTER_SCANNING,
	RK_BT_STATE_ADAPTER_NO_SCANNING,
	RK_BT_STATE_ADAPTER_NAME_CHANGE,
	RK_BT_STATE_ADAPTER_BLE_ADV_START,
	RK_BT_STATE_ADAPTER_BLE_ADV_STOP,
	RK_BT_STATE_ADAPTER_BLE_ADV_ERR,
	RK_BT_STATE_ADAPTER_LOCAL_NAME,

	/* common cmd resp */
	RK_BT_STATE_COMMAND_RESP_ERR,
	RK_BT_STATE_COMMAND_RESP_OK,

	/* a2dp transport */
	RK_BT_STATE_TRANSPORT_IDLE,			/** Not acquired and suspended */
	RK_BT_STATE_TRANSPORT_PENDING,		/** Playing but not acquired */
	RK_BT_STATE_TRANSPORT_REQUESTING,	/** Acquire in progress */
	RK_BT_STATE_TRANSPORT_ACTIVE,		/** Acquired and playing */
	RK_BT_STATE_TRANSPORT_SUSPENDING,	/** Release in progress */
	RK_BT_STATE_TRANSPORT_VOLUME,		/** volume */

	//avrcp
	RK_BT_STATE_SINK_PLAY,
	RK_BT_STATE_SINK_PAUSE,
	RK_BT_STATE_SINK_STOP,
	RK_BT_STATE_SINK_TRACK,        /** track info */
	RK_BT_STATE_SINK_POSITION,        /** track position */

	/* a2dp event */
	RK_BT_STATE_SINK_ADD,
	RK_BT_STATE_SINK_DEL,
	RK_BT_STATE_SRC_ADD,
	RK_BT_STATE_SRC_DEL,

	RK_BT_STATE_ANCS
	/* profile state */
} RK_BT_STATE;

/* input event */
typedef enum {
	/** local client role - operate by local */
	RK_BLE_GATT_CLIENT_READ_BY_LOCAL,
	RK_BLE_GATT_CLIENT_READ_BY_LOCAL_ERR,
	RK_BLE_GATT_CLIENT_WRITE_RESP_BY_LOCAL,
	RK_BLE_GATT_CLIENT_WRITE_RESP_BY_LOCAL_ERR,
	RK_BLE_GATT_CLIENT_ANCS,
	/* call gatt_client_notify(UUID, enable) API
	 * event:
	 * iface: org.bluez.GattCharacteristic1, 
	 * path: /org/bluez/hci0/dev_63_A1_00_00_01_05/service0048/char004c,
	 * name: Notifying
	 *
	 * Attribute /org/bluez/hci0/dev_63_A1_00_00_01_05/service0048/char004c Notifying: yes
	 */
	RK_BLE_GATT_CLIENT_NOTIFY_ENABLE,
	RK_BLE_GATT_CLIENT_NOTIFY_DISABLE,
	RK_BLE_GATT_CLIENT_NOTIFY_ERR,
	//RK_BLE_GATT_CLIENT_INDICATED,

	//Deprecated !!!
	//device mtu: ref: remote_dev att_mtu
	RK_BLE_GATT_MTU,

	/** local server role - operate by remote */
	RK_BLE_GATT_SERVER_READ_BY_REMOTE,
	RK_BLE_GATT_SERVER_READ_NOT_PERMIT_BY_REMOTE,
	RK_BLE_GATT_SERVER_WRITE_BY_REMOTE,
	RK_BLE_GATT_SERVER_INDICATE_RESP_BY_REMOTE,
	RK_BLE_GATT_SERVER_ENABLE_NOTIFY_BY_REMOTE,
	RK_BLE_GATT_SERVER_DISABLE_NOTIFY_BY_REMOTE,
	RK_BLE_GATT_SERVER_ERR_NOTIFY_BY_REMOTE,

	//CMD STATUS
	RK_BLE_GATT_CMD_CLIENT_READ_OK,
	RK_BLE_GATT_CMD_CLIENT_WRITE_OK,
	RK_BLE_GATT_CMD_CLIENT_NOTIFYD_OK,
	RK_BLE_GATT_CMD_CLIENT_READ_ERR,
	RK_BLE_GATT_CMD_CLIENT_WRITE_ERR,
	RK_BLE_GATT_CMD_CLIENT_NOTIFYD_ERR,

	RK_BLE_GATT_CMD_SERVER_NOTIFY_OK,
	RK_BLE_GATT_CMD_SERVER_ENABLE_NOTIFY_OK,
	RK_BLE_GATT_CMD_SERVER_DISABLE_NOTIFY_OK,
	RK_BLE_GATT_CMD_SERVER_NOTIFY_ERR,
	RK_BLE_GATT_CMD_SERVER_ENABLE_NOTIFY_ERR,
	RK_BLE_GATT_CMD_SERVER_DISABLE_NOTIFY_ERR,

	RK_BLE_GATT_CMD_OK,
	RK_BLE_GATT_CMD_ERR,
} RK_BLE_GATT_STATE;

typedef enum {
	SCAN_TYPE_AUTO,		/**< BREDR AND LE */
	SCAN_TYPE_BREDR,	/**< BREDR */
	SCAN_TYPE_LE,		/**< LE */
} RK_BT_SCAN_TYPE;

typedef enum {
	RK_BT_DEV_PLATFORM_UNKNOWN = 0,	/**< unknown platform */
	RK_BT_DEV_PLATFORM_IOS,			/**< Apple iOS */
} RK_BT_DEV_PLATFORM_TYPE;

/* bt control cmd */
enum BtControl {
	BT_PLAY,
	BT_PAUSE_PLAY,
	BT_RESUME_PLAY,
	BT_VOLUME_UP,
	BT_VOLUME_DOWN,
	BT_AVRCP_FWD,
	BT_AVRCP_BWD,
	BT_AVRCP_STOP,
};

typedef struct {
	BLE_UUID_TYPE server_uuid;
	BLE_UUID_TYPE chr_uuid[MAX_GATT_CHARACTERISTIC];
	/** characteristic cnt */
	uint8_t chr_cnt;
} RkBleGattService;
#define RK_DEV_DISCONN_UNKNOWN				0x00
#define RK_DEV_DISCONN_TIMEOUT				0x01
#define RK_DEV_DISCONN_LOCAL_HOST			0x02
#define RK_DEV_DISCONN_REMOTE				0x03
#define RK_DEV_DISCONN_AUTH_FAILURE			0x04
#define RK_DEV_DISCONN_LOCAL_HOST_SUSPEND	0x05

static char *DIS_REASON_ID[] =
{
	"UNKNOW",
	"TIMEOUT",
	"LOCAL_HOST",
	"REMOTE",
	"AUTH_FAILURE",
	"LOCAL_HOST_SUSPEND",
};

/* ANCS event description */
typedef struct
{
	uint32_t    *padding;
	uint32_t    notification_uid;
	uint8_t     command;
	uint8_t     flags;
	uint8_t     category;
	uint8_t     categoryCnt;
	uint8_t     appid[128];
	uint8_t     title[20];
	uint8_t     message[255];
	uint8_t     positive_action_label[32];
	uint8_t     negative_action_label[32];
} rANCS_EVENT;

struct bt_conf {
	/* Restricts all controllers to the specified transport. Default value
	 * is "dual", i.e. both BR/EDR and LE enabled (when supported by the HW).
	 * Possible values: "dual", "bredr", "le"
	 */
	const char *mode;

	/* ref: static const char *class_to_icon(uint32_t class) */
	const char *Class;

	/* simple secure paired */
	const char *ssp;
	/*
	 * # Default Secure Connections setting.
	 * # Enables the Secure Connections setting for adapters that support it. It
	 * # provides better crypto algorithms for BT links and also enables CTKD (cross
	 * # transport key derivation) during pairing on any link.
	 * # Possible values: "off", "on", "only"
	 * # - "off": Secure Connections are disabled
	 * # - "on": Secure Connections are enabled when peer device supports them
	 * # - "only": we allow only Secure Connections
	 * # Defaults to "on"
	 */
	const char *SecureConnections;

	//gap name
	const char *BleName;
	/*
	 * How long to stay in discoverable mode before going back to non-discoverable
	 * The value is in seconds. Default is 180, i.e. 3 minutes.
	 * 0 = disable timer, i.e. stay discoverable forever
	 * DiscoverableTimeout = 0
	 */
	const char *discoverableTimeout;

	/* Specify the policy to the JUST-WORKS repairing initiated by peer
	 * Possible values: "never", "confirm", "always" 
	 * Defaults to "never" 
	 */
	const char *JustWorksRepairing;

	/*
	 * # This enables the GATT client functionally, so it can be disabled in system
	 * # which can only operate as a peripheral.
	 * # Defaults to 'true'.
	 */
	const char *gatt_client;

	/*
	 * # GATT attribute cache.
	 * # Possible values:
	 * # always: Always cache attributes even for devices not paired, this is
	 * # recommended as it is best for interoperability, with more consistent
	 * # reconnection times and enables proper tracking of notifications for all
	 * # devices.
	 * # yes: Only cache attributes of paired devices.
	 * # no: Never cache attributes
	 * # Default: always
	 */
	const char *gatt_client_cache;
};

/*
 * https://www.bluetooth.com/specifications/specs/core-specification-supplement-9/
 * Core Specification Supplement 9
 *
 * CSS_v9.pdf
 * Assigned_Numbers.pdf
 * GATT_Specification_Supplement_v10.pdf
 */
typedef struct {
	/** ble controller name */
	char ble_name[32];

	/** standard ble advtertise data */
	/**
	 * SERVICE UUID
	 * GAP and GATT service UUIDs should not be included in a Service UUIDs AD type,
	 * for either a complete or incomplete list.
	 *
	 * The Service UUID data type is used to include a list of Service or Service Class UUIDs. \n
	 * 16/32/128-bit Bluetooth Service UUIDs.
	 *
	 * 16-bit and 32-bit UUIDs shall only be used if they are assigned by the Bluetooth SIG.\n
	 * The Bluetooth SIG may assign 16-bit and 32-bit UUIDs to member companies or organizations.
	 *
	 * #define BT_AD_UUID16_SOME		0x02 \n
	 * #define BT_AD_UUID16_ALL			0x03 \n
	 * #define BT_AD_UUID32_SOME		0x04 \n
	 * #define BT_AD_UUID32_ALL			0x05 \n
	 * #define BT_AD_UUID128_SOME		0x06 \n
	 * #define BT_AD_UUID128_ALL		0x07 \n
	 */
	BLE_UUID_TYPE adv_server_uuid;

	/**
	 * The TX Power Level data type indicates
	 * the transmitted power level of the packet containing the data type. \n
	 * The TX Power Level should be the radiated power level. \n
	 * 
	 * If the power level is included in a TX Power Level
	 * AD Structure (see [Vol 3] Part C, Section 11) created by the Host,
	 * then **the Host should set the value to be as accurate as possible. \n
	 *
	 * #define BT_AD_TX_POWER			0x0a
	 */
	uint8_t tx_power;

	/**
	 * The Appearance data type defines the external appearance of the device. \n
	 * #define BT_AD_GAP_APPEARANCE		0x19
	 */
	uint16_t Appearance;

	/**
	 * The first 2 octets contain the Company Identifier Code
	 * followed by additional manufacturer specific data \n
	 * #define BT_AD_MANUFACTURER_DATA		0xff
	 */
	uint16_t manufacturer_id;
	uint8_t manufacturer_data[25];

	/** ble is advtertised ? */
	bool ble_advertised;

	/** ble att mtu */
	uint16_t att_mtu;

	/** ble service/char */
	RkBleGattService gatt_instance[MAX_GATT_SERVICE];
	/** service cnt */
	uint8_t srv_cnt;

	/* recevice data */
	void (*cb_ble_recv_fun)(const char *uuid, char *data, int *len, RK_BLE_GATT_STATE event);
} RkBleContent;

/**
 * A2dp Media
 */
typedef enum {
	TRANSPORT_STATE_IDLE,			/* Not acquired and suspended */
	TRANSPORT_STATE_PENDING,		/* Playing but not acquired */
	TRANSPORT_STATE_REQUESTING,		/* Acquire in progress */
	TRANSPORT_STATE_ACTIVE,			/* Acquired and playing */
	TRANSPORT_STATE_SUSPENDING,		/* Release in progress */
} Rk_transport_state_t;

/**
 * Channel_Mode
 * #define SBC_CHANNEL_MODE_MONO			(1 << 3)
 * #define SBC_CHANNEL_MODE_DUAL_CHANNEL	(1 << 2)
 * #define SBC_CHANNEL_MODE_STEREO			(1 << 1)
 * #define SBC_CHANNEL_MODE_JOINT_STEREO	(1 << 0)
 *
 * Frequency
 * #define SBC_SAMPLING_FREQ_16000			(1 << 3)
 * #define SBC_SAMPLING_FREQ_32000			(1 << 2)
 * #define SBC_SAMPLING_FREQ_44100			(1 << 1)
 * #define SBC_SAMPLING_FREQ_48000			(1 << 0)
 *
 * Block_length
 * #define SBC_BLOCK_LENGTH_4				(1 << 3)
 * #define SBC_BLOCK_LENGTH_8				(1 << 2)
 * #define SBC_BLOCK_LENGTH_12				(1 << 1)
 * #define SBC_BLOCK_LENGTH_16				(1 << 0)
 * 
 * Subbands
 * #define SBC_SUBBANDS_4					(1 << 1)
 * #define SBC_SUBBANDS_8					(1 << 0)
 *
 * Allocation_Method
 * #define SBC_ALLOCATION_SNR				(1 << 1)
 * #define SBC_ALLOCATION_LOUDNESS			(1 << 0)
 * 
 * Bitpool
 * #define SBC_MIN_BITPOOL					2
 * #define SBC_MAX_BITPOOL					250
 */
typedef struct {
	uint8_t channel_mode:4;
	uint8_t frequency:4;
	uint8_t allocation_method:2;
	uint8_t subbands:2;
	uint8_t block_length:4;
	uint8_t min_bitpool;
	uint8_t max_bitpool;
} __attribute__ ((packed)) Rk_a2dp_sbc_t;

typedef struct {
	/** 
	 * Available sep
	 * '/org/bluez/hci0/dev_F0_13_C3_50_FF_26/sep1'
	 */
	char endpoint[MAX_MTPOINT_STR_LENGTH + 1];
	char remote_uuids[MAX_UUID_STR_LENGTH + 1];

	/** 
	 * CODEC
	 * #define A2DP_CODEC_SBC			0x00
	 * #define A2DP_CODEC_MPEG12 		0x01
	 * #define A2DP_CODEC_MPEG24 		0x02
	 * #define A2DP_CODEC_ATRAC			0x04
	 * #define A2DP_CODEC_VENDOR 		0xFF
	 */
	uint8_t codec;

	//Config
	Rk_a2dp_sbc_t sbc;

	RK_BT_STATE state;

	//transport volume
	uint16_t volume;

	/** /org/bluez/hci0/dev_F0_13_C3_50_FF_26/sep1/fd0 */
	//char transport[MAX_MTPOINT_STR_LENGTH + 1];
} RkBtMedia;

typedef struct {
	/** adapter name for BREDR */
	char bt_name[32];
	/** adapter address for BREDR */
	const char *bt_addr;

	/* preset pincode without ssp */
	const char *pincode;
	/* auth callback */
	void (*cb_auth_func)(uint8_t *confirm, uint32_t *code);

#define IO_CAPABILITY_DISPLAYONLY		0x00
#define IO_CAPABILITY_DISPLAYYESNO		0x01
#define IO_CAPABILITY_KEYBOARDONLY		0x02
#define IO_CAPABILITY_NOINPUTNOOUTPUT	0x03
#define IO_CAPABILITY_KEYBOARDDISPLAY	0x04
	/** io capability for adapter
	 * Note: The Car must be paired with IO_CAPABILITY_DISPLAYYESNO or IO_CAPABILITY_KEYBOARDONLY
	 */
	uint8_t io_capability;

	/** STORE DIR */
	const char *bt_dir_name;

	/** bt adapter state */
	volatile bool init;
	volatile bool power;
	volatile bool pairable;
	volatile bool discoverable;
	volatile bool scanning;

	volatile bool connecting;

	char connected_a2dp_addr[18];

	/**
	 * audio server
	 * Only one can be enabled
	 */
	bool bluealsa;
	//bool pulseaudio;

	/** profile */
#define PROFILE_A2DP_SINK_HF			(1 << 0)
#define PROFILE_A2DP_SOURCE_AG			(1 << 1)
#define PROFILE_SPP						(1 << 2)
#define PROFILE_BLE						(1 << 3)
#define PROFILE_OBEX					(1 << 4)
	uint8_t profile;

	/** ble context */
	RkBleContent ble_content;

	pthread_mutex_t bt_mutex;
	/* updates notification */
	pthread_cond_t cond;
} RkBtContent;

/**
 * @brief Indicates a remote Bluetooth device
 */
typedef struct remote_dev {
	/**
	 * Example: 00:0C:3E:3A:4B:69, \n
	 * On the UI level any number shall have the MSB -> LSB (from left to right) ‘natural’ ordering.
	 */
	char remote_address[18];

	/** "random" or "public" */
	char remote_address_type[7];

	/**
	 * The Bluetooth Device Name can be up to 248 bytes (see [Vol 2] Part C, Section 4.3.5) \n
	 * It shall be encoded according to UTF-8 \n
	 * the UI level may be restricted to as few as 62 characters
	 */
	char remote_name[MAX_NAME_LENGTH + 1];
	char remote_alias[MAX_NAME_LENGTH + 1];

	/**
	 * Class of Device \n
	 * is a parameter received during the device discovery procedure  \n
	 * on the BR/EDR physical transport, indicating the type of device. \n
	 * The terms for the defined Bluetooth Device Classes and Bluetooth Service Types are defined in [3] \n
	 * [3] Assigned Numbers Specification: https://www.bluetooth.com/specifications/assigned-numbers \n
	 */
	uint32_t cod;

	/**
	 * Generic Access Profile  \n
	 * The Appearance characteristic contains a 16-bit number \n
	 * It is a characteristic of the GAP service located on the device’s GATT Server. \n
	 * See Section 12.2(APPEARANCE CHARACTERISTIC). \n
	 * Assigned Numbers Specification: https://www.bluetooth.com/specifications/assigned-numbers \n
	 * 2.6 Appearance Values
	 */
	uint16_t appearance;

	/** class_to_icon \n
	 * gap_appearance_to_icon
	 */
	char icon[64];

	char modalias[64 + 1];

	/* disconnected info
	 *---- HCI Error Codes ----
	 * HCI_ERROR_UNKNOWN_CONN_ID		0x02	RK_DEV_DISCONN_UNKNOWN
	 * HCI_ERROR_AUTH_FAILURE			0x05	RK_DEV_DISCONN_UNKNOWN
	 * HCI_ERROR_PIN_OR_KEY_MISSING		0x06	RK_DEV_DISCONN_UNKNOWN
	 * HCI_ERROR_MEMORY_EXCEEDED		0x07	RK_DEV_DISCONN_UNKNOWN
	 * HCI_ERROR_CONNECTION_TIMEOUT		0x08	RK_DEV_DISCONN_TIMEOUT
	 * HCI_ERROR_REJ_LIMITED_RESOURCES	0x0d	RK_DEV_DISCONN_UNKNOWN
	 * HCI_ERROR_REJ_BAD_ADDR			0x0f	RK_DEV_DISCONN_UNKNOWN
	 * HCI_ERROR_INVALID_PARAMETERS		0x12	RK_DEV_DISCONN_UNKNOWN
	 * HCI_ERROR_REMOTE_USER_TERM		0x13	RK_DEV_DISCONN_REMOTE
	 * HCI_ERROR_REMOTE_LOW_RESOURCES	0x14	RK_DEV_DISCONN_REMOTE
	 * HCI_ERROR_REMOTE_POWER_OFF		0x15	RK_DEV_DISCONN_REMOTE
	 * HCI_ERROR_LOCAL_HOST_TERM		0x16	RK_DEV_DISCONN_LOCAL_HOST
	 * HCI_ERROR_PAIRING_NOT_ALLOWED	0x18	RK_DEV_DISCONN_UNKNOWN
	 * HCI_ERROR_INVALID_LL_PARAMS		0x1e	RK_DEV_DISCONN_UNKNOWN
	 * HCI_ERROR_UNSPECIFIED			0x1f	RK_DEV_DISCONN_UNKNOWN
	 * HCI_ERROR_ADVERTISING_TIMEOUT	0x3c	RK_DEV_DISCONN_UNKNOWN
	 * HCI_ERROR_CANCELLED_BY_HOST 		0x44	RK_DEV_DISCONN_UNKNOWN
	 *
	 * MGMT
	 * MGMT_STATUS_NO_RESOURCES			0x07
	 * MGMT_STATUS_SUCCESS				0x00
	 * MGMT_STATUS_UNKNOWN_COMMAND		0x01
	 * MGMT_STATUS_NOT_CONNECTED		0x02
	 * MGMT_STATUS_FAILED				0x03
	 * MGMT_STATUS_CONNECT_FAILED		0x04
	 * MGMT_STATUS_AUTH_FAILED			0x05
	 * MGMT_STATUS_NOT_PAIRED			0x06
	 * MGMT_STATUS_NO_RESOURCES			0x07
	 * MGMT_STATUS_TIMEOUT				0x08
	 * MGMT_STATUS_ALREADY_CONNECTED	0x09
	 * MGMT_STATUS_BUSY					0x0a
	 * MGMT_STATUS_REJECTED				0x0b
	 * MGMT_STATUS_NOT_SUPPORTED		0x0c
	 * MGMT_STATUS_INVALID_PARAMS		0x0d
	 * MGMT_STATUS_DISCONNECTED			0x0e
	 * MGMT_STATUS_NOT_POWERED			0x0f
	 * MGMT_STATUS_CANCELLED			0x10
	 * MGMT_STATUS_INVALID_INDEX		0x11
	 * MGMT_STATUS_RFKILLED				0x12
	 * MGMT_STATUS_ALREADY_PAIRED		0x13
	 * MGMT_STATUS_PERMISSION_DENIED	0x14
	 */
#define RK_DEV_DISCONN_UNKNOWN				0x00
#define RK_DEV_DISCONN_TIMEOUT				0x01
#define RK_DEV_DISCONN_LOCAL_HOST			0x02
#define RK_DEV_DISCONN_REMOTE				0x03
#define RK_DEV_DISCONN_AUTH_FAILURE			0x04
#define RK_DEV_DISCONN_LOCAL_HOST_SUSPEND	0x05
	uint8_t discon_reason;
	uint8_t discon_addr_type;

	/** The ATT MTU */
	uint16_t att_mtu;
	//for notify/indicate
	uint32_t gatt_notify_fd;

	/* AdvertisingFlags */
	uint8_t flags;

	int16_t rssi;
	int8_t tx_power;

	//EIR UUID
	char remote_uuids[MAX_UUID_INDEX][MAX_UUID_STR_LENGTH + 1];

	RkBtMedia media;

	volatile bool exist;

	//change event/reason
	//Failed to connect: org.bluez.Error.Failed|br-connection-page-timeout
	char change_name[128];

	//fail reason
	//char fail_reason[64];

	//remote dev path: /org/bluez/hci0/dev_F8_7D_76_F2_12_F3
	char dev_path[37 + 1];

 	volatile bool connected;	//deprecated
	volatile bool paired;		//deprecated
	volatile bool bonded;		//deprecated
	volatile bool svc_refreshed;

	//bredr (A2DP/HFP/SPP/PBAP)
	volatile bool br_connected;
	volatile bool br_paired;
	volatile bool br_bonded;

	//ble (GATT/ATT/ANCS)
	volatile bool ble_connected;
	volatile bool ble_paired;
	volatile bool ble_bonded;

	volatile bool blocked;
	volatile bool auto_connect;

	volatile bool disable_auto_connect;
	volatile bool general_connect;

	volatile bool duplicate;

	/*
	 * bredr_state.paired			(1 << 0)
	 * bredr_state.connected		(1 << 1)
	 * bredr_state.bonded			(1 << 2)
	 * bredr_state.svc_resolved 	(1 << 3)
	 * ble_state.paired				(1 << 8)
	 * ble_state.connected			(1 << 9)
	 * ble_state.bonded				(1 << 10)
	 * ble_state.svc_resolved		(1 << 11)
	 */
	volatile uint16_t info;

	//avrcp
	RK_BT_STATE player_state;
	unsigned int player_position;
	unsigned int player_total_len;
	char title[MAX_NAME_LENGTH + 1];
	char artist[MAX_NAME_LENGTH + 1];

	/** len == "/org/bluez/hci0/dev_70_5D_1F_65_EE_E0" + 1 \n
	 * device path: "/org/bluez/hci0/dev_70_5D_1F_65_EE_E0"
	 */
	char obj_path[38];

	void *data;
} RkBtRemoteDev;

struct adapter_connect_device {
	char addr[18];
	char addr_type[7]; //public random
};

typedef bool (*RK_BT_VENDOR_CALLBACK)(bool enable);
typedef bool (*RK_BT_AUDIO_SERVER_CALLBACK)(bool enable);

typedef void (*RK_BT_STATE_CALLBACK)(RkBtRemoteDev *rdev, RK_BT_STATE state);
typedef void (*RK_BLE_GATT_CALLBACK)(const char *bd_addr, unsigned int mtu);

typedef void (*RK_BT_RFCOMM_AT_CALLBACK)(char *at_evt);

void rk_bt_register_vendor_callback(RK_BT_VENDOR_CALLBACK cb);
void rk_bt_register_audio_server_callback(RK_BT_AUDIO_SERVER_CALLBACK cb);

void rk_bt_set_profile(uint8_t profile, bool enable);


/**
 * @ingroup  rk_bt_basic
 * @brief  RK_BT_STATE_CALLBACK
 *
 * @par Description
        RK_BT_STATE_CALLBACK
 *
 * @attention  !!!Never write or call delayed or blocked code or functions inside this function.
 * @attention  !!!Never write or call delayed or blocked code or functions inside this function.
 * @attention  !!!Never write or call delayed or blocked code or functions inside this function.
 
 * @param cb void (*RK_BT_STATE_CALLBACK)(RkBtRemoteDev *rdev, RK_BT_STATE state);
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
void rk_bt_register_state_callback(RK_BT_STATE_CALLBACK cb);

/**
 * @ingroup  rk_bt_basic
 * @brief  rk_bt_version
 *
 * @par Description
        rk_bt_version
 *
 * @attention
 * @param NULL
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
char *rk_bt_version(void);

/**
 * @ingroup  rk_bt_basic
 * @brief  rk_bt_set_discoverable
 *
 * @par Description
        rk_bt_set_discoverable
 *
 * @attention
 * @param  enable 
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
void rk_bt_set_discoverable(bool enable);

/**
 * @ingroup  rk_bt_basic
 * @brief  rk_bt_set_local_name
 *
 * @par Description
        rk_bt_set_local_name
 *
 * @attention
 * @param  name
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
void rk_bt_set_local_name(char *name);
/**
 * @ingroup  rk_bt_basic
 * @brief  rk_bt_is_powered_on
 *
 * @par Description
        rk_bt_is_powered_on
 *
 * @attention
 * @param  enable
 *
 * @retval true Powered.
 * @retval false Not powered.
 */
bool rk_bt_is_powered_on(void);
/**
 * @ingroup  rk_bt_basic
 * @brief  rk_bt_set_power
 *
 * @par Description
        rk_bt_set_power
 *
 * @attention
 * @param  enable
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
void rk_bt_set_power(bool enable);

/**
 * @ingroup  rk_bt_basic
 * @brief  rk_bt_set_pairable
 *
 * @par Description
        rk_bt_set_pairable
 *
 * @attention
 * @param  enable 
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */

void rk_bt_set_pairable(bool enable);

/**
 * @ingroup  rk_bt_basic
 * @brief  bt avrcp cmd for sink
 *
 * @par Description
        bt avrcp cmd for sink
 *
 * @attention  Wait for RK_BT_STATE_SINK_* from the RK_BT_STATE_CALLBACK callback
 * @param  cmd "play" "pause" "next" "previous"
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int rk_bt_sink_media_control(char *cmd);


/**
 * @ingroup  rk_bt_basic
 * @brief  get all remote dev
 *
 * @par Description
        bt get all remote dev
 *
 * @attention
 * @param  scan_list [OUT]  device list
 * @param  len [OUT]  device nums
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int bt_get_devices(struct remote_dev **scan_list, int *len);

/**
 * @ingroup  rk_bt_basic
 * @brief  bt_get_dev_info
 *
 * @par Description
        get info abot the specified remote dev
 *
 * @attention
 * @param  pdev [OUT] Information is stored here
 * @param  t_addr [IN] Address of the remote device
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int bt_get_dev_info(struct remote_dev *pdev, char *t_addr);

/**
 * @ingroup  rk_bt_basic
 * @brief  bt connect remote dev
 *
 * @par Description
        bt connect remote dev
 *
 * @attention  Wait for RK_BT_STATE_CONNECT* from the RK_BT_STATE_CALLBACK callback
 * @param  address remote dev addr
 * @param  addr_type remote dev type
 * "bredr":  remote_address_type == ""
 * "public": remote_address_type == "public"
 * "random": remote_address_type == "random"
 * "auto"
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int rk_bt_connect_by_addr(char *addr, char *addr_type);
int rk_bt_connect_spp_by_addr(char *addr);


/**
 * @ingroup  rk_bt_basic
 * @brief  bt disconnect remote dev
 *
 * @par Description
        bt disconnect remote dev
 *
 * @attention  Wait for RK_BT_STATE_DISCONN* from the RK_BT_STATE_CALLBACK callback
 * @param  addr [IN] remote dev addr
 * @param  addr_type [IN] remote dev type
 * "bredr":  remote_address_type == ""
 * "public": remote_address_type == "public"
 * "random": remote_address_type == "random"
 * "auto"
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int rk_bt_disconnect_by_addr(char *addr, char *addr_type);

//TODO void rk_bt_adapter_info(char *data);

/**
 * @ingroup  rk_bt_basic
 * @brief  bt initialize
 *
 * @par Description
        bt adapter init
 *
 * @attention  Must wait for RK_BT_STATE_INIT_ON of the RK_BT_STATE_CALLBACK callback
 * @param  p_bt_content [IN]  Type  [RkBtContent *]
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int rk_bt_init(RkBtContent *p_bt_content);

/**
 * @ingroup  rk_bt_basic
 * @brief  bt de-initialize
 *
 * @par Description
        bt adapter init
 *
 * @attention  Wait for RK_BT_STATE_INIT_OFF from the RK_BT_STATE_CALLBACK callback
 * @param  NULL
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int rk_bt_deinit(void);

/**
 * @ingroup  rk_bt_basic
 * @brief  bt start discovery
 *
 * @par Description
 *        Start the Bluetooth device discovery process.
 *
 * @attention  Wait for RK_BT_STATE_SCAN_STARTED from the RK_BT_STATE_CALLBACK callback
 * @param  scan_type [IN]  Type  [RK_BT_SCAN_TYPE]
 *        The type of scan to perform (e.g., BLE, BR/EDR, etc.)
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int rk_bt_start_discovery(RK_BT_SCAN_TYPE scan_type);

/**
 * @ingroup  rk_bt_basic
 * @brief  bt scan stop
 *
 * @par Description
        bt scan stop
 *
 * @attention  Wait for RK_BT_STATE_SCAN_STOPPED from the RK_BT_STATE_CALLBACK callback
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int rk_bt_cancel_discovery(void);

/**
 * @ingroup  rk_bt_basic
 * @brief  bt pair remote ble dev
 *
 * @par Description
        bt pair remote ble dev
 *
 * @attention  Wait for RK_BT_STATE_CONNECT* from the RK_BT_STATE_CALLBACK callback
 * @param  addr remote dev addr
 *
 * @retval 0 Excute successfully, see attention.
 * @retval -1 Error code
 * @see  NULL
 */
int rk_bt_pair_by_addr(char *addr);

/**
 * @ingroup  rk_bt_basic
 * @brief  Unpair a remote Bluetooth LE device.
 *
 * @par Description
 *        This function unpairs a remote Bluetooth LE device. It removes the bond
 *        between the local device and the specified remote device.
 *
 * @attention
 *        - The function waits for RK_BT_STATE_DISCONN* from the RK_BT_STATE_CALLBACK callback.
 *        - The `addr` parameter should be the address of the remote device.
 *        - The `addr_type` parameter should be the type of the remote device's address,
 *          which can be "bredr", "public", "random", or "auto".
 *
 * @param addr [IN] The address of the remote Bluetooth LE device to unpair.
 * @param addr_type [IN] The type of the remote device's address ("bredr", "public", "random", or "auto").
 *
 * @retval 0 If the unpair operation is successful.
 * @retval -1 If an error occurs during the unpair operation.
 * @see RK_BT_STATE_CALLBACK
 */
int rk_bt_unpair_by_addr(char *addr, char *addr_type);

//TODO
void rk_bt_adapter_info(char *data);

/**
 * @brief Check if the Bluetooth is open.
 *
 * @return true if Bluetooth is open, false otherwise.
 *
 * @details This function checks the current state of the Bluetooth adapter
 *          to determine if it is open and ready for operations.
 */
bool rk_bt_is_open(void);

/**
 * @brief      Initiate an OBEX GET operation for PBAP
 *
 * @param[in]  dst    The destination device address
 * @param[in]  dir    The directory to get from
 * @param[in]  file   The file to get
 *
 * #define PBAP_UUID "0000112f-0000-1000-8000-00805f9b34fb"
 * 
 * dir:
 * pb: address book 通讯录
 * ich/och: incoming/outgoing call records 来电/去电通讯录
 * mch: missed calls 未接通话
 * cch: all communication records 所有通信记录
 * fav: favorite persons 最喜欢的人
 * 
 * @return     Zero on success, -1 on failure
 *
 * @details    This function initiates an OBEX GET operation for PBAP. It
 *             connects to the OBEX server on the specified destination device
 *             and initiates a SELECT operation to the specified directory. If
 *             the SELECT operation is successful, it initiates a GET operation
 *             to the specified file.
 */
int rk_bt_pbap_get_vcf(const char *dst, const char *object, const char *file);

/**
 * Send a file to a Bluetooth device.
 *
 * @param dst The destination device's address.
 * @param file The file to send.
 *
 * @return 0 on success, -1 on failure.
 * 
 * @details 
 * #define OPP_UUID "00001105-0000-1000-8000-00805f9b34fb"
 * Only for Android
 */
int rk_bt_opp_send(const char *dst, const char *file);

/**
 * @brief Open an RFCOMM channel to a Bluetooth device
 *
 * @param addr The address of the Bluetooth device to connect to
 * @param callback Callback function to receive AT events
 *
 * @return Zero on success, -1 on failure
 *
 * Opens an RFCOMM channel to the specified Bluetooth device and registers
 * the provided callback function to receive AT events.
 */
int rk_bt_rfcomm_open(const char *addr, RK_BT_RFCOMM_AT_CALLBACK callback);

/**
 * @brief Close the RFCOMM channel
 *
 * @return True if the channel was closed successfully, false otherwise
 *
 * Closes the RFCOMM channel if it is open.
 */
bool rk_bt_rfcomm_close(void);

/**
 * @brief Send a line of AT commands over the RFCOMM channel
 *
 * @param line The line of AT commands to send
 *
 * Sends the provided line of AT commands over the RFCOMM channel.
 */
void rk_bt_rfcomm_send(char *line);

/**
 * @brief Connect to a Bluetooth device
 *
 * This function initiates a connection to a Bluetooth device. It takes the
 * device's address and the type of the device's address as input parameters.
 *
 * @param addr The address of the Bluetooth device to connect to.
 * @param ble_addrtype The type of the ble device's address. This can be "public"
 *                    for a public address, "random" for a random address, or
 *                    NULL for an address of bredr device.
 * @return An integer value. 0 if the connection was successfully initiated.
 */
int rk_adapter_connect(const char *addr, const char *ble_addrtype);

void rk_debug_init(bool value);

#ifdef __cplusplus
}
#endif

#endif /* __BT_BASE_H__ */
