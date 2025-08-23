/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef RK960_H
#define RK960_H

#include <linux/version.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/atomic.h>
#include <net/mac80211.h>

#define RK960_VERSION   "svn5899.huwg 2022-11-01 18:01:08 +0800 (Tue, 01 Nov 2022)"

#ifdef P2P_MULTIVIF
#define RK960_MAX_VIFS			(3)
#else
#define RK960_MAX_VIFS			(2)
#endif
#define RK960_GENERIC_IF_ID		(2)
#define RK960_HOST_VIF0_11N_THROTTLE	(RK960_MAX_QUEUE_SZ/2 - 1)
#define RK960_HOST_VIF1_11N_THROTTLE	(RK960_MAX_QUEUE_SZ/2 - 1)
#define RK960_HOST_VIF0_11BG_THROTTLE	(RK960_MAX_QUEUE_SZ/2 - 1)
#define RK960_HOST_VIF1_11BG_THROTTLE	(RK960_MAX_QUEUE_SZ/2 - 1)
#if 0
#define RK960_FW_VIF0_THROTTLE		(15)
#define RK960_FW_VIF1_THROTTLE		(15)
#endif
#define RK960_MAX_QUEUE_SZ		(256)

#define RK960_MAX_TX_COUNT     (15)

#define IEEE80211_FCTL_WEP      0x4000
#define IEEE80211_QOS_DATAGRP   0x0080
#define WSM_KEY_MAX_IDX		20

#define RK960_VIF_LIST_USE_RCU_LOCK
//#define RK960_USE_RX_THREAD

#include "queue.h"
#include "wsm.h"
#include "scan.h"
#include "txrx.h"
#include "ht.h"
#include "pm.h"
#include "fwio.h"
#ifdef CONFIG_RK960_TESTMODE
#include "rk960_nl80211_testmode_msg_copy.h"
#endif /*CONFIG_RK960_TESTMODE */

/* extern */ struct hwbus_ops;
/* extern */ struct task_struct;
/* extern */ struct rk960_debug_priv;
/* extern */ struct rk960_debug_common;
/* extern */ struct firmware;

/* #define ROC_DEBUG */

#define RK960_DEVICE_ID         0x5347
#define RK960_DEVICE_ID_C       0x5348
#define RK960_DEVICE_ID_D       0x5349
#define RK960_MANUFACTURER      0x296

/* hidden ssid is only supported when separate probe resp IE
   configuration is supported */
#ifdef PROBE_RESP_EXTRA_IE
#define HIDDEN_SSID	1
#endif

#define RK960_MAX_CTRL_FRAME_LEN	(0x1000)

#define WLAN_LINK_ID_MAX		(RK960_MAX_STA_IN_AP_MODE + 3)

#define RK960_MAX_STA_IN_AP_MODE	(5)
#define RK960_MAX_REQUEUE_ATTEMPTS	(5)
#define RK960_LINK_ID_UNMAPPED		(15)

#define RK960_MAX_TID			(8)

#define RK960_TX_BLOCK_ACK_ENABLED_FOR_ALL_TID         (0xFF)
#define RK960_RX_BLOCK_ACK_ENABLED_FOR_ALL_TID         (0xFF)
#define RK960_RX_BLOCK_ACK_ENABLED_FOR_BE_TID \
	(RK960_TX_BLOCK_ACK_ENABLED_FOR_ALL_TID & 0x01)
#define RK960_TX_BLOCK_ACK_DISABLED_FOR_ALL_TID	(0)
#define RK960_RX_BLOCK_ACK_DISABLED_FOR_ALL_TID	(0)

#define RK960_BLOCK_ACK_CNT		(30)
#define RK960_BLOCK_ACK_THLD		(800)
#define RK960_BLOCK_ACK_HIST		(3)
#define RK960_BLOCK_ACK_INTERVAL	(1 * HZ / RK960_BLOCK_ACK_HIST)
#define RK960_ALL_IFS			(-1)
#ifdef ROAM_OFFLOAD
#define RK960_SCAN_TYPE_ACTIVE 0x1000
#define RK960_SCAN_BAND_5G 0X2000
#endif /*ROAM_OFFLOAD */

#define IEEE80211_FCTL_WEP      0x4000
#define IEEE80211_QOS_DATAGRP   0x0080
#ifdef CONFIG_RK960_TESTMODE
#define RK960_SCAN_MEASUREMENT_PASSIVE (0)
#define RK960_SCAN_MEASUREMENT_ACTIVE  (1)
#endif

#define RK960_SCAN_TXRX_BURST_THRESHOLD    100

#define RK960_CSYNC_ADJUST
#ifdef RK960_CSYNC_ADJUST
/*
 * use latest e.g. 100 rx rssi within 1 seconds to calcute average rssi
 * use average rssi to calcute phy csync thresh
 */
