#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"
RK_SDK_DIR="${RK_SDK_DIR:-$RK_SCRIPTS_DIR/../../../..}"
RK_DATA_DIR="${RK_DATA_DIR:-$RK_SCRIPTS_DIR/../data}"

cd "$RK_SDK_DIR"

"$RK_SCRIPTS_DIR/check-grow-align.sh"

check_usb_gadget()
{
	FUNC="$1"
	shift
	CONFIGS="$@"

	for cfg in $CONFIGS; do
		if grep -wq "$cfg=y" kernel/.config; then
			continue
		fi

		echo -e "\e[35m"
		echo "Your kernel doesn't support USB gadget: $FUNC"
		echo "Please enable:"
		echo "$CONFIGS"
		echo -e "\e[0m"
		exit 1
	done
}

if [ -r "kernel/.config" ]; then
	EXT4_CONFIGS=$(export | grep -oE "\<RK_.*=\"ext4\"$" || true)

	if [ "$EXT4_CONFIGS" ] && \
		! grep -q "CONFIG_EXT4_FS=y" kernel/.config; then
		echo -e "\e[35m"
		echo "Your kernel doesn't support ext4 filesystem"
		echo "Please enable CONFIG_EXT4_FS for:"
		echo "$EXT4_CONFIGS"
		echo -e "\e[0m"
		exit 1
	fi

	# For bypassing DRM permision checks.
	if grep -q "CONFIG_DRM=y" kernel/.config &&
		! grep -q "CONFIG_DRM_IGNORE_IOTCL_PERMIT=y" kernel/.config; then
		echo -e "\e[35m"
		echo "Please enable CONFIG_DRM_IGNORE_IOTCL_PERMIT in kernel."
		echo -e "\e[0m"
		exit 1
	fi

	"$RK_SCRIPTS_DIR/check-security.sh" kernel config

	# For using flock.
	if [ "$RK_USB_GADGET" -o "$RK_ROOTFS_INPUT_EVENT_DAEMON" ] && \
		! grep -q "CONFIG_FILE_LOCKING=y" kernel/.config; then
		echo -e "\e[35m"
		echo "Please enable CONFIG_FILE_LOCKING in kernel."
		echo -e "\e[0m"
		exit 1
	fi

	[ -z "$RK_USB_ADBD" ] || check_usb_gadget adb CONFIG_USB_CONFIGFS_F_FS
	[ -z "$RK_USB_MTP" ] || check_usb_gadget mtp CONFIG_USB_CONFIGFS_F_FS
	[ -z "$RK_USB_NTB" ] || check_usb_gadget ntb CONFIG_USB_CONFIGFS_F_FS
	[ -z "$RK_USB_ACM" ] || check_usb_gadget acm CONFIG_USB_CONFIGFS_ACM
	[ -z "$RK_USB_UVC" ] || check_usb_gadget uvc CONFIG_USB_CONFIGFS_F_UVC
	[ -z "$RK_USB_UAC1" ] || check_usb_gadget uac1 CONFIG_USB_CONFIGFS_F_UAC1
	[ -z "$RK_USB_UAC2" ] || check_usb_gadget uac2 CONFIG_USB_CONFIGFS_F_UAC2
	[ -z "$RK_USB_MIDI" ] || check_usb_gadget midi CONFIG_USB_CONFIGFS_F_MIDI
	[ -z "$RK_USB_HID" ] || check_usb_gadget hid CONFIG_USB_CONFIGFS_F_HID
	[ -z "$RK_USB_ECM" ] || check_usb_gadget ecm CONFIG_USB_CONFIGFS_ECM
	[ -z "$RK_USB_EEM" ] || check_usb_gadget eem CONFIG_USB_CONFIGFS_EEM
	[ -z "$RK_USB_NCM" ] || check_usb_gadget ncm CONFIG_USB_CONFIGFS_NCM
	[ -z "$RK_USB_RNDIS" ] || check_usb_gadget rndis CONFIG_USB_CONFIGFS_RNDIS
	[ -z "$RK_USB_UMS" ] || \
		check_usb_gadget ums CONFIG_USB_CONFIGFS_MASS_STORAGE
	[ -z "$RK_USB_SERIAL" ] || check_usb_gadget gser CONFIG_USB_CONFIGFS_SERIAL
fi

if ! kernel/scripts/mkbootimg &>/dev/null; then
	echo -e "\e[35m"
	echo "Your python3 is too old for kernel: $(python3 --version)"
	echo "Please update it:"
	"$RK_SCRIPTS_DIR/install-python3.sh"
	echo -e "\e[0m"
	exit 1
fi

if ! lz4 -h 2>&1 | grep -q favor-decSpeed; then
	echo -e "\e[35m"
	echo "Your lz4 is too old for kernel: $(lz4 --version)"
	echo "Please update it:"
	echo "git clone https://github.com/lz4/lz4.git --depth 1 -b v1.9.4"
	echo "cd lz4"
	echo "sudo make install -j8"
	echo -e "\e[0m"
	exit 1
fi

"$RK_SCRIPTS_DIR/check-package.sh" python-is-python3 python
"$RK_SCRIPTS_DIR/check-package.sh" flex

"$RK_SCRIPTS_DIR/check-header.sh" openssl openssl/ssl.h libssl-dev
"$RK_SCRIPTS_DIR/check-header.sh" gmp gmp.h libgmp-dev
"$RK_SCRIPTS_DIR/check-header.sh" mpc mpc.h libmpc-dev
"$RK_SCRIPTS_DIR/check-header.sh" ncurses ncurses.h libncurses-dev
