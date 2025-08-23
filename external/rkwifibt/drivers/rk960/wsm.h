/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef RK960_WSM_H_INCLUDED
#define RK960_WSM_H_INCLUDED

#include <linux/spinlock.h>

#define RK960_CHANNEL_CONFLICT_OPT

struct rk960_common;

/* Bands */
/* Radio band 2.412 -2.484 GHz. */
#define WSM_PHY_BAND_2_4G		(0)

/* Radio band 4.9375-5.8250 GHz. */
#define WSM_PHY_BAND_5G			(1)

/* Transmit rates */
/* 1   Mbps            ERP-DSSS */
#define WSM_TRANSMIT_RATE_1		(0)

/* 2   Mbps            ERP-DSSS */
#define WSM_TRANSMIT_RATE_2		(1)

/* 5.5 Mbps            ERP-CCK, ERP-PBCC (Not supported) */
/* #define WSM_TRANSMIT_RATE_5		(2) */

/* 11  Mbps            ERP-CCK, ERP-PBCC (Not supported) */
/* #define WSM_TRANSMIT_RATE_11		(3) */

/* 22  Mbps            ERP-PBCC (Not supported) */
/* #define WSM_TRANSMIT_RATE_22		(4) */

/* 33  Mbps            ERP-PBCC (Not supported) */
/* #define WSM_TRANSMIT_RATE_33		(5) */

/* 6   Mbps   (3 Mbps) ERP-OFDM, BPSK coding rate 1/2 */
#define WSM_TRANSMIT_RATE_6		(6)

/* 9   Mbps (4.5 Mbps) ERP-OFDM, BPSK coding rate 3/4 */
#define WSM_TRANSMIT_RATE_9		(7)

/* 12  Mbps  (6 Mbps)  ERP-OFDM, QPSK coding rate 1/2 */
#define WSM_TRANSMIT_RATE_12		(8)

/* 18  Mbps  (9 Mbps)  ERP-OFDM, QPSK coding rate 3/4 */
#define WSM_TRANSMIT_RATE_18		(9)

/* 24  Mbps (12 Mbps)  ERP-OFDM, 16QAM coding rate 1/2 */
#define WSM_TRANSMIT_RATE_24		(10)

/* 36  Mbps (18 Mbps)  ERP-OFDM, 16QAM coding rate 3/4 */
#define WSM_TRANSMIT_RATE_36		(11)

/* 48  Mbps (24 Mbps)  ERP-OFDM, 64QAM coding rate 1/2 */
#define WSM_TRANSMIT_RATE_48		(12)

/* 54  Mbps (27 Mbps)  ERP-OFDM, 64QAM coding rate 3/4 */
#define WSM_TRANSMIT_RATE_54		(13)

/* 6.5 Mbps            HT-OFDM, BPSK coding rate 1/2 */
#define WSM_TRANSMIT_RATE_HT_6		(14)

/* 13  Mbps            HT-OFDM, QPSK coding rate 1/2 */
#define WSM_TRANSMIT_RATE_HT_13		(15)

/* 19.5 Mbps           HT-OFDM, QPSK coding rate 3/4 */
#define WSM_TRANSMIT_RATE_HT_19		(16)

/* 26  Mbps            HT-OFDM, 16QAM coding rate 1/2 */
#define WSM_TRANSMIT_RATE_HT_26		(17)

/* 39  Mbps            HT-OFDM, 16QAM coding rate 3/4 */
#define WSM_TRANSMIT_RATE_HT_39		(18)

/* 52  Mbps            HT-OFDM, 64QAM coding rate 2/3 */
#define WSM_TRANSMIT_RATE_HT_52		(19)

/* 58.5 Mbps           HT-OFDM, 64QAM coding rate 3/4 */
#define WSM_TRANSMIT_RATE_HT_58		(20)

/* 65  Mbps            HT-OFDM, 64QAM coding rate 5/6 */
#define WSM_TRANSMIT_RATE_HT_65		(21)

/* Scan types */
/* Foreground scan */
#define WSM_SCAN_TYPE_FOREGROUND	(0)

/* Background scan */
#define WSM_SCAN_TYPE_BACKGROUND	(1)

/* Auto scan */
#define WSM_SCAN_TYPE_AUTO		(2)

/* Scan flags */
/* Forced background scan means if the station cannot */
/* enter the power-save mode, it shall force to perform a */
/* background scan. Only valid when ScanType is */
/* background scan. */
#define WSM_SCAN_FLAG_FORCE_BACKGROUND	(BIT(0))

/* The WLAN device scans one channel at a time so */
/* that disturbance to the data traffic is minimized. */
#define WSM_SCAN_FLAG_SPLIT_METHOD	(BIT(1))

/* Preamble Type. Long if not set. */
#define WSM_SCAN_FLAG_SHORT_PREAMBLE	(BIT(2))

/* 11n Tx Mode. Mixed if not set. */
#define WSM_SCAN_FLAG_11N_GREENFIELD	(BIT(3))

#define WSM_FLAG_MAC_INSTANCE_1	(BIT(4))

#define WSM_FLAG_MAC_INSTANCE_0	(~(BIT(4)))

/* Scan constraints */
/* Maximum number of channels to be scanned. */
#define WSM_SCAN_MAX_NUM_OF_CHANNELS	(48)

/* The maximum number of SSIDs that the device can scan for. */
#define WSM_SCAN_MAX_NUM_OF_SSIDS	(2)
#define WSM_SCAN_MAX_MATCH_SETS_NUM_OF_SSIDS	(16)
#ifdef CONFIG_RK960_TESTMODE
/* Transmit flags */
/* Start Expiry time from the receipt of tx request */
#define WSM_TX_FLAG_EXPIRY_TIME		(BIT(0))
#endif /*CONFIG_RK960_TESTMODE */

/* Power management modes */
/* 802.11 Active mode */
#define WSM_PSM_ACTIVE			(0)

/* 802.11 PS mode */
#define WSM_PSM_PS			BIT(0)

/* Fast Power Save bit */
#define WSM_PSM_FAST_PS_FLAG		BIT(7)

/* Dynamic aka Fast power save */
#define WSM_PSM_FAST_PS			(BIT(0) | BIT(7))

/* Undetermined */
/* Note : Undetermined status is reported when the */
/* NULL data frame used to advertise the PM mode to */
/* the AP at Pre or Post Background Scan is not Acknowledged */
#define WSM_PSM_UNKNOWN			BIT(1)

/* Queue IDs */
/* best effort/legacy */
#define WSM_QUEUE_BEST_EFFORT		(0)

/* background */
#define WSM_QUEUE_BACKGROUND		(1)

/* video */
#define WSM_QUEUE_VIDEO			(2)

/* voice */
#define WSM_QUEUE_VOICE			(3)

/* HT TX parameters */
/* Non-HT */
#define WSM_HT_TX_NON_HT		(0)

/* Mixed format */
#define WSM_HT_TX_MIXED			(1)

/* Greenfield format */
#define WSM_HT_TX_GREENFIELD		(2)

#define WSM_HT_TX_SGI				(4)

/* STBC allowed */
#define WSM_HT_TX_STBC			(BIT(7))

/* EPTA prioirty flags for BT Coex */
/* default epta priority */
#define WSM_EPTA_PRIORITY_DEFAULT	4
/* use for normal data */
#define WSM_EPTA_PRIORITY_DATA		4
/* use for connect/disconnect/roaming*/
#define WSM_EPTA_PRIORITY_MGT		5
/* use for action frames */
#define WSM_EPTA_PRIORITY_ACTION	5
/* use for AC_VI data */
#define WSM_EPTA_PRIORITY_VIDEO		5
/* use for AC_VO data */
#define WSM_EPTA_PRIORITY_VOICE		6
/* use for EAPOL exchange */
#define WSM_EPTA_PRIORITY_EAPOL		7

/* TX status */
/* Frame was sent aggregated */
/* Only valid for WSM_SUCCESS status. */
#define WSM_TX_STATUS_AGGREGATION	(BIT(0))

/* Host should requeue this frame later. */
/* Valid only when status is WSM_REQUEUE. */
#define WSM_TX_STATUS_REQUEUE		(BIT(1))

/* Normal Ack */
#define WSM_TX_STATUS_NORMAL_ACK	(0<<2)

/* No Ack */
#define WSM_TX_STATUS_NO_ACK		(1<<2)

/* No explicit acknowledgement */
#define WSM_TX_STATUS_NO_EXPLICIT_ACK	(2<<2)

/* Block Ack */
/* Only valid for WSM_SUCCESS status. */
#define WSM_TX_STATUS_BLOCK_ACK		(3<<2)

/* RX status */
/* Unencrypted */
#define WSM_RX_STATUS_UNENCRYPTED	(0<<0)

/* WEP */
#define WSM_RX_STATUS_WEP		(1<<0)

/* TKIP */
#define WSM_RX_STATUS_TKIP		(2<<0)

/* AES */
#define WSM_RX_STATUS_AES		(3<<0)

/* WAPI */
#define WSM_RX_STATUS_WAPI		(4<<0)

/* Macro to fetch encryption subfield. */
#define WSM_RX_STATUS_ENCRYPTION(status) ((status) & 0x07)

/* Frame was part of an aggregation */
#define WSM_RX_STATUS_AGGREGATE		(BIT(3))

/* Frame was first in the aggregation */
#define WSM_RX_STATUS_AGGREGATE_FIRST	(BIT(4))

/* Frame was last in the aggregation */
#define WSM_RX_STATUS_AGGREGATE_LAST	(BIT(5))

/* Indicates a defragmented frame */
#define WSM_RX_STATUS_DEFRAGMENTED	(BIT(6))

/* Indicates a Beacon frame */
#define WSM_RX_STATUS_BEACON		(BIT(7))

/* Indicates STA bit beacon TIM field */
#define WSM_RX_STATUS_TIM		(BIT(8))

/* Indicates Beacon frame's virtual bitmap contains multicast bit */
#define WSM_RX_STATUS_MULTICAST		(BIT(9))

/* Indicates frame contains a matching SSID */
#define WSM_RX_STATUS_MATCHING_SSID	(BIT(10))

/* Indicates frame contains a matching BSSI */
#define WSM_RX_STATUS_MATCHING_BSSI	(BIT(11))

/* Indicates More bit set in Framectl field */
#define WSM_RX_STATUS_MORE_DATA		(BIT(12))

/* Indicates frame received during a measurement process */
#define WSM_RX_STATUS_MEASUREMENT	(BIT(13))

/* Indicates frame received as an HT packet */
#define WSM_RX_STATUS_HT		(BIT(14))

