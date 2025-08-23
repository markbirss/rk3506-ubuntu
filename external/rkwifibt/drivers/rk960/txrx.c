/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <net/mac80211.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/crc32.h>
#include <../net/mac80211/ieee80211_i.h>

#include "rk960.h"
#include "wsm.h"
#include "bh.h"
#include "ap.h"
#include "debug.h"
#include "sta.h"
#include "hwbus.h"
#include "debug.h"

#define RK960_INVALID_RATE_ID (0xFF)

#ifdef CONFIG_RK960_TESTMODE
#include "rk960_nl80211_testmode_msg_copy.h"
#endif /* CONFIG_RK960_TESTMODE */
static const struct ieee80211_rate *rk960_get_tx_rate(const struct rk960_common
						      *hw_priv,
						      const struct
						      ieee80211_tx_rate *rate);

/* ******************************************************************** */
/* TX policy cache implementation					*/
#ifndef DISABLE_TX_POLICY
static void tx_policy_dump(struct tx_policy *policy)
{
	RK960_DEBUG_TXPOLICY("[TX policy] "
			     "%.1X%.1X%.1X%.1X%.1X%.1X%.1X%.1X"
			     "%.1X%.1X%.1X%.1X%.1X%.1X%.1X%.1X"
			     "%.1X%.1X%.1X%.1X%.1X%.1X%.1X%.1X: %d\n",
			     policy->raw[0] & 0x0F, policy->raw[0] >> 4,
			     policy->raw[1] & 0x0F, policy->raw[1] >> 4,
			     policy->raw[2] & 0x0F, policy->raw[2] >> 4,
			     policy->raw[3] & 0x0F, policy->raw[3] >> 4,
			     policy->raw[4] & 0x0F, policy->raw[4] >> 4,
			     policy->raw[5] & 0x0F, policy->raw[5] >> 4,
			     policy->raw[6] & 0x0F, policy->raw[6] >> 4,
			     policy->raw[7] & 0x0F, policy->raw[7] >> 4,
			     policy->raw[8] & 0x0F, policy->raw[8] >> 4,
			     policy->raw[9] & 0x0F, policy->raw[9] >> 4,
			     policy->raw[10] & 0x0F, policy->raw[10] >> 4,
			     policy->raw[11] & 0x0F, policy->raw[11] >> 4,
			     policy->defined);
}
#endif

static void rk960_check_go_neg_conf_success(struct rk960_common *hw_priv,
					    u8 * action)
{
	if (action[2] == 0x50 && action[3] == 0x6F && action[4] == 0x9A &&
	    action[5] == 0x09 && action[6] == 0x02) {
		if (action[17] == 0) {
			hw_priv->is_go_thru_go_neg = true;
		} else {
			hw_priv->is_go_thru_go_neg = false;
		}
	}
}

static void rk960_check_prov_desc_req(struct rk960_common *hw_priv, u8 * action)
{
	if (action[2] == 0x50 && action[3] == 0x6F && action[4] == 0x9A &&
	    action[5] == 0x09 && action[6] == 0x07) {
		hw_priv->is_go_thru_go_neg = false;
	}
}

static void rk960_change_p2p_ch_list(u8 * action, int size, int ch)
{
	int offset = 0;
	int i, chs;

	if (ch <= 0 || ch > 13)
		return;

	offset += 8;		// 802.11 Management - Action
	if (action[offset] == 221) {	// Vendor Specific - Wi-Fi Alliance
		offset += 6;
		while (offset < size) {
			if (action[offset] == 11) {	// channel list
				offset += 6;
				if (action[offset] == 81) {
					chs = action[offset + 1];
					offset += 2;
					if (chs <= 13) {
						for (i = 0; i < chs; i++) {
							action[offset + i] = ch;
						}
					}
				}
			} else {
				offset +=
				    3 +
				    (action[offset + 1] |
				     (action[offset + 2] << 8));
			}
		}
	}
}

static void rk960_check_inv_resp(struct rk960_common *hw_priv,
				 struct rk960_vif *priv, u8 * action, int size)
{
	if (action[2] == 0x50 && action[3] == 0x6F && action[4] == 0x9A &&
	    action[5] == 0x09 && action[6] == 0x04) {
		// P2P Invitation Response
		int if_id2;
		struct rk960_vif *priv2;

		if (priv->if_id)
			if_id2 = 0;
		else
			if_id2 = 1;
		if (hw_priv->if_id_slot & BIT(if_id2)) {
			priv2 =
			    rk960_get_vif_from_ieee80211(hw_priv->
							 vif_list[if_id2]);
			if (priv2->join_status == RK960_JOIN_STATUS_STA) {
				RK960_DEBUG_TXRX
				    ("%s: invite resp ch list force to %d\n",
				     __func__, priv2->current_channel);
				rk960_change_p2p_ch_list(action, size,
							 priv2->
							 current_channel);
			}
		}
	}
}

size_t tx_rate_overound(size_t count, struct ieee80211_tx_rate *rates) 
{
	/* HACK!!! Device has problems (at least) switching from
	 * 54Mbps CTS to 1Mbps. This switch takes enormous amount
	 * of time (100-200 ms), leading to valuable throughput drop.
	 * As a workaround, additional g-rates are injected to the
	 * policy.
	 */
	if (count == 2 && !(rates[0].flags & IEEE80211_TX_RC_MCS) &&
	    rates[0].idx > 4 && rates[0].count > 2 && rates[1].idx < 2) {
		/* ">> 1" is an equivalent of "/ 2", but faster */
		int mid_rate = (rates[0].idx + 4) >> 1;

		/* Decrease number of retries for the initial rate */
		rates[0].count -= 2;

		if (mid_rate != 4) {
			/* Keep fallback rate at 1Mbps. */
			rates[3] = rates[1];

			/* Inject 1 transmission on lowest g-rate */
			rates[2].idx = 4;
			rates[2].count = 1;
			rates[2].flags = rates[1].flags;

			/* Inject 1 transmission on mid-rate */
			rates[1].idx = mid_rate;
			rates[1].count = 1;

			/* Fallback to 1 Mbps is a really bad thing,
			 * so let's try to increase probability of
			 * successful transmission on the lowest g rate
			 * even more */
			if (rates[0].count >= 3) {
				--rates[0].count;
				++rates[2].count;
			}

			/* Adjust amount of rates defined */
			count += 2;
		} else {
			/* Keep fallback rate at 1Mbps. */
			rates[2] = rates[1];

			/* Inject 2 transmissions on lowest g-rate */
			rates[1].idx = 4;
			rates[1].count = 2;

			/* Adjust amount of rates defined */
			count += 1;
		}
	}

        return count;
}

#ifndef DISABLE_TX_POLICY
/* for example rate from umac:
 * [6 2] [0 1] [11 2] [0 15]
 * after tx_policy_build, only [6 2] will be build and send to lmac
 * that will easy cause TX failed (max tx retry reached)
 * after fix, [11 2] [6 2] [0 3] will be build and send to lmac
*/
#define RATES_DESC_ORDER_FIX
/* for example rate from umac:
 * [7 1 8] [0 2 8] [6 5 9] [-1 0 0]
 * after tx_policy_build
 * [7 1 8] [0 2 8] [0 5 8] [-1 0 0]
 */
//#define RATES_DESC_ORDER_FIX2

//static int g_tx_len;
static void tx_policy_build(const struct rk960_common *hw_priv,
			    /* [out] */ struct tx_policy *policy,
			    struct ieee80211_tx_rate *rates, size_t count)
{
	int i, j;
	unsigned limit = hw_priv->short_frame_max_tx_count;
	unsigned total = 0;
	BUG_ON(rates[0].idx < 0);
	memset(policy, 0, sizeof(*policy));

	/*if (g_tx_len > 1000) {
	   for (i = 0; i < count; ++i) {
	   if (rates[i].idx < 0) {
	   break;
	   }
	   rates[i].idx = 6;
	   }
	   } */

	/*pr_info("tx count %d limit %d [%d %d %x] [%d %d %x] [%d %d %x] [%d %d %x]\n",
	   (int)count, limit,
	   rates[0].idx, rates[0].count, rates[0].flags,
	   rates[1].idx, rates[1].count, rates[1].flags,
	   rates[2].idx, rates[2].count, rates[2].flags,
	   rates[3].idx, rates[3].count, rates[3].flags);*/

	/* minstrel is buggy a little bit, so distille
	 * incoming rates first. */
#ifdef RATES_DESC_ORDER_FIX2
	for (i = 1; i < count; ++i) {
		if (rates[i].idx < 0) {
			count = i;
			break;
		}
		if (((rates[i].flags & IEEE80211_TX_RC_MCS) ==
		     (rates[i - 1].flags & IEEE80211_TX_RC_MCS)) &&
		    rates[i].idx > rates[i - 1].idx) {
			rates[i].idx = rates[i - 1].idx;
			rates[i].flags = rates[i - 1].flags;
		}
	}
#else
#ifdef RATES_DESC_ORDER_FIX
	BUG_ON(count < 1);
	/* Sort rates in descending order. */
	for (j = 0; j < count - 1; j++) {
#endif
		for (i = 1; i < count; ++i) {
			if (rates[i].idx < 0) {
				count = i;
				break;
			}
			if (((rates[i].flags & IEEE80211_TX_RC_MCS) ==
		            (rates[i - 1].flags & IEEE80211_TX_RC_MCS)) &&
				   rates[i].idx > rates[i - 1].idx) {
				struct ieee80211_tx_rate tmp = rates[i - 1];
				rates[i - 1] = rates[i];
				rates[i] = tmp;
			}
		}
#ifdef RATES_DESC_ORDER_FIX
	}
#endif
#endif

	/* Eliminate duplicates. */
	total = rates[0].count;
	for (i = 0, j = 1; j < count; ++j) {
		if (rates[j].idx == rates[i].idx) {
			rates[i].count += rates[j].count;
		} else if (rates[j].idx > rates[i].idx) {
			break;
		} else {
			++i;
			if (i != j)
				rates[i] = rates[j];
		}
		total += rates[j].count;
	}
	count = i + 1;

	/* Re-fill policy trying to keep every requested rate and with
	 * respect to the global max tx retransmission count. */
	if (limit < count)
		limit = count;
	if (total > limit) {
		for (i = 0; i < count; ++i) {
			int left = count - i - 1;
			if (rates[i].count > limit - left)
				rates[i].count = limit - left;
			limit -= rates[i].count;
		}
	}

	count = tx_rate_overound(count, rates);

	policy->defined = rk960_get_tx_rate(hw_priv, &rates[0])->hw_value + 1;

	for (i = 0; i < count; ++i) {
		register unsigned rateid, off, shift, retries;

		rateid = rk960_get_tx_rate(hw_priv, &rates[i])->hw_value;
		//pr_info("rate[%d] hw_value %d retry %d\n", i, rateid, rates[i].count);
		off = rateid >> 3;	/* eq. rateid / 8 */
		shift = (rateid & 0x07) << 2;	/* eq. (rateid % 8) * 4 */

		retries = rates[i].count;
		if (unlikely(retries > 0x0F))
			rates[i].count = retries = 0x0F;
		policy->tbl[off] |= __cpu_to_le32(retries << shift);
		policy->retry_count += retries;
	}
        //pr_info("policy->retry_count %d\n", policy->retry_count);
}

static inline bool tx_policy_is_equal(const struct tx_policy *wanted,
				      const struct tx_policy *cached)
{
	size_t count = wanted->defined >> 1;
	if (wanted->defined > cached->defined)
		return false;
	if (count) {
		if (memcmp(wanted->raw, cached->raw, count))
			return false;
	}
	if (wanted->defined & 1) {
		if ((wanted->raw[count] & 0x0F) != (cached->raw[count] & 0x0F))
			return false;
	}
	return true;
}

static int tx_policy_find(struct tx_policy_cache *cache,
			  const struct tx_policy *wanted)
{
	/* O(n) complexity. Not so good, but there's only 8 entries in
	 * the cache.
	 * Also lru helps to reduce search time. */
	struct tx_policy_cache_entry *it;
	/* Search for policy in "used" list */
	list_for_each_entry(it, &cache->used, link) {
		if (tx_policy_is_equal(wanted, &it->policy))
			return it - cache->cache;
	}
	/* Then - in "free list" */
	list_for_each_entry(it, &cache->free, link) {
		if (tx_policy_is_equal(wanted, &it->policy))
			return it - cache->cache;
	}
	return -1;
}
#endif

static inline void tx_policy_use(struct tx_policy_cache *cache,
				 struct tx_policy_cache_entry *entry)
{
	++entry->policy.usage_count;
	list_move(&entry->link, &cache->used);
}

static inline int tx_policy_release(struct tx_policy_cache *cache,
				    struct tx_policy_cache_entry *entry)
{
	int ret = --entry->policy.usage_count;
	if (!ret)
		list_move(&entry->link, &cache->free);
	return ret;
}

/* ******************************************************************** */
/* External TX policy cache API						*/

void tx_policy_init(struct rk960_common *hw_priv)
{
	struct tx_policy_cache *cache = &hw_priv->tx_policy_cache;
	int i;

	memset(cache, 0, sizeof(*cache));

	spin_lock_init(&cache->lock);
	INIT_LIST_HEAD(&cache->used);
	INIT_LIST_HEAD(&cache->free);

	for (i = 0; i < TX_POLICY_CACHE_SIZE; ++i)
		list_add(&cache->cache[i].link, &cache->free);
}

#ifndef DISABLE_TX_POLICY
static int tx_policy_get(struct rk960_common *hw_priv,
			 struct ieee80211_tx_rate *rates,
			 size_t count, bool * renew)
{
	int idx;
	struct tx_policy_cache *cache = &hw_priv->tx_policy_cache;
	struct tx_policy wanted;

	tx_policy_build(hw_priv, &wanted, rates, count);

	spin_lock_bh(&cache->lock);
	if (WARN_ON_ONCE(list_empty(&cache->free))) {
		spin_unlock_bh(&cache->lock);
		return RK960_INVALID_RATE_ID;
	}
	idx = tx_policy_find(cache, &wanted);
	if (idx >= 0) {
		RK960_DEBUG_TXPOLICY("[TX policy] Used TX policy: %d\n", idx);
		*renew = false;
	} else {
		struct tx_policy_cache_entry *entry;
		*renew = true;
		/* If policy is not found create a new one
		 * using the oldest entry in "free" list */
		entry = list_entry(cache->free.prev,
				   struct tx_policy_cache_entry, link);
		entry->policy = wanted;
		idx = entry - cache->cache;
		RK960_DEBUG_TXPOLICY("[TX policy] New TX policy: %d\n", idx);
		tx_policy_dump(&entry->policy);
	}
	tx_policy_use(cache, &cache->cache[idx]);
	if (unlikely(list_empty(&cache->free))) {
		/* Lock TX queues. */
		rk960_tx_queues_lock(hw_priv);
	}
	spin_unlock_bh(&cache->lock);
	return idx;
}
#endif

static void tx_policy_put(struct rk960_common *hw_priv, int idx)
{
	int usage, locked;
	struct tx_policy_cache *cache = &hw_priv->tx_policy_cache;

	spin_lock_bh(&cache->lock);
	locked = list_empty(&cache->free);
	usage = tx_policy_release(cache, &cache->cache[idx]);
	if (unlikely(locked) && !usage) {
		/* Unlock TX queues. */
		rk960_tx_queues_unlock(hw_priv);
	}
	spin_unlock_bh(&cache->lock);
}

/*
bool tx_policy_cache_full(struct rk960_common *hw_priv)
{
	bool ret;
	struct tx_policy_cache *cache = &hw_priv->tx_policy_cache;
	spin_lock_bh(&cache->lock);
	ret = list_empty(&cache->free);
	spin_unlock_bh(&cache->lock);
	return ret;
}
*/

static int tx_policy_upload(struct rk960_common *hw_priv)
{
	struct tx_policy_cache *cache = &hw_priv->tx_policy_cache;
	int i;
	struct wsm_set_tx_rate_retry_policy arg = {
		.hdr = {
			.numTxRatePolicies = 0,
			}
	};
	int if_id = 0;
	spin_lock_bh(&cache->lock);

	/* Upload only modified entries. */
	for (i = 0; i < TX_POLICY_CACHE_SIZE; ++i) {
		struct tx_policy *src = &cache->cache[i].policy;
		if (src->retry_count && !src->uploaded) {
			struct wsm_set_tx_rate_retry_policy_policy *dst =
			    &arg.tbl[arg.hdr.numTxRatePolicies];
			dst->policyIndex = i;
			dst->shortRetryCount =
			    hw_priv->short_frame_max_tx_count;
			dst->longRetryCount = hw_priv->long_frame_max_tx_count;

			/* BIT(2) - Terminate retries when Tx rate retry policy
			 *          finishes.
			 * BIT(3) - Count initial frame transmission as part of
			 *          rate retry counting but not as a retry
			 *          attempt */
			dst->policyFlags = BIT(2) | BIT(3);

			memcpy(dst->rateCountIndices, src->tbl,
			       sizeof(dst->rateCountIndices));
			src->uploaded = 1;
			++arg.hdr.numTxRatePolicies;
		}
	}
	spin_unlock_bh(&cache->lock);
	rk960_debug_tx_cache_miss(hw_priv);
	RK960_DEBUG_TXPOLICY("[TX policy] Upload %d policies\n",
			     arg.hdr.numTxRatePolicies);
	/*TODO: COMBO */
	return wsm_set_tx_rate_retry_policy(hw_priv, &arg, if_id);
}

