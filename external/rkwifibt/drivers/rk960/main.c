/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*Linux version 3.4.0 compilation*/
//#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
#include <linux/module.h>
//#endif
#include <linux/init.h>
#include <linux/firmware.h>
#include <linux/etherdevice.h>
#include <linux/vmalloc.h>
#include <linux/random.h>
#include <linux/sched.h>
#include <net/mac80211.h>
#include <../net/mac80211/ieee80211_i.h>

#include "rk960.h"
#include "txrx.h"
#include "hwbus.h"
#include "fwio.h"
#include "hwio.h"
#include "bh.h"
#include "sta.h"
#include "ap.h"
#include "scan.h"
#include "debug.h"
#include "pm.h"

MODULE_AUTHOR("Rockchips");
MODULE_DESCRIPTION("Rockchips RK960 Wireless Lan Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("rk960_core");

static int jtag_debug = 0;
module_param(jtag_debug, int, 0644);
MODULE_PARM_DESC(jtag_debug, "jtag debug");

static int sdio_width = 4;
module_param(sdio_width, int, 0644);
MODULE_PARM_DESC(sdio_width, "sdio width");

static int sgi = 0;
module_param(sgi, int, 0644);
MODULE_PARM_DESC(sgi, "short gi");

static int stbc = 0;
module_param(stbc, int, 0644);
MODULE_PARM_DESC(stbc, "stbc");

static int ht_support = 1;
module_param(ht_support, int, 0644);
MODULE_PARM_DESC(ht_support, "ht support");

static int mcs_mask = 0xFF;
module_param(mcs_mask, int, 0644);
MODULE_PARM_DESC(mcs_mask, "mcs mask");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
int rk960_vendor_init(struct wiphy *wiphy);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
#define vfs_write(fp, buf, len, pos) kernel_write(fp, buf, len, pos)
#define vfs_read(fp, buf, len, pos) kernel_read(fp, buf, len, pos)
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0) */

/* Accept MAC address of the form macaddr=0x00,0x80,0xE1,0x30,0x40,0x50 */
static u8 rk960_mac_templates[RK960_MAX_VIFS][ETH_ALEN] = {
#ifndef CUSTOM_FEATURE		/* To use macaddr of customers */
	{0x00, 0x80, 0xe1, 0x01, 0x02, 0x03},
	{0x00, 0x80, 0xe1, 0x01, 0x02, 0x01},
#else
	{0x00, 0x12, 0x34, 0x01, 0x02, 0x03},
	{0x00, 0x12, 0x34, 0x01, 0x02, 0x01},
#endif
};

module_param_array_named(macaddr, rk960_mac_templates[0], byte, NULL, S_IRUGO);
module_param_array_named(macaddr2, rk960_mac_templates[1], byte, NULL, S_IRUGO);
MODULE_PARM_DESC(macaddr, "First MAC address");
MODULE_PARM_DESC(macaddr2, "Second MAC address");
#ifdef CUSTOM_FEATURE		/* To use macaddr and PS Mode of customers */
#define PATH_WIFI_MACADDR		"/efs/wifi/.mac.info"
#define PATH_WIFI_MACADDR_TMP	"/data/.mac.info"
#define PATH_WIFI_PSM_INFO		"/data/.psm.info"
static int savedpsm = 0;
#endif

static u8 rk960_tx_max_agg_num_tbl[8] = {
        2, 4, 8, 12, 16, 16, 16, 16
};

/* TODO: use rates and channels from the device */
#define RATETAB_ENT(_rate, _rateid, _flags)		\
	{						\
		.bitrate	= (_rate),		\
		.hw_value	= (_rateid),		\
		.flags		= (_flags),		\
	}

static struct ieee80211_rate rk960_rates[] = {
	RATETAB_ENT(10, 0, 0),
	RATETAB_ENT(20, 1, 0),
	RATETAB_ENT(55, 2, 0),
	RATETAB_ENT(110, 3, 0),
	RATETAB_ENT(60, 6, 0),
	RATETAB_ENT(90, 7, 0),
	RATETAB_ENT(120, 8, 0),
	RATETAB_ENT(180, 9, 0),
	RATETAB_ENT(240, 10, 0),
	RATETAB_ENT(360, 11, 0),
	RATETAB_ENT(480, 12, 0),
	RATETAB_ENT(540, 13, 0),
};

