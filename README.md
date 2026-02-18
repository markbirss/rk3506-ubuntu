# rk3506-ubuntu Ubuntu 24.04.4 with 4.2 inch ST7305 RLCD Display


Rotate the Console Display Rotation (Does not affect Framebuffer)
```
adb shell "systemctl enable display.service"
adb shell reboot
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
