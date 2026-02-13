#!/bin/bash
#
# fix-runtime-pm.sh - Fix SAI1 Runtime PM Issue
#
# The Rockchip SAI driver uses Runtime PM to manage clocks.
# By default, the device is suspended which keeps clocks disabled.
# This script forces the device to stay active.
#
# Usage: ./fix-runtime-pm.sh [TARGET]
#   TARGET: SSH target (default: lyra@192.168.123.100)
#

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

TARGET="${1:-lyra@192.168.123.100}"
SAI_DEVICE="/sys/devices/platform/ff310000.sai"

echo "======================================================================="
echo "  SAI1 Runtime PM Fix"
echo "  Target: $TARGET"
echo "======================================================================="
echo ""

# Check SSH connection
if ! ssh -q "$TARGET" exit 2>/dev/null; then
    echo -e "${RED}✗${NC} Cannot connect to $TARGET"
    echo "Make sure:"
    echo "  1. Device is powered on and connected"
    echo "  2. SSH keys are set up (run: ssh-copy-id $TARGET)"
    exit 1
fi
echo -e "${GREEN}✓${NC} SSH connection OK"
echo ""

# Function to run commands on target
run_remote() {
    ssh "$TARGET" "$@"
}

# Check if SAI device exists
if ! run_remote "test -d $SAI_DEVICE"; then
    echo -e "${RED}✗${NC} SAI device not found at $SAI_DEVICE"
    echo "Make sure the correct device tree is loaded"
    exit 1
fi
echo -e "${GREEN}✓${NC} SAI device found"
echo ""

# Check current status
echo "Current Status:"
echo "-------------------------------------------------------------------"
CURRENT_STATUS=$(run_remote "cat $SAI_DEVICE/power/runtime_status")
CURRENT_CONTROL=$(run_remote "cat $SAI_DEVICE/power/control")
echo "  runtime_status: $CURRENT_STATUS"
echo "  control: $CURRENT_CONTROL"
echo ""

if [ "$CURRENT_STATUS" = "active" ] && [ "$CURRENT_CONTROL" = "on" ]; then
    echo -e "${GREEN}✓${NC} Runtime PM is already configured correctly"
    exit 0
fi

# Apply fix
echo "Applying Runtime PM Fix..."
echo "-------------------------------------------------------------------"

# Force device to stay active
if run_remote "echo on | sudo tee $SAI_DEVICE/power/control > /dev/null"; then
    echo -e "${GREEN}✓${NC} Set power/control = on"
else
    echo -e "${RED}✗${NC} Failed to set power control"
    exit 1
fi

# Verify new status
NEW_STATUS=$(run_remote "cat $SAI_DEVICE/power/runtime_status")
NEW_CONTROL=$(run_remote "cat $SAI_DEVICE/power/control")

echo ""
echo "New Status:"
echo "-------------------------------------------------------------------"
echo "  runtime_status: $NEW_STATUS"
echo "  control: $NEW_CONTROL"
echo ""

if [ "$NEW_STATUS" = "active" ] && [ "$NEW_CONTROL" = "on" ]; then
    echo -e "${GREEN}✓${NC} Runtime PM fix applied successfully"
    
    # Check if clock is now enabled
    CLOCK_ENABLED=$(run_remote "cat /sys/kernel/debug/clk/mclk_sai1/clk_enable_count")
    echo -e "${GREEN}✓${NC} MCLK clock enable_count: $CLOCK_ENABLED"
else
    echo -e "${YELLOW}⚠${NC} Unexpected status after fix"
    echo "Please check manually"
    exit 1
fi

echo ""
echo "======================================================================="
echo "  Runtime PM Fix Complete"
echo "======================================================================="
echo ""
echo "NOTE: This fix is NOT persistent across reboots!"
echo "      You need to apply it after each boot."
echo ""
echo "To make it persistent, add to /etc/rc.local or systemd service:"
echo "  echo on > /sys/devices/platform/ff310000.sai/power/control"
echo ""
echo "Or create a udev rule in /etc/udev/rules.d/99-sai-pm.rules:"
echo '  SUBSYSTEM=="platform", KERNEL=="ff310000.sai", ATTR{power/control}="on"'
echo ""
