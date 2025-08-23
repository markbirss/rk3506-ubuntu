#!/bin/sh -e

IF_FILE="/var/run/.wifibt-interfaces"

do_insmod()
{
	if ! lsmod | grep -wq "$1"; then
		echo "Installing $1.ko ..."
		if [ "$1" = "rk960" ]; then
			insmod "$1.ko" fw_no_sleep=1
		else
			insmod "$1.ko"
		fi
		sleep "${2:-0}"
	fi
}

try_insmod()
{
	if [ -f "$1.ko" ]; then
		do_insmod "$1" $2
	fi
}

wifi_interfaces()
{
	for DEV in $(sed '1,2d;s/:.*//' /proc/net/dev); do
		case $DEV in
			lo | eth*) ;;
			p2p* | wlan*) echo $DEV ;;
			*)
				if grep -wq "DEVTYPE=wlan" \
					/sys/class/net/$DEV/uevent; then
					echo $DEV
				fi
				;;
		esac
	done
}

wifi_ready()
{
	[ -n "$(wifi_interfaces)" ]
}

bt_ready()
{
	hciconfig | grep -wqE "hci0"
}

rfkill_for_type()
{
	grep -rl "^${1:-bluetooth}$" /sys/class/rfkill/*/type | \
		sed 's/type$/state/' 2>/dev/null || true
}

bt_reset()
{
	RFKILL=$(rfkill_for_type bluetooth)
	[ "$RFKILL" ] || return 0

	echo 0 | tee $RFKILL >/dev/null
	echo 0 > /proc/bluetooth/sleep/btwrite
	sleep .5
	echo 1 | tee $RFKILL >/dev/null
	echo 1 > /proc/bluetooth/sleep/btwrite
	sleep .5
}

start_bt_brcm()
{
	killall -q -9 brcm_patchram_plus1 || true
	which brcm_patchram_plus1 >/dev/null

	bt_reset

	brcm_patchram_plus1 --enable_hci --no2bytes \
		--use_baudrate_for_download --tosleep 200000 \
		--baudrate 1500000 \
		--patchram ${WIFIBT_FIRMWARE_DIR:-/lib/firmware}/ $WIFIBT_TTY&
}


start_bt_rk_uart()
{
	killall -q -9 rk_hciattach || true
	which rk_hciattach >/dev/null

	bt_reset

	rk_hciattach -n -s 115200 $WIFIBT_TTY rockchip 3000000 flow nosleep 11:22:33:44:55:66&
}

start_bt_rtk_uart()
{
	killall -q -9 rtk_hciattach || true
	which rtk_hciattach >/dev/null

	bt_reset

	if [ -d /sys/module/hci_uart ]; then
		echo "Please disable CONFIG_BT_HCIUART in kernel!"
		return -1
	fi

	do_insmod hci_uart 0.5

	rtk_hciattach -n -s 115200 $WIFIBT_TTY rtk_h5&
}

start_bt_rtk_usb()
{
	bt_reset

	if [ -d /sys/module/btusb ]; then
		echo "Please disable CONFIG_BT_HCIBTUSB in kernel!"
		return -1
	fi

	do_insmod rtk_btusb
}

start_wifi()
{
	if wifi_ready; then
		echo "Wi-Fi is already inited..."
		for iface in $(wifi_interfaces); do
			ifup $iface 2>/dev/null || true &
			ifconfig $iface up || true
		done
		return 0
	fi

	cd "${WIFIBT_MODULE_DIR:-/lib/modules}"

	case "$WIFIBT_VENDOR" in
		Broadcom) try_insmod dhd_static_buf ;;
		Realtek) try_insmod rtkm ;;
		Aicsemi) try_insmod aic_load_fw ;;
	esac

	echo "Wi-Fi/BT module: $WIFIBT_MODULE.ko"
	do_insmod "$WIFIBT_MODULE"

	for i in `seq 60`; do
		if wifi_ready; then
			for iface in $(wifi_interfaces); do
				ifup $iface 2>/dev/null || true &
				ifconfig $iface up || true
			done
			echo "Successfully init Wi-Fi for $WIFIBT_CHIP!"
			return 0
		fi
		sleep .1
	done

	echo "Failed to init Wi-Fi for $WIFIBT_CHIP!"
	return 1
}

do_start_bt()
{
	cd "${WIFIBT_MODULE_DIR:-/lib/modules}"

	case "$WIFIBT_VENDOR" in
		Rockchip) start_bt_rk_uart;;
		Broadcom) start_bt_brcm;;
		Realtek)
			case "$WIFIBT_BUS" in
				usb) start_bt_rtk_usb;;
				*) start_bt_rtk_uart;;
			esac
			;;
		*)
			echo "Unknown Wi-Fi/BT chip, fallback to Broadcom..."
			start_bt_brcm
			;;
	esac
}

start_bt()
{
	if ! wifi_ready; then
		echo "Wi-Fi is not ready..."
		return 1
	fi

	if bt_ready; then
		echo "BT is already inited..."
		#hciconfig hci0 up 2>/dev/null || true
		return 0
	fi

	if do_start_bt; then
		for i in `seq 60`; do
			if bt_ready; then
				echo "Successfully init BT for $WIFIBT_CHIP!"
				#hciconfig hci0 up 2>/dev/null || true
				return 0
			fi
			sleep .1
		done
	fi

	echo "Failed to init BT for $WIFIBT_CHIP!"
	return 1
}

start_wifibt()
{
	WIFIBT_CHIP=$(wifibt-util.sh chip || true)
	if [ -z "$WIFIBT_CHIP" ]; then
		sleep 2
		WIFIBT_CHIP=$(wifibt-util.sh chip || true)
		if [ -z "$WIFIBT_CHIP" ];then
			echo "Failed to detect Wi-Fi/BT chip!"
			return 1
		fi
	fi

	WIFIBT_VENDOR="$(wifibt-util.sh vendor)"
	WIFIBT_BUS="$(wifibt-util.sh bus)"
	WIFIBT_MODULE="$(wifibt-util.sh module | cut -d'.' -f1)"
	WIFIBT_TTY=$(wifibt-util.sh tty)

	echo -e "\nHandling $1 for Wi-Fi/BT chip:\n$(wifibt-util.sh info)"

	case "$1" in
		start | restart)
			echo "Starting Wi-Fi/BT..."
			start_wifi
			start_bt
			echo "Done"
			;;
		start_wifi)
			echo "Starting Wi-Fi..."
			start_wifi
			echo "Done"
			;;
		start_bt)
			echo "Starting BT..."
			start_bt
			echo "Done"
			;;
	esac
}

stop_wifi()
{
	for iface in $(wifi_interfaces); do
		ifdown $iface 2>/dev/null || true &
		ifconfig $iface down || true
	done
}

stop_bt()
{
	hciconfig hci0 down 2>/dev/null || true
	killall -q -9 brcm_patchram_plus1 rtk_hciattach || true
}

stop_wifibt()
{
	echo -n "Stopping Wi-Fi/BT..."
	stop_wifi
	stop_bt
	echo "Done"
}

suspend_wifibt()
{
	# Store enabled Wi-Fi interfaces
	wifi_interfaces > "$IF_FILE" || true

	# Disable enabled Wi-Fi interfaces
	for iface in $(cat "$IF_FILE"); do
		echo "Disabling $iface..."
		ifconfig $iface down || true
	done

	# Restart BT later in resume, since it might lose power during S2R
	if bt_ready; then
		echo "Disabling BT..."
		echo "BT" >> "$IF_FILE"
		stop_bt
	fi
}

resume_wifibt()
{
	[ -r "$IF_FILE" ] || return 0

	# Retore enabled interfaces
	for iface in $(cat "$IF_FILE"); do
		echo "Enabling $iface..."
		case $iface in
			BT) start_wifibt start_bt || true ;;
			*) ifconfig $iface up || true ;;
		esac
	done

	rm -f "$IF_FILE"
}

case "$1" in
	start | restart | start_wifi | start_bt | "")
		start_wifibt "${1:-start}" &
		;;
	stop) stop_wifibt ;;
	suspend) suspend_wifibt ;;
	resume) resume_wifibt & ;;
	*)
		echo "Usage: [start|stop|start_wifi|start_bt|restart|suspend|resume]" >&2
		exit 3
		;;
esac

:
