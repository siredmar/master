#!/bin/bash
# ./write_bootloader.sh /dev/sdX2 apex.bin
dd if=/dev/zero of=$1 bs=512 count=1 && dd if=$2 of=$1
echo done...
