/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef RK960_DEBUG_H_INCLUDED
#define RK960_DEBUG_H_INCLUDED

#include "itp.h"

extern int rk960_debug_flag;
extern int rk960_debug_level;

//#define RK960_SDIO_TX_TP_TEST
//#define RK960_SDIO_RX_TP_TEST
#define DUMP_TXRX_MAC_FRAME_INFO

#define MODULE_TAG "RK960: "
#define DEBUG_TAG "DEBUG: "
#define INFO_TAG "INFO: "
#define ERROR_TAG "ERROR: "

void dump_ieee80211_hdr_info(unsigned char *data, int len, int tx, s8 rssi);

#define RK960_DEBUG_BH(fmt, ...)            \
do {                                          \
	if (rk960_debug_flag & DEBUG_BH &&	\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)      \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_INFO_BH(fmt, ...)            \
do {                                          \
	if (rk960_debug_flag & DEBUG_BH &&	\
		rk960_debug_level & DEBUG_LEVEL_INFO)      \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_ERROR_BH(fmt, ...)            \
do {                                          \
	if (rk960_debug_flag & DEBUG_BH &&	\
		rk960_debug_level & DEBUG_LEVEL_ERROR)      \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_DEBUG_IO(fmt, ...)        \
do {                                         \
	if (rk960_debug_flag & DEBUG_IO &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)  \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);  \
} while (0)

#define RK960_INFO_IO(fmt, ...)        \
do {                                         \
	if (rk960_debug_flag & DEBUG_IO &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)  \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);  \
} while (0)

#define RK960_ERROR_IO(fmt, ...)        \
do {                                         \
	if (rk960_debug_flag & DEBUG_IO &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)  \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);  \
} while (0)

#define RK960_DEBUG_FW(fmt, ...)           \
do {                                           \
	if (rk960_debug_flag & DEBUG_FW &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)     \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);    \
} while (0)

#define RK960_INFO_FW(fmt, ...)           \
do {                                           \
	if (rk960_debug_flag & DEBUG_FW &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)     \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);    \
} while (0)

#define RK960_ERROR_FW(fmt, ...)           \
do {                                           \
	if (rk960_debug_flag & DEBUG_FW &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)     \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);    \
} while (0)

#define RK960_DEBUG_MAIN(fmt, ...)			\
do {                                             \
	if (rk960_debug_flag & DEBUG_MAIN &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)		  \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);      \
} while (0)

#define RK960_INFO_MAIN(fmt, ...)			\
do {                                             \
	if (rk960_debug_flag & DEBUG_MAIN &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)		  \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);      \
} while (0)

#define RK960_ERROR_MAIN(fmt, ...)			\
do {                                             \
	if (rk960_debug_flag & DEBUG_MAIN &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)		  \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);      \
} while (0)

#define RK960_DEBUG_PM(fmt, ...)			\
do {                                             \
	if (rk960_debug_flag & DEBUG_PM &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)		  \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);      \
} while (0)

#define RK960_INFO_PM(fmt, ...)			\
do {                                             \
	if (rk960_debug_flag & DEBUG_PM &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)		  \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);      \
} while (0)

#define RK960_ERROR_PM(fmt, ...)			\
do {                                             \
	if (rk960_debug_flag & DEBUG_PM &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)		  \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);      \
} while (0)

#define RK960_DEBUG_SCAN(fmt, ...)            \
do {                                          \
	if (rk960_debug_flag & DEBUG_SCAN &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)      \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_INFO_SCAN(fmt, ...)            \
do {                                          \
	if (rk960_debug_flag & DEBUG_SCAN &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)      \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_ERROR_SCAN(fmt, ...)            \
do {                                          \
	if (rk960_debug_flag & DEBUG_SCAN &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)      \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_DEBUG_QUEUE(fmt, ...)            \
do {                                         \
	if (rk960_debug_flag & DEBUG_QUEUE &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)      \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);  \
} while (0)

#define RK960_INFO_QUEUE(fmt, ...)            \
do {                                         \
	if (rk960_debug_flag & DEBUG_QUEUE &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)      \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);  \
} while (0)

#define RK960_ERROR_QUEUE(fmt, ...)            \
do {                                         \
	if (rk960_debug_flag & DEBUG_QUEUE &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)      \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);  \
} while (0)

