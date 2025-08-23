/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/sched.h>
#include "rk960.h"
#include "scan.h"
#include "sta.h"
#include "pm.h"
#include "debug.h"
#include <../net/mac80211/ieee80211_i.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
#define IEEE80211_CHAN_PASSIVE_SCAN IEEE80211_CHAN_NO_IR
#endif

static void rk960_scan_restart_delayed(struct rk960_vif *priv);

#ifdef CONFIG_RK960_TESTMODE
static int rk960_advance_scan_start(struct rk960_common *hw_priv)
{
	int tmo = 0;
	tmo += hw_priv->advanceScanElems.duration;
	rk960_pm_stay_awake(&hw_priv->pm_state, tmo * HZ / 1000);
	/* Invoke Advance Scan Duration Timeout Handler */
	queue_delayed_work(hw_priv->workqueue,
			   &hw_priv->advance_scan_timeout, tmo * HZ / 1000);
	return 0;
}
#endif

static void rk960_remove_wps_p2p_ie(struct wsm_template_frame *frame)
{
	u8 *ies;
	u32 ies_len;
	u32 ie_len;
	u32 p2p_ie_len = 0;
	u32 wps_ie_len = 0;

	ies = &frame->skb->data[sizeof(struct ieee80211_hdr_3addr)];
	ies_len = frame->skb->len - sizeof(struct ieee80211_hdr_3addr);

	while (ies_len >= 6) {
		ie_len = ies[1] + 2;
		if ((ies[0] == WLAN_EID_VENDOR_SPECIFIC)
		    && (ies[2] == 0x00 && ies[3] == 0x50 && ies[4] == 0xf2
			&& ies[5] == 0x04)) {
			wps_ie_len = ie_len;
			memmove(ies, ies + ie_len, ies_len);
			ies_len -= ie_len;

		} else if ((ies[0] == WLAN_EID_VENDOR_SPECIFIC) &&
			   (ies[2] == 0x50 && ies[3] == 0x6f && ies[4] == 0x9a
			    && ies[5] == 0x09)) {
			p2p_ie_len = ie_len;
			memmove(ies, ies + ie_len, ies_len);
			ies_len -= ie_len;
		} else {
			ies += ie_len;
			ies_len -= ie_len;
		}
	}

	if (p2p_ie_len || wps_ie_len) {
		skb_trim(frame->skb,
			 frame->skb->len - (p2p_ie_len + wps_ie_len));
	}
}

#ifdef CONFIG_RK960_TESTMODE
static int rk960_disable_filtering(struct rk960_vif *priv)
{
	int ret = 0;
	bool bssid_filtering = 0;
	struct wsm_rx_filter rx_filter;
	struct wsm_beacon_filter_control bf_control;
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);

	/* RX Filter Disable */
	rx_filter.promiscuous = 0;
	rx_filter.bssid = 0;
	rx_filter.fcs = 0;
	ret = wsm_set_rx_filter(hw_priv, &rx_filter, priv->if_id);

	/* Beacon Filter Disable */
	bf_control.enabled = 0;
	bf_control.bcn_count = 1;
	if (!ret)
		ret = wsm_beacon_filter_control(hw_priv, &bf_control,
						priv->if_id);

	/* BSSID Filter Disable */
	if (!ret)
		ret = wsm_set_bssid_filtering(hw_priv, bssid_filtering,
					      priv->if_id);

	return ret;
}
#endif

static int rk960_scan_start(struct rk960_vif *priv, struct wsm_scan *scan)
{
	int ret, i;
#ifdef FPGA_SETUP
	int tmo = 5000;
#else
	int tmo = 5000;
#endif
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);

	for (i = 0; i < scan->numOfChannels; ++i)
		tmo += scan->ch[i].maxChannelTime + 10;

#ifdef RK960_CSYNC_ADJUST
	rk960_csync_scan(hw_priv);
#endif
	atomic_set(&hw_priv->scan.in_progress, 1);
	atomic_set(&hw_priv->recent_scan, 1);
	rk960_pm_stay_awake(&hw_priv->pm_state, tmo * HZ / 1000);
	queue_delayed_work(hw_priv->workqueue, &hw_priv->scan.timeout,
			   tmo * HZ / 1000);
#ifdef P2P_MULTIVIF
	ret = wsm_scan(hw_priv, scan, priv->if_id ? 1 : 0);
#else
	ret = wsm_scan(hw_priv, scan, priv->if_id);
#endif
	if (unlikely(ret)) {
#ifdef RK960_CSYNC_ADJUST
		rk960_csync_scan_complete(hw_priv);
#endif
		atomic_set(&hw_priv->scan.in_progress, 0);
		cancel_delayed_work_sync(&hw_priv->scan.timeout);
		rk960_scan_restart_delayed(priv);
	}
	return ret;
}

#ifdef ROAM_OFFLOAD
static int rk960_sched_scan_start(struct rk960_vif *priv, struct wsm_scan *scan)
{
	int ret;
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);

	ret = wsm_scan(hw_priv, scan, priv->if_id);
	if (unlikely(ret)) {
#ifdef RK960_CSYNC_ADJUST
		rk960_csync_scan_complete(hw_priv);
#endif
		atomic_set(&hw_priv->scan.in_progress, 0);
	}
	return ret;
}
#endif /*ROAM_OFFLOAD */

int rk960_hw_scan(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
		  struct ieee80211_scan_request *hw_req)
#else
		  struct cfg80211_scan_request *req)
#endif
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
	struct cfg80211_scan_request *req = &hw_req->req;
#endif
	struct rk960_common *hw_priv = hw->priv;
	struct rk960_vif *priv = rk960_get_vif_from_ieee80211(vif);
	struct wsm_template_frame frame = {
		.frame_type = WSM_FRAME_TYPE_PROBE_REQUEST,
	};
	int i;
	int ret = 0;
#ifdef CONFIG_RK960_TESTMODE
	u16 advance_scan_req_channel;
#endif

	/* Scan when P2P_GO corrupt firmware MiniAP mode */
	if (priv->join_status == RK960_JOIN_STATUS_AP)
		return -EOPNOTSUPP;

#ifdef ROAM_OFFLOAD
	if (hw_priv->scan.sched_req) {
		RK960_DEBUG_SCAN("%s: sched_scan inprocess\n", __func__);
		return -EBUSY;
	}
#endif
	RK960_DEBUG_SCAN("[SCAN] Scan request for %d chs %d SSIDs no_cck %d.\n",
			 req->n_channels, req->n_ssids, req->no_cck);

#ifdef SUPPORT_FWCR
        if (priv->join_status < RK960_JOIN_STATUS_STA)
                rk960_fwcr_deinit(hw_priv);
        
        if (!rk960_fwcr_recovery_scan(hw_priv, priv))
                return 0;
#endif

	if (work_pending(&priv->offchannel_work) ||
	    (hw_priv->roc_if_id != -1) ||
	    (!req->no_cck && delayed_work_pending(&priv->join_timeout))) {
		RK960_INFO_SCAN("[SCAN] Offchannel work pending, "
				"ignoring scan work %d\n", hw_priv->roc_if_id);
		return -EBUSY;
	}

	if (req->n_ssids == 1 && !req->ssids[0].ssid_len)
		req->n_ssids = 0;

	if (req->n_ssids > hw->wiphy->max_scan_ssids)
		return -EINVAL;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0))
	frame.skb = ieee80211_probereq_get(hw, priv->vif->addr, NULL, 0,
					   req->ie_len);
#else
	frame.skb = ieee80211_probereq_get(hw, vif, NULL, 0, req->ie_len);
