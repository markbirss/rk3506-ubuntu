/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <net/mac80211.h>
#include <linux/kthread.h>

#include "rk960.h"
#include "bh.h"
#include "hwio.h"
#include "wsm.h"
#include "hwbus.h"
#include "debug.h"
#include <linux/gpio.h>
#if (LINUX_VERSION_CODE > KERNEL_VERSION(4, 6, 0))
#include <uapi/linux/sched/types.h>
#endif

static int fw_no_sleep = 0;
module_param(fw_no_sleep, int, 0644);
MODULE_PARM_DESC(fw_no_sleep, "disable fw sleep");

extern int rk960_jtag_debug_enable(void);

static int rk960_bh(void *arg);
#ifdef RK960_USE_RX_THREAD
static int rk960_rx(void *arg);
#endif

/* TODO: Verify these numbers with WSM specification. */
#define DOWNLOAD_BLOCK_SIZE_WR	(0x1000 - 4)
/* an SPI message cannot be bigger than (2"12-1)*2 bytes
 * "*2" to cvt to bytes */
#define MAX_SZ_RD_WR_BUFFERS	(DOWNLOAD_BLOCK_SIZE_WR*2)
#define PIGGYBACK_CTRL_REG	(2)
#define EFFECTIVE_BUF_SIZE	(MAX_SZ_RD_WR_BUFFERS - PIGGYBACK_CTRL_REG)

/* Suspend state privates */
enum rk960_bh_pm_state {
	RK960_BH_RESUMED = 0,
	RK960_BH_SUSPEND,
	RK960_BH_SUSPENDED,
	RK960_BH_RESUME,
};

typedef int (*rk960_wsm_handler) (struct rk960_common * hw_priv,
				  u8 * data, size_t size);

#ifdef MCAST_FWDING
int wsm_release_buffer_to_fw(struct rk960_vif *priv, int count);
#endif

int rk960_register_bh(struct rk960_common *hw_priv)
{
	int err = 0;
	//struct sched_param param = {.sched_priority = 1 };
	RK960_DEBUG_BH("[BH] register.\n");
	BUG_ON(hw_priv->bh_thread);
	atomic_set(&hw_priv->bh_rx, 0);
	atomic_set(&hw_priv->bh_tx, 0);
	atomic_set(&hw_priv->bh_term, 0);
	atomic_set(&hw_priv->bh_suspend, RK960_BH_RESUMED);
	hw_priv->buf_id_tx = 0;
	hw_priv->buf_id_rx = 0;
#ifdef RK960_USE_RX_THREAD
	init_waitqueue_head(&hw_priv->bh_rx_wq);
#endif
	init_waitqueue_head(&hw_priv->bh_wq);
	init_waitqueue_head(&hw_priv->bh_evt_wq);
	hw_priv->bh_thread = kthread_create(&rk960_bh, hw_priv, "rk960_bh");
	if (IS_ERR(hw_priv->bh_thread)) {
		err = PTR_ERR(hw_priv->bh_thread);
		hw_priv->bh_thread = NULL;
	} else {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
		sched_set_fifo_low(hw_priv->bh_thread);
#else
		WARN_ON(sched_setscheduler_nocheck(hw_priv->bh_thread,
						   SCHED_FIFO, &param));
#endif
#else
		WARN_ON(sched_setscheduler(hw_priv->bh_thread,
					   SCHED_FIFO, &param));
#endif
#ifdef HAS_PUT_TASK_STRUCT
		get_task_struct(hw_priv->bh_thread);
#endif
		wake_up_process(hw_priv->bh_thread);
	}

#ifdef RK960_USE_RX_THREAD
	hw_priv->rx_thread = kthread_create(&rk960_rx, hw_priv, "rk960_rx");
	if (IS_ERR(hw_priv->rx_thread)) {
		err = PTR_ERR(hw_priv->rx_thread);
		hw_priv->rx_thread = NULL;
	} else {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
		sched_set_fifo_low(hw_priv->rx_thread);
#else
		WARN_ON(sched_setscheduler_nocheck(hw_priv->rx_thread,
						   SCHED_FIFO, &param));
#endif
#else
		WARN_ON(sched_setscheduler(hw_priv->rx_thread,
					   SCHED_FIFO, &param));
#endif
#ifdef HAS_PUT_TASK_STRUCT
		get_task_struct(hw_priv->rx_thread);
#endif
		wake_up_process(hw_priv->rx_thread);

		skb_queue_head_init(&hw_priv->rxq);
	}
#endif
	return err;
}

void rk960_unregister_bh(struct rk960_common *hw_priv)
{
#ifdef RK960_USE_RX_THREAD
	struct sk_buff *skb_rx;
#endif
	struct task_struct *thread = hw_priv->bh_thread;
	if (WARN_ON(!thread))
		return;

	hw_priv->bh_thread = NULL;
	RK960_DEBUG_BH("[BH] unregister.\n");
	atomic_add(1, &hw_priv->bh_term);
	wake_up(&hw_priv->bh_wq);
	kthread_stop(thread);
#ifdef HAS_PUT_TASK_STRUCT
	put_task_struct(thread);
#endif

#ifdef RK960_USE_RX_THREAD
	thread = hw_priv->rx_thread;
	if (WARN_ON(!thread))
		return;

	hw_priv->rx_thread = NULL;
	atomic_add(1, &hw_priv->bh_rx_term);
	kthread_stop(thread);
#ifdef HAS_PUT_TASK_STRUCT
	put_task_struct(thread);
#endif

	while ((skb_rx = skb_dequeue(&hw_priv->rxq)))
		dev_kfree_skb_any(skb_rx);
#endif
}

void rk960_irq_handler(struct rk960_common *hw_priv)
{
	//RK960_DEBUG_BH("[BH] irq.\n");
	if ( /* WARN_ON */ (hw_priv->bh_error))
		return;

#ifdef CONFIG_WAKELOCK
	if (hw_priv->wow_suspend_done)
		rk960_pm_stay_awake(&hw_priv->pm_state, 3 * HZ / 100);
#endif

	if (atomic_add_return(1, &hw_priv->bh_rx) == 1)
		wake_up(&hw_priv->bh_wq);
}

