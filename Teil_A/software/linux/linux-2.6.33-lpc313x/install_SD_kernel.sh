#!/bin/bash
# Armin Schlegel (2014)
# This script installs the kernel on the sd card
# Usage: ./intall_SD_kernel.sh sdX
# Note: sdX is the device for the sd card.
# Note: /dev/sdX1 must be the boot partition.

sd=$1
ready_path=$(pwd)/ready
mkdir $ready_path

echo mounting sd card
mount /dev/$11 /mnt/hd

cp ready/boot/zImage /mnt/hd
sleep 1
echo unmounting sd card
umount /mnt/hd

echo done ...
