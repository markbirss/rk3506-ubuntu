/*
 * Exported  API by wl_cfg80211 Modules
 * Common function shared by MASTER driver
 *
 * Portions of this code are copyright (c) 2023 Cypress Semiconductor Corporation
 *
 * Copyright (C) 1999-2018, Broadcom Corporation
 *
 *      Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2 (the "GPL"),
 * available at http://www.broadcom.com/licenses/GPLv2.php, with the
 * following added to such license:
 *
 *      As a special exception, the copyright holders of this software give you
 * permission to link this software with independent modules, and to copy and
 * distribute the resulting executable under terms of your choice, provided that
 * you also meet, for each linked independent module, the terms and conditions of
 * the license of that module.  An independent module is a module which is not
 * derived from this software.  The special exception does not apply to any
 * modifications of the software.
 *
 *      Notwithstanding the above, under no circumstances may you combine this
 * software in any way with any other Broadcom software provided under a license
 * other than the GPL, without Broadcom's express prior written consent.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id$
 */

#ifdef  WL_DHD_XR
#include <osl.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/netdevice.h>

#include <wldev_common.h>
#include <bcmutils.h>
#include <dhd.h>
#include <dhd_dbg.h>
#ifdef WL_CFG80211
#include <wl_cfg80211.h>
#include <wl_cfg80211_xr.h>
#endif /* WL_CFG80211 */
#include <wl_cfgscan.h>
#ifdef DHD_BANDSTEER
#include <dhd_bandsteer.h>
#endif /* DHD_BANDSTEER */
#ifdef  WL_DHD_XR_MASTER
int wl_cfg80211_dhd_xr_prim_netdev_attach(struct net_device *ndev,
	wl_iftype_t  wl_iftype, void *ifp,
		u8 bssidx, u8 ifidx) {
	struct wireless_dev *wdev = NULL;
	int ret =  0;
	struct bcm_cfg80211 *cfg = wl_cfg80211_get_bcmcfg();

	wdev = (struct wireless_dev *)MALLOCZ(cfg->osh, sizeof(*wdev));
	if (!wdev) {
		DHD_ERROR(("BCMDHDX wireless_dev alloc failed!\n"));
		return BCME_ERROR;
	}

	wdev->wiphy = bcmcfg_to_wiphy(cfg);
	wdev->iftype = wl_mode_to_nl80211_iftype(WL_MODE_BSS);

	wdev->netdev = ndev;
	ndev->ieee80211_ptr = wdev;
	SET_NETDEV_DEV(ndev, wiphy_dev(wdev->wiphy));

	/* Initialize with the station mode params */
	ret = wl_alloc_netinfo(cfg, ndev, wdev, wl_iftype,
			PM_ENABLE, bssidx, ifidx);
	if (unlikely(ret)) {
		printk("BCMDHDX wl_alloc_netinfo Error (%d)\n", ret);
		return BCME_ERROR;
	}

	cfg->xr_slave_prim_wdev = wdev;
	return BCME_OK;
}
EXPORT_SYMBOL(wl_cfg80211_dhd_xr_prim_netdev_attach);
#endif /* WL_DHD_XR_MASTER */

int dhd_xr_init(dhd_pub_t *dhdp)
{
	int ret = BCME_OK;
	xr_ctx_t *xr_ctx = NULL;
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	if (!dhdp) {
		WL_ERR(("dhdp is null\n"));
		return BCME_ERROR;
	}

	dhdp->xr_ctx = (void *) kzalloc(sizeof(xr_ctx_t), flags);
	if (!dhdp->xr_ctx) {
		DHD_ERROR(("XR ctx allocation failed\n"));
		return BCME_ERROR;
	}

	xr_ctx = (xr_ctx_t *)dhdp->xr_ctx;
	xr_ctx->xr_role = XR_ROLE;
	return ret;
}

int dhd_xr_deinit(dhd_pub_t *dhdp)
{
	int ret = BCME_OK;

	if (!dhdp) {
		WL_ERR(("dhdp is null\n"));
		return BCME_ERROR;
	}

	if (dhdp->xr_ctx) {
		DHD_ERROR(("XR ctx freed \n"));
		kfree(dhdp->xr_ctx);
		dhdp->xr_ctx = NULL;
	}

	return ret;
}

/* add_if */
struct wireless_dev *wl_cfg80211_add_if_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
		u8 wl_iftype, const char *name, u8 *mac)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_add_if_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_add_if_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return NULL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_ADD_IF;
	cmd->len = sizeof(xr_cmd_add_if_t);
	data = (xr_cmd_add_if_t *)&cmd->data[0];

	data->wl_iftype = wl_iftype;
	data->name = name;
	data->mac = mac;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->add_if_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return NULL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.add_if_wdev;

}

int wl_cfg80211_add_if_xr_reply(dhd_pub_t *dest_pub, struct wireless_dev *wdev)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_add_if_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_add_if_t *data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_ADD_IF;
	cmd->len = sizeof(xr_cmd_reply_add_if_t);
	data = (xr_cmd_reply_add_if_t *)&cmd->data[0];

	data->wdev = wdev;
	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);
	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return BCME_ERROR;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}
int xr_cmd_add_if_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	struct net_device *primary_ndev = dhd_linux_get_primary_netdev(pub);
	struct bcm_cfg80211 *cfg = wl_get_cfg(primary_ndev);
	struct wireless_dev *wdev = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_add_if_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_add_if_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_add_if_t *)&xr_cmd->data[0];

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	wdev = wl_cfg80211_add_if(cfg, primary_ndev, cmd->wl_iftype, cmd->name, cmd->mac);

	if (dest_pub)
		ret = wl_cfg80211_add_if_xr_reply(dest_pub, wdev);

	return ret;
}

int xr_cmd_reply_add_if_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_add_if_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_add_if_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.add_if_wdev = reply->wdev;
	complete(&xr_ctx->xr_cmd_wait.add_if_wait);
	return ret;
}

/* del_if */
s32 wl_cfg80211_del_if_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
		struct wireless_dev *wdev, char *ifname)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_del_if_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_del_if_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_DEL_IF;
	cmd->len = sizeof(xr_cmd_del_if_t);
	data = (xr_cmd_del_if_t *)&cmd->data[0];

	data->wdev = wdev;
	data->ifname = ifname;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->del_if_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}
	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.del_if_status;

}

