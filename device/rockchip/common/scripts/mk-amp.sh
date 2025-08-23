#!/bin/bash -e

RK_RTOS_BSP_DIR=$RK_SDK_DIR/rtos/bsp/rockchip
ITS_FILE="$RK_CHIP_DIR/$RK_AMP_FIT_ITS"

RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"

usage_hook()
{
	usage_oneline "amp" "build and pack amp system"
}

amp_get_value()
{
	echo "$1" | grep -owP "$2\s*=\s*<([^>]+)>" | awk -F'<|>' '{print $2}'
}

amp_get_string()
{
	echo "$1" | grep -owP "$2\s*=.*\"([^>]+)\"" | awk -F'"' '{print $2}'
}

amp_get_node()
{
	echo "$1" | \
	awk -v node="$2" \
		'$0 ~ node " {" {
		in_block = 1;
		block = $0;
		next;
		}
		in_block {
			block = block "\n" $0;
			if (/}/) {
				count_open = gsub(/{/, "&", block);
				count_close = gsub(/}/, "&", block);
				if (count_open == count_close) {
					in_block = 0;
					print block;
					block = "";
				}
			}
		}'
}

amp_touch_export()
{
	if [ -n "$2" ]; then
		DST=$2
	else
		DST=$1
	fi

	[ -n "${!1}" ] && export "$DST"="${!1}" || true
}

build_hal()
{
	local append=

	check_config "$1" || return 0

	message "=========================================="
	message "  Building CPU $2: HAL-->${!1}"
	message "=========================================="

	cd "$RK_RTOS_BSP_DIR/common/hal/project/"${!1}"/GCC"

	[ ! -n "$CC" ] || append=$CC
	(
		amp_touch_export FIRMWARE_CPU_BASE
		amp_touch_export DRAM_SIZE
		amp_touch_export SRAM_BASE
		amp_touch_export SRAM_SIZE
		amp_touch_export CUR_CPU

		make clean > /dev/null
		rm -rf $3.elf $3.bin
		make $append -j$(nproc) > ${RK_SDK_DIR}/hal.log 2>&1
	)

	cp TestDemo.elf $3.elf
	mv TestDemo.bin $3.bin
	ln -rsf $3.bin $RK_OUTDIR/$3.bin

	finish_build build_hal $@
}

build_rtthread()
{
	local append=

	check_config "$1" || return 0

	message "=========================================="
	message "  Building CPU $2: RT-Thread-->${!1}"
	message "                  Config-->$4"
	message "=========================================="

	cd "$RK_RTOS_BSP_DIR/${!1}"

	export RTT_ROOT=$RK_RTOS_BSP_DIR/../../

	amp_touch_export FIRMWARE_CPU_BASE RTT_PRMEM_BASE
	amp_touch_export DRAM_SIZE RTT_PRMEM_SIZE
	amp_touch_export SRAM_BASE RTT_SRAM_BASE
	amp_touch_export SRAM_SIZE RTT_SRAM_SIZE
	amp_touch_export SHMEM_BASE RTT_SHMEM_BASE
	amp_touch_export SHMEM_SIZE RTT_SHMEM_SIZE
	amp_touch_export CC RTT_EXEC_PATH

	ROOT_PART_OFFSET=$(rk_partition_start root)
	ROOT_PART_SIZE=$(rk_partition_size root)

	if [ -f "$4" ] ;then
		scons --useconfig="$4"
	else
		warning "Warning: Config $4 not exit!\n"
		warning "Default config(.config) will be used!\n"
	fi

	scons -c > /dev/null
	rm -rf gcc_arm.ld Image/rtt$2.elf Image/rtt$2.bin
	scons -j$(nproc) > ${RK_SDK_DIR}/rtt.log 2>&1

	cp rtthread.elf Image/rtt$2.elf
	mv rtthread.bin Image/rtt$2.bin
	ln -rsf Image/rtt$2.bin $RK_OUTDIR/$3.bin

	if [ -n "$RK_AMP_RTT_ROOT_DATA" ] && [ -n "$ROOT_PART_SIZE" ] ;then

		RTT_ROOT_USERDAT=$RK_RTOS_BSP_DIR/$RK_AMP_RTT_TARGET/$RK_AMP_RTT_ROOT_DATA

		ROOT_SECTOR_SIZE=$(grep -r "CONFIG_RT_DFS_ELM_MAX_SECTOR_SIZE" "$4" | cut -d '=' -f 2)
		if [ -z $ROOT_SECTOR_SIZE ];then
			ROOT_SECTOR_SIZE=4096
		fi

		./mkroot.sh root $RTT_ROOT_USERDAT $RK_CHIP_DIR/$RK_PARAMETER $ROOT_SECTOR_SIZE $RK_FIRMWARE_DIR/root.img
	fi

	finish_build build_rtthread $@
}

clean_hook()
{
	[ "$RK_AMP" ] || return 0

	if [ "$RK_AMP_RTT_TARGET" ]; then
		cd "$RK_RTOS_BSP_DIR/$RK_AMP_RTT_TARGET"
		scons -c >/dev/null || true
	fi

	if [ "$RK_AMP_HAL_TARGET" ]; then
		cd "$RK_RTOS_BSP_DIR/common/hal/project/$RK_AMP_HAL_TARGET/GCC"
		make clean >/dev/null || true
	fi

	rm -rf "$RK_FIRMWARE_DIR/amp.img"
}