/* Indicates frame received with STBC */
#define WSM_RX_STATUS_STBC		(BIT(15))

/* Indicates Address 1 field matches dot11StationId */
#define WSM_RX_STATUS_ADDRESS1		(BIT(16))

/* Indicates Group address present in the Address 1 field */
#define WSM_RX_STATUS_GROUP		(BIT(17))

/* Indicates Broadcast address present in the Address 1 field */
#define WSM_RX_STATUS_BROADCAST		(BIT(18))

/* Indicates group key used with encrypted frames */
#define WSM_RX_STATUS_GROUP_KEY		(BIT(19))

/* Macro to fetch encryption key index. */
#define WSM_RX_STATUS_KEY_IDX(status)	(((status >> 20)) & 0x0F)

/* Frame Control field starts at Frame offset + 2 */
#define WSM_TX_2BYTES_SHIFT		(BIT(7))

/* Join mode */
/* IBSS */
#define WSM_JOIN_MODE_IBSS		(0)

/* BSS */
#define WSM_JOIN_MODE_BSS		(1)

/* PLCP preamble type */
/* For long preamble */
#define WSM_JOIN_PREAMBLE_LONG		(0)

/* For short preamble (Long for 1Mbps) */
#define WSM_JOIN_PREAMBLE_SHORT		(1)

/* For short preamble (Long for 1 and 2Mbps) */
#define WSM_JOIN_PREAMBLE_SHORT_2	(2)

/* Join flags */
/* Unsynchronized */
#define WSM_JOIN_FLAGS_UNSYNCRONIZED	BIT(0)
/* The BSS owner is a P2P GO */
#define WSM_JOIN_FLAGS_P2P_GO		BIT(1)
/* Force to join BSS with the BSSID and the
 * SSID specified without waiting for beacons. The
 * ProbeForJoin parameter is ignored. */
#define WSM_JOIN_FLAGS_FORCE		BIT(2)
/* Give probe request/response higher
 * priority over the BT traffic */
#define WSM_JOIN_FLAGS_PRIO		BIT(3)

/* Key types */
#define WSM_KEY_TYPE_WEP_DEFAULT	(0)
#define WSM_KEY_TYPE_WEP_PAIRWISE	(1)
#define WSM_KEY_TYPE_TKIP_GROUP		(2)
#define WSM_KEY_TYPE_TKIP_PAIRWISE	(3)
#define WSM_KEY_TYPE_AES_GROUP		(4)
#define WSM_KEY_TYPE_AES_PAIRWISE	(5)
#define WSM_KEY_TYPE_WAPI_GROUP		(6)
#define WSM_KEY_TYPE_WAPI_PAIRWISE	(7)
#define WSM_KEY_TYPE_IGTK_GROUP	    (8)

/* Key indexes */
#define RK960_WSM_KEY_MAX_INDEX	(15)
#define WSM_KEY_MAX_INDEX		(10)

/* ACK policy */
#define WSM_ACK_POLICY_NORMAL		(0)
#define WSM_ACK_POLICY_NO_ACK		(1)

/* Start modes */
#define WSM_START_MODE_AP		(0)	/* Mini AP */
#define WSM_START_MODE_P2P_GO		(1)	/* P2P GO */
#define WSM_START_MODE_P2P_DEV		(2)	/* P2P device */

/* SetAssociationMode MIB flags */
#define WSM_ASSOCIATION_MODE_USE_PREAMBLE_TYPE		(BIT(0))
#define WSM_ASSOCIATION_MODE_USE_HT_MODE		(BIT(1))
#define WSM_ASSOCIATION_MODE_USE_BASIC_RATE_SET		(BIT(2))
#define WSM_ASSOCIATION_MODE_USE_MPDU_START_SPACING	(BIT(3))
#define WSM_ASSOCIATION_MODE_SNOOP_ASSOC_FRAMES		(BIT(4))

/* RcpiRssiThreshold MIB flags */
#define WSM_RCPI_RSSI_THRESHOLD_ENABLE	(BIT(0))
#define WSM_RCPI_RSSI_USE_RSSI		(BIT(1))
#define WSM_RCPI_RSSI_DONT_USE_UPPER	(BIT(2))
#define WSM_RCPI_RSSI_DONT_USE_LOWER	(BIT(3))

/* Update-ie constants */
#define WSM_UPDATE_IE_BEACON		(BIT(0))
#define WSM_UPDATE_IE_PROBE_RESP	(BIT(1))
#define WSM_UPDATE_IE_PROBE_REQ		(BIT(2))

/* WSM events */
/* Error */
#define WSM_EVENT_ERROR			(0)

/* BSS lost */
#define WSM_EVENT_BSS_LOST		(1)

/* BSS regained */
#define WSM_EVENT_BSS_REGAINED		(2)

/* Radar detected */
#define WSM_EVENT_RADAR_DETECTED	(3)

/* RCPI or RSSI threshold triggered */
#define WSM_EVENT_RCPI_RSSI		(4)

/* BT inactive */
#define WSM_EVENT_BT_INACTIVE		(5)

/* BT active */
#define WSM_EVENT_BT_ACTIVE		(6)

#define WSM_EVENT_PS_MODE_ERROR         (7)

#define WSM_EVENT_INACTIVITY		(9)

/* MIB IDs */
/* 4.1  dot11StationId */
#define WSM_MIB_ID_DOT11_STATION_ID		0x0000

/* 4.2  dot11MaxtransmitMsduLifeTime */
#define WSM_MIB_ID_DOT11_MAX_TRANSMIT_LIFTIME	0x0001

/* 4.3  dot11MaxReceiveLifeTime */
#define WSM_MIB_ID_DOT11_MAX_RECEIVE_LIFETIME	0x0002

/* 4.4  dot11SlotTime */
#define WSM_MIB_ID_DOT11_SLOT_TIME		0x0003

/* 4.5  dot11GroupAddressesTable */
#define WSM_MIB_ID_DOT11_GROUP_ADDRESSES_TABLE	0x0004
#define WSM_MAX_GRP_ADDRTABLE_ENTRIES		8

/* 4.6  dot11WepDefaultKeyId */
#define WSM_MIB_ID_DOT11_WEP_DEFAULT_KEY_ID	0x0005

/* 4.7  dot11CurrentTxPowerLevel */
#define WSM_MIB_ID_DOT11_CURRENT_TX_POWER_LEVEL	0x0006

/* 4.8  dot11RTSThreshold */
#define WSM_MIB_ID_DOT11_RTS_THRESHOLD		0x0007

/* 4.9  NonErpProtection */
#define WSM_MIB_ID_NON_ERP_PROTECTION		0x1000

/* 4.10 ArpIpAddressesTable */
#define WSM_MIB_ID_ARP_IP_ADDRESSES_TABLE	0x1001
#define WSM_MAX_ARP_IP_ADDRTABLE_ENTRIES	1

/* 4.11 TemplateFrame */
#define WSM_MIB_ID_TEMPLATE_FRAME		0x1002

/* 4.12 RxFilter */
#define WSM_MIB_ID_RX_FILTER			0x1003

/* 4.13 BeaconFilterTable */
#define WSM_MIB_ID_BEACON_FILTER_TABLE		0x1004

/* 4.14 BeaconFilterEnable */
#define WSM_MIB_ID_BEACON_FILTER_ENABLE		0x1005

/* 4.15 OperationalPowerMode */
#define WSM_MIB_ID_OPERATIONAL_POWER_MODE	0x1006

/* 4.16 BeaconWakeUpPeriod */
#define WSM_MIB_ID_BEACON_WAKEUP_PERIOD		0x1007

/* 4.17 RcpiRssiThreshold */
#define WSM_MIB_ID_RCPI_RSSI_THRESHOLD		0x1009

/* 4.18 StatisticsTable */
#define WSM_MIB_ID_STATISTICS_TABLE		0x100A

/* 4.19 IbssPsConfig */
#define WSM_MIB_ID_IBSS_PS_CONFIG		0x100B

/* 4.20 CountersTable */
#define WSM_MIB_ID_COUNTERS_TABLE		0x100C

/* 4.21 BlockAckPolicy */
#define WSM_MIB_ID_BLOCK_ACK_POLICY		0x100E

/* 4.22 OverrideInternalTxRate */
#define WSM_MIB_ID_OVERRIDE_INTERNAL_TX_RATE	0x100F

/* 4.23 SetAssociationMode */
#define WSM_MIB_ID_SET_ASSOCIATION_MODE		0x1010

/* 4.24 UpdateEptaConfigData */
#define WSM_MIB_ID_UPDATE_EPTA_CONFIG_DATA	0x1011

/* 4.25 SelectCcaMethod */
#define WSM_MIB_ID_SELECT_CCA_METHOD		0x1012

/* 4.26 SetUpasdInformation */
#define WSM_MIB_ID_SET_UAPSD_INFORMATION	0x1013

/* 4.27 SetAutoCalibrationMode  WBF00004073 */
#define WSM_MIB_ID_SET_AUTO_CALIBRATION_MODE	0x1015

/* 4.28 SetTxRateRetryPolicy */
#define WSM_MIB_ID_SET_TX_RATE_RETRY_POLICY	0x1016

/* 4.29 SetHostMessageTypeFilter */
#define WSM_MIB_ID_SET_HOST_MSG_TYPE_FILTER	0x1017

/* 4.30 P2PFindInfo */
#define WSM_MIB_ID_P2P_FIND_INFO		0x1018

/* 4.31 P2PPsModeInfo */
#define WSM_MIB_ID_P2P_PS_MODE_INFO		0x1019

/* 4.32 SetEtherTypeDataFrameFilter */
#define WSM_MIB_ID_SET_ETHERTYPE_DATAFRAME_FILTER 0x101A

/* 4.33 SetUDPPortDataFrameFilter */
#define WSM_MIB_ID_SET_UDPPORT_DATAFRAME_FILTER	0x101B

/* 4.34 SetMagicDataFrameFilter */
#define WSM_MIB_ID_SET_MAGIC_DATAFRAME_FILTER	0x101C

/* This is the end of specification. */

/* 4.35 P2PDeviceInfo */
#define WSM_MIB_ID_P2P_DEVICE_INFO		0x101D

/* 4.36 SetWCDMABand */
#define WSM_MIB_ID_SET_WCDMA_BAND		0x101E

/* 4.37 GroupTxSequenceCounter */
#define WSM_MIB_ID_GRP_SEQ_COUNTER		0x101F

/* 4.38 ProtectedMgmtPolicy */
#define WSM_MIB_ID_PROTECTED_MGMT_POLICY	0x1020

/* 4.39 SetHtProtection */
#define WSM_MID_ID_SET_HT_PROTECTION		0x1021