int wl_cfg80211_del_if_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_del_if_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_del_if_t *data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_DEL_IF;
	cmd->len = sizeof(xr_cmd_reply_del_if_t);
	data = (xr_cmd_reply_del_if_t *)&cmd->data[0];

	data->status = status;
	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return BCME_ERROR;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}
int xr_cmd_del_if_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	s32 status = 0;
	int ret = BCME_OK;
	struct net_device *primary_ndev = dhd_linux_get_primary_netdev(pub);
	struct bcm_cfg80211 *cfg = wl_get_cfg(primary_ndev);
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_del_if_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_del_if_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_del_if_t *)&xr_cmd->data[0];

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_del_if(cfg, primary_ndev, cmd->wdev, cmd->ifname);

	if (dest_pub) {
		ret = wl_cfg80211_del_if_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_del_if_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_del_if_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_del_if_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.del_if_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.del_if_wait);
	return ret;
}

#if defined(WL_CFG80211_P2P_DEV_IF)
s32
wl_cfg80211_scan_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy,
	struct cfg80211_scan_request *request)
#else
s32
wl_cfg80211_scan_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy,
	struct net_device *ndev, struct cfg80211_scan_request *request)
#endif /* WL_CFG80211_P2P_DEV_IF */
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_scan_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_scan_t *data = NULL;
	int ret = BCME_OK;
	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		return -EINVAL;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_SCAN;
	cmd->len = sizeof(xr_cmd_scan_t);
	data = (xr_cmd_scan_t *)&cmd->data[0];

	data->wiphy = wiphy;
#if !defined(WL_CFG80211_P2P_DEV_IF)
	data->ndev = ndev;
#endif // endif
	data->request = request;
	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}

int xr_cmd_scan_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_scan_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);
	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_scan_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_scan_t *)&xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {

		DHD_ERROR(("XR_CMD_SCAN cfg is NULL\n"));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

#if defined(WL_CFG80211_P2P_DEV_IF)
	status = wl_cfg80211_scan(cmd->wiphy, cmd->request);
#else
	status = wl_cfg80211_scan(cmd->wiphy, cmd->ndev, cmd->request);
#endif /* WL_CFG80211_P2P_DEV_IF */

	return ret;
}

#if defined(WL_CFG80211_P2P_DEV_IF)
s32 wl_cfg80211_get_tx_power_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy,
        struct wireless_dev *wdev, s32 *dbm)
#else
s32 wl_cfg80211_get_tx_power_xr(dhd_pub_t *src_pub,dhd_pub_t *dest_pub, struct wiphy *wiphy, s32 *dbm)
#endif /* WL_CFG80211_P2P_DEV_IF */
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_get_tx_power_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_get_tx_power_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = 0;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL){
		return -EINVAL;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_GET_TX_POWER;
	cmd->len = sizeof(xr_cmd_get_tx_power_t);
	data = (xr_cmd_get_tx_power_t *)&cmd->data[0];

	data->wiphy = wiphy;
#if defined(WL_CFG80211_P2P_DEV_IF)
	data->wdev = wdev;
#endif // endif
	data->dbm = dbm;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->get_tx_power_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.get_tx_power_status;

}

int wl_cfg80211_get_tx_power_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_get_tx_power_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_get_tx_power_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_GET_TX_POWER;
	cmd->len = sizeof(xr_cmd_reply_get_tx_power_t);
	data = (xr_cmd_reply_get_tx_power_t *) &cmd->data[0];

	data->status = status;
	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);
	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);
	return ret;
}

int xr_cmd_get_tx_power_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_get_tx_power_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_get_tx_power_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_get_tx_power_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("XR_CMD_GET_TX_POWER cfg is NULL\n"));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

#if defined(WL_CFG80211_P2P_DEV_IF)
	status = wl_cfg80211_get_tx_power(cmd->wiphy, cmd->wdev, cmd->dbm);
#else
	status = wl_cfg80211_get_tx_power(cmd->wiphy, cmd->dbm);
#endif /* WL_CFG80211_P2P_DEV_IF */
	if (dest_pub) {
		wl_cfg80211_get_tx_power_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_get_tx_power_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_get_tx_power_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_get_tx_power_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.get_tx_power_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.get_tx_power_wait);

	return ret;
}
/* set_power_mgmt */
s32
wl_cfg80211_set_power_mgmt_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev,
        bool enabled, s32 timeout)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_set_power_mgmt_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_set_power_mgmt_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = 0;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		return -EINVAL;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_SET_POWER_MGMT;
	cmd->len = sizeof(xr_cmd_set_power_mgmt_t);
	data = (xr_cmd_set_power_mgmt_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->enabled = enabled;
	data->timeout = timeout;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->set_power_mgmt_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.set_power_mgmt_status;

}

int wl_cfg80211_set_power_mgmt_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_set_power_mgmt_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_set_power_mgmt_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_SET_POWER_MGMT;
	cmd->len = sizeof(xr_cmd_reply_set_power_mgmt_t);
	data = (xr_cmd_reply_set_power_mgmt_t *) &cmd->data[0];

	data->status = status;
	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);
	return ret;
}

int xr_cmd_set_power_mgmt_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_set_power_mgmt_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_set_power_mgmt_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_set_power_mgmt_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);
	status = wl_cfg80211_set_power_mgmt(cmd->wiphy, cmd->dev, cmd->enabled, cmd->timeout);

	if (dest_pub) {
		wl_cfg80211_set_power_mgmt_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_set_power_mgmt_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_set_power_mgmt_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_set_power_mgmt_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.set_power_mgmt_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.set_power_mgmt_wait);

	return ret;
}

/* wl_cfg80211_flush_pmksa */
s32 wl_cfg80211_flush_pmksa_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_flush_pmksa_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_flush_pmksa_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL){
		return -EINVAL;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_FLUSH_PMKSA;
	cmd->len = sizeof(xr_cmd_flush_pmksa_t);
	data = (xr_cmd_flush_pmksa_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->flush_pmksa_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.flush_pmksa_status;
}

int wl_cfg80211_flush_pmksa_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_flush_pmksa_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_flush_pmksa_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_FLUSH_PMKSA;
	cmd->len = sizeof(xr_cmd_reply_flush_pmksa_t);
	data = (xr_cmd_reply_flush_pmksa_t *) &cmd->data[0];

	data->status = status;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}

