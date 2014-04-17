#!/bin/bash
make
make modules
cp arch/arm/boot/zImage ready/boot
make modules_install INSTALL_MOD_PATH=ready

cd ready
rm ready.tar.gz
tar cvfz ready.tar.gz boot lib
cd ..