/* 4.40 GPIO Command */
#define WSM_MIB_ID_GPIO_COMMAND			0x1022

/* 4.41 TSF Counter Value */
#define WSM_MIB_ID_TSF_COUNTER			0x1023

/* Test Purposes Only */
#define WSM_MIB_ID_BLOCK_ACK_INFO		0x100D

/* 4.42 UseMultiTxConfMessage */
#define WSM_MIB_USE_MULTI_TX_CONF		0x1024

/* 4.43 Keep-alive period */
#define WSM_MIB_ID_KEEP_ALIVE_PERIOD		0x1025

/* 4.44 Disable BSSID filter */
#define WSM_MIB_ID_DISABLE_BSSID_FILTER		0x1026

/* Inactivity */
#define WSM_MIB_ID_SET_INACTIVITY		0x1035

/* MAC Addr Filter */
#define WSM_MIB_ID_MAC_ADDR_FILTER		0x1030

#ifdef MCAST_FWDING
/* 4.51 Set Forwarding Offload */
#define WSM_MIB_ID_FORWARDING_OFFLOAD		0x1033
#endif

#define WSM_MIB_IP_IPV4_ADDR_FILTER		0x1031
#ifdef IPV6_FILTERING
/* IpV6 Addr Filter */
/* 4.52 Neighbor solicitation IPv6 address table */
#define WSM_MIB_IP_IPV6_ADDR_FILTER		0x1032
#define WSM_MIB_ID_NS_IP_ADDRESSES_TABLE	0x1034
#define WSM_MAX_NDP_IP_ADDRTABLE_ENTRIES	1
#endif /*IPV6_FILTERING */

/* platform: sdio data width */
#define WSM_MIB_ID_SDIO_DATA_WIDTH		            0x1036

/* debug: device debug struct info */
#define WSM_MIB_ID_DEVICE_DEBUG_STRUCT_INFO		    0x1037

	       /* platform *//* sdio rx tp test */
#define WSM_MIB_ID_SDIO_RX_TP_TEST		            0x1038

/* set mac address */
#define WSM_MIB_ID_SET_MAC_ADDR	    	            0x1039

/* set csync threshold */
#define WSM_MIB_ID_SET_CSYNC_THR    	            0x1040

/* read efuse */
#define WSM_MIB_ID_READ_EFUSE                       0x1041

/* write efuse */
#define WSM_MIB_ID_WRITE_EFUSE                      0x1042

/* get rx thoughput info */
#define WSM_MIB_ID_GET_RX_TP_INFO                   0x1043

/* set manual rx gain */
#define WSM_MIB_ID_SET_MANUAL_RXGAIN                0x1044

/* set tx power compensate */
#define WSM_MIB_ID_SET_TXPWR_COMP                   0x1045

/* set lpo source */
#define WSM_MIB_ID_SET_LPO_SOURCE                   0x1046

/* set fw debug level */
#define WSM_MIB_ID_SET_FW_DBG_LEVEL                 0x1047

/* get fw debug level */
#define WSM_MIB_ID_GET_FW_DBG_LEVEL                 0x1048

/* set fw recovery */
#define WSM_MIB_ID_SET_FW_RECOVERY                  0x1049

/* disable tx policy */
#define WSM_MIB_ID_DISABLE_TX_POLICY                0x104A

/* tx max aggregation control */
#define WSM_MIB_ID_TX_AGG_CONTROL                   0x104B

/* Frame template types */
#define WSM_FRAME_TYPE_PROBE_REQUEST	(0)
#define WSM_FRAME_TYPE_BEACON		(1)
#define WSM_FRAME_TYPE_NULL		(2)
#define WSM_FRAME_TYPE_QOS_NULL		(3)
#define WSM_FRAME_TYPE_PS_POLL		(4)
#define WSM_FRAME_TYPE_PROBE_RESPONSE	(5)
#define WSM_FRAME_TYPE_ARP_REPLY        (6)

#ifdef IPV6_FILTERING
#define WSM_FRAME_TYPE_NA               (7)
#endif /*IPV6_FILTERING */

#define WSM_FRAME_GREENFIELD		(0x80)	/* See 4.11 */

/* Status */
/* The WSM firmware has completed a request */
/* successfully. */
#define WSM_STATUS_SUCCESS              (0)

/* This is a generic failure code if other error codes do */
/* not apply. */
#define WSM_STATUS_FAILURE              (1)

/* A request contains one or more invalid parameters. */
#define WSM_INVALID_PARAMETER           (2)

/* The request cannot perform because the device is in */
/* an inappropriate mode. */
#define WSM_ACCESS_DENIED               (3)

/* The frame received includes a decryption error. */
#define WSM_STATUS_DECRYPTFAILURE       (4)

/* A MIC failure is detected in the received packets. */
#define WSM_STATUS_MICFAILURE           (5)

/* The transmit request failed due to retry limit being */
/* exceeded. */
#define WSM_STATUS_RETRY_EXCEEDED       (6)

/* The transmit request failed due to MSDU life time */
/* being exceeded. */
#define WSM_STATUS_TX_LIFETIME_EXCEEDED (7)

/* The link to the AP is lost. */
#define WSM_STATUS_LINK_LOST            (8)

/* No key was found for the encrypted frame */
#define WSM_STATUS_NO_KEY_FOUND         (9)

/* Jammer was detected when transmitting this frame */
#define WSM_STATUS_JAMMER_DETECTED      (10)

/* The message should be requeued later. */
/* This is applicable only to Transmit */
#define WSM_REQUEUE                     (11)

/* Advanced filtering options */
#define WSM_MAX_FILTER_ELEMENTS		(4)

#define WSM_FILTER_ACTION_IGNORE	(0)
#define WSM_FILTER_ACTION_FILTER_IN	(1)
#define WSM_FILTER_ACTION_FILTER_OUT	(2)

#define WSM_FILTER_PORT_TYPE_DST	(0)
#define WSM_FILTER_PORT_TYPE_SRC	(1)

struct wsm_hdr {
	__le16 len;
	__le16 id;
};

#define WSM_TX_SEQ_MAX			(7)
#define WSM_TX_SEQ(seq)			\
		((seq & WSM_TX_SEQ_MAX) << 13)
#define WSM_TX_LINK_ID_MAX		(0x0F)
#define WSM_TX_LINK_ID(link_id)		\
		((link_id & WSM_TX_LINK_ID_MAX) << 6)

#define WSM_TX_IF_ID_MAX		(0x0F)
#define WSM_TX_IF_ID(if_id)		\
		((if_id & WSM_TX_IF_ID_MAX) << 6)

#define MAX_BEACON_SKIP_TIME_MS 1000

#ifdef FPGA_SETUP
#define WSM_CMD_LAST_CHANCE_TIMEOUT (HZ * 9 / 2)
#else
#define WSM_CMD_LAST_CHANCE_TIMEOUT (HZ * 20 / 2)
#endif
#define WSM_CMD_EXTENDED_TIMEOUT (HZ * 20 / 2)

#define WSM_RI_GET_PEER_ID_FROM_FLAGS(_f)         (((_f)&(0xF<<25)>>25))

/* ******************************************************************** */
/* WSM capcbility							*/

struct wsm_caps {
	u16 numInpChBufs;
	u16 sizeInpChBuf;
	u16 hardwareId;
	u16 hardwareSubId;
	u16 firmwareCap;
	u16 firmwareType;
	u16 firmwareApiVer;
	u16 firmwareBuildNumber;
	u16 firmwareVersion;
	int firmwareReady;
        int firmwareSvnVersion;
};

/* ******************************************************************** */
/* WSM commands								*/

struct wsm_tx_power_range {
	int min_power_level;
	int max_power_level;
	u32 stepping;
};

struct wsm_mem_rw {
	int link_id;
	u32 address;
	u16 length;
	u16 flags;
	u32 data[4];
};

#define WSM_RW_F_IRQ_PROTECT     0x80

int wsm_mem_rw(struct rk960_common *hw_priv, const struct wsm_mem_rw *arg,
	       int rw, int if_id);

struct wsm_adc_data_r {
	u32 size;
	u32 block;
	u32 offset;
	u32 channel;
	u32 count;
	u32 abbf;
	u32 lna;
	u32 data[4];
};

int wsm_adc_data_read(struct rk960_common *hw_priv,
		      const struct wsm_adc_data_r *arg, int if_id);

/* 3.1 */
struct wsm_configuration {
	/* [in] */ u32 dot11MaxTransmitMsduLifeTime;
	/* [in] */ u32 dot11MaxReceiveLifeTime;
	/* [in] */ u32 dot11RtsThreshold;
	/* [in, out] */ u8 dot11StationId[6];
	/* [in] */ const void *dpdData;
	/* [in] */ size_t dpdData_size;
	/* [out] */ u8 dot11FrequencyBandsSupported;
	/* [out] */ u32 supportedRateMask;
	/* [out] */ struct wsm_tx_power_range txPowerRange[2];
};

int wsm_configuration(struct rk960_common *hw_priv,
		      struct wsm_configuration *arg, int if_id);

/* 3.3 */
struct wsm_reset {
	/* [in] */ int link_id;
	/* [in] */ bool reset_statistics;
};

int wsm_reset(struct rk960_common *hw_priv, const struct wsm_reset *arg,
	      int if_id);

/* 3.5 */
int wsm_read_mib(struct rk960_common *hw_priv, u16 mibId, void *buf,
		 size_t buf_size);

/* 3.7 */
int wsm_write_mib(struct rk960_common *hw_priv, u16 mibId, void *buf,
		  size_t buf_size, int if_id);

/* 3.9 */
struct wsm_ssid {
	u8 ssid[32];
	u32 length;
};

struct wsm_scan_ch {
	u16 number;
	u32 minChannelTime;
	u32 maxChannelTime;
	u32 txPowerLevel;
};

/* 3.13 */
struct wsm_scan_complete {
	/* WSM_STATUS_... */
	u32 status;

	/* WSM_PSM_... */
	u8 psm;

	/* Number of channels that the scan operation completed. */
	u8 numChannels;
#ifdef ROAM_OFFLOAD
	u16 reserved;
#endif				/*ROAM_OFFLOAD */
};

typedef void (*wsm_scan_complete_cb) (struct rk960_common * hw_priv,
				      struct wsm_scan_complete * arg);

/* 3.9 */
struct wsm_scan {
	/* WSM_PHY_BAND_... */
	/* [in] */ u8 band;

	/* WSM_SCAN_TYPE_... */
	/* [in] */ u8 scanType;

	/* WSM_SCAN_FLAG_... */
	/* [in] */ u8 scanFlags;

	/* WSM_TRANSMIT_RATE_... */
	/* [in] */ u8 maxTransmitRate;

