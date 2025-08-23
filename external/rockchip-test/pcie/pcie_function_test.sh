#!/bin/bash

# List detailed information of all PCI devices
echo "Listing detailed information of all PCI devices:"
lspci -vvv

# Specify the ID of the PCIe device to test (replace this with the actual device ID)
DEVICE_ID="21:00.0"

# Reading a specific PCIe device's configuration register (e.g., reading Vendor ID and Device ID)
# Note: 0x00 is the offset where the Vendor ID and Device ID are located
echo "Reading Vendor ID and Device ID of device $DEVICE_ID:"
VENDOR_DEVICE=$(setpci -s $DEVICE_ID 0x00.L)
echo $VENDOR_DEVICE

# Add more setpci commands to read other values...

# Performance testing (example using dd for a storage device connected via PCIe)
# Ensure to replace /dev/nvme0n1 with your actual PCIe storage device node
echo "Testing read/write performance of PCIe storage device..."

WRITE_OUTPUT=$(dd if=/dev/zero of=/dev/nvme0n1 bs=1M count=1024 conv=fdatasync oflag=direct 2>&1)
READ_OUTPUT=$(dd if=/dev/nvme0n1 of=/dev/null bs=1M count=1024 iflag=direct 2>&1)

echo "Write Performance:"
echo "$WRITE_OUTPUT" | grep -e "copied" -e "bytes"
echo "Read Performance:"
echo "$READ_OUTPUT" | grep -e "copied" -e "bytes"

echo "PCIe interface testing completed.
