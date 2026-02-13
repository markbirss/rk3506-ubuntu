#!/bin/bash
#
# sai-control.sh - Minimal SAI and DAC Power Control for shairport-sync
#
# Usage:
#   ./sai-control.sh activate    # Enable SAI and DAC for audio playback
#   ./sai-control.sh deactivate  # Disable SAI and DAC to save power
#

SAI_DEVICE="/sys/devices/platform/ff310000.sai/power/control"
DAC_ENABLE_GPIO="/sys/class/gpio/gpio11/value"  # GPIO0_B3 = 11
DAC_EXPORT="/sys/class/gpio/export"
DAC_DIRECTION="/sys/class/gpio/gpio11/direction"

# Initialize GPIO if not already exported
init_gpio() {
    if [ ! -d "/sys/class/gpio/gpio11" ]; then
        echo 11 > "$DAC_EXPORT" 2>/dev/null || true
        sleep 0.1
    fi
    echo out > "$DAC_DIRECTION" 2>/dev/null || true
}

case "$1" in
    activate)
        init_gpio
        echo 1 > "$DAC_ENABLE_GPIO" 2>/dev/null || exit 1
        echo on > "$SAI_DEVICE" 2>/dev/null || exit 1
        ;;
    deactivate)
        echo auto > "$SAI_DEVICE" 2>/dev/null || exit 1
        init_gpio
        echo 0 > "$DAC_ENABLE_GPIO" 2>/dev/null || exit 1
        ;;
    *)
        echo "Usage: $0 {activate|deactivate}" >&2
        exit 1
        ;;
esac