void rk960_bh_wakeup(struct rk960_common *hw_priv)
{
	RK960_DEBUG_BH("[BH] wakeup.\n");
	if (WARN_ON(hw_priv->bh_error))
		return;

	if (atomic_add_return(1, &hw_priv->bh_tx) == 1)
		wake_up(&hw_priv->bh_wq);
}

int rk960_bh_suspend(struct rk960_common *hw_priv)
{
#ifdef MCAST_FWDING
	int i = 0;
	struct rk960_vif *priv = NULL;
#endif

	RK960_DEBUG_BH("[BH] suspend.\n");
	if (hw_priv->bh_error) {
		RK960_ERROR_BH("BH error -- can't suspend\n");
		return -EINVAL;
	}
#ifdef MCAST_FWDING

	//rk960_for_each_vif(hw_priv, priv, i) {
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		priv = hw_priv->vif_list[i] ?
		    rk960_get_vif_from_ieee80211(hw_priv->vif_list[i]) : NULL;
		if (!priv)
			continue;
		if ((priv->multicast_filter.enable)
		    && (priv->join_status == RK960_JOIN_STATUS_AP)) {
			wsm_release_buffer_to_fw(priv,
						 (hw_priv->wsm_caps.
						  numInpChBufs - 1));
			break;
		}
	}
#endif

	atomic_set(&hw_priv->bh_suspend, RK960_BH_SUSPEND);
	wake_up(&hw_priv->bh_wq);
	return wait_event_timeout(hw_priv->bh_evt_wq, hw_priv->bh_error ||
				  (RK960_BH_SUSPENDED ==
				   atomic_read(&hw_priv->bh_suspend)),
				  1 * HZ) ? 0 : -ETIMEDOUT;
}

int rk960_bh_resume(struct rk960_common *hw_priv)
{
#ifdef MCAST_FWDING
	int i = 0;
	int ret;
	struct rk960_vif *priv = NULL;
#endif

	RK960_DEBUG_BH("[BH] resume.\n");
	if (hw_priv->bh_error) {
		RK960_ERROR_BH("BH error -- can't resume\n");
		return -EINVAL;
	}

	atomic_set(&hw_priv->bh_suspend, RK960_BH_RESUME);
	wake_up(&hw_priv->bh_wq);

#ifdef MCAST_FWDING
	ret = wait_event_timeout(hw_priv->bh_evt_wq, hw_priv->bh_error ||
				 (RK960_BH_RESUMED ==
				  atomic_read(&hw_priv->bh_suspend)),
				 1 * HZ) ? 0 : -ETIMEDOUT;

	//rk960_for_each_vif(hw_priv, priv, i) {
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		priv = hw_priv->vif_list[i] ?
		    rk960_get_vif_from_ieee80211(hw_priv->vif_list[i]) : NULL;
		if (!priv)
			continue;
		if ((priv->join_status == RK960_JOIN_STATUS_AP)
		    && (priv->multicast_filter.enable)) {
			u8 count = 0;
			WARN_ON(wsm_request_buffer_request(priv, &count));
			RK960_DEBUG_BH("[BH] BH resume. Reclaim Buff %d \n",
				       count);
			break;
		}
	}

	return ret;
#else
	return wait_event_timeout(hw_priv->bh_evt_wq, hw_priv->bh_error ||
				  (RK960_BH_RESUMED ==
				   atomic_read(&hw_priv->bh_suspend)),
				  1 * HZ) ? 0 : -ETIMEDOUT;
#endif

}

static inline void wsm_alloc_tx_buffer(struct rk960_common *hw_priv)
{
#ifdef RK960_USE_RX_THREAD
	spin_lock_bh(&hw_priv->hw_bufs_used_lock);
#endif
	++hw_priv->hw_bufs_used;
#ifdef RK960_USE_RX_THREAD
	spin_unlock_bh(&hw_priv->hw_bufs_used_lock);
#endif
}

int wsm_release_tx_buffer(struct rk960_common *hw_priv, int count)
{
	int ret = 0;
	int hw_bufs_used;

#ifdef RK960_USE_RX_THREAD
	spin_lock_bh(&hw_priv->hw_bufs_used_lock);
#endif

	hw_bufs_used = hw_priv->hw_bufs_used;
	hw_priv->hw_bufs_used -= count;

	if (WARN_ON(hw_priv->hw_bufs_used < 0))
		ret = -1;
	/* Tx data patch stops when all but one hw buffers are used.
	   So, re-start tx path in case we find hw_bufs_used equals
	   numInputChBufs - 1.
	 */
	else if (hw_bufs_used >= (hw_priv->wsm_caps.numInpChBufs - 1))
		ret = 1;
	if (!hw_priv->hw_bufs_used)
		wake_up(&hw_priv->bh_evt_wq);

#ifdef RK960_USE_RX_THREAD
	spin_unlock_bh(&hw_priv->hw_bufs_used_lock);
#endif
	return ret;
}

int wsm_release_vif_tx_buffer(struct rk960_common *hw_priv, int if_id,
			      int count)
{
	int ret = 0;

#ifdef RK960_USE_RX_THREAD
	spin_lock_bh(&hw_priv->hw_bufs_used_lock);
#endif

	hw_priv->hw_bufs_used_vif[if_id] -= count;

	if (!hw_priv->hw_bufs_used_vif[if_id])
		wake_up(&hw_priv->bh_evt_wq);

	if (WARN_ON(hw_priv->hw_bufs_used_vif[if_id] < 0))
		ret = -1;

#ifdef RK960_USE_RX_THREAD
	spin_unlock_bh(&hw_priv->hw_bufs_used_lock);
#endif
	return ret;
}

