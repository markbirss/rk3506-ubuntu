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
#ifndef __WL_CFG8021_XR_H__
#define __WL_CFG8021_XR_H__

typedef struct _xr_buf {
#define MAX_XR_CMD_SIZE 1024
	char	buf[MAX_XR_CMD_SIZE];
	int	len;
	bool	sync;
	bool	in_use;
} xr_buf_t;

enum xr_cmds {
	 /* sync */
	XR_CMD_ADD_IF,
	XR_CMD_REPLY_ADD_IF,
	/* sync */
	XR_CMD_DEL_IF,
	XR_CMD_REPLY_DEL_IF,
	 /* Async */
	XR_CMD_SCAN,
	/* sync */
        XR_CMD_GET_TX_POWER,
        XR_CMD_REPLY_GET_TX_POWER,
	/* sync */
        XR_CMD_SET_POWER_MGMT,
        XR_CMD_REPLY_SET_POWER_MGMT,
	 /* sync */
        XR_CMD_FLUSH_PMKSA,
        XR_CMD_REPLY_FLUSH_PMKSA,
	 /* sync */
        XR_CMD_CHANGE_VIRUTAL_IFACE,
        XR_CMD_REPLY_CHANGE_VIRUTAL_IFACE,
	 /* sync */
        XR_CMD_START_AP,
        XR_CMD_REPLY_START_AP,
	 /* sync */
        XR_CMD_SET_MAC_ACL,
        XR_CMD_REPLY_SET_MAC_ACL,
	 /* sync */
        XR_CMD_CHANGE_BSS,
        XR_CMD_REPLY_CHANGE_BSS,
	 /* sync */
        XR_CMD_ADD_KEY,
        XR_CMD_REPLY_ADD_KEY,
	 /* sync */
        XR_CMD_SET_CHANNEL,
        XR_CMD_REPLY_SET_CHANNEL,
	 /* sync */
        XR_CMD_CONFIG_DEFAULT_KEY,
        XR_CMD_REPLY_CONFIG_DEFAULT_KEY,
	 /* sync */
        XR_CMD_STOP_AP,
        XR_CMD_REPLY_STOP_AP,
	/* sync */
        XR_CMD_DEL_STATION,
        XR_CMD_REPLY_DEL_STATION,
	 /* sync */
        XR_CMD_CHANGE_STATION,
        XR_CMD_REPLY_CHANGE_STATION,
	 /* sync */
        XR_CMD_MGMT_TX,
        XR_CMD_REPLY_MGMT_TX,
	 /* sync */
        XR_CMD_EXTERNAL_AUTH,
        XR_CMD_REPLY_EXTERNAL_AUTH,
	 /* sync */
        XR_CMD_DEL_KEY,
        XR_CMD_REPLY_DEL_KEY,
	 /* sync */
        XR_CMD_GET_KEY,
        XR_CMD_REPLY_GET_KEY,
	 /* sync */
        XR_CMD_DEL_VIRTUAL_IFACE,
        XR_CMD_REPLY_DEL_VIRTUAL_IFACE,
	/* sync */
        XR_CMD_GET_STATION,
        XR_CMD_REPLY_GET_STATION,
	/* sync */
	XR_CMD_BSTR_UPDATE_IFACES,
	XR_CMD_REPLY_BSTR_UPDATE_IFACES,
#ifdef WL_6E
	/* sync */
	XR_CMD_STOP_FILS_6G,
	XR_CMD_REPLY_STOP_FILS_6G,
#endif /* WL_6E */
};

typedef struct _xr_cmd {
	int cmd_id;
	int len;
	uint8  data[0];
} xr_cmd_t;

/* add_if */
typedef struct _xr_cmd_add_if {
	u8 wl_iftype;
	const char *name;
	u8 *mac;
} xr_cmd_add_if_t;
typedef struct _xr_cmd_reply_add_if {
	struct wireless_dev *wdev;
} xr_cmd_reply_add_if_t;
/* del_if */
typedef struct _xr_cmd_del_if {
	struct wireless_dev *wdev;
	char *ifname;
} xr_cmd_del_if_t;

typedef struct _xr_cmd_reply_del_if {
	s32 status;
} xr_cmd_reply_del_if_t;

typedef struct _xr_cmd_scan {
	struct wiphy *wiphy;
	struct net_device *ndev;
	struct cfg80211_scan_request *request;
} xr_cmd_scan_t;

