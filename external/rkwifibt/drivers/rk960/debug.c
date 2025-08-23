/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*Linux version 3.4.0 compilation*/
//#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0))
#include<linux/module.h>
//#endif
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/ieee80211.h>
#include "rk960.h"
#include "debug.h"
#include "scan.h"

int rk960_debug_flag =
	DEBUG_BH	|
	DEBUG_IO	|
	DEBUG_FW	|
	DEBUG_MAIN	|
	DEBUG_PM	|
	DEBUG_QUEUE	|
	DEBUG_SCAN	|
	DEBUG_STA	|
	DEBUG_TXRX	|
	DEBUG_WSM	|
	DEBUG_AP	|
	DEBUG_TXPOLICY  |
	DEBUG_FW_REC;

static int debug_level =
	DEBUG_LEVEL_ERROR;
module_param(debug_level, int, 0644);
MODULE_PARM_DESC(debug_level, "debug level");

static int max_payload = 2304;
module_param(max_payload, int, 0644);
MODULE_PARM_DESC(max_payload, "max payload size");

int rk960_debug_level;

void rk960_debug_level_init(void)
{
	rk960_debug_level = debug_level;
	//pr_info("%s: rk960_debug_level %d\n", __func__, rk960_debug_level);
}

#define MAX_WRITE_STR_LEN 128

/* join_status */
static const char *const rk960_debug_join_status[] = {
	"passive",
	"monitor",
	"station",
	"access point",
};

/* WSM_JOIN_PREAMBLE_... */
static const char *const rk960_debug_preamble[] = {
	"long",
	"short",
	"long on 1 and 2 Mbps",
};

static const char *const rk960_debug_fw_types[] = {
	"ETF",
	"WFM",
	"WSM",
	"HI test",
	"Platform test",
};

static const char *const rk960_debug_link_id[] = {
	"OFF",
	"REQ",
	"SOFT",
	"HARD",
};

int rk960_rate_to_rate_index(int rate)
{
        int rate_index = -EINVAL;
        
	switch (rate) {
	case 10:
		rate_index = 0;
		break;
	case 20:
		rate_index = 1;
		break;
	case 55:
		rate_index = 2;
		break;
	case 110:
		rate_index = 3;
		break;
	case 60:
		rate_index = 6;
		break;
	case 90:
		rate_index = 7;
		break;
	case 120:
		rate_index = 8;
		break;
	case 180:
		rate_index = 9;
		break;
	case 240:
		rate_index = 10;
		break;
	case 360:
		rate_index = 11;
		break;
	case 480:
		rate_index = 12;
		break;
	case 540:
		rate_index = 13;
		break;
	case 65:
		rate_index = 14;
		break;
	case 130:
		rate_index = 15;
		break;
	case 195:
		rate_index = 16;
		break;
	case 260:
		rate_index = 17;
		break;
	case 390:
		rate_index = 18;
		break;
	case 520:
		rate_index = 19;
		break;
	case 585:
		rate_index = 20;
		break;
	case 650:
		rate_index = 21;
		break;
	default:
		//RK960_ERROR_IO("invalid rate %d\n", rate);
		return -EINVAL;
	}

        return rate_index;
}

static const char *rk960_debug_mode(int mode)
{
	switch (mode) {
	case NL80211_IFTYPE_UNSPECIFIED:
		return "unspecified";
	case NL80211_IFTYPE_MONITOR:
		return "monitor";
	case NL80211_IFTYPE_STATION:
		return "station";
	case NL80211_IFTYPE_ADHOC:
		return "ad-hok";
	case NL80211_IFTYPE_MESH_POINT:
		return "mesh point";
	case NL80211_IFTYPE_AP:
		return "access point";
	case NL80211_IFTYPE_P2P_CLIENT:
		return "p2p client";
	case NL80211_IFTYPE_P2P_GO:
		return "p2p go";
	default:
		return "unsupported";
	}
}

static void rk960_queue_status_show(struct seq_file *seq, struct rk960_queue *q)
{
	int i, if_id;
	seq_printf(seq, "Queue       %d:\n", q->queue_id);
	seq_printf(seq, "  capacity: %d\n", (int)q->capacity);
	seq_printf(seq, "  queued:   %d\n", (int)q->num_queued);
	seq_printf(seq, "  pending:  %d\n", (int)q->num_pending);
	seq_printf(seq, "  sent:     %d\n", (int)q->num_sent);
	seq_printf(seq, "  locked:   %s\n", q->tx_locked_cnt ? "yes" : "no");
	seq_printf(seq, "  overfull: %s\n", q->overfull ? "yes" : "no");
	seq_puts(seq, "  link map: 0-> ");
	for (if_id = 0; if_id < RK960_MAX_VIFS; if_id++) {
		for (i = 0; i < q->stats->map_capacity; ++i)
			seq_printf(seq, "%.2d ", q->link_map_cache[if_id][i]);
		seq_printf(seq, "<-%d\n", (int)q->stats->map_capacity);
	}
}

static void rk960_debug_print_map(struct seq_file *seq,
				  struct rk960_vif *priv,
				  const char *label, u32 map)
{
	int i;
	seq_printf(seq, "%s0-> ", label);
	for (i = 0; i < priv->hw_priv->tx_queue_stats.map_capacity; ++i)
		seq_printf(seq, "%s ", (map & BIT(i)) ? "**" : "..");
	seq_printf(seq, "<-%d\n",
		   (int)priv->hw_priv->tx_queue_stats.map_capacity - 1);
}

static int rk960_status_show_common(struct seq_file *seq, void *v)
{
	int i;
	struct list_head *item;
	struct rk960_common *hw_priv = seq->private;
	struct rk960_debug_common *d = hw_priv->debug;
	int ba_cnt, ba_acc, ba_cnt_rx, ba_acc_rx, ba_avg = 0, ba_avg_rx = 0;
	bool ba_ena;
#ifdef RK960_CSYNC_ADJUST
	struct rk960_vif *priv;
	struct csync_params *csync = &hw_priv->csync_params;
#endif
	spin_lock_bh(&hw_priv->ba_lock);
	ba_cnt = hw_priv->debug->ba_cnt;
	ba_acc = hw_priv->debug->ba_acc;
	ba_cnt_rx = hw_priv->debug->ba_cnt_rx;
	ba_acc_rx = hw_priv->debug->ba_acc_rx;
	ba_ena = hw_priv->ba_ena;
	if (ba_cnt)
		ba_avg = ba_acc / ba_cnt;
	if (ba_cnt_rx)
		ba_avg_rx = ba_acc_rx / ba_cnt_rx;
	spin_unlock_bh(&hw_priv->ba_lock);

	seq_puts(seq, "RK960 Wireless LAN driver status\n");
	seq_printf(seq, "Hardware:   %d.%d\n",
		   hw_priv->wsm_caps.hardwareId,
		   hw_priv->wsm_caps.hardwareSubId);
	seq_printf(seq, "Firmware:   %s %d.%d\n",
		   rk960_debug_fw_types[hw_priv->wsm_caps.firmwareType],
		   hw_priv->wsm_caps.firmwareVersion,
		   hw_priv->wsm_caps.firmwareBuildNumber);
	seq_printf(seq, "FW API:     %d\n", hw_priv->wsm_caps.firmwareApiVer);
	seq_printf(seq, "FW caps:    0x%.4X\n", hw_priv->wsm_caps.firmwareCap);
	if (hw_priv->channel)
		seq_printf(seq, "Channel:    %d%s\n",
			   hw_priv->channel->hw_value,
			   hw_priv->channel_switch_in_progress ?
			   " (switching)" : "");
	seq_printf(seq, "HT:         %s\n",
		   rk960_is_ht(&hw_priv->ht_info) ? "on" : "off");
	if (rk960_is_ht(&hw_priv->ht_info)) {
		seq_printf(seq, "Greenfield: %s\n",
			   rk960_ht_greenfield(&hw_priv->
					       ht_info) ? "yes" : "no");
		seq_printf(seq, "AMPDU dens: %d\n",
			   rk960_ht_ampdu_density(&hw_priv->ht_info));
	}
	spin_lock_bh(&hw_priv->tx_policy_cache.lock);
	i = 0;
	list_for_each(item, &hw_priv->tx_policy_cache.used)
	    ++ i;
	spin_unlock_bh(&hw_priv->tx_policy_cache.lock);
	seq_printf(seq, "RC in use:  %d\n", i);
	seq_printf(seq, "BA stat:    %d, %d (%d)\n", ba_cnt, ba_acc, ba_avg);
	seq_printf(seq, "BA RX stat:    %d, %d (%d)\n",
		   ba_cnt_rx, ba_acc_rx, ba_avg_rx);
	seq_printf(seq, "Block ACK:  %s\n", ba_ena ? "on" : "off");

	seq_puts(seq, "\n");
	for (i = 0; i < 4; ++i) {
		rk960_queue_status_show(seq, &hw_priv->tx_queue[i]);
		seq_puts(seq, "\n");
	}

	seq_printf(seq, "TX conf max(ms):   %d\n",
		   (int)hw_priv->ms_max_tx_conf_time);
	seq_printf(seq, "TX conf max queue_id:   %d\n",
		   hw_priv->max_tx_conf_queueid);
	seq_printf(seq, "TX conf max status:   %d\n",
		   hw_priv->max_tx_conf_status);
	seq_printf(seq, "TX conf max txedRate:   %d\n",
		   hw_priv->max_tx_conf_txedrate);
	seq_printf(seq, "TX conf max ackFailures:   %d\n",
		   hw_priv->max_tx_conf_ackfailures);
	seq_printf(seq, "TX conf max mediaDelay:   %d\n",
		   hw_priv->max_tx_conf_mediadelay);
	seq_printf(seq, "TX conf max txQueueDelay:   %d\n",
		   hw_priv->max_tx_conf_txQueuedelay);

	seq_printf(seq, "TX burst:   %d\n", d->tx_burst);
	seq_printf(seq, "RX burst:   %d\n", d->rx_burst);
	seq_printf(seq, "TX miss:    %d\n", d->tx_cache_miss);
	seq_printf(seq, "TX drops:   %d\n", d->tx_drops);
	seq_printf(seq, "Long retr:  %d\n", hw_priv->long_frame_max_tx_count);
	seq_printf(seq, "Short retr: %d\n", hw_priv->short_frame_max_tx_count);

	seq_printf(seq, "BH status:  %s\n",
		   atomic_read(&hw_priv->bh_term) ? "terminated" : "alive");
	seq_printf(seq, "Pending RX: %d\n", atomic_read(&hw_priv->bh_rx));
	seq_printf(seq, "Pending TX: %d\n", atomic_read(&hw_priv->bh_tx));
	if (hw_priv->bh_error)
		seq_printf(seq, "BH errcode: %d\n", hw_priv->bh_error);
	seq_printf(seq, "TX bufs:    %d x %d bytes\n",
		   hw_priv->wsm_caps.numInpChBufs,
		   hw_priv->wsm_caps.sizeInpChBuf);
	seq_printf(seq, "Used bufs:  %d\n", hw_priv->hw_bufs_used);
	seq_printf(seq, "Device:     %s\n",
		   hw_priv->device_can_sleep ? "alseep" : "awake");

	spin_lock(&hw_priv->wsm_cmd.lock);
	seq_printf(seq, "WSM status: %s\n",
		   hw_priv->wsm_cmd.done ? "idle" : "active");
	seq_printf(seq, "WSM cmd:    0x%.4X (%d bytes)\n",
		   hw_priv->wsm_cmd.cmd, (int)hw_priv->wsm_cmd.len);
	seq_printf(seq, "WSM retval: %d\n", hw_priv->wsm_cmd.ret);
	spin_unlock(&hw_priv->wsm_cmd.lock);

	seq_printf(seq, "Datapath:   %s\n",
		   atomic_read(&hw_priv->tx_lock) ? "locked" : "unlocked");
	if (atomic_read(&hw_priv->tx_lock))
		seq_printf(seq, "TXlock cnt: %d\n",
			   atomic_read(&hw_priv->tx_lock));

	seq_printf(seq, "Scan:       %s\n",
		   atomic_read(&hw_priv->scan.in_progress) ? "active" : "idle");
	seq_printf(seq, "Scan counter: %d %d\n",
		   hw_priv->scan_counter, hw_priv->scan_comp_counter);
	seq_printf(seq, "Led state:  0x%.2X\n", hw_priv->softled_state);

#ifdef RK960_CSYNC_ADJUST
	seq_printf(seq, "\ncsync enable(%d):\n", !csync->csync_disable);
	for (i = 0; i < 2; i++) {
		if (hw_priv->if_id_slot & BIT(i)) {
			priv =
			    rk960_get_vif_from_ieee80211(hw_priv->vif_list[i]);
			if (priv) {
				seq_printf(seq,
					   "idx(%d) vif_addr = %pM, bssid = %pM,"
					   " join_status = %d\n", i,
					   hw_priv->vif_list[i]->addr,
					   priv->join_bssid, priv->join_status);
				seq_printf(seq, "thresh_accum = %d\n",
					   csync->thresh_accum[i]);
				seq_printf(seq, "avg_thresh = %d\n",
					   csync->avg_thresh[i]);
				seq_printf(seq, "new_thresh = %d\n",
					   csync->new_thresh[i]);
			}
		}
	}
	seq_printf(seq, "cur_seted_thresh: %03d\n", csync->cur_seted_thresh);
	seq_printf(seq, "last_sam_size: %d %d\n", csync->last_sam_size[0],
		   csync->last_sam_size[1]);
	seq_printf(seq, "threld history:\n");
	seq_printf(seq, "offset %02d: ", csync->cur_thr_offset);
	for (i = 0; i < RK960_CSYNC_SETED_THRESH_COUNT; i++) {
		if (csync->cur_thr_offset == i + 1) {
			seq_printf(seq, "***");
		} else if (csync->cur_thr_offset == 0) {
			if (i == RK960_CSYNC_SETED_THRESH_COUNT - 1)
				seq_printf(seq, "***");
		}
		seq_printf(seq, "%03d ", csync->thr_history[i]);
	}
	seq_printf(seq, "\n");
	seq_printf(seq, "\ntimer hit %d (%d)\n",
		   csync->timer_hit, csync->set_hit);
	seq_printf(seq, "tx_counter %d in last 1s\n", hw_priv->last_tx_counter);
	seq_printf(seq, "rx_counter %d in last 1s\n", hw_priv->last_rx_counter);
#endif

	return 0;
}

