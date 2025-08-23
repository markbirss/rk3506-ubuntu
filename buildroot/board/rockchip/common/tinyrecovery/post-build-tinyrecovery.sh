#!/bin/bash -e

TARGET=$1

OTA_SCRIPT="$TARGET/../../../../output/firmware/RK_OTA_update.sh"
PROJECT_OUTPUT_IMAGE=$(realpath $OTA_SCRIPT)
OTA_SCRIPT_PATH=$(realpath $OTA_SCRIPT)
ERASE_MISC_SCRIPT="$TARGET/usr/bin/RK_OTA_erase_misc.sh"
PROJECT_FILE_RECOVERY_LUNCH_SCRIPT=$TARGET/etc/init.d/S99lunch_recovery
PROJECT_FILE_RECOVERY_SCRIPT=$TARGET/etc/init.d/S15linkmount_recovery
PARAMETER_FILE=$CHIP_DIR/$RK_PARAMETER
RKLUNCH_FILE=$TARGET/../../../board/rockchip/common/tinyrecovery/RkLunch-recovery.sh

mkdir -p $(dirname $PROJECT_FILE_RECOVERY_LUNCH_SCRIPT)
cat > $PROJECT_FILE_RECOVERY_LUNCH_SCRIPT <<EOF
#!/bin/sh
case \$1 in
	start)
		sh /usr/bin/RkLunch.sh
		;;
	stop)
		sh /usr/bin/RkLunch-stop.sh
		;;
	*)
		exit 1
		;;
esac
EOF
chmod a+x $PROJECT_FILE_RECOVERY_LUNCH_SCRIPT

mkdir -p $(dirname $ERASE_MISC_SCRIPT)
echo "#!/bin/sh" > $ERASE_MISC_SCRIPT
echo "set -e" >> $ERASE_MISC_SCRIPT
echo "COMMON_DIR=\`dirname \$(realpath \$0)\`" >> $ERASE_MISC_SCRIPT
echo "TOP_DIR=\$(realpath \$COMMON_DIR/../..)" >> $ERASE_MISC_SCRIPT
echo "cd \$TOP_DIR" >> $ERASE_MISC_SCRIPT
echo "echo \"Erase misc partition\"" >> $ERASE_MISC_SCRIPT

cat >> $ERASE_MISC_SCRIPT <<EOF
cmdline=\$(cat /proc/cmdline)
IFS=' ' read -ra items <<< "\$cmdline"
for item in "\${items[@]}"; do
	if [[ \$item =~ nand ]]; then
		bootmedium=spi_nand
	elif [[ \$item =~ emmc ]]; then
		bootmedium=emmc
	elif [[ \$item =~ nor ]]; then
		bootmedium=spi_nor
	fi
done

case bootmedium in
	emmc|spi_nor)
		dd if=/dev/zero of=/dev/block/by-name/misc bs=32 count=1 seek=512
		if [ \$? -ne 0 ];then
			echo "Error: Erase misc partition failed."
			exit 2
		fi
		;;
	spi_nand|slc_nand)
		flash_eraseall /dev/block/by-name/misc
		if [ \$? -ne 0 ];then
			echo "Error: Erase misc partition failed."
			exit 2
		fi
		;;
	*)
		echo "Not support storage medium type: \$bootmedium"
		exit 2
		;;
esac
EOF
chmod a+x $ERASE_MISC_SCRIPT

mkdir -p $(dirname $OTA_SCRIPT)
echo "#!/bin/sh" > $OTA_SCRIPT
echo "set -e" >> $OTA_SCRIPT
echo "COMMON_DIR=\`dirname \$(realpath \$0)\`" >> $OTA_SCRIPT
echo "TOP_DIR=\$(realpath \$COMMON_DIR/../..)" >> $OTA_SCRIPT
echo "cd \$TOP_DIR" >> $OTA_SCRIPT
echo "echo \"Start to write partitions\"" >> $OTA_SCRIPT

cat >> $OTA_SCRIPT <<EOF
cmdline=\$(cat /proc/cmdline)
IFS=' ' read -ra items <<< "\$cmdline"
for item in "\${items[@]}"; do
	if [[ \$item =~ nand ]]; then
		bootmedium=spi_nand
	elif [[ \$item =~ emmc ]]; then
		bootmedium=emmc
	elif [[ \$item =~ nor ]]; then
		bootmedium=spi_nor
	fi
done