/* get_tx_power */
typedef struct _xr_cmd_get_tx_power {
	struct wiphy *wiphy;
#if defined(WL_CFG80211_P2P_DEV_IF)
	struct wireless_dev *wdev;
#endif // endif
	s32 *dbm;
}xr_cmd_get_tx_power_t;

typedef struct _xr_cmd_reply_get_tx_power {
	s32 status;
}xr_cmd_reply_get_tx_power_t;

/*set_power_mgmt*/
typedef struct _xr_cmd_set_power_mgmt {
	struct wiphy *wiphy;
	struct net_device *dev;
	bool enabled;
	s32 timeout;
}xr_cmd_set_power_mgmt_t;

typedef struct _xr_cmd_reply_set_power_mgmt {
	s32 status;
}xr_cmd_reply_set_power_mgmt_t;

/* flush pmksa */
typedef struct _xr_cmd_flush_pmksa {
	struct wiphy *wiphy;
	struct net_device *dev;
}xr_cmd_flush_pmksa_t;

typedef struct _xr_cmd_reply_flush_pmksa {
	s32 status;
}xr_cmd_reply_flush_pmksa_t;

typedef struct _xr_cmd_change_virtual_iface {
	struct wiphy *wiphy;
	struct net_device *ndev;
	int type;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0))
	u32 *flags;
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0) */
	struct vif_params *params;
}xr_cmd_change_virtual_iface_t;

typedef struct _xr_cmd_reply_change_virtual_iface {
	s32 status;
}xr_cmd_reply_change_virtual_iface_t;

/* start_ap */
typedef struct _xr_cmd_start_ap {
	struct wiphy *wiphy;
	struct net_device *dev;
	struct cfg80211_ap_settings *info;
} xr_cmd_start_ap_t;

typedef struct _xr_cmd_reply_start_ap {
	s32 status;
}xr_cmd_reply_start_ap_t;
#ifdef WL_CFG80211_ACL
/* set_mac_acl */
typedef struct _xr_cmd_set_mac_acl {
	struct wiphy *wiphy;
	struct net_device *cfgdev;
	const struct cfg80211_acl_data *acl;
}xr_cmd_set_mac_acl_t;

typedef struct _xr_cmd_reply_set_mac_acl {
	s32 status;
}xr_cmd_reply_set_mac_acl_t;
#endif /* WL_CFG80211_ACL */
/* change_bss */
typedef struct _xr_cmd_change_bss {
	struct wiphy *wiphy;
	struct net_device *dev;
	struct bss_parameters *params;
}xr_cmd_change_bss_t;

typedef struct _xr_cmd_reply_change_bss {
	s32 status;
}xr_cmd_reply_change_bss_t;

/* add_key */
typedef struct _xr_cmd_add_key {
	struct wiphy *wiphy;
	struct net_device *dev;
	u8 key_idx;
	bool pairwise;
	const u8 *mac_addr;
	struct key_params *params;
}xr_cmd_add_key_t;

typedef struct _xr_cmd_reply_add_key {
	s32 status;
}xr_cmd_reply_add_key_t;

/* set_channel */
typedef struct _xr_cmd_set_channel {
	struct wiphy *wiphy;
	struct net_device *dev;
	struct ieee80211_channel *chan;
	int channel_type;
}xr_cmd_set_channel_t;

typedef struct _xr_cmd_reply_set_channel {
	s32 status;
}xr_cmd_reply_set_channel_t;

/* config_default_key */
typedef struct _xr_cmd_config_default_key {
	struct wiphy *wiphy;
	struct net_device *dev;
	u8 key_idx;
	bool unicast;
	bool multicast;
}xr_cmd_config_default_key_t;

typedef struct _xr_cmd_reply_config_default_key {
	s32 status;
}xr_cmd_reply_config_default_key_t;

/* stop_ap */
typedef struct _xr_cmd_stop_ap {
	struct wiphy *wiphy;
	struct net_device *dev;
}xr_cmd_stop_ap_t;

typedef struct _xr_cmd_reply_stop_ap {
	s32 status;
}xr_cmd_reply_stop_ap_t;

/* del_station */
typedef struct _xr_cmd_del_station {
	struct wiphy *wiphy;
	struct net_device *ndev;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
	struct station_del_parameters *params;
#else
	u8* mac_addr;
#endif // endif
}xr_cmd_del_station_t;

typedef struct _xr_cmd_reply_del_station {
        s32 status;
}xr_cmd_reply_del_station_t;