	/* Interval period in TUs that the device shall the re- */
	/* execute the requested scan. Max value supported by the device */
	/* is 256s. */
	/* [in] */ u32 autoScanInterval;

	/* Number of probe requests (per SSID) sent to one (1) */
	/* channel. Zero (0) means that none is send, which */
	/* means that a passive scan is to be done. Value */
	/* greater than zero (0) means that an active scan is to */
	/* be done. */
	/* [in] */ u32 numOfProbeRequests;

	/* Number of channels to be scanned. */
	/* Maximum value is WSM_SCAN_MAX_NUM_OF_CHANNELS. */
	/* [in] */ u8 numOfChannels;

	/* Number of SSID provided in the scan command (this */
	/* is zero (0) in broadcast scan) */
	/* The maximum number of SSIDs is WSM_SCAN_MAX_NUM_OF_SSIDS. */
	/* [in] */ u8 numOfSSIDs;

	/* The delay time (in microseconds) period */
	/* before sending a probe-request. */
	/* [in] */ u8 probeDelay;

	/* SSIDs to be scanned [numOfSSIDs]; */
	/* [in] */ struct wsm_ssid *ssids;

	/* Channels to be scanned [numOfChannels]; */
	/* [in] */ struct wsm_scan_ch *ch;
};

int wsm_scan(struct rk960_common *hw_priv, const struct wsm_scan *arg,
	     int if_id);

/* 3.11 */
int wsm_stop_scan(struct rk960_common *hw_priv, int if_id);

struct wsm_tx_seq_info {
	u16 QosSeqNum[16];
};

struct wsm_tx_seq_counter {
	u32 Bits_47_16;
	u16 Bits_15_00;
	u8 Padding[2];
};

struct wsm_lmc_key_entry {
	struct wsm_tx_seq_counter TSC;
};

struct wsm_rec_info {
	struct wsm_tx_seq_info LmcInterface[2];
	struct wsm_lmc_key_entry sLmcKeyEntry[24];
};

/* 3.14 */
struct wsm_tx_confirm {
	/* Packet identifier used in wsm_tx. */
	/* [out] */ u32 packetID;

	/* WSM_STATUS_... */
	/* [out] */ u32 status;

	/* WSM_TRANSMIT_RATE_... */
	/* [out] */ u8 txedRate;

	/* The number of times the frame was transmitted */
	/* without receiving an acknowledgement. */
	/* [out] */ u8 ackFailures;

	/* WSM_TX_STATUS_... */
	/* [out] */ u16 flags;

	/* The total time in microseconds that the frame spent in */
	/* the WLAN device before transmission as completed. */
	/* [out] */ u32 mediaDelay;

	/* The total time in microseconds that the frame spent in */
	/* the WLAN device before transmission was started. */
	/* [out] */ u32 txQueueDelay;

	/* [out] */ u32 link_id;

	/*[out] */ int if_id;
};

/* 3.15 */
typedef void (*wsm_tx_confirm_cb) (struct rk960_common * hw_priv,
				   struct wsm_tx_confirm * arg);

/* Note that ideology of wsm_tx struct is different against the rest of
 * WSM API. wsm_hdr is /not/ a caller-adapted struct to be used as an input
 * argument for WSM call, but a prepared bytestream to be sent to firmware.
 * It is filled partly in rk960_tx, partly in low-level WSM code.
 * Please pay attention once again: ideology is different.
 *
 * Legend:
 * - [in]: rk960_tx must fill this field.
 * - [wsm]: the field is filled by low-level WSM.
 */
struct wsm_tx {
	/* common WSM header */
	/* [in/wsm] */ struct wsm_hdr hdr;

	/* Packet identifier that meant to be used in completion. */
	/* [in] */ __le32 packetID;

	/* WSM_TRANSMIT_RATE_... */
	/* [in] */ u8 maxTxRate;

	/* WSM_QUEUE_... */
	/* [in] */ u8 queueId;

	/* True: another packet is pending on the host for transmission. */
	/* [wsm] */ u8 more;

	/* Bit 0 = 0 - Start expiry time from first Tx attempt (default) */
	/* Bit 0 = 1 - Start expiry time from receipt of Tx Request */
	/* Bits 3:1  - PTA Priority */
	/* Bits 6:4  - Tx Rate Retry Policy */
	/* Bit 7 - Reserved */
	/* [in] */ u8 flags;

	/* Should be 0. */
	/* [in] */ __le32 reserved;

	/* The elapsed time in TUs, after the initial transmission */
	/* of an MSDU, after which further attempts to transmit */
	/* the MSDU shall be terminated. Overrides the global */
	/* dot11MaxTransmitMsduLifeTime setting [optional] */
	/* Device will set the default value if this is 0. */
	/* [wsm] */ __le32 expireTime;

	/* WSM_HT_TX_... */
	/* [in] */ __le32 htTxParameters;
};

/* = sizeof(generic hi hdr) + sizeof(wsm hdr) + sizeof(alignment) */
#define WSM_TX_EXTRA_HEADROOM (28)

/* 3.16 */
struct wsm_rx {
	/* WSM_STATUS_... */
	/* [out] */ u32 status;

	/* Specifies the channel of the received packet. */
	/* [out] */ u16 channelNumber;

	/* WSM_TRANSMIT_RATE_... */
	/* [out] */ u8 rxedRate;

	/* This value is expressed in signed Q8.0 format for */
	/* RSSI and unsigned Q7.1 format for RCPI. */
	/* [out] */ u8 rcpiRssi;

	/* WSM_RX_STATUS_... */
	/* [out] */ u32 flags;

	/* An 802.11 frame. */
	/* [out] */ void *frame;

	/* Size of the frame */
	/* [out] */ size_t frame_size;

	/* Link ID */
	/* [out] */ int link_id;
	/* [out] */ int if_id;
};

/* = sizeof(generic hi hdr) + sizeof(wsm hdr) */
#define WSM_RX_EXTRA_HEADROOM (16)

typedef void (*wsm_rx_cb) (struct rk960_vif * priv, struct wsm_rx * arg,
			   struct sk_buff ** skb_p);

/* 3.17 */
struct wsm_event {
	/* WSM_STATUS_... */
	/* [out] */ u32 eventId;

	/* Indication parameters. */
	/* For error indication, this shall be a 32-bit WSM status. */
	/* For RCPI or RSSI indication, this should be an 8-bit */
	/* RCPI or RSSI value. */
	/* [out] */ u32 eventData;
};

struct rk960_wsm_event {
	struct list_head link;
	struct wsm_event evt;
	u8 if_id;
};

/* 3.18 - 3.22 */
/* Measurement. Skipped for now. Irrelevent. */

typedef void (*wsm_event_cb) (struct rk960_common * hw_priv,
			      struct wsm_event * arg);

/* 3.23 */
struct wsm_join {
	/* WSM_JOIN_MODE_... */
	/* [in] */ u8 mode;

	/* WSM_PHY_BAND_... */
	/* [in] */ u8 band;

	/* Specifies the channel number to join. The channel */
	/* number will be mapped to an actual frequency */
	/* according to the band */
	/* [in] */ u16 channelNumber;

	/* Specifies the BSSID of the BSS or IBSS to be joined */
	/* or the IBSS to be started. */
	/* [in] */ u8 bssid[6];

	/* ATIM window of IBSS */
	/* When ATIM window is zero the initiated IBSS does */
	/* not support power saving. */
	/* [in] */ u16 atimWindow;

	/* WSM_JOIN_PREAMBLE_... */
	/* [in] */ u8 preambleType;

	/* Specifies if a probe request should be send with the */
	/* specified SSID when joining to the network. */
	/* [in] */ u8 probeForJoin;

	/* DTIM Period (In multiples of beacon interval) */
	/* [in] */ u8 dtimPeriod;

	/* WSM_JOIN_FLAGS_... */
	/* [in] */ u8 flags;

	/* Length of the SSID */
	/* [in] */ u32 ssidLength;

	/* Specifies the SSID of the IBSS to join or start */
	/* [in] */ u8 ssid[32];

	/* Specifies the time between TBTTs in TUs */
	/* [in] */ u32 beaconInterval;

	/* A bit mask that defines the BSS basic rate set. */
	/* [in] */ u32 basicRateSet;

	/* Minimum transmission power level in units of 0.1dBm */
	/* [out] */ int minPowerLevel;

	/* Maximum transmission power level in units of 0.1dBm */
	/* [out] */ int maxPowerLevel;
};

int wsm_join(struct rk960_common *hw_priv, struct wsm_join *arg, int if_id);

/* 3.25 */
struct wsm_set_pm {
	/* WSM_PSM_... */
	/* [in] */ u8 pmMode;

	/* in unit of 500us; 0 to use default */
	/* [in] */ u8 fastPsmIdlePeriod;

	/* in unit of 500us; 0 to use default */
	/* [in] */ u8 apPsmChangePeriod;

	/* in unit of 500us; 0 to disable auto-pspoll */
	/* [in] */ u8 minAutoPsPollPeriod;
};

int wsm_set_pm(struct rk960_common *hw_priv, const struct wsm_set_pm *arg,
	       int if_id);

/* 3.27 */
struct wsm_set_pm_complete {
	u8 psm;			/* WSM_PSM_... */
};

typedef void (*wsm_set_pm_complete_cb) (struct rk960_common * hw_priv,
					struct wsm_set_pm_complete * arg);

/* 3.28 */
struct wsm_set_bss_params {
	/* The number of lost consecutive beacons after which */
	/* the WLAN device should indicate the BSS-Lost event */
	/* to the WLAN host driver. */
	u8 beaconLostCount;

	/* The AID received during the association process. */
	u16 aid;

	/* The operational rate set mask */
	u32 operationalRateSet;
};

int wsm_set_bss_params(struct rk960_common *hw_priv,
		       const struct wsm_set_bss_params *arg, int if_id);

