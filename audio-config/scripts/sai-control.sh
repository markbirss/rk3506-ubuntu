#!/bin/bash
#
# sai-control.sh - Minimal SAI Power Control for shairport-sync
#
# Usage:
#   ./sai-control.sh activate    # Enable SAI for audio playback
#   ./sai-control.sh deactivate  # Disable SAI to save power
#


SAI_DEVICE="/sys/devices/platform/ff310000.sai/power/control"

case "$1" in
    activate)
        echo on > "$SAI_DEVICE" 2>/dev/null || exit 1
        ;;
    deactivate)
        echo auto > "$SAI_DEVICE" 2>/dev/null || exit 1
        ;;
    *)
        echo "Usage: $0 {activate|deactivate}" >&2
        exit 1
        ;;
esac