#define RK960_CSYNC_ADJUST_INTERVAL    (1 * HZ)
#define RK960_CSYNC_MAX_RSSI_SAMPLE 100
#define RK960_CSYNC_MIN_RSSI_SAMPLE 10

#define RK960_CSYNC_THRESH_MIN	110
#define RK960_CSYNC_THRESH_MAX	190

#define RK960_CSYNC_THRESH_OFFSET 50	// means set the csync thresholds 25 dB below the true RSSI
								// half-dB  steps  (e.g.,  60  means  30  dBm)
#define RK960_CSYNC_THRESH_EXPONENT 4

#define RK960_CSYNC_SETED_THRESH_COUNT	60

#define RK960_CSYNC_DEFAULT_THRESH 0
#define RK960_CSYNC_DEFAULT_SCAN   2

struct csync_params {
	int csync_disable;

	// save rx frame rssi samples
	s8 sam_history[RK960_MAX_VIFS][RK960_CSYNC_MAX_RSSI_SAMPLE];
	int sam_read[RK960_MAX_VIFS];
	int sam_write[RK960_MAX_VIFS];
	int sam_size[RK960_MAX_VIFS];
	int last_sam_size[RK960_MAX_VIFS];

	unsigned int thresh_accum[RK960_MAX_VIFS];
	unsigned int avg_thresh[RK960_MAX_VIFS];
	unsigned int new_thresh[RK960_MAX_VIFS];	// calc from rssi

	unsigned int cur_seted_thresh;	// current seted csync thresh
	unsigned int restore_thresh;	// restore csync thresh
	s8 to_fw_csync;

	// history of seted csync thresh        
	unsigned int thr_history[RK960_CSYNC_SETED_THRESH_COUNT];
	int cur_thr_offset;
	int last_thresh;

	int timer_hit;
	int set_hit;
};
#endif

/* actaul tx power = 
    RK960_DEFAULT_TX_POWER * 10
    for exampel
    20 * 10 = 200 = 0xC8 (Q12.4) = 12.5db */
#define RK960_DEFAULT_TX_POWER 32	//20

#ifdef MCAST_FWDING
#define WSM_MAX_BUF		30
#endif

/* Please keep order */
enum rk960_join_status {
	RK960_JOIN_STATUS_PASSIVE = 0,
	RK960_JOIN_STATUS_MONITOR,
	RK960_JOIN_STATUS_STA,
	RK960_JOIN_STATUS_AP,
};

enum rk960_link_status {
	RK960_LINK_OFF,
	RK960_LINK_RESERVE,
	RK960_LINK_SOFT,
	RK960_LINK_HARD,
#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
	RK960_LINK_RESET,
	RK960_LINK_RESET_REMAP,
#endif
};

enum rk960_bss_loss_status {
	RK960_BSS_LOSS_NONE,
	RK960_BSS_LOSS_CHECKING,
	RK960_BSS_LOSS_CONFIRMING,
	RK960_BSS_LOSS_CONFIRMED,
};

struct rk960_link_entry {
	unsigned long timestamp;
	enum rk960_link_status status;
#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
	enum rk960_link_status prev_status;
#endif
	u8 mac[ETH_ALEN];
	u8 buffered[RK960_MAX_TID];
	struct sk_buff_head rx_queue;
};

#if defined(ROAM_OFFLOAD) || defined(CONFIG_RK960_TESTMODE)
struct rk960_testframe {
	u8 len;
	u8 *data;
};
#endif
#ifdef CONFIG_RK960_TESTMODE
struct advance_scan_elems {
	u8 scanMode;
	u16 duration;
};
/**
 * rk960_tsm_info - Keeps information about ongoing TSM collection
 * @ac: Access category for which metrics to be collected
 * @use_rx_roaming: Use received voice packets to compute roam delay
 * @sta_associated: Set to 1 after association
 * @sta_roamed: Set to 1 after successful roaming
 * @roam_delay: Roam delay
 * @rx_timestamp_vo: Timestamp of received voice packet
 * @txconf_timestamp_vo: Timestamp of received tx confirmation for
 * successfully transmitted VO packet
 * @sum_pkt_q_delay: Sum of packet queue delay
 * @sum_media_delay: Sum of media delay
 *
 */
struct rk960_tsm_info {
	u8 ac;
	u8 use_rx_roaming;
	u8 sta_associated;
	u8 sta_roamed;
	u16 roam_delay;
	u32 rx_timestamp_vo;
	u32 txconf_timestamp_vo;
	u32 sum_pkt_q_delay;
	u32 sum_media_delay;
};