/* 3.30 */
struct wsm_add_key {
	u8 type;		/* WSM_KEY_TYPE_... */
	u8 entryIndex;		/* Key entry index: 0 -- WSM_KEY_MAX_INDEX */
	u16 reserved;
	union {
		struct {
			u8 peerAddress[6];	/* MAC address of the
						 * peer station */
			u8 reserved;
			u8 keyLength;	/* Key length in bytes */
			u8 keyData[16];	/* Key data */
		} __packed wepPairwiseKey;
		struct {
			u8 keyId;	/* Unique per key identifier
					 * (0..3) */
			u8 keyLength;	/* Key length in bytes */
			u16 reserved;
			u8 keyData[16];	/* Key data */
		} __packed wepGroupKey;
		struct {
			u8 peerAddress[6];	/* MAC address of the
						 * peer station */
			u8 reserved[2];
			u8 tkipKeyData[16];	/* TKIP key data */
			u8 rxMicKey[8];	/* Rx MIC key */
			u8 txMicKey[8];	/* Tx MIC key */
		} __packed tkipPairwiseKey;
		struct {
			u8 tkipKeyData[16];	/* TKIP key data */
			u8 rxMicKey[8];	/* Rx MIC key */
			u8 keyId;	/* Key ID */
			u8 reserved[3];
			u8 rxSeqCounter[8];	/* Receive Sequence Counter */
		} __packed tkipGroupKey;
		struct {
			u8 peerAddress[6];	/* MAC address of the
						 * peer station */
			u16 reserved;
			u8 aesKeyData[16];	/* AES key data */
		} __packed aesPairwiseKey;
		struct {
			u8 aesKeyData[16];	/* AES key data */
			u8 keyId;	/* Key ID */
			u8 reserved[3];
			u8 rxSeqCounter[8];	/* Receive Sequence Counter */
		} __packed aesGroupKey;
		struct {
			u8 igtKeyData[16];
			u8 keyId;
			u8 reserved[3];
			u8 ipn[8];	/* actual IPN is only 6 bytes long */
		} __packed igtkGroupKey;
		struct {
			u8 peerAddress[6];	/* MAC address of the
						 * peer station */
			u8 keyId;	/* Key ID */
			u8 reserved;
			u8 wapiKeyData[16];	/* WAPI key data */
			u8 micKeyData[16];	/* MIC key data */
		} __packed wapiPairwiseKey;
		struct {
			u8 wapiKeyData[16];	/* WAPI key data */
			u8 micKeyData[16];	/* MIC key data */
			u8 keyId;	/* Key ID */
			u8 reserved[3];
		} __packed wapiGroupKey;
	} __packed;
} __packed;

int wsm_add_key(struct rk960_common *hw_priv, const struct wsm_add_key *arg,
		int if_id);

/* 3.32 */
struct wsm_remove_key {
	/* Key entry index : 0-10 */
	u8 entryIndex;
};

int wsm_remove_key(struct rk960_common *hw_priv,
		   const struct wsm_remove_key *arg, int if_id);

/* 3.34 */
struct wsm_set_tx_queue_params {
	/* WSM_ACK_POLICY_... */
	u8 ackPolicy;

	/* Medium Time of TSPEC (in 32us units) allowed per */
	/* One Second Averaging Period for this queue. */
	u16 allowedMediumTime;

	/* dot11MaxTransmitMsduLifetime to be used for the */
	/* specified queue. */
	u32 maxTransmitLifetime;
};

struct wsm_tx_queue_params {
	/* NOTE: index is a linux queue id. */
	struct wsm_set_tx_queue_params params[4];
};

#define WSM_TX_QUEUE_SET(queue_params, queue, ack_policy, allowed_time,     \
			 max_life_time)					    \
do {									    \
	struct wsm_set_tx_queue_params *p = &(queue_params)->params[queue]; \
	p->ackPolicy = (ack_policy);				\
	p->allowedMediumTime = (allowed_time);				\
	p->maxTransmitLifetime = (max_life_time);			\
} while (0)

int wsm_set_tx_queue_params(struct rk960_common *hw_priv,
			    const struct wsm_set_tx_queue_params *arg,
			    u8 id, int if_id);

/* 3.36 */
struct wsm_edca_queue_params {
	/* CWmin (in slots) for the access class. */
	/* [in] */ u16 cwMin;

	/* CWmax (in slots) for the access class. */
	/* [in] */ u16 cwMax;

	/* AIFS (in slots) for the access class. */
	/* [in] */ u8 aifns;

	/* TX OP Limit (in microseconds) for the access class. */
	/* [in] */ u16 txOpLimit;

	/* dot11MaxReceiveLifetime to be used for the specified */
	/* the access class. Overrides the global */
	/* dot11MaxReceiveLifetime value */
	/* [in] */ u32 maxReceiveLifetime;

	/* UAPSD trigger support for the access class. */
	/* [in] */ bool uapsdEnable;
};

struct wsm_edca_params {
	/* NOTE: index is a linux queue id. */
	struct wsm_edca_queue_params params[4];
};

#define TXOP_UNIT 32
#define WSM_EDCA_SET(edca, queue, aifs, cw_min, cw_max, txop, life_time,\
		uapsd)	\
	do {							\
		struct wsm_edca_queue_params *p = &(edca)->params[queue]; \
		p->cwMin = (cw_min);				\
		p->cwMax = (cw_max);				\
		p->aifns = (aifs);				\
		p->txOpLimit = ((txop) * TXOP_UNIT);		\
		p->maxReceiveLifetime = (life_time);		\
		p->uapsdEnable = (uapsd);			\
	} while (0)

int wsm_set_edca_params(struct rk960_common *hw_priv,
			const struct wsm_edca_params *arg, int if_id);

int wsm_set_uapsd_param(struct rk960_common *hw_priv,
			const struct wsm_edca_params *arg);

/* 3.38 */
/* Set-System info. Skipped for now. Irrelevent. */

/* 3.40 */
struct wsm_switch_channel {
	/* 1 - means the STA shall not transmit any further */
	/* frames until the channel switch has completed */
	/* [in] */ u8 channelMode;

	/* Number of TBTTs until channel switch occurs. */
	/* 0 - indicates switch shall occur at any time */
	/* 1 - occurs immediately before the next TBTT */
	/* [in] */ u8 channelSwitchCount;

	/* The new channel number to switch to. */
	/* Note this is defined as per section 2.7. */
	/* [in] */ u16 newChannelNumber;
};

int wsm_switch_channel(struct rk960_common *hw_priv,
		       const struct wsm_switch_channel *arg, int if_id);

typedef void (*wsm_channel_switch_cb) (struct rk960_common * hw_priv);

struct wsm_start {
	/* WSM_START_MODE_... */
	/* [in] */ u8 mode;

	/* WSM_PHY_BAND_... */
	/* [in] */ u8 band;

	/* Channel number */
	/* [in] */ u16 channelNumber;

	/* Client Traffic window in units of TU */
	/* Valid only when mode == ..._P2P */
	/* [in] */ u32 CTWindow;

	/* Interval between two consecutive */
	/* beacon transmissions in TU. */
	/* [in] */ u32 beaconInterval;

	/* DTIM period in terms of beacon intervals */
	/* [in] */ u8 DTIMPeriod;

	/* WSM_JOIN_PREAMBLE_... */
	/* [in] */ u8 preambleType;

	/* The delay time (in microseconds) period */
	/* before sending a probe-request. */
	/* [in] */ u8 probeDelay;

	/* Length of the SSID */
	/* [in] */ u8 ssidLength;

	/* SSID of the BSS or P2P_GO to be started now. */
	/* [in] */ u8 ssid[32];

	/* The basic supported rates for the MiniAP. */
	/* [in] */ u32 basicRateSet;
};

int wsm_start(struct rk960_common *hw_priv, const struct wsm_start *arg,
	      int if_id);

#if 0
struct wsm_beacon_transmit {
	/* 1: enable; 0: disable */
	/* [in] */ u8 enableBeaconing;
};

int wsm_beacon_transmit(struct rk960_common *hw_priv,
			const struct wsm_beacon_transmit *arg, int if_id);
#endif

int wsm_start_find(struct rk960_common *hw_priv, int if_id);

int wsm_stop_find(struct rk960_common *hw_priv, int if_id);

typedef void (*wsm_find_complete_cb) (struct rk960_common * hw_priv,
				      u32 status);

struct wsm_suspend_resume {
	/* See 3.52 */
	/* Link ID */
	/* [out] */ int link_id;
	/* Stop sending further Tx requests down to device for this link */
	/* [out] */ bool stop;
	/* Transmit multicast Frames */
	/* [out] */ bool multicast;
	/* The AC on which Tx to be suspended /resumed. */
	/* This is applicable only for U-APSD */
	/* WSM_QUEUE_... */
	/* [out] */ int queue;
	/* [out] */ int if_id;
};

typedef void (*wsm_suspend_resume_cb) (struct rk960_vif * priv,
				       struct wsm_suspend_resume * arg);

/* 3.54 Update-IE request. */
struct wsm_update_ie {
	/* WSM_UPDATE_IE_... */
	/* [in] */ u16 what;
	/* [in] */ u16 count;
	/* [in] */ u8 *ies;
	/* [in] */ size_t length;
};

int wsm_update_ie(struct rk960_common *hw_priv,
		  const struct wsm_update_ie *arg, int if_id);

/* 3.56 */
struct wsm_map_link {
	/* MAC address of the remote device */
	/* [in] */ u8 mac_addr[6];
	/* [in] */ u8 unmap;
	/* [in] */ u8 link_id;
};

int wsm_map_link(struct rk960_common *hw_priv, const struct wsm_map_link *arg,
		 int if_id);

struct wsm_cbc {
	wsm_scan_complete_cb scan_complete;
	wsm_tx_confirm_cb tx_confirm;
	wsm_rx_cb rx;
	wsm_event_cb event;
	wsm_set_pm_complete_cb set_pm_complete;
	wsm_channel_switch_cb channel_switch;
	wsm_find_complete_cb find_complete;
	wsm_suspend_resume_cb suspend_resume;
};
#ifdef MCAST_FWDING

/* 3.65	Give Buffer Request */
int wsm_init_release_buffer_request(struct rk960_common *priv, u8 index);

/* 3.67	Request Buffer Request */
int wsm_request_buffer_request(struct rk960_vif *priv, u8 * arg);
#endif
/* ******************************************************************** */
/* MIB shortcats							*/

int wsm_set_output_power(struct rk960_common *hw_priv,
			 int power_level, int if_id);

int wsm_set_beacon_wakeup_period(struct rk960_common *hw_priv,
					       unsigned dtim_interval,
					       unsigned listen_interval,
					       int if_id);

struct wsm_rcpi_rssi_threshold {
	u8 rssiRcpiMode;	/* WSM_RCPI_RSSI_... */
	u8 lowerThreshold;
	u8 upperThreshold;
	u8 rollingAverageCount;
};

static inline int wsm_set_rcpi_rssi_threshold(struct rk960_common *hw_priv,
					      struct wsm_rcpi_rssi_threshold
					      *arg, int if_id)
{
	return wsm_write_mib(hw_priv, WSM_MIB_ID_RCPI_RSSI_THRESHOLD, arg,
			     sizeof(*arg), if_id);
}

