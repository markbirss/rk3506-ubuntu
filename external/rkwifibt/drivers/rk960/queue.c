/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <net/mac80211.h>
#include <linux/sched.h>
#include "rk960.h"
#include "queue.h"
#include "debug.h"
#ifdef CONFIG_RK960_TESTMODE
#include <linux/time.h>
#endif /*CONFIG_RK960_TESTMODE */

/* private */ struct rk960_queue_item {
	struct list_head head;
	struct sk_buff *skb;
	u32 packetID;
	unsigned long queue_timestamp;
	unsigned long xmit_timestamp;
	unsigned long xmited_timestamp;
	unsigned long conf_timestamp;
#ifdef CONFIG_RK960_TESTMODE
	unsigned long mdelay_timestamp;
	unsigned long qdelay_timestamp;
#endif				/*CONFIG_RK960_TESTMODE */
	struct rk960_txpriv txpriv;
	u8 generation;
};

static inline void __rk960_queue_lock(struct rk960_queue *queue)
{
	struct rk960_queue_stats *stats = queue->stats;
	if (queue->tx_locked_cnt++ == 0) {
		RK960_DEBUG_TXRX("[TX] Queue %d is locked.\n", queue->queue_id);
		ieee80211_stop_queue(stats->hw_priv->hw, queue->queue_id);
	}
}

static inline void __rk960_queue_unlock(struct rk960_queue *queue)
{
	struct rk960_queue_stats *stats = queue->stats;
	BUG_ON(!queue->tx_locked_cnt);
	if (--queue->tx_locked_cnt == 0) {
		RK960_DEBUG_TXRX("[TX] Queue %d is unlocked.\n",
				 queue->queue_id);
		ieee80211_wake_queue(stats->hw_priv->hw, queue->queue_id);
	}
}

static inline void rk960_queue_parse_id(u32 packetID, u8 * queue_generation,
					u8 * queue_id,
					u8 * item_generation,
					u8 * item_id, u8 * if_id, u8 * link_id)
{
	*item_id = (packetID >> 0) & 0xFF;
	*item_generation = (packetID >> 8) & 0xFF;
	*queue_id = (packetID >> 16) & 0xF;
	*if_id = (packetID >> 20) & 0xF;
	*link_id = (packetID >> 24) & 0xF;
	*queue_generation = (packetID >> 28) & 0xF;
}

static inline u32 rk960_queue_make_packet_id(u8 queue_generation, u8 queue_id,
					     u8 item_generation, u8 item_id,
					     u8 if_id, u8 link_id)
{
	/*TODO:COMBO: Add interfaceID to the packetID */
	return ((u32) item_id << 0) |
	    ((u32) item_generation << 8) |
	    ((u32) queue_id << 16) |
	    ((u32) if_id << 20) |
	    ((u32) link_id << 24) | ((u32) queue_generation << 28);
}

static void rk960_queue_post_gc(struct rk960_queue_stats *stats,
				struct list_head *gc_list)
{
	struct rk960_queue_item *item;

	while (!list_empty(gc_list)) {
		item = list_first_entry(gc_list, struct rk960_queue_item, head);
		list_del(&item->head);
		stats->skb_dtor(stats->hw_priv, item->skb, &item->txpriv);
		kfree(item);
	}
}

static void rk960_queue_register_post_gc(struct list_head *gc_list,
					 struct rk960_queue_item *item)
{
	struct rk960_queue_item *gc_item;
	gc_item = kmalloc(sizeof(struct rk960_queue_item), GFP_ATOMIC);
	BUG_ON(!gc_item);
	memcpy(gc_item, item, sizeof(struct rk960_queue_item));
	list_add_tail(&gc_item->head, gc_list);
}

static void __rk960_queue_gc(struct rk960_queue *queue,
			     struct list_head *head, bool unlock)
{
	struct rk960_queue_stats *stats = queue->stats;
	struct rk960_queue_item *item = NULL;
	struct rk960_vif *priv;
	int if_id;
	bool wakeup_stats = false;

	while (!list_empty(&queue->queue)) {
		struct rk960_txpriv *txpriv;
		item =
		    list_first_entry(&queue->queue, struct rk960_queue_item,
				     head);
		if (jiffies - item->queue_timestamp < queue->ttl)
			break;

		txpriv = &item->txpriv;
		if_id = txpriv->if_id;
		--queue->num_queued;
		--queue->num_queued_vif[if_id];
		--queue->link_map_cache[if_id][txpriv->link_id];
		spin_lock_bh(&stats->lock);
		--stats->num_queued[if_id];
		if (!--stats->link_map_cache[if_id][txpriv->link_id])
			wakeup_stats = true;
		spin_unlock_bh(&stats->lock);
		priv = rk960_hwpriv_to_vifpriv(stats->hw_priv, if_id);
		if (priv) {
			rk960_debug_tx_ttl(priv);
			rk960_priv_vif_list_read_unlock(&priv->vif_lock);
		}
		rk960_queue_register_post_gc(head, item);
		item->skb = NULL;
		list_move_tail(&item->head, &queue->free_pool);
	}

