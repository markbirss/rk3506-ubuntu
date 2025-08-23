/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef RK960_TXRX_H
#define RK960_TXRX_H

#include <linux/list.h>

/* extern */ struct ieee80211_hw;
/* extern */ struct sk_buff;
/* extern */ struct wsm_tx;
/* extern */ struct wsm_rx;
/* extern */ struct wsm_tx_confirm;
/* extern */ struct rk960_txpriv;
/* extern */ struct rk960_vif;

struct tx_policy {
	union {
		__le32 tbl[3];
		u8 raw[12];
	};
	u8 defined;		/* TODO: u32 or u8, profile and select best */
	u8 usage_count;		/* --// -- */
	u8 retry_count;		/* --// -- */
	u8 uploaded;
};

struct tx_policy_cache_entry {
	struct tx_policy policy;
	struct list_head link;
};

#define TX_POLICY_CACHE_SIZE	(8)
struct tx_policy_cache {
	struct tx_policy_cache_entry cache[TX_POLICY_CACHE_SIZE];
	struct list_head used;
	struct list_head free;
	spinlock_t lock;
};

/* ******************************************************************** */
/* TX policy cache							*/
/* Intention of TX policy cache is an overcomplicated WSM API.
 * Device does not accept per-PDU tx retry sequence.
 * It uses "tx retry policy id" instead, so driver code has to sync
 * linux tx retry sequences with a retry policy table in the device.
 */
void tx_policy_init(struct rk960_common *hw_priv);
void tx_policy_upload_work(struct work_struct *work);

/* ******************************************************************** */
/* TX implementation							*/

u32 rk960_rate_mask_to_wsm(struct rk960_common *hw_priv, u32 rates);
void rk960_tx(struct ieee80211_hw *dev,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	      struct ieee80211_tx_control *control,
#endif
	      struct sk_buff *skb);
void rk960_skb_dtor(struct rk960_common *hw_priv,
		    struct sk_buff *skb, const struct rk960_txpriv *txpriv);

/* ******************************************************************** */
/* WSM callbacks							*/

void rk960_tx_confirm_cb(struct rk960_common *hw_priv,
			 struct wsm_tx_confirm *arg);
void rk960_rx_cb(struct rk960_vif *priv,
		 struct wsm_rx *arg, struct sk_buff **skb_p);
void rk960_rx_cb_monitor(struct rk960_common *priv,
			 struct wsm_rx *arg, struct sk_buff **skb_p);

/* ******************************************************************** */
/* Timeout								*/

void rk960_tx_timeout(struct work_struct *work);

/* ******************************************************************** */
/* Security								*/
int rk960_alloc_key(struct rk960_common *hw_priv);
void rk960_free_key(struct rk960_common *hw_priv, int idx);
void rk960_free_keys(struct rk960_common *hw_priv);
int rk960_upload_keys(struct rk960_vif *priv);

/* ******************************************************************** */
/* Workaround for WFD test case 6.1.10					*/
#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
void rk960_link_id_reset(struct work_struct *work);
#endif

void rk960_csync_param_init(struct rk960_common *hw_priv);
void rk960_csync_work(struct work_struct *work);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
void rk960_csync_timer(struct timer_list *t);
#else
void rk960_csync_timer(unsigned long arg);
#endif

#ifdef SUPPORT_FWCR
void rk960_fwcr_init(struct rk960_common *hw_priv);
void rk960_fwcr_deinit(struct rk960_common *hw_priv);
void rk960_fwcr_write(struct rk960_common *hw_priv);
void rk960_fwcr_read(struct rk960_common *hw_priv);
int rk960_fwcr_recovery_scan(struct rk960_common *hw_priv,
        struct rk960_vif *priv);
int rk960_fwcr_recovery_auth(struct rk960_common *hw_priv,
        struct sk_buff *skb, struct rk960_vif *priv);
int rk960_wow_resume_fwcr(struct rk960_vif *priv);
void rk960_fwcr_work(struct work_struct *work);
void rk960_fwcr_wait_resumed(struct rk960_common *hw_priv);
void rk960_fwcr_group_frame_capture(struct rk960_common *hw_priv,
        int key_id);
#endif
int rk960_pae_type(struct sk_buff *skb, int tx);

#endif /* RK960_TXRX_H */
