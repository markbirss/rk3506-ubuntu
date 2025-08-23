/*
 * Copyright (c) 2022, Fuzhou Rockchip Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/skbuff.h>
#include <linux/wait.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/random.h>
#include <linux/etherdevice.h>

#include "rk960.h"
#include "wsm.h"
#include "bh.h"
#include "debug.h"
#include "itp.h"
#include "sta.h"
#include "hwbus.h"

//#define DISABLE_MULTI_WSM_TX

#define WSM_CMD_TIMEOUT		(2 * HZ)	/* With respect to interrupt loss */
#define WSM_CMD_JOIN_TIMEOUT	(7 * HZ)	/* Join timeout is 5 sec. in FW   */
#define WSM_CMD_START_TIMEOUT	(7 * HZ)
#define WSM_CMD_RESET_TIMEOUT	(3 * HZ)	/* 2 sec. timeout was observed.   */
#define WSM_CMD_DEFAULT_TIMEOUT	(3 * HZ)
#define WSM_SKIP(buf, size)						\
	do {								\
		if (unlikely((buf)->data + size > (buf)->end))		\
			goto underflow;					\
		(buf)->data += size;					\
	} while (0)

#define WSM_GET(buf, ptr, size)						\
	do {								\
		if (unlikely((buf)->data + size > (buf)->end))		\
			goto underflow;					\
		memcpy(ptr, (buf)->data, size);				\
		(buf)->data += size;					\
	} while (0)

#define __WSM_GET(buf, type, cvt)					\
	({								\
		type val;						\
		if (unlikely((buf)->data + sizeof(type) > (buf)->end))	\
			goto underflow;					\
		val = cvt(*(type *)(buf)->data);			\
		(buf)->data += sizeof(type);				\
		val;							\
	})

#define WSM_GET8(buf)  __WSM_GET(buf, u8, (u8))
#define WSM_GET16(buf) __WSM_GET(buf, u16, __le16_to_cpu)
#define WSM_GET32(buf) __WSM_GET(buf, u32, __le32_to_cpu)

#define WSM_PUT(buf, ptr, size)						\
	do {								\
		if (unlikely((buf)->data + size > (buf)->end))		\
			if (unlikely(wsm_buf_reserve((buf), size)))	\
				goto nomem;				\
		memcpy((buf)->data, ptr, size);				\
		(buf)->data += size;					\
	} while (0)

#define __WSM_PUT(buf, val, type, cvt)					\
	do {								\
		if (unlikely((buf)->data + sizeof(type) > (buf)->end))	\
			if (unlikely(wsm_buf_reserve((buf), sizeof(type)))) \
				goto nomem;				\
		*(type *)(buf)->data = cvt(val);			\
		(buf)->data += sizeof(type);				\
	} while (0)

#define WSM_PUT8(buf, val)  __WSM_PUT(buf, val, u8, (u8))
#define WSM_PUT16(buf, val) __WSM_PUT(buf, val, u16, __cpu_to_le16)
#define WSM_PUT32(buf, val) __WSM_PUT(buf, val, u32, __cpu_to_le32)

#define WSM_COM_CMD_SAVE(type, v, cmd_id, qu_id)        \
        do {                                            \
                struct wsm_com_cmd_s *cmd;              \
                if (if_id == -1)                        \
                        if_id = 0;                      \
                if (if_id > 1)                          \
                        break;                          \
                cmd = &hw_priv->wsm_cmds.v[if_id];      \
                if (cmd->buf != arg) {                  \
                        mutex_lock(&hw_priv->wsm_cmd_save_mux); \
                        cmd->valid = 0;                 \
                        cmd->cmd = cmd_id;              \
                        cmd->if_id = if_id;             \
                        cmd->qid = qu_id;               \
                        cmd->args = (void *)arg;        \
                        if (!cmd->buf) {                \
                                cmd->buf = kmalloc(sizeof(type), GFP_KERNEL);   \
                                if (!cmd->buf) {                                \
                                        mutex_unlock(&hw_priv->wsm_cmd_save_mux); \
                                        return -ENOMEM;                         \
                                }                                               \
                                list_add_tail(&cmd->list, &hw_priv->wsm_cmds_list); \
                        }                                                       \
                        memcpy(cmd->buf, arg, sizeof(type));                    \
                        cmd->valid = 1;                                         \
                        mutex_unlock(&hw_priv->wsm_cmd_save_mux);               \
                }                                                               \
        } while (0)

static void wsm_buf_reset(struct wsm_buf *buf);
static int wsm_buf_reserve(struct wsm_buf *buf, size_t extra_size);
static int get_interface_id_scanning(struct rk960_common *hw_priv);

static int wsm_cmd_send(struct rk960_common *hw_priv,
			struct wsm_buf *buf,
			void *arg, u16 cmd, long tmo, int if_id);

static struct rk960_vif
*wsm_get_interface_for_tx(struct rk960_common *hw_priv);

static inline void wsm_cmd_lock(struct rk960_common *hw_priv)
{
	mutex_lock(&hw_priv->wsm_cmd_mux);
}

static inline void wsm_cmd_unlock(struct rk960_common *hw_priv)
{
	mutex_unlock(&hw_priv->wsm_cmd_mux);
}

static inline void wsm_oper_lock(struct rk960_common *hw_priv)
{
	mutex_lock(&hw_priv->wsm_oper_lock);
}

static inline void wsm_oper_unlock(struct rk960_common *hw_priv)
{
	if (mutex_is_locked(&hw_priv->wsm_oper_lock))
		mutex_unlock(&hw_priv->wsm_oper_lock);
}

/* ******************************************************************** */
/* WSM API implementation						*/

char *wsm_conv_req_resp_to_str(int id)
{
	switch (id) {
	case 0x0400:
	case 0x0401:
		return "WSM_MEM_RW_IND";
	case 0x0000:
	case 0x0001:
		return "WSM_MEM_RW_REQ";
	case 0x0801:
		return "WSM_STARTUP_IND";
	case 0x0803:
		return "WSM_GENERIC_IND";
	case 0x0009:
		return "WSM_CONFIGURATION_REQ";
	case 0x0409:
		return "WSM_CONFIGURATION_RESP";
	case 0x000A:
		return "WSM_RESET_REQ";
	case 0x040A:
		return "WSM_RESET_RESP";
	case 0x0005:
		return "WSM_READ_MIB_REQ";
	case 0x0405:
		return "WSM_READ_MIB_RESP";
	case 0x0006:
		return "WSM_WRITE_MIB_REQ";
	case 0x0406:
		return "WSM_WRITE_MIB_RESP";
	case 0x0007:
		return "WSM_START_SCAN_REQ";
	case 0x0407:
		return "WSM_START_SCAN_RESP";
	case 0x0008:
		return "WSM_STOP_SCAN_REQ";
	case 0x0408:
		return "WSM_STOP_SCAN_RESP";
	case 0x0806:
		return "WSM_SCAN_COMPLETE_IND";
	case 0x0404:
		return "WSM_TX_CONFIRM_IND";
	case 0x041E:
		return "WSM_MULTI_TX_CONFIRM";
	case 0x0804:
		return "WSM_RECEIVE_IND";
	case 0x000B:
		return "WSM_JOIN_REQ";
	case 0x040B:
		return "WSM_JOIN_RESP";
	case 0x0010:
		return "WSM_SET_PM_REQ";
	case 0x0410:
		return "WSM_SET_PM_RESP";
	case 0x0011:
		return "WSM_SET_BSS_PARAMS_REQ";
	case 0x0411:
		return "WSM_SET_BSS_PARAMS_RESP";
	case 0x000C:
		return "WSM_ADD_KEY_REQ";
	case 0x040C:
		return "WSM_ADD_KEY_RESP";
	case 0x000D:
		return "WSM_REMOVE_KEY_REQ";
	case 0x040D:
		return "WSM_REMOVE_KEY_RESP";
	case 0x0013:
		return "WSM_EDCA_PARAMS_REQ";
	case 0x0413:
		return "WSM_EDCA_PARAMS_RESP";
	case 0x0016:
		return "WSM_SWITCH_CHANNEL_REQ";
	case 0x0416:
		return "WSM_SWITCH_CHANNEL_RESP";
	case 0x0017:
		return "WSM_START_REQ";
	case 0x0417:
		return "WSM_START_RESP";
	case 0x0018:
		return "WSM_BEACON_TRANSMIT_REQ";
	case 0x0418:
		return "WSM_BEACON_TRANSMIT_RESP";
	case 0x0012:
		return "WSM_TXQUEUE_PARAMS_REQ";
	case 0x0412:
		return "WSM_TXQUEUE_PARAMS_RESP";
	case 0x0019:
		return "WSM_START_FIND_REQ";
	case 0x0419:
		return "WSM_START_FIND_RESP";
	case 0x001A:
		return "WSM_STOP_FIND_REQ";
	case 0x041A:
		return "WSM_STOP_FIND_RESP";
	case 0x001B:
		return "WSM_UPDATE_IE_REQ";
	case 0x041B:
		return "WSM_UPDATE_IE_RESP";
	case 0x001C:
		return "WSM_MAP_LINK_REQ";
	case 0x041C:
		return "WSM_MAP_LINK_RESP";
	case 0x0024:
		return "WSM_ADC_READ_REQ";
	case 0x0424:
		return "WSM_ADC_READ_RESP";
	case 0x0805:
		return "WSM_EVENT_IND";
	case 0x0809:
		return "WSM_SET_PM_IND";
	case 0x080C:
		return "WSM_SUS_RES_IND";
	case 0x0810:
		return "WSM_SCHED_SCAN_COMPLETE_IND";
	default:
		return "UNKNOW";
	}
}

static int wsm_reset_cmd_save(struct rk960_common *hw_priv,
			      const struct wsm_reset *arg, int if_id)
{
	struct wsm_com_cmd_s *cmd;

	if (if_id == RK960_GENERIC_IF_ID)
		return 0;
	if (arg->link_id)
		return 0;

	mutex_lock(&hw_priv->wsm_cmd_save_mux);

	cmd = &hw_priv->wsm_cmds.wsm_join_cmd[if_id];
	if (cmd->valid && cmd->buf) {
		cmd->valid = 0;
		kfree(cmd->buf);
		cmd->buf = NULL;
		list_del(&cmd->list);
		// delete wsm_set_bss_params_cmd
		cmd = &hw_priv->wsm_cmds.wsm_set_bss_params_cmd[if_id];
		if (cmd->valid && cmd->buf) {
			cmd->valid = 0;
			kfree(cmd->buf);
			cmd->buf = NULL;
			list_del(&cmd->list);
		}
	}
	cmd = &hw_priv->wsm_cmds.wsm_start_cmd[if_id];
	if (cmd->valid && cmd->buf) {
		cmd->valid = 0;
		kfree(cmd->buf);
		cmd->buf = NULL;
		list_del(&cmd->list);
	}

	mutex_unlock(&hw_priv->wsm_cmd_save_mux);
	return 0;
}

static int wsm_join_confirm_cmd_save(struct rk960_common *hw_priv,
				     struct wsm_join *arg)
{
	struct wsm_com_cmd_s *cmd;
	int if_id;

	if (hw_priv->wsm_cmds.wsm_join_cmd[0].args == arg)
		if_id = 0;
	else
		if_id = 1;

	mutex_lock(&hw_priv->wsm_cmd_save_mux);

	cmd = &hw_priv->wsm_cmds.wsm_join_cmd[if_id];
	cmd->valid = 0;
	if (cmd->buf) {
		kfree(cmd->buf);
		cmd->buf = NULL;
		list_del(&cmd->list);
	}

	mutex_unlock(&hw_priv->wsm_cmd_save_mux);
	return 0;
}

static int wsm_add_key_cmd_save(struct rk960_common *hw_priv,
				const struct wsm_add_key *arg, int if_id)
{
	struct wsm_com_cmd_s *cmd;

	cmd = &hw_priv->wsm_cmds.wsm_add_key_cmd[arg->entryIndex];
	if (cmd->buf != arg) {
		mutex_lock(&hw_priv->wsm_cmd_save_mux);

		cmd->valid = 0;
		cmd->cmd = 0x000C;
		cmd->if_id = if_id;
		if (!cmd->buf) {
			list_add_tail(&cmd->list, &hw_priv->wsm_cmds_list);
		}
		cmd->buf = (void *)arg;
		cmd->valid = 1;

		mutex_unlock(&hw_priv->wsm_cmd_save_mux);
	}

	return 0;
}

static int wsm_remove_key_cmd_save(struct rk960_common *hw_priv,
				   const struct wsm_remove_key *arg, int if_id)
{
	struct wsm_com_cmd_s *cmd;

	mutex_lock(&hw_priv->wsm_cmd_save_mux);

	cmd = &hw_priv->wsm_cmds.wsm_add_key_cmd[arg->entryIndex];
	if (cmd->buf) {
		cmd->buf = NULL;
		cmd->valid = 0;
		list_del(&cmd->list);
	}

	mutex_unlock(&hw_priv->wsm_cmd_save_mux);
	return 0;
}

static int wsm_write_mib_cmd_save(struct rk960_common *hw_priv,
				  u16 mibId, void *_buf, size_t buf_size,
				  int if_id)
{
	struct wsm_com_cmd_s *cmd;
	struct rk960_sended_wsm_cmds_s *cmds;
	int idx;

	BUG_ON(if_id > 1);

	cmds = &hw_priv->wsm_cmds;
	if (mibId == WSM_MIB_ID_SET_CSYNC_THR ||
	    mibId == WSM_MIB_ID_WRITE_EFUSE ||
	    mibId == WSM_MIB_ID_SDIO_DATA_WIDTH ||
	    mibId == WSM_MIB_ID_SET_FW_RECOVERY ||
	    mibId == WSM_MIB_ID_SET_TX_RATE_RETRY_POLICY)
		return 0;

	if (if_id == -1)
		if_id = 0;
	if (mibId == WSM_MIB_ID_TEMPLATE_FRAME) {
		idx = *((u8 *) _buf);
		BUG_ON(idx >= 8);
		cmd = &cmds->wsm_wmib_tem_fra_cmd[if_id][idx];
	} else {
		idx = mibId & 0xfff;
		BUG_ON(idx >= 128);
		cmd = &cmds->wsm_wmib_cmd[if_id][idx];
	}
	if (cmd->buf != _buf) {
		RK960_DEBUG_FWREC
		    ("%s: idx %d if_id %d mibId 0x%04x buf_size %d", __func__,
		     idx, if_id, mibId, (int)buf_size);
		mutex_lock(&hw_priv->wsm_cmd_save_mux);

		cmd->valid = 0;
		cmd->cmd = 0x0006;
		cmd->if_id = if_id;
		cmd->mibId = mibId;
		if (cmd->buf && cmd->buf_size < buf_size) {
			kfree(cmd->buf);
			cmd->buf = kmalloc(buf_size, GFP_KERNEL);
			if (!cmd->buf) {
				mutex_unlock(&hw_priv->wsm_cmd_save_mux);
				return -ENOMEM;
			}
		}
		cmd->buf_size = buf_size;
		if (!cmd->buf) {
			cmd->buf = kmalloc(buf_size, GFP_KERNEL);
			if (!cmd->buf) {
				mutex_unlock(&hw_priv->wsm_cmd_save_mux);
				return -ENOMEM;
			}
			list_add_tail(&cmd->list, &hw_priv->wsm_cmds_list);
		}
		memcpy(cmd->buf, _buf, buf_size);
		cmd->valid = 1;

		mutex_unlock(&hw_priv->wsm_cmd_save_mux);
	}

	return 0;
}

