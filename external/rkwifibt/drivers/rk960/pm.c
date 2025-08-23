/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/if_ether.h>
#include "rk960.h"
#include "pm.h"
#include "sta.h"
#include "bh.h"
#include "hwbus.h"
#include "debug.h"

//0: disable: all mc will send to host during suspend
//1: enable: mc in maAddress¿¨DA) will send to host during suspend
//2: drop: no mc will send to host during suspend
static int mc_filter = 1;
module_param(mc_filter, int, 0644);
MODULE_PARM_DESC(mc_filter, "multicaset filter");

#define RK960_BEACON_SKIPPING_MULTIPLIER 3

struct rk960_udp_port_filter {
	struct wsm_udp_port_filter_hdr hdr;
	struct wsm_udp_port_filter dhcp;
	struct wsm_udp_port_filter upnp;
} __packed;

struct rk960_ether_type_filter {
	struct wsm_ether_type_filter_hdr hdr;
	struct wsm_ether_type_filter ip;
	struct wsm_ether_type_filter pae;
	struct wsm_ether_type_filter wapi;
} __packed;

static struct rk960_udp_port_filter rk960_udp_port_filter_on = {
	.hdr.nrFilters = 2,
	.dhcp = {
		 .filterAction = WSM_FILTER_ACTION_FILTER_OUT,
		 .portType = WSM_FILTER_PORT_TYPE_DST,
		 .udpPort = __cpu_to_le16(67),
		 },
	.upnp = {
		 .filterAction = WSM_FILTER_ACTION_FILTER_OUT,
		 .portType = WSM_FILTER_PORT_TYPE_DST,
		 .udpPort = __cpu_to_le16(1900),
		 },
	/* Please add other known ports to be filtered out here and
	 * update nrFilters field in the header.
	 * Up to 4 filters are allowed. */
};

static struct wsm_udp_port_filter_hdr rk960_udp_port_filter_off = {
	.nrFilters = 0,
};

#ifndef ETH_P_WAPI
#define ETH_P_WAPI     0x88B4
#endif

static struct rk960_ether_type_filter rk960_ether_type_filter_on = {
	.hdr.nrFilters = 2,
/*	.ip = {
		.filterAction = WSM_FILTER_ACTION_FILTER_IN,
		.etherType = __cpu_to_le16(ETH_P_IP),
	},*/
	.pae = {
		.filterAction = WSM_FILTER_ACTION_FILTER_IN,
		.etherType = __cpu_to_le16(ETH_P_PAE),
		},
	.wapi = {
		 .filterAction = WSM_FILTER_ACTION_FILTER_IN,
		 .etherType = __cpu_to_le16(ETH_P_WAPI),
		 },
	/* Please add other known ether types to be filtered out here and
	 * update nrFilters field in the header.
	 * Up to 4 filters are allowed. */
};

static struct wsm_ether_type_filter_hdr rk960_ether_type_filter_off = {
	.nrFilters = 0,
};

static int rk960_suspend_late(struct device *dev);
static void rk960_pm_release(struct device *dev);
static int rk960_pm_probe(struct platform_device *pdev);
static int __rk960_wow_suspend(struct rk960_vif *priv,
			       struct cfg80211_wowlan *wowlan);
static int __rk960_wow_resume(struct rk960_vif *priv);

/* private */
struct rk960_suspend_state {
	unsigned long bss_loss_tmo;
	unsigned long connection_loss_tmo;
	unsigned long join_tmo;
	unsigned long direct_probe;
	unsigned long link_id_gc;
	bool beacon_skipping;
	u8 prev_ps_mode;
};

static const struct dev_pm_ops rk960_pm_ops = {
	.suspend_noirq = rk960_suspend_late,
};

static struct platform_driver rk960_power_driver = {
	.probe = rk960_pm_probe,
	.driver = {
		   .name = "rk960_power",
		   .pm = &rk960_pm_ops,
		   },
};

static int rk960_pm_init_common(struct rk960_pm_state *pm,
				struct rk960_common *hw_priv)
{
	int ret;

	spin_lock_init(&pm->lock);
	ret = platform_driver_register(&rk960_power_driver);
	if (ret)
		return ret;
	pm->pm_dev = platform_device_alloc("rk960_power", 0);
	if (!pm->pm_dev) {
		platform_driver_unregister(&rk960_power_driver);
		return -ENOMEM;
	}

