#!/bin/bash

# build OSA
scons -j8
if [ $? -ne 0 ]
then
    echo "build fw happen Errors!"
    exit -1
fi
# build OSB if need, and generate firmware
scons -j8 --backup-fw
if [ $? -ne 0 ]
then
    echo "build back up fw happen Errors!"
    exit -1
fi
if [ "$1" = cfu ]; then
    ../tools/cfu/cfu_fw_gen Image/rtthread.img 0 Image/bank0.cfu
    ../tools/cfu/cfu_fw_gen Image/rtthread_bak.img 0 Image/bank1.cfu
fi