void tx_policy_upload_work(struct work_struct *work)
{
	struct rk960_common *hw_priv =
	    container_of(work, struct rk960_common, tx_policy_upload_work);

	RK960_DEBUG_TXPOLICY("[TX] TX policy upload.\n");
	WARN_ON(tx_policy_upload(hw_priv));

	wsm_unlock_tx(hw_priv);
	rk960_tx_queues_unlock(hw_priv);
}

static bool tx_check_sgi(struct ieee80211_tx_rate *rates)
{
	int i;

	for (i = 0; i < IEEE80211_TX_MAX_RATES; ++i) {
		if (rates[i].idx < 0)
			break;
		if ((rates[i].idx == 7) && (rates[i].flags &
					    IEEE80211_TX_RC_SHORT_GI))
			return true;
	}
	return false;
}

/* ******************************************************************** */
/* rk960 TX implementation						*/

struct rk960_txinfo {
	struct sk_buff *skb;
	unsigned queue;
	struct ieee80211_tx_info *tx_info;
	const struct ieee80211_rate *rate;
	struct ieee80211_hdr *hdr;
	size_t hdrlen;
	const u8 *da;
	struct rk960_sta_priv *sta_priv;
	struct ieee80211_sta *sta;
	struct rk960_txpriv txpriv;
	int ethertype;
};

u32 rk960_rate_mask_to_wsm(struct rk960_common *hw_priv, u32 rates)
{
	u32 ret = 0;
	int i;
	struct ieee80211_rate *bitrates =
	    hw_priv->hw->wiphy->bands[hw_priv->channel->band]->bitrates;
	for (i = 0; i < 32; ++i) {
		if (rates & BIT(i))
			ret |= BIT(bitrates[i].hw_value);
	}
	return ret;
}

static const struct ieee80211_rate *rk960_get_tx_rate(const struct rk960_common
						      *hw_priv,
						      const struct
						      ieee80211_tx_rate *rate)
{
	if (rate->idx < 0)
		return NULL;
	if (rate->flags & IEEE80211_TX_RC_MCS)
		return &hw_priv->mcs_rates[rate->idx];
	return &hw_priv->hw->wiphy->bands[hw_priv->channel->
					  band]->bitrates[rate->idx];
}

static int
rk960_tx_h_calc_link_ids(struct rk960_vif *priv, struct rk960_txinfo *t)
{
#ifndef P2P_MULTIVIF
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);
	if ((t->tx_info->flags & IEEE80211_TX_CTL_TX_OFFCHAN) ||
	    (hw_priv->roc_if_id == priv->if_id))
		t->txpriv.offchannel_if_id = 2;
	else
		t->txpriv.offchannel_if_id = 0;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	if (likely(t->sta && t->sta_priv->link_id))
#else
	if (likely(t->tx_info->control.sta && t->sta_priv->link_id))
#endif
		t->txpriv.raw_link_id =
		    t->txpriv.link_id = t->sta_priv->link_id;
	else if (priv->mode != NL80211_IFTYPE_AP)
		t->txpriv.raw_link_id = t->txpriv.link_id = 0;
	else if (is_multicast_ether_addr(t->da)) {
		if (priv->enable_beacon) {
			t->txpriv.raw_link_id = 0;
			t->txpriv.link_id = priv->link_id_after_dtim;
		} else {
			t->txpriv.raw_link_id = 0;
			t->txpriv.link_id = 0;
		}
	} else {
		t->txpriv.link_id = rk960_find_link_id(priv, t->da);
		/* Do not assign valid link id for deauth/disassoc frame being
		   transmitted to an unassociated STA */
		if (!(t->txpriv.link_id) &&
		    (ieee80211_is_deauth(t->hdr->frame_control) ||
		     ieee80211_is_disassoc(t->hdr->frame_control))) {
			t->txpriv.link_id = 0;
		} else if (!(t->txpriv.link_id) &&
			   ieee80211_is_probe_resp(t->hdr->frame_control)) {
			t->txpriv.link_id = 0;
		} else {
			if (!t->txpriv.link_id)
				t->txpriv.link_id =
				    rk960_alloc_link_id(priv, t->da);
			if (!t->txpriv.link_id) {
				RK960_ERROR_TXRX
				    ("%s: No more link IDs available.\n",
				     __func__);
				return -ENOENT;
			}
		}
		t->txpriv.raw_link_id = t->txpriv.link_id;
	}
	if (t->txpriv.raw_link_id)
		priv->link_id_db[t->txpriv.raw_link_id - 1].timestamp = jiffies;

#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	if (t->sta && (t->sta->uapsd_queues & BIT(t->queue)))
#else
	if (t->tx_info->control.sta &&
	    (t->tx_info->control.sta->uapsd_queues & BIT(t->queue)))
#endif
		t->txpriv.link_id = priv->link_id_uapsd;
#endif /* CONFIG_RK960_USE_STE_EXTENSIONS */
	return 0;
}

static void rk960_tx_h_pm(struct rk960_vif *priv, struct rk960_txinfo *t)
{
	if (unlikely(ieee80211_is_auth(t->hdr->frame_control))) {
		u32 mask = ~BIT(t->txpriv.raw_link_id);
		spin_lock_bh(&priv->ps_state_lock);
		priv->sta_asleep_mask &= mask;
		priv->pspoll_mask &= mask;
		spin_unlock_bh(&priv->ps_state_lock);
	}
}

static void rk960_tx_h_calc_tid(struct rk960_vif *priv, struct rk960_txinfo *t)
{
	if (ieee80211_is_data_qos(t->hdr->frame_control)) {
		u8 *qos = ieee80211_get_qos_ctl(t->hdr);
		t->txpriv.tid = qos[0] & IEEE80211_QOS_CTL_TID_MASK;
	} else if (ieee80211_is_data(t->hdr->frame_control)) {
		t->txpriv.tid = 0;
	}
}

/* IV/ICV injection. */
/* TODO: Quite unoptimal. It's better co modify mac80211
 * to reserve space for IV */
#if 0
static int rk960_tx_h_crypt(struct rk960_vif *priv, struct rk960_txinfo *t)
{
	size_t iv_len;
	size_t icv_len;
	u8 *icv;
	u8 *newhdr;

	/*if (t->tx_info->control.hw_key)
	   {
	   if (t->tx_info->control.hw_key->cipher == WLAN_CIPHER_SUITE_AES_CMAC) {
	   struct ieee80211_mmie *mmie;

	   RK960_INFO_TXRX("11w WLAN_CIPHER_SUITE_AES_CMAC\n");
	   mmie = (struct ieee80211_mmie *) skb_put(t->skb, sizeof(*mmie));
	   memset(mmie, 0, sizeof(struct ieee80211_mmie));
	   mmie->element_id = WLAN_EID_MMIE;
	   mmie->length = sizeof(*mmie) - 2;
	   mmie->key_id = cpu_to_le16(t->tx_info->control.hw_key->keyidx);
	   t->hdr->duration_id = 0x00;
	   }
	   } */

	if (unlikely(t->tx_info->flags & IEEE80211_TX_INTFL_RETRANSMISSION)) {
		return 0;
	}

	if (!t->tx_info->control.hw_key ||
	    !(t->hdr->frame_control & __cpu_to_le32(IEEE80211_FCTL_PROTECTED)))
		return 0;

	iv_len = t->tx_info->control.hw_key->iv_len;
	icv_len = t->tx_info->control.hw_key->icv_len;

	if (t->tx_info->control.hw_key->cipher == WLAN_CIPHER_SUITE_TKIP)
		icv_len += 8;	/* MIC */

	if ((skb_headroom(t->skb) + skb_tailroom(t->skb) <
	     iv_len + icv_len + WSM_TX_EXTRA_HEADROOM) ||
	    (skb_headroom(t->skb) < iv_len + WSM_TX_EXTRA_HEADROOM)) {
		RK960_ERROR_TXRX("Bug: no space allocated for crypto headers.\n"
				 "headroom: %d, tailroom: %d, "
				 "req_headroom: %d, req_tailroom: %d\n"
				 "Please fix it in rk960_get_skb().\n",
				 skb_headroom(t->skb), skb_tailroom(t->skb),
				 (int)iv_len + WSM_TX_EXTRA_HEADROOM,
				 (int)icv_len);
		return -ENOMEM;
	} else if (skb_tailroom(t->skb) < icv_len) {
		size_t offset = icv_len - skb_tailroom(t->skb);
		u8 *p;
		RK960_INFO_TXRX("Slowpath: tailroom is not big enough. "
				"Req: %d, got: %d.\n",
				(int)icv_len, skb_tailroom(t->skb));

		p = skb_push(t->skb, offset);
		memmove(p, &p[offset], t->skb->len - offset);
		skb_trim(t->skb, t->skb->len - offset);
	}

	newhdr = skb_push(t->skb, iv_len);
	memmove(newhdr, newhdr + iv_len, t->hdrlen);
	t->hdr = (struct ieee80211_hdr *)newhdr;
	t->hdrlen += iv_len;
	icv = skb_put(t->skb, icv_len);

	return 0;
}
#else
static int rk960_tx_h_crypt(struct rk960_vif *priv, struct rk960_txinfo *t)
{
	size_t iv_len;
	size_t icv_len;
	u8 *newhdr;

	/*if (t->tx_info->control.hw_key)
	   {
	   if (t->tx_info->control.hw_key->cipher == WLAN_CIPHER_SUITE_AES_CMAC) {
	   struct ieee80211_mmie *mmie;

	   RK960_INFO_TXRX("11w WLAN_CIPHER_SUITE_AES_CMAC\n");
	   mmie = (struct ieee80211_mmie *) skb_put(t->skb, sizeof(*mmie));
	   memset(mmie, 0, sizeof(struct ieee80211_mmie));
	   mmie->element_id = WLAN_EID_MMIE;
	   mmie->length = sizeof(*mmie) - 2;
	   mmie->key_id = cpu_to_le16(t->tx_info->control.hw_key->keyidx);
	   t->hdr->duration_id = 0x00;
	   }
	   } */

	if (unlikely(t->tx_info->flags & IEEE80211_TX_INTFL_RETRANSMISSION)) {
		return 0;
	}

	if (!t->tx_info->control.hw_key ||
	    !(t->hdr->frame_control & __cpu_to_le32(IEEE80211_FCTL_PROTECTED)))
		return 0;

	iv_len = t->tx_info->control.hw_key->iv_len;
	icv_len = t->tx_info->control.hw_key->icv_len;

	if (t->tx_info->control.hw_key->cipher == WLAN_CIPHER_SUITE_TKIP)
		icv_len += 8;	/* MIC */
	if (!skb_is_nonlinear(t->skb)) {
		if ((skb_headroom(t->skb) + skb_tailroom(t->skb) <
		     iv_len + icv_len + WSM_TX_EXTRA_HEADROOM) ||
		    (skb_headroom(t->skb) < iv_len + WSM_TX_EXTRA_HEADROOM)) {
			RK960_ERROR_TXRX
			    ("Bug: no space allocated for crypto headers.\n"
			     "headroom: %d, tailroom: %d, "
			     "req_headroom: %d, req_tailroom: %d\n"
			     "Please fix it in rk960_get_skb().\n",
			     skb_headroom(t->skb), skb_tailroom(t->skb),
			     (int)iv_len + WSM_TX_EXTRA_HEADROOM, (int)icv_len);
			return -ENOMEM;
		} else if (skb_tailroom(t->skb) < icv_len) {
			size_t offset = icv_len - skb_tailroom(t->skb);
			u8 *p;

			if (!
			    (t->tx_info->control.hw_key->
			     flags & IEEE80211_KEY_FLAG_PUT_IV_SPACE)) {
				p = skb_push(t->skb, offset);
				memmove(p, &p[offset], t->skb->len - offset);
				skb_trim(t->skb, t->skb->len - offset);
			} else {
				RK960_ERROR_TXRX
				    ("Slowpath: tailroom is not big enough. "
				     "Req: %d, got: %d.\n", (int)icv_len,
				     skb_tailroom(t->skb));
				return -ENOMEM;
			}

		}
		skb_put(t->skb, icv_len);
	} else {
		if (skb_headroom(t->skb) < iv_len + WSM_TX_EXTRA_HEADROOM) {
			RK960_ERROR_TXRX
			    ("Bug: skb_sg_no space allocated for crypto headers.\n"
			     "headroom: %d"
			     "req_headroom: %d, req_tailroom: %d\n"
			     "Please fix it in get_skb().\n",
			     skb_headroom(t->skb),
			     (int)iv_len + WSM_TX_EXTRA_HEADROOM, (int)icv_len);
			return -ENOMEM;
		}
	}
	newhdr = skb_push(t->skb, iv_len);
	memmove(newhdr, newhdr + iv_len, t->hdrlen);
	t->hdr = (struct ieee80211_hdr *)newhdr;
	t->hdrlen += iv_len;
	return 0;
}
#endif

static int
rk960_tx_h_align(struct rk960_vif *priv, struct rk960_txinfo *t, u8 * flags)
{
	size_t offset = (size_t) t->skb->data & 3;

	if (!offset)
		return 0;

	if (offset & 1) {
		RK960_ERROR_TXRX("Bug: attempt to transmit a frame "
				 "with wrong alignment: %d\n", (int)offset);
		return -EINVAL;
	}

	if (skb_headroom(t->skb) < offset) {
		RK960_ERROR_TXRX("Bug: no space allocated "
				 "for DMA alignment.\n"
				 "headroom: %d\n", skb_headroom(t->skb));
		return -ENOMEM;
	}
	skb_push(t->skb, offset);
	t->hdrlen += offset;
	t->txpriv.offset += offset;
	*flags |= WSM_TX_2BYTES_SHIFT;
	rk960_debug_tx_align(priv);
	return 0;
}

static int rk960_tx_h_action(struct rk960_vif *priv, struct rk960_txinfo *t)
{
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)t->hdr;

	if (ieee80211_is_action(t->hdr->frame_control) &&
/*	    mgmt->u.action.category == WLAN_CATEGORY_BACK &&
	    mgmt->u.action.u.delba.action_code == WLAN_ACTION_DELBA)
			return 0;

	if (ieee80211_is_action(t->hdr->frame_control) &&*/
	    mgmt->u.action.category == WLAN_CATEGORY_BACK)
		return 1;
	else
		return 0;
}

#ifdef DISABLE_TX_POLICY
static int rk960_tx_rate_to_rate_index(struct ieee80211_tx_rate *rates)
{
        int rate_index = -1;

        if (rates->idx < 0)
                return -1;
        
        if (rates->flags & IEEE80211_TX_RC_MCS) // 11n
                rate_index = rates->idx + 14;
        else if (rates->idx >= 4) // 11g
                rate_index = rates->idx + 2;
        else if (rates->idx < 4) // 11b
                rate_index = rates->idx;
        return rate_index;
}

static void rk960_tx_h_wsm_txrate(struct wsm_tx *wsm,
                        struct rk960_txinfo *t)
{
        u8 *rate = (u8 *)(&wsm->reserved);
        u8 *retry = (u8 *)(&wsm->expireTime);
        int i;
        struct ieee80211_tx_rate *rates = t->tx_info->control.rates;

        if (rates[1].idx >= 0 && rates[2].idx < 0)
                tx_rate_overound(2, rates);

        for (i = 0; i < IEEE80211_TX_MAX_RATES; i++) {
                rate[i] =
                        rk960_tx_rate_to_rate_index(&rates[i]);
                retry[i] = rates[i].count;
        }
        /*pr_info("wsm tx [%d %d] [%d %d] [%d %d] [%d %d]\n",
	   rate[0], retry[0],
	   rate[1], retry[1],
	   rate[2], retry[2],
	   rate[3], retry[3]);*/
}
#endif

/* Add WSM header */
static struct wsm_tx *rk960_tx_h_wsm(struct rk960_vif *priv,
				     struct rk960_txinfo *t)
{
	struct wsm_tx *wsm;

	if (skb_headroom(t->skb) < sizeof(struct wsm_tx)) {
		RK960_ERROR_TXRX("Bug: no space allocated "
				 "for WSM header.\n"
				 "headroom: %d\n", skb_headroom(t->skb));
                BUG_ON(1);
		return NULL;
	}

	wsm = (struct wsm_tx *)skb_push(t->skb, sizeof(struct wsm_tx));
	t->txpriv.offset += sizeof(struct wsm_tx);
	memset(wsm, 0, sizeof(*wsm));
	wsm->hdr.len = __cpu_to_le16(t->skb->len);
	wsm->hdr.id = __cpu_to_le16(0x0004);
	wsm->queueId =
	    (t->txpriv.raw_link_id << 2) | wsm_queue_id_to_wsm(t->queue);
#ifdef DISABLE_TX_POLICY
        rk960_tx_h_wsm_txrate(wsm, t);
#endif
	return wsm;
}

