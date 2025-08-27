# rk3506-ubuntu
<img width="1024" height="576" alt="image" src="https://github.com/user-attachments/assets/6295b83a-7a8e-4d2b-b0ba-6c5242364663" />


Ubuntu 24.04.3 OS image builder for various RK3506 SBC
(based on the Luckfox Lyra SDK when it could still build Ubuntu 22.04 OS Images and further modified to include a updated Ubuntu 24.04.3 OS and support for the later Luckfox Lyra SBC boards with ability to run luckfox-config) 


BOARD | Tested OK with luckfox-config |
|:--|:--|
| Luckfox Lyra | Yes |
| Luckfox Lyra PLUS | Yes | 
| Luckfox Lyra Zero W | Yes |
| Luckfox Lyra Pi W emmc| Yes |
| ArmSoM Forge 1 | Still need to modify luckfox-config for the availabe RM_IO gpio for this board - see below *|

Supported Boards
1. luckfox_lyra_pi-w_ubuntu_emmc_defconfig
2. luckfox_lyra_pi-w_ubuntu_sdmmc_defconfig
3. luckfox_lyra_plus_ubuntu_sdmmc_defconfig
4. luckfox_lyra_ubuntu_sdmmc_defconfig
5. luckfox_lyra_ultra-w_ubuntu_emmc_defconfig
6. luckfox_lyra_zero-w_ubuntu_sdmmc_defconfig
7. rk3506-armsom-forge1_ubuntu_sdmmc_defconfig

<img width="1044" height="810" alt="Screenshot_20250811_221618" src="https://github.com/user-attachments/assets/6ddc87d3-118e-4c18-bd1b-7213a583ea9d" />

<img width="1044" height="810" alt="Screenshot_20250811_221640" src="https://github.com/user-attachments/assets/243fcbed-6f48-4417-ba7e-6a6203df4d14" />

<img width="1044" height="810" alt="Screenshot_20250811_221648" src="https://github.com/user-attachments/assets/b02b38e1-ece0-4d57-887b-de53b5d9d7d0" />

Luckfox Lyra pinout

https://wiki.luckfox.com/Luckfox-Lyra/Pinout/

Forge1 RM_IO pinout

<img width="599" height="456" alt="ArmSoM-Forge1-RockChip-RK3506J-SBC" src="https://github.com/user-attachments/assets/0cdb2628-1c2d-4dca-ab5a-171c4b9258f1" />


<img width="1178" height="985" alt="Screenshot_20250822_114824" src="https://github.com/user-attachments/assets/567eced2-b9ec-4428-b498-12ce52f47dac" />

Key: Green=GPIO, Blue=RM_IO, Red=Power, Black=GND

```
[Wireless still requires wifi kernel modules installed]
adb shell "cd /home/lyra/aic800/ && make install; reboot"

test
adb shell nmcli dev wifi list

```

Example Hardware running DSI 5" 720x1280 display and Luckfox Lyra Pi

Front
<img width="1848" height="4000" alt="image" src="https://github.com/user-attachments/assets/88bd6d05-3c2c-4b7e-97df-c2ff23c6cf2c" />

Back
<img width="576" height="1188" alt="image" src="https://github.com/user-attachments/assets/9ac3e177-f7fe-4e58-b193-923bd2253adf" />


Display
https://www.waveshare.com/5-dsi-touch-a.htm

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
git clone https://github.com/markbirss/rk3506-ubuntu.git

cd rk3506-ubuntu/device/rockchip/.chips/rk3506
ln -s .chips/rk3506 ../../rk3506
ln -s .chips/rk3506 ../../.chip
cd ../../../../

#sha256sum
#d6f58545b0b9c679665a8ff58dd2a7a75aa2b2648871e4be5a2c2288b4261545  ubuntu_24.04.3.tar.gz

git clone https://github.com/markbirss/ubuntu_24.04.3.git
cd ubuntu_24.04.3
rm -fr .git
7z x ubuntu_24.04.3.7z.001
sha256sum ubuntu_24.04.3.tar.gz

rm -f ubuntu_24.04.3.7z.*

mv ubuntu_24.04.3.tar.gz ../
cd ../
mkdir ubuntu
mv ubuntu_24.04.3.tar.gz ubuntu

#./build.sh lunch
# sudo ./build.sh
# sudo ./rkflash.sh update

```
Support my work and considder **buying  me a coffee**

https://buymeacoffee.com/mark.birss
