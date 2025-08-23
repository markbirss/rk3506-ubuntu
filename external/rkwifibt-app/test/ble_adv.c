#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

/* Extended Inquiry Response field types */
#define EIR_FLAGS		    0x01 /* flags */
#define EIR_UUID16_SOME		0x02 /* 16-bit UUID, more available */
#define EIR_UUID16_ALL		0x03 /* 16-bit UUID, all listed */
#define EIR_UUID32_SOME		0x04 /* 32-bit UUID, more available */
#define EIR_UUID32_ALL		0x05 /* 32-bit UUID, all listed */
#define EIR_UUID128_SOME	0x06 /* 128-bit UUID, more available */
#define EIR_UUID128_ALL		0x07 /* 128-bit UUID, all listed */
#define EIR_NAME_SHORT		0x08 /* shortened local name */
#define EIR_NAME_COMPLETE	0x09 /* complete local name */
#define EIR_TX_POWER		0x0A /* transmit power level */
#define EIR_CLASS_OF_DEV	0x0D /* Class of Device */
#define EIR_SSP_HASH_C192	0x0E /* Simple Pairing Hash C-192 */
#define EIR_SSP_RAND_R192	0x0F /* Simple Pairing Randomizer R-192 */
#define EIR_DEVICE_ID		0x10 /* device ID */
#define EIR_APPEARANCE		0x19 /* Device appearance */
#define EIR_SERVICE_DATA	0x16 /* Service Data */
#define EIR_LE_BDADDR		0x1B /* LE Bluetooth device address */
#define EIR_LE_ROLE		    0x1C /* LE role */
#define EIR_SSP_HASH_C256	0x1D /* Simple Pairing Hash C-256 */
#define EIR_SSP_RAND_R256	0x1E /* Simple Pairing Rand R-256 */
#define EIR_LE_SC_CONFIRM	0x22 /* LE SC Confirmation Value */
#define EIR_LE_SC_RANDOM	0x23 /* LE SC Random Value */

/* Low Energy Advertising Flags */
#define LE_AD_LIMITED		    0x01 /* Limited Discoverable */
#define LE_AD_GENERAL		    0x02 /* General Discoverable */
#define LE_AD_NO_BREDR		    0x04 /* BR/EDR not supported */
#define LE_AD_SIM_LE_BREDR_CTRL	0x08 /* Simultaneous LE & BR/EDR Controller */
#define LE_AD_SIM_LE_BREDR_HOST	0x10 /* Simultaneous LE & BR/EDR Host */

//#define HCI_MAX_AD_LENGTH		31

//#define HCI_OP_LE_SET_ADV_DATA		0x2008
struct hci_le_set_adv_data {
	__u8	length;
	__u8	data[HCI_MAX_AD_LENGTH];
} __packed;

//#define HCI_OP_LE_SET_SCAN_RSP_DATA	0x2009
struct hci_le_set_scan_rsp_data {
	__u8	length;
	__u8	data[HCI_MAX_AD_LENGTH];
} __packed;

/*
 * #define HCI_OP_LE_SET_ADV_PARAM		0x2006
 * min_interval: 0x00A0   Minimum advertising interval for undirected and low duty cycle directed advertising. 
 *                        Range: 0x0020 to 0x4000 default: 0x0800 1.28s Time = N * 0.625 msec Time Range: 20 ms to 10.24s
 * max_interval: 0x00A0
 * type: 0x00
 *  0x00 Connectable and scannable undirected advertising (ADV_IND) (default) 
 *  0x01 Connectable high duty cycle directed advertising (ADV_DIRECT_IND, high duty cycle)
 *  0x02 Scannable undirected advertising (ADV_SCAN_IND)
 *  0x03 Non connectable undirected advertising (ADV_NONCONN_IND)
 *  0x04 Connectable low duty cycle directed advertising (ADV_DIRECT_IND, low duty cycle)
 * 
 * own_address_type: 0x01   
 *  0x00 Public Device Address (default) 
 *  0x01 Random Device Address
 *  0x02 Controller generates Resolvable Private Address based on the local IRK from the resolving list. If the resolving list contains no matching entry, use the public address.
 *  0x03 Controller generates Resolvable Private Address based on the local IRK from the resolving list. If the resolving list contains no matching entry, use the random address from LE_Set_Random_Address
 * 
 * direct_addr_type: 0x00
 *  0x00 Public Device Address (default) or Public Identity Address 
 *  0x01 Random Device Address or Random (static) Identity Address
 * 
 * direct_addr(6byte): 0x000000000000
 * 
 * channel_map: 0x07
 *  0 Channel 37 shall be used
 *  1 Channel 38 shall be used
 *  2 Channel 39 shall be used
 * 
 * filter_policy: 0x00
 *  0x00 Process scan and connection requests from all devices (i.e., the White List is not in use) (default). 
 *  0x01 Process connection requests from all devices and scan requests only from devices that are in the White List. 
 *  0x02 Process scan requests from all devices and connection requests only from devices that are in the White List. 
 *  0x03 Process scan and connection requests only from devices in the White List.
 */
