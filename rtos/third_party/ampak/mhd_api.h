/*
 * Copyright 2018, Broadcom Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Inc.;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Inc.
 */

#ifndef INCLUDED_MHD_API_H
#define INCLUDED_MHD_API_H

#include <stdint.h>

#define PM1_POWERSAVE_MODE          ( 1 )
#define PM2_POWERSAVE_MODE          ( 2 )
#define NO_POWERSAVE_MODE           ( 0 )

#define MHD_MK_CNTRY(a, b, rev)                          \
   (((unsigned char)(a)) + (((unsigned char)(b)) << 8) + \
   (((unsigned short)(rev)) << 16))

/**
 * Enumeration of Wi-Fi security modes
 */
typedef enum
{
    MHD_SECURE_OPEN = 0, // 0 OPEN
    MHD_WPA_PSK_AES,     // WPA-PSK AES
    MHD_WPA2_PSK_AES,    // WPA2-PSK AES
    MHD_WEP_OPEN,        // WEP+OPEN
    MHD_WEP_SHARED,      // WEP+SHARE
    MHD_WPA_PSK_TKIP,    // WPA-PSK TKIP
    MHD_WPA_PSK_MIXED,   // WPA-PSK AES & TKIP MIXED
    MHD_WPA2_PSK_TKIP,   // WPA2-PSK TKIP
    MHD_WPA2_PSK_MIXED,  // WPA2-PSK AES & TKIP MIXED
    MHD_WPA2_PSK_SHA256, // WPA2-PSK-SHA256
    MHD_WPA3_WPA2_PSK,   // WPA3 SAE & WPA2-PSK AES
    MHD_WPA3_PSK_SAE,    // WPA3-PSK SAE
    MHD_WPS_OPEN,        // WPS OPEN, NOT supported
    MHD_WPS_AES,         // WPS AES, NOT supported
    MHD_IBSS_OPEN,       // ADHOC, NOT supported
    MHD_WPA_ENT_AES,     // WPA-ENT AES, NOT supported
    MHD_WPA_ENT_TKIP,    // WPA-ENT TKIP, NOT supported
    MHD_WPA_ENT_MIXED,   // WPA-ENT AES & TKIP MIXED, NOT supported
    MHD_WPA2_ENT_AES,    // WPA2-ENT AES, NOT supported
    MHD_WPA2_ENT_TKIP,   // WPA2-ENT TKIP, NOT supported
    MHD_WPA2_ENT_MIXED,  // WPA2-ENT AES & TKIP MIXED, NOT supported
    MHD_STA_SECURITY_AUTO = 99, // MHD auto select security
    MHD_STA_SECURITY_MAX // Enum count, used for sanity check.
} mhd_sta_security_t;

typedef enum
{
    MHD_AP_OPEN = 0,       // 0 OPEN
    MHD_AP_WPA_AES_PSK,    // 1 WPA-PSK AES
    MHD_AP_WPA2_AES_PSK,   // 2 WPA2-PSK AES
    MHD_AP_WEP_OPEN,       // 3 WEP+OPEN
    MHD_AP_WEP_SHARED,     // 4 WEP+SHARE
    MHD_AP_WPA_TKIP_PSK,   // 5 WPA-PSK TKIP
    MHD_AP_WPA_MIXED_PSK,  // 6 WPA-PSK AES & TKIP MIXED
    MHD_AP_WPA2_TKIP_PSK,  // 7 WPA2-PSK TKIP
    MHD_AP_WPA2_MIXED_PSK, // 8 WPA2-PSK AES & TKIP MIXED
    MHD_AP_WPA3_WPA2_PSK,  //10 WPA3 & WPA2 AES
    MHD_AP_WPA3_SAE,       //10 WPA3 SAE
    MHD_AP_WPS_OPEN,       //11 WPS OPEN, NOT supported
    MHD_AP_WPS_AES,        //12 WPS AES, NOT supported
    MHD_AP_WPA2_WPA_AES_PSK,   //13 WPA2/WPA-PSK AES
    MHD_AP_WPA2_WPA_TKIP_PSK,  //14 WPA2/WPA-PSK TKIP
    MHD_AP_WPA2_WPA_MIXED_PSK, //15 WPA2/WPA-PSK AES & TKIP MIXED
} mhd_ap_security_t;

typedef struct
{
    uint8_t octet[6]; /* Unique 6-byte MAC address */
} mhd_mac_t;

typedef struct
{
    uint8_t length;    /* SSID length */
    uint8_t value[32]; /* SSID name (AP name)  */
} mhd_ssid_t;

typedef struct
{
    char ssid[32];
    char bssid[6];
    uint32_t channel;
    uint32_t security;
    uint32_t rssi;
    uint32_t data_rate;
    char ccode[4];
} mhd_ap_info_t;

