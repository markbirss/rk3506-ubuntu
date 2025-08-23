#!/bin/bash -e

[ "$BR2_PACKAGE_DHCPCD" ]
[ "$BR2_PACKAGE_WPA_SUPPLICANT" ]
[ "$BR2_PACKAGE_IFUPDOWN_SCRIPTS" ]

if ! grep -wq "interfaces.d" "$TARGET_DIR/etc/network/interfaces"; then
	echo -e "\nsource-directory /etc/network/interfaces.d" >> \
		"$TARGET_DIR/etc/network/interfaces"
fi