static int wsm_set_tx_rate_retry_policy_save_cmd(struct rk960_common *hw_priv,
						 u16 mibId, void *_buf,
						 size_t buf_size, int if_id)
{
	struct wsm_com_cmd_s *cmd;
	struct rk960_sended_wsm_cmds_s *cmds;
	struct wsm_set_tx_rate_retry_policy *arg;
	int i;
	int idx = 0;

	cmds = &hw_priv->wsm_cmds;
	cmd = &cmds->wsm_wmib_tx_pol_cmd[if_id];
	if (cmd->buf == _buf)
		return 0;

	RK960_DEBUG_FWREC("%s: idx %d if_id %d mibId 0x%04x buf_size %d",
			  __func__, idx, if_id, mibId, (int)buf_size);

	arg = (struct wsm_set_tx_rate_retry_policy *)_buf;
	for (i = 0; i < arg->hdr.numTxRatePolicies; i++) {
		idx = arg->tbl[i].policyIndex;
		//RK960_DEBUG_FWREC("i %d idx %d\n", i, idx);
		memcpy(&hw_priv->wsm_tx_rate.tbl[idx], &arg->tbl[i],
		       sizeof(struct wsm_set_tx_rate_retry_policy_policy));
	}
	idx++;
	if (idx > hw_priv->wsm_tx_rate.hdr.numTxRatePolicies)
		hw_priv->wsm_tx_rate.hdr.numTxRatePolicies = idx;

	BUG_ON(hw_priv->wsm_tx_rate.hdr.numTxRatePolicies > 8);

	//RK960_DEBUG_FWREC("arg %d save %d\n",
	//        arg->hdr.numTxRatePolicies,
	//        hw_priv->wsm_tx_rate.hdr.numTxRatePolicies);

	mutex_lock(&hw_priv->wsm_cmd_save_mux);

	cmd->valid = 0;
	cmd->cmd = 0x0006;
	cmd->if_id = if_id;
	cmd->mibId = mibId;
	cmd->buf_size =
	    sizeof(struct wsm_set_tx_rate_retry_policy_header) +
	    hw_priv->wsm_tx_rate.hdr.numTxRatePolicies *
	    sizeof(struct wsm_set_tx_rate_retry_policy_policy);
	if (!cmd->buf) {
		cmd->buf = (void *)&hw_priv->wsm_tx_rate;
		list_add_tail(&cmd->list, &hw_priv->wsm_cmds_list);
	}
	cmd->valid = 1;

	mutex_unlock(&hw_priv->wsm_cmd_save_mux);

	return 0;
}

static int wsm_map_link_save_cmd(struct rk960_common *hw_priv,
				 const struct wsm_map_link *arg, int if_id)
{
	struct wsm_com_cmd_s *cmd;

	if (if_id == -1)
		if_id = 0;
	if (if_id > 1 || arg->link_id >= WLAN_LINK_ID_MAX)
		return 0;

	cmd = &hw_priv->wsm_cmds.wsm_map_link_cmd[if_id][arg->link_id];
	if (cmd->buf != arg) {
		mutex_lock(&hw_priv->wsm_cmd_save_mux);

		cmd->valid = 0;
		cmd->cmd = 0x001C;
		cmd->if_id = if_id;
		if (!cmd->buf) {
			cmd->buf = kmalloc(sizeof(*arg), GFP_KERNEL);
			if (!cmd->buf) {
				mutex_unlock(&hw_priv->wsm_cmd_save_mux);
				return -ENOMEM;
			}
			list_add_tail(&cmd->list, &hw_priv->wsm_cmds_list);
		}
		memcpy(cmd->buf, arg, sizeof(*arg));
		cmd->valid = 1;

		mutex_unlock(&hw_priv->wsm_cmd_save_mux);
	}

	return 0;
}

static int wsm_unmap_link_save_cmd(struct rk960_common *hw_priv,
				   const struct wsm_map_link *arg, int if_id)
{
	struct wsm_com_cmd_s *cmd;

	if (if_id == -1)
		if_id = 0;
	if (if_id > 1 || arg->link_id >= WLAN_LINK_ID_MAX)
		return 0;

	mutex_lock(&hw_priv->wsm_cmd_save_mux);

	cmd = &hw_priv->wsm_cmds.wsm_map_link_cmd[if_id][arg->link_id];
	cmd->valid = 0;
	if (cmd->buf) {
		kfree(cmd->buf);
		cmd->buf = NULL;
		list_del(&cmd->list);
	}

	mutex_unlock(&hw_priv->wsm_cmd_save_mux);
	return 0;
}

static int wsm_update_ie_cmd_save(struct rk960_common *hw_priv,
				  const struct wsm_update_ie *arg, int if_id)
{
	struct wsm_com_cmd_s *cmd;
	struct wsm_update_ie *argd;
	int idx;

	BUG_ON(if_id > 1);

	if (if_id == -1)
		if_id = 0;
	if (arg->what == WSM_UPDATE_IE_BEACON)
		idx = 0;
	else if (arg->what == WSM_UPDATE_IE_PROBE_RESP)
		idx = 1;
	else
		idx = 2;

	cmd = &hw_priv->wsm_cmds.wsm_update_ie_cmd[if_id][idx];
	if (cmd->buf != arg) {
		mutex_lock(&hw_priv->wsm_cmd_save_mux);

		cmd->valid = 0;
		cmd->cmd = 0x001B;
		cmd->if_id = if_id;
		if (!cmd->buf) {
			cmd->buf = kzalloc(sizeof(*arg), GFP_KERNEL);
			if (!cmd->buf) {
				mutex_unlock(&hw_priv->wsm_cmd_save_mux);
				return -ENOMEM;
			}
			list_add_tail(&cmd->list, &hw_priv->wsm_cmds_list);
		}
		argd = (struct wsm_update_ie *)cmd->buf;
		if (argd->ies)
			kfree(argd->ies);
		memcpy(cmd->buf, arg, sizeof(*arg));
		argd->ies = kmalloc(arg->length, GFP_KERNEL);
		if (!argd->ies) {
			mutex_unlock(&hw_priv->wsm_cmd_save_mux);
			return -ENOMEM;
		}
		memcpy(argd->ies, arg->ies, arg->length);
		cmd->valid = 1;

		mutex_unlock(&hw_priv->wsm_cmd_save_mux);
	}

	return 0;
}

int wsm_send_fw_recovery_cmd(struct rk960_common *hw_priv)
{
	struct wsm_com_cmd_s *cmd;
	struct wsm_join *join;
	int i;

	RK960_INFO_FWREC("%s\n", __func__);

	wsm_oper_unlock(hw_priv);

	list_for_each_entry(cmd, &hw_priv->wsm_cmds_list, list) {
		RK960_DEBUG_FWREC("cmd 0x%04x mibId 0x%04x valid %d\n",
				  cmd->cmd, cmd->mibId, cmd->valid);
		if (WARN_ON(!cmd->valid || !cmd->buf))
			continue;

		if (cmd->cmd == 0x0006) {
			wsm_write_mib(hw_priv, cmd->mibId, cmd->buf,
				      cmd->buf_size, cmd->if_id);
		} else if (cmd->cmd == 0x0009) {
			wsm_configuration(hw_priv,
					  (struct wsm_configuration *)cmd->buf,
					  cmd->if_id);
		} else if (cmd->cmd == 0x000B) {
			join = (struct wsm_join *)cmd->buf;
			join->probeForJoin = 0;
			join->flags |= WSM_JOIN_FLAGS_FORCE;
			wsm_join(hw_priv,
				 (struct wsm_join *)cmd->buf, cmd->if_id);
		} else if (cmd->cmd == 0x0011) {
			wsm_set_bss_params(hw_priv,
					   (struct wsm_set_bss_params *)cmd->
					   buf, cmd->if_id);
		} else if (cmd->cmd == 0x000C) {
			wsm_add_key(hw_priv,
				    (struct wsm_add_key *)cmd->buf, cmd->if_id);
		} else if (cmd->cmd == 0x0012) {
			wsm_set_tx_queue_params(hw_priv,
						(struct wsm_set_tx_queue_params
						 *)cmd->buf, cmd->qid,
						cmd->if_id);
		} else if (cmd->cmd == 0x0013) {
			wsm_set_edca_params(hw_priv,
					    (struct wsm_edca_params *)cmd->buf,
					    cmd->if_id);
		} else if (cmd->cmd == 0x0010) {
			/*//struct wsm_set_pm *pm;
			   //pm = (struct wsm_set_pm *)cmd->buf;
			   //pm->pmMode = WSM_PSM_ACTIVE;
			   wsm_set_pm(hw_priv,
			   (struct wsm_set_pm *)cmd->buf,
			   cmd->if_id); */
		} else if (cmd->cmd == 0x0017) {
			wsm_start(hw_priv,
				  (struct wsm_start *)cmd->buf, cmd->if_id);
		} else if (cmd->cmd == 0x001C) {
			wsm_map_link(hw_priv,
				     (struct wsm_map_link *)cmd->buf,
				     cmd->if_id);
		} else if (cmd->cmd == 0x001B) {
			wsm_update_ie(hw_priv,
				      (struct wsm_update_ie *)cmd->buf,
				      cmd->if_id);
		} else {
			RK960_ERROR_FWREC("%s: error cmd 0x%04x\n",
					  __func__, cmd->cmd);
			WARN_ON(1);
		}
	}

	// set pm at last
	for (i = 0; i < 2; i++) {
		cmd = &hw_priv->wsm_cmds.wsm_set_pm_cmd[i];
		if (!cmd->valid || !cmd->buf)
			continue;
		wsm_set_pm(hw_priv, (struct wsm_set_pm *)cmd->buf, cmd->if_id);
	}

	return 0;
}

void wsm_sended_cmds_init(struct rk960_common *hw_priv)
{
	mutex_lock(&hw_priv->wsm_cmd_save_mux);

	memset(&hw_priv->wsm_cmds, 0, sizeof(struct rk960_sended_wsm_cmds_s));
	memset(&hw_priv->wsm_tx_rate, 0,
	       sizeof(struct wsm_set_tx_rate_retry_policy));

	mutex_unlock(&hw_priv->wsm_cmd_save_mux);
}

void wsm_sended_cmds_deinit(struct rk960_common *hw_priv)
{
	struct wsm_com_cmd_s *cmd;

	mutex_lock(&hw_priv->wsm_cmd_save_mux);

	list_for_each_entry(cmd, &hw_priv->wsm_cmds_list, list) {
		if (cmd->buf) {
			if (cmd->cmd == 0x000C)
				continue;
			if (cmd->cmd == 0x0006 &&
			    cmd->mibId == WSM_MIB_ID_SET_TX_RATE_RETRY_POLICY)
				continue;
			if (cmd->cmd == 0x001B) {
				struct wsm_update_ie *argd;

				argd = (struct wsm_update_ie *)cmd->buf;
				if (argd->ies)
					kfree(argd->ies);
			}
			kfree(cmd->buf);
		}
	}

	mutex_unlock(&hw_priv->wsm_cmd_save_mux);
}