/**
 * rk960_start_stop_tsm - To start or stop collecting TSM metrics in
 * rk960 driver
 * @start: To start or stop collecting TSM metrics
 * @up: up for which metrics to be collected
 * @packetization_delay: Packetization delay for this TID
 *
 */
struct rk960_start_stop_tsm {
	u8 start;		/*1: To start, 0: To stop */
	u8 up;
	u16 packetization_delay;
};
#endif /* CONFIG_RK960_TESTMODE */

#define RK960_FW_ERROR_RECOVERY
#define RK960_FWERR_REASON_TEST         0
#define RK960_FWERR_REASON_TX_CONF      1
#define RK960_FWERR_REASON_SDIO         2
#define RK960_FWERR_REASON_CMD          3
#define RK960_FWERR_REASON_FLUSH        4

#define RK960_FWERR_SIZE        128

void rk960_signal_fw_error(struct rk960_common *hw_priv, int reason);

struct wsm_com_cmd_s {
	u16 cmd;
	void *buf;
	int if_id;
	int valid;
	size_t buf_size;
	u16 mibId;
	u8 qid;
	void *args;
	struct list_head list;
};

struct rk960_sended_wsm_cmds_s {
	struct wsm_com_cmd_s wsm_wmib_cmd[2][128];
	struct wsm_com_cmd_s wsm_wmib_tem_fra_cmd[2][8];
	struct wsm_com_cmd_s wsm_wmib_tx_pol_cmd[2];
	struct wsm_com_cmd_s wsm_configuration_cmd[2];
	struct wsm_com_cmd_s wsm_join_cmd[2];
	struct wsm_com_cmd_s wsm_set_bss_params_cmd[2];
	struct wsm_com_cmd_s wsm_add_key_cmd[WSM_KEY_MAX_IDX];
	struct wsm_com_cmd_s wsm_set_tx_queue_params_cmd[2];
	struct wsm_com_cmd_s wsm_set_edca_params_cmd[2];
	struct wsm_com_cmd_s wsm_set_pm_cmd[2];
	struct wsm_com_cmd_s wsm_start_cmd[2];
	struct wsm_com_cmd_s wsm_map_link_cmd[2][WLAN_LINK_ID_MAX];
	struct wsm_com_cmd_s wsm_update_ie_cmd[2][3];
};

/* fast wifi connect recovery */
#define RK960_FWCR_BCN_CNT      128     // beacon, probe resp
#define RK960_FWCR_AUTH_CNT     8       // auth, assoc resp, 4-way;
#define RK960_FWCR_GROUP_CNT    4

#define RK960_FWCR_FRAME_TYPE_BEACON            BIT(0)
#define RK960_FWCR_FRAME_TYPE_PROBE_RESP        BIT(1)
#define RK960_FWCR_FRAME_TYPE_AUTH              BIT(2)
#define RK960_FWCR_FRAME_TYPE_ASSOC             BIT(3)
#define RK960_FWCR_FRAME_TYPE_1_4WAY            BIT(4)
#define RK960_FWCR_FRAME_TYPE_3_4WAY            BIT(5)
#define RK960_FWCR_FRAME_TYPE_2_4WAY            BIT(6)
#define RK960_FWCR_FRAME_TYPE_4_4WAY            BIT(7)
#define RK960_FWCR_FRAME_TYPE_1_2GRO            BIT(8)

#ifdef SUPPORT_FWCR
#define RK960_FWCR_FRAME_SAVE_PATH      "/data/rk960_fwcr.bin"

struct rk960_fwcr_frame_s {
        u16 type;
        u16 frame_len;
        struct wsm_rx *arg;
        u8 *frame;
};
#endif

struct rk960_common {
	struct rk960_debug_common *debug;
	struct rk960_queue tx_queue[4];
	struct rk960_queue_stats tx_queue_stats;

	struct ieee80211_hw *hw;
	struct mac_address addresses[RK960_MAX_VIFS];

	/*Will be a pointer to a list of VIFs - Dynamically allocated */
#ifdef RK960_VIF_LIST_USE_RCU_LOCK
	struct ieee80211_vif __rcu *vif_list[RK960_MAX_VIFS];
#else
	struct ieee80211_vif *vif_list[RK960_MAX_VIFS];
#endif
	atomic_t num_vifs;
	int def_vif_id;
	spinlock_t vif_list_lock;
	u32 if_id_slot;
	struct device *pdev;
	struct workqueue_struct *workqueue;

	struct mutex conf_mutex;

	/* Hardware interface */
	const struct hwbus_ops *hwbus_ops;
	struct hwbus_priv *hwbus_priv;
	u8 *hwbus_rcvbuf;
	int rcvbuf_offset;
	u8 *hwbus_sndbuf;
	int sndbuf_offset;

