#!/bin/bash -e

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"
RK_SDK_DIR="${RK_SDK_DIR:-$RK_SCRIPTS_DIR/../../../..}"

cd "$RK_SDK_DIR"

CHIPS="${@:-$(ls device/rockchip/.chips/)}"

for chip in $CHIPS; do
	for cfg in $(find "device/rockchip/.chips/$(basename "$chip")" \
		-mindepth 1 -maxdepth 2 -type f \
		-name "rockchip_*_defconfig"); do
		./build.sh $chip:$(basename $cfg)
		make savedefconfig
	done
done