#endif
	if (req->ie_len)
		memcpy(skb_put(frame.skb, req->ie_len), req->ie, req->ie_len);
#else
	frame.skb = ieee80211_probereq_get(hw, vif, NULL, 0,
					   req->ie, req->ie_len);
#endif
	if (!frame.skb)
		return -ENOMEM;

	hw_priv->scan.in_txrx_burst = rk960_is_txrx_burst(hw_priv);

	/* will be unlocked in rk960_scan_work() */
	down(&hw_priv->scan.lock);
	mutex_lock(&hw_priv->conf_mutex);
#ifdef CONFIG_RK960_TESTMODE
	/* Active Scan - Serving Channel Request Handling */
	advance_scan_req_channel = req->channels[0]->hw_value;
	if (hw_priv->enable_advance_scan &&
	    (hw_priv->advanceScanElems.scanMode ==
	     RK960_SCAN_MEASUREMENT_ACTIVE) &&
	    (priv->join_status == RK960_JOIN_STATUS_STA) &&
	    (hw_priv->channel->hw_value == advance_scan_req_channel)) {
		BUG_ON(hw_priv->scan.req);
		/* wsm_lock_tx(hw_priv); */
		wsm_vif_lock_tx(priv);
		hw_priv->scan.if_id = priv->if_id;
		/* Disable Power Save */
		if (priv->powersave_mode.pmMode & WSM_PSM_PS) {
			struct wsm_set_pm pm = priv->powersave_mode;
			pm.pmMode = WSM_PSM_ACTIVE;
			wsm_set_pm(hw_priv, &pm, priv->if_id);
		}
		/* Disable Rx Beacon and Bssid filter */
		ret = rk960_disable_filtering(priv);
		if (ret)
			RK960_ERROR_SCAN
			    ("%s: Disable BSSID or Beacon filtering failed: %d.\n",
			     __func__, ret);
		wsm_unlock_tx(hw_priv);
		mutex_unlock(&hw_priv->conf_mutex);
		/* Transmit Probe Request with Broadcast SSID */
		rk960_tx(hw, frame.skb);
		/* Start Advance Scan Timer */
		rk960_advance_scan_start(hw_priv);
	} else {
#endif
		if (frame.skb) {
			if (priv->if_id == 0)
				rk960_remove_wps_p2p_ie(&frame);

#ifdef P2P_MULTIVIF
			ret = wsm_set_template_frame(hw_priv, &frame,
						     priv->if_id ? 1 : 0);
#else
			ret = wsm_set_template_frame(hw_priv, &frame,
						     priv->if_id);
#endif
			if (ret) {
				mutex_unlock(&hw_priv->conf_mutex);
				up(&hw_priv->scan.lock);
				dev_kfree_skb(frame.skb);
				return ret;
			}
		}

		wsm_vif_lock_tx(priv);

		BUG_ON(hw_priv->scan.req);
		hw_priv->scan.req = req;
		hw_priv->scan.n_ssids = 0;
		hw_priv->scan.status = 0;
		hw_priv->scan.begin = &req->channels[0];
		hw_priv->scan.curr = hw_priv->scan.begin;
		hw_priv->scan.end = &req->channels[req->n_channels];
		hw_priv->scan.output_power = hw_priv->output_power;
		hw_priv->scan.if_id = priv->if_id;
		hw_priv->scan.p2p_probe = req->no_cck;
		/* TODO:COMBO: Populate BIT4 in scanflags to decide on which MAC
		 * address the SCAN request will be sent */

		for (i = 0; i < req->n_ssids; ++i) {
			struct wsm_ssid *dst =
			    &hw_priv->scan.ssids[hw_priv->scan.n_ssids];
			BUG_ON(req->ssids[i].ssid_len > sizeof(dst->ssid));
			memcpy(&dst->ssid[0], req->ssids[i].ssid,
			       sizeof(dst->ssid));
			dst->length = req->ssids[i].ssid_len;
			++hw_priv->scan.n_ssids;
			RK960_DEBUG_SCAN("ssid%d %s\n", i, req->ssids[i].ssid);
		}

		mutex_unlock(&hw_priv->conf_mutex);

		hw_priv->scan_counter++;
		RK960_DEBUG_SCAN("scan_start: %d\n", hw_priv->scan_counter);
		if (frame.skb)
			dev_kfree_skb(frame.skb);
		queue_work(hw_priv->workqueue, &hw_priv->scan.work);
#ifdef CONFIG_RK960_TESTMODE
	}
#endif
	return 0;
}

#ifdef ROAM_OFFLOAD
int rk960_hw_sched_scan_start(struct ieee80211_hw *hw,
			      struct ieee80211_vif *vif,
			      struct cfg80211_sched_scan_request *req,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
			      struct ieee80211_scan_ies *ies)
#else
			      struct ieee80211_sched_scan_ies *ies)
#endif
{
	struct rk960_common *hw_priv = hw->priv;
	struct rk960_vif *priv = rk960_get_vif_from_ieee80211(vif);
	struct wsm_template_frame frame = {
		.frame_type = WSM_FRAME_TYPE_PROBE_REQUEST,
	};
	int i;

	if (hw_priv->wowlan_disable) {
		RK960_DEBUG_SCAN("%s: EOPNOTSUPP\n", __func__);
		return -EOPNOTSUPP;
	}

	if (hw_priv->scan.req) {
		RK960_DEBUG_SCAN("%s: scan inprocess\n", __func__);
		return -EBUSY;
	}

	RK960_INFO_SCAN("[SCAN] Scheduled scan request-->.\n");

	if (!priv->vif)
		return -EINVAL;

	/* Scan when P2P_GO corrupt firmware MiniAP mode */
	if (priv->join_status == RK960_JOIN_STATUS_AP)
		return -EOPNOTSUPP;

	RK960_DEBUG_SCAN
	    ("[SCAN] Scheduled scan: n_ssids %d, ssid[0].len = %d\n",
	     req->n_ssids, req->ssids[0].ssid_len);
	RK960_DEBUG_SCAN("       n_channels = %d\n", req->n_channels);
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 10, 0))
	RK960_DEBUG_SCAN("       interval = %d\n", req->interval);
#endif
	RK960_DEBUG_SCAN("       ie_len = %d\n", req->ie_len);
	RK960_DEBUG_SCAN("       flags = %x\n", req->flags);
	RK960_DEBUG_SCAN("       n_match_sets = %x\n", req->n_match_sets);
	for (i = 0; i < req->n_match_sets; i++) {
		if (req->n_match_sets)
			RK960_DEBUG_SCAN("       ssid %s rssi_thold = %d\n",
					 req->match_sets[i].ssid,
					 req->match_sets[i].rssi_thold);
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
	RK960_DEBUG_SCAN("       interval = %x\n", req->scan_plans[0].interval);
	RK960_DEBUG_SCAN("       relative_rssi_set = %x\n",
			 req->relative_rssi_set);
	RK960_DEBUG_SCAN("       relative_rssi = %x\n", req->relative_rssi);
	RK960_DEBUG_SCAN("       rssi_adjust = %x\n", req->rssi_adjust);
#else
	RK960_DEBUG_SCAN("       min_rssi_thold = %x\n", req->min_rssi_thold);
#endif

	if (req->n_ssids == 1 && !req->ssids[0].ssid_len)
		req->n_ssids = 0;

	RK960_DEBUG_SCAN("[SCAN] Scan request for %d SSIDs.\n", req->n_ssids);

	if (req->n_ssids > hw->wiphy->max_scan_ssids)
		return -EINVAL;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 19, 0))
	frame.skb = ieee80211_probereq_get(hw, priv->vif->addr, NULL, 0,
					   ies->len[0] + ies->common_ie_len);
	if (ies->len[0])
		memcpy(skb_put(frame.skb, ies->len[0]), ies->ies[0],
		       ies->len[0]);
	if (ies->common_ie_len)
		memcpy(skb_put(frame.skb, ies->common_ie_len),
		       ies->common_ies, ies->common_ie_len);