/* BT Coex specific handling */
static void
rk960_tx_h_bt(struct rk960_vif *priv,
	      struct rk960_txinfo *t, struct wsm_tx *wsm)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);

	u8 priority = 0;

	if (!hw_priv->is_BT_Present)
		return;

	if (unlikely(ieee80211_is_nullfunc(t->hdr->frame_control)))
		priority = WSM_EPTA_PRIORITY_MGT;
	else if (ieee80211_is_data(t->hdr->frame_control)) {
		/* Skip LLC SNAP header (+6) */
		u8 *payload = &t->skb->data[t->hdrlen];
		u16 *ethertype = (u16 *) & payload[6];
		if (unlikely(*ethertype == __be16_to_cpu(ETH_P_PAE)))
			priority = WSM_EPTA_PRIORITY_EAPOL;
	} else if (unlikely(ieee80211_is_assoc_req(t->hdr->frame_control) ||
			    ieee80211_is_reassoc_req(t->hdr->frame_control))) {
		struct ieee80211_mgmt *mgt_frame =
		    (struct ieee80211_mgmt *)t->hdr;

		if (mgt_frame->u.assoc_req.listen_interval <
		    priv->listen_interval) {
			RK960_DEBUG_TXRX
			    ("Modified Listen Interval to %d from %d\n",
			     priv->listen_interval,
			     mgt_frame->u.assoc_req.listen_interval);
			/* Replace listen interval derieved from
			 * the one read from SDD */
			mgt_frame->u.assoc_req.listen_interval =
			    priv->listen_interval;
		}
	}

	if (likely(!priority)) {
		if (ieee80211_is_action(t->hdr->frame_control))
			priority = WSM_EPTA_PRIORITY_ACTION;
		else if (ieee80211_is_mgmt(t->hdr->frame_control))
			priority = WSM_EPTA_PRIORITY_MGT;
		else if (wsm->queueId == WSM_QUEUE_VOICE)
			priority = WSM_EPTA_PRIORITY_VOICE;
		else if (wsm->queueId == WSM_QUEUE_VIDEO)
			priority = WSM_EPTA_PRIORITY_VIDEO;
		else
			priority = WSM_EPTA_PRIORITY_DATA;
	}

	RK960_DEBUG_TXRX("[TX] EPTA priority %d.\n", priority);

	wsm->flags |= priority << 1;
}

static bool rk960_rx_is_pae(struct sk_buff *skb)
{
	// only for non protect frames
	struct ieee80211_hdr *frame = (struct ieee80211_hdr *)skb->data;
	size_t hdrlen = ieee80211_hdrlen(frame->frame_control);

	if (ieee80211_is_data(frame->frame_control) && skb->len < 512) {
		u16 *ethertype = (u16 *) (&skb->data[hdrlen + 6]);

		if (unlikely(*ethertype == __be16_to_cpu(ETH_P_PAE))) {
			RK960_DEBUG_TXRX("[RX]: last_rx_is_nonprotect_pae\n");
			return true;
		}
	}

	return false;
}

static void rk960_tx_h_pae(struct rk960_common *hw_priv, struct rk960_txinfo *t)
{
	t->ethertype = 0;
	if (ieee80211_is_data(t->hdr->frame_control) && t->skb->len < 512) {
		/* Skip LLC SNAP header (+6) */
		u8 *payload = &t->skb->data[t->hdrlen];
		u16 *ethertype = (u16 *) & payload[6];
		t->ethertype = *ethertype;

		if (unlikely(t->ethertype == __be16_to_cpu(ETH_P_PAE))) {
			int i;
			struct ieee80211_tx_rate *rates =
			    t->tx_info->control.rates;

			RK960_DEBUG_TXRX
			    ("[TX] force ETH_P_PAE frame to the lowest rate\n");
			for (i = 0; i < IEEE80211_TX_MAX_RATES; i++) {
				if (rates[i].idx < 0) {
					break;
				}
				rates[i].idx = 0;
				rates[i].flags = 0;
				rates[i].count = 5;
			}

			if (hw_priv->last_rx_is_nonprotect_pae)
				t->hdr->frame_control &=
				    ~(__cpu_to_le32(IEEE80211_FCTL_PROTECTED));
			hw_priv->last_rx_is_nonprotect_pae = false;
		}
	}
}

void tx_fixed_rate_build(struct rk960_common *hw_priv,
                struct rk960_txinfo *t, size_t count)
{
        int rate_index;
        int mcs_rate = -1;
        int i;
        struct ieee80211_tx_rate *rates = t->tx_info->control.rates;
        struct ieee80211_hdr *frame = (struct ieee80211_hdr *)t->skb->data;

        //if (t->skb->len < 1400)
        //        return;
        if (ieee80211_is_mgmt(frame->frame_control))
                return;
        
        rate_index = rk960_rate_to_rate_index(hw_priv->tx_fixed_rate);
        if (rate_index < 0)
                return;

        if (rate_index >= 14) // 11n
                mcs_rate = rate_index - 14;
        else if (rate_index >= 6) // 11g
                rate_index -= 2;

        for (i = 0; i < count; ++i) {
                if (rates[i].idx < 0) {
                        break;
                }
                if (mcs_rate >= 0) {
                        rates[i].idx = mcs_rate;
                        rates[i].flags |= IEEE80211_TX_RC_MCS;
                } else {
                        rates[i].idx = rate_index;
                        rates[i].flags &= ~IEEE80211_TX_RC_MCS;
                }
        }  
}

static void
rk960_tx_rate_limit(struct rk960_common *hw_priv,
		       struct rk960_txinfo *t)
{
        struct ieee80211_tx_rate *rates = t->tx_info->control.rates;
        int i;
        u8 mask;

        if (t->sta) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
                   mask = t->sta->deflink.ht_cap.mcs.rx_mask[0];
#else
        	   mask = t->sta->ht_cap.mcs.rx_mask[0];
#endif
        	   for (i = 0; i < IEEE80211_TX_MAX_RATES; ++i) {
        	        if (rates[i].idx < 0)
        	                break;
                        if (rates[i].flags & IEEE80211_TX_RC_MCS) {
                                if (!(BIT(rates[i].idx) & mask))
                                        rates[i].idx = ffs(mask) - 1;
                        }
        	   }
        }
}

static int
rk960_tx_h_rate_policy(struct rk960_common *hw_priv,
		       struct rk960_txinfo *t, struct wsm_tx *wsm)
{
	bool tx_policy_renew = false;
	struct rk960_vif *priv =
	    rk960_get_vif_from_ieee80211(t->tx_info->control.vif);
#ifdef DISABLE_TX_POLICY
        struct ieee80211_tx_rate *rates = t->tx_info->control.rates;
        unsigned limit = hw_priv->short_frame_max_tx_count;
#endif

	rk960_tx_rate_limit(hw_priv, t);

//        if (t->tx_info->flags & IEEE80211_TX_CTL_RATE_CTRL_PROBE)
//                pr_info("rate probe\n");
#ifdef DISABLE_TX_POLICY
	/*pr_info("tx count %d limit %d [%d %d %x] [%d %d %x] [%d %d %x] [%d %d %x]\n",
	   (int)IEEE80211_TX_MAX_RATES, limit,
	   rates[0].idx, rates[0].count, rates[0].flags,
	   rates[1].idx, rates[1].count, rates[1].flags,
	   rates[2].idx, rates[2].count, rates[2].flags,
	   rates[3].idx, rates[3].count, rates[3].flags);*/

        t->txpriv.rate_id = RK960_INVALID_RATE_ID;
#else
	t->txpriv.rate_id = tx_policy_get(hw_priv,
					  t->tx_info->control.rates,
					  IEEE80211_TX_MAX_RATES,
					  &tx_policy_renew);
	if (t->txpriv.rate_id == RK960_INVALID_RATE_ID)
		return -EFAULT;

	wsm->flags |= t->txpriv.rate_id << 4;
#endif

	t->rate = rk960_get_tx_rate(hw_priv,
				    &t->tx_info->control.rates[0]),
	    wsm->maxTxRate = t->rate->hw_value;
	if (t->rate->flags & IEEE80211_TX_RC_MCS) {
		if (priv->association_mode.greenfieldMode)
			wsm->htTxParameters |=
			    __cpu_to_le32(WSM_HT_TX_GREENFIELD);
		else
			wsm->htTxParameters |= __cpu_to_le32(WSM_HT_TX_MIXED);
#if 0
		if (t->tx_info->control.rates[0].flags &
		    IEEE80211_TX_RC_SHORT_GI) {
#else
		if (tx_check_sgi(t->tx_info->control.rates)) {
#endif
			if (wsm->htTxParameters &
			    __cpu_to_le32(WSM_HT_TX_MIXED))
				wsm->htTxParameters |=
				    __cpu_to_le32(WSM_HT_TX_SGI);
		}
		//pr_info("htTxParameters %x\n", wsm->htTxParameters); 
	}

	if (tx_policy_renew) {
		RK960_DEBUG_TXPOLICY("[TX] TX policy renew.\n");
		/* It's not so optimal to stop TX queues every now and then.
		 * Maybe it's better to reimplement task scheduling with
		 * a counter. */
		/* rk960_tx_queues_lock(priv); */
		/* Definetly better. TODO. */
		wsm_lock_tx_async(hw_priv);
		rk960_tx_queues_lock(hw_priv);
		if (queue_work(hw_priv->workqueue,
			       &hw_priv->tx_policy_upload_work) <= 0) {
			rk960_tx_queues_unlock(hw_priv);
			wsm_unlock_tx(hw_priv);
		}
	}
	return 0;
}

static bool rk960_tx_h_pm_state(struct rk960_vif *priv, struct rk960_txinfo *t)
{
	int was_buffered = 1;

	RK960_DEBUG_AP("%s: if_id %d link_id %d link_id_after_dtim %d"
		       " buffered_multicasts %d sta_asleep_mask %x\n",
		       __func__, priv->if_id, t->txpriv.link_id,
		       priv->link_id_after_dtim, priv->buffered_multicasts,
		       priv->sta_asleep_mask);
	if (t->txpriv.link_id == priv->link_id_after_dtim &&
	    !priv->buffered_multicasts &&
	    !ieee80211_is_probe_resp(t->hdr->frame_control)) {
		priv->buffered_multicasts = true;
		if (priv->sta_asleep_mask) {
			RK960_DEBUG_AP("%s: multicast_start_work\n", __func__);
			queue_work(priv->hw_priv->workqueue,
				   &priv->multicast_start_work);
		}
	}

	if (t->txpriv.raw_link_id && t->txpriv.tid < RK960_MAX_TID)
		was_buffered = priv->link_id_db[t->txpriv.raw_link_id - 1]
		    .buffered[t->txpriv.tid]++;

	return !was_buffered;
}

static void rk960_tx_h_ba_stat(struct rk960_vif *priv, struct rk960_txinfo *t)
{
	struct rk960_common *hw_priv = priv->hw_priv;

	if (priv->join_status != RK960_JOIN_STATUS_STA)
		return;
	if (!rk960_is_ht(&hw_priv->ht_info))
		return;
	if (!priv->setbssparams_done)
		return;
	if (!ieee80211_is_data(t->hdr->frame_control))
		return;

	spin_lock_bh(&hw_priv->ba_lock);
	hw_priv->ba_acc += t->skb->len - t->hdrlen;
	if (!(hw_priv->ba_cnt_rx || hw_priv->ba_cnt)) {
		mod_timer(&hw_priv->ba_timer,
			  jiffies + RK960_BLOCK_ACK_INTERVAL);
	}
	hw_priv->ba_cnt++;
	spin_unlock_bh(&hw_priv->ba_lock);
}

static int
rk960_tx_h_skb_pad(struct rk960_common *priv,
		   struct wsm_tx *wsm, struct sk_buff *skb)
{
#if !USE_SDIO_TXRX_BLKSIZE_ALIGN
	size_t len = __le16_to_cpu(wsm->hdr.len);
	size_t padded_len = priv->hwbus_ops->align_size(priv->hwbus_priv, len);

	if (WARN_ON(skb_padto(skb, padded_len) != 0)) {
		return -EINVAL;
	}
#endif
	return 0;
}

/* ******************************************************************** */

void rk960_tx(struct ieee80211_hw *dev,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	      struct ieee80211_tx_control *control,
#endif
	      struct sk_buff *skb)
{
	struct rk960_common *hw_priv = dev->priv;
	struct rk960_txinfo t = {
		.skb = skb,
		.queue = skb_get_queue_mapping(skb),
		.tx_info = IEEE80211_SKB_CB(skb),
		.hdr = (struct ieee80211_hdr *)skb->data,
		.txpriv.tid = RK960_MAX_TID,
		.txpriv.rate_id = RK960_INVALID_RATE_ID,
#ifdef P2P_MULTIVIF
		.txpriv.raw_if_id = 0,
#endif
	};
	struct ieee80211_sta *sta;
	struct wsm_tx *wsm;
	bool tid_update = 0;
	u8 flags = 0;
	int ret;
	struct rk960_vif *priv;
	struct ieee80211_hdr *frame = (struct ieee80211_hdr *)skb->data;
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)skb->data;

	dump_ieee80211_hdr_info((unsigned char *)skb->data, skb->len, 1, 0);

#ifdef RK960_CSYNC_ADJUST
	atomic_add_return(1, &hw_priv->tx_counter);
#endif
//      g_tx_len = skb->len;

	/*if (ieee80211_has_protected(frame->frame_control)) {
	   pr_info("%s drop\n", __func__);
	   goto drop;
	   } */

	if (!skb->data)
		BUG_ON(1);

	if (!(t.tx_info->control.vif)) {
		goto drop;
	}
	priv = rk960_get_vif_from_ieee80211(t.tx_info->control.vif);
	if (!priv)
		goto drop;

	if (atomic_read(&priv->enabled) == 0)
		goto drop;

#ifdef SUPPORT_FWCR
        if (!rk960_fwcr_recovery_auth(hw_priv, skb, priv))
                goto drop;
#endif

#ifdef CONFIG_RK960_TESTMODE
	spin_lock_bh(&hw_priv->tsm_lock);
	if (hw_priv->start_stop_tsm.start) {
		if (hw_priv->tsm_info.ac == t.queue)
			hw_priv->tsm_stats.txed_msdu_count++;
	}
	spin_unlock_bh(&hw_priv->tsm_lock);
#endif /*CONFIG_RK960_TESTMODE */

	t.txpriv.if_id = priv->if_id;
	t.hdrlen = ieee80211_hdrlen(t.hdr->frame_control);
	t.da = ieee80211_get_DA(t.hdr);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	if (control) {
		t.sta = control->sta;
		t.sta_priv = (struct rk960_sta_priv *)&t.sta->drv_priv;
	}
#else
	t.sta_priv = (struct rk960_sta_priv *)&t.tx_info->control.sta->drv_priv;
#endif

	if (WARN_ON(t.queue >= 4))
		goto drop;

	if ((ieee80211_is_action(frame->frame_control)) &&
	    (mgmt->u.action.category == WLAN_CATEGORY_PUBLIC)) {
		u8 *action = (u8 *) & mgmt->u.action.category;

		rk960_check_go_neg_conf_success(hw_priv, action);
		rk960_check_prov_desc_req(hw_priv, action);
		rk960_check_inv_resp(hw_priv, priv, action,
				     skb->len - t.hdrlen);
	}

	priv->deauthAll = false;
	if (unlikely(ieee80211_is_deauth(frame->frame_control))) {
		u8 *DA = ieee80211_get_DA(frame);

		if (is_broadcast_ether_addr(DA))
			priv->deauthAll = true;
	}
#if 0
	spin_lock_bh(&hw_priv->tx_queue[t.queue].lock);

	if ((priv->if_id == 0) &&
	    (hw_priv->tx_queue[t.queue].num_queued_vif[0] >=
	     hw_priv->vif0_throttle)) {
		spin_unlock_bh(&hw_priv->tx_queue[t.queue].lock);
		goto drop;
	} else if ((priv->if_id == 1) &&
		   (hw_priv->tx_queue[t.queue].num_queued_vif[1] >=
		    hw_priv->vif1_throttle)) {
		spin_unlock_bh(&hw_priv->tx_queue[t.queue].lock);
		goto drop;
	}

	spin_unlock_bh(&hw_priv->tx_queue[t.queue].lock);
#endif

        tx_fixed_rate_build(hw_priv, &t, IEEE80211_TX_MAX_RATES);

	ret = rk960_tx_h_calc_link_ids(priv, &t);
	if (ret)
		goto drop;

	rk960_tx_h_pae(hw_priv, &t);

	if (t.txpriv.offchannel_if_id == 2 &&
	    (t.hdr->frame_control & __cpu_to_le32(IEEE80211_FCTL_PROTECTED))) {
		RK960_INFO_TXRX("drop protect frame in offchannel_if_id = 2\n");
		dev_kfree_skb(skb);
		return;
	}

	/*RK960_DEBUG_TXRX("[TX] TX %d bytes (if_id: %d,"
	   " queue: %d, link_id: %d (%d)).\n",
	   skb->len, priv->if_id, t.queue, t.txpriv.link_id,
	   t.txpriv.raw_link_id); */

	BUG_ON(!hw_priv->channel);

	ret = rk960_tx_h_action(priv, &t);
	if (ret) {
		RK960_INFO_TXRX("%s: drop addba action frame\n", __func__);
		goto drop;
	}

	rk960_tx_h_pm(priv, &t);
	rk960_tx_h_calc_tid(priv, &t);
	/* for protected frame, mac80211 was not insert cmmp header in skb, 
	   driver will insert ccmp header in rk960_tx_h_crypt */
	ret = rk960_tx_h_crypt(priv, &t);
	if (ret)
		goto drop;
	ret = rk960_tx_h_align(priv, &t, &flags);
	if (ret)
		goto drop;
	wsm = rk960_tx_h_wsm(priv, &t);
	if (!wsm) {
		ret = -ENOMEM;
		goto drop;
	}

#ifdef CONFIG_RK960_TESTMODE
	flags |= WSM_TX_FLAG_EXPIRY_TIME;
#endif /*CONFIG_RK960_TESTMODE */
	wsm->flags |= flags;
	rk960_tx_h_bt(priv, &t, wsm);
	ret = rk960_tx_h_rate_policy(hw_priv, &t, wsm);
	if (ret)
		goto drop;