	if (wakeup_stats)
		wake_up(&stats->wait_link_id_empty);

	if (queue->overfull) {
		if (queue->num_queued <= ((stats->hw_priv->vif0_throttle +
					   stats->hw_priv->vif1_throttle +
					   2) / 2)) {
			queue->overfull = false;
			if (unlock)
				__rk960_queue_unlock(queue);
		} else if (item) {
			unsigned long tmo = item->queue_timestamp + queue->ttl;
			mod_timer(&queue->gc, tmo);
			rk960_pm_stay_awake(&stats->hw_priv->pm_state,
					    tmo - jiffies);
		}
	}
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
static void rk960_queue_gc(struct timer_list *t)
{
	struct rk960_queue *queue = from_timer(queue, t, gc);
#else
static void rk960_queue_gc(unsigned long arg)
{
	struct rk960_queue *queue = (struct rk960_queue *)arg;
#endif
	LIST_HEAD(list);
	spin_lock_bh(&queue->lock);
	__rk960_queue_gc(queue, &list, true);
	spin_unlock_bh(&queue->lock);
	rk960_queue_post_gc(queue->stats, &list);
}

int rk960_queue_stats_init(struct rk960_queue_stats *stats,
			   size_t map_capacity,
			   rk960_queue_skb_dtor_t skb_dtor,
			   struct rk960_common *hw_priv)
{
	int i;

	memset(stats, 0, sizeof(*stats));
	stats->map_capacity = map_capacity;
	stats->skb_dtor = skb_dtor;
	stats->hw_priv = hw_priv;
	spin_lock_init(&stats->lock);
	init_waitqueue_head(&stats->wait_link_id_empty);
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		stats->link_map_cache[i] = kzalloc(sizeof(int) * map_capacity,
						   GFP_KERNEL);
		if (!stats->link_map_cache[i]) {
			for (; i >= 0; i--)
				kfree(stats->link_map_cache[i]);
			return -ENOMEM;
		}
	}

	return 0;
}

int rk960_queue_init(struct rk960_queue *queue,
		     struct rk960_queue_stats *stats,
		     u8 queue_id, size_t capacity, unsigned long ttl)
{
	int i;

	memset(queue, 0, sizeof(*queue));
	queue->stats = stats;
	queue->capacity = capacity;
	queue->queue_id = queue_id;
	queue->ttl = ttl;
	INIT_LIST_HEAD(&queue->queue);
	INIT_LIST_HEAD(&queue->pending);
	INIT_LIST_HEAD(&queue->free_pool);
	spin_lock_init(&queue->lock);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
	timer_setup(&queue->gc, rk960_queue_gc, 0);
#else
	init_timer(&queue->gc);
	queue->gc.data = (unsigned long)queue;
	queue->gc.function = rk960_queue_gc;
#endif

	queue->pool = kzalloc(sizeof(struct rk960_queue_item) * capacity,
			      GFP_KERNEL);
	if (!queue->pool)
		return -ENOMEM;

	for (i = 0; i < RK960_MAX_VIFS; i++) {
		queue->link_map_cache[i] =
		    kzalloc(sizeof(int) * stats->map_capacity, GFP_KERNEL);
		if (!queue->link_map_cache[i]) {
			for (; i >= 0; i--)
				kfree(queue->link_map_cache[i]);
			kfree(queue->pool);
			queue->pool = NULL;
			return -ENOMEM;
		}
	}

	for (i = 0; i < capacity; ++i)
		list_add_tail(&queue->pool[i].head, &queue->free_pool);

	return 0;
}

/* TODO:COMBO: Flush only a particular interface specific parts */
int rk960_queue_clear(struct rk960_queue *queue, int if_id)
{
	int i, cnt, iter;
	struct rk960_queue_stats *stats = queue->stats;
	struct rk960_queue_item *item = NULL;
	struct rk960_queue_item *tmp = NULL;
	LIST_HEAD(gc_list);

	cnt = 0;
	spin_lock_bh(&queue->lock);
	queue->generation++;
	queue->generation &= 0xf;
#if 0
	list_splice_tail_init(&queue->queue, &queue->pending);
	while (!list_empty(&queue->pending)) {
		struct rk960_queue_item *item =
		    list_first_entry(&queue->pending, struct rk960_queue_item,
				     head);
		WARN_ON(!item->skb);
		if (RK960_ALL_IFS == if_id || item->txpriv.if_id == if_id) {
			rk960_queue_register_post_gc(&gc_list, item);
			item->skb = NULL;
			list_move_tail(&item->head, &queue->free_pool);
			cnt++;
		}
	}
	queue->num_queued -= cnt;
	queue->num_pending -= cnt;
#else
	list_for_each_entry_safe(item, tmp, &queue->pending, head) {
		if (RK960_ALL_IFS == if_id || item->txpriv.if_id == if_id) {
			rk960_queue_register_post_gc(&gc_list, item);
			item->skb = NULL;
			list_move_tail(&item->head, &queue->free_pool);
			cnt++;
		}
	}
	BUG_ON(cnt > queue->num_pending);
	queue->num_pending -= cnt;
	cnt = 0;
	list_for_each_entry_safe(item, tmp, &queue->queue, head) {
		if (RK960_ALL_IFS == if_id || item->txpriv.if_id == if_id) {
			rk960_queue_register_post_gc(&gc_list, item);
			item->skb = NULL;
			list_move_tail(&item->head, &queue->free_pool);
			cnt++;
		}
	}
	BUG_ON(cnt > queue->num_queued);
	queue->num_queued -= cnt;
#endif
	if (RK960_ALL_IFS != if_id) {
		queue->num_queued_vif[if_id] = 0;
		queue->num_pending_vif[if_id] = 0;
	} else {
		for (iter = 0; iter < RK960_MAX_VIFS; iter++) {
			queue->num_queued_vif[iter] = 0;
			queue->num_pending_vif[iter] = 0;
		}
	}
	spin_lock_bh(&stats->lock);
	if (RK960_ALL_IFS != if_id) {
		for (i = 0; i < stats->map_capacity; ++i) {
			stats->num_queued[if_id] -=
			    queue->link_map_cache[if_id][i];
			stats->link_map_cache[if_id][i] -=
			    queue->link_map_cache[if_id][i];
			queue->link_map_cache[if_id][i] = 0;
		}
	} else {
		for (iter = 0; iter < RK960_MAX_VIFS; iter++) {
			for (i = 0; i < stats->map_capacity; ++i) {
				stats->num_queued[iter] -=
				    queue->link_map_cache[iter][i];
				stats->link_map_cache[iter][i] -=
				    queue->link_map_cache[iter][i];
				queue->link_map_cache[iter][i] = 0;
			}
		}
	}
	spin_unlock_bh(&stats->lock);
	if (unlikely(queue->overfull)) {
		queue->overfull = false;
		__rk960_queue_unlock(queue);
	}
	spin_unlock_bh(&queue->lock);
	wake_up(&stats->wait_link_id_empty);
	rk960_queue_post_gc(stats, &gc_list);
	return 0;
}

void rk960_queue_stats_deinit(struct rk960_queue_stats *stats)
{
	int i;

	for (i = 0; i < RK960_MAX_VIFS; i++) {
		kfree(stats->link_map_cache[i]);
		stats->link_map_cache[i] = NULL;
	}
}

void rk960_queue_deinit(struct rk960_queue *queue)
{
	int i;

	rk960_queue_clear(queue, RK960_ALL_IFS);
	del_timer_sync(&queue->gc);
	INIT_LIST_HEAD(&queue->free_pool);
	kfree(queue->pool);
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		kfree(queue->link_map_cache[i]);
		queue->link_map_cache[i] = NULL;
	}
	queue->pool = NULL;
	queue->capacity = 0;
}

