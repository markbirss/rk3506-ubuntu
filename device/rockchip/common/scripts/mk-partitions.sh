#!/bin/bash -e

print_usage()
{
	normalized_usage | grep -v "^mod-parts"
	usage_oneline "done" "done modifying and quit"
}

modify_partitions()
{
	message "=========================================="
	message "          Start modifying partitions"
	message "=========================================="

	rk_partition_print

	echo
	echo "Usage:"
	print_usage

	while true; do
		echo
		read -p "Commands (? for help): " SUB_CMD ARGS || break
		case "${SUB_CMD:-print-parts}" in
			done | d) break ;;
			print-parts | p | list-parts | l)
				rk_partition_print
				continue
				;;
			edit-parts | e) FUNC=rk_partition_edit ;;
			new-parts | n) FUNC=rk_partition_create ;;
			insert-part | i) FUNC=rk_partition_insert ;;
			del-part | d) FUNC=rk_partition_del ;;
			move-part | m) FUNC=rk_partition_move ;;
			rename-part | rn | r) FUNC=rk_partition_rename ;;
			resize-part | rs) FUNC=rk_partition_resize ;;
			help | h | -h | --help | \?) FUNC=false ;;
			*)
				error "Unknown command: $SUB_CMD"
				FUNC=false
				;;
		esac

		if $FUNC $ARGS; then
			rk_partition_print
		else
			print_usage
		fi
	done
}

# Hooks

usage_hook()
{
	usage_oneline "print-parts" "print partitions"
	usage_oneline "list-parts" "alias of print-parts"
	usage_oneline "mod-parts" "interactive partition table modify"
	usage_oneline "edit-parts" "edit raw partitions"
	usage_oneline "new-parts:<offset>:<name>:<size>..." "re-create partitions"
	usage_oneline "insert-part:<idx>:<name>[:<size>]" "insert partition"
	usage_oneline "del-part:(<idx>|<name>)" "delete partition"
	usage_oneline "move-part:(<idx>|<name>):<idx>" "move partition"
	usage_oneline "rename-part:(<idx>|<name>):<name>" "rename partition"
	usage_oneline "resize-part:(<idx>|<name>):<size>" "resize partition"
}

PRE_BUILD_CMDS="print-parts list-parts mod-parts edit-parts new-parts insert-part del-part move-part rename-part resize-part"
pre_build_hook()
{
	check_config RK_PARAMETER || false

	CMD=$1
	shift

	case "$CMD" in
		print-parts | list-parts) rk_partition_print $@ ;;
		mod-parts) modify_partitions $@ ;;
		edit-parts) rk_partition_edit $@ ;;
		new-parts) rk_partition_create $@ ;;
		insert-part) rk_partition_insert $@ ;;
		del-part) rk_partition_del $@ ;;
		move-part) rk_partition_move $@ ;;
		rename-part) rk_partition_rename $@ ;;
		resize-part) rk_partition_resize $@ ;;
		*)
			normalized_usage
			exit 1
			;;
	esac

	finish_build $CMD $@
}

source "${RK_BUILD_HELPER:-$(dirname "$(realpath "$0")")/../build-hooks/build-helper}"

pre_build_hook $@
