cmd_drivers/video/vfb.ko := arm-linux-gnueabi-ld -EL -r  -T /home/armin/Masterarbeit/gnublin/linux-2.6.33-lpc313x-fb/scripts/module-common.lds --build-id -o drivers/video/vfb.ko drivers/video/vfb.o drivers/video/vfb.mod.o