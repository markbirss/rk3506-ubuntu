#!/bin/bash
#
# check-sai-pm.sh - Check Runtime PM status of all SAI devices
#
# Usage: ./check-sai-pm.sh [TARGET]
#   TARGET: SSH target (default: lyra@192.168.123.100)
#   Use 'local' for local check
#

set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

TARGET="${1:-lyra@192.168.123.100}"

echo "======================================================================="
echo "  Rockchip SAI Runtime PM Status"
echo "  Target: $TARGET"
echo "======================================================================="
echo ""

# Function to run commands
if [ "$TARGET" = "local" ]; then
    RUN_CMD=""
    SAI_DEVICES=$(find /sys/devices/platform -name "ff3*0000.sai" 2>/dev/null || true)
else
    if ! ssh -q "$TARGET" exit 2>/dev/null; then
        echo -e "${RED}✗${NC} Cannot connect to $TARGET"
        exit 1
    fi
    RUN_CMD="ssh $TARGET"
    SAI_DEVICES=$($RUN_CMD "find /sys/devices/platform -name 'ff3*0000.sai' 2>/dev/null || true")
fi

if [ -z "$SAI_DEVICES" ]; then
    echo -e "${RED}✗${NC} No SAI devices found"
    exit 1
fi

echo "Found SAI devices:"
echo "-------------------------------------------------------------------"

for SAI_DEV in $SAI_DEVICES; do
    SAI_NAME=$(basename "$SAI_DEV")
    echo ""
    echo -e "${BLUE}Device: $SAI_NAME${NC}"
    echo "  Path: $SAI_DEV"
    
    if [ "$TARGET" = "local" ]; then
        STATUS=$(cat "$SAI_DEV/power/runtime_status" 2>/dev/null || echo "unknown")
        CONTROL=$(cat "$SAI_DEV/power/control" 2>/dev/null || echo "unknown")
        USAGE=$(cat "$SAI_DEV/power/runtime_usage" 2>/dev/null || echo "unknown")
    else
        STATUS=$($RUN_CMD "cat $SAI_DEV/power/runtime_status 2>/dev/null || echo unknown")
        CONTROL=$($RUN_CMD "cat $SAI_DEV/power/control 2>/dev/null || echo unknown")
        USAGE=$($RUN_CMD "cat $SAI_DEV/power/runtime_usage 2>/dev/null || echo unknown")
    fi
    
    echo "  runtime_status: $STATUS"
    echo "  control: $CONTROL"
    echo "  runtime_usage: $USAGE"
    
    # Check clock status if debugfs available
    if [ "$TARGET" = "local" ]; then
        MCLK_PATH="/sys/kernel/debug/clk/mclk_${SAI_NAME/ff3/sai}/clk_enable_count"
        if [ -f "$MCLK_PATH" ]; then
            CLOCK_COUNT=$(cat "$MCLK_PATH" 2>/dev/null || echo "N/A")
            echo "  clock_enable_count: $CLOCK_COUNT"
        fi
    else
        MCLK_PATH="/sys/kernel/debug/clk/mclk_${SAI_NAME/ff3/sai}/clk_enable_count"
        CLOCK_COUNT=$($RUN_CMD "cat $MCLK_PATH 2>/dev/null || echo 'N/A'")
        if [ "$CLOCK_COUNT" != "N/A" ]; then
            echo "  clock_enable_count: $CLOCK_COUNT"
        fi
    fi
    
    # Status check
    if [ "$STATUS" = "active" ] && [ "$CONTROL" = "on" ]; then
        echo -e "  ${GREEN}✓ Status: OK (Active and forced on)${NC}"
    elif [ "$STATUS" = "active" ]; then
        echo -e "  ${YELLOW}⚠ Status: Active but not forced (may suspend)${NC}"
    else
        echo -e "  ${RED}✗ Status: PROBLEM (Not active)${NC}"
    fi
done

echo ""
echo "-------------------------------------------------------------------"

# Check if udev rule is installed
UDEV_RULE="/etc/udev/rules.d/99-rockchip-sai-pm.rules"
if [ "$TARGET" = "local" ]; then
    RULE_EXISTS=$(test -f "$UDEV_RULE" && echo "yes" || echo "no")
else
    RULE_EXISTS=$($RUN_CMD "test -f $UDEV_RULE && echo yes || echo no")
fi

echo ""
if [ "$RULE_EXISTS" = "yes" ]; then
    echo -e "${GREEN}✓${NC} Persistent udev rule is installed: $UDEV_RULE"
else
    echo -e "${YELLOW}⚠${NC} Persistent udev rule NOT installed"
    echo "  Run: ./setup-sai-pm.sh to install"
fi

echo ""
echo "======================================================================="
