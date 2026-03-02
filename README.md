# rk3506-ubuntu 

# Ubuntu 24.04.4 with 4.2 inch 300x400 ST7305 RLCD Display

<img width="1121" height="793" alt="image" src="https://github.com/user-attachments/assets/ee3260f4-e215-40d6-ada3-9e21fabba77a" />

Enable Console Display Rotation (Does not affect Framebuffer Display Rotation)
```
adb shell "systemctl enable display.service"
adb shell reboot
```

<img width="1014" height="732" alt="image" src="https://github.com/user-attachments/assets/2813c5c0-5395-42e9-b252-179bc68a3b8d" />

# ToDo
```
Modify board device tree for your specific display model and the pins used for connection before compile


luckfox_lyra_pi-w_ubuntu_emmc_defconfig			./kernel-6.1/arch/arm/boot/dts/rk3506b-luckfox-lyra-pi-w
luckfox_lyra_plus_ubuntu_sdmmc_defconfig  		./kernel-6.1/arch/arm/boot/dts/rk3506g-luckfox-lyra-plus-sd
luckfox_lyra_ultra-w_ubuntu_emmc_defconfig		./kernel-6.1/arch/arm/boot/dts/rk3506b-luckfox-lyra-ultra-w
luckfox_lyra_pi-w_ubuntu_sdmmc_defconfig  		./kernel-6.1/arch/arm/boot/dts/rk3506b-luckfox-lyra-pi-w-sd
luckfox_lyra_ubuntu_sdmmc_defconfig				./kernel-6.1/arch/arm/boot/dts/rk3506g-luckfox-lyra-sd       
luckfox_lyra_zero-w_ubuntu_sdmmc_defconfig		./kernel-6.1/arch/arm/boot/dts/rk3506b-luckfox-lyra-zero-w-sd

example - https://github.com/markbirss/st7305-kernel-drivers/blob/luckfox-lyra/rk3506g-luckfox-lyra-sd.dts
// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

/dts-v1/;

#include "rk3506-luckfox-lyra.dtsi"

/ {
	model = "Luckfox Lyra";
	compatible = "rockchip,rk3506g-demo-display-control", "rockchip,rk3506";

	chosen {
		bootargs = "earlycon=uart8250,mmio32,0xff0a0000 console=tty1 console=ttyFIQ0 storagemedia=sd root=/dev/mmcblk0p3 rootfstype=ext4 rootwait snd_aloop.index=7 snd_aloop.use_raw_jiffies=1";
	};
};

/**********display**********/
&cma {
	size = <0x2000000>;
};

&dsi {
	status = "disabled";
};

&dsi_dphy {
	status = "disabled";
};

&dsi_in_vop {
	status = "disabled";
};

&route_dsi {
	status = "disabled";
};

&dsi_panel {
	status = "disabled";
};

/**********ethernet**********/
&gmac1 {
	status = "disabled";
};

&mdio1 {
	status = "disabled";
};

/**********usb**********/
&usb20_otg0 {
	dr_mode = "peripheral";
	status = "okay";
};

&usb20_otg1 {
	dr_mode = "host";
	status = "okay";
};

&pinctrl {
	tft {
		tft_pins: tft-pins {
			rockchip,pins = <0 RK_PA2 RK_FUNC_GPIO &pcfg_pull_none>, // reset
					<0 RK_PA3 RK_FUNC_GPIO &pcfg_pull_none>; // dc
		};
	};
};

&spi0 {
	pinctrl-names = "default";
	pinctrl-0 = <&rm_io29_spi0_clk &rm_io28_spi0_mosi &rm_io4_spi0_csn0>;

	status = "okay";

	tft: st7305@0 {
		#address-cells = <1>;
		#size-cells = <1>;

		pinctrl-names = "default";
		pinctrl-0 = <&tft_pins>;

		// compatible = "osptek,ydp154h008-v3";
		// compatible = "osptek,ydp213h001-v3";
		// compatible = "osptek,ydp290h001-v3";
		// compatible = "osptek,ydp420h001-v3";
		compatible = "swi,lhf420tb-f07";

		spi-max-frequency = <50000000>;
		reg = <0>;

		reset-gpios = <&gpio0 RK_PA2 GPIO_ACTIVE_HIGH>;
		dc-gpios = <&gpio0 RK_PA3 GPIO_ACTIVE_HIGH>;

		status = "okay";
	};
};
```

# ST7305 DRM Display Driver developed by

hua.zheng@embeddedboys.com

https://github.com/IotaHydrae/st7305-kernel-drivers

SDK Usage

```
[prepare]
Use either a docker Ubuntu 22.04 or Ubuntu 22.04 environment

Install dependency packages.

sudo apt update

sudo apt-get update && sudo apt-get install git ssh make gcc libssl-dev \
liblz4-tool expect expect-dev g++ patchelf chrpath gawk texinfo chrpath \
diffstat binfmt-support qemu-user-static live-build bison flex fakeroot \
cmake gcc-multilib g++-multilib unzip device-tree-compiler ncurses-dev \
libgucharmap-2-90-dev bzip2 expat gpgv2 cpp-aarch64-linux-gnu libgmp-dev \
libmpc-dev bc python-is-python3 python2 

sudo ln -sf /usr/bin/python2 /usr/bin/python 

[build instructions]
git clone -b st7305 https://github.com/markbirss/rk3506-ubuntu.git

cd rk3506-ubuntu/device/rockchip/.chips/rk3506
ln -s .chips/rk3506 ../../rk3506
ln -s .chips/rk3506 ../../.chip
cd ../../../../

# sha256sum
# 231554183fc807b066c33633b1b6066b800d830c6435e533597f659300c24a25  ubuntu_24.04.4.tar.gz

git clone https://github.com/markbirss/ubuntu_24.04.4.git
cd ubuntu_24.04.4
rm -fr .git
7z x ubuntu_24.04.4.7z.001

rm -f ubuntu_24.04.4.7z.*

mv ubuntu_24.04.4.tar.gz ../
cd ../
mkdir ubuntu
mv ubuntu_24.04.4.tar.gz ubuntu/ubuntu_24.04.3.tar.gz

#./build.sh lunch
# sudo ./build.sh
# sudo ./rkflash.sh update

```

#IMPORTANT NOTE
This SDK is provided for non commercial use only

UBUNTU require official autorization for commerical use

This SDK is provided without any warranty
Use at your own risk

Support my work and consider **buying  me a coffee**

https://buymeacoffee.com/mark.birss