#else
	frame.skb = ieee80211_probereq_get(hw, vif, NULL, 0, ies->len[0]);
	if (ies->len[0])
		memcpy(skb_put(frame.skb, ies->len[0]), ies->ie[0],
		       ies->len[0]);
#endif
#else
	frame.skb = ieee80211_probereq_get(hw, priv->vif, NULL, 0,
					   ies->ie[0], ies->len[0]);
#endif
	if (!frame.skb)
		return -ENOMEM;

	if (req->channels[0]->band == IEEE80211_BAND_2GHZ)
		hw_priv->num_scanchannels = 0;
	else
		hw_priv->num_scanchannels = hw_priv->num_2g_channels;

	for (i = 0; i < req->n_channels; i++) {
		hw_priv->scan_channels[hw_priv->num_scanchannels + i].number =
		    req->channels[i]->hw_value;
		if (req->channels[i]->flags & IEEE80211_CHAN_PASSIVE_SCAN) {
			hw_priv->scan_channels[hw_priv->num_scanchannels +
					       i].minChannelTime = 50;
			hw_priv->scan_channels[hw_priv->num_scanchannels +
					       i].maxChannelTime = 110;
		} else {
			hw_priv->scan_channels[hw_priv->num_scanchannels +
					       i].minChannelTime = 10;
			hw_priv->scan_channels[hw_priv->num_scanchannels +
					       i].maxChannelTime = 40;
		}
		hw_priv->scan_channels[hw_priv->num_scanchannels +
				       i].txPowerLevel =
		    req->channels[i]->max_power;
	}
	if (req->channels[0]->band == IEEE80211_BAND_2GHZ)
		hw_priv->num_2g_channels = req->n_channels;
	else
		hw_priv->num_5g_channels = req->n_channels;
	hw_priv->num_scanchannels =
	    hw_priv->num_2g_channels + hw_priv->num_5g_channels;

	/* will be unlocked in rk960_scan_work() */
	down(&hw_priv->scan.lock);
	mutex_lock(&hw_priv->conf_mutex);
	if (frame.skb) {
		int ret;
		if (priv->if_id == 0)
			rk960_remove_wps_p2p_ie(&frame);
		ret = wsm_set_template_frame(hw_priv, &frame, priv->if_id);
		if (0 == ret) {
			/* Host want to be the probe responder. */
			ret = wsm_set_probe_responder(priv, true);
		}
		if (ret) {
			mutex_unlock(&hw_priv->conf_mutex);
			up(&hw_priv->scan.lock);
			dev_kfree_skb(frame.skb);
			return ret;
		}
	}

	wsm_lock_tx(hw_priv);

	BUG_ON(hw_priv->scan.req);
	hw_priv->scan.sched_req = req;
	hw_priv->scan.n_ssids = 0;
	hw_priv->scan.status = 0;
	hw_priv->scan.begin = &req->channels[0];
	hw_priv->scan.curr = hw_priv->scan.begin;
	hw_priv->scan.end = &req->channels[req->n_channels];
	hw_priv->scan.output_power = hw_priv->output_power;

	for (i = 0; i < req->n_ssids; ++i) {
		struct wsm_ssid *dst =
		    &hw_priv->scan.ssids[hw_priv->scan.n_ssids];
		if (req->ssids[i].ssid_len > sizeof(dst->ssid))
			req->ssids[i].ssid_len = sizeof(dst->ssid);
		memcpy(&dst->ssid[0], req->ssids[i].ssid, sizeof(dst->ssid));
		dst->length = req->ssids[i].ssid_len;
		++hw_priv->scan.n_ssids;
		{
			u8 j;
			RK960_DEBUG_SCAN("[SCAN] SSID %d\n", i);
			for (j = 0; j < req->ssids[i].ssid_len; j++)
				RK960_DEBUG_SCAN("[SCAN] 0x%x\n",
						 req->ssids[i].ssid[j]);
		}
	}

	mutex_unlock(&hw_priv->conf_mutex);

	if (frame.skb)
		dev_kfree_skb(frame.skb);
	queue_work(hw_priv->workqueue, &hw_priv->scan.swork);
	RK960_DEBUG_SCAN("<--[SCAN] Scheduled scan request.\n");
	return 0;
}
#endif /*ROAM_OFFLOAD */

