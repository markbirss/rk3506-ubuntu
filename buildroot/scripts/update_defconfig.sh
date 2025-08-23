#!/bin/bash
#
# Usage:
# update_defconfig.sh <defconfig> [output dir]

set -e

# Usage: savedefconfig <output defconfig> [input defconfig]
savedefconfig()
{
	TARGET="$1"
	TMP_CONFIG="${CONFIG}.tmp"

	if [ "$2" ]; then
		"$SCRIPT_DIR/parse_defconfig.sh" "$2" "$TMP_CONFIG" >/dev/null
	else
		cp -f "$CONFIG" "$TMP_CONFIG"
	fi

	sed -i "/BR2_DEFCONFIG/d" "$TMP_CONFIG"
	echo "BR2_DEFCONFIG=\"$TARGET\"" >> "$TMP_CONFIG"

	# Based on buildroot/Makefile's savedefconfig
	BR2_DEFCONFIG="$TARGET" BR2_CONFIG="$TMP_CONFIG" \
		KCONFIG_AUTOCONFIG=$CONFIG_DIR/auto.conf \
		KCONFIG_AUTOHEADER=$CONFIG_DIR/autoconf.h \
		KCONFIG_TRISTATE=$CONFIG_DIR/tristate.config \
		BASE_DIR="$OUTPUT_DIR" SKIP_LEGACY= \
		HOST_GCC_VERSION="$HOST_GCC_VERSION" \
		CUSTOM_KERNEL_VERSION="$CUSTOM_KERNEL_VERSION" \
		HOSTARCH="$HOSTARCH" BR2_VERSION_FULL="$BR2_VERSION_FULL" \
		$CONFIG_DIR/conf --savedefconfig="$TARGET" \
		Config.in >/dev/null
	sed -i '/^BR2_DEFCONFIG=/d' "$TARGET"
}

BOARD="$(basename "${1%_defconfig}")"
[ -n "$BOARD" ] || exit 1

SCRIPT_DIR="$(realpath "$(dirname "$0")")"
BUILDROOT_DIR="$(dirname "$SCRIPT_DIR")"
OUTPUT_DIR="${2:-$BUILDROOT_DIR/output/$BOARD}"
cd "$BUILDROOT_DIR"

DEFCONFIG="configs/${BOARD}_defconfig"
[ -r "$DEFCONFIG" ] || exit 1

echo "Updating defconfig: $DEFCONFIG"

CONFIG="$OUTPUT_DIR/.config"
ORIG_DEFCONFIG="$OUTPUT_DIR/.defconfig"
BASE_DEFCONFIG="$OUTPUT_DIR/.base_defconfig"
NEW_DEFCONFIG="$OUTPUT_DIR/.new_defconfig"
FRAGMENT="$OUTPUT_DIR/.fragment"

CONFIG_DIR="$OUTPUT_DIR/build/buildroot-config"
HOST_GCC_VERSION="$(gcc --version | head -n 1 | xargs -n 1 | \
	tail -n 1 | cut -d'.' -f1)"
CUSTOM_KERNEL_VERSION="$(head -n 2 "$BUILDROOT_DIR/../kernel/Makefile" | \
	cut -d' ' -f3 | paste -sd'.')"
HOSTARCH="$(uname -m)"
BR2_VERSION_FULL="$(grep "export BR2_VERSION := " \
	"$BUILDROOT_DIR/Makefile" | xargs -n 1 | tail -n 1)"

# Generate original defconfig
if [ ! -r "$CONFIG" ]; then
	mkdir -p "$OUTPUT_DIR"
	savedefconfig "$ORIG_DEFCONFIG" "$DEFCONFIG"
else
	savedefconfig "$ORIG_DEFCONFIG"
fi
echo "Original defconfig saved to $ORIG_DEFCONFIG"

