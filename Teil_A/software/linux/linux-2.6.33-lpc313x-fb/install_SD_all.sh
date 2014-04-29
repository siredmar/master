#!/bin/bash
sd=$1
ready_path=$(pwd)/ready

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