static int rk960_status_open_common(struct inode *inode, struct file *file)
{
	return single_open(file, &rk960_status_show_common, inode->i_private);
}

static const struct file_operations fops_status_common = {
	.open = rk960_status_open_common,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.owner = THIS_MODULE,
};

static int rk960_counters_show(struct seq_file *seq, void *v)
{
	int ret;
	struct rk960_common *hw_priv = seq->private;
	struct wsm_counters_table counters;

	ret = wsm_get_counters_table(hw_priv, &counters);
	if (ret)
		return ret;

#define CAT_STR(x, y) x ## y
#define PUT_COUNTER(tab, name) \
	seq_printf(seq, "%s:" tab "%d\n", #name, \
		__le32_to_cpu(counters.CAT_STR(count, name)))

	PUT_COUNTER("\t\t", PlcpErrors);
	PUT_COUNTER("\t\t", FcsErrors);
	PUT_COUNTER("\t\t", TxPackets);
	PUT_COUNTER("\t\t", RxPackets);
	PUT_COUNTER("\t\t", RxPacketErrors);
	PUT_COUNTER("\t\t", RtsSuccess);
	PUT_COUNTER("\t\t", RtsFailures);
	PUT_COUNTER("\t\t", RxFramesSuccess);
	PUT_COUNTER("\t", RxDecryptionFailures);
	PUT_COUNTER("\t\t", RxMicFailures);
	PUT_COUNTER("\t", RxNoKeyFailures);
	PUT_COUNTER("\t", TxMulticastFrames);
	PUT_COUNTER("\t", TxFramesSuccess);
	PUT_COUNTER("\t", TxFrameFailures);
	PUT_COUNTER("\t", TxFramesRetried);
	PUT_COUNTER("\t", TxFramesMultiRetried);
	PUT_COUNTER("\t", RxFrameDuplicates);
	PUT_COUNTER("\t\t", AckFailures);
	PUT_COUNTER("\t", RxMulticastFrames);
	PUT_COUNTER("\t", RxCMACICVErrors);
	PUT_COUNTER("\t\t", RxCMACReplays);
	PUT_COUNTER("\t", RxMgmtCCMPReplays);
	PUT_COUNTER("\t", RxBIPMICErrors);

#undef PUT_COUNTER
#undef CAT_STR

	return 0;
}

static int rk960_counters_open(struct inode *inode, struct file *file)
{
	return single_open(file, &rk960_counters_show, inode->i_private);
}

static const struct file_operations fops_counters = {
	.open = rk960_counters_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.owner = THIS_MODULE,
};

static int rk960_generic_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

static ssize_t rk960_11n_read(struct file *file,
			      char __user * user_buf, size_t count,
			      loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	struct ieee80211_supported_band *band =
	    hw_priv->hw->wiphy->bands[IEEE80211_BAND_2GHZ];
	return simple_read_from_buffer(user_buf, count, ppos,
				       band->ht_cap.
				       ht_supported ? "1\n" : "0\n", 2);
}

static ssize_t rk960_11n_write(struct file *file,
			       const char __user * user_buf, size_t count,
			       loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	struct ieee80211_supported_band *band[2] = {
		hw_priv->hw->wiphy->bands[IEEE80211_BAND_2GHZ],
		hw_priv->hw->wiphy->bands[IEEE80211_BAND_5GHZ],
	};
	char buf[1];
	int ena = 0;

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, 1))
		return -EFAULT;
	if (buf[0] == 1)
		ena = 1;

	band[0]->ht_cap.ht_supported = ena;
#ifdef CONFIG_RK960_5GHZ_SUPPORT
	band[1]->ht_cap.ht_supported = ena;
#endif /* CONFIG_RK960_5GHZ_SUPPORT */

	return count;
}

static const struct file_operations fops_11n = {
	.open = rk960_generic_open,
	.read = rk960_11n_read,
	.write = rk960_11n_write,
	.llseek = default_llseek,
};