	/* HW/FW type (HIF_...) */
	int hw_type;
	int hw_revision;
	int fw_revision;

	/* chip id */
	unsigned int chip_id;

	/* firmware/hardware info */
	unsigned int tx_hdr_len;

	/* Radio data */
	int output_power;
	int fw_output_power;
	int noise;

	/* calibration, output power limit and rssi<->dBm conversation data */

	/* BBP/MAC state */
	const struct firmware *sdd;
	struct ieee80211_rate *rates;
	struct ieee80211_rate *mcs_rates;
	u8 mac_addr[ETH_ALEN];
	/*TODO:COMBO: To be made per VIFF after mac80211 support */
        int last_channel;
	struct ieee80211_channel *channel;
	struct ieee80211_channel *ro_channel;
	int channel_switch_in_progress;
	wait_queue_head_t channel_switch_done;
	u8 long_frame_max_tx_count;
	u8 short_frame_max_tx_count;
	/* TODO:COMBO: According to Hong aggregation will happen per VIFF.
	 * Keeping in common structure for the time being. Will be moved to VIFF
	 * after the mechanism is clear */
	u8 ba_tid_mask;
	int ba_acc;		/*TODO: Same as above */
	int ba_cnt;		/*TODO: Same as above */
	int ba_cnt_rx;		/*TODO: Same as above */
	int ba_acc_rx;		/*TODO: Same as above */
	int ba_hist;		/*TODO: Same as above */
	struct timer_list ba_timer;	/*TODO: Same as above */
	spinlock_t ba_lock;	/*TODO: Same as above */
	bool ba_ena;		/*TODO: Same as above */
	struct work_struct ba_work;	/*TODO: Same as above */
	struct rk960_pm_state pm_state;
	bool is_BT_Present;
	bool is_go_thru_go_neg;
	u8 conf_listen_interval;

	/* BH */
	atomic_t bh_rx;
	atomic_t bh_tx;
	atomic_t bh_term;
	atomic_t bh_suspend;
	struct task_struct *bh_thread;
#ifdef RK960_USE_RX_THREAD
	struct task_struct *rx_thread;
	struct sk_buff_head rxq;
	atomic_t bh_rx_rx;
	atomic_t bh_rx_term;
	wait_queue_head_t bh_rx_wq;
#endif
	int bh_error;
	wait_queue_head_t bh_wq;
	wait_queue_head_t bh_evt_wq;
	int buf_id_tx;		/* byte */
	int buf_id_rx;		/* byte */
	int wsm_rx_seq;		/* byte */
	int wsm_tx_seq;		/* byte */
	int hw_bufs_used;
	int hw_bufs_used_vif[RK960_MAX_VIFS];
	spinlock_t hw_bufs_used_lock;
	struct sk_buff *skb_cache;
	bool device_can_sleep;
	bool device_wakeing;
	/* Keep rk960 awake (WUP = 1) 1 second after each scan to avoid
	 * FW issue with sleeping/waking up. */
	atomic_t recent_scan;
	atomic_t msg_idx;
	atomic_t evt_idx;

	/* WSM */
	struct wsm_caps wsm_caps;
	struct mutex wsm_cmd_mux;
	struct wsm_buf wsm_cmd_buf;
	struct wsm_cmd wsm_cmd;
	wait_queue_head_t wsm_cmd_wq;
	wait_queue_head_t wsm_startup_done;
	struct wsm_cbc wsm_cbc;
	int tx_pending;
	atomic_t tx_lock;
	u32 pending_frame_id;
#ifdef CONFIG_RK960_TESTMODE
	/* Device Power Range */
	struct wsm_tx_power_range txPowerRange[2];
	/* Advance Scan */
	struct advance_scan_elems advanceScanElems;
	bool enable_advance_scan;
	struct delayed_work advance_scan_timeout;
#endif				/* CONFIG_RK960_TESTMODE */

	/* WSM debug */
	int wsm_enable_wsm_dumps;
	u32 wsm_dump_max_size;

	/* Scan status */
	struct rk960_scan scan;

	/* TX/RX */
	unsigned long rx_timestamp;

	/* Scan Timestamp */
	unsigned long scan_timestamp;
	u32 scan_counter;
	u32 scan_comp_counter;

	/* WSM events */
	spinlock_t event_queue_lock;
	struct list_head event_queue;
	struct work_struct event_handler;

	/* TX rate policy cache */
	struct tx_policy_cache tx_policy_cache;
	struct work_struct tx_policy_upload_work;

	/* cryptographic engine information */

	/* bit field of glowing LEDs */
	u16 softled_state;

	/* statistics */
	struct ieee80211_low_level_stats stats;

	struct rk960_ht_info ht_info;
	int tx_burst_idx;

