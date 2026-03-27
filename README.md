# rk3506-ubuntu
<img width="1024" height="576" alt="image" src="https://github.com/user-attachments/assets/6295b83a-7a8e-4d2b-b0ba-6c5242364663" />


<img width="4419" height="3015" alt="image" src="https://github.com/user-attachments/assets/621024d4-d007-47d2-a976-4c72b9c89b37" />

<img width="4239" height="3003" alt="image" src="https://github.com/user-attachments/assets/e535f1d6-3b87-47ca-9fc3-8c0bf0ec35a6" />

Pinout
<img width="1227" height="534" alt="image" src="https://github.com/user-attachments/assets/bcbc2913-c405-48e1-b12f-f4ac71b19c71" />

<img width="1155" height="867" alt="image" src="https://github.com/user-attachments/assets/cea89eee-59c2-4e2e-813c-0d1463763863" />

<img width="1155" height="867" alt="image" src="https://github.com/user-attachments/assets/0eff3aa9-748f-41e2-a781-4c4912a3535c" />

<img width="651" height="574" alt="image" src="https://github.com/user-attachments/assets/0746a4ff-6595-4591-ad87-4f46252d6203" />

```
git clone -b hd-rk3506g-mini https://github.com/markbirss/rk3506-ubuntu.git

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

# IMPORTANT NOTE ENABLE WIFI
```
adb shell "cd /home/lyra/aic800/ && make install; reboot"

#test
adb shell nmcli dev wifi list

#connect
nmtui
```

# Linux Host Internet Connection Sharing over USB

<img width="696" height="694" alt="image" src="https://github.com/user-attachments/assets/aecd01fe-b5cf-411b-ad0d-a4eb04512c07" />

#IMPORTANT NOTE
This SDK is provided for non commercial use only

UBUNTU require official autorization for commerical use

This SDK is provided without any warranty
Use at your own risk

Support my work and consider **buying  me a coffee**

https://buymeacoffee.com/mark.birss