int xr_cmd_flush_pmksa_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_flush_pmksa_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_flush_pmksa_t))	{
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_flush_pmksa_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);
	status = wl_cfg80211_flush_pmksa(cmd->wiphy, cmd->dev);

	if (dest_pub) {
		wl_cfg80211_flush_pmksa_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_flush_pmksa_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_flush_pmksa_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_flush_pmksa_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.flush_pmksa_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.flush_pmksa_wait);

	return ret;
}
/* change_virtual_iface */
s32 wl_cfg80211_change_virtual_iface_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *ndev,
        enum nl80211_iftype type,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0))
        u32 *flags,
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0) */
        struct vif_params *params)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_change_virtual_iface_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_change_virtual_iface_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL){
		return -EINVAL;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_CHANGE_VIRUTAL_IFACE;
	cmd->len = sizeof(xr_cmd_change_virtual_iface_t);
	data = (xr_cmd_change_virtual_iface_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->ndev = ndev;
	data->type = type;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0))
	data->flags = flags;
#endif // endif
	data->params = params;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->change_virtual_iface_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.change_virtual_iface_status;

}

int wl_cfg80211_change_virtual_iface_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_change_virtual_iface_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_change_virtual_iface_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_CHANGE_VIRUTAL_IFACE;
	cmd->len = sizeof(xr_cmd_reply_change_virtual_iface_t);
	data = (xr_cmd_reply_change_virtual_iface_t *) &cmd->data[0];

	data->status = status;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);
	return ret;
}

int xr_cmd_change_virtual_iface_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_change_virtual_iface_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n",__func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_change_virtual_iface_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_change_virtual_iface_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0))
	status = wl_cfg80211_change_virtual_iface(cmd->wiphy, cmd->ndev, cmd->type, cmd->flags, cmd->params);
#else
	status = wl_cfg80211_change_virtual_iface(cmd->wiphy, cmd->ndev, cmd->type, cmd->params);
#endif // endif
	if (dest_pub) {
		wl_cfg80211_change_virtual_iface_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_change_virtual_iface_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_change_virtual_iface_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_change_virtual_iface_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.change_virtual_iface_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.change_virtual_iface_wait);

	return ret;
}

#ifdef WL_6E
s32
wl_stop_fils_6g_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, u8 fils_stop)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_stop_fils_6g_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_stop_fils_6g_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_STOP_FILS_6G;
	cmd->len = sizeof(xr_cmd_stop_fils_6g_t);
	data = (xr_cmd_stop_fils_6g_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->stop_fils_6g_value = fils_stop;
	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->stop_fils_6g_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.stop_fils_6g_status;

}
#endif /* WL_6E */

/* start_ap */
s32
wl_cfg80211_start_ap_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, struct cfg80211_ap_settings *info)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_start_ap_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_start_ap_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_START_AP;
	cmd->len = sizeof(xr_cmd_start_ap_t);
	data = (xr_cmd_start_ap_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->info = info;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->start_ap_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.start_ap_status;

}

int wl_cfg80211_start_ap_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_start_ap_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_start_ap_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_START_AP;
	cmd->len = sizeof(xr_cmd_reply_start_ap_t);
	data = (xr_cmd_reply_start_ap_t *) &cmd->data[0];

	data->status = status;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}
int xr_cmd_start_ap_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_start_ap_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_start_ap_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_start_ap_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_start_ap(cmd->wiphy, cmd->dev, cmd->info);

	if(dest_pub) {
		wl_cfg80211_start_ap_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_start_ap_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_start_ap_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_start_ap_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.start_ap_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.start_ap_wait);

	return ret;
}

#ifdef WL_CFG80211_ACL
/*set_mac_acl*/
int wl_cfg80211_set_mac_acl_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *cfgdev, const struct cfg80211_acl_data *acl)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_set_mac_acl_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_set_mac_acl_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_SET_MAC_ACL;
	cmd->len = sizeof(xr_cmd_set_mac_acl_t);
	data = (xr_cmd_set_mac_acl_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->cfgdev = cfgdev;
	data->acl = acl;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->set_mac_acl_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.set_mac_acl_status;

}

int wl_cfg80211_set_mac_acl_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_set_mac_acl_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_set_mac_acl_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_SET_MAC_ACL;
	cmd->len = sizeof(xr_cmd_reply_set_mac_acl_t);
	data = (xr_cmd_reply_set_mac_acl_t *) &cmd->data[0];

	data->status = status;

	dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}
int xr_cmd_set_mac_acl_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_set_mac_acl_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_set_mac_acl_t))	{
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_set_mac_acl_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {

		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_set_mac_acl(cmd->wiphy, cmd->cfgdev, cmd->acl);

	if (dest_pub) {
		wl_cfg80211_set_mac_acl_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_set_mac_acl_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_set_mac_acl_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_set_mac_acl_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.set_mac_acl_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.set_mac_acl_wait);

	return ret;
}
#endif /* WL_CFG80211_ACL */
/* change_bss */
s32
wl_cfg80211_change_bss_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, struct bss_parameters *params)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_change_bss_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_change_bss_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_CHANGE_BSS;
	cmd->len = sizeof(xr_cmd_change_bss_t);
	data = (xr_cmd_change_bss_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->params = params;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->change_bss_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.change_bss_status;

}

int wl_cfg80211_change_bss_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_change_bss_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_change_bss_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_CHANGE_BSS;
	cmd->len = sizeof(xr_cmd_reply_change_bss_t);
	data = (xr_cmd_reply_change_bss_t *) &cmd->data[0];

	data->status = status;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}