struct hci_le_set_adv_param {
	__le16   min_interval;
	__le16   max_interval;
	__u8     type;
	__u8     own_address_type;
	__u8     direct_addr_type;
	bdaddr_t direct_addr;
	__u8     channel_map;
	__u8     filter_policy;
} __packed;

//#define HCI_OP_LE_SET_EXT_ADV_DATA		0x2037
/*
 * Advertising_Handle: 	0x01
 * Options:				0x03
 * 	Value Parameter Description
 * 	0x00 Intermediate fragment of fragmented extended advertising data
 * 	0x01 First fragment of fragmented extended advertising data
 * 	0x02 Last fragment of fragmented extended advertising data
 * 	0x03 Complete extended advertising data
 * 	0x04 Unchanged data (just update the Advertising DID)
 *
 * Fragment_Preference:	0x01
 * 	Value Parameter Description
 * 	0x00 The Controller may fragment all Host advertising data
 * 	0x01 The Controller should not fragment or should minimize fragmentation of Host advertising data
 */
struct hci_le_set_ext_adv_data {
	__u8  handle;
	__u8  operation;
	__u8  frag_pref;
	__u8  length;
	__u8  data[];
} __packed;

//#define HCI_OP_LE_SET_EXT_SCAN_RSP_DATA		0x2038
struct hci_le_set_ext_scan_rsp_data {
	__u8  handle;
	__u8  operation;
	__u8  frag_pref;
	__u8  length;
	__u8  data[];
} __packed;

//#define HCI_OP_LE_SET_EXT_ADV_ENABLE		0x2039
struct hci_le_set_ext_adv_enable {
	__u8  enable;
	__u8  num_of_sets;
	__u8  data[];
} __packed;

//#define HCI_OP_LE_SET_ADV_SET_RAND_ADDR	0x2035
struct hci_le_set_adv_set_rand_addr {
	__u8  handle;
	bdaddr_t  bdaddr;
} __packed;

/*
 * Advertising_Handle: 				    0x01
 * Advertising_Event_Properties: 		0x0013     		//00010011
 *  bit0: Connectable advertising
 *  bit1: Scannable advertising
 *  bit2: Directed advertising
 *  bit3: High Duty Cycle Directed Connectable advertising (≤ 3.75 ms Advertising Interval)
 *  bit4: Use Legacy advertising PDUs
 *  bit5: Omit advertiser's address from all PDUs ("anonymous advertising")
 *  bit6: Include TxPower in the extended header of at least one advertising PDU
 * 
 * Primary_Advertising_Interval_Min: 	0x0000AO		//Range: 0x000020 to 0xFFFFFF Time = N * 0.625 ms Time Range: 20 ms to 10,485.759375 s
 * Primary_Advertising_Interval_Max:	0x0000A0		//Range: 0x000020 to 0xFFFFFF Time = N * 0.625 ms Time Range: 20 ms to 10,485.759375 s
 * Primary_Advertising_Channel_Map:	    0x07			//bit0: CHAN_37 bit1: CHAN_38 bit2: CHAN_39
 * Own_Address_Type:					0x01			//0x00: Public Device Address, 0x01: Random Device Address, 0x02/0x03：Controller generated ...
 * Peer_Address_Type:					0x00			//0x00 Public Device Address or Public Identity Address, 0x01: Random Device Address or Random (static) Identity Address
 * Peer_Address:						0x00		    //6byte
 * Advertising_Filter_Policy:			0x00			//0x00: Process scan and connection requests from all devices (i.e., the White List is not in use)
 * Advertising_TX_Power:				0x7F			//Range: -127 to +20, 0x7F: Host has no preference
 * Primary_Advertising_PHY:			    0x01			//0x01: 1M, 0x03: Le Coded
 * Secondary_Advertising_Max_Skip:		0x00			//AUX_ADV_IND shall be sent prior to the next advertising event
 * Secondary_Advertising_PHY:			0x01			//0x01: 1M, 0x02: 2M, 0x03: Le Coded
 * Advertising_SID:					    0x00			//0x00 to 0x0F Value of the Advertising SID subfield in the ADI field of the PDU
 * Scan_Request_Notification_Enable:	0x00			//0x00: Scan Request Notification is disabled, 0x01: Scan Request Notification is enabled
 */
struct hci_le_set_ext_adv_params {
	__u8      handle;
	__le16    evt_properties;
	__u8      min_interval[3];
	__u8      max_interval[3];
	__u8      channel_map;
	__u8      own_addr_type;
	__u8      peer_addr_type;
	bdaddr_t  peer_addr;
	__u8      filter_policy;
	__u8      tx_power;
	__u8      primary_phy;
	__u8      secondary_max_skip;
	__u8      secondary_phy;
	__u8      sid;
	__u8      notif_enable;
} __packed;

//hcitool simple version
static void rk_hex_dump(char *pref, int width, unsigned char *buf, int len)
{
	register int i,n;

	for (i = 0, n = 1; i < len; i++, n++) {
		if (n == 1)
			printf("%s", pref);
		printf("%2.2X ", buf[i]);
		if (n == width) {
			printf("\n");
			n = 0;
		}
	}
	if (i && n!=1)
		printf("\n");
}

