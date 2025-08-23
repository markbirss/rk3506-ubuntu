#!/bin/bash -e

POST_ROOTFS_ONLY=1

source "${RK_POST_HELPER:-$(dirname "$(realpath "$0")")/../post-hooks/post-helper}"

[ "$RK_ROOTFS_DEBUG_INFO" ] || exit 0

INFO_DIR="$TARGET_DIR/info"

message "Adding info dir..."

rm -rf "$INFO_DIR"
mkdir -p "$INFO_DIR"

cd "$RK_SDK_DIR"

yes | python3 .repo/repo/repo manifest -r \
	-o "$INFO_DIR/manifest.xml" &>/dev/null || true

cat "$RK_CONFIG" | sed "s/\(PASSWORD=\)\".*\"/\1\"********\"/" > \
	"$INFO_DIR/rockchip_config"

cp kernel/.config "$INFO_DIR/config-$RK_KERNEL_VERSION_RAW"
cp kernel/System.map "$INFO_DIR/System.map-$RK_KERNEL_VERSION_RAW"

EXTRA_FILES=" \
	/etc/os-release /etc/fstab /proc/config.gz \
	/proc/cpuinfo /proc/version /proc/cmdline /proc/kallsyms \
	/proc/interrupts /proc/softirqs /proc/device-tree /proc/diskstats \
	/proc/iomem /proc/meminfo /proc/partitions /proc/slabinfo \
	/proc/mpp_service /proc/rk_dmabuf /proc/rkcif-mipi-lvds /proc/rkisp0-vir0 \
	/sys/kernel/debug/wakeup_sources /sys/kernel/debug/clk/clk_summary \
	/sys/kernel/debug/gpio /sys/kernel/debug/pinctrl/ \
	/sys/kernel/debug/dma_buf /sys/kernel/debug/dri \
	"
ln -sf $EXTRA_FILES "$INFO_DIR/"

mkdir -p "$TARGET_DIR/etc/generate_logs.d"
echo -e '#!/bin/sh\ncp -rl /info/* . 2>/dev/null || true' > \
	"$TARGET_DIR/etc/generate_logs.d/10-info.sh"
chmod 755 "$TARGET_DIR/etc/generate_logs.d/10-info.sh"
