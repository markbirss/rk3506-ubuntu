# rk3506-ubuntu
<img width="1024" height="576" alt="image" src="https://github.com/user-attachments/assets/6295b83a-7a8e-4d2b-b0ba-6c5242364663" />


Ubuntu 24.04.3 OS image builder for various RK3506 SBC

(based off the Luckfox Lyra SDK when it could still build Ubuntu 22.04 OS Images but further modified to include updated Ubuntu 24.04.3 OS and support for the later Luckfox Lyra SBC boards with ability to run luckfox-config) 


BOARD | Tested OK with luckfox-config |
|:--|:--|
| Luckfox Lyra | Yes |
| Luckfox Lyra PLUS | Yes | 
| Luckfox Lyra Zero W | Yes |
| Luckfox Lyra Pi W emmc| Yes |
| ArmSoM Forge 1 | Solved with https://github.com/markbirss/rk3506-ubuntu/releases/tag/1.1 |

Supported Boards
Board Name | SD/EMMC |  Defconfig |
|:--|:--|:--|
| Luckfox Lyra | SDCard | 4. luckfox_lyra_ubuntu_sdmmc |
| Luckfox Lyra PLUS | SDCard | 3. luckfox_lyra_plus_ubuntu_sdmmc |
| Luckfox Lyra Ultra W | eMMC | 5. luckfox_lyra_ultra-w_ubuntu_emmc |
| Luckfox Zero W | SDCard | 6. luckfox_lyra_zero-w_ubuntu_sdmmc |
| Luckfox Lyra Pi W | SDCard | 2. luckfox_lyra_pi-w_ubuntu_sdmmc |
| Luckfox Lyra Pi W | eMMC | 1. luckfox_lyra_pi-w_ubuntu_emmc |
| ArmSom Forge (BPI Forge1) | SDCard | 7. rk3506-armsom-forge1_ubuntu_sdmmc |

```
Default User Login Credentials

User:     root
Password: root

User:     lyra
Password: luckfox

ADB shell requires no password and can be used to set or change existing passwords
```

# IMPORTANT NOTE FOR LYRA BOARDS WITH WIFI

```
adb shell "cd /home/lyra/aic800/ && make install; reboot"

#test
adb shell nmcli dev wifi list

#connect
nmtui
```

Luckfox Lyra boards Specifications

<img width="2069" height="589" alt="image" src="https://github.com/user-attachments/assets/83f69150-153a-47c7-aff1-520b722be1f4" />


<img width="1044" height="810" alt="Screenshot_20250811_221618" src="https://github.com/user-attachments/assets/6ddc87d3-118e-4c18-bd1b-7213a583ea9d" />

<img width="1044" height="810" alt="Screenshot_20250811_221640" src="https://github.com/user-attachments/assets/243fcbed-6f48-4417-ba7e-6a6203df4d14" />

<img width="1044" height="810" alt="Screenshot_20250811_221648" src="https://github.com/user-attachments/assets/b02b38e1-ece0-4d57-887b-de53b5d9d7d0" />

<img width="960" height="686" alt="image" src="https://github.com/user-attachments/assets/750a08c5-6305-41ad-ace7-5f2db111cbc3" />

4G LTE howto

```
nmcli connection add type gsm ifname '*' apn 'internet' connection.autoconnect yes
nmcli conn up gsm --ask

route
route del default gw x.x.x.x dev end1

nmcli dev status
DEVICE    TYPE      STATE                   CONNECTION         
end1      ethernet  connected               Wired connection 2 
cdc-wdm0  gsm       connected               gsm                
lo        loopback  connected (externally)  lo                 
end0      ethernet  unavailable             --
```

<img width="839" height="805" alt="image" src="https://github.com/user-attachments/assets/44470aee-309c-4d59-802e-39911f24f157" />