	ret = rk960_tx_h_skb_pad(hw_priv, wsm, skb);
	if (ret)
		goto drop;

	if (hw_priv->hwbus_ops->align_size(hw_priv->hwbus_priv, skb->len) >
	    RK960_SDIO_TX_MSG_SIZE) {
		RK960_ERROR_TXRX("%s: drop abnormal size(%d) frame\n", __func__,
				skb->len);
                BUG_ON(1);
		goto drop;
	}
        
	rcu_read_lock();
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	sta = rcu_dereference(control->sta);
#else
	sta = rcu_dereference(t.tx_info->control.sta);
#endif

	rk960_tx_h_ba_stat(priv, &t);
	spin_lock_bh(&priv->ps_state_lock);
	{
		tid_update = rk960_tx_h_pm_state(priv, &t);
		BUG_ON(rk960_queue_put(&hw_priv->tx_queue[t.queue],
				       t.skb, &t.txpriv));
#ifdef ROC_DEBUG
		RK960_DEBUG_TXRX("QPUT %x, %pM, if_id - %d\n",
				 t.hdr->frame_control, t.da, priv->if_id);
#endif
	}
	spin_unlock_bh(&priv->ps_state_lock);

#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
	if (tid_update && sta)
		ieee80211_sta_set_buffered(sta, t.txpriv.tid, true);
#endif /* CONFIG_RK960_USE_STE_EXTENSIONS */

	rcu_read_unlock();

	rk960_bh_wakeup(hw_priv);

	return;

drop:
	rk960_debug_tx_drops(hw_priv);
	rk960_skb_dtor(hw_priv, skb, &t.txpriv);
	return;
}

/* ******************************************************************** */

static int rk960_handle_pspoll(struct rk960_vif *priv, struct sk_buff *skb)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);
	struct ieee80211_sta *sta;
	struct ieee80211_pspoll *pspoll = (struct ieee80211_pspoll *)skb->data;
	int link_id = 0;
	u32 pspoll_mask = 0;
	int drop = 1;
	int i;

	if (priv->join_status != RK960_JOIN_STATUS_AP)
		goto done;
	if (memcmp(priv->vif->addr, pspoll->bssid, ETH_ALEN))
		goto done;

	rcu_read_lock();
	sta = ieee80211_find_sta(priv->vif, pspoll->ta);
	if (sta) {
		struct rk960_sta_priv *sta_priv;
		sta_priv = (struct rk960_sta_priv *)&sta->drv_priv;
		link_id = sta_priv->link_id;
		pspoll_mask = BIT(sta_priv->link_id);
	}
	rcu_read_unlock();
	if (!link_id)
		goto done;

	priv->pspoll_mask |= pspoll_mask;
	drop = 0;

	/* Do not report pspols if data for given link id is
	 * queued already. */
	for (i = 0; i < 4; ++i) {
		if (rk960_queue_get_num_queued(priv,
					       &hw_priv->tx_queue[i],
					       pspoll_mask)) {
			rk960_bh_wakeup(hw_priv);
			drop = 1;
			break;
		}
	}
	RK960_DEBUG_TXRX("[RX] PSPOLL: %s\n", drop ? "local" : "fwd");
done:
	return drop;
}

/* ******************************************************************** */

void rk960_tx_confirm_cb(struct rk960_common *hw_priv,
			 struct wsm_tx_confirm *arg)
{
	u8 queue_id = rk960_queue_get_queue_id(arg->packetID);
	struct rk960_queue *queue = &hw_priv->tx_queue[queue_id];
	struct sk_buff *skb;
	const struct rk960_txpriv *txpriv;
	struct rk960_vif *priv;

	//txrx_printk(KERN_DEBUG "[TX] TX confirm: %d, %d.\n",
	//      arg->status, arg->ackFailures);

	if (unlikely(rk960_itp_tx_running(hw_priv)))
		return;

	priv = rk960_hwpriv_to_vifpriv(hw_priv, arg->if_id);
	if (unlikely(!priv))
		return;
	if (unlikely(priv->mode == NL80211_IFTYPE_UNSPECIFIED)) {
		/* STA is stopped. */
		rk960_priv_vif_list_read_unlock(&priv->vif_lock);
		return;
	}

	if (WARN_ON(queue_id >= 4)) {
		rk960_priv_vif_list_read_unlock(&priv->vif_lock);
		return;
	}

	if (arg->status)
		RK960_INFO_TXRX("TX failed: %d ackFailures %d flags %x.\n",
				arg->status, arg->ackFailures, arg->flags);

#ifdef CONFIG_RK960_TESTMODE
	spin_lock_bh(&hw_priv->tsm_lock);
	if ((arg->status == WSM_STATUS_RETRY_EXCEEDED) ||
	    (arg->status == WSM_STATUS_TX_LIFETIME_EXCEEDED)) {
		hw_priv->tsm_stats.msdu_discarded_count++;
	} else if ((hw_priv->start_stop_tsm.start) &&
		   (arg->status == WSM_STATUS_SUCCESS)) {
		if (queue_id == hw_priv->tsm_info.ac) {
			struct timeval tmval;
			do_gettimeofday(&tmval);
			u16 pkt_delay =
			    hw_priv->start_stop_tsm.packetization_delay;
			if (hw_priv->tsm_info.sta_roamed &&
			    !hw_priv->tsm_info.use_rx_roaming) {
				hw_priv->tsm_info.roam_delay = tmval.tv_usec -
				    hw_priv->tsm_info.txconf_timestamp_vo;
				if (hw_priv->tsm_info.roam_delay > pkt_delay)
					hw_priv->tsm_info.roam_delay -=
					    pkt_delay;
				RK960_DEBUG_TXRX("[TX] txConf"
						 "Roaming: roam_delay = %u\n",
						 hw_priv->tsm_info.roam_delay);
				hw_priv->tsm_info.sta_roamed = 0;
			}
			hw_priv->tsm_info.txconf_timestamp_vo = tmval.tv_usec;
		}
	}
	spin_unlock_bh(&hw_priv->tsm_lock);
#endif /*CONFIG_RK960_TESTMODE */
	if ((arg->status == WSM_REQUEUE) &&
	    (arg->flags & WSM_TX_STATUS_REQUEUE)) {
		/* "Requeue" means "implicit suspend" */
		struct wsm_suspend_resume suspend = {
			.link_id = arg->link_id,
			.stop = 1,
			.multicast = !arg->link_id,
			.if_id = arg->if_id,
		};
		rk960_suspend_resume(priv, &suspend);
		RK960_INFO_TXRX("Requeue for link_id %d (try %d)."
				" STAs asleep: 0x%.8X\n",
				arg->link_id,
				rk960_queue_get_generation(arg->packetID) + 1,
				priv->sta_asleep_mask);
#ifdef CONFIG_RK960_TESTMODE
		WARN_ON(rk960_queue_requeue(hw_priv, queue,
					    arg->packetID, true));
#else
		WARN_ON(rk960_queue_requeue(queue, arg->packetID, true));
#endif
		spin_lock_bh(&priv->ps_state_lock);
		if (!arg->link_id) {
			priv->buffered_multicasts = true;
			if (priv->sta_asleep_mask) {
				queue_work(hw_priv->workqueue,
					   &priv->multicast_start_work);
			}
		}
		spin_unlock_bh(&priv->ps_state_lock);
		rk960_priv_vif_list_read_unlock(&priv->vif_lock);
	} else {
	    if (!WARN_ON
		(rk960_queue_get_skb(queue, arg->packetID, &skb, &txpriv))) {
		struct ieee80211_tx_info *tx = IEEE80211_SKB_CB(skb);
		int tx_count = arg->ackFailures;
		u8 ht_flags = 0;
		int i;

		if (hw_priv->max_tx_conf_set) {
			hw_priv->max_tx_conf_set = 0;
			hw_priv->max_tx_conf_queueid = queue->queue_id;
			hw_priv->max_tx_conf_status = arg->status;
			hw_priv->max_tx_conf_txedrate = arg->txedRate;
			hw_priv->max_tx_conf_ackfailures = arg->ackFailures;
			hw_priv->max_tx_conf_mediadelay = arg->mediaDelay;
			hw_priv->max_tx_conf_txQueuedelay = arg->txQueueDelay;
		}
#ifdef ROC_DEBUG
		if (txpriv->offchannel_if_id)
			RK960_DEBUG_TXRX("TX CONFIRM %x - %d - %d\n",
					 skb->data[txpriv->offset],
					 txpriv->offchannel_if_id, arg->status);
#endif

#if 1
		/*
		 * re-transmit some p2p action frame
		 */
		while (arg->status == WSM_STATUS_RETRY_EXCEEDED) {
			struct ieee80211_hdr *frame =
			    (struct ieee80211_hdr *)(skb->data +
						     txpriv->offset);
			struct ieee80211_mgmt *mgmt =
			    (struct ieee80211_mgmt *)frame;
			u8 *action;

			if (frame == NULL) {
				rk960_priv_vif_list_read_unlock(&priv->
								vif_lock);
				BUG_ON(1);
				return;
			}

			if (!ieee80211_is_action(frame->frame_control)) {
				break;
			}

			if (mgmt->u.action.category != WLAN_CATEGORY_PUBLIC) {
				break;
			}

			if (rk960_queue_get_generation(arg->packetID) >= 5) {
				RK960_DEBUG_TXRX
				    ("p2p action retry: too many times(%x)\n",
				     arg->packetID);
				break;
			}

			if (!atomic_read(&hw_priv->remain_on_channel)) {
				break;
			}

			if (txpriv->if_id != hw_priv->roc_if_id) {
				break;
			}

			if (!(tx->flags & IEEE80211_TX_INTFL_NL80211_FRAME_TX)) {
				break;
			}

			if (!(tx->flags & IEEE80211_TX_CTL_REQ_TX_STATUS)) {
				break;
			}

			if (!time_is_after_jiffies(hw_priv->roc_start_time
						   +
						   msecs_to_jiffies(hw_priv->
								    roc_duration)))
			{
				RK960_DEBUG_TXRX("p2p action retry: timeout\n");
				break;
			}

			action = (u8 *) & mgmt->u.action.category;
			/*if (action[2] == 0x50 && action[3] == 0x6F && action[4] == 0x9A &&
										   action[5] == 0x09 && action[6] == 0x04) */  {
										// P2P Invitation Response
				RK960_DEBUG_TXRX("p2p action retry\n");
				WARN_ON(rk960_queue_requeue
					(queue, arg->packetID, false));
				rk960_priv_vif_list_read_unlock(&priv->
								vif_lock);

				rk960_bh_wakeup(hw_priv);
				return;
			}

			//break;
		}
#endif

		if (priv->association_mode.greenfieldMode)
			ht_flags |= IEEE80211_TX_RC_GREEN_FIELD;

		if (likely(!arg->status)) {
			tx->flags |= IEEE80211_TX_STAT_ACK;
			priv->cqm_tx_failure_count = 0;
			++tx_count;
			rk960_debug_txed(priv);
			if (arg->flags & WSM_TX_STATUS_AGGREGATION) {
				/* Do not report aggregation to mac80211:
				 * it confuses minstrel a lot. */
				/* tx->flags |= IEEE80211_TX_STAT_AMPDU; */
				rk960_debug_txed_agg(priv);
			}
		} else {
			spin_lock(&priv->bss_loss_lock);
			if (priv->bss_loss_status ==
			    RK960_BSS_LOSS_CONFIRMING &&
			    priv->bss_loss_confirm_id == arg->packetID) {
				priv->bss_loss_status =
				    RK960_BSS_LOSS_CONFIRMED;
				spin_unlock(&priv->bss_loss_lock);
				cancel_delayed_work(&priv->bss_loss_work);
				queue_delayed_work(hw_priv->workqueue,
						   &priv->bss_loss_work, 0);
			} else
				spin_unlock(&priv->bss_loss_lock);

			/* TODO: Update TX failure counters */
			if (unlikely(priv->cqm_tx_failure_thold &&
				     (++priv->cqm_tx_failure_count >
				      priv->cqm_tx_failure_thold))) {
				priv->cqm_tx_failure_thold = 0;
				queue_work(hw_priv->workqueue,
					   &priv->tx_failure_work);
			}
			if (tx_count)
				++tx_count;
		}
		rk960_priv_vif_list_read_unlock(&priv->vif_lock);

		for (i = 0; i < IEEE80211_TX_MAX_RATES; ++i) {
			if (tx->status.rates[i].count >= tx_count) {
				tx->status.rates[i].count = tx_count;
				break;
			}
			tx_count -= tx->status.rates[i].count;
			if (tx->status.rates[i].flags & IEEE80211_TX_RC_MCS)
				tx->status.rates[i].flags |= ht_flags;
		}

		for (++i; i < IEEE80211_TX_MAX_RATES; ++i) {
			tx->status.rates[i].count = 0;
			tx->status.rates[i].idx = -1;
		}

	/*pr_info("tx_conf ackfail %d [%d %d %x] [%d %d %x] [%d %d %x] [%d %d %x]\n",
	   (int)arg->ackFailures,
	   tx->status.rates[0].idx, tx->status.rates[0].count, tx->status.rates[0].flags,
	   tx->status.rates[1].idx, tx->status.rates[1].count, tx->status.rates[1].flags,
	   tx->status.rates[2].idx, tx->status.rates[2].count, tx->status.rates[2].flags,
	   tx->status.rates[3].idx, tx->status.rates[3].count, tx->status.rates[3].flags);*/
                
#ifdef CONFIG_RK960_TESTMODE
		rk960_queue_remove(hw_priv, queue, arg->packetID);
#else
		rk960_queue_remove(queue, arg->packetID);
#endif /*CONFIG_RK960_TESTMODE */
                } else {
                        rk960_priv_vif_list_read_unlock(&priv->vif_lock);
                }
        }
}

static void rk960_notify_buffered_tx(struct rk960_vif *priv,
				     struct sk_buff *skb, int link_id, int tid)
{
#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
	struct ieee80211_sta *sta;
	struct ieee80211_hdr *hdr;
	u8 *buffered;
	u8 still_buffered = 0;

	if (link_id && tid < RK960_MAX_TID) {
		buffered = priv->link_id_db[link_id - 1].buffered;

		spin_lock_bh(&priv->ps_state_lock);
		if (!WARN_ON(!buffered[tid]))
			still_buffered = --buffered[tid];
		spin_unlock_bh(&priv->ps_state_lock);

		if (!still_buffered && tid < RK960_MAX_TID) {
			hdr = (struct ieee80211_hdr *)skb->data;
			rcu_read_lock();
			sta = ieee80211_find_sta(priv->vif, hdr->addr1);
			if (sta)
				ieee80211_sta_set_buffered(sta, tid, false);
			rcu_read_unlock();
		}
	}
#endif /* CONFIG_RK960_USE_STE_EXTENSIONS */
}

void rk960_skb_dtor(struct rk960_common *hw_priv,
		    struct sk_buff *skb, const struct rk960_txpriv *txpriv)
{
	struct rk960_vif *priv =
	    __rk960_hwpriv_to_vifpriv(hw_priv, txpriv->if_id);

	skb_pull(skb, txpriv->offset);
	if (priv && txpriv->rate_id != RK960_INVALID_RATE_ID) {
		rk960_notify_buffered_tx(priv, skb,
					 txpriv->raw_link_id, txpriv->tid);
		tx_policy_put(hw_priv, txpriv->rate_id);
	}
	if (likely(!rk960_is_itp(hw_priv)))
		ieee80211_tx_status(hw_priv->hw, skb);
	//ieee80211_tx_status_irqsafe(hw_priv->hw, skb);

}

#ifdef CONFIG_RK960_TESTMODE
/* TODO It should be removed before official delivery */
static void frame_hexdump(char *prefix, u8 * data, int len)
{
	int i;

	RK960_DEBUG_TXRX("%s hexdump:\n", prefix);
	for (i = 0; i < len; i++) {
		if (i + 10 < len) {
			RK960_DEBUG_TXRX("%.1X %.1X %.1X %.1X"
					 "%.1X %.1X %.1X %.1X %.1X %.1X",
					 data[i], data[i + 1], data[i + 2],
					 data[i + 3], data[i + 4], data[i + 5],
					 data[i + 6], data[i + 7], data[i + 8],
					 data[i + 9]);
			i += 9;
		} else {
			RK960_DEBUG_TXRX("%.1X ", data[i]);
		}
	}
}

/**
 * c1200_tunnel_send_testmode_data - Send test frame to the driver
 *
 * @priv: pointer to rk960 private structure
 * @skb: skb with frame
 *
 * Returns: 0 on success or non zero value on failure
 */
static int rk960_tunnel_send_testmode_data(struct rk960_common *hw_priv,
					   struct sk_buff *skb)
{
	if (rk960_tesmode_event(hw_priv->hw->wiphy, STE_MSG_EVENT_FRAME_DATA,
				skb->data, skb->len, GFP_ATOMIC))
		return -EINVAL;

	return 0;
}

/**
 * rk960_frame_test_detection - Detection frame_test
 *
 * @priv: pointer to rk960 vif structure
 * @frame: ieee80211 header
 * @skb: skb with frame
 *
 * Returns: 1 - frame test detected, 0 - not detected
 */
static int rk960_frame_test_detection(struct rk960_vif *priv,
				      struct ieee80211_hdr *frame,
				      struct sk_buff *skb)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);
	int hdrlen = ieee80211_hdrlen(frame->frame_control);
	int detected = 0;
	int ret;

	if (hdrlen + hw_priv->test_frame.len <= skb->len &&
	    memcmp(skb->data + hdrlen, hw_priv->test_frame.data,
		   hw_priv->test_frame.len) == 0) {
		detected = 1;
		RK960_DEBUG_TXRX("TEST FRAME detected");
		frame_hexdump("TEST FRAME original:", skb->data, skb->len);
		ret = ieee80211_data_to_8023(skb, hw_priv->mac_addr,
					     priv->mode);
		if (!ret) {
			frame_hexdump("FRAME 802.3:", skb->data, skb->len);
			ret = rk960_tunnel_send_testmode_data(hw_priv, skb);
		}
		if (ret)
			RK960_DEBUG_TXRX("Send TESTFRAME failed(%d)", ret);
	}
	return detected;
}
#endif /* CONFIG_RK960_TESTMODE */

static void
rk960_rx_h_ba_stat(struct rk960_vif *priv, size_t hdrlen, size_t skb_len)
{
	struct rk960_common *hw_priv = priv->hw_priv;
	if (priv->join_status != RK960_JOIN_STATUS_STA)
		return;
	if (!rk960_is_ht(&hw_priv->ht_info))
		return;
	if (!priv->setbssparams_done)
		return;

	spin_lock_bh(&hw_priv->ba_lock);
	hw_priv->ba_acc_rx += skb_len - hdrlen;
	if (!(hw_priv->ba_cnt_rx || hw_priv->ba_cnt)) {
		mod_timer(&hw_priv->ba_timer,
			  jiffies + RK960_BLOCK_ACK_INTERVAL);
	}
	hw_priv->ba_cnt_rx++;
	spin_unlock_bh(&hw_priv->ba_lock);
}

#ifdef RK960_CSYNC_ADJUST

void rk960_csync_param_init(struct rk960_common *hw_priv)
{
	struct csync_params *csync = &hw_priv->csync_params;

	RK960_DEBUG_TXRX("%s\n", __func__);

	memset(csync, 0, sizeof(struct csync_params));
	// 0 means default csync setting
	csync->cur_seted_thresh = 0;
}

void rk960_csync_clr_history(struct rk960_common *hw_priv, int index)
{
	struct csync_params *csync = &hw_priv->csync_params;

	csync->last_sam_size[index] = csync->sam_size[index];
	csync->sam_read[index] = 0;
	csync->sam_write[index] = 0;
	csync->sam_size[index] = 0;
}

/*
 *         read           write
 *  |_____|_________|______|
 *                   size
 */
#define SAM_STEP(x, y)						\
	do {								\
		x++;							\
		x = x % y;	\
	} while(0)

static s8 rk960_csync_get_sample(struct rk960_common *hw_priv, int index)
{
	struct csync_params *csync = &hw_priv->csync_params;
	s8 sample;

	sample = csync->sam_history[index][csync->sam_read[index]];
	SAM_STEP(csync->sam_read[index], RK960_CSYNC_MAX_RSSI_SAMPLE);

	return sample;
}

static void rk960_csync_set_sample(struct rk960_common *hw_priv, s8 rss,
				   int index)
{
	struct csync_params *csync = &hw_priv->csync_params;

	if (csync->sam_size[index] >= RK960_CSYNC_MAX_RSSI_SAMPLE) {
		return;
	}

	csync->sam_history[index][csync->sam_write[index]] = rss;
	SAM_STEP(csync->sam_write[index], RK960_CSYNC_MAX_RSSI_SAMPLE);
	csync->sam_size[index]++;
}

static void rk960_csync_clr_accum(struct rk960_common *hw_priv, int index)
{
	struct csync_params *csync = &hw_priv->csync_params;

	RK960_DEBUG_TXRX("%s\n", __func__);

	csync->thresh_accum[index] = 0;
	csync->avg_thresh[index] = 0;
	csync->new_thresh[index] = 0;
}

static void rk960_csync_calc_accum(struct rk960_common *hw_priv, s8 rss,
				   int index)
{
	struct csync_params *csync = &hw_priv->csync_params;
	unsigned int update, b, c_rss;
	int adapt_thresh_offset = RK960_CSYNC_THRESH_OFFSET;
	int adapt_thresh_exponent = RK960_CSYNC_THRESH_EXPONENT;

	if (rss < -110)
		rss = -110;

	// convert rss from dBm
	c_rss = (110 + rss) * 2;

	if (c_rss > adapt_thresh_offset)
		update = (c_rss - adapt_thresh_offset) << 4;
	else
		update = 0;
	b = (1 << adapt_thresh_exponent) - 1;

	if (!csync->thresh_accum[index])
		csync->thresh_accum[index] = update << adapt_thresh_exponent;

	csync->thresh_accum[index] =
	    b * (csync->thresh_accum[index] >> adapt_thresh_exponent) + update;

	//RK960_DEBUG_TXRX("idx(%d) rss = %d, c_rss = %d, update = %d, accum = %d\n",
	//                      index, rss, c_rss, update,
	//                      csync->thresh_accum[index]);
}

static void rk960_csync_calc_new_thresh(struct rk960_common *hw_priv, int index)
{
	struct csync_params *csync = &hw_priv->csync_params;
	int adapt_thresh_exponent = 4;

	csync->avg_thresh[index] =
	    csync->thresh_accum[index] >> (adapt_thresh_exponent + 4);
	csync->new_thresh[index] =
	    max((unsigned int)RK960_CSYNC_THRESH_MIN,
		min(220 - csync->avg_thresh[index],
		    (unsigned int)RK960_CSYNC_THRESH_MAX));

	RK960_DEBUG_TXRX("idx(%d): accum = %d, avg = %d, new = %d\n", index,
			 csync->thresh_accum[index], csync->avg_thresh[index],
			 csync->new_thresh[index]);
}

void rk960_csync_save_history_thresh(struct rk960_common *hw_priv, int thresh)
{
	struct csync_params *csync = &hw_priv->csync_params;

	if (thresh >= csync->last_thresh + 10 ||
	    thresh <= csync->last_thresh - 10) {
		RK960_DEBUG_TXRX("cur thresh = %d, last thresh = %d\n", thresh,
				 csync->last_thresh);
	}
	csync->last_thresh = thresh;

	csync->thr_history[csync->cur_thr_offset] = thresh;
	SAM_STEP(csync->cur_thr_offset, RK960_CSYNC_SETED_THRESH_COUNT);
}

int rk960_csync_set_thresh(struct rk960_common *hw_priv, int thresh)
{
	struct csync_params *csync = &hw_priv->csync_params;
	int need_update = 0;

	if (csync->csync_disable) {
		return 0;
	}

	if (csync->cur_seted_thresh != thresh) {
		RK960_DEBUG_TXRX("%s: thresh = %d\n", __func__, thresh);
		csync->cur_seted_thresh = thresh;
		need_update = 1;
		csync->to_fw_csync = (s8) (-1) * (csync->cur_seted_thresh / 2);
		rk960_csync_save_history_thresh(hw_priv, thresh);
	}

	return need_update;
}

static int rk960_csync_process(struct rk960_common *hw_priv, int index)
{
	struct csync_params *csync = &hw_priv->csync_params;
	int i;

	RK960_DEBUG_TXRX("%s: index %d sam_size = %d\n", __func__, index,
			 csync->sam_size[index]);

	rk960_csync_clr_accum(hw_priv, index);

	if (csync->sam_size[index] < RK960_CSYNC_MIN_RSSI_SAMPLE) {
		return 0;
	}

	if (csync->sam_size[index] > RK960_CSYNC_MAX_RSSI_SAMPLE)
		csync->sam_size[index] = RK960_CSYNC_MAX_RSSI_SAMPLE;

	for (i = 0; i < csync->sam_size[index]; i++) {
		rk960_csync_calc_accum(hw_priv,
				       rk960_csync_get_sample(hw_priv, index),
				       index);
	}

	rk960_csync_calc_new_thresh(hw_priv, index);
	return 1;
}

void rk960_csync_work(struct work_struct *work)
{
	struct rk960_common *hw_priv =
	    container_of(work, struct rk960_common, csync_work);
	struct csync_params *csync = &hw_priv->csync_params;

	wsm_set_csync_thr(hw_priv, csync->to_fw_csync, 0);
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
void rk960_csync_timer(struct timer_list *t)
{
	struct rk960_common *hw_priv = from_timer(hw_priv, t, csync_timer);
#else
void rk960_csync_timer(unsigned long arg)
{
	struct rk960_common *hw_priv = (struct rk960_common *)arg;
#endif

	int i;
	int update = 0;
	int sta = 0;
	int ap = 0;
	struct rk960_vif *priv = NULL;
	struct rk960_vif *priv2 = NULL;
	struct csync_params *csync = &hw_priv->csync_params;

	hw_priv->last_tx_counter = atomic_read(&hw_priv->tx_counter);
	atomic_set(&hw_priv->tx_counter, 0);
	hw_priv->last_rx_counter = atomic_read(&hw_priv->rx_counter);
	atomic_set(&hw_priv->rx_counter, 0);

	csync->timer_hit++;
	mod_timer(&hw_priv->csync_timer, jiffies + RK960_CSYNC_ADJUST_INTERVAL);

	spin_lock(&hw_priv->csync_lock);

	if (csync->csync_disable ||
	    atomic_read(&hw_priv->scan.in_progress) ||
	    atomic_read(&hw_priv->remain_on_channel) ||
	    hw_priv->wow_suspend_done || hw_priv->fw_error_processing) {
		goto csync_out;
	}

	rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
	for (i = 0; i < 2; i++) {
		if (hw_priv->if_id_slot & BIT(i)) {
			priv =
			    rk960_get_vif_from_ieee80211(hw_priv->vif_list[i]);
			if (priv && priv->join_status == RK960_JOIN_STATUS_STA) {
				sta++;
				priv2 = priv;
			} else if (priv
				   && priv->join_status ==
				   RK960_JOIN_STATUS_AP) {
				ap++;
			}
		}
	}
	rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);

	if (!priv && !priv2)
		goto csync_out;

	if (sta == 1 && !ap) {
		update = rk960_csync_process(hw_priv, priv2->if_id);
		if (update)
			rk960_csync_set_thresh(hw_priv,
					       csync->new_thresh[priv2->if_id]);
	} else {
		update =
		    rk960_csync_set_thresh(hw_priv, RK960_CSYNC_DEFAULT_THRESH);
	}

csync_out:
	rk960_csync_clr_history(hw_priv, 0);
	rk960_csync_clr_history(hw_priv, 1);
	spin_unlock(&hw_priv->csync_lock);

	if (update) {
		csync->set_hit++;
		queue_work(hw_priv->workqueue, &hw_priv->csync_work);
	}
}

void rk960_csync_set_rssi(struct rk960_common *hw_priv, s8 rssi)
{
	struct csync_params *csync = &hw_priv->csync_params;
	int update;
        int thresh;

        thresh = (s8) (-1) * rssi;
        thresh = thresh * 2 + RK960_CSYNC_THRESH_OFFSET;
        if (thresh < RK960_CSYNC_THRESH_MIN)
                thresh = RK960_CSYNC_THRESH_MIN;
        else if (thresh > RK960_CSYNC_THRESH_MAX)
                thresh = RK960_CSYNC_THRESH_MAX;

        RK960_DEBUG_TXRX("%s: thresh %d\n", __func__, thresh);
	spin_lock_bh(&hw_priv->csync_lock);
	update = rk960_csync_set_thresh(hw_priv, thresh);
	spin_unlock_bh(&hw_priv->csync_lock);

	if (update)
		wsm_set_csync_thr(hw_priv, csync->to_fw_csync, 0);
}

/*
 * set defaut phy thresh when scan
 * and stop dapt
 */
void rk960_csync_scan(struct rk960_common *hw_priv)
{
	struct csync_params *csync = &hw_priv->csync_params;
	int update;
        int thresh = RK960_CSYNC_DEFAULT_THRESH;

        //if (hw_priv->wsm_caps.firmwareSvnVersion >= 7989)
        //        thresh = RK960_CSYNC_DEFAULT_SCAN;

	spin_lock_bh(&hw_priv->csync_lock);
	csync->restore_thresh = csync->cur_seted_thresh;
	update = rk960_csync_set_thresh(hw_priv, thresh);
	spin_unlock_bh(&hw_priv->csync_lock);

	if (update)
		wsm_set_csync_thr(hw_priv, csync->to_fw_csync, 0);
}

/* restart dapt */
void rk960_csync_scan_complete(struct rk960_common *hw_priv)
{
#if 0				// restore csync in rk960_csync_timer
	struct csync_params *csync = &hw_priv->csync_params;
	int update;

	spin_lock_bh(&hw_priv->csync_lock);
	update = rk960_csync_set_thresh(hw_priv, csync->restore_thresh);
	spin_unlock_bh(&hw_priv->csync_lock);

	if (update)
		wsm_set_csync_thr(hw_priv, csync->to_fw_csync, 0);
#endif
}

void rk960_csync_join(struct rk960_common *hw_priv, s8 rssi)
{
	int i;
	int sta = 0;
	int ap = 0;
	struct rk960_vif *priv = NULL;
	struct rk960_vif *priv2 = NULL;
        
	rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
	for (i = 0; i < 2; i++) {
		if (hw_priv->if_id_slot & BIT(i)) {
			priv =
			    rk960_get_vif_from_ieee80211(hw_priv->vif_list[i]);
			if (priv && priv->join_status == RK960_JOIN_STATUS_STA) {
				sta++;
				priv2 = priv;
			} else if (priv
				   && priv->join_status ==
				   RK960_JOIN_STATUS_AP) {
				ap++;
			}
		}
	}
	rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);

	if (!priv && !priv2)
		return;

	if (sta == 0 && !ap) {
                RK960_DEBUG_TXRX("%s: rssi %d\n", __func__, rssi);
		rk960_csync_set_rssi(hw_priv, rssi);
	} else {
		rk960_csync_scan(hw_priv);
	}
}

static void rk960_csync_rx(struct rk960_common *hw_priv,
			   struct ieee80211_hdr *hdr, s8 rssi)
{
	struct csync_params *csync = &hw_priv->csync_params;
	int i;
	struct rk960_vif *priv;

	atomic_add_return(1, &hw_priv->rx_counter);
	if (!csync->csync_disable) {
		u8 *bssid = ieee80211_get_BSSID(hdr);

		//RK960_DEBUG_TXRX("BSSID: %pM, SA: %pM, DA: %pM\n",
		//                      bssid, ieee80211_get_SA(hdr), ieee80211_get_DA(hdr));
		if (bssid) {
			for (i = 0; i < 2; i++) {
				if (hw_priv->if_id_slot & BIT(i)) {
					priv =
					    rk960_get_vif_from_ieee80211
					    (hw_priv->vif_list[i]);
					if (priv
					    && priv->join_status ==
					    RK960_JOIN_STATUS_STA) {
						if (ether_addr_equal
						    (bssid, priv->join_bssid)) {
							spin_lock_bh(&hw_priv->
								     csync_lock);
							rk960_csync_set_sample
							    (hw_priv, rssi,
							     priv->if_id);
							spin_unlock_bh
							    (&hw_priv->
							     csync_lock);
							break;
						}
					}
				}
			}
		}
	}
}

bool rk960_is_txrx_burst(struct rk960_common *hw_priv)
{
	if (hw_priv->last_tx_counter >= RK960_SCAN_TXRX_BURST_THRESHOLD)
		return true;
	else if (hw_priv->last_rx_counter >= RK960_SCAN_TXRX_BURST_THRESHOLD)
		return true;
	return false;
}
#else
bool rk960_is_txrx_burst(struct rk960_common *hw_priv)
{
	return false;
}
#endif

bool rk960_is_apsta_cowork(struct rk960_common *hw_priv)
{
	int i;
	int sta = 0;
	int ap = 0;
	struct rk960_vif *priv = NULL;
	struct rk960_vif *priv2 = NULL;
        
	rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
	for (i = 0; i < 2; i++) {
		if (hw_priv->if_id_slot & BIT(i)) {
			priv =
			    rk960_get_vif_from_ieee80211(hw_priv->vif_list[i]);
			if (priv && priv->join_status == RK960_JOIN_STATUS_STA) {
				sta++;
				priv2 = priv;
			} else if (priv
				   && priv->join_status ==
				   RK960_JOIN_STATUS_AP) {
				ap++;
			}
		}
	}
	rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);

        return ap > 0 && sta > 0;
}

static u64 rk960_get_systime_us(void)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
	struct timespec64 ts;
#else
	struct timespec ts;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
	ts = ktime_to_timespec64(ktime_get_boottime());
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 20, 0)
	ts = ktime_to_timespec(ktime_get_boottime());
#else
	get_monotonic_boottime(&ts);
#endif
	return ((u64) ts.tv_sec * 1000000) + ts.tv_nsec / 1000;
#else
	struct timeval tv;

	do_gettimeofday(&tv);
	return ((u64) tv.tv_sec * 1000000) + tv.tv_usec;
