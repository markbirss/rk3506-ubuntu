#!/bin/bash -e

TARGET_DIR="$1"
[ "$TARGET_DIR" ] || exit 1

OVERLAY_DIR="$(dirname "$(realpath "$0")")"

message "Installing full-busybox..."
RK_ROOTFS_PREFER_PREBUILT_TOOLS=y ensure_tools "$TARGET_DIR/bin/busybox"

# Login root on serial console
if [ -r "$TARGET_DIR/etc/inittab" ]; then
	sed -i 's~\(respawn:.*\)/bin/start_getty.*~\1/bin/login -p root~' \
		"$TARGET_DIR/etc/inittab"
fi

# Drop Poky warnings in motd
if [ -r "$TARGET_DIR/etc/motd" ]; then
	sed -i '/^WARNING: Poky/,+2d' "$TARGET_DIR/etc/motd"
fi

# Use uid to detect root user
if [ -r "$TARGET_DIR/etc/profile" ]; then
	sed -i 's~"$HOME" != "/home/root"~$(id -u) -ne 0~' \
		"$TARGET_DIR/etc/profile"
fi

if [ -r "$TARGET_DIR/etc/ntp.conf" ] && \
	! grep -q "^server .*ntp" "$TARGET_DIR/etc/ntp.conf"; then
	message "Applying global NTP server..."
	echo >> "$TARGET_DIR/etc/ntp.conf"
	echo "server 0.pool.ntp.org iburst" >> "$TARGET_DIR/etc/ntp.conf"
	echo "server 1.pool.ntp.org iburst" >> "$TARGET_DIR/etc/ntp.conf"
	echo "server 2.pool.ntp.org iburst" >> "$TARGET_DIR/etc/ntp.conf"
	echo "server 3.pool.ntp.org iburst" >> "$TARGET_DIR/etc/ntp.conf"
fi

# Switch from the compat to the files module
# See: https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=880846
if [ -r "$TARGET_DIR/etc/nsswitch.conf" ]; then
	sed -i 's/\<compat$/files/' "$TARGET_DIR/etc/nsswitch.conf"
fi

if [ -x "$TARGET_DIR/usr/bin/weston" ]; then
	sed -i 's/\(WESTON_USER=\)weston/\1root/' \
		"$TARGET_DIR/etc/init.d/weston"

	message "Installing weston overlay: $OVERLAY_DIR/weston to $TARGET_DIR..."
	$RK_RSYNC "$OVERLAY_DIR/weston/" "$TARGET_DIR/" \
		--exclude="$(basename "$(realpath "$0")")"

	message "Installing Rockchip test scripts to $TARGET_DIR..."
	$RK_RSYNC "$RK_SDK_DIR/external/rockchip-test/" \
		"$TARGET_DIR/rockchip-test/" \
		--include="camera/" --include="video/" --exclude="/*"
fi

if [ "$RK_YOCTO_USBMOUNT" ]; then
	message "Installing usbmount..."

	tar xvf "$OVERLAY_DIR/usbmount.tar" -C "$TARGET_DIR"

	for type in storage udisk sdcard; do
		mkdir -p "$TARGET_DIR/media/$type"{1,2,3}
		mkdir -p "$TARGET_DIR/mnt/$type"
		rm -rf "$TARGET_DIR/media/${type}0"
		ln -sf "/mnt/$type" "$TARGET_DIR/media/${type}0"
	done
fi

if [ -x "$TARGET_DIR/usr/bin/pulseaudio" ]; then
	message "Installing pulseaudio files..."

	$RK_RSYNC "$OVERLAY_DIR/pulseaudio/etc/pulse" "$TARGET_DIR/etc/"

	mkdir -p "$TARGET_DIR/etc/rcS.d"
	install -m 0755 "$OVERLAY_DIR/pulseaudio/etc/rcS.d/S50pulseaudio" \
		"$TARGET_DIR/etc/rcS.d/"
fi
