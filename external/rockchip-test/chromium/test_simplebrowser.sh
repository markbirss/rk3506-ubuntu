#!/bin/bash -e


# Export mpp_syslog_perror
export mpp_syslog_perror=1

# Get kernel version
KERNEL_VERSION=$(uname -r)

# Set rk_vcodec debug parameter based on kernel version
if echo "$KERNEL_VERSION" | grep -q '^4\.4'; then
    echo 0x100 > /sys/module/rk_vcodec/parameters/debug
else
    echo 0x100 > /sys/module/rk_vcodec/parameters/mpp_dev_debug
fi

## Set CPU governor to performance
# Find all CPU governor files
GOVERNOR_FILES=$(find /sys/ -name *governor)

# Set governor to performance
echo performance | tee $GOVERNOR_FILES > /dev/null 2>&1 || true

if [ -e "/usr/lib/qt/examples/webenginewidgets/simplebrowser" ] ;
then
	cd /usr/lib/qt/examples/webenginewidgets/simplebrowser
	./simplebrowser
	#./simplebrowser https://www.baidu.com
	#./simplebrowser "file:///oem/SampleVideo_1280x720_5mb.mp4"
else
	echo "Please sure the config/rockchip_xxx_defconfig include "qtwebengine.config"........"
fi
echo "the governor is performance for now, please restart it........"
