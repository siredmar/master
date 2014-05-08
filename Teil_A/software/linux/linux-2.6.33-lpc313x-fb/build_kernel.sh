#!/bin/bash
cd ready
rm -r lib/*
rm ready.tar.gz
cd ..

make -j4
make modules -j4
cp arch/arm/boot/zImage ready/boot
make modules_install INSTALL_MOD_PATH=ready

cd ready
tar cvfz ready.tar.gz boot lib
cd ..