	struct ieee80211_iface_limit if_limits1[2];
	struct ieee80211_iface_limit if_limits2[2];
	struct ieee80211_iface_limit if_limits3[2];
	struct ieee80211_iface_combination if_combs[3];

	struct mutex wsm_oper_lock;
	struct delayed_work rem_chan_timeout;
	atomic_t remain_on_channel;
	atomic_t cancel_roc;
	unsigned long roc_start_time;
	unsigned long roc_duration;
	int roc_if_id;
	int roc_if_id_last;
	u64 roc_cookie;
	wait_queue_head_t offchannel_wq;
	u16 offchannel_done;
	u16 prev_channel;
	int if_id_selected;
	u32 key_map;
	struct wsm_add_key keys[WSM_KEY_MAX_INDEX + 1];
#ifdef MCAST_FWDING
	struct wsm_buf wsm_release_buf[WSM_MAX_BUF];
	u8 buf_released;
#endif
#ifdef ROAM_OFFLOAD
	u8 auto_scanning;
	u8 frame_rcvd;
	u8 num_scanchannels;
	u8 num_2g_channels;
	u8 num_5g_channels;
	struct wsm_scan_ch scan_channels[48];
	struct sk_buff *beacon;
	struct sk_buff *beacon_bkp;
	struct rk960_testframe testframe;
#endif				/*ROAM_OFFLOAD */
#ifdef CONFIG_RK960_TESTMODE
	struct rk960_testframe test_frame;
	struct ste_tsm_stats tsm_stats;
	struct rk960_tsm_info tsm_info;
	spinlock_t tsm_lock;
	struct rk960_start_stop_tsm start_stop_tsm;
#endif				/* CONFIG_RK960_TESTMODE */
	u8 connected_sta_cnt;
	u16 vif0_throttle;
	u16 vif1_throttle;
	int delayed_unjoin_if_id;

	/* legacy PS mode switch in suspend */
	int ps_mode_switch_in_progress;
	wait_queue_head_t ps_mode_switch_done;

#ifdef RK960_CSYNC_ADJUST
	struct timer_list csync_timer;
	struct work_struct csync_work;
	struct csync_params csync_params;
	spinlock_t csync_lock;
	s8 fw_csync;
	atomic_t rx_counter;
	int last_rx_counter;
	atomic_t tx_counter;
	int last_tx_counter;
#endif

	struct firmware_info firmware;

	bool last_rx_is_nonprotect_pae;
	u8 igtk_key_index;

	int wow_suspend_done;
	bool wowlan_disable;

	s8 txpwr_comp_tbl[14][11];	// 11 rates, 14 channel

	/* debug dump info */
	u32 mem_rw_addr;
	u32 mem_rw_len;
	int debug_dump_ready;
	wait_queue_head_t debug_dump_done;
	u8 *debug_dump_data;
	u8 *debug_dump_data_str;
	int debug_adc_size;
	int debug_adc_offset;
	int debug_adc_block;
	int sdio_tx_tp_start;
	int rftest_scan;

	struct list_head wsm_cmds_list;
	struct mutex wsm_cmd_save_mux;
	struct rk960_sended_wsm_cmds_s wsm_cmds;
	struct wsm_set_tx_rate_retry_policy wsm_tx_rate;
	struct work_struct fw_err_work;
	int fw_error_enable;
	int fw_error_counter;
	int fw_error_processing;
	u8 fw_error_reason[RK960_FWERR_SIZE];
	struct wsm_rec_info wsm_rec_info;
	unsigned long ms_max_tx_conf_time;
	int max_tx_conf_set;
	int max_tx_conf_queueid;
	int max_tx_conf_status;
	int max_tx_conf_txedrate;
	int max_tx_conf_ackfailures;
	int max_tx_conf_mediadelay;
	int max_tx_conf_txQueuedelay;

	u8 efuse[RK960_EFUSE_SIZE];
	u8 wifi_efuse_mac_addr[6];
	u8 bt_efuse_mac_addr[6];
        unsigned char vif_macs[2][ETH_ALEN];

        int rts_threshold;

#ifdef SUPPORT_FWCR
        struct mutex fwcr_mux;
        int fwcr_bcn_cnt;
        int fwcr_4way_set;
        int fwcr_recovery;
        struct rk960_fwcr_frame_s fwcr_bcn[RK960_FWCR_BCN_CNT];
        struct rk960_fwcr_frame_s fwcr_auth[RK960_FWCR_AUTH_CNT];
        struct rk960_fwcr_frame_s fwcr_group[RK960_FWCR_AUTH_CNT+1];
        struct work_struct fwcr_work;
        int fwcr_fw_resumed;
        wait_queue_head_t fwcr_resume_done;
        u8 fwcr_bssid[6];
        int fwcr_update_key;
        u32 fwcr_key_map;
        struct wsm_add_key fwcr_keys[WSM_KEY_MAX_INDEX + 1];
#endif
        int fw_hotboot;
        int tx_fixed_rate;

