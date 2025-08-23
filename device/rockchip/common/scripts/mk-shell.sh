#!/bin/bash -e

# Hooks

usage_hook()
{
	usage_oneline "shell" "setup a shell for developing"
	usage_oneline "buildroot-shell" "setup a shell for buildroot developing"
	usage_oneline "bshell" "alias of buildroot-shell"
}

PRE_BUILD_CMDS="shell buildroot-shell bshell"
pre_build_hook()
{
	warning "Doing this is dangerous and for developing only."
	# No error handling in develop shell.
	set +e; trap ERR

	case "${1:-shell}" in
		buildroot-shell | bshell)
			BUILDROOT_DIR="$RK_SDK_DIR/buildroot"
			BUILDROOT_CFG="${2:-$RK_BUILDROOT_CFG}"
			/bin/bash -c "cd $BUILDROOT_DIR; \
				source envsetup.sh ${BUILDROOT_CFG}_defconfig; \
				PS1='\u@\h:\w ($BUILDROOT_CFG)\$ ' \
				/bin/bash -norc"
			;;
		*) PS1="\u@\h:\w (rksdk)\$ " /bin/bash --norc ;;
	esac

	warning "Exit from $BASH_SOURCE ${@:-shell}."
}

source "${RK_BUILD_HELPER:-$(dirname "$(realpath "$0")")/../build-hooks/build-helper}"

pre_build_hook $@