static ssize_t rk960_mem_read(struct file *file,
			      char __user * user_buf, size_t count,
			      loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	int i, size = 0;
	u32 *data;
	u32 addr;
	int ret = 0;

	if (!hw_priv->debug_dump_data)
		return 0;

	if (wait_event_interruptible_timeout(hw_priv->debug_dump_done,
					     hw_priv->debug_dump_ready,
					     3 * HZ) <= 0) {
		RK960_ERROR_IO("%s: wait debug_dump_ready failed\n", __func__);
		goto mem_read_exit;
	}

	hw_priv->debug_dump_data_str = vmalloc(1024 * 4);
	if (!hw_priv->debug_dump_data_str)
		goto mem_read_exit;

	hw_priv->mem_rw_len = round_up(hw_priv->mem_rw_len, 16);
	data = (u32 *) hw_priv->debug_dump_data;
	addr = hw_priv->mem_rw_addr;
	for (i = 0; i < hw_priv->mem_rw_len / 16; i++) {
		size +=
		    sprintf(&hw_priv->debug_dump_data_str[size],
			    "%08x: %08x %08x %08x %08x\n", addr, *data,
			    *(data + 1), *(data + 2), *(data + 3));
		addr += 16;
		data += 4;
	}

	ret = simple_read_from_buffer(user_buf, count, ppos,
				      hw_priv->debug_dump_data_str, size);

mem_read_exit:
	if (hw_priv->debug_dump_data) {
		vfree(hw_priv->debug_dump_data);
		hw_priv->debug_dump_data = NULL;
	}
	if (hw_priv->debug_dump_data_str) {
		vfree(hw_priv->debug_dump_data_str);
		hw_priv->debug_dump_data_str = NULL;
	}
	return ret;
}

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_mem_write(struct file *file,
			       const char __user * user_buf, size_t count,
			       loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[MAX_WRITE_STR_LEN];
	unsigned long addr, len;
	char *addr_s, *len_s;

	if (!count || count > MAX_WRITE_STR_LEN)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	// r=0x10001000,0x4
	// w=0x10001000,0x20002000
	if (buf[0] == 'r' || buf[0] == 'w') {
		addr_s = strstr(buf, "=");
		len_s = strstr(buf, ",");
		if (!addr_s || !len_s)
			return -EINVAL;
		*len_s = 0;
		buf[count] = 0;
		if (kstrtoul(addr_s + 1, 16, &addr))
			return -EINVAL;
		if (kstrtoul(len_s + 1, 16, &len))
			return -EINVAL;
		if (buf[0] == 'r') {
			struct wsm_mem_rw mem_rw = {
				.link_id = 0,
				.address = addr,
				.length = len,
				.flags = WSM_RW_F_IRQ_PROTECT,
			};

			if (len > 1024)
				len = 1024;

			RK960_INFO_IO("%s: r 0x%08x %d\n", __func__, (u32) addr,
				      (u32) len);

			hw_priv->mem_rw_addr = addr;
			hw_priv->mem_rw_len = len;
			hw_priv->debug_dump_ready = 0;
			if (hw_priv->debug_dump_data)
				vfree(hw_priv->debug_dump_data);
			hw_priv->debug_dump_data = vmalloc(1024);
			if (hw_priv->debug_dump_data) {
				memset(hw_priv->debug_dump_data, 0xAA, 1024);
				wsm_mem_rw(hw_priv, &mem_rw, 1, 0);
			}
		} else {
			struct wsm_mem_rw mem_rw = {
				.link_id = 0,
				.address = addr,
				.length = 4,
				.flags = WSM_RW_F_IRQ_PROTECT,
			};

			RK960_INFO_IO("%s: w 0x%08x 0x%08x\n", __func__,
				      (u32) addr, (u32) len);

			mem_rw.data[0] = len;
			wsm_mem_rw(hw_priv, &mem_rw, 0, 0);
		}
	} else {
		return -EINVAL;
	}

	return count;
}
#endif

static const struct file_operations fops_mem = {
	.open = rk960_generic_open,
	.read = rk960_mem_read,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_mem_write,
#endif
	.llseek = default_llseek,
};

#ifdef ENABLE_DBGFS_WRITE
#define RK960_ADC_SAVE_PATH "/data/rk960_adc.txt"
static ssize_t rk960_adc_read(struct file *file,
			      char __user * user_buf, size_t count,
			      loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	int i, size = 0;
	u32 *data;
	int ret = 0;

	if (!hw_priv->debug_dump_data)
		return 0;

	if (wait_event_interruptible_timeout(hw_priv->debug_dump_done,
					     hw_priv->debug_dump_ready,
					     3 * HZ) <= 0) {
		RK960_ERROR_IO("%s: wait debug_dump_ready failed\n", __func__);
		goto adc_read_exit;
	}

	hw_priv->debug_dump_data_str = vmalloc(hw_priv->debug_adc_size * 3);
	if (!hw_priv->debug_dump_data_str)
		goto adc_read_exit;

	data = (u32 *) hw_priv->debug_dump_data;
	for (i = 0; i < hw_priv->debug_adc_size / 4; i++) {
		size +=
		    sprintf(&hw_priv->debug_dump_data_str[size], "%08x\n",
			    *data);
		data++;
	}

	rk960_access_file(RK960_ADC_SAVE_PATH, hw_priv->debug_dump_data_str,
			  size, 0);

	size = sprintf(&hw_priv->debug_dump_data_str[0], "%s\n",
		       "adc data is saved at: " RK960_ADC_SAVE_PATH);
	ret = simple_read_from_buffer(user_buf, count, ppos,
				      hw_priv->debug_dump_data_str, size);

adc_read_exit:
	if (hw_priv->debug_dump_data) {
		vfree(hw_priv->debug_dump_data);
		hw_priv->debug_dump_data = NULL;
	}
	if (hw_priv->debug_dump_data_str) {
		vfree(hw_priv->debug_dump_data_str);
		hw_priv->debug_dump_data_str = NULL;
	}
	return ret;
}

static ssize_t rk960_adc_write(struct file *file,
			       const char __user * user_buf, size_t count,
			       loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[MAX_WRITE_STR_LEN];
	int adc_size;
	int channel;
	//int i;
	int adc_count;
	int abbf, lna;

	if (!count || count > MAX_WRITE_STR_LEN)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	// adc 1 10 65536
	if (strstr(buf, "adc") != NULL) {
		sscanf(buf + 4, "%d %d %d %d %d", &channel, &adc_count,
		       &adc_size, &abbf, &lna);

		if (channel > 14 || channel < 1) {
			RK960_ERROR_IO("invalid channel %d\n", channel);
			return -EINVAL;
		}
		if (abbf > 48 || abbf < 24) {
			RK960_ERROR_IO("invalid abbf %d\n", abbf);
			abbf = 0;
		}
		if (lna > 6 || lna < 0) {
			RK960_ERROR_IO("invalid lna %d\n", lna);
			lna = 0;
		}

		if (adc_count <= 0)
			adc_count = 1;

		if (adc_size > 65536)
			adc_size = 65536;
		if (adc_size < 25600)
			adc_size = 25600;

		RK960_INFO_IO
		    ("%s: adc: ch %d count %d size %d abbf %d lna %d\n",
		     __func__, channel, adc_count, adc_size, abbf, lna);

		adc_size <<= 2;	// I/Q data

		hw_priv->debug_adc_offset = 0;
		hw_priv->debug_adc_block = 1024;
		hw_priv->debug_adc_size = adc_size;
		hw_priv->debug_dump_ready = 0;

		if (hw_priv->debug_dump_data)
			vfree(hw_priv->debug_dump_data);
		hw_priv->debug_dump_data = vmalloc(adc_size);
		if (hw_priv->debug_dump_data) {
			struct wsm_adc_data_r adc_data_r;

			adc_data_r.size = adc_size;
			adc_data_r.block = hw_priv->debug_adc_block;
			adc_data_r.channel = channel;
			adc_data_r.count = adc_count;
			adc_data_r.abbf = abbf;
			adc_data_r.lna = lna;
			//for (i = 0; i < adc_size/hw_priv->debug_adc_block; i++) {
			adc_data_r.offset = 0 * hw_priv->debug_adc_block;
			wsm_adc_data_read(hw_priv, &adc_data_r, 0);
			//}
		}
	}

	return count;
}
#endif

static const struct file_operations fops_adc = {
	.open = rk960_generic_open,
#ifdef ENABLE_DBGFS_WRITE
	.read = rk960_adc_read,
	.write = rk960_adc_write,
#endif
	.llseek = default_llseek,
};

#ifdef ENABLE_DBGFS_WRITE
#define DEVICE_DEBUG_STRUCT_INFO_SAVE_PATH "/sdcard/rk960_device_debug_struct_info.bin"
static ssize_t rk960_device_debug_struct_info_read(struct file *file,
						   char __user * user_buf,
						   size_t count, loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	int size = 0;
	int ret = 0;

	if (!hw_priv->debug_dump_data)
		return 0;

	if (wait_event_interruptible_timeout(hw_priv->debug_dump_done,
					     hw_priv->debug_dump_ready,
					     3 * HZ) <= 0) {
		RK960_ERROR_IO("%s: wait debug_dump_ready failed\n", __func__);
		goto device_debug_read_exit;
	}

	RK960_INFO_MAIN("%s: read 0x%08x %d!!!\n", __func__, (u32) size,
			(u32) count);

	hw_priv->debug_dump_data_str = vmalloc(1024 * 4);
	if (!hw_priv->debug_dump_data_str)
		goto device_debug_read_exit;
	size = sprintf(&hw_priv->debug_dump_data_str[0], "%s\n",
		       "device_debug_struct_info data is saved at: "
		       DEVICE_DEBUG_STRUCT_INFO_SAVE_PATH);
	ret =
	    simple_read_from_buffer(user_buf, count, ppos,
				    hw_priv->debug_dump_data_str, size);
	//save data to file
	rk960_access_file(DEVICE_DEBUG_STRUCT_INFO_SAVE_PATH,
			  hw_priv->debug_dump_data, hw_priv->mem_rw_len, 0);

device_debug_read_exit:
	if (hw_priv->debug_dump_data) {
		vfree(hw_priv->debug_dump_data);
		hw_priv->debug_dump_data = NULL;
	}
	if (hw_priv->debug_dump_data_str) {
		vfree(hw_priv->debug_dump_data_str);
		hw_priv->debug_dump_data_str = NULL;
	}

	return ret;
}

static ssize_t rk960_device_debug_struct_info_write(struct file *file,
						    const char __user *
						    user_buf, size_t count,
						    loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[MAX_WRITE_STR_LEN];
	unsigned long addr, len;
	struct wsm_device_debug_struct_info info;
	struct wsm_mem_rw mem_rw;

	if (!count || count > MAX_WRITE_STR_LEN) {
		count = MAX_WRITE_STR_LEN;
	}
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;
	memset(&info, 0, sizeof(struct wsm_device_debug_struct_info));
	wsm_get_device_debug_struct_info(hw_priv, &info);
	addr = info.mem_addr;
	len = info.size;
	mem_rw.link_id = 0;
	mem_rw.address = addr;
	mem_rw.length = len;
	mem_rw.flags = WSM_RW_F_IRQ_PROTECT;

	if (len > 1024)
		len = 1024;

	RK960_INFO_MAIN("%s: read 0x%08x %d!!!\n", __func__, (u32) addr,
			(u32) len);

	hw_priv->mem_rw_addr = addr;
	hw_priv->mem_rw_len = len;
	hw_priv->debug_dump_ready = 0;
	if (hw_priv->debug_dump_data)
		vfree(hw_priv->debug_dump_data);
	hw_priv->debug_dump_data = vmalloc(1024);
	if (hw_priv->debug_dump_data) {
		memset(hw_priv->debug_dump_data, 0x0, 1024);
		wsm_mem_rw(hw_priv, &mem_rw, 1, 0);
	}

	return count;
}
#endif

