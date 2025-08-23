#!/bin/bash -e

RK_SCRIPTS_DIR="$(dirname "$(realpath "$BASH_SOURCE")")"
RK_DEVICE_DIR="$(realpath "$RK_SCRIPTS_DIR/../../")"
RK_CHIPS_DIR="$RK_DEVICE_DIR/.chips"
RK_CHIP_DIR="$RK_DEVICE_DIR/.chip"

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

echo "Releasing chip: $CHIP"

cd "$RK_DEVICE_DIR"

ORIG_COMMIT=$(git log --oneline -1 | cut -d' ' -f1)

COMMIT_MSG=$(mktemp)
cat << EOF > $COMMIT_MSG
Release $CHIP - $(date +%Y-%m-%d)

Based on:
$(git log -1 --format="%h %s")
EOF

git add -f .
git stash &>/dev/null

# Drop other chips
rm -f "$RK_CHIP_DIR"
ln -rsf "$RK_CHIPS_DIR/$CHIP" "$RK_CHIP_DIR"
ln -rsf "$RK_CHIPS_DIR/$CHIP" .

# Checkout branch
if ! git branch | grep -wq "$CHIP"; then
	# Create new branch
	git branch -D $CHIP &>/dev/null || true
	git checkout --orphan $CHIP &>/dev/null
	git reset &>/dev/null
else
	git checkout $CHIP &>/dev/null
	git checkout $ORIG_COMMIT . &>/dev/null
fi

# Commit files
git add -f .gitignore common "$RK_CHIPS_DIR/$CHIP" "$RK_CHIP_DIR" "$CHIP"
git commit --allow-empty -s -F $COMMIT_MSG &>/dev/null
rm -f $COMMIT_MSG
git checkout -B $CHIP &>/dev/null

# Recover
git add -f .
git checkout $ORIG_COMMIT &>/dev/null