static void rk_hcitool_cmd(uint8_t ogf, uint16_t ocf, char *buf, int len)
{
	struct hci_filter flt;
	hci_event_hdr *hdr;
	int i, opt, dd;

	errno = 0;

	if (errno == ERANGE || (ogf > 0x3f) || (ocf > 0x3ff)) {
		printf("invalid command\n");
		return;
	}

	dd = hci_open_dev(0);
	if (dd < 0) {
		perror("Device open failed");
		exit(EXIT_FAILURE);
	}

	/* Setup filter */
	hci_filter_clear(&flt);
	hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
	hci_filter_all_events(&flt);
	if (setsockopt(dd, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
		perror("HCI filter setup failed");
		exit(EXIT_FAILURE);
	}

	printf("< HCI Command: ogf 0x%02x, ocf 0x%04x, plen %d\n", ogf, ocf, len);
	rk_hex_dump("  ", 20, buf, len); fflush(stdout);

	if (hci_send_cmd(dd, ogf, ocf, len, buf) < 0) {
		perror("Send failed");
		exit(EXIT_FAILURE);
	}

	len = read(dd, buf, sizeof(buf));
	if (len < 0) {
		perror("Read failed");
		exit(EXIT_FAILURE);
	}

	hdr = (void *)(buf + 1);
	ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
	len -= (1 + HCI_EVENT_HDR_SIZE);

	printf("> HCI Event: 0x%02x plen %d\n", hdr->evt, hdr->plen);
	rk_hex_dump("  ", 20, ptr, len); fflush(stdout);

	hci_close_dev(dd);
}

/*
reference: Assigned_Numbers.pdf

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
Advertising_Data_Length: 0 to 251 The number of octets in the Advertising Data parameter
Advertising_Data: Size: Advertising_Data_Length octets
hcitool -i hci0 cmd 0x08 0x0037 01 03 01 0x8 09 54 65 73 74 20 4C 46

#Command Code    LE Set Extended Scan Response Data command
hcitool -i hci0 cmd 0x08 0x0038

#Command Code    LE Set Extended Advertising Enable Command
hcitool -i hci0 cmd 0x08 0x0039  01 01 01 00 00 00
*/



/*
 * 		BT 4.X
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
 */
static int ble_direct_set_adv_param(char *ble_name , char *ble_mac_addr)
{
	char cmd_para[128];
	char ret_buff[128];
	char temp[32];

    struct hci_le_set_adv_param param;

    param.min_interval = 0x00A0;
    param.max_interval = 0x00A0;
    param.type = 0x00;
    param.own_address_type = 0x01;
    param.peer_address_type = 0x00;
    memcpy(param.direct_addr, 0x00, 6);
    param.channel_map = 0x07;
    param.filter_policy = 0x00;

	//using ble adv
	ble_direct_flag = true;

    //LE SET PARAMETERS 0x0006
	rk_hcitool_cmd(0x8, 0x0006, &param, sizeof(param));

	//LE Set Random Address Command 0x0005
    rk_hcitool_cmd(0x8, 0x0005, ble_mac_addr, 6);

	//LE Set Advertising Data Command
    rk_hcitool_cmd(0x8, 0x0008, ble_name, strlen(ble_name));

	char CMD_ADV_RESP_DATA[128] = "hcitool -i hci0 cmd 0x08 0x0009";
	//char CMD_ADV_RESP_DATA[128] = "hcitool -i hci0 cmd 0x08 0x0037 01 03 01";

	memset(cmd_para, 0, 128);
	cmd_para[0] = strlen(ble_name) + 1 + 1;
	cmd_para[1] = strlen(ble_name) + 1;
	cmd_para[2] = 0x09;
	sprintf(cmd_para + 3, "%s", ble_name);
	//printf("cmd_para ret: %s\n", cmd_para);

	memset(temp, 0, 32);
	for (int i = 0; i < strlen(cmd_para); i++) {
		sprintf(temp, "%02x", cmd_para[i]);
		strcat(CMD_ADV_RESP_DATA, " ");
		strcat(CMD_ADV_RESP_DATA, temp);
	}
	printf("CMD_ADV_RESP_DATA: %s\n", CMD_ADV_RESP_DATA);
	exec_command(CMD_ADV_RESP_DATA, ret_buff, 128);
	printf("CMD_ADV_RESP_DATA ret: %s\n", ret_buff);

	return 1;
}

static int ble_direct_enable_adv(void)
{
	int ret_buff[32];

	//enable
	exec_command("hcitool -i hci0 cmd 0x08 0x000a 1", ret_buff, 128);
	printf("enable adv ret: %s\n", ret_buff);

	return 1;
}

static int ble_direct_close_adv(void)
{
	int ret_buff[32];
	//disable
	exec_command("hcitool -i hci0 cmd 0x08 0x000a 0", ret_buff, 128);
	printf("disable adv ret: %s\n", ret_buff);

	return 1;
}