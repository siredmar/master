#!/bin/bash

dd if=/dev/zero of=$1 bs=512 count=1 && dd if=src/arch-arm/rom/apex.bin of=$1
echo done...