#ifdef MCAST_FWDING
int wsm_release_buffer_to_fw(struct rk960_vif *priv, int count)
{
	int i;
	u8 flags;
	struct wsm_buf *buf;
	size_t buf_len;
	struct wsm_hdr *wsm;
	struct rk960_common *hw_priv = priv->hw_priv;

#if 1
	if (priv->join_status != RK960_JOIN_STATUS_AP) {
		return 0;
	}
#endif
	RK960_DEBUG_BH("Rel buffer to FW %d, %d\n", count,
		       hw_priv->hw_bufs_used);

	for (i = 0; i < count; i++) {
		if ((hw_priv->hw_bufs_used + 1) <
		    hw_priv->wsm_caps.numInpChBufs) {
			flags = i ? 0 : 0x1;

			wsm_alloc_tx_buffer(hw_priv);

			buf = &hw_priv->wsm_release_buf[i];
			buf_len = buf->data - buf->begin;

			/* Add sequence number */
			wsm = (struct wsm_hdr *)buf->begin;
			BUG_ON(buf_len < sizeof(*wsm));

			wsm->id &= __cpu_to_le32(~WSM_TX_SEQ(WSM_TX_SEQ_MAX));
			wsm->id |= cpu_to_le32(WSM_TX_SEQ(hw_priv->wsm_tx_seq));

			RK960_DEBUG_BH("REL %d\n", hw_priv->wsm_tx_seq);
			if (WARN_ON(rk960_data_write(hw_priv,
						     buf->begin, buf_len))) {
				break;
			}
			hw_priv->buf_released = 1;
			hw_priv->wsm_tx_seq = (hw_priv->wsm_tx_seq + 1) &
			    WSM_TX_SEQ_MAX;
		} else
			break;
	}

	if (i == count) {
		return 0;
	}

	/* Should not be here */
	RK960_ERROR_BH("[BH] Less HW buf %d,%d.\n", hw_priv->hw_bufs_used,
		       hw_priv->wsm_caps.numInpChBufs);
	WARN_ON(1);

	return -1;
}
#endif