#define RK960_DEBUG_STA(fmt, ...)             \
do {                                           \
	if (rk960_debug_flag & DEBUG_STA &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)       \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);    \
} while (0)

#define RK960_INFO_STA(fmt, ...)             \
do {                                           \
	if (rk960_debug_flag & DEBUG_STA &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)       \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);    \
} while (0)

#define RK960_ERROR_STA(fmt, ...)             \
do {                                           \
	if (rk960_debug_flag & DEBUG_STA &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)       \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);    \
} while (0)

#define RK960_DEBUG_TXRX(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_TXRX &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)        \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_INFO_TXRX(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_TXRX &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)        \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_ERROR_TXRX(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_TXRX &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)        \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_DEBUG_WSM(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_WSM &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)        \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_INFO_WSM(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_WSM &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)        \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_ERROR_WSM(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_WSM &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)        \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_DEBUG_AP(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_AP &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)        \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_INFO_AP(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_AP &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)        \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_ERROR_AP(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_AP &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)        \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_DEBUG_TXPOLICY(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_TXPOLICY &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)        \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_INFO_TXPOLICY(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_TXPOLICY &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)        \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_ERROR_TXPOLICY(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_TXPOLICY &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)        \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_DEBUG_FWREC(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_FW_REC &&		\
		rk960_debug_level & DEBUG_LEVEL_DEBUG)        \
		pr_info(MODULE_TAG DEBUG_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_INFO_FWREC(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_FW_REC &&		\
		rk960_debug_level & DEBUG_LEVEL_INFO)        \
		pr_info(MODULE_TAG INFO_TAG fmt, ##__VA_ARGS__);   \
} while (0)

#define RK960_ERROR_FWREC(fmt, ...)              \
do {                                          \
	if (rk960_debug_flag & DEBUG_FW_REC &&		\
		rk960_debug_level & DEBUG_LEVEL_ERROR)        \
		pr_err(MODULE_TAG ERROR_TAG fmt, ##__VA_ARGS__);   \
} while (0)

enum rk960_debug_flag_e {
	DEBUG_BH = BIT(1),
	DEBUG_IO = BIT(2),
	DEBUG_FW = BIT(3),
	DEBUG_MAIN = BIT(4),
	DEBUG_PM = BIT(5),
	DEBUG_QUEUE = BIT(6),
	DEBUG_SCAN = BIT(7),
	DEBUG_STA = BIT(8),
	DEBUG_TXRX = BIT(9),
	DEBUG_WSM = BIT(10),
	DEBUG_AP = BIT(11),
	DEBUG_TXPOLICY = BIT(12),
	DEBUG_FW_REC = BIT(13),
};

enum rk960_debug_level_e {
	DEBUG_LEVEL_ERROR = BIT(1),
	DEBUG_LEVEL_INFO = BIT(2),
	DEBUG_LEVEL_DEBUG = BIT(3),
};

struct cw200_common;

#ifdef CONFIG_RK960_DEBUGFS

struct rk960_debug_common {
	struct dentry *debugfs_phy;
	int tx_cache_miss;
	int tx_burst;
	int tx_drops;
	int rx_burst;
	int ba_cnt;
	int ba_acc;
	int ba_cnt_rx;
	int ba_acc_rx;
#ifdef CONFIG_RK960_ITP
	struct rk960_itp itp;
#endif				/* CONFIG_RK960_ITP */
};

struct rk960_debug_priv {
	struct dentry *debugfs_phy;
	int tx;
	int tx_agg;
	int rx;
	int rx_agg;
	int tx_multi;
	int tx_multi_frames;
	int tx_align;
	int tx_ttl;
};

void rk960_debug_level_init(void);
int rk960_debug_init_common(struct rk960_common *hw_priv);
int rk960_debug_init_priv(struct rk960_common *hw_priv, struct rk960_vif *priv);
void rk960_debug_release_common(struct rk960_common *hw_priv);
void rk960_debug_release_priv(struct rk960_vif *priv);
int rk960_access_file(char *path, char *buffer, int size, int isRead);
int rk960_get_file_size(char *path);
int ieee80211_crypt_hdrlen(u16 fc);
int rk960_rate_to_rate_index(int rate);

static inline void rk960_debug_txed(struct rk960_vif *priv)
{
	if (priv->debug)
		++priv->debug->tx;
}

static inline void rk960_debug_txed_agg(struct rk960_vif *priv)
{
	if (priv->debug)
		++priv->debug->tx_agg;
}

static inline void rk960_debug_txed_multi(struct rk960_vif *priv, int count)
{
	if (priv->debug) {
		++priv->debug->tx_multi;
		priv->debug->tx_multi_frames += count;
	}
}

static inline void rk960_debug_rxed(struct rk960_vif *priv)
{
	if (priv->debug)
		++priv->debug->rx;
}

static inline void rk960_debug_rxed_agg(struct rk960_vif *priv)
{
	if (priv->debug)
		++priv->debug->rx_agg;
}

static inline void rk960_debug_tx_cache_miss(struct rk960_common *common)
{
	++common->debug->tx_cache_miss;
}

static inline void rk960_debug_tx_align(struct rk960_vif *priv)
{
	if (priv->debug)
		++priv->debug->tx_align;
}

static inline void rk960_debug_tx_ttl(struct rk960_vif *priv)
{
	if (priv->debug)
		++priv->debug->tx_ttl;
}

static inline void rk960_debug_tx_burst(struct rk960_common *hw_priv)
{
	++hw_priv->debug->tx_burst;
}

static inline void rk960_debug_tx_drops(struct rk960_common *hw_priv)
{
	++hw_priv->debug->tx_drops;
}

static inline void rk960_debug_rx_burst(struct rk960_common *hw_priv)
{
	++hw_priv->debug->rx_burst;
}

static inline void rk960_debug_ba(struct rk960_common *hw_priv,
				  int ba_cnt, int ba_acc, int ba_cnt_rx,
				  int ba_acc_rx)
{
	hw_priv->debug->ba_cnt = ba_cnt;
	hw_priv->debug->ba_acc = ba_acc;
	hw_priv->debug->ba_cnt_rx = ba_cnt_rx;
	hw_priv->debug->ba_acc_rx = ba_acc_rx;
}

int rk960_print_fw_version(struct rk960_common *hw_priv, u8 * buf, size_t len);

#else /* CONFIG_RK960_DEBUGFS */

static inline int rk960_debug_init_common(struct rk960_common *hw_priv)
{
	return 0;
}

static inline int rk960_debug_init_priv(struct rk960_common *hw_priv,
					struct rk960_vif *priv)
{
	return 0;
}

static inline void rk960_debug_release_common(struct rk960_common *hw_priv)
{
}

static inline void rk960_debug_release_priv(struct rk960_vif *priv)
{
}

static inline void rk960_debug_txed(struct rk960_vif *priv)
{
}

static inline void rk960_debug_txed_agg(struct rk960_vif *priv)
{
}

static inline void rk960_debug_txed_multi(struct rk960_vif *priv, int count)
{
}

static inline void rk960_debug_rxed(struct rk960_vif *priv)
{
}

static inline void rk960_debug_rxed_agg(struct rk960_vif *priv)
{
}

static inline void rk960_debug_tx_cache_miss(struct rk960_vif *priv)
{
}

static inline void rk960_debug_tx_align(struct rk960_vif *priv)
{
}

static inline void rk960_debug_tx_ttl(struct rk960_vif *priv)
{
}

static inline void rk960_debug_tx_burst(struct rk960_vif *priv)
{
}

static inline void rk960_debug_tx_drops(struct rk960_vif *priv)
{
}

static inline void rk960_debug_rx_burst(struct rk960_vif *priv)
{
}

static inline void rk960_debug_ba(struct rk960_common *hw_priv,
				  int ba_cnt, int ba_acc, int ba_cnt_rx,
				  int ba_acc_rx)
{
}

int rk960_print_fw_version(struct rk960_vif *priv, u8 * buf, size_t len)
{
}

#endif /* CONFIG_RK960_DEBUGFS */

#endif /* RK960_DEBUG_H_INCLUDED */
