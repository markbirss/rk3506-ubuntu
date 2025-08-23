#!/bin/sh

# This file is executed by ifupdown in pre-up, post-up, pre-down and
# post-down phases of network interface configuration.

# run this script only for interfaces which have wpa-conf option
[ -z "$IF_WPA_CONF" ] && exit 0

# Check for original conf
WPA_CONF="${WPA_CONF:-/etc/wpa_supplicant.conf}"
[ -r "$WPA_CONF" ] || exit 0

mkdir -p "$(dirname "$IF_WPA_CONF")" || exit 0
rm -rf "$IF_WPA_CONF"

# The original conf is writable
if touch "$WPA_CONF" 2>/dev/null; then
	ln -sf "$WPA_CONF" "$IF_WPA_CONF"
	exit 0
fi

# Prefer using /userdata to store new conf
if [ -d /userdata ] && touch /userdata 2>/dev/null; then
	mkdir -p /userdata/cfg
	cp -p "$WPA_CONF" /userdata/cfg/wpa_config.conf
	ln -sf /userdata/cfg/wpa_config.conf "$IF_WPA_CONF"
	exit 0
fi

cp -p "$WPA_CONF" "$IF_WPA_CONF"
exit 0
