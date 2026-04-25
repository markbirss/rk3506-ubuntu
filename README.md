# rk3506-ubuntu

Ubuntu 24.04.x OS image builder for various RK3506 SBC

(based off the Luckfox Lyra SDK when it could still build Ubuntu 22.04 OS Images but further modified to include updated Ubuntu 24.04.x OS and support for the later Luckfox Lyra SBC boards with ability to run luckfox-config) 

Supported Boards

Board Name | SD/EMMC |  Repo |
|:--|:--|:--|
| Luckfox Lyra | SDCard | https://github.com/markbirss/rk3506-ubuntu/tree/luckfox-bpi |
| Luckfox Lyra PLUS | SDCard | https://github.com/markbirss/rk3506-ubuntu/tree/luckfox-bpi |
| Luckfox Lyra Ultra W | eMMC | https://github.com/markbirss/rk3506-ubuntu/tree/luckfox-bpi |
| Luckfox Zero W | SDCard | https://github.com/markbirss/rk3506-ubuntu/tree/luckfox-bpi |
| Luckfox Lyra Pi W | SDCard | https://github.com/markbirss/rk3506-ubuntu/tree/luckfox-bpi |
| Luckfox Lyra Pi W | eMMC | https://github.com/markbirss/rk3506-ubuntu/tree/luckfox-bpi |
| ArmSom Forge (BPI Forge1) | SDCard | https://github.com/markbirss/rk3506-ubuntu/tree/luckfox-bpi |
| HD-RK3506G-MINI V1.0 | SDCard | https://github.com/markbirss/rk3506-ubuntu/tree/hd-rk3506g-mini |
| eByte ECB41-PGE1N2-N | SDCard | https://github.com/markbirss/rk3506-ubuntu/tree/e-byte |
| Qiyang RK3506B ( 256MB STAMP ) Dev Kit | SDCard | https://github.com/markbirss/rk3506-ubuntu/tree/qiyang |
| Luckfox Lyra with ST7305 300x400 RCLD Display | SDCard | https://github.com/markbirss/rk3506-ubuntu/tree/st7305 |

WIP Boards
Board Name | USB |  Repo |
|:--|:--|:--|
| U7-RK3506-128+256HMI 1024x600 7" | USB | https://github.com/markbirss/u7.git |

<img width="600" height="1024" alt="image" src="https://github.com/user-attachments/assets/53e3861a-1731-40b9-84bd-127ab81d78d7" />

```
# sha256sum  ubuntu_26.04.tar.gz 
# 22ed3726694dc244d0975555675884d2a92578aea4b5ab33aa64d47145b2b557  ubuntu_26.04.tar.gz

git clone https://github.com/markbirss/ubuntu_26.04.git
cd ubuntu_26.04
#rm -fr .git
7z x ubuntu_26.04.7z.001

sha256sum ubuntu_26.04.tar.gz
mv ubuntu_26.04.tar.gz ../
cd ../
mkdir ubuntu
mv ubuntu_26.04.tar.gz ubuntu/ubuntu_24.04.3.tar.gz
```

<img width="1234" height="628" alt="image" src="https://github.com/user-attachments/assets/ddfacd8a-06e0-4ca7-875a-d475b33d90d4" />


#IMPORTANT NOTE
This SDK is provided for non commercial use only

UBUNTU require official autorization for commerical use

This SDK is provided without any warranty
Use at your own risk

Support my work and consider **buying  me a coffee**

https://buymeacoffee.com/mark.birss
