cmd_drivers/hid/hid.ko := arm-linux-gnueabi-ld -EL -r  -T /home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/scripts/module-common.lds --build-id -o drivers/hid/hid.ko drivers/hid/hid.o drivers/hid/hid.mod.o