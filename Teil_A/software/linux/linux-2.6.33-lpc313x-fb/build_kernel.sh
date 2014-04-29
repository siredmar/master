#!/bin/bash
cd ready
rm -r lib/*
rm ready.tar.gz
cd ..

make
make modules
cp arch/arm/boot/zImage ready/boot
make modules_install INSTALL_MOD_PATH=ready

cd ready
tar cvfz ready.tar.gz boot lib
cd ..

