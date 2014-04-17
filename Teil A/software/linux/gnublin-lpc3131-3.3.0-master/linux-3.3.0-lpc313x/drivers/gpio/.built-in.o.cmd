cmd_drivers/gpio/built-in.o :=  arm-linux-gnueabi-ld -EL    -r -o drivers/gpio/built-in.o drivers/gpio/gpiolib.o drivers/gpio/devres.o drivers/gpio/gpio-lpc313x.o 
