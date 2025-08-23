#!/bin/sh -e

BT_PINCTRL_DIR="/sys/firmware/devicetree/base/pinctrl/wireless-bluetooth"
CHIPS_FILE="/var/run/wifibt-chips.txt"
CHIP_FILE="/var/run/wifibt-info.txt"

bt_tty()
{
	if [ ! -d "$BT_PINCTRL_DIR" ]; then
		# No uart BT
		case "$(wifibt_bus)" in
			usb | "") return ;;
		esac

		# Fallback to the first uart device
		find /dev/ -name "ttyS*" | sort | head -n 1
		echo "# Missing $(basename "$BT_PINCTRL_DIR") in kernel dts" >&2
	else
		UART_DIR=$(find "$BT_PINCTRL_DIR/" -name "uart*" | head -n 1)
		echo "$UART_DIR" | sed 's~.*uart\([0-9]\+\).*~/dev/ttyS\1~'
	fi
}

wifibt_info()
{
	if [ -r "$CHIP_FILE" ]; then
		cat "$CHIP_FILE"
		return
	fi

	VIDS="$(grep -v "^#" "$CHIPS_FILE" | cut -f3 | cut -d":" -f1 | \
		sort | uniq | xargs | tr ' ' '|')"
	VENDORS="$(find \
		/sys/bus/usb/devices/*/ \
		/sys/bus/pci/devices/*/ \
		/sys/bus/sdio/devices/*/ \
		-type f -name vendor -o -name idVendor 2>/dev/null | \
		xargs grep -El "$VIDS" || true)"
	for VENDOR in $VENDORS; do
		BUS=$(echo "$VENDOR" | cut -d'/' -f4)
		case $BUS in
			usb) PRODUCT="$(dirname "$VENDOR")/idProduct" ;;
			*) PRODUCT="$(dirname "$VENDOR")/device" ;;
		esac

		VID="$(sed 's/^0x//' "$VENDOR")"
		PID="$(sed 's/^0x//' "$PRODUCT")"
		ID="$VID:$PID"
		CHIP="$(grep -v "^#" "$CHIPS_FILE" | grep -w -m 1 "$ID" || true)"
		if [ "$CHIP" ]; then
			echo "$CHIP" | sed "s/\($ID\)/$BUS\t\1/" | \
				tee "$CHIP_FILE"
			return
		fi
	done
}

wifibt_vendor()
{
	wifibt_info | cut -f 1
}

wifibt_chip()
{
	wifibt_info | cut -f 2
}

wifibt_bus()
{
	wifibt_info | cut -f 3
}

wifibt_id()
{
	wifibt_info | cut -f 4
}

wifibt_module()
{
	FIRST_KO=`wifibt_info | cut -f 5`
	SECOND_KO=`wifibt_info | cut -f 6`
	if [[ "$SECOND_KO" =~ "cyw" ]] && [ -e "/lib/modules/$SECOND_KO" ]; then
		echo "$SECOND_KO"
	else
		echo "$FIRST_KO"
	fi
}

if [ ! -r "$CHIPS_FILE" ]; then
	echo "# Vendor Name VID:PID Module\n" > "$CHIPS_FILE"

	# Prefer /etc/ version
	cat "/etc/wifibt-chips.txt" >>"$CHIPS_FILE" 2>/dev/null || true

	# Append built-in chip list
	cat << EOF >> "$CHIPS_FILE"
Realtek	RTL8188EU	0bda:0179	RTL8188EU.ko
Realtek	RTL8188EU	0bda:8179	RTL8188EU.ko
Realtek	RTL8188FU	0bda:f179	RTL8189FU.ko
Realtek	RTL8189ES	024c:8179	RTL8189ES.ko
Realtek	RTL8189FS	024c:f179	RTL8189FS.ko
Realtek	RTL8192DU	0bda:8194	RTL8192DU.ko
Realtek	RTL8192EU	0bda:818b	RTL8192EU.ko
Realtek	RTL8723BS	024c:b723	RTL8723BS.ko
Realtek	RTL8723BU	0bda:b720	RTL8723BU.ko
Realtek	RTL8723CS	024c:b703	RTL8723CS.ko
Realtek	RTL8723DS	024c:d723	RTL8723DS.ko
Realtek	RTL8812AU	0bda:8812	RTL8812AU.ko
Realtek	RTL8821CS	024c:c821	RTL8821CS.ko
Realtek	RTL8821CS	0bda:c821	RTL8821CS.ko
Realtek	RTL8822BE	10ec:b822	RTL8822BE.ko
Realtek	RTL8822BS	024c:b822	RTL8822BS.ko
Realtek	RTL8822BU	0bda:b82c	RTL8822BU.ko
Realtek	RTL8822CE	0bda:c123	RTL8822CE.ko
Realtek	RTL8822CE	10ec:c822	RTL8822CE.ko
Realtek	RTL8822CS	024c:c822	RTL8822CS.ko
Realtek	RTL8822CS	0bda:c822	RTL8822CS.ko
Realtek	RTL8852BE	10ec:b852	8852be.ko
Realtek	RTL8852BS	024c:b852	8852bs.ko
Realtek	RTL8852BU	0bda:a85b	8852bu.ko
Aicsemi	AIC8800DC	a69c:88dc	aic8800_fdrv.ko
Broadcom	AP6212A	02d0:a9a6	bcmdhd.ko
Broadcom	AP625X	02d0:a9bf	bcmdhd.ko	cyw43455.ko	# AP6255/AP6256/AP6745
Broadcom	AP6275P	14e4:449d	bcmdhd_pcie.ko
Broadcom	AP6275S	02d0:aae8	bcmdhd.ko
Broadcom	AP6276P	14e4:44a0	bcmdhd_pcie.ko
Broadcom	AP6276S	02d0:aaec	bcmdhd.ko
Broadcom	AP6330	02d0:4330	bcmdhd.ko
Broadcom	AP6335	02d0:4335	bcmdhd.ko
Broadcom	AP6354	02d0:4354	bcmdhd.ko
Broadcom	AP6356S	02d0:4356	bcmdhd.ko
Broadcom	AP6398S	02d0:4359	bcmdhd.ko
Rockchip	RK960	0296:5349	rk960.ko
Aicsemi	AIC8800DC	a69c:88dc	aic_load_fw.ko  aic8800_fdrv.ko
EOF
fi

if [ -e "$CHIP_FILE" ]; then
	# Invalid out-dated cache file
	if [ "$CHIP_FILE" -ot "$CHIPS_FILE" ] ||
		[ "$CHIP_FILE" -ot "$(realpath "$0")" ]; then
		rm -f "$CHIP_FILE"
	fi
fi

for cmd in "$(basename "$0")" "$1"; do
	# Invalid cache file when forcing reload
	case "$1" in
		-f | --force-reload) rm -f "$CHIP_FILE" ;;
	esac

	case "$cmd" in
		tty | bt-tty) bt_tty; exit ;;
		info | wifibt-info) wifibt_info; exit ;;
		vendor | wifibt-vendor) wifibt_vendor; exit ;;
		chip | wifibt-chip) wifibt_chip; exit ;;
		id | wifibt-id) wifibt_id; exit ;;
		bus | wifibt-bus) wifibt_bus; exit ;;
		module | wifibt-module) wifibt_module; exit ;;
	esac

	[ -z "$1" ] || shift
done

echo "$(basename "$0") <tty|info|vendor|id|bus|chip|module> [-f|--force-reload]"