#endif
}

void rk960_add_scan_resp_timestamp(struct ieee80211_hdr *hdr)
{
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)hdr;

	if (ieee80211_is_beacon(hdr->frame_control)) {
		mgmt->u.beacon.timestamp = cpu_to_le64(rk960_get_systime_us());
	}

	if (ieee80211_is_probe_resp(hdr->frame_control)) {
		mgmt->u.probe_resp.timestamp =
		    cpu_to_le64(rk960_get_systime_us());
	}
}

u32 rk960_ieee802_11_parse_elems(const u8 * start, size_t len, bool action,
				 struct ieee802_11_elems *elems,
				 u64 filter, u32 crc)
{
	size_t left = len;
	const u8 *pos = start;
	bool calc_crc = filter != 0;
	DECLARE_BITMAP(seen_elems, 256);

	bitmap_zero(seen_elems, 256);
	memset(elems, 0, sizeof(*elems));
	elems->ie_start = start;
	elems->total_len = len;

	while (left >= 2) {
		u8 id, elen;
		bool elem_parse_failed;

		id = *pos++;
		elen = *pos++;
		left -= 2;

		if (elen > left) {
			elems->parse_error = true;
			break;
		}

		switch (id) {
		case WLAN_EID_SSID:
		case WLAN_EID_SUPP_RATES:
		case WLAN_EID_FH_PARAMS:
		case WLAN_EID_DS_PARAMS:
		case WLAN_EID_CF_PARAMS:
		case WLAN_EID_TIM:
		case WLAN_EID_IBSS_PARAMS:
		case WLAN_EID_CHALLENGE:
		case WLAN_EID_RSN:
		case WLAN_EID_ERP_INFO:
		case WLAN_EID_EXT_SUPP_RATES:
		case WLAN_EID_HT_CAPABILITY:
		case WLAN_EID_HT_OPERATION:
		case WLAN_EID_VHT_CAPABILITY:
		case WLAN_EID_VHT_OPERATION:
		case WLAN_EID_MESH_ID:
		case WLAN_EID_MESH_CONFIG:
		case WLAN_EID_PEER_MGMT:
		case WLAN_EID_PREQ:
		case WLAN_EID_PREP:
		case WLAN_EID_PERR:
		case WLAN_EID_RANN:
		case WLAN_EID_CHANNEL_SWITCH:
		case WLAN_EID_EXT_CHANSWITCH_ANN:
		case WLAN_EID_COUNTRY:
		case WLAN_EID_PWR_CONSTRAINT:
		case WLAN_EID_TIMEOUT_INTERVAL:
		case WLAN_EID_SECONDARY_CHANNEL_OFFSET:
		case WLAN_EID_WIDE_BW_CHANNEL_SWITCH:
			/*
			 * not listing WLAN_EID_CHANNEL_SWITCH_WRAPPER -- it seems possible
			 * that if the content gets bigger it might be needed more than once
			 */
			if (test_bit(id, seen_elems)) {
				elems->parse_error = true;
				left -= elen;
				pos += elen;
				continue;
			}
			break;
		}

		if (calc_crc && id < 64 && (filter & (1ULL << id)))
			crc = crc32_be(crc, pos - 2, elen + 2);

		elem_parse_failed = false;

		switch (id) {
		case WLAN_EID_SSID:
			elems->ssid = pos;
			elems->ssid_len = elen;
			break;
		default:
			break;
		}

		if (elem_parse_failed)
			elems->parse_error = true;
		else
			__set_bit(id, seen_elems);

		left -= elen;
		pos += elen;
	}

	if (left != 0)
		elems->parse_error = true;

	return crc;
}

void rk960_check_ssid_ie(struct sk_buff *skb)
{
	struct ieee80211_mgmt *mgmt = (void *)skb->data;
	u8 *elements;
	size_t baselen;
	struct ieee802_11_elems elems;
	u8 *pos;

	if (skb->len < 24 ||
	    (!ieee80211_is_probe_resp(mgmt->frame_control) &&
	     !ieee80211_is_beacon(mgmt->frame_control)))
		return;

	if (ieee80211_is_probe_resp(mgmt->frame_control)) {
		elements = mgmt->u.probe_resp.variable;
		baselen =
		    offsetof(struct ieee80211_mgmt, u.probe_resp.variable);
	} else {
		baselen = offsetof(struct ieee80211_mgmt, u.beacon.variable);
		elements = mgmt->u.beacon.variable;
	}

	if (baselen > skb->len)
		return;

	rk960_ieee802_11_parse_elems(elements, skb->len - baselen,
				     false, &elems, 0, 0);
	if (elems.ssid_len > 32) {
		pos = (u8 *) elems.ssid - 1;
		*pos = 0;
	}
}

#define WPA_KEY_INFO_KEY_TYPE BIT(3) /* 1 = Pairwise, 0 = Group key */
#define WPA_KEY_INFO_MIC BIT(8)
#define WPA_KEY_INFO_SECURE BIT(9)

int rk960_pae_type(struct sk_buff *skb, int tx)
{
	struct ieee80211_hdr *frame = (struct ieee80211_hdr *)skb->data;
	size_t hdrlen = ieee80211_hdrlen(frame->frame_control);

	if (ieee80211_is_data(frame->frame_control) && skb->len < 512) {
                u16 *ethertype;
                
                if (!tx && ieee80211_has_protected(frame->frame_control))
                        hdrlen += ieee80211_crypt_hdrlen(
                                frame->frame_control);	/* crypt header */
		ethertype = (u16 *) (&skb->data[hdrlen + 6]);

		if (unlikely(*ethertype == __be16_to_cpu(ETH_P_PAE))) {
                        // skip 2 byte ethertype
                        u8 *ieee802_1x_hdr = (u8 *)(ethertype + 1);
                        // skip 4 byte ieee802_1x_hdr
                        u8 *wpa_eapol_key = ieee802_1x_hdr + 4;
                        u16 key_info;

                        wpa_eapol_key += 1; // skip 1 byte type
                        key_info = (wpa_eapol_key[0] << 8) |
                                        wpa_eapol_key[1];
                	if (key_info & WPA_KEY_INFO_KEY_TYPE) {
                		if (key_info & WPA_KEY_INFO_MIC) {
                                        if (!tx) {
                			        /* 3/4 4-Way Handshake */
                                                return 
                                                RK960_FWCR_FRAME_TYPE_3_4WAY;
                                        } else if (tx &&
                                                (key_info &
                                                WPA_KEY_INFO_SECURE)) {
                                                /* 4/4 4-Way Handshake */
                                                return 
                                                RK960_FWCR_FRAME_TYPE_4_4WAY;
                                        } else if (tx) {
                                                /* 2/4 4-Way Handshake */
                                                return
                                                RK960_FWCR_FRAME_TYPE_2_4WAY;
                                        }
                                               
                		} else {
                			/* 1/4 4-Way Handshake */
                                        return RK960_FWCR_FRAME_TYPE_1_4WAY;
                		}
                	} else {
                		if (key_info & WPA_KEY_INFO_MIC) {
                			/* 1/2 Group Key Handshake */
                                        return  RK960_FWCR_FRAME_TYPE_1_2GRO;
                		}
                	}
		}
	}

	return 0;
}

#ifdef SUPPORT_FWCR
void rk960_fwcr_init(struct rk960_common *hw_priv)
{
        RK960_INFO_TXRX("%s\n", __func__);
        
        mutex_init(&hw_priv->fwcr_mux);
        
        hw_priv->fwcr_bcn_cnt = 0;
        hw_priv->fwcr_4way_set = 0;
        hw_priv->fwcr_fw_resumed = 1;
        hw_priv->fwcr_update_key = 1;
        memset(hw_priv->fwcr_bssid, 0, 6);
        
        memset(hw_priv->fwcr_bcn, 0,
                sizeof(struct rk960_fwcr_frame_s)
                * RK960_FWCR_BCN_CNT);
        memset(hw_priv->fwcr_auth, 0,
                sizeof(struct rk960_fwcr_frame_s)
                * RK960_FWCR_AUTH_CNT);
        memset(hw_priv->fwcr_group, 0,
                sizeof(struct rk960_fwcr_frame_s)
                * (RK960_FWCR_GROUP_CNT + 1));
}

void rk960_fwcr_deinit(struct rk960_common *hw_priv)
{
        int i;
        struct rk960_fwcr_frame_s *frame;

        if (hw_priv->fwcr_recovery)
                return;

        RK960_INFO_TXRX("%s\n", __func__);

        mutex_lock(&hw_priv->fwcr_mux);
        
        for (i = 0; i < RK960_FWCR_BCN_CNT; i++) {
                frame = &hw_priv->fwcr_bcn[i];
                if (frame->arg)
                        kfree(frame->arg);
                if (frame->frame)
                        kfree(frame->frame);
                memset(frame, 0, sizeof(struct rk960_fwcr_frame_s));
        }

        for (i = 0; i < RK960_FWCR_AUTH_CNT; i++) {
                frame = &hw_priv->fwcr_auth[i];
                if (frame->arg)
                        kfree(frame->arg);
                if (frame->frame)
                        kfree(frame->frame);
                memset(frame, 0, sizeof(struct rk960_fwcr_frame_s));
        }

        for (i = 0; i < RK960_FWCR_GROUP_CNT + 1; i++) {
                frame = &hw_priv->fwcr_group[i];
                if (frame->arg)
                        kfree(frame->arg);
                if (frame->frame)
                        kfree(frame->frame);
                memset(frame, 0, sizeof(struct rk960_fwcr_frame_s));
        }

        hw_priv->fwcr_bcn_cnt = 0;
        hw_priv->fwcr_4way_set = 0;

        mutex_unlock(&hw_priv->fwcr_mux);
}

void rk960_fwcr_frame_capture(struct rk960_common *hw_priv,
        struct wsm_rx *arg, struct sk_buff *skb)
{
        struct rk960_fwcr_frame_s *frame = NULL;
        struct ieee80211_hdr *hdr =
                (struct ieee80211_hdr *)skb->data;
        int type = 0;

        if (hw_priv->fwcr_recovery)
                return;

        mutex_lock(&hw_priv->fwcr_mux);

        if (ieee80211_is_probe_resp(hdr->frame_control) ||
            ieee80211_is_beacon(hdr->frame_control)) {
                if (hw_priv->fwcr_4way_set)
                        goto fwcr_out;
                type = RK960_FWCR_FRAME_TYPE_PROBE_RESP;
                if (ieee80211_is_beacon(hdr->frame_control))
                        type = RK960_FWCR_FRAME_TYPE_BEACON;
                if (hw_priv->fwcr_bcn_cnt >= RK960_FWCR_BCN_CNT) {
                        RK960_ERROR_TXRX("%s: fwcr_frm_cnt over\n",
                                __func__);
                        goto fwcr_out;
                }
                frame = &hw_priv->fwcr_bcn[hw_priv->fwcr_bcn_cnt++];
        } else if (ieee80211_is_auth(hdr->frame_control)) {
                type = RK960_FWCR_FRAME_TYPE_AUTH;
                frame = &hw_priv->fwcr_auth[0];
        } else if (ieee80211_is_assoc_resp(hdr->frame_control)) {
                type = RK960_FWCR_FRAME_TYPE_ASSOC;
                frame = &hw_priv->fwcr_auth[1];
        } else {
                type = rk960_pae_type(skb, 0);
                if (type == RK960_FWCR_FRAME_TYPE_1_4WAY) {
                        frame = &hw_priv->fwcr_auth[2];
                } else if (type == RK960_FWCR_FRAME_TYPE_3_4WAY) {
                        frame = &hw_priv->fwcr_auth[3];
                        hw_priv->fwcr_4way_set = 1;
                } else if (type == RK960_FWCR_FRAME_TYPE_1_2GRO) {
                        frame = &hw_priv->fwcr_group[RK960_FWCR_GROUP_CNT];
                }
        }

        if (frame) {
                RK960_DEBUG_TXRX("%s: type %x cnt %d len %d\n",
                                __func__, type,
                                hw_priv->fwcr_bcn_cnt, skb->len);
                if (frame->arg)
                        kfree(frame->arg);
                if (frame->frame)
                        kfree(frame->frame);
                frame->arg = kmalloc(sizeof(struct wsm_rx), GFP_KERNEL);
                if (!frame->arg) {
                        RK960_ERROR_TXRX("%s: malloc arg failed\n",
                                __func__);
                        goto fwcr_out;
                }
                memcpy(frame->arg, arg, sizeof(struct wsm_rx));
                
                frame->frame = kmalloc(skb->len, GFP_KERNEL);
                if (!frame->frame) {
                        kfree(frame->arg);
                        RK960_ERROR_TXRX("%s: malloc frame failed\n",
                                __func__);
                        goto fwcr_out;
                }
                memcpy(frame->frame, skb->data, skb->len);

                frame->type = type;
                frame->frame_len = skb->len;
        }

fwcr_out:
        mutex_unlock(&hw_priv->fwcr_mux);
}

void rk960_fwcr_group_frame_capture(struct rk960_common *hw_priv,
        int key_id)
{/*
        struct rk960_fwcr_frame_s *frame_src;
        struct rk960_fwcr_frame_s *frame_dst;
        
        BUG_ON(!key_id || key_id > RK960_FWCR_GROUP_CNT);

        mutex_lock(&hw_priv->fwcr_mux);

        key_id -= 1;
        frame_src = &hw_priv->fwcr_group[RK960_FWCR_GROUP_CNT];
        frame_dst = &hw_priv->fwcr_group[key_id];

        if (frame_src->arg == NULL ||
                frame_src->frame == NULL) {
                mutex_unlock(&hw_priv->fwcr_mux);
                return;
        }
        
        if (frame_dst->arg)
                kfree(frame_dst->arg);
        if (frame_dst->frame)
                kfree(frame_dst->frame);
        frame_dst->arg = frame_src->arg;
        frame_dst->frame = frame_src->frame;
        frame_dst->type = frame_src->type;
        frame_dst->frame_len = frame_src->frame_len;
        frame_src->arg = NULL;
        frame_src->frame = NULL;

        RK960_DEBUG_TXRX("%s: type %x key_id %d len %d\n",
                        __func__, frame_dst->type,
                        key_id, frame_dst->frame_len);

        mutex_unlock(&hw_priv->fwcr_mux);*/
}

void rk960_fwcr_write(struct rk960_common *hw_priv)
{
        int i;
        struct rk960_fwcr_frame_s *frame;
        int total = 0;
	int sta = 0;
	struct rk960_vif *priv = NULL;
        u8 *data, *p;
        int auth_flag = 0;
        
	rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
	for (i = 0; i < 2; i++) {
		if (hw_priv->if_id_slot & BIT(i)) {
			priv =
			    rk960_get_vif_from_ieee80211(
			        hw_priv->vif_list[i]);
			if (priv && priv->join_status ==
                                RK960_JOIN_STATUS_STA) {
				sta++;
			}
		}
	}
	rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);

        if (!sta)
                return;

        mutex_lock(&hw_priv->fwcr_mux);

        /* some hw_priv number */
        total += sizeof(struct wsm_caps);
        total += 6; // wifi_efuse_mac_addr
        total += 6; // bt_efuse_mac_addr
        total += 12; // vif_macs[2]
        total += 4; // key_map
        total += sizeof(struct wsm_add_key) * (WSM_KEY_MAX_INDEX + 1); // keys[WSM_KEY_MAX_INDEX + 1];
        
        for (i = 0; i < RK960_FWCR_BCN_CNT; i++) {
                u8 *bssid;
                struct ieee80211_hdr *hdr;
                
                frame = &hw_priv->fwcr_bcn[i];
                if (!frame->arg || !frame->frame)
                        continue;

                hdr = (struct ieee80211_hdr *)frame->frame;
                bssid = ieee80211_get_BSSID(hdr);
                if (bssid && ether_addr_equal(bssid, priv->join_bssid) &&
                    !(auth_flag & frame->type)) {
                        auth_flag |= frame->type;
                        total += 2 + 2 + frame->frame_len
                                + sizeof(struct wsm_rx);
                }
        }

        for (i = 0; i < RK960_FWCR_AUTH_CNT; i++) {
                frame = &hw_priv->fwcr_auth[i];
                if (!frame->arg || !frame->frame)
                        continue;
                total += 2 + 2 + frame->frame_len
                        + sizeof(struct wsm_rx);
        }

        for (i = 0; i < RK960_FWCR_GROUP_CNT; i++) {
                frame = &hw_priv->fwcr_group[i];
                if (!frame->arg || !frame->frame)
                        continue;
                total += 2 + 2 + frame->frame_len
                        + sizeof(struct wsm_rx);
        }

        if (!total) {
                mutex_unlock(&hw_priv->fwcr_mux);
                return;
        }

        data = kmalloc(total, GFP_KERNEL);
        if (!data) {
                mutex_unlock(&hw_priv->fwcr_mux);
                return;
        }

        p = data;
        memcpy(p, &hw_priv->wsm_caps, sizeof(struct wsm_caps));
        p += sizeof(struct wsm_caps);
        memcpy(p, hw_priv->wifi_efuse_mac_addr, 6);
        p += 6;
        memcpy(p, hw_priv->bt_efuse_mac_addr, 6);
        p += 6;
        memcpy(p, hw_priv->vif_macs, 12);
        p += 12;
        memcpy(p, &hw_priv->key_map, 4);
        p += 4;
        memcpy(p, hw_priv->keys,
                sizeof(struct wsm_add_key)*(WSM_KEY_MAX_INDEX + 1));
        p += sizeof(struct wsm_add_key) * (WSM_KEY_MAX_INDEX + 1);
        
        auth_flag = 0;
        for (i = 0; i < RK960_FWCR_BCN_CNT; i++) {
                u8 *bssid;
                struct ieee80211_hdr *hdr;
                
                frame = &hw_priv->fwcr_bcn[i];
                if (!frame->arg || !frame->frame)
                        continue;

                hdr = (struct ieee80211_hdr *)frame->frame;
                bssid = ieee80211_get_BSSID(hdr);
                if (bssid && ether_addr_equal(bssid, priv->join_bssid) &&
                    !(auth_flag & frame->type)) {
                        RK960_DEBUG_TXRX("%s: type %x len %d bssid %pM\n",
                                __func__, frame->type,
                                frame->frame_len, bssid);
                        auth_flag |= frame->type;
                        memcpy(p, &frame->type, 2);
                        p += 2;
                        memcpy(p, &frame->frame_len, 2);
                        p += 2;
                        memcpy(p, frame->arg, sizeof(struct wsm_rx));
                        p += sizeof(struct wsm_rx);
                        memcpy(p, frame->frame, frame->frame_len);
                        p += frame->frame_len;
                }
        }

        for (i = 0; i < RK960_FWCR_AUTH_CNT; i++) {
                frame = &hw_priv->fwcr_auth[i];
                if (!frame->arg || !frame->frame)
                        continue;
                RK960_DEBUG_TXRX("%s: type %x len %d\n",
                        __func__, frame->type,
                        frame->frame_len);
                memcpy(p, &frame->type, 2);
                p += 2;
                memcpy(p, &frame->frame_len, 2);
                p += 2;
                memcpy(p, frame->arg, sizeof(struct wsm_rx));
                p += sizeof(struct wsm_rx);
                memcpy(p, frame->frame, frame->frame_len);
                p += frame->frame_len;
        }

        for (i = 0; i < RK960_FWCR_GROUP_CNT; i++) {
                frame = &hw_priv->fwcr_group[i];
                if (!frame->arg || !frame->frame)
                        continue;
                RK960_DEBUG_TXRX("%s: type %x key_id %d len %d\n",
                        __func__, frame->type, i,
                        frame->frame_len);
                memcpy(p, &frame->type, 2);
                p += 2;
                memcpy(p, &frame->frame_len, 2);
                p += 2;
                memcpy(p, frame->arg, sizeof(struct wsm_rx));
                p += sizeof(struct wsm_rx);
                memcpy(p, frame->frame, frame->frame_len);
                p += frame->frame_len;
        }

        RK960_INFO_TXRX("%s: size %d\n", __func__, total);
        rk960_access_file(
                RK960_FWCR_FRAME_SAVE_PATH, data, total, 0);
        kfree(data);

        mutex_unlock(&hw_priv->fwcr_mux);
}