static struct ieee80211_rate rk960_mcs_rates[] = {
	RATETAB_ENT(65, 14, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(130, 15, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(195, 16, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(260, 17, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(390, 18, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(520, 19, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(585, 20, IEEE80211_TX_RC_MCS),
	RATETAB_ENT(650, 21, IEEE80211_TX_RC_MCS),
};

#define rk960_a_rates		(rk960_rates + 4)
#define rk960_a_rates_size	(ARRAY_SIZE(rk960_rates) - 4)
#define rk960_g_rates		(rk960_rates + 0)
#define rk960_g_rates_size	(ARRAY_SIZE(rk960_rates))
#define rk960_n_rates		(rk960_mcs_rates)
#define rk960_n_rates_size	(ARRAY_SIZE(rk960_mcs_rates))

#define CHAN2G(_channel, _freq, _flags) {			\
	.band			= IEEE80211_BAND_2GHZ,		\
	.center_freq		= (_freq),			\
	.hw_value		= (_channel),			\
	.flags			= (_flags),			\
	.max_antenna_gain	= 0,				\
	.max_power		= 30,				\
}

#define CHAN5G(_channel, _flags) {				\
	.band			= IEEE80211_BAND_5GHZ,		\
	.center_freq	= 5000 + (5 * (_channel)),		\
	.hw_value		= (_channel),			\
	.flags			= (_flags),			\
	.max_antenna_gain	= 0,				\
	.max_power		= 30,				\
}

static struct ieee80211_channel rk960_2ghz_chantable[] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

#ifdef CONFIG_RK960_5GHZ_SUPPORT
static struct ieee80211_channel rk960_5ghz_chantable[] = {
	CHAN5G(34, 0), CHAN5G(36, 0),
	CHAN5G(38, 0), CHAN5G(40, 0),
	CHAN5G(42, 0), CHAN5G(44, 0),
	CHAN5G(46, 0), CHAN5G(48, 0),
	CHAN5G(52, 0), CHAN5G(56, 0),
	CHAN5G(60, 0), CHAN5G(64, 0),
	CHAN5G(100, 0), CHAN5G(104, 0),
	CHAN5G(108, 0), CHAN5G(112, 0),
	CHAN5G(116, 0), CHAN5G(120, 0),
	CHAN5G(124, 0), CHAN5G(128, 0),
	CHAN5G(132, 0), CHAN5G(136, 0),
	CHAN5G(140, 0), CHAN5G(149, 0),
	CHAN5G(153, 0), CHAN5G(157, 0),
	CHAN5G(161, 0), CHAN5G(165, 0),
	CHAN5G(184, 0), CHAN5G(188, 0),
	CHAN5G(192, 0), CHAN5G(196, 0),
	CHAN5G(200, 0), CHAN5G(204, 0),
	CHAN5G(208, 0), CHAN5G(212, 0),
	CHAN5G(216, 0),
};
#endif /* CONFIG_RK960_5GHZ_SUPPORT */

static struct ieee80211_supported_band rk960_band_2ghz = {
	.channels = rk960_2ghz_chantable,
	.n_channels = ARRAY_SIZE(rk960_2ghz_chantable),
	.bitrates = rk960_g_rates,
	.n_bitrates = rk960_g_rates_size,
#if 1
	.ht_cap = {
		   .cap = IEEE80211_HT_CAP_GRN_FLD
		   /*| IEEE80211_HT_CAP_SGI_20 */
		   /*| (1 << IEEE80211_HT_CAP_RX_STBC_SHIFT) */ ,
		   .ht_supported = 1,
		   .ampdu_factor = IEEE80211_HT_MAX_AMPDU_32K,
		   .ampdu_density = IEEE80211_HT_MPDU_DENSITY_NONE,
		   .mcs = {
			   .rx_mask[0] = 0xFF,
			   .rx_highest = __cpu_to_le16(0x41),
			   .tx_params = IEEE80211_HT_MCS_TX_DEFINED,
			   },
		   },
#endif
};

#ifdef CONFIG_RK960_5GHZ_SUPPORT
static struct ieee80211_supported_band rk960_band_5ghz = {
	.channels = rk960_5ghz_chantable,
	.n_channels = ARRAY_SIZE(rk960_5ghz_chantable),
	.bitrates = rk960_a_rates,
	.n_bitrates = rk960_a_rates_size,
	.ht_cap = {
		   .cap = IEEE80211_HT_CAP_GRN_FLD |
		   (1 << IEEE80211_HT_CAP_RX_STBC_SHIFT),
		   .ht_supported = 1,
		   .ampdu_factor = IEEE80211_HT_MAX_AMPDU_8K,
		   .ampdu_density = IEEE80211_HT_MPDU_DENSITY_NONE,
		   .mcs = {
			   .rx_mask[0] = 0xFF,
			   .rx_highest = __cpu_to_le16(0x41),
			   .tx_params = IEEE80211_HT_MCS_TX_DEFINED,
			   },
		   },
};
#endif /* CONFIG_RK960_5GHZ_SUPPORT */

static const unsigned long rk960_ttl[] = {
	1 * HZ,			/* VO */
	2 * HZ,			/* VI */
	5 * HZ,			/* BE */
	10 * HZ			/* BK */
};

static const struct ieee80211_ops rk960_ops = {
	.start = rk960_start,
	.stop = rk960_stop,
	.add_interface = rk960_add_interface,
	.remove_interface = rk960_remove_interface,
	.change_interface = rk960_change_interface,
	.tx = rk960_tx,
	.hw_scan = rk960_hw_scan,
#ifdef ROAM_OFFLOAD
	.sched_scan_start = rk960_hw_sched_scan_start,
	.sched_scan_stop = rk960_hw_sched_scan_stop,
#endif /*ROAM_OFFLOAD */
	.set_tim = rk960_set_tim,
	.sta_notify = rk960_sta_notify,
	.sta_add = rk960_sta_add,
	.sta_remove = rk960_sta_remove,
	.set_key = rk960_set_key,
	.set_rts_threshold = rk960_set_rts_threshold,
	.config = rk960_config,
	.bss_info_changed = rk960_bss_info_changed,
	.prepare_multicast = rk960_prepare_multicast,
	.configure_filter = rk960_configure_filter,
	.conf_tx = rk960_conf_tx,
	.get_stats = rk960_get_stats,
	.ampdu_action = rk960_ampdu_action,
	.flush = rk960_flush,
#ifdef CONFIG_PM
	.suspend = rk960_wow_suspend,
	.resume = rk960_wow_resume,
#endif /* CONFIG_PM */
	/* Intentionally not offloaded:                                 */
	/*.channel_switch       = rk960_channel_switch,         */
	.remain_on_channel = rk960_remain_on_channel,
	.cancel_remain_on_channel = rk960_cancel_remain_on_channel,
#ifdef IPV6_FILTERING
//      .set_data_filter        = rk960_set_data_filter,
#endif /*IPV6_FILTERING */
#ifdef CONFIG_RK960_TESTMODE
	.testmode_cmd = rk960_testmode_cmd,
#endif /* CONFIG_RK960_TESTMODE */
};

static const struct wiphy_wowlan_support uccp_wowlan_support = {
	.flags = WIPHY_WOWLAN_ANY,
};

int rk960_jtag_debug_enable(void)
{
	return jtag_debug;
}

int rk960_get_sdio_width_param(void)
{
        return sdio_width;
}

void rk960_sdio_width_switch(struct rk960_common *hw_priv)
{
        int src_width = rk960_sdio_get_bus_width(hw_priv);
        int dst_width = sdio_width;

        if (jtag_debug)
                return;
        
	if (src_width != dst_width) {
                RK960_INFO_MAIN("%s: %dbit -> %dbit\n",
                        __func__, src_width, dst_width);
		wsm_set_sdio_data_width(hw_priv, dst_width, 0);
		rk960_sdio_set_bus_width(hw_priv, dst_width, 1);
	}
}

/*void rk960_delba_process(struct rk960_common *hw_priv)
{
        struct ieee80211_vif *vif;
        struct rk960_vif *priv;
        struct ieee80211_sub_if_data *sdata = NULL;
        int if_id = 0;
        int i;

	rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
        vif = hw_priv->vif_list[if_id];
        if (vif)
                sdata = vif_to_sdata(vif);
        priv = rk960_get_vif_from_ieee80211_hw_priv(hw_priv, if_id);
        rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);

        if (sdata && priv &&
                priv->join_status == RK960_JOIN_STATUS_STA) {
                for (i = 0; i <= 7; i++)
                        ieee80211_send_delba(sdata,
                                priv->join_bssid, i, WLAN_BACK_RECIPIENT,
                                WLAN_REASON_QSTA_REQUIRE_SETUP);
        }
}*/

void rk960_fw_error_work(struct work_struct *work)
{
	struct rk960_common *hw_priv =
	    container_of(work, struct rk960_common, fw_err_work);
	int wanted = hw_priv->wsm_caps.firmwareReady + 1;

	if (!hw_priv->fw_error_enable)
	        BUG_ON(1);

	// 1. reset rk960
	rockchip_wifi_power(0);
	mdelay(10);
	rockchip_wifi_power(1);
	mdelay(50);

	rk960_sdio_recovery_init(hw_priv);

	// 2. download fw
	rk960_sdio_device_init(hw_priv->hwbus_priv);

	hw_priv->wsm_rx_seq = 0;
	atomic_set(&hw_priv->evt_idx, 0);

	rk960_download_fw(hw_priv, 1);

	hw_priv->wsm_tx_seq = 0;
	hw_priv->hw_bufs_used = 0;
	atomic_set(&hw_priv->msg_idx, 0);

	if (wait_event_interruptible_timeout(hw_priv->wsm_startup_done,
					     hw_priv->wsm_caps.firmwareReady ==
					     wanted, 2 * HZ) <= 0) {
		RK960_ERROR_FWREC("Timeout waiting on device startup\n");
		return;
	}

	//rk960_sdio_width_switch(hw_priv);
        
	wsm_set_fw_recovery(hw_priv, &hw_priv->wsm_rec_info);
	memset(&hw_priv->wsm_rec_info, 0, sizeof(struct wsm_rec_info));

	// 3. recovery cmds
	wsm_send_fw_recovery_cmd(hw_priv);

	// 4. delba process
	//rk960_delba_process(hw_priv);

	// 5. complete unfinished event (scan, pending tx)
	//rk960_pending_scan_complete_indication(hw_priv);
	wsm_set_pm_indication(hw_priv, NULL);

	mutex_unlock(&hw_priv->wsm_cmd_save_mux);
	rk960_tx_queues_unlock(hw_priv);

	wsm_unlock_tx_rec(hw_priv);

	hw_priv->fw_error_processing = 0;

	RK960_INFO_FWREC("-------- fw error recovery end --------\n");
}

#ifdef RK960_FW_ERROR_RECOVERY
void rk960_pending_wsm_cmd(struct rk960_common *hw_priv)
{
	if (hw_priv->wsm_cmd.ptr) {
		spin_lock(&hw_priv->wsm_cmd.lock);
		hw_priv->wsm_cmd.ret = 0;
		hw_priv->wsm_cmd.done = 1;
		hw_priv->wsm_cmd.ptr = NULL;
		spin_unlock(&hw_priv->wsm_cmd.lock);

		wake_up(&hw_priv->wsm_cmd_wq);
	}
}

void rk960_signal_fw_error(struct rk960_common *hw_priv, int reason)
{
	int idx;

	if (!hw_priv->fw_error_processing) {
		RK960_INFO_FWREC("%s\n", __func__);
		hw_priv->fw_error_processing = 1;

		wsm_lock_tx_rec(hw_priv);
		rk960_tx_queues_lock(hw_priv);
		rk960_pm_stay_awake(&hw_priv->pm_state, 3 * HZ);

		hw_priv->device_can_sleep = false;
		hw_priv->device_wakeing = false;

		mutex_lock(&hw_priv->wsm_cmd_save_mux);

		wake_up(&hw_priv->tx_queue_stats.wait_link_id_empty);

		rk960_pending_wsm_cmd(hw_priv);
		rk960_queue_flush_pending_tx(hw_priv, -1);

		BUG_ON(hw_priv->fw_error_counter >= RK960_FWERR_SIZE);
		idx = hw_priv->fw_error_counter;
		hw_priv->fw_error_reason[idx] = reason;
		hw_priv->fw_error_counter++;

		RK960_INFO_FWREC("-------- fw error"
				 " recovery %d(%d) start --------\n", idx,
				 reason);

		// trigger recovery work
		schedule_work(&hw_priv->fw_err_work);
	} else {
		if (reason == RK960_FWERR_REASON_SDIO) {
			rk960_pending_wsm_cmd(hw_priv);
		}
	}
}
#endif

#define Rk960_WIFI_MAC_ADDR_FROM_VENDOR_STORAGE
void rk960_init_mac_addr(struct rk960_common *hw_priv, int init)
{
	int i;

        if (init) {
        	// 1. check mac addr from wifi efuse
        	if (is_valid_ether_addr(hw_priv->wifi_efuse_mac_addr)) {
        		RK960_INFO_MAIN("wifi mac addr from efuse\n");
        		memcpy(hw_priv->vif_macs[0],
                                hw_priv->wifi_efuse_mac_addr, ETH_ALEN);
        	} else {
        		// 2. check mac addr from host vendor storage
        		if (rockchip_wifi_mac_addr(hw_priv->vif_macs[0])
                                != 0) {
        			RK960_INFO_MAIN("rockchip_wifi_mac_addr"
                                        " failed!\n");
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
				eth_random_addr(hw_priv->vif_macs[0]);
#else
        			random_ether_addr(hw_priv->vif_macs[0]);
#endif
        		}
        	}
        }
	memcpy(hw_priv->vif_macs[1], hw_priv->vif_macs[0], ETH_ALEN);
#if 0
	/* Set the Locally Administered bit */
	vif_macs[1][0] |= 0x02;

	/* Increment the MSB by 1 (excluding 2 special bits) */
	vif_macs[1][0] += (1 << 2);
#else
	for (i = 5; i >= 0; i--) {
		hw_priv->vif_macs[1][i]++;
		if (hw_priv->vif_macs[1][i] != 0) {
			break;
		}
	}
#endif

	// 3. register mac addr to ieee80211_hw
#ifdef Rk960_WIFI_MAC_ADDR_FROM_VENDOR_STORAGE
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		memcpy(hw_priv->addresses[i].addr, hw_priv->vif_macs[i % 2], ETH_ALEN);
	}
#else
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		memcpy(hw_priv->addresses[i].addr,
		       rk960_mac_templates[i], ETH_ALEN);
	}
#endif

	if (hw_priv->addresses[0].addr[3] == 0 &&
	    hw_priv->addresses[0].addr[4] == 0 &&
	    hw_priv->addresses[0].addr[5] == 0) {
		get_random_bytes(&hw_priv->addresses[0].addr[3], 3);
		hw_priv->addresses[1].addr[3] = hw_priv->addresses[0].addr[3];
		hw_priv->addresses[1].addr[4] = hw_priv->addresses[0].addr[4];
		hw_priv->addresses[1].addr[5] =
		    hw_priv->addresses[0].addr[5] + 1;
	}
#ifdef P2P_MULTIVIF
	memcpy(hw_priv->addresses[2].addr, hw_priv->addresses[1].addr,
	       ETH_ALEN);
	hw_priv->addresses[2].addr[4] ^= 0x80;
#endif
	RK960_INFO_MAIN("addresses[0]: %pM\n", hw_priv->addresses[0].addr);
	RK960_INFO_MAIN("addresses[1]: %pM\n", hw_priv->addresses[1].addr);
#ifdef P2P_MULTIVIF
	RK960_INFO_MAIN("addresses[2]: %pM\n", hw_priv->addresses[2].addr);
#endif

	SET_IEEE80211_PERM_ADDR(hw_priv->hw, hw_priv->addresses[0].addr);
}

struct ieee80211_hw *rk960_init_common(size_t hw_priv_data_len)
{
	int i;
	struct ieee80211_hw *hw;
	struct rk960_common *hw_priv;
	struct ieee80211_supported_band *sband;
	int band;
#ifdef CUSTOM_FEATURE		/* To use macaddr of customers */
	char readmac[17 + 1] = { 0, };
#endif

	hw = ieee80211_alloc_hw(hw_priv_data_len, &rk960_ops);
	if (!hw)
		return NULL;

	hw_priv = hw->priv;
	/* TODO:COMBO this debug message can be removed */
	RK960_DEBUG_MAIN("Allocated hw_priv @ %p\n", hw_priv);
	hw_priv->if_id_slot = 0;
	hw_priv->roc_if_id = -1;
	hw_priv->roc_if_id_last = -1;
	atomic_set(&hw_priv->num_vifs, 0);
	hw_priv->def_vif_id = -1;
	hw_priv->delayed_unjoin_if_id = -1;

	hw_priv->hwbus_rcvbuf = kmalloc(EFFECTIVE_RX_BUF_SIZE, GFP_KERNEL);
	if (!hw_priv->hwbus_rcvbuf) {
		return NULL;
	}

	hw_priv->hwbus_sndbuf = kmalloc(EFFECTIVE_TX_BUF_SIZE, GFP_KERNEL);
	if (!hw_priv->hwbus_sndbuf) {
		return NULL;
	}

	hw_priv->hw = hw;
	hw_priv->rates = rk960_rates;	/* TODO: fetch from FW */
	hw_priv->mcs_rates = rk960_n_rates;
#ifdef ROAM_OFFLOAD
	hw_priv->auto_scanning = 0;
	hw_priv->frame_rcvd = 0;
	hw_priv->num_scanchannels = 0;
	hw_priv->num_2g_channels = 0;
	hw_priv->num_5g_channels = 0;
#endif /*ROAM_OFFLOAD */
#ifdef AP_AGGREGATE_FW_FIX
	/* Enable block ACK for 4 TID (BE,VI,VI,VO). */
	/*due to HW limitations */
	hw_priv->ba_tid_mask = RK960_TX_BLOCK_ACK_ENABLED_FOR_ALL_TID;	//0xB1;
#else
	/* Enable block ACK for every TID but voice. */
	hw_priv->ba_tid_mask = 0x3F;
#endif
#if 0
	/* Enable block ACK for every TID but voice. */
	hw_priv->ba_tid_mask = 0x3F;
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
	ieee80211_hw_set(hw, NEED_DTIM_BEFORE_ASSOC);
	ieee80211_hw_set(hw, TX_AMPDU_SETUP_IN_HW);
	ieee80211_hw_set(hw, AMPDU_AGGREGATION);
	ieee80211_hw_set(hw, CONNECTION_MONITOR);
	ieee80211_hw_set(hw, REPORTS_TX_ACK_STATUS);
	ieee80211_hw_set(hw, SUPPORTS_DYNAMIC_PS);
	ieee80211_hw_set(hw, SIGNAL_DBM);
	ieee80211_hw_set(hw, SUPPORTS_PS);
	ieee80211_hw_set(hw, MFP_CAPABLE);
//	ieee80211_hw_set(hw, SUPPORTS_HT_CCK_RATES);
#else
	hw->flags = IEEE80211_HW_SIGNAL_DBM |
	    IEEE80211_HW_SUPPORTS_PS |
	    IEEE80211_HW_SUPPORTS_DYNAMIC_PS |
	    IEEE80211_HW_REPORTS_TX_ACK_STATUS | IEEE80211_HW_SUPPORTS_UAPSD |
//                  IEEE80211_HW_MFP_CAPABLE |
	    IEEE80211_HW_CONNECTION_MONITOR;	// |
	/*IEEE80211_HW_SUPPORTS_CQM_RSSI | */
	/* Aggregation is fully controlled by firmware.
	 * Do not need any support from the mac80211 stack */
	/* IEEE80211_HW_AMPDU_AGGREGATION | */
#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
	/*IEEE80211_HW_SUPPORTS_P2P_PS | */
	/*IEEE80211_HW_SUPPORTS_CQM_BEACON_MISS | */
	/*IEEE80211_HW_SUPPORTS_CQM_TX_FAIL | */
#endif /* CONFIG_RK960_USE_STE_EXTENSIONS */
	/*IEEE80211_HW_BEACON_FILTER; */
#endif

	hw->wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION) |
	    BIT(NL80211_IFTYPE_ADHOC) |
	    BIT(NL80211_IFTYPE_AP) |
	    BIT(NL80211_IFTYPE_MESH_POINT) |
	    BIT(NL80211_IFTYPE_P2P_CLIENT) | BIT(NL80211_IFTYPE_P2P_GO);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0))
	hw->wiphy->wowlan = &uccp_wowlan_support;
