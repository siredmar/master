#!/bin/bash

# Armin Schlegel (2014)
# This script installs the kernel and the modules on the sd card
# Usage: ./intall_SD_all.sh sdX
# Note: sdX is the device for the sd card.
# Note: /dev/sdX3 must be the root partition and /dev/sdX1 the boot partition.

sd=$1
ready_path=$(pwd)/ready
mkdir $ready_path

echo mounting sd card
mount /dev/$13 /mnt/hd
mount /dev/$11 /mnt/hd/boot

cd /mnt/hd

echo  extracting $ready_path/ready.tar.gz

tar xvfz $ready_path/ready.tar.gz
cd $ready_path/..

echo unmounting sd card
umount /mnt/hd/boot
umount /mnt/hd

echo done ...