```
https://www.waveshare.com/sma-to-ipex-cable.htm?sku=21120

Get Location (Guess you should really connect up a GPS antenna, but it not supplied) IPEX-4 to SMA
mmcli -m 0 --location-status
  ------------------------
  Location | capabilities: 3gpp-lac-ci, gps-raw, gps-nmea, gps-unmanaged, agps-msa, 
           |               agps-msb
           |      enabled: gps-raw, gps-nmea
           |      signals: no
  ------------------------
  GPS      | refresh rate: 30 seconds

mmcli -m 0 --location-get
mmcli -m 0


root@luckfox:~# lsusb
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 001 Device 002: ID 1a86:8091 QinHeng Electronics USB HUB
Bus 001 Device 004: ID 1e0e:9001 Qualcomm / Option SimTech, Incorporated
Bus 001 Device 005: ID a69c:88dc AICSemi AIC8800DC

cat /etc/luckfox.cfg 
TP_STATUS=1
I2C2_STATUS=1
I2C2_SCL_RM_IO=1
I2C2_SDA_RM_IO=0
MODULE_4G_ENABLE=1
MODULE_4G_MODE=ppp
MODULE_4G_APN=internet

root@luckfox:~# lsusb -tv
/:  Bus 001.Port 001: Dev 001, Class=root_hub, Driver=dwc2/1p, 480M
    ID 1d6b:0002 Linux Foundation 2.0 root hub
    |__ Port 001: Dev 002, If 0, Class=Hub, Driver=hub/4p, 480M
        ID 1a86:8091 QinHeng Electronics 
        |__ Port 001: Dev 004, If 0, Class=Wireless, Driver=btusb, 480M
            ID a69c:88dc  
        |__ Port 001: Dev 004, If 1, Class=Wireless, Driver=btusb, 480M
            ID a69c:88dc  
        |__ Port 001: Dev 004, If 2, Class=Vendor Specific Class, Driver=[none], 480M
            ID a69c:88dc  
        |__ Port 002: Dev 003, If 0, Class=Vendor Specific Class, Driver=option, 480M
            ID 1e0e:9001 Qualcomm / Option 
        |__ Port 002: Dev 003, If 1, Class=Vendor Specific Class, Driver=option, 480M
            ID 1e0e:9001 Qualcomm / Option 
        |__ Port 002: Dev 003, If 2, Class=Vendor Specific Class, Driver=option, 480M
            ID 1e0e:9001 Qualcomm / Option 
        |__ Port 002: Dev 003, If 3, Class=Vendor Specific Class, Driver=option, 480M
            ID 1e0e:9001 Qualcomm / Option 
        |__ Port 002: Dev 003, If 4, Class=Vendor Specific Class, Driver=option, 480M
            ID 1e0e:9001 Qualcomm / Option 
        |__ Port 002: Dev 003, If 5, Class=Vendor Specific Class, Driver=qmi_wwan, 480M
            ID 1e0e:9001 Qualcomm / Option
```



Luckfox Lyra pinout

https://wiki.luckfox.com/Luckfox-Lyra/Pinout/

<img width="960" height="491" alt="image" src="https://github.com/user-attachments/assets/1165ee4a-bf58-4776-9caa-3a51aacdc886" />

Forge1 RM_IO pinout

Board cator more for CanBus/Flexibus use than RM_IO gpio

<img width="599" height="456" alt="ArmSoM-Forge1-RockChip-RK3506J-SBC" src="https://github.com/user-attachments/assets/0cdb2628-1c2d-4dca-ab5a-171c4b9258f1" />


<img width="1178" height="985" alt="Screenshot_20250822_114824" src="https://github.com/user-attachments/assets/567eced2-b9ec-4428-b498-12ce52f47dac" />

Key: Green=GPIO, Blue=RM_IO, Red=Power, Black=GND