#else
	/* Support only for limited wowlan functionalities */
	hw->wiphy->wowlan.flags = WIPHY_WOWLAN_ANY | WIPHY_WOWLAN_DISCONNECT;
	hw->wiphy->wowlan.n_patterns = 0;
#endif
#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
	hw->wiphy->flags |= WIPHY_FLAG_AP_UAPSD;
#endif /* CONFIG_RK960_USE_STE_EXTENSIONS */

#if defined(CONFIG_RK960_DISABLE_BEACON_HINTS)
	hw->wiphy->flags |= WIPHY_FLAG_DISABLE_BEACON_HINTS;
#endif

	hw->wiphy->flags |= WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD;
	hw->wiphy->probe_resp_offload =
	    NL80211_PROBE_RESP_OFFLOAD_SUPPORT_WPS |
	    NL80211_PROBE_RESP_OFFLOAD_SUPPORT_WPS2 |
	    NL80211_PROBE_RESP_OFFLOAD_SUPPORT_P2P;

	hw->wiphy->n_addresses = RK960_MAX_VIFS;
	hw->wiphy->addresses = hw_priv->addresses;
	hw->wiphy->max_remain_on_channel_duration = 500;
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 10, 0))
	hw->channel_change_time = 500;	/* TODO: find actual value */