struct wsm_counters_table {
//typedef struct MIB_COMMON_COUNTERS_TABLE_S
	__le32 countPlcpErrors;
	__le32 countFcsErrors;
	__le32 countTxPackets;
	__le32 countRxPackets;
	__le32 countRxPacketErrors;
	__le32 countRtsSuccess;
	__le32 countRtsFailures;
	__le32 countRxFramesSuccess;
//typedef struct MIB_COUNTERS_TABLE_S   
	__le32 countRxDecryptionFailures;
	__le32 countRxMicFailures;
	__le32 countRxNoKeyFailures;
	__le32 countTxMulticastFrames;
	__le32 countTxFramesSuccess;
	__le32 countTxFrameFailures;
	__le32 countTxFramesRetried;
	__le32 countTxFramesMultiRetried;
	__le32 countRxFrameDuplicates;
//      __le32 countRtsSuccess;
//      __le32 countRtsFailures;
	__le32 countAckFailures;
	__le32 countRxMulticastFrames;
//      __le32 countRxFramesSuccess;
//typedef struct MIB_11W_COUNTERS_S
	__le32 countRxCMACICVErrors;
	__le32 countRxCMACReplays;
	__le32 countRxMgmtCCMPReplays;
	__le32 countRxBIPMICErrors;
};

static inline int wsm_get_counters_table(struct rk960_common *hw_priv,
					 struct wsm_counters_table *arg)
{
	return wsm_read_mib(hw_priv, WSM_MIB_ID_COUNTERS_TABLE,
			    arg, sizeof(*arg));
}

#define RK960_RX_TP_INFO_RATE_NUM 22
#define RK960_RX_TP_INFO_DUMP_NUM 3
struct wsm_rx_tp_info_s {
	u16 frame_type;
	u16 size;
	u8 rssi;
	u8 snr;
	u8 ch;
	u8 mode;
};

struct wsm_rx_tp_info {
	u32 count;		// The packet received count at the current rate
	u8 dump_count;		// recent dump num
	u8 reserved[3];
	struct wsm_rx_tp_info_s rx_tp_info_s[RK960_RX_TP_INFO_DUMP_NUM];
};

static inline int wsm_get_rx_tp_info(struct rk960_common *hw_priv,
				     struct wsm_rx_tp_info *info)
{
	return wsm_read_mib(hw_priv, WSM_MIB_ID_GET_RX_TP_INFO,
			    info, sizeof(*info) * RK960_RX_TP_INFO_RATE_NUM);
}

static inline int wsm_get_station_id(struct rk960_common *hw_priv, u8 * mac)
{
	return wsm_read_mib(hw_priv, WSM_MIB_ID_DOT11_STATION_ID, mac,
			    ETH_ALEN);
}

#define RK960_EFUSE_SIZE    64
static inline int wsm_read_efuse(struct rk960_common *hw_priv, u8 * efuse)
{
	return wsm_read_mib(hw_priv, WSM_MIB_ID_READ_EFUSE, efuse,
			    RK960_EFUSE_SIZE);
}

static inline int wsm_write_efuse(struct rk960_common *hw_priv, u8 offset,
				  u8 value)
{
	struct {
		u8 offset;
		u8 value;
	} val;
	val.offset = offset;
	val.value = value;
	return wsm_write_mib(hw_priv, WSM_MIB_ID_WRITE_EFUSE, &val,
			     sizeof(val), 0);
}

static inline int wsm_set_manual_rxgain(struct rk960_common *hw_priv,
					u8 en, u8 abbf, u8 lna)
{
	struct {
		u8 abbf;
		u8 lna;
		u8 en;
	} val;
	val.abbf = abbf;
	val.lna = lna;
	val.en = en;
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_MANUAL_RXGAIN, &val,
			     sizeof(val), 0);
}

int wsm_set_txpwr_comp(struct rk960_common *hw_priv);

struct wsm_rx_filter {
	bool promiscuous;
	bool bssid;
	bool fcs;
	bool probeResponder;
	bool keepalive;
};

static inline int wsm_set_rx_filter(struct rk960_common *hw_priv,
				    const struct wsm_rx_filter *arg, int if_id)
{
	__le32 val = 0;
	if (arg->promiscuous)
		val |= __cpu_to_le32(BIT(0));
	if (arg->bssid)
		val |= __cpu_to_le32(BIT(1));
	if (arg->fcs)
		val |= __cpu_to_le32(BIT(2));
	if (arg->probeResponder)
		val |= __cpu_to_le32(BIT(3));
	if (arg->keepalive)
		val |= __cpu_to_le32(BIT(4));
	return wsm_write_mib(hw_priv, WSM_MIB_ID_RX_FILTER, &val, sizeof(val),
			     if_id);
}

int wsm_set_probe_responder(struct rk960_vif *priv, bool enable);
int wsm_set_keepalive_filter(struct rk960_vif *priv, bool enable);

#define WSM_BEACON_FILTER_IE_HAS_CHANGED	BIT(0)
#define WSM_BEACON_FILTER_IE_NO_LONGER_PRESENT	BIT(1)
#define WSM_BEACON_FILTER_IE_HAS_APPEARED	BIT(2)

struct wsm_beacon_filter_table_entry {
	u8 ieId;
	u8 actionFlags;
	u8 oui[3];
	u8 matchData[3];
} __packed;

struct wsm_beacon_filter_table {
	__le32 numOfIEs;
	struct wsm_beacon_filter_table_entry entry[10];
} __packed;

static inline int wsm_set_beacon_filter_table(struct rk960_common *hw_priv,
					      struct wsm_beacon_filter_table
					      *ft, int if_id)
{
	size_t size = __le32_to_cpu(ft->numOfIEs) *
	    sizeof(struct wsm_beacon_filter_table_entry) + sizeof(__le32);

	return wsm_write_mib(hw_priv, WSM_MIB_ID_BEACON_FILTER_TABLE, ft, size,
			     if_id);
}

#define WSM_BEACON_FILTER_ENABLE	BIT(0)	/* Enable/disable beacon filtering */
#define WSM_BEACON_FILTER_AUTO_ERP	BIT(1)	/* If 1 FW will handle ERP IE changes internally */

struct wsm_beacon_filter_control {
	int enabled;
	int bcn_count;
};

static inline int wsm_beacon_filter_control(struct rk960_common *hw_priv,
					    struct wsm_beacon_filter_control
					    *arg, int if_id)
{
	struct {
		__le32 enabled;
		__le32 bcn_count;
	} val;
	val.enabled = __cpu_to_le32(arg->enabled);
	val.bcn_count = __cpu_to_le32(arg->bcn_count);
	return wsm_write_mib(hw_priv, WSM_MIB_ID_BEACON_FILTER_ENABLE, &val,
			     sizeof(val), if_id);
}

enum wsm_power_mode {
	wsm_power_mode_active = 0,
	wsm_power_mode_doze = 1,
	wsm_power_mode_quiescent = 2,
};

struct wsm_operational_mode {
	enum wsm_power_mode power_mode;
	int disableMoreFlagUsage;
	int performAntDiversity;
};

static inline int wsm_set_operational_mode(struct rk960_common *hw_priv,
					   const struct wsm_operational_mode
					   *arg, int if_id)
{
	u32 val = arg->power_mode;
	if (arg->disableMoreFlagUsage)
		val |= BIT(4);
	if (arg->performAntDiversity)
		val |= BIT(5);
	return wsm_write_mib(hw_priv, WSM_MIB_ID_OPERATIONAL_POWER_MODE, &val,
			     sizeof(val), if_id);
}

struct wsm_sdio_parameter {
	__le16 data_width;
	u8 reserved[2];
} __packed;

static inline int wsm_set_sdio_data_width(struct rk960_common *hw_priv,
					  int widtd, int if_id)
{
	struct wsm_sdio_parameter arg = {
		.data_width = __cpu_to_le16(widtd),
	};
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SDIO_DATA_WIDTH,
			     &arg, sizeof(arg), if_id);
}

#define RK960_LPO_INTERNAL  2
#define RK960_LPO_EXTERNAL  1

struct wsm_lpo_source {
	__le16 lpo_source;
} __packed;

static inline int wsm_set_lpo_source(struct rk960_common *hw_priv,
				     int lpo_source, int if_id)
{
	struct wsm_lpo_source arg = {
		.lpo_source = __cpu_to_le16(lpo_source),
	};
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_LPO_SOURCE,
			     &arg, sizeof(arg), if_id);
}

struct wsm_dbg_level {
	__le32 dbg_level;
} __packed;

static inline int wsm_set_fw_dbg_level(struct rk960_common *hw_priv,
				       int dbg_level)
{
	struct wsm_dbg_level arg = {
		.dbg_level = __cpu_to_le32(dbg_level),
	};
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_FW_DBG_LEVEL,
			     &arg, sizeof(arg), 0);
}

static inline int wsm_get_fw_dbg_level(struct rk960_common *hw_priv,
				       u32 * dbg_level)
{
	return wsm_read_mib(hw_priv, WSM_MIB_ID_GET_FW_DBG_LEVEL, dbg_level, 4);
}

static inline int wsm_set_fw_recovery(struct rk960_common *hw_priv,
				      struct wsm_rec_info *arg)
{
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_FW_RECOVERY,
			     arg, sizeof(*arg), 0);
}

#ifdef DISABLE_TX_POLICY
#define RK960_RATE_INFO_SIZE    16
struct wsm_disable_tx_policy {
	__le32 disable;
} __packed;

static inline int wsm_disable_tx_policy(struct rk960_common *hw_priv, int disable)
{
	struct wsm_disable_tx_policy arg = {
		.disable = disable,
	};
	return wsm_write_mib(hw_priv, WSM_MIB_ID_DISABLE_TX_POLICY,
			     &arg, sizeof(arg), 0);
}
#endif

struct wsm_set_tx_agg_control {
        /* mcs 0-7 tx max aggregation number,if tx aggregation num equal 0,not aggregation */
	u8 tx_max_agg_num[8];
} __packed;

static inline int wsm_set_agg_control(struct rk960_common *hw_priv,
                struct wsm_set_tx_agg_control *arg)
{
	return wsm_write_mib(hw_priv, WSM_MIB_ID_TX_AGG_CONTROL,
			     arg, sizeof(struct wsm_set_tx_agg_control), 0);
}

static inline int wsm_set_sdio_rx_tp_test(struct rk960_common *hw_priv,
					  int enable, int if_id)
{
	struct wsm_sdio_parameter arg = {
		.data_width = __cpu_to_le16(enable),
	};
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SDIO_RX_TP_TEST,
			     &arg, sizeof(arg), if_id);
}

struct wsm_mac_addr {
	u8 addr[6];
} __packed;

