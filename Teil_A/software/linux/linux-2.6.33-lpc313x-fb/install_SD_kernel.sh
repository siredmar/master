#!/bin/bash
sd=$1
ready_path=$(pwd)/ready

echo mounting sd card
mount /dev/$11 /mnt/hd

cp ready/boot/zImage /mnt/hd
sleep 1
echo unmounting sd card
umount /mnt/hd

echo done ...