#endif
	/* hw_priv->beacon_req_id = cpu_to_le32(0); */
	hw->queues = 4;
	hw_priv->noise = -94;

	hw->max_rates = 8;
	hw->max_rate_tries = 15;
	hw->extra_tx_headroom = WSM_TX_EXTRA_HEADROOM + 8 /* TKIP IV */  +
	    12 /* TKIP ICV and MIC */ ;

	hw->sta_data_size = sizeof(struct rk960_sta_priv);
	hw->vif_data_size = sizeof(struct rk960_vif);

	if (sgi) {
		RK960_INFO_MAIN("enable sgi\n");
		rk960_band_2ghz.ht_cap.cap |= IEEE80211_HT_CAP_SGI_20;
	}
	if (stbc) {
		RK960_INFO_MAIN("enable stbc\n");
		rk960_band_2ghz.ht_cap.cap |=
		    (1 << IEEE80211_HT_CAP_RX_STBC_SHIFT);
	}
	if (!ht_support) {
		RK960_INFO_MAIN("disable ht\n");
		rk960_band_2ghz.ht_cap.ht_supported = 0;
	}
	if (mcs_mask != 0xFF) {
		RK960_INFO_MAIN("mcs_mask = %x\n", mcs_mask);
		rk960_band_2ghz.ht_cap.mcs.rx_mask[0] = mcs_mask;
	}

	hw->wiphy->bands[IEEE80211_BAND_2GHZ] = &rk960_band_2ghz;