size_t rk960_queue_get_num_queued(struct rk960_vif *priv,
				  struct rk960_queue *queue, u32 link_id_map)
{
	size_t ret;
	int i, bit;
	size_t map_capacity = queue->stats->map_capacity;

	if (!link_id_map)
		return 0;

	spin_lock_bh(&queue->lock);
	if (likely(link_id_map == (u32) - 1)) {
		ret = queue->num_queued_vif[priv->if_id] -
		    queue->num_pending_vif[priv->if_id];
	} else {
		ret = 0;
		for (i = 0, bit = 1; i < map_capacity; ++i, bit <<= 1) {
			if (link_id_map & bit)
				ret += queue->link_map_cache[priv->if_id][i];
		}
	}
	spin_unlock_bh(&queue->lock);
	return ret;
}

int rk960_queue_put(struct rk960_queue *queue,
		    struct sk_buff *skb, struct rk960_txpriv *txpriv)
{
	int ret = 0;
#ifdef CONFIG_RK960_TESTMODE
	struct timeval tmval;
#endif /*CONFIG_RK960_TESTMODE */
	LIST_HEAD(gc_list);
	struct rk960_queue_stats *stats = queue->stats;
	/* TODO:COMBO: Add interface ID info to queue item */

	if (txpriv->link_id >= queue->stats->map_capacity)
		return -EINVAL;