#define SCAN_TIME_ADJUST
void rk960_scan_work(struct work_struct *work)
{
	struct rk960_common *hw_priv = container_of(work,
						    struct rk960_common,
						    scan.work);
	struct rk960_vif *priv, *vif;
	struct ieee80211_channel **it;
	struct wsm_scan scan = {
		.scanType = WSM_SCAN_TYPE_FOREGROUND,
		.scanFlags = 0,	/* TODO:COMBO */
		//.scanFlags = WSM_SCAN_FLAG_SPLIT_METHOD, /* TODO:COMBO */
	};
	bool first_run;
	int i;
	const u32 ProbeRequestTime = 2;
	const u32 ChannelRemainTime = 15;
	u32 maxChannelTime;
#ifdef CONFIG_RK960_TESTMODE
	int ret = 0;
	u16 advance_scan_req_channel = hw_priv->scan.begin[0]->hw_value;
#endif
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
	struct cfg80211_scan_info info = {
		.aborted = false,
	};
#endif

	priv = __rk960_hwpriv_to_vifpriv(hw_priv, hw_priv->scan.if_id);

	/*TODO: COMBO: introduce locking so vif is not removed in meanwhile */

	if (!priv) {
		RK960_INFO_SCAN("[SCAN] interface removed, "
				"ignoring scan work\n");
		return;
	}

	if (priv->if_id)
		scan.scanFlags |= WSM_FLAG_MAC_INSTANCE_1;
	else
		scan.scanFlags &= ~WSM_FLAG_MAC_INSTANCE_1;

	//rk960_for_each_vif(hw_priv, vif, i) {
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		vif = hw_priv->vif_list[i] ?
		    rk960_get_vif_from_ieee80211(hw_priv->vif_list[i]) : NULL;
		if (!vif)
			continue;
		if (vif->bss_loss_status > RK960_BSS_LOSS_NONE)
			scan.scanFlags |= WSM_SCAN_FLAG_FORCE_BACKGROUND;
	}

	first_run = hw_priv->scan.begin == hw_priv->scan.curr &&
	    hw_priv->scan.begin != hw_priv->scan.end;

	if (first_run) {
		/* Firmware gets crazy if scan request is sent
		 * when STA is joined but not yet associated.
		 * Force unjoin in this case. */
		if (cancel_delayed_work_sync(&priv->join_timeout) > 0)
			rk960_join_timeout(&priv->join_timeout.work);
	}

	mutex_lock(&hw_priv->conf_mutex);

	if (first_run) {
#ifdef CONFIG_RK960_TESTMODE
		/* Passive Scan - Serving Channel Request Handling */
		if (hw_priv->enable_advance_scan &&
		    (hw_priv->advanceScanElems.scanMode ==
		     RK960_SCAN_MEASUREMENT_PASSIVE) &&
		    (priv->join_status == RK960_JOIN_STATUS_STA) &&
		    (hw_priv->channel->hw_value == advance_scan_req_channel)) {
			/* If Advance Scan Request is for Serving Channel Device
			 * should be Active and Filtering Should be Disable */
			if (priv->powersave_mode.pmMode & WSM_PSM_PS) {
				struct wsm_set_pm pm = priv->powersave_mode;
				pm.pmMode = WSM_PSM_ACTIVE;
				wsm_set_pm(hw_priv, &pm, priv->if_id);
			}
			/* Disable Rx Beacon and Bssid filter */
			ret = rk960_disable_filtering(priv);
			if (ret)
				RK960_ERROR_SCAN
				    ("%s: Disable BSSID or Beacon filtering failed: %d.\n",
				     __func__, ret);
		} else if (hw_priv->enable_advance_scan
			   && (hw_priv->advanceScanElems.scanMode ==
			       RK960_SCAN_MEASUREMENT_PASSIVE)
			   && (priv->join_status == RK960_JOIN_STATUS_STA)) {
			if (priv->join_status == RK960_JOIN_STATUS_STA
			    && !(priv->powersave_mode.pmMode & WSM_PSM_PS)) {
				struct wsm_set_pm pm = priv->powersave_mode;
				pm.pmMode = WSM_PSM_PS;
				rk960_set_pm(priv, &pm);
			}
		} else {
#endif
#if 0
			if (priv->join_status == RK960_JOIN_STATUS_STA &&
			    !(priv->powersave_mode.pmMode & WSM_PSM_PS)) {
				struct wsm_set_pm pm = priv->powersave_mode;
				pm.pmMode = WSM_PSM_PS;
				rk960_set_pm(priv, &pm);
			} else
#endif
			if (priv->join_status == RK960_JOIN_STATUS_MONITOR) {
				/* FW bug: driver has to restart p2p-dev mode
				 * after scan */
				rk960_disable_listening(priv);
			}
#ifdef CONFIG_RK960_TESTMODE
		}
#endif
	}

	if (!hw_priv->scan.req || (hw_priv->scan.curr == hw_priv->scan.end)) {
#ifdef CONFIG_RK960_TESTMODE
		if (hw_priv->enable_advance_scan &&
		    (hw_priv->advanceScanElems.scanMode ==
		     RK960_SCAN_MEASUREMENT_PASSIVE) &&
		    (priv->join_status == RK960_JOIN_STATUS_STA) &&
		    (hw_priv->channel->hw_value == advance_scan_req_channel)) {
			/* WSM Lock should be held here for WSM APIs */
			wsm_vif_lock_tx(priv);
			/* wsm_lock_tx(priv); */
			/* Once Duration is Over, enable filtering
			 * and Revert Back Power Save */
			if (priv->powersave_mode.pmMode & WSM_PSM_PS)
				wsm_set_pm(hw_priv, &priv->powersave_mode,
					   priv->if_id);
			rk960_update_filtering(priv);
		} else {
			if (!hw_priv->enable_advance_scan) {
#endif
				if (hw_priv->scan.output_power !=
				    hw_priv->output_power)
					/* TODO:COMBO: Change when mac80211 implementation
					 * is available for output power also */
#ifdef P2P_MULTIVIF
					WARN_ON(wsm_set_output_power(hw_priv,
						hw_priv->output_power * 10,
						priv->if_id ? 1 : 0));
#else
					WARN_ON(wsm_set_output_power(hw_priv,
						hw_priv->output_power * 10,
						priv->if_id));
#endif
#ifdef CONFIG_RK960_TESTMODE
			}
		}
#endif
#if 0
		if (priv->join_status == RK960_JOIN_STATUS_STA &&
		    !(priv->powersave_mode.pmMode & WSM_PSM_PS))
			rk960_set_pm(priv, &priv->powersave_mode);
#endif
		if (hw_priv->scan.status < 0)
			RK960_ERROR_SCAN("[SCAN] Scan failed (%d).\n",
					 hw_priv->scan.status);
		else if (hw_priv->scan.req)
			RK960_DEBUG_SCAN("[SCAN] Scan completed.\n");
		else
			RK960_INFO_SCAN("[SCAN] Scan canceled.\n");

		hw_priv->scan.in_txrx_burst = false;
		hw_priv->scan.req = NULL;
		rk960_scan_restart_delayed(priv);
#ifdef CONFIG_RK960_TESTMODE
		hw_priv->enable_advance_scan = false;
#endif /* CONFIG_RK960_TESTMODE */
		wsm_unlock_tx(hw_priv);
		mutex_unlock(&hw_priv->conf_mutex);
		hw_priv->scan_comp_counter++;
		RK960_DEBUG_SCAN("scan_completed: %d(%d)\n",
				 hw_priv->scan_comp_counter,
				 hw_priv->scan.status);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
		if (hw_priv->scan.status)
			info.aborted = true;
		ieee80211_scan_completed(hw_priv->hw, &info);
#else
		ieee80211_scan_completed(hw_priv->hw,
					 hw_priv->scan.status ? 1 : 0);
#endif
		up(&hw_priv->scan.lock);
		return;
	} else {
		struct ieee80211_channel *first = *hw_priv->scan.curr;
		for (it = hw_priv->scan.curr + 1, i = 1;
		     it != hw_priv->scan.end &&
		     i < WSM_SCAN_MAX_NUM_OF_CHANNELS; ++it, ++i) {
			if ((*it)->band != first->band)
				break;
			if (((*it)->flags ^ first->flags) &
			    IEEE80211_CHAN_PASSIVE_SCAN)
				break;
			if (!(first->flags & IEEE80211_CHAN_PASSIVE_SCAN) &&
			    (*it)->max_power != first->max_power)
				break;
		}
		scan.band = first->band;

		if (hw_priv->scan.req->no_cck)
			scan.maxTransmitRate = WSM_TRANSMIT_RATE_6;
		else
			scan.maxTransmitRate = WSM_TRANSMIT_RATE_1;
#ifdef CONFIG_RK960_TESTMODE
		if (hw_priv->enable_advance_scan) {
			if (hw_priv->advanceScanElems.scanMode ==
			    RK960_SCAN_MEASUREMENT_PASSIVE)
				scan.numOfProbeRequests = 0;
			else
				scan.numOfProbeRequests = 1;
		} else {
#endif
			/* TODO: Is it optimal? */
			scan.numOfProbeRequests =
			    (first->
			     flags & IEEE80211_CHAN_PASSIVE_SCAN) ? 0 : 3;
#ifdef CONFIG_RK960_TESTMODE
		}
#endif /* CONFIG_RK960_TESTMODE */
		scan.numOfSSIDs = hw_priv->scan.n_ssids;
		scan.ssids = &hw_priv->scan.ssids[0];
		scan.numOfChannels = it - hw_priv->scan.curr;
		/* TODO: Is it optimal? */
#if 0				//def SCAN_TIME_ADJUST
		scan.probeDelay = 10;
		if (hw_priv->scan.req->no_cck) {
			scan.numOfProbeRequests = 3;
			scan.probeDelay = 15;
		}
#else
		scan.probeDelay = 100;
#endif

		/* It is not stated in WSM specification, however
		 * FW team says that driver may not use FG scan
		 * when joined. */
		if (priv->join_status == RK960_JOIN_STATUS_STA) {
			scan.scanType = WSM_SCAN_TYPE_BACKGROUND;
			scan.scanFlags = WSM_SCAN_FLAG_FORCE_BACKGROUND;
		}
		scan.ch =
		    kzalloc(sizeof(struct wsm_scan_ch) *
			    (it - hw_priv->scan.curr), GFP_KERNEL);
		if (!scan.ch) {
			hw_priv->scan.status = -ENOMEM;
			goto fail;
		}
		maxChannelTime = (scan.numOfSSIDs * scan.numOfProbeRequests *
				  ProbeRequestTime) + ChannelRemainTime;
		maxChannelTime = (maxChannelTime < 35) ? 35 : maxChannelTime;
		for (i = 0; i < scan.numOfChannels; ++i) {
			scan.ch[i].number = hw_priv->scan.curr[i]->hw_value;
#ifdef CONFIG_RK960_TESTMODE
			if (hw_priv->enable_advance_scan) {
				scan.ch[i].minChannelTime =
				    hw_priv->advanceScanElems.duration;
				scan.ch[i].maxChannelTime =
				    hw_priv->advanceScanElems.duration;
			} else {
#endif
#ifdef SCAN_TIME_ADJUST
				if (hw_priv->scan.req->no_cck) {
					/* decrease channel time in p2p search
					   to solve go-neg action hard to receive issue
					   which cause user confirm UI eject slowly */
#if 1
					scan.ch[i].minChannelTime = 5;
					scan.ch[i].maxChannelTime = 10;
#else
					scan.ch[i].minChannelTime = 35;
					scan.ch[i].maxChannelTime =
					    maxChannelTime;
#endif
				} else if (hw_priv->scan.in_txrx_burst) {
					scan.ch[i].minChannelTime = 10;
					scan.ch[i].maxChannelTime = 15;
                                } else if (rk960_is_apsta_cowork(hw_priv)) {
                                        scan.ch[i].minChannelTime = 10;
                                        scan.ch[i].maxChannelTime = 32;
				} else {
					scan.ch[i].minChannelTime = 50;
                                        if (hw_priv->last_channel ==
                                                scan.ch[i].number)
                                                scan.ch[i].maxChannelTime = 210;
                                        else
					        scan.ch[i].maxChannelTime = 110;
				}
#else
				if (hw_priv->scan.curr[i]->
				    flags & IEEE80211_CHAN_PASSIVE_SCAN) {
					scan.ch[i].minChannelTime = 50;
					scan.ch[i].maxChannelTime = 110;
				} else {
					scan.ch[i].minChannelTime = 15;
					scan.ch[i].maxChannelTime =
					    maxChannelTime;
				}
#endif
#ifdef CONFIG_RK960_TESTMODE
			}
#endif
		}
#if 0
#ifdef CONFIG_RK960_TESTMODE
		if (!hw_priv->enable_advance_scan) {
#endif
			if (!(first->flags & IEEE80211_CHAN_PASSIVE_SCAN) &&
			    hw_priv->scan.output_power != first->max_power) {
				hw_priv->scan.output_power = first->max_power;
				/* TODO:COMBO: Change after mac80211 implementation
				 * complete */
#ifdef P2P_MULTIVIF
				WARN_ON(wsm_set_output_power(hw_priv,
						hw_priv->scan.output_power * 10,
						priv->if_id ? 1 : 0));
#else
				WARN_ON(wsm_set_output_power(hw_priv,
						hw_priv->scan.output_power * 10,
						priv->if_id));
#endif
			}
#ifdef CONFIG_RK960_TESTMODE
		}
#endif
#endif
#ifdef CONFIG_RK960_TESTMODE
		if (hw_priv->enable_advance_scan &&
		    (hw_priv->advanceScanElems.scanMode ==
		     RK960_SCAN_MEASUREMENT_PASSIVE) &&
		    (priv->join_status == RK960_JOIN_STATUS_STA) &&
		    (hw_priv->channel->hw_value == advance_scan_req_channel)) {
			/* Start Advance Scan Timer */
			hw_priv->scan.status =
			    rk960_advance_scan_start(hw_priv);
			wsm_unlock_tx(hw_priv);
		} else
#endif
			hw_priv->scan.status = rk960_scan_start(priv, &scan);
		kfree(scan.ch);
		if (WARN_ON(hw_priv->scan.status))
			goto fail;
		hw_priv->scan.curr = it;
	}
	mutex_unlock(&hw_priv->conf_mutex);
	return;

