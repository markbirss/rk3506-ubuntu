/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef AP_H_INCLUDED
#define AP_H_INCLUDED

#define RK960_NOA_NOTIFICATION_DELAY 10

int rk960_set_tim(struct ieee80211_hw *dev, struct ieee80211_sta *sta,
		  bool set);
int rk960_sta_add(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
		  struct ieee80211_sta *sta);
int rk960_sta_remove(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
		     struct ieee80211_sta *sta);
void rk960_sta_notify(struct ieee80211_hw *dev, struct ieee80211_vif *vif,
		      enum sta_notify_cmd notify_cmd,
		      struct ieee80211_sta *sta);
void rk960_bss_info_changed(struct ieee80211_hw *dev,
			    struct ieee80211_vif *vif,
			    struct ieee80211_bss_conf *info,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
                            u64 changed);
#else
                            u32 changed);
#endif
int rk960_ampdu_action(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
		       struct ieee80211_ampdu_params *params);
#else
		       enum ieee80211_ampdu_mlme_action action,
		       struct ieee80211_sta *sta, u16 tid, u16 * ssn,
		       u8 buf_size);
#endif
void rk960_suspend_resume(struct rk960_vif *priv,
			  struct wsm_suspend_resume *arg);
void rk960_set_tim_work(struct work_struct *work);
void rk960_set_cts_work(struct work_struct *work);
void rk960_multicast_start_work(struct work_struct *work);
void rk960_multicast_stop_work(struct work_struct *work);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
void rk960_mcast_timeout(struct timer_list *t);
#else
void rk960_mcast_timeout(unsigned long arg);
#endif
int rk960_find_link_id(struct rk960_vif *priv, const u8 * mac);
int rk960_alloc_link_id(struct rk960_vif *priv, const u8 * mac);
void rk960_link_id_work(struct work_struct *work);
void rk960_link_id_gc_work(struct work_struct *work);
#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
void rk960_notify_noa(struct rk960_vif *priv, int delay);
#endif
int rk960_unmap_link(struct rk960_vif *priv, int link_id);
#ifdef AP_HT_CAP_UPDATE
void rk960_ht_info_update_work(struct work_struct *work);
#endif
int rk960_update_beaconing(struct rk960_vif *priv, int force);

#endif
