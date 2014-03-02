cmd_kernel/power/built-in.o :=  arm-linux-gnueabi-ld -EL    -r -o kernel/power/built-in.o kernel/power/main.o kernel/power/qos.o kernel/power/process.o kernel/power/suspend.o 