fail:
	hw_priv->scan.curr = hw_priv->scan.end;
	mutex_unlock(&hw_priv->conf_mutex);
	queue_work(hw_priv->workqueue, &hw_priv->scan.work);
	return;
}

#ifdef ROAM_OFFLOAD
void rk960_sched_scan_work(struct work_struct *work)
{
	struct rk960_common *hw_priv = container_of(work, struct rk960_common,
						    scan.swork);
	struct wsm_scan scan;
	struct cfg80211_sched_scan_request *req;
	struct wsm_ssid scan_ssid[WSM_SCAN_MAX_MATCH_SETS_NUM_OF_SSIDS];
	int i;
	struct rk960_vif *priv = rk960_hwpriv_to_vifpriv(hw_priv,
							 hw_priv->scan.if_id);
	if (unlikely(!priv)) {
		WARN_ON(1);
		return;
	}

	rk960_priv_vif_list_read_unlock(&priv->vif_lock);

	/* Firmware gets crazy if scan request is sent
	 * when STA is joined but not yet associated.
	 * Force unjoin in this case. */
	if (cancel_delayed_work_sync(&priv->join_timeout) > 0) {
		rk960_join_timeout(&priv->join_timeout.work);
	}
	mutex_lock(&hw_priv->conf_mutex);
	hw_priv->auto_scanning = 1;

	scan.band = 0;

	if (priv->join_status == RK960_JOIN_STATUS_STA)
		scan.scanType = 3;	/* auto background */
	else
		scan.scanType = 2;	/* auto foreground */

	scan.scanFlags = 0x01;	/* bit 0 set => forced background scan */
	scan.maxTransmitRate = WSM_TRANSMIT_RATE_6;
	scan.autoScanInterval = (0xba << 24) | (1 * 1024);	/* 1 seconds, -70 rssi */
	scan.numOfProbeRequests = 1;
	//scan.numOfChannels = 11;
	scan.numOfChannels = hw_priv->num_scanchannels;
	scan.probeDelay = 100;

	if (!hw_priv->scan.sched_req) {
		scan.numOfSSIDs = 1;
		scan_ssid[0].length = priv->ssid_length;
		memcpy(scan_ssid[0].ssid, priv->ssid, priv->ssid_length);
		scan.ssids = &scan_ssid[0];
	} else {
		req = hw_priv->scan.sched_req;
		scan.numOfSSIDs = req->n_match_sets;
		memset(&scan_ssid, 0, sizeof(scan_ssid));
		for (i = 0; i < req->n_match_sets; i++) {
			if (req->match_sets[i].ssid.ssid_len > 32) {
				req->match_sets[i].ssid.ssid_len = 32;
			}
			memcpy(scan_ssid[i].ssid, req->match_sets[i].ssid.ssid,
			       req->match_sets[i].ssid.ssid_len);
			scan_ssid[i].length = req->match_sets[i].ssid.ssid_len;
		}
		scan.ssids = &scan_ssid[0];
	}

	scan.ch = kzalloc(sizeof(struct wsm_scan_ch) * scan.numOfChannels,
			  GFP_KERNEL);
	if (!scan.ch) {
		hw_priv->scan.status = -ENOMEM;
		goto fail;
	}

	for (i = 0; i < scan.numOfChannels; i++) {
		scan.ch[i].number = hw_priv->scan_channels[i].number;
		scan.ch[i].minChannelTime =
		    hw_priv->scan_channels[i].minChannelTime;
		scan.ch[i].maxChannelTime =
		    hw_priv->scan_channels[i].maxChannelTime;
		scan.ch[i].txPowerLevel =
		    hw_priv->scan_channels[i].txPowerLevel;
	}

#if 0
	for (i = 1; i <= scan.numOfChannels; i++) {
		scan.ch[i - 1].number = i;
		scan.ch[i - 1].minChannelTime = 10;
		scan.ch[i - 1].maxChannelTime = 40;
	}
#endif

	hw_priv->scan.status = rk960_sched_scan_start(priv, &scan);
	kfree(scan.ch);
	if (hw_priv->scan.status)
		goto fail;
	mutex_unlock(&hw_priv->conf_mutex);
	return;

fail:
	RK960_ERROR_SCAN("%s: fail %d\n", __func__, hw_priv->scan.status);
	mutex_unlock(&hw_priv->conf_mutex);
	queue_work(hw_priv->workqueue, &hw_priv->scan.swork);
	return;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
int
#else
void
#endif
rk960_hw_sched_scan_stop(struct ieee80211_hw *hw, struct ieee80211_vif *vif)
{
	struct rk960_common *hw_priv = hw->priv;

	mutex_lock(&hw_priv->conf_mutex);

	if (!hw_priv->auto_scanning) {
		mutex_unlock(&hw_priv->conf_mutex);
		return 0;
	}

	RK960_INFO_SCAN("[SCAN] Scheduled scan stop-->.\n");
	_rk960_hw_sched_scan_stop(hw_priv);

	hw_priv->scan.sched_req = NULL;
	if (hw_priv->auto_scanning) {
		hw_priv->auto_scanning = 0;
		ieee80211_sched_scan_results(hw_priv->hw);
	}

	up(&hw_priv->scan.lock);
	wsm_unlock_tx(hw_priv);

	mutex_unlock(&hw_priv->conf_mutex);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
	return 0;
#endif
}

void _rk960_hw_sched_scan_stop(struct rk960_common *hw_priv)
{
	struct rk960_vif *priv = rk960_hwpriv_to_vifpriv(hw_priv,
							 hw_priv->scan.if_id);
	if (unlikely(!priv))
		return;

	rk960_priv_vif_list_read_unlock(&priv->vif_lock);

	wsm_stop_scan(hw_priv, priv->if_id);

	return;
}

void rk960_sched_scan_stop_work(struct work_struct *work)
{
	struct rk960_common *hw_priv = container_of(work, struct rk960_common,
						    scan.sched_scan_stop_work.
						    work);

	rk960_hw_sched_scan_stop(hw_priv->hw, NULL);
}
#else
void rk960_sched_scan_stop_work(struct work_struct *work)
{
}
#endif /*ROAM_OFFLOAD */

static void rk960_scan_restart_delayed(struct rk960_vif *priv)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);

	if (priv->delayed_link_loss) {
		int tmo = priv->cqm_beacon_loss_count;

		if (hw_priv->scan.direct_probe)
			tmo = 0;

		priv->delayed_link_loss = 0;
		/* Restart beacon loss timer and requeue
		   BSS loss work. */
		RK960_DEBUG_SCAN("[CQM] Requeue BSS loss in %d "
				 "beacons.\n", tmo);
		spin_lock(&priv->bss_loss_lock);
		priv->bss_loss_status = RK960_BSS_LOSS_NONE;
		spin_unlock(&priv->bss_loss_lock);
		cancel_delayed_work_sync(&priv->bss_loss_work);
		queue_delayed_work(hw_priv->workqueue,
				   &priv->bss_loss_work, tmo * HZ / 10);
	}

	/* FW bug: driver has to restart p2p-dev mode after scan. */
	if (priv->join_status == RK960_JOIN_STATUS_MONITOR) {
		/*rk960_enable_listening(priv); */
		WARN_ON(1);
		rk960_update_filtering(priv);
	}

	if (hw_priv->delayed_unjoin_if_id != -1) {
		struct rk960_vif *priv2 = rk960_hwpriv_to_vifpriv(hw_priv,
								  hw_priv->
								  delayed_unjoin_if_id);

		hw_priv->delayed_unjoin_if_id = -1;
		if (priv2) {
			rk960_priv_vif_list_read_unlock(&priv2->vif_lock);
			if (priv2->delayed_unjoin) {
				priv2->delayed_unjoin = false;
				RK960_DEBUG_SCAN("%s: unjoin_work %d\n",
						 __func__, priv2->if_id);
				if (queue_work
				    (hw_priv->workqueue,
				     &priv2->unjoin_work) <= 0)
					wsm_unlock_tx(hw_priv);
			}
		}
	}
}