```
RMIO pins and GPIO pins
GPIO pins larger than 32 still require you to subract 32 (gpiochip1)

                Armsom Forge1 RK3506J
                     + -USB- +
-         -  3V3_OUT | 1   2 | VSYS     -         -
- RM_IO4  - GPIO0_A4 | 3   4 | VSYS     -         -
- RM_IO5  - GPIO0_A5 | 5   6 | GND      -         -
- RM_IO31 - GPIO1_D3 | 7   8 |*GPIO0_C6 - RM_IO22 -
-         -      GND | 9  10 |*GPIO0_C7 - RM_IO23 -
- RM_IO30 - GPIO1_D2 | 11 12 | GPIO1_D1 - RM_IO29 -
-         -       52 | 13 14 | GND      -         -
- RM_IO28 - GPIO1_C3 | 15 16 | GPIO1_B4 - RM_IO27 -
-         -  3V3_OUT | 17 18 | 49       -         -
-         -       48 | 19 20 | GND      -         -
-         -       46 | 21 22 | 47       -         -
-         -       44 | 23 24 | 45       -         -
-         -      GND | 25 26 | GPIO1_B3 - RM_IO26 -
-         -          | 27 28 | GPIO1_B2 - RM_IO25 -
-         -       40 | 29 30 | GND      -         -
-         -       38 | 31 32 | 39       -         -
-         -       37 | 33 34 | GND      -         -
-         -       35 | 35 36 | 36       -         -
-         -       33 | 37 38 | 34       -         -
-         -      GND | 39 40 | 32       -         -
                     + - + - +
                     
RM_IO4		4		4	
RM_IO5		5		5
RM_IO22		22		22
RM_IO23		23		23
RM_IO25		42		42-32=10
RM_IO26		43		43-32=11
RM_IO27		50		50-32-18
RM_IO28		51		51-32=19
RM_IO29		57		57-32=25
RM_IO30		58		58-32=26
RM_IO31		59		59-32=27
```


```
root@forge1:~# cat /sys/kernel/debug/gpio
gpiochip0: GPIOs 0-31, parent: platform/ff940000.gpio, gpio0:
 gpio-1   (                    |vcc3v3-lcd0-n       ) out hi 
 gpio-3   (                    |cd                  ) in  lo IRQ ACTIVE LOW
 gpio-24  (                    |hp-det              ) in  lo IRQ 

gpiochip1: GPIOs 32-63, parent: platform/ff870000.gpio, gpio1:
 gpio-54  (                    |spk-con             ) out lo 
 gpio-56  (                    |vcc5v0-otg1-regulato) out hi 

gpiochip2: GPIOs 64-95, parent: platform/ff1c0000.gpio, gpio2:

gpiochip3: GPIOs 96-127, parent: platform/ff1d0000.gpio, gpio3:

gpiochip4: GPIOs 128-159, parent: platform/ff1e0000.gpio, gpio4:
 gpio-139 (                    |rockchip:work_led:sy) out lo

cat /sys/firmware/devicetree/base/model
Rockchip RK3506J(BGA) ArmSoM Forge1

gpiodetect 
gpiochip0 [gpio0] (32 lines)
gpiochip1 [gpio1] (32 lines)
gpiochip2 [gpio2] (32 lines)
gpiochip3 [gpio3] (32 lines)
gpiochip4 [gpio4] (32 lines)

gpioinfo 
gpiochip0 - 32 lines:
        line   0:      unnamed       unused   input  active-high 
        line   1:      unnamed "vcc3v3-lcd0-n" output active-high [used]
        line   3:      unnamed         "cd"   input   active-low [used]
        line  24:      unnamed     "hp-det"   input  active-high [used]

gpiochip1 - 32 lines:
        line  24:      unnamed "vcc5v0-otg1-regulator" output active-high [used]
```

Example Hardware running DSI 5" 720x1280 display and Luckfox Lyra Pi

Front
<img width="924" height="1731" alt="image" src="https://github.com/user-attachments/assets/11ce59f7-f726-40e7-8af3-f55c63c593ad" />


Back
<img width="576" height="1188" alt="image" src="https://github.com/user-attachments/assets/9ac3e177-f7fe-4e58-b193-923bd2253adf" />


Display
https://www.waveshare.com/5-dsi-touch-a.htm


XFCE4 Desktop

<img width="720" height="1280" alt="image" src="https://github.com/user-attachments/assets/34296bcb-434e-4f23-b10a-1991d2b2ece3" />

Install XFCE4 Desktop

```
dpkg-reconfigure tzdata
timedatectl set-ntp off; timedatectl set-ntp on

apt -y update; apt install -y --no-install-recommends xserver-xorg-input-all xserver-xorg-core xinit xfce4-terminal xserver-xorg-video-fbdev x11-utils;

apt -y install xfce4 dbus-x11 mesa-utils xubuntu-default-settings xfce4-goodies lightdm-gtk-greeter lightdm-gtk-greeter-settings;

rm /etc/systemd/system/default.target;
systemctl set-default graphical.target;
reboot

```

