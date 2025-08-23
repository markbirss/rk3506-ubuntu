#!/bin/bash -e

###################################################
RK_SCRIPTS_DIR="${RK_SCRIPTS_DIR:-$(dirname "$(realpath "$0")")}"
RK_SDK_DIR="${RK_SDK_DIR:-$RK_SCRIPTS_DIR/../../../..}"
UBOOT=$RK_SDK_DIR/u-boot
KERNEL=$RK_SDK_DIR/kernel
BUILDROOT=$RK_SDK_DIR/buildroot
RK_SIGN_TOOL=$RK_SDK_DIR/rkbin/tools/rk_sign_tool
###################################################

ROOTFS_UPDATE_ENGINEBIN_CONFIGS=" \
	BR2_PACKAGE_RECOVERY \
	BR2_PACKAGE_RECOVERY_UPDATEENGINEBIN"

ROOTFS_AB_FIXED_CONFIGS=" \
	$ROOTFS_UPDATE_ENGINEBIN_CONFIGS \
	BR2_PACKAGE_RECOVERY_BOOTCONTROL"

UBOOT_FIT_FIXED_CONFIGS=" \
	CONFIG_FIT_SIGNATURE \
	CONFIG_SPL_FIT_SIGNATURE"

UBOOT_AVB_FIXED_CONFIGS=" \
	CONFIG_ANDROID_AVB \
	CONFIG_AVB_LIBAVB \
	CONFIG_AVB_LIBAVB_AB \
	CONFIG_AVB_LIBAVB_ATX \
	CONFIG_AVB_LIBAVB_USER \
	CONFIG_RK_AVB_LIBAVB_USER \
	CONFIG_OPTEE_CLIENT \
	CONFIG_AVB_VBMETA_PUBLIC_KEY_VALIDATE \
	CONFIG_RK_AVB_LIBAVB_ENABLE_ATH_UNLOCK \
	CONFIG_OPTEE_V."

# TODO:  CONFIG_ROCKCHIP_PRELOADER_PUB_KEY

RAMBOOT_FIXED_CONFIG=" \
	BR2_PACKAGE_TEE_USER_APP \
	BR2_PACKAGE_LUKSMETA"

rk_security_check_keys()
{
	if [ ! -d "$UBOOT/keys" ]; then
		echo "ERROR: No root keys(u-boot/keys) found in u-boot"
		echo "       Create it by ./build.sh security-createkeys or move your key to it"
		exit -1
	fi

	if echo "$1" | grep system ; then
		if [ ! -f $UBOOT/keys/root_passwd ]; then
			echo "ERROR: No root passwd(u-boot/keys/root_passwd) found in u-boot"
			echo "       echo your root key for sudo to u-boot/keys/root_passwd"
			echo "       some operations need supper user permission when create encrypt image"
			exit -1
		fi

		if [ "$1" = "system-encryption" ] && \
			[ ! -f $UBOOT/keys/system_enc_key ]; then
			echo "ERROR: No enc key(u-boot/keys/system_enc_key) found in u-boot"
			echo "       Create it by ./build.sh security-createkeys or move your key to it"
			exit -1
		fi
	fi
}

BOOT_FIXED_CONFIGS=" \
	CONFIG_BLK_DEV_DM \
	CONFIG_DM_CRYPT \
	CONFIG_DM_VERITY"

BOOT_FIXED_UNDER_6_1_CONFIG="
	CONFIG_BLK_DEV_CRYPTOLOOP"

BOOT_OPTEE_FIXED_CONFIGS=" \
	CONFIG_TEE \
	CONFIG_OPTEE"

config_check()
{
	# 1. config 2. match item
	echo debug-$1
	for i in $2
	do
		echo "look for $i"
		result=$(cat $1 | grep "${i}=y" -w || echo "No found")
		if [ "$result" = "No found" ]; then
			echo -e "\e[41;1;37mSecurity: No found config ${i} in $1 \e[0m"
			echo "make sure your config include this list"
			echo "---------------------------------------"
			echo "$2" | xargs -n1
			echo "---------------------------------------"
			exit -1;
		fi
	done
	return 0
}

rk_security_match_overlay()
{
	result=$(cat "$2" | grep "$3" || echo "No found")
	if [ "$result" = "No found" ]; then
		echo -e "\e[41;1;37mSecurity: No found BR2_ROOTFS_OVERLAY+=\"board/rockchip/common/$3/\" in $1 config\e[0m"
		exit -1
	fi
}

rk_security_check_system()
{
	case $1 in
		system-encryption|system-verity) rk_security_match_overlay system $2 security-system-overlay;;
		base) return 0;;
		*) exit -1;;
	esac
}