static void rk960_scan_complete(struct rk960_common *hw_priv, int if_id)
{
	struct rk960_vif *priv;
	atomic_xchg(&hw_priv->recent_scan, 0);

	if (hw_priv->scan.direct_probe) {
		mutex_lock(&hw_priv->conf_mutex);
		priv = __rk960_hwpriv_to_vifpriv(hw_priv, if_id);
		if (priv) {
			RK960_DEBUG_SCAN("[SCAN] Direct probe " "complete.\n");
			rk960_scan_restart_delayed(priv);
		} else {
			RK960_DEBUG_SCAN("[SCAN] Direct probe "
					 "complete without interface!\n");
		}
		mutex_unlock(&hw_priv->conf_mutex);
		hw_priv->scan.direct_probe = 0;
		up(&hw_priv->scan.lock);
		wsm_unlock_tx(hw_priv);
	} else {
		rk960_scan_work(&hw_priv->scan.work);
	}
}

void rk960_scan_complete_cb(struct rk960_common *hw_priv,
			    struct wsm_scan_complete *arg)
{
	struct rk960_vif *priv = rk960_hwpriv_to_vifpriv(hw_priv,
							 hw_priv->scan.if_id);

	if (unlikely(!priv))
		return;

#ifdef ROAM_OFFLOAD
	if (hw_priv->auto_scanning)
		queue_delayed_work(hw_priv->workqueue,
				   &hw_priv->scan.timeout, 0);
#endif /*ROAM_OFFLOAD */

	if (unlikely(priv->mode == NL80211_IFTYPE_UNSPECIFIED)) {
		/* STA is stopped. */
		rk960_priv_vif_list_read_unlock(&priv->vif_lock);
		return;
	}
	rk960_priv_vif_list_read_unlock(&priv->vif_lock);

	if (hw_priv->scan.status == -ETIMEDOUT)
		RK960_INFO_SCAN
		    ("Scan timeout already occured. Don't cancel work");
	if ((hw_priv->scan.status != -ETIMEDOUT)
	    && (cancel_delayed_work_sync(&hw_priv->scan.timeout) > 0)) {
		hw_priv->scan.status = 1;
		queue_delayed_work(hw_priv->workqueue,
				   &hw_priv->scan.timeout, 0);
	}
}

void rk960_scan_timeout(struct work_struct *work)
{
	struct rk960_common *hw_priv =
	    container_of(work, struct rk960_common, scan.timeout.work);

#ifdef RK960_CSYNC_ADJUST
	rk960_csync_scan_complete(hw_priv);
#endif
	if (likely(atomic_xchg(&hw_priv->scan.in_progress, 0))) {
		if (hw_priv->scan.status > 0)
			hw_priv->scan.status = 0;
		else if (!hw_priv->scan.status) {
			RK960_INFO_SCAN("Timeout waiting for scan "
					"complete notification.\n");
			hw_priv->scan.status = -ETIMEDOUT;
			hw_priv->scan.curr = hw_priv->scan.end;
			WARN_ON(wsm_stop_scan(hw_priv,
					      hw_priv->scan.if_id ? 1 : 0));
		}
		rk960_scan_complete(hw_priv, hw_priv->scan.if_id);
#ifdef ROAM_OFFLOAD
	} else if (hw_priv->auto_scanning) {
		hw_priv->auto_scanning = 0;
		ieee80211_sched_scan_results(hw_priv->hw);
#endif /*ROAM_OFFLOAD */
	}
}