	pm->pm_dev->dev.platform_data = hw_priv;
	ret = platform_device_add(pm->pm_dev);
	if (ret) {
		kfree(pm->pm_dev);
		pm->pm_dev = NULL;
	}

	return ret;
}

static void rk960_pm_deinit_common(struct rk960_pm_state *pm)
{
	platform_driver_unregister(&rk960_power_driver);
	if (pm->pm_dev) {
		pm->pm_dev->dev.platform_data = NULL;
		platform_device_unregister(pm->pm_dev);
		pm->pm_dev = NULL;
	}
}

#ifdef CONFIG_WAKELOCK

int rk960_pm_init(struct rk960_pm_state *pm, struct rk960_common *hw_priv)
{
	int ret = rk960_pm_init_common(pm, hw_priv);
	if (!ret)
		wake_lock_init(&pm->wakelock, WAKE_LOCK_SUSPEND, "rk960_wlan");
	return ret;
}

void rk960_pm_deinit(struct rk960_pm_state *pm)
{
	if (wake_lock_active(&pm->wakelock))
		wake_unlock(&pm->wakelock);
	wake_lock_destroy(&pm->wakelock);
	rk960_pm_deinit_common(pm);
}

void rk960_pm_stay_awake(struct rk960_pm_state *pm, unsigned long tmo)
{
	long cur_tmo;

	if (!pm->pm_dev) {
		return;
	}

	spin_lock_bh(&pm->lock);
#if (LINUX_VERSION_CODE >=KERNEL_VERSION(3,4,0))
	cur_tmo = pm->wakelock.ws.timer_expires - jiffies;
#else
	cur_tmo = pm->wakelock.expires - jiffies;
#endif
	if (!wake_lock_active(&pm->wakelock) || cur_tmo < (long)tmo)
		wake_lock_timeout(&pm->wakelock, tmo);
	spin_unlock_bh(&pm->lock);
}

#else /* CONFIG_WAKELOCK */

#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
static void rk960_pm_stay_awake_tmo(struct timer_list *t)
#else
static void rk960_pm_stay_awake_tmo(unsigned long arg)
#endif
{
}

int rk960_pm_init(struct rk960_pm_state *pm, struct rk960_common *hw_priv)
{
	int ret = rk960_pm_init_common(pm, hw_priv);
	if (!ret) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
		timer_setup(&pm->stay_awake, rk960_pm_stay_awake_tmo, 0);
#else
		init_timer(&pm->stay_awake);
		pm->stay_awake.data = (unsigned long)pm;
		pm->stay_awake.function = rk960_pm_stay_awake_tmo;
#endif
	}
	return ret;
}

void rk960_pm_deinit(struct rk960_pm_state *pm)
{
	del_timer_sync(&pm->stay_awake);
	rk960_pm_deinit_common(pm);
}

void rk960_pm_stay_awake(struct rk960_pm_state *pm, unsigned long tmo)
{
	long cur_tmo;
	spin_lock_bh(&pm->lock);
	cur_tmo = pm->stay_awake.expires - jiffies;
	if (!timer_pending(&pm->stay_awake) || cur_tmo < (long)tmo)
		mod_timer(&pm->stay_awake, jiffies + tmo);
	spin_unlock_bh(&pm->lock);
}

#endif /* CONFIG_WAKELOCK */

static long rk960_suspend_work(struct delayed_work *work)
{
	int ret = cancel_delayed_work(work);
	long tmo;
	if (ret > 0) {
		/* Timer is pending */
		tmo = work->timer.expires - jiffies;
		if (tmo < 0)
			tmo = 0;
	} else {
		tmo = -1;
	}
	return tmo;
}

static int rk960_resume_work(struct rk960_common *hw_priv,
			     struct delayed_work *work, unsigned long tmo)
{
	if ((long)tmo < 0)
		return 1;

	return queue_delayed_work(hw_priv->workqueue, work, tmo);
}

static int rk960_suspend_late(struct device *dev)
{
	struct rk960_common *hw_priv = dev->platform_data;
	if (atomic_read(&hw_priv->bh_rx)) {
		RK960_INFO_PM("%s: Suspend interrupted.\n", __func__);
		return -EAGAIN;
	}
	return 0;
}

static void rk960_pm_release(struct device *dev)
{
}

static int rk960_pm_probe(struct platform_device *pdev)
{
	pdev->dev.release = rk960_pm_release;
	return 0;
}