int xr_cmd_change_bss_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_change_bss_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if(xr_cmd->len != sizeof(xr_cmd_change_bss_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_change_bss_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_change_bss(cmd->wiphy, cmd->dev, cmd->params);

	if (dest_pub) {
		wl_cfg80211_change_bss_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_change_bss_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_change_bss_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_change_bss_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.change_bss_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.change_bss_wait);

	return ret;
}

/* add_key */
s32
wl_cfg80211_add_key_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, u8 key_idx, bool pairwise, const u8 *mac_addr,
        struct key_params *params)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_add_key_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_add_key_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_ADD_KEY;
	cmd->len = sizeof(xr_cmd_add_key_t);
	data = (xr_cmd_add_key_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->key_idx = key_idx;
	data->pairwise = pairwise;
	data->mac_addr = mac_addr;
	data->params = params;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->add_key_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.add_key_status;

}

int wl_cfg80211_add_key_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_add_key_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_add_key_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_ADD_KEY;
	cmd->len = sizeof(xr_cmd_reply_add_key_t);
	data = (xr_cmd_reply_add_key_t *) &cmd->data[0];

	data->status = status;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if(cmd)
		kfree(cmd);

	return ret;
}

int xr_cmd_add_key_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_add_key_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_add_key_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_add_key_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);

	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_add_key(cmd->wiphy, cmd->dev, cmd->key_idx, cmd->pairwise, cmd->mac_addr, cmd->params);

	if (dest_pub) {
		wl_cfg80211_add_key_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_add_key_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_add_key_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_add_key_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.add_key_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.add_key_wait);

	return ret;
}

/* set_channel */
s32
wl_cfg80211_set_channel_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, struct ieee80211_channel *chan, enum nl80211_channel_type channel_type)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_set_channel_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_set_channel_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_SET_CHANNEL;
	cmd->len = sizeof(xr_cmd_set_channel_t);
	data = (xr_cmd_set_channel_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->chan = chan;
	data->channel_type = channel_type;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->set_channel_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.set_channel_status;

}

int wl_cfg80211_set_channel_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_set_channel_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_set_channel_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_SET_CHANNEL;
	cmd->len = sizeof(xr_cmd_reply_set_channel_t);
	data = (xr_cmd_reply_set_channel_t *) &cmd->data[0];

	data->status = status;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}
int xr_cmd_set_channel_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_set_channel_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_set_channel_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_set_channel_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_set_channel(cmd->wiphy, cmd->dev, cmd->chan,
		cmd->channel_type);

	if (dest_pub) {
		wl_cfg80211_set_channel_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_set_channel_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_set_channel_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_set_channel_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.set_channel_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.set_channel_wait);

	return ret;
}

/* config_default_key */
s32
wl_cfg80211_config_default_key_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy,
	struct net_device *dev, u8 key_idx, bool unicast, bool multicast)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_config_default_key_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_config_default_key_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_CONFIG_DEFAULT_KEY;
	cmd->len = sizeof(xr_cmd_config_default_key_t);
	data = (xr_cmd_config_default_key_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->key_idx = key_idx;
	data->unicast = unicast;
	data->multicast = multicast;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->config_default_key_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.config_default_key_status;

}

int wl_cfg80211_config_default_key_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_config_default_key_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_config_default_key_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_CONFIG_DEFAULT_KEY;
	cmd->len = sizeof(xr_cmd_reply_config_default_key_t);
	data = (xr_cmd_reply_config_default_key_t *) &cmd->data[0];

	data->status = status;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}
int xr_cmd_config_default_key_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_config_default_key_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_config_default_key_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_config_default_key_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_config_default_key(cmd->wiphy, cmd->dev, cmd->key_idx, cmd->unicast, cmd->multicast);

	if (dest_pub) {
		wl_cfg80211_config_default_key_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_config_default_key_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_config_default_key_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_config_default_key_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.config_default_key_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.config_default_key_wait);

	return ret;
}

/* stop_ap */
s32
wl_cfg80211_stop_ap_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_stop_ap_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_stop_ap_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_STOP_AP;
	cmd->len = sizeof(xr_cmd_stop_ap_t);
	data = (xr_cmd_stop_ap_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->stop_ap_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.stop_ap_status;

}

int wl_cfg80211_stop_ap_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_stop_ap_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_stop_ap_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_STOP_AP;
	cmd->len = sizeof(xr_cmd_reply_stop_ap_t);
	data = (xr_cmd_reply_stop_ap_t *) &cmd->data[0];

	data->status = status;

	dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}
int xr_cmd_stop_ap_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_stop_ap_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_stop_ap_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_stop_ap_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 19, 2)) || \
	defined(DHD_ANDROID_KERNEL5_15_SUPPORT)
	status = wl_cfg80211_stop_ap(cmd->wiphy, cmd->dev, 0);
#else
	status = wl_cfg80211_stop_ap(cmd->wiphy, cmd->dev);
#endif // endif
	if (dest_pub) {
		wl_cfg80211_stop_ap_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_stop_ap_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_stop_ap_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_stop_ap_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.stop_ap_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.stop_ap_wait);

	return ret;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
s32
wl_cfg80211_del_station_xr(
		dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
                struct wiphy *wiphy, struct net_device *ndev,
                struct station_del_parameters *params)
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
s32
wl_cfg80211_del_station_xr(
	dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *ndev,
        const u8* mac_addr)
#else
s32
wl_cfg80211_del_station_xr(
	dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *ndev,
        u8* mac_addr)
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)) */
{

	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_del_station_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_del_station_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL){
		return -EINVAL;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_DEL_STATION;
	cmd->len = sizeof(xr_cmd_del_station_t);
	data = (xr_cmd_del_station_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->ndev = ndev;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
	data->params = params;
#else
	data->mac_addr = mac_addr;
#endif // endif

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->del_station_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.del_station_status;

}

int wl_cfg80211_del_station_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_del_station_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_del_station_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_DEL_STATION;
	cmd->len = sizeof(xr_cmd_reply_del_station_t);
	data = (xr_cmd_reply_del_station_t *) &cmd->data[0];

	data->status = status;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);
	return ret;
}

int xr_cmd_del_station_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_del_station_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_del_station_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_del_station_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("XR_CMD_DEL_STATION cfg is NULL\n"));
		ret = BCME_ERROR;
	}
	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
	status = wl_cfg80211_del_station(cmd->wiphy, cmd->ndev, cmd->params);