Return to Console only

```

systemctl set-default multi-user.target

```

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

Docker Notes

```
mkdir ~/sdk

docker run --platform linux/amd64 -it -v ~/sdk:/sdk --rm ubuntu:22.04 bash
```

```
apt update;

apt -y dist-upgrade;

apt-get -y install git ssh make gcc libssl-dev \
liblz4-tool expect expect-dev g++ patchelf chrpath gawk texinfo chrpath \
diffstat binfmt-support qemu-user-static live-build bison flex fakeroot \
cmake gcc-multilib g++-multilib unzip device-tree-compiler ncurses-dev \
libgucharmap-2-90-dev bzip2 expat gpgv2 cpp-aarch64-linux-gnu libgmp-dev \
libmpc-dev bc python-is-python3 python2 rsync sudo bsdmainutils nano;

ln -sf /usr/bin/python2 /usr/bin/python

#1. Africa
#Geographic area: 1

#25. Johannesburg
#Time zone: 24

adduser user

usermod -aG sudo user
su user
```

```
[build container] - run once
docker build --rm -f rk3506-ubuntu.dockerfile -t lyra:rk3506-ubuntu-build .

[use container]
cd /to-sdk
docker run --rm -it -v $PWD:/build -w /build --user $(id -u):$(id -g) lyra:rk3506-ubuntu-build
```

Related Repo's

```
WiFi Dongles
https://github.com/markbirss/rtw88
https://github.com/markbirss/rtw89
https://github.com/markbirss/aic8800d80.git
https://github.com/markbirss/aic800

Kernel 6.1.99 Headers
https://github.com/markbirss/linux-6.1.99
https://github.com/markbirss/linux-6.1.99/releases/download/1/linux-headers-6.1.99_6.1.99-3_armhf.deb

Flash Erase and switch storage
https://github.com/markbirss/rkdeveloptool
```

#IMPORTANT NOTE
This SDK is provided for non commercial use only

UBUNTU require official autorization for commerical use

This SDK is provided without any warranty
Use at your own risk

Support my work and consider **buying  me a coffee**

https://buymeacoffee.com/mark.birss

# How-To build

In WSL, create a containing folder (e.g. lyra-build), and get all needed repos:

```shell
mkdir lyra-build
cd lyra-build
git clone --depth=1 https://github.com/cnadler86/rk3506-ubuntu
git clone --depth=1 https://github.com/markbirss/ubuntu_24.04.3.git
cd ubuntu_24.04.3
rm -fr .git
7z x ubuntu_24.04.3.7z.001
sha256sum ubuntu_24.04.3.tar.gz
rm -f ubuntu_24.04.3.7z.*
mkdir ../rk3506-ubuntu/ubuntu/
mv ubuntu_24.04.3.tar.gz ../rk3506-ubuntu/ubuntu/
cd ..
rm -rf ubuntu_24.04.3
cd rk3506-ubuntu/device/rockchip/.chips/rk3506
ln -s .chips/rk3506 ../../rk3506
ln -s .chips/rk3506 ../../.chip
cd ../../../../
```

Check sha256sum of the ubuntu_24.04.3.tar.gz file matches the expected value (d6f58545b0b9c679665a8ff58dd2a7a75aa2b2648871e4be5a2c2288b4261545) to ensure the integrity of the file.

Then build the docker image:

```shell
docker build -f rk3506-ubuntu.dockerfile -t lyra:rk3506-ubuntu-build .
```

Then run the docker container:

```shell
docker run --platform linux/amd64 \
  --privileged \
  --mount type=bind,source=$(pwd),target=/build \
  -it lyra:rk3506-ubuntu-build /bin/bash
```

In the docker container, you can then run the build commands:

```shell
cd /build
./build.sh lunch
./build.sh
```

If kernel build fails, probably a new user need to be added to the docker container with sudo permissions, and then run the build commands again:

```shell
adduser user
usermod -aG sudo user
```
