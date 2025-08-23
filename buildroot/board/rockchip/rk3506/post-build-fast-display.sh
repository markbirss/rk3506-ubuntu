#!/bin/bash -e

TARGET=$1

SH_LV_DEMO=$(ls $TARGET/etc/init.d/S* | grep lv_demo || true)
SH_ASYNC_COMMIT=$(ls $TARGET/etc/init.d/S* | grep sync-commit || true)

mkdir -p $TARGET/etc/init.d/pre_init

for i in "${SH_LV_DEMO}" "${SH_ASYNC_COMMIT}" ; do
	if [ -n "$i" ]; then
		mv $i $TARGET/etc/init.d/pre_init/.
	fi
done

# async-commit need first
mv $TARGET/etc/init.d/pre_init/S05async-commit.sh $TARGET/etc/init.d/pre_init/S00async-commit.sh
