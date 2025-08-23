/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef RK960_QUEUE_H_INCLUDED
#define RK960_QUEUE_H_INCLUDED

/* private */ struct rk960_queue_item;

/* extern */ struct sk_buff;
/* extern */ struct wsm_tx;
/* extern */ struct rk960_common;
/* extern */ struct rk960_vif;
/* extern */ struct ieee80211_tx_queue_stats;
/* extern */ struct rk960_txpriv;

/* forward */ struct rk960_queue_stats;

typedef void (*rk960_queue_skb_dtor_t) (struct rk960_common * priv,
					struct sk_buff * skb,
					const struct rk960_txpriv * txpriv);

struct rk960_queue {
	struct rk960_queue_stats *stats;
	size_t capacity;
	size_t num_queued;
	size_t num_queued_vif[RK960_MAX_VIFS];
	size_t num_pending;
	size_t num_pending_vif[RK960_MAX_VIFS];
	size_t num_sent;
	struct rk960_queue_item *pool;
	struct list_head queue;
	struct list_head free_pool;
	struct list_head pending;
	int tx_locked_cnt;
	int *link_map_cache[RK960_MAX_VIFS];
	bool overfull;
	spinlock_t lock;
	u8 queue_id;
	u8 generation;
	struct timer_list gc;
	unsigned long ttl;
};

struct rk960_queue_stats {
	spinlock_t lock;
	int *link_map_cache[RK960_MAX_VIFS];
	int num_queued[RK960_MAX_VIFS];
	size_t map_capacity;
	wait_queue_head_t wait_link_id_empty;
	rk960_queue_skb_dtor_t skb_dtor;
	struct rk960_common *hw_priv;
};

struct rk960_txpriv {
	u8 link_id;
	u8 raw_link_id;
	u8 tid;
	u8 rate_id;
	u8 offset;
	u8 if_id;
#ifndef P2P_MULTIVIF
	u8 offchannel_if_id;
#else
	u8 raw_if_id;
#endif
};

int rk960_queue_stats_init(struct rk960_queue_stats *stats,
			   size_t map_capacity,
			   rk960_queue_skb_dtor_t skb_dtor,
			   struct rk960_common *priv);
int rk960_queue_init(struct rk960_queue *queue,
		     struct rk960_queue_stats *stats,
		     u8 queue_id, size_t capacity, unsigned long ttl);
int rk960_queue_clear(struct rk960_queue *queue, int if_id);
void rk960_queue_stats_deinit(struct rk960_queue_stats *stats);
void rk960_queue_deinit(struct rk960_queue *queue);

size_t rk960_queue_get_num_queued(struct rk960_vif *priv,
				  struct rk960_queue *queue, u32 link_id_map);
int rk960_queue_put(struct rk960_queue *queue,
		    struct sk_buff *skb, struct rk960_txpriv *txpriv);
int rk960_queue_get(struct rk960_queue *queue,
		    int if_id,
		    u32 link_id_map,
		    struct wsm_tx **tx,
		    struct ieee80211_tx_info **tx_info,
		    struct rk960_txpriv **txpriv,
		    unsigned long **xmited_timestamp);
#ifdef CONFIG_RK960_TESTMODE
int rk960_queue_requeue(struct rk960_common *hw_priv,
			struct rk960_queue *queue, u32 packetID, bool check);
#else
int rk960_queue_requeue(struct rk960_queue *queue, u32 packetID, bool check);
#endif
int rk960_queue_requeue_all(struct rk960_queue *queue);
#ifdef CONFIG_RK960_TESTMODE
int rk960_queue_remove(struct rk960_common *hw_priv,
		       struct rk960_queue *queue, u32 packetID);
#else
int rk960_queue_remove(struct rk960_queue *queue, u32 packetID);
#endif /*CONFIG_RK960_TESTMODE */
int rk960_queue_get_skb(struct rk960_queue *queue, u32 packetID,
			struct sk_buff **skb,
			const struct rk960_txpriv **txpriv);
void rk960_queue_lock(struct rk960_queue *queue);
void rk960_queue_unlock(struct rk960_queue *queue);
bool rk960_queue_get_xmit_timestamp(struct rk960_queue *queue,
				    unsigned long *timestamp, int if_id,
				    u32 pending_frameID);
void rk960_queue_dump_pending_tx(struct rk960_common *hw_priv, int if_id);
void rk960_queue_flush_pending_tx(struct rk960_common *hw_priv, int if_id);
int rk960_queue_tx_without_conf(struct rk960_common *hw_priv, int if_id);

bool rk960_queue_stats_is_empty(struct rk960_queue_stats *stats,
				u32 link_id_map, int if_id);

static inline u8 rk960_queue_get_queue_id(u32 packetID)
{
	return (packetID >> 16) & 0xF;
}

static inline u8 rk960_queue_get_if_id(u32 packetID)
{
	return (packetID >> 20) & 0xF;
}

static inline u8 rk960_queue_get_link_id(u32 packetID)
{
	return (packetID >> 24) & 0xF;
}

static inline u8 rk960_queue_get_generation(u32 packetID)
{
	return (packetID >> 8) & 0xFF;
}

#endif /* RK960_QUEUE_H_INCLUDED */
