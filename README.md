# rk3506-ubuntu
<img width="1024" height="576" alt="image" src="https://github.com/user-attachments/assets/6295b83a-7a8e-4d2b-b0ba-6c5242364663" />

<img width="750" height="805" alt="image" src="https://github.com/user-attachments/assets/c7277eb4-2fff-4f3f-acee-21915a6b2c33" />

<img width="750" height="1040" alt="image" src="https://github.com/user-attachments/assets/11ac862c-de62-415a-a380-de043a691bb2" />

<img width="750" height="796" alt="image" src="https://github.com/user-attachments/assets/3417e9d1-f991-426f-bb6f-f2afe243ef82" />

<img width="760" height="695" alt="image" src="https://github.com/user-attachments/assets/2a4baff5-b6d1-4fc5-a5bb-7b703dc43c21" />

<img width="994" height="631" alt="image" src="https://github.com/user-attachments/assets/7d31978d-cfa9-402c-be57-af54b155b2e0" />


Hardware Manual & Test Manual

https://github.com/markbirss/rk3506-ubuntu/releases/tag/1.2


Important Note: ( pg 21 2.7 Test Manual )

The WLAN function shares interface with TF card and eMMC, only one can be used at a time

RK3506(256MB+256MB) WiP ( still to test all functions)

eg
watchdog ( not enabled in device tree yet)

in order to boot directly from SDCard move and set to 3V3 jumper position (default is 1V8 for SPI Nand Boot) J28 (close by sdcard slot)

<img width="1449" height="1269" alt="Boot_with_sdcard_instead_of_spi_nand_move_jumper_to_3v3" src="https://github.com/user-attachments/assets/e9a89622-3c97-4542-b90b-0af648462dd2" />


```
git clone -b qiyang https://github.com/markbirss/rk3506-ubuntu.git
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

Installation steps (Connect UART and USB Cable, baud rate now is 1500000 not 115200)
```
Set Jumpber J28 to 3V3 position ( close by sdcard slot )

Hold down MaskROM Button while insering Power
or
Hold down MaskROM Button, press Reset Button, Release Reset Button, while still holding down MaskRom Button, then release MaskRom Button

Confirm board is in MaskRom Mode
lsusb
Bus 003 Device 041: ID 2207:350f Fuzhou Rockchip Electronics Company

or

upgrade_tool ld
List of rockusb connected(1)
DevNo=1 Vid=0x2207,Pid=0x350f,LocationID=391    Mode=Maskrom    SerialNo=


# https://github.com/markbirss/rkdeveloptool
rkdeveloptool db rockdev/MiniLoaderAll.bin

rkdeveloptool rfi
Flash Info:
        Manufacturer: SAMSUNG, value=00
        Flash Size: 255 MB
        Flash Size: 523264 Sectors
        Block Size: 128 KB
        Page Size: 2 KB
        ECC Bits: 0
        Access Time: 40
        Flash CS: Flash<0>

#erase the 256MB Onboard SPI Nand Flash
rkdeveloptool ef

#insert a sdcard into the dev board and change default storage device to sdcard
rkdeveloptool cs 1

# Confirm your sdcard is detected
rkdeveloptool rfi

Flash Info:
        Manufacturer: SAMSUNG, value=00
        Flash Size: 15193 MB
        Flash Size: 31116288 Sectors
        Block Size: 512 KB
        Page Size: 2 KB
        ECC Bits: 0
        Access Time: 40
        Flash CS: Flash<0> 

#flash Ubuntu OS to sdcard

sudo ./rkflash.sh update
or
upgrade_tool uf rockdev/update.img
upgrade_tool rd

```

```
To be able to compile kernel drivers on the board, install

https://github.com/markbirss/rk3506-ubuntu/releases/tag/1.2

wget -c https://github.com/markbirss/linux-6.1.118/releases/download/1/linux-headers-6.1.118_6.1.118-17_armhf.deb
sudo dpkg -i linux-headers-6.1.118_6.1.118-17_armhf.deb


```

```
Default User Login Credentials

User:     root
Password: root

User:     lyra
Password: luckfox

ADB shell requires no password and can be used to set or change existing passwords

```

#IMPORTANT NOTE
This SDK is provided for non commercial use only

UBUNTU require official autorization for commerical use

This SDK is provided without any warranty
Use at your own risk

Support my work and consider **buying  me a coffee**

https://buymeacoffee.com/mark.birss