	spin_lock_bh(&queue->lock);
	if (!WARN_ON(list_empty(&queue->free_pool))) {
		struct rk960_queue_item *item =
		    list_first_entry(&queue->free_pool, struct rk960_queue_item,
				     head);
		BUG_ON(item->skb);

		list_move_tail(&item->head, &queue->queue);
		item->skb = skb;
		item->txpriv = *txpriv;
		item->generation = 0;
		item->packetID =
		    rk960_queue_make_packet_id(queue->generation,
					       queue->queue_id,
					       item->generation,
					       item - queue->pool,
					       txpriv->if_id,
					       txpriv->raw_link_id);
		//pr_info("gen pid = %x\n", item->packetID);
		item->queue_timestamp = jiffies;
		item->xmited_timestamp = 0;
#ifdef CONFIG_RK960_TESTMODE
		do_gettimeofday(&tmval);
		item->qdelay_timestamp = tmval.tv_usec;
#endif /*CONFIG_RK960_TESTMODE */

		++queue->num_queued;
		++queue->num_queued_vif[txpriv->if_id];
		++queue->link_map_cache[txpriv->if_id][txpriv->link_id];

		spin_lock_bh(&stats->lock);
		++stats->num_queued[txpriv->if_id];
		++stats->link_map_cache[txpriv->if_id][txpriv->link_id];
		spin_unlock_bh(&stats->lock);

		/*
		 * TX may happen in parallel sometimes.
		 * Leave extra queue slots so we don't overflow.
		 */
		if (queue->overfull == false &&
		    queue->num_queued >=
		    ((stats->hw_priv->vif0_throttle +
		      stats->hw_priv->vif1_throttle + 2)
		     - (num_present_cpus() - 1))) {
			queue->overfull = true;
			__rk960_queue_lock(queue);
			mod_timer(&queue->gc, jiffies);
		}
	} else {
		ret = -ENOENT;
	}
#if 0
	RK960_DEBUG_QUEUE("queue_put queue %d, %d, %d\n",
			  queue->num_queued,
			  queue->link_map_cache[txpriv->if_id][txpriv->link_id],
			  queue->num_pending);
	RK960_DEBUG_QUEUE("queue_put stats %d, %d\n", stats->num_queued,
			  stats->link_map_cache[txpriv->if_id][txpriv->
							       link_id]);
#endif
	spin_unlock_bh(&queue->lock);
	return ret;
}

void rk960_queue_generation_check(struct rk960_queue *queue,
				  struct rk960_queue_item *item)
{
	u8 queue_generation, queue_id, item_generation, item_id, if_id, link_id;

	rk960_queue_parse_id(item->packetID, &queue_generation, &queue_id,
			     &item_generation, &item_id, &if_id, &link_id);

	/* queue_generation may changed in rk960_queue_clear
	 * which will cause queue_generation mismatch, 
	 * like in rk960_queue_get_skb
	 * re-make_packet_id here
	 */
	if (queue_generation != queue->generation) {
		item->packetID =
		    rk960_queue_make_packet_id(queue->generation, queue_id,
					       item_generation, item_id, if_id,
					       link_id);
	}
}

int rk960_queue_get(struct rk960_queue *queue,
		    int if_id,
		    u32 link_id_map,
		    struct wsm_tx **tx,
		    struct ieee80211_tx_info **tx_info,
		    struct rk960_txpriv **txpriv,
		    unsigned long **xmited_timestamp)
{
	int ret = -ENOENT;
	struct rk960_queue_item *item;
	struct rk960_queue_stats *stats = queue->stats;
	bool wakeup_stats = false;
#ifdef CONFIG_RK960_TESTMODE
	struct timeval tmval;
#endif /*CONFIG_RK960_TESTMODE */

	*xmited_timestamp = NULL;

	spin_lock_bh(&queue->lock);
	list_for_each_entry(item, &queue->queue, head) {
		if ((item->txpriv.if_id == if_id) &&
		    (link_id_map & BIT(item->txpriv.link_id))) {
			ret = 0;
			break;
		}
	}

	if (!WARN_ON(ret)) {
		*tx = (struct wsm_tx *)item->skb->data;
		*tx_info = IEEE80211_SKB_CB(item->skb);
		*txpriv = &item->txpriv;
		rk960_queue_generation_check(queue, item);
		(*tx)->packetID = __cpu_to_le32(item->packetID);
		list_move_tail(&item->head, &queue->pending);
		++queue->num_pending;
		++queue->num_pending_vif[item->txpriv.if_id];
		--queue->link_map_cache[item->txpriv.if_id]
		    [item->txpriv.link_id];
		item->xmit_timestamp = jiffies;
		*xmited_timestamp = &item->xmited_timestamp;
#ifdef CONFIG_RK960_TESTMODE
		do_gettimeofday(&tmval);
		item->mdelay_timestamp = tmval.tv_usec;
#endif /*CONFIG_RK960_TESTMODE */

		spin_lock_bh(&stats->lock);
		--stats->num_queued[item->txpriv.if_id];
		if (!--stats->link_map_cache[item->txpriv.if_id]
		    [item->txpriv.link_id])
			wakeup_stats = true;

		spin_unlock_bh(&stats->lock);
#if 0
		RK960_DEBUG_QUEUE("queue_get queue %d, %d, %d\n",
				  queue->num_queued,
				  queue->link_map_cache[item->txpriv.
							if_id][item->txpriv.
							       link_id],
				  queue->num_pending);
		RK960_DEBUG_QUEUE("queue_get stats %d, %d\n", stats->num_queued,
				  stats->link_map_cache[item->txpriv.if_id]
				  [item->txpriv.link_id]);
#endif
	}
	spin_unlock_bh(&queue->lock);
	if (wakeup_stats)
		wake_up(&stats->wait_link_id_empty);
	return ret;
}