static const struct file_operations fops_device_debug_struct_info = {
	.open = rk960_generic_open,
#ifdef ENABLE_DBGFS_WRITE
	.read = rk960_device_debug_struct_info_read,
	.write = rk960_device_debug_struct_info_write,
#endif
	.llseek = default_llseek,
};

static ssize_t rk960_rftest_read(struct file *file,
				 char __user * user_buf, size_t count,
				 loff_t * ppos)
{
	return 0;
}

#if 1//def ENABLE_DBGFS_WRITE
static ssize_t rk960_rftest_write(struct file *file,
				  const char __user * user_buf, size_t count,
				  loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[MAX_WRITE_STR_LEN];
	int channel, rate, rate_index, len;
	int times = 0;
	int sgi = 0;
	int tx_power;

	if (!count || count > MAX_WRITE_STR_LEN)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	// start_tx channel rate length
	if (strstr(buf, "start_tx") != NULL) {
		sscanf(buf + 9, "%d %d %d %d %d %d",
		       &channel, &rate, &tx_power, &len, &times, &sgi);

		if (channel > 14 || channel < 1) {
			RK960_ERROR_IO("invalid channel %d\n", channel);
			return -EINVAL;
		}

                rate_index = rk960_rate_to_rate_index(rate);

		if (len < 200 || len > max_payload) {
			RK960_ERROR_IO("invalid len %d[200-%d]\n", len,
				       max_payload);
			return -EINVAL;
		}
		if (rate_index < 14) {
			sgi = 0;
		}

		if (!tx_power)
			tx_power = RK960_DEFAULT_TX_POWER * 10;
		else if (tx_power > 20)
			tx_power = 20 * 16;	// 20db
		else
			tx_power *= 16;

		RK960_INFO_IO("%s: start_tx: channel:%d rate:%d tx_power %d "
			      "len:%d count:%d sgi:%d\n",
			      __func__, channel, rate, tx_power, len, times,
			      sgi);
		rk960_rftest_start_tx(hw_priv, channel, rate_index, tx_power,
				      len, times, sgi);
		// stop_txrx
	} else if (strstr(buf, "stop_txrx") != NULL) {
		RK960_INFO_IO("%s: stop_txrx\n", __func__);
		rk960_rftest_stop_txrx(hw_priv);
		// start_rx channel
	} else if (strstr(buf, "start_rx") != NULL) {
		sscanf(buf + 9, "%d", &channel);

		if (channel > 14 || channel < 1) {
			RK960_ERROR_IO("invalid channel %d\n", channel);
			return -EINVAL;
		}

		RK960_INFO_IO("%s: start_rx: channel:%d\n", __func__, channel);
		rk960_rftest_start_rx(hw_priv, channel);
		// start_tone channel
	} else if (strstr(buf, "start_tone") != NULL) {
		sscanf(buf + 11, "%d", &channel);

		if (channel > 14 || channel < 1) {
			RK960_ERROR_IO("invalid channel %d\n", channel);
			return -EINVAL;
		}

		RK960_INFO_IO("%s: start_tone: channel:%d\n", __func__,
			      channel);
		rk960_rftest_start_tone(hw_priv, channel, 1);
		// start_dc channel
	} else if (strstr(buf, "start_dc") != NULL) {
		sscanf(buf + 9, "%d", &channel);

		if (channel > 14 || channel < 1) {
			RK960_ERROR_IO("invalid channel %d\n", channel);
			return -EINVAL;
		}

		RK960_INFO_IO("%s: start_dc: channel:%d\n", __func__, channel);
		rk960_rftest_start_tone(hw_priv, channel, 0);
		// start_monitor channel
	} else if (strstr(buf, "start_monitor") != NULL) {
		sscanf(buf + 14, "%d", &channel);

		if (channel > 14 || channel < 1) {
			RK960_ERROR_IO("invalid channel %d\n", channel);
			return -EINVAL;
		}

		RK960_INFO_IO("%s: start_monitor: channel:%d\n", __func__,
			      channel);
		rk960_rftest_start_rx(hw_priv, channel | 0x80);
	}

	return count;
}
#endif

static const struct file_operations fops_rftest = {
	.open = rk960_generic_open,
	.read = rk960_rftest_read,
#if 1//def ENABLE_DBGFS_WRITE
	.write = rk960_rftest_write,
#endif
	.llseek = default_llseek,
};

static ssize_t rk960_csync_read(struct file *file,
				char __user * user_buf, size_t count,
				loff_t * ppos)
{
#ifdef RK960_CSYNC_ADJUST
	struct rk960_common *hw_priv = file->private_data;
	char buf[20];
	size_t size = 0;

	sprintf(buf, "%d\n", hw_priv->fw_csync);
	size = strlen(buf);
	return simple_read_from_buffer(user_buf, count, ppos, buf, size);
#else
	return 0;
#endif
}

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_csync_write(struct file *file,
				 const char __user * user_buf, size_t count,
				 loff_t * ppos)
{
#ifdef RK960_CSYNC_ADJUST
	struct rk960_common *hw_priv = file->private_data;
	char buf[MAX_WRITE_STR_LEN];
	int csync;

	if (!count || count > MAX_WRITE_STR_LEN)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	sscanf(buf, "%d", &csync);
	if (csync > 0 || csync < -95) {
		RK960_ERROR_IO("invalid csync %d\n", csync);
		hw_priv->csync_params.csync_disable = 0;
		return -EINVAL;
	}

	hw_priv->csync_params.csync_disable = 1;
	wsm_set_csync_thr(hw_priv, (s8) csync, 0);
#endif
	return count;
}
#endif

static const struct file_operations fops_csync = {
	.open = rk960_generic_open,
	.read = rk960_csync_read,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_csync_write,
#endif
	.llseek = default_llseek,
};

static ssize_t rk960_fw_dbg_leve_read(struct file *file,
				      char __user * user_buf, size_t count,
				      loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[16];
	size_t size = 0;
	u32 dbg_level;

	wsm_get_fw_dbg_level(hw_priv, &dbg_level);

	sprintf(buf, "%x\n", dbg_level);
	size = strlen(buf);
	return simple_read_from_buffer(user_buf, count, ppos, buf, size);
}

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_fw_dbg_leve_write(struct file *file,
				       const char __user * user_buf,
				       size_t count, loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[16];
	u32 value;

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	sscanf(buf, "%x", &value);

	RK960_DEBUG_IO("%s: value %x\n", __func__, value);

	wsm_set_fw_dbg_level(hw_priv, value);
	return count;
}
#endif

static const struct file_operations fops_fw_dbg_level = {
	.open = rk960_generic_open,
	.read = rk960_fw_dbg_leve_read,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_fw_dbg_leve_write,
#endif
	.llseek = default_llseek,
};

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_drv_dbg_level_write(struct file *file,
					 const char __user * user_buf,
					 size_t count, loff_t * ppos)
{
	char buf[16];

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	sscanf(buf, "%x", &rk960_debug_level);

	RK960_DEBUG_IO("%s: value %x\n", __func__, rk960_debug_level);
	return count;
}
#endif

static const struct file_operations fops_drv_dbg_level = {
	.open = rk960_generic_open,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_drv_dbg_level_write,
#endif
	.llseek = default_llseek,
};

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_drv_dbg_flag_write(struct file *file,
					const char __user * user_buf,
					size_t count, loff_t * ppos)
{
	char buf[16];

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	sscanf(buf, "%x", &rk960_debug_flag);

	RK960_DEBUG_IO("%s: value %x\n", __func__, rk960_debug_flag);
	return count;
}
#endif

static const struct file_operations fops_drv_dbg_flag = {
	.open = rk960_generic_open,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_drv_dbg_flag_write,
#endif
	.llseek = default_llseek,
};

#ifdef RK960_FW_ERROR_RECOVERY
static ssize_t rk960_fw_err_read(struct file *file,
				 char __user * user_buf, size_t count,
				 loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	size_t size = 0;
	struct wsm_com_cmd_s *cmd;
	int length = 0;
	int n = 0;
	int ret;
	u8 *buf;
	int i;

	buf = vmalloc(8 * 1024);
	if (!buf)
		return 0;

	n += sprintf(buf, "fw_error_counter: %d:", hw_priv->fw_error_counter);
	for (i = 0; i < hw_priv->fw_error_counter; i++)
		n += sprintf(buf + n, " %d", hw_priv->fw_error_reason[i]);
	n += sprintf(buf + n, "\nsave cmds:\n");

	list_for_each_entry(cmd, &hw_priv->wsm_cmds_list, list) {
		length++;
		if (cmd->cmd == 0x0006)
			n += sprintf(buf + n, "    %s 0x%.4X [MIB: 0x%.4X] "
				     "if_id %d\n",
				     wsm_conv_req_resp_to_str(cmd->cmd),
				     cmd->cmd, cmd->mibId, cmd->if_id);
		else
			n += sprintf(buf + n, "    %s 0x%.4X if_id %d\n",
				     wsm_conv_req_resp_to_str(cmd->cmd),
				     cmd->cmd, cmd->if_id);
	}

	buf[n] = 0;
	size = strlen(buf);
	ret = simple_read_from_buffer(user_buf, count, ppos, buf, size);
	vfree(buf);
	return ret;
}

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_fw_err_write(struct file *file,
				  const char __user * user_buf, size_t count,
				  loff_t * ppos)
{
	char buf[16];
	struct rk960_common *hw_priv = file->private_data;
	int enable;

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	sscanf(buf, "%d", &enable);

	if (enable)
		hw_priv->fw_error_enable = 1;
	else
		hw_priv->fw_error_enable = 0;

	//rk960_signal_fw_error(hw_priv, RK960_FWERR_REASON_TEST);
	return count;
}
#endif

