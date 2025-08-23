#!/bin/bash -e

TARGET_DIR="$1"
[ "$TARGET_DIR" ] || exit 1

OVERLAY_DIR="$(dirname "$(realpath "$0")")"

find "$TARGET_DIR/etc" "$TARGET_DIR/lib" "$TARGET_DIR/usr/bin" \
	-name "*usbdevice*" -print0 -o -name ".usb_config" -print0 \
	-o -name "android-tools-adbd*" -print0 \
	-o -name "android-gadget*" -print0 \
	-o -name "adbd.sh" -print0 2>/dev/null | xargs -0 rm -rf

if [ ! "$RK_USB_GADGET" ]; then
	notice "USB gadget disabled..."
	exit 0
fi

install_adbd()
{
	[ -n "$RK_USB_ADBD" ] || return 0

	message "Installing adbd..."

	if [ -e "$TARGET_DIR/usr/bin/adbd" ] && \
		! grep -q ADBD_SHELL "$TARGET_DIR/usr/bin/adbd"; then
		message "Found incompatible adbd, removing it..."
		rm -f "$TARGET_DIR/usr/bin/adbd"
	fi

	ensure_tools "$TARGET_DIR/usr/bin/adbd"

	if [ "$RK_USB_ADBD_TCP_PORT" -ne 0 ]; then
		echo "export ADB_TCP_PORT=$RK_USB_ADBD_TCP_PORT" >> \
			"$TARGET_DIR/etc/profile.d/adbd.sh"
	fi

	if [ -n "$RK_USB_ADBD_SHELL" ]; then
		echo "export ADBD_SHELL=$RK_USB_ADBD_SHELL" >> \
			"$TARGET_DIR/etc/profile.d/adbd.sh"
	fi

	echo -e "#!/bin/sh\ntail -f \${@:--n 99999999} /var/log/messages" > \
		"$TARGET_DIR/usr/bin/logcat"
	chmod 755 "$TARGET_DIR/usr/bin/logcat"

	[ -n "$RK_USB_ADBD_SECURE" ] || return 0

	echo "export ADB_SECURE=1" >> "$TARGET_DIR/etc/profile.d/adbd.sh"

	if [ -n "$RK_USB_ADBD_PASSWORD" ]; then
		echo "export ADBD_AUTH_COMMAND=/usr/bin/adbd-auth.sh" >> \
			"$TARGET_DIR/etc/profile.d/adbd.sh"
		ADBD_PASSWORD_MD5="$(echo $RK_USB_ADBD_PASSWORD | md5sum)"
		install -m 0755 "$OVERLAY_DIR/adbd-auth.sh" \
			"$TARGET_DIR/usr/bin/adbd-auth.sh"
		sed -i "s/ADBD_PASSWORD_MD5/$ADBD_PASSWORD_MD5/g" \
			"$TARGET_DIR/usr/bin/adbd-auth.sh"
	fi

	[ "$RK_USB_ADBD_KEYS" ] || return 0

	sudo -u "#$RK_OWNER_UID" sh -c "cat $RK_USB_ADBD_KEYS" > \
		"$TARGET_DIR/adb_keys"
}

install_mtp()
{
	[ -n "$RK_USB_MTP" ] || return 0

	message "Installing MTP..."

	ensure_tools "$TARGET_DIR/usr/bin/umtprd"

	mkdir -p "$TARGET_DIR/etc/umtprd"

	MTP_ICON="$RK_CHIP_DIR/$RK_USB_MTP_ICON"
	if [ ! -r "$MTP_ICON" ]; then
		MTP_ICON="$OVERLAY_DIR/$RK_USB_MTP_ICON"
	fi
	install -m 0644 "$MTP_ICON" "$TARGET_DIR/etc/umtprd/devicon.ico"

	MTP_CONF="$RK_CHIP_DIR/$RK_USB_MTP_CONF"
	if [ ! -r "$MTP_CONF" ]; then
		MTP_CONF="$OVERLAY_DIR/$RK_USB_MTP_CONF"
	fi
	install -m 0644 "$MTP_CONF" "$TARGET_DIR/etc/umtprd/umtprd.conf"
}