static inline int wsm_set_mac_addr(struct rk960_common *hw_priv,
				   u8 * addr, int if_id)
{
	struct wsm_mac_addr arg;

	memcpy(arg.addr, addr, 6);
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_MAC_ADDR,
			     &arg, sizeof(arg), if_id);
}

struct wsm_csync_thr {
	s8 csync;
} __packed;

int wsm_set_csync_thr(struct rk960_common *hw_priv, s8 csync, int if_id);

struct wsm_device_debug_struct_info {
	int size;
	int mem_addr;
	int reserved1;
	int reserved2;
};

static inline int wsm_get_device_debug_struct_info(struct rk960_common *hw_priv,
						   struct
						   wsm_device_debug_struct_info
						   *arg)
{
	return wsm_read_mib(hw_priv, WSM_MIB_ID_DEVICE_DEBUG_STRUCT_INFO,
			    arg, sizeof(*arg));
}

struct wsm_inactivity {
	u8 max_inactivity;
	u8 min_inactivity;
};

static inline int wsm_set_inactivity(struct rk960_common *hw_priv,
				     const struct wsm_inactivity *arg,
				     int if_id)
{
	struct {
		u8 min_inactive;
		u8 max_inactive;
		u16 reserved;
	} val;

	val.max_inactive = arg->max_inactivity;
	val.min_inactive = arg->min_inactivity;
	val.reserved = 0;

	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_INACTIVITY, &val,
			     sizeof(val), if_id);
}

struct wsm_template_frame {
	u8 frame_type;
	u8 rate;
	bool disable;
	struct sk_buff *skb;
};

#define RK960_MAX_BEACON_SIZE  1024
extern void rk960_adjust_beacon_channel_info(struct rk960_common *hw_priv,
					     struct wsm_template_frame *arg,
					     int if_id, struct sk_buff *skb);

static inline int wsm_set_template_frame(struct rk960_common *hw_priv,
					 struct wsm_template_frame *arg,
					 int if_id)
{
	int ret;
	u8 *p;

#ifdef RK960_CHANNEL_CONFLICT_OPT
	if (arg->frame_type == WSM_FRAME_TYPE_BEACON ||
	    arg->frame_type == WSM_FRAME_TYPE_PROBE_RESPONSE) {
		rk960_adjust_beacon_channel_info(hw_priv, arg, if_id, arg->skb);
	}
#endif

	p = skb_push(arg->skb, 4);
	p[0] = arg->frame_type;
	p[1] = arg->rate;
	if (arg->disable)
		((u16 *) p)[1] = 0;
	else
		((u16 *) p)[1] = __cpu_to_le16(arg->skb->len - 4);
	ret = wsm_write_mib(hw_priv, WSM_MIB_ID_TEMPLATE_FRAME, p,
			    arg->skb->len, if_id);
	skb_pull(arg->skb, 4);
	return ret;
}

struct wsm_protected_mgmt_policy {
	bool protectedMgmtEnable;
	bool unprotectedMgmtFramesAllowed;
	bool encryptionForAuthFrame;
};

static inline int
wsm_set_protected_mgmt_policy(struct rk960_common *hw_priv,
			      struct wsm_protected_mgmt_policy *arg, int if_id)
{
	__le32 val = 0;
	int ret;
	if (arg->protectedMgmtEnable)
		val |= __cpu_to_le32(BIT(0));
	if (arg->unprotectedMgmtFramesAllowed)
		val |= __cpu_to_le32(BIT(1));
	if (arg->encryptionForAuthFrame)
		val |= __cpu_to_le32(BIT(2));
	ret = wsm_write_mib(hw_priv, WSM_MIB_ID_PROTECTED_MGMT_POLICY, &val,
			    sizeof(val), if_id);
	return ret;
}

static inline int wsm_set_block_ack_policy(struct rk960_common *hw_priv,
					   u8 blockAckTxTidPolicy,
					   u8 blockAckRxTidPolicy, int if_id)
{
	struct {
		u8 blockAckTxTidPolicy;
		u8 reserved1;
		u8 blockAckRxTidPolicy;
		u8 reserved2;
	} val = {
	.blockAckTxTidPolicy =
		    blockAckTxTidPolicy,.blockAckRxTidPolicy =
		    blockAckRxTidPolicy,};
	//pr_info("%s: tx %x rx %x if_id %d\n",
	//        __func__, blockAckTxTidPolicy, blockAckRxTidPolicy, if_id);
	return wsm_write_mib(hw_priv, WSM_MIB_ID_BLOCK_ACK_POLICY, &val,
			     sizeof(val), if_id);
}

struct wsm_association_mode {
	u8 flags;		/* WSM_ASSOCIATION_MODE_... */
	u8 preambleType;	/* WSM_JOIN_PREAMBLE_... */
	u8 greenfieldMode;	/* 1 for greenfield */
	u8 mpduStartSpacing;
	__le32 basicRateSet;
};

static inline int wsm_set_association_mode(struct rk960_common *hw_priv,
					   struct wsm_association_mode *arg,
					   int if_id)
{
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_ASSOCIATION_MODE, arg,
			     sizeof(*arg), if_id);
}

struct wsm_set_tx_rate_retry_policy_header {
	u8 numTxRatePolicies;
	u8 reserved[3];
} __packed;

struct wsm_set_tx_rate_retry_policy_policy {
	u8 policyIndex;
	u8 shortRetryCount;
	u8 longRetryCount;
	u8 policyFlags;
	u8 rateRecoveryCount;
	u8 reserved[3];
	__le32 rateCountIndices[3];
} __packed;

struct wsm_set_tx_rate_retry_policy {
	struct wsm_set_tx_rate_retry_policy_header hdr;
	struct wsm_set_tx_rate_retry_policy_policy tbl[8];
} __packed;

static inline int wsm_set_tx_rate_retry_policy(struct rk960_common *hw_priv,
					       struct
					       wsm_set_tx_rate_retry_policy
					       *arg, int if_id)
{
	size_t size = sizeof(struct wsm_set_tx_rate_retry_policy_header) +
	    arg->hdr.numTxRatePolicies *
	    sizeof(struct wsm_set_tx_rate_retry_policy_policy);
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_TX_RATE_RETRY_POLICY, arg,
			     size, if_id);
}

/* 4.32 SetEtherTypeDataFrameFilter */
struct wsm_ether_type_filter_hdr {
	u8 nrFilters;		/* Up to WSM_MAX_FILTER_ELEMENTS */
	u8 reserved[3];
} __packed;

struct wsm_ether_type_filter {
	u8 filterAction;	/* WSM_FILTER_ACTION_XXX */
	u8 reserved;
	__le16 etherType;	/* Type of ethernet frame */
} __packed;

static inline int wsm_set_ether_type_filter(struct rk960_common *hw_priv,
					    struct wsm_ether_type_filter_hdr
					    *arg, int if_id)
{
	size_t size = sizeof(struct wsm_ether_type_filter_hdr) +
	    arg->nrFilters * sizeof(struct wsm_ether_type_filter);
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_ETHERTYPE_DATAFRAME_FILTER,
			     arg, size, if_id);
}

/* 4.33 SetUDPPortDataFrameFilter */
struct wsm_udp_port_filter_hdr {
	u8 nrFilters;		/* Up to WSM_MAX_FILTER_ELEMENTS */
	u8 reserved[3];
} __packed;

struct wsm_udp_port_filter {
	u8 filterAction;	/* WSM_FILTER_ACTION_XXX */
	u8 portType;		/* WSM_FILTER_PORT_TYPE_XXX */
	__le16 udpPort;		/* Port number */
} __packed;

static inline int wsm_set_udp_port_filter(struct rk960_common *hw_priv,
					  struct wsm_udp_port_filter_hdr *arg,
					  int if_id)
{
	size_t size = sizeof(struct wsm_udp_port_filter_hdr) +
	    arg->nrFilters * sizeof(struct wsm_udp_port_filter);
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_UDPPORT_DATAFRAME_FILTER,
			     arg, size, if_id);
}

/* Undocumented MIBs: */
/* 4.35 P2PDeviceInfo */
#define D11_MAX_SSID_LEN		(32)

struct wsm_p2p_device_type {
	__le16 categoryId;
	u8 oui[4];
	__le16 subCategoryId;
} __packed;

struct wsm_p2p_device_info {
	struct wsm_p2p_device_type primaryDevice;
	u8 reserved1[3];
	u8 devNameSize;
	u8 localDevName[D11_MAX_SSID_LEN];
	u8 reserved2[3];
	u8 numSecDevSupported;
	struct wsm_p2p_device_type secondaryDevices[0];
} __packed;

/* 4.36 SetWCDMABand - WO */
struct wsm_cdma_band {
	u8 WCDMA_Band;
	u8 reserved[3];
} __packed;

/* 4.37 GroupTxSequenceCounter - RO */
struct wsm_group_tx_seq {
	__le32 bits_47_16;
	__le16 bits_15_00;
	__le16 reserved;
} __packed;

/* 4.39 SetHtProtection - WO */
#define WSM_DUAL_CTS_PROT_ENB		(1 << 0)
#define WSM_NON_GREENFIELD_STA		PRESENT(1 << 1)
#define WSM_HT_PROT_MODE__NO_PROT	(0 << 2)
#define WSM_HT_PROT_MODE__NON_MEMBER	(1 << 2)
#define WSM_HT_PROT_MODE__20_MHZ	(2 << 2)
#define WSM_HT_PROT_MODE__NON_HT_MIXED	(3 << 2)
#define WSM_LSIG_TXOP_PROT_FULL		(1 << 4)
#define WSM_LARGE_L_LENGTH_PROT		(1 << 5)

struct wsm_ht_protection {
	__le32 flags;
} __packed;

/* 4.40 GPIO Command - R/W */
#define WSM_GPIO_COMMAND_SETUP	0
#define WSM_GPIO_COMMAND_READ	1
#define WSM_GPIO_COMMAND_WRITE	2
#define WSM_GPIO_COMMAND_RESET	3
#define WSM_GPIO_ALL_PINS	0xFF

struct wsm_gpio_command {
	u8 GPIO_Command;
	u8 pin;
	__le16 config;
} __packed;

/* 4.41 TSFCounter - RO */
struct wsm_tsf_counter {
	__le64 TSF_Counter;
} __packed;

/* 4.43 Keep alive period */
struct wsm_keep_alive_period {
	__le16 keepAlivePeriod;
	u8 reserved[2];
} __packed;

static inline int wsm_keep_alive_period(struct rk960_common *hw_priv,
					int period, int if_id)
{
	struct wsm_keep_alive_period arg = {
		.keepAlivePeriod = __cpu_to_le16(period),
	};
	return wsm_write_mib(hw_priv, WSM_MIB_ID_KEEP_ALIVE_PERIOD,
			     &arg, sizeof(arg), if_id);
};