static int wsm_mem_rw_confirm(struct rk960_common *hw_priv,
			      void *arg, struct wsm_buf *buf)
{
	u32 status = WSM_GET32(buf);

	if (status != WSM_STATUS_SUCCESS) {
		RK960_ERROR_WSM("%s: failed (status %d)\n", __func__, status);
		return -EINVAL;
	}

	memcpy(hw_priv->debug_dump_data, buf->data, hw_priv->mem_rw_len);
	hw_priv->debug_dump_ready = 1;
	wake_up(&hw_priv->debug_dump_done);

	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

static int wsm_adc_read_confirm(struct rk960_common *hw_priv,
				void *arg, struct wsm_buf *buf)
{
	u32 status = WSM_GET32(buf);

	if (status != WSM_STATUS_SUCCESS) {
		RK960_ERROR_WSM("%s: failed (status %d)\n", __func__, status);
		return -EINVAL;
	}

	WSM_GET32(buf);
	WSM_GET32(buf);
	memcpy(hw_priv->debug_dump_data + hw_priv->debug_adc_offset,
	       buf->data, hw_priv->debug_adc_block);
	hw_priv->debug_adc_offset += hw_priv->debug_adc_block;
	if (hw_priv->debug_adc_offset == hw_priv->debug_adc_size) {
		hw_priv->debug_dump_ready = 1;
		wake_up(&hw_priv->debug_dump_done);
	}

	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

static int wsm_generic_confirm(struct rk960_common *hw_priv,
			       void *arg, struct wsm_buf *buf)
{
	u32 status = WSM_GET32(buf);
	if (status != WSM_STATUS_SUCCESS) {
		RK960_ERROR_WSM("%s: failed (status %d)\n", __func__, status);
		return -EINVAL;
	}
	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

int wsm_configuration(struct rk960_common *hw_priv,
		      struct wsm_configuration *arg, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	WSM_COM_CMD_SAVE(struct wsm_configuration,
			 wsm_configuration_cmd, 0x0009, 0);

	wsm_cmd_lock(hw_priv);

	WSM_PUT32(buf, arg->dot11MaxTransmitMsduLifeTime);
	WSM_PUT32(buf, arg->dot11MaxReceiveLifeTime);
	WSM_PUT32(buf, arg->dot11RtsThreshold);

	/* DPD block. */
	WSM_PUT16(buf, arg->dpdData_size + 12);
	WSM_PUT16(buf, 1);	/* DPD version */
	WSM_PUT(buf, arg->dot11StationId, ETH_ALEN);
	WSM_PUT16(buf, 5);	/* DPD flags */
	if (arg->dpdData_size)
		WSM_PUT(buf, arg->dpdData, arg->dpdData_size);

	ret = wsm_cmd_send(hw_priv, buf, arg, 0x0009, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

static int wsm_configuration_confirm(struct rk960_common *hw_priv,
				     struct wsm_configuration *arg,
				     struct wsm_buf *buf)
{
	int i;
	int status;

	status = WSM_GET32(buf);
	if (WARN_ON(status != WSM_STATUS_SUCCESS))
		return -EINVAL;

	WSM_GET(buf, arg->dot11StationId, ETH_ALEN);
	arg->dot11FrequencyBandsSupported = WSM_GET8(buf);
	WSM_SKIP(buf, 1);
	arg->supportedRateMask = WSM_GET32(buf);
	for (i = 0; i < 2; ++i) {
		arg->txPowerRange[i].min_power_level = WSM_GET32(buf);
		arg->txPowerRange[i].max_power_level = WSM_GET32(buf);
		arg->txPowerRange[i].stepping = WSM_GET32(buf);
	}
	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

/* ******************************************************************** */

int wsm_mem_rw(struct rk960_common *hw_priv, const struct wsm_mem_rw *arg,
	       int rw, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	u16 cmd;

	if (rw)			// read
		cmd = 0x0000 | WSM_TX_LINK_ID(arg->link_id);
	else			// write
		cmd = 0x0001 | WSM_TX_LINK_ID(arg->link_id);

	wsm_cmd_lock(hw_priv);

	WSM_PUT32(buf, arg->address);
	WSM_PUT16(buf, arg->length);
	WSM_PUT16(buf, arg->flags);
	if (!rw)		// only support write one word now
		WSM_PUT32(buf, arg->data[0]);
	ret = wsm_cmd_send(hw_priv, buf, NULL, cmd, WSM_CMD_RESET_TIMEOUT,
			   if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

int wsm_adc_data_read(struct rk960_common *hw_priv,
		      const struct wsm_adc_data_r *arg, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	u16 cmd;

	cmd = 0x0024;

	wsm_cmd_lock(hw_priv);

	WSM_PUT32(buf, arg->size);
	WSM_PUT32(buf, arg->block);
	WSM_PUT32(buf, arg->offset);
	WSM_PUT32(buf, arg->channel);
	WSM_PUT32(buf, arg->count);
	WSM_PUT32(buf, arg->abbf);
	WSM_PUT32(buf, arg->lna);
	ret = wsm_cmd_send(hw_priv, buf, NULL, cmd, WSM_CMD_RESET_TIMEOUT,
			   if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

int wsm_reset(struct rk960_common *hw_priv, const struct wsm_reset *arg,
	      int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	u16 cmd = 0x000A | WSM_TX_LINK_ID(arg->link_id);

	if (if_id == RK960_GENERIC_IF_ID && hw_priv->fw_error_processing)
		return -ENOMEM;

	if (wsm_reset_cmd_save(hw_priv, arg, if_id))
		return -ENOMEM;

	wsm_cmd_lock(hw_priv);

	WSM_PUT32(buf, arg->reset_statistics ? 0 : 1);
	ret = wsm_cmd_send(hw_priv, buf, NULL, cmd, WSM_CMD_RESET_TIMEOUT,
			   if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

struct wsm_mib {
	u16 mibId;
	void *buf;
	size_t buf_size;
};

int wsm_read_mib(struct rk960_common *hw_priv, u16 mibId, void *_buf,
		 size_t buf_size)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	struct wsm_mib mib_buf = {
		.mibId = mibId,
		.buf = _buf,
		.buf_size = buf_size,
	};

	if (hw_priv->fw_error_processing)
		return -EINVAL;

	wsm_cmd_lock(hw_priv);

	WSM_PUT16(buf, mibId);
	WSM_PUT16(buf, 0);

	ret = wsm_cmd_send(hw_priv, buf, &mib_buf, 0x0005, WSM_CMD_TIMEOUT, -1);
	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

static int wsm_read_mib_confirm(struct rk960_common *hw_priv,
				struct wsm_mib *arg, struct wsm_buf *buf)
{
	u16 size;
	if (WARN_ON(WSM_GET32(buf) != WSM_STATUS_SUCCESS))
		return -EINVAL;

	if (WARN_ON(WSM_GET16(buf) != arg->mibId))
		return -EINVAL;

	size = WSM_GET16(buf);
	if (size > arg->buf_size)
		size = arg->buf_size;

	WSM_GET(buf, arg->buf, size);
	arg->buf_size = size;
	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

/* ******************************************************************** */

int wsm_write_mib(struct rk960_common *hw_priv, u16 mibId, void *_buf,
		  size_t buf_size, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	struct wsm_mib mib_buf = {
		.mibId = mibId,
		.buf = _buf,
		.buf_size = buf_size,
	};

	if (mibId == WSM_MIB_ID_SET_TX_RATE_RETRY_POLICY)
		wsm_set_tx_rate_retry_policy_save_cmd(hw_priv,
						      mibId, _buf, buf_size,
						      if_id);

	if (wsm_write_mib_cmd_save(hw_priv, mibId, _buf, buf_size, if_id))
		return -ENOMEM;

	wsm_cmd_lock(hw_priv);

	WSM_PUT16(buf, mibId);
	WSM_PUT16(buf, buf_size);
	WSM_PUT(buf, _buf, buf_size);

	ret = wsm_cmd_send(hw_priv, buf, &mib_buf, 0x0006, WSM_CMD_TIMEOUT,
			   if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

static int wsm_write_mib_confirm(struct rk960_common *hw_priv,
				 struct wsm_mib *arg,
				 struct wsm_buf *buf, int interface_link_id)
{
	int ret;
	struct rk960_vif *priv;

	if (!is_hardware_version1(hw_priv) || is_hardware_version2(hw_priv))
		interface_link_id = 0;

	ret = wsm_generic_confirm(hw_priv, arg, buf);
	if (ret)
		return ret;

	if (arg && arg->mibId == 0x1006) {
		const char *p = arg->buf;

		/* Power save is enabled before add_interface is called */
		if (!hw_priv->vif_list[interface_link_id])
			return 0;
		/* OperationalMode: update PM status. */
		priv = rk960_hwpriv_to_vifpriv(hw_priv, interface_link_id);
		if (!priv)
			return 0;
		rk960_enable_powersave(priv, (p[0] & 0x0F) ? true : false);
		rk960_priv_vif_list_read_unlock(&priv->vif_lock);
	}
	return 0;
}

int wsm_set_csync_thr(struct rk960_common *hw_priv, s8 csync, int if_id)
{
#ifdef RK960_CSYNC_ADJUST
	struct wsm_csync_thr arg;

	if (csync == hw_priv->fw_csync)
		return 0;

	RK960_DEBUG_WSM("%s: %d\n", __func__, csync);
	arg.csync = csync;
	hw_priv->fw_csync = csync;
	if (csync <= -80)
		arg.csync = 0;
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_CSYNC_THR,
			     &arg, sizeof(arg), if_id);
#else
	return 0;
#endif
}

int wsm_set_output_power(struct rk960_common *hw_priv,
			 int power_level, int if_id)
{
	__le32 val = __cpu_to_le32(power_level);

	hw_priv->fw_output_power = power_level;
	return wsm_write_mib(hw_priv, WSM_MIB_ID_DOT11_CURRENT_TX_POWER_LEVEL,
			     &val, sizeof(val), if_id);
}

int wsm_set_txpwr_comp(struct rk960_common *hw_priv)
{
	return wsm_write_mib(hw_priv, WSM_MIB_ID_SET_TXPWR_COMP,
			     hw_priv->txpwr_comp_tbl,
			     sizeof(hw_priv->txpwr_comp_tbl), 0);
}

int wsm_set_beacon_wakeup_period(struct rk960_common *hw_priv,
					       unsigned dtim_interval,
					       unsigned listen_interval,
					       int if_id)
{
	struct {
		u8 numBeaconPeriods;
		u8 reserved;
		__le16 listenInterval;
	} val = {
	dtim_interval, 0, __cpu_to_le16(listen_interval)};

	hw_priv->dtim_interval = dtim_interval;
	hw_priv->listen_interval = listen_interval;
	//pr_info("dtim_interval %d listen_interval %d\n", dtim_interval, listen_interval);
	if (dtim_interval > 0xFF || listen_interval > 0xFFFF)
		return -EINVAL;
	else
		return wsm_write_mib(hw_priv, WSM_MIB_ID_BEACON_WAKEUP_PERIOD,
				     &val, sizeof(val), if_id);
}

/* ******************************************************************** */

int wsm_scan(struct rk960_common *hw_priv, const struct wsm_scan *arg,
	     int if_id)
{
	int i;
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	int rftest = 0;

	if (hw_priv->fw_error_processing)
		return -EINVAL;

	if (unlikely(arg->numOfChannels > 48))
		return -EINVAL;

	/*if (unlikely(arg->numOfSSIDs > WSM_SCAN_MAX_NUM_OF_SSIDS))
	   return -EINVAL; */

	if (unlikely(arg->band > 1))
		return -EINVAL;

	if (arg->numOfProbeRequests >= 0xf0) {
		RK960_INFO_WSM("%s: for rftest\n", __func__);
		rftest = 1;
	}

	if (!rftest)
		wsm_oper_lock(hw_priv);
	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->band);
	WSM_PUT8(buf, arg->scanType);
	WSM_PUT8(buf, arg->scanFlags);
	WSM_PUT8(buf, arg->maxTransmitRate);
	WSM_PUT32(buf, arg->autoScanInterval);
	WSM_PUT8(buf, arg->numOfProbeRequests);
	WSM_PUT8(buf, arg->numOfChannels);
	WSM_PUT8(buf, arg->numOfSSIDs);
	WSM_PUT8(buf, arg->probeDelay);

        /*pr_info("band %x scanType %x scanFlags %x\n",
                arg->band, arg->scanType, arg->scanFlags);
        pr_info("maxTransmitRate %x autoScanInterval %x"
                " numOfProbeRequests %x\n",
                arg->maxTransmitRate, arg->autoScanInterval,
                arg->numOfProbeRequests);
        pr_info("numOfChannels %x numOfSSIDs %x probeDelay %x\n",
                arg->numOfChannels, arg->numOfSSIDs,
                arg->probeDelay);*/
	for (i = 0; i < arg->numOfChannels; ++i) {
                /*pr_info("ch %d minChannelTime %d maxChannelTime %d\n",
                        arg->ch[i].number,
                        arg->ch[i].minChannelTime,
                        arg->ch[i].maxChannelTime);*/
		WSM_PUT16(buf, arg->ch[i].number);
		WSM_PUT16(buf, 0);
		WSM_PUT32(buf, arg->ch[i].minChannelTime);
		WSM_PUT32(buf, arg->ch[i].maxChannelTime);
		WSM_PUT32(buf, 0);
	}

	for (i = 0; i < arg->numOfSSIDs; ++i) {
                //pr_info("ssid len %d\n", arg->ssids[i].length);
		WSM_PUT32(buf, arg->ssids[i].length);
		WSM_PUT(buf, &arg->ssids[i].ssid[0],
			sizeof(arg->ssids[i].ssid));
	}

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0007, WSM_CMD_TIMEOUT, if_id);
	wsm_cmd_unlock(hw_priv);
	if (ret && !rftest)
		wsm_oper_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	if (!rftest)
		wsm_oper_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_stop_scan(struct rk960_common *hw_priv, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	if (hw_priv->fw_error_processing)
		return -EINVAL;

	wsm_cmd_lock(hw_priv);
	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0008, WSM_CMD_TIMEOUT, if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;
}

static int wsm_tx_confirm(struct rk960_common *hw_priv,
			  struct wsm_buf *buf, int interface_link_id, int last)
{
	struct wsm_tx_confirm tx_confirm;

#ifdef RK960_SDIO_TX_TP_TEST
	return 0;
#endif

	tx_confirm.packetID = WSM_GET32(buf);
	tx_confirm.status = WSM_GET32(buf);
	tx_confirm.txedRate = WSM_GET8(buf);
	tx_confirm.ackFailures = WSM_GET8(buf);
	tx_confirm.flags = WSM_GET16(buf);
	tx_confirm.mediaDelay = WSM_GET32(buf);
	tx_confirm.txQueueDelay = WSM_GET32(buf);
	if (last && (buf->end - buf->data >= sizeof(struct wsm_rec_info)))
		WSM_GET(buf, &hw_priv->wsm_rec_info,
			sizeof(struct wsm_rec_info));

	if (is_hardware_version1(hw_priv) || is_hardware_version2(hw_priv)) {
		/* TODO:COMBO:linkID will be stored in packetID */
		/* TODO:COMBO: Extract traffic resumption map */
		tx_confirm.if_id = rk960_queue_get_if_id(tx_confirm.packetID);
		tx_confirm.link_id =
		    rk960_queue_get_link_id(tx_confirm.packetID);
	} else {
		tx_confirm.link_id = interface_link_id;
		tx_confirm.if_id = 0;
	}

	RK960_DEBUG_WSM("wsm_tx_confirm: pid %x sta %d rate %d"
			" ackf %d flags %d media %d txq %d link_id %d if_id %d %d\n",
			tx_confirm.packetID,
			tx_confirm.status, tx_confirm.txedRate,
			tx_confirm.ackFailures, tx_confirm.flags,
			tx_confirm.mediaDelay, tx_confirm.txQueueDelay,
			tx_confirm.link_id, tx_confirm.if_id,
			hw_priv->hw_bufs_used);

	wsm_release_vif_tx_buffer(hw_priv, tx_confirm.if_id, 1);

	if (hw_priv->wsm_cbc.tx_confirm)
		hw_priv->wsm_cbc.tx_confirm(hw_priv, &tx_confirm);
#ifdef DISABLE_MULTI_WSM_TX
	if (hw_priv->tx_pending != 1)
		WARN_ON(1);
	hw_priv->tx_pending--;
	rk960_bh_wakeup(hw_priv);
#endif
	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

int rk960_pending_tx_confirm(struct rk960_common *hw_priv,
			     int if_id, u32 packetID)
{
	struct wsm_tx_confirm tx_confirm;

	memset(&tx_confirm, 0, sizeof(struct wsm_tx_confirm));
	tx_confirm.packetID = packetID;
	tx_confirm.status = WSM_STATUS_RETRY_EXCEEDED;
	tx_confirm.ackFailures = 10;
	tx_confirm.if_id = if_id;

	if (hw_priv->hw_bufs_used_vif[if_id] >= 1)
		wsm_release_vif_tx_buffer(hw_priv, tx_confirm.if_id, 1);

	if (hw_priv->wsm_cbc.tx_confirm)
		hw_priv->wsm_cbc.tx_confirm(hw_priv, &tx_confirm);

	return 0;
}

static int wsm_multi_tx_confirm(struct rk960_common *hw_priv,
				struct wsm_buf *buf, int interface_link_id)
{
//      struct rk960_vif *priv;
	int ret;
	int count;
	int i;

	count = WSM_GET32(buf);
	//pr_info("tx multi conf %d\n", count);
	if (WARN_ON(count <= 0))
		return -EINVAL;
	else if (count > 1) {
		ret = wsm_release_tx_buffer(hw_priv, count - 1);
		if (ret < 0)
			return ret;
		else if (ret > 0)
			rk960_bh_wakeup(hw_priv);
	}
	/*priv = rk960_hwpriv_to_vifpriv(hw_priv, interface_link_id);
	   if (priv) {
	   rk960_debug_txed_multi(priv, count);
	   rk960_priv_vif_list_read_unlock(&priv->vif_lock);
	   } */
	for (i = 0; i < count; ++i) {
		ret = wsm_tx_confirm(hw_priv, buf, interface_link_id,
				     i + 1 == count);
		if (ret)
			return ret;
	}
	return ret;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

/* ******************************************************************** */

static int wsm_join_confirm(struct rk960_common *hw_priv,
			    struct wsm_join *arg, struct wsm_buf *buf)
{
	u32 status;

	wsm_oper_unlock(hw_priv);

	status = WSM_GET32(buf);
	if ( /*WARN_ON */ (status != WSM_STATUS_SUCCESS)) {
		RK960_ERROR_WSM("%s faile status %d\n", __func__, status);
		wsm_join_confirm_cmd_save(hw_priv, arg);
		return -EINVAL;
	}

	arg->minPowerLevel = WSM_GET32(buf);
	arg->maxPowerLevel = WSM_GET32(buf);

	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

int wsm_join(struct rk960_common *hw_priv, struct wsm_join *arg, int if_id)
/*TODO: combo: make it work per vif.*/
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	WSM_COM_CMD_SAVE(struct wsm_join, wsm_join_cmd, 0x000B, 0);

	wsm_oper_lock(hw_priv);
	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->mode);
	WSM_PUT8(buf, arg->band);
	WSM_PUT16(buf, arg->channelNumber);
	WSM_PUT(buf, &arg->bssid[0], sizeof(arg->bssid));
	WSM_PUT16(buf, arg->atimWindow);
	WSM_PUT8(buf, arg->preambleType);
	WSM_PUT8(buf, arg->probeForJoin);
	WSM_PUT8(buf, arg->dtimPeriod);
	WSM_PUT8(buf, arg->flags);
	WSM_PUT32(buf, arg->ssidLength);
	WSM_PUT(buf, &arg->ssid[0], sizeof(arg->ssid));
	WSM_PUT32(buf, arg->beaconInterval);
	WSM_PUT32(buf, arg->basicRateSet);

	hw_priv->tx_burst_idx = -1;
	ret = wsm_cmd_send(hw_priv, buf, arg, 0x000B, WSM_CMD_JOIN_TIMEOUT,
			   if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	wsm_oper_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_set_bss_params(struct rk960_common *hw_priv,
		       const struct wsm_set_bss_params *arg, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	WSM_COM_CMD_SAVE(struct wsm_set_bss_params,
			 wsm_set_bss_params_cmd, 0x0011, 0);

	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, 0);
	WSM_PUT8(buf, arg->beaconLostCount);
	WSM_PUT16(buf, arg->aid);
	WSM_PUT32(buf, arg->operationalRateSet);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0011, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_add_key(struct rk960_common *hw_priv, const struct wsm_add_key *arg,
		int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	if (wsm_add_key_cmd_save(hw_priv, arg, if_id))
		return -ENOMEM;

	wsm_cmd_lock(hw_priv);

	WSM_PUT(buf, arg, sizeof(*arg));

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x000C, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_remove_key(struct rk960_common *hw_priv,
		   const struct wsm_remove_key *arg, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	if (wsm_remove_key_cmd_save(hw_priv, arg, if_id))
		return -ENOMEM;

	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->entryIndex);
	WSM_PUT8(buf, 0);
	WSM_PUT16(buf, 0);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x000D, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_set_tx_queue_params(struct rk960_common *hw_priv,
			    const struct wsm_set_tx_queue_params *arg,
			    u8 id, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	u8 queue_id_to_wmm_aci[] = { 3, 2, 0, 1 };

	WSM_COM_CMD_SAVE(struct wsm_set_tx_queue_params,
			 wsm_set_tx_queue_params_cmd, 0x0012, id);

	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, queue_id_to_wmm_aci[id]);
	WSM_PUT8(buf, 0);
	WSM_PUT8(buf, arg->ackPolicy);
	WSM_PUT8(buf, 0);
	WSM_PUT32(buf, arg->maxTransmitLifetime);
	WSM_PUT16(buf, arg->allowedMediumTime);
	WSM_PUT16(buf, 0);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0012, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_set_edca_params(struct rk960_common *hw_priv,
			const struct wsm_edca_params *arg, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	WSM_COM_CMD_SAVE(struct wsm_edca_params,
			 wsm_set_edca_params_cmd, 0x0013, 0);

	wsm_cmd_lock(hw_priv);

	/* Implemented according to specification. */

	WSM_PUT16(buf, arg->params[3].cwMin);
	WSM_PUT16(buf, arg->params[2].cwMin);
	WSM_PUT16(buf, arg->params[1].cwMin);
	WSM_PUT16(buf, arg->params[0].cwMin);

	WSM_PUT16(buf, arg->params[3].cwMax);
	WSM_PUT16(buf, arg->params[2].cwMax);
	WSM_PUT16(buf, arg->params[1].cwMax);
	WSM_PUT16(buf, arg->params[0].cwMax);

	WSM_PUT8(buf, arg->params[3].aifns);
	WSM_PUT8(buf, arg->params[2].aifns);
	WSM_PUT8(buf, arg->params[1].aifns);
	WSM_PUT8(buf, arg->params[0].aifns);

	WSM_PUT16(buf, arg->params[3].txOpLimit);
	WSM_PUT16(buf, arg->params[2].txOpLimit);
	WSM_PUT16(buf, arg->params[1].txOpLimit);
	WSM_PUT16(buf, arg->params[0].txOpLimit);

	WSM_PUT32(buf, arg->params[3].maxReceiveLifetime);
	WSM_PUT32(buf, arg->params[2].maxReceiveLifetime);
	WSM_PUT32(buf, arg->params[1].maxReceiveLifetime);
	WSM_PUT32(buf, arg->params[0].maxReceiveLifetime);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0013, WSM_CMD_TIMEOUT, if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_switch_channel(struct rk960_common *hw_priv,
		       const struct wsm_switch_channel *arg, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	if (hw_priv->fw_error_processing)
		return -EINVAL;

	wsm_lock_tx(hw_priv);
	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->channelMode);
	WSM_PUT8(buf, arg->channelSwitchCount);
	WSM_PUT16(buf, arg->newChannelNumber);

	hw_priv->channel_switch_in_progress = 1;

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0016, WSM_CMD_TIMEOUT, if_id);
	wsm_cmd_unlock(hw_priv);
	if (ret) {
		wsm_unlock_tx(hw_priv);
		hw_priv->channel_switch_in_progress = 0;
	}
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	wsm_unlock_tx(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_set_pm(struct rk960_common *hw_priv, const struct wsm_set_pm *arg,
	       int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	RK960_DEBUG_WSM("%s: pmMode %x\n", __func__, arg->pmMode);

	WSM_COM_CMD_SAVE(struct wsm_set_pm, wsm_set_pm_cmd, 0x0010, 0);

	wsm_oper_lock(hw_priv);

	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->pmMode);
	WSM_PUT8(buf, arg->fastPsmIdlePeriod);
	WSM_PUT8(buf, arg->apPsmChangePeriod);
	WSM_PUT8(buf, arg->minAutoPsPollPeriod);

	hw_priv->ps_mode_switch_in_progress = 1;
	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0010, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	if (ret)
		wsm_oper_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	wsm_oper_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_start(struct rk960_common *hw_priv, const struct wsm_start *arg,
	      int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	if (if_id == RK960_GENERIC_IF_ID && hw_priv->fw_error_processing)
		return -ENOMEM;

	WSM_COM_CMD_SAVE(struct wsm_start, wsm_start_cmd, 0x0017, 0);

	wsm_cmd_lock(hw_priv);

	WSM_PUT8(buf, arg->mode);
	WSM_PUT8(buf, arg->band);
	WSM_PUT16(buf, arg->channelNumber);
	WSM_PUT32(buf, arg->CTWindow);
	WSM_PUT32(buf, arg->beaconInterval);
	WSM_PUT8(buf, arg->DTIMPeriod);
	WSM_PUT8(buf, arg->preambleType);
	WSM_PUT8(buf, arg->probeDelay);
	WSM_PUT8(buf, arg->ssidLength);
	WSM_PUT(buf, arg->ssid, sizeof(arg->ssid));
	WSM_PUT32(buf, arg->basicRateSet);

	RK960_DEBUG_WSM("%s: mode %d ch %d ssid %s\n", __func__,
			arg->mode & 0xF, arg->channelNumber, arg->ssid);

	hw_priv->tx_burst_idx = -1;
	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0017, WSM_CMD_START_TIMEOUT,
			   if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

#if 0
/* This API is no longer present in WSC */
/* ******************************************************************** */

int wsm_beacon_transmit(struct rk960_common *hw_priv,
			const struct wsm_beacon_transmit *arg, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	wsm_cmd_lock(hw_priv);

	WSM_PUT32(buf, arg->enableBeaconing ? 1 : 0);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0018, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}
#endif

/* ******************************************************************** */

int wsm_start_find(struct rk960_common *hw_priv, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	if (hw_priv->fw_error_processing)
		return -EINVAL;

	wsm_cmd_lock(hw_priv);
	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x0019, WSM_CMD_TIMEOUT, if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;
}

/* ******************************************************************** */

int wsm_stop_find(struct rk960_common *hw_priv, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	if (hw_priv->fw_error_processing)
		return -EINVAL;

	wsm_cmd_lock(hw_priv);
	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x001A, WSM_CMD_TIMEOUT, if_id);
	wsm_cmd_unlock(hw_priv);
	return ret;
}

/* ******************************************************************** */

int wsm_map_link(struct rk960_common *hw_priv, const struct wsm_map_link *arg,
		 int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;
	u16 cmd = 0x001C;

	RK960_DEBUG_WSM("LDB: %s link: if_id %d link_id %d %pM\n",
			arg->unmap ? "unmap" : "map", if_id,
			arg->link_id, &arg->mac_addr[0]);

	if (arg->unmap)
		wsm_unmap_link_save_cmd(hw_priv, arg, if_id);
	else
		wsm_map_link_save_cmd(hw_priv, arg, if_id);

	wsm_cmd_lock(hw_priv);

	WSM_PUT(buf, &arg->mac_addr[0], sizeof(arg->mac_addr));

	if (is_hardware_version1(hw_priv) || is_hardware_version2(hw_priv)) {
		WSM_PUT8(buf, arg->unmap);
		WSM_PUT8(buf, arg->link_id);
	} else {
		cmd |= WSM_TX_LINK_ID(arg->link_id);
		WSM_PUT16(buf, 0);
	}

	ret = wsm_cmd_send(hw_priv, buf, NULL, cmd, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;
}

/* ******************************************************************** */

int wsm_update_ie(struct rk960_common *hw_priv,
		  const struct wsm_update_ie *arg, int if_id)
{
	int ret;
	struct wsm_buf *buf = &hw_priv->wsm_cmd_buf;

	if (wsm_update_ie_cmd_save(hw_priv, arg, if_id))
		return -ENOMEM;

	wsm_cmd_lock(hw_priv);

	WSM_PUT16(buf, arg->what);
	WSM_PUT16(buf, arg->count);
	WSM_PUT(buf, arg->ies, arg->length);

	ret = wsm_cmd_send(hw_priv, buf, NULL, 0x001B, WSM_CMD_TIMEOUT, if_id);

	wsm_cmd_unlock(hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(hw_priv);
	return -ENOMEM;

}

/* ******************************************************************** */
#ifdef MCAST_FWDING
/* 3.66 */
static int wsm_give_buffer_confirm(struct rk960_common *hw_priv,
				   struct wsm_buf *buf)
{
	RK960_DEBUG_WSM("[WSM] HW Buf count %d\n", hw_priv->hw_bufs_used);
	if (!hw_priv->hw_bufs_used)
		wake_up(&hw_priv->bh_evt_wq);

	return 0;
}

/* 3.65 */
int wsm_init_release_buffer_request(struct rk960_common *hw_priv, u8 index)
{
	struct wsm_buf *buf = &hw_priv->wsm_release_buf[index];
	u16 cmd = 0x0022;	/* Buffer Request */
	u8 flags;
	size_t buf_len;

	wsm_buf_init(buf);

	flags = index ? 0 : 0x1;

	WSM_PUT8(buf, flags);
	WSM_PUT8(buf, 0);
	WSM_PUT16(buf, 0);

	buf_len = buf->data - buf->begin;

	/* Fill HI message header */
	((__le16 *) buf->begin)[0] = __cpu_to_le16(buf_len);
	((__le16 *) buf->begin)[1] = __cpu_to_le16(cmd);

	return 0;
nomem:
	return -ENOMEM;
}

/* 3.68 */
static int wsm_request_buffer_confirm(struct rk960_vif *priv,
				      u8 * arg, struct wsm_buf *buf)
{
	u8 count;
	u32 sta_asleep_mask = 0;
	int i;
	u32 mask = 0;
	u32 change_mask = 0;
	struct rk960_common *hw_priv = priv->hw_priv;

	/* There is no status field in this message */
	sta_asleep_mask = WSM_GET32(buf);
	count = WSM_GET8(buf);
	count -= 1;		/* Current workaround for FW issue */

	spin_lock_bh(&priv->ps_state_lock);
	change_mask = (priv->sta_asleep_mask ^ sta_asleep_mask);
	RK960_DEBUG_WSM("CM %x, HM %x, FWM %x\n", change_mask,
			priv->sta_asleep_mask, sta_asleep_mask);
	spin_unlock_bh(&priv->ps_state_lock);

	if (change_mask) {
		struct ieee80211_sta *sta;
		int ret = 0;

		for (i = 0; i < RK960_MAX_STA_IN_AP_MODE; ++i) {

			if (RK960_LINK_HARD != priv->link_id_db[i].status)
				continue;

			mask = BIT(i + 1);

			/* If FW state and host state for this link are different then notify OMAC */
			if (change_mask & mask) {

				RK960_DEBUG_WSM
				    ("PS State Changed %d for sta %pM\n",
				     (sta_asleep_mask & mask) ? 1 : 0,
				     priv->link_id_db[i].mac);

				rcu_read_lock();
				sta =
				    ieee80211_find_sta(priv->vif,
						       priv->link_id_db[i].mac);
				if (!sta) {
					RK960_DEBUG_WSM
					    ("[WSM] WRBC - could not find sta %pM\n",
					     priv->link_id_db[i].mac);
				} else {
					ret =
					    ieee80211_sta_ps_transition_ni(sta,
									   (sta_asleep_mask
									    &
									    mask)
									   ?
									   true
									   :
									   false);
					RK960_DEBUG_WSM
					    ("PS State NOTIFIED %d\n", ret);
					WARN_ON(ret);
				}
				rcu_read_unlock();
			}
		}
		/* Replace STA mask with one reported by FW */
		spin_lock_bh(&priv->ps_state_lock);
		priv->sta_asleep_mask = sta_asleep_mask;
		spin_unlock_bh(&priv->ps_state_lock);
	}

	RK960_DEBUG_WSM("[WSM] WRBC - HW Buf count %d SleepMask %d\n",
			hw_priv->hw_bufs_used, sta_asleep_mask);
	hw_priv->buf_released = 0;
	WARN_ON(count != (hw_priv->wsm_caps.numInpChBufs - 1));

	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

/* 3.67 */
int wsm_request_buffer_request(struct rk960_vif *priv, u8 * arg)
{
	int ret;
	struct wsm_buf *buf = &priv->hw_priv->wsm_cmd_buf;

	if (hw_priv->fw_error_processing)
		return -EINVAL;

	wsm_cmd_lock(priv->hw_priv);

	WSM_PUT8(buf, (*arg));
	WSM_PUT8(buf, 0);
	WSM_PUT16(buf, 0);

	ret =
	    wsm_cmd_send(priv->hw_priv, buf, arg, 0x0023, WSM_CMD_JOIN_TIMEOUT,
			 priv->if_id);

	wsm_cmd_unlock(priv->hw_priv);
	return ret;

nomem:
	wsm_cmd_unlock(priv->hw_priv);
	return -ENOMEM;
}

#endif

int wsm_set_keepalive_filter(struct rk960_vif *priv, bool enable)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);

	priv->rx_filter.keepalive = enable;
	return wsm_set_rx_filter(hw_priv, &priv->rx_filter, priv->if_id);
}

int wsm_set_probe_responder(struct rk960_vif *priv, bool enable)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);

	priv->rx_filter.probeResponder = enable;
	return wsm_set_rx_filter(hw_priv, &priv->rx_filter, priv->if_id);
}

/* ******************************************************************** */
/* WSM indication events implementation					*/

static int wsm_startup_indication(struct rk960_common *hw_priv,
				  struct wsm_buf *buf)
{
	u16 status;
	char fw_label[129];
	static const char *const fw_types[] = {
		"ETF",
		"WFM",
		"WSM",
		"HI test",
		"Platform test"
	};
        char *svn;
        char svn_str[8];
        int i = 0;

	hw_priv->wsm_caps.numInpChBufs = WSM_GET16(buf);
	hw_priv->wsm_caps.sizeInpChBuf = WSM_GET16(buf);
	hw_priv->wsm_caps.hardwareId = WSM_GET16(buf);
	hw_priv->wsm_caps.hardwareSubId = WSM_GET16(buf);
	status = WSM_GET16(buf);
	hw_priv->wsm_caps.firmwareCap = WSM_GET16(buf);
	hw_priv->wsm_caps.firmwareType = WSM_GET16(buf);
	hw_priv->wsm_caps.firmwareApiVer = WSM_GET16(buf);
	hw_priv->wsm_caps.firmwareBuildNumber = WSM_GET16(buf);
	hw_priv->wsm_caps.firmwareVersion = WSM_GET16(buf);
	WSM_GET(buf, &fw_label[0], sizeof(fw_label) - 1);
	fw_label[sizeof(fw_label) - 1] = 0;	/* Do not trust FW too much. */

        svn = strstr(fw_label, "svn");
        if (WARN_ON(!svn))
                return -EINVAL;
        
        svn += 3;
        while (svn[i] != '.') {
               svn_str[i] = svn[i];
               i++;
        }
        svn_str[i] = 0;
        
        hw_priv->wsm_caps.firmwareSvnVersion = simple_strtol(svn_str, NULL, 10);

	BUG_ON(hw_priv->wsm_caps.sizeInpChBuf != RK960_SDIO_TX_MSG_SIZE);

	if (WARN_ON(status))
		return -EINVAL;

	if (WARN_ON(hw_priv->wsm_caps.firmwareType > 4))
		return -EINVAL;

	RK960_INFO_WSM("RK960 WSM init done.\n"
		       "   Input buffers: %d x %d bytes\n"
		       "   Hardware: %d.%d\n"
		       "   %s firmware [%s], ver: %d, build: %d,"
		       " api: %d, cap: 0x%.4X svn: %d\n",
		       hw_priv->wsm_caps.numInpChBufs,
		       hw_priv->wsm_caps.sizeInpChBuf,
		       hw_priv->wsm_caps.hardwareId,
		       hw_priv->wsm_caps.hardwareSubId,
		       fw_types[hw_priv->wsm_caps.firmwareType],
		       &fw_label[0],
		       hw_priv->wsm_caps.firmwareVersion,
		       hw_priv->wsm_caps.firmwareBuildNumber,
		       hw_priv->wsm_caps.firmwareApiVer,
		       hw_priv->wsm_caps.firmwareCap,
		       hw_priv->wsm_caps.firmwareSvnVersion);

	hw_priv->wsm_caps.firmwareReady += 1;

	wake_up(&hw_priv->wsm_startup_done);
	return 0;

underflow:
	WARN_ON(1);
	return -EINVAL;
}

static int wsm_receive_indication(struct rk960_common *hw_priv,
				  int interface_link_id,
				  struct wsm_buf *buf, struct sk_buff **skb_p)
{
	struct rk960_vif *priv;

	hw_priv->rx_timestamp = jiffies;
	if (hw_priv->wsm_cbc.rx) {
		struct wsm_rx rx;
		struct ieee80211_hdr *hdr;
		size_t hdr_len;
		__le16 fctl;
		u8 *da, *sa;
		s8 rssi;

		rx.status = WSM_GET32(buf);
		rx.channelNumber = WSM_GET16(buf);
		rx.rxedRate = WSM_GET8(buf);
		rx.rcpiRssi = WSM_GET8(buf);
		rx.flags = WSM_GET32(buf);

		/* TODO:COMBO: Frames received from scanning are received
		 * with interface ID == 2 */
		if (is_hardware_version1(hw_priv)
		    || is_hardware_version2(hw_priv)) {
			if (interface_link_id == RK960_GENERIC_IF_ID) {
				/* Frames received in response to SCAN
				 * Request */
				interface_link_id =
				    get_interface_id_scanning(hw_priv);
				if (interface_link_id == -1) {
					interface_link_id = hw_priv->roc_if_id;
				}
#ifdef ROAM_OFFLOAD
				if (hw_priv->auto_scanning) {
					interface_link_id = hw_priv->scan.if_id;
				}
#endif /*ROAM_OFFLOAD */
			}
			/* linkid (peer sta id is encoded in bit 25-28 of
			   flags field */
			rx.link_id = ((rx.flags & (0xf << 25)) >> 25);
			rx.if_id = interface_link_id;
		} else {
			rx.link_id = interface_link_id;
			rx.if_id = 0;
		}
		//pr_info("%s: if_id %d link_id %d\n", __func__, rx.if_id, rx.link_id);

		fctl = *(__le16 *) buf->data;
		hdr_len = buf->data - buf->begin;
		skb_pull(*skb_p, hdr_len);

		priv = rk960_hwpriv_to_vifpriv(hw_priv, rx.if_id);
		if (!priv) {
			if (rx.if_id == -1) {
				//if (!priv->cqm_use_rssi)
				rx.rcpiRssi = rx.rcpiRssi / 2 - 110;
				rk960_rx_cb_monitor(hw_priv, &rx, skb_p);
			} else {
				RK960_INFO_WSM
				    ("%s: NULL priv drop frame %d %d\n",
				     __func__, rx.if_id, rx.link_id);
			}
			return 0;
		}

		/* FW Workaround: Drop probe resp or
		   beacon when RSSI is 0 */
		hdr = (struct ieee80211_hdr *)(*skb_p)->data;

		if (!rx.rcpiRssi &&
		    (ieee80211_is_probe_resp(hdr->frame_control) ||
		     ieee80211_is_beacon(hdr->frame_control))) {
			rk960_priv_vif_list_read_unlock(&priv->vif_lock);
			return 0;
		}

		/* If no RSSI subscription has been made,
		 * convert RCPI to RSSI here */
		if (!priv->cqm_use_rssi)
			rx.rcpiRssi = rx.rcpiRssi / 2 - 110;

		rssi = rx.rcpiRssi;
		RK960_INFO_WSM("%s: id %d sta %d ch %d rate %d rssi %d"
			       " flag %x link_id %d if_id %d\n",
			       __func__, interface_link_id, rx.status,
			       rx.channelNumber, rx.rxedRate, rssi, rx.flags,
			       rx.link_id, rx.if_id);

		if (!rx.status && unlikely(ieee80211_is_deauth(fctl) ||
					   ieee80211_is_disassoc(fctl))) {
			hdr = (struct ieee80211_hdr *)(*skb_p)->data;
			sa = ieee80211_get_SA(hdr);
			da = ieee80211_get_DA(hdr);
			//pr_info("%pM, %pM, %pM\n", sa, da, priv->join_bssid);

			/* P2P GC: submit disassoc to wpa_s will casue reconn
			 * which cause p2p can not disconnect immediately
			 * so generate deauth to wpa_s before disassoc
			 */
			if (hw_priv->roc_if_id_last == rx.if_id &&	// means on p2p0
			    ieee80211_is_disassoc(fctl) &&
			    ether_addr_equal(sa, priv->join_bssid)) {
				struct sk_buff *skb;

				RK960_INFO_WSM
				    ("generate deauth before disassoc\n");
				skb =
				    rk960_build_deauth_frame(sa, da, sa, false);
				if (skb)
					ieee80211_rx_irqsafe(priv->hw, skb);
			}

			if (priv->join_status == RK960_JOIN_STATUS_STA &&
			    ether_addr_equal(sa, priv->join_bssid)) {
				RK960_INFO_WSM("havePMF %d %d\n", priv->havePMF,
					       WSM_RX_STATUS_ENCRYPTION(rx.
									flags));
				if (!priv->havePMF
				    || (priv->havePMF
					&& WSM_RX_STATUS_ENCRYPTION(rx.
								    flags))) {
					/* Shedule unjoin work */
					RK960_DEBUG_WSM
					    ("[WSM] Issue unjoin command (RX).\n");
					wsm_lock_tx_async(hw_priv);
					if (queue_work(hw_priv->workqueue,
						       &priv->unjoin_work) <= 0)
						wsm_unlock_tx(hw_priv);
				}
			}
		}
#ifdef SUPPORT_FWCR
                if (rk960_pae_type(*skb_p, 0))
                        hw_priv->fwcr_update_key = 1;
#endif                
		hw_priv->wsm_cbc.rx(priv, &rx, skb_p);
		if (*skb_p)
			skb_push(*skb_p, hdr_len);
		rk960_priv_vif_list_read_unlock(&priv->vif_lock);
	}
	return 0;

underflow:
	return -EINVAL;
}

static int wsm_event_indication(struct rk960_common *hw_priv,
				struct wsm_buf *buf, int interface_link_id)
{
	int first;
	struct rk960_wsm_event *event;
	struct rk960_vif *priv;

	if (!is_hardware_version1(hw_priv) && !is_hardware_version2(hw_priv))
		interface_link_id = 0;

	priv = rk960_hwpriv_to_vifpriv(hw_priv, interface_link_id);

	if (unlikely(!priv)) {
		RK960_INFO_WSM("[WSM] Event:  for removed "
			       "interface, ignoring\n");
		return 0;
	}

	if (unlikely(priv->mode == NL80211_IFTYPE_UNSPECIFIED)) {
		/* STA is stopped. */
		rk960_priv_vif_list_read_unlock(&priv->vif_lock);
		return 0;
	}
	rk960_priv_vif_list_read_unlock(&priv->vif_lock);

	event = kzalloc(sizeof(struct rk960_wsm_event), GFP_KERNEL);

	event->evt.eventId = __le32_to_cpu(WSM_GET32(buf));
	event->evt.eventData = __le32_to_cpu(WSM_GET32(buf));
	event->if_id = interface_link_id;

	RK960_DEBUG_WSM("[WSM] Event: %d(%d) if_id %d\n",
			event->evt.eventId, event->evt.eventData,
			interface_link_id);

	spin_lock(&hw_priv->event_queue_lock);
	first = list_empty(&hw_priv->event_queue);
	list_add_tail(&event->link, &hw_priv->event_queue);
	spin_unlock(&hw_priv->event_queue_lock);

	if (first)
		queue_work(hw_priv->workqueue, &hw_priv->event_handler);

	return 0;

underflow:
	kfree(event);
	return -EINVAL;
}

/* TODO:COMBO:Make this perVIFF once mac80211 support is available */
static int wsm_channel_switch_indication(struct rk960_common *hw_priv,
					 struct wsm_buf *buf)
{
	wsm_unlock_tx(hw_priv);	/* Re-enable datapath */
	WARN_ON(WSM_GET32(buf));

	hw_priv->channel_switch_in_progress = 0;
	wake_up(&hw_priv->channel_switch_done);

	if (hw_priv->wsm_cbc.channel_switch)
		hw_priv->wsm_cbc.channel_switch(hw_priv);
	return 0;

underflow:
	return -EINVAL;
}

int wsm_set_pm_indication(struct rk960_common *hw_priv, struct wsm_buf *buf)
{
	if (hw_priv->ps_mode_switch_in_progress) {
		hw_priv->ps_mode_switch_in_progress = 0;
		wsm_oper_unlock(hw_priv);

		wake_up(&hw_priv->ps_mode_switch_done);
	}
	return 0;
}

static int wsm_scan_complete_indication(struct rk960_common *hw_priv,
					struct wsm_buf *buf)
{
#ifdef ROAM_OFFLOAD
	if (hw_priv->auto_scanning == 0)
		wsm_oper_unlock(hw_priv);
#else
	wsm_oper_unlock(hw_priv);
#endif /*ROAM_OFFLOAD */

	if (hw_priv->wsm_cbc.scan_complete) {
		struct wsm_scan_complete arg;
		arg.status = WSM_GET32(buf);
		arg.psm = WSM_GET8(buf);
		arg.numChannels = WSM_GET8(buf);
		hw_priv->wsm_cbc.scan_complete(hw_priv, &arg);
	}
	return 0;

underflow:
	return -EINVAL;
}

int rk960_pending_scan_complete_indication(struct rk960_common *hw_priv)
{
	if (!hw_priv->scan.req)
		return 0;

#ifdef ROAM_OFFLOAD
	if (hw_priv->auto_scanning == 0)
		wsm_oper_unlock(hw_priv);
#else
	wsm_oper_unlock(hw_priv);
#endif /*ROAM_OFFLOAD */

	if (hw_priv->wsm_cbc.scan_complete) {
		struct wsm_scan_complete arg;
		arg.status = 0;
		arg.psm = 0;
		arg.numChannels = 1;
		hw_priv->wsm_cbc.scan_complete(hw_priv, &arg);
	}

	return 0;
}

static int wsm_find_complete_indication(struct rk960_common *hw_priv,
					struct wsm_buf *buf)
{
	/* TODO: Implement me. */
	//STUB();
	return 0;
}

static int wsm_suspend_resume_indication(struct rk960_common *hw_priv,
					 int interface_link_id,
					 struct wsm_buf *buf)
{
	if (hw_priv->wsm_cbc.suspend_resume) {
		u32 flags;
		struct wsm_suspend_resume arg;
		struct rk960_vif *priv;

		arg.link_id = 0;
		if (is_hardware_version1(hw_priv) ||
		    is_hardware_version2(hw_priv)) {
			int i;
			arg.if_id = interface_link_id;
			/* TODO:COMBO: Extract bitmap from suspend-resume
			 * TX indication */
			//rk960_for_each_vif(hw_priv, priv, i) {
			for (i = 0; i < RK960_MAX_VIFS; i++) {
				priv = hw_priv->vif_list[i] ?
				    rk960_get_vif_from_ieee80211(hw_priv->
								 vif_list[i]) :
				    NULL;
				if (!priv)
					continue;
				if (priv->join_status == RK960_JOIN_STATUS_AP) {
					arg.if_id = priv->if_id;
					break;
				}
				arg.link_id = 0;
			}
		} else {
			arg.if_id = 0;
			arg.link_id = interface_link_id;
		}

		flags = WSM_GET32(buf);
		arg.stop = !(flags & 1);
		arg.multicast = ! !(flags & 8);
		arg.queue = (flags >> 1) & 3;

		priv = rk960_hwpriv_to_vifpriv(hw_priv, arg.if_id);
		if (unlikely(!priv)) {
			RK960_INFO_WSM("[WSM] suspend-resume indication"
				       " for removed interface!\n");
			return 0;
		}
		hw_priv->wsm_cbc.suspend_resume(priv, &arg);
		rk960_priv_vif_list_read_unlock(&priv->vif_lock);
	}
	return 0;

underflow:
	return -EINVAL;
}

/* ******************************************************************** */
/* WSM TX								*/

int wsm_cmd_send(struct rk960_common *hw_priv,
		 struct wsm_buf *buf, void *arg, u16 cmd, long tmo, int if_id)
{
	size_t buf_len = buf->data - buf->begin;
	int ret;

#if 1
	if (cmd == 0x0006)	/* Write MIB */
		RK960_INFO_WSM
		    ("[WSM] >>> %s 0x%.4X [MIB: 0x%.4X] (%d) %d %d %d\n",
		     wsm_conv_req_resp_to_str(cmd), cmd,
		     __le16_to_cpu(((__le16 *) buf->begin)[2]), (int)buf_len,
		     if_id, hw_priv->hw_bufs_used, atomic_add_return(1,
								     &hw_priv->
								     msg_idx));
	else
		RK960_INFO_WSM("[WSM] >>> %s 0x%.4X (%d) %d %d %d\n",
			       wsm_conv_req_resp_to_str(cmd), cmd, (int)buf_len,
			       if_id, hw_priv->hw_bufs_used,
			       atomic_add_return(1, &hw_priv->msg_idx));
#endif
//    if (cmd == 0x0007)
//        pr_info("scan\n");

#ifdef SUPPORT_FWCR
        if (hw_priv->fwcr_recovery) {
                RK960_INFO_WSM("[WSM] <<< skip\n");
                wsm_buf_reset(buf);

                if (cmd == 0x0007 || cmd == 0x000B || cmd == 0x0010) {
                        hw_priv->ps_mode_switch_in_progress = 1;
                        wsm_oper_unlock(hw_priv);
                }
                return 0;
        }
#endif

	/* Fill HI message header */
	/* BH will add sequence number */

	/* TODO:COMBO: Add if_id from  to the WSM header */
	/* if_id == -1 indicates that command is HW specific,
	 * eg. wsm_configuration which is called during driver initialzation
	 *  (mac80211 .start callback called when first ifce is created. )*/

	/* send hw specific commands on if 0 */
	if (if_id == -1)
		if_id = 0;

	((__le16 *) buf->begin)[0] = __cpu_to_le16(buf_len);
	((__le16 *) buf->begin)[1] = __cpu_to_le16(cmd |
						   ((is_hardware_version1
						     (hw_priv)
						     ||
						     is_hardware_version2
						     (hw_priv)) ? (if_id << 6) :
						    0));

	spin_lock(&hw_priv->wsm_cmd.lock);
	BUG_ON(hw_priv->wsm_cmd.ptr);
	hw_priv->wsm_cmd.done = 0;
	hw_priv->wsm_cmd.ptr = buf->begin;
	hw_priv->wsm_cmd.len = buf_len;
	hw_priv->wsm_cmd.arg = arg;
	hw_priv->wsm_cmd.cmd = cmd;
	spin_unlock(&hw_priv->wsm_cmd.lock);

	rk960_bh_wakeup(hw_priv);

	if (unlikely(hw_priv->bh_error)) {
		/* Do not wait for timeout if BH is dead. Exit immediately. */
		ret = 0;
	} else {
#ifdef RK960_FW_ERROR_RECOVERY
                if (hw_priv->fw_error_processing)
                        tmo = HZ / 10;
                ret = wait_event_timeout(hw_priv->wsm_cmd_wq,
                                         hw_priv->wsm_cmd.done, tmo);
#else
		long rx_timestamp;
		long wsm_cmd_starttime = jiffies;
		long wsm_cmd_runtime;
		long wsm_cmd_max_tmo = WSM_CMD_DEFAULT_TIMEOUT;

		/* Give start cmd a little more time */
		if (tmo == WSM_CMD_START_TIMEOUT)
			wsm_cmd_max_tmo = WSM_CMD_START_TIMEOUT;
		/* Firmware prioritizes data traffic over control confirm.
		 * Loop below checks if data was RXed and increases timeout
		 * accordingly. */
		do {
			/* It's safe to use unprotected access to
			 * wsm_cmd.done here */
			ret = wait_event_timeout(hw_priv->wsm_cmd_wq,
						 hw_priv->wsm_cmd.done, tmo);
			rx_timestamp = jiffies - hw_priv->rx_timestamp;
			wsm_cmd_runtime = jiffies - wsm_cmd_starttime;
			if (unlikely(rx_timestamp < 0) || wsm_cmd_runtime < 0)
				rx_timestamp = tmo + 1;
		} while (!ret && rx_timestamp <= tmo &&
			 wsm_cmd_runtime < wsm_cmd_max_tmo);
#endif                
	}

	if (unlikely(ret == 0)) {
		u16 raceCheck;
		//BUG_ON(1);
#ifdef RK960_FW_ERROR_RECOVERY
		RK960_ERROR_WSM("[WSM] cmd send timeout\n");
		rk960_signal_fw_error(hw_priv, RK960_FWERR_REASON_CMD);
#endif
		spin_lock(&hw_priv->wsm_cmd.lock);
		raceCheck = hw_priv->wsm_cmd.cmd;
		hw_priv->wsm_cmd.arg = NULL;
		hw_priv->wsm_cmd.ptr = NULL;
		spin_unlock(&hw_priv->wsm_cmd.lock);

#ifdef RK960_FW_ERROR_RECOVERY
		spin_lock(&hw_priv->wsm_cmd.lock);
		hw_priv->wsm_cmd.ret = ret;
		hw_priv->wsm_cmd.done = 1;
		spin_unlock(&hw_priv->wsm_cmd.lock);
#else
		/* Race condition check to make sure _confirm is not called
		 * after exit of _send */
		if (raceCheck == 0xFFFF) {
			/* If wsm_handle_rx got stuck in _confirm we will hang
			 * system there. It's better than silently currupt
			 * stack or heap, isn't it? */
			BUG_ON(wait_event_timeout(hw_priv->wsm_cmd_wq,
						  hw_priv->wsm_cmd.done,
						  WSM_CMD_LAST_CHANCE_TIMEOUT)
			       <= 0);
		}

		/* Kill BH thread to report the error to the top layer. */
		RK960_ERROR_WSM("[WSM] >>> %s 0x%.4X [MIB: 0x%.4X] timeout\n",
				wsm_conv_req_resp_to_str(cmd),
				cmd, __le16_to_cpu(((__le16 *) buf->begin)[2]));
		hw_priv->bh_error = 1;
		wake_up(&hw_priv->bh_wq);
		ret = -ETIMEDOUT;
#endif
	} else {
		spin_lock(&hw_priv->wsm_cmd.lock);
		BUG_ON(!hw_priv->wsm_cmd.done);
		ret = hw_priv->wsm_cmd.ret;
		spin_unlock(&hw_priv->wsm_cmd.lock);
	}
	wsm_buf_reset(buf);
	return ret;
}

/* ******************************************************************** */
/* WSM TX port control							*/

void wsm_lock_tx(struct rk960_common *hw_priv)
{
	wsm_cmd_lock(hw_priv);
	if (atomic_add_return(1, &hw_priv->tx_lock) == 1) {
		if (wsm_flush_tx(hw_priv))
			RK960_DEBUG_WSM("[WSM] TX is locked.\n");
	}
	//pr_info("%s: tx_lock %d\n", __func__, atomic_read(&hw_priv->tx_lock));
	wsm_cmd_unlock(hw_priv);
}

void wsm_lock_tx_rec(struct rk960_common *hw_priv)
{
	atomic_add_return(1, &hw_priv->tx_lock);
}

void wsm_vif_lock_tx(struct rk960_vif *priv)
{
	struct rk960_common *hw_priv = priv->hw_priv;
	wsm_cmd_lock(hw_priv);
	if (atomic_add_return(1, &hw_priv->tx_lock) == 1) {
		if (wsm_vif_flush_tx(priv))
			RK960_DEBUG_WSM("[WSM] TX is locked for"
					" if_id %d.\n", priv->if_id);
	}
	//pr_info("%s: tx_lock %d\n", __func__, atomic_read(&hw_priv->tx_lock));
	wsm_cmd_unlock(hw_priv);
}

void wsm_lock_tx_async(struct rk960_common *hw_priv)
{
	if (atomic_add_return(1, &hw_priv->tx_lock) == 1)
		RK960_DEBUG_WSM("[WSM] TX is locked (async).\n");
	//pr_info("%s: tx_lock %d\n", __func__, atomic_read(&hw_priv->tx_lock));
}

bool wsm_flush_tx(struct rk960_common *hw_priv)
{
	unsigned long timestamp = jiffies;
	bool pending = false;
	long timeout;
	int i;

	/* Flush must be called with TX lock held. */
	BUG_ON(!atomic_read(&hw_priv->tx_lock));

	/* First check if we really need to do something.
	 * It is safe to use unprotected access, as hw_bufs_used
	 * can only decrements. */

	if (!hw_priv->hw_bufs_used)
		return true;

	if (hw_priv->bh_error) {
		/* In case of failure do not wait for magic. */
		RK960_ERROR_WSM("[WSM] Fatal error occured, "
				"will not flush TX.\n");
		return false;
	} else {
		/* Get a timestamp of "oldest" frame */
		for (i = 0; i < 4; ++i)
			pending |=
			    rk960_queue_get_xmit_timestamp(&hw_priv->
							   tx_queue[i],
							   &timestamp,
							   RK960_ALL_IFS,
							   0xffffffff);
		/* It is allowed to lock TX with only a command in the pipe. */
		if (!pending)
			return true;
#ifdef RK960_FW_ERROR_RECOVERY
		timeout = timestamp + WSM_CMD_LAST_CHANCE_TIMEOUT - jiffies;
		if (timeout < 0 || wait_event_timeout(hw_priv->bh_evt_wq,
						      !hw_priv->hw_bufs_used,
						      timeout) <= 0) {
			RK960_ERROR_WSM("%s: hw_bufs_used %d timeout %d\n",
					__func__, hw_priv->hw_bufs_used,
					jiffies_to_msecs(timeout));
			hw_priv->hw_bufs_used = 0;
			rk960_signal_fw_error(hw_priv,
					      RK960_FWERR_REASON_FLUSH);
		}
#else
		timeout = WSM_CMD_LAST_CHANCE_TIMEOUT;
		if (timeout < 0 || wait_event_timeout(hw_priv->bh_evt_wq,
						      !hw_priv->hw_bufs_used,
						      timeout) <= 0) {
			/* Hmmm... Not good. Frame had stuck in firmware. */
			hw_priv->bh_error = 1;
			RK960_ERROR_WSM("%s:  bh_error=1 %d\n", __func__,
					hw_priv->hw_bufs_used);
			rk960_queue_dump_pending_tx(hw_priv, RK960_ALL_IFS);
			WARN_ON(1);
			wake_up(&hw_priv->bh_wq);
			return false;
		}
#endif
		/* Ok, everything is flushed. */
		return true;
	}
}

bool wsm_vif_flush_tx(struct rk960_vif * priv)
{
	struct rk960_common *hw_priv = priv->hw_priv;
	unsigned long timestamp = jiffies;
	long timeout;
	int i;
	int if_id = priv->if_id;

	/* Flush must be called with TX lock held. */
	BUG_ON(!atomic_read(&hw_priv->tx_lock));

	/* First check if we really need to do something.
	 * It is safe to use unprotected access, as hw_bufs_used
	 * can only decrements. */

	if (!hw_priv->hw_bufs_used_vif[priv->if_id])
		return true;

	if (hw_priv->bh_error) {
		/* In case of failure do not wait for magic. */
		RK960_ERROR_WSM("[WSM] Fatal error occured, "
				"will not flush TX.\n");
		return false;
	} else {
		/* Get a timestamp of "oldest" frame */
		for (i = 0; i < 4; ++i)
			rk960_queue_get_xmit_timestamp(&hw_priv->tx_queue[i],
						       &timestamp, if_id,
						       0xffffffff);
		/* It is allowed to lock TX with only a command in the pipe. */
		if (!hw_priv->hw_bufs_used_vif[if_id])
			return true;

#ifdef RK960_FW_ERROR_RECOVERY
		timeout = timestamp + WSM_CMD_LAST_CHANCE_TIMEOUT - jiffies;
		if (timeout < 0 || wait_event_timeout(hw_priv->bh_evt_wq,
						      !hw_priv->
						      hw_bufs_used_vif[if_id],
						      timeout) <= 0) {
			RK960_ERROR_WSM
			    ("%s: hw_bufs_used_vif[%d] %d timeout %d\n",
			     __func__, if_id, hw_priv->hw_bufs_used_vif[if_id],
			     jiffies_to_msecs(timeout));
			hw_priv->hw_bufs_used_vif[if_id] = 0;
			rk960_signal_fw_error(hw_priv,
					      RK960_FWERR_REASON_FLUSH);
		}
#else
		timeout = WSM_CMD_LAST_CHANCE_TIMEOUT;
		if (timeout < 0 || wait_event_timeout(hw_priv->bh_evt_wq,
						      !hw_priv->
						      hw_bufs_used_vif[if_id],
						      timeout) <= 0) {

			/* Hmmm... Not good. Frame had stuck in firmware. */
			hw_priv->bh_error = 1;
			RK960_ERROR_WSM("%s:  bh_error=1 %d %d\n", __func__,
					hw_priv->hw_bufs_used_vif[priv->if_id],
					hw_priv->hw_bufs_used);
			rk960_queue_dump_pending_tx(hw_priv, if_id);
			WARN_ON(1);
			wake_up(&hw_priv->bh_wq);
			return false;
		}
#endif
		/* Ok, everything is flushed. */
		return true;
	}
}

void wsm_unlock_tx(struct rk960_common *hw_priv)
{
	int tx_lock;
	if (hw_priv->bh_error)
		RK960_ERROR_WSM("fatal error occured, unlock is unsafe\n");
	else {
		tx_lock = atomic_sub_return(1, &hw_priv->tx_lock);
		//pr_info("%s: tx_lock %d\n", __func__, tx_lock);
		if (tx_lock < 0) {
			BUG_ON(1);
		} else if (tx_lock == 0) {
			rk960_bh_wakeup(hw_priv);
			RK960_DEBUG_WSM("[WSM] TX is unlocked.\n");
		}
	}
}

void wsm_unlock_tx_rec(struct rk960_common *hw_priv)
{
	int tx_lock;

	tx_lock = atomic_sub_return(1, &hw_priv->tx_lock);
	if (tx_lock < 0) {
		BUG_ON(1);
	} else if (tx_lock == 0) {
		rk960_bh_wakeup(hw_priv);
	}
}

/* ******************************************************************** */
/* WSM RX								*/

int wsm_handle_exception(struct rk960_common *hw_priv, u8 * data, size_t len)
{
	struct wsm_buf buf;
	u32 reason;
	u32 reg[18];
	char fname[48];
	int i;
#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
	struct rk960_vif *priv = NULL;
	int if_id = 0;
#endif

	static const char *const reason_str[] = {
		"undefined instruction",
		"prefetch abort",
		"data abort",
		"unknown error",
	};

#if defined(CONFIG_RK960_USE_STE_EXTENSIONS)
	/* Send the event upwards on the FW exception */
	rk960_pm_stay_awake(&hw_priv->pm_state, 3 * HZ);

	rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
	//rk960_for_each_vif(hw_priv, priv, if_id) {
	for (i = 0; i < RK960_MAX_VIFS; i++) {
		priv = rk960_get_vif_from_ieee80211_hw_priv(hw_priv, i);
		if (!priv)
			continue;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
		RK960_ERROR_WSM("%s: ieee80211_driver_hang_notify\n", __func__);
#else
		ieee80211_driver_hang_notify(priv->vif, GFP_KERNEL);
#endif
	}
	rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);
#endif

	buf.begin = buf.data = data;
	buf.end = &buf.begin[len];

	reason = WSM_GET32(&buf);
	for (i = 0; i < ARRAY_SIZE(reg); ++i)
		reg[i] = WSM_GET32(&buf);
	WSM_GET(&buf, fname, sizeof(fname));

	if (reason < 4)
		RK960_ERROR_WSM("Firmware exception: %s.\n",
				reason_str[reason]);
	else
		RK960_ERROR_WSM("Firmware assert at %.*s, line %d\n",
				(int)sizeof(fname), fname, reg[1]);

	for (i = 0; i < 12; i += 4)
		RK960_DEBUG_WSM
		    ("R%d: 0x%.8X, R%d: 0x%.8X, R%d: 0x%.8X, R%d: 0x%.8X,\n",
		     i + 0, reg[i + 0], i + 1, reg[i + 1], i + 2, reg[i + 2],
		     i + 3, reg[i + 3]);
	RK960_DEBUG_WSM("R12: 0x%.8X, SP: 0x%.8X, LR: 0x%.8X, PC: 0x%.8X,\n",
			reg[i + 0], reg[i + 1], reg[i + 2], reg[i + 3]);
	i += 4;
	RK960_DEBUG_WSM("CPSR: 0x%.8X, SPSR: 0x%.8X\n", reg[i + 0], reg[i + 1]);

	print_hex_dump_bytes("R1: ", DUMP_PREFIX_NONE, fname, sizeof(fname));
	return 0;

underflow:
	RK960_ERROR_WSM("Firmware exception.\n");
	print_hex_dump_bytes("Exception: ", DUMP_PREFIX_NONE, data, len);
	return -EINVAL;
}

int wsm_handle_rx(struct rk960_common *hw_priv, int id,
		  struct wsm_hdr *wsm, struct sk_buff **skb_p)
{
	int ret = 0;
	struct wsm_buf wsm_buf;
	int interface_link_id = (id >> 6) & 0x0F;
#ifdef ROAM_OFFLOAD
#if 0
	struct rk960_vif *priv;
	priv = rk960_hwpriv_to_vifpriv(hw_priv, interface_link_id);
	if (unlikely(!priv)) {
		WARN_ON(1);
		return 0;
	}
	rk960_priv_vif_list_read_unlock(&priv->vif_lock);
#endif
#endif /*ROAM_OFFLOAD */

	/* Strip link id. */
	id &= ~WSM_TX_LINK_ID(WSM_TX_LINK_ID_MAX);

	wsm_buf.begin = (u8 *) & wsm[0];
	wsm_buf.data = (u8 *) & wsm[1];
	wsm_buf.end = &wsm_buf.begin[__le32_to_cpu(wsm->len)];

#if 1
	RK960_INFO_WSM("[WSM] <<< %s 0x%.4X (%d) lid %d %d %d\n",
		       wsm_conv_req_resp_to_str(id), id,
		       (int)(wsm_buf.end - wsm_buf.begin),
		       interface_link_id,
		       hw_priv->hw_bufs_used,
		       atomic_add_return(1, &hw_priv->evt_idx));
#endif
	if (id == 0x404) {
		ret = wsm_tx_confirm(hw_priv, &wsm_buf, interface_link_id, 1);
#ifdef MCAST_FWDING
#if 1
	} else if (id == 0x422) {
		ret = wsm_give_buffer_confirm(hw_priv, &wsm_buf);
#endif
#endif

	} else if (id == 0x41E) {
		ret = wsm_multi_tx_confirm(hw_priv, &wsm_buf,
					   interface_link_id);
	} else if (id & 0x0400) {
		void *wsm_arg;
		u16 wsm_cmd;

		/* Do not trust FW too much. Protection against repeated
		 * response and race condition removal (see above). */
		spin_lock(&hw_priv->wsm_cmd.lock);
		wsm_arg = hw_priv->wsm_cmd.arg;
		wsm_cmd = hw_priv->wsm_cmd.cmd &
		    ~WSM_TX_LINK_ID(WSM_TX_LINK_ID_MAX);
		hw_priv->wsm_cmd.cmd = 0xFFFF;
		spin_unlock(&hw_priv->wsm_cmd.lock);

		if (WARN_ON((id & ~0x0400) != wsm_cmd)) {
			/* Note that any non-zero is a fatal retcode. */
			ret = -EINVAL;
			goto out;
		}

		switch (id) {
		case 0x0400:	/* wsm_mem_rw read */
			ret = wsm_mem_rw_confirm(hw_priv, wsm_arg, &wsm_buf);
			break;
		case 0x0409:
			/* Note that wsm_arg can be NULL in case of timeout in
			 * wsm_cmd_send(). */
			if (likely(wsm_arg))
				ret = wsm_configuration_confirm(hw_priv,
								wsm_arg,
								&wsm_buf);
			break;
		case 0x0405:
			if (likely(wsm_arg))
				ret = wsm_read_mib_confirm(hw_priv, wsm_arg,
							   &wsm_buf);
			break;
		case 0x0406:
			if (likely(wsm_arg))
				ret = wsm_write_mib_confirm(hw_priv, wsm_arg,
							    &wsm_buf,
							    interface_link_id);
			break;
		case 0x040B:
			if (likely(wsm_arg))
				ret = wsm_join_confirm(hw_priv, wsm_arg,
						       &wsm_buf);
			break;

#ifdef MCAST_FWDING
		case 0x0423:	/* req buffer cfm */
			if (likely(wsm_arg)) {
				int i;
				struct rk960_vif *priv = NULL;
				//rk960_for_each_vif(hw_priv, priv, i) {
				for (i = 0; i < RK960_MAX_VIFS; i++) {
					priv = hw_priv->vif_list[i] ?
					    rk960_get_vif_from_ieee80211
					    (hw_priv->vif_list[i]) : NULL;
					if (priv
					    && (priv->join_status ==
						RK960_JOIN_STATUS_AP))
						ret =
						    wsm_request_buffer_confirm
						    (priv, wsm_arg, &wsm_buf);
				}
			}
			break;
#endif
		case 0x0424:
			ret = wsm_adc_read_confirm(hw_priv, wsm_arg, &wsm_buf);
			break;
		case 0x0407:	/* start-scan */
#ifdef ROAM_OFFLOAD
			if (hw_priv->auto_scanning) {
				if (atomic_read(&hw_priv->scan.in_progress)) {
					hw_priv->auto_scanning = 0;
				} else {
					wsm_oper_unlock(hw_priv);
					up(&hw_priv->scan.lock);
				}
			}
#endif /*ROAM_OFFLOAD */
		case 0x0401:	/* wsm_mem_rw write */
		case 0x0408:	/* stop-scan */
		case 0x040A:	/* wsm_reset */
		case 0x040C:	/* add_key */
		case 0x040D:	/* remove_key */
		case 0x0410:	/* wsm_set_pm */
		case 0x0411:	/* set_bss_params */
		case 0x0412:	/* set_tx_queue_params */
		case 0x0413:	/* set_edca_params */
		case 0x0416:	/* switch_channel */
		case 0x0417:	/* start */
		case 0x0418:	/* beacon_transmit */
		case 0x0419:	/* start_find */
		case 0x041A:	/* stop_find */
		case 0x041B:	/* update_ie */
		case 0x041C:	/* map_link */
			WARN_ON(wsm_arg != NULL);
			ret = wsm_generic_confirm(hw_priv, wsm_arg, &wsm_buf);
			if (ret)
				RK960_ERROR_WSM("wsm_generic_confirm "
						"failed for request 0x%.4X.\n",
						id & ~0x0400);
			break;
		default:
			BUG_ON(1);
		}

		spin_lock(&hw_priv->wsm_cmd.lock);
		hw_priv->wsm_cmd.ret = ret;
		hw_priv->wsm_cmd.done = 1;
		spin_unlock(&hw_priv->wsm_cmd.lock);
		ret = 0;	/* Error response from device should ne stop BH. */

		wake_up(&hw_priv->wsm_cmd_wq);
	} else if (id & 0x0800) {
		switch (id) {
		case 0x0801:
			ret = wsm_startup_indication(hw_priv, &wsm_buf);
			break;
		case 0x0803:
			hw_priv->device_can_sleep = false;
			hw_priv->device_wakeing = false;
#ifdef RK960_USE_RX_THREAD
			rk960_bh_wakeup(hw_priv);
#endif
			ret = 0;
			break;
		case 0x0804:
			ret = wsm_receive_indication(hw_priv, interface_link_id,
						     &wsm_buf, skb_p);
			break;
		case 0x0805:
			ret = wsm_event_indication(hw_priv, &wsm_buf,
						   interface_link_id);
			break;
		case 0x080A:
			ret = wsm_channel_switch_indication(hw_priv, &wsm_buf);
			break;
		case 0x0809:
			ret = wsm_set_pm_indication(hw_priv, &wsm_buf);
			break;
		case 0x0806:
#ifdef ROAM_OFFLOAD
			if (hw_priv->auto_scanning && hw_priv->frame_rcvd) {
				struct rk960_vif *priv;
				hw_priv->frame_rcvd = 0;
				priv =
				    rk960_hwpriv_to_vifpriv(hw_priv,
							    hw_priv->scan.
							    if_id);
				if (unlikely(!priv)) {
					WARN_ON(1);
					return 0;
				}
				rk960_priv_vif_list_read_unlock(&priv->
								vif_lock);
				if (hw_priv->beacon) {
					struct wsm_scan_complete *scan_cmpl =
					    (struct wsm_scan_complete *)
					    ((u8 *) wsm +
					     sizeof(struct wsm_hdr));
					struct ieee80211_rx_status *rhdr =
					    IEEE80211_SKB_RXCB(hw_priv->beacon);
					rhdr->signal = (s8) scan_cmpl->reserved;
					if (!priv->cqm_use_rssi) {
						rhdr->signal =
						    rhdr->signal / 2 - 110;
					}
					if (!hw_priv->beacon_bkp)
						hw_priv->beacon_bkp =
						    skb_copy(hw_priv->beacon,
							     GFP_ATOMIC);
					ieee80211_rx_irqsafe(hw_priv->hw,
							     hw_priv->beacon);
					hw_priv->beacon = hw_priv->beacon_bkp;

					hw_priv->beacon_bkp = NULL;
				}
				RK960_DEBUG_WSM("[WSM] Send Testmode Event.\n");
				rk960_testmode_event(priv->hw->wiphy,
						     NL80211_CMD_NEW_SCAN_RESULTS,
						     0, 0, GFP_KERNEL);

			}
#endif /*ROAM_OFFLOAD */
			ret = wsm_scan_complete_indication(hw_priv, &wsm_buf);
			break;
		case 0x080B:
			ret = wsm_find_complete_indication(hw_priv, &wsm_buf);
			break;
		case 0x080C:
			ret = wsm_suspend_resume_indication(hw_priv,
							    interface_link_id,
							    &wsm_buf);
			break;
#ifdef ROAM_OFFLOAD
		case 0x0810:
			if (hw_priv->auto_scanning && hw_priv->frame_rcvd) {
				hw_priv->frame_rcvd = 0;
				pr_info("%s: sched_scan_stop_work\n", __func__);
				queue_delayed_work(hw_priv->workqueue,
						   &hw_priv->scan.
						   sched_scan_stop_work, 0);
			}
			break;
#endif
		default:
			//STUB();
			break;
		}
	} else {
		WARN_ON(1);
		ret = -EINVAL;
	}
out:
	return ret;
}

static bool wsm_handle_tx_data(struct rk960_vif *priv,
			       const struct wsm_tx *wsm,
			       const struct ieee80211_tx_info *tx_info,
			       struct rk960_txpriv *txpriv,
			       struct rk960_queue *queue)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);
#ifdef P2P_MULTIVIF
	struct rk960_vif *p2p_if_vif = NULL;
#endif
	bool handled = false;
	const struct ieee80211_hdr *frame =
	    (struct ieee80211_hdr *)&((u8 *) wsm)[txpriv->offset];
	__le16 fctl = frame->frame_control;
	enum {
		doProbe,
		doDrop,
		doJoin,
		doOffchannel,
		doWep,
		doTx,
	} action = doTx;

	hw_priv = rk960_vifpriv_to_hwpriv(priv);
#ifdef P2P_MULTIVIF
	if (priv->if_id == RK960_GENERIC_IF_ID)
		p2p_if_vif = __rk960_hwpriv_to_vifpriv(hw_priv, 1);
#endif
	frame = (struct ieee80211_hdr *)&((u8 *) wsm)[txpriv->offset];
	fctl = frame->frame_control;

	/*RK960_DEBUG_WSM("%s: mode %d if_id %d join_status %d "
	   "join_bssid %pM addr1 %pM fc %x\n",
	   __func__, priv->mode, priv->if_id, priv->join_status,
	   priv->join_bssid, frame->addr1, fctl); */
	switch (priv->mode) {
	case NL80211_IFTYPE_STATION:
		if (unlikely((priv->join_status == RK960_JOIN_STATUS_STA) &&
			     ieee80211_is_nullfunc(fctl))) {
			spin_lock(&priv->bss_loss_lock);
			if (priv->bss_loss_status == RK960_BSS_LOSS_CHECKING) {
				priv->bss_loss_status =
				    RK960_BSS_LOSS_CONFIRMING;
				priv->bss_loss_confirm_id = wsm->packetID;
			}
			spin_unlock(&priv->bss_loss_lock);
		} else if (unlikely((priv->join_status <=
				     RK960_JOIN_STATUS_MONITOR)
				    || memcmp(frame->addr1, priv->join_bssid,
					      sizeof(priv->join_bssid)))) {
#ifdef P2P_MULTIVIF
			if (p2p_if_vif && (p2p_if_vif->join_status >
					   RK960_JOIN_STATUS_MONITOR)
			    && (priv->join_status < RK960_JOIN_STATUS_MONITOR)) {
				/*
				 * Post group formation, frame transmission on p2p0
				 * interafce should not use offchannel/generic channel.
				 * Instead, the frame should be transmitted on interafce
				 * 1. This is needed by wsc fw.
				 */
				action = doTx;
				txpriv->raw_if_id = 1;
			} else
#endif
			if (ieee80211_is_auth(fctl))
				action = doJoin;
			else if (ieee80211_is_probe_req(fctl))
				action = doTx;
			else if (memcmp(frame->addr1, priv->join_bssid,
					sizeof(priv->join_bssid)) &&
				 (priv->join_status ==
				  RK960_JOIN_STATUS_STA) &&
				 (ieee80211_is_data(fctl))) {
				action = doDrop;
			} else if (ieee80211_has_protected(fctl)) {
				/* Drop protected frame during listen
				 */
				if (!memcmp(frame->addr1, priv->join_bssid,
					    sizeof(priv->join_bssid)) ||
				    priv->join_status <
				    RK960_JOIN_STATUS_MONITOR) {
					RK960_INFO_WSM
					    ("Drop during offchannel\n");
					action = doDrop;
				}
			} else if (priv->join_status >=
				   RK960_JOIN_STATUS_MONITOR)
				action = doTx;
			else if (get_interface_id_scanning(hw_priv) != -1) {
				RK960_INFO_WSM
				    ("Scan ONGOING dropping offchannel"
				     " eligible frame.\n");
				action = doDrop;
			} else if (txpriv->if_id == 0
				   && is_zero_ether_addr(priv->join_bssid)) {
				action = doDrop;
			} else if (ieee80211_is_probe_resp(fctl) ||
				   ieee80211_is_probe_req(fctl) ||
				   ieee80211_is_action(fctl)) {
				action = doOffchannel;
			} else
				action = doDrop;
		}
		break;
	case NL80211_IFTYPE_AP:
		if (unlikely(!priv->join_status))
			action = doDrop;
		else if (unlikely(!(BIT(txpriv->raw_link_id) &
				    (BIT(0) | priv->link_id_map)))) {
			RK960_INFO_WSM("A frame with expired link id "
				       "is dropped.\n");
			action = doDrop;
		}
		if (rk960_queue_get_generation(wsm->packetID) >
		    RK960_MAX_REQUEUE_ATTEMPTS) {
			/* HACK!!! WSM324 firmware has tendency to requeue
			 * multicast frames in a loop, causing performance
			 * drop and high power consumption of the driver.
			 * In this situation it is better just to drop
			 * the problematic frame. */
			RK960_INFO_WSM("Too many attempts "
				       "to requeue a frame. "
				       "Frame is dropped.\n");
			action = doDrop;
		}
		break;
	case NL80211_IFTYPE_ADHOC:
#ifdef IBSS_SUPPORT
		if (ieee80211_is_auth(fctl))
			action = doTx;
		if (!priv->join_status || !priv->link_id_map)
			action = doDrop;
		break;
#endif
	case NL80211_IFTYPE_MESH_POINT:
		//STUB();
	case NL80211_IFTYPE_MONITOR:
	default:
		action = doDrop;
		break;
	}

	if (action == doTx) {
		if (unlikely(ieee80211_is_probe_req(fctl))) {
#ifdef CONFIG_RK960_TESTMODE
			if (hw_priv->enable_advance_scan &&
			    (priv->join_status == RK960_JOIN_STATUS_STA) &&
			    (hw_priv->advanceScanElems.scanMode ==
			     RK960_SCAN_MEASUREMENT_ACTIVE))
				/* If Advance Scan is Requested on Active Scan
				 * then transmit the Probe Request */
				action = doTx;
			else
#endif
				action = doProbe;
		} else if ((fctl & __cpu_to_le32(IEEE80211_FCTL_PROTECTED)) &&
			   tx_info->control.hw_key &&
			   unlikely(tx_info->control.hw_key->keyidx !=
				    priv->wep_default_key_id) &&
			   (tx_info->control.hw_key->cipher ==
			    WLAN_CIPHER_SUITE_WEP40 ||
			    tx_info->control.hw_key->cipher ==
			    WLAN_CIPHER_SUITE_WEP104))
			action = doWep;
	}

	switch (action) {
	case doProbe:
		{
			/* An interesting FW "feature". Device filters
			 * probe responses.
			 * The easiest way to get it back is to convert
			 * probe request into WSM start_scan command. */
			RK960_DEBUG_WSM
			    ("[WSM] Convert probe request to scan.\n");
			wsm_lock_tx_async(hw_priv);
			hw_priv->pending_frame_id =
			    __le32_to_cpu(wsm->packetID);
			queue_delayed_work(hw_priv->workqueue,
					   &hw_priv->scan.probe_work, 0);
			handled = true;
		}
		break;
	case doDrop:
		{
			/* See detailed description of "join" below.
			 * We are dropping everything except AUTH in non-joined mode. */
			RK960_DEBUG_WSM("[WSM] Drop frame (0x%.4X).\n", fctl);
#ifdef CONFIG_RK960_TESTMODE
			BUG_ON(rk960_queue_remove(hw_priv, queue,
						  __le32_to_cpu(wsm->
								packetID)));
#else
			BUG_ON(rk960_queue_remove(queue,
						  __le32_to_cpu(wsm->
								packetID)));
#endif /*CONFIG_RK960_TESTMODE */
			handled = true;
		}
		break;
	case doJoin:
		{
#ifdef RK960_CHANNEL_CONFLICT_OPT
			queue_work(hw_priv->workqueue,
				   &priv->channel_switch_work);
#endif
			/* There is one more interesting "feature"
			 * in FW: it can't do RX/TX before "join".
			 * "Join" here is not an association,
			 * but just a syncronization between AP and STA.
			 * priv->join_status is used only in bh thread and does
			 * not require protection */
			RK960_DEBUG_WSM("[WSM] Issue join command.\n");
			wsm_lock_tx_async(hw_priv);
			hw_priv->pending_frame_id =
			    __le32_to_cpu(wsm->packetID);
			if (queue_work(hw_priv->workqueue, &priv->join_work) <=
			    0)
				wsm_unlock_tx(hw_priv);
			handled = true;
		}
		break;
	case doOffchannel:
		{
			RK960_DEBUG_WSM("[WSM] Offchannel TX request.\n");
			wsm_lock_tx_async(hw_priv);
			hw_priv->pending_frame_id =
			    __le32_to_cpu(wsm->packetID);
			if (queue_work
			    (hw_priv->workqueue, &priv->offchannel_work) <= 0)
				wsm_unlock_tx(hw_priv);
			handled = true;
		}
		break;
	case doWep:
		{
			RK960_DEBUG_WSM("[WSM] Issue set_default_wep_key.\n");
			wsm_lock_tx_async(hw_priv);
			priv->wep_default_key_id =
			    tx_info->control.hw_key->keyidx;
			hw_priv->pending_frame_id =
			    __le32_to_cpu(wsm->packetID);
			if (queue_work(hw_priv->workqueue, &priv->wep_key_work)
			    <= 0)
				wsm_unlock_tx(hw_priv);
			handled = true;
		}
		break;
	case doTx:
		{
#if 0
			/* Kept for history. If you want to implement wsm->more,
			 * make sure you are able to send a frame after that. */
			wsm->more = (count > 1) ? 1 : 0;
			if (wsm->more) {
				/* HACK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				 * It's undocumented in WSM spec, but RK960 hangs
				 * if 'more' is set and no TX is performed due to TX
				 * buffers limitation. */
				if (priv->hw_bufs_used + 1 ==
				    priv->wsm_caps.numInpChBufs)
					wsm->more = 0;
			}

			/* BUG!!! FIXME: we can't use 'more' at all: we don't know
			 * future. It could be a request from upper layer with TX lock
			 * requirements (scan, for example). If "more" is set device
			 * will not send data and wsm_tx_lock() will fail...
			 * It's not obvious how to fix this deadlock. Any ideas?
			 * As a workaround more is set to 0. */
			wsm->more = 0;
#endif /* 0 */

			if (ieee80211_is_deauth(fctl) &&
			    priv->mode != NL80211_IFTYPE_AP) {
				/* Shedule unjoin work */
				RK960_DEBUG_WSM("[WSM] Issue unjoin command"
						" (TX).\n");
#if 0
				wsm->more = 0;
#endif /* 0 */
				wsm_lock_tx_async(hw_priv);
				if (queue_work(hw_priv->workqueue,
					       &priv->unjoin_work) <= 0)
					wsm_unlock_tx(hw_priv);
			}
		}
		break;
	}
	return handled;
}

static int rk960_get_prio_queue(struct rk960_vif *priv,
				u32 link_id_map, int *total)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);
	static u32 urgent;
	struct wsm_edca_queue_params *edca;
	unsigned score, best = -1;
	int winner = -1;
	int queued;
	int i;
	urgent = BIT(priv->link_id_after_dtim) | BIT(priv->link_id_uapsd);

	/* search for a winner using edca params */
	for (i = 0; i < 4; ++i) {
		queued = rk960_queue_get_num_queued(priv,
						    &hw_priv->tx_queue[i],
						    link_id_map);
		if (!queued)
			continue;
		*total += queued;
		edca = &priv->edca.params[i];
		score = ((edca->aifns + edca->cwMin) << 16) +
		    (edca->cwMax - edca->cwMin) *
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
		    (get_random_u16() & 0xFFFF);
#else	
		    (get_random_int() & 0xFFFF);
#endif		
#else
		    (random32() & 0xFFFF);
#endif
		if (score < best && (winner < 0 || i != 3)) {
			best = score;
			winner = i;
		}
	}

	/* override winner if bursting */
	if (winner >= 0 && hw_priv->tx_burst_idx >= 0 &&
	    winner != hw_priv->tx_burst_idx &&
	    !rk960_queue_get_num_queued(priv,
					&hw_priv->tx_queue[winner],
					link_id_map & urgent) &&
	    rk960_queue_get_num_queued(priv,
				       &hw_priv->tx_queue[hw_priv->
							  tx_burst_idx],
				       link_id_map))
		winner = hw_priv->tx_burst_idx;

	return winner;
}

static int wsm_get_tx_queue_and_mask(struct rk960_vif *priv,
				     struct rk960_queue **queue_p,
				     u32 * tx_allowed_mask_p, bool * more)
{
	struct rk960_common *hw_priv = rk960_vifpriv_to_hwpriv(priv);
	int idx;
	u32 tx_allowed_mask;
	int total = 0;

	/* Search for a queue with multicast frames buffered */
	if (priv->tx_multicast) {
		tx_allowed_mask = BIT(priv->link_id_after_dtim);
		idx = rk960_get_prio_queue(priv, tx_allowed_mask, &total);
		if (idx >= 0) {
			*more = total > 1;
			goto found;
		}
	}

	/* Search for unicast traffic */
	tx_allowed_mask = ~priv->sta_asleep_mask;
	tx_allowed_mask |= BIT(priv->link_id_uapsd);
	if (priv->sta_asleep_mask) {
		tx_allowed_mask |= priv->pspoll_mask;
		tx_allowed_mask &= ~BIT(priv->link_id_after_dtim);
	} else {
		tx_allowed_mask |= BIT(priv->link_id_after_dtim);
	}
	idx = rk960_get_prio_queue(priv, tx_allowed_mask, &total);
	if (idx < 0)
		return -ENOENT;

found:
	*queue_p = &hw_priv->tx_queue[idx];
	*tx_allowed_mask_p = tx_allowed_mask;
	return 0;
}

int wsm_get_tx(struct rk960_common *hw_priv, u8 ** data,
	       size_t * tx_len, int *burst, int *vif_selected)
{
	struct wsm_tx *wsm = NULL;
	struct ieee80211_tx_info *tx_info;
	struct rk960_queue *queue = NULL;
	int queue_num;
	u32 tx_allowed_mask = 0;
	struct rk960_txpriv *txpriv = NULL;
#ifdef P2P_MULTIVIF
	int first = 1;
	int tmp_if_id = -1;
#endif
	/*
	 * Count was intended as an input for wsm->more flag.
	 * During implementation it was found that wsm->more
	 * is not usable, see details above. It is kept just
	 * in case you would like to try to implement it again.
	 */
	int count = 0;
#ifdef P2P_MULTIVIF
	int if_pending = RK960_MAX_VIFS - 1;
#else
	int if_pending = 1;
#endif

	/* More is used only for broadcasts. */
	bool more = false;
	unsigned long *xmited_timestamp;

	count = rk960_itp_get_tx(hw_priv, data, tx_len, burst);
	if (count)
		return count;

	if (hw_priv->wsm_cmd.ptr) {
		++count;
		spin_lock(&hw_priv->wsm_cmd.lock);
		BUG_ON(!hw_priv->wsm_cmd.ptr);
		*data = hw_priv->wsm_cmd.ptr;
		*tx_len = hw_priv->wsm_cmd.len;
		*burst = 1;
		*vif_selected = -1;
		spin_unlock(&hw_priv->wsm_cmd.lock);
	} else {
		for (;;) {
			int ret;
			struct rk960_vif *priv;
#if 0
			int num_pending_vif0, num_pending_vif1;
#endif
			if (atomic_add_return(0, &hw_priv->tx_lock)	/* &&
									   !hw_priv->scan.req */ )
				break;
			/* Keep one buffer reserved for commands. Note
			   that, hw_bufs_used has already been incremented
			   before reaching here. */
			if (hw_priv->hw_bufs_used >=
			    hw_priv->wsm_caps.numInpChBufs)
				break;
#ifdef DISABLE_MULTI_WSM_TX
			if (hw_priv->tx_pending)
				break;
#endif
#ifdef P2P_MULTIVIF
			if (first) {
				tmp_if_id = hw_priv->if_id_selected;
				hw_priv->if_id_selected = 2;
			}
#endif
			priv = wsm_get_interface_for_tx(hw_priv);
			/* go to next interface ID to select next packet */
#ifdef P2P_MULTIVIF
			if (first) {
				hw_priv->if_id_selected = tmp_if_id;
				first = 0;
			} else
#endif
				hw_priv->if_id_selected ^= 1;

			/* There might be no interface before add_interface
			 * call */
			if (!priv) {
				if (if_pending) {
#ifdef P2P_MULTIVIF
					if_pending--;
#else
					if_pending = 0;
#endif
					continue;
				}
				break;
			}
#if 0
			if (((priv->if_id == 0) &&
			     (hw_priv->hw_bufs_used_vif[0] >=
			      RK960_FW_VIF0_THROTTLE)) ||
			    ((priv->if_id == 1) &&
			     (hw_priv->hw_bufs_used_vif[1] >=
			      RK960_FW_VIF1_THROTTLE))) {
				rk960_priv_vif_list_read_unlock(&priv->
								vif_lock);
				if (if_pending) {
					if_pending = 0;
					continue;
				}
				break;
			}
#endif

			/* This can be removed probably: rk960_vif will not
			 * be in hw_priv->vif_list (as returned from
			 * wsm_get_interface_for_tx) until it's fully
			 * enabled, so statement above will take case of that*/
			if (!atomic_read(&priv->enabled)) {
				rk960_priv_vif_list_read_unlock(&priv->
								vif_lock);
				break;
			}

			/* TODO:COMBO: Find the next interface for which
			 * packet needs to be found */
			spin_lock_bh(&priv->ps_state_lock);
			ret = wsm_get_tx_queue_and_mask(priv, &queue,
							&tx_allowed_mask,
							&more);
			queue_num = queue - hw_priv->tx_queue;

			if (priv->buffered_multicasts &&
			    (ret || !more) &&
			    (priv->tx_multicast || !priv->sta_asleep_mask)) {
				priv->buffered_multicasts = false;
				if (priv->tx_multicast) {
					priv->tx_multicast = false;
					RK960_DEBUG_AP
					    ("%s: multicast_stop_work\n",
					     __func__);
					queue_work(hw_priv->workqueue,
						   &priv->multicast_stop_work);
				}
			}

			spin_unlock_bh(&priv->ps_state_lock);

			if (ret) {
				rk960_priv_vif_list_read_unlock(&priv->
								vif_lock);
#ifdef P2P_MULTIVIF
				if (if_pending) {
#else
				if (if_pending == 1) {
#endif
#ifdef P2P_MULTIVIF
					if_pending--;
#else
					if_pending = 0;
#endif
					continue;
				}
				break;
			}

			if (rk960_queue_get(queue,
					    priv->if_id,
					    tx_allowed_mask,
					    &wsm, &tx_info, &txpriv,
					    &xmited_timestamp)) {
				rk960_priv_vif_list_read_unlock(&priv->
								vif_lock);
				if_pending = 0;
				continue;
			}
#ifdef ROC_DEBUG
			{
				struct ieee80211_hdr *hdr =
				    (struct ieee80211_hdr *)
				    &((u8 *) wsm)[txpriv->offset];

				RK960_DEBUG_WSM("QGET-1 %x, off_id %d,"
						" if_id %d\n",
						hdr->frame_control,
						txpriv->offchannel_if_id,
						priv->if_id);
			}
#endif

			if (wsm_handle_tx_data(priv, wsm,
					       tx_info, txpriv, queue)) {
				rk960_priv_vif_list_read_unlock(&priv->
								vif_lock);
				if_pending = 0;
				rk960_bh_wakeup(hw_priv);
				continue;	/* Handled by WSM */
			}

			if (xmited_timestamp)
				*xmited_timestamp = jiffies;
			wsm->hdr.id &=
			    __cpu_to_le16(~WSM_TX_IF_ID(WSM_TX_IF_ID_MAX));
#ifdef P2P_MULTIVIF
			if (txpriv->raw_if_id)
				wsm->hdr.id |=
				    cpu_to_le16(WSM_TX_IF_ID
						(txpriv->raw_if_id));
#else
			if (txpriv->offchannel_if_id)
				wsm->hdr.id |=
				    cpu_to_le16(WSM_TX_IF_ID
						(txpriv->offchannel_if_id));
#endif
			else
				wsm->hdr.id |=
				    cpu_to_le16(WSM_TX_IF_ID(priv->if_id));

			*vif_selected = priv->if_id;
#ifdef ROC_DEBUG
			{
				struct ieee80211_hdr *hdr =
				    (struct ieee80211_hdr *)
				    &((u8 *) wsm)[txpriv->offset];

				RK960_DEBUG_WSM("QGET-2 %x, off_id %d,"
						" if_id %d\n",
						hdr->frame_control,
						txpriv->offchannel_if_id,
						priv->if_id);
			}
#endif

			priv->pspoll_mask &= ~BIT(txpriv->raw_link_id);

			*data = (u8 *) wsm;
			*tx_len = __le16_to_cpu(wsm->hdr.len);

			/* allow bursting if txop is set */
#if USE_SDIO_TX_CONTINUE
			if (1)
#else
			if (priv->edca.params[queue_num].txOpLimit)
#endif
				*burst = min(*burst,
					     (int)
					     rk960_queue_get_num_queued(priv,
									queue,
									tx_allowed_mask)
					     + 1);
			else
				*burst = 1;
#if USE_SDIO_TX_CONTINUE
			*burst = min(*burst, 8);
#endif

			//pr_info("txOpLimit %d burst %d\n", priv->edca.params[queue_num].txOpLimit, *burst);
			/* store index of bursting queue */
			if (*burst > 1
			    && priv->edca.params[queue_num].txOpLimit)
				hw_priv->tx_burst_idx = queue_num;
			else
				hw_priv->tx_burst_idx = -1;

			if (more) {
				struct ieee80211_hdr *hdr =
				    (struct ieee80211_hdr *)
				    &((u8 *) wsm)[txpriv->offset];
				if (strstr(&priv->ssid[0], "6.1.12")) {
					if (hdr->addr1[0] & 0x01) {
						hdr->frame_control |=
						    cpu_to_le16
						    (IEEE80211_FCTL_MOREDATA);
					}
				} else {
					/* more buffered multicast/broadcast frames
					 *  ==> set MoreData flag in IEEE 802.11 header
					 *  to inform PS STAs */
					hdr->frame_control |=
					    cpu_to_le16
					    (IEEE80211_FCTL_MOREDATA);
				}
			}
			RK960_INFO_WSM
			    ("[WSM] >>> WSM_TX_REQ 0x%.4X (%d) pid %x %d %c %d %d\n",
			     0x0004, (int)*tx_len, wsm->packetID, priv->if_id,
			     wsm->more ? 'M' : ' ', hw_priv->hw_bufs_used,
			     atomic_add_return(1, &hw_priv->msg_idx));
#ifdef DISABLE_MULTI_WSM_TX
			hw_priv->tx_pending++;
#endif
			++count;
			rk960_priv_vif_list_read_unlock(&priv->vif_lock);
			break;
		}
	}

	return count;
}

void wsm_txed(struct rk960_common *hw_priv, u8 * data)
{
	if (data == hw_priv->wsm_cmd.ptr) {
		spin_lock(&hw_priv->wsm_cmd.lock);
		hw_priv->wsm_cmd.ptr = NULL;
		spin_unlock(&hw_priv->wsm_cmd.lock);
	}
}

/* ******************************************************************** */
/* WSM buffer								*/

#define WSM_BUF_BLOCK_SIZE (2048)

void wsm_buf_init(struct wsm_buf *buf)
{
	BUG_ON(buf->begin);
	buf->begin = kmalloc(WSM_BUF_BLOCK_SIZE, GFP_KERNEL | GFP_DMA);
	buf->end = buf->begin ? &buf->begin[WSM_BUF_BLOCK_SIZE] : buf->begin;
	wsm_buf_reset(buf);
}

void wsm_buf_deinit(struct wsm_buf *buf)
{
	kfree(buf->begin);
	buf->begin = buf->data = buf->end = NULL;
}

static void wsm_buf_reset(struct wsm_buf *buf)
{
	if (buf->begin) {
		buf->data = &buf->begin[4];
		*(u32 *) buf->begin = 0;
	} else
		buf->data = buf->begin;
}

static int wsm_buf_reserve(struct wsm_buf *buf, size_t extra_size)
{
	size_t pos = buf->data - buf->begin;
	size_t size = pos + extra_size;

	if (size & (WSM_BUF_BLOCK_SIZE - 1)) {
		size &= WSM_BUF_BLOCK_SIZE;
		size += WSM_BUF_BLOCK_SIZE;
	}

	buf->begin = krealloc(buf->begin, size, GFP_KERNEL | GFP_DMA);
	if (buf->begin) {
		buf->data = &buf->begin[pos];
		buf->end = &buf->begin[size];
		return 0;
	} else {
		buf->end = buf->data = buf->begin;
		return -ENOMEM;
	}
}

#ifdef RK960_VIF_LIST_USE_RCU_LOCK
static struct rk960_vif
*wsm_get_interface_for_tx(struct rk960_common *hw_priv)
{
	struct rk960_vif *priv = NULL;
	int i = hw_priv->if_id_selected;
	struct ieee80211_vif *i_priv = NULL;

	rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);

	i_priv = RK960_HW_VIF_GET(hw_priv->vif_list[i]);
	if (i_priv)
		priv = rk960_get_vif_from_ieee80211(i_priv);

	if (!priv)
		rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);
	/* TODO:COMBO:
	 * Find next interface based on TX bitmap announced by the FW
	 * Find next interface based on load balancing */

	return priv;
}
#else
static struct rk960_vif
*wsm_get_interface_for_tx(struct rk960_common *hw_priv)
{
	struct rk960_vif *priv = NULL, *i_priv;
	int i = hw_priv->if_id_selected;

	if (is_hardware_version1(hw_priv) || 1 /*TODO:COMBO */ ) {
		rk960_hw_vif_read_lock(&hw_priv->vif_list_lock);
#if 0
		rk960_for_each_vif(hw_priv, i_priv, i) {
			for (i = 0; i < RK960_MAX_VIFS; i++) {
				i_priv = hw_priv->vif_list[i] ?
				    rk960_get_vif_from_ieee80211(hw_priv->
								 vif_list[i]) :
				    NULL;
				if (i_priv) {
					priv = i_priv;
					rk960_priv_vif_list_read_lock(&priv->
								      vif_lock);
					break;
				}
			}
#endif
			i_priv = hw_priv->vif_list[i] ?
			    rk960_get_vif_from_ieee80211(hw_priv->
							 vif_list[i]) : NULL;
			if (i_priv) {
				priv = i_priv;
				rk960_priv_vif_list_read_lock(&priv->vif_lock);
			}
			/* TODO:COMBO:
			 * Find next interface based on TX bitmap announced by the FW
			 * Find next interface based on load balancing */
			rk960_hw_vif_read_unlock(&hw_priv->vif_list_lock);
		}
		else {
			priv = rk960_hwpriv_to_vifpriv(hw_priv, 0);
		}

		return priv;
	}
#endif

	static inline int get_interface_id_scanning(struct rk960_common
						    *hw_priv) {
		if ((hw_priv->scan.req) || (hw_priv->scan.direct_probe))
			return hw_priv->scan.if_id;
		else
			return -1;
	}