#else
	status = wl_cfg80211_del_station(cmd->wiphy, cmd->ndev, cmd->mac_addr);
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)) */

	if (dest_pub) {
		wl_cfg80211_del_station_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_del_station_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_del_station_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_del_station_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.del_station_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.del_station_wait);

	return ret;
}

/*change station*/
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
s32
wl_cfg80211_change_station_xr(
	dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *dev,
        const u8* mac,
	struct station_parameters *params)
#else
s32
wl_cfg80211_change_station_xr(
	dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *dev,
        u8* mac,
	struct station_parameters *params)
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)) */
{

	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_change_station_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_change_station_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL){
		return -EINVAL;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_CHANGE_STATION;
	cmd->len = sizeof(xr_cmd_change_station_t);
	data = (xr_cmd_change_station_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->params = params;
	data->mac = mac;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->change_station_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.change_station_status;

}

int wl_cfg80211_change_station_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_change_station_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_change_station_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_CHANGE_STATION;
	cmd->len = sizeof(xr_cmd_reply_change_station_t);
	data = (xr_cmd_reply_change_station_t *) &cmd->data[0];

	data->status = status;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);
	return ret;
}

int xr_cmd_change_station_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_change_station_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_change_station_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_change_station_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("XR_CMD_DEL_STATION cfg is NULL\n"));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_change_station(cmd->wiphy, cmd->dev, cmd->mac, cmd->params);

	if (dest_pub) {
		wl_cfg80211_change_station_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_change_station_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_change_station_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_change_station_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.change_station_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.change_station_wait);

	return ret;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
s32
wl_cfg80211_mgmt_tx_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, bcm_struct_cfgdev *cfgdev,
        struct cfg80211_mgmt_tx_params *params, u64 *cookie)
#else
s32
wl_cfg80211_mgmt_tx_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, bcm_struct_cfgdev *cfgdev,
        struct ieee80211_channel *channel, bool offchan,
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 7, 0))
        enum nl80211_channel_type channel_type,
        bool channel_type_valid,
#endif /* LINUX_VERSION_CODE <= KERNEL_VERSION(3, 7, 0) */
        unsigned int wait, const u8* buf, size_t len,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)) || defined(WL_COMPAT_WIRELESS)
        bool no_cck,
#endif // endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)) || defined(WL_COMPAT_WIRELESS)
        bool dont_wait_for_ack,
#endif // endif
        u64 *cookie)
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0) */
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_mgmt_tx_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_mgmt_tx_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_MGMT_TX;
	cmd->len = sizeof(xr_cmd_mgmt_tx_t);
	data = (xr_cmd_mgmt_tx_t *) &cmd->data[0];
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
	data->wiphy = wiphy;
	data->cfgdev = cfgdev;
	data->params = params;
	data->cookie = cookie;
#else
	data->wiphy = wiphy;
	data->cfgdev = cfgdev;
        data->channel = channel;
	data->offchan = offchan;
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 7, 0))
	data->channel_type = channel_type;
	data->channel_type_valid = channel_type_valid;
#endif /* (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 7, 0)) */
	data->wait = wait;
	data->buf = buf;
	data->len = len;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)) || defined(WL_COMPAT_WIRELESS)
        data->no_cck = no_cck;
#endif // endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)) || defined(WL_COMPAT_WIRELESS)
	data->dont_wait_for_ack = dont_wait_for_ack;
#endif // endif
        data->cookie = cookie;
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)) */

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->mgmt_tx_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.mgmt_tx_status;

}

int wl_cfg80211_mgmt_tx_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_mgmt_tx_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_mgmt_tx_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_MGMT_TX;
	cmd->len = sizeof(xr_cmd_reply_mgmt_tx_t);
	data = (xr_cmd_reply_mgmt_tx_t *) &cmd->data[0];

	data->status = status;
	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}
int xr_cmd_mgmt_tx_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_mgmt_tx_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_mgmt_tx_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_mgmt_tx_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}
	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
	status = wl_cfg80211_mgmt_tx(cmd->wiphy, cmd->cfgdev, cmd->params, cmd->cookie);
#else
	status = wl_cfg80211_mgmt_tx(cmd->wiphy, cmd->cfgdev, cmd->channel, cmd->offchan,
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 7, 0))
			cmd->channel_type, cmd->channel_type_valid,
#endif /* LINUX_VERSION_CODE <= KERNEL_VERSION(3, 7, 0) */
			cmd->wait, cmd->buf, cmd->len,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)) || defined(WL_COMPAT_WIRELESS)
			cmd->no_cck,
#endif // endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)) || defined(WL_COMPAT_WIRELESS)
			cmd->dont_wait_for_ack,
#endif // endif
			cmd->cookie);
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0) */

	if (dest_pub) {
		wl_cfg80211_mgmt_tx_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_mgmt_tx_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_mgmt_tx_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_mgmt_tx_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.mgmt_tx_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.mgmt_tx_wait);

	return ret;
}

#ifdef WL_SAE
int
wl_cfg80211_external_auth_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev,
        struct cfg80211_external_auth_params *params)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_external_auth_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_external_auth_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL){
		return -EINVAL;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_EXTERNAL_AUTH;
	cmd->len = sizeof(xr_cmd_external_auth_t);
	data = (xr_cmd_external_auth_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->params = params;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->external_auth_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.external_auth_status;

}

int wl_cfg80211_external_auth_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_external_auth_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_external_auth_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_EXTERNAL_AUTH;
	cmd->len = sizeof(xr_cmd_reply_external_auth_t);
	data = (xr_cmd_reply_external_auth_t *) &cmd->data[0];

	data->status = status;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);
	return ret;
}

int xr_cmd_external_auth_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_external_auth_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_external_auth_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_external_auth_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_external_auth(cmd->wiphy, cmd->dev, cmd->params);

	if (dest_pub) {
		wl_cfg80211_external_auth_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_external_auth_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_external_auth_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_external_auth_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.external_auth_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.external_auth_wait);

	return ret;
}

#endif /* WL_SAE */

/* del_key */
s32 wl_cfg80211_del_key_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev,
        u8 key_idx, bool pairwise, const u8 *mac_addr)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_del_key_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_del_key_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_DEL_KEY;
	cmd->len = sizeof(xr_cmd_del_key_t);
	data = (xr_cmd_del_key_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->key_idx = key_idx;
	data->pairwise = pairwise;
	data->mac_addr = mac_addr;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->del_key_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.del_key_status;

}

