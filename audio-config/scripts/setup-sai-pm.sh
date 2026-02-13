#!/bin/bash
#
# setup-sai-pm.sh - Install persistent Runtime PM fix for Rockchip SAI
#
# This script installs a udev rule that automatically disables Runtime PM
# for SAI devices at boot time, ensuring audio clocks stay active.
#
# Usage: ./setup-sai-pm.sh [TARGET]
#   TARGET: SSH target (default: root@192.168.123.100)
#   Use 'local' for local installation
#

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

TARGET="${1:-root@192.168.123.100}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
UDEV_RULE="99-rockchip-sai-pm.rules"
UDEV_RULE_PATH="$SCRIPT_DIR/$UDEV_RULE"

echo "======================================================================="
echo "  Rockchip SAI Runtime PM Setup (Persistent)"
echo "  Target: $TARGET"
echo "======================================================================="
echo ""

# Check if rule file exists
if [ ! -f "$UDEV_RULE_PATH" ]; then
    echo -e "${RED}✗${NC} udev rule file not found: $UDEV_RULE_PATH"
    exit 1
fi

# Function to run commands locally or remotely
if [ "$TARGET" = "local" ]; then
    echo -e "${BLUE}ℹ${NC} Installing locally"
    RUN_CMD=""
    COPY_CMD="sudo cp"
else
    echo -e "${BLUE}ℹ${NC} Installing on remote target: $TARGET"
    
    # Check SSH connection
    if ! ssh -q "$TARGET" exit 2>/dev/null; then
        echo -e "${RED}✗${NC} Cannot connect to $TARGET"
        echo "Make sure:"
        echo "  1. Device is powered on and connected"
        echo "  2. SSH keys are set up (run: ssh-copy-id $TARGET)"
        exit 1
    fi
    echo -e "${GREEN}✓${NC} SSH connection OK"
    
    RUN_CMD="ssh $TARGET"
    COPY_CMD="scp"
fi
echo ""

# Install udev rule
echo "Installing udev rule..."
echo "-------------------------------------------------------------------"

if [ "$TARGET" = "local" ]; then
    sudo cp "$UDEV_RULE_PATH" /etc/udev/rules.d/
    echo -e "${GREEN}✓${NC} Copied $UDEV_RULE to /etc/udev/rules.d/"
else
    # Copy to target
    scp "$UDEV_RULE_PATH" "$TARGET:/tmp/" > /dev/null
    $RUN_CMD "sudo cp /tmp/$UDEV_RULE /etc/udev/rules.d/"
    echo -e "${GREEN}✓${NC} Copied $UDEV_RULE to target:/etc/udev/rules.d/"
fi
echo ""

# Reload udev rules
echo "Reloading udev rules..."
echo "-------------------------------------------------------------------"

if [ "$TARGET" = "local" ]; then
    sudo udevadm control --reload-rules
    sudo udevadm trigger
else
    $RUN_CMD "sudo udevadm control --reload-rules"
    $RUN_CMD "sudo udevadm trigger"
fi
echo -e "${GREEN}✓${NC} udev rules reloaded"
echo ""

# Apply immediately (find all SAI devices)
echo "Applying Runtime PM fix to all SAI devices..."
echo "-------------------------------------------------------------------"

if [ "$TARGET" = "local" ]; then
    SAI_DEVICES=$(find /sys/devices/platform -name "ff3*0000.sai" 2>/dev/null || true)
else
    SAI_DEVICES=$($RUN_CMD "find /sys/devices/platform -name 'ff3*0000.sai' 2>/dev/null || true")
fi

if [ -z "$SAI_DEVICES" ]; then
    echo -e "${YELLOW}⚠${NC} No SAI devices found"
    echo "This is normal if device tree hasn't loaded SAI yet"
else
    for SAI_DEV in $SAI_DEVICES; do
        SAI_NAME=$(basename "$SAI_DEV")
        echo "  Processing $SAI_NAME..."
        
        if [ "$TARGET" = "local" ]; then
            echo on | sudo tee "$SAI_DEV/power/control" > /dev/null
            STATUS=$(cat "$SAI_DEV/power/runtime_status")
            CONTROL=$(cat "$SAI_DEV/power/control")
        else
            $RUN_CMD "echo on | sudo tee $SAI_DEV/power/control > /dev/null"
            STATUS=$($RUN_CMD "cat $SAI_DEV/power/runtime_status")
            CONTROL=$($RUN_CMD "cat $SAI_DEV/power/control")
        fi
        
        echo "    runtime_status: $STATUS"
        echo "    control: $CONTROL"
        
        if [ "$STATUS" = "active" ] && [ "$CONTROL" = "on" ]; then
            echo -e "    ${GREEN}✓${NC} $SAI_NAME is now active"
        else
            echo -e "    ${YELLOW}⚠${NC} $SAI_NAME: Unexpected status"
        fi
        echo ""
    done
fi

# Verify installation
echo "Verifying installation..."
echo "-------------------------------------------------------------------"

if [ "$TARGET" = "local" ]; then
    if [ -f "/etc/udev/rules.d/$UDEV_RULE" ]; then
        echo -e "${GREEN}✓${NC} udev rule is installed"
    else
        echo -e "${RED}✗${NC} udev rule installation failed"
        exit 1
    fi
else
    if $RUN_CMD "test -f /etc/udev/rules.d/$UDEV_RULE"; then
        echo -e "${GREEN}✓${NC} udev rule is installed on target"
    else
        echo -e "${RED}✗${NC} udev rule installation failed on target"
        exit 1
    fi
fi

echo ""
echo "======================================================================="
echo "  ${GREEN}Installation Complete!${NC}"
echo "======================================================================="
echo ""
echo "The Runtime PM fix is now PERSISTENT across reboots."
echo ""
echo "What was installed:"
echo "  • udev rule: /etc/udev/rules.d/$UDEV_RULE"
echo "  • Covers all Rockchip SAI devices (SAI0, SAI1, etc.)"
echo ""
echo "The fix will be applied automatically at every boot."
echo ""
echo "To verify after reboot:"
echo "  cat /sys/devices/platform/ff300000.sai/power/control"
echo "  cat /sys/devices/platform/ff300000.sai/power/runtime_status"
echo ""
echo "To uninstall (if needed):"
echo "  sudo rm /etc/udev/rules.d/$UDEV_RULE"
echo "  sudo udevadm control --reload-rules"
echo ""
