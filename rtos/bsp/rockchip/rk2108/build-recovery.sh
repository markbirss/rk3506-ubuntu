#!/bin/bash

# build OSA
scons -j8
if [ $? -ne 0 ]
then
    echo "build OSA happen Errors!"
    exit -1
fi
cp rtthread.map rtthread-OSA.map
# build OSB, no XIP, using common/recovery_defconfig, if need, and generate firmware
export RTT_BUILD_XIP=N
scons --useconfig=./board/common/recovery_defconfig
scons -j8 --backup-fw
if [ $? -ne 0 ]
then
    echo "build OSB happen Errors!"
    exit -1
fi
#create different map file
mv rtthread.map rtthread-bak.map
mv rtthread-OSA.map rtthread.map

#restore to normal defconfig
scons --useconfig=./.config
