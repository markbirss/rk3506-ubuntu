#!/bin/bash
# Script to compile Device Tree Overlays for PCM5102A

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
KERNEL_DIR="$(cd "${SCRIPT_DIR}/../../../../.." && pwd)"
DTC="${KERNEL_DIR}/scripts/dtc/dtc"
DTC_FLAGS="-@ -I dts -O dtb -Wno-unit_address_vs_reg"

# Check if dtc exists
if [ ! -f "${DTC}" ]; then
    echo "Error: dtc not found at ${DTC}"
    echo "Please build the kernel first or install device-tree-compiler"
    exit 1
fi

echo "Compiling PCM5102A Device Tree Overlays..."
echo "Using DTC: ${DTC}"

# Compile SAI0 overlay
echo "Building rk3506-pcm5102a-sai0.dtbo..."
"${DTC}" ${DTC_FLAGS} -o "${SCRIPT_DIR}/rk3506-pcm5102a-sai0.dtbo" "${SCRIPT_DIR}/rk3506-pcm5102a-sai0.dts"

# Compile SAI1 overlay
echo "Building rk3506-pcm5102a-sai1.dtbo..."
"${DTC}" ${DTC_FLAGS} -o "${SCRIPT_DIR}/rk3506-pcm5102a-sai1.dtbo" "${SCRIPT_DIR}/rk3506-pcm5102a-sai1.dts"

echo ""
echo "Build complete! Overlay files created:"
echo "  - ${SCRIPT_DIR}/rk3506-pcm5102a-sai0.dtbo"
echo "  - ${SCRIPT_DIR}/rk3506-pcm5102a-sai1.dtbo"
echo ""
echo "To install on your device:"
echo "  sudo mkdir -p /boot/overlays"
echo "  sudo cp rk3506-pcm5102a-sai*.dtbo /boot/overlays/"
echo ""
echo "Then enable the overlay in your boot configuration."