void rk960_fwcr_read(struct rk960_common *hw_priv)
{
        int size;
        u8 *data, *p;
        int ret;
        int cnt = 0;
        int flag = 0;
        int key_id = 0;
        int struct_size;

        hw_priv->fwcr_recovery = 0;
        
        size = rk960_get_file_size(RK960_FWCR_FRAME_SAVE_PATH);
        if (size <= 0) {
                RK960_ERROR_TXRX("%s: get file fail\n", __func__);
                return;
        }

        data = kmalloc(size, GFP_KERNEL);
        if (!data) {
                RK960_ERROR_TXRX("%s: kmalloc %d fail\n", __func__, size);
                return;
        }

        ret = rk960_access_file(
                RK960_FWCR_FRAME_SAVE_PATH, data, size, 1);
        if (ret < 0) {
                RK960_ERROR_TXRX("%s: access file fail\n", __func__);
                goto fwcr_read_out;
        }
        
        RK960_DEBUG_TXRX("%s: size %d\n", __func__, size);
        struct_size = sizeof(struct wsm_caps) + 6 + 6 + 12 + 4 +
                sizeof(struct wsm_add_key)*(WSM_KEY_MAX_INDEX + 1);
        if (WARN_ON(size <= struct_size))
                goto fwcr_read_out;
        p = data;
        memcpy(&hw_priv->wsm_caps, p, sizeof(struct wsm_caps));
        p += sizeof(struct wsm_caps);
        memcpy(hw_priv->wifi_efuse_mac_addr, p, 6);
        p += 6;
        memcpy(hw_priv->bt_efuse_mac_addr, p, 6);
        p += 6;
        memcpy(hw_priv->vif_macs, p, 12);
        p += 12;
        memcpy(&hw_priv->fwcr_key_map, p, 4);
        p += 4;
        memcpy(hw_priv->fwcr_keys, p,
                sizeof(struct wsm_add_key)*(WSM_KEY_MAX_INDEX + 1));
        p += sizeof(struct wsm_add_key) * (WSM_KEY_MAX_INDEX + 1);
        size -= struct_size;
        
        while (1) {
                u16 type;
                u16 frame_len;
                struct rk960_fwcr_frame_s *frame;
                struct ieee80211_hdr *hdr;

                if (!size)
                        break;
                
                if (WARN_ON(size <= (4 + sizeof(struct wsm_rx))))
                        break;
                type = *(u16 *)p;
                p += 2;
                frame_len = *(u16 *)p;
                p += 2;
                flag |= type;
                if (type & (RK960_FWCR_FRAME_TYPE_BEACON |
                    RK960_FWCR_FRAME_TYPE_PROBE_RESP)) {
                        frame = &hw_priv->fwcr_bcn[cnt++];
                } else if (type == RK960_FWCR_FRAME_TYPE_AUTH) {
                        frame = &hw_priv->fwcr_auth[0];
                } else if (type == RK960_FWCR_FRAME_TYPE_ASSOC) {
                        frame = &hw_priv->fwcr_auth[1];
                } else if (type == RK960_FWCR_FRAME_TYPE_1_4WAY) {
                        frame = &hw_priv->fwcr_auth[2];
                } else if (type == RK960_FWCR_FRAME_TYPE_3_4WAY) {
                        frame = &hw_priv->fwcr_auth[3];
                } else if (type == RK960_FWCR_FRAME_TYPE_1_2GRO) {
                        frame = &hw_priv->fwcr_group[key_id++];              
                } else {
                        WARN_ON(1);
                        break;
                }
                
                if (WARN_ON(size < (4 +
                        sizeof(struct wsm_rx) + frame_len)))
                        break;

                frame->arg = kmalloc(sizeof(struct wsm_rx), GFP_KERNEL);
                if (!frame->arg) {
                        RK960_ERROR_TXRX("%s: malloc arg failed\n",
                                __func__);
                        goto fwcr_read_out;
                }
                memcpy(frame->arg, p, sizeof(struct wsm_rx));
                p += sizeof(struct wsm_rx);

                frame->frame = kmalloc(frame_len, GFP_KERNEL);
                if (!frame->frame) {
                        RK960_ERROR_TXRX("%s: malloc frame failed\n",
                                __func__);
                        goto fwcr_read_out;
                }
                memcpy(frame->frame, p, frame_len);
                p += frame_len;
                frame->type = type;
                frame->frame_len = frame_len;
                size -= 4 + sizeof(struct wsm_rx) + frame_len;

                hdr = (struct ieee80211_hdr *)frame->frame;
                if (type & (RK960_FWCR_FRAME_TYPE_BEACON |
                    RK960_FWCR_FRAME_TYPE_PROBE_RESP))
                        RK960_DEBUG_TXRX("%s: type %x cnt %d len %d"
                                " bssid %pM\n",
                                __func__, type, cnt, frame_len,
                                ieee80211_get_BSSID(hdr));
                else if (type == RK960_FWCR_FRAME_TYPE_1_2GRO)
                        RK960_DEBUG_TXRX("%s: type %x key_id %d len %d"
                                " bssid %pM\n",
                                __func__, type, key_id, frame_len,
                                ieee80211_get_BSSID(hdr));
                else
                        RK960_DEBUG_TXRX("%s: type %x len %d"
                                " bssid %pM\n",
                                __func__, type, frame_len,
                                ieee80211_get_BSSID(hdr));
        }

        if (WARN_ON(size))
                goto fwcr_read_out;

        if (WARN_ON(!(flag & RK960_FWCR_FRAME_TYPE_AUTH)))
                goto fwcr_read_out;

        if (WARN_ON(!(flag & RK960_FWCR_FRAME_TYPE_ASSOC)))
                goto fwcr_read_out;

        if (WARN_ON(!(flag & RK960_FWCR_FRAME_TYPE_1_4WAY)))
                goto fwcr_read_out;

        if (WARN_ON(!(flag & RK960_FWCR_FRAME_TYPE_3_4WAY)))
                goto fwcr_read_out;

        hw_priv->fwcr_bcn_cnt = cnt;
        hw_priv->fwcr_recovery = 1;
        RK960_INFO_TXRX("%s: success\n", __func__);
        
fwcr_read_out:
        kfree(data);
}

int rk960_fwcr_recovery_auth(struct rk960_common *hw_priv,
        struct sk_buff *skb, struct rk960_vif *priv)
{
        struct rk960_fwcr_frame_s *frame = NULL;
        struct sk_buff *rx_skb;
        int type;
        int ret = 0;
        struct ieee80211_hdr *hdr =
                (struct ieee80211_hdr *)skb->data;
        int key_id = 0;
        
        if (!hw_priv->fwcr_recovery)
                return -1;

        mutex_lock(&hw_priv->fwcr_mux);
        
        if (ieee80211_is_auth(hdr->frame_control)) {
                frame = &hw_priv->fwcr_auth[0]; // auth
                memcpy(hw_priv->fwcr_bssid,
                        ieee80211_get_BSSID(hdr), 6);
                wsm_lock_tx_async(hw_priv);
                if (queue_work(hw_priv->workqueue,
                                &priv->join_work) <= 0)
                        wsm_unlock_tx(hw_priv);                
        } else if (ieee80211_is_assoc_req(hdr->frame_control)) {
                frame = &hw_priv->fwcr_auth[1]; // assoc resp
        } else {
                type = rk960_pae_type(skb, 1);
                if (type == RK960_FWCR_FRAME_TYPE_2_4WAY) {
                        frame = &hw_priv->fwcr_auth[3]; // 3/4 4-way
                } else if (type == RK960_FWCR_FRAME_TYPE_4_4WAY) {
                        hw_priv->fwcr_4way_set = 1;
                        hw_priv->fwcr_recovery = 0;
                        hw_priv->fw_hotboot = 0;
                        
                        schedule_work(&hw_priv->fwcr_work);

                        RK960_INFO_TXRX("%s: 4-way done\n", __func__);
                        ret = 0;
                        goto fwcr_auth_out;
                }
        }

fwcr_auth_send:
        if (!frame || !frame->arg || !frame->frame) {
                ret = -1;
                goto fwcr_auth_out;
        }
                
        rx_skb = dev_alloc_skb(frame->frame_len);
        if (!rx_skb) {
                RK960_ERROR_TXRX("%s: alloc skb fail\n", __func__);
                ret = -1;
                goto fwcr_auth_out;
        }

        if (frame->type == RK960_FWCR_FRAME_TYPE_1_2GRO)
                RK960_DEBUG_TXRX("%s: type %x key_id %d len %d\n",
                        __func__, frame->type, key_id + 1,
                        frame->frame_len);
        else
                RK960_DEBUG_TXRX("%s: type %x len %d\n",
                        __func__, frame->type, frame->frame_len);
        memcpy(skb_put(rx_skb, frame->frame_len),
                frame->frame, frame->frame_len);

        mdelay(1);
        rk960_rx_cb(priv, frame->arg, &rx_skb);

        if (frame->type == RK960_FWCR_FRAME_TYPE_ASSOC) {
                frame = &hw_priv->fwcr_auth[2]; // 1/4 4-way
                goto fwcr_auth_send;
        }/* else if (frame->type == RK960_FWCR_FRAME_TYPE_3_4WAY ||
                   frame->type == RK960_FWCR_FRAME_TYPE_1_2GRO) {
                frame = &hw_priv->fwcr_group[key_id++];
                goto fwcr_auth_send;
        }*/

fwcr_auth_out:
        mutex_unlock(&hw_priv->fwcr_mux);
        return ret;    
}

int rk960_fwcr_recovery_scan(struct rk960_common *hw_priv,
        struct rk960_vif *priv)
{
        struct rk960_fwcr_frame_s *frame;
        int i;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
	struct cfg80211_scan_info info = {
		.aborted = false,
	};
#endif        
        
        if (!hw_priv->fwcr_recovery)
                return -1;

        if (!hw_priv->fwcr_bcn_cnt)
                return -1;

        mutex_lock(&hw_priv->fwcr_mux);

        for (i = 0; i < hw_priv->fwcr_bcn_cnt; i++) {
                struct sk_buff *skb;
                
                frame = &hw_priv->fwcr_bcn[i];
                if (!frame->arg || !frame->frame) {
                        mutex_unlock(&hw_priv->fwcr_mux);
                        return -1;
                }
                
                skb = dev_alloc_skb(frame->frame_len);
                if (!skb) {
                        RK960_ERROR_TXRX("%s: alloc skb fail\n", __func__);
                        mutex_unlock(&hw_priv->fwcr_mux);
                        return -1;
                }
                
                memcpy(skb_put(skb, frame->frame_len),
                        frame->frame, frame->frame_len);

                mdelay(1);
                rk960_rx_cb(priv, frame->arg, &skb);
        }
        //hw_priv->fwcr_bcn_cnt = 0;

        mutex_unlock(&hw_priv->fwcr_mux);

        RK960_DEBUG_SCAN("scan_completed\n");
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
        ieee80211_scan_completed(hw_priv->hw, &info);
#else
        ieee80211_scan_completed(hw_priv->hw, 0);
#endif
        return 0;
}

void rk960_fwcr_work(struct work_struct *work)
{
	struct rk960_common *hw_priv =
	    container_of(work, struct rk960_common, fwcr_work);
        struct rk960_vif *priv = NULL;
        
        priv = rk960_get_vif_from_ieee80211(hw_priv->vif_list[0]);
        
        rk960_wow_resume_fwcr(priv);
}


void rk960_fwcr_wait_resumed(struct rk960_common *hw_priv)
{
	if (wait_event_interruptible_timeout(
                        hw_priv->fwcr_resume_done,
			hw_priv->fwcr_fw_resumed == 1,
                        2 * HZ) <= 0) {
		RK960_ERROR_TXRX("Timeout waiting on %s\n",
                        __func__);
	}
}

#endif

void rk960_rx_cb(struct rk960_vif *priv,
		 struct wsm_rx *arg, struct sk_buff **skb_p)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);
	struct sk_buff *skb = *skb_p;
	struct ieee80211_rx_status *hdr = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_hdr *frame = (struct ieee80211_hdr *)skb->data;
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)skb->data;
	struct rk960_link_entry *entry = NULL;
	unsigned long grace_period;
	bool early_data = false;
	size_t hdrlen = 0;

	hdr->flag = 0;

	dump_ieee80211_hdr_info((unsigned char *)skb->data, skb->len, 0,
				(s8) arg->rcpiRssi);

#ifdef RK960_CSYNC_ADJUST
	rk960_csync_rx(hw_priv, (struct ieee80211_hdr *)skb->data,
		       (s8) arg->rcpiRssi);
#endif

	if (unlikely(priv->mode == NL80211_IFTYPE_UNSPECIFIED)) {
		/* STA is stopped. */
		goto drop;
	}

	if (unlikely(ieee80211_is_probe_resp(frame->frame_control) ||
		     ieee80211_is_beacon(frame->frame_control)))
		rk960_check_ssid_ie(skb);

	if ((ieee80211_is_action(frame->frame_control))
	    && (mgmt->u.action.category == WLAN_CATEGORY_PUBLIC)) {
		u8 *action = (u8 *) & mgmt->u.action.category;
		rk960_check_go_neg_conf_success(hw_priv, action);
	}

	/* because beacon and probe_resp timestamp is error. */
	rk960_add_scan_resp_timestamp(frame);

#ifdef CONFIG_RK960_TESTMODE
	spin_lock_bh(&hw_priv->tsm_lock);
	if (hw_priv->start_stop_tsm.start) {
		unsigned queue_id = skb_get_queue_mapping(skb);
		if (queue_id == 0) {
			struct timeval tmval;
			do_gettimeofday(&tmval);
			if (hw_priv->tsm_info.sta_roamed &&
			    hw_priv->tsm_info.use_rx_roaming) {
				hw_priv->tsm_info.roam_delay = tmval.tv_usec -
				    hw_priv->tsm_info.rx_timestamp_vo;
				RK960_DEBUG_TXRX("[RX] RxInd Roaming:"
						 "roam_delay = %u\n",
						 hw_priv->tsm_info.roam_delay);
				hw_priv->tsm_info.sta_roamed = 0;
			}
			hw_priv->tsm_info.rx_timestamp_vo = tmval.tv_usec;
		}
	}
	spin_unlock_bh(&hw_priv->tsm_lock);
