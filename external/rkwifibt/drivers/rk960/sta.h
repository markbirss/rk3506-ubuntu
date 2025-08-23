/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef STA_H_INCLUDED
#define STA_H_INCLUDED

/* ******************************************************************** */
/* mac80211 API								*/

int rk960_start(struct ieee80211_hw *dev);
void rk960_stop(struct ieee80211_hw *dev);
int rk960_add_interface(struct ieee80211_hw *dev, struct ieee80211_vif *vif);
void rk960_remove_interface(struct ieee80211_hw *dev,
			    struct ieee80211_vif *vif);
int rk960_change_interface(struct ieee80211_hw *dev,
			   struct ieee80211_vif *vif,
			   enum nl80211_iftype new_type, bool p2p);

int rk960_config(struct ieee80211_hw *dev, u32 changed);
int rk960_change_interface(struct ieee80211_hw *dev,
			   struct ieee80211_vif *vif,
			   enum nl80211_iftype new_type, bool p2p);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
void rk960_configure_filter(struct ieee80211_hw *hw,
			    unsigned int changed_flags,
			    unsigned int *total_flags, u64 multicast);
#else
void rk960_configure_filter(struct ieee80211_hw *dev,
			    struct ieee80211_vif *vif,
			    unsigned int changed_flags,
			    unsigned int *total_flags, u64 multicast);
#endif
int rk960_conf_tx(struct ieee80211_hw *dev, struct ieee80211_vif *vif,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
                unsigned int link_id,
#endif
		  u16 queue, const struct ieee80211_tx_queue_params *params);
int rk960_get_stats(struct ieee80211_hw *dev,
		    struct ieee80211_low_level_stats *stats);
/* Not more a part of interface?
int rk960_get_tx_stats(struct ieee80211_hw *dev,
			struct ieee80211_tx_queue_stats *stats);
*/
int rk960_set_key(struct ieee80211_hw *dev, enum set_key_cmd cmd,
		  struct ieee80211_vif *vif, struct ieee80211_sta *sta,
		  struct ieee80211_key_conf *key);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
int rk960_set_rts_threshold(struct ieee80211_hw *hw, u32 value);
#else
int rk960_set_rts_threshold(struct ieee80211_hw *hw,
			    struct ieee80211_vif *vif, u32 value);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
void rk960_flush(struct ieee80211_hw *hw,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
		 struct ieee80211_vif *vif,
#endif
		 u32 queues, bool drop);
#else
void rk960_flush(struct ieee80211_hw *hw, struct ieee80211_vif *vif, bool drop);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
int rk960_remain_on_channel(struct ieee80211_hw *hw,
			    struct ieee80211_vif *vif,
			    struct ieee80211_channel *chan,
			    int duration, enum ieee80211_roc_type type);
#else
int rk960_remain_on_channel(struct ieee80211_hw *hw,
			    struct ieee80211_vif *vif,
			    struct ieee80211_channel *chan,
			    enum nl80211_channel_type channel_type,
			    int duration, u64 cookie);
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(5, 10, 0))
int rk960_cancel_remain_on_channel(struct ieee80211_hw *hw,
				   struct ieee80211_vif *vif);
#else
int rk960_cancel_remain_on_channel(struct ieee80211_hw *hw);
#endif

int rk960_set_arpreply(struct ieee80211_hw *hw, struct ieee80211_vif *vif);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
u64 rk960_prepare_multicast(struct ieee80211_hw *hw,
			    struct netdev_hw_addr_list *mc_list);
#else
u64 rk960_prepare_multicast(struct ieee80211_hw *hw,
			    struct ieee80211_vif *vif,
			    struct netdev_hw_addr_list *mc_list);
#endif

int rk960_set_pm(struct rk960_vif *priv, const struct wsm_set_pm *arg);

void rk960_set_data_filter(struct ieee80211_hw *hw,
			   struct ieee80211_vif *vif, void *data, int len);
