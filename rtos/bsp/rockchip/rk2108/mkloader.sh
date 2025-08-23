#! /bin/bash

port=$1
value=$((10#${2}*2+1))
mux=$value
file=Image/rk2108_loader.bin

if [ ! -n "$1" ] ;then
echo disable debug console
echo Write 00 to the 0x08 offset address of $file
printf "\x00" > inject.bin
else
echo set debug console to uart$1\_mux$2
echo Write 0x$port$mux to the 0x08 offset address of $file
printf "\x$port$mux" > inject.bin
fi

dd if=inject.bin of=$file seek=$((0x8)) bs=1 conv=notrunc