/* BSSID filtering */
struct wsm_set_bssid_filtering {
	u8 filter;
	u8 reserved[3];
} __packed;

static inline int wsm_set_bssid_filtering(struct rk960_common *hw_priv,
					  bool enabled, int if_id)
{
	struct wsm_set_bssid_filtering arg = {
		.filter = !enabled,
	};
	return wsm_write_mib(hw_priv, WSM_MIB_ID_DISABLE_BSSID_FILTER,
			     &arg, sizeof(arg), if_id);
}

/* Multicat filtering - 4.5 */
struct wsm_multicast_filter {
	__le32 enable;
	__le32 numOfAddresses;
	u8 macAddress[WSM_MAX_GRP_ADDRTABLE_ENTRIES][ETH_ALEN];
} __packed;

/* Mac Addr Filter Info */
struct wsm_mac_addr_info {
	u8 filter_mode;
	u8 address_mode;
	u8 MacAddr[6];
} __packed;

/* Mac Addr Filter */
struct wsm_mac_addr_filter {
	u8 numfilter;
	u8 action_mode;
	u8 Reserved[2];
	struct wsm_mac_addr_info macaddrfilter[0];
} __packed;

/* Broadcast Addr Filter */
struct wsm_broadcast_addr_filter {
	u8 action_mode;
	u8 nummacaddr;
	u8 filter_mode;
	u8 address_mode;
	u8 MacAddr[6];
} __packed;

static inline int wsm_set_multicast_filter(struct rk960_common *hw_priv,
					   struct wsm_multicast_filter *fp,
					   int if_id)
{
	return wsm_write_mib(hw_priv, WSM_MIB_ID_DOT11_GROUP_ADDRESSES_TABLE,
			     fp, sizeof(*fp), if_id);
}

#define WSM_MAX_IPV4_ADDRESS_FILTER_ELEMENTS  8
#define WSM_MAX_IPV6_ADDRESS_FILTER_ELEMENTS  4
#define MIB_IP_DATA_FRAME_ADDRESSMODE_SRC       0x01
#define MIB_IP_DATA_FRAME_ADDRESSMODE_DEST      0x02
#define MIB_DATA_FRAME_FILTERMODE_DISABLED      0
#define MIB_DATA_FRAME_FILTERMODE_FILTEROUT     1
#define MIB_DATA_FRAME_FILTERMODE_FILTERIN      2

/* ARP IPv4 filtering - 4.10 */
struct wsm_arp_ipv4_filter {
	__le32 enable;
	__be32 ipv4Address[WSM_MAX_ARP_IP_ADDRTABLE_ENTRIES];
} __packed;

#ifdef IPV6_FILTERING
/* NDP IPv6 filtering */
struct wsm_ndp_ipv6_filter {
	__le32 enable;
	struct in6_addr ipv6Address[WSM_MAX_NDP_IP_ADDRTABLE_ENTRIES];
} __packed;
/* IPV6 Addr Filter Info */
struct wsm_ip6_addr_info {
	u8 filter_mode;
	u8 address_mode;
	u8 Reserved[2];
	u8 ipv6[16];
};

/* IPV6 Addr Filter */
struct wsm_ipv6_filter {
	u8 numfilter;
	u8 action_mode;
	u8 Reserved[2];
	struct wsm_ip6_addr_info ipv6filter[WSM_MAX_IPV6_ADDRESS_FILTER_ELEMENTS];
} __packed;
#endif /*IPV6_FILTERING */

/* IPV4 Addr Filter Info */
struct wsm_ip4_addr_info {
	u8 filter_mode;
	u8 address_mode;
	u8 Reserved[2];
	u8 ipv4[4];
};

/* IPV4 Addr Filter */
struct wsm_ipv4_filter {
	u8 numfilter;
	u8 action_mode;
	u8 Reserved[2];
	struct wsm_ip4_addr_info ipv4filter[WSM_MAX_IPV4_ADDRESS_FILTER_ELEMENTS];
} __packed;

static inline int wsm_set_arp_ipv4_filter(struct rk960_common *hw_priv,
					  struct wsm_arp_ipv4_filter *fp,
					  int if_id)
{
	return wsm_write_mib(hw_priv, WSM_MIB_ID_ARP_IP_ADDRESSES_TABLE,
			     fp, sizeof(*fp), if_id);
}

#ifdef IPV6_FILTERING
static inline int wsm_set_ndp_ipv6_filter(struct rk960_common *priv,
					  struct wsm_ndp_ipv6_filter *fp,
					  int if_id)
{
	return wsm_write_mib(priv, WSM_MIB_ID_NS_IP_ADDRESSES_TABLE,
			     fp, sizeof(*fp), if_id);
}
#endif /*IPV6_FILTERING */

/* P2P Power Save Mode Info - 4.31 */
struct wsm_p2p_ps_modeinfo {
	u8 oppPsCTWindow;
	u8 count;
	u8 reserved;
	u8 dtimCount;
	__le32 duration;
	__le32 interval;
	__le32 startTime;
} __packed;

static inline int wsm_set_p2p_ps_modeinfo(struct rk960_common *hw_priv,
					  struct wsm_p2p_ps_modeinfo *mi,
					  int if_id)
{
	return wsm_write_mib(hw_priv, WSM_MIB_ID_P2P_PS_MODE_INFO,
			     mi, sizeof(*mi), if_id);
}

static inline int wsm_get_p2p_ps_modeinfo(struct rk960_common *hw_priv,
					  struct wsm_p2p_ps_modeinfo *mi)
{
	return wsm_read_mib(hw_priv, WSM_MIB_ID_P2P_PS_MODE_INFO,
			    mi, sizeof(*mi));
}

/* UseMultiTxConfMessage */

static inline int wsm_use_multi_tx_conf(struct rk960_common *hw_priv,
					bool enabled, int if_id)
{
	__le32 arg = enabled ? __cpu_to_le32(1) : 0;

	return wsm_write_mib(hw_priv, WSM_MIB_USE_MULTI_TX_CONF,
			     &arg, sizeof(arg), if_id);
}

/* 4.26 SetUpasdInformation */
struct wsm_uapsd_info {
	__le16 uapsdFlags;
	__le16 minAutoTriggerInterval;
	__le16 maxAutoTriggerInterval;
	__le16 autoTriggerStep;
};

static inline int wsm_set_uapsd_info(struct rk960_common *hw_priv,
				     struct wsm_uapsd_info *arg, int if_id)
{
	/* TODO:COMBO:UAPSD will be supported only on one interface */
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_UAPSD_INFORMATION,
			     arg, sizeof(*arg), if_id);
}

/* 4.22 OverrideInternalTxRate */
struct wsm_override_internal_txrate {
	u8 internalTxRate;
	u8 nonErpInternalTxRate;
	u8 reserved[2];
} __packed;

static inline int
wsm_set_override_internal_txrate(struct rk960_common *hw_priv,
				 struct wsm_override_internal_txrate *arg,
				 int if_id)
{
	return wsm_write_mib(hw_priv, WSM_MIB_ID_OVERRIDE_INTERNAL_TX_RATE,
			     arg, sizeof(*arg), if_id);
}

#ifdef MCAST_FWDING
/* 4.51 SetForwardingOffload */
struct wsm_forwarding_offload {
	u8 fwenable;
	u8 flags;
	u8 reserved[2];
} __packed;

static inline int wsm_set_forwarding_offlad(struct rk960_common *hw_priv,
					    struct wsm_forwarding_offload *arg,
					    int if_id)
{
	return wsm_write_mib(hw_priv, WSM_MIB_ID_FORWARDING_OFFLOAD,
			     arg, sizeof(*arg), if_id);
}
#endif

/* ******************************************************************** */
/* WSM TX port control							*/

void wsm_lock_tx(struct rk960_common *hw_priv);
void wsm_vif_lock_tx(struct rk960_vif *priv);
void wsm_lock_tx_async(struct rk960_common *hw_priv);
bool wsm_flush_tx(struct rk960_common *hw_priv);
bool wsm_vif_flush_tx(struct rk960_vif *priv);
void wsm_unlock_tx(struct rk960_common *hw_priv);
void wsm_lock_tx_rec(struct rk960_common *hw_priv);
void wsm_unlock_tx_rec(struct rk960_common *hw_priv);

/* ******************************************************************** */
/* WSM / BH API								*/

int wsm_handle_exception(struct rk960_common *hw_priv, u8 * data, size_t len);
int wsm_handle_rx(struct rk960_common *hw_priv, int id, struct wsm_hdr *wsm,
		  struct sk_buff **skb_p);

/* ******************************************************************** */
/* wsm_buf API								*/

struct wsm_buf {
	u8 *begin;
	u8 *data;
	u8 *end;
};

void wsm_buf_init(struct wsm_buf *buf);
void wsm_buf_deinit(struct wsm_buf *buf);

/* ******************************************************************** */
/* wsm_cmd API								*/

struct wsm_cmd {
	spinlock_t lock;
	int done;
	u8 *ptr;
	size_t len;
	void *arg;
	int ret;
	u16 cmd;
};

/* ******************************************************************** */
/* WSM TX buffer access							*/

int wsm_get_tx(struct rk960_common *hw_priv, u8 ** data,
	       size_t * tx_len, int *burst, int *vif_selected);
void wsm_txed(struct rk960_common *hw_priv, u8 * data);

/* ******************************************************************** */
/* Queue mapping: WSM <---> linux					*/
/* Linux: VO VI BE BK							*/
/* WSM:   BE BK VI VO							*/

static inline u8 wsm_queue_id_to_linux(u8 queueId)
{
	static const u8 queue_mapping[] = {
		2, 3, 1, 0
	};
	return queue_mapping[queueId];
}

static inline u8 wsm_queue_id_to_wsm(u8 queueId)
{
	static const u8 queue_mapping[] = {
		3, 2, 0, 1
	};
	return queue_mapping[queueId];
}

char *wsm_conv_req_resp_to_str(int id);
int wsm_set_pm_indication(struct rk960_common *hw_priv, struct wsm_buf *buf);

void wsm_sended_cmds_init(struct rk960_common *hw_priv);
void wsm_sended_cmds_deinit(struct rk960_common *hw_priv);
int wsm_send_fw_recovery_cmd(struct rk960_common *hw_priv);
int rk960_pending_tx_confirm(struct rk960_common *hw_priv,
			     int if_id, u32 packetID);
int rk960_pending_scan_complete_indication(struct rk960_common *hw_priv);

#endif /* RK960_HWIO_H_INCLUDED */