int wl_cfg80211_del_key_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_del_key_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_del_key_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_DEL_KEY;
	cmd->len = sizeof(xr_cmd_reply_del_key_t);
	data = (xr_cmd_reply_del_key_t *) &cmd->data[0];

	data->status = status;
	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}

int xr_cmd_del_key_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_del_key_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_del_key_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_del_key_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_del_key(cmd->wiphy, cmd->dev, cmd->key_idx, cmd->pairwise, cmd->mac_addr);

	if (dest_pub) {
		wl_cfg80211_del_key_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_del_key_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_del_key_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_del_key_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.del_key_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.del_key_wait);

	return ret;
}
/* get_key */
s32 wl_cfg80211_get_key_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, u8 key_idx, bool pairwise, const u8 *mac_addr, void *cookie,
        void (*callback) (void *cookie, struct key_params * params))
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_get_key_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_get_key_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_GET_KEY;
	cmd->len = sizeof(xr_cmd_get_key_t);
	data = (xr_cmd_get_key_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->key_idx = key_idx;
	data->pairwise = pairwise;
	data->mac_addr = mac_addr;
	data->cookie = cookie;
	data->callback = callback;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->get_key_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.get_key_status;

}

int wl_cfg80211_get_key_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_get_key_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_get_key_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_GET_KEY;
	cmd->len = sizeof(xr_cmd_reply_get_key_t);
	data = (xr_cmd_reply_get_key_t *) &cmd->data[0];

	data->status = status;
	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}

int xr_cmd_get_key_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_get_key_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_get_key_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_get_key_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_get_key(cmd->wiphy, cmd->dev, cmd->key_idx, cmd->pairwise, cmd->mac_addr, cmd->cookie, cmd->callback);

	if(dest_pub) {
		wl_cfg80211_get_key_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_get_key_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_get_key_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_get_key_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.get_key_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.get_key_wait);

	return ret;
}
/* del_virtual_iface */
s32
wl_cfg80211_del_virtual_iface_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, bcm_struct_cfgdev *cfgdev)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_del_virtual_iface_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_del_virtual_iface_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_DEL_VIRTUAL_IFACE;
	cmd->len = sizeof(xr_cmd_del_virtual_iface_t);
	data = (xr_cmd_del_virtual_iface_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->cfgdev = cfgdev;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->del_virtual_iface_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.del_virtual_iface_status;

}

int wl_cfg80211_del_virtual_iface_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_del_virtual_iface_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_del_virtual_iface_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_DEL_VIRTUAL_IFACE;
	cmd->len = sizeof(xr_cmd_reply_del_virtual_iface_t);
	data = (xr_cmd_reply_del_virtual_iface_t *) &cmd->data[0];

	data->status = status;
	dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}
int xr_cmd_del_virtual_iface_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_del_virtual_iface_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_del_virtual_iface_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_del_virtual_iface_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_del_virtual_iface(cmd->wiphy, cmd->cfgdev);

	if (dest_pub) {
		wl_cfg80211_del_virtual_iface_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_del_virtual_iface_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_del_virtual_iface_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_del_virtual_iface_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.del_virtual_iface_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.del_virtual_iface_wait);

	return ret;
}

/* get station */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
s32
wl_cfg80211_get_station_xr(
	dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *dev,
        const u8* mac,
	struct station_info *sinfo)
#else
s32
wl_cfg80211_get_station_xr(
	dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *dev,
        u8* mac,
	struct station_info *sinfo)
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)) */
{

	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_get_station_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_get_station_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL){
		return -EINVAL;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_GET_STATION;
	cmd->len = sizeof(xr_cmd_get_station_t);
	data = (xr_cmd_get_station_t *) &cmd->data[0];

	data->wiphy = wiphy;
	data->dev = dev;
	data->sinfo = sinfo;
	data->mac = mac;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->get_station_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}

	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.get_station_status;

}

int wl_cfg80211_get_station_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_get_station_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_get_station_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_GET_STATION;
	cmd->len = sizeof(xr_cmd_reply_get_station_t);
	data = (xr_cmd_reply_get_station_t *) &cmd->data[0];

	data->status = status;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);
	return ret;
}

int xr_cmd_get_station_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_get_station_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_get_station_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_get_station_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("XR_CMD_GET_STATION cfg is NULL\n"));
		ret = BCME_ERROR;
	}

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_cfg80211_get_station(cmd->wiphy, cmd->dev, cmd->mac, cmd->sinfo);

	if (dest_pub) {
		wl_cfg80211_get_station_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_get_station_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_get_station_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_get_station_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.get_station_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.get_station_wait);

	return ret;
}

#ifdef WL_6E
s32 wl_xr_stop_fils_6g(struct wiphy *wiphy,
        struct net_device *dev,
        u8 stop_fils)
{
	s32 err = BCME_OK;
	s32 bssidx = 0;
	struct bcm_cfg80211 *cfg = wiphy_priv(wiphy);
	u8 stop_fils_6g = 0;

	if ((bssidx = wl_get_bssidx_by_wdev(cfg, dev->ieee80211_ptr)) < 0) {
                WL_ERR(("Find p2p index from wdev(%p) failed\n", dev->ieee80211_ptr));
                return BCME_ERROR;
        }

	stop_fils_6g = stop_fils;
	/* send IOVAR to firmware */
	err = wldev_iovar_setbuf_bsscfg(dev, "stop_fils_6g", &stop_fils_6g, sizeof(u8),
                        cfg->ioctl_buf, WLC_IOCTL_MAXLEN, bssidx, &cfg->ioctl_buf_sync);

	return err;
}

int wl_xr_stop_fils_6g_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_stop_fils_6g_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_stop_fils_6g_t * data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_STOP_FILS_6G;
	cmd->len = sizeof(xr_cmd_reply_stop_fils_6g_t);
	data = (xr_cmd_reply_stop_fils_6g_t *) &cmd->data[0];

	data->status = status;
	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return ret;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}