/* change_station */
typedef struct _xr_cmd_change_station {
	struct wiphy *wiphy;
	struct net_device *dev;
	const u8 *mac;
	struct station_parameters *params;
} xr_cmd_change_station_t;

typedef struct _xr_cmd_reply_change_station {
        s32 status;
} xr_cmd_reply_change_station_t;

/* mgmt_tx */
typedef struct _xr_cmd_mgmt_tx {
	struct wiphy *wiphy;
#if defined(WL_CFG80211_P2P_DEV_IF)
struct wireless_dev *cfgdev;
#else
struct net_device *cfgdev;
#endif /* WL_CFG80211_P2P_DEV_IF */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
	struct cfg80211_mgmt_tx_params *params;
	u64 *cookie;
#else
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 7, 0))
        enum nl80211_channel_type channel_type;
        bool channel_type_valid;
#endif /* (LINUX_VERSION_CODE <= KERNEL_VERSION(3, 7, 0)) */
        unsigned int wait;
	const u8* buf;
	size_t len;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)) || defined(WL_COMPAT_WIRELESS)
        bool no_cck;
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)) || \
        defined(WL_COMPAT_WIRELESS) */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)) || defined(WL_COMPAT_WIRELESS)
        bool dont_wait_for_ack;
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)) || \
        defined(WL_COMPAT_WIRELESS) */
	u64 *cookie;
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0)) */

} xr_cmd_mgmt_tx_t;

typedef struct _xr_cmd_reply_mgmt_tx {
        s32 status;
} xr_cmd_reply_mgmt_tx_t;

#ifdef WL_SAE
/* external_auth */
typedef struct _xr_cmd_external_auth {
	struct wiphy *wiphy;
	struct net_device *dev;
        struct cfg80211_external_auth_params *params;
} xr_cmd_external_auth_t;

typedef struct _xr_cmd_reply_external_auth {
        s32 status;
} xr_cmd_reply_external_auth_t;
#endif /* WL_SAE */

/* del_key */
typedef struct _xr_cmd_del_key {
	struct wiphy *wiphy;
	struct net_device *dev;
	u8 key_idx;
	bool pairwise;
	const u8 *mac_addr;
}xr_cmd_del_key_t;

typedef struct _xr_cmd_reply_del_key {
	s32 status;
}xr_cmd_reply_del_key_t;

/* get_key */
typedef struct _xr_cmd_get_key {
	struct wiphy *wiphy;
	struct net_device *dev;
        u8 key_idx;
	bool pairwise;
	const u8 *mac_addr;
	void *cookie;
        void (*callback) (void *cookie, struct key_params * params);
}xr_cmd_get_key_t;

typedef struct _xr_cmd_reply_get_key {
	s32 status;
}xr_cmd_reply_get_key_t;

/* del_virtual_iface */
typedef struct _xr_cmd_del_virtual_iface {
	struct wiphy *wiphy;
	bcm_struct_cfgdev *cfgdev;
}xr_cmd_del_virtual_iface_t;

typedef struct _xr_cmd_reply_del_virtual_iface {
	s32 status;
}xr_cmd_reply_del_virtual_iface_t;

/* get_station */
typedef struct _xr_cmd_get_station {
	struct wiphy *wiphy;
	struct net_device *dev;
	const u8 *mac;
	struct station_info *sinfo;
} xr_cmd_get_station_t;

typedef struct _xr_cmd_reply_get_station {
        s32 status;
} xr_cmd_reply_get_station_t;

#ifdef DHD_BANDSTEER
/* bstr_update_ifaces */
typedef struct _xr_cmd_bstr_update_ifaces {
	struct net_device *ndev;
} xr_cmd_bstr_update_ifaces_t;

typedef struct _xr_cmd_reply_bstr_update_ifaces {
	s32 status;
} xr_cmd_reply_bstr_update_ifaces_t;
#endif /* DHD_BANDSTEER */

#ifdef WL_6E
/* stop fils 6g */
typedef struct _xr_cmd_stop_fils_6g {
	struct wiphy *wiphy;
	struct net_device *dev;
	u8 stop_fils_6g_value;
} xr_cmd_stop_fils_6g_t;

typedef struct _xr_cmd_reply_stop_fils_6g {
        s32 status;
} xr_cmd_reply_stop_fils_6g_t;
#endif /* WL_6E */

