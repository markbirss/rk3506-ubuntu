#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"
RK_SDK_DIR="${RK_SDK_DIR:-$RK_SCRIPTS_DIR/../../../..}"
RK_CHIP_DIR="${RK_CHIP_DIR:-$RK_SDK_DIR/device/rockchip/.chip}"
RK_PARAMETER="${RK_PARAMETER:-parameter.txt}"

cd "$RK_SDK_DIR"

[ -r "kernel/.config" -a -r "$RK_CHIP_DIR/$RK_PARAMETER" ] || exit 0
grep -q "^CMDLINE:.*:grow)$" "$RK_CHIP_DIR/$RK_PARAMETER" || exit 0

DM_VERITY=$(grep "^CONFIG_DM_VERITY=y$" kernel/.config || true)
GROW_ALIGN_CFG="$(grep "^GROW_ALIGN:" "$RK_CHIP_DIR/$RK_PARAMETER" || true)"
GROW_ALIGN_VAL="$(echo $GROW_ALIGN_CFG | cut -d':' -f2- | xargs || true)"

if [ "$DM_VERITY" -a "$GROW_ALIGN_VAL" = "1" ]; then
	# DM verity + grow align
	exit 0
fi

if [ -z "$DM_VERITY" -a "$GROW_ALIGN_CFG" -a "$GROW_ALIGN_VAL" != "1" ]; then
	# !DM verity + !grow align
	exit 0
fi

echo -e "\e[35m"
if [ "$DM_VERITY" ]; then
	echo "CONFIG_DM_VERITY is enabled in kernel!"
	echo "Please set \"GROW_ALIGN: 1\" in $RK_CHIP_DIR/$RK_PARAMETER:"
else
	echo "CONFIG_DM_VERITY isn't enabled in kernel!"
	echo "Please set \"GROW_ALIGN: 0\" in $RK_CHIP_DIR/$RK_PARAMETER:"
fi
echo -e "\e[0m"
exit 1