#ifdef CONFIG_RK960_TESTMODE
void rk960_advance_scan_timeout(struct work_struct *work)
{
	struct rk960_common *hw_priv =
	    container_of(work, struct rk960_common, advance_scan_timeout.work);

	struct rk960_vif *priv = rk960_hwpriv_to_vifpriv(hw_priv,
							 hw_priv->scan.if_id);
	if (WARN_ON(!priv))
		return;
	rk960_priv_vif_list_read_unlock(&priv->vif_lock);

	hw_priv->scan.status = 0;
	if (hw_priv->advanceScanElems.scanMode ==
	    RK960_SCAN_MEASUREMENT_PASSIVE) {
		/* Passive Scan on Serving Channel
		 * Timer Expire */
		rk960_scan_complete(hw_priv, hw_priv->scan.if_id);
	} else {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
		struct cfg80211_scan_info info = {
			.aborted = false,
		};
#endif
		/* Active Scan on Serving Channel
		 * Timer Expire */
		mutex_lock(&hw_priv->conf_mutex);
		//wsm_lock_tx(priv);
		wsm_vif_lock_tx(priv);
		/* Once Duration is Over, enable filtering
		 * and Revert Back Power Save */
		if ((priv->powersave_mode.pmMode & WSM_PSM_PS))
			wsm_set_pm(hw_priv, &priv->powersave_mode, priv->if_id);
		hw_priv->scan.req = NULL;
		rk960_update_filtering(priv);
		hw_priv->enable_advance_scan = false;
		wsm_unlock_tx(hw_priv);
		mutex_unlock(&hw_priv->conf_mutex);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
		ieee80211_scan_completed(hw_priv->hw, &info);
#else
		ieee80211_scan_completed(hw_priv->hw,
					 hw_priv->scan.status ? true : false);
#endif
		up(&hw_priv->scan.lock);
	}
}
#endif

void rk960_probe_work(struct work_struct *work)
{
	struct rk960_common *hw_priv =
	    container_of(work, struct rk960_common, scan.probe_work.work);
	struct rk960_vif *priv, *vif;
	u8 queueId = rk960_queue_get_queue_id(hw_priv->pending_frame_id);
	struct rk960_queue *queue = &hw_priv->tx_queue[queueId];
	const struct rk960_txpriv *txpriv;
	struct wsm_tx *wsm;
	struct wsm_template_frame frame = {
		.frame_type = WSM_FRAME_TYPE_PROBE_REQUEST,
	};
	struct wsm_ssid ssids[1] = { {
				      .length = 0,
				      }
	};
	struct wsm_scan_ch ch[1] = { {
#ifdef SCAN_TIME_ADJUST
				      .minChannelTime = 50,
				      .maxChannelTime = 110,
#else
				      .minChannelTime = 0,
				      .maxChannelTime = 10,
#endif
				      }
	};
	struct wsm_scan scan = {
		.scanType = WSM_SCAN_TYPE_FOREGROUND,
		.numOfProbeRequests = 1,
		.probeDelay = 0,
		.numOfChannels = 1,
		.ssids = ssids,
		.ch = ch,
	};
	u8 *ies;
	size_t ies_len;
	int ret = 1;
	int i;
	RK960_DEBUG_SCAN("[SCAN] Direct probe work.\n");

	BUG_ON(queueId >= 4);
	BUG_ON(!hw_priv->channel);

	mutex_lock(&hw_priv->conf_mutex);
	if (unlikely(down_trylock(&hw_priv->scan.lock))) {
		/* Scan is already in progress. Requeue self. */
		schedule();
		queue_delayed_work(hw_priv->workqueue,
				   &hw_priv->scan.probe_work, HZ / 10);
		mutex_unlock(&hw_priv->conf_mutex);
		return;
	}

	if (rk960_queue_get_skb(queue, hw_priv->pending_frame_id,
				&frame.skb, &txpriv)) {
		up(&hw_priv->scan.lock);
		mutex_unlock(&hw_priv->conf_mutex);
		wsm_unlock_tx(hw_priv);
		return;
	}
	priv = __rk960_hwpriv_to_vifpriv(hw_priv, txpriv->if_id);
	if (!priv) {
		up(&hw_priv->scan.lock);
		mutex_unlock(&hw_priv->conf_mutex);
		return;
	}
	wsm = (struct wsm_tx *)frame.skb->data;
	scan.maxTransmitRate = wsm->maxTxRate;
	scan.band = (hw_priv->channel->band == IEEE80211_BAND_5GHZ) ?
	    WSM_PHY_BAND_5G : WSM_PHY_BAND_2_4G;
	if (priv->join_status == RK960_JOIN_STATUS_STA) {
		scan.scanType = WSM_SCAN_TYPE_BACKGROUND;
		scan.scanFlags = WSM_SCAN_FLAG_FORCE_BACKGROUND;
		if (priv->if_id)
			scan.scanFlags |= WSM_FLAG_MAC_INSTANCE_1;
		else
			scan.scanFlags &= ~WSM_FLAG_MAC_INSTANCE_1;
	}
	//rk960_for_each_vif(hw_priv, vif, i) {
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		vif = hw_priv->vif_list[i] ?
		    rk960_get_vif_from_ieee80211(hw_priv->vif_list[i]) : NULL;
		if (!vif)
			continue;
		if (vif->bss_loss_status > RK960_BSS_LOSS_NONE)
			scan.scanFlags |= WSM_SCAN_FLAG_FORCE_BACKGROUND;
	}
	ch[0].number = hw_priv->channel->hw_value;

	skb_pull(frame.skb, txpriv->offset);

	ies = &frame.skb->data[sizeof(struct ieee80211_hdr_3addr)];
	ies_len = frame.skb->len - sizeof(struct ieee80211_hdr_3addr);

	if (ies_len) {
		u8 *ssidie =
		    (u8 *) cfg80211_find_ie(WLAN_EID_SSID, ies, ies_len);
		if (ssidie && ssidie[1] && ssidie[1] <= sizeof(ssids[0].ssid)) {
			u8 *nextie = &ssidie[2 + ssidie[1]];
			/* Remove SSID from the IE list. It has to be provided
			 * as a separate argument in rk960_scan_start call */

			/* Store SSID localy */
			ssids[0].length = ssidie[1];
			memcpy(ssids[0].ssid, &ssidie[2], ssids[0].length);
			scan.numOfSSIDs = 1;

			/* Remove SSID from IE list */
			ssidie[1] = 0;
			memmove(&ssidie[2], nextie, &ies[ies_len] - nextie);
			skb_trim(frame.skb, frame.skb->len - ssids[0].length);
		}
	}

	if (priv->if_id == 0)
		rk960_remove_wps_p2p_ie(&frame);

	/* FW bug: driver has to restart p2p-dev mode after scan */
	if (priv->join_status == RK960_JOIN_STATUS_MONITOR) {
		WARN_ON(1);
		/*rk960_disable_listening(priv); */
	}
	ret = WARN_ON(wsm_set_template_frame(hw_priv, &frame, priv->if_id));

	hw_priv->scan.direct_probe = 1;
	hw_priv->scan.if_id = priv->if_id;
	if (!ret) {
		wsm_flush_tx(hw_priv);
		ret = WARN_ON(rk960_scan_start(priv, &scan));
	}
	mutex_unlock(&hw_priv->conf_mutex);

	skb_push(frame.skb, txpriv->offset);
	if (!ret)
		IEEE80211_SKB_CB(frame.skb)->flags |= IEEE80211_TX_STAT_ACK;
