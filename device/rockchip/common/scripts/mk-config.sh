#!/bin/bash -e

switch_defconfig()
{
	DEFCONFIG="$1"

	[ -f "$DEFCONFIG" ] || DEFCONFIG="$RK_CHIP_DIR/$DEFCONFIG"

	if [ ! -f "$DEFCONFIG" ]; then
		error "No such defconfig: $1"
		exit 1
	fi

	notice "Switching to defconfig: $DEFCONFIG"
	rm -f "$RK_DEFCONFIG_LINK"
	ln -rsf "$DEFCONFIG" "$RK_DEFCONFIG_LINK"

	DEFCONFIG="$(realpath "$DEFCONFIG")"
	rm -rf "$RK_CHIP_DIR"
	ln -rsf "$(dirname "$DEFCONFIG")" "$RK_CHIP_DIR"

	make $(basename "$DEFCONFIG")
}

rockchip_defconfigs()
{
	cd "$RK_CHIP_DIR"
	ls rockchip_defconfig 2>/dev/null || true
	ls *_defconfig | grep -v rockchip_defconfig || true
}

choose_defconfig()
{
	DEFCONFIG_ARRAY=( $(rockchip_defconfigs | grep "$1" || true) )

	DEFCONFIG_ARRAY_LEN=${#DEFCONFIG_ARRAY[@]}

	case $DEFCONFIG_ARRAY_LEN in
		0)
			error "No available defconfigs${1:+" for: $1"}"
			return 1
			;;
		1)	DEFCONFIG=${DEFCONFIG_ARRAY[0]} ;;
		*)
			if [ "$1" = ${DEFCONFIG_ARRAY[0]} ]; then
				# Prefer exact-match
				DEFCONFIG="$1"
			else
				message "Pick a defconfig:\n"

				echo ${DEFCONFIG_ARRAY[@]} | xargs -n 1 | \
					sed "=" | sed "N;s/\n/. /"

				local INDEX
				read -p "Which would you like? [1]: " INDEX
				INDEX=$((${INDEX:-1} - 1))
				DEFCONFIG="${DEFCONFIG_ARRAY[$INDEX]}"
			fi
			;;
	esac

	switch_defconfig $DEFCONFIG
}

choose_chip()
{
	CHIP_ARRAY=( $(ls "$RK_CHIPS_DIR" | grep "$1" || true) )
	CHIP_ARRAY_LEN=${#CHIP_ARRAY[@]}

	case $CHIP_ARRAY_LEN in
		0)
			error "No available chips${1:+" for: $1"}"
			return 1
			;;
		1)	CHIP=${CHIP_ARRAY[0]} ;;
		*)
			if [ "$1" = ${CHIP_ARRAY[0]} ]; then
				# Prefer exact-match
				CHIP="$1"
			else
				message "Pick a chip:\n"

				echo ${CHIP_ARRAY[@]} | xargs -n 1 | sed "=" | \
					sed "N;s/\n/. /"

				local INDEX
				read -p "Which would you like? [1]: " INDEX
				INDEX=$((${INDEX:-1} - 1))
				CHIP="${CHIP_ARRAY[$INDEX]}"
			fi
			;;
	esac

	notice "Switching to chip: $CHIP"
	rm -rf "$RK_CHIP_DIR"
	ln -rsf "$RK_CHIPS_DIR/$CHIP" "$RK_CHIP_DIR"

	choose_defconfig $2
}

prepare_config()
{
	[ -e "$RK_CHIP_DIR" ] || choose_chip

	cd "$RK_DEVICE_DIR"
	rm -f $(ls "$RK_CHIPS_DIR")
	ln -rsf "$(readlink "$RK_CHIP_DIR")" .
	cd "$RK_SDK_DIR"

	if [ ! -r "$RK_DEFCONFIG_LINK" ]; then
		warning "WARN: $RK_DEFCONFIG_LINK not exists"
		choose_defconfig
		return 0
	fi

	DEFCONFIG=$(basename "$(realpath "$RK_DEFCONFIG_LINK")")
	if [ ! "$RK_DEFCONFIG_LINK" -ef "$RK_CHIP_DIR/$DEFCONFIG" ]; then
		warning "WARN: $RK_DEFCONFIG_LINK is invalid"
		choose_defconfig
		return 0
	fi

	if [ "$RK_CONFIG" -ot "$RK_DEFCONFIG_LINK" ]; then
		warning "WARN: $RK_CONFIG is out-dated"
		make $DEFCONFIG
		return 0
	fi

	CONFIG_DIR="$(dirname "$RK_CONFIG_IN")"
	if [ "$(find "$CONFIG_DIR" -cnewer "$RK_CONFIG")" ]; then
		warning "WARN: $CONFIG_DIR is updated"
		make $DEFCONFIG
		return 0
	fi

	CFG="RK_DEFCONFIG=\"$DEFCONFIG\""
	if ! grep -wq "$CFG" "$RK_CONFIG"; then
		warning "WARN: $RK_CONFIG is invalid"
		make $DEFCONFIG
		return 0
	fi

	if [ "$RK_CONFIG" -nt "${RK_CONFIG}.old" ]; then
		make olddefconfig >/dev/null
		touch "${RK_CONFIG}.old"
	fi
}

# Hooks

usage_hook()
{
	usage_oneline "chip[:<chip>[:<config>]]" "choose chip"
	usage_oneline "defconfig[:<config>]" "choose defconfig"
	usage_oneline " *_defconfig" "switch to specified defconfig"
	echo "    available defconfigs:"
	ls "$RK_CHIP_DIR/" | grep "defconfig$" | sed "s/^/    /"
	usage_oneline " olddefconfig" "resolve any unresolved symbols in .config"
	usage_oneline " savedefconfig" "save current config to defconfig"
	usage_oneline " menuconfig" "interactive curses-based configurator"
	usage_oneline "config" "modify SDK defconfig"
}

clean_hook()
{
	rm -rf "$RK_OUTDIR"/*config* "$RK_OUTDIR/kconf"
}

INIT_CMDS="chip defconfig lunch [^:]*_defconfig olddefconfig savedefconfig menuconfig config default"
init_hook()
{
	case "${1:-default}" in
		chip) shift; choose_chip $@ ;;
		lunch|defconfig) shift; choose_defconfig $@ ;;
		*_defconfig) switch_defconfig "$1" ;;
		olddefconfig | savedefconfig | menuconfig)
			prepare_config
			make $1
			;;
		config)
			prepare_config
			make menuconfig
			make savedefconfig
			;;
		default) prepare_config ;; # End of init
		*) usage ;;
	esac
}

source "${RK_BUILD_HELPER:-$(dirname "$(realpath "$0")")/../build-hooks/build-helper}"

init_hook $@