#ifdef CONFIG_RK960_5GHZ_SUPPORT
	hw->wiphy->bands[IEEE80211_BAND_5GHZ] = &rk960_band_5ghz;
#endif /* CONFIG_RK960_5GHZ_SUPPORT */

	/* Channel params have to be cleared before registering wiphy again */
	for (band = 0; band < IEEE80211_NUM_BANDS; band++) {
		sband = hw->wiphy->bands[band];
		if (!sband)
			continue;
		for (i = 0; i < sband->n_channels; i++) {
			sband->channels[i].flags = 0;
			sband->channels[i].max_antenna_gain = 0;
			sband->channels[i].max_power = 30;
		}
	}

	hw->wiphy->max_scan_ssids = WSM_SCAN_MAX_NUM_OF_SSIDS;
	hw->wiphy->max_scan_ie_len = IEEE80211_MAX_DATA_LEN;
#ifdef ROAM_OFFLOAD
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
	hw->wiphy->max_sched_scan_reqs = 1;
#endif
	hw->wiphy->max_match_sets = WSM_SCAN_MAX_MATCH_SETS_NUM_OF_SSIDS;
	hw->wiphy->max_sched_scan_ssids = WSM_SCAN_MAX_MATCH_SETS_NUM_OF_SSIDS;
	hw->wiphy->max_sched_scan_ie_len = IEEE80211_MAX_DATA_LEN;
#endif

	spin_lock_init(&hw_priv->hw_bufs_used_lock);
	spin_lock_init(&hw_priv->vif_list_lock);
	mutex_init(&hw_priv->wsm_cmd_mux);
	mutex_init(&hw_priv->conf_mutex);
	mutex_init(&hw_priv->wsm_oper_lock);
	mutex_init(&hw_priv->wsm_cmd_save_mux);
#ifdef CONFIG_RK960_TESTMODE
	spin_lock_init(&hw_priv->tsm_lock);
#endif /*CONFIG_RK960_TESTMODE */
	hw_priv->workqueue = create_singlethread_workqueue("rk960_wq");
	sema_init(&hw_priv->scan.lock, 1);
	INIT_WORK(&hw_priv->scan.work, rk960_scan_work);
#ifdef ROAM_OFFLOAD
	INIT_WORK(&hw_priv->scan.swork, rk960_sched_scan_work);
#endif /*ROAM_OFFLOAD */
	INIT_DELAYED_WORK(&hw_priv->scan.probe_work, rk960_probe_work);
	INIT_DELAYED_WORK(&hw_priv->scan.sched_scan_stop_work,
			  rk960_sched_scan_stop_work);
	INIT_DELAYED_WORK(&hw_priv->scan.timeout, rk960_scan_timeout);
#ifdef CONFIG_RK960_TESTMODE
	INIT_DELAYED_WORK(&hw_priv->advance_scan_timeout,
			  rk960_advance_scan_timeout);
#endif
	INIT_DELAYED_WORK(&hw_priv->rem_chan_timeout, rk960_rem_chan_timeout);
	INIT_WORK(&hw_priv->tx_policy_upload_work, tx_policy_upload_work);
	spin_lock_init(&hw_priv->event_queue_lock);
	INIT_LIST_HEAD(&hw_priv->event_queue);
	INIT_WORK(&hw_priv->event_handler, rk960_event_handler);
	INIT_WORK(&hw_priv->ba_work, rk960_ba_work);
	spin_lock_init(&hw_priv->ba_lock);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
	timer_setup(&hw_priv->ba_timer, rk960_ba_timer, 0);
#else
	init_timer(&hw_priv->ba_timer);
	hw_priv->ba_timer.data = (unsigned long)hw_priv;
	hw_priv->ba_timer.function = rk960_ba_timer;
#endif
#ifdef RK960_CSYNC_ADJUST
	spin_lock_init(&hw_priv->csync_lock);
	rk960_csync_param_init(hw_priv);
	INIT_WORK(&hw_priv->csync_work, rk960_csync_work);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
	timer_setup(&hw_priv->csync_timer, rk960_csync_timer, 0);