int xr_cmd_stop_fils_6g_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	s32 status = 0;
	struct bcm_cfg80211 *cfg = NULL;
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_stop_fils_6g_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_stop_fils_6g_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_stop_fils_6g_t *) &xr_cmd->data[0];
	cfg = (struct bcm_cfg80211 *)wiphy_priv(cmd->wiphy);
	if (!cfg) {
		DHD_ERROR(("%s cfg is NULL\n", __func__));
		ret = BCME_ERROR;
	}
	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = wl_xr_stop_fils_6g(cmd->wiphy, cmd->dev, cmd->stop_fils_6g_value);

	if(dest_pub) {
		wl_xr_stop_fils_6g_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_stop_fils_6g_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_stop_fils_6g_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_stop_fils_6g_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.stop_fils_6g_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.stop_fils_6g_wait);

	return ret;
}
#endif /* WL_6E */

#ifdef DHD_BANDSTEER
/* dhd_bandsteer_update_slave_ifaces */
s32 dhd_bandsteer_update_ifaces_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
		struct net_device *ndev)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_bstr_update_ifaces_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_bstr_update_ifaces_t *data = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(src_pub);
	xr_comp_wait_t *cmd_wait = &xr_ctx->xr_cmd_wait;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);

	if (cmd == NULL) {
		return -EINVAL;
	}

	/*Create cmd*/
	cmd->cmd_id = XR_CMD_BSTR_UPDATE_IFACES;
	cmd->len = sizeof(xr_cmd_bstr_update_ifaces_t);
	data = (xr_cmd_bstr_update_ifaces_t *)&cmd->data[0];

	data->ndev = ndev;

	ret = dhd_send_xr_cmd(dest_pub, cmd, size, &cmd_wait->bstr_update_ifaces_wait, TRUE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return -EINVAL;
	}
	if (cmd)
		kfree(cmd);

	return xr_ctx->xr_cmd_reply_status.bstr_update_ifaces_status;

}

int dhd_bstr_update_ifaces_xr_reply(dhd_pub_t *dest_pub, s32 status)
{
	xr_cmd_t *cmd = NULL;
	int size = sizeof(xr_cmd_t) + sizeof(xr_cmd_reply_bstr_update_ifaces_t);
	gfp_t flags = (in_atomic()) ? GFP_ATOMIC : GFP_KERNEL;
	xr_cmd_reply_bstr_update_ifaces_t *data = NULL;
	int ret = BCME_OK;

	cmd = (xr_cmd_t *) kzalloc(size, flags);
	if (cmd == NULL) {
		DHD_ERROR(("cmd is NULL\n"));
		return BCME_ERROR;
	}
	/*Create cmd*/
	cmd->cmd_id = XR_CMD_REPLY_BSTR_UPDATE_IFACES;
	cmd->len = sizeof(xr_cmd_reply_bstr_update_ifaces_t);
	data = (xr_cmd_reply_bstr_update_ifaces_t *)&cmd->data[0];

	data->status = status;
	ret = dhd_send_xr_cmd(dest_pub, cmd, size, NULL, FALSE);

	if (ret != BCME_OK) {
		DHD_ERROR(("%s: dhd_send_xr_cmd fail\n", __func__));
		return BCME_ERROR;
	}

	if (cmd)
		kfree(cmd);

	return ret;
}
int xr_cmd_bstr_update_ifaces_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	s32 status = 0;
	int ret = BCME_OK;
	struct net_device *primary_ndev = dhd_linux_get_primary_netdev(pub);
	struct bcm_cfg80211 *cfg = wl_get_cfg(primary_ndev);
	dhd_pub_t *dest_pub = NULL;
	xr_cmd_bstr_update_ifaces_t *cmd  = NULL;
	uint8 xr_role = DHD_GET_XR_ROLE(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	if (xr_cmd->len != sizeof(xr_cmd_bstr_update_ifaces_t)) {
		DHD_ERROR(("%s: cmd len error\n", __func__));
		ret = BCME_ERROR;
	}

	cmd  = (xr_cmd_bstr_update_ifaces_t *)&xr_cmd->data[0];

	dest_pub = XR_CMD_GET_DEST_PUB(cfg, xr_role);

	status = dhd_bandsteer_update_slave_ifaces(pub, cmd->ndev);

	if (dest_pub) {
		ret = dhd_bstr_update_ifaces_xr_reply(dest_pub, status);
	}

	return ret;
}

int xr_cmd_reply_bstr_update_ifaces_hndlr(dhd_pub_t *pub, xr_cmd_t *xr_cmd)
{
	int ret = BCME_OK;
	xr_cmd_reply_bstr_update_ifaces_t *reply = NULL;
	xr_ctx_t *xr_ctx = (xr_ctx_t *) DHD_GET_XR_CTX(pub);

	if (!xr_cmd) {
		DHD_ERROR(("%s: xr_cmd null\n", __func__));
		ret = BCME_ERROR;
	}

	reply = (xr_cmd_reply_bstr_update_ifaces_t *) &xr_cmd->data[0];
	xr_ctx->xr_cmd_reply_status.bstr_update_ifaces_status = reply->status;
	complete(&xr_ctx->xr_cmd_wait.bstr_update_ifaces_wait);
	return ret;
}
#endif /* DHD_BANDSTEER */

/* xr_cmd_handler */
int xr_cmd_deferred_handler(dhd_pub_t *pub, xr_buf_t *xr_buf)
{
	xr_cmd_t *xr_cmd = NULL;
	int ret = BCME_OK;

	if (!xr_buf) {
		DHD_ERROR(("xr_buf is NULL\n"));
		return BCME_ERROR;
	}

	xr_cmd = (xr_cmd_t *) &xr_buf->buf[0];

	switch (xr_cmd->cmd_id) {
	case XR_CMD_ADD_IF:
		{
			ret = xr_cmd_add_if_hndlr(pub, xr_cmd);
			break;
		}
	case XR_CMD_DEL_IF:
		{
			ret = xr_cmd_del_if_hndlr(pub, xr_cmd);
			break;
		}
#ifdef DHD_BANDSTEER
	case XR_CMD_BSTR_UPDATE_IFACES:
		{
			ret = xr_cmd_bstr_update_ifaces_hndlr(pub, xr_cmd);
			break;
		}
#endif /* DHD_BANDSTEER */
	case XR_CMD_DEL_VIRTUAL_IFACE:
		{
			xr_cmd_del_virtual_iface_hndlr(pub, xr_cmd);
			break;
		}
	default:
		DHD_ERROR(("%s:cmd id is not found\n", __func__));
		ret = BCME_ERROR;
	};

	return ret;
}