#endif /*CONFIG_RK960_TESTMODE */
	if (arg->link_id && (arg->link_id != RK960_LINK_ID_UNMAPPED)
	    && (arg->link_id <= RK960_MAX_STA_IN_AP_MODE)) {
		entry = &priv->link_id_db[arg->link_id - 1];
		if (entry->status == RK960_LINK_SOFT &&
		    ieee80211_is_data(frame->frame_control))
			early_data = true;
		entry->timestamp = jiffies;
	}
#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
	else if ((arg->link_id == RK960_LINK_ID_UNMAPPED)
		 && (priv->vif->p2p == WSM_START_MODE_P2P_GO)
		 && ieee80211_is_action(frame->frame_control)
		 && (mgmt->u.action.category == WLAN_CATEGORY_PUBLIC)) {
		RK960_DEBUG_TXRX("[RX] Going to MAP&RESET link ID\n");

		if (work_pending(&priv->linkid_reset_work))
			WARN_ON(1);

		memcpy(&priv->action_frame_sa[0],
		       ieee80211_get_SA(frame), ETH_ALEN);
		priv->action_linkid = 0;
		schedule_work(&priv->linkid_reset_work);
	}

	if (arg->link_id && (arg->link_id != RK960_LINK_ID_UNMAPPED)
	    && (priv->vif->p2p == WSM_START_MODE_P2P_GO)
	    && ieee80211_is_action(frame->frame_control)
	    && (mgmt->u.action.category == WLAN_CATEGORY_PUBLIC)) {
		/* Link ID already exists for the ACTION frame.
		 * Reset and Remap */
		if (work_pending(&priv->linkid_reset_work))
			WARN_ON(1);
		memcpy(&priv->action_frame_sa[0],
		       ieee80211_get_SA(frame), ETH_ALEN);
		priv->action_linkid = arg->link_id;
		schedule_work(&priv->linkid_reset_work);
	}
#endif
	if (unlikely(arg->status)) {
		if (arg->status == WSM_STATUS_MICFAILURE) {
			RK960_INFO_TXRX("[RX] MIC failure.\n");
			hdr->flag |= RX_FLAG_MMIC_ERROR;
		} else if (arg->status == WSM_STATUS_NO_KEY_FOUND) {
			RK960_INFO_TXRX("[RX] No key found.\n");
			goto drop;
		} else {
			RK960_INFO_TXRX("[RX] Receive failure: %d.\n",
					arg->status);
			goto drop;
		}
	}

	if (skb->len < sizeof(struct ieee80211_pspoll)) {
		RK960_INFO_TXRX("Mailformed SDU rx'ed. "
				"Size is lesser than IEEE header.\n");
		goto drop;
	}

	if (unlikely(ieee80211_is_pspoll(frame->frame_control)))
		if (rk960_handle_pspoll(priv, skb))
			goto drop;

	hdr->mactime = 0;	/* Not supported by WSM */
	hdr->band = (arg->channelNumber > 14) ?
	    IEEE80211_BAND_5GHZ : IEEE80211_BAND_2GHZ;
	hdr->freq = ieee80211_channel_to_frequency(arg->channelNumber,
						   hdr->band);

	if (arg->rxedRate >= 14) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
		hdr->encoding = RX_ENC_HT;
#else
		hdr->flag |= RX_FLAG_HT;
#endif
		hdr->rate_idx = arg->rxedRate - 14;
	} else if (arg->rxedRate >= 4) {
		if (hdr->band == IEEE80211_BAND_5GHZ)
			hdr->rate_idx = arg->rxedRate - 6;
		else
			hdr->rate_idx = arg->rxedRate - 2;
	} else {
		hdr->rate_idx = arg->rxedRate;
	}

	hdr->signal = (s8) arg->rcpiRssi;
	hdr->antenna = 0;

	hdrlen = ieee80211_hdrlen(frame->frame_control);

	hw_priv->last_rx_is_nonprotect_pae = false;
	if (WSM_RX_STATUS_ENCRYPTION(arg->flags)) {
		size_t iv_len = 0, icv_len = 0;

		hdr->flag |= RX_FLAG_DECRYPTED;

		/* Oops... There is no fast way to ask mac80211 about
		 * IV/ICV lengths. Even defineas are not exposed.*/
		switch (WSM_RX_STATUS_ENCRYPTION(arg->flags)) {
		case WSM_RX_STATUS_WEP:
			iv_len = 4 /* WEP_IV_LEN */ ;
			icv_len = 4 /* WEP_ICV_LEN */ ;
			break;
		case WSM_RX_STATUS_TKIP:
			iv_len = 8 /* TKIP_IV_LEN */ ;
			icv_len = 4	/* TKIP_ICV_LEN */
			    + 8 /*MICHAEL_MIC_LEN */ ;
			hdr->flag |= RX_FLAG_MMIC_STRIPPED;
			break;
		case WSM_RX_STATUS_AES:
			iv_len = 8 /* CCMP_HDR_LEN */ ;
			icv_len = 8 /* CCMP_MIC_LEN */ ;
			break;
		case WSM_RX_STATUS_WAPI:
			iv_len = 18 /* WAPI_HDR_LEN */ ;
			icv_len = 16 /* WAPI_MIC_LEN */ ;
			hdr->flag |= RX_FLAG_IV_STRIPPED;
			break;
		default:
			WARN_ON("Unknown encryption type");
			goto drop;
		}

		/* Firmware strips ICV in case of MIC failure. */
		if (arg->status == WSM_STATUS_MICFAILURE) {
			icv_len = 0;
			hdr->flag |= RX_FLAG_IV_STRIPPED;
		}

		if (skb->len < hdrlen + iv_len + icv_len) {
			RK960_INFO_TXRX("Mailformed SDU rx'ed. "
					"Size is lesser than crypto headers.\n");
			goto drop;
		}

		/* Protocols not defined in mac80211 should be
		   stripped/crypted in driver/firmware */
		if (WSM_RX_STATUS_ENCRYPTION(arg->flags) == WSM_RX_STATUS_WAPI) {
			/* Remove IV, ICV and MIC */
			skb_trim(skb, skb->len - icv_len);
			memmove(skb->data + iv_len, skb->data, hdrlen);
			skb_pull(skb, iv_len);
		}

	} else {		// not protected
		hw_priv->last_rx_is_nonprotect_pae = rk960_rx_is_pae(skb);
	}

#ifdef SUPPORT_FWCR
        rk960_fwcr_frame_capture(hw_priv, arg, *skb_p);
#endif

	rk960_debug_rxed(priv);
	if (arg->flags & WSM_RX_STATUS_AGGREGATE)
		rk960_debug_rxed_agg(priv);

	if (ieee80211_is_beacon(frame->frame_control) &&
	    !arg->status &&
	    !memcmp(ieee80211_get_SA(frame), priv->join_bssid, ETH_ALEN)) {
		const u8 *tim_ie;
		u8 *ies;
		size_t ies_len;
		priv->disable_beacon_filter = false;
		queue_work(hw_priv->workqueue, &priv->update_filtering_work);
		ies = ((struct ieee80211_mgmt *)
		       (skb->data))->u.beacon.variable;
		ies_len = skb->len - (ies - (u8 *) (skb->data));

		tim_ie = rk960_get_ie(ies, ies_len, WLAN_EID_TIM);
		if (tim_ie) {
			struct ieee80211_tim_ie *tim =
			    (struct ieee80211_tim_ie *)&tim_ie[2];

			if (priv->join_dtim_period != tim->dtim_period) {
				priv->join_dtim_period = tim->dtim_period;
				queue_work(hw_priv->workqueue,
					   &priv->
					   set_beacon_wakeup_period_work);
			}
		}
		if (unlikely(priv->disable_beacon_filter)) {
			priv->disable_beacon_filter = false;
			queue_work(hw_priv->workqueue,
				   &priv->update_filtering_work);
		}
	}
#ifdef AP_HT_CAP_UPDATE
	if (priv->mode == NL80211_IFTYPE_AP &&
	    ieee80211_is_beacon(frame->frame_control) && !arg->status) {

		u8 *ies;
		size_t ies_len;
		const u8 *ht_cap;
		ies = ((struct ieee80211_mgmt *)
		       (skb->data))->u.beacon.variable;
		ies_len = skb->len - (ies - (u8 *) (skb->data));
		ht_cap = rk960_get_ie(ies, ies_len, WLAN_EID_HT_CAPABILITY);
		if (!ht_cap) {
			priv->ht_info |= 0x0011;
		}
		queue_work(hw_priv->workqueue, &priv->ht_info_update_work);

	}
#endif

#ifdef ROAM_OFFLOAD
	if ((ieee80211_is_beacon(frame->frame_control)
	     || ieee80211_is_probe_resp(frame->frame_control))
	    && !arg->status) {
		if (hw_priv->auto_scanning
		    && !atomic_read(&hw_priv->scan.in_progress))
			hw_priv->frame_rcvd = 1;

		if (!memcmp
		    (ieee80211_get_SA(frame), priv->join_bssid, ETH_ALEN)) {
			if (hw_priv->beacon)
				dev_kfree_skb(hw_priv->beacon);
			hw_priv->beacon = skb_copy(skb, GFP_ATOMIC);
			if (!hw_priv->beacon)
				RK960_ERROR_TXRX
				    ("rk960: sched_scan: own beacon storing failed\n");
		}
	}
#endif /*ROAM_OFFLOAD */

	/* Stay awake for 1sec. after frame is received to give
	 * userspace chance to react and acquire appropriate
	 * wakelock. */
	if (ieee80211_is_auth(frame->frame_control))
		grace_period = 5 * HZ;
	else if (ieee80211_is_deauth(frame->frame_control))
		grace_period = 5 * HZ;
	else
		grace_period = 0;	//1 * HZ;

	if (ieee80211_is_data(frame->frame_control))
		rk960_rx_h_ba_stat(priv, hdrlen, skb->len);

	if (grace_period)
		rk960_pm_stay_awake(&hw_priv->pm_state, grace_period);

#ifdef CONFIG_RK960_TESTMODE
	if (hw_priv->test_frame.len > 0 && priv->mode == NL80211_IFTYPE_STATION) {
		if (rk960_frame_test_detection(priv, frame, skb) == 1) {
			consume_skb(skb);
			*skb_p = NULL;
			return;
		}
	}
#endif /* CONFIG_RK960_TESTMODE */

	if (unlikely(rk960_itp_rxed(hw_priv, skb)))
		consume_skb(skb);
	else if (unlikely(early_data)) {
		RK960_DEBUG_TXRX("LDB: RX: link_id %d status %d %pM\n",
				 arg->link_id, entry->status, entry->mac);
		spin_lock_bh(&priv->ps_state_lock);
		/* Double-check status with lock held */
		if (entry->status == RK960_LINK_SOFT)
			skb_queue_tail(&entry->rx_queue, skb);
		else
			ieee80211_rx_irqsafe(priv->hw, skb);
		spin_unlock_bh(&priv->ps_state_lock);
	} else {
		ieee80211_rx_irqsafe(priv->hw, skb);
	}
	*skb_p = NULL;

	return;

drop:
	/* TODO: update failure counters */
	return;
}

void rk960_rx_cb_monitor(struct rk960_common *priv,
			 struct wsm_rx *arg, struct sk_buff **skb_p)
{
	struct sk_buff *skb = *skb_p;
	struct ieee80211_rx_status *hdr = IEEE80211_SKB_RXCB(skb);

	hdr->flag = 0;

	if (unlikely(arg->status)) {
		if (arg->status == WSM_STATUS_MICFAILURE) {
			hdr->flag |= RX_FLAG_MMIC_ERROR;
		} else if (arg->status == WSM_STATUS_NO_KEY_FOUND) {
			RK960_INFO_TXRX("[RX] No key found.\n");
		} else {
			RK960_INFO_TXRX("[RX] Receive failure: %d.\n",
					arg->status);
		}
	}

	hdr->mactime = 0;	/* Not supported by WSM */
	hdr->band = (arg->channelNumber > 14) ?
	    IEEE80211_BAND_5GHZ : IEEE80211_BAND_2GHZ;
	hdr->freq = ieee80211_channel_to_frequency(arg->channelNumber,
						   hdr->band);

	if (arg->rxedRate >= 14) {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
		hdr->encoding = RX_ENC_HT;
#else
		hdr->flag |= RX_FLAG_HT;
#endif
		hdr->rate_idx = arg->rxedRate - 14;
	} else if (arg->rxedRate >= 4) {
		if (hdr->band == IEEE80211_BAND_5GHZ)
			hdr->rate_idx = arg->rxedRate - 6;
		else
			hdr->rate_idx = arg->rxedRate - 2;
	} else {
		hdr->rate_idx = arg->rxedRate;
	}

	hdr->signal = (s8) arg->rcpiRssi;
	hdr->antenna = 0;

	if (WSM_RX_STATUS_ENCRYPTION(arg->flags)) {
		size_t iv_len = 0, icv_len = 0;

		hdr->flag |= RX_FLAG_DECRYPTED;

		/* Oops... There is no fast way to ask mac80211 about
		 * IV/ICV lengths. Even defineas are not exposed.*/
		switch (WSM_RX_STATUS_ENCRYPTION(arg->flags)) {
		case WSM_RX_STATUS_WEP:
			iv_len = 4 /* WEP_IV_LEN */ ;
			icv_len = 4 /* WEP_ICV_LEN */ ;
			break;
		case WSM_RX_STATUS_TKIP:
			iv_len = 8 /* TKIP_IV_LEN */ ;
			icv_len = 4	/* TKIP_ICV_LEN */
			    + 8 /*MICHAEL_MIC_LEN */ ;
			hdr->flag |= RX_FLAG_MMIC_STRIPPED;
			break;
		case WSM_RX_STATUS_AES:
			iv_len = 8 /* CCMP_HDR_LEN */ ;
			icv_len = 8 /* CCMP_MIC_LEN */ ;
			break;
		case WSM_RX_STATUS_WAPI:
			iv_len = 18 /* WAPI_HDR_LEN */ ;
			icv_len = 16 /* WAPI_MIC_LEN */ ;
			hdr->flag |= RX_FLAG_IV_STRIPPED;
			break;
		}

		/* Firmware strips ICV in case of MIC failure. */
		if (arg->status == WSM_STATUS_MICFAILURE) {
			icv_len = 0;
			hdr->flag |= RX_FLAG_IV_STRIPPED;
		}
	}

	ieee80211_rx_irqsafe(priv->hw, skb);
	*skb_p = NULL;
}

/* ******************************************************************** */
/* Security								*/

int rk960_alloc_key(struct rk960_common *hw_priv)
{
	int idx;

	idx = ffs(~hw_priv->key_map) - 1;
	if (idx < 0 || idx > WSM_KEY_MAX_INDEX)
		return -1;

	hw_priv->key_map |= BIT(idx);
	hw_priv->keys[idx].entryIndex = idx;
	return idx;
}

void rk960_free_key(struct rk960_common *hw_priv, int idx)
{
	BUG_ON(!(hw_priv->key_map & BIT(idx)));
	memset(&hw_priv->keys[idx], 0, sizeof(hw_priv->keys[idx]));
	hw_priv->key_map &= ~BIT(idx);
}

void rk960_free_keys(struct rk960_common *hw_priv)
{
	memset(&hw_priv->keys, 0, sizeof(hw_priv->keys));
	hw_priv->key_map = 0;
}

int rk960_upload_keys(struct rk960_vif *priv)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);
	int idx, ret = 0;

	for (idx = 0; idx <= WSM_KEY_MAX_IDX; ++idx)
		if (hw_priv->key_map & BIT(idx)) {
			ret =
			    wsm_add_key(hw_priv, &hw_priv->keys[idx],
					priv->if_id);
			if (ret < 0)
				break;
		}
	return ret;
}

#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
/* Workaround for WFD test case 6.1.10 */
void rk960_link_id_reset(struct work_struct *work)
{
	struct rk960_vif *priv =
	    container_of(work, struct rk960_vif, linkid_reset_work);
	struct rk960_common *hw_priv = priv->hw_priv;
	int temp_linkid;

	if (!priv->action_linkid) {
		/* In GO mode we can receive ACTION frames without a linkID */
		temp_linkid = rk960_alloc_link_id(priv,
						  &priv->action_frame_sa[0]);
		WARN_ON(!temp_linkid);
		if (temp_linkid) {
			/* Make sure we execute the WQ */
			flush_workqueue(hw_priv->workqueue);
			/* Release the link ID */
			spin_lock_bh(&priv->ps_state_lock);
			priv->link_id_db[temp_linkid - 1].prev_status =
			    priv->link_id_db[temp_linkid - 1].status;
			priv->link_id_db[temp_linkid - 1].status =
			    RK960_LINK_RESET;
			spin_unlock_bh(&priv->ps_state_lock);
			wsm_lock_tx_async(hw_priv);
			if (queue_work(hw_priv->workqueue,
				       &priv->link_id_work) <= 0)
				wsm_unlock_tx(hw_priv);
		}
	} else {
		spin_lock_bh(&priv->ps_state_lock);
		priv->link_id_db[priv->action_linkid - 1].prev_status =
		    priv->link_id_db[priv->action_linkid - 1].status;
		priv->link_id_db[priv->action_linkid - 1].status =
		    RK960_LINK_RESET_REMAP;
		spin_unlock_bh(&priv->ps_state_lock);
		wsm_lock_tx_async(hw_priv);
		if (queue_work(hw_priv->workqueue, &priv->link_id_work) <= 0)
			wsm_unlock_tx(hw_priv);
		flush_workqueue(hw_priv->workqueue);
	}
}
#endif