static const struct file_operations fops_fw_err = {
	.open = rk960_generic_open,
	.read = rk960_fw_err_read,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_fw_err_write,
#endif
	.llseek = default_llseek,
};
#endif

static ssize_t rk960_rts_threshold_read(struct file *file,
				      char __user * user_buf, size_t count,
				      loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[16];
	size_t size = 0;

        if (!hw_priv->rts_threshold)
                hw_priv->rts_threshold = 3000; // default value
	sprintf(buf, "%d\n", hw_priv->rts_threshold);
	size = strlen(buf);
	return simple_read_from_buffer(user_buf, count, ppos, buf, size);
}

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_rts_threshold_write(struct file *file,
				       const char __user * user_buf,
				       size_t count, loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[16];
	__le32 rts_threshold;

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	sscanf(buf, "%d", &rts_threshold);
        if (rts_threshold >= 0) {                
                if (!rts_threshold)
                        rts_threshold = 3000;
	        RK960_DEBUG_IO("%s: rts_threshold %x\n", __func__, rts_threshold);

                hw_priv->rts_threshold = rts_threshold;
                wsm_write_mib(hw_priv, WSM_MIB_ID_DOT11_RTS_THRESHOLD,
				    &rts_threshold, sizeof(rts_threshold), 0);
        }

	return count;
}
#endif

static const struct file_operations fops_rts_threshold = {
	.open = rk960_generic_open,
	.read = rk960_rts_threshold_read,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_rts_threshold_write,
#endif
	.llseek = default_llseek,
};

#ifdef SUPPORT_FWCR
static ssize_t rk960_fwcr_save_read(struct file *file,
				      char __user * user_buf, size_t count,
				      loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;

        rk960_fwcr_write(hw_priv);
        
	return 0;
}

static const struct file_operations fops_fwcr_save = {
	.open = rk960_generic_open,
	.read = rk960_fwcr_save_read,
	.llseek = default_llseek,
};
#endif

static ssize_t rk960_fixed_rate_read(struct file *file,
				      char __user * user_buf, size_t count,
				      loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[256];
	size_t size = 0;

	size += sprintf(buf + size, "tx_fixed_rate: %d\n",
                        hw_priv->tx_fixed_rate);
        size += sprintf(buf + size,
                        "support rates:\n");
        size += sprintf(buf + size,
                        "10, 20, 55, 110\n");
        size += sprintf(buf + size,
                        "60, 90, 120, 180, 240, 360, 480, 540\n");
        size += sprintf(buf + size,
                        "65, 130, 195, 260, 390, 520, 585, 650\n");
	return simple_read_from_buffer(user_buf, count, ppos, buf, size);
}

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_fixed_rate_write(struct file *file,
				       const char __user * user_buf,
				       size_t count, loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[16];
	int tx_fixed_rate;
        int index;

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	sscanf(buf, "%d", &tx_fixed_rate);
        index = rk960_rate_to_rate_index(tx_fixed_rate);
        if (index >= 0)
                hw_priv->tx_fixed_rate = tx_fixed_rate;
        else
                hw_priv->tx_fixed_rate = 0;

	return count;
}
#endif

static const struct file_operations fops_fixed_rate = {
	.open = rk960_generic_open,
	.read = rk960_fixed_rate_read,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_fixed_rate_write,
#endif
	.llseek = default_llseek,
};

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_tx_agg_num_write(struct file *file,
				       const char __user * user_buf,
				       size_t count, loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[16];
	int tx_agg_num;
        struct wsm_set_tx_agg_control tx_agg_ctrl;
        int i;

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	sscanf(buf, "%d", &tx_agg_num);
        if (tx_agg_num > 16)
                tx_agg_num = 16;
        for (i = 0; i < 8; i++)
                tx_agg_ctrl.tx_max_agg_num[i] = tx_agg_num;
        wsm_set_agg_control(hw_priv, &tx_agg_ctrl);

	return count;
}
#endif

static const struct file_operations fops_tx_agg_num = {
	.open = rk960_generic_open,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_tx_agg_num_write,
#endif
	.llseek = default_llseek,
};

static ssize_t rk960_dtim_int_read(struct file *file,
				      char __user * user_buf, size_t count,
				      loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[32];
	size_t size = 0;

	size += sprintf(buf, "dtim_interval: %d\n",
                        hw_priv->dtim_interval);
	return simple_read_from_buffer(user_buf, count, ppos, buf, size);
}

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_dtim_int_write(struct file *file,
				       const char __user * user_buf,
				       size_t count, loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[16];
	int dtim_interval;

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	sscanf(buf, "%d", &dtim_interval);
        hw_priv->dtim_interval = dtim_interval;
        wsm_set_beacon_wakeup_period(hw_priv,
                        hw_priv->dtim_interval,
                        hw_priv->listen_interval, 0);

	return count;
}
#endif

static const struct file_operations fops_dtim_int = {
	.open = rk960_generic_open,
	.read = rk960_dtim_int_read,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_dtim_int_write,
#endif
	.llseek = default_llseek,
};

static ssize_t rk960_listen_int_read(struct file *file,
				      char __user * user_buf, size_t count,
				      loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[32];
	size_t size = 0;

	size += sprintf(buf, "listen_interval: %d\n",
                        hw_priv->listen_interval);
	return simple_read_from_buffer(user_buf, count, ppos, buf, size);
}

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_listen_int_write(struct file *file,
				       const char __user * user_buf,
				       size_t count, loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[16];
	int listen_interval;

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	sscanf(buf, "%d", &listen_interval);
        hw_priv->listen_interval = listen_interval;
        wsm_set_beacon_wakeup_period(hw_priv,
                        hw_priv->dtim_interval,
                        hw_priv->listen_interval, 0);

	return count;
}
#endif

static const struct file_operations fops_listen_int = {
	.open = rk960_generic_open,
	.read = rk960_listen_int_read,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_listen_int_write,
#endif
	.llseek = default_llseek,
};

static ssize_t rk960_efuse_read(struct file *file,
				char __user * user_buf, size_t count,
				loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[RK960_EFUSE_SIZE];
	char buf_str[RK960_EFUSE_SIZE * 4];
	size_t size = 0;
	int i;

	if (*ppos)
		return size;

	wsm_read_efuse(hw_priv, buf);

	for (i = 0; i < RK960_EFUSE_SIZE; i++) {
		size += sprintf(&buf_str[size], "%02x ", buf[i]);
		if (((i + 1) % 16) == 0)
			size += sprintf(&buf_str[size], "\n");
	}

	return simple_read_from_buffer(user_buf, count, ppos, buf_str, size);
}

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_efuse_write(struct file *file,
				 const char __user * user_buf, size_t count,
				 loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[MAX_WRITE_STR_LEN];
	int offset, value;

	if (!count || count > MAX_WRITE_STR_LEN)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	// echo offset value > efuse
	// e.g. offset = 16, value = 0x55
	// echo 16 0x55 > efuse
	sscanf(buf, "%d %x", &offset, &value);
	if (offset > 63 || offset < 0) {
		RK960_ERROR_IO("invalid offset %d\n", offset);
		return -EINVAL;
	}

	RK960_DEBUG_IO("%s: off %d val 0x%02x\n", __func__, offset, value);

	wsm_write_efuse(hw_priv, offset, value);
	return count;
}
#endif

static const struct file_operations fops_efuse = {
	.open = rk960_generic_open,
	.read = rk960_efuse_read,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_efuse_write,
#endif
	.llseek = default_llseek,
};

static ssize_t rk960_wifi_efuse_mac_read(struct file *file,
					 char __user * user_buf, size_t count,
					 loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf_str[32];
	size_t size = 0;
	int i;

	for (i = 0; i < 6; i++) {
		size += sprintf(&buf_str[size], "%02x",
				hw_priv->wifi_efuse_mac_addr[i]);
		if (i < 5)
			size += sprintf(&buf_str[size], ":");
	}
	size += sprintf(&buf_str[size], "\n");

	return simple_read_from_buffer(user_buf, count, ppos, buf_str, size);
}

static const struct file_operations fops_wifi_efuse_mac = {
	.open = rk960_generic_open,
	.read = rk960_wifi_efuse_mac_read,
	.llseek = default_llseek,
};

static ssize_t rk960_bt_efuse_mac_read(struct file *file,
				       char __user * user_buf, size_t count,
				       loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf_str[32];
	size_t size = 0;
	int i;

	for (i = 0; i < 6; i++) {
		size += sprintf(&buf_str[size], "%02x",
				hw_priv->bt_efuse_mac_addr[i]);
		if (i < 5)
			size += sprintf(&buf_str[size], ":");
	}
	size += sprintf(&buf_str[size], "\n");

	return simple_read_from_buffer(user_buf, count, ppos, buf_str, size);
}

static const struct file_operations fops_bt_efuse_mac = {
	.open = rk960_generic_open,
	.read = rk960_bt_efuse_mac_read,
	.llseek = default_llseek,
};

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_rxgain_write(struct file *file,
				  const char __user * user_buf, size_t count,
				  loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[MAX_WRITE_STR_LEN];
	int abbf, lna, en;

	if (!count || count > MAX_WRITE_STR_LEN)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	// echo abbf lna en > rxgain
	// e.g.: min gain: echo 48 8 1 > rxgain
	// e.g.: enable agc: echo 48 8 0 > rxgain
	sscanf(buf, "%d %d %d", &abbf, &lna, &en);
	if (abbf > 48 || abbf < 24) {
		RK960_ERROR_IO("invalid abbf %d[24-48]\n", abbf);
		return -EINVAL;
	}
	if (lna > 8 || lna < 0) {
		RK960_ERROR_IO("invalid lna %d[0-8]\n", lna);
		return -EINVAL;
	}
	if (en)
		en = 1;

	RK960_DEBUG_IO("%s: abbf %d lna %d en %d\n", __func__, abbf, lna, en);

	wsm_set_manual_rxgain(hw_priv, en, abbf, lna);
	return count;
}
#endif

