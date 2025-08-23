/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef SCAN_H_INCLUDED
#define SCAN_H_INCLUDED

#include <linux/semaphore.h>
#include "wsm.h"

/* external */ struct sk_buff;
/* external */ struct cfg80211_scan_request;
/* external */ struct ieee80211_channel;
/* external */ struct ieee80211_hw;
/* external */ struct work_struct;

struct rk960_scan {
	struct semaphore lock;
	struct work_struct work;
#ifdef ROAM_OFFLOAD
	struct work_struct swork;	/* scheduled scan work */
	struct cfg80211_sched_scan_request *sched_req;
#endif				/*ROAM_OFFLOAD */
	struct delayed_work timeout;
	struct cfg80211_scan_request *req;
	struct ieee80211_channel **begin;
	struct ieee80211_channel **curr;
	struct ieee80211_channel **end;
	struct wsm_ssid ssids[WSM_SCAN_MAX_NUM_OF_SSIDS];
	int output_power;
	int n_ssids;
	int status;
	atomic_t in_progress;
	/* Direct probe requests workaround */
	struct delayed_work probe_work;
	struct delayed_work sched_scan_stop_work;
	int direct_probe;
	u8 if_id;
	int p2p_probe;
	bool in_txrx_burst;
};

int rk960_hw_scan(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
		  struct ieee80211_scan_request *hw_req);
#else
		  struct cfg80211_scan_request *req);
#endif
#ifdef ROAM_OFFLOAD
int rk960_hw_sched_scan_start(struct ieee80211_hw *hw,
			      struct ieee80211_vif *vif,
			      struct cfg80211_sched_scan_request *req,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
			      struct ieee80211_scan_ies *ies);
#else
			      struct ieee80211_sched_scan_ies *ies);
#endif
void _rk960_hw_sched_scan_stop(struct rk960_common *hw_priv);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
int
#else
void
#endif
 rk960_hw_sched_scan_stop(struct ieee80211_hw *hw, struct ieee80211_vif *vif);
void rk960_sched_scan_work(struct work_struct *work);
#endif /*ROAM_OFFLOAD */
void rk960_scan_work(struct work_struct *work);
void rk960_scan_timeout(struct work_struct *work);
void rk960_scan_complete_cb(struct rk960_common *priv,
			    struct wsm_scan_complete *arg);

/* ******************************************************************** */
/* Raw probe requests TX workaround					*/
void rk960_probe_work(struct work_struct *work);
void rk960_sched_scan_stop_work(struct work_struct *work);
#ifdef CONFIG_RK960_TESTMODE
/* Advance Scan Timer							*/
void rk960_advance_scan_timeout(struct work_struct *work);
#endif
int rk960_rftest_start_tx(struct rk960_common *hw_priv, int channel,
			  int rate, int tx_power, int len, int count, int sgi);
int rk960_rftest_stop_txrx(struct rk960_common *hw_priv);
int rk960_rftest_start_rx(struct rk960_common *hw_priv, int channel);
int rk960_rftest_start_tone(struct rk960_common *hw_priv, int channel,
			    int tone);
void rk960_csync_scan(struct rk960_common *hw_priv);
void rk960_csync_scan_complete(struct rk960_common *hw_priv);
void rk960_csync_join(struct rk960_common *hw_priv, s8 rssi);
bool rk960_is_txrx_burst(struct rk960_common *hw_priv);
bool rk960_is_apsta_cowork(struct rk960_common *hw_priv);
#endif
