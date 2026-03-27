# rk3506-ubuntu
<img width="1024" height="576" alt="image" src="https://github.com/user-attachments/assets/6295b83a-7a8e-4d2b-b0ba-6c5242364663" />


<img width="2829" height="2078" alt="image" src="https://github.com/user-attachments/assets/8964ecbc-39c7-43e3-b621-95b8693fc11f" />

<img width="784" height="1245" alt="image" src="https://github.com/user-attachments/assets/ff6639fc-5c0b-471b-984c-c213a06a8691" />

<img width="950" height="1485" alt="image" src="https://github.com/user-attachments/assets/30b48fbd-2138-4433-bddf-9c675edd5a4e" />

<img width="950" height="1589" alt="image" src="https://github.com/user-attachments/assets/c890d975-9885-4819-8333-5515aa7fa167" />


Product
```
5.5" MIPI Display - ECA11-5LM
RK3506 Dev Board -  ECB41-PGE1N2-N
```

SDK Usage

```
[build instructions]
git clone -b e-byte https://github.com/markbirss/rk3506-ubuntu.git

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