int rk960_set_ipv4addrfilter(struct rk960_common *hw_priv,
			    struct rk960_vif *priv, bool enable);
/* ******************************************************************** */
/* WSM callbacks							*/

/* void rk960_set_pm_complete_cb(struct rk960_common *hw_priv,
	struct wsm_set_pm_complete *arg); */
void rk960_channel_switch_cb(struct rk960_common *hw_priv);

/* ******************************************************************** */
/* WSM events								*/

void rk960_free_event_queue(struct rk960_common *hw_priv);
void rk960_event_handler(struct work_struct *work);
void rk960_bss_loss_work(struct work_struct *work);
void rk960_connection_loss_work(struct work_struct *work);
void rk960_keep_alive_work(struct work_struct *work);
void rk960_tx_failure_work(struct work_struct *work);
struct sk_buff *rk960_build_deauth_frame(u8 * sa, u8 * da,
					 u8 * bssid, bool tods);

/* ******************************************************************** */
/* Internal API								*/

int rk960_setup_mac(struct rk960_common *hw_priv);
void rk960_join_work(struct work_struct *work);
#ifdef IBSS_SUPPORT
void rk960_ibss_join_work(struct rk960_vif *priv);
#endif
void rk960_join_timeout(struct work_struct *work);
void rk960_unjoin_work(struct work_struct *work);
void rk960_offchannel_work(struct work_struct *work);
void rk960_channel_switch_work(struct work_struct *work);
void rk960_wep_key_work(struct work_struct *work);
void rk960_update_filtering(struct rk960_vif *priv);
void rk960_update_filtering_work(struct work_struct *work);
int __rk960_flush(struct rk960_common *hw_priv, bool drop, int if_id);
void rk960_set_beacon_wakeup_period_work(struct work_struct *work);
int rk960_enable_listening(struct rk960_vif *priv,
			   struct ieee80211_channel *chan);
int rk960_disable_listening(struct rk960_vif *priv);
int rk960_set_uapsd_param(struct rk960_vif *priv,
			  const struct wsm_edca_params *arg);
void rk960_ba_work(struct work_struct *work);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
void rk960_ba_timer(struct timer_list *t);
#else
void rk960_ba_timer(unsigned long arg);
#endif
const u8 *rk960_get_ie(u8 * start, size_t len, u8 ie);
int rk960_vif_setup(struct rk960_vif *priv, int set);
int rk960_setup_mac_pvif(struct rk960_vif *priv);
void rk960_iterate_vifs(void *data, u8 * mac, struct ieee80211_vif *vif);
void rk960_rem_chan_timeout(struct work_struct *work);
int rk960_set_macaddrfilter(struct rk960_common *hw_priv,
			    struct rk960_vif *priv, u8 * data);
#ifdef ROAM_OFFLOAD
int rk960_testmode_event(struct wiphy *wiphy, const u32 msg_id,
			 const void *data, int len, gfp_t gfp);
#endif /*ROAM_OFFLOAD */
#ifdef IPV6_FILTERING
int rk960_set_na(struct ieee80211_hw *hw, struct ieee80211_vif *vif);
#endif /*IPV6_FILTERING */
#ifdef CONFIG_RK960_TESTMODE
void rk960_device_power_calc(struct rk960_common *priv,
			     s16 max_output_power, s16 fe_cor, u32 band);
int rk960_testmode_cmd(struct ieee80211_hw *hw, void *data, int len);
int rk960_tesmode_event(struct wiphy *wiphy, const u32 msg_id,
			const void *data, int len, gfp_t gfp);
int rk960_get_tx_power_range(struct ieee80211_hw *hw);
int rk960_get_tx_power_level(struct ieee80211_hw *hw);
#endif /* CONFIG_RK960_TESTMODE */
int rk960_sdd_parse(struct rk960_common *hw_priv, u8 * sdd, int size);
#endif
