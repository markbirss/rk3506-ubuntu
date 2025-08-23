#!/bin/bash -e

RK_SCRIPTS_DIR="$(dirname "$(realpath "$BASH_SOURCE")")"
RK_DEVICE_DIR="$(realpath "$RK_SCRIPTS_DIR/../../")"
RK_SDK_DIR="$(realpath "$RK_DEVICE_DIR/../../")"
RK_CHIPS_DIR="$RK_DEVICE_DIR/.chips"

choose_chip()
{
	CHIP_ARRAY=( $(ls "$RK_CHIPS_DIR") )
	CHIP_ARRAY_LEN=${#CHIP_ARRAY[@]}
	echo "Pick a chip:"
	echo ""

	echo ${CHIP_ARRAY[@]} | xargs -n 1 | sed "=" | sed "N;s/\n/. /"

	local INDEX
	read -p "Which would you like? [1]: " INDEX
	INDEX=$((${INDEX:-1} - 1))
	CHIP="${CHIP_ARRAY[$INDEX]}"
}

CHIP=$1
if [ -z "$CHIP" -o ! -e "$RK_CHIPS_DIR/$CHIP" ]; then
	choose_chip
	[ "$CHIP" ] || exit 1
fi

GPU_DIR="$RK_SDK_DIR/external/libmali"
GPU_CHIP="$(grep $CHIP "$GPU_DIR/gpu-chips.txt" | cut -d':' -f1)"
if [ -z "$GPU_CHIP" ]; then
	echo "There's no Mali GPU for $CHIP"
	exit 0
fi

echo "Releasing libmali $GPU_CHIP for $CHIP"

cd "$GPU_DIR"

ORIG_COMMIT=$(git log --oneline -1 | cut -d' ' -f1)

COMMIT_MSG=$(mktemp)
cat << EOF > $COMMIT_MSG
Release $GPU_CHIP - $(date +%Y-%m-%d)

Based on:
$(git log -1 --format="%h %s")
EOF

git add -f .
git stash &>/dev/null

# Drop other libraries
find "$GPU_DIR" -name "*.so" -print0 | grep -wv "$GPU_CHIP" | xargs rm -f

# Checkout branch
if ! git branch | grep -wq "$GPU_CHIP"; then
	# Create new branch
	git branch -D $GPU_CHIP &>/dev/null || true
	git checkout --orphan $GPU_CHIP &>/dev/null
	git reset &>/dev/null
else
	git checkout $GPU_CHIP &>/dev/null
	git checkout $ORIG_COMMIT . &>/dev/null
fi

if ! git log -1 | grep -wq "$ORIG_COMMIT"; then
	# Commit files
	git add .
	git commit --allow-empty -s -F $COMMIT_MSG &>/dev/null
	rm -f $COMMIT_MSG
	git checkout -B $GPU_CHIP &>/dev/null
fi

# Recover
git checkout $ORIG_COMMIT &>/dev/null
cd "$RK_SDK_DIR"
