cmd_src/arch-arm/rom/apex.bin := arm-linux-gnueabi-objcopy -O binary -R .note -R .comment -R .bss  apex src/arch-arm/rom/apex.bin
