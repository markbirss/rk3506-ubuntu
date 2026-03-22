# rk3506-ubuntu
<img width="1024" height="576" alt="image" src="https://github.com/user-attachments/assets/6295b83a-7a8e-4d2b-b0ba-6c5242364663" />

<img width="750" height="805" alt="image" src="https://github.com/user-attachments/assets/c7277eb4-2fff-4f3f-acee-21915a6b2c33" />

<img width="750" height="1040" alt="image" src="https://github.com/user-attachments/assets/11ac862c-de62-415a-a380-de043a691bb2" />

<img width="750" height="796" alt="image" src="https://github.com/user-attachments/assets/3417e9d1-f991-426f-bb6f-f2afe243ef82" />

<img width="760" height="695" alt="image" src="https://github.com/user-attachments/assets/2a4baff5-b6d1-4fc5-a5bb-7b703dc43c21" />

RK3506(256MB+256MB) WiP

in order to boot directly from SDCard not SPI nand set 3V3 jumper J28 (close by sdcard slot)

<img width="1449" height="1269" alt="Boot_with_sdcard_instead_of_spi_nand_move_jumper_to_3v3" src="https://github.com/user-attachments/assets/e9a89622-3c97-4542-b90b-0af648462dd2" />


```
git clone -b qiyang https://github.com/markbirss/rk3506-ubuntu.git
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

#IMPORTANT NOTE
This SDK is provided for non commercial use only

UBUNTU require official autorization for commerical use

This SDK is provided without any warranty
Use at your own risk

Support my work and consider **buying  me a coffee**

https://buymeacoffee.com/mark.birss