static const struct file_operations fops_rxgain = {
	.open = rk960_generic_open,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_rxgain_write,
#endif
	.llseek = default_llseek,
};

static ssize_t rk960_txpower_read(struct file *file,
				  char __user * user_buf, size_t count,
				  loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[20];
	size_t size = 0;

	sprintf(buf, "%d\n", hw_priv->fw_output_power);
	size = strlen(buf);
	return simple_read_from_buffer(user_buf, count, ppos, buf, size);
}

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_txpower_write(struct file *file,
				   const char __user * user_buf, size_t count,
				   loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[MAX_WRITE_STR_LEN];
	int tx_power;

	if (!count || count > MAX_WRITE_STR_LEN)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	// echo txpower > txpower
	// echo 20 > txpower
	sscanf(buf, "%d", &tx_power);
	if (!tx_power)
		tx_power = RK960_DEFAULT_TX_POWER * 10;
	else if (tx_power > 20)
		tx_power = 20 * 16;	// 20db
	else
		tx_power *= 16;

	RK960_DEBUG_IO("%s: tx_power %d\n", __func__, tx_power);

	wsm_set_output_power(hw_priv, tx_power, 0);
	return count;
}
#endif

static const struct file_operations fops_txpower = {
	.open = rk960_generic_open,
	.read = rk960_txpower_read,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_txpower_write,
#endif
	.llseek = default_llseek,
};

#define RK960_TXPWR_COMP_DUMP(rate, n)  \
do {                                    \
	size += sprintf(buf + size, "%s=", rate);  \
	for (i = 0; i < 14; i++) {          \
		size += sprintf(buf + size, "%d,", hw_priv->txpwr_comp_tbl[i][n]);  \
    }                                   \
	size += sprintf(buf + size, "\n");         \
} while (0)

static ssize_t rk960_txpower_comp_read(struct file *file,
				       char __user * user_buf, size_t count,
				       loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	size_t size = 0;
	int i, ret = 0;
	u8 *buf;

	hw_priv->debug_dump_data_str = vmalloc(1024 * 4);
	if (!hw_priv->debug_dump_data_str)
		goto txpower_comp_read_exit;

	buf = hw_priv->debug_dump_data_str;

	RK960_TXPWR_COMP_DUMP("txpwr_dsss_comp", 0);
	RK960_TXPWR_COMP_DUMP("txpwr_cck_comp", 1);
	RK960_TXPWR_COMP_DUMP("txpwr_bpsk_12_comp", 2);
	RK960_TXPWR_COMP_DUMP("txpwr_bpsk_34_comp", 3);
	RK960_TXPWR_COMP_DUMP("txpwr_qpsk_12_comp", 4);
	RK960_TXPWR_COMP_DUMP("txpwr_qpsk_34_comp", 5);
	RK960_TXPWR_COMP_DUMP("txpwr_16qam_12_comp", 6);
	RK960_TXPWR_COMP_DUMP("txpwr_16qam_34_comp", 7);
	RK960_TXPWR_COMP_DUMP("txpwr_64qam_23_comp", 8);
	RK960_TXPWR_COMP_DUMP("txpwr_64qam_34_comp", 9);
	RK960_TXPWR_COMP_DUMP("txpwr_64qam_56_comp", 10);

	ret = simple_read_from_buffer(user_buf, count, ppos, buf, size);

txpower_comp_read_exit:
	if (hw_priv->debug_dump_data_str) {
		vfree(hw_priv->debug_dump_data_str);
		hw_priv->debug_dump_data_str = NULL;
	}
	return ret;
}

static const struct file_operations fops_txpower_comp = {
	.open = rk960_generic_open,
	.read = rk960_txpower_comp_read,
	.llseek = default_llseek,
};

static ssize_t rk960_wsm_dumps(struct file *file,
			       const char __user * user_buf, size_t count,
			       loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[1];

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, 1))
		return -EFAULT;

	if (buf[0] == '1')
		hw_priv->wsm_enable_wsm_dumps = 1;
	else
		hw_priv->wsm_enable_wsm_dumps = 0;

	return count;
}

static const struct file_operations fops_wsm_dumps = {
	.open = rk960_generic_open,
	.write = rk960_wsm_dumps,
	.llseek = default_llseek,
};

#if defined(CONFIG_RK960_WSM_DUMPS_SHORT)
static ssize_t rk960_short_dump_read(struct file *file,
				     char __user * user_buf, size_t count,
				     loff_t * ppos)
{
	struct rk960_common *hw_priv = file->private_data;
	char buf[20];
	size_t size = 0;

	sprintf(buf, "Size: %u\n", hw_priv->wsm_dump_max_size);
	size = strlen(buf);

	return simple_read_from_buffer(user_buf, count, ppos, buf, size);
}

#ifdef ENABLE_DBGFS_WRITE
static ssize_t rk960_short_dump_write(struct file *file,
				      const char __user * user_buf,
				      size_t count, loff_t * ppos)
{
	struct rk960_common *priv = file->private_data;
	char buf[20];
	unsigned long dump_size = 0;

	if (!count || count > 20)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	if (kstrtoul(buf, 10, &dump_size))
		return -EINVAL;

	priv->wsm_dump_max_size = dump_size;

	return count;
}
#endif

static const struct file_operations fops_short_dump = {
	.open = rk960_generic_open,
#ifdef ENABLE_DBGFS_WRITE
	.write = rk960_short_dump_write,
#endif
	.read = rk960_short_dump_read,
	.llseek = default_llseek,
};
#endif /* CONFIG_RK960_WSM_DUMPS_SHORT */

int rk960_debug_init_common(struct rk960_common *hw_priv)
{
	int ret = -ENOMEM;
	struct rk960_debug_common *d =
	    kzalloc(sizeof(struct rk960_debug_common), GFP_KERNEL);
	hw_priv->debug = d;
	if (!d)
		return ret;

	d->debugfs_phy = debugfs_create_dir("rk960",
					    hw_priv->hw->wiphy->debugfsdir);
	if (!d->debugfs_phy)
		goto err;

	if (!debugfs_create_file("status", S_IRUSR, d->debugfs_phy,
				 hw_priv, &fops_status_common))
		goto err;

	if (!debugfs_create_file("counters", S_IRUSR, d->debugfs_phy,
				 hw_priv, &fops_counters))
		goto err;

	if (!debugfs_create_file("11n", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_11n))
		goto err;

	if (!debugfs_create_file("mem", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_mem))
		goto err;

	if (!debugfs_create_file("rftest", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_rftest))
		goto err;

	if (!debugfs_create_file("adc", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_adc))
		goto err;

	if (!debugfs_create_file("csync", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_csync))
		goto err;

	if (!debugfs_create_file("efuse", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_efuse))
		goto err;

	if (!debugfs_create_file("wifi_efuse_mac", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_wifi_efuse_mac))
		goto err;

	if (!debugfs_create_file("bt_efuse_mac", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_bt_efuse_mac))
		goto err;

	if (!debugfs_create_file("rxgain", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_rxgain))
		goto err;

	if (!debugfs_create_file("txpower", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_txpower))
		goto err;

	if (!debugfs_create_file("txpower_comp", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_txpower_comp))
		goto err;

	if (!debugfs_create_file("fw_dbg_level", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_fw_dbg_level))
		goto err;

	if (!debugfs_create_file("drv_dbg_level", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_drv_dbg_level))
		goto err;

	if (!debugfs_create_file("drv_dbg_flag", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_drv_dbg_flag))
		goto err;

#ifdef RK960_FW_ERROR_RECOVERY
	if (!debugfs_create_file("fw_err", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_fw_err))
		goto err;
#endif

	if (!debugfs_create_file("rts_threshold", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_rts_threshold))
		goto err;

#ifdef SUPPORT_FWCR
	if (!debugfs_create_file("fwcr_save", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_fwcr_save))
		goto err;
#endif

	if (!debugfs_create_file("fixed_rate", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_fixed_rate))
		goto err;

	if (!debugfs_create_file("tx_agg_num", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_tx_agg_num))
		goto err;

	if (!debugfs_create_file("dtim_int", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_dtim_int))
		goto err;

	if (!debugfs_create_file("listen_int", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_listen_int))
		goto err;

	/*if (!debugfs_create_file("device_debug", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
	   d->debugfs_phy, hw_priv, &fops_device_debug_struct_info))
	   goto err; */

	if (!debugfs_create_file("wsm_dumps", S_IWUSR, d->debugfs_phy,
				 hw_priv, &fops_wsm_dumps))
		goto err;

#if defined(CONFIG_RK960_WSM_DUMPS_SHORT)
	if (!debugfs_create_file("wsm_dump_size", S_IRUSR | S_IWUSR,
				 d->debugfs_phy, hw_priv, &fops_short_dump))
		goto err;
#endif /* CONFIG_RK960_WSM_DUMPS_SHORT */

	ret = rk960_itp_init(hw_priv);
	if (ret)
		goto err;

	return 0;

err:
	hw_priv->debug = NULL;
	debugfs_remove_recursive(d->debugfs_phy);
	kfree(d);
	return ret;
}

void rk960_debug_release_common(struct rk960_common *hw_priv)
{
	struct rk960_debug_common *d = hw_priv->debug;
	if (d) {
		rk960_itp_release(hw_priv);
		hw_priv->debug = NULL;
		kfree(d);
	}
}