case \$bootmedium in
	emmc|spi_nor)
		for image in \$(ls /dev/block/by-name)
		do
			if [ -f \$COMMON_DIR/\${image}.img ];then
				echo "Writing \$image..."
				dd if=\$COMMON_DIR/\${image}.img of=/dev/block/by-name/\$image
				if [ \$? -ne 0 ];then
					echo "Error: \$image write failed."
					exit 1
				fi
			fi
		done
		echo "Erase misc partition"
		dd if=/dev/zero of=/dev/block/by-name/misc bs=32 count=1 seek=512
		if [ \$? -ne 0 ];then
			echo "Error: Erase misc partition failed."
			exit 2
		fi
		;;
	spi_nand|slc_nand)
		for image in \$(ls /dev/block/by-name)
		do
			if [ -f \$COMMON_DIR/\${image}.img ];then
				echo "Writing \$image..."
				mtd_path=\$(realpath /dev/block/by-name/\${image})
				flash_eraseall \$mtd_path
				nandwrite -p \$mtd_path \$COMMON_DIR/\${image}.img
				if [ \$? -ne 0 ];then
					echo "Error: \$image write failed."
					exit 1
				fi
			fi
		done
		echo "Erase misc partition"
		flash_eraseall /dev/block/by-name/misc
		if [ \$? -ne 0 ];then
			echo "Error: Erase misc partition failed."
			exit 2
		fi
		;;
	*)
			echo "Not support storage medium type: \$bootmedium"
			exit 1
			;;
	esac
EOF
chmod a+x $OTA_SCRIPT

mtdparts=$(cat $PARAMETER_FILE | sed -n 's/.*CMDLINE:mtdparts=//p')
IFS=',' read -ra partitions <<< "$mtdparts"
partition_names=""
for partition in "${partitions[@]}"; do
	name=$(echo $partition | sed -n 's/.*(\(.*\)).*//p')
	if [[ $name =~ :grow ]]; then
		partition_names+="${name%:grow} "
	else
		partition_names+="$name "
	fi
done
partition_names=${partition_names% }

mkdir -p $(dirname $PROJECT_FILE_RECOVERY_SCRIPT)
echo "#!/bin/sh" > $PROJECT_FILE_RECOVERY_SCRIPT
cat >> $PROJECT_FILE_RECOVERY_SCRIPT <<EOF
partition_names="uboot trust misc recovery boot rootfs oem userdata"
linkdev(){
	if [ -e /dev/mmcblk0 ]; then
		storage_dev_prefix=mmcblk0p
		part_num=1
	elif [ -e /dev/rkflash0 ]; then
		storage_dev_prefix=rkflash0p
		part_num=1
	elif [ -e /dev/mtd0 ]; then
		storage_dev_prefix=mtd
		part_num=0
	elif [ -e /dev/mtdblock0 ]; then
		storage_dev_prefix=mtdblock
		part_num=0
	else
		echo unknow device 
	fi

	if [ ! -d "/dev/block/by-name" ];then
		mkdir -p /dev/block/by-name
		cd /dev/block/by-name

		for part_name in \$partition_names;
		do
			ln -sf /dev/\${storage_dev_prefix}\${part_num} \${part_name}
			part_num=\$(( part_num + 1 ))
		done
	fi
}

cmdline=\$(cat /proc/cmdline)
IFS=' ' read -ra items <<< "\$cmdline"
for item in "\${items[@]}"; do
	if [[ \$item =~ nand ]]; then
		bootmedium=spi_nand
	elif [[ \$item =~ emmc ]]; then
		bootmedium=emmc
	elif [[ \$item =~ nor ]]; then
		bootmedium=spi_nor
	fi
done