int rk960_can_suspend(struct rk960_common *priv)
{
	if (atomic_read(&priv->bh_rx) ||
	    priv->device_wakeing || !priv->wsm_cmd.done) {
		RK960_DEBUG_PM("Suspend interrupted. (%d %d %d)\n",
			       atomic_read(&priv->bh_rx),
			       priv->device_wakeing, priv->wsm_cmd.done);
		return 0;
	}
	return 1;
}

//EXPORT_SYMBOL_GPL(rk960_can_suspend);

int rk960_wow_suspend(struct ieee80211_hw *hw, struct cfg80211_wowlan *wowlan)
{
	struct rk960_common *hw_priv = hw->priv;
	struct rk960_vif *priv;
	int i, ret = 0;

	RK960_DEBUG_PM("%s.\n", __func__);

	WARN_ON(!atomic_read(&hw_priv->num_vifs));

#if 0				//def ROAM_OFFLOAD
	//rk960_for_each_vif(hw_priv, priv, i) {
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		priv = hw_priv->vif_list[i] ?
		    rk960_get_vif_from_ieee80211(hw_priv->vif_list[i]) : NULL;
#ifdef P2P_MULTIVIF
		if ((i == (RK960_MAX_VIFS - 1)) || !priv)
#else
		if (!priv)
#endif
			continue;
		if ((priv->vif->type == NL80211_IFTYPE_STATION)
		    && (priv->join_status == RK960_JOIN_STATUS_STA)) {
			down(&hw_priv->scan.lock);
			hw_priv->scan.if_id = priv->if_id;
			rk960_sched_scan_work(&hw_priv->scan.swork);
		}
	}
#endif /*ROAM_OFFLOAD */

	/* Do not suspend when datapath is not idle */
	if (hw_priv->tx_queue_stats.num_queued[0]
	    + hw_priv->tx_queue_stats.num_queued[1])
		return -EBUSY;

	/* Make sure there is no configuration requests in progress. */
	if (!mutex_trylock(&hw_priv->conf_mutex))
		return -EBUSY;

	/* Do not suspend when scanning or ROC */
	if (down_trylock(&hw_priv->scan.lock))
		goto revert1;

	if (delayed_work_pending(&hw_priv->scan.probe_work))
		goto revert2;

	if (delayed_work_pending(&hw_priv->scan.sched_scan_stop_work))
		goto revert2;

	/* Lock TX. */
	wsm_lock_tx_async(hw_priv);

	/* Wait to avoid possible race with bh code.
	 * But do not wait too long... */
	if (wait_event_timeout(hw_priv->bh_evt_wq,
			       !hw_priv->hw_bufs_used, HZ / 10) <= 0)
		goto revert3;

	//rk960_for_each_vif(hw_priv, priv, i) {
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		priv = hw_priv->vif_list[i] ?
		    rk960_get_vif_from_ieee80211(hw_priv->vif_list[i]) : NULL;
#ifdef P2P_MULTIVIF
		if ((i == (RK960_MAX_VIFS - 1)) || !priv)
#else
		if (!priv)
#endif
			continue;

		ret = __rk960_wow_suspend(priv, wowlan);
		if (ret) {
			for (; i >= 0; i--) {
				if (!hw_priv->vif_list[i])
					continue;
				priv = (struct rk960_vif *)
				    hw_priv->vif_list[i]->drv_priv;
				__rk960_wow_resume(priv);
			}
			goto revert3;
		}
	}

	/* Stop serving thread */
	if (rk960_bh_suspend(hw_priv)) {
		RK960_ERROR_PM("%s: rk960_bh_suspend failed\n", __func__);
		rk960_wow_resume(hw);
		return -EBUSY;
	}

	/* Enable IRQ wake */
	ret = hw_priv->hwbus_ops->power_mgmt(hw_priv->hwbus_priv, true);
	if (ret) {
		RK960_ERROR_PM("%s: PM request failed: %d. WoW is disabled.\n",
			       __func__, ret);
		rk960_wow_resume(hw);
		return -EBUSY;
	}

	/* Force resume if event is coming from the device. */
	if (atomic_read(&hw_priv->bh_rx)) {
		RK960_INFO_PM("%s: incoming event present - resume\n",
			      __func__);
		rk960_wow_resume(hw);
		return -EAGAIN;
	}

	hw_priv->wow_suspend_done = 1;
	return 0;
revert3:
	wsm_unlock_tx(hw_priv);
revert2:
	up(&hw_priv->scan.lock);
revert1:
	mutex_unlock(&hw_priv->conf_mutex);
	return -EBUSY;
}