static int rk960_status_show_priv(struct seq_file *seq, void *v)
{
	int i;
	struct rk960_vif *priv = seq->private;
	struct rk960_debug_priv *d = priv->debug;

	seq_printf(seq, "Mode:       %s%s\n",
		   rk960_debug_mode(priv->mode),
		   priv->listening ? " (listening)" : "");
	seq_printf(seq, "Assoc:      %s\n",
		   rk960_debug_join_status[priv->join_status]);
	if (priv->rx_filter.promiscuous)
		seq_puts(seq, "Filter:     promisc\n");
	else if (priv->rx_filter.fcs)
		seq_puts(seq, "Filter:     fcs\n");
	if (priv->rx_filter.bssid)
		seq_puts(seq, "Filter:     bssid\n");
	if (priv->bf_control.bcn_count)
		seq_puts(seq, "Filter:     beacons\n");

	if (priv->enable_beacon ||
	    priv->mode == NL80211_IFTYPE_AP ||
	    priv->mode == NL80211_IFTYPE_ADHOC ||
	    priv->mode == NL80211_IFTYPE_MESH_POINT ||
	    priv->mode == NL80211_IFTYPE_P2P_GO)
		seq_printf(seq, "Beaconing:  %s\n",
			   priv->enable_beacon ? "enabled" : "disabled");
	if (priv->ssid_length ||
	    priv->mode == NL80211_IFTYPE_AP ||
	    priv->mode == NL80211_IFTYPE_ADHOC ||
	    priv->mode == NL80211_IFTYPE_MESH_POINT ||
	    priv->mode == NL80211_IFTYPE_P2P_GO)
		seq_printf(seq, "SSID:       %.*s\n",
			   (int)priv->ssid_length, priv->ssid);

	for (i = 0; i < 4; ++i) {
		seq_printf(seq, "EDCA(%d):    %d, %d, %d, %d, %d\n", i,
			   priv->edca.params[i].cwMin,
			   priv->edca.params[i].cwMax,
			   priv->edca.params[i].aifns,
			   priv->edca.params[i].txOpLimit,
			   priv->edca.params[i].maxReceiveLifetime);
	}
	if (priv->join_status == RK960_JOIN_STATUS_STA) {
		static const char *pmMode = "unknown";
		switch (priv->powersave_mode.pmMode) {
		case WSM_PSM_ACTIVE:
			pmMode = "off";
			break;
		case WSM_PSM_PS:
			pmMode = "on";
			break;
		case WSM_PSM_FAST_PS:
			pmMode = "dynamic";
			break;
		}
		seq_printf(seq, "Preamble:   %s\n",
			   rk960_debug_preamble[priv->association_mode.
						preambleType]);
		seq_printf(seq, "AMPDU spcn: %d\n",
			   priv->association_mode.mpduStartSpacing);
		seq_printf(seq, "Basic rate: 0x%.8X\n",
			   le32_to_cpu(priv->association_mode.basicRateSet));
		seq_printf(seq, "Bss lost:   %d beacons\n",
			   priv->bss_params.beaconLostCount);
		seq_printf(seq, "AID:        %d\n", priv->bss_params.aid);
		seq_printf(seq, "Rates:      0x%.8X\n",
			   priv->bss_params.operationalRateSet);
		seq_printf(seq, "Powersave:  %s\n", pmMode);
	}
	seq_printf(seq, "RSSI thold: %d\n", priv->cqm_rssi_thold);
	seq_printf(seq, "RSSI hyst:  %d\n", priv->cqm_rssi_hyst);
	seq_printf(seq, "TXFL thold: %d\n", priv->cqm_tx_failure_thold);
	seq_printf(seq, "Linkloss:   %d\n", priv->cqm_link_loss_count);
	seq_printf(seq, "Bcnloss:    %d\n", priv->cqm_beacon_loss_count);

	rk960_debug_print_map(seq, priv, "Link map:   ", priv->link_id_map);
	rk960_debug_print_map(seq, priv, "Asleep map: ", priv->sta_asleep_mask);
	rk960_debug_print_map(seq, priv, "PSPOLL map: ", priv->pspoll_mask);

	seq_puts(seq, "\n");

	for (i = 0; i < RK960_MAX_STA_IN_AP_MODE; ++i) {
		if (priv->link_id_db[i].status) {
			seq_printf(seq, "Link %d:     %s, %pM\n",
				   i + 1,
				   rk960_debug_link_id[priv->link_id_db[i].
						       status],
				   priv->link_id_db[i].mac);
		}
	}

	seq_puts(seq, "\n");

	seq_printf(seq, "Powermgmt: 0x%02x\n", priv->firmware_ps_mode.pmMode);

	seq_printf(seq, "TXed:       %d\n", d->tx);
	seq_printf(seq, "AGG TXed:   %d\n", d->tx_agg);
	seq_printf(seq, "MULTI TXed: %d (%d)\n",
		   d->tx_multi, d->tx_multi_frames);
	seq_printf(seq, "RXed:       %d\n", d->rx);
	seq_printf(seq, "AGG RXed:   %d\n", d->rx_agg);
	seq_printf(seq, "TX align:   %d\n", d->tx_align);
	seq_printf(seq, "TX TTL:     %d\n", d->tx_ttl);
	return 0;
}

static int rk960_status_open_priv(struct inode *inode, struct file *file)
{
	return single_open(file, &rk960_status_show_priv, inode->i_private);
}

static const struct file_operations fops_status_priv = {
	.open = rk960_status_open_priv,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.owner = THIS_MODULE,
};

static int rk960_tp_show_priv(struct seq_file *seq, void *v)
{
	int i, j;
	struct rk960_vif *priv = seq->private;
	struct rk960_common *hw_priv = priv->hw_priv;
	struct wsm_rx_tp_info rx_tp_info[RK960_RX_TP_INFO_RATE_NUM];
	int rate_table[] = {
		10, 20, 55, 110, 0, 0,
		60, 90, 120, 180, 240, 360, 480, 540,
		65, 130, 195, 260, 390, 520, 585, 650
	};

	wsm_get_rx_tp_info(hw_priv, rx_tp_info);

	for (i = 0; i < RK960_RX_TP_INFO_RATE_NUM; i++) {
		struct wsm_rx_tp_info_s *info = rx_tp_info[i].rx_tp_info_s;
		s8 s_rssi;

		if (i == 4 || i == 5)
			continue;

		seq_printf(seq, "Rate %d: frames %d ", rate_table[i],
			   rx_tp_info[i].count);
		for (j = 0; j < RK960_RX_TP_INFO_DUMP_NUM; j++) {
			s_rssi = (s8) info->rssi;
			seq_printf(seq,
				   " (type %02x size %d rssi %d snr %d ch %d mode %d)",
				   info->frame_type, info->size, s_rssi,
				   info->snr, info->ch, info->mode);
		}
		seq_printf(seq, "\n");
	}
	return 0;
}

static int rk960_tp_open_priv(struct inode *inode, struct file *file)
{
	return single_open(file, &rk960_tp_show_priv, inode->i_private);
}

static const struct file_operations fops_tp_priv = {
	.open = rk960_tp_open_priv,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.owner = THIS_MODULE,
};

#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)

static ssize_t rk960_hang_write(struct file *file,
				const char __user * user_buf, size_t count,
				loff_t * ppos)
{
	struct rk960_vif *priv = file->private_data;
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);
	char buf[1];

	if (!count)
		return -EINVAL;
	if (copy_from_user(buf, user_buf, 1))
		return -EFAULT;

	if (priv->vif) {
		rk960_pm_stay_awake(&hw_priv->pm_state, 3 * HZ);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
		pr_err("%s: ieee80211_driver_hang_notify\n", __func__);
#else
		ieee80211_driver_hang_notify(priv->vif, GFP_KERNEL);
#endif
	} else
		return -ENODEV;

	return count;
}

static const struct file_operations fops_hang = {
	.open = rk960_generic_open,
	.write = rk960_hang_write,
	.llseek = default_llseek,
};
#endif

#define VIF_DEBUGFS_NAME_S 10
int rk960_debug_init_priv(struct rk960_common *hw_priv, struct rk960_vif *priv)
{
	int ret = -ENOMEM;
	struct rk960_debug_priv *d;
	char name[VIF_DEBUGFS_NAME_S];

	if (WARN_ON(!hw_priv))
		return ret;

	if (WARN_ON(!hw_priv->debug))
		return ret;

	d = kzalloc(sizeof(struct rk960_debug_priv), GFP_KERNEL);
	priv->debug = d;
	if (WARN_ON(!d))
		return ret;

	memset(name, 0, VIF_DEBUGFS_NAME_S);
	ret = snprintf(name, VIF_DEBUGFS_NAME_S, "vif_%d", priv->if_id);
	if (WARN_ON(ret < 0))
		goto err;

	d->debugfs_phy = debugfs_create_dir(name, hw_priv->debug->debugfs_phy);
	if (WARN_ON(!d->debugfs_phy))
		goto err;

#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
	if (WARN_ON(!debugfs_create_file("hang", S_IWUSR, d->debugfs_phy,
					 priv, &fops_hang)))
		goto err;
#endif

	if (!debugfs_create_file("status", S_IRUSR, d->debugfs_phy,
				 priv, &fops_status_priv))
		goto err;

	if (!debugfs_create_file("tp", S_IRUSR, d->debugfs_phy,
				 priv, &fops_tp_priv))
		goto err;

	return 0;
err:
	priv->debug = NULL;
	debugfs_remove_recursive(d->debugfs_phy);
	kfree(d);
	return ret;

}

void rk960_debug_release_priv(struct rk960_vif *priv)
{
	struct rk960_debug_priv *d = priv->debug;
	if (d) {
		priv->debug = NULL;
		kfree(d);
	}
}

int rk960_print_fw_version(struct rk960_common *hw_priv, u8 * buf, size_t len)
{
	return snprintf(buf, len, "%s %d.%d",
			rk960_debug_fw_types[hw_priv->wsm_caps.firmwareType],
			hw_priv->wsm_caps.firmwareVersion,
			hw_priv->wsm_caps.firmwareBuildNumber);
}

#ifdef DUMP_TXRX_MAC_FRAME_INFO

#include <net/cfg80211.h>
#include <net/ip.h>
#include <linux/tcp.h>
#include <linux/etherdevice.h>

#define CIPHER_TYPE_WEP40 0
#define CIPHER_TYPE_WEP104 1
#define CIPHER_TYPE_TKIP 2
#define CIPHER_TYPE_CCMP 3
#define CIPHER_TYPE_WAPI 4