        struct wsm_set_tx_agg_control tx_agg_ctrl;

        struct io_cmd *fw_scmd;
        int sdio_cmd_addr;

        int dtim_interval;
        int listen_interval;        
};

/* Virtual Interface State. One copy per VIF */
struct rk960_vif {
	atomic_t enabled;
	spinlock_t vif_lock;
	int if_id;
	/*TODO: Split into Common and VIF parts */
	struct rk960_debug_priv *debug;
	/* BBP/MAC state */
	u8 bssid[ETH_ALEN];
	struct wsm_edca_params edca;
	struct wsm_tx_queue_params tx_queue_params;
	struct wsm_association_mode association_mode;
	struct wsm_set_bss_params bss_params;
	struct wsm_set_pm powersave_mode;
	struct wsm_set_pm firmware_ps_mode;
	int power_set_true;
	int user_power_set_true;
	u8 user_pm_mode;
	int cqm_rssi_thold;
	unsigned cqm_rssi_hyst;
	unsigned cqm_tx_failure_thold;
	unsigned cqm_tx_failure_count;
	bool cqm_use_rssi;
	int cqm_link_loss_count;
	int cqm_beacon_loss_count;
	int mode;
	bool enable_beacon;
	int beacon_int;
	size_t ssid_length;
	u8 ssid[IEEE80211_MAX_SSID_LEN];
#ifdef HIDDEN_SSID
	bool hidden_ssid;
#endif
	bool listening;
	struct wsm_rx_filter rx_filter;
	struct wsm_beacon_filter_table bf_table;
	struct wsm_beacon_filter_control bf_control;
	struct wsm_multicast_filter multicast_filter;
	bool has_multicast_subscription;
	struct wsm_broadcast_addr_filter broadcast_filter;
	bool disable_beacon_filter;
	struct wsm_arp_ipv4_filter filter4;
#ifdef IPV6_FILTERING
	struct wsm_ndp_ipv6_filter filter6;
#endif				/*IPV6_FILTERING */
	struct work_struct update_filtering_work;
	struct work_struct set_beacon_wakeup_period_work;
	struct rk960_pm_state_vif pm_state_vif;
	/*TODO: Add support in mac80211 for psmode info per VIF */
	struct wsm_p2p_ps_modeinfo p2p_ps_modeinfo;
	struct wsm_uapsd_info uapsd_info;
	bool setbssparams_done;
	u32 listen_interval;
	u32 erp_info;
	bool powersave_enabled;

	/* WSM Join */
	enum rk960_join_status join_status;
	enum rk960_join_status join_status_restore;
	u8 join_bssid[ETH_ALEN];
	struct work_struct join_work;
	struct delayed_work join_timeout;
	struct work_struct unjoin_work;
	struct work_struct offchannel_work;
	int join_dtim_period;
	bool delayed_unjoin;
	int current_channel;
	struct work_struct channel_switch_work;

	/* saved template frame info */
	struct wsm_template_frame beacon_template[2];
	u8 beacon_template_frame[2][RK960_MAX_BEACON_SIZE];
	int beacon_template_len[2];

	/* Security */
	s8 wep_default_key_id;
	struct work_struct wep_key_work;
	unsigned long rx_timestamp;
	u32 cipherType;

	/* AP powersave */
	u32 link_id_map;
	u32 max_sta_ap_mode;
	u32 link_id_after_dtim;
	u32 link_id_uapsd;
	u32 link_id_max;
	u32 wsm_key_max_idx;
	struct rk960_link_entry link_id_db[RK960_MAX_STA_IN_AP_MODE];
	struct work_struct link_id_work;
	struct delayed_work link_id_gc_work;
	u32 sta_asleep_mask;
	u32 pspoll_mask;
	bool aid0_bit_set;
	spinlock_t ps_state_lock;
	bool buffered_multicasts;
	bool tx_multicast;
	struct work_struct set_tim_work;
	struct delayed_work set_cts_work;
	struct work_struct multicast_start_work;
	struct work_struct multicast_stop_work;
	struct timer_list mcast_timeout;

	/* CQM Implementation */
	struct delayed_work bss_loss_work;
	struct delayed_work connection_loss_work;
	struct work_struct tx_failure_work;
	int delayed_link_loss;
	spinlock_t bss_loss_lock;
	int bss_loss_status;
	int bss_loss_confirm_id;

	struct ieee80211_vif *vif;
	struct rk960_common *hw_priv;
	struct ieee80211_hw *hw;