typedef struct
{
    mhd_ssid_t SSID;
    uint32_t security;
    uint8_t channel;
    uint8_t security_key_length;
    char security_key[ 64 ];
    mhd_mac_t BSSID;
} mhd_ap_connect_info_t;

extern void mhd_set_country_code(uint32_t country);
extern void mhd_get_country_code(uint32_t *country);
extern int mhd_module_init(void);
extern int mhd_module_exit(void);

extern int mhd_start_scan(void);
extern int mhd_stop_scan(void);
extern int mhd_get_scan_results(mhd_ap_info_t *results, int *num);

typedef void (*mhd_scan_report_callback_t)(mhd_ap_info_t *result);
extern int mhd_scan_ap(mhd_scan_report_callback_t scan_report_cb, mhd_ap_info_t *scan_report_data);

extern int mhd_get_mac_address(mhd_mac_t *mac);
extern int mhd_set_mac_address(mhd_mac_t mac);

// station connects to ap. 0:success, others:failed
// security: 0-open, 1-wpa_psk_aes, 2-wpa2_psk_aes
extern int mhd_sta_connect(const char *ssid, char *bssid, uint8_t security, const char *password, uint8_t channel);
extern int mhd_sta_disconnect(uint8_t force);
extern int mhd_sta_get_connection(void);
extern int mhd_join_ap(const unsigned char *ssid, const char *password);
extern int mhd_leave_ap(void);

extern int mhd_sta_network_up(uint32_t ip, uint32_t gateway, uint32_t netmask);
extern int mhd_sta_network_down(void);

typedef void (*mhd_link_callback_t)(void);
extern int mhd_sta_register_link_callback(mhd_link_callback_t link_up_cb, mhd_link_callback_t link_down_cb);
extern int mhd_sta_deregister_link_callback(mhd_link_callback_t link_up_cb, mhd_link_callback_t link_down_cb);

extern int mhd_sta_get_rssi(void);
extern int mhd_sta_get_rate(void);
extern int mhd_sta_get_noise(void);

extern int mhd_sta_get_bssid(char mac_addr[]);
extern int mhd_sta_get_ssid(char ssid_data[]);

extern uint32_t mhd_sta_ipv4_ipaddr(void);
extern uint32_t mhd_sta_ipv4_gateway(void);
extern uint32_t mhd_sta_ipv4_netmask(void);

extern int mhd_sta_set_powersave(uint8_t mode, uint8_t time_ms);
extern int mhd_sta_get_powersave(uint8_t *mode, uint8_t *time_ms);

extern int mhd_sta_set_bcn_li_dtim(uint8_t dtim);
extern int mhd_sta_get_bcn_li_dtim(void);
extern int mhd_sta_set_dtim_interval(int dtim_interval_ms);

extern int mhd_host_get_mac_address(mhd_mac_t *mac);
extern void mhd_host_set_mac_address(mhd_mac_t mac);

// ssid:  less than 32 bytes
// password: less than 32 bytes
// security: 0-open, 1-wpa_psk_aes, 2-wpa2_psk_aes
// channel: 1~13
extern int mhd_softap_start(const char *ssid, const char *password, uint8_t security, uint8_t channel);
extern int mhd_softap_stop(uint8_t force);

extern int mhd_softap_set_hidden(int enable);
extern int mhd_softap_get_hidden(void);

extern int mhd_softap_start_dhcpd(uint32_t ip_address);
extern int mhd_softap_stop_dhcpd(void);

extern int mhd_softap_get_mac_list(mhd_mac_t *mac_list, uint32_t *count);
extern int mhd_softap_get_rssi(mhd_mac_t *mac_addr);
extern int mhd_softap_deauth_assoc_sta(const mhd_mac_t *mac);

typedef void (*mhd_client_callback_t)(mhd_mac_t);
extern int mhd_softap_register_client_callback(mhd_client_callback_t client_assoc_cb, mhd_client_callback_t client_disassoc_cb);

extern int mhd_wifi_get_channel(int interface, uint32_t *channel);
extern int mhd_wifi_set_channel(int interface, uint32_t channel);
extern int mhd_wifi_get_max_associations(uint32_t *max_assoc);
extern int mhd_wifi_set_max_associations(uint32_t max_assoc);

extern int mhd_enable_monitor_mode(void);
extern int mhd_disable_monitor_mode(void);

extern void mhd_gpio_config(uint8_t type, uint32_t port, uint32_t pin);

typedef void (*mhd_packet_callback_t)(void *buffer, int interface);
extern void mhd_set_netif_rx_callback(mhd_packet_callback_t function);
extern int mhd_wifi_set_raw_packet_processor(mhd_packet_callback_t function);
extern int mhd_host_network_xmit(void *packet, int interface);
extern int mhd_network_xmit(void *packet, int interface);

//WIFI Test Using
extern void bcm_wifi_commands(uint32_t argc, char **argv);
extern int mhd_wl_cmd(uint32_t argc, char **argv);

#endif /* ifndef INCLUDED_MHD_API_H */