typedef struct _xr_cmd_reply_status {
	struct wireless_dev *add_if_wdev;
	s32 del_if_status;
	s32 get_tx_power_status;
	s32 set_power_mgmt_status;
	s32 flush_pmksa_status;
	s32 change_virtual_iface_status;
	s32 start_ap_status;
#ifdef WL_CFG80211_ACL
	s32 set_mac_acl_status;
#endif /* WL_CFG80211_ACL */
	s32 change_bss_status;
	s32 add_key_status;
	s32 set_channel_status;
	s32 config_default_key_status;
	s32 stop_ap_status;
	s32 del_station_status;
	s32 change_station_status;
	s32 mgmt_tx_status;
#ifdef WL_SAE
	s32 external_auth_status;
#endif /* WL_SAE */
	s32 del_key_status;
	s32 get_key_status;
	s32 del_virtual_iface_status;
	s32 get_station_status;
#ifdef DHD_BANDSTEER
	s32 bstr_update_ifaces_status;
#endif /* DHD_BANDSTEER */
#ifdef WL_6E
	s32 stop_fils_6g_status;
#endif /* WL_6E */
} xr_cmd_reply_status_t;
typedef struct _xr_comp_wait {
	struct completion add_if_wait;
	struct completion del_if_wait;
	struct completion get_tx_power_wait;
	struct completion set_power_mgmt_wait;
	struct completion flush_pmksa_wait;
	struct completion change_virtual_iface_wait;
	struct completion start_ap_wait;
#ifdef WL_CFG80211_ACL
	struct completion set_mac_acl_wait;
#endif /* WL_CFG80211_ACL */
	struct completion change_bss_wait;
	struct completion add_key_wait;
	struct completion set_channel_wait;
	struct completion config_default_key_wait;
	struct completion stop_ap_wait;
	struct completion del_station_wait;
	struct completion change_station_wait;
	struct completion mgmt_tx_wait;
#ifdef WL_SAE
	struct completion external_auth_wait;
#endif /* WL_SAE */
	struct completion del_key_wait;
	struct completion get_key_wait;
	struct completion del_virtual_iface_wait;
	struct completion get_station_wait;
#ifdef DHD_BANDSTEER
	struct completion bstr_update_ifaces_wait;
#endif /* DHD_BANDSTEER */
#ifdef WL_6E
	struct completion stop_fils_6g_wait;
#endif /* WL_6E */
} xr_comp_wait_t;
#define XR_CMD_GET_DEST_PUB(cfg, role) ((role == XR_MASTER) ? cfg->xr_slave_dhd_pub : cfg->pub)
#define DHD_XR_GET_SLAVE_WDEV(cfg) (cfg->xr_slave_prim_wdev ? cfg->xr_slave_prim_wdev : NULL)
#define DHD_XR_GET_SLAVE_NDEV(cfg)(DHD_XR_GET_SLAVE_WDEV(cfg) ? DHD_XR_GET_SLAVE_WDEV(cfg)->netdev : NULL)
#define MAX_XR_CMD_NUM 4
typedef struct _xr_ctx {
	uint8 xr_role;
	xr_buf_t xr_cmd_buf[MAX_XR_CMD_NUM];
	uint32 xr_cmd_store_idx;
	uint32 xr_cmd_sent_idx;
	xr_comp_wait_t xr_cmd_wait;
	xr_cmd_reply_status_t xr_cmd_reply_status;
} xr_ctx_t;
#define DHD_GET_XR_ROLE(pub) (((xr_ctx_t*)(pub->xr_ctx))->xr_role)
#define DHD_GET_XR_CTX(pub) (((dhd_pub_t*)pub)->xr_ctx)
/* The level of bus communication with the dongle */
extern int wl_cfg80211_dhd_xr_netdev_attach(struct net_device *ndev,  int wl_iftype, void *ifp,
	u8 bssidx, u8 ifidx);
extern int wl_cfg80211_dhd_xr_prim_netdev_attach(struct net_device *ndev,  wl_iftype_t wl_iftype, void *ifp,
	u8 bssidx, u8 ifidx);

extern int dhd_xr_init(dhd_pub_t *dhdp);
extern int dhd_xr_deinit(dhd_pub_t *dhdp);
extern s32 wl_cfg80211_dhd_xr_notify_ifchange(struct net_device *dev, int ifidx, char *name, uint8 *mac, uint8 bssidx);
extern int xr_cmd_handler(dhd_pub_t *pub, xr_buf_t *xr_buf);

extern s32 wl_cfg80211_del_if_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
	struct wireless_dev *wdev, char *ifname);
extern struct wireless_dev *wl_cfg80211_add_if_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
	u8 wl_iftype, const char *name, u8 *mac);