static int __rk960_wow_suspend(struct rk960_vif *priv,
			       struct cfg80211_wowlan *wowlan)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);
	struct rk960_pm_state_vif *pm_state_vif = &priv->pm_state_vif;
	struct rk960_suspend_state *state;
	int ret;

#ifdef MCAST_FWDING
	struct wsm_forwarding_offload fwdoffload = {
		.fwenable = 0x1,
		.flags = 0x1,
	};
#endif
	/* Do not suspend when join work is scheduled */
	if (work_pending(&priv->join_work))
		goto revert1;

#ifdef RK960_CHANNEL_CONFLICT_OPT
	if (work_pending(&priv->channel_switch_work))
		goto revert1;
#endif

	if (work_pending(&priv->update_filtering_work))
		goto revert1;

	/* Set UDP filter */
	wsm_set_udp_port_filter(hw_priv, &rk960_udp_port_filter_on.hdr,
				priv->if_id);

	/* Set ethernet frame type filter */
	wsm_set_ether_type_filter(hw_priv, &rk960_ether_type_filter_on.hdr,
				  priv->if_id);

	rk960_set_ipv4addrfilter(hw_priv, priv, true);

	if (priv->join_status == RK960_JOIN_STATUS_AP)
		WARN_ON(wsm_set_keepalive_filter(priv, true));

#ifdef RK960_SUSPEND_RESUME_FILTER_ENABLE
	/* Set Multicast Address Filter */
	if (priv->multicast_filter.numOfAddresses) {
		priv->multicast_filter.enable = mc_filter;
		wsm_set_multicast_filter(hw_priv, &priv->multicast_filter,
					 priv->if_id);
	}

	/* Set Enable Broadcast Address Filter */

	priv->broadcast_filter.action_mode = 1;
	if (priv->join_status == RK960_JOIN_STATUS_AP)
		priv->broadcast_filter.address_mode = 3;

	rk960_set_macaddrfilter(hw_priv, priv, (u8 *) & priv->broadcast_filter);

#endif

#ifdef MCAST_FWDING
	if (priv->join_status == RK960_JOIN_STATUS_AP)
		WARN_ON(wsm_set_forwarding_offlad(hw_priv,
						  &fwdoffload, priv->if_id));
#endif

	/* Allocate state */
	state = kzalloc(sizeof(struct rk960_suspend_state), GFP_KERNEL);
	if (!state)
		goto revert2;

	/* Change to legacy PS while going to suspend */
	if (!priv->vif->p2p &&
	    priv->join_status == RK960_JOIN_STATUS_STA &&
	    priv->powersave_mode.pmMode != WSM_PSM_PS) {
		state->prev_ps_mode = priv->powersave_mode.pmMode;
		priv->powersave_mode.pmMode = WSM_PSM_PS;
		rk960_set_pm(priv, &priv->powersave_mode);
		if (wait_event_interruptible_timeout
		    (hw_priv->ps_mode_switch_done,
		     !hw_priv->ps_mode_switch_in_progress, 1 * HZ) <= 0) {
			goto revert2;
		}
	}

	/* Store delayed work states. */
	state->bss_loss_tmo = rk960_suspend_work(&priv->bss_loss_work);
	state->connection_loss_tmo =
	    rk960_suspend_work(&priv->connection_loss_work);
	state->join_tmo = rk960_suspend_work(&priv->join_timeout);
	state->link_id_gc = rk960_suspend_work(&priv->link_id_gc_work);

	/* Enable beacon skipping */
	if (priv->join_status == RK960_JOIN_STATUS_STA
	    && priv->join_dtim_period && !priv->has_multicast_subscription) {
		state->beacon_skipping = true;
		wsm_set_beacon_wakeup_period(hw_priv,
					     priv->join_dtim_period,
					     RK960_BEACON_SKIPPING_MULTIPLIER
					     * priv->join_dtim_period,
					     priv->if_id);
	}

	ret = timer_pending(&priv->mcast_timeout);
	if (ret)
		goto revert3;

	/* Store suspend state */
	pm_state_vif->suspend_state = state;

	return 0;

revert3:
	rk960_resume_work(hw_priv, &priv->bss_loss_work, state->bss_loss_tmo);
	rk960_resume_work(hw_priv, &priv->connection_loss_work,
			  state->connection_loss_tmo);
	rk960_resume_work(hw_priv, &priv->join_timeout, state->join_tmo);
	rk960_resume_work(hw_priv, &priv->link_id_gc_work, state->link_id_gc);
	kfree(state);