#ifdef CONFIG_RK960_TESTMODE
	BUG_ON(rk960_queue_remove(hw_priv, queue, hw_priv->pending_frame_id));
#else
	BUG_ON(rk960_queue_remove(queue, hw_priv->pending_frame_id));
#endif

	if (ret) {
		hw_priv->scan.direct_probe = 0;
		up(&hw_priv->scan.lock);
		wsm_unlock_tx(hw_priv);
	}

	return;
}

static
struct sk_buff *ieee80211_probereq_get_rftest(struct ieee80211_hw *hw,
					      struct ieee80211_vif *vif,
					      const u8 * ssid, size_t ssid_len,
					      size_t total_len)
{
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_local *local;
	struct ieee80211_hdr_3addr *hdr;
	struct sk_buff *skb;
	size_t ie_ssid_len;
	int data_len;

	u8 *pos;

	data_len = total_len - sizeof(*hdr) - ssid_len;

	sdata = vif_to_sdata(vif);
	local = sdata->local;
	ie_ssid_len = 2 + ssid_len;

	skb = dev_alloc_skb(local->hw.extra_tx_headroom + sizeof(*hdr) +
			    ie_ssid_len + data_len + 8);
	if (!skb)
		return NULL;

	skb_reserve(skb, local->hw.extra_tx_headroom);

	hdr = (struct ieee80211_hdr_3addr *)skb_put(skb, sizeof(*hdr));
	memset(hdr, 0, sizeof(*hdr));
	hdr->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
					 IEEE80211_STYPE_PROBE_REQ);
	memset(hdr->addr1, 0xff, ETH_ALEN);
	memcpy(hdr->addr2, vif->addr, ETH_ALEN);
	memset(hdr->addr3, 0xff, ETH_ALEN);
	pos = skb_put(skb, ie_ssid_len);
	*pos++ = 0;
	*pos++ = ssid_len;
	if (ssid)
		memcpy(pos, ssid, ssid_len);
	pos += ssid_len;

	while (1) {
		pos = skb_put(skb, 8);
		*pos++ = 221;
		*pos++ = 6;
		pos += 6;
		data_len -= 8;
		if (data_len < 0)
			break;
	}

	return skb;
}

int rk960_rftest_start_tx(struct rk960_common *hw_priv,
			  int channel, int rate, int tx_power,
			  int len, int count, int sgi)
{
	int ret = 0;
	struct wsm_template_frame frame = {
		.frame_type = WSM_FRAME_TYPE_PROBE_REQUEST,
	};
	struct ieee80211_vif *vif = hw_priv->vif_list[hw_priv->def_vif_id];
	struct wsm_scan scan;
	struct wsm_ssid ssids;
	struct wsm_scan_ch ch[1];

	rk960_rftest_stop_txrx(hw_priv);

	WARN_ON(wsm_set_output_power(hw_priv, tx_power, 0));

	frame.skb =
	    ieee80211_probereq_get_rftest(hw_priv->hw, vif, "tttttttt", 8, 200);

	//RK960_INFO_SCAN("%s: frame len %d\n", __func__, frame.skb->len);

	ret = wsm_set_template_frame(hw_priv, &frame, 0);
	if (frame.skb)
		dev_kfree_skb(frame.skb);
	if (ret)
		return ret;

	hw_priv->scan.if_id = hw_priv->def_vif_id;
	memset(&scan, 0, sizeof(struct wsm_scan));

	scan.scanFlags = 0;
	scan.maxTransmitRate = rate;
	if (sgi > 0)
		scan.maxTransmitRate |= 1 << 7;
	if (!count) {
		scan.autoScanInterval = (0xFFFF << 16) | len;
	} else {
		scan.autoScanInterval = (count << 16) | len;
	}
	scan.numOfProbeRequests = 0xff;
	scan.numOfChannels = 1;
	scan.numOfSSIDs = 1;
	scan.probeDelay = 1;
	scan.scanType = WSM_SCAN_TYPE_FOREGROUND;

	scan.ssids = &ssids;
	scan.ssids->length = 4;
	memcpy(ssids.ssid, "tttt", 4);
	scan.ch = &ch[0];
	scan.ch[0].number = channel;
	scan.ch[0].minChannelTime = 10;
	scan.ch[0].maxChannelTime = 11;
	scan.ch[0].txPowerLevel = 3;

	hw_priv->rftest_scan = 1;

	return wsm_scan(hw_priv, &scan, hw_priv->def_vif_id);
}

int rk960_rftest_start_rx(struct rk960_common *hw_priv, int channel)
{
	struct wsm_scan scan;
	struct wsm_ssid ssids;
	struct wsm_scan_ch ch[1];
	int monitor = channel & 0x80;

	channel &= ~0x80;

	rk960_rftest_stop_txrx(hw_priv);

	hw_priv->scan.if_id = hw_priv->def_vif_id;
	memset(&scan, 0, sizeof(struct wsm_scan));

	scan.scanFlags = 0;
	scan.maxTransmitRate = 0;
	scan.autoScanInterval = (0xba << 24) | (30 * 1024);
	if (monitor)
		scan.numOfProbeRequests = 0xfd;
	else
		scan.numOfProbeRequests = 0xfe;
	scan.numOfChannels = 1;
	scan.numOfSSIDs = 1;
	scan.probeDelay = 1;
	scan.scanType = WSM_SCAN_TYPE_BACKGROUND;

	scan.ssids = &ssids;
	scan.ssids->length = 4;
	memcpy(ssids.ssid, "tttt", 4);
	scan.ch = &ch[0];
	scan.ch[0].number = channel;
	scan.ch[0].minChannelTime = 10;
	scan.ch[0].maxChannelTime = 0x7fffffff / 1024;
	scan.ch[0].txPowerLevel = 3;

	hw_priv->rftest_scan = 1;

	return wsm_scan(hw_priv, &scan, hw_priv->def_vif_id);
}

int rk960_rftest_start_tone(struct rk960_common *hw_priv, int channel, int tone)
{
	struct wsm_scan scan;
	struct wsm_ssid ssids;
	struct wsm_scan_ch ch[1];

	rk960_rftest_stop_txrx(hw_priv);

	hw_priv->scan.if_id = hw_priv->def_vif_id;
	memset(&scan, 0, sizeof(struct wsm_scan));

	scan.scanFlags = 0;
	scan.maxTransmitRate = 0;
	scan.autoScanInterval = channel;
	if (tone)
		scan.numOfProbeRequests = 0xfc;
	else
		scan.numOfProbeRequests = 0xfb;
	scan.numOfChannels = 1;
	scan.numOfSSIDs = 1;
	scan.probeDelay = 1;
	scan.scanType = WSM_SCAN_TYPE_BACKGROUND;

	scan.ssids = &ssids;
	scan.ssids->length = 4;
	memcpy(ssids.ssid, "tttt", 4);
	scan.ch = &ch[0];
	scan.ch[0].number = channel;
	scan.ch[0].minChannelTime = 10;
	scan.ch[0].maxChannelTime = 0x7fffffff / 1024;
	scan.ch[0].txPowerLevel = 3;

	hw_priv->rftest_scan = 1;

	return wsm_scan(hw_priv, &scan, hw_priv->def_vif_id);
}

int rk960_rftest_stop_txrx(struct rk960_common *hw_priv)
{
	if (!hw_priv->rftest_scan)
		return 0;
	hw_priv->rftest_scan = 0;

	return wsm_stop_scan(hw_priv, hw_priv->def_vif_id);
}