# Generate base defconfig
grep "^#include " $DEFCONFIG > "$FRAGMENT" || true
savedefconfig "$BASE_DEFCONFIG" "$FRAGMENT"
echo "Base defconfig saved to $BASE_DEFCONFIG"

# Update defconfig fragment
SED_CONFIG_EXP1="s/^\(<\|>\) \([a-zA-Z0-9_]*\)=.*/\2/p"
SED_CONFIG_EXP2="s/^\(<\|>\) # \([a-zA-Z0-9_]*\) is not set$/\2/p"
SED_STRING_EXP="s/^.*=\"\(.*\)\"/\1/p"

CFG_LIST=$(diff "$ORIG_DEFCONFIG" "$BASE_DEFCONFIG" | \
	sed -n -e "$SED_CONFIG_EXP1" -e "$SED_CONFIG_EXP2" | sort | uniq)

for CFG in $CFG_LIST ; do
	BASE_VAL=$(grep -w $CFG "$BASE_DEFCONFIG" || true)
	ORIG_NEW_VAL=$(grep -w $CFG "$ORIG_DEFCONFIG" || true)

	if [ -z "$ORIG_NEW_VAL" ]; then
		# Reset to default
		NEW_VAL="# $CFG is reset to default"
	else
		# Replace
		NEW_VAL="$ORIG_NEW_VAL"

		if grep -q "^$CFG+=" "$DEFCONFIG"; then
			BASE_STR=$(echo "$BASE_VAL" | sed -n "$SED_STRING_EXP")
			NEW_STR=$(echo "$NEW_VAL" | sed -n "$SED_STRING_EXP")
			if [ -n "$BASE_STR" -a -n "$NEW_VAL" ]; then
				# Try to extract additional strings
				for s in $BASE_STR; do
					NEW_STR=$(echo "$NEW_STR" | \
						xargs -n 1 | grep -v "^$s$" | \
						xargs)
				done

				NEW_VAL="${NEW_STR:+$CFG+=\"$NEW_STR\"}"
			fi
		fi
	fi

	echo "$NEW_VAL" >> $FRAGMENT

	echo "Value of $CFG applied to fragment:"
	echo -e "Base value:\t$BASE_VAL"
	if [ -n "$ORIG_NEW_VAL" -a "$NEW_VAL" != "$ORIG_NEW_VAL" ]; then
		echo -e "New value:\t$ORIG_NEW_VAL"
	fi
	echo -e "Final value:\t$NEW_VAL"
	echo
done

# Update defconfig fragment for dependency changes
savedefconfig "$NEW_DEFCONFIG" "$FRAGMENT"

CFG_LIST=$(diff "$ORIG_DEFCONFIG" "$NEW_DEFCONFIG" | \
	sed -n -e "$SED_CONFIG_EXP1" -e "$SED_CONFIG_EXP2" | sort | uniq)
for CFG in $CFG_LIST ; do
	grep -wq $CFG "$FRAGMENT" || \
		echo "# $CFG is reset to default" >> $FRAGMENT
done

cat $FRAGMENT > $DEFCONFIG

# Verify defconfig fragment
savedefconfig "$NEW_DEFCONFIG" "$DEFCONFIG"
if diff "$ORIG_DEFCONFIG" "$NEW_DEFCONFIG" | grep ""; then
	echo "Configs unmatched, might be something wrong."
	exit 1
fi

# Strip unneeded config resets
TEMP_FILE=$(mktemp)
for CFG in $(grep " is reset to default$" "$DEFCONFIG" | cut -d' ' -f2); do
	grep -wv "$CFG" "$DEFCONFIG" > $TEMP_FILE
	savedefconfig "$NEW_DEFCONFIG" $TEMP_FILE
	if ! diff "$ORIG_DEFCONFIG" "$NEW_DEFCONFIG" | grep -q ""; then
		cat $TEMP_FILE > "$DEFCONFIG"
	fi
done

echo "Done updating $DEFCONFIG."