revert2:
	wsm_set_udp_port_filter(hw_priv, &rk960_udp_port_filter_off,
				priv->if_id);
	wsm_set_ether_type_filter(hw_priv, &rk960_ether_type_filter_off,
				  priv->if_id);

	if (priv->join_status == RK960_JOIN_STATUS_AP)
		WARN_ON(wsm_set_keepalive_filter(priv, false));

#ifdef RK960_SUSPEND_RESUME_FILTER_ENABLE
	/* Set Multicast Address Filter */
	if (priv->multicast_filter.numOfAddresses) {
		priv->multicast_filter.enable = 0;
		wsm_set_multicast_filter(hw_priv, &priv->multicast_filter,
					 priv->if_id);
	}

	/* Set Enable Broadcast Address Filter */

	priv->broadcast_filter.action_mode = 0;
	if (priv->join_status == RK960_JOIN_STATUS_AP)
		priv->broadcast_filter.address_mode = 0;
	rk960_set_macaddrfilter(hw_priv, priv, (u8 *) & priv->broadcast_filter);

#endif

#ifdef MCAST_FWDING
	fwdoffload.flags = 0x0;
	if (priv->join_status == RK960_JOIN_STATUS_AP)
		WARN_ON(wsm_set_forwarding_offlad
			(hw_priv, &fwdoffload, priv->if_id));
#endif
revert1:
	//mutex_unlock(&hw_priv->conf_mutex);
	return -EBUSY;
}

int rk960_wow_resume(struct ieee80211_hw *hw)
{

	struct rk960_common *hw_priv = hw->priv;
	struct rk960_vif *priv;
	int i, ret = 0;

	RK960_DEBUG_PM("%s.\n", __func__);

	hw_priv->wow_suspend_done = 0;
	hw_priv->wowlan_disable = false;

	WARN_ON(!atomic_read(&hw_priv->num_vifs));

	/* Disable IRQ wake */
	hw_priv->hwbus_ops->power_mgmt(hw_priv->hwbus_priv, false);

	up(&hw_priv->scan.lock);

	/* Resume BH thread */
	WARN_ON(rk960_bh_resume(hw_priv));

	//rk960_for_each_vif(hw_priv, priv, i) {
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		priv = hw_priv->vif_list[i] ?
		    rk960_get_vif_from_ieee80211(hw_priv->vif_list[i]) : NULL;
#ifdef P2P_MULTIVIF
		if ((i == (RK960_MAX_VIFS - 1)) || !priv)
#else
		if (!priv)
#endif
			continue;
		ret = __rk960_wow_resume(priv);
		if (ret)
			break;
	}

	wsm_unlock_tx(hw_priv);
	/* Unlock configuration mutex */
	mutex_unlock(&hw_priv->conf_mutex);

	return ret;
}

static int __rk960_wow_resume(struct rk960_vif *priv)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);
	struct rk960_pm_state_vif *pm_state_vif = &priv->pm_state_vif;
	struct rk960_suspend_state *state;

#ifdef MCAST_FWDING
	struct wsm_forwarding_offload fwdoffload = {
		.fwenable = 0x1,
		.flags = 0x0,
	};
#endif

	if (!pm_state_vif->suspend_state) {
		return 0;
	}

	state = pm_state_vif->suspend_state;
	pm_state_vif->suspend_state = NULL;

#if 0				//def ROAM_OFFLOAD
	if ((priv->vif->type == NL80211_IFTYPE_STATION)
	    && (priv->join_status == RK960_JOIN_STATUS_STA))
		_rk960_hw_sched_scan_stop(hw_priv);
#endif /*ROAM_OFFLOAD */

	rk960_set_ipv4addrfilter(hw_priv, priv, false);

	if (state->beacon_skipping) {
		wsm_set_beacon_wakeup_period(hw_priv, priv->beacon_int *
					     priv->join_dtim_period >
					     MAX_BEACON_SKIP_TIME_MS ? 1 :
					     priv->join_dtim_period, 0,
					     priv->if_id);
		state->beacon_skipping = false;
	}

	if (priv->join_status == RK960_JOIN_STATUS_AP)
		WARN_ON(wsm_set_keepalive_filter(priv, false));