static struct sk_buff *rk960_get_skb(struct rk960_common *hw_priv, size_t len)
{
	struct sk_buff *skb;
	size_t alloc_len =
	    (len > RK960_SDIO_BLOCK_SIZE) ? len : RK960_SDIO_BLOCK_SIZE;

#if 1
	if (1) {
#else
	if (len > RK960_SDIO_BLOCK_SIZE || !hw_priv->skb_cache) {
#endif
		skb = dev_alloc_skb(alloc_len + WSM_TX_EXTRA_HEADROOM + 8	/* TKIP IV */
				    + 12	/* TKIP ICV + MIC */
				    - 2 /* Piggyback */ );
		if (!skb) {
			RK960_ERROR_BH("[BH]: %s fail (%d)\n", __func__,
				       (int)alloc_len);
			return skb;
		}
		/* In AP mode RXed SKB can be looped back as a broadcast.
		 * Here we reserve enough space for headers. */
		skb_reserve(skb, WSM_TX_EXTRA_HEADROOM + 8	/* TKIP IV */
			    - WSM_RX_EXTRA_HEADROOM);
	} else {
		skb = hw_priv->skb_cache;
		hw_priv->skb_cache = NULL;
	}
	return skb;
}

static void rk960_put_skb(struct rk960_common *hw_priv, struct sk_buff *skb)
{
#if 1
	dev_kfree_skb(skb);
#else
	if (hw_priv->skb_cache)
		dev_kfree_skb(skb);
	else
		hw_priv->skb_cache = skb;
#endif
}

int rk960_bh_read_ctrl_reg(struct rk960_common *priv, u16 * ctrl_reg)
{
	unsigned char len[2];
	unsigned int length;

	len[0] = cw2000_sdio_readb(priv, IO_RECV_LEN_L);
	if (len[0] < 0) {
		*ctrl_reg = 0;
		return 0;
	}
	len[1] = cw2000_sdio_readb(priv, IO_RECV_LEN_H);
	if (len[1] < 0) {
		*ctrl_reg = 0;
		return 0;
	}
	length = (len[1] << 8) | len[0];
	*ctrl_reg = length;

	//RK960_DEBUG_BH("%s len = %d.\n", __func__, length);   

	return 0;
}

int rk960_device_wakeup(struct rk960_common *hw_priv, int wake)
{
	int ret;

	if (fw_no_sleep)
		return 1;

	if (!hw_priv->wsm_caps.firmwareReady)
		return 1;

	//RK960_DEBUG_BH("[BH] Device wakeup.\n");

	if (hw_priv->device_wakeing)
		return 0;
	if (wake) {
		hw_priv->device_wakeing = true;
	} else if (!wake && hw_priv->device_can_sleep) {
		return 0;
	}

	ret = __rk960_wakeup_device(hw_priv, wake);
	if (WARN_ON(ret)) {
#ifdef RK960_FW_ERROR_RECOVERY
		hw_priv->device_wakeing = false;
		rk960_signal_fw_error(hw_priv, RK960_FWERR_REASON_SDIO);
#else
		return ret;
#endif
	}

	if (!wake)
		hw_priv->device_can_sleep = true;

	RK960_INFO_BH("[BH] Notify Device %s.\n", wake ? "wake" : "sleep");
	return 0;
}

/* Must be called from BH thraed. */
void rk960_enable_powersave(struct rk960_vif *priv, bool enable)
{
	RK960_DEBUG_BH("[BH] Powerave(if=%d) is %s.\n", priv->if_id,
		       enable ? "enabled" : "disabled");
	priv->powersave_enabled = enable;
}

#if (defined RK960_SDIO_TX_TP_TEST) || (defined RK960_SDIO_RX_TP_TEST)
#include <asm/div64.h>
#define CALCULATE_RATE_LENGTH (50*1024*1024)

#define INIT_GET_SPEND_TIME(start_time, end_time)		\
	struct timeval start_time, end_time;

#define START_GET_SPEND_TIME(start_time, end_time)		\
	do_gettimeofday(&start_time);

#define END_GET_SPEND_TIME(start_time, end_time)		\
	do_gettimeofday(&end_time);

#define GET_SPEND_TIME_US(start_time, end_time)			\
	(((end_time.tv_sec & 0xFFF) * 1000000 + end_time.tv_usec) -	\
	((start_time.tv_sec & 0xFFF) * 1000000 + start_time.tv_usec))
#endif

#if USE_SDIO_RX_BURST_MULTI
static void hwbus_rcvbuf_init(struct rk960_common *hw_priv)
{
	int i;
	struct wsm_hdr *wsm;

	hw_priv->rcvbuf_offset = 0;
	for (i = 0; i < EFFECTIVE_RX_BUF_SIZE; i += RK960_SDIO_BLOCK_SIZE) {
		wsm = (struct wsm_hdr *)(hw_priv->hwbus_rcvbuf + i);
		wsm->len = 0xFFFF;
	}
}

static u8 *hwbus_rcvbuf_next_msg(struct rk960_common *hw_priv, u16 * next_len)
{
	struct wsm_hdr *wsm;
	u8 *start, *end;

	if (hw_priv->rcvbuf_offset >= EFFECTIVE_RX_BUF_SIZE)
		return NULL;

	*next_len = 0;
	wsm =
	    (struct wsm_hdr *)(hw_priv->hwbus_rcvbuf + hw_priv->rcvbuf_offset);
	if (wsm->len == 0xFFFF)
		return NULL;

	// get the next rx msgs length
	// bit15 = 1 means have next msgs, else no next msgs
	// store the next msgs length at the end of cur sending msg  
	if (wsm->len & 0x8000) {
		wsm->len &= 0x7FFF;
		start = (u8 *) wsm;
		end = start + wsm->len;
		*next_len = *end | (*(end + 1) << 8);
	}

	hw_priv->rcvbuf_offset += round_up(wsm->len + 2, RK960_SDIO_BLOCK_SIZE);	// last 2 bytes for save next msgs info
	return (u8 *) wsm;
}
#endif

#if USE_SDIO_TX_BURST_MULTI
static int hwbus_sndbuf_send(struct rk960_common *hw_priv)
{
	if (hw_priv->sndbuf_offset) {
		//pr_info("%s: %d(%d)\n", __func__, hw_priv->sndbuf_offset,
		//                      hw_priv->sndbuf_offset/RK960_SDIO_TX_MSG_SIZE);
		if (WARN_ON(rk960_data_write(hw_priv,
					     hw_priv->hwbus_sndbuf,
					     hw_priv->sndbuf_offset))) {
			return -1;
		}
		hw_priv->sndbuf_offset = 0;
	}
	return 0;
}
#endif

#ifdef RK960_USE_RX_THREAD
static int rk960_rx(void *arg)
{
	struct rk960_common *hw_priv = arg;
	long status;
	int rx, term;

	for (;;) {
		status = wait_event_interruptible_timeout(hw_priv->bh_rx_wq, ( {
				rx = atomic_xchg(&hw_priv->bh_rx_rx, 0);
				term = atomic_xchg(&hw_priv->bh_rx_term, 0);
				(rx || term);
			}), 1 * HZ);

		if (status < 0 || term) {
			break;
		}

		for (;;) {
			struct sk_buff *skb_rx;
			struct wsm_hdr *wsm;
			int wsm_id;

			//if (skb_peek(&hw_priv->rxq) == NULL)
			//    break;

			skb_rx = skb_dequeue(&hw_priv->rxq);
			if (skb_rx == NULL) {
				break;
			}

			wsm = (struct wsm_hdr *)skb_rx->data;
			wsm_id = __le32_to_cpu(wsm->id) & 0xFFF;
			if (WARN_ON(wsm_handle_rx(hw_priv, wsm_id, wsm,
						  &skb_rx)))
				break;

			if (wsm_id & 0x0400) {
				int rc = wsm_release_tx_buffer(hw_priv, 1);
				if (WARN_ON(rc < 0))
					break;
			}

			rk960_put_skb(hw_priv, skb_rx);
		}
	}

	RK960_INFO_BH("%s exit\n", __func__);
	return 0;
}
#endif

static bool rk960_is_tx_empty(struct rk960_common *hw_priv)
{
	bool have_cmd = hw_priv->wsm_cmd.ptr != NULL;
	bool empty0, empty1;

	empty0 = rk960_queue_stats_is_empty(&hw_priv->tx_queue_stats, -1, 0);
	empty1 = rk960_queue_stats_is_empty(&hw_priv->tx_queue_stats, -1, 1);
	return !have_cmd && empty0 && empty1;
}

int TxReqIdx = 0;
//#define INTERRUPT_WORKAROUND
static int rk960_bh(void *arg)
{
	struct rk960_common *hw_priv = arg;
	//struct rk960_vif *priv = NULL;
	struct sk_buff *skb_rx = NULL;
	int read_len = 0;
	int rx, tx, term, suspend;
	struct wsm_hdr *wsm;
	int wsm_len;
	int wsm_id;
	u8 wsm_seq;
	int rx_resync = 1;
	u16 ctrl_reg = 0;
	int rx_con_need = 0;
	int tx_allowed;
	int pending_tx = 0;
	int tx_burst;
	int rx_burst = 0;
	//int rx_burst_con = 0;
	int clr_irq;
	long status;
#if defined(CONFIG_RK960_WSM_DUMPS)
	size_t wsm_dump_max = -1;
#endif
//      u32 dummy;
	bool powersave_enabled;
	//int i;
	int vif_selected;
#ifdef RK960_SDIO_RX_TP_TEST
	int total = 0;
	INIT_GET_SPEND_TIME(start_time, end_time);
#endif
#ifdef RK960_SDIO_TX_TP_TEST
	int sdio_tx_len = 2048;
	int total = 0;
	u8 *tx_data;
	struct wsm_tx *txwsm;
	INIT_GET_SPEND_TIME(start_time, end_time);

	tx_data = kmalloc(sdio_tx_len, GFP_KERNEL);
	if (tx_data) {
		memset(tx_data, 0x5a, sdio_tx_len);
		txwsm = (struct wsm_tx *)tx_data;
		txwsm->hdr.len = __cpu_to_le16(sdio_tx_len);
		txwsm->hdr.id = __cpu_to_le16(0x0004);
	}
#endif

	for (;;) {
		powersave_enabled = 1;
		/*rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
		   //rk960_for_each_vif(hw_priv, priv, i) {
		   for (i = 0; i < RK960_MAX_VIFS - 1; i++) {
		   priv = rk960_get_vif_from_ieee80211_hw_priv(hw_priv, i); 
		   #ifdef P2P_MULTIVIF
		   if ((i = (RK960_MAX_VIFS - 1)) || !priv)
		   #else
		   if (!priv)
		   #endif
		   continue;
		   powersave_enabled &= !!priv->powersave_enabled;
		   }
		   rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock); */
		if (!hw_priv->hw_bufs_used
		    && powersave_enabled
		    && !hw_priv->device_can_sleep
		    && !atomic_read(&hw_priv->recent_scan)
		    && !status) {
			status = 1 * HZ;
			RK960_DEBUG_BH("[BH] No Device wakedown.\n");
			rk960_device_wakeup(hw_priv, 0);
			hw_priv->device_can_sleep = true;
		} else if (hw_priv->hw_bufs_used)
			/* Interrupt loss detection */
			status = 1 * HZ;
		else
			status = 1 * HZ;	//MAX_SCHEDULE_TIMEOUT;

#if 0
		/* Dummy Read for SDIO retry mechanism */
		if (((atomic_read(&hw_priv->bh_rx) == 0) &&
		     (atomic_read(&hw_priv->bh_tx) == 0)))
			rk960_reg_read(hw_priv, ST90TDS_CONFIG_REG_ID,
				       &dummy, sizeof(dummy));
#endif
#if defined(CONFIG_RK960_WSM_DUMPS_SHORT)
		wsm_dump_max = hw_priv->wsm_dump_max_size;
#endif /* CONFIG_RK960_WSM_DUMPS_SHORT */

#ifdef INTERRUPT_WORKAROUND
		/* If a packet has already been txed to the device then read the 
		   control register for a probable interrupt miss before going
		   further to wait for interrupt; if the read length is non-zero
		   then it means there is some data to be received */
		if (hw_priv->hw_bufs_used) {
			rk960_bh_read_ctrl_reg(hw_priv, &ctrl_reg);
			if (ctrl_reg & ST90TDS_CONT_NEXT_LEN_MASK) {
				rx = 1;
				goto test;
			}
		}
#endif
		//RK960_DEBUG_BH("[BH] waiting ... %lx\n", status);
		status = wait_event_interruptible_timeout(hw_priv->bh_wq, ( {
				rx = atomic_xchg(&hw_priv->bh_rx, 0);
				tx = atomic_xchg(&hw_priv->bh_tx, 0);
				term = atomic_xchg(&hw_priv->bh_term, 0);
				suspend = (pending_tx || rx) ?
					0 : atomic_read(&hw_priv->bh_suspend);
				(rx || tx || term || suspend || hw_priv->bh_error);
			}), status);

#if 0
		/* force suspend = 0 */
		suspend = 0;
#endif

		//RK960_DEBUG_BH("[BH] - rx: %d, tx: %d(%d), term: %d, bh_err: %d, suspend: %d, status: %lx\n",
		//       rx, tx, pending_tx, term, hw_priv->bh_error, suspend, status);
		if (status < 0 || term || hw_priv->bh_error) {
			RK960_ERROR_BH
			    ("%s break: status %d term %d bh_error %d\n",
			     __func__, (int)status, term, hw_priv->bh_error);
			break;
		}
#ifdef INTERRUPT_WORKAROUND
		if (!status) {
			rk960_bh_read_ctrl_reg(hw_priv, &ctrl_reg);
			if (ctrl_reg & ST90TDS_CONT_NEXT_LEN_MASK) {
				RK960_ERROR_BH("MISS 1\n");
				rx = 1;
				goto test;
			}
		}
#endif
#if (defined RK960_SDIO_TX_TP_TEST) || (defined RK960_SDIO_RX_TP_TEST)
		status = 1;
#endif

#ifdef RK960_FW_ERROR_RECOVERY
		if (!hw_priv->fw_error_processing &&
		    rk960_queue_tx_without_conf(hw_priv, -1))
			rk960_signal_fw_error(hw_priv,
					      RK960_FWERR_REASON_TX_CONF);
#endif

		if (!status && hw_priv->hw_bufs_used) {
#if 1				//def RK960_FW_ERROR_RECOVERY
			//rk960_signal_fw_error(hw_priv, RK960_FWERR_REASON_TX_CONF);
#else
			unsigned long timestamp = jiffies;
			long timeout;
			bool pending = false;
			int i;

			RK960_ERROR_BH("Missed interrupt?\n");
			rx = 1;

			__rk960_jtag_enable(hw_priv);

			/* Get a timestamp of "oldest" frame */
			for (i = 0; i < 4; ++i)
				pending |=
				    rk960_queue_get_xmit_timestamp(&hw_priv->
								   tx_queue[i],
								   &timestamp,
								   -1,
								   hw_priv->
								   pending_frame_id);
			if (pending)
				rk960_queue_dump_pending_tx(hw_priv, -1);

			/* Check if frame transmission is timed out.
			 * Add an extra second with respect to possible
			 * interrupt loss. */
			timeout = timestamp +
			    WSM_CMD_LAST_CHANCE_TIMEOUT + 1 * HZ - jiffies;

			/* And terminate BH tread if the frame is "stuck" */
			if (pending && timeout < 0) {
				RK960_ERROR_BH
				    ("rk960_bh: Timeout waiting for TX confirm.\n");
				break;
			}
			BUG_ON(1);
#endif
		} else if (!status) {
			if (!hw_priv->device_can_sleep
			    && !atomic_read(&hw_priv->recent_scan)) {
				RK960_DEBUG_BH
				    ("[BH] Device wakedown. Timeout.\n");
				rk960_device_wakeup(hw_priv, 0);
				hw_priv->device_can_sleep = true;
			}
			continue;
		} else if (suspend && !tx) {
			RK960_DEBUG_BH("[BH] Device suspend.\n");
			powersave_enabled = 1;
			/*rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
			   //rk960_for_each_vif(hw_priv, priv, i) {
			   for (i = 0; i < RK960_MAX_VIFS; i++) {
			   priv = rk960_get_vif_from_ieee80211_hw_priv(hw_priv, i); 
			   #ifdef P2P_MULTIVIF
			   if ((i = (RK960_MAX_VIFS - 1)) || !priv)
			   #else
			   if (!priv)
			   #endif
			   continue;
			   powersave_enabled &= !!priv->powersave_enabled;
			   }
			   rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock); */
			if (powersave_enabled) {
				RK960_DEBUG_BH
				    ("[BH] No Device wakedown. Suspend.\n");
				rk960_device_wakeup(hw_priv, 0);
				hw_priv->device_can_sleep = true;
			}

			atomic_set(&hw_priv->bh_suspend, RK960_BH_SUSPENDED);
			wake_up(&hw_priv->bh_evt_wq);
			status = wait_event_interruptible(hw_priv->bh_wq,
							  RK960_BH_RESUME ==
							  atomic_read(&hw_priv->
								      bh_suspend));
			if (status < 0) {
				RK960_ERROR_BH
				    ("%s: Failed to wait for resume: %ld.\n",
				     __func__, status);
				break;
			}
			RK960_DEBUG_BH("[BH] Device resume.\n");
			atomic_set(&hw_priv->bh_suspend, RK960_BH_RESUMED);
			wake_up(&hw_priv->bh_evt_wq);
			//atomic_add(1, &hw_priv->bh_rx);
			if (atomic_read(&hw_priv->bh_rx))
				msleep(100);
			continue;
		}
//test:
		tx += pending_tx;
		pending_tx = 0;

		if (rx) {
			size_t alloc_len;
			u8 *data;

#ifdef INTERRUPT_WORKAROUND
			if (!(ctrl_reg & ST90TDS_CONT_NEXT_LEN_MASK))
#endif
				if (WARN_ON
				    (rk960_bh_read_ctrl_reg
				     (hw_priv, &ctrl_reg)))
					break;
			clr_irq = 1;
			//pr_info("ctrl_reg %d\n", ctrl_reg);
rx:
			read_len = ctrl_reg;
			ctrl_reg = 0;
			if (!read_len) {
				rx_burst = 0;
				goto tx;
			}

			if ( /*WARN_ON */
			    ((read_len < sizeof(struct wsm_hdr)) ||
#if USE_SDIO_RX_BURST_MULTI
			     (read_len > EFFECTIVE_RX_BUF_SIZE) ||
			     (read_len & (RK960_SDIO_BLOCK_SIZE - 1)))) {
#else
			     (read_len > EFFECTIVE_BUF_SIZE))) {
#endif
				RK960_ERROR_BH("Invalid read len: %d",
					       read_len);
				continue;
			}

#if USE_SDIO_RX_BURST_MULTI
			hwbus_rcvbuf_init(hw_priv);

			if (clr_irq)
				__rk960_clear_irq(hw_priv);
			clr_irq = 0;
			if (WARN_ON
			    (rk960_data_read
			     (hw_priv, hw_priv->hwbus_rcvbuf, read_len))) {
#ifdef RK960_FW_ERROR_RECOVERY
				rk960_signal_fw_error(hw_priv,
						      RK960_FWERR_REASON_SDIO);
				continue;
#else
				break;
#endif
			}

rx_con:
			rx_con_need = 0;
			wsm =
			    (struct wsm_hdr *)hwbus_rcvbuf_next_msg(hw_priv,
								    &ctrl_reg);
			if (!wsm)
				goto tx;

			read_len = round_up(wsm->len, 4);
			//pr_info("read_len %d ctrl_reg %d len %d\n", read_len, ctrl_reg, wsm->len);
#endif
			alloc_len =
			    hw_priv->hwbus_ops->align_size(hw_priv->hwbus_priv,
							   read_len);

			/* Check if not exceeding RK960 capabilities */
			if (WARN_ON_ONCE(alloc_len > EFFECTIVE_BUF_SIZE)) {
				RK960_ERROR_BH("Read aligned len: %d\n",
					       (int)alloc_len);
                                continue;
			}

			skb_rx = rk960_get_skb(hw_priv, alloc_len);
			if (WARN_ON(!skb_rx))
				break;

			skb_trim(skb_rx, 0);
			skb_put(skb_rx, read_len);
			data = skb_rx->data;
			if (WARN_ON(!data))
				break;

#if USE_SDIO_RX_BURST_MULTI
			memcpy(data, wsm, read_len);
#else
			if (clr_irq)
				__rk960_clear_irq(hw_priv);
			clr_irq = 0;
			if (WARN_ON(rk960_data_read(hw_priv, data, alloc_len)))
				break;
#endif
#if USE_SDIO_RX_BURST
			// get the next rx msg length (last 4 bytes)
			ctrl_reg = *(u16 *) (data + alloc_len - 4);
#if 0
			if (!ctrl_reg && rx_burst_con)
				RK960_DEBUG_BH("rx continue burst %d\n",
					       rx_burst_con);
			if (ctrl_reg > 0)
				rx_burst_con++;
			else
				rx_burst_con = 0;
#endif
#endif

			wsm = (struct wsm_hdr *)data;
			wsm_len = __le32_to_cpu(wsm->len);
			if (WARN_ON(wsm_len > read_len))
				break;

#if defined(CONFIG_RK960_WSM_DUMPS)
			if (unlikely(hw_priv->wsm_enable_wsm_dumps)) {
				u16 msgid, ifid;
				u16 *p = (u16 *) data;
				msgid = (*(p + 1)) & 0xC3F;
				ifid = (*(p + 1)) >> 6;
				ifid &= 0xF;
				printk(KERN_DEBUG
				       "[DUMP] <<< msgid 0x%.4X ifid %d len %d\n",
				       msgid, ifid, *p);
				print_hex_dump_bytes("<-- ", DUMP_PREFIX_NONE,
						     data, min(wsm_len,
							       wsm_dump_max));
			}
#endif /* CONFIG_RK960_WSM_DUMPS */

			wsm_id = __le32_to_cpu(wsm->id) & 0xFFF;
			wsm_seq = (__le32_to_cpu(wsm->id) >> 13) & 7;

			skb_trim(skb_rx, wsm_len);

			if (unlikely(wsm_id == 0x0800)) {
				wsm_handle_exception(hw_priv,
						     &data[sizeof(*wsm)],
						     wsm_len - sizeof(*wsm));
				break;
			} else if (unlikely(!rx_resync)) {
#if (!defined RK960_SDIO_TX_TP_TEST) && (!defined RK960_SDIO_RX_TP_TEST)
#ifndef SUPPORT_FWCR
				if (WARN_ON(wsm_seq != hw_priv->wsm_rx_seq)) {
					BUG_ON(1);
					break;
				}
#endif
#endif
			}
			hw_priv->wsm_rx_seq = (wsm_seq + 1) & 7;
			rx_resync = 0;

#ifdef RK960_SDIO_RX_TP_TEST
			if (wsm_id == 0x0804) {
				if (!total) {
					RK960_INFO_BH("start rx\n");
					START_GET_SPEND_TIME(start_time,
							     end_time);
				}
				total += wsm_len;
				if (total >= CALCULATE_RATE_LENGTH) {
					int spend_time;

					END_GET_SPEND_TIME(start_time,
							   end_time);
					spend_time =
					    GET_SPEND_TIME_US(start_time,
							      end_time) / 1000;
					RK960_INFO_BH
					    ("received %d Mbytes (every pkgs len = %d) use %d ms\n",
					     total / 1024 / 1024, wsm_len,
					     spend_time);
					total = 0;
				}
			} else
#endif

#ifdef RK960_USE_RX_THREAD
				skb_queue_tail(&hw_priv->rxq, skb_rx);
			if (atomic_add_return(1, &hw_priv->bh_rx_rx) == 1)
				wake_up(&hw_priv->bh_rx_wq);
#else
				/* rk960_wsm_rx takes care on SKB livetime */
			if (WARN_ON(wsm_handle_rx(hw_priv, wsm_id, wsm,
							  &skb_rx)))
				break;

			if (wsm_id & 0x0400) {
				int rc = wsm_release_tx_buffer(hw_priv, 1);
				if (WARN_ON(rc < 0))
					break;
				else if (rc > 0)
					tx = 1;
			}

			if (skb_rx) {
				rk960_put_skb(hw_priv, skb_rx);
				skb_rx = NULL;
			}
#endif
#if USE_SDIO_RX_BURST_MULTI
			if (!ctrl_reg) {
#if USE_SDIO_RX_CONTINUE
				goto rx_con;
#else
				rx_con_need = 1;
				goto tx;
#endif
			}
#endif
			read_len = 0;

			if (rx_burst) {
				rk960_debug_rx_burst(hw_priv);
				--rx_burst;
				goto rx;
			}
#if USE_SDIO_RX_CONTINUE
			if (ctrl_reg & ST90TDS_CONT_NEXT_LEN_MASK)
				goto rx;
#endif
		}

tx:
		BUG_ON(hw_priv->hw_bufs_used > hw_priv->wsm_caps.numInpChBufs);
		tx_burst = hw_priv->wsm_caps.numInpChBufs -
		    hw_priv->hw_bufs_used;
		tx_allowed = tx_burst > 0;

#ifdef RK960_SDIO_TX_TP_TEST
		if (hw_priv->sdio_tx_tp_start) {
			if (tx_burst > 0) {
				wsm_alloc_tx_buffer(hw_priv);
				if (!total) {
					RK960_INFO_BH("start send\n");
					START_GET_SPEND_TIME(start_time,
							     end_time);
				}
				sdio_tx_len = hw_priv->wsm_caps.sizeInpChBuf;
				txwsm->hdr.len = __cpu_to_le16(sdio_tx_len);
				//RK960_INFO_WSM("[WSM] >>> WSM_TX_REQ 0x%.4X (%d)\n",
				//                0x0004, sdio_tx_len);
				//gpio_direction_output(50, 1);
				rk960_data_write(hw_priv, (void *)tx_data,
						 sdio_tx_len);
				//gpio_direction_output(50, 0);
				sdio_tx_len = 1632;
				total += sdio_tx_len;
				if (total >= CALCULATE_RATE_LENGTH) {
					int spend_time;

					END_GET_SPEND_TIME(start_time,
							   end_time);
					spend_time =
					    GET_SPEND_TIME_US(start_time,
							      end_time) / 1000;
					RK960_INFO_BH
					    ("sended %d Mbytes (every pkgs len = %d) use %d ms\n",
					     total / 1024 / 1024, sdio_tx_len,
					     spend_time);
					total = 0;
				}
				goto tx;
			}
			tx_allowed = 0;
		}
#endif

#if 0
		/* Not support multi commands concurrent now */
		if (hw_priv->hw_bufs_used) {
			pending_tx = 1;
			RK960_DEBUG_BH("[BH] tx pending, waiting ...\n");
			continue;
		}
#endif

		if (tx && tx_allowed) {
			size_t tx_len;
			u8 *data;
			int ret;

			if (hw_priv->device_can_sleep &&
			    (ctrl_reg & ST90TDS_CONT_NEXT_LEN_MASK)) {
#if USE_SDIO_TX_BURST_MULTI
				BUG_ON(hw_priv->sndbuf_offset != 0);
#endif
				goto rx;
			}

			if (hw_priv->device_can_sleep &&
			    !rk960_is_tx_empty(hw_priv)) {
				ret = rk960_device_wakeup(hw_priv, 1);
				if (WARN_ON(ret < 0))
					break;
				else if (ret)
					hw_priv->device_can_sleep = false;
				else {
#if USE_SDIO_TX_BURST_MULTI
					BUG_ON(hw_priv->sndbuf_offset != 0);
#endif
					/* Wait for "awake" interrupt */
					pending_tx = tx;
					if (rx_con_need) {
						rx_con_need = 0;
						goto rx_con;
					}
					continue;
				}
			}

			wsm_alloc_tx_buffer(hw_priv);
			ret = wsm_get_tx(hw_priv, &data, &tx_len, &tx_burst,
					 &vif_selected);
			if (ret <= 0) {
				if (hw_priv->hw_bufs_used >=
				    hw_priv->wsm_caps.numInpChBufs)
					pending_tx = tx;
				wsm_release_tx_buffer(hw_priv, 1);
				if (WARN_ON(ret < 0))
					break;
			} else {
				wsm = (struct wsm_hdr *)data;
				BUG_ON(tx_len < sizeof(*wsm));
				BUG_ON(__le32_to_cpu(wsm->len) != tx_len);

#if 0				/* count is not implemented */
				if (ret > 1)
					atomic_add(1, &hw_priv->bh_tx);
#else
				atomic_add(1, &hw_priv->bh_tx);
#endif

				tx_len =
				    hw_priv->hwbus_ops->align_size(hw_priv->
								   hwbus_priv,
								   tx_len);

				/* Check if not exceeding RK960
				   capabilities */
				if (WARN_ON_ONCE
				    (tx_len > RK960_SDIO_TX_MSG_SIZE)) {
					RK960_ERROR_BH("Write aligned len:"
						       " %d\n", (int)tx_len);
				}

				wsm->id &=
				    __cpu_to_le32(~WSM_TX_SEQ(WSM_TX_SEQ_MAX));
				wsm->id |=
				    cpu_to_le32(WSM_TX_SEQ
						(hw_priv->wsm_tx_seq));

#if USE_SDIO_TX_BURST_MULTI
				if ((wsm->id & 0x3F) == 0x0004 &&
				    tx_len == RK960_SDIO_TX_MSG_SIZE &&
				    hw_priv->sndbuf_offset <
				    EFFECTIVE_TX_BUF_SIZE) {
					memcpy(hw_priv->hwbus_sndbuf +
					       hw_priv->sndbuf_offset, data,
					       tx_len);
					hw_priv->sndbuf_offset +=
					    RK960_SDIO_TX_MSG_SIZE;
					if (hw_priv->sndbuf_offset ==
					    EFFECTIVE_TX_BUF_SIZE)
						hwbus_sndbuf_send(hw_priv);
				} else {
					hwbus_sndbuf_send(hw_priv);
#endif
					if (WARN_ON(rk960_data_write(hw_priv,
								     data,
								     tx_len))) {
#ifdef RK960_FW_ERROR_RECOVERY
						if (vif_selected != -1)
							hw_priv->
							    hw_bufs_used_vif
							    [vif_selected]++;
						wsm_release_tx_buffer(hw_priv,
								      1);
						rk960_signal_fw_error(hw_priv,
								      RK960_FWERR_REASON_SDIO);
						continue;
#else
						wsm_release_tx_buffer(hw_priv,
								      1);
						break;
#endif
					}
#if USE_SDIO_TX_BURST_MULTI
				}
#endif

				if (vif_selected != -1) {
#ifdef RK960_USE_RX_THREAD
					spin_lock_bh(&hw_priv->
						     hw_bufs_used_lock);
#endif
					hw_priv->
					    hw_bufs_used_vif[vif_selected]++;
#ifdef RK960_USE_RX_THREAD
					spin_unlock_bh(&hw_priv->
						       hw_bufs_used_lock);
#endif
				}
#if defined(CONFIG_RK960_WSM_DUMPS)
				if (unlikely(hw_priv->wsm_enable_wsm_dumps)) {
					u16 msgid, ifid;
					u16 *p = (u16 *) data;
					msgid = (*(p + 1)) & 0x3F;
					ifid = (*(p + 1)) >> 6;
					ifid &= 0xF;
					if (msgid == 0x0006) {
						printk(KERN_DEBUG "[DUMP] >>> "
						       "msgid 0x%.4X "
						       "ifid %d len %d"
						       " MIB 0x%.4X\n",
						       msgid, ifid,
						       *p, *(p + 2));
					} else {
						printk(KERN_DEBUG "[DUMP] >>> "
						       "msgid 0x%.4X "
						       "ifid %d "
						       "len %d\n",
						       msgid, ifid, *p);
					}
					print_hex_dump_bytes("--> ",
							     DUMP_PREFIX_NONE,
							     data,
							     min(__le32_to_cpu
								 (wsm->len),
								 wsm_dump_max));
				}
#endif /* CONFIG_RK960_WSM_DUMPS */

				wsm_txed(hw_priv, data);
				hw_priv->wsm_tx_seq = (hw_priv->wsm_tx_seq + 1)
				    & WSM_TX_SEQ_MAX;

				if (tx_burst > 1) {
					rk960_debug_tx_burst(hw_priv);
					//++rx_burst;
					goto tx;
				}
			}
		}
#if USE_SDIO_TX_BURST_MULTI
		hwbus_sndbuf_send(hw_priv);
#endif
		if (ctrl_reg & ST90TDS_CONT_NEXT_LEN_MASK)
			goto rx;
		if (rx_con_need) {
			rx_con_need = 0;
			goto rx_con;
		}
	}

#ifndef RK960_USE_RX_THREAD
	if (skb_rx) {
		rk960_put_skb(hw_priv, skb_rx);
		skb_rx = NULL;
	}
#endif

	if (!term) {
		RK960_ERROR_BH("[BH] Fatal error, exitting.\n");
		__rk960_jtag_enable(hw_priv);
		BUG_ON(1);
		hw_priv->bh_error = 1;
#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
		rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
		//rk960_for_each_vif(hw_priv, priv, i) {
		for (i = 0; i < RK960_MAX_VIFS; i++) {
			priv = rk960_get_vif_from_ieee80211_hw_priv(hw_priv, i);
			if (!priv)
				continue;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
			RK960_ERROR_BH("%s: ieee80211_driver_hang_notify\n",
				       __func__);
#else
			ieee80211_driver_hang_notify(priv->vif, GFP_KERNEL);
#endif
		}
		rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);
		rk960_pm_stay_awake(&hw_priv->pm_state, 3 * HZ);
#endif
		/* TODO: schedule_work(recovery) */
#ifndef HAS_PUT_TASK_STRUCT
		/* The only reason of having this stupid code here is
		 * that __put_task_struct is not exported by kernel. */
		for (;;) {
			int status = wait_event_interruptible(hw_priv->bh_wq, ({
				term = atomic_xchg(&hw_priv->bh_term, 0);
				(term);
				}));

			if (status || term)
				break;
		}
#endif
	}
	return 0;
}