#if defined(WL_CFG80211_P2P_DEV_IF)
s32
extern wl_cfg80211_scan_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy,
	struct cfg80211_scan_request *request);
#else
s32
extern wl_cfg80211_scan_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy,
	struct net_device *ndev, struct cfg80211_scan_request *request);
#endif /* WL_CFG80211_P2P_DEV_IF */
#if defined(WL_CFG80211_P2P_DEV_IF)
extern s32 wl_cfg80211_get_tx_power_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy,
        struct wireless_dev *wdev, s32 *dbm);
#else
extern s32 wl_cfg80211_get_tx_power_xr(dhd_pub_t *src_pub,dhd_pub_t *dest_pub, struct wiphy *wiphy, s32 *dbm);
#endif /* WL_CFG80211_P2P_DEV_IF */
extern s32 wl_cfg80211_set_power_mgmt_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev,
        bool enabled, s32 timeout);
extern s32 wl_cfg80211_flush_pmksa_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev);
extern s32 wl_cfg80211_change_virtual_iface_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *ndev,
        enum nl80211_iftype type,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0))
        u32 *flags,
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0) */
        struct vif_params *params);
extern s32 wl_cfg80211_start_ap_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy 	*wiphy, struct net_device *dev, struct cfg80211_ap_settings *info);

extern int wl_cfg80211_set_mac_acl_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *cfgdev, const struct cfg80211_acl_data *acl);
extern s32 wl_cfg80211_change_bss_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, struct bss_parameters *params);
extern s32 wl_cfg80211_add_key_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, u8 key_idx, bool pairwise, const u8 *mac_addr,
        struct key_params *params);
extern s32
wl_cfg80211_set_channel_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, struct ieee80211_channel *chan, enum nl80211_channel_type channel_type);
s32
wl_cfg80211_config_default_key_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy,
        struct net_device *dev, u8 key_idx, bool unicast, bool multicast);
extern s32
wl_cfg80211_stop_ap_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
extern s32 wl_cfg80211_del_station_xr(
                dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
                struct wiphy *wiphy, struct net_device *ndev,
                struct station_del_parameters *params);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
extern s32 wl_cfg80211_del_station_xr(
        dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *ndev,
        const u8* mac_addr);
#else
extern s32 wl_cfg80211_del_station_xr(
        dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *ndev,
        u8* mac_addr);
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)) */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
extern s32
wl_cfg80211_change_station_xr(
	dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *dev,
        const u8* mac,
	struct station_parameters *params);
#else
extern s32
wl_cfg80211_change_station_xr(
	dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *dev,
        u8* mac,
	struct station_parameters *params);
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)) */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0))
s32
wl_cfg80211_mgmt_tx_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, bcm_struct_cfgdev *cfgdev,
        struct cfg80211_mgmt_tx_params *params, u64 *cookie);
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
        u64 *cookie);
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(3, 14, 0) */

#ifdef WL_SAE
extern int
wl_cfg80211_external_auth_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev,
        struct cfg80211_external_auth_params *params);
#endif /* WL_SAE */
extern s32 wl_cfg80211_del_key_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, u8 key_idx, bool pairwise, const u8 *mac_addr);
/* get_key */
extern s32 wl_cfg80211_get_key_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, u8 key_idx, bool pairwise, const u8 *mac_addr, void *cookie,
        void (*callback) (void *cookie, struct key_params * params));
extern s32
wl_cfg80211_del_virtual_iface_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, bcm_struct_cfgdev *cfgdev);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 16, 0))
s32
wl_cfg80211_get_station_xr(
	dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *dev,
        const u8* mac,
	struct station_info *sinfo);
#else
s32
wl_cfg80211_get_station_xr(
	dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
        struct wiphy *wiphy,
        struct net_device *dev,
        u8* mac,
	struct station_info *sinfo);
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)) */
extern int xr_cmd_deferred_handler(dhd_pub_t *pub, xr_buf_t *xr_buf);
#ifdef DHD_BANDSTEER
/* dhd_bandsteer_update_slave_ifaces */
s32 dhd_bandsteer_update_ifaces_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub,
                struct net_device *ndev);
#endif /* DHD_BANDSTEER */
#ifdef WL_6E
s32
wl_stop_fils_6g_xr(dhd_pub_t *src_pub, dhd_pub_t *dest_pub, struct wiphy *wiphy, struct net_device *dev, u8 fils_stop);
#endif /* WL_6E */
#endif /* __WL_CFG8021_XR_H__ */