rk_security_check_kernel_config()
{
	[ ! -z "$RK_SECURITY" ] || return 0

	if [ $(echo "$RK_KERNEL_VERSION_RAW < 6.1" | bc) -eq 1 ]; then
		BOOT_FIXED_CONFIGS="$BOOT_FIXED_CONFIGS $BOOT_FIXED_UNDER_6_1_CONFIG"
	fi

	case $1 in
		system-encryption) BOOT_FIXED_CONFIGS="$BOOT_FIXED_CONFIGS $BOOT_OPTEE_FIXED_CONFIGS" ;& # fallthrough
		system-verity) config_check $2 "$BOOT_FIXED_CONFIGS" ;;
		base) return 0;;
		*) exit -1;;
	esac
}

rk_security_check_kernel_dts()
{
	test "$1" = "system-encryption" || return 0

	if [ "${2##*.}" = "dtb" ]; then
		dtsfile=$(mktemp)
		dtc -q -I dtb -O dts -o $dtsfile $2
	else
		dtsfile=$2
	fi

	tmp_file=$(mktemp)
	if ! grep -Pzo "\toptee \s*{(\n|\w|-|;|=|<|>|\"|_|\s|,)*};" $dtsfile 1>$tmp_file 2>/dev/null; then
		echo -e "\e[41;1;37mNo found optee node in dts\e[0m"
		echo "Please add: "
		echo "        optee: optee {"
		echo "                compatible = \"linaro,optee-tz\";"
		echo "                method = \"smc\";"
		echo "                status = \"okay\";"
		echo "        };"
		echo "To kernel dts"

		rm -f $tmp_file
		test "$2" = "$dtsfile" || rm $dtsfile
		exit -1
	fi

	status=$(cat $tmp_file | grep -a status || true)
	if [ "$(echo $status | grep disabled)" ]; then
		rm -f $tmp_file
		test "$2" = "$dtsfile" || rm $dtsfile
		echo -e "\e[41;1;37mOptee Found, but disabled!!!\e[0m"
		exit -1
	fi

	rm -f $tmp_file
	test "$2" = "$dtsfile" || rm $dtsfile
}

rk_security_check_kernel()
{
	append=$1
	shift
	case $append in
		config|dts) rk_security_check_kernel_$append $@;;
		*) exit -1;;
	esac
}

rk_security_check_ramboot()
{
	if [ "$1" != "system-encryption" ]; then
		return 0
	fi
	shift

	if [ ! -f "$1" ]; then
		echo -e "\e[41;1;37m$1 is not found\e[0m"
		exit -1
	fi

	echo "check ramdisk config"
	config_check $1 "$(echo $ROOTFS_UPDATE_ENGINEBIN_CONFIGS $RAMBOOT_FIXED_CONFIG)"
	rk_security_match_overlay ramboot $1 security-ramdisk-overlay
}

rk_security_check_uboot()
{
	METHOD=$1
	shift

	if [ "$METHOD" = "fit" ]; then
		config_check $1 "$UBOOT_FIT_FIXED_CONFIGS"
	else
		config_check $1 "$UBOOT_AVB_FIXED_CONFIGS"
	fi
}

rk_security_check_main()
{
	CHECK_LIST="keys kernel system uboot ramboot"

	for item in $CHECK_LIST
	do
		if [ "$item" = "$1" ]; then
			append=$1
			shift
			"rk_security_check_$append" $@
		fi
	done
}

# -----------------------------------
# For SDK
# -----------------------------------

rk_security_check_sdk()
{
	[ ! -z "$RK_SECURITY" ] || return 0

	case $1 in
		keys) rk_security_check_main keys $RK_SECURITY_CHECK_METHOD ;;
		kernel)
			case $2 in
				config) rk_security_check_main $@ $RK_SECURITY_CHECK_METHOD $RK_SDK_DIR/kernel/.config ;;
				dts) rk_security_check_main $@ $RK_SECURITY_CHECK_METHOD $RK_KERNEL_DTB ;;
			esac
			;;
		system) rk_security_check_main system $RK_SECURITY_CHECK_METHOD $RK_SDK_DIR/buildroot/output/$RK_BUILDROOT_CFG/.config ;;
		ramboot) rk_security_check_main ramboot $RK_SECURITY_CHECK_METHOD $RK_SDK_DIR/buildroot/output/$RK_SECURITY_INITRD_CFG/.config ;;
		uboot) rk_security_check_main uboot $RK_SECUREBOOT_METHOD $RK_SDK_DIR/u-boot/.config ;;
	esac
}

if [ "$RK_SESSION" ]; then
	rk_security_check_sdk $@
else
	rk_security_check_main $@
fi
