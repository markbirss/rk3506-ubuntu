# rk3506-ubuntu
Ubuntu 24.04.3 OS image builder for various RK3506 SBC

BOARD | Tested OK with luckfox-config |
|:--|:--|
| Luckfox Lyra | Yes |
| Luckfox Lyra PLUS | Yes | 
| Luckfox Lyra Zero W | Yes |
| Luckfox Lyra Pi W emmc| Yes |
| ArmSoM Forge 1 | Still need to modify luckfox-config for the availabe RM_IO gpio for this board - see below *|

Suppored Boards
1. luckfox_lyra_pi-w_ubuntu_emmc_defconfig
2. luckfox_lyra_pi-w_ubuntu_sdmmc_defconfig
3. luckfox_lyra_plus_ubuntu_sdmmc_defconfig
4. luckfox_lyra_ubuntu_sdmmc_defconfig
5. luckfox_lyra_ultra-w_ubuntu_emmc_defconfig
6. luckfox_lyra_zero-w_ubuntu_sdmmc_defconfig
7. rk3506-armsom-forge1_ubuntu_sdmmc_defconfig



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

```
git clone https://github.com/markbirss/rk3506-ubuntu.git

cd rk3506-ubuntu/device/rockchip/.chips/rk3506
ln -s .chips/rk3506 ../../rk3506
ln -s .chips/rk3506 ../../.chip
cd ../../../../

d6f58545b0b9c679665a8ff58dd2a7a75aa2b2648871e4be5a2c2288b4261545  ubuntu_24.04.3.tar.gz

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
