#!/bin/sh -e

SYSPATH="$1"
SYSNAME="$(basename "$SYSPATH")"

DEVNAME_PATH="$SYSPATH/device/name"
if [ -r "$DEVNAME_PATH" ]; then
	NAME="$(cat "$DEVNAME_PATH")"
else
	NAME="$SYSNAME"
fi

OUTPUT="/etc/profile.d/weston-calibration-$SYSNAME.sh"
cat <<EOF >"$OUTPUT"
# Calibration for $SYSPATH
export WESTON_TOUCH_CALIBRATION="\$(echo \$WESTON_TOUCH_CALIBRATION | tr ',' '\n' | grep -v "^$NAME" || true)"
export WESTON_TOUCH_CALIBRATION="$NAME:$2 $3 $4 $5 $6 $7,\$WESTON_TOUCH_CALIBRATION"
EOF

echo "Generated: $OUTPUT"
cat "$OUTPUT"
