cmd_fs/nfs/nfs.ko := arm-linux-gnueabi-ld -EL -r  -T /home/armin/Masterarbeit/gnublin/gnublin-lpc3131-3.3.0-master/linux-3.3.0-lpc313x/scripts/module-common.lds --build-id  -o fs/nfs/nfs.ko fs/nfs/nfs.o fs/nfs/nfs.mod.o