## Quick Start

```shell
cp board/rk2108b_evb/defconfig  .config         #选择对应板子的配置文件作为基础配置
scons --menuconfig                              #根据需求修改配置
./build.sh                                      #编译并打包固件
./update_fimeware.sh                            #烧写固件（需要设备先处于maskrom状态）并重启设备
```

## Note

1. 如果用windows烧写工具，需要2.63以上版本，烧的时候同时选择rk2108_db_loader.bin和Firmware.img 

2. 如果要启用xip功能，请修改rtconfig.py文件中XIP = 'enable'

## Documents

   更多开发相关文档，可以通过Doxygen生成，具体命令如下：

```shell
doxygen documents/Doxyfile                      #生存html文档
ls documents/html/index.html                    #html文档入口，请用浏览器打开
```