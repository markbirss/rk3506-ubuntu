#!/bin/bash

# Unload and reload the Bluetooth driver
echo "Unloading the btusb module..."
rmmod btusb || { echo "Failed to unload btusb module, it may not be loaded."; exit 1; }
echo "Reloading the btusb module..."
modprobe btusb || { echo "Failed to load btusb module. Please check the driver."; exit 1; }
sleep 2

# Reset the Bluetooth adapter
echo "Resetting the Bluetooth adapter..."
hcitool cmd 0x03 0x0003
sleep 1

# Ensure hci0 exists
if hciconfig hci0 > /dev/null 2>&1; then
	echo "hci0 found, proceeding with further operations..."
else
	echo "hci0 not detected. Please ensure the Bluetooth device is connected."
	exit 1
fi

# Disable, reset, and enable hci0
echo "Disabling hci0..."
hciconfig hci0 down || { echo "Failed to disable hci0."; exit 1; }
sleep 0.5

echo "Resetting hci0..."
hciconfig hci0 reset || { echo "Failed to reset hci0."; exit 1; }
sleep 0.5

echo "Enabling hci0..."
hciconfig hci0 up || { echo "Failed to enable hci0."; exit 1; }

# Set hci0 name to the hostname
echo "Setting hci0 name to the hostname ($HOSTNAME)..."
hciconfig hci0 name "$HOSTNAME" || { echo "Failed to set hci0 name."; exit 1; }

echo "Bluetooth device initialized successfully."
