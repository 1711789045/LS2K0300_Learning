#!/bin/bash 
echo "执行自动编译+scp传输"

export PATH=/home/xiaom/loongson_2k0300/toolchain/loongson-gnu-toolchain-8.3-x86_64-loongarch64-linux-gnu-rc1.6/bin:$PATH

make -j12

scp -O vmlinuz root@192.168.2.57:/boot

sync