#ifdef RK960_SUSPEND_RESUME_FILTER_ENABLE
	/* Set Multicast Address Filter */
	if (priv->multicast_filter.numOfAddresses) {
		priv->multicast_filter.enable = 0;
		wsm_set_multicast_filter(hw_priv, &priv->multicast_filter,
					 priv->if_id);
	}

	/* Set Enable Broadcast Address Filter */

	priv->broadcast_filter.action_mode = 0;
	if (priv->join_status == RK960_JOIN_STATUS_AP)
		priv->broadcast_filter.address_mode = 0;

	rk960_set_macaddrfilter(hw_priv, priv, (u8 *) & priv->broadcast_filter);

#endif

#ifdef MCAST_FWDING
	if (priv->join_status == RK960_JOIN_STATUS_AP)
		WARN_ON(wsm_set_forwarding_offlad
			(hw_priv, &fwdoffload, priv->if_id));
#endif

	/* Restores previous PS mode */
	if (!priv->vif->p2p && priv->join_status == RK960_JOIN_STATUS_STA) {
		priv->powersave_mode.pmMode = state->prev_ps_mode;
		rk960_set_pm(priv, &priv->powersave_mode);
	}

	/* Resume delayed work */
	rk960_resume_work(hw_priv, &priv->bss_loss_work, state->bss_loss_tmo);
	rk960_resume_work(hw_priv, &priv->connection_loss_work,
			  state->connection_loss_tmo);
	rk960_resume_work(hw_priv, &priv->join_timeout, state->join_tmo);
	rk960_resume_work(hw_priv, &priv->link_id_gc_work, state->link_id_gc);

	/* Remove UDP port filter */
	wsm_set_udp_port_filter(hw_priv, &rk960_udp_port_filter_off,
				priv->if_id);

	/* Remove ethernet frame type filter */
	wsm_set_ether_type_filter(hw_priv, &rk960_ether_type_filter_off,
				  priv->if_id);
	/* Free memory */
	kfree(state);

	return 0;
}

#ifdef SUPPORT_FWCR
int rk960_wow_resume_fwcr(struct rk960_vif *priv)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);

	if (1) {
		wsm_set_beacon_wakeup_period(hw_priv,
					     priv->join_dtim_period, 0,
					     priv->if_id);
	}

#ifdef RK960_SUSPEND_RESUME_FILTER_ENABLE
	/* Set Multicast Address Filter */
	if (priv->multicast_filter.numOfAddresses) {
		priv->multicast_filter.enable = 0;
		wsm_set_multicast_filter(hw_priv, &priv->multicast_filter,
					 priv->if_id);
	}

	/* Set Enable Broadcast Address Filter */

	priv->broadcast_filter.action_mode = 0;
	rk960_set_macaddrfilter(hw_priv, priv, (u8 *) & priv->broadcast_filter);

#endif

	/* Restores previous PS mode */
	if (priv->join_status == RK960_JOIN_STATUS_STA) {
		priv->powersave_mode.pmMode = WSM_PSM_FAST_PS;
		rk960_set_pm(priv, &priv->powersave_mode);
	}

	/* Remove UDP port filter */
	wsm_set_udp_port_filter(hw_priv, &rk960_udp_port_filter_off,
				priv->if_id);

	/* Remove ethernet frame type filter */
	wsm_set_ether_type_filter(hw_priv, &rk960_ether_type_filter_off,
				  priv->if_id);

        RK960_INFO_PM("%s: done\n", __func__);
        hw_priv->fwcr_fw_resumed = 1;
        wake_up(&hw_priv->fwcr_resume_done);
        
	return 0;
}
#endif

int rk960_suspend(struct rk960_common *hw_priv)
{
	int ret;

	if (hw_priv->wow_suspend_done)
		return 0;

	RK960_DEBUG_PM("%s.\n", __func__);

	if (rk960_bh_suspend(hw_priv)) {
		RK960_ERROR_PM("%s: rk960_bh_suspend failed\n", __func__);
		return -EBUSY;
	}

	/* Enable IRQ wake */
	ret = hw_priv->hwbus_ops->power_mgmt(hw_priv->hwbus_priv, true);
	if (ret) {
		RK960_ERROR_PM("%s: PM request failed: %d. WoW is disabled.\n",
			       __func__, ret);
		return -EBUSY;
	}

	return 0;
}

int rk960_resume(struct rk960_common *hw_priv)
{
	if (hw_priv->wow_suspend_done)
		return 0;
	hw_priv->wowlan_disable = true;

	RK960_DEBUG_PM("%s.\n", __func__);

	/* Disable IRQ wake */
	hw_priv->hwbus_ops->power_mgmt(hw_priv->hwbus_priv, false);

	WARN_ON(rk960_bh_resume(hw_priv));

	return 0;
}