	/* ROC implementation */
	struct delayed_work pending_offchanneltx_work;
#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
	/* Workaround for WFD testcase 6.1.10 */
	struct work_struct linkid_reset_work;
	u8 action_frame_sa[ETH_ALEN];
	u8 action_linkid;
#endif
	bool htcap;
#ifdef  AP_HT_CAP_UPDATE
	u16 ht_info;
	struct work_struct ht_info_update_work;
#endif
	bool havePMF;
	bool deauthAll;
	u8 tim_data[16];
	int tim_length;
};
struct rk960_sta_priv {
	int link_id;
	struct rk960_vif *priv;
};
enum rk960_data_filterid {
	IPV4ADDR_FILTER_ID = 0,
#ifdef IPV6_FILTERING
	IPV6ADDR_FILTER_ID,
#endif /*IPV6_FILTERING */
};

static inline
    struct rk960_common *rk960_vifpriv_to_hwpriv(struct rk960_vif *priv)
{
	return priv->hw_priv;
}

static inline
    struct rk960_vif *rk960_get_vif_from_ieee80211(struct ieee80211_vif *vif)
{
	return vif ? (struct rk960_vif *)vif->drv_priv : NULL;
}

static inline void rk960_hw_vif_read_lock(spinlock_t * lock)
{
#ifndef  RK960_VIF_LIST_USE_RCU_LOCK
	spin_lock_bh(lock);
#else
	BUG_ON(!lock);
	rcu_read_lock();
#endif
}

static inline void rk960_hw_vif_read_unlock(spinlock_t * lock)
{
#ifndef  RK960_VIF_LIST_USE_RCU_LOCK
	spin_unlock_bh(lock);
#else
	BUG_ON(!lock);
	rcu_read_unlock();
#endif
}

static inline void rk960_hw_vif_write_lock(spinlock_t * lock)
{
	spin_lock_bh(lock);
}

static inline void rk960_hw_vif_write_unlock(spinlock_t * lock)
{
	spin_unlock_bh(lock);
}

static inline void rk960_priv_vif_list_read_lock(spinlock_t * lock)
{
#ifndef  RK960_VIF_LIST_USE_RCU_LOCK
	spin_lock_bh(lock);
#else
	BUG_ON(!lock);
	rcu_read_lock();
#endif
}

static inline void rk960_priv_vif_list_read_unlock(spinlock_t * lock)
{
#ifndef  RK960_VIF_LIST_USE_RCU_LOCK
	spin_unlock_bh(lock);
#else
	BUG_ON(!lock);
	rcu_read_unlock();
#endif
}

static inline void rk960_priv_vif_list_write_lock(spinlock_t * lock)
{
#ifndef  RK960_VIF_LIST_USE_RCU_LOCK
	spin_lock_bh(lock);
#else
	BUG_ON(!lock);
#endif
}

static inline void rk960_priv_vif_list_write_unlock(spinlock_t * lock)
{
#ifndef  RK960_VIF_LIST_USE_RCU_LOCK
	spin_unlock_bh(lock);
#else
	BUG_ON(!lock);
#endif
}

#ifndef  RK960_VIF_LIST_USE_RCU_LOCK

#define RK960_HW_VIF_SET(vif_list,vif)			vif_list = vif
#define RK960_HW_VIF_GET(vif_list)				(vif_list)

static inline
    struct rk960_vif *rk960_hwpriv_to_vifpriv(struct rk960_common *hw_priv,
					      int if_id)
{
	struct rk960_vif *vif;

	if (WARN_ON((-1 == if_id) || (if_id > RK960_MAX_VIFS)))
		return NULL;
	/* TODO:COMBO: During scanning frames can be received
	 * on interface ID 3 */
	rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
	if (!hw_priv->vif_list[if_id]) {
		rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);
		return NULL;
	}

	vif = rk960_get_vif_from_ieee80211(hw_priv->vif_list[if_id]);
	WARN_ON(!vif);
	if (vif)
		rk960_priv_vif_list_read_lock(&vif->vif_lock);
	rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);
	return vif;
}

#else

#define RK960_HW_VIF_SET(vif_list,vif)			rcu_assign_pointer(vif_list,vif)
#define RK960_HW_VIF_GET(vif_list)				rcu_dereference(vif_list)

static inline
    struct rk960_vif *rk960_hwpriv_to_vifpriv(struct rk960_common *hw_priv,
					      int if_id)
{
	struct rk960_vif *priv;
	struct ieee80211_vif *vif;

	rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
	if (((-1 == if_id) || (if_id > RK960_MAX_VIFS))) {
		rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);
		return NULL;
	}

	vif = RK960_HW_VIF_GET(hw_priv->vif_list[if_id]);
	if (!vif) {
		rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);
		return NULL;
	}

	priv = rk960_get_vif_from_ieee80211(vif);
	WARN_ON(!priv);
	if (!priv)
		rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);
	return priv;
}