install_ums()
{
	[ -n "$RK_USB_UMS" ] || return 0

	message "Installing UMS..."

	{
		echo "export UMS_FILE=${RK_USB_UMS_FILE:-/userdata/ums_shared.img}"
		echo "export UMS_SIZE=${RK_USB_UMS_SIZE:-256M}"
		echo "export UMS_FSTYPE=${RK_USB_UMS_FSTYPE:-vfat}"
		echo "export UMS_MOUNT=$([ -z "$RK_USB_UMS_MOUNT" ] || echo 1)"
		echo "export UMS_MOUNTPOINT=${RK_USB_UMS_MOUNTPOINT:-/mnt/ums}"
		echo "export UMS_RO=$([ -z "$RK_USB_UMS_RO" ] || echo 1)"
	} >> "$TARGET_DIR/etc/profile.d/usbdevice.sh"
}

install_uvc()
{
	[ -n "$RK_USB_UVC" ] || return 0

	message "Installing UVC..."

	ensure_tools "$TARGET_DIR/usr/bin/uvc-gadget"
}

usb_funcs()
{
	{
		echo "${RK_USB_ADBD:+adb}"
		echo "${RK_USB_ACM:+acm}"
		echo "${RK_USB_UVC:+uvc}"
		echo "${RK_USB_UAC1:+uac1}"
		echo "${RK_USB_UAC2:+uac2}"
		echo "${RK_USB_MIDI:+midi}"
		echo "${RK_USB_HID:+hid}"
		echo "${RK_USB_ECM:+ecm}"
		echo "${RK_USB_EEM:+eem}"
		echo "${RK_USB_NCM:+ncm}"
		echo "${RK_USB_RNDIS:+rndis}"
		echo "${RK_USB_NTB:+ntb}"
		echo "${RK_USB_MTP:+mtp}"
		echo "${RK_USB_UMS:+ums}"
		echo "${RK_USB_SERIAL:+gser}"
		echo "$RK_USB_EXTRA"
	} | xargs
}

message "Installing USB gadget to $TARGET_DIR..."

cd "$RK_SDK_DIR"

mkdir -p "$TARGET_DIR/etc" "$TARGET_DIR/lib" "$TARGET_DIR/usr/bin" \
	"$TARGET_DIR/usr/lib"

message "USB gadget functions: $(usb_funcs)"
mkdir -p "$TARGET_DIR/etc/profile.d"
{
	echo "export USB_FUNCS=\"$(usb_funcs)\""
	echo "export USB_VENDOR_ID=\"$RK_USB_VID\""
	if echo "$RK_USB_PID" | grep -iq '^0x'; then
		echo "export USB_PRODUCT_ID=\"$RK_USB_PID\""
	fi
	echo "export USB_FW_VERSION=\"$RK_USB_FW_VER\""
	echo "export USB_MANUFACTURER=\"$RK_USB_MANUFACTURER\""
	echo "export USB_PRODUCT=\"$RK_USB_PRODUCT\""
} > "$TARGET_DIR/etc/profile.d/usbdevice.sh"

install_adbd
install_mtp
install_ums
install_uvc

mkdir -p "$TARGET_DIR/lib/udev/rules.d"
install -m 0644 external/rkscript/61-usbdevice.rules \
	"$TARGET_DIR/lib/udev/rules.d/"

install -m 0755 external/rkscript/usbdevice "$TARGET_DIR/usr/bin/"

message "Installing USB services..."

install_sysv_service external/rkscript/S*usbdevice.sh 5 4 3 2 K01 0 1 6
install_busybox_service external/rkscript/S*usbdevice.sh
install_systemd_service external/rkscript/usbdevice.service

mkdir -p "$TARGET_DIR/etc/usbdevice.d"
for hook in $RK_USB_HOOKS; do
	if [ -r "$RK_CHIP_DIR/$hook" ]; then
		hook="$RK_CHIP_DIR/$hook"
	elif [ ! -r "$hook" ]; then
		warning "Ignore non-existant USB hook: $hook"
		continue
	fi

	message "Installing USB hook: $hook"
	install -m 0644 "$hook" "$TARGET_DIR/etc/usbdevice.d/"
done