#ifdef CONFIG_RK960_TESTMODE
int rk960_queue_requeue(struct rk960_common *hw_priv,
			struct rk960_queue *queue, u32 packetID, bool check)
#else
int rk960_queue_requeue(struct rk960_queue *queue, u32 packetID, bool check)
#endif
{
	int ret = 0;
	u8 queue_generation, queue_id, item_generation, item_id, if_id, link_id;
	struct rk960_queue_item *item;
	struct rk960_queue_stats *stats = queue->stats;

	rk960_queue_parse_id(packetID, &queue_generation, &queue_id,
			     &item_generation, &item_id, &if_id, &link_id);

	item = &queue->pool[item_id];
#ifdef P2P_MULTIVIF
	if (check && item->txpriv.if_id == RK960_GENERIC_IF_ID) {
#else
	if (check && item->txpriv.offchannel_if_id == RK960_GENERIC_IF_ID) {
#endif
		RK960_DEBUG_QUEUE("Requeued frame dropped for "
				  "generic interface id.\n");
#ifdef CONFIG_RK960_TESTMODE
		rk960_queue_remove(hw_priv, queue, packetID);
#else
		rk960_queue_remove(queue, packetID);
#endif
		return 0;
	}
#ifndef P2P_MULTIVIF
	if (!check)
		item->txpriv.offchannel_if_id = RK960_GENERIC_IF_ID;
#endif

	/*if_id = item->txpriv.if_id; */

	spin_lock_bh(&queue->lock);
	BUG_ON(queue_id != queue->queue_id);
	if (unlikely(queue_generation != queue->generation)) {
		ret = -ENOENT;
	} else if (unlikely(item_id >= (unsigned)queue->capacity)) {
		WARN_ON(1);
		ret = -EINVAL;
	} else if (unlikely(item->generation != item_generation)) {
		WARN_ON(1);
		ret = -ENOENT;
	} else {
		--queue->num_pending;
		--queue->num_pending_vif[if_id];
		++queue->link_map_cache[if_id][item->txpriv.link_id];

		spin_lock_bh(&stats->lock);
		++stats->num_queued[item->txpriv.if_id];
		++stats->link_map_cache[if_id][item->txpriv.link_id];
		spin_unlock_bh(&stats->lock);

		item->generation = ++item_generation;
		item->packetID =
		    rk960_queue_make_packet_id(queue_generation, queue_id,
					       item_generation, item_id, if_id,
					       link_id);
		list_move(&item->head, &queue->queue);
#if 0
		RK960_DEBUG_QUEUE("queue_requeue queue %d, %d, %d\n",
				  queue->num_queued,
				  queue->link_map_cache[if_id][item->txpriv.
							       link_id],
				  queue->num_pending);
		RK960_DEBUG_QUEUE("queue_requeue stats %d, %d\n",
				  stats->num_queued,
				  stats->link_map_cache[if_id][item->txpriv.
							       link_id]);
#endif
	}
	spin_unlock_bh(&queue->lock);
	return ret;
}

int rk960_queue_requeue_all(struct rk960_queue *queue)
{
	struct rk960_queue_stats *stats = queue->stats;
	spin_lock_bh(&queue->lock);
	while (!list_empty(&queue->pending)) {
		struct rk960_queue_item *item =
		    list_entry(queue->pending.prev, struct rk960_queue_item,
			       head);

		--queue->num_pending;
		--queue->num_pending_vif[item->txpriv.if_id];
		++queue->link_map_cache[item->txpriv.if_id]
		    [item->txpriv.link_id];

		spin_lock_bh(&stats->lock);
		++stats->num_queued[item->txpriv.if_id];
		++stats->link_map_cache[item->txpriv.if_id]
		    [item->txpriv.link_id];
		spin_unlock_bh(&stats->lock);

		++item->generation;
		item->packetID =
		    rk960_queue_make_packet_id(queue->generation,
					       queue->queue_id,
					       item->generation,
					       item - queue->pool,
					       item->txpriv.if_id,
					       item->txpriv.raw_link_id);
		list_move(&item->head, &queue->queue);
	}
	spin_unlock_bh(&queue->lock);

	return 0;
}

#ifdef CONFIG_RK960_TESTMODE
int rk960_queue_remove(struct rk960_common *hw_priv,
		       struct rk960_queue *queue, u32 packetID)
#else
int rk960_queue_remove(struct rk960_queue *queue, u32 packetID)
#endif				/*CONFIG_RK960_TESTMODE */
{
	int ret = 0;
	u8 queue_generation, queue_id, item_generation, item_id, if_id, link_id;
	struct rk960_queue_item *item;
	struct rk960_queue_stats *stats = queue->stats;
	struct sk_buff *gc_skb = NULL;
	struct rk960_txpriv gc_txpriv;

	rk960_queue_parse_id(packetID, &queue_generation, &queue_id,
			     &item_generation, &item_id, &if_id, &link_id);

	item = &queue->pool[item_id];

	spin_lock_bh(&queue->lock);
	BUG_ON(queue_id != queue->queue_id);
	/*TODO:COMBO:Add check for interface ID also */
	if (unlikely(queue_generation != queue->generation)) {
		ret = -ENOENT;
	} else if (unlikely(item_id >= (unsigned)queue->capacity)) {
		WARN_ON(1);
		ret = -EINVAL;
	} else if (unlikely(item->generation != item_generation)) {
		WARN_ON(1);
		ret = -ENOENT;
	} else {
		gc_txpriv = item->txpriv;
		gc_skb = item->skb;
		item->skb = NULL;
		--queue->num_pending;
		--queue->num_pending_vif[if_id];
		--queue->num_queued;
		--queue->num_queued_vif[if_id];
		++queue->num_sent;
		++item->generation;
#ifdef CONFIG_RK960_TESTMODE
		spin_lock_bh(&hw_priv->tsm_lock);
		if (hw_priv->start_stop_tsm.start) {
			if (queue_id == hw_priv->tsm_info.ac) {
				struct timeval tmval;
				unsigned long queue_delay;
				unsigned long media_delay;
				do_gettimeofday(&tmval);

				if (tmval.tv_usec > item->qdelay_timestamp)
					queue_delay = tmval.tv_usec -
					    item->qdelay_timestamp;
				else
					queue_delay = tmval.tv_usec +
					    1000000 - item->qdelay_timestamp;

				if (tmval.tv_usec > item->mdelay_timestamp)
					media_delay = tmval.tv_usec -
					    item->mdelay_timestamp;
				else
					media_delay = tmval.tv_usec +
					    1000000 - item->mdelay_timestamp;
				hw_priv->tsm_info.sum_media_delay +=
				    media_delay;
				hw_priv->tsm_info.sum_pkt_q_delay +=
				    queue_delay;
				if (queue_delay <= 10000)
					hw_priv->tsm_stats.bin0++;
				else if (queue_delay <= 20000)
					hw_priv->tsm_stats.bin1++;
				else if (queue_delay <= 40000)
					hw_priv->tsm_stats.bin2++;
				else
					hw_priv->tsm_stats.bin3++;
			}
		}
		spin_unlock_bh(&hw_priv->tsm_lock);
#endif /*CONFIG_RK960_TESTMODE */
		/* Do not use list_move_tail here, but list_move:
		 * try to utilize cache row.
		 */
		list_move(&item->head, &queue->free_pool);

		if (unlikely(queue->overfull) &&
		    (queue->num_queued <=
		     ((stats->hw_priv->vif0_throttle +
		       stats->hw_priv->vif1_throttle + 2) / 2))) {
			queue->overfull = false;
			__rk960_queue_unlock(queue);
		}
	}
	spin_unlock_bh(&queue->lock);

#if 0
	RK960_DEBUG_QUEUE("queue_drop queue %d, %d, %d\n",
			  queue->num_queued, queue->link_map_cache[if_id][0],
			  queue->num_pending);
	RK960_DEBUG_QUEUE("queue_drop stats %d, %d\n", stats->num_queued,
			  stats->link_map_cache[if_id][0]);
#endif
	if (gc_skb)
		stats->skb_dtor(stats->hw_priv, gc_skb, &gc_txpriv);

	return ret;
}

int rk960_queue_get_skb(struct rk960_queue *queue, u32 packetID,
			struct sk_buff **skb,
			const struct rk960_txpriv **txpriv)
{
	int ret = 0;
	u8 queue_generation, queue_id, item_generation, item_id, if_id, link_id;
	struct rk960_queue_item *item;
	struct rk960_common *hw_priv = queue->stats->hw_priv;

	rk960_queue_parse_id(packetID, &queue_generation, &queue_id,
			     &item_generation, &item_id, &if_id, &link_id);

	item = &queue->pool[item_id];
	/*item->conf_timestamp = jiffies;
	   pr_info("txed timestamp: if_id %d(%d) %lld %lld %lld %d %d\n",
	   if_id, link_id, item->queue_timestamp,
	   item->xmit_timestamp,
	   item->conf_timestamp,
	   item->xmit_timestamp - item->queue_timestamp,
	   item->conf_timestamp - item->queue_timestamp); */

	hw_priv->max_tx_conf_set = 0;
	if (packetID != hw_priv->pending_frame_id &&
	    item->xmited_timestamp != 0) {
		unsigned long milliseconds =
		    jiffies_to_msecs(jiffies - item->xmited_timestamp);
		if (milliseconds > hw_priv->ms_max_tx_conf_time) {
			hw_priv->max_tx_conf_set = 1;
			hw_priv->ms_max_tx_conf_time = milliseconds;
		}
	}

	spin_lock_bh(&queue->lock);
	BUG_ON(queue_id != queue->queue_id);
	/* TODO:COMBO: Add check for interface ID here */
	if (unlikely(queue_generation != queue->generation)) {
		ret = -ENOENT;
	} else if (unlikely(item_id >= (unsigned)queue->capacity)) {
		WARN_ON(1);
		ret = -EINVAL;
	} else if (unlikely(item->generation != item_generation)) {
		WARN_ON(1);
		ret = -ENOENT;
	} else {
		*skb = item->skb;
		*txpriv = &item->txpriv;
	}
	spin_unlock_bh(&queue->lock);
	return ret;
}

void rk960_queue_lock(struct rk960_queue *queue)
{
	spin_lock_bh(&queue->lock);
	__rk960_queue_lock(queue);
	spin_unlock_bh(&queue->lock);
}

void rk960_queue_unlock(struct rk960_queue *queue)
{
	spin_lock_bh(&queue->lock);
	__rk960_queue_unlock(queue);
	spin_unlock_bh(&queue->lock);
}

bool rk960_queue_get_xmit_timestamp(struct rk960_queue *queue,
				    unsigned long *timestamp, int if_id,
				    u32 pending_frameID)
{
	struct rk960_queue_item *item;
	bool ret;

	spin_lock_bh(&queue->lock);
	ret = !list_empty(&queue->pending);
	if (ret) {
		list_for_each_entry(item, &queue->pending, head) {
			if (((if_id == RK960_GENERIC_IF_ID) ||
			     (if_id == RK960_ALL_IFS) ||
			     (item->txpriv.if_id == if_id)) &&
			    (item->packetID != pending_frameID)) {
				if (time_before(item->xmit_timestamp,
						*timestamp))
					*timestamp = item->xmit_timestamp;
			}
		}
	}
	spin_unlock_bh(&queue->lock);
	return ret;
}

bool _rk960_queue_dump_pending_tx(struct rk960_queue * queue,
				  unsigned long *timestamp, int if_id,
				  u32 pending_frameID)
{
	struct rk960_queue_item *item;
	bool ret;

	spin_lock_bh(&queue->lock);
	ret = !list_empty(&queue->pending);
	if (ret) {
		list_for_each_entry(item, &queue->pending, head) {
			if (((if_id == RK960_GENERIC_IF_ID) ||
			     (if_id == RK960_ALL_IFS) ||
			     (item->txpriv.if_id == if_id)) &&
			    (item->packetID != pending_frameID)) {
				if (time_before(item->xmit_timestamp,
						*timestamp))
					RK960_INFO_QUEUE("queue_id %d if_id %d"
							 " packetID %x time(ms) %d\n",
							 queue->queue_id,
							 item->txpriv.if_id,
							 item->packetID,
							 jiffies_to_msecs
							 (*timestamp -
							  item->
							  xmit_timestamp));
			}
		}
	}
	spin_unlock_bh(&queue->lock);
	return ret;
}

void rk960_queue_dump_pending_tx(struct rk960_common *hw_priv, int if_id)
{
	unsigned long timestamp = jiffies;
	int i;

	RK960_ERROR_QUEUE("tx without complete happened: " "dump frames\n");
	for (i = 0; i < 4; ++i)
		_rk960_queue_dump_pending_tx(&hw_priv->tx_queue[i],
					     &timestamp, if_id,
					     hw_priv->pending_frame_id);
}

bool _rk960_queue_flush_pending_tx(struct rk960_queue *queue,
				   unsigned long *timestamp, int if_id,
				   u32 pending_frameID)
{
	struct rk960_queue_item *item;
	struct rk960_queue_item *items[RK960_MAX_QUEUE_SZ / 2];
	int items_count = 0;
	int i;
	bool ret;
	struct rk960_common *hw_priv;

	spin_lock_bh(&queue->lock);
	ret = !list_empty(&queue->pending);
	if (ret) {
		list_for_each_entry(item, &queue->pending, head) {
			if (((if_id == RK960_GENERIC_IF_ID) ||
			     (if_id == RK960_ALL_IFS) ||
			     (item->txpriv.if_id == if_id)) &&
			    (item->packetID != pending_frameID)) {
				if (time_before(item->xmit_timestamp,
						*timestamp))
					RK960_DEBUG_QUEUE("queue_id %d if_id %d"
							  " packetID %x time(ms) %d\n",
							  queue->queue_id,
							  item->txpriv.if_id,
							  item->packetID,
							  jiffies_to_msecs
							  (*timestamp -
							   item->
							   xmit_timestamp));
				BUG_ON(items_count >= RK960_MAX_QUEUE_SZ / 2);
				items[items_count++] = item;
			}
		}
	}
	spin_unlock_bh(&queue->lock);

	RK960_INFO_FWREC("%s items_count %d\n", __func__, items_count);

	hw_priv = queue->stats->hw_priv;
	for (i = 0; i < items_count; i++) {
		rk960_pending_tx_confirm(hw_priv,
					 items[i]->txpriv.if_id,
					 items[i]->packetID);
	}

	return ret;
}

void rk960_queue_flush_pending_tx(struct rk960_common *hw_priv, int if_id)
{
	unsigned long timestamp = jiffies;
	int i;

	for (i = 0; i < 4; ++i)
		_rk960_queue_flush_pending_tx(&hw_priv->tx_queue[i],
					      &timestamp, if_id,
					      hw_priv->pending_frame_id);
}

int _rk960_queue_tx_without_conf(struct rk960_queue *queue,
				 unsigned long *timestamp, int if_id,
				 u32 pending_frameID)
{
	struct rk960_queue_item *item;
	bool ret = 0;

	spin_lock_bh(&queue->lock);
	if (!list_empty(&queue->pending)) {
		list_for_each_entry(item, &queue->pending, head) {
			if (((if_id == RK960_GENERIC_IF_ID) ||
			     (if_id == RK960_ALL_IFS) ||
			     (item->txpriv.if_id == if_id)) &&
			    (item->packetID != pending_frameID) &&
			    item->xmited_timestamp) {
				if (time_before(item->xmited_timestamp +
						WSM_CMD_LAST_CHANCE_TIMEOUT,
						*timestamp)) {
					RK960_INFO_QUEUE("tx without conf: "
							 "queue_id %d if_id %d"
							 " pid %x time(ms) %d\n",
							 queue->queue_id,
							 item->txpriv.if_id,
							 item->packetID,
							 jiffies_to_msecs
							 (*timestamp -
							  item->
							  xmited_timestamp));
					ret = 1;
					break;
				}
			}
		}
	}
	spin_unlock_bh(&queue->lock);
	return ret;
}

int rk960_queue_tx_without_conf(struct rk960_common *hw_priv, int if_id)
{
	unsigned long timestamp = jiffies;
	int i;
	int ret = 0;

	for (i = 0; i < 4; ++i)
		ret += _rk960_queue_tx_without_conf(&hw_priv->tx_queue[i],
						    &timestamp, if_id,
						    hw_priv->pending_frame_id);
	return ret;
}

bool rk960_queue_stats_is_empty(struct rk960_queue_stats * stats,
				u32 link_id_map, int if_id)
{
	bool empty = true;

	spin_lock_bh(&stats->lock);
	if (link_id_map == (u32) - 1)
		empty = stats->num_queued[if_id] == 0;
	else {
		int i, if_id;
		for (if_id = 0; if_id < RK960_MAX_VIFS; if_id++) {
			for (i = 0; i < stats->map_capacity; ++i) {
				if (link_id_map & BIT(i)) {
					if (stats->link_map_cache[if_id][i]) {
						empty = false;
						break;
					}
				}
			}
		}
	}
	spin_unlock_bh(&stats->lock);

	return empty;
}
