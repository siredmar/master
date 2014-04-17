#!/bin/bash


path1=$(pwd)
path2="$1"

if [ ! -e .config ]
then
  make gnublin_defconfig
fi

make zImage  || exit 1 
make modules || exit 1
sudo rm -r /home/brenson/tmp/modules_install/*
make modules_install INSTALL_MOD_PATH=/home/brenson/tmp/modules_install || exit 1

sudo cp -r "/home/brenson/tmp/modules_install/lib" "${path2}"
sudo cp arch/arm/boot/zImage "${path2}"
sleep 1
sudo sync
sleep 1
sudo umount "${path2}"
