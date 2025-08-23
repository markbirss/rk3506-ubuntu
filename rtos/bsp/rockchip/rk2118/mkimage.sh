#! /bin/bash

export LC_ALL=C.UTF-8
export LANG=C.UTF-8

usage() {
    echo "usage: ./mkimage.sh [partition_setting] [soundbar|partybox|gamebar]"
}

get_part_size_kb() {
    if [[ "$1" == *"setting.ini"* ]]; then
        local part_size=$(awk -v name="$2" '$0 ~ "Name="name {getline; getline; getline; print}' "$1" | grep -o '0x[0-9A-Fa-f]\+')
    else
        local line=$(grep -oP "0x[0-9a-fA-F]+@0x[0-9a-fA-F]+(?=\($2\))" "$1")
        local part_size=$(echo "$line" | cut -d '@' -f 1)
    fi

    if [ -z "$part_size" ]; then
        echo "part size not found in $1"
        return 1
    fi

    local part_size_decimal=$((part_size / 2))

    echo $part_size_decimal
    return 0
}

mk_misc_img() {
    config="$1"
    if [[ $1 == *"package-file"* ]]; then
       config=$(echo $1 | awk -F/ '{print $1 "/" $2 "/" "parameter.txt"}')
    fi

    ret=$(get_part_size_kb $config "misc")
    if [ $? -ne 0 ]; then
        echo "no misc partintion"
        return
    fi

    echo "make misc.img: size is: $ret kb"
    ./mkmisc.py $ret
}

replace_demo() {
  if [[ "$1" != "soundbar" && "$1" != "partybox" && "$1" != "gamebar" ]]; then
    echo "Invalid argument. Please pass 'soundbar', 'partybox', or 'gamebar'."
    usage
    return 1
  fi

  sub_str="evb_$1_demo"

  sed -i "s/evb_soundbar_demo/$sub_str/g" $2
}

CUR_DIR=$(pwd)
TOOLS=$CUR_DIR/../tools
IMAGE=$(pwd)/Image

../tools/sign_tool/bin/boot_merger ./Image/rk2118_ddr.ini
../tools/sign_tool/bin/boot_merger ./Image/rk2118_no_ddr.ini
# Expand the rk2118_idb_no_ddr/rk2118_idb_ddr file to 64KB and make a backup copy
./package_idb.py Image/rk2118_idb_no_ddr.img
./package_idb.py Image/rk2118_idb_ddr.img

rm -rf $IMAGE/rtt*.img $IMAGE/Firmware*

if [ ! -n "$1" ] ;then
    ./package_image.py board/common/setting.ini
    if [ ! $? -eq 0 ] ;then
        echo "mkimage fail"
        exit
    fi
    $TOOLS/firmware_merger/firmware_merger -p board/common/setting.ini $IMAGE/
else
    mk_misc_img $1

    case $1 in
        *"package-file"*)
            #./package_image.py board/evb/dual_cpu_setting.ini
            parameter_file=$(echo $1 | awk -F/ '{print $1 "/" $2 "/" "parameter.txt"}')

            cp "$1" "$1.bk"
            if [ -n "$2" ] ;then
                replace_demo $2 $1
            fi

            ./package_image.py $parameter_file

            path=$1
            $TOOLS/sign_tool/bin/afptool -pack ${path%package-file*} $IMAGE/update.img $path
            cp "$1.bk" "$1"
            rm "$1.bk"

            $TOOLS/sign_tool/bin/rkImageMaker "-RK2118" $IMAGE/MiniLoaderAllDDR.bin $IMAGE/update.img tmp_update.img -os_type:rt-thread
            mv tmp_update.img $IMAGE/update.img
            ;;
        *)
            ./package_image.py $1
            if [ ! $? -eq 0 ] ;then
                echo "mkimage fail"
                exit
            fi

            $TOOLS/firmware_merger/firmware_merger -p $1 $IMAGE/
            ;;
    esac
fi
