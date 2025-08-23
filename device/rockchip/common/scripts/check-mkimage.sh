#!/bin/bash -e

if ! echo "$RK_EXTRA_PARTITION_STR" | grep -q ":ext[234]:"; then
	exit 0
fi

# mk-image.sh requires -d option to pack e2fs for non-root user
if ! mke2fs -h 2>&1 | grep -wq "\-d"; then
	echo -e "\e[35m"
	echo "Your mke2fs is too old: $(mke2fs -V 2>&1 | head -n 1)"
	echo "Please update it:"
	"$RK_SCRIPTS_DIR/install-e2fsprogs.sh"
	echo -e "\e[0m"
	exit 1
fi

# The rootfs's e2fsprogs might not support new features like
# metadata_csum_seed and orphan_file
if grep -wq metadata_csum_seed /etc/mke2fs.conf; then
	echo -e "\e[35m"
	echo "Your mke2fs is too new: $(mke2fs -V 2>&1 | head -n 1)"
	echo "Please downgrade it:"
	"$RK_SCRIPTS_DIR/install-e2fsprogs.sh"
	echo -e "\e[0m"
	exit 1
fi