/* xr_cmd_handler */
int xr_cmd_handler(dhd_pub_t *pub, xr_buf_t *xr_buf)
{
	xr_cmd_t *xr_cmd = NULL;
	int ret = BCME_OK;

	if (!xr_buf) {
		DHD_ERROR(("xr_buf is NULL\n"));
		ret = BCME_ERROR;
		goto fail;
	}

	xr_cmd = (xr_cmd_t *) &xr_buf->buf[0];

	switch (xr_cmd->cmd_id) {
	case XR_CMD_ADD_IF:
	{
		dhd_wq_xr_cmd_handler(pub, xr_buf);
		return ret;
	}

	case XR_CMD_REPLY_ADD_IF:
	{
		ret = xr_cmd_reply_add_if_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_DEL_IF:
	{
		dhd_wq_xr_cmd_handler(pub, xr_buf);
		return ret;
	}

	case XR_CMD_REPLY_DEL_IF:
	{
		ret = xr_cmd_reply_del_if_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_SCAN:
	{
		ret = xr_cmd_scan_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_GET_TX_POWER:
	{
		ret = xr_cmd_get_tx_power_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_GET_TX_POWER:
	{
		ret = xr_cmd_reply_get_tx_power_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_SET_POWER_MGMT:
	{
		ret = xr_cmd_set_power_mgmt_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_SET_POWER_MGMT:
	{
		ret = xr_cmd_reply_set_power_mgmt_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_FLUSH_PMKSA:
	{
		ret = xr_cmd_flush_pmksa_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_FLUSH_PMKSA:
	{
		ret = xr_cmd_reply_flush_pmksa_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_CHANGE_VIRUTAL_IFACE:
	{
		ret = xr_cmd_change_virtual_iface_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_CHANGE_VIRUTAL_IFACE:
	{
		ret = xr_cmd_reply_change_virtual_iface_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_START_AP:
	{
		ret = xr_cmd_start_ap_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_START_AP:
	{
		ret = xr_cmd_reply_start_ap_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_SET_MAC_ACL:
	{
		ret = xr_cmd_set_mac_acl_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_SET_MAC_ACL:
	{
		ret = xr_cmd_reply_set_mac_acl_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_CHANGE_BSS:
	{
		ret = xr_cmd_change_bss_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_CHANGE_BSS:
	{
		ret = xr_cmd_reply_change_bss_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_ADD_KEY:
	{
		ret = xr_cmd_add_key_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_ADD_KEY:
	{
		ret = xr_cmd_reply_add_key_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_SET_CHANNEL:
	{
		ret = xr_cmd_set_channel_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_SET_CHANNEL:
	{
		ret = xr_cmd_reply_set_channel_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_CONFIG_DEFAULT_KEY:
	{
		ret = xr_cmd_config_default_key_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_CONFIG_DEFAULT_KEY:
	{
		ret = xr_cmd_reply_config_default_key_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_STOP_AP:
	{
		ret = xr_cmd_stop_ap_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_STOP_AP:
	{
		ret = xr_cmd_reply_stop_ap_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_DEL_STATION:
	{
		ret = xr_cmd_del_station_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_DEL_STATION:
	{
		ret = xr_cmd_reply_del_station_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_CHANGE_STATION:
	{
		ret = xr_cmd_change_station_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_CHANGE_STATION:
	{
		ret = xr_cmd_reply_change_station_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_MGMT_TX:
	{
		ret = xr_cmd_mgmt_tx_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_MGMT_TX:
	{
		ret = xr_cmd_reply_mgmt_tx_hndlr(pub, xr_cmd);
		break;
	}
#ifdef WL_SAE
	case XR_CMD_EXTERNAL_AUTH:
	{
		ret = xr_cmd_external_auth_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_EXTERNAL_AUTH:
	{
		ret = xr_cmd_reply_external_auth_hndlr(pub, xr_cmd);
		break;
	}
#endif /* WL_SAE */
	case XR_CMD_DEL_KEY:
	{
		ret = xr_cmd_del_key_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_DEL_KEY:
	{
		ret = xr_cmd_reply_del_key_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_GET_KEY:
	{
		ret = xr_cmd_get_key_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_GET_KEY:
	{
		ret = xr_cmd_reply_get_key_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_DEL_VIRTUAL_IFACE:
	{
		dhd_wq_xr_cmd_handler(pub, xr_buf);
		return ret;
	}
	case XR_CMD_REPLY_DEL_VIRTUAL_IFACE:
	{
		ret = xr_cmd_reply_del_virtual_iface_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_GET_STATION:
	{
		ret = xr_cmd_get_station_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_GET_STATION:
	{
		ret = xr_cmd_reply_get_station_hndlr(pub, xr_cmd);
		break;
	}
#ifdef DHD_BANDSTEER
	case XR_CMD_BSTR_UPDATE_IFACES:
	{
		dhd_wq_xr_cmd_handler(pub, xr_buf);
		return ret;
	}

	case XR_CMD_REPLY_BSTR_UPDATE_IFACES:
	{
		ret = xr_cmd_reply_bstr_update_ifaces_hndlr(pub, xr_cmd);
		break;
	}
#endif /* DHD_BANDSTEER */
#ifdef WL_6E
	case XR_CMD_STOP_FILS_6G:
	{
		ret = xr_cmd_stop_fils_6g_hndlr(pub, xr_cmd);
		break;
	}
	case XR_CMD_REPLY_STOP_FILS_6G:
	{
		ret = xr_cmd_reply_stop_fils_6g_hndlr(pub, xr_cmd);
		break;
	}
#endif /* WL_6E */
	default:
		DHD_ERROR(("%s:cmd id (%d) is not found\n", __func__, xr_cmd->cmd_id));
		ret = BCME_ERROR;
	};

fail:
	if (xr_buf)
		kfree(xr_buf);

	return ret;
}
#endif /* WL_DHD_XR */