#endif

static inline
    struct rk960_vif *rk960_get_vif_from_ieee80211_hw_priv(struct rk960_common
							   *hw_priv, int i)
{
	struct ieee80211_vif *vif;

	vif = RK960_HW_VIF_GET(hw_priv->vif_list[i]);
	return vif ? (struct rk960_vif *)vif->drv_priv : NULL;
}

static inline
    struct rk960_vif *__rk960_hwpriv_to_vifpriv(struct rk960_common *hw_priv,
						int if_id)
{
	WARN_ON((-1 == if_id) || (if_id > RK960_MAX_VIFS));
	/* TODO:COMBO: During scanning frames can be received
	 * on interface ID 3 */
	if (!hw_priv->vif_list[if_id]) {
		return NULL;
	}

	return rk960_get_vif_from_ieee80211(hw_priv->vif_list[if_id]);
}

static inline
    struct rk960_vif *rk960_get_activevif(struct rk960_common *hw_priv)
{
	return rk960_hwpriv_to_vifpriv(hw_priv, ffs(hw_priv->if_id_slot) - 1);
}

static inline bool is_hardware_version1(struct rk960_common *hw_priv)
{
	return false;
}

static inline bool is_hardware_version2(struct rk960_common *hw_priv)
{
	return true;
}

static inline int rk960_get_nr_hw_ifaces(struct rk960_common *hw_priv)
{
	return 1;
}

#ifdef IPV6_FILTERING
/* IPV6 host addr info */
struct ipv6_addr_info {
	u8 filter_mode;
	u8 address_mode;
	u16 ipv6[8];
};
#endif /*IPV6_FILTERING */

/* interfaces for the drivers */
int rk960_core_probe(const struct hwbus_ops *hwbus_ops,
		     struct hwbus_priv *hwbus,
		     struct device *pdev,
		     int device_id,
		     struct rk960_common **core,
		     int ref_clk, const u8 * macaddr,
		     const char *sdd_path, bool have_5ghz);
void rk960_core_release(struct rk960_common *self);

static inline void rk960_tx_queues_lock(struct rk960_common *hw_priv)
{
	int i;
	for (i = 0; i < 4; ++i)
		rk960_queue_lock(&hw_priv->tx_queue[i]);
}

static inline void rk960_tx_queues_unlock(struct rk960_common *hw_priv)
{
	int i;
	for (i = 0; i < 4; ++i)
		rk960_queue_unlock(&hw_priv->tx_queue[i]);
}

/* Datastructure for LLC-SNAP HDR */
#define P80211_OUI_LEN  3

struct ieee80211_snap_hdr {
	u8 dsap;		/* always 0xAA */
	u8 ssap;		/* always 0xAA */
	u8 ctrl;		/* always 0x03 */
	u8 oui[P80211_OUI_LEN];	/* organizational universal id */
} __packed;

#define RK960_DBG_MSG		0x00000001
#define RK960_DBG_NIY		0x00000002
#define RK960_DBG_SBUS		0x00000004
#define RK960_DBG_INIT		0x00000008
#define RK960_DBG_ERROR	0x00000010
#define RK960_DBG_LEVEL	0xFFFFFFFF

#define rk960_for_each_vif(_hw_priv, _priv, _i)			\
for (									\
	_i = 0;								\
	_priv = hw_priv->vif_list[_i] ? 				\
	rk960_get_vif_from_ieee80211(hw_priv->vif_list[_i]) : NULL,	\
	_i < RK960_MAX_VIFS;						\
	_i++								\
)

#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
#define	IEEE80211_BAND_2GHZ NL80211_BAND_2GHZ
#define IEEE80211_BAND_5GHZ NL80211_BAND_5GHZ
#define IEEE80211_BAND_60GHZ NL80211_BAND_60GHZ
#define IEEE80211_NUM_BANDS (IEEE80211_BAND_60GHZ + 1)
#endif

/*
 * IEEE 802.11 address fields:
 * ToDS FromDS Addr1 Addr2 Addr3 Addr4
 *   0        0        DA    SA    BSSID n/a
 *   0        1        DA    BSSID SA    n/a
 *   1        0        BSSID SA    DA    n/a
 *   1        1        RA    TA    DA    SA
 */
static inline u8 *ieee80211_get_BSSID(struct ieee80211_hdr *hdr)
{
	if (ieee80211_has_a4(hdr->frame_control))
		return NULL;
	if (ieee80211_has_fromds(hdr->frame_control))
		return hdr->addr2;
	if (ieee80211_has_tods(hdr->frame_control))
		return hdr->addr1;
	return hdr->addr3;
}

#endif /* RK960_H */