#else
	init_timer(&hw_priv->csync_timer);
	hw_priv->csync_timer.data = (unsigned long)hw_priv;
	hw_priv->csync_timer.function = rk960_csync_timer;
#endif
#endif

	hw_priv->fw_error_enable = 1;
	INIT_LIST_HEAD(&hw_priv->wsm_cmds_list);
	INIT_WORK(&hw_priv->fw_err_work, rk960_fw_error_work);
	wsm_sended_cmds_init(hw_priv);

#ifdef SUPPORT_FWCR
        INIT_WORK(&hw_priv->fwcr_work, rk960_fwcr_work);
        init_waitqueue_head(&hw_priv->fwcr_resume_done);
#endif

	if (unlikely(rk960_queue_stats_init(&hw_priv->tx_queue_stats,
					    WLAN_LINK_ID_MAX,
					    rk960_skb_dtor, hw_priv))) {
		ieee80211_free_hw(hw);
		return NULL;
	}

	for (i = 0; i < 4; ++i) {
		if (unlikely(rk960_queue_init(&hw_priv->tx_queue[i],
					      &hw_priv->tx_queue_stats, i,
					      RK960_MAX_QUEUE_SZ,
					      rk960_ttl[i]))) {
			for (; i > 0; i--)
				rk960_queue_deinit(&hw_priv->tx_queue[i - 1]);
			rk960_queue_stats_deinit(&hw_priv->tx_queue_stats);
			ieee80211_free_hw(hw);
			return NULL;
		}
	}

	init_waitqueue_head(&hw_priv->channel_switch_done);
	init_waitqueue_head(&hw_priv->wsm_cmd_wq);
	init_waitqueue_head(&hw_priv->wsm_startup_done);
	init_waitqueue_head(&hw_priv->offchannel_wq);
	init_waitqueue_head(&hw_priv->debug_dump_done);
	init_waitqueue_head(&hw_priv->ps_mode_switch_done);
	hw_priv->offchannel_done = 0;
	wsm_buf_init(&hw_priv->wsm_cmd_buf);
	spin_lock_init(&hw_priv->wsm_cmd.lock);
	tx_policy_init(hw_priv);
#ifdef CONFIG_RK960_TESTMODE
	hw_priv->test_frame.data = NULL;
	hw_priv->test_frame.len = 0;
#endif /* CONFIG_RK960_TESTMODE */
#if defined(CONFIG_RK960_WSM_DUMPS_SHORT)
	hw_priv->wsm_dump_max_size = 20;
#endif /* CONFIG_RK960_WSM_DUMPS_SHORT */

	for (i = 0; i < RK960_MAX_VIFS; i++)
		hw_priv->hw_bufs_used_vif[i] = 0;

#ifdef MCAST_FWDING
	for (i = 0; i < WSM_MAX_BUF; i++)
		wsm_init_release_buffer_request(hw_priv, i);
	hw_priv->buf_released = 0;
#endif
	hw_priv->vif0_throttle = RK960_HOST_VIF0_11BG_THROTTLE;
	hw_priv->vif1_throttle = RK960_HOST_VIF1_11BG_THROTTLE;
	return hw;
}

//EXPORT_SYMBOL_GPL(rk960_init_common);

