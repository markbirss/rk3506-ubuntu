#!/bin/bash -e

source "${RK_POST_HELPER:-$(dirname "$(realpath "$0")")/../post-hooks/post-helper}"

if [ "$RK_SUDO_ROOT" ]; then
	message "Fixing up owner for $RK_OUTDIR..."
	find "$RK_OUTDIR" -user root -exec \
		chown -ch $RK_OWNER_UID:$RK_OWNER_UID {} \;
fi

# buildroot would fixup owner in its fakeroot script
case "$POST_OS" in
	buildroot | ramboot | recovery) exit 0 ;;
esac

message "Fixing up owner for $TARGET_DIR..."

if [ "$RK_OWNER_UID" -ne 0 ]; then
	message "Fixing up uid=$RK_OWNER($RK_OWNER_UID) to 0(root)..."
	find . -user $RK_OWNER_UID -exec chown -ch 0:0 {} \;
fi

if [ -d home ]; then
	for u in $(ls home/); do
		ID=$(grep "^$u:" etc/passwd | cut -d':' -f3 || true)
		[ "$ID" ] || continue
		message "Fixing up /home/$u for uid=$ID($u)..."
		chown -ch -R $ID:$ID home/$u
	done
fi