static int g_cipher_type = CIPHER_TYPE_TKIP;
int ieee80211_crypt_hdrlen(u16 fc)
{
	int hdrlen = 0;

	if (ieee80211_has_protected(fc)) {
		switch (g_cipher_type) {
		case CIPHER_TYPE_WEP40:
		case CIPHER_TYPE_WEP104:
			hdrlen = 4;	//WEP_IV_LEN;
			break;
		case CIPHER_TYPE_TKIP:
		case CIPHER_TYPE_CCMP:
			hdrlen = 8;
			break;
		}
	}
	return hdrlen;
}

static int ieee8022_ll_hdrlen(u8 * payload, u16 ethertype)
{
	if ((ether_addr_equal(payload, rfc1042_header) &&
	     ethertype != ETH_P_AARP && ethertype != ETH_P_IPX) ||
	    ether_addr_equal(payload, bridge_tunnel_header)) {
		return 8;
	} else {
		return 0;
	}
}

/*
 * 802.11 frame struct  (for example ICMP):
 *
 * | 802.11 header | crypt header | 802.2 LL header | IP header| ICMP |
 *
 *                                option              option
 */
static void dump_ip_info(u8 * data, int len, u8 * str, int tx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)data;
	u16 fc = hdr->frame_control;

	if (ieee80211_is_data(fc)	/*&&
					   is_unicast_ether_addr(ieee80211_get_DA(hdr)) */ ) {
		int offset, ieee8022_hdrlen;
		u8 *ieee8022_payload;
		int ethertype, n;
		struct iphdr *ip;

		offset = ieee80211_hdrlen(fc);	/* 802.11 header */
		if (!tx)
			offset += ieee80211_crypt_hdrlen(fc);	/* crypt header */

		if (offset >= len) {
			return;
		}

		ieee8022_payload = data + offset;	/* ieee802.2 ll header */
		ethertype = (ieee8022_payload[6] << 8) | ieee8022_payload[7];
		ieee8022_hdrlen =
		    ieee8022_ll_hdrlen(ieee8022_payload, ethertype);

		n = sprintf(str, "ethertype %04x ", ethertype);
		str += n;
		if (ieee8022_hdrlen && ethertype == ETH_P_IP	/* &&
								   ethertype == ETH_P_IPV6 */ ) {
			offset += ieee8022_hdrlen;
			ip = (struct iphdr *)(data + offset);	/* IP header */
			n = sprintf(str, "protocol %03d %pI4 -> %pI4 ",
				    ip->protocol, &(ip->saddr), &(ip->daddr));
			str += n;
			if (ip->protocol == IPPROTO_UDP) {
				struct udphdr *uh =
				    (struct udphdr *)((u8 *) ip + ip->ihl * 4);

				sprintf(str, "UDP Port %d -> %d ",
					ntohs(uh->source), ntohs(uh->dest));
			} else if (ip->protocol == IPPROTO_TCP) {
				struct tcphdr *th =
				    (struct tcphdr *)((u8 *) ip + ip->ihl * 4);

				sprintf(str, "TCP Port %d -> %d ",
					ntohs(th->source), ntohs(th->dest));
			} else if (ip->protocol == IPPROTO_ICMP) {
				u8 *icmp_payload = (u8 *) ip + ip->ihl * 4;
				u16 *seq = (u16 *) & icmp_payload[6];

				if (icmp_payload[0] == 0x00) {	// Replay
					sprintf(str, "icmp rep %d\n",
						ntohs(*seq));
				} else if (icmp_payload[0] == 0x08) {	// Request
					sprintf(str, "icmp req %d\n",
						ntohs(*seq));
				}
			}
		}
	}
}

enum p2p_action_frame_type {
	P2P_GO_NEG_REQ = 0,
	P2P_GO_NEG_RESP = 1,
	P2P_GO_NEG_CONF = 2,
	P2P_INVITATION_REQ = 3,
	P2P_INVITATION_RESP = 4,
	P2P_DEV_DISC_REQ = 5,
	P2P_DEV_DISC_RESP = 6,
	P2P_PROV_DISC_REQ = 7,
	P2P_PROV_DISC_RESP = 8
};

static inline u32 WPA_GET_BE32(const u8 * a)
{
	return ((u32) a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3];
}

static char *dump_action_type(struct ieee80211_hdr *hdr, int tx)
{
	u8 *payload;
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)hdr;
	u8 *category;

	category = (u8 *) & mgmt->u.action.category;
	if (!tx) {
		category += ieee80211_crypt_hdrlen(mgmt->frame_control);
	}

	payload = (u8 *) hdr + sizeof(struct ieee80211_hdr_3addr);

#define WLAN_ACTION_PUBLIC 4
#define WLAN_PA_VENDOR_SPECIFIC 9
#define P2P_IE_VENDOR_TYPE 0x506f9a09

	if (ieee80211_is_action(mgmt->frame_control) &&
	    *category == WLAN_CATEGORY_BACK) {
		switch (*(category + 1)) {
		case WLAN_ACTION_ADDBA_REQ:
			return "ADDBA_REQ";
		case WLAN_ACTION_ADDBA_RESP:
			return "ADDBA_RESP";
		case WLAN_ACTION_DELBA:
			return "ADDBA_DELBA";
		default:
			break;
		}
	} else if (mgmt->u.action.category == WLAN_ACTION_PUBLIC) {
		switch (payload[1]) {
		case WLAN_PA_VENDOR_SPECIFIC:
			payload += 2;
			if (WPA_GET_BE32(payload) != P2P_IE_VENDOR_TYPE)
				return "";

			payload += 4;
			switch (payload[0]) {
			case P2P_GO_NEG_REQ:
				return "P2P_GO_NEG_REQ";
			case P2P_GO_NEG_RESP:
				return "P2P_GO_NEG_RESP";
			case P2P_GO_NEG_CONF:
				return "P2P_GO_NEG_CONF";
			case P2P_INVITATION_REQ:
				return "P2P_INVITATION_REQ";
			case P2P_INVITATION_RESP:
				return "P2P_INVITATION_RESP";
			case P2P_PROV_DISC_REQ:
				return "P2P_PROV_DISC_REQ";
			case P2P_PROV_DISC_RESP:
				return "P2P_PROV_DISC_RESP";
			case P2P_DEV_DISC_REQ:
				return "P2P_DEV_DISC_REQ";
			case P2P_DEV_DISC_RESP:
				return "P2P_DEV_DISC_RESP";
			}
			break;
		}
	}
	return "";
}

void dump_ieee80211_hdr_info(unsigned char *data, int len, int tx, s8 rssi)
{
	int n;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)data;
	char direct_str[512];
	u8 *DA = ieee80211_get_DA(hdr);
	u8 *SA = ieee80211_get_SA(hdr);
	int retr = (hdr->frame_control & 0x0800) >> 11;
	int pm = (hdr->frame_control & 0x1000) >> 12;

	if (!(rk960_debug_level & DEBUG_LEVEL_DEBUG)) {
		return;
	}
	//print_hex_dump(KERN_INFO, " ", DUMP_PREFIX_NONE, 16, 1, data, len, 1);

	/* at here: tx protected frame have no ccmp header,
	   rx protected frame have ccmp header, so need to skip ccmp header
	   with rx frame when parse */

	n = sprintf(direct_str,
		    "%s fc %04x sn %04d retr %d pm %d len %04d %pM -> %pM rssi %d ",
		    tx ? "tx" : "rx", cpu_to_le16(hdr->frame_control),
		    hdr->seq_ctrl >> 4, retr, pm, len, SA, DA, rssi);
	dump_ip_info(data, len, &direct_str[n], tx);

	if (hdr != NULL) {
		//RK960_DEBUG_UMACIF("%s\n", __func__);
		if (ieee80211_is_mgmt(hdr->frame_control)) {
			if (ieee80211_is_assoc_req(hdr->frame_control)) {
				RK960_INFO_TXRX("%s assoc req\n", direct_str);
			} else if (ieee80211_is_assoc_resp(hdr->frame_control)) {
				RK960_INFO_TXRX("%s assoc resp\n", direct_str);
			} else if (ieee80211_is_reassoc_req(hdr->frame_control)) {
				RK960_INFO_TXRX("%s reassoc req\n", direct_str);
			} else
			    if (ieee80211_is_reassoc_resp(hdr->frame_control)) {
				RK960_INFO_TXRX("%s reassoc resp\n",
						direct_str);
			} else if (ieee80211_is_probe_req(hdr->frame_control)) {
				RK960_INFO_TXRX("%s probe req\n", direct_str);
			} else if (ieee80211_is_probe_resp(hdr->frame_control)) {
				RK960_INFO_TXRX("%s probe resp\n", direct_str);
			} else if (ieee80211_is_beacon(hdr->frame_control)) {
				RK960_INFO_TXRX("%s beacon\n", direct_str);
			} else if (ieee80211_is_atim(hdr->frame_control)) {
				RK960_INFO_TXRX("%s atim\n", direct_str);
			} else if (ieee80211_is_disassoc(hdr->frame_control)) {
				RK960_INFO_TXRX("%s disassoc\n", direct_str);
			} else if (ieee80211_is_auth(hdr->frame_control)) {
				RK960_INFO_TXRX("%s auth\n", direct_str);
			} else if (ieee80211_is_deauth(hdr->frame_control)) {
				RK960_INFO_TXRX("%s deauth\n", direct_str);
			} else if (ieee80211_is_action(hdr->frame_control)) {
				RK960_INFO_TXRX("%s action %s\n", direct_str,
						dump_action_type(hdr, tx));
			} else {
				RK960_INFO_TXRX("%s mgmt\n", direct_str);
			}
		} else if (ieee80211_is_ctl(hdr->frame_control)) {
			RK960_INFO_TXRX("%s ctl\n", direct_str);
		} else if (ieee80211_is_data(hdr->frame_control)) {
			RK960_INFO_TXRX("%s data\n", direct_str);
		} else {
			RK960_INFO_TXRX("%s unknow\n", direct_str);
		}
	}
}
#else
void dump_ieee80211_hdr_info(unsigned char *data, int len, int tx, s8 rssi)
{
}
#endif