int rk960_register_common(struct ieee80211_hw *dev)
{
	struct rk960_common *hw_priv = dev->priv;
	int err;

	err = rk960_pm_init(&hw_priv->pm_state, hw_priv);
	if (err) {
		RK960_ERROR_MAIN("Cannot init PM. (%d).\n", err);
		return err;
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
	rk960_vendor_init(dev->wiphy);
#endif
	err = ieee80211_register_hw(dev);
	if (err) {
		RK960_ERROR_MAIN("Cannot register device (%d).\n", err);
		rk960_pm_deinit(&hw_priv->pm_state);
		return err;
	}
#ifdef CUSTOM_REG_SET
	regulatory_hint(dev->wiphy, "US");
#endif

#ifdef CONFIG_RK960_LEDS
	err = rk960_init_leds(priv);
	if (err) {
		rk960_pm_deinit(&hw_priv->pm_state);
		ieee80211_unregister_hw(dev);
		return err;
	}
#endif /* CONFIG_RK960_LEDS */

	rk960_debug_init_common(hw_priv);

	RK960_INFO_MAIN("is registered as '%s'\n", wiphy_name(dev->wiphy));
	return 0;
}

//EXPORT_SYMBOL_GPL(rk960_register_common);

void rk960_free_common(struct ieee80211_hw *dev)
{
	/* struct rk960_common *hw_priv = dev->priv; */
#ifdef CONFIG_RK960_TESTMODE
	struct rk960_common *hw_priv = dev->priv;
	kfree(hw_priv->test_frame.data);
#endif /* CONFIG_RK960_TESTMODE */
	/* unsigned int i; */

	ieee80211_free_hw(dev);
}

//EXPORT_SYMBOL_GPL(rk960_free_common);

void rk960_unregister_common(struct ieee80211_hw *dev)
{
	struct rk960_common *hw_priv = dev->priv;
	int i;

	ieee80211_unregister_hw(dev);

	del_timer_sync(&hw_priv->ba_timer);
#ifdef RK960_CSYNC_ADJUST
	del_timer_sync(&hw_priv->csync_timer);
#endif

	//hw_priv->hwbus_ops->irq_unsubscribe(hw_priv->hwbus_priv);
	rk960_unregister_bh(hw_priv);

	rk960_debug_release_common(hw_priv);

#ifdef CONFIG_RK960_LEDS
	rk960_unregister_leds(hw_priv);
#endif /* CONFIG_RK960_LEDS */

	mutex_destroy(&hw_priv->conf_mutex);

	wsm_buf_deinit(&hw_priv->wsm_cmd_buf);

	wsm_sended_cmds_deinit(hw_priv);

	destroy_workqueue(hw_priv->workqueue);
	hw_priv->workqueue = NULL;

	if (hw_priv->skb_cache) {
		dev_kfree_skb(hw_priv->skb_cache);
		hw_priv->skb_cache = NULL;
	}

	if (hw_priv->sdd) {
		release_firmware(hw_priv->sdd);
		hw_priv->sdd = NULL;
	}

	for (i = 0; i < 4; ++i)
		rk960_queue_deinit(&hw_priv->tx_queue[i]);
	rk960_queue_stats_deinit(&hw_priv->tx_queue_stats);
	rk960_pm_deinit(&hw_priv->pm_state);

	for (i = 0; i < RK960_MAX_VIFS; i++) {
		kfree(hw_priv->vif_list[i]);
		hw_priv->vif_list[i] = NULL;
	}

	if (hw_priv->hwbus_rcvbuf)
		kfree(hw_priv->hwbus_rcvbuf);
	if (hw_priv->hwbus_sndbuf)
		kfree(hw_priv->hwbus_sndbuf);
}

//EXPORT_SYMBOL_GPL(rk960_unregister_common);

static void rk960_set_ifce_comb(struct rk960_common *hw_priv,
				struct ieee80211_hw *hw)
{
#ifdef P2P_MULTIVIF
	hw_priv->if_limits1[0].max = 2;
#else
	hw_priv->if_limits1[0].max = 1;
#endif

	hw_priv->if_limits1[0].types = BIT(NL80211_IFTYPE_STATION);
	hw_priv->if_limits1[1].max = 1;
	hw_priv->if_limits1[1].types = BIT(NL80211_IFTYPE_AP);

#ifdef P2P_MULTIVIF
	hw_priv->if_limits2[0].max = 3;
#else
	hw_priv->if_limits2[0].max = 2;
#endif
	hw_priv->if_limits2[0].types = BIT(NL80211_IFTYPE_STATION);

#ifdef P2P_MULTIVIF
	hw_priv->if_limits3[0].max = 2;
#else
	hw_priv->if_limits3[0].max = 1;
#endif

	hw_priv->if_limits3[0].types = BIT(NL80211_IFTYPE_STATION);
	hw_priv->if_limits3[1].max = 1;
	hw_priv->if_limits3[1].types = BIT(NL80211_IFTYPE_P2P_CLIENT) |
	    BIT(NL80211_IFTYPE_P2P_GO);

	/* TODO:COMBO: mac80211 doesn't yet support more than 1
	 * different channel */
	hw_priv->if_combs[0].num_different_channels = 1;
#ifdef P2P_MULTIVIF
	hw_priv->if_combs[0].max_interfaces = 3;
#else
	hw_priv->if_combs[0].max_interfaces = 2;
#endif
	hw_priv->if_combs[0].limits = hw_priv->if_limits1;
	hw_priv->if_combs[0].n_limits = 2;

	hw_priv->if_combs[1].num_different_channels = 1;

#ifdef P2P_MULTIVIF
	hw_priv->if_combs[1].max_interfaces = 3;
#else
	hw_priv->if_combs[1].max_interfaces = 2;
#endif
	hw_priv->if_combs[1].limits = hw_priv->if_limits2;
	hw_priv->if_combs[1].n_limits = 1;

	hw_priv->if_combs[2].num_different_channels = 1;
#ifdef P2P_MULTIVIF
	hw_priv->if_combs[2].max_interfaces = 3;
#else
	hw_priv->if_combs[2].max_interfaces = 2;
#endif
	hw_priv->if_combs[2].limits = hw_priv->if_limits3;
	hw_priv->if_combs[2].n_limits = 2;

	hw->wiphy->iface_combinations = &hw_priv->if_combs[0];
	hw->wiphy->n_iface_combinations = 3;

}

int rk960_core_probe(const struct hwbus_ops *hwbus_ops,
		     struct hwbus_priv *hwbus,
		     struct device *pdev,
		     int device_id,
		     struct rk960_common **core,
		     int ref_clk, const u8 * macaddr,
		     const char *sdd_path, bool have_5ghz)
{
	int err = -ENOMEM;
	struct ieee80211_hw *dev;
	struct rk960_common *hw_priv;
	struct wsm_operational_mode mode = {
		.power_mode = wsm_power_mode_quiescent,
		.disableMoreFlagUsage = true,
	};
	int wait_time = 3;
#ifdef CUSTOM_FEATURE		/* To control ps mode */
	char buffer[2];
	savedpsm = mode.power_mode;
	if (rk960_access_file(PATH_WIFI_PSM_INFO, buffer, 2, 1) > 0) {
		if (buffer[0] == 0x30) {
			mode.power_mode = wsm_power_mode_active;
		} else {
			if (savedpsm)
				mode.power_mode = savedpsm;
			else	/* Set default */
				mode.power_mode = wsm_power_mode_quiescent;
		}
		RK960_INFO_MAIN("RK960 : PSM changed to %d\n", mode.power_mode);
	} else {
		RK960_INFO_MAIN("RK960 : Using default PSM %d\n",
				mode.power_mode);
	}
#endif
	int if_id;
	u16 ctrl_reg;

	dev = rk960_init_common(sizeof(struct rk960_common));
	if (!dev)
		goto err;

	hw_priv = dev->priv;

	hw_priv->chip_id = device_id;
	hw_priv->hwbus_ops = hwbus_ops;
	hw_priv->hwbus_priv = hwbus;
	hw_priv->pdev = pdev;
	SET_IEEE80211_DEV(hw_priv->hw, pdev);

	/* WSM callbacks. */
	hw_priv->wsm_cbc.scan_complete = rk960_scan_complete_cb;
	hw_priv->wsm_cbc.tx_confirm = rk960_tx_confirm_cb;
	hw_priv->wsm_cbc.rx = rk960_rx_cb;
	hw_priv->wsm_cbc.suspend_resume = rk960_suspend_resume;
	/* hw_priv->wsm_cbc.set_pm_complete = rk960_set_pm_complete_cb; */
	hw_priv->wsm_cbc.channel_switch = rk960_channel_switch_cb;

	/* Pass struct rk960_common back up */
	*core = hw_priv;

	err = rk960_register_bh(hw_priv);
	if (err)
		goto err1;

	rk960_sdio_device_init(hw_priv->hwbus_priv);

        hw_priv->fw_hotboot = 0;
	rk960_bh_read_ctrl_reg(hw_priv, &ctrl_reg);
	if (ctrl_reg) {
		RK960_INFO_MAIN("FW hotboot\n");
		hw_priv->fw_hotboot = 1;
		__rk960_clear_irq(hw_priv);
	}

	if (jtag_debug) {
		wait_time = 600;
	} else {
		err = rk960_load_firmware(hw_priv);
		if (err)
			goto err2;
	}

	err = rk960_sdio_irq_subscribe(hw_priv->hwbus_priv);
	if (err)
		goto err2;

        if (!hw_priv->fw_hotboot)
                rk960_start_fw(hw_priv);

	/*hw_priv->hwbus_ops->lock(hw_priv->hwbus_priv);
	   WARN_ON(hw_priv->hwbus_ops->set_block_size(hw_priv->hwbus_priv,
	   SDIO_BLOCK_SIZE));
	   hw_priv->hwbus_ops->unlock(hw_priv->hwbus_priv); */

	rk960_set_ifce_comb(hw_priv, dev);
	/*hw_priv->hwbus_ops->lock(hw_priv->hwbus_priv);
	   WARN_ON(hw_priv->hwbus_ops->set_block_size(hw_priv->hwbus_priv,
	   SDIO_BLOCK_SIZE));
	   hw_priv->hwbus_ops->unlock(hw_priv->hwbus_priv); */

        memcpy(hw_priv->tx_agg_ctrl.tx_max_agg_num,
                        rk960_tx_max_agg_num_tbl, 8);

#ifdef SUPPORT_FWCR
        rk960_fwcr_init(hw_priv);
        if (hw_priv->fw_hotboot) {
                rk960_fwcr_read(hw_priv);
                if (!hw_priv->fwcr_recovery) {
                        RK960_ERROR_MAIN("%s: fwcr_read fail in fw_hotboot\n",
                                __func__);
                        goto err2;
                }
                hw_priv->fwcr_fw_resumed = 0;
                hw_priv->fwcr_update_key = 0;
        }
#endif

	if (!hw_priv->fw_hotboot) {
		if (wait_event_interruptible_timeout(hw_priv->wsm_startup_done,
						     hw_priv->wsm_caps.
						     firmwareReady,
						     wait_time * HZ) <= 0) {

			/* TODO: Needs to find how to reset device */
			/*       in QUEUE mode properly.           */
			RK960_ERROR_MAIN("Timeout waiting on device startup\n");
			goto err3;
		}

                rk960_sdio_width_switch(hw_priv);

		wsm_read_efuse(hw_priv, hw_priv->efuse);

		RK960_INFO_MAIN("dump efuse:\n");
		print_hex_dump(KERN_INFO, " ", DUMP_PREFIX_NONE, 16, 1,
			       hw_priv->efuse, 64, 1);

		memcpy(hw_priv->wifi_efuse_mac_addr, &hw_priv->efuse[35], 6);
		memcpy(hw_priv->bt_efuse_mac_addr, &hw_priv->efuse[41], 6);
		rk960_init_mac_addr(hw_priv, 1);

		wsm_set_lpo_source(hw_priv, RK960_LPO_EXTERNAL, 0);
#ifdef DISABLE_TX_POLICY
                wsm_disable_tx_policy(hw_priv, 1);
#endif
		wsm_set_txpwr_comp(hw_priv);

                wsm_set_agg_control(hw_priv, &hw_priv->tx_agg_ctrl);

		for (if_id = 0; if_id < rk960_get_nr_hw_ifaces(hw_priv);
		     if_id++) {
			/* Set low-power mode. */
			WARN_ON(wsm_set_operational_mode
				(hw_priv, &mode, if_id));
			/* Enable multi-TX confirmation */
			WARN_ON(wsm_use_multi_tx_conf(hw_priv, true, if_id));
		}
	} else {
#ifdef SUPPORT_FWCR
                rk960_init_mac_addr(hw_priv, 0);
#endif
        }

	err = rk960_register_common(dev);
	if (err) {
		//hw_priv->sbus_ops->hwbus_ops(hw_priv->hwbus_priv);
		goto err3;
	}
#ifdef RK960_SDIO_RX_TP_TEST
	wsm_set_sdio_rx_tp_test(hw_priv, 1, 0);
	return err;
#endif
#ifdef RK960_SDIO_TX_TP_TEST
	hw_priv->sdio_tx_tp_start = 1;
	rk960_bh_wakeup(hw_priv);
#endif

	*core = dev->priv;
	return err;

err3:
	//sbus_ops->reset(sbus);
err2:
	rk960_unregister_bh(hw_priv);
err1:
	rk960_free_common(dev);
err:
	return err;
}

//EXPORT_SYMBOL_GPL(rk960_core_probe);

void rk960_core_release(struct rk960_common *self)
{
	rk960_stop(self->hw);

	rk960_free_firmware_buf(&self->firmware);

	rk960_unregister_common(self->hw);

	/*while (atomic_read(&self->num_vifs) != 0) {
	   msleep(10);
	   } */
	__rk960_reset_device(self);
	//__rk960_notify_device_pwrdown(self);

	rk960_free_common(self->hw);
	return;
}

//EXPORT_SYMBOL_GPL(rk960_core_release);

int rk960_access_file(char *path, char *buffer, int size, int isRead)
{
	int ret = 0;
	struct file *fp;
	//mm_segment_t old_fs = get_fs();

	if (isRead)
		fp = filp_open(path, O_RDONLY, S_IRUSR);
	else
		fp = filp_open(path, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR);

	if (IS_ERR(fp)) {
		RK960_ERROR_MAIN("RK960 : can't open %s\n", path);
		return -1;
	}

	if (isRead) {
		/*if (fp->f_op->read == NULL) {
			RK960_ERROR_MAIN("RK960 : Not allow to read\n");
			return -2;
		} else */{
			fp->f_pos = 0;
			//set_fs(KERNEL_DS);
			ret = vfs_read(fp, buffer, size, &fp->f_pos);
			//set_fs(old_fs);
		}
	} else {
		/*if (fp->f_op->write == NULL) {
			RK960_ERROR_MAIN("RK960 : Not allow to write\n");
			return -2;
		} else*/ {
			fp->f_pos = 0;
			//set_fs(KERNEL_DS);
			ret = vfs_write(fp, buffer, size, &fp->f_pos);
			//set_fs(old_fs);
		}
	}
	filp_close(fp, NULL);

	//RK960_DEBUG_MAIN("RK960 : access_file return code(%d)\n",ret);
	return ret;
}

int rk960_get_file_size(char *path)
{
    struct file *filp;
    loff_t file_size;

    filp = filp_open(path, O_RDONLY, 0);
    if (IS_ERR(filp)) {
        RK960_ERROR_MAIN("RK960 : can't open %s\n", path);
        return 0;
    }

    file_size = filp->f_inode->i_size;
    filp_close(filp, NULL);

    return (int)file_size;
}