build_images()
{
	for item in $1
	do
		ITS_IMAGE=$(amp_get_node "$(cat $ITS_FILE)" $item)

		# update all parameters
		FIRMWARE_CPU_BASE=$(amp_get_value "$ITS_IMAGE" load)
		DRAM_SIZE=$(amp_get_value "$ITS_IMAGE" size)
		SRAM_BASE=$(amp_get_value "$ITS_IMAGE" srambase)
		SRAM_SIZE=$(amp_get_value "$ITS_IMAGE" sramsize)
		CUR_CPU=$(amp_get_value "$ITS_IMAGE" cpu)
		CPU_BIN=$(amp_get_string "$ITS_IMAGE" data)
		if (( $CUR_CPU > 0xff )); then
			CUR_CPU=$((CUR_CPU >> 8))
		fi
		CUR_CPU=$(($CUR_CPU))

		echo Image info: $item
		for p in FIRMWARE_CPU_BASE DRAM_SIZE SRAM_BASE SRAM_SIZE SHMEM_BASE \
			 SHMEM_SIZE LINUX_RPMSG_BASE LINUX_RPMSG_SIZE CUR_CPU
		do
			echo $(env | grep -w $p && true)
		done

		SYS=$(amp_get_string "$ITS_IMAGE" sys)
		CORE=$(amp_get_string "$ITS_IMAGE" core)

		# In RTT: 'CC' means the directory where the GCC tools are located.
		# In HAL: 'CC' means the directory and the prefix of GCC.
		CC=$(amp_get_string "$ITS_IMAGE" cc)
		[ ! -n "$CC" ] || CC="${RK_SDK_DIR}/${CC}"

		SYS="${SYS}${CORE:+_$CORE}"

		case $SYS in
			hal_mcu)
				build_hal RK_AMP_MCU_HAL_TARGET mcu \
					  "$(basename -s .bin $CPU_BIN)"
				;;
			hal|hal_ap)
				build_hal RK_AMP_HAL_TARGET $CUR_CPU \
					  "$(basename -s .bin $CPU_BIN)"
				;;
			rtt_mcu)
				build_rtthread RK_AMP_MCU_RTT_TARGET mcu \
					       "$(basename -s .bin $CPU_BIN)" \
					       "$(amp_get_string "$ITS_IMAGE" rtt_config)"
				;;
			rtt|rtt_ap)
				build_rtthread RK_AMP_RTT_TARGET $CUR_CPU \
					       "$(basename -s .bin $CPU_BIN)" \
					       "$(amp_get_string "$ITS_IMAGE" rtt_config)" \
				;;
			*)
				break;;
		esac
	done
}

BUILD_CMDS="amp"
build_hook()
{
	local i

	check_config RK_AMP || false

	message "=========================================="
	message "          Start building AMP"
	message "=========================================="

	"$RK_SCRIPTS_DIR/check-amp.sh"

	export CROSS_COMPILE=$(get_toolchain AMP "$RK_AMP_ARCH" "" none)
	[ "$CROSS_COMPILE" ] || exit 1

	if [ -f "$RK_CHIP_DIR/$RK_AMP_CFG" ]; then
		set -a
		source $RK_CHIP_DIR/$RK_AMP_CFG
		set +a
	fi

	CORE_NUMBERS=$(grep -wcE "amp[0-9]* {|mcu {" $ITS_FILE)
	echo "CORE_NUMBERS=$CORE_NUMBERS"

	EXT_SHARE=$(amp_get_node "$(cat $ITS_FILE)" share)
	if [ "$EXT_SHARE" ]; then
		SHMEM_BASE=$(amp_get_value "$EXT_SHARE" "shm_base")
		if [ "$SHMEM_BASE" ]; then
			export SHMEM_BASE
			export SHMEM_SIZE=$(amp_get_value "$EXT_SHARE" "shm_size")
			AMP_PRIMARY_CORE=$(amp_get_value "$EXT_SHARE" primary)
			[ ! $AMP_PRIMARY_CORE ] || export AMP_PRIMARY_CORE=$(($AMP_PRIMARY_CORE))
		fi

		LINUX_RPMSG_BASE=$(amp_get_value "$EXT_SHARE" "rpmsg_base")
		if [ "$LINUX_RPMSG_BASE" ]; then
			export LINUX_RPMSG_BASE=$LINUX_RPMSG_BASE
			export LINUX_RPMSG_SIZE=$(amp_get_value "$EXT_SHARE" "rpmsg_size")
		fi
	fi

	ITS_IMAGES=$(grep -wE "amp[0-9]* {|mcu {" $ITS_FILE | grep -oE "amp[0-9]*|mcu")
	build_images "$ITS_IMAGES"

	cd "$RK_OUTDIR"
	ln -rsf $ITS_FILE amp.its
	sed -i '/share {/,/}/d' amp.its
	sed -i '/compile {/,/}/d' amp.its

	$RK_RTOS_BSP_DIR/tools/mkimage -f amp.its -E -p 0xe00 $RK_FIRMWARE_DIR/amp.img

	finish_build amp $@
}

source "${RK_BUILD_HELPER:-$(dirname "$(realpath "$0")")/../build-hooks/build-helper}"

build_hook $@