mount_part(){
if [ -z "\$1" -o -z "\$2" -o -z "\$3" ];then
	echo "Invalid paramter, exit !!!"
	exit 1
fi
root_dev=\$(mountpoint -n /)
root_dev=\${root_dev%% *}
partname=\$1
part_dev=/dev/block/by-name/\$1
mountpt=\$2
part_fstype=\$3
part_realdev=\$(realpath \$part_dev)
if [ ! -d \$mountpt ]; then
	if [ "\$mountpt" = "IGNORE" -a "emmc" = "\$bootmedium" ];then
		if [ "\$root_dev" = "\$part_realdev" ];then
			resize2fs \$part_dev
		fi
		return 0;
	else
		echo "\${0} info: mount point path [\$mountpt] not found, skip..."
		return 1;
	fi
fi
if test -h \$part_dev; then
case \$bootmedium in
	emmc)
		if [ "\$root_dev" = "\$part_realdev" ];then
			resize2fs \$part_dev
		else
			e2fsck -y \$part_dev
			mount -t \$part_fstype \$part_dev \$mountpt
			if [ \$? -eq 0 ]; then
				resize2fs \$part_dev
				tune2fs \$part_dev -L \$partname
			else
				echo "mount \$partname error, try to format..."
				mke2fs -F -L \$partname \$part_dev  && \
					tune2fs -c 0 -i 0 \$part_dev && \
					mount -t \$part_fstype \$part_dev \$mountpt
			fi
		fi
		;;
	spi_nand|slc_nand)
		if [ \$partname = "rootfs" ];then
			echo "rootfs mount on \$root_dev"
		elif [ "\$part_fstype" = "ubifs" ]; then
			part_no=\$(echo \$part_realdev | grep -oE "[0-9]*$")
			ubi_dev=/dev/ubi\${part_no}
			ubi_vol=\${ubi_dev}_0
			mount | grep \$ubi_vol
			if [ \$? -eq 0 ];then
				echo "***********\$partname has been mounted***********"
			else
				if [ ! -e \$ubi_vol ];then
					echo "***********\$ubi_vol not exist***********"
					if [ ! -e \$ubi_dev ];then
						echo "***********\$ubi_dev not exist***********"
						ubiattach /dev/ubi_ctrl -m \$part_no -d \$part_no
						if [ \$? -ne 0 ];then
							echo "ubiattach \$part_realdev error, try to format..."
							ubiformat -y \$part_realdev
							ubiattach /dev/ubi_ctrl -m \$part_no -d \$part_no
						fi
					fi
					ubi_info_dir=/sys/class/ubi/ubi\${part_no}
					avail_eraseblocks=\$(cat \$ubi_info_dir/avail_eraseblocks)
					eraseblock_size=\$(cat \$ubi_info_dir/eraseblock_size)
					echo "try to make volume: \$ubi_vol ..."
					ubimkvol \$ubi_dev -N \$partname -s \$((avail_eraseblocks*eraseblock_size))
				fi
				mount -t \$part_fstype \$ubi_vol \$mountpt
			fi
		elif [ "\$part_fstype" = "squashfs" ]; then
			part_no=\$(echo \$part_realdev | grep -oE "[0-9]*$")
			ubi_dev=/dev/ubi\${part_no}
			ubi_vol=\${ubi_dev}_0
			ubi_block=/dev/ubiblock\${part_no}_0
			mount | grep \$ubi_block
			if [ \$? -eq 0 ];then
				echo "***********\$partname has been mounted***********"
			else
				if [ ! -e \$ubi_block ];then
					echo "***********\$ubi_block not exist***********"
					ubiattach /dev/ubi_ctrl -m \$part_no -d \$part_no
					if [ \$? -ne 0 ];then
						echo "ubiattach \$part_realdev error, return !!!"
						echo "Please check the device: \$part_realdev"
						return 1
					fi
					ubiblock -c \$ubi_vol
				fi
				mount -t \$part_fstype \$ubi_block \$mountpt
			fi
		elif [ "\$part_fstype" = "erofs" ]; then
			part_no=\$(echo \$part_realdev | grep -oE "[0-9]*$")
			ubi_dev=/dev/ubi\${part_no}
			ubi_vol=\${ubi_dev}_0
			ubi_block=/dev/ubiblock\${part_no}_0
			mount | grep \$ubi_block
			if [ \$? -eq 0 ];then
				echo "***********\$partname has been mounted***********"
			else
				if [ ! -e \$ubi_block ];then
					echo "***********\$ubi_block not exist***********"
					ubiattach /dev/ubi_ctrl -m \$part_no -d \$part_no
					if [ \$? -ne 0 ];then
						echo "ubiattach \$part_realdev error, return !!!"
						echo "Please check the device: \$part_realdev"
						return 1
					fi
					ubiblock -c \$ubi_vol
				fi
				mount -t \$part_fstype \$ubi_block \$mountpt
			fi
		else
			echo "Error: wrong filesystem type: \$part_fstype, return !!!"
			return 1
		fi
		;;
	spi_nor)
		if [ "\$root_dev" = "\$part_realdev" ];then
			echo "***********\$part_dev has been mounted, skipping***********"
		else
			echo "mount -t \$part_fstype \$part_dev \$mountpt"
			mount -t \$part_fstype \$part_dev \$mountpt
			if [ \$? -eq 0 ]; then
				echo "***********succeed in mounting***********"
			elif [ "\$part_fstype" = "jffs2" ]; then
				echo "mount \$partname error, try to format..."
				echo "flash_erase -j \${part_realdev/block/} 0 0 && mount -t \$part_fstype \$part_dev \$mountpt"
				flash_erase -j \${part_realdev/block/} 0 0 && mount -t \$part_fstype \$part_dev \$mountpt
			else
				echo "mount \$partname error, skipping! Please check the filesystem."
			fi
		fi
		;;
	*)
		echo "Invalid Parameter: Check bootmedium !!!"
		exit 1
		;;
esac
fi
}

case \$1 in start) linkdev;
;; stop) printf stop \$0 finished\n ;; *) echo Usage: \$0 {start|stop} exit 1 ;; esac
EOF

chmod a+x $PROJECT_FILE_RECOVERY_SCRIPT

cp -fa $RKLUNCH_FILE $TARGET/usr/bin/RkLunch.sh
chmod a+x $TARGET/usr/bin/RkLunch.sh
