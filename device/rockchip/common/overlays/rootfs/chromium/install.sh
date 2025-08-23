#!/bin/bash -e

TARGET_DIR="$1"
[ -d "$TARGET_DIR" ] || exit 1

stat "$TARGET_DIR/usr/bin/chromium" >/dev/null 2>&1 || exit 0

message "Disabling chromium commandline flag security warnings..."

CHROMIUM_POLICY="$TARGET_DIR/etc/chromium/policies/managed/managed_policies.json"
mkdir -p "$(dirname "$CHROMIUM_POLICY")"
sed -i "/CommandLineFlagSecurityWarningsEnabled/d" \
	"$CHROMIUM_POLICY" 2>/dev/null || true
echo '{"CommandLineFlagSecurityWarningsEnabled":false}' >> "$CHROMIUM_POLICY"
