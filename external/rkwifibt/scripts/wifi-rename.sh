#!/bin/bash

# Get all net devices
interfaces=$(ifconfig -a | grep -E "wl.*" | awk '{print $1}' | sed 's/:$//')

# List all devices
for interface in $interfaces; do
# Get Mac Address
mac_address=$(ip link show $interface | grep "link/ether" | awk '{print $2}')

if [[ -n "$mac_address" && $interface =~ ^wl.* ]]; then
	echo "Found WiFi device: $interface with MAC: $mac_address"

	if [[ "$interface" != "wlan0" ]]; then
		sudo ip link set $interface down
		sudo ip link set $interface name wlan0
		sudo ip link set wlan0 up
		echo "Renamed $interface to wlan0"
	else
		echo "$interface is already named wlan0"
	fi
else
	echo "Failed to retrieve MAC address for $interface or invalid device."
fi
